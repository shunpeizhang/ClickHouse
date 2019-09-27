#include "core/ConfigWatcher.h"


#include "core/IServer.h"
#include "Common/logger_adapt.h"


namespace core
{


	ConfigWatcher::ConfigWatcher(const std::string& configPath, std::shared_ptr<coordination::ICoordinationClientApi> coordinationClientApi)
		: configPath_(configPath)
		, coordinationClientApi_(coordinationClientApi)
	{
		this->isInit_ = false;
	}
	
	ConfigWatcher::~ConfigWatcher()
	{
		if (this->isInit_)
		{
			coordinationClientApi_->cancelWatchNode(this->configPath_);
		}
		isInit_ = false;
	}
		
	
	bool ConfigWatcher::initialize()
	{
		coordination::KeyValue keyValue;
		auto ret = coordinationClientApi_->getNodeValue(this->configPath_, keyValue);
		if (true != ret)
		{
			LOG_ERROR(&g_serverInstance->logger(), "coordinationClientApi_->getNodeValue failed! configPath_:%s", configPath_.c_str());
			return false;
		}
		
		try
		{
			load(keyValue.second);
		}
		catch (...)
		{
			LOG_ERROR(&g_serverInstance->logger(), "load failed! configPath_:%s", configPath_.c_str());
			return false;
		}
		
		ret = coordinationClientApi_->watchNode(this->configPath_, this);
		if (true != ret)
		{
			LOG_ERROR(&g_serverInstance->logger(), "coordinationClientApi_->watchNode failed! configPath_:%s", configPath_.c_str());
			return false;
		}
		
		this->isInit_ = true;
		
		return true;
	}
	

	void ConfigWatcher::currentValueChange(const coordination::KeyValue& changeKeyValue)
	{
		try
		{
			load(changeKeyValue.second);
		}
		catch (...)
		{
			LOG_ERROR(&g_serverInstance->logger(), "load failed! configPath_:%s", configPath_.c_str());
			return;
		}
	}
	
	void ConfigWatcher::currentNodeDELETE(const string& key)
	{
		//不处理		
	}
	
	
	ConfigApi::ConfigApi(std::shared_ptr<ConfigWatcher> configWatcher, std::shared_ptr<IConfigSetting> configSetting)
		: configWatcher_(configWatcher)
		, configSetting_(configSetting)
	{
		
	}
	
	ConfigApi::~ConfigApi()
	{
		
	}
		
	std::string ConfigApi::getString(const std::string& key) const
	{
		std::string defalut;
		std::shared_ptr<const ConfigSettingEntry> entry = configSetting_->getSettingEntryByAlias(key);
		if (entry.get()) defalut = entry->stringValue_;
		
		return configWatcher_->getString(entry->configName_, defalut);
	}
	
	int32 ConfigApi::getInt(const std::string& key) const
	{
		int32 defalut = 0;
		std::shared_ptr<const ConfigSettingEntry> entry = configSetting_->getSettingEntryByAlias(key);		
		if (entry.get()) defalut = entry->configValue_.intValue_;
		
		auto result = configWatcher_->getInt(entry->configName_, defalut);
		if (entry.get() && entry->hasLimitValue_)
		{
			if (result < entry->minValue_.intValue_ || result > entry->maxValue_.intValue_) result = entry->configValue_.intValue_;
		}
		
		return result;
	}
	
	uint32 ConfigApi::getUInt(const std::string& key) const
	{
		uint32 defalut = 0;
		std::shared_ptr<const ConfigSettingEntry> entry = configSetting_->getSettingEntryByAlias(key);		
		if (entry.get()) defalut = entry->configValue_.uintValue_;
		
		auto result = configWatcher_->getUInt(entry->configName_, defalut);
		if (entry.get() && entry->hasLimitValue_)
		{
			if (result < entry->minValue_.uintValue_ || result > entry->maxValue_.uintValue_) result = entry->configValue_.uintValue_;
		}
		
		return result;
	}
	
	int64 ConfigApi::getInt64(const std::string& key) const
	{
		int64 defalut = 0;
		std::shared_ptr<const ConfigSettingEntry> entry = configSetting_->getSettingEntryByAlias(key);		
		if (entry.get()) defalut = entry->configValue_.intValue_;
		
		auto result = configWatcher_->getInt64(entry->configName_, defalut);
		if (entry.get() && entry->hasLimitValue_)
		{
			if (result < entry->minValue_.intValue_ || result > entry->maxValue_.intValue_) result = entry->configValue_.intValue_;
		}
		
		return result;
	}
	
	uint64 ConfigApi::getUInt64(const std::string& key) const
	{
		uint64 defalut = 0;
		std::shared_ptr<const ConfigSettingEntry> entry = configSetting_->getSettingEntryByAlias(key);		
		if (entry.get()) defalut = entry->configValue_.uintValue_;
		
		auto result = configWatcher_->getUInt64(entry->configName_, defalut);
		if (entry.get() && entry->hasLimitValue_)
		{
			if (result < entry->minValue_.uintValue_ || result > entry->maxValue_.uintValue_) result = entry->configValue_.uintValue_;
		}
		
		return result;
	}
	
	bool ConfigApi::getBool(const std::string& key) const
	{
		int32 defalut = 0;
		std::shared_ptr<const ConfigSettingEntry> entry = configSetting_->getSettingEntryByAlias(key);		
		if (entry.get()) defalut = entry->configValue_.intValue_;
		
		auto result = configWatcher_->getBool(entry->configName_, 0 != defalut);
		
		return result;
	}
	
	float64 ConfigApi::getDouble(const std::string& key) const
	{
		double defalut = 0;
		std::shared_ptr<const ConfigSettingEntry> entry = configSetting_->getSettingEntryByAlias(key);		
		if (entry.get()) defalut = entry->configValue_.floatValue_;
		
		auto result = configWatcher_->getDouble(entry->configName_, defalut);
		if (entry.get() && entry->hasLimitValue_)
		{
			if (result < entry->minValue_.floatValue_ || result > entry->maxValue_.floatValue_) result = entry->configValue_.floatValue_;
		}
		
		return result;
	}


	
	
	
	
	

}


