#include "core/PocoMessageHandler.h"


#include "core/request.h"
#include "core/IBaseModule.h"


namespace core
{
	
	
	PocoMessageHandler::PocoMessageHandler(std::map<MessageID, std::shared_ptr<IBaseEntry>>& entrys)
		: entrys_(entrys)
	{
		
	}
	
	PocoMessageHandler::~PocoMessageHandler()
	{
		
	}
	
	
	bool PocoMessageHandler::onData(ISocketChannel<std::shared_ptr<Poco::Net::Socket>> *channel, std::shared_ptr<Request> request)
	{
    	if (NULL == channel || NULL == request.get()) return false;
		
    	auto iter = entrys_.find(request->header_.message_id_);
    	if (entrys_.end() == iter)
    	{
        	return false;
    	}		
		
    	iter->second->handleMessage(channel->getChannelID(), request);
		
    	return true;
	}
	
	
	
	
	
	
	
	
	
	
	
	
}




