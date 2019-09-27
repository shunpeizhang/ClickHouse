#include "EtcdCoordinationReactor.h"

#include "etcdGrpc/rpc.grpc.pb.h"


#include "EtcdCoordinationPublicFunction.h"


using namespace coordination::etcd;
using namespace coordination;


const int THREAD_SLEEP_MTIME = 100;//线程睡眠时长（毫秒）


EtcdCoordinationReactor::EtcdCoordinationReactor(std::shared_ptr<::grpc::Channel> etcdChannel)	
{
	this->etcdChannel_ = etcdChannel;

    this->isWatchRun_ = false;
    this->isLeaderWatchRun_ = false;
    this->isLeaseKeepaliveRun_ = false;
}

EtcdCoordinationReactor::~EtcdCoordinationReactor()
{
    if (this->isWatchRun_ || this->isLeaderWatchRun_ || this->isLeaseKeepaliveRun_) stop();
}


bool EtcdCoordinationReactor::initialize()
{
	this->watchStub_ = std::move(etcdserverpb::Watch::NewStub(this->etcdChannel_));
	this->leaderStub_ = std::move(v3electionpb::Election::NewStub(this->etcdChannel_));
	this->leaseStub_ = std::move(etcdserverpb::Lease::NewStub(this->etcdChannel_));

	this->watchReadWriter_ = std::move(this->watchStub_->Watch(&this->watchContext_));
    if (NULL == this->watchReadWriter_.get())
    {
        printf("watchStub_->Watch failed!\n");
        return false;
    }

	this->leaseReaderWriter_ = std::move(this->leaseStub_->LeaseKeepAlive(&this->leaseKeepaliveContext_));
    if (NULL == this->leaseReaderWriter_.get())
    {
        printf("watchStub_->leaseReaderWriter_ failed!\n");
        return false;
    }

	return true;
}

bool EtcdCoordinationReactor::start()
{
	if (!isWatchRun_ && !isLeaderWatchRun_ && !isLeaseKeepaliveRun_)
	{
		this->isWatchRun_ = true;
		this->isLeaderWatchRun_ = true;
		this->isLeaseKeepaliveRun_ = true;

		this->watchThread_ = thread_ptr(new std::thread(&EtcdCoordinationReactor::watchRun, std::ref(*this)));
		this->leaderWatchThread_ = thread_ptr(new std::thread(&EtcdCoordinationReactor::leaderWatchRun, std::ref(*this)));
		this->leaseKeepaliveThread_ = thread_ptr(new std::thread(&EtcdCoordinationReactor::leaseKeepaliveRun, std::ref(*this)));

		return true;
	}

	return false;
}

void EtcdCoordinationReactor::stop()
{
    this_guard guard(mutex_);

	this->isWatchRun_ = false;
	this->isLeaderWatchRun_ = false;
	this->isLeaseKeepaliveRun_ = false;

    if (watchReadWriter_.get()) {
        watchReadWriter_->Finish();
        watchReadWriter_.release();
    }

    if (leaderReader_.get()) {
        leaderReader_->Finish();
        leaderReader_.release();
    }

    if (leaseReaderWriter_.get()) {
        leaseReaderWriter_->Finish();
        leaseReaderWriter_.release();
    }

	this->watchThread_->join();
	this->leaderWatchThread_->join();
	this->leaseKeepaliveThread_->join();
}


bool EtcdCoordinationReactor::watchNode(const string& nodeKey, ICoordinationEventHandler *handler)
{
    auto watch = getWatchInfo(nodeKey);
    if (watch.get() || !watchReadWriter_.get())
    {
        printf("nodeKey has watch  || !watchReadWriter_.get()! nodeKey:%s\n", nodeKey.c_str());
        return false;
    }

	etcdserverpb::WatchRequest request;
	etcdserverpb::WatchCreateRequest* watchCreateRequest = request.mutable_create_request();
	watchCreateRequest->set_key(nodeKey);

	if (!watchReadWriter_->Write(request))
	{
		printf("watchReadWriter_->Write failed!\n");
        resetWatch();
		return false;
	}

    std::shared_ptr<WatchInfo> watchInfo = std::shared_ptr<WatchInfo>(new WatchInfo());
	watchInfo->key_ = nodeKey;
	watchInfo->handler_ = handler;
    watchInfo->isWatchChild_ = false;

	this_guard guard(mutex_);
	this->requestWatchInfos_.push(watchInfo);
    this->allWatchInfos_[nodeKey] = watchInfo;

	return true;
}


