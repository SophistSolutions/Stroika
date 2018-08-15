/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_LRUCache_h_
#define _Stroika_Foundation_Cache_LRUCache_h_ 1

#include "../StroikaPreComp.h"

#include <optional>
#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/TypeHints.h"
#include "../Containers/Mapping.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Memory/SmallStackBuffer.h"

#include "Statistics.h"

/**
 *      \file
 *
 * TODO:
 *      @todo   Find some reasonable/simple way to get
 *              LRUCache<PHRShortcutSpec, PHRShortcutSpec, PHRShortcutSpecNoAuthCacheTraits_>   sRecentlyUsedCache (kMaxEltsInReceltlyUsedCache_);
 *              Working with ONE T argument
 *              Add(elt2cache).
 *
 *      @todo   Currently we have redundant storage - _Buf, and _First, and _Last (really just need _Buf cuz
 *              has first/last, or do our own storage managemnet with secondary array? - we do the mallocs/frees.
 *              To re-free, even though order distorted by shuffles, we can always figure out which was
 *              the original array head by which has the lowest address!
 *
 *              Also somewhat related, _Last usage is C++ unconvnetional - though maybe OK. If not more awkward
 *              in impl, consider using _fEnd? Or if it is (I think last maybe better then document clearly why
 *              its better.
 *
 */

namespace Stroika::Foundation::Cache {

    /**
     *  \brief LRUCache implements a simple least-recently-used caching strategy, with optional (key) hashing to make it faster.
     *
     *  \note   LRUCache destroys objects when they are cleared from the cache. This guarantee is
     *          relevant only in case where the objects use significant resources, or where their lifetime has
     *          externally visible (e.g. lockfiles) impact.
     *
     *  \par Example Usage
     *      \code
     *          LRUCache<string, string> tmp (3);
     *          tmp.Add ("a", "1");
     *          tmp.Add ("b", "2");
     *          tmp.Add ("c", "3");
     *          tmp.Add ("d", "4");
     *          VerifyTestResult (not tmp.Lookup ("a").has_value ());
     *          VerifyTestResult (tmp.Lookup ("b") == "2");
     *          VerifyTestResult (tmp.Lookup ("d") == "4");
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // using C++17 deduction guides, and hash table of size 10
     *          LRUCache tmp (pair<string, string>{}, 3, 10, hash<string>{});
     *          tmp.Add ("a", "1");
     *          tmp.Add ("b", "2");
     *          tmp.Add ("c", "3");
     *          tmp.Add ("d", "4");
     *          VerifyTestResult (not tmp.Lookup ("a").has_value () or *tmp.Lookup ("a") == "1"); // could be missing or found but if found same value
     *          VerifyTestResult (tmp.Lookup ("b") == "2");
     *          VerifyTestResult (tmp.Lookup ("d") == "4");
     *      \endcode
     *
     *  @see SynchronizedLRUCache<> - internally synchronized version
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#ExternallySynchronized">ExternallySynchronized</a>
     *
     */
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER = equal_to<KEY>, typename KEY_HASH_FUNCTION = nullptr_t, typename STATS_TYPE = Statistics::StatsType_DEFAULT>
    class LRUCache : private Debug::AssertExternallySynchronizedLock, private STATS_TYPE {
    public:
        using KeyType = KEY;

    public:
        using KeyEqualsCompareFunctionType = KEY_EQUALS_COMPARER;

    public:
        using StatsType = STATS_TYPE;

    public:
        /**
         *  \note the overloads taking pair<KEY,VALUE> as the first argument are just tricks to allow deduction guides to work (because
         *        you cannot specify some template parameters and then have deduction guides take effect).
         */
        LRUCache (size_t maxCacheSize = 1, const KeyEqualsCompareFunctionType& keyEqualsComparer = {}, size_t hashTableSize = 1, KEY_HASH_FUNCTION hashFunction = KEY_HASH_FUNCTION{});
        LRUCache (pair<KEY, VALUE> ignored, size_t maxCacheSize = 1, const KeyEqualsCompareFunctionType& keyEqualsComparer = {}, size_t hashTableSize = 1, KEY_HASH_FUNCTION hashFunction = KEY_HASH_FUNCTION{});
        LRUCache (size_t maxCacheSize, size_t hashTableSize, KEY_HASH_FUNCTION hashFunction = hash<KEY>{});
        LRUCache (pair<KEY, VALUE> ignored, size_t maxCacheSize, size_t hashTableSize, KEY_HASH_FUNCTION hashFunction = hash<KEY>{});
        LRUCache (const LRUCache& from);

    public:
        nonvirtual const LRUCache& operator= (const LRUCache& rhs);

