#pragma once

#include <Poco/Util/ServerApplication.h>



#include "baseType.h"


namespace coordination {
    class ICoordinationClientApi;
}


namespace core
{
    class IConfigSetting;
    class IConfigApi;
    class IServerWatch;
    class Context;
    class ISocketChannelHandleApi;
    
    
	/// @brief 服务基类，定义server需要实现的基本接口
	class IServer : public Poco::Util::ServerApplication
	{
    	typedef std::recursive_mutex this_mutex;
    	typedef std::lock_guard<std::recursive_mutex> this_guard;
    	
	public:
		virtual ~IServer() {}		
	    
    	/**
		 *	@brief 设置此服务名
		 *
		 *  @param serverName 服务名
		 */
		virtual void setServerName(const std::string& serverName) = 0;
    	/// @brief 获取服务名
		virtual std::string getServerName() = 0;
		
    	/// @brief 得到此服务的唯一标识
		virtual ServerUniqueID getServerUniqueID() = 0;
    	
    	/**
		 *	@brief 通过label得到配制api
		 *
		 *  @param label 标识是哪个配制
		 *  @return 返回对应配制的操作api
		 */
		virtual std::shared_ptr<IConfigApi> getConfigApi(const std::string& label) = 0;
    	
    	/**
		 *	@brief 添加某配制到服务中，配制的管理外部不需关心
		 *
		 *  @param label 标识是哪个配制
		 *  @param configPath 配制在协调服上的路径
		 *  @param configSetting 配制文件对应的默认配制
		 *  @return 添加配制是否成功
		 */
		virtual bool addConfig(const std::string& label, const std::string& configPath, std::shared_ptr<IConfigSetting> configSetting) = 0;    	
		
    	/// @brief 得到连接操作api，方便业务层使用，屏蔽底层细节
		virtual std::shared_ptr<ISocketChannelHandleApi> getClientHandleApi() = 0;
    	/// @brief 设置连接操作api，方便业务层使用，屏蔽底层细节
		virtual void setClientHandleApi(std::shared_ptr<ISocketChannelHandleApi> clientHandleApi) = 0;
    	
		
    	/// @brief 得到协调服操作api
		virtual std::shared_ptr<coordination::ICoordinationClientApi> getCoordinationClientApi() = 0;
		
    	/// @brief 添加需要监听的服务在协调服上的路径
		virtual bool addServerPathToWatch(const std::string& serverPath) = 0;
    	/// @brief 得到服务发现类
		virtual std::shared_ptr<IServerWatch> getServerWatch() = 0;
    	
    	virtual std::shared_ptr<Context> getContext() = 0;
    	
    	/// @brief 生成一个唯一id, 从0开始
    	virtual uint32 createUniqueID()
    	{
        	this_guard guard(mutex_);
        	
        	static uint32 uniqueID = 0;
        	return ++uniqueID;
    	}
    	
	protected:
    	this_mutex mutex_;
	};

	
    /// @brief 服务实例只能有一个，方便业务层使用
	extern std::shared_ptr<IServer> g_serverInstance;

}





