#pragma once


#include <Poco/Net/Socket.h>
#include <Poco/AbstractObserver.h>


#include "core/ILink.h"


namespace core
{
	/// @brief 见基类
	class PocoSocketChannelManager : public ISocketChannelManager<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>
	{
    	typedef std::recursive_mutex this_mutex;
    	typedef std::lock_guard<std::recursive_mutex> this_guard;
    	
	public:
		PocoSocketChannelManager();
		~PocoSocketChannelManager();
		
		/// @brief 见基类
		virtual void addSocketChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel);
    	/// @brief 见基类
		virtual void removeSocketChannel(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel);
    	/// @brief 见基类
		virtual void eraseSocketChannelNotDelete(SocketChannelID channelID);
    	/// @brief 见基类
		virtual void removeSocketChannel(SocketChannelID channelID);
    	/// @brief 见基类
		virtual ISocketChannel<std::shared_ptr<Poco::Net::Socket>>* getSocketChannelByChannelId(SocketChannelID channelID);
		
    	/// @brief 见基类
		virtual void closeAll();
	protected:
    	/// @brief 见基类
		virtual void onClose(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel);
    	/// @brief 见基类
		virtual void onError(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel, const std::string& errorMsg);
		
    	/// @brief 见基类
		virtual void onTimeout(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel);
		
	protected:
    	/// @brief 保存所有连接
		std::map<SocketChannelID, ISocketChannel<std::shared_ptr<Poco::Net::Socket>>*> allSocketChannel_;
		
    	this_mutex mutex_;
	};
	
	
	
	
	
	
	
}





