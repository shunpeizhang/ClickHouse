#include "core/IConfig.h"


namespace core
{
	
	
	const std::map<std::string, std::shared_ptr<ConfigSettingEntry>>& IConfigSetting::getAllConfigSetting()  const
	{
		return this->configSetting_;
	}
	
	std::shared_ptr<const ConfigSettingEntry> IConfigSetting::getSettingEntryByAlias(const std::string& configNameAlias)  const
	{
		auto iter = this->configSetting_.find(configNameAlias);
		if (this->configSetting_.end() != iter)
		{
			return iter->second;
		}
		
		return std::shared_ptr<const ConfigSettingEntry>(NULL);
	}
	
	
	bool IConfigSetting::setIntValueNoLimit(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, int64 value, const std::string& comment)
	{
		auto entry = std::make_shared<ConfigSettingEntry>();
		entry->comment = comment;
		entry->configNameAlias_ = configNameAlias;
		entry->configName_ = configName;
		entry->configEntryType_ = configEntryType;
		entry->configValue_.intValue_ = value;
		entry->hasLimitValue_ = false;
		
		configSetting_[entry->configNameAlias_] = entry;
		
		return true;
	}
	
	bool IConfigSetting::setUIntValueNoLimit(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, uint64 value, const std::string& comment)
	{
		auto entry = std::make_shared<ConfigSettingEntry>();
		entry->comment = comment;
		entry->configNameAlias_ = configNameAlias;
		entry->configName_ = configName;
		entry->configEntryType_ = configEntryType;
		entry->configValue_.uintValue_ = value;
		entry->hasLimitValue_ = false;
		
		configSetting_[entry->configNameAlias_] = entry;
		
		return true;
	}
	
	bool IConfigSetting::setFloatValueNoLimit(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, float64 value, const std::string& comment)
	{
		auto entry = std::make_shared<ConfigSettingEntry>();
		entry->comment = comment;
		entry->configNameAlias_ = configNameAlias;
		entry->configName_ = configName;
		entry->configEntryType_ = configEntryType;
		entry->configValue_.floatValue_ = value;
		entry->hasLimitValue_ = false;
		
		configSetting_[entry->configNameAlias_] = entry;
		
		return true;
	}
	
	bool IConfigSetting::setStringValueNoLimit(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, const std::string& value, const std::string& comment)
	{
		auto entry = std::make_shared<ConfigSettingEntry>();
		entry->comment = comment;
		entry->configNameAlias_ = configNameAlias;
		entry->configName_ = configName;
		entry->configEntryType_ = configEntryType;
		entry->stringValue_ = value;
		entry->hasLimitValue_ = false;
		
		configSetting_[entry->configNameAlias_] = entry;
		
		return true;
	}
		
	bool IConfigSetting::setIntValue(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, int64 value, int64 minValue, int64 maxValue, const std::string& comment)
	{
		auto entry = std::make_shared<ConfigSettingEntry>();
		entry->comment = comment;
		entry->configNameAlias_ = configNameAlias;
		entry->configName_ = configName;
		entry->configEntryType_ = configEntryType;
		entry->configValue_.intValue_ = value;
		entry->hasLimitValue_ = true;
		
		entry->minValue_.intValue_ = minValue;
		entry->maxValue_.intValue_ = maxValue;
		
		configSetting_[entry->configNameAlias_] = entry;
		
		return true;
	}
	
	bool IConfigSetting::setUIntValue(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, uint64 value, uint64 minValue, uint64 maxValue, const std::string& comment)
	{
		auto entry = std::make_shared<ConfigSettingEntry>();
		entry->comment = comment;
		entry->configNameAlias_ = configNameAlias;
		entry->configName_ = configName;
		entry->configEntryType_ = configEntryType;
		entry->configValue_.uintValue_ = value;
		entry->hasLimitValue_ = true;
		
		entry->minValue_.uintValue_ = minValue;
		entry->maxValue_.uintValue_ = maxValue;
		
		configSetting_[entry->configNameAlias_] = entry;
		
		return true;
	}
	
	bool IConfigSetting::setFloatValue(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, float64 value, float64 minValue, float64 maxValue, const std::string& comment)
	{
		auto entry = std::make_shared<ConfigSettingEntry>();
		entry->comment = comment;
		entry->configNameAlias_ = configNameAlias;
		entry->configName_ = configName;
		entry->configEntryType_ = configEntryType;
		entry->configValue_.floatValue_ = value;
		entry->hasLimitValue_ = true;
		
		entry->minValue_.floatValue_ = minValue;
		entry->maxValue_.floatValue_ = maxValue;
		
		configSetting_[entry->configNameAlias_] = entry;
		
		return true;
	}
	
	
	
	
	
	
}