    public:
        /**
         */
        nonvirtual size_t GetMaxCacheSize () const;

    public:
        /**
         */
        nonvirtual void SetMaxCacheSize (size_t maxCacheSize);

    public:
        /**
         */
        nonvirtual StatsType GetStats () const;

    public:
        /**
         *  Clear all, or just the given elements from the cache.
         */
        nonvirtual void clear ();
        nonvirtual void clear (typename Configuration::ArgByValueType<KEY> key);
        nonvirtual void clear (function<bool(typename Configuration::ArgByValueType<KEY>)> clearPredicate);

    public:
        /**
         *  The value associated with KEY may not be present, so an missing optional value is returned.
         *
         *  @see LookupValue ()
         */
        nonvirtual optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key) const;

    public:
        /**
         *  LookupValue () finds the value in the cache, and returns it, or if not present, uses the argument valueFetcher to retrieve it.
         *
         *  So LookupValue (v) is equivilent to:
         *      \code
         *          if (auto o = Lookup (k)) {
         *              return o;
         *          }
         *          else {
         *              auto v = valueFetcher (k);
         *              Add (k, v);
         *              return v;
         *          }
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          struct Details_ {
         *          };
         *          using DetailsID = int;
         *          Details_ ReadDetailsFromFile_ (DetailsID id);
         *
         *          Execution::Synchronized<LRUCache<DetailsID, Details_>>      fDetailsCache_; // caches often helpful in multithreaded situations
         *
         *          // returns the value from LRUCache, or automatically pages it in from file
         *          Details_    GetDetails (DetailsID id) {
         *              return
         *                  fDetailsCache_->LookupValue (
         *                      id,
         *                      [] (DetailsID id) -> Details_ { return ReadDetailsFromFile_ (id); }
         *                  );
         *          }
         *      \endcode
         */
        nonvirtual VALUE LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& valueFetcher);

    public:
        /**
         *  Add the given value to the cache. This is rarely directly used.
         */
        nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> value);

    public:
        /**
         *  Collect all the elements of the cache, where mapping KEY and VALUE correspond to cache KEY and VALUE.
         */
        nonvirtual Containers::Mapping<KEY, VALUE> Elements () const;

    private:
        const size_t fHashtableSize_{1};

    private:
        struct KeyValuePair_ {
            KEY   fKey;
            VALUE fValue;
        };

    private:
        // invoke selected hash function, and return number 0..fHashtableSize_
        nonvirtual size_t H_ (typename Configuration::ArgByValueType<KEY> k) const;

    private:
        const KeyEqualsCompareFunctionType fKeyEqualsComparer_;
        const KEY_HASH_FUNCTION            fHashFunction_;

        struct CacheElement_;
        struct CacheIterator_;

        nonvirtual CacheIterator_ begin_ () const;
        nonvirtual CacheIterator_ end_ () const;

        nonvirtual void ClearCache_ ();

        /*
         *  Create a new LRUCache_ element (potentially bumping some old element out of the cache). This new element
         *  will be considered most recently used. Note that this routine re-orders the cache so that the most recently looked
         *  up element is first, and because of this re-ordering, its illegal to do a Lookup while
         *  a @'LRUCache_<ELEMENT>::CacheIterator_' exists for this LRUCache_.</p>
         */
        nonvirtual optional<KeyValuePair_>* AddNew_ (typename Configuration::ArgByValueType<KeyType> item);

        /*
         * Check and see if the given element is in the cache. Return that element if its there, and nullptr otherwise.
         * Note that this routine re-orders the cache so that the most recently looked up element is first, and because
         * of this re-ordering, its illegal to do a Lookup while a @'LRUCache_<ELEMENT>::CacheIterator_' exists
         *       for this LRUCache_.
         */
        nonvirtual optional<KeyValuePair_>* LookupElement_ (typename Configuration::ArgByValueType<KeyType> item) const;

        /*
         */
        nonvirtual void ShuffleToHead_ (size_t chainIdx, CacheElement_* b);

        static constexpr size_t                                                      kPreallocatedHashtableSize_ = 5; // size where no memory allocation overhead for lrucache
        Memory::SmallStackBuffer<vector<CacheElement_>, kPreallocatedHashtableSize_> fCachedElts_BUF_{};
        Memory::SmallStackBuffer<CacheElement_*, kPreallocatedHashtableSize_>        fCachedElts_First_{};
        Memory::SmallStackBuffer<CacheElement_*, kPreallocatedHashtableSize_>        fCachedElts_Last_{};
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LRUCache.inl"

#endif /*_Stroika_Foundation_Cache_LRUCache_h_*/
