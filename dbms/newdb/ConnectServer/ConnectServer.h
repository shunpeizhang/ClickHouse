#pragma once


#include "core/PocoServer.h"
#include "core/baseType.h"

using namespace core;


namespace ultradb
{


    class ConnectServer : public core::PocoServer
    {
    public:
        ConnectServer(const std::string& coordinationAddress, const std::map<std::string, uint16>& listenPorts);
        ~ConnectServer();

    protected:  
        virtual bool buildServerRegister();
        
    };












    
    }











