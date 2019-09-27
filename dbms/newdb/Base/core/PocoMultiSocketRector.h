#pragma once


#include <Poco/Net/Socket.h>
#include <Poco/AbstractObserver.h>
#include <Poco/Net/SocketReactor.h>


#include "core/ILink.h"


namespace core
{
	
	/// @brief 见基类
	class PocoMultiSocketRector : public ISocketRector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>
	{
	public:
    	/**
		 *  @param handleThreadCount 创建多少个SocketReactor和对应的线程
		 */
		PocoMultiSocketRector(int32 handleThreadCount);
		~PocoMultiSocketRector();
		
		/// @brief 见基类
		virtual void run();
    	/// @brief 见基类
		virtual void stop();
		
    	/// @brief 见基类
		virtual void addEventHandler(const ISocketChannel<std::shared_ptr<Poco::Net::Socket>>& socket, const Poco::AbstractObserver& observer);
    	/// @brief 见基类
		virtual bool hasEventHandler(const ISocketChannel<std::shared_ptr<Poco::Net::Socket>>& socket, const Poco::AbstractObserver& observer);
    	/// @brief 见基类
		virtual void removeEventHandler(const ISocketChannel<std::shared_ptr<Poco::Net::Socket>>& socket, const Poco::AbstractObserver& observer);
		
		
	protected:
    	/**
		 *	@brief 内部接口，得到连接对应的rector，发挥多个rector
		 *
		 *  @param socket 连接
		 */
		std::shared_ptr<Poco::Net::SocketReactor> getReactor(const ISocketChannel<std::shared_ptr<Poco::Net::Socket>>& socket);
	
	protected:
    	/// @brief 创建多少个SocketReactor和对应的线程
		int32 handleThreadCount_;
    	/// @brief 线程容器，与reactors_数量对应
		std::vector<std::shared_ptr<Poco::Thread>> threads_;
    	/// @brief reactor容器，handleThreadCount_个
		std::vector<std::shared_ptr<Poco::Net::SocketReactor>> reactors_;
		
    	/// @brief 是否已开启事件分派
		bool isStart_;
		
		
	};
	
	
	
	
	
	
}



