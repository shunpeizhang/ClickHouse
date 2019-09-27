#pragma once

#include <Poco/Util/Application.h>


#include "baseType.h"
#include "IBaseModule.h"
#include "request.h"
#include "protocol/base.capnp.h"



namespace core
{


    class CommonModule : public IBaseModule
    {
    public:
        CommonModule();
        ~CommonModule();
        
        MODULE_MESSAGE_HANDLE(CommonModule);

        void initialize(Poco::Util::Application& app) {IBaseModule::initialize(app);}
        void uninitialize() {}
        
    protected:
        void heatbeatRequest(SocketChannelID channelID, const MessageHeader& header, const typename ultradb::protocol::base::HeatbeatRequest::Reader& protoObject);
        
    };






    
    }




