#pragma once

#include <Common/Arena.h>
#include <Common/HashTable/HashTable.h>

namespace DB
{

/**
  * ArenaKeyHolder is a key holder for hash tables that serializes a StringRef key
  * to an Arena.
  */
struct ArenaKeyHolder
{
    StringRef key;
    Arena & pool;

    StringRef & ALWAYS_INLINE operator * ()
    {
        return key;
    }

    void ALWAYS_INLINE persistKey()
    {
        // Hash table shouldn't ask us to persist a zero key
        assert(key.size > 0);
        key.data = pool.insert(key.data, key.size);
    }

    void ALWAYS_INLINE discardKey()
    {
    }
};

/**
  * SerializedKeyHolder is a key holder for a StringRef key that is already
  * serialized to an Arena. The key must be the last allocation in this Arena,
  * and is discarded by rolling back the allocation.
  */
struct SerializedKeyHolder
{
    StringRef key;
    Arena & pool;

    StringRef & ALWAYS_INLINE operator * ()
    {
        return key;
    }

    void ALWAYS_INLINE persistKey()
    {
    }

    void ALWAYS_INLINE discardKey()
    {
        [[maybe_unused]] void * new_head = pool.rollback(key.size);
        assert(new_head == key.data);
        key.data = nullptr;
        key.size = 0;
    }
};

} /// namespace DB

