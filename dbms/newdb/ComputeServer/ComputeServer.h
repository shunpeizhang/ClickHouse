#pragma once


#include "core/PocoServer.h"
#include "core/baseType.h"

using namespace core;


namespace ultradb
{


    class ComputeServer : public core::PocoServer
    {
    public:
        ComputeServer(const std::string& coordinationAddress, const std::map<std::string, uint16>& listenPorts);
        ~ComputeServer();


    protected:  
        virtual bool buildServerRegister();

        
    };












    
}