bool EtcdCoordinationReactor::cancelWatchNode(const string& nodeKey)
{
    auto watchInfo = getWatchInfo(nodeKey);
    if (!watchInfo.get() || !watchReadWriter_.get())
	{
		printf("getWatchInfo failed  || !watchReadWriter_.get()! nodeKey:%s\n", nodeKey.c_str());
		return false;
	}

	etcdserverpb::WatchRequest request;
	etcdserverpb::WatchCancelRequest* watchCancelRequest = request.mutable_cancel_request();
	watchCancelRequest->set_watch_id(watchInfo->watch_id_);

	if (!watchReadWriter_->Write(request))
	{
		printf("watchReadWriter_->Write failed!\n");
        resetWatch();
		return false;
	}
    this->allWatchInfos_.erase(this->allWatchInfos_.find(nodeKey));

	return true;
}


bool EtcdCoordinationReactor::watchChildNode(const string& nodeKey, ICoordinationEventHandler *handler)
{
    auto watch = getWatchInfo(nodeKey);
    if (watch.get() || !watchReadWriter_.get())
    {
        printf("nodeKey has watch  || !watchReadWriter_.get()! nodeKey:%s\n", nodeKey.c_str());
        return false;
    }

	etcdserverpb::WatchRequest request;
	etcdserverpb::WatchCreateRequest* watchCreateRequest = request.mutable_create_request();
	watchCreateRequest->set_key(nodeKey);
	watchCreateRequest->set_range_end(getChildNodeEndKey(nodeKey));

	if (!watchReadWriter_->Write(request))
	{
		printf("watchReadWriter_->Write failed!\n");
        resetWatch();
		return false;
	}

    std::shared_ptr<WatchInfo> watchInfo = std::shared_ptr<WatchInfo>(new WatchInfo());
	watchInfo->key_ = nodeKey;
	watchInfo->handler_ = handler;
    watchInfo->isWatchChild_ = true;

	this_guard guard(mutex_);
	this->requestWatchInfos_.push(watchInfo);
    this->allWatchInfos_[nodeKey] = watchInfo;

	return true;
}

bool EtcdCoordinationReactor::cancelWatchChildNode(const string& nodeKey)
{
    return cancelWatchNode(nodeKey);
}


bool EtcdCoordinationReactor::watchLeader(const string& name, ICoordinationEventHandler *handler)
{
	//目前只支持一个leader选择
	{
		this_guard guard(mutex_);
		if (NULL != this->leaderReader_.get())
		{
			printf("leader watch only one, error! name:%s\n", name.c_str());
			return false;
		}
	}	

	v3electionpb::LeaderRequest request;
	this->leaderReader_ = this->leaderStub_->Observe(&this->leaderContext_, request);
	if (NULL == this->leaderReader_.get())
	{
		printf("leaderStub_->Observe failed! name:%s\n", name.c_str());
		return false;
	}

	{
		this_guard guard(mutex_);

		this->leaderHandler_ = handler;
		this->leaderName_ = name;
	}	

	return true;
}

bool EtcdCoordinationReactor::cancelWatchLeader(const string& name)
{
	this_guard guard(mutex_);

	//目前只支持一个leader选择
	if (NULL == this->leaderReader_.get())
	{
		printf("leader watch not exist, error! name:%s\n", name.c_str());
		return false;
	}

	this->leaderReader_->Finish();

	return true;
}


bool EtcdCoordinationReactor::registerLeaseTask(const LeaseKeepAliveTask& task)
{
	this_guard guard(mutex_);

	if (this->needLeaseKeepAlives_.end() != needLeaseKeepAlives_.find(task.key_))
	{
		return false;
	}

    std::shared_ptr<LeaseKeepAliveTask> leaseTask = std::shared_ptr<LeaseKeepAliveTask>(new LeaseKeepAliveTask());
	*leaseTask = task;
	leaseTask->etcdCoordinationReactor_ = this;
	needLeaseKeepAlives_[task.key_] = leaseTask;

	Poco::Util::TimerTask::Ptr taskPtr = new Poco::Util::TimerTaskAdapter<LeaseKeepAliveTask>(*leaseTask, &LeaseKeepAliveTask::onLeaseTimer);
	this->leaseKeepaliveTimer_.schedule(taskPtr, 0, leaseTask->TTL_ / 2);

	return true;
}


