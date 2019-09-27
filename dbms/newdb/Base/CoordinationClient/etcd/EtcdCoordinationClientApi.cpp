#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>




#include "EtcdCoordinationClientApi.h"
#include "EtcdCoordinationPublicFunction.h"
#include "EtcdCoordinationReactor.h"

using namespace coordination;
using namespace coordination::etcd;


const int MAX_CHILD_NODE_NUM = 1000;//子节点最大数量



EtcdCoordinationClientApi::EtcdCoordinationClientApi()
{

}

EtcdCoordinationClientApi::~EtcdCoordinationClientApi()
{

}


bool EtcdCoordinationClientApi::initialize(const string& coordinationAddress)
{
	this->etcdChannel_ = grpc::CreateChannel(coordinationAddress, grpc::InsecureChannelCredentials());
	if (NULL == this->etcdChannel_.get())
	{
		printf("grpc::CreateChannel failed!");
		return false;
	}

	this->kvStub_ = std::move(etcdserverpb::KV::NewStub(this->etcdChannel_));
	this->leaseStub_ = std::move(etcdserverpb::Lease::NewStub(this->etcdChannel_));
	this->lockStub_ = std::move(v3lockpb::Lock::NewStub(this->etcdChannel_));
	this->leaderStub_ = std::move(v3electionpb::Election::NewStub(this->etcdChannel_));

	this->etcdCoordinationReactor_ = std::make_shared<EtcdCoordinationReactor>(this->etcdChannel_);
    if (!this->etcdCoordinationReactor_->initialize())
    {
        printf("etcdCoordinationReactor_->initialize failed!\n");
        return false;
    }

	return true;
}


bool EtcdCoordinationClientApi::start()
{
	return this->etcdCoordinationReactor_->start();
}

void EtcdCoordinationClientApi::stop()
{
	this->etcdCoordinationReactor_->stop();
}


bool EtcdCoordinationClientApi::setNodeValue(const KeyValue& keyValue, bool isCreate)
{
	grpc::ClientContext context;
	etcdserverpb::PutRequest request;
	request.set_key(keyValue.first);
	request.set_value(keyValue.second);
	request.set_ignore_value(!isCreate);

	etcdserverpb::PutResponse putResponse;
	grpc::Status status = this->kvStub_->Put(&context, request, &putResponse);
	if (!status.ok())
	{
		printf("kvStub_.Put failed! %s %s\n", status.error_message().c_str(), status.error_details().c_str());
		return false;
	}

	return true;
}

bool EtcdCoordinationClientApi::delNode(const string& key)
{
	grpc::ClientContext context;
	etcdserverpb::DeleteRangeRequest request;
	request.set_key(key);

	etcdserverpb::DeleteRangeResponse response;
	grpc::Status status = this->kvStub_->DeleteRange(&context, request, &response);
	if (!status.ok())
	{
		printf("kvStub_.DeleteRange failed!\n");
		return false;
	}

	return true;
}

bool EtcdCoordinationClientApi::getNodeValue(const string& key, KeyValue& keyValue)
{
	grpc::ClientContext context;
	etcdserverpb::RangeRequest request;
	request.set_key(key);
	request.set_limit(1);

	etcdserverpb::RangeResponse response;
	grpc::Status status = this->kvStub_->Range(&context, request, &response);
	if (!status.ok())
	{
		printf("kvStub_.Range failed! %s\n", status.error_message().c_str());
		return false;
	}

	if (0 == response.kvs_size())
	{
		return false;
	}

	keyValue.first = response.kvs(0).key();
	keyValue.second = response.kvs(0).value();

	return true;
}

bool EtcdCoordinationClientApi::watchNode(const string& nodeKey, ICoordinationEventHandler *handler)
{
	return etcdCoordinationReactor_->watchNode(nodeKey, handler);
}

bool EtcdCoordinationClientApi::cancelWatchNode(const string& nodeKey)
{
	return etcdCoordinationReactor_->cancelWatchNode(nodeKey);
}

bool EtcdCoordinationClientApi::getChildNode(const string& nodeKey, std::vector<KeyValue>& childNodeInfo)
{
	grpc::ClientContext context;
	etcdserverpb::RangeRequest request;
	request.set_key(nodeKey);
	request.set_range_end(getChildNodeEndKey(nodeKey));	
	request.set_limit(MAX_CHILD_NODE_NUM);
	request.set_sort_order(etcdserverpb::RangeRequest::ASCEND);
	request.set_sort_target(etcdserverpb::RangeRequest::KEY);

	etcdserverpb::RangeResponse response;
	grpc::Status status = this->kvStub_->Range(&context, request, &response);
	if (!status.ok())
	{
		printf("kvStub_.Range failed!\n");
		return false;
	}

	for (int iPos = 0; response.kvs_size() > iPos; ++iPos)
	{
		KeyValue keyValue;
		keyValue.first = response.kvs(iPos).key();
		keyValue.second = response.kvs(iPos).value();

		childNodeInfo.push_back(keyValue);
	}

	return true;
}

bool EtcdCoordinationClientApi::watchChildNode(const string& nodeKey, ICoordinationEventHandler *handler)
{
	return etcdCoordinationReactor_->watchChildNode(nodeKey, handler);
}

bool EtcdCoordinationClientApi::cancelWatchChildNode(const string& nodeKey)
{
	return etcdCoordinationReactor_->cancelWatchChildNode(nodeKey);
}


