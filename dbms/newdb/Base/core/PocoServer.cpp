#include "core/PocoServer.h"

#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "Common/logger_adapt.h"
#include "core/baseType.h"
#include "protocol/base.capnp.h"

#include "core/PocoMultiSocketRector.h"
#include "core/PocoSocketAcceptor.h"
#include "core/PocoSocketChannelManager.h"
#include "core/PocoSocketConnector.h"
#include "core/PocoSocketChannelPool.h"
#include "core/PocoMessageHandler.h"
#include "core/DistributeServerWatcher.h"
#include "CoordinationClient/etcd/EtcdCoordinationClientApi.h"
#include "ConfigWatcher.h"
#include "CommonModule.h"


namespace core
{
  std::shared_ptr<IServer> g_serverInstance;

    
	const int CONNECTOR_TIMEOUT_STIME = 5;//连接超时时长
	const int CONNECTOR_POOL_MIN_COUNT = 5;//连接池最小连接数
	const int CONNECTOR_POOL_MAX_COUNT = 50;//连接池最大连接数

    /// @brief 用于连接唯一标识生成
    SocketChannelID g_lastCreateSocketID;
    
    
    PocoServer::PocoServer(const std::string& coordinationAddress, const std::map<std::string, uint16>& listenPorts)
		: coordinationAddress_(coordinationAddress)
		, listenPorts_(listenPorts)
	{
    	g_lastCreateSocketID = time(NULL);
    	
		this->threadCoreCount_ = get_nprocs_conf();
		this->sharedMemoryPoolPtr_ = std::make_shared<SharedMemoryPool>();
		this->context_ = std::make_shared<Context>();

    	g_serverInstance = std::shared_ptr<IServer>(this);

    	addSubsystem(new CommonModule());
	}
	
	PocoServer::~PocoServer()
	{
		
	}
		
    void PocoServer::setSocketChannelManager(std::shared_ptr<ISocketChannelManager<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketChannelManager)
	{
    	this->socketChannelManager_ = socketChannelManager;
	}
		
    std::shared_ptr<ISocketChannelManager<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> PocoServer::getSocketChannelManager()
	{
		return this->socketChannelManager_;
	}
	
	void PocoServer::setServerName(const std::string& serverName)
	{
		this->serverName_ = serverName;
	}
	
	std::string PocoServer::getServerName()
	{
		return this->serverName_;
	}
		
	std::shared_ptr<IConfigApi> PocoServer::getConfigApi(const std::string& label)
	{
		this_guard guard(mutex_);
		
		auto iter = configsApi_.find(label);
		if (configsApi_.end() == iter) return std::shared_ptr<IConfigApi>(NULL);
		
		return iter->second;
	}
	
	bool PocoServer::addConfig(const std::string& label, const std::string& configPath, std::shared_ptr<IConfigSetting> configSetting)
	{
		this_guard guard(mutex_);
		
		auto iter = configsApi_.find(label);
		if (configsApi_.end() != iter) return false;
		
		auto configWatcher = std::make_shared<ConfigWatcher>(configPath, this->coordinationClientApi_);
		if (!configWatcher->initialize()) return false;
		
		std::shared_ptr<IConfigApi> configApi(new ConfigApi(configWatcher, configSetting));
		configsApi_[label] = configApi;
		
		return true;
	}
		
	std::shared_ptr<Context> PocoServer::getContext()
	{
		return this->context_;
	}
		
	std::shared_ptr<ISocketChannelHandleApi> PocoServer::getClientHandleApi()
	{
		return this->socketChannelHandleApi_;
	}
	
	void PocoServer::setClientHandleApi(std::shared_ptr<ISocketChannelHandleApi> clientHandleApi)
	{
		this->socketChannelHandleApi_ = clientHandleApi;
	}
	
	
	ServerUniqueID PocoServer::getServerUniqueID()
	{
		return this->serverUniqueID_;
	}
	
	std::shared_ptr<coordination::ICoordinationClientApi> PocoServer::getCoordinationClientApi()
	{
		return this->coordinationClientApi_;
	}
	
	
	bool PocoServer::addServerPathToWatch(const std::string& serverPath)
	{
		if (this->serverWatch_.get())
		{
			return this->serverWatch_->addServerPathToWatch(serverPath);
		}
		
		return false;
	}
	
	std::shared_ptr<IServerWatch> PocoServer::getServerWatch()
	{
		return this->serverWatch_;
	}
	

	void PocoServer::initialize(Application& self)
	{
    	if (!build())
    	{
        	LOG_ERROR(&g_serverInstance->logger(), "build failed!");
        	exit(1);
    	}
    	
    	Poco::Util::Application::initialize(self);
	}
	
	void PocoServer::uninitialize()
	{
    	Poco::Util::ServerApplication::uninitialize();
	}
	
	void PocoServer::reinitialize(Application& self)
	{
    	Poco::Util::Application::reinitialize(self);
	}
	
