#pragma once



#include "core/baseType.h"
#include "core/IServerWatch.h"
#include "CoordinationClient/ICoordinationEventHandler.h"
#include "CoordinationClient/ICoordinationClientApi.h"

namespace core
{
	
	
	/// @brief 见基类
	class DistributeServerWatcher : public IServerWatch, public coordination::ICoordinationEventHandler
	{
        typedef std::recursive_mutex this_mutex;
        typedef std::lock_guard<std::recursive_mutex> this_guard;

	  public:
		  DistributeServerWatcher(std::shared_ptr<coordination::ICoordinationClientApi> coordinationClientApi);
		  virtual ~DistributeServerWatcher();
		
    	  /// @brief 见基类
		  virtual bool addServerPathToWatch(const std::string& serverPath);

      /// @brief 见基类
      virtual bool setOwnServerInfo(const std::string& ownServerPath, const ServerInfo& serverInfo);
		
    	  /// @brief 见基类
		  virtual bool getServerInfo(ServerUniqueID serverUniqueID, ServerInfo& serverInfo);
		
    	  /// @brief 见基类
		  virtual bool randServerByType(ServerNodeType ServerNodeType, ServerUniqueID& serverUniqueID);
		
	  protected:
    	  /// @brief 见基类
		  void subNodeCreate(const string& currentKey, const coordination::KeyValue& subNewKeyValue);
    	  /// @brief 见基类
		  void subNodeDELETE(const string& currentKey, const string& subKey);
    	  /// @brief 见基类
		  void subValueChange(const string& currentKey, const coordination::KeyValue& subChangeKeyValue);

      /// @brief 见基类
      void rewatch(const string& key);
		
      /// @brief 解析服务信息
      std::shared_ptr<ServerInfo> parseServerInfo(const std::string& serverInfo);

    	/// 重新获取服务信息
    	bool reLoadServerInfo(const string& serverPath);

    	/// 将ServerInfo转换为json
    	string convertServerInfoToJson(const ServerInfo& serverInfo);

    	
    protected:
    	std::string serverPath_;
    	
    	std::string ownServerPath_;
    	std::string ownServerinfo_;

    	std::map<ServerUniqueID, std::shared_ptr<ServerInfo>> watchServerInfo_;
    	std::shared_ptr<coordination::ICoordinationClientApi> coordinationClientApi_;

    	this_mutex mutex_;
	};
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	



	}