bool EtcdCoordinationClientApi::registerServer(const string& serverKey, const string& serverValue, int32 timeOutSTime)
{
	int64 leaseID = 0;
	if (!leaseGrant(this->etcdChannel_, timeOutSTime, leaseID))
	{
		printf("leaseGrant failed!");
		return false;
	}

	grpc::ClientContext context;
	etcdserverpb::PutRequest request;
	request.set_key(serverKey);
	request.set_value(serverValue);
	request.set_ignore_value(false);
	request.set_lease(leaseID);

	etcdserverpb::PutResponse putResponse;
	grpc::Status status = this->kvStub_->Put(&context, request, &putResponse);
	if (!status.ok())
	{
		printf("kvStub_.Put failed!\n");
		return false;
	}

	EtcdCoordinationReactor::LeaseKeepAliveTask leaseKeepAliveTask;
	leaseKeepAliveTask.leaseTaskID_ = leaseID;
	leaseKeepAliveTask.TTL_ = timeOutSTime;
	leaseKeepAliveTask.key_ = serverKey;
	etcdCoordinationReactor_->registerLeaseTask(leaseKeepAliveTask);

	return true;
}


bool EtcdCoordinationClientApi::campaignLeader(const string& name, const string& value, int32 timeOutSTime)
{
	int64 leaseID = 0;
	if (!leaseGrant(this->etcdChannel_, timeOutSTime, leaseID))
	{
		printf("leaseGrant failed!");
		return false;
	}

	grpc::ClientContext context;
	v3electionpb::CampaignRequest request;
	request.set_name(name);
	request.set_lease(leaseID);
	request.set_value(value);

	v3electionpb::CampaignResponse response;
	grpc::Status status = this->leaderStub_->Campaign(&context, request, &response);
	if (!status.ok())
	{
		printf("kvStub_.Range failed!\n");
		return false;
	}

	EtcdCoordinationReactor::LeaseKeepAliveTask leaseKeepAliveTask;
	leaseKeepAliveTask.leaseTaskID_ = leaseID;
	leaseKeepAliveTask.TTL_ = timeOutSTime;
	leaseKeepAliveTask.key_ = response.leader().key();
	leaseKeepAliveTask.name_ = name;
	etcdCoordinationReactor_->registerLeaseTask(leaseKeepAliveTask);

	return true;
}

bool EtcdCoordinationClientApi::resignLeader(const string& name)
{
	EtcdCoordinationReactor::LeaseKeepAliveTask leaseKeepAliveTask;
	if (!etcdCoordinationReactor_->getLeaseTaskByName(name, leaseKeepAliveTask))
	{
		printf("getLeaseTaskByKey failed! name:%s\n", name.c_str());
		return false;
	}

	grpc::ClientContext context;
	v3electionpb::ResignRequest request;
	v3electionpb::LeaderKey* leaderKeyPB = request.mutable_leader();

	leaderKeyPB->set_name(name);
	leaderKeyPB->set_key(leaseKeepAliveTask.key_);
	leaderKeyPB->set_lease(leaseKeepAliveTask.leaseTaskID_);

	v3electionpb::ResignResponse response;
	grpc::Status status = this->leaderStub_->Resign(&context, request, &response);
	if (!status.ok())
	{
		printf("kvStub_.Range failed!\n");
		return false;
	}

	return true;
}

bool EtcdCoordinationClientApi::leader(const string& name, KeyValue& leaderInfo)
{
	grpc::ClientContext context;
	v3electionpb::LeaderRequest request;
	request.set_name(name);

	v3electionpb::LeaderResponse response;
	grpc::Status status = this->leaderStub_->Leader(&context, request, &response);
	if (!status.ok())
	{
		printf("kvStub_.Range failed!\n");
		return false;
	}

	leaderInfo.first = response.kv().key();
	leaderInfo.second = response.kv().value();

	return true;
}

bool EtcdCoordinationClientApi::watchLeader(const string& name, ICoordinationEventHandler *handler)
{
	return etcdCoordinationReactor_->watchLeader(name, handler);
}

bool EtcdCoordinationClientApi::cancelWatchLeader(const string& name)
{
	return etcdCoordinationReactor_->cancelWatchLeader(name);
}


bool EtcdCoordinationClientApi::lock(const string& name, int32 timeOutSTime)
{
	grpc::ClientContext context;

	int64 leaseID = 0;
	if (!leaseGrant(this->etcdChannel_, timeOutSTime, leaseID))
	{
		printf("leaseGrant failed!");
		return false;
	}

	v3lockpb::LockRequest request;
	request.set_name(name);
	request.set_lease(leaseID);

	v3lockpb::LockResponse response;
	grpc::Status status = this->lockStub_->Lock(&context, request, &response);
	if (!status.ok())
	{
		printf("kvStub_.Range failed!\n");
		return false;
	}

	this->lockNameAndKeyMapper_[name] = response.key();

	return true;
}

bool EtcdCoordinationClientApi::unlock(const string& name)
{
	if (this->lockNameAndKeyMapper_.end() == this->lockNameAndKeyMapper_.find(name)) return false;

	grpc::ClientContext context;
	v3lockpb::UnlockRequest request;
	request.set_key(this->lockNameAndKeyMapper_[name]);

	v3lockpb::UnlockResponse response;
	grpc::Status status = this->lockStub_->Unlock(&context, request, &response);
	if (!status.ok())
	{
		printf("kvStub_.Range failed!\n");
		return false;
	}
	this->lockNameAndKeyMapper_.erase(name);

	return true;
}




