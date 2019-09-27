#pragma once



#include <string>
#include <thread>
#include <map>
#include <memory>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>

#include "etcdGrpc/rpc.grpc.pb.h"
#include "etcdGrpc/v3election.grpc.pb.h"

#include "Poco/Util/Util.h"
#include "Poco/Util/TimerTask.h"
#include "Poco/Event.h"
#include "Poco/Util/Timer.h"
#include "Poco/Util/TimerTaskAdapter.h"

#include "CoordinationClient/baseType.h"
#include "CoordinationClient/ICoordinationEventHandler.h"
#include "CoordinationClient/ICoordinationReactor.h"



namespace coordination{
namespace etcd {


    
    typedef std::recursive_mutex this_mutex;
    typedef std::lock_guard<std::recursive_mutex> this_guard;
    typedef std::shared_ptr<std::thread> thread_ptr;

    class EtcdCoordinationReactor : public ICoordinationReactor
    {
        struct WatchInfo
        {
            /// @brief watch的key
            string key_;
            /// @brief 是否watch key的child node
            bool isWatchChild_;

            /// @brief watch_id 是要取消的观察者的id，这样就不再有更多事件传播过来了
            int64 watch_id_;
            /// @brief 事件对应的回调类
            ICoordinationEventHandler *handler_;

            WatchInfo()
            {
                isWatchChild_ = false;
                watch_id_ = 0;
                handler_ = NULL;
            }
        };

    public:	
        /// @brief lease需要定时器不断keepalive
        struct LeaseKeepAliveTask
        {
            /// @brief 有些接口中有name标识
            string name_;
            string key_;

            /// @brief lease的唯一标识
            int64 leaseTaskID_;
            /// @brief TTL 是建议的以秒为单位的 time-to-live
            int64 TTL_;

            EtcdCoordinationReactor *etcdCoordinationReactor_;
            bool isNeedCancel_;

            void onLeaseTimer(Poco::Util::TimerTask& task);

            LeaseKeepAliveTask()
            {
                etcdCoordinationReactor_ = NULL;
                TTL_ = -1;
                leaseTaskID_ = -1;
                isNeedCancel_ = false;
            }
        };



        EtcdCoordinationReactor(std::shared_ptr<grpc::Channel> etcdChannel);
        ~EtcdCoordinationReactor();


        bool initialize();
        bool start();
        void stop();


        /// @brief 见基类
        bool watchNode(const string& nodeKey, ICoordinationEventHandler *handler);
        /// @brief 见基类
        bool cancelWatchNode(const string& nodeKey);
        /// @brief 见基类
        bool watchChildNode(const string& nodeKey, ICoordinationEventHandler *handler);
        /// @brief 见基类
        bool cancelWatchChildNode(const string& nodeKey);


        /// @brief 见基类
        bool watchLeader(const string& name, ICoordinationEventHandler *handler);
        /// @brief 见基类
        bool cancelWatchLeader(const string& name);
			

        /// @brief 见基类
        bool registerLeaseTask(const LeaseKeepAliveTask& task);
        /// @brief 见基类
        bool unregisterLeaseTask(const LeaseKeepAliveTask& task);
        /// @brief 见基类
        bool getLeaseTaskByKey(const string& key, LeaseKeepAliveTask& task);
        /// @brief 见基类
        bool getLeaseTaskByName(const string& name, LeaseKeepAliveTask& task);


    protected:
        /// @brief 节点监听事件处理
        void watchRun();
        /// @brief leader监听事件处理
        void leaderWatchRun();
        /// @brief 处理lease keep alive
        void leaseKeepaliveRun();

        /// @brief 通过key得到watch信息
        std::shared_ptr<WatchInfo> getWatchInfo(const string& key);
        /// @brief 因为网络或其他原因导致watch处理失败，需要清理watch
        void resetWatch();
        void resetLease();

    private:
        /// @brief grpc连接
        std::shared_ptr<::grpc::Channel> etcdChannel_;

        // ========watch相关信息保存=======
        /// @brief 保存所有的监听信息，grpc连接出现异常也要在此模块处理，重新将这些需监听的进行注册，
        /// 外部只要注册监听了就不用关心此模块的异常了
        std::map<string, std::shared_ptr<WatchInfo>> allWatchInfos_;
        /// @brief 正在请求监听的队列
        std::queue<std::shared_ptr<WatchInfo>> requestWatchInfos_;
        /// @brief 正在进行中的监听(两种映射关系，保存信息相同)
        std::map<string, std::shared_ptr<WatchInfo>> watchsInfo_;
        std::map<int64, std::shared_ptr<WatchInfo>> watchsInfoByID_;

        /// @brief lease定时keepalive
        std::map<std::string, std::shared_ptr<LeaseKeepAliveTask>> needLeaseKeepAlives_;
        Poco::Util::Timer leaseKeepaliveTimer_;

        /// @brief 控制线程是否运行
        bool isWatchRun_;
        bool isLeaderWatchRun_;
        bool isLeaseKeepaliveRun_;

        /// @brief threads
        this_mutex mutex_;
        thread_ptr watchThread_;
        thread_ptr leaderWatchThread_;
        thread_ptr leaseKeepaliveThread_;


        /// @brief watch stub
        grpc::ClientContext watchContext_;
        std::unique_ptr<etcdserverpb::Watch::Stub> watchStub_;
        std::unique_ptr<::grpc::ClientReaderWriter<::etcdserverpb::WatchRequest,::etcdserverpb::WatchResponse>> watchReadWriter_;

        /// @brief lead stub
        grpc::ClientContext leaderContext_;
        std::unique_ptr<v3electionpb::Election::Stub> leaderStub_;
        std::unique_ptr<::grpc::ClientReader<::v3electionpb::LeaderResponse>> leaderReader_;
        ICoordinationEventHandler *leaderHandler_;
        std::string leaderName_;

        /// @brief lease stub
        grpc::ClientContext leaseKeepaliveContext_;
        std::unique_ptr<etcdserverpb::Lease::Stub> leaseStub_;
        std::unique_ptr<::grpc::ClientReaderWriterInterface<::etcdserverpb::LeaseKeepAliveRequest,::etcdserverpb::LeaseKeepAliveResponse>> leaseReaderWriter_;


        friend LeaseKeepAliveTask;
    };


}

}

