#include "core/PocoSocketChannelPool.h"


#include <time.h>


#include "core/DistributeServerWatcher.h"
#include "core/PocoServer.h"
#include "core/PocoSocketChannel.h"
#include "Common/logger_adapt.h"
#include "core/request.h"

namespace core
{
    const int MIN_KEEP_CHANNEL_STIME = 10; //连接最小不被删除时间(健康的连接)，防止频繁创建与删除
    const int WAIT_CHANNEL_MAX_STIME = 10; //获取一个连接，等待最大时长(秒)
    const int HEARTBEAT_INTVAL_STIME = 30; //心跳发送间隔时长(秒)
	
	
    PocoSocketChannelPoolItem::ChannelInfo::ChannelInfo()
    {
        this->lastHeartbeatSTime_ = 0;
        this->createSTime_ = 0;
        this->channel_ = NULL;
    }
	
    PocoSocketChannelPoolItem::ChannelInfo::~ChannelInfo()
    {
        if (this->channel_)
        {
            auto socketChannelManager = ((PocoServer*)g_serverInstance.get())->getSocketChannelManager();
            if (NULL == socketChannelManager.get()) return;
			
            socketChannelManager->eraseSocketChannelNotDelete(this->channel_->getChannelID());
            delete this->channel_;
            this->channel_ = NULL;
        }
    }
	
	
    PocoSocketChannelPoolItem::PocoSocketChannelPoolItem(std::shared_ptr<ISocketConnector<std::shared_ptr<Poco::Net::Socket>,
        Poco::AbstractObserver>> socketConnector, 
        uint16 minChannelCount,
        uint16 maxChannelCount,
        ServerUniqueID serverUniqueID, 
        std::shared_ptr<IServerWatch> serverWatcher)
        : socketConnector_(socketConnector)
        , minChannelCount_(minChannelCount)
        , maxChannelCount_(maxChannelCount)
        , serverWatcher_(serverWatcher)
        , serverUniqueID_(serverUniqueID)
    {
		
    }


    PocoSocketChannelPoolItem::~PocoSocketChannelPoolItem()
    {
		
    }
	
    ISocketChannel<std::shared_ptr<Poco::Net::Socket>>* PocoSocketChannelPoolItem::getSocketChannel()
    {
        ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel = NULL;
		
        uint32 current = time(NULL);
        while (time(NULL) - current <= WAIT_CHANNEL_MAX_STIME)
        {
            channel = getOrCreateChannel();
            if (NULL != channel) break;
			
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
					
        if (NULL == channel) 
        {
            LOG_ERROR(&g_serverInstance->logger(), "get channel failed! serverUniqueID:%u", this->serverUniqueID_);
            return NULL;
        }
		
        return channel;
    }
	
    void PocoSocketChannelPoolItem::releaseSocketChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *socketChannel)
    {
        if (NULL == socketChannel) return;
		
        this_guard guard(mutex_);
		
        auto iter = this->useSocketChannel_.find(socketChannel->getChannelID());
        if (this->useSocketChannel_.end() == iter)
        {
            //maybe in needRemoveSocketChannel_
            iter = this->needRemoveSocketChannel_.find(socketChannel->getChannelID());
            if (this->needRemoveSocketChannel_.end() == iter) return;
			
            this->needRemoveSocketChannel_.erase(iter);
        }
        else
        {
            this->freeSocketChannel_[socketChannel->getChannelID()] = iter->second;
            this->useSocketChannel_.erase(iter);
        }		
    }
	
