#include "ShardProtcolServer.h"

#include <set>
#include <sstream>

#include "Interpreters/executeQuery.h"
#include "logger_useful.h"


using namespace ShardAccess;
using namespace std;
using namespace DB;



#define DEFAULT_DATABASE_NAME "default"
const int64_t max_read_buffer_size = 500 * 1024 * 1024;
const int32_t max_row_size = 10 * 1024;

ShardProtcolServerImpl::ShardProtcolServerImpl(IServer & server_)
    : server(server_)
    , log(Poco::Logger::get("ShardProtcolServerImpl"))
    , query_context(server.context())
{
    tableShardsManagerPtr = make_shared<TableShardsManager>("");
    if(!tableShardsManagerPtr->loadAllTableInfo())
    {
        ERROR(log, "tableShardsManagerPtr->loadAllTableInfo failed!");
        throw new Poco::Exception("tableShardsManagerPtr->loadAllTableInfo failed!");
    }
}

ShardProtcolServerImpl::~ShardProtcolServerImpl()
{

}

::grpc::Status ShardProtcolServerImpl::CreateShard(::grpc::ServerContext* context, const ::clickhouse::CreateShardArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "CreateShard:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    auto& shard_arg = sqlCommonInfo.shard_arg();

    //参数检查
    std::set<std::string> shardIDs;
    if(!checkSqlCommonInfo(sqlCommonInfo))
    {
        SetBoolResult(*response, "checkSqlCommonInfo failed! " + shard_arg.table_name());
        return ::grpc::Status::OK;
    }

    auto tableShards = tableShardsManagerPtr->getTableShardsOrCreate(shard_arg.table_name());
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);

    //执行, 需支持幂等性
    std::string errorInfo;
    for(size_t pos = 0; sqlCommonInfo.sql_size() > pos; ++pos))
    {
        //检测表是否已存在
        if(defaultDatabase.tryGetTable(sqlCommonInfo.table_shard_name(pos), this->query_context).get())
        {
            LOG_WARNING(log, "table has exist!" + sqlCommonInfo.table_shard_name(pos));
            continue;//已经创建，不再重复创建
        }

        if(!execSqlNoInputAndOutput(request->sqls(), errorInfo))
        {
            SetBoolResult(*response, "execSqlCommonInfo failed! " + errorInfo);
            return ::grpc::Status::OK;
        }
        tableShards->AddShardId(sqlCommonInfo.shard_ids(pos));
    }    

    //更新table->shards
    {
        if(!tableShards->saveToFile())
        {
            SetBoolResult(*response, "tableShards->saveToFile failed! table_name:" + shard_arg.table_name());
            return ::grpc::Status::OK;
        }
    }

    response->set_ok(true);

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::DropShard(::grpc::ServerContext* context, const ::clickhouse::DropShardArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "DropShard:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    auto& shard_arg = sqlCommonInfo.shard_arg();

    //参数检查
    std::set<std::string> shardIDs;
    if(!checkSqlCommonInfo(sqlCommonInfo))
    {
        SetBoolResult(*response, "checkSqlCommonInfo failed! " + shard_arg.table_name());
        return ::grpc::Status::OK;
    }

    auto tableShards = tableShardsManagerPtr->getTableShardsOrCreate(shard_arg.table_name());
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);

    //执行, 需支持幂等性
    std::string errorInfo;
    for(size_t pos = 0; sqlCommonInfo.sql_size() > pos; ++pos))
    {
        //检测表是否已存在
        if(!defaultDatabase.tryGetTable(sqlCommonInfo.table_shard_name(pos), this->query_context).get())
        {
            LOG_WARNING(log, "table not exist!" + sqlCommonInfo.table_shard_name(pos));
            continue;
        }

        if(!execSqlNoInputAndOutput(request->sqls(), errorInfo))
        {
            SetBoolResult(*response, "execSqlCommonInfo failed! " + errorInfo);
            return ::grpc::Status::OK;
        }
        tableShards->RemoveShardId(sqlCommonInfo.shard_ids(pos));
    }    

    //更新table->shards
    {
        if(!tableShards->saveToFile())
        {
            SetBoolResult(*response, "tableShards->saveToFile failed! table_name:" + shard_arg.table_name());
            return ::grpc::Status::OK;
        }
    }

    response->set_ok(true);

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::TruncateShard(::grpc::ServerContext* context, const ::clickhouse::TruncateShardArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "DropShard:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    auto& shard_arg = sqlCommonInfo.shard_arg();

    //参数检查
    std::set<std::string> shardIDs;
    if(!checkSqlCommonInfo(sqlCommonInfo))
    {
        SetBoolResult(*response, "checkSqlCommonInfo failed! " + shard_arg.table_name());
        return ::grpc::Status::OK;
    }

    auto tableShards = tableShardsManagerPtr->getTableShardsOrCreate(shard_arg.table_name());
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);

    //执行, 需支持幂等性
    std::string errorInfo;
    for(size_t pos = 0; sqlCommonInfo.sql_size() > pos; ++pos))
    {
        //检测表是否已存在
        if(!defaultDatabase.tryGetTable(sqlCommonInfo.table_shard_name(pos), this->query_context).get())
        {
            LOG_ERROR(log, "table not exist!" + sqlCommonInfo.table_shard_name(pos));
            errorInfo = errorInfo + "table not exist!" + sqlCommonInfo.table_shard_name(pos);
            continue;
        }

        if(!execSqlNoInputAndOutput(request->sqls(), errorInfo))
        {
            SetBoolResult(*response, "execSqlCommonInfo failed! " + errorInfo);
            errorInfo = errorInfo + "execSqlCommonInfo failed!";
            continue;
        }
    }

    SetBoolResult(*response, errorInfo);

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::DropTable(::grpc::ServerContext* context, const ::clickhouse::DropTableArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "DropTable:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    auto& shard_arg = sqlCommonInfo.shard_arg();

    auto tableShards = tableShardsManagerPtr->getTableShardsOrCreate(shard_arg.table_name());
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);

    //参数检查
    std::set<std::string> shardIDs;
    {
        if(!checkSqlCommonInfo(sqlCommonInfo))
        {
            SetBoolResult(*response, "checkSqlCommonInfo failed! " + shard_arg.table_name());
            return ::grpc::Status::OK;
        }

        if(shardIDs.size() != tableShards->ShardCount())
        {
            SetBoolResult(*response, "shardIDs.size:" + std::to_string(shardIDs.size() + 
                " != tableShards->ShardCount:" + std::to_string(tableShards->ShardCount()) ", error! ");
            return ::grpc::Status::OK;
        }
    }

    //执行, 需支持幂等性
    std::string errorInfo;
    for(size_t pos = 0; sqlCommonInfo.sql_size() > pos; ++pos))
    {
        //检测表是否已存在
        if(!defaultDatabase.tryGetTable(sqlCommonInfo.table_shard_name(pos), this->query_context).get())
        {
            LOG_WARNING(log, "table not exist!" + sqlCommonInfo.table_shard_name(pos));
            continue;
        }

        std::string error;
        if(!execSqlNoInputAndOutput(request->sqls(), error))
        {
            SetBoolResult(*response, "execSqlCommonInfo failed! " + errorInfo);
            errorInfo = errorInfo + error;
            continue;
        }
    }

    //更新table->shards
    if(0 == errorInfo.size())
    {
        if(!tableShards->removeFile())
        {
            SetBoolResult(*response, "tableShards->removeFile failed! table_name:" + shard_arg.table_name());
            return ::grpc::Status::OK;
        }
        tableShardsManagerPtr->remove(shard_arg.table_name());
    }

    SetBoolResult(*response, errorInfo);

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::ChangeTableName(::grpc::ServerContext* context, const ::clickhouse::ChangeTableNameArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "ChangeTableName:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    auto& shard_arg = sqlCommonInfo.shard_arg();

    auto tableShards = tableShardsManagerPtr->getTableShardsOrCreate(shard_arg.table_name());
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);

    //参数检查
    std::set<std::string> shardIDs;
    {
        if(!checkSqlCommonInfo(sqlCommonInfo))
        {
            SetBoolResult(*response, "checkSqlCommonInfo failed! " + shard_arg.table_name());
            return ::grpc::Status::OK;
        }

        if(shardIDs.size() != tableShards->ShardCount())
        {
            SetBoolResult(*response, "shardIDs.size:" + std::to_string(shardIDs.size() + 
                " != tableShards->ShardCount:" + std::to_string(tableShards->ShardCount()) ", error! ");
            return ::grpc::Status::OK;
        }
    }

    //执行, 需支持幂等性
    std::string errorInfo;
    for(size_t pos = 0; sqlCommonInfo.sql_size() > pos; ++pos))
    {
        //检测表是否已存在
        if(!defaultDatabase.tryGetTable(sqlCommonInfo.table_shard_name(pos), this->query_context).get())
        {
            LOG_WARNING(log, "table not exist!" + sqlCommonInfo.table_shard_name(pos));
            continue;
        }

        std::string error;
        if(!execSqlNoInputAndOutput(request->sqls(), error))
        {
            SetBoolResult(*response, "execSqlCommonInfo failed! " + errorInfo);
            errorInfo = errorInfo + error;
            continue;
        }
    }

    //更新table->shards
    if(0 == errorInfo.size())
    {
        tableShards->SetTableName(request->new_table_name());
        if(!tableShards->saveToFile())
        {
            SetBoolResult(*response, "tableShards->saveToFile failed! table_name:" + shard_arg.table_name());
            return ::grpc::Status::OK;
        }
    }

    SetBoolResult(*response, errorInfo);

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::ChangeColName(::grpc::ServerContext* context, const ::clickhouse::ChangeColNameArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "ChangeColName:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    std::string errorInfo;

    string old_col_name = sqlCommonInfo.old_col_name();
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);
    auto needSkipFun = [&defaultDatabase, this, &old_col_name](const std::string& tableShardName){
        auto storagePtr = defaultDatabase.tryGetTable(tableShardName, this->query_context);
        if(!storagePtr.get())
        {
            LOG_WARNING(log, "table not exist!" + sqlCommonInfo.table_shard_name(pos));
            return true;
        }

        StorageMetadataPtr storageMetadataPtr = storagePtr->getInMemoryMetadataPtr();
        if(!storageMetadataPtr.get())
            return true;

        return !storageMetadataPtr->columns.has(old_col_name);
    };

    response->set_ok(true);
    if(!commonDDLExec(sqlCommonInfo, errorInfo))
    {
        LOG_ERROR(log, errorInfo);
        SetBoolResult(*response, errorInfo);
    }

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::ChangeColType(::grpc::ServerContext* context, const ::clickhouse::ChangeColTypeArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "ChangeColType:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    std::string errorInfo;

    auto needSkipFun = [](const std::string& tableShardName){
        return false;
    };

    response->set_ok(true);
    if(!commonDDLExec(sqlCommonInfo, errorInfo, needSkipFun))
    {
        LOG_ERROR(log, errorInfo);
        SetBoolResult(*response, errorInfo);
    }

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::AddCol(::grpc::ServerContext* context, const ::clickhouse::AddColArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "AddCol:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    std::string errorInfo;

    string col_name = sqlCommonInfo.col_name();
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);
    auto needSkipFun = [&defaultDatabase, this, &col_name](const std::string& tableShardName){
        auto storagePtr = defaultDatabase.tryGetTable(tableShardName, this->query_context);
        if(!storagePtr.get())
        {
            LOG_WARNING(log, "table not exist!" + sqlCommonInfo.table_shard_name(pos));
            return true;
        }

        StorageMetadataPtr storageMetadataPtr = storagePtr->getInMemoryMetadataPtr();
        if(!storageMetadataPtr.get())
            return true;

        return storageMetadataPtr->columns.has(col_name);
    };

    response->set_ok(true);
    if(!commonDDLExec(sqlCommonInfo, errorInfo, needSkipFun))
    {
        LOG_ERROR(log, errorInfo);
        SetBoolResult(*response, errorInfo);
    }

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::DropCol(::grpc::ServerContext* context, const ::clickhouse::DropColArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "DropCol:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    std::string errorInfo;

    string col_name = sqlCommonInfo.col_name();
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);
    auto needSkipFun = [&defaultDatabase, this, &col_name](const std::string& tableShardName){
        auto storagePtr = defaultDatabase.tryGetTable(tableShardName, this->query_context);
        if(!storagePtr.get())
        {
            LOG_WARNING(log, "table not exist!" + sqlCommonInfo.table_shard_name(pos));
            return true;
        }

        StorageMetadataPtr storageMetadataPtr = storagePtr->getInMemoryMetadataPtr();
        if(!storageMetadataPtr.get())
            return true;

        return !storageMetadataPtr->columns.has(col_name);
    };

    response->set_ok(true);
    if(!commonDDLExec(sqlCommonInfo, errorInfo, needSkipFun))
    {
        LOG_ERROR(log, errorInfo);
        SetBoolResult(*response, errorInfo);
    }

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::ChangeIdxName(::grpc::ServerContext* context, const ::clickhouse::ChangeIdxNameArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "ChangeIdxName:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    std::string errorInfo;

    string old_idx_name = sqlCommonInfo.old_idx_name();
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);
    auto needSkipFun = [&defaultDatabase, this, &old_idx_name](const std::string& tableShardName){
        auto storagePtr = defaultDatabase.tryGetTable(tableShardName, this->query_context);
        if(!storagePtr.get())
        {
            LOG_WARNING(log, "table not exist!" + sqlCommonInfo.table_shard_name(pos));
            return true;
        }

        StorageMetadataPtr storageMetadataPtr = storagePtr->getInMemoryMetadataPtr();
        if(!storageMetadataPtr.get())
            return true;

        return !storageMetadataPtr->secondary_indices.has(old_idx_name);
    };

    response->set_ok(true);
    if(!commonDDLExec(sqlCommonInfo, errorInfo, needSkipFun))
    {
        LOG_ERROR(log, errorInfo);
        SetBoolResult(*response, errorInfo);
    }

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::AddIdx(::grpc::ServerContext* context, const ::clickhouse::AddIdxArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "AddIdx:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    std::string errorInfo;

    string idxName = sqlCommonInfo.idx_name();
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);
    auto needSkipFun = [&defaultDatabase, this, &idxName](const std::string& tableShardName){
        auto storagePtr = defaultDatabase.tryGetTable(tableShardName, this->query_context);
        if(!storagePtr.get())
        {
            LOG_WARNING(log, "table not exist!" + sqlCommonInfo.table_shard_name(pos));
            return true;
        }

        StorageMetadataPtr storageMetadataPtr = storagePtr->getInMemoryMetadataPtr();
        if(!storageMetadataPtr.get())
            return true;

        return storageMetadataPtr->secondary_indices.has(idxName);
    };

    response->set_ok(true);
    if(!commonDDLExec(sqlCommonInfo, errorInfo, needSkipFun))
    {
        LOG_ERROR(log, errorInfo);
        SetBoolResult(*response, errorInfo);
    }

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::DropIdx(::grpc::ServerContext* context, const ::clickhouse::DropIdxArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "DropIdx:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    std::string errorInfo;

    string idxName = sqlCommonInfo.idx_name();
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);
    auto needSkipFun = [&defaultDatabase, this, &idxName](const std::string& tableShardName){
        auto storagePtr = defaultDatabase.tryGetTable(tableShardName, this->query_context);
        if(!storagePtr.get())
        {
            LOG_WARNING(log, "table not exist!" + sqlCommonInfo.table_shard_name(pos));
            return true;
        }

        StorageMetadataPtr storageMetadataPtr = storagePtr->getInMemoryMetadataPtr();
        if(!storageMetadataPtr.get())
            return true;

        return !storageMetadataPtr->secondary_indices.has(idxName);
    };

    response->set_ok(true);
    if(!commonDDLExec(sqlCommonInfo, errorInfo, needSkipFun))
    {
        LOG_ERROR(log, errorInfo);
        SetBoolResult(*response, errorInfo);
    }

    return ::grpc::Status::OK;
}

