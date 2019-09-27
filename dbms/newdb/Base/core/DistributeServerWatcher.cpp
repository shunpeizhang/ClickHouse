#include "core/DistributeServerWatcher.h"


#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Query.h"
#include "Poco/JSON/JSONException.h"

#include "Poco/Dynamic/Struct.h"

#include "Common/logger_adapt.h"
#include "core/IServer.h"





namespace core
{

    static const int32 SERVER_NODE_MAX_STIME = 10;

	DistributeServerWatcher::DistributeServerWatcher(std::shared_ptr<coordination::ICoordinationClientApi> coordinationClientApi)
        :coordinationClientApi_(coordinationClientApi)
	{
		
	}
	
	DistributeServerWatcher::~DistributeServerWatcher()
	{
	  
	}
		
	bool DistributeServerWatcher::addServerPathToWatch(const std::string& serverPath)
	{
    	if (!reLoadServerInfo(serverPath))
    	{
        	LOG_ERROR(&g_serverInstance->logger(), "reLoadServerInfo failed! serverPath:%s", serverPath.c_str());
        	return false;
    	}
    	this->serverPath_ = serverPath;
    	
    	return coordinationClientApi_->watchChildNode(serverPath, this);
	}

    bool DistributeServerWatcher::setOwnServerInfo(const std::string& ownServerPath, const ServerInfo& serverInfo)
    {
        string info = convertServerInfoToJson(serverInfo);
        
        coordination::KeyValue keyValue;
        keyValue.first = ownServerPath;
        keyValue.second = info;
        auto ret = this->coordinationClientApi_->registerServer(ownServerPath, info, SERVER_NODE_MAX_STIME);
        if (!ret)
        {
            LOG_ERROR(&g_serverInstance->logger(), "coordinationClientApi_->setNodeValue failed!");
            return false;
        }

        if (0 == this->ownServerPath_.size())
        {
            ret = coordinationClientApi_->watchNode(ownServerPath, this);
            if (!ret)
            {
                LOG_ERROR(&g_serverInstance->logger(), "coordinationClientApi_->watchNode failed!");
                return false;
            }
        }

        this_guard guard(mutex_);

        this->ownServerPath_ = ownServerPath;
        this->ownServerinfo_ = info;

        return ret;
    }
		
	bool DistributeServerWatcher::getServerInfo(ServerUniqueID serverUniqueID, ServerInfo& serverInfo)
	{
        this_guard guard(mutex_);

        auto iter = this->watchServerInfo_.find(serverUniqueID);
        if (this->watchServerInfo_.end() == iter) return false;

        serverInfo = *iter->second;

        return true;
	}
		
	bool DistributeServerWatcher::randServerByType(ServerNodeType ServerNodeType, ServerUniqueID& serverUniqueID)
	{
        this_guard guard(mutex_);

        return false;
	}
		

	void DistributeServerWatcher::subNodeCreate(const string& currentKey, const coordination::KeyValue& subNewKeyValue)
	{
        auto info = parseServerInfo(subNewKeyValue.second);
        if (!info.get()) return;

        this_guard guard(mutex_);

        watchServerInfo_[info->serverUniqueID_] = info;
	}
	
	void DistributeServerWatcher::subNodeDELETE(const string& currentKey, const string& subKey)
	{
        this_guard guard(mutex_);

        auto iter = this->watchServerInfo_.begin();
        for (; this->watchServerInfo_.end() != iter; ++iter)
        {
            if (0 == subKey.compare(iter->second->serverPath_))
            {
                this->watchServerInfo_.erase(iter);
                break;
            }
        }
	}
	
	void DistributeServerWatcher::subValueChange(const string& currentKey, const coordination::KeyValue& subChangeKeyValue)
	{
        auto info = parseServerInfo(subChangeKeyValue.second);
        if (!info.get()) return;

        this_guard guard(mutex_);
        watchServerInfo_[info->serverUniqueID_] = info;
	}

    void DistributeServerWatcher::rewatch(const string& key)
    {
        if (0 < this->serverPath_.size())
        {
            reLoadServerInfo(serverPath_);
        }
        
        if (0 < ownServerPath_.size())
        {
            this->coordinationClientApi_->registerServer(ownServerPath_, ownServerinfo_, SERVER_NODE_MAX_STIME);
        }
    }

