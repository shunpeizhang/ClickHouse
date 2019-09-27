#pragma once

#include <thread>
#include <mutex>


#include "../IIndexMetadataClientApi.h"
#include "FDBIndexMetadataTransaction.h"


namespace index_metadata_client {
namespace fdb {


    /// @brief 见基类
    class FDBIndexMetadataClientApi : public IIndexMetadataClientApi
    {
        typedef std::recursive_mutex this_mutex;
        typedef std::lock_guard<std::recursive_mutex> this_guard;
        typedef std::shared_ptr<std::thread> thread_ptr;

    public:
        FDBIndexMetadataClientApi();
        ~FDBIndexMetadataClientApi();


        /// @brief 见基类
        virtual bool initialize(const std::string& serverAddress);

        /// @brief 见基类
        bool start();
        /// @brief 见基类
        void stop();


        /// @brief 见基类
        bool getValue(const std::string& key, std::string& value);

        /// @brief 见基类
        bool setValue(const std::string& key, const std::string& value);

        /// @brief 见基类
        bool deleteValue(const std::string& key);

        /// @brief 见基类
        bool getRangeValue(const std::string& starKey, const std::string& endKey, int limitCount, std::vector<KeyValue>& result);

        /// @brief 见基类
        virtual bool deleteRangeValue(const std::string& starKey, const std::string& endKey);

        /// @brief 见基类
        std::shared_ptr<IIndexMetadataTransaction> createTransaction();


    protected:
        FDBDatabase* openDatabase(const std::string& fdbAddress);
        void runFDBNetwork();

    protected:
        FDBDatabase *fdbDatabase_;
        std::string serverAddress_;

        this_mutex mutex_;
        thread_ptr netWorkThread_;

        bool isInit_;
    };





















}

}