	void PocoServer::defineOptions(Poco::Util::OptionSet& options)
	{
		
	}
	
	void PocoServer::handleConfig(const std::string& name, const std::string& value)
	{
		// 不处理
	}
		
	int PocoServer::main(const ArgVec& args)
	{
		//start
		{
			this->coordinationClientApi_->start();
			this->socketChannelManager_->getSocketRector()->run();
		}
		
		//等待退出信号
		waitForTerminationRequest();
		
		return Application::EXIT_OK;
	}



	bool PocoServer::build()
	{
		if (!buildLogger())
		{
    		LOG_ERROR(&g_serverInstance->logger(), "buildLogger failed!");
			return false;
		}
		
		if (!buildCoordination())
		{
    		LOG_ERROR(&g_serverInstance->logger(), "buildCoordination failed!");
			return false;
		}
		
		if (!buildNet())
		{
    		LOG_ERROR(&g_serverInstance->logger(), "buildNet failed!");
			return false;
		}
		
		if (!buildServerRegister())
		{
    		LOG_ERROR(&g_serverInstance->logger(), "buildServerRegister failed!");
			return false;
		}
		
		return true;
	}
		
	bool PocoServer::buildLogger()
	{
		return true;
	}
	
	bool PocoServer::buildCoordination()
	{		
    	this->coordinationClientApi_ = std::shared_ptr<coordination::ICoordinationClientApi>(new coordination::etcd::EtcdCoordinationClientApi());
		  if (!this->coordinationClientApi_->initialize(this->coordinationAddress_))
		  {
    		  LOG_ERROR(&g_serverInstance->logger(), "coordinationClientApi_->initialize failed!");
			  return false;
		  }

    	this->serverWatch_ = std::shared_ptr<IServerWatch>(new DistributeServerWatcher(this->coordinationClientApi_));
		
		  return true;
	}
	
	bool PocoServer::buildNet()
	{
		auto socketRector = std::shared_ptr<ISocketRector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>>(new PocoMultiSocketRector(this->threadCoreCount_));
		socketChannelManager_ = std::shared_ptr<ISocketChannelManager<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>>(new PocoSocketChannelManager());
		auto linkHandle = std::shared_ptr<ILinkHandler<std::shared_ptr<Poco::Net::Socket>>>(new PocoMessageHandler(this->context_->entrys_));	
		auto connector = std::shared_ptr<ISocketConnector<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>>(new PocoSocketConnector(socketRector, linkHandle, socketChannelManager_, (uint32)CONNECTOR_TIMEOUT_STIME, this->sharedMemoryPoolPtr_));
	
    connectPool_ = std::shared_ptr<ISocketChannelPool<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>>(new PocoSocketChannelPool(connector, CONNECTOR_POOL_MIN_COUNT, CONNECTOR_POOL_MAX_COUNT, this->serverWatch_));
    connector->setLinkEvent(connectPool_);		
    socketChannelHandleApi_ = std::shared_ptr<ISocketChannelHandleApi>(new PocoSocketChannelHandleApi(connectPool_));
		
		
		socketChannelManager_->addSocketConnector(CONNECTOR_BASE_LABEL, connector);
		socketChannelManager_->setSocketRector(socketRector);
		
		auto iter = this->listenPorts_.begin();
		for (; listenPorts_.end() != iter; ++iter)
		{
			auto acceptor = std::shared_ptr<ISocketAcceptor<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>>(new PocoSocketAcceptor(socketRector, linkHandle, socketChannelManager_, this->sharedMemoryPoolPtr_));
			if (!acceptor->initialize(iter->second))
			{
    			LOG_ERROR(&g_serverInstance->logger(), "acceptor->initialize failed! port:%u", iter->second);
				  return false;
			}
			
			socketChannelManager_->addSocketAcceptor(iter->first, acceptor);
		}

    /// 心跳
    {
        capnp::MallocMessageBuilder messBuilder;
        ultradb::protocol::base::HeatbeatRequest::Builder heatbeatBuilder = messBuilder.initRoot<ultradb::protocol::base::HeatbeatRequest>();
        heatbeatBuilder.setTimestamp(time(NULL));
	
        ultradb::protocol::base::RequestHeader::Builder headerBuilder = heatbeatBuilder.initHeader();
        headerBuilder.setSessionID(serverUniqueID_);
	
        kj::VectorOutputStream vectorOutputStream;
        capnp::writePackedMessage(vectorOutputStream, messBuilder);
	
        kj::ArrayPtr<kj::byte> result = vectorOutputStream.getArray();
        std::string data((char*)result.begin(), result.size());
        connectPool_->setHeartbeat(data, ultradb::protocol::base::HeatbeatRequest::MESSAGE_I_D);
    }
		
		return true;
	}
	
	bool PocoServer::buildServerRegister()
	{
    	
    	
    	return true;
	}
	
	
}
	

