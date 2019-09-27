#pragma once

#include "DataTypeDef.h"

namespace ultradb
{
/// @brief
/// 集群节点类型
enum CLUSTER_NODE_TYPE
{
  /// 集群节点类型：连接节点
  CNTYPE_CONNECT_NODE = 0x1,
  /// 集群节点类型：计算节点
  CNTYPE_COMPUTE_NODE,
  /// 集群节点类型：数据存储节点
  CNTYPE_DATA_NODE,
  /// 集群节点类型：索引节点
  CNTYPE_INDEX_NODE,
  /// 集群节点类型：协调服务节点
  CNTYPE_COORDINATE_NODE,
  /// 集群节点类型：未知类型
  CNTYPE_UNKNOWN = 0x99
};

/// @brief
/// 权限类型
enum ISOLATE_TYPE
{
  /// 仅仅分组
  ISOT_ONLY_GROUP = 0,
  /// 强制物理隔离
  ISOT_POSITIVE_ISOLATION
};

/// @brief
/// 操作类型
enum OPERATE_TYPE
{
  /// 查询数据权限
  OPTYPE_QUERY_DATA = 0,
  /// 插入数据权限
  OPTYPE_INSERT_DATA,
  /// 更新数据操作
  OPTYPE_UPDATE_DATA,
  /// 删除数据操作
  OPTYPE_DELETE_DATA
};

/// @brief
/// 类型值
enum class TypeIndex
{
  Nothing = 0,
  UInt8,
  UInt16,
  UInt32,
  UInt64,
  UInt128,
  Int8,
  Int16,
  Int32,
  Int64,
  Int128,
  Float32,
  Float64,
  Date,
  DateTime,
  Time,
  TimeStamp,
  String,
  FixedString,
  Char,
  Json,
  Enum8,
  Enum16,
  Decimal32,
  Decimal64,
  Decimal128,
  UUID,
  Array,
  Tuple,
  Set,
  Interval,
  Nullable,
  Function,
  AggregateFunction,
  LowCardinality,
};

/// @brief
/// 租户信息表
struct Tenantries
{
  /// 租户编号
  UdbUUID tenanty_id;
  /// 租户名称
  UdbString tenanty_name;
  /// 描述信息
  UdbString description;
};

/// @brief
/// 节点组
struct Groups
{
  /// 组编号，仅限内部使用
  UdbUUID group_id;
  /// 组名称
  UdbString group_name;
  /// 租户编号，可为空
  UdbUUID tenanty_id;
  /// 组描述
  UdbString description;
};

/// @brief
/// 用户信息表
struct Users
{
  /// 用户编号，仅限内部使用
  UdbUUID user_id;
  /// 用户名称
  UdbString user_name;
  /// 租户编号，可为空
  UdbUUID tenanty_id;
  /// 用户描述
  UdbString description;
  /// 登录名称
  UdbString login_name;
  /// 用户密码，仅限内部使用
  UdbString password;
  /// 创建时间，仅限内部使用
  UdbDateTime create_time;
  /// 修改时间，仅限内部使用
  UdbDateTime update_time;
  /// 最近访问时间，仅限内部使用
  UdbDateTime last_login_time;
  /// 状态，仅限内部使用
  UdbUInt16 status;
};

/// @brief
/// 用户组
struct UserGroup
{
  /// 用户编号
  UdbUUID user_id;
  /// 组编号
  UdbUUID group_id;
};

/// @brief
/// 机房信息表
struct MachineRoom
{
  /// 机房编号
  UdbUUID room_id;
  /// 机房名称
  UdbString room_name;
  /// 扩展位置
  UdbString location;
  /// 机房描述
  UdbString description;
};

/// @brief
/// 机柜信息表
struct MachineCabient
{
  /// 机柜编号
  UdbUUID cabient_id;
  /// 机柜名称
  UdbString cabient_name;
  /// 机房编号
  UdbUUID room_id;
  /// 行名称
  UdbString row_name;
  /// 列名称
  UdbString column_name;
};

/// @brief
/// 服务器信息表
struct MachineInfo
{
  /// 服务器编号
  UdbUUID machine_id;
  /// 服务器名称
  UdbString machine_name;
  /// 机柜编号
  UdbUUID cabient_id;
  /// 所在层名称
  UdbString level_name;
  /// IPV4地址
  UdbUInt32 ipaddr;
};

/// @brief
/// 集群节点
struct ClusterNodes
{
  /// 节点编号，仅限内部使用
  UdbUUID node_id;
  /// IPV4地址
  UdbUInt32 ipaddr;
  /// IPV4访问端口
  UdbUInt16 port;
  /// 组编号
  UdbUUID group_id;
  /// 节点类型
  CLUSTER_NODE_TYPE node_type;
  /// 节点元数据路径
  UdbString metadata_path;
};

/// @brief
/// 集群组
struct ClusterGroup
{
  /// 节点编号
  UdbUUID node_id;
  /// 组编号
  UdbUUID group_id;
};

/// @brief
/// ultradb 数据库
struct UdbDatabase
{
  /// 库编号
  UdbUUID db_id;
  /// 库名称
  UdbString name;
  /// 引擎名称
  UdbUInt16 engine_type;
  /// 创建时间，仅限内部使用
  UdbDateTime create_time;
  /// 修改时间，仅限内部使用
  UdbDateTime update_time;
};

/// @brief
/// 集群数据库
struct ClusterDatabase
{
  /// 节点编号
  UdbUUID node_id;
  /// 数据库编号
  UdbUUID db_id;
};

/// @brief
/// 数据库组关联表
struct UdbDatabaseGroup
{
  /// 数据库编号
  UdbUUID db_id;
  /// 组编号
  UdbUUID group_id;
};

/// @brief
/// ultradb 数据表
struct UdbDataTable
{
  /// 表编号
  UdbUUID table_id;
  /// 数据库编号
  UdbUUID db_id;
  /// 表名称
  UdbString name;
  /// 表引擎名称
  UdbString engine_name;
  /// 建表语句
  UdbString create_sql;
  /// 备注
  UdbString comment;
  /// 是否临时表
  bool is_temporary;
  /// 创建时间，仅限内部使用
  UdbDateTime create_time;
  /// 修改时间，仅限内部使用
  UdbDateTime update_time;
};

/// @brief
/// 数据表组关联表
struct UdbDataTableGroup
{
  /// 数据表编号
  UdbUUID table_id;
  /// 组编号
  UdbUUID group_id;
  /// 操作类型
  OPERATE_TYPE op_type;
};

/// @brief
/// 集群表关联表
struct ClusterDataTable
{
  /// 节点编号
  UdbUUID node_id;
  /// 表编号
  UdbUUID table_id;
};

/// @brief
/// UltraDB 数据列
struct UdbColumns
{
  /// 列编号
  UdbUUID column_id;
  /// 列名称
  UdbString column_name;
  /// 表编号
  UdbUUID table_id;
  /// 类型
  TypeIndex type;
  /// 长度
  UdbUInt16 length;
  /// 该列的精度级别
  UdbUInt16 prec;
  /// 该列的小数位数
  UdbUInt16 scale;
  /// 是否可为空
  UdbUInt16 isnullable;
  /// 默认值
  UdbString default_value;
  /// 列描述
  UdbString comment;
};

/// @brief
/// 黑名单
struct UdbBlacklist
{
  /// 黑名单编号
  UdbUUID blacklist_id;
  /// IPV4地址开始网段
  UdbUInt32 start_ipaddr;
  /// IPV4地址截止网段
  UdbUInt32 end_ipaddr;
  /// 启用状态
  UdbUInt16 enabled_state;
};

/// @brief
/// 白名单
struct UdbWhitelist
{
  /// 白名单编号
  UdbUUID whitelist_id;
  /// IPV4地址开始网段
  UdbUInt32 start_ipaddr;
  /// IPV4地址截止网段
  UdbUInt32 end_ipaddr;
  /// 载止日期
  UdbDate end_date;
  /// 启用状态
  UdbUInt16 enabled_state;
};

} // namespace ultradb