bool EtcdCoordinationReactor::unregisterLeaseTask(const LeaseKeepAliveTask& task)
{
	this_guard guard(mutex_);

	auto iter = needLeaseKeepAlives_.find(task.key_);
	if (this->needLeaseKeepAlives_.end() == iter)
	{
		return false;
	}

	iter->second->isNeedCancel_ = true;

	return true;
}

bool EtcdCoordinationReactor::getLeaseTaskByKey(const string& key, LeaseKeepAliveTask& task)
{
	this_guard guard(mutex_);

	if (this->needLeaseKeepAlives_.end() == needLeaseKeepAlives_.find(key))
	{
		return false;
	}

	task = *needLeaseKeepAlives_[key];

	return true;
}

bool EtcdCoordinationReactor::getLeaseTaskByName(const string& name, LeaseKeepAliveTask& task)
{
	this_guard guard(mutex_);

	auto iter = needLeaseKeepAlives_.begin();
	for (; needLeaseKeepAlives_.end() != iter; ++iter)
	{
		if (0 == iter->second->name_.compare(name))
		{
			task = *iter->second;
			return true;
		}
	}

	return false;
}



//节点监听事件处理
void EtcdCoordinationReactor::watchRun()
{
	while (this->isWatchRun_)
	{
		if (NULL != watchReadWriter_.get())
		{
			etcdserverpb::WatchResponse watchResponse;
			while (watchReadWriter_->Read(&watchResponse)) {
				this_guard guard(mutex_);

				//处理监听请求响应
				if (watchResponse.created() && 0 < this->requestWatchInfos_.size())
				{
                    std::shared_ptr<WatchInfo> watchInfo = this->requestWatchInfos_.back();
					this->requestWatchInfos_.pop();

					watchInfo->watch_id_ = watchResponse.watch_id();
					watchsInfo_[watchInfo->key_] = watchInfo;
					watchsInfoByID_[watchInfo->watch_id_] = watchInfo;
				}

				//处理取消
				if (watchResponse.canceled() && 0 < this->requestWatchInfos_.size())
				{
					auto iter = watchsInfoByID_.find(watchResponse.watch_id());
					if (watchsInfoByID_.end() == iter) continue;

					watchsInfo_.erase(iter->second->key_);
					watchsInfoByID_.erase(watchResponse.watch_id());
				}

				//检测watch_id是否存在
                std::shared_ptr<WatchInfo> watchInfo;
				{
					auto iter = watchsInfoByID_.find(watchResponse.watch_id());
					if (watchsInfoByID_.end() == iter || NULL == iter->second->handler_) continue;

					watchInfo = iter->second;
				}

				//处理事件
				for(int iPos = 0; watchResponse.events_size() > iPos; ++iPos)
				{
					const ::mvccpb::Event& event = watchResponse.events(iPos);
					KeyValue keyValue;

					keyValue.first = event.kv().key();
					keyValue.second = event.kv().value();

					if (mvccpb::Event::PUT == event.type())
					{
						if (1 == event.kv().version())//kv.Version=1 的 PUT 事件表明 key 的创建
						{
							if (0 == watchInfo->key_.compare(event.kv().key()))
							{
								watchInfo->handler_->currentValueChange(keyValue);
							}
							else {
								watchInfo->handler_->subNodeCreate(watchInfo->key_, keyValue);
							}
						}
						else {
							if (0 == watchInfo->key_.compare(event.kv().key()))
							{
								watchInfo->handler_->currentValueChange(keyValue);
							}
							else {
								watchInfo->handler_->subValueChange(watchInfo->key_, keyValue);
							}
						}
					}
					else if (mvccpb::Event::DELETE == event.type())
					{
						if (0 == watchInfo->key_.compare(event.kv().key()))
						{
							watchInfo->handler_->currentNodeDELETE(keyValue.first);
						}
						else {
							watchInfo->handler_->subNodeDELETE(watchInfo->key_, keyValue.first);
						}
					}
				}
			}

			//release
            resetWatch();
		}
        else
        {
            /// 尝试重新发起监听
            this->watchReadWriter_ = this->watchStub_->Watch(&this->watchContext_);
            if (this->watchReadWriter_.get())
            {
                this_guard guard(mutex_);

                auto iter = watchsInfo_.begin();
                for (; watchsInfo_.end() != iter; ++iter)
                {
                    iter->second->handler_->rewatch(iter->second->key_);
                    if (iter->second->isWatchChild_)
                    {
                        if (!watchChildNode(iter->second->key_, iter->second->handler_)) break;                        
                    }
                    else
                    {
                        if (!watchNode(iter->second->key_, iter->second->handler_)) break;
                    }                    
                }
            }
        }

		//sleep
		std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_SLEEP_MTIME));
	}
}

