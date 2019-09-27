#pragma once


#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/Util/IniFileConfiguration.h>


#include "baseType.h"
#include "CoordinationClient/ICoordinationEventHandler.h"
#include "CoordinationClient/ICoordinationClientApi.h"
#include "IConfig.h"

namespace core
{
	
    /// @brief 配置的实际处理、配置的变更监听
	class ConfigWatcher : public Poco::Util::IniFileConfiguration, public coordination::ICoordinationEventHandler
	{
	public:
    	/**
		 *  @param configPath 配置在协调服上的路径
		 *  @param coordinationClientApi 协调服操作api
		 */
		ConfigWatcher(const std::string& configPath, std::shared_ptr<coordination::ICoordinationClientApi> coordinationClientApi);
		~ConfigWatcher();
		
    	/// @brief 初始化，主要获取配置值及注册监听
		virtual bool initialize();
		
	protected:
    	/// @brief 见基类
		virtual void currentValueChange(const coordination::KeyValue& changeKeyValue);
    	/// @brief 见基类
		virtual void currentNodeDELETE(const string& key);
		
	protected:
    	/// @brief 配置在协调服上的路径
		std::string configPath_;
    	/// @brief 协调服操作api
		std::shared_ptr<coordination::ICoordinationClientApi> coordinationClientApi_;
		
    	/// @brief 是否已初始化
		bool isInit_;
	};
	
	
    /// @brief 见基类
	class ConfigApi : public IConfigApi
	{
	public:
    	/**
    	 *  @brief 此类是一个配制适配器类，方便业务层使用，实际的配置处理由configWatcher、configSetting完成
    	 *
		 *  @param configWatcher 配置处理类
		 *  @param configSetting 配置默认值
		 */
		ConfigApi(std::shared_ptr<ConfigWatcher> configWatcher, std::shared_ptr<IConfigSetting> configSetting);
		~ConfigApi();
		
    	/// @brief 见基类
		virtual std::string getString(const std::string& key) const;
		virtual int32 getInt(const std::string& key) const;
		virtual uint32 getUInt(const std::string& key) const;
		virtual int64 getInt64(const std::string& key) const;
		virtual uint64 getUInt64(const std::string& key) const;
		virtual bool getBool(const std::string& key) const;
		virtual float64 getDouble(const std::string& key) const;
		
	protected:
    	/// @brief 配置处理类
		std::shared_ptr<ConfigWatcher> configWatcher_;
    	/// @brief 配置默认值
		std::shared_ptr<IConfigSetting> configSetting_;
	};
	
	
	
	
	
	
	
	
	
}