::grpc::Status ShardProtcolServerImpl::GetSnapshot(::grpc::ServerContext* context, const ::clickhouse::GetSnapshotArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "GetSnapshot:" + ToString(*request));

    response->set_ok(true);

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::ReleaseSnapshot(::grpc::ServerContext* context, const ::clickhouse::ReleaseSnapshotArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "ReleaseSnapshot:" + ToString(*request));

    response->set_ok(true);

    return ::grpc::Status::OK;
}


::grpc::Status ShardProtcolServerImpl::Begin(::grpc::ServerContext* context, const ::clickhouse::BeginArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "Begin:" + ToString(*request));

    response->set_ok(true);

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::Commit(::grpc::ServerContext* context, const ::clickhouse::CommitArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "Commit:" + ToString(*request));

    response->set_ok(true);

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::Rollback(::grpc::ServerContext* context, const ::clickhouse::RollbackArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "Rollback:" + ToString(*request));

    response->set_ok(true);

    return ::grpc::Status::OK;
}

::grpc::Status ShardProtcolServerImpl::ShardInsert(::grpc::ServerContext* context, const ::clickhouse::ShardInsertArg* request, ::clickhouse::BoolResult* response) 
{
    LOG_INFO(log, "ShardInsert:" + ToString(*request));

    string sql = request->sql();
    string txn_id = request->txn_id();
    int row_cnt = request->row_cnt();
    int success_node_cnt = request->success_node_cnt();
    bool is_compress = request->is_compress();

    //执行
    string errorInfo;
    try
    {
        //创建临时context
        Context tmp_context = *this->query_context;
        tmp_context->setSetting("insert_quorum", Field(success_node_cnt));
        
        //执行
        BlockIO io = executeQuery(sql, tmp_context);
        if(!io.out)
            throw std::runtime_error(string("sql:") + sql + " executeQuery, but not out");
        
        // 创建InputStream
        Block header = io.out->getHeader();
        std::istringstream inbuf(request->arrow_record_batch());
        std::shared_ptr<ReadBuffer> in = std::make_shared<ReadBufferFromIStream>(inbuf, request->arrow_record_batch().size());
        
        std::shared_ptr<ReadBuffer> maybe_compressed_in;
        if (is_compress)
            maybe_compressed_in = std::make_shared<CompressedReadBuffer>(*in);
        else
            maybe_compressed_in = in;        

        BlockInputStreamPtr block_in = FormatFactory.instance().getInput("ArrowStream", 
            *maybe_compressed_in.get(), header, tmp_context, 
            tmp_context->getSettings().get("max_block_size").get());

        //将InputStream写入
        copyData(block_in, io.out);
    }
    catch (const Poco::Exception & e)
    {
        errorInfo = e.message();
    }
    catch (const std::exception & e)
    {
        errorInfo = e.what();
    }
    catch (...)
    {
        errorInfo = "unknow exception";
    }

    SetBoolResult(*response, errorInfo);

    return ::grpc::Status::OK;
}

