#include "ComputeServer.h"

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "Common/logger_adapt.h"
#include "core/IServerWatch.h"
#include "protocol/base.capnp.h"


using namespace ultradb;



ComputeServer::ComputeServer(const std::string& coordinationAddress, const std::map<std::string, uint16>& listenPorts)
    : core::PocoServer(coordinationAddress, listenPorts)
{
    
}

ComputeServer::~ComputeServer()
{



    
}


bool ComputeServer::buildServerRegister()
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
        info.port_ = 10001;
        info.serverPath_ = "/service/ComputeServer";
        info.ServerType_ = ServerNodeType::CALCUTE_NODE;
        info.serverUniqueID_ = 101;
        
        getServerWatch()->setOwnServerInfo("/service/ComputeServer", info);

        getServerWatch()->addServerPathToWatch("/service/");
    }

    return true;
}



int main(int argc, char *argv[])
{
    try 							
    {
        std::map<std::string, uint16> listenPorts;
        listenPorts["base"] = 10001;
    	
        ultradb::ComputeServer computeServer("127.0.0.1:2379", listenPorts);
        return computeServer.run(argc, argv);
    }
    catch (Poco::Exception& exc)
    {
        std::cerr << exc.displayText() << std::endl;
        return Poco::Util::Application::EXIT_SOFTWARE;
    }

    return Poco::Util::Application::EXIT_OK;
}

