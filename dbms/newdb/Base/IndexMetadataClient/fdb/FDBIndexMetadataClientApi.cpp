#include "FDBIndexMetadataClientApi.h"




using namespace index_metadata_client;
using namespace index_metadata_client::fdb;

static const int MAX_LIMIT_COUNT = 1000;


FDBIndexMetadataClientApi::FDBIndexMetadataClientApi()
{
    isInit_ = false;
    fdbDatabase_ = NULL;
}

FDBIndexMetadataClientApi::~FDBIndexMetadataClientApi()
{
    if (this->isInit_)
    {
        stop();
    }
}



bool FDBIndexMetadataClientApi::initialize(const std::string& serverAddress)
{
    this->serverAddress_ = serverAddress;    

    return true;
}

bool FDBIndexMetadataClientApi::start()
{
    if (!this->isInit_)
    {        
        this->fdbDatabase_ = openDatabase(this->serverAddress_);
        if (NULL == this->fdbDatabase_)
        {
            printf("openDatabase failed！ serverAddress_:%s", this->serverAddress_.c_str());
            return false;
        }        
        this->isInit_ = true;

        return true;
    }

    return false;
}


void FDBIndexMetadataClientApi::stop()
{
    if (this->isInit_)
    {
        this->isInit_ = false;

        int ret = fdb_stop_network();
        if (0 != ret)
        {
            printf("fdb_stop_network failed! %s", fdb_get_error(ret));
            return;
        }

        this->netWorkThread_->join();
    }
}


bool FDBIndexMetadataClientApi::getValue(const std::string& key, std::string& value)
{
    FDBTransaction *transaction = NULL;
    int ret = fdb_database_create_transaction(this->fdbDatabase_, &transaction);
    if (0 != ret || NULL == transaction)
    {
        printf("fdb_database_create_transaction failed! %s", fdb_get_error(ret));
        return false;
    }

    FDBFuture *future = fdb_transaction_get(transaction, (uint8_t const*)key.c_str(), key.length(), 0);
    if (NULL == future)
    {
        printf("fdb_transaction_get failed! key: %s", key.c_str());
        return false;
    }

    ret = fdb_future_block_until_ready(future);
    if (0 != ret)
    {
        printf("fdb_future_block_until_ready failed! %s", fdb_get_error(ret));
        return false;
    }

    int present;
    uint8_t const *retValue = NULL;
    int length;
    ret = fdb_future_get_value(future, &present, &retValue, &length);
    if (0 != ret || NULL == retValue)
    {
        printf("fdb_future_block_until_ready failed! %s", fdb_get_error(ret));
        return false;
    }
    fdb_future_destroy(future);
    fdb_transaction_destroy(transaction);

    value.assign((const char *)retValue, length);

    return true;
}


bool FDBIndexMetadataClientApi::setValue(const std::string& key, const std::string& value)
{
    FDBTransaction *transaction = NULL;
    int ret = fdb_database_create_transaction(this->fdbDatabase_, &transaction);
    if (0 != ret || NULL == transaction)
    {
        printf("fdb_database_create_transaction failed! %s", fdb_get_error(ret));
        return false;
    }

    fdb_transaction_set(transaction, (uint8_t const*)key.c_str(), key.length(), (uint8_t const*)value.c_str(), value.length());
    FDBFuture *future = fdb_transaction_commit(transaction);
    if (NULL == future)
    {
        printf("fdb_transaction_commit failed! key: %s", key.c_str());
        return false;
    }

    ret = fdb_future_block_until_ready(future);
    if (0 != ret)
    {
        printf("fdb_future_block_until_ready failed! %s", fdb_get_error(ret));
        return false;
    }

    ret = fdb_future_get_error(future);
    if (0 != ret)
    {
        printf("fdb_transaction_set failed! fdb_future_get_error: %s", fdb_get_error(ret));
        return false;
    }
    fdb_future_destroy(future);
    fdb_transaction_destroy(transaction);

    return true;
}


bool FDBIndexMetadataClientApi::deleteValue(const std::string& key)
{
    FDBTransaction *transaction = NULL;
    int ret = fdb_database_create_transaction(this->fdbDatabase_, &transaction);
    if (0 != ret || NULL == transaction)
    {
        printf("fdb_database_create_transaction failed! %s", fdb_get_error(ret));
        return false;
    }

    fdb_transaction_clear(transaction, (uint8_t const*)key.c_str(), key.length());
    FDBFuture *future = fdb_transaction_commit(transaction);
    if (NULL == future)
    {
        printf("fdb_transaction_commit failed! key: %s", key.c_str());
        return false;
    }

    ret = fdb_future_block_until_ready(future);
    if (0 != ret)
    {
        printf("fdb_future_block_until_ready failed! %s", fdb_get_error(ret));
        return false;
    }

    ret = fdb_future_get_error(future);
    if (0 != ret)
    {
        printf("fdb_transaction_set failed! fdb_future_get_error: %s", fdb_get_error(ret));
        return false;
    }
    fdb_future_destroy(future);
    fdb_transaction_destroy(transaction);

    return true;
}


