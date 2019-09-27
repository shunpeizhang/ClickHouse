#include "core/PocoSocketChannelManager.h"

#include "core/PocoServer.h"
#include "Common/logger_adapt.h"

namespace core
{


    PocoSocketChannelManager::PocoSocketChannelManager()
    {
		
    }
	
    PocoSocketChannelManager::~PocoSocketChannelManager()
    {
		
    }
		
		
    void PocoSocketChannelManager::addSocketChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel)
    {
        this_guard guard(mutex_);
    	
        if (channel) this->allSocketChannel_[channel->getChannelID()] = channel;		
    }
	
    void PocoSocketChannelManager::removeSocketChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel)
    {
        this_guard guard(mutex_);
    	
        if (channel) 
        {
            auto iter = this->allSocketChannel_.find(channel->getChannelID());
            if (this->allSocketChannel_.end() == iter)
            {
                LOG_WARNING(&g_serverInstance->logger(), "eraseSocketChannel failed! not found channelID:%u", channel->getChannelID());
                return;
            }

            auto channelID = channel->getChannelID();
            if (iter->second)
            {
                delete iter->second;
                iter->second = NULL;
            }
		
            this->allSocketChannel_.erase(channelID);
        }
    }
	
    void PocoSocketChannelManager::removeSocketChannel(SocketChannelID channelID)
    {
        this_guard guard(mutex_);
    	
        auto iter = this->allSocketChannel_.find(channelID);
        if (this->allSocketChannel_.end() == iter)
        {
            LOG_WARNING(&g_serverInstance->logger(), "eraseSocketChannel failed! not found channelID:%u", channelID);
            return;
        }
		
        if (iter->second)
        {
            delete iter->second;
            iter->second = NULL;
        }
		
        this->allSocketChannel_.erase(channelID);
    }
	
    void PocoSocketChannelManager::eraseSocketChannelNotDelete(SocketChannelID channelID)
    {
        this_guard guard(mutex_);
    	
        auto iter = this->allSocketChannel_.find(channelID);
        if (this->allSocketChannel_.end() == iter)
        {
            LOG_WARNING(&g_serverInstance->logger(), "eraseSocketChannel failed! not found channelID:%u", channelID);
            return;
        }
		
        this->allSocketChannel_.erase(channelID);
    }
	
    ISocketChannel<std::shared_ptr<Poco::Net::Socket>>* PocoSocketChannelManager::getSocketChannelByChannelId(SocketChannelID channelID)
    {
        this_guard guard(mutex_);
    	
        auto iter = this->allSocketChannel_.find(channelID);
        if (this->allSocketChannel_.end() == iter)
        {
            return NULL;
        }
		
        return this->allSocketChannel_[channelID];
    }
		
    void PocoSocketChannelManager::closeAll()
    {
        this_guard guard(mutex_);
    	
        auto iter = this->allSocketChannel_.begin();
        for (; this->allSocketChannel_.end() != iter; ++iter)
        {
            eraseSocketChannelNotDelete(iter->first);
        }
    }
	
    void PocoSocketChannelManager::onClose(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel)
    {
        if (channel) LOG_INFO(&g_serverInstance->logger(), "channelID:%u onClose", channel->getChannelID());
		
        removeSocketChannel(channel);
    }
	
    void PocoSocketChannelManager::onError(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel, const std::string& errorMsg)
    {
        if (channel) LOG_INFO(&g_serverInstance->logger(), "channelID:%u onError! errorMsg:%s", channel->getChannelID(), errorMsg.c_str());
		
        removeSocketChannel(channel);
    }
		
    void PocoSocketChannelManager::onTimeout(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel)
    {
        //目前不处理
    }




	
	
}
	
	
	