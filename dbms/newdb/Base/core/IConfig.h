#pragma once

#include "baseType.h"

namespace core
{
    /// @brief 配置中支持的类型
	enum class ConfigEntryType
	{
		Invalid,
		Bool,
		Int8,
		Int16,
		Int32,
		Int64,
		UInt8,
		UInt16,
		UInt32,
		UInt64,
		Float32,
		Float64,
		String,
	};
	
    /// @brief 配置中的一个配置的所有信息
	struct ConfigSettingEntry
	{
    	/// @brief 配置的值类型定义
		union Value
		{
			int64 intValue_;
			uint64 uintValue_;
			float64 floatValue_;
//			std::string stringValue_;
		};
		
    	/// @brief 配置的值范围类型定义
		union LimitValue
		{
			int64 intValue_;
			uint64 uintValue_;
			float64 floatValue_;
		};
		
    	/// @brief 配置的别名，方便业务层使用  例如：root.module.config(配置名) -> module_config(别名)
		std::string configNameAlias_;
    	/// @brief 配置名，方便业务层使用  例如：root.module.config(配置名) -> module_config(别名)
		std::string configName_;
    	/// @brief 配置说明信息
		std::string comment;
    	/// @brief 配置类型
		ConfigEntryType configEntryType_;
		
    	/// @brief 配置值
		Value configValue_;
    	std::string stringValue_;
		
    	/// @brief 是否存在配置值范围限制
		bool hasLimitValue_;
    	/// @brief 配置值的最小值
		LimitValue minValue_;
    	/// @brief 配置值的最大值
		LimitValue maxValue_;
	};
	
    
    /// @brief 配置默认值基类，用于设置某配制的默认值
	class IConfigSetting
	{
	public:
    	/**
		 *	@brief 设置配置值的默认值，没有范围限制
		 * 
		 *  @tparam T 配置项的具体类型
		 *  @param configNameAlias 配置别名
		 *  @param configName 配置名
		 *  @param comment 配置说明
		 *  @param value 配置值
		 */
		template <class T>
		bool setValueNoLimit(const std::string& configNameAlias, const std::string& configName, T value, const std::string& comment)
		{
			ConfigEntryType configEntryType = getConfigEntryType(value);
			
			if (ConfigEntryType::Int8 == configEntryType || ConfigEntryType::Int16 == configEntryType ||
				ConfigEntryType::Int32 == configEntryType || ConfigEntryType::Int64 == configEntryType ||
				ConfigEntryType::Bool == configEntryType)
			{
				return setIntValueNoLimit(configNameAlias, configName, configEntryType, value, comment);
			}
			
			if (ConfigEntryType::UInt8 == configEntryType || ConfigEntryType::UInt16 == configEntryType ||
				ConfigEntryType::UInt32 == configEntryType || ConfigEntryType::UInt64 == configEntryType)
			{
				return setUIntValueNoLimit(configNameAlias, configName, configEntryType, value, comment);
			}
			
			if (ConfigEntryType::Float32 == configEntryType || ConfigEntryType::Float64 == configEntryType)
			{
				return setFloatValueNoLimit(configNameAlias, configName, configEntryType, value, comment);
			}
			
			if (ConfigEntryType::Float32 == configEntryType || ConfigEntryType::Float64 == configEntryType)
			{
				return setStringValueNoLimit(configNameAlias, configName, configEntryType, value, comment);
			}
			
			return false;
		}
		
    	/**
		 *	@brief 设置配置值的默认值，有范围限制
		 *
		 *  @tparam T 配置项的具体类型
		 *  @param configNameAlias 配置别名
		 *  @param configName 配置名
		 *  @param comment 配置说明
		 *  @param value 配置值
		 *  @param minValue 配置最小值
		 *  @param maxValue 配置最大值
		 */
		template <class T>
		bool setValue(const std::string& configNameAlias, const std::string& configName, T value, T minValue, T maxValue, const std::string& comment)
		{
			ConfigEntryType configEntryType = getConfigEntryType(value);
			
			if (ConfigEntryType::Int8 == configEntryType || ConfigEntryType::Int16 == configEntryType ||
				ConfigEntryType::Int32 == configEntryType || ConfigEntryType::Int64 == configEntryType)
			{
				return setIntValue(configNameAlias, configName, configEntryType, value, minValue, maxValue, comment);
			}
			
			if (ConfigEntryType::UInt8 == configEntryType || ConfigEntryType::UInt16 == configEntryType ||
				ConfigEntryType::UInt32 == configEntryType || ConfigEntryType::UInt64 == configEntryType)
			{
				return setUIntValue(configNameAlias, configName, configEntryType, value, minValue, maxValue, comment);
			}
			
			if (ConfigEntryType::Float32 == configEntryType || ConfigEntryType::Float64 == configEntryType)
			{
				return setFloatValue(configNameAlias, configName, configEntryType, value, minValue, maxValue, comment);
			}
			
			if (ConfigEntryType::Float32 == configEntryType || ConfigEntryType::Float64 == configEntryType)
			{
				return setStringValue(configNameAlias, configName, configEntryType, value, comment);
			}
			
			return false;
		}
		