::grpc::Status ShardProtcolServerImpl::ShardDelete(::grpc::ServerContext* context, const ::clickhouse::ShardDeleteArg* request, ::clickhouse::ShardDeleteResult* response) 
{
    LOG_INFO(log, "ShardDelete:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();
    int success_node_cnt = request->success_node_cnt();

    std::string errorInfo;
    commonMutateExec(sqlCommonInfo, errorInfo, success_node_cnt);
    if(0 != errorInfo.size())
    {
        SetBoolResult(*response->mutable_result(), errorInfo);
        return ::grpc::Status::OK;
    }
    response->mutable_result()->set_ok(true);

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::ShardUpdate(::grpc::ServerContext* context, const ::clickhouse::ShardUpdateArg* request, ::clickhouse::ShardUpdateResult* response) 
{
    LOG_INFO(log, "ShardUpdate:" + ToString(*request));

    const ::clickhouse::SqlCommonInfo& sqlCommonInfo = request->sqls();  
    int success_node_cnt = request->success_node_cnt();

    std::string errorInfo;
    commonMutateExec(sqlCommonInfo, errorInfo, success_node_cnt);
    if(0 != errorInfo.size())
    {
        SetBoolResult(*response->mutable_result(), errorInfo);
        return ::grpc::Status::OK;
    }
    response->mutable_result()->set_ok(true);

    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::TableShards(::grpc::ServerContext* context, const ::clickhouse::TableShardsArg* request, ::clickhouse::TableShardsResult* response) 
{
    LOG_INFO(log, "TableShards:" + ToString(*request));

    auto& table_name = request->request();

    auto tableShards = tableShardsManagerPtr->getTableShards(shard_arg.table_name());
    if(!tableShards.get() || 0 == tableShards->ShardCount())
    {
        SetBoolResult(*response->mutable_result(), "table not exist! table_name:" + table_name);
        return ::grpc::Status::OK;
    }

    response->mutable_result()->set_ok(true);
    response->set_table_name(table_name);
    for(auto iter = tableShards->begin(); tableShards->end() != iter; ++iter)    
        response->add_shard_ids(*iter);
    
    return ::grpc::Status::OK;
}
::grpc::Status ShardProtcolServerImpl::ReadShards(::grpc::ServerContext* context, const ::clickhouse::ReadShardsArg* request, ::grpc::ServerWriter< ::clickhouse::ShardReadResult>* writer) 
{
    LOG_INFO(log, "ReadShards:" + ToString(*request));

    auto& ShardReadArg = request->read_arg();
    auto& shard_arg = ShardReadArg.shard_arg();
    string snapshot_id = ShardReadArg.snapshot_id();
    string sql = ShardReadArg.sql();
    int batch_rows = ShardReadArg.batch_rows();
    bool need_compress = ShardReadArg.need_compress();  


    //执行
    string errorInfo;
    try
    {
        // 初始化Cluster
        


        //执行
        commonQueryExec(context, sql, writer, batch_rows, need_compress, read_buffer_size);
    }
    catch (const Poco::Exception & e)
    {
        errorInfo = e.message();
    }
    catch (const std::exception & e)
    {
        errorInfo = e.what();
    }
    catch (...)
    {
        errorInfo = "unknow exception";
    }

    return ::grpc::Status::OK;

}
::grpc::Status ShardProtcolServerImpl::ShardRead(::grpc::ServerContext* context, const ::clickhouse::ShardReadArg* request, ::grpc::ServerWriter< ::clickhouse::ShardReadResult>* writer) 
{
    LOG_INFO(log, "ShardRead:" + ToString(*request));

    auto& shard_arg = request->shard_arg();
    string snapshot_id = request->snapshot_id();
    string sql = request->sql();
    int batch_rows = request->batch_rows();
    bool need_compress = request->need_compress();    

    //执行
    string errorInfo;
    try
    {
        commonQueryExec(context, sql, writer, batch_rows, need_compress, read_buffer_size);
    }
    catch (const Poco::Exception & e)
    {
        errorInfo = e.message();
    }
    catch (const std::exception & e)
    {
        errorInfo = e.what();
    }
    catch (...)
    {
        errorInfo = "unknow exception";
    }

    return ::grpc::Status::OK;
}

bool ShardProtcolServerImpl::checkSqlCommonInfo(const ::clickhouse::SqlCommonInfo &info, std::set<std::string>& shardIDs)
{
    set<string> shardIDs;

    if(0 < info.shard_arg().shard_id().size())
        shardIDs.insert(info.shard_arg().shard_id());

    for(auto& shardID : info.shard_ids())    
        shardIDs.insert(shardID);
    
    if(shardIDs.size() != info.sql_size())
        return false;

    return true;
}

bool ShardProtcolServerImpl::execSqlNoInputAndOutput(const std::string &sql, std::string& errorInfo)
{    
    try
    {
        auto io = DB::executeQuery(sql, this->query_context);

        if(io.out)
            throw new std::exception("execSqlNoInputAndOutput not support output");
        else if(io.in)
            throw new std::exception("execSqlNoInputAndOutput not support input");            
    }
    catch (const Poco::Net::NetException & e)
    {
        errorInfo = e.message();
    }
    catch (const Poco::Exception & e)
    {
        errorInfo = e.message();
    }
    catch (const std::exception & e)
    {
        errorInfo = e.what();
    }
    catch (...)
    {
        errorInfo = "unknow exception";
    }    

    return 0 == errorInfo.size();
}

bool ShardProtcolServerImpl::commonDDLExec(const ::clickhouse::SqlCommonInfo &sqlCommonInfo, 
    std::string& errorInfo, std::function<bool(std::string)>& needSkipFun)
{
    auto& shard_arg = sqlCommonInfo.shard_arg();

    //参数检查
    std::set<std::string> shardIDs;
    if(!checkSqlCommonInfo(sqlCommonInfo))
    {
        errorInfo = errorInfo + "checkSqlCommonInfo failed! " + shard_arg.table_name();
        return ::grpc::Status::OK;
    }

    auto tableShards = tableShardsManagerPtr->getTableShardsOrCreate(shard_arg.table_name());
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);

    //执行, 需支持幂等性
    for(size_t pos = 0; sqlCommonInfo.sql_size() > pos; ++pos))
    {
        //检测表是否已存在
        if(needSkipFun(sqlCommonInfo.table_shard_name(pos)))        
            continue;

        if(!execSqlNoInputAndOutput(request->sqls(), errorInfo))
        {
            errorInfo = errorInfo + "execSqlCommonInfo failed!";
            continue;
        }
    }

    return 0 == errorInfo.size();
}

bool ShardProtcolServerImpl::commonMutateExec(const ::clickhouse::SqlCommonInfo &info, 
    std::string& errorInfo, int success_node_cnt)
{
    auto& shard_arg = sqlCommonInfo.shard_arg();

    //参数检查
    std::set<std::string> shardIDs;
    if(!checkSqlCommonInfo(sqlCommonInfo))
    {
        errorInfo = errorInfo + "checkSqlCommonInfo failed! " + shard_arg.table_name();
        return false;
    }

    auto tableShards = tableShardsManagerPtr->getTableShardsOrCreate(shard_arg.table_name());
    auto defaultDatabase = DatabaseCatalog::instance().getDatabase(DEFAULT_DATABASE_NAME);

    //创建临时context
    Context tmp_context = this->query_context;
    {
        int mutations_sync = 2;
        if(1 == success_node_cnt)
            mutations_sync = 1;

        MergeTreeSettings & setting = const_cast<MergeTreeSettings & >(tmp_context->getMergeTreeSettings());
        setting.set("mutations_sync", mutations_sync);
    }

    //执行
    try
    {
        for(size_t pos = 0; sqlCommonInfo.sql_size() > pos; ++pos))
        {
            //检测表是否已存在
            if(!defaultDatabase.tryGetTable(sqlCommonInfo.table_shard_name(pos), this->query_context).get())
            {
                errorInfo = errorInfo + "table not exist!" + sqlCommonInfo.table_shard_name(pos);
                return false;
            }

            string sql = sqlCommonInfo.sql(pos);
            auto io = DB::executeQuery(sql, this->query_context);
        }
    }
    catch (const Poco::Exception & e)
    {
        errorInfo = e.message();
    }
    catch (const std::exception & e)
    {
        errorInfo = e.what();
    }
    catch (...)
    {
        errorInfo = "unknow exception";
    }

    return 0 == errorInfo.size();
}

