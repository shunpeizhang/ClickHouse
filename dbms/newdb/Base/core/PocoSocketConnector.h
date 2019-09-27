#pragma once



#include <Poco/Net/Socket.h>
#include <Poco/AbstractObserver.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketNotification.h>


#include "ILink.h"



namespace core
{
    /// @brief 见基类
	class PocoSocketConnector: public ISocketConnector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>
	{
	public:
    	/// @brief 见基类
		PocoSocketConnector(std::shared_ptr<ISocketRector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketRector,
        	std::shared_ptr<ILinkHandler<std::shared_ptr<Poco::Net::Socket>>> linkHandler, 
        	std::shared_ptr<ILinkEvent<std::shared_ptr<Poco::Net::Socket>>> linkEvent,
			uint32 timeMTimeOut,
			SharedMemoryPoolPtr sharedMemoryPoolPtr);
		
		~PocoSocketConnector();
		
    	/// @brief 见基类
		virtual ISocketChannel<std::shared_ptr<Poco::Net::Socket>>* createConnect(const std::string& ip, uint16 port);
		
	
	protected:
    	/// @brief 内存池，内存空间复用
		SharedMemoryPoolPtr sharedMemoryPoolPtr_;
		
	};
	
	
	
}








