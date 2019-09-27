#include "FDBIndexMetadataTransaction.h"





using namespace index_metadata_client;
using namespace index_metadata_client::fdb;


static const int MAX_LIMIT_COUNT = 1000;


FDBIndexMetadataTransaction::FDBIndexMetadataTransaction(FDBDatabase *fdbDatabase): fdbDatabase_(fdbDatabase)
{
    state_ = State::Invalid;
    transaction_ = NULL;
}

FDBIndexMetadataTransaction::~FDBIndexMetadataTransaction()
{
    this_guard guard(mutex_);
    if (State::Start == state_) rollbackTx();
    if (NULL != transaction_)
    {
        fdb_transaction_destroy(transaction_);
        transaction_ = NULL;
    }
}


bool FDBIndexMetadataTransaction::beginTx()
{
    if (State::Invalid != state_ && State::Ready != state_) return false;

    this_guard guard(mutex_);
    if (NULL == transaction_)
    {
        int ret = fdb_database_create_transaction(this->fdbDatabase_, &transaction_);
        if (0 != ret || NULL == transaction_)
        {
            printf("fdb_database_create_transaction failed! %s", fdb_get_error(ret));
            return false;
        }
    }
    state_ = State::Start;

    return true;
}


bool FDBIndexMetadataTransaction::commitTx()
{
    if (State::Start != state_) return false;

    this_guard guard(mutex_);
    FDBFuture *future = fdb_transaction_commit(transaction_);
    if (NULL == future)
    {
        printf("fdb_transaction_commit failed!");
        resetTransaction();
        return false;
    }

    int ret = fdb_future_block_until_ready(future);
    if (0 != ret)
    {
        printf("fdb_future_block_until_ready failed! %s", fdb_get_error(ret));
        resetTransaction();
        return false;
    }

    ret = fdb_future_get_error(future);
    if (0 != ret)
    {
        printf("fdb_transaction_set failed! fdb_future_get_error: %s", fdb_get_error(ret));
        resetTransaction();
        return false;
    }

    resetTransaction();

    return true;
}


bool FDBIndexMetadataTransaction::rollbackTx()
{
    if (State::Start != state_) return false;

    this_guard guard(mutex_);
    fdb_transaction_cancel(transaction_);
    resetTransaction();

    return true;
}


bool FDBIndexMetadataTransaction::getValue(const std::string& key, std::string& value)
{
    if (State::Start != state_) return false;

    this_guard guard(mutex_);
    FDBFuture *future = fdb_transaction_get(transaction_, (uint8_t const*)key.c_str(), key.length(), 0);
    if (NULL == future)
    {
        printf("fdb_transaction_get failed! key: %s", key.c_str());
        return false;
    }

    int ret = fdb_future_block_until_ready(future);
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

    value.assign((const char *)retValue, length);

    return true;
}


bool FDBIndexMetadataTransaction::setValue(const std::string& key, const std::string& value)
{
    if (State::Start != state_) return false;

    this_guard guard(mutex_);
    fdb_transaction_set(transaction_, (uint8_t const*)key.c_str(), key.length(), (uint8_t const*)value.c_str(), value.length());    

    return true;
}


bool FDBIndexMetadataTransaction::deleteValue(const std::string& key)
{
    if (State::Start != state_) return false;

    this_guard guard(mutex_);
    fdb_transaction_clear(transaction_, (uint8_t const*)key.c_str(), key.length());

    return true;
}


bool FDBIndexMetadataTransaction::getRangeValue(const std::string& starKey, const std::string& endKey, int limitCount, std::vector<KeyValue>& result)
{
    if (State::Start != state_) return false;
    this_guard guard(mutex_);

    if (0 >= limitCount) limitCount = MAX_LIMIT_COUNT;

    FDBFuture *future = fdb_transaction_get_range(transaction_, (uint8_t const*)FDB_KEYSEL_FIRST_GREATER_OR_EQUAL(starKey.c_str(), starKey.length()),
        (uint8_t const*)FDB_KEYSEL_LAST_LESS_OR_EQUAL(endKey.c_str(), endKey.length()), limitCount, 0, FDB_STREAMING_MODE_SERIAL, 1, 0, 0);
    if (NULL == future)
    {
        printf("fdb_transaction_get failed! starKey: %s endKey:%s", starKey.c_str(), endKey.c_str());
        return false;
    }

    int ret = fdb_future_block_until_ready(future);
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

    return true;
}

bool FDBIndexMetadataTransaction::deleteRangeValue(const std::string& starKey, const std::string& endKey)
{
    if (State::Start != state_) return false;
    this_guard guard(mutex_);

    fdb_transaction_clear_range(transaction_, (uint8_t const*)starKey.c_str(), starKey.length(), (uint8_t const*)endKey.c_str(), endKey.length());

    return true;
}

void FDBIndexMetadataTransaction::resetTransaction()
{
    if (NULL != transaction_)
    {
        fdb_transaction_reset(transaction_);
        state_ = State::Ready;
    }    
}