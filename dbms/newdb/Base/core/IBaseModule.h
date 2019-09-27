#pragma once

#include <Poco/Util/Subsystem.h>
#include <Poco/Util/Application.h>

#include <capnp/message.h>
#include <kj/io.h>
#include <capnp/serialize-packed.h>

#include "baseType.h"
#include "IContext.h"
#include "IServer.h"
#include "request.h"
#include "Common/logger_adapt.h"

namespace core
{
    
    /// @brief 业务模块基类，所有的业务模块都应该继承该类
	class IBaseModule : public Poco::Util::Subsystem
	{
	public:
    	IBaseModule() {}
		~IBaseModule() {}
		
    	/// @brief 模块初始化，此类中主要注册消息处理函数
    	void initialize(Poco::Util::Application& self)
    	{
        	this->initializeAllEntry();
    	}
    	
    	void reinitialize(Poco::Util::Application& app) {}
		
	protected:
    	/**
		 *	@brief 注册消息处理函数
		 *
		 *  @param messageID 消息ID
		 *  @param entry 消息对应的处理包装类
		 */
    	virtual void registerEntry(MessageID messageID, std::shared_ptr<IBaseEntry> entry)
    	{
        	g_serverInstance->getContext()->entrys_[messageID] = entry;
    	}
    	
    	//@brief 配合下面的宏定义，让子模块处理消息ID对应的处理注册
		virtual void initializeAllEntry() = 0;
	};
	
	
	
    /// @brief 消息处理的包装基类
	class IBaseEntry
	{
	public:
		IBaseEntry() {}
		virtual ~IBaseEntry() {}
		
    	/**
    	 *	@brief 消息处理函数
    	 *
    	 *  @param channelID 连接唯一标识
    	 *  @param req 请求消息包
    	 */
		virtual void handleMessage(SocketChannelID channelID, std::shared_ptr<Request> req) = 0;		
	};
	
	
    /// @brief 消息处理的包装类
    /// @tparam Owner 业务模块类型
    /// @tparam MessageType 消息类型
	template <class Owner, typename MessageType>
	class MessageHandleEntry : public IBaseEntry
	{		
	public:
    	/// @brief 业务的消息处理函数类型定义
		typedef void(Owner::*MessageHandleFun)(SocketChannelID channelID, const MessageHeader& header, const typename MessageType::Reader& protoObject);
		
    	/**
    	 *  @param owner 业务模块指针
    	 *  @param callFun 业务模块消息对应的处理函数
    	 */
		MessageHandleEntry(Owner *owner, MessageHandleFun callFun)
			: owner_(owner)
			, callFun_(callFun)
		{
			
		}
		
		~MessageHandleEntry() {}
		
		
    	/// @brief 见基类
		void handleMessage(SocketChannelID channelID, std::shared_ptr<Request> req)
		{
    		kj::ArrayPtr<const kj::byte>  arrayBuf((kj::byte*)req->body_data_buf_, (size_t)req->body_data_len_);

    		try
    		{
        		kj::ArrayInputStream inputStream(arrayBuf);
        		capnp::PackedMessageReader reader(inputStream);
				
        		typename MessageType::Reader personReader = reader.getRoot<MessageType>();
        		(owner_->*callFun_)(channelID, req->header_, personReader);
    		}
    		catch (...)
    		{
        		LOG_ERROR(&g_serverInstance->logger(), "handleMessage failed! channelID:%u", channelID);
        		return;
    		}
		}
		
		
	protected:
    	/// @brief 业务模块指针
		Owner *owner_;
    	/// @brief 业务模块消息对应的处理函数
		MessageHandleFun callFun_;
	};



/// @brief 放入业务模块的类定义中
#define MODULE_MESSAGE_HANDLE(moduleName) \
	void initializeAllEntry();\
    const char* name() const{ return #moduleName;}
	

/// @brief 放入业务模块的cpp实现中
#define MODULE_MESSAGE_HANDLE_BEGIN(moduleName) \
	void moduleName::initializeAllEntry(){
	


/// @brief 放入业务模块的cpp实现中  放在MODULE_MESSAGE_HANDLE_BEGIN、MODULE_MESSAGE_HANDLE_END中
#define MODULE_MESSAGE_HANDLE_REGISTER(moduleName, messageType, messageID, callFun) \
	this->registerEntry(messageID, std::make_shared<MessageHandleEntry<moduleName, messageType>>(this, callFun));
	
	
/// @brief 放入业务模块的cpp实现中
#define MODULE_MESSAGE_HANDLE_END() \
	}
	
	
	
	
}
	
	
