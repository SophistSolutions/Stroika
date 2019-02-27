/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_SynchronizedLRUCache_h_
#define _Stroika_Foundation_Cache_SynchronizedLRUCache_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <shared_mutex>

#include "LRUCache.h"

/**
 *      \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   Get rid of copied/cloned CTORs (just for deduction guides) - and get the explicit deduction guides working
 *
 *      @todo   Add 'overrides' for Add/Remove methods - so those are safe too!
 *              and add more overloads (from base class) of Lookup
 *
 *      @todo   Copy CTOR should copy Stats as well (lopri)
 */

namespace Stroika::Foundation::Cache {

    /**
     *  \brief simple wrapper on LRUCache (with the same API) - but internally synchronized in a way that is
     *         more performant than using RWSyncrhonzied<LRUCache<...>>
     *
     *  @see LRUCache<> - for unsynchonized base version
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER = equal_to<KEY>, typename KEY_HASH_FUNCTION = nullptr_t, typename STATS_TYPE = Statistics::StatsType_DEFAULT>
    class SynchronizedLRUCache : private LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE> {
        using inherited = LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>;

    public:
        using StatsType = typename inherited::StatsType;

    public:
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;

    public:
        /**
         *  @see LRUCache constructor for examples
         */
        template <typename... ARGS>
        SynchronizedLRUCache (ARGS... args);
        SynchronizedLRUCache (const SynchronizedLRUCache& src);

        //tmphack - really just for deduction guide - but cannot get working below
        SynchronizedLRUCache (pair<KEY, VALUE> ignored, size_t maxCacheSize = 1, const KEY_EQUALS_COMPARER& keyEqualsComparer = {}, size_t hashTableSize = 1, KEY_HASH_FUNCTION hashFunction = KEY_HASH_FUNCTION{})
            : inherited (maxCacheSize, keyEqualsComparer, hashTableSize, hashFunction)
        {
        }
        //tmphack - really just for deduction guide - but cannot get working below
        SynchronizedLRUCache (pair<KEY, VALUE> ignored, size_t maxCacheSize, size_t hashTableSize, KEY_HASH_FUNCTION hashFunction = hash<KEY>{})
            : inherited (maxCacheSize, hashTableSize, hashFunction)
        {
        }

    public:
        // @todo support - sb easy
        nonvirtual SynchronizedLRUCache& operator= (const SynchronizedLRUCache&) = delete;

    public:
        /*
         *  Note:   We choose to not hold any lock while filling the cache (fHoldWriteLockDuringCacheFill false by default).
         *  This is because typically, filling the cache will be slow (otherwise you wouldn't be us using a cache).
         *
         *  But this has the downside, that you could try filling the cache multiple times with the same value.
         *
         *  That's perfectly safe, but not speedy.
         *
         *  Which is better depends on the likihood the caller will make multiple requests for the same non-existent value at
         *  the same time. If yes, you should set fHoldWriteLockDuringCacheFill. If no (or if you care more about being able to
         *  read the rest of the data and not having threads block needlessly for other values) set fHoldWriteLockDuringCacheFill false (default).
         */
        bool fHoldWriteLockDuringCacheFill{false};

    public:
        /**
         *  @see LRUCache::GetMaxCacheSize ()
         */
        nonvirtual size_t GetMaxCacheSize () const;

    public:
        /**
         *  @see LRUCache::SetMaxCacheSize ()
         */
        nonvirtual void SetMaxCacheSize (size_t maxCacheSize);

    public:
        /**
         *  @see LRUCache::GetStats ()
         */
        nonvirtual StatsType GetStats () const;

    public:
        /**
         *  @see LRUCache::GetKeyEqualsCompareFunction ()
         */
        nonvirtual KeyEqualsCompareFunctionType GetKeyEqualsCompareFunction () const;

    public:
        /**
         *  @see LRUCache::GetHashTableSize ()
         */
        nonvirtual size_t GetHashTableSize () const;

    public:
        /**
         *  @see LRUCache::GetKeyHashFunction ()
         */
        nonvirtual KEY_HASH_FUNCTION GetKeyHashFunction () const;

    public:
        /**
         *  @see LRUCache::clear ()
         */
        nonvirtual void clear ();
        nonvirtual void clear (typename Configuration::ArgByValueType<KEY> key);
        nonvirtual void clear (function<bool(typename Configuration::ArgByValueType<KEY>)> clearPredicate);

    public:
        /**
         *  @see LRUCache::Lookup ()
         */
        nonvirtual optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key) const;

    public:
        /**
         *  @see LRUCache::LookupValue ()
         */
        nonvirtual VALUE LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& valueFetcher);

    public:
        /**
         *  Add the given value to the cache. This is rarely directly used.
         */
        nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> value);

    public:
        /**
         *  @see LRUCache::Elements ()
         */
        nonvirtual Containers::Mapping<KEY, VALUE> Elements () const;

    private:
        mutable shared_timed_mutex fMutex_;
    };

#if 0
    // didn't work
     template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION , typename STATS_TYPE >
     SynchronizedLRUCache (pair<KEY, VALUE> ignored, size_t maxCacheSize = 1, const KEY_EQUALS_COMPARER& keyEqualsComparer = {}, size_t hashTableSize = 1, KEY_HASH_FUNCTION hashFunction = KEY_HASH_FUNCTION{}) -> SynchronizedLRUCache<KEY,VALUE,KEY_EQUALS_COMPARER,KEY_HASH_FUNCTION,Statistics::StatsType_DEFAULT>;
     template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION , typename STATS_TYPE >
    SynchronizedLRUCache (pair<KEY, VALUE> ignored, size_t maxCacheSize, size_t hashTableSize, KEY_HASH_FUNCTION hashFunction = hash<KEY>{})->  SynchronizedLRUCache<KEY,VALUE,KEY_EQUALS_COMPARER,KEY_HASH_FUNCTION,Statistics::StatsType_DEFAULT>;
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SynchronizedLRUCache.inl"

#endif /*_Stroika_Foundation_Cache_SynchronizedLRUCache_h_*/
