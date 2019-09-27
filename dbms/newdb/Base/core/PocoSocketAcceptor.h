#pragma once

#include <Poco/Net/Socket.h>
#include <Poco/AbstractObserver.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/ServerSocket.h>


#include "ILink.h"



namespace core
{
	
	class PocoSocketChannel;
    class PocoServerSocketChannel;
	
    
    /// @brief 见基类
	class PocoSocketAcceptor : public ISocketAcceptor<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>
    {
        typedef Poco::Observer<PocoSocketAcceptor, Poco::Net::ReadableNotification> Observer;
		
    public:
        /// @brief 见基类
        PocoSocketAcceptor(std::shared_ptr<ISocketRector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketRector,
            std::shared_ptr<ILinkHandler<std::shared_ptr<Poco::Net::Socket>>> linkHandler, 
            std::shared_ptr<ILinkEvent<std::shared_ptr<Poco::Net::Socket>>> linkEvent,
            SharedMemoryPoolPtr sharedMemoryPoolPtr);
        ~PocoSocketAcceptor();
		
        /// @brief 见基类
        virtual bool initialize(int serverPort);
		
        /**
		 *	@brief 处理有新连接事件
		 *
		 *  @param pNotification 新连接事件
		 */
        void onAccept(Poco::Net::ReadableNotification* pNotification);
		
    private:
        /// @brief 监听用的socket
        std::shared_ptr<PocoServerSocketChannel> serverSocket_;
        /// @brief 内存池，内存空间复用
        SharedMemoryPoolPtr sharedMemoryPoolPtr_;
    };
    
    
    /// @brief 监听用的socket，监听socket与普通socket处理方式不一样，所以不能复用
	class PocoServerSocketChannel : public ISocketChannel<std::shared_ptr<Poco::Net::Socket>>
	{
	public:
    	/**
		 *  @param socket poco中的监听socket
		 */
		PocoServerSocketChannel(std::shared_ptr<Poco::Net::ServerSocket> socket)
			: socket_(socket)			
		{
			
		}
		
    	/// @brief 只是简单实现，不会被使用
		virtual bool send(std::shared_ptr<Request> resp) {}

    	/// @brief 只是简单实现，不会被使用
		virtual void setTimeout(int sTime) {}
		
    	/// @brief 获取poco中的socket
		virtual const std::shared_ptr<Poco::Net::ServerSocket> getServerSocket() const {return this->socket_;}
    	/// @brief 获取poco中的socket
		virtual const std::shared_ptr<Poco::Net::Socket> getSocket() const {return this->socket_;}
		
		
	private:
    	/// @brief socket poco中的监听socket
		std::shared_ptr<Poco::Net::ServerSocket> socket_;
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	}




