#pragma once


#include <string>
#include <vector>


#include "CoordinationClient/baseType.h"
#include "CoordinationClient/ICoordinationEventHandler.h"


using std::string;


namespace coordination
{

	class ICoordinationClientApi
	{
	public:
    	ICoordinationClientApi() {}
    	virtual ~ICoordinationClientApi() {}

		
        /**
         *	@brief 初始化
         *
         *  @param coordinationAddress 例如 "172.16.11.11:10000, 172.16.11.12:10000, 172.16.11.13:10000"
         */
		virtual bool initialize(const string& coordinationAddress) = 0;

		virtual bool start() = 0;
		virtual void stop() = 0;
		

        /// @brief 设置节点值, 支持key不存在则创建(参数指定)
		virtual bool setNodeValue(const KeyValue& keyValue, bool isCreate = false) = 0;

        /// @brief 删除节点
		virtual bool delNode(const string& key) = 0;

        /// @brief 获取节点值
		virtual bool getNodeValue(const string& key, KeyValue& keyValue) = 0;

        /// @brief 监听节点变更, 一个key只能创建一个监听
		virtual bool watchNode(const string& nodeKey, ICoordinationEventHandler *handler) = 0;

        /// @brief 取消监听节点变更
		virtual bool cancelWatchNode(const string& nodeKey) = 0;

        /// @brief 获取目录child信息
		virtual bool getChildNode(const string& nodeKey, std::vector<KeyValue>& childNodeInfo) = 0;

        /// @brief 监听目录下节点变更, 一个key只能创建一个监听
		virtual bool watchChildNode(const string& nodeKey, ICoordinationEventHandler *handler) = 0;

        /// @brief 取消监听目录下节点变更
		virtual bool cancelWatchChildNode(const string& nodeKey) = 0;


        /**
         *	@brief 注册服务
         *
         *  @param timeOutSTime 表示多长时间没有网络交互就代表服务异常(单位：秒)
         */
		virtual bool registerServer(const string& serverKey, const string& serverValue, int32 timeOutSTime) = 0;


        /**
         *	@brief 设置参与master竞选
         *
         *  @param name 是选举的标识符，用来参加竞选
         *  @param value 是竞选者赢得选举时设置的初始化公告值
         *  @param timeOutSTime 表示多长时间没有网络交互就代表服务异常(单位：秒)
         */
		virtual bool campaignLeader(const string& name, const string& value, int32 timeOutSTime) = 0;

		
        /**
         *	@brief 取消参与master竞选
         *
         *  @param name 是选举的标识符，用来参加竞选
         */
		virtual bool resignLeader(const string& name) = 0;

        /**
         *	@brief 获取master
         *
         *  @param name 是选举的标识符，用来参加竞选
         *  @param leaderInfo 返回的leader信息
         *  @param isOwner 本服务是否为leader
         */
		virtual bool leader(const string& name, KeyValue& leaderInfo) = 0;

        /// @brief 监听leader变更
		virtual bool watchLeader(const string& name, ICoordinationEventHandler *handler) = 0;

        /// @brief 取消监听leader变更
		virtual bool cancelWatchLeader(const string& name) = 0;


		
        /**
         *	@brief 在给定命令锁上获得分布式共享锁
         *
         *  @param name 是要获取的分布式共享锁的标识
         *  @param timeOutMTime 锁超时时长(单位：秒)
         *  @return CEC_Success: 表示上锁成功
		 *		false: 表示锁已被占有
		 *		其他都表示失败
         */
		virtual bool lock(const string& name, int32 timeOutSTime) = 0;

		
        /**
         *	@brief 释放对锁的持有
         *
         *  @param lockKey lock接口返回的
         */
		virtual bool unlock(const string& name) = 0;




	};







}








