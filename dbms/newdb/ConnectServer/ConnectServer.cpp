#include "ConnectServer.h"

#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>

#include "Common/logger_adapt.h"
#include "core/IServerWatch.h"
#include "protocol/base.capnp.h"


using namespace ultradb;



ConnectServer::ConnectServer(const std::string& coordinationAddress, const std::map<std::string, uint16>& listenPorts)
    : core::PocoServer(coordinationAddress, listenPorts)
{
    
}

ConnectServer::~ConnectServer()
{



    
}


bool ConnectServer::buildServerRegister()
{
    if (!PocoServer::buildServerRegister())
    {
        LOG_ERROR(&g_serverInstance->logger(), "PocoServer::buildServerRegister failed!");
        return false;
    }
    
    if (this->serverWatch_.get())
    {
        ServerInfo info;
        info.ip_ = "172.16.11.17";
        info.port_ = 10000;
        info.serverPath_ = "/service/ConnectServer";
        info.ServerType_ = ServerNodeType::LINK_NODE;
        info.serverUniqueID_ = 100;
        
        getServerWatch()->setOwnServerInfo("/service/ConnectServer", info);

        getServerWatch()->addServerPathToWatch("/service/");
    }

    LOG_INFO(&g_serverInstance->logger(), "buildServerRegister success");

    return true;
}


int main(int argc, char *argv[])
{
    try
    {
        std::map<std::string, uint16> listenPorts;
        listenPorts["base"] = 10000;
    	
        ultradb::ConnectServer connectServer("127.0.0.1:2379", listenPorts);
        return connectServer.run(argc, argv);
    }
    catch (Poco::Exception& exc)
    {
        std::cerr << exc.displayText() << std::endl;
        return Poco::Util::Application::EXIT_SOFTWARE;
    }

    return Poco::Util::Application::EXIT_OK;
}