//leader监听事件处理
void EtcdCoordinationReactor::leaderWatchRun()
{
	while (isLeaderWatchRun_)
	{
		if(NULL != this->leaderReader_.get())
		{
			v3electionpb::LeaderResponse leaderResponse;
			while (this->leaderReader_->Read(&leaderResponse)) {
				if (this->leaderHandler_)
				{
					KeyValue keyValue;
					keyValue.first = leaderResponse.kv().key();
					keyValue.second = leaderResponse.kv().value();

					this->leaderHandler_->leaderChange(this->leaderName_, 0 < keyValue.first.size(), keyValue);
				}
			}

			//release
			{
				this_guard guard(mutex_);

                if (leaderReader_.get())
                {
                    this->leaderReader_->Finish();
                    this->leaderReader_.release();
                }
			}
			
		}

		//sleep
		std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_SLEEP_MTIME));
	}
}

//处理lease keep alive
void EtcdCoordinationReactor::leaseKeepaliveRun()
{
	while (this->isLeaseKeepaliveRun_)
	{
		if (this->leaseReaderWriter_.get())
		{
			etcdserverpb::LeaseKeepAliveResponse leaseKeepAliveResponse;
			while (this->leaseReaderWriter_->Read(&leaseKeepAliveResponse))
			{

			}

            //release
            resetLease();
		}
        else
        {
            this_guard guard(mutex_);

            this->leaseReaderWriter_ = std::move(this->leaseStub_->LeaseKeepAlive(&this->leaseKeepaliveContext_));
            if (NULL == this->leaseReaderWriter_.get())
            {
                printf("watchStub_->leaseReaderWriter_ failed!\n");
            }
        }

		//sleep
		std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_SLEEP_MTIME));
	}


}


std::shared_ptr<EtcdCoordinationReactor::WatchInfo> EtcdCoordinationReactor::getWatchInfo(const string& key)
{
	this_guard guard(mutex_);

	auto iter = this->allWatchInfos_.find(key);
	if (this->allWatchInfos_.end() == iter)
	{
		return std::shared_ptr<EtcdCoordinationReactor::WatchInfo>(NULL);
	}

	return iter->second;
}

void EtcdCoordinationReactor::resetWatch()
{
    this_guard guard(mutex_);

    if (this->watchReadWriter_.get())
    {
        this->watchReadWriter_->Finish();
        this->watchReadWriter_.release();

        while(!requestWatchInfos_.empty()) requestWatchInfos_.pop();
        watchsInfo_.clear();
        watchsInfoByID_.clear();
    }    
}

void EtcdCoordinationReactor::resetLease()
{
    this_guard guard(mutex_);

    if (this->leaseReaderWriter_.get())
    {
        this->leaseReaderWriter_->Finish();
        this->leaseReaderWriter_.release();

        auto iter = needLeaseKeepAlives_.begin();
        for (; needLeaseKeepAlives_.end() != iter; ++iter)
        {
            iter->second->isNeedCancel_ = true;
        }
    }

}


void EtcdCoordinationReactor::LeaseKeepAliveTask::onLeaseTimer(Poco::Util::TimerTask& task)
{
	if (this->isNeedCancel_)
	{
		task.cancel();
		
		this->etcdCoordinationReactor_->needLeaseKeepAlives_.erase(this->key_);
		delete this;
	}
	else
	{
		etcdserverpb::LeaseKeepAliveRequest request;
		request.set_id(this->leaseTaskID_);

		if (this->etcdCoordinationReactor_)
		{
            if (!this->etcdCoordinationReactor_->leaseReaderWriter_->Write(request))
            {
                this->etcdCoordinationReactor_->resetLease();
            }
		}
	}
}

