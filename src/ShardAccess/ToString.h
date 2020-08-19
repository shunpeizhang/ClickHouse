#ifndef TO_STRING_H
#define TO_STRING_H



#include <string>


namespace ShardAccess {




inline std::string ToString(const rpc::ShardArg & arg)
{
	return arg.table_name() + ":" + std::to_string(arg.shard_id());
}

template <typename C>
std::string ToString(const C & c, char sep, const char * bracket = "()")
{
    std::ostringstream os;
    os << bracket[0];
    for (auto & x : c)
        os << x << sep;
    auto s = os.str();
    if (s.length() == 1)
        s.append(1, bracket[1]);
    else if (s[s.length() - 1] == sep)
        s[s.length() - 1] = bracket[1];
    return s;
}

inline std::string ToString(const rpc::SqlCommonInfo & info)
{
	std::ostringstream os;
	os << ToString(info.shard_arg()) << " shard_ids:" << ToString(info.shard_ids(), ',');
	os << " sql:" << ToString(info.sql(), ',') << " table_shard_name:" << ToString(info.table_shard_name(), ',');
	
	return os.str();
}

inline std::string ToString(const rpc::LocatedShard & info)
{
	std::ostringstream os;
    os << " LocatedShard("

	os << " storageNodeId:" << info.storageNodeId();
	os << " host:" << info.host();
    os << " grpcPort:" << info.grpcPort();
    os << " rack:" << info.rack();
    os << " zone:" << info.zone();

    os << ")";
	
	return os.str();
}

inline std::string ToString(const rpc::Shard & info)
{
	std::ostringstream os;
    os << " Shard("

	os << " table_name:" << info.table_name();
	os << " shard_id:" << info.shard_id();
    os << " locs_size:" << info.locs_size();

    for(auto& loc : info.locs)
    {
        os << ToString(loc);
    }

    os << ")";
	
	return os.str();
}

inline std::string toString(const clickhouse::CreateShardArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
	
	return os.str();
}

inline std::string toString(const clickhouse::DropShardArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
	
	return os.str();
}

inline std::string toString(const clickhouse::TruncateShardArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
	
	return os.str();
}

inline std::string toString(const clickhouse::DropTableArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
	
	return os.str();
}

inline std::string toString(const clickhouse::ChangeTableNameArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());

    os << " table_name:" << arg.table_name() << " new_table_name: " << arg.new_table_name();
	
	return os.str();
}

inline std::string toString(const clickhouse::ChangeColNameArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
    os << " old_col_name:" << arg.old_col_name();
    os << " new_col_name:" << arg.new_col_name();
	
	return os.str();
}

inline std::string toString(const clickhouse::ChangeColTypeArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
	
	return os.str();
}

inline std::string toString(const clickhouse::AddColArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
    os << " col_name:" << arg.col_name();
	
	return os.str();
}

inline std::string toString(const clickhouse::DropColArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
    os << " col_name:" << arg.col_name();
	
	return os.str();
}

inline std::string toString(const clickhouse::ChangeIdxNameArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
    os << " old_idx_name:" << arg.old_idx_name();
    os << " new_idx_name:" << arg.new_idx_name();
	
	return os.str();
}

inline std::string toString(const clickhouse::AddIdxArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
    os << " idx_name:" << arg.idx_name();
	
	return os.str();
}

inline std::string toString(const clickhouse::DropIdxArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());
    os << " idx_name:" << arg.idx_name();
	
	return os.str();
}

inline std::string toString(const clickhouse::ShardDeleteArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());

    os << " txn_id:" << arg.txn_id() << " success_node_cnt:" << arg.success_node_cnt();
	
	return os.str();
}

inline std::string toString(const clickhouse::ShardUpdateArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());

    os << " txn_id:" << arg.txn_id() << " success_node_cnt:" << arg.success_node_cnt();
	
	return os.str();
}

inline std::string toString(const clickhouse::ShardInsertArg &arg)
{
    std::ostringstream os;
	os << ToString(arg.sqls());

    os << " txn_id:" << arg.txn_id() << " success_node_cnt:" << arg.success_node_cnt();
	os << " row_cnt:" << arg.row_cnt();

	return os.str();
}

inline std::string toString(const clickhouse::ShardReadShardArg &arg)
{
    std::ostringstream os;

    os << ToString(arg.shard_arg()) << " snapshot_id:" << arg.snapshot_id();
	os << " sql:" << arg.sql();

	return os.str();
}

inline std::string toString(const clickhouse::ShardReadArg &arg)
{
    std::ostringstream os;

    os << ToString(arg.read_arg());

    os << " shards_size:" << arg.shards_size();
    for(auto& shard : arg.shards())
    {
        os << ToString(shard);
    }

	return os.str();
}

inline std::string toString(const clickhouse::GetSnapshotArg &arg)
{
    std::ostringstream os;

    os << " snapshot_id:" << arg.snapshot_id();

	return os.str();
}

inline std::string toString(const clickhouse::ReleaseSnapshotArg &arg)
{
    std::ostringstream os;

    os << " snapshot_id:" << arg.snapshot_id();

	return os.str();
}

inline std::string toString(const clickhouse::BeginArg &arg)
{
    std::ostringstream os;

    os << " txn_id:" << arg.txn_id();
    os << " set_snapshot:" << arg.set_snapshot();
    os << " disable_wal:" << arg.disable_wal();

	return os.str();
}

inline std::string toString(const clickhouse::CommitArg &arg)
{
    std::ostringstream os;
    os << " txn_id:" << arg.txn_id();
	return os.str();
}

inline std::string toString(const clickhouse::RollbackArg &arg)
{
    std::ostringstream os;
    os << " txn_id:" << arg.txn_id();
	return os.str();
}

inline std::string toString(const clickhouse::TableShardsArg &arg)
{
    std::ostringstream os;
    os << " table_name:" << arg.table_name();
	return os.str();
}

} //namespace ShardAccess

#endif //TO_STRING_H
