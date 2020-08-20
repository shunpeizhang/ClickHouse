#ifndef SHARD_PROTCOL_SERVER
#define SHARD_PROTCOL_SERVER


#include <grpc++/grpc++.h>
#include "worker.pb.h"
#include "worker.grpc.pb.h"


namespace ShardAccess {


class ShardProtcolServerImpl : public clickhouse::Worker::Service
{
public:
    ShardProtcolServerImpl(IServer & server_);
    virtual ~ShardProtcolServerImpl();

    virtual ::grpc::Status CreateShard(::grpc::ServerContext* context, const ::clickhouse::CreateShardArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status DropShard(::grpc::ServerContext* context, const ::clickhouse::DropShardArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status TruncateShard(::grpc::ServerContext* context, const ::clickhouse::TruncateShardArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status DropTable(::grpc::ServerContext* context, const ::clickhouse::DropTableArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status ChangeTableName(::grpc::ServerContext* context, const ::clickhouse::ChangeTableNameArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status ChangeColName(::grpc::ServerContext* context, const ::clickhouse::ChangeColNameArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status ChangeColType(::grpc::ServerContext* context, const ::clickhouse::ChangeColTypeArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status AddCol(::grpc::ServerContext* context, const ::clickhouse::AddColArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status DropCol(::grpc::ServerContext* context, const ::clickhouse::DropColArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status ChangeIdxName(::grpc::ServerContext* context, const ::clickhouse::ChangeIdxNameArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status AddIdx(::grpc::ServerContext* context, const ::clickhouse::AddIdxArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status DropIdx(::grpc::ServerContext* context, const ::clickhouse::DropIdxArg* request, ::clickhouse::BoolResult* response) override;
    
    virtual ::grpc::Status GetSnapshot(::grpc::ServerContext* context, const ::clickhouse::GetSnapshotArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status ReleaseSnapshot(::grpc::ServerContext* context, const ::clickhouse::ReleaseSnapshotArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status ReadShards(::grpc::ServerContext* context, const ::clickhouse::ReadShardsArg* request, ::grpc::ServerWriter< ::clickhouse::ShardReadResult>* writer) override;
    virtual ::grpc::Status ShardRead(::grpc::ServerContext* context, const ::clickhouse::ShardReadArg* request, ::grpc::ServerWriter< ::clickhouse::ShardReadResult>* writer) override;
    
    virtual ::grpc::Status Begin(::grpc::ServerContext* context, const ::clickhouse::BeginArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status Commit(::grpc::ServerContext* context, const ::clickhouse::CommitArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status Rollback(::grpc::ServerContext* context, const ::clickhouse::RollbackArg* request, ::clickhouse::BoolResult* response) override;
    
    virtual ::grpc::Status ShardInsert(::grpc::ServerContext* context, const ::clickhouse::ShardInsertArg* request, ::clickhouse::BoolResult* response) override;
    virtual ::grpc::Status ShardDelete(::grpc::ServerContext* context, const ::clickhouse::ShardDeleteArg* request, ::clickhouse::ShardDeleteResult* response) override;
    virtual ::grpc::Status ShardUpdate(::grpc::ServerContext* context, const ::clickhouse::ShardUpdateArg* request, ::clickhouse::ShardUpdateResult* response) override;
    
    virtual ::grpc::Status TableShards(::grpc::ServerContext* context, const ::clickhouse::TableShardsArg* request, ::clickhouse::TableShardsResult* response) override;


protected:
    // 检测shard与sql是否对应
    bool checkSqlCommonInfo(const ::clickhouse::SqlCommonInfo &info, std::set<std::string>& shardIDs);
    // 执行sql, 只处理没有输入及输出的sql(DDL)
    bool execSqlNoInputAndOutput(const std::string &sql, std::string& errorInfo);

    // DDL通用实现
    bool commonDDLExec(const ::clickhouse::SqlCommonInfo &info, std::string& errorInfo, 
        std::function<bool(std::string)>& needSkipFun);
    bool commonMutateExec(const ::clickhouse::SqlCommonInfo &info, std::string& errorInfo, int success_node_cnt);
    void commonQueryExec(::grpc::ServerContext* context, const std::string& sql, 
        ::grpc::ServerWriter< ::clickhouse::ShardReadResult>* writer,
        int batch_rows, bool need_compress, int64_t read_buffer_size);

    inline void SetBoolResult(const rpc::BoolResult & arg, std::string& errorInfo);

private:
    IServer & server;
    Poco::Logger & log;

    Context query_context;

    std::shared_ptr<TableShardsManager> tableShardsManagerPtr;
};




} //namespace ShardAccess

#endif //SHARD_PROTCOL_SERVER