bool FDBIndexMetadataClientApi::getRangeValue(const std::string& starKey, const std::string& endKey, int limitCount, std::vector<KeyValue>& result)
{
    if (0 >= limitCount) limitCount = MAX_LIMIT_COUNT;

    FDBTransaction *transaction = NULL;
    int ret = fdb_database_create_transaction(this->fdbDatabase_, &transaction);
    if (0 != ret || NULL == transaction)
    {
        printf("fdb_database_create_transaction failed! %s", fdb_get_error(ret));
        return false;
    }

    FDBFuture *future = fdb_transaction_get_range(transaction, (uint8_t const*)FDB_KEYSEL_FIRST_GREATER_OR_EQUAL(starKey.c_str(), starKey.length()),
        (uint8_t const*)FDB_KEYSEL_LAST_LESS_OR_EQUAL(endKey.c_str(), endKey.length()), limitCount, 0, FDB_STREAMING_MODE_SERIAL, 1, 0, 0);
    if (NULL == future)
    {
        printf("fdb_transaction_get failed! starKey: %s endKey:%s", starKey.c_str(), endKey.c_str());
        return false;
    }

    ret = fdb_future_block_until_ready(future);
    if (0 != ret)
    {
        printf("fdb_future_block_until_ready failed! %s", fdb_get_error(ret));
        return false;
    }

    int count = 0;
    const FDBKeyValue *kvs = NULL;
    int more = 0;
    while (0 == (ret = fdb_future_get_keyvalue_array(future, &kvs, &count, &more)))
    {
        for (int iPos = 0; count > iPos; ++iPos)
        {
            const FDBKeyValue *item = kvs + iPos;
            if (NULL == item)
            {
                printf("fdb_future_get_keyvalue_array failed! NULL == item");
                return false;
            }

            KeyValue keyValue;
            keyValue.first = std::string((const char *)item->key, item->key_length);
            keyValue.second = std::string((const char *)item->value, item->value_length);

            result.push_back(keyValue);
        }

        if (!more) break;
    }
    if (0 != ret || NULL == kvs)
    {
        printf("fdb_future_get_keyvalue_array failed! %s", fdb_get_error(ret));
        return false;
    }    
    fdb_future_destroy(future);
    fdb_transaction_destroy(transaction);

    return true;
}


bool FDBIndexMetadataClientApi::deleteRangeValue(const std::string& starKey, const std::string& endKey)
{
    FDBTransaction *transaction = NULL;
    int ret = fdb_database_create_transaction(this->fdbDatabase_, &transaction);
    if (0 != ret || NULL == transaction)
    {
        printf("fdb_database_create_transaction failed! %s", fdb_get_error(ret));
        return false;
    }

    fdb_transaction_clear_range(transaction, (uint8_t const*)starKey.c_str(), starKey.length(), (uint8_t const*)endKey.c_str(), endKey.length());
    FDBFuture *future = fdb_transaction_commit(transaction);
    if (NULL == future)
    {
        printf("fdb_transaction_commit failed! key: %s", starKey.c_str());
        return false;
    }

    ret = fdb_future_block_until_ready(future);
    if (0 != ret)
    {
        printf("fdb_future_block_until_ready failed! %s", fdb_get_error(ret));
        return false;
    }

    ret = fdb_future_get_error(future);
    if (0 != ret)
    {
        printf("fdb_transaction_set failed! fdb_future_get_error: %s", fdb_get_error(ret));
        return false;
    }
    fdb_future_destroy(future);
    fdb_transaction_destroy(transaction);

    return true;
}

std::shared_ptr<IIndexMetadataTransaction> FDBIndexMetadataClientApi::createTransaction()
{
    return std::shared_ptr<IIndexMetadataTransaction>(new FDBIndexMetadataTransaction(fdbDatabase_));
}


FDBDatabase* FDBIndexMetadataClientApi::openDatabase(const std::string& fdbAddress)
{
    int ret = fdb_select_api_version(FDB_API_VERSION);
    if (0 != ret)
    {
        printf("fdb_select_api_version failed! %s", fdb_get_error(ret));
        return NULL;
    }
    printf("fdb client version: %s\n", fdb_get_client_version());

    //ret = fdb_network_set_option(FDB_NET_OPTION_LOCAL_ADDRESS, (uint8_t const*)fdbAddress.c_str(), fdbAddress.length());
    //ret = fdb_network_set_option(FDB_NET_OPTION_CLUSTER_FILE, (uint8_t const*)"/data500G/work/foundationdb/172bin/fdb.cluster", strlen("/data500G/work/foundationdb/172bin/fdb.cluster"));
    if (0 != ret)
    {
        printf("fdb_network_set_option failed! %s", fdb_get_error(ret));
        return NULL;
    }

    ret = fdb_setup_network();
    if (0 != ret)
    {
        printf("fdb_setup_network failed! fdbAddress:%s %s", fdbAddress.c_str(), fdb_get_error(ret));
        return NULL;
    }
    netWorkThread_ = thread_ptr(new std::thread(&FDBIndexMetadataClientApi::runFDBNetwork, std::ref(*this)));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    FDBDatabase *db = NULL;
    ret = fdb_create_database(NULL, &db);
    if (0 != ret || NULL == db)
    {
        printf("fdb_create_database failed! %s", fdb_get_error(ret));
        return NULL;
    }

    FDBTransaction *transaction = NULL;
    ret = fdb_database_create_transaction(db, &transaction);
    if (0 != ret || NULL == transaction)
    {
        printf("fdb_database_create_transaction failed! %s", fdb_get_error(ret));
        return NULL;
    }

    FDBFuture *future = fdb_transaction_get_read_version(transaction);
    if (NULL == future)
    {
        printf("fdb_transaction_get_read_version failed!");
        return NULL;
    }
    ret = fdb_future_block_until_ready(future);
    if (0 != ret)
    {
        printf("fdb_future_block_until_ready failed! %s", fdb_get_error(ret));
        return NULL;
    }

    int64_t version;
    ret = fdb_future_get_version(future, &version);
    if (0 != ret)
    {
        printf("fdb_future_get_version failed! %s", fdb_get_error(ret));
        return NULL;
    }
    fdb_future_destroy(future);

    return db;
}

void FDBIndexMetadataClientApi::runFDBNetwork()
{
    int ret = fdb_run_network();
    if (0 != ret)
    {
        printf("fdb_run_network failed!  %s", fdb_get_error(ret));
        return;
    }
}