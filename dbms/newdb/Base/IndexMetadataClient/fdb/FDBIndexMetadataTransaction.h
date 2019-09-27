#pragma once

#include <thread>
#include <mutex>

#ifndef FDB_API_VERSION
#define FDB_API_VERSION 610
#endif

#include "fdb_c.h"
#include "fdb_c_options.g.h"


#include "../IIndexMetadataTransaction.h"




namespace index_metadata_client {
namespace fdb {


    /// @brief 见基类
    class FDBIndexMetadataTransaction: public IIndexMetadataTransaction
    {
        typedef std::recursive_mutex this_mutex;
        typedef std::lock_guard<std::recursive_mutex> this_guard;

        /// @brief 此类的状态
        enum class State
        {
            /// @brief 新创建此类时的状态
            Invalid = 0,
            /// @brief 此状态只会在commit或rollback之后，新的事务已准备好时设置
            Ready,
            /// @brief 开启一个新事务时的状态
            Start,
        };

    public:
        FDBIndexMetadataTransaction(FDBDatabase *fdbDatabase);
        ~FDBIndexMetadataTransaction();

        /// @brief 见基类
        virtual bool beginTx();

        /// @brief 见基类
        virtual bool commitTx();

        /// @brief 见基类
        virtual bool rollbackTx();

        /// @brief 见基类
        virtual bool getValue(const std::string& key, std::string& value);

        /// @brief 见基类
        virtual bool setValue(const std::string& key, const std::string& value);

        /// @brief 见基类
        virtual bool deleteValue(const std::string& key);

        /// @brief 见基类
        virtual bool getRangeValue(const std::string& starKey, const std::string& endKey, int limitCount, std::vector<KeyValue>& result);

        /// @brief 见基类
        virtual bool deleteRangeValue(const std::string& starKey, const std::string& endKey);

    protected:
        /// @brief 销毁之后的事务，重新开启一个新事务
        void resetTransaction();


    protected:
        /// @brief fdb数据库实例
        FDBDatabase *fdbDatabase_;
        /// @brief 一个事务
        FDBTransaction *transaction_;

        /// @brief 此类的当前状态
        State state_;
        this_mutex mutex_;
    };




}
}