    	/// @brief 获取配置中的所有配置信息
		virtual const std::map<std::string, std::shared_ptr<ConfigSettingEntry>>& getAllConfigSetting() const;
    	/// @brief 获取配置中的某配置信息
		virtual std::shared_ptr<const ConfigSettingEntry> getSettingEntryByAlias(const std::string& configNameAlias) const;
		
	protected:    	
    	/**
		 *	@brief 通过配置项模板得到配置项的具体类型
		 *
		 *  @tparam T 配置项的具体类型
		 *  @return 配置项的具体类型
		 */
		template <class T>
		ConfigEntryType getConfigEntryType(T t)
		{
			ConfigEntryType configEntryType = ConfigEntryType::Invalid;
			if (std::is_same<bool, T>::value) configEntryType = ConfigEntryType::Bool;
			else if (std::is_same<int8, T>::value) configEntryType = ConfigEntryType::Int8;
			else if (std::is_same<int16, T>::value) configEntryType = ConfigEntryType::Int16;
			else if (std::is_same<int32, T>::value) configEntryType = ConfigEntryType::Int32;
			else if (std::is_same<int64, T>::value) configEntryType = ConfigEntryType::Int64;
			else if (std::is_same<uint8, T>::value) configEntryType = ConfigEntryType::UInt8;
			else if (std::is_same<uint16, T>::value) configEntryType = ConfigEntryType::UInt16;
			else if (std::is_same<uint32, T>::value) configEntryType = ConfigEntryType::UInt32;
			else if (std::is_same<uint64, T>::value) configEntryType = ConfigEntryType::UInt64;
			else if (std::is_same<float32, T>::value) configEntryType = ConfigEntryType::Float32;
			else if (std::is_same<float64, T>::value) configEntryType = ConfigEntryType::Float64;
			else configEntryType = ConfigEntryType::String;
			
			return configEntryType;
		}
		
    	/**
		 *	@brief 设置配置值的默认值，没有范围限制
		 * 
		 *  @param configNameAlias 配置别名
		 *  @param configName 配置名
		 *  @param comment 配置说明
		 *  @param value 配置值
		 */
		virtual bool setIntValueNoLimit(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, int64 value, const std::string& comment);
		virtual bool setUIntValueNoLimit(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, uint64 value, const std::string& comment);
		virtual bool setFloatValueNoLimit(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, float64 value, const std::string& comment);
		virtual bool setStringValueNoLimit(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, const std::string& value, const std::string& comment);
		
    	/**
		 *	@brief 设置配置值的默认值，有范围限制
		 *
		 *  @param configNameAlias 配置别名
		 *  @param configName 配置名
		 *  @param comment 配置说明
		 *  @param value 配置值
		 *  @param minValue 配置最小值
		 *  @param maxValue 配置最大值
		 */
		virtual bool setIntValue(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, int64 value, int64 minValue, int64 maxValue, const std::string& comment);
		virtual bool setUIntValue(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, uint64 value, uint64 minValue, uint64 maxValue, const std::string& comment);
		virtual bool setFloatValue(const std::string& configNameAlias, const std::string& configName, ConfigEntryType configEntryType, float64 value, float64 minValue, float64 maxValue, const std::string& comment);
	
		
	protected:
    	/// @brief 某配置所有配置项信息
		std::map<std::string, std::shared_ptr<ConfigSettingEntry>> configSetting_;
	};
	
	
    /// @brief 业务层访问配制值的适配类
	class IConfigApi
	{
	public:
		virtual ~IConfigApi() {}
		
    	/**
		 *	@brief 通过配制别名得到配制值
		 *
		 *  @param configNameAlias 配置别名
		 *  @return 对应的配置值
		 */
    	virtual std::string getString(const std::string& configNameAlias) const = 0;
    	virtual int32 getInt(const std::string& configNameAlias) const = 0;
    	virtual uint32 getUInt(const std::string& configNameAlias) const = 0;
    	virtual int64 getInt64(const std::string& configNameAlias) const = 0;
    	virtual uint64 getUInt64(const std::string& configNameAlias) const = 0;
    	virtual bool getBool(const std::string& configNameAlias) const = 0;
    	virtual float64 getDouble(const std::string& configNameAlias) const = 0;
	};
	
	
	
	
	
	
	
}