void ShardProtcolServerImpl::SetBoolResult(const rpc::BoolResult & arg, std::string& errorInfo)
{
    if(0 < errorInfo.size())
    {
        arg.set_ok(false);
        arg.set_errinfo(errorInfo);

        LOG_ERROR(log, errorInfo);
    }        
    else
        arg.set_ok(true);
}

void ShardProtcolServerImpl::commonQueryExec(::grpc::ServerContext* context, const std::string& sql, 
    ::grpc::ServerWriter< ::clickhouse::ShardReadResult>* writer,
    int batch_rows, bool need_compress, int64_t read_buffer_size)
{
    // 是否每批读取行数对应size超过max_read_buffer_size
    int64_t read_buffer_size = batch_rows * max_row_size * 2;
    if(read_buffer_size > max_read_buffer_size)
    {
        string errmsg = "batch_rows * max_row_size * 2 > max_read_buffer_size";
        throw runtime_error(errmsg);
    }

    //创建临时context
    Context tmp_context = *this->query_context;
    tmp_context->setSetting("max_block_size", Field(batch_rows));

    //执行
    BlockIO io = executeQuery(sql, tmp_context);
    if(!io.in)
        throw std::runtime_error(string("sql:") + sql + " executeQuery, but not in");

    Block block;
    std::ostringstream outbuf;
    std::shared_ptr<WriteBuffer> out = std::make_shared<DB::WriteBufferFromOStream>(outbuf, read_buffer_size);
    while(block = io.in->read())
    {
        // 转换block到ostringstream
        {
            //清理之前的写入
            outbuf.clear();
            out->internalBuffer().resize(0);
            out->buffer().resize(0);
            out->pos = out->buffer().begin();

            std::shared_ptr<WriteBuffer> maybe_compressed_out;
            if (need_compress)
                maybe_compressed_out = std::make_shared<CompressedWriteBuffer>(
                    *out, CompressionCodecFactory::instance().get("ZSTD", 1, false));
            else
                maybe_compressed_out = out;

            //转换Blcok
            std::shared_ptr<arrow::Table> arrow_table;
            Chunk chunk(block.mutateColumns(), block.rows());
            DB::CHColumnToArrowColumn::chChunkToArrowTable(arrow_table, header, chunk, header.columns(), "ArrowStream");
    
            // 结果序列化到outbuf
            arrow::Status status;
            std::shared_ptr<arrow::ipc::RecordBatchWriter> writer;
            std::shared_ptr<ArrowBufferedOutputStream> arrow_ostream = std::make_shared<ArrowBufferedOutputStream>(maybe_compressed_out);
            arrow::ipc::RecordBatchStreamWriter::Open(arrow_ostream.get(), arrow_table->schema(), &writer);
            auto status = writer->WriteTable(*arrow_table, batch_rows);
            if (!status.ok())
                throw Exception{"Error while writing a table: " + status.ToString(), ErrorCodes::UNKNOWN_EXCEPTION};
            
            auto status = writer->Close();
            if (!status.ok())
                throw Exception{"Error while closing a table: " + status.ToString(), ErrorCodes::UNKNOWN_EXCEPTION};

            maybe_compressed_out->next();
        }

        //构造结果并发送
        ::clickhouse::ShardReadResult result;
        result.mutable_result()->set_ok(true);
        result.set_row_cnt(arrow_table->num_rows());
        result.set_arrow_record_batch(outbuf.str());

        if(context->IsCancelled())
        {
            string errmsg = "context->IsCancelled() rpc write fail:" + context->peer() + " aborted the stream! snapshot_id:" + snapshot_id;
            throw runtime_error(errmsg);
        }

        if(!writer->write(result))
        {
            string errmsg = "rpc write fail:" + context->peer() + " aborted the stream";
            throw runtime_error(errmsg);
        }
    }
}
