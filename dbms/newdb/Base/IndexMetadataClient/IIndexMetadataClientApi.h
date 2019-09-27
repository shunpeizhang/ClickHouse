#pragma once


#include <string>
#include <vector>
#include <memory>


namespace index_metadata_client
{
    typedef std::pair<std::string, std::string> KeyValue;
    class IIndexMetadataTransaction;

    /// @brief 提供访问索引服的基础接口，此类的接口都是单条原子性操作
    class IIndexMetadataClientApi
    {
    public:
        virtual ~IIndexMetadataClientApi() {}

        
        /**
         *	@brief 初始化client环境
         *
         *  @param serverAddress: 例如 "172.16.11.11:10000, 172.16.11.12:10000, 172.16.11.13:10000"
         */
        virtual bool initialize(const std::string& serverAddress) = 0;

        /// @brief 开启处理
        virtual bool start() = 0;
        /// @brief 停止处理，等待线程退出后才返回
        virtual void stop() = 0;


        /// @brief 通过key得到value, 失败表示key不存在
        virtual bool getValue(const std::string& key, std::string& value) = 0;

        /// @brief 设置key对应的value, key存在则自动创建
        virtual bool setValue(const std::string& key, const std::string& value) = 0;

        /// @brief 删除指定key, 如果key不存在也返回成功
        virtual bool deleteValue(const std::string& key) = 0;

        /**
         *	@brief 得到一定范围的kv值，区间为[starKey, endKey) limit limitCount( >0才生效)
         *
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


        /// @brief 如果提供事务操作（支持多语句的原子操作，mvcc机制），则返回对应的事务对象
        virtual std::shared_ptr<IIndexMetadataTransaction> createTransaction() = 0;

    };























}