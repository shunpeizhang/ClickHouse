#pragma once


#include <Poco/Net/SocketStream.h>

#include "baseType.h"
#include "ILink.h"
#include "IContext.h"


namespace core
{
	/// @brief 见基类
	class PocoMessageHandler : public ILinkHandler<std::shared_ptr<Poco::Net::Socket>>
	{
	public:
		PocoMessageHandler(std::map<MessageID, std::shared_ptr<IBaseEntry>>& entrys);
		~PocoMessageHandler();
		
    	/// @brief 见基类
		virtual bool onData(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel, std::shared_ptr<Request> req);
		
	protected:
    	/// @brief 弱引用，全局的消息ID与处理映射
		std::map<MessageID, std::shared_ptr<IBaseEntry>>& entrys_;
	};
	
	
	
	
	
	
	
}