    std::shared_ptr<ServerInfo> DistributeServerWatcher::parseServerInfo(const std::string& serverInfo)
    {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result;
        try
        {
            result = parser.parse(serverInfo);
            if (result.type() != typeid(Poco::JSON::Object::Ptr))
            {
                LOG_ERROR(&g_serverInstance->logger(), "parser.parse failed! serverInfo:%s", serverInfo.c_str());
                return std::shared_ptr<ServerInfo>(NULL);
            }
        }
        catch (Poco::JSON::JSONException& jsone)
        {
            std::cout << jsone.message() << std::endl;
            return std::shared_ptr<ServerInfo>(NULL);
        }

        std::shared_ptr<ServerInfo> info = std::make_shared<ServerInfo>();
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        Poco::Dynamic::Var value = object->get("serverUniqueID");
        if (!value.isNumeric())
        {
            LOG_ERROR(&g_serverInstance->logger(), "serverUniqueID test.isNumeric() failed! serverInfo:%s", serverInfo.c_str());
            return std::shared_ptr<ServerInfo>(NULL);
        }
        info->serverUniqueID_ = value;
        
        value = object->get("ServerType");
        if (!value.isNumeric())
        {
            LOG_ERROR(&g_serverInstance->logger(), "ServerType test.isNumeric() failed! serverInfo:%s", serverInfo.c_str());
            return std::shared_ptr<ServerInfo>(NULL);
        }
        uint32 serverTypeValue = value;
        info->ServerType_ = (ServerNodeType)serverTypeValue;
        
        value = object->get("port");
        if (!value.isNumeric())
        {
            LOG_ERROR(&g_serverInstance->logger(), "port_ test.isNumeric() failed! serverInfo:%s", serverInfo.c_str());
            return std::shared_ptr<ServerInfo>(NULL);
        }
        info->port_ = value;

        value = object->get("ip");
        if (!value.isString())
        {
            LOG_ERROR(&g_serverInstance->logger(), "ip_ test.isString() failed! serverInfo:%s", serverInfo.c_str());
            return std::shared_ptr<ServerInfo>(NULL);
        }
        info->ip_ = value.convert<std::string>();

        value = object->get("serverPath");
        if (!value.isString())
        {
            LOG_ERROR(&g_serverInstance->logger(), "ip_ test.isString() failed! serverInfo:%s", serverInfo.c_str());
            return std::shared_ptr<ServerInfo>(NULL);
        }
        info->serverPath_ = value.convert<std::string>();
        
        return info;
    }


    bool DistributeServerWatcher::reLoadServerInfo(const string& serverPath)
    {
        std::vector<coordination::KeyValue> childNodeInfo;
        if (!coordinationClientApi_->getChildNode(serverPath, childNodeInfo))
        {
            LOG_ERROR(&g_serverInstance->logger(), "coordinationClientApi_->getChildNode failed!");
            return false;
        }

        this_guard guard(mutex_);
        watchServerInfo_.clear();
        for (int iPos = 0; childNodeInfo.size() > iPos; ++iPos)
        {
            auto info = parseServerInfo(childNodeInfo[iPos].second);
            if (!info.get())
            {
                LOG_ERROR(&g_serverInstance->logger(), "parseServerInfo failed! %s", childNodeInfo[iPos].second.c_str());
                return false;
            }

            watchServerInfo_[info->serverUniqueID_] = info;
        }

        LOG_INFO(&g_serverInstance->logger(), "reLoadServerInfo this:%p", this);
        
        return true;
    }

    string DistributeServerWatcher::convertServerInfoToJson(const ServerInfo& serverInfo)
    {
        Poco::JSON::Object jObj(false);
        jObj.set("ip", serverInfo.ip_);
        jObj.set("port", serverInfo.port_);
        jObj.set("serverPath", serverInfo.serverPath_);
        jObj.set("serverUniqueID", serverInfo.serverUniqueID_);
        jObj.set("ServerType", (uint32)serverInfo.ServerType_);

        std::stringstream ss;
        jObj.stringify(ss);

        return ss.str();
    }

    
}





