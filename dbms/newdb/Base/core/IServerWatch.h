#pragma once

#include "baseType.h"



namespace core
{
    /// @brief 服务基础信息
	struct ServerInfo
	{
		std::string ip_;
		uint16 port_;
    std::string serverPath_;
		
    	/// @brief 服务唯一标识
		ServerUniqueID serverUniqueID_;
    	/// @brief 服务类型
		ServerNodeType ServerType_;
		
		ServerInfo()
		{
			port_ = 0;
			serverUniqueID_ = 0;
			ServerType_ = ServerNodeType::INVALID;
		}
	};
	
	
    /// @brief 服务发现基类
	class IServerWatch
	{
	public:
    IServerWatch(){}
		virtual ~IServerWatch() {}
		
    /**
		 *	@brief 添加需要监听的服务目录
		 *
		 *  @param serverPath 服务在协调服上的路径
		 */
		virtual bool addServerPathToWatch(const std::string& serverPath) = 0;

    /**
      *	@brief 注册服务
      *
      *  @param ownServerPath 此服务在协调服上的路径
      *  @param info 此服务相关信息
      */
    	virtual bool setOwnServerInfo(const std::string& ownServerPath, const ServerInfo& serverInfo) = 0;
		
    /**
		 *	@brief 通过服务唯一标识得到服务基础信息
		 *
		 *  @param serverUniqueID 服务唯一标识
		 *  @param serverInfo 返回的服务基础信息
		 */
		virtual bool getServerInfo(ServerUniqueID serverUniqueID, ServerInfo& serverInfo) = 0;
		
    	/**
		 *	@brief 随机得到某一类型服务实例唯一ID
		 *
		 *  @param ServerNodeType 服务类型
		 *  @param serverUniqueID 返回服务实例唯一ID
		 */
		virtual bool randServerByType(ServerNodeType ServerNodeType, ServerUniqueID& serverUniqueID) = 0;
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	}



