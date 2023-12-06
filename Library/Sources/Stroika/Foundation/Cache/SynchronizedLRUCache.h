/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
     *  @see LRUCache<> - for unsynchronized base version
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
        nonvirtual void clear (function<bool (typename Configuration::ArgByValueType<KEY>)> clearPredicate);

    public:
        /**
         *  \note - though this is not technically 'const' - it acts (from a thread safety point of view) as const, so its marked const
         *  @see LRUCache::Lookup ()
         */
        nonvirtual optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key) const;

    public:
        /**
         *  @see LRUCache::LookupValue ()
         */
        nonvirtual VALUE LookupValue (typename Configuration::ArgByValueType<KEY>                          key,
                                      const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& valueFetcher);

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


 namespace Factory {
        /**
         *  \note - no way to extract the KEY from the KEY_EQUALS_COMPARER, because this comparer might have templated operator(), such
         *          as String::EqualsComparer.
         * 
         *  \par Example Usage
         *      \code
         *          auto                 t0{Factory::SynchronizedLRUCache_NoHash<string, string>{}()};
         *          auto                 t1{Factory::SynchronizedLRUCache_NoHash<string, string>{}(3)};
         *          SynchronizedLRUCache t2{Factory::SynchronizedLRUCache_NoHash<String, string>{}(3, kStringCIComparer_)};
         *      \endcode
         */
        template <typename KEY, typename VALUE, typename STATS_TYPE = Statistics::StatsType_DEFAULT>
        struct SynchronizedLRUCache_NoHash {
            template <Common::IEqualsComparer<KEY> KEY_EQUALS_COMPARER = equal_to<KEY>>
            auto operator() (size_t maxCacheSize = 1, const KEY_EQUALS_COMPARER& keyComparer = {}) const
            {
                return  SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, nullptr_t, STATS_TYPE>{maxCacheSize, keyComparer};
            }
        };

        /**
         *  \par Example Usage
         *      \code
         *          auto                 t0{Factory::SynchronizedLRUCache_WithHash<string, string>{}(3, 3)};
         *          auto                 t1{Factory::SynchronizedLRUCache_WithHash<String, string>{}(3, 3, hashFunction)};
         *          SynchronizedLRUCache t2{Factory::SynchronizedLRUCache_WithHash<String, string>{}(3, equal_to<String>{}, 3)};
         *          SynchronizedLRUCache t3{Factory::SynchronizedLRUCache_WithHash<String, string, Statistics::Stats_Basic>{}(3, equal_to<String>{}, 3)}; // throw in stats object
         *          SynchronizedLRUCache t4{Factory::SynchronizedLRUCache_WithHash<String, string>{}(3, kStringCIComparer_, 3)}; // alt equality comparer
         *      \endcode
         */
        template <typename KEY, typename VALUE, typename STATS_TYPE = Statistics::StatsType_DEFAULT, typename DEFAULT_KEY_EQUALS_COMPARER = equal_to<KEY>>
        struct  SynchronizedLRUCache_WithHash {
            template <typename KEY_HASH_FUNCTION = hash<KEY>>
            auto operator() (size_t maxCacheSize, size_t hastTableSize, const KEY_HASH_FUNCTION& hashFunction = {}) const
            {
                Require (maxCacheSize >= hastTableSize);
                return  SynchronizedLRUCache<KEY, VALUE, DEFAULT_KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>{
                    maxCacheSize, DEFAULT_KEY_EQUALS_COMPARER{}, hastTableSize, hashFunction};
            }
            template <typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION = hash<KEY>>
            auto operator() (size_t maxCacheSize, const KEY_EQUALS_COMPARER& keyComparer, size_t hastTableSize,
                             const KEY_HASH_FUNCTION& hashFunction = {}) const
            {
                Require (maxCacheSize >= hastTableSize);
                return  SynchronizedLRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>{maxCacheSize, keyComparer, hastTableSize, hashFunction};
            }
        };

    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SynchronizedLRUCache.inl"

#endif /*_Stroika_Foundation_Cache_SynchronizedLRUCache_h_*/