    void PocoSocketChannelPoolItem::removeChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *socketChannel)
    {
        if (NULL == socketChannel) return;
		
        this_guard guard(mutex_);
        auto iter = this->useSocketChannel_.find(socketChannel->getChannelID());
        if (this->useSocketChannel_.end() == iter)
        {
            //maybe in freeSocketChannel_
            iter = this->freeSocketChannel_.find(socketChannel->getChannelID());
            if (this->freeSocketChannel_.end() == iter)
            {
                iter = this->needRemoveSocketChannel_.find(socketChannel->getChannelID());
                if (this->needRemoveSocketChannel_.end() == iter) return;
				
                this->needRemoveSocketChannel_.erase(iter);
            }
            else
            {
                this->needRemoveSocketChannel_[socketChannel->getChannelID()] = iter->second;
                this->freeSocketChannel_.erase(iter);
            }
        }
        else
        {
            this->needRemoveSocketChannel_[socketChannel->getChannelID()] = iter->second;
            this->useSocketChannel_.erase(iter);
        }
    }
		
    bool PocoSocketChannelPoolItem::hasSocketChannel(SocketChannelID channelID)
    {
        this_guard guard(mutex_);
        auto iter = this->useSocketChannel_.find(channelID);
        if (this->useSocketChannel_.end() == iter)
        {
            //maybe in freeSocketChannel_
            iter = this->freeSocketChannel_.find(channelID);
            if (this->freeSocketChannel_.end() == iter)
            {
                iter = this->needRemoveSocketChannel_.find(channelID);
                if (this->needRemoveSocketChannel_.end() != iter) return true;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }
		
        return false;
    }
		
    bool PocoSocketChannelPoolItem::setMinChannelCount(uint16 minChannelCount)
    {
        this_guard guard(mutex_);
		
        this->minChannelCount_ = minChannelCount;
        return true;
    }
	
    bool PocoSocketChannelPoolItem::setMaxChannelCount(uint16 maxChannelCount)
    {
        this_guard guard(mutex_);
		
        this->maxChannelCount_ = maxChannelCount;
        return true;
    }
	
    bool PocoSocketChannelPoolItem::setSocketConnector(std::shared_ptr<ISocketConnector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketConnector)
    {
        this_guard guard(mutex_);
		
        this->socketConnector_ = socketConnector;
        return true;
    }

    bool PocoSocketChannelPoolItem::setHeartbeat(const std::string& heartbeatMessageData, MessageID heartbeatMessageID)
    {
        heartbeatMessageData_ = heartbeatMessageData;
        heartbeatMessageID_ = heartbeatMessageID;
    }
	

    void PocoSocketChannelPoolItem::sendHeartbeat()
    {
        this_guard guard(mutex_);
		
        uint32 timeNow = time(NULL);
		
        //保持最小连接数
        if(this->freeSocketChannel_.size() + this->useSocketChannel_.size() > this->minChannelCount_)
        {			
            auto iter = this->freeSocketChannel_.begin();
            std::vector<SocketChannelID> needRemove;
            for (; this->freeSocketChannel_.end() != iter; ++iter)
            {
                if (this->freeSocketChannel_.size() + this->useSocketChannel_.size() > this->minChannelCount_)
                {
                    if (timeNow - iter->second->createSTime_ > MIN_KEEP_CHANNEL_STIME)
                    {
                        needRemove.push_back(iter->first);
                    }
                }
            }
			
            for (uint32 iPos = 0; needRemove.size() > iPos; ++iPos)
            {
                this->freeSocketChannel_.erase(needRemove[iPos]);
            }
        }
		
        //sendHeartbeat
        auto iter = this->freeSocketChannel_.begin();
        for (; this->freeSocketChannel_.end() != iter; ++iter)
        {
            //判断最后发送心跳时间
            if(timeNow - iter->second->lastHeartbeatSTime_ < HEARTBEAT_INTVAL_STIME)
            {
                continue;
            }
			
            iter->second->lastHeartbeatSTime_ = timeNow;
            sendHeartbeat(iter->second);
        }
    }
	
    bool PocoSocketChannelPoolItem::isSocketChannelHealth(std::shared_ptr<ChannelInfo> channelInfo)
    {
        return sendHeartbeat(channelInfo);
    }
	
    bool PocoSocketChannelPoolItem::sendHeartbeat(std::shared_ptr<ChannelInfo> channelInfo)
    {
        if (channelInfo.get() && channelInfo->channel_)
        {
            std::shared_ptr<Request> request = std::make_shared<Request>();
            request->header_.message_id_ = this->heartbeatMessageID_;
            request->header_.ret_code_ = 0;
            request->body_data_buf_ = this->heartbeatMessageData_.c_str();
            request->body_data_len_ = this->heartbeatMessageData_.length();
			
            return channelInfo->channel_->send(request);
        }
		
        return false;
    }
	
    ISocketChannel<std::shared_ptr<Poco::Net::Socket>>* PocoSocketChannelPoolItem::getOrCreateChannel()
    {
        std::shared_ptr<ChannelInfo> channelInfo(NULL);
		
        //get a free channel
        {
            this_guard guard(mutex_);
			
            while (0 < this->freeSocketChannel_.size())
            {
                auto iter = this->freeSocketChannel_.begin();
                if (!isSocketChannelHealth(iter->second))
                {
                    this->freeSocketChannel_.erase(iter);
                    continue;
                }
			
                channelInfo = iter->second;
                this->useSocketChannel_[iter->first] = iter->second;
                this->freeSocketChannel_.erase(iter);
            }
        }		
		
        //create
        {
            if (NULL == channelInfo.get())
            {
                if (this->freeSocketChannel_.size() + this->useSocketChannel_.size() < this->maxChannelCount_)
                {
                    LOG_INFO(&g_serverInstance->logger(), "serverWatcher_:%p", this->serverWatcher_.get());
    				
                    ServerInfo serverInfo;
                    if (!this->serverWatcher_->getServerInfo(this->serverUniqueID_, serverInfo))
                    {
                        LOG_ERROR(&g_serverInstance->logger(), "getServerInfo failed! serverUniqueID:%u", this->serverUniqueID_);
                        return NULL;
                    }
		
                    auto channel = this->socketConnector_->createConnect(serverInfo.ip_, serverInfo.port_);
                    if (NULL == channel) return NULL;
			
                    channelInfo = std::make_shared<ChannelInfo>();
                    channelInfo->createSTime_ = time(NULL);
                    channelInfo->channel_ = channel;
				
                    this_guard guard(mutex_);
                    this->useSocketChannel_[channel->getChannelID()] = channelInfo;
                }
                else
                {
                    return NULL;
                }
            }
        }		
        return channelInfo->channel_;
    }
	

	
    PocoSocketChannelPool::PocoSocketChannelPool(std::shared_ptr<ISocketConnector<std::shared_ptr<Poco::Net::Socket>,
        Poco::AbstractObserver>> socketConnector, 
        uint16 minChannelCount,
        uint16 maxChannelCount,
        std::shared_ptr<IServerWatch> serverWatcher)
        : ISocketChannelPool<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>(socketConnector, minChannelCount, maxChannelCount)
        , serverWatcher_(serverWatcher)
    {
        Poco::Util::TimerTask::Ptr taskPtr = new Poco::Util::TimerTaskAdapter<PocoSocketChannelPool>(*this, &PocoSocketChannelPool::onHeartbeatTimer);
        this->heartbeatTimer_.schedule(taskPtr, 0, HEARTBEAT_INTVAL_STIME / 3);
    }
	
    PocoSocketChannelPool::~PocoSocketChannelPool()
    {
        heartbeatTimer_.cancel();
    }
		
		
    bool PocoSocketChannelPool::setMinChannelCount(ServerUniqueID serverUniqueID, uint16 minChannelCount)
    {
        std::shared_ptr<PocoSocketChannelPoolItem> item = getOrCreatePoolItem(serverUniqueID);
        if (NULL == item.get()) return false;
		
        item->setMinChannelCount(minChannelCount);
    }
	
    bool PocoSocketChannelPool::setMaxChannelCount(ServerUniqueID serverUniqueID, uint16 maxChannelCount)
    {
        std::shared_ptr<PocoSocketChannelPoolItem> item = getOrCreatePoolItem(serverUniqueID);
        if (NULL == item.get()) return false;
		
        item->setMaxChannelCount(maxChannelCount);
    }
	
    bool PocoSocketChannelPool::setHeartbeat(const std::string& heartbeatMessageData, MessageID heartbeatMessageID)
    {
        heartbeatMessageData_ = heartbeatMessageData;
        heartbeatMessageID_ = heartbeatMessageID;
    }
	
    bool PocoSocketChannelPool::setSocketConnector(ServerUniqueID serverUniqueID, std::shared_ptr<ISocketConnector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketConnector)
    {
        std::shared_ptr<PocoSocketChannelPoolItem> item = getOrCreatePoolItem(serverUniqueID);
        if (NULL == item.get()) return false;
		
        item->setSocketConnector(socketConnector);
    }
		
    ISocketChannel<std::shared_ptr<Poco::Net::Socket>>* PocoSocketChannelPool::getSocketChannel(ServerUniqueID serverUniqueID)
    {
        std::shared_ptr<PocoSocketChannelPoolItem> item = getOrCreatePoolItem(serverUniqueID);
        if (NULL == item.get()) return NULL;
		
        return item->getSocketChannel();
    }
	
    void PocoSocketChannelPool::releaseSocketChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *socketChannel)
    {
        if (NULL == socketChannel) return;
		
        std::shared_ptr<PocoSocketChannelPoolItem> item = getItemByChannel(socketChannel);
        if (NULL == item.get()) return;
		
        item->releaseSocketChannel(socketChannel);
    }
	

    void PocoSocketChannelPool::onConnected(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel)
    {
        //不需要处理
    }
	
    void PocoSocketChannelPool::onClose(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel)
    {
        if (NULL == channel) return;
		
        std::shared_ptr<PocoSocketChannelPoolItem> item = getItemByChannel(channel);
        if (NULL == item.get()) return;
		
        LOG_TRACE(&g_serverInstance->logger(), "onClose ChannelID:%u", channel->getChannelID());
		
        item->removeChannel(channel);
    }
	
    void PocoSocketChannelPool::onError(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel, const std::string& errorMsg)
    {
        if (NULL == channel) return;
		
        std::shared_ptr<PocoSocketChannelPoolItem> item = getItemByChannel(channel);
        if (NULL == item.get()) return;
		
        LOG_TRACE(&g_serverInstance->logger(), "onError ChannelID:%u errorMsg:%s", channel->getChannelID(), errorMsg.c_str());
		
        item->removeChannel(channel);
    }
		
    void PocoSocketChannelPool::onTimeout(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel)
    {
        //目前先不处理
    }

    void PocoSocketChannelPool::onHeartbeatTimer(Poco::Util::TimerTask& task)
    {
        static bool isHanding = false;
        if (!isHanding)
        {
            isHanding = true;
			
            std::vector<std::shared_ptr<PocoSocketChannelPoolItem>> needHandleItems;
            {
                this_guard guard(mutex_);
                auto iter = this->channelPoolItems_.begin();
                for (; this->channelPoolItems_.end() != iter; ++iter)
                {
                    needHandleItems.push_back(iter->second);
                }			
            }
		
            for (uint32 iPos = 0; needHandleItems.size() > iPos; ++iPos)
            {
                needHandleItems[iPos]->sendHeartbeat();
            }
			
            isHanding = false;
        }
    }
	
    std::shared_ptr<PocoSocketChannelPoolItem> PocoSocketChannelPool::getOrCreatePoolItem(ServerUniqueID serverUniqueID)
    {
        this_guard guard(mutex_);
		
        auto iter = this->channelPoolItems_.find(serverUniqueID);
        if (this->channelPoolItems_.end() == iter)
        {
            std::shared_ptr<PocoSocketChannelPoolItem> item = std::make_shared<PocoSocketChannelPoolItem>(socketConnector_, minChannelCount_, 
            	maxChannelCount_, serverUniqueID, serverWatcher_);

            item->setHeartbeat(this->heartbeatMessageData_, this->heartbeatMessageID_);
            this->channelPoolItems_[serverUniqueID] = item;
			
            return item;
        }
		
        return iter->second;
    }
	
	
    std::shared_ptr<PocoSocketChannelPoolItem> PocoSocketChannelPool::getItemByChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel)
    {
        if (NULL == channel) return std::shared_ptr<PocoSocketChannelPoolItem>(NULL);
		
        this_guard guard(mutex_);
        auto iter = this->channelPoolItems_.begin();
        for (; this->channelPoolItems_.end() != iter; ++iter)
        {
            if (iter->second->hasSocketChannel(channel->getChannelID())) return iter->second;
        }
		
        return std::shared_ptr<PocoSocketChannelPoolItem>(NULL);
    }
	
	
	
    PocoSocketChannelHandleApi::PocoSocketChannelHandleApi(std::shared_ptr<ISocketChannelPool<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> channelPool)
        : channelPool_(channelPool)
    {
		
    }
	
    PocoSocketChannelHandleApi::~PocoSocketChannelHandleApi()
    {
		
    }
		
    bool PocoSocketChannelHandleApi::sendMessageToServer(ServerUniqueID serverUniqueID, std::shared_ptr<Request> req)
    {
        auto channel = ((PocoSocketChannelPool*)this->channelPool_.get())->getSocketChannel(serverUniqueID);
        if (NULL == channel) return false;
		
        bool ret = channel->send(req);
        this->channelPool_->releaseSocketChannel(channel);

        return ret;
    }
	
    bool PocoSocketChannelHandleApi::sendMessageToClient(SocketChannelID channelID, std::shared_ptr<Request> req)
    {
        PocoServer *pocoServer = (PocoServer *)g_serverInstance.get();
        auto socketChannelManager = pocoServer->getSocketChannelManager();
        if (NULL == socketChannelManager.get()) return false;
		
        auto channel = socketChannelManager->getSocketChannelByChannelId(channelID);
        if (NULL == channel) return false;
		
        bool ret = channel->send(req);
        this->channelPool_->releaseSocketChannel(channel);

        return ret;
    }
	
	
	
	
	
	
	
	
	
}






