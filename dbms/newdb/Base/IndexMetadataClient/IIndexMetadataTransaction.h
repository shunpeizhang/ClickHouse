#pragma once

#include <string>
#include <vector>
#include <memory>


namespace index_metadata_client
{
    typedef std::pair<std::string, std::string> KeyValue;


    /// @brief 提供事务操作api（支持多语句的原子操作，mvcc机制）
    class IIndexMetadataTransaction
    {
    public:
        virtual ~IIndexMetadataTransaction() {}

        /// @brief 开启一个事务
        virtual bool beginTx() = 0;
        
        /**
         *	@brief 提交一个事务, 提交后此类可重复使用
         *  @details
         *      尝试将先前应用的集合和清除提交transaction到实际数据库所代表的数据库快照。
         *      提交可能成功也可能不成功 - 特别是，如果先前提交的冲突事务，则提交必须失败才能保留事务隔离
         *      基于某事务的操作，基于事务的隔离性，其他事务中数据不可见
         *
         */
        virtual bool commitTx() = 0;

        /// @brief 回滚一个事务, 回滚后此类可重复使用
        virtual bool rollbackTx() = 0;

        /// @brief 通过key得到value, 失败表示key不存在
        virtual bool getValue(const std::string& key, std::string& value) = 0;

        /// @brief 设置key对应的value, key存在则自动创建
        virtual bool setValue(const std::string& key, const std::string& value) = 0;

        /// @brief 删除指定key, 如果key不存在也返回成功
        virtual bool deleteValue(const std::string& key) = 0;

        /**
         *	@brief 得到一定范围的kv值，区间为[starKey, endKey) limit limitCount( >0才生效)
         *
         *  @param transaction 基于某事务的操作，基于事务的隔离性，其他事务中数据不可见
         *  @param starKey 起始key
         *  @param endKey 结束key
         *  @param limitCount 最多返回的数量，如果 0 >= limitCount 则返回结果数最大为1000
         */
        virtual bool getRangeValue(const std::string& starKey, const std::string& endKey, int limitCount, std::vector<KeyValue>& result) = 0;

        /**
         *	@brief 删除一定范围的kv值，区间为[starKey, endKey)
         *
         *  @param starKey 起始key
         *  @param endKey 结束key
         */
        virtual bool deleteRangeValue(const std::string& starKey, const std::string& endKey) = 0;
    };

    



}