#pragma once


#include <Poco/Net/Socket.h>
#include <Poco/AbstractObserver.h>
#include <Poco/Net/SocketReactor.h>


#include "core/IServer.h"
#include "core/ILink.h"


namespace core
{
	
    class IServerWatch;
    
    /// @brief 见基类
	class PocoServer : public IServer
	{
		typedef std::recursive_mutex this_mutex;
		typedef std::lock_guard<std::recursive_mutex> this_guard;
		
	public:
    	/**
		 *  @param coordinationAddress 协调服地址信息
		 *  @param listenPorts 网络监听信息
		 */
		PocoServer(const std::string& coordinationAddress, const std::map<std::string , uint16>& listenPorts);
		~PocoServer();
		
    	/// @brief 设置连接管理器
		void setSocketChannelManager(std::shared_ptr<ISocketChannelManager<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketChannelManager);
    	/// @brief 得到连接管理器
		std::shared_ptr<ISocketChannelManager<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> getSocketChannelManager();
	
    	/// @brief 见基类
		void setServerName(const std::string& serverName);
    	/// @brief 见基类
		std::string getServerName();
		
    	/// @brief 见基类
		std::shared_ptr<IConfigApi> getConfigApi(const std::string& label);
    	/// @brief 见基类
		bool addConfig(const std::string& label, const std::string& configPath, std::shared_ptr<IConfigSetting> configSetting);
		
    	/// @brief 见基类
		std::shared_ptr<Context> getContext();
		
    	/// @brief 见基类
		std::shared_ptr<ISocketChannelHandleApi> getClientHandleApi();
    	/// @brief 见基类
		void setClientHandleApi(std::shared_ptr<ISocketChannelHandleApi> clientHandleApi);
		
    	/// @brief 见基类
		ServerUniqueID getServerUniqueID();
		
    	/// @brief 见基类
		std::shared_ptr<coordination::ICoordinationClientApi> getCoordinationClientApi();
		
    	/// @brief 见基类
		bool addServerPathToWatch(const std::string& serverPath);
    	/// @brief 见基类
		std::shared_ptr<IServerWatch> getServerWatch();
		
	protected:
    	/// @brief 见基类
		void initialize(Application& self);
    	/// @brief 见基类
		void uninitialize();
    	/// @brief 见基类
		void reinitialize(Application& self);
    	/// @brief 见基类
		void defineOptions(Poco::Util::OptionSet& options);
    	/// @brief 见基类
		void handleConfig(const std::string& name, const std::string& value);
		
    	/// @brief 见基类
		int main(const ArgVec& args);
		
		
	protected:
		//以下接口方便子类对具体实现进行改写
		virtual bool build();
		
    	/// @brief 初始化日志
		virtual bool buildLogger();
    	/// @brief 初始化协调client模块
		virtual bool buildCoordination();
    	/// @brief 初始化网络模块
		virtual bool buildNet();
    	/// @brief 注册此服务
		virtual bool buildServerRegister();
		
		
	protected:
    	/// @brief 协调服地址
		std::string coordinationAddress_;
    	/// @brief 网络监听信息
		std::map<std::string, uint16> listenPorts_;
			
    	/// @brief 协调服client api
		std::shared_ptr<coordination::ICoordinationClientApi> coordinationClientApi_;
    	/// @brief 所有配制api模块
		std::map<std::string, std::shared_ptr<IConfigApi>> configsApi_;
		
    	/// @brief 内存池
		SharedMemoryPoolPtr sharedMemoryPoolPtr_;
		std::shared_ptr<Context> context_;
    
		
    	/// @brief 使用线程数
		int threadCoreCount_;
    	/// @brief 连接管理器
		std::shared_ptr<ISocketChannelManager<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> socketChannelManager_;
    std::shared_ptr<ISocketChannelPool<std::shared_ptr<Poco::Net::Socket>, Poco::AbstractObserver>> connectPool_;
    std::string serverName_;
		
    	/// @brief 服务发现
		std::shared_ptr<IServerWatch> serverWatch_;
    	/// @brief 连接处理api
		std::shared_ptr<ISocketChannelHandleApi> socketChannelHandleApi_;
		
    	/// @brief 此服务唯一标识
		ServerUniqueID serverUniqueID_;
		
		this_mutex mutex_;
	};
	
	
	
	
}














