#include "CommonModule.h"


#include "Common/logger_adapt.h"
#include "IServer.h"
#include "protocol/base.capnp.h"

using namespace core;


MODULE_MESSAGE_HANDLE_BEGIN(CommonModule)
MODULE_MESSAGE_HANDLE_REGISTER(CommonModule, ultradb::protocol::base::HeatbeatRequest, ultradb::protocol::base::HeatbeatRequest::MESSAGE_I_D, &CommonModule::heatbeatRequest)    
MODULE_MESSAGE_HANDLE_END()

    
CommonModule::CommonModule()
{
    
}

CommonModule::~CommonModule()
{
    
}



void CommonModule::heatbeatRequest(SocketChannelID channelID, const MessageHeader& header, const typename ultradb::protocol::base::HeatbeatRequest::Reader& protoObject)
{
    LOG_INFO(&g_serverInstance->logger(), "heatbeatRequest channelID:%u sessionID:%u Timestamp:%u", channelID, protoObject.getHeader().getSessionID(), protoObject.getTimestamp());
}
