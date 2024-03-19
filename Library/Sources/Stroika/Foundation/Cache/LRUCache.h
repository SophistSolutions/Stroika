/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_LRUCache_h_
#define _Stroika_Foundation_Cache_LRUCache_h_ 1

#include "../StroikaPreComp.h"

#include <optional>
#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Configuration/TypeHints.h"
#include "../Containers/Mapping.h"
#include "../Debug/AssertExternallySynchronizedMutex.h"
#include "../Memory/InlineBuffer.h"

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
 *              PROBABLY add overload of Add() with one argument, IF VALUETYPE == KEYTYPE?
 *
 *              ADDED EXPERIMENTALLY in v2.1d6
 *
 *              But - REVIEW that usage to make sure it makes sense. Explain better here if/why it does.
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
     *  \brief LRUCache implements a simple least-recently-used caching strategy, with optional hashing (of keys) to make it faster.
     *
     *  \note Comparison with TimedCache
     *        The main difference between an LRUCache and TimedCache has to do with when an element is evicted from the Cache.
     *        With a TimedCache, its evicted only when its overly aged (now - when added to cache). With an LRUCache, its more random, and depends a
     *        bit on luck (when using hashing) and how recently an item was last accessed.
     *
     *  \par Example Usage
     *      \code
     *          LRUCache<string, string> tmp{3};    // no hashing used in cache
     *          tmp.Add ("a", "1");
     *          tmp.Add ("b", "2");
     *          tmp.Add ("c", "3");
     *          tmp.Add ("d", "4");
     *          EXPECT_TRUE (not tmp.Lookup ("a").has_value ());
     *          EXPECT_TRUE (tmp.Lookup ("b") == "2");
     *          EXPECT_TRUE (tmp.Lookup ("d") == "4");
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // using deduction guides, and hash table of size 10
     *          LRUCache tmp{pair<string, string>{}, 3, 10, hash<string>{}};
     *          tmp.Add ("a", "1");
     *          tmp.Add ("b", "2");
     *          tmp.Add ("c", "3");
     *          tmp.Add ("d", "4");
     *          EXPECT_TRUE (not tmp.Lookup ("a").has_value () or *tmp.Lookup ("a") == "1"); // could be missing or found but if found same value
     *          EXPECT_TRUE (tmp.Lookup ("b") == "2");
     *          EXPECT_TRUE (tmp.Lookup ("d") == "4");
     *      \endcode
     *
     *  \note   LRUCache destroys objects when they are cleared from the cache. This guarantee is
     *          relevant only in case where the objects use significant resources, or where their lifetime has
     *          externally visible (e.g. lockfiles) impact.
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *        o No comparison of LRUCache objects is currently supported. It might make sense, but would be of questionable use.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  @see SynchronizedLRUCache<> - internally synchronized version
     */
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER = equal_to<KEY>, typename KEY_HASH_FUNCTION = nullptr_t, typename STATS_TYPE = Statistics::StatsType_DEFAULT>
    class LRUCache {
    public:
        using KeyType = KEY;

    public:
        using KeyEqualsCompareFunctionType = KEY_EQUALS_COMPARER;

    public:
        using StatsType = STATS_TYPE;

    public:
        /**
         *  There are two basic kinds of LRUCache - with hashing, and without.
         * 
         *  If there is no KEY_HASH_FUNCTION (==nullptr) - then the GetHashTableSize () always returns 1;
         * 
         *  Note the hash function can be hash<KEY_TYPE>{}, and this is one of the constructor defaults.
         * 
         *  Note cannot move easily because this contains internal pointers (fCachedElts_First_): still declare move CTOR, but its not
         *  noexcept because its really copying...
         * 
         *  Because of a couple key limitations/constraints in C++ (as of C++20) - you cannot both do template argument deduction, and default parameters).
         *  This greatly constrains how the class works (at least constructors).
         * 
         *  So this is somewhat subject to change as the language evolves (or my understanding of tricks evolves). But for now, deduction is limited.
         * 
         *  \par Example Usage
         *      \code
         *          LRUCache<string, string> tmp{3};    // no hashing, size 3, no deduced types (just defaulted ones)
         *          LRUCache                 t0{Factory::LRUCache_WithHash<string, string>{}(3, 3)};
         *          LRUCache                 t1{Factory::LRUCache_WithHash<String, string>{}(3, 3, hashFunction)};  // types (except key/value) deducted from arguments
         *      \endcode
         * 
         *  \todo default CTOR requires no hashing, but we could make hashing work in this case with default params - just not worth it yet --LGP 2023-12-06
         */
        LRUCache ()
            requires (same_as<KEY_HASH_FUNCTION, nullptr_t> and same_as<KEY_EQUALS_COMPARER, equal_to<KEY>>);
        LRUCache (size_t maxCacheSize, const KEY_EQUALS_COMPARER& keyEqualsComparer = {})
            requires (same_as<KEY_HASH_FUNCTION, nullptr_t>);
        LRUCache (size_t maxCacheSize, const KEY_EQUALS_COMPARER& keyEqualsComparer = {}, size_t hashTableSize = 1,
                  const KEY_HASH_FUNCTION& hashFunction = KEY_HASH_FUNCTION{})
            requires (not same_as<KEY_HASH_FUNCTION, nullptr_t>);
        LRUCache (size_t maxCacheSize, size_t hashTableSize, const KEY_HASH_FUNCTION& hashFunction = KEY_HASH_FUNCTION{})
            requires (not same_as<KEY_HASH_FUNCTION, nullptr_t>);

#if qCompilerAndStdLib_MoveCTORDelete_N4285_Buggy
        LRUCache (LRUCache&& from) noexcept;
#else
        LRUCache (LRUCache&& from);
#endif
        LRUCache (const LRUCache& from)
            requires (same_as<KEY_HASH_FUNCTION, nullptr_t>);
        LRUCache (const LRUCache& from)
            requires (not same_as<KEY_HASH_FUNCTION, nullptr_t>);

    public:
        /**
         */
        nonvirtual LRUCache& operator= (LRUCache&& rhs) noexcept;
        nonvirtual LRUCache& operator= (const LRUCache& rhs);

    public:
        /**
         */
        nonvirtual size_t GetMaxCacheSize () const;

    public:
        /**
         *  Size given maybe automatically adjusted upward to be a multiple of GetHashTableSize ()
         */
        nonvirtual void SetMaxCacheSize (size_t maxCacheSize);

    public:
        /**
         */
        nonvirtual KeyEqualsCompareFunctionType GetKeyEqualsCompareFunction () const;

    public:
        /**
         */
        nonvirtual StatsType GetStats () const;

    public:
        /**
         */
        nonvirtual size_t GetHashTableSize () const;

    public:
        /**
         */
        nonvirtual KEY_HASH_FUNCTION GetKeyHashFunction () const;

    public:
        /**
         *  Clear all, or just the given elements from the cache.
         */
        nonvirtual void clear ();
        nonvirtual void clear (typename Configuration::ArgByValueType<KEY> key);
        nonvirtual void clear (function<bool (typename Configuration::ArgByValueType<KEY>)> clearPredicate);

    public:
        /**
         *  The value associated with KEY may not be present, so an missing optional value is returned.
         * 
         *  \note Unintuitively, Lookup () is non-const **intentionally** - because it updates internal data structures to track the most recently accessed item. This has implication for thread-safety!
         *
         *  @see LookupValue ()
         */
        nonvirtual optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key);

    public:
        /**
         *  LookupValue () finds the value in the cache, and returns it, or if not present, uses the argument valueFetcher to retrieve it.
         *
         *  So LookupValue (v) is equivalent to:
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
         * 
         *  \note - LookupValue () only caches successful lookups, and propagates any exceptions looking up.
         *          To negatively cache, be sure you use an optional<X> for the VALUE type, and then you can wrap
         *          the LookupValue function with try/catch and on failure, cache nullopt.
         */
        nonvirtual VALUE LookupValue (typename Configuration::ArgByValueType<KEY>                          key,
                                      const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& valueFetcher);

    public:
        /**
         *  Add the given value to the cache. This is rarely directly used. 
         *  Typically you Lookup with something like LookupValue() which implicitly does the adds.
         */
        nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> value);
        nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key)
            requires (is_same_v<KEY, VALUE>);

    public:
        /**
         *  Collect all the elements of the cache, where mapping KEY and VALUE correspond to cache KEY and VALUE.
         */
        nonvirtual Containers::Mapping<KEY, VALUE> Elements () const;

    public:
        //         *  \note the overloads taking pair<KEY, VALUE> as the first argument are just tricks to allow deduction guides to
        //                                    work (because* you cannot specify some template parameters and then have deduction guides take effect)
        //           .
        // find better way todo deduction guides so I can deprecate this
        [[deprecated ("Since Stroika v3.0d5 use Cache::Factory::LRUCache_WithHash or NoHash")]] LRUCache (
            pair<KEY, VALUE> ignored, size_t maxCacheSize = 1, const KEY_EQUALS_COMPARER& keyEqualsComparer = {}, size_t hashTableSize = 1,
            const KEY_HASH_FUNCTION& hashFunction = KEY_HASH_FUNCTION{})
            : LRUCache{maxCacheSize, keyEqualsComparer, hashTableSize, hashFunction}
        {
        }
        [[deprecated ("Since Stroika v3.0d5 use Cache::Factory::LRUCache_WithHash or NoHash")]] LRUCache (
            pair<KEY, VALUE> ignored, size_t maxCacheSize, size_t hashTableSize, const KEY_HASH_FUNCTION& hashFunction = hash<KEY>{})
            : LRUCache{maxCacheSize, hashTableSize, hashFunction}
        {
        }

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
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fAssertExternallySyncrhonized_;

    private:
        [[no_unique_address]] const KeyEqualsCompareFunctionType fKeyEqualsComparer_;
        [[no_unique_address]] const KEY_HASH_FUNCTION            fHashFunction_;
        [[no_unique_address]] STATS_TYPE                         fStats_;

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
        nonvirtual optional<KeyValuePair_>* LookupElement_ (typename Configuration::ArgByValueType<KeyType> item);

        /*
         */
        nonvirtual void ShuffleToHead_ (size_t chainIdx, CacheElement_* b);

        static constexpr size_t kPreallocatedHashtableSize_ = same_as<KEY_HASH_FUNCTION, nullptr_t> ? 1 : 5; // size where no memory allocation overhead for lrucache
        Memory::InlineBuffer<vector<CacheElement_>, kPreallocatedHashtableSize_> fCachedElts_BUF_{};
        Memory::InlineBuffer<CacheElement_*, kPreallocatedHashtableSize_>        fCachedElts_First_{};
        Memory::InlineBuffer<CacheElement_*, kPreallocatedHashtableSize_>        fCachedElts_Last_{};
    };

    namespace Factory {
        /**
         *  \note - no way to extract the KEY from the KEY_EQUALS_COMPARER, because this comparer might have templated operator(), such
         *          as String::EqualsComparer.
         * 
         *  \par Example Usage
         *      \code
         *          auto t0{Factory::LRUCache_NoHash<string, string>{}()};
         *          auto t1{Factory::LRUCache_NoHash<string, string>{}(3)};
         *          LRUCache t2{Factory::LRUCache_NoHash<String, string>{}(3, kStringCIComparer_)};
         *      \endcode
         */
        template <typename KEY, typename VALUE, typename STATS_TYPE = Statistics::StatsType_DEFAULT>
        struct LRUCache_NoHash {
            template <Common::IEqualsComparer<KEY> KEY_EQUALS_COMPARER = equal_to<KEY>>
            auto operator() (size_t maxCacheSize = 1, const KEY_EQUALS_COMPARER& keyComparer = {}) const
            {
                return LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, nullptr_t, STATS_TYPE>{maxCacheSize, keyComparer};
            }
        };

        /**
         *  \par Example Usage
         *      \code
         *          auto     t0{Factory::LRUCache_WithHash<string, string>{}(3, 3)};
         *          auto     t1{Factory::LRUCache_WithHash<String, string>{}(3, 3, hashFunction)};
         *          LRUCache t2{Factory::LRUCache_WithHash<String, string>{}(3, equal_to<String>{}, 3)};
         *          LRUCache t3{Factory::LRUCache_WithHash<String, string, Statistics::Stats_Basic>{}(3, equal_to<String>{}, 3)}; // throw in stats object
         *          LRUCache t4{Factory::LRUCache_WithHash<String, string>{}(3, kStringCIComparer_, 3)}; // alt equality comparer
         *      \endcode
         */
        template <typename KEY, typename VALUE, typename STATS_TYPE = Statistics::StatsType_DEFAULT, typename DEFAULT_KEY_EQUALS_COMPARER = equal_to<KEY>>
        struct LRUCache_WithHash {
            template <typename KEY_HASH_FUNCTION = hash<KEY>>
            auto operator() (size_t maxCacheSize, size_t hastTableSize, const KEY_HASH_FUNCTION& hashFunction = {}) const
            {
                Require (maxCacheSize >= hastTableSize);
                return LRUCache<KEY, VALUE, DEFAULT_KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>{
                    maxCacheSize, DEFAULT_KEY_EQUALS_COMPARER{}, hastTableSize, hashFunction};
            }
            template <typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION = hash<KEY>>
            auto operator() (size_t maxCacheSize, const KEY_EQUALS_COMPARER& keyComparer, size_t hastTableSize,
                             const KEY_HASH_FUNCTION& hashFunction = {}) const
            {
                Require (maxCacheSize >= hastTableSize);
                return LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>{maxCacheSize, keyComparer, hastTableSize, hashFunction};
            }
        };

    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LRUCache.inl"

#endif /*_Stroika_Foundation_Cache_LRUCache_h_*/
