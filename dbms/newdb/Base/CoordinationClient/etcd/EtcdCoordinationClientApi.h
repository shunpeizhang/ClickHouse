#pragma once


#include <string>
#include <map>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>

#include "etcdGrpc/rpc.grpc.pb.h"
#include "etcdGrpc/v3lock.grpc.pb.h"
#include "etcdGrpc/lease.pb.h"
#include "etcdGrpc/v3election.grpc.pb.h"

#include "CoordinationClient/baseType.h"
#include "CoordinationClient/ICoordinationEventHandler.h"
#include "CoordinationClient/ICoordinationClientApi.h"




namespace coordination
{
	namespace etcd {


		class EtcdCoordinationReactor;

		class EtcdCoordinationClientApi : public ICoordinationClientApi
		{
			typedef std::recursive_mutex this_mutex;
			typedef std::lock_guard<std::recursive_mutex> this_guard;
			typedef std::shared_ptr<std::thread> thread_ptr;


		public:
			EtcdCoordinationClientApi();
			~EtcdCoordinationClientApi();


            /// @brief 见基类
			virtual bool initialize(const string& coordinationAddress);

			bool start();
			void stop();

            /// @brief 见基类
			virtual bool setNodeValue(const KeyValue& keyValue, bool isCreate = false);
            /// @brief 见基类
			virtual bool delNode(const string& key);
            /// @brief 见基类
			virtual bool getNodeValue(const string& key, KeyValue& keyValue);
            /// @brief 见基类
			virtual bool watchNode(const string& nodeKey, ICoordinationEventHandler *handler);
            /// @brief 见基类
			virtual bool cancelWatchNode(const string& nodeKey);

            /// @brief 见基类
			virtual bool getChildNode(const string& nodeKey, std::vector<KeyValue>& childNodeInfo);
            /// @brief 见基类
			virtual bool watchChildNode(const string& nodeKey, ICoordinationEventHandler *handler);
            /// @brief 见基类
			virtual bool cancelWatchChildNode(const string& nodeKey);

            /// @brief 见基类
			virtual bool registerServer(const string& serverKey, const string& serverValue, int32 timeOutSTime);

            /// @brief 见基类
			virtual bool campaignLeader(const string& name, const string& value, int32 timeOutSTime);
            /// @brief 见基类
			virtual bool resignLeader(const string& name);
            /// @brief 见基类
			virtual bool leader(const string& name, KeyValue& leaderInfo);

            /// @brief 见基类
			virtual bool watchLeader(const string& name, ICoordinationEventHandler *handler);
            /// @brief 见基类
			virtual bool cancelWatchLeader(const string& name);

            /// @brief 见基类
			virtual bool lock(const string& name, int32 timeOutSTime);
            /// @brief 见基类
			virtual bool unlock(const string& name);



		private:
            /// @brief grpc连接
            std::shared_ptr<::grpc::Channel> etcdChannel_;
            /// @brief etcd事件处理类
			std::shared_ptr<EtcdCoordinationReactor> etcdCoordinationReactor_;

            /// @brief lock需要，lock name 与 etcd上的key进行关联
            std::map<string, string> lockNameAndKeyMapper_;

            /// @brief  stubs
			std::unique_ptr<etcdserverpb::KV::Stub> kvStub_;
			std::unique_ptr<etcdserverpb::Lease::Stub> leaseStub_;
			std::unique_ptr<v3lockpb::Lock::Stub> lockStub_;
			std::unique_ptr<v3electionpb::Election::Stub> leaderStub_;            

			this_mutex mutex_;
		};


	}

}






