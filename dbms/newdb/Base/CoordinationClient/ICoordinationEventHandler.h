#pragma once


#include <string>


#include "CoordinationClient/baseType.h"


using std::string;


namespace coordination
{

	class ICoordinationEventHandler
	{
	public:
		ICoordinationEventHandler() = default;
		virtual ~ICoordinationEventHandler() = default;

        /// @brief 监听的节点，其值发生变更
		virtual void currentValueChange(const KeyValue& changeKeyValue) {};

        /// @brief 监听的节点被删除
		virtual void currentNodeDELETE(const string& key) {};

        /// @brief 监听目录，目录中有新节点被创建(目前只处理目录下的一层节点)
		virtual void subNodeCreate(const string& currentKey, const KeyValue& subNewKeyValue) {};

        /// @brief 监听目录，目录中有节点被删除
		virtual void subNodeDELETE(const string& currentKey, const string& subKey) {};

        /// @brief 监听目录，目录中有节点值发生变更
		virtual void subValueChange(const string& currentKey, const KeyValue& subChangeKeyValue) {};


		
        /**
         *	@brief 监听leader变更
         *
         *  @param name 是选举的标识符，用来参加竞选
         *  @param hasLeader 如果当前没有任务leader则设置些值为true
         *  @param newLeaderInfo 变化之后的leader信息(可能为Leader发生变化, 或者leader对应的信息发生变化)
         */
		virtual void leaderChange(const string& name, bool hasLeader, const KeyValue& leaderInfo) {}

        /// @brief 网络异常，重新连接成功，将自动重新watch
        virtual void rewatch(const string& key) {}
	};

}








