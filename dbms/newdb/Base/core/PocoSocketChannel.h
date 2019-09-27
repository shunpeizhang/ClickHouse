#pragma once


#include <Poco/Net/SocketStream.h>
#include <Poco/Observer.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Logger.h>


#include "ILink.h"

namespace core
{
    /// @brief 见基类
	class PocoSocketChannel : public ISocketChannel<std::shared_ptr<Poco::Net::Socket>>
	{
	public:
    	/// @brief 数据接收处理状态
		enum class SocketChannelState
		{
			Invalid,
    		/// @brief 可以开始接收一个新消息
			ready,
    		/// @brief 一个新消息数据正在处理中
			readingData,
    		/// @brief 一个新消息数据已接收完成，正在处理消息中
			handlingMessage,
		};
		
		
		typedef Poco::Observer<PocoSocketChannel, Poco::Net::ReadableNotification> ReadableObserver;
		typedef Poco::Observer<PocoSocketChannel, Poco::Net::ShutdownNotification> ShutdownObserver;
		typedef Poco::Observer<PocoSocketChannel, Poco::Net::ErrorNotification> ErrorObserver;
		
		
    	/**
		 *  @param socket poco中的socket
		 *  @param reactor 反应器
		 *  @param sharedMemoryPoolPtr 内存池
		 */
		PocoSocketChannel(std::shared_ptr<Poco::Net::StreamSocket> socket, 
			std::shared_ptr<ISocketRector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>>  reactor, 
			SharedMemoryPoolPtr sharedMemoryPoolPtr);
		~PocoSocketChannel();
		
		
    	/// @brief 见基类
		virtual bool send(std::shared_ptr<Request> resp);
    	/// @brief 见基类
		virtual void setTimeout(int sTime);
		
    	/// @brief 见基类
		virtual void close();
    	/// @brief 连接异常处理
		virtual void errorHandle(const std::string& errMsg);
    	/// @brief filter新消息的消息包
    	/// @return 如果返回为空，说明消息包被过滤了，不需要处理
		virtual std::shared_ptr<Request> inFilterHandle(std::shared_ptr<Request> request);
    	/// @brief filter需要发送的消息包
    	/// @return 如果返回为空，说明消息包被过滤了，不需要处理
		virtual std::shared_ptr<Request> outFilterHandle(std::shared_ptr<Request> request);
		
    	/// @brief 见基类
		virtual const std::shared_ptr<Poco::Net::Socket> getSocket() const;
		
	protected:
    	/// @brief 见基类
		void onReadable(Poco::Net::ReadableNotification* pNf);
    	/// @brief 见基类
		void onShutdown(Poco::Net::ShutdownNotification* pNf);
    	/// @brief 见基类
		void onTimeout(Poco::Net::TimeoutNotification* pNf);
    	/// @brief 见基类
		void onError(Poco::Net::ErrorNotification* pNf);
		
    	/// @brief 向反应器中注册此对象(观察者)
		bool addObserver();
    	/// @brief 从反应器中删除此对象(观察者)
		bool removeObserver();
		
	protected:
		PocoSocketChannel() {}
		
		PocoSocketChannel(const PocoSocketChannel&) {}
		PocoSocketChannel& operator =(const PocoSocketChannel&) {}
		
		
	protected:
    	/// @brief poco中的socket
		std::shared_ptr<Poco::Net::StreamSocket> socket_;
    	/// @brief 反应器
		std::shared_ptr<ISocketRector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> reactor_;
    	/// @brief 内存池
		SharedMemoryPoolPtr sharedMemoryPoolPtr_;
		
    	/// @brief 消息包接收状态
		SocketChannelState socketChannelState_;
    	/// @brief 消息包总长度
		MessageLen messageLen_;
    	/// @brief 接收消息缓冲区
		char *recvMessageBuf_;
    	/// @brief 消息包已接收长度
		int recvMessageLen_;
    	
	};
	
	
	
	
	
	
	
	
	


	
	
	
	
	
	
	



}




