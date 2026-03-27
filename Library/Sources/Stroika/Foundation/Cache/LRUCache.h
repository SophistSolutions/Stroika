/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_LRUCache_h_
#define _Stroika_Foundation_Cache_LRUCache_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>
#include <vector>

#include "Stroika/Foundation/Cache/Common.h"
#include "Stroika/Foundation/Cache/Statistics.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/TypeHints.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Memory/InlineBuffer.h"

/**
 *      \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
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
 *      @todo   Support same_as<void> for KEY and VALUE (and IKeyedCache) as I did with TimedCache
 *              This probably replaces the BAD logic about having KEY and VALUE being the same type!!!
 */

namespace Stroika::Foundation::Cache {

    /**
     * LRUCacheSupport mostly for defining TRAITS object that configures the cache behavior.
     */
    namespace LRUCacheSupport {

        using Execution::InternallySynchronized;

        /**
         * @brief Check if argument TRAITS is a valid TRAITS object for TimedCache<>
         * 
         *  \note   ONE of (but not both) - kTrackFreshness or kTrackExpiration
         *  \note   kTrackExpiration not compatible with kAutomaticallyMarkDataAsRefreshedEachTimeAccessed
         *  \note   valid meaningful TRAITS::kDefaultMaxAge or TRAITS::kPerCacheMaxAge
         *  \note   KeyEqualsCompareFunctionType must always be a valid key comparer (unless/until we support KEY=void)
         *  \note   KeyHashFunctionType must be a valid hash function on KEY or nullptr_t, or KEY=void (NOT YET CHECKED)
         */
        template <typename TRAITS, typename KEY, typename VALUE>
        concept ITraits = IKey<KEY> and IValue<VALUE> and
                          requires (TRAITS) {
                              typename TRAITS::KeyType;
                              typename TRAITS::ValueType;
                              typename TRAITS::StatsType;
                              typename TRAITS::KeyEqualsCompareFunctionType;
                              { TRAITS::kInternallySynchronized } -> convertible_to<InternallySynchronized>;
                          } and same_as<typename TRAITS::KeyType, KEY> and same_as<typename TRAITS::ValueType, VALUE> and
                          Common::IEqualsComparer<typename TRAITS::KeyEqualsCompareFunctionType, typename TRAITS::KeyType> and
                          Cache::Statistics::IStatsType<typename TRAITS::StatsType>;

        /**
         * The DefaultTraits<> is a simple default traits implementation for building an TimedCache<>.
         * 
         *  \note This class was incompatibly changed in Stroika 3.0d1. It used to have a TRACK_READ_ACCESS parameter.
         *        Since Stroika 3.0d1, instead, if you wish to set that true, call Lookup (..., eTreatFoundThroughLookupAsRefreshed) instead
         *        of Lookup ()
         * 
         *  \note this class was incompatibly changed in Stroika 3.0d23. It used to have the STRICT_INORDER_COPARER as third arugment
         *        but InternallySynchronized added as new third pushing comarer to fourth.
         * 
         *  \note Use of this directly IS allowed, but its fragile, as there isn't a good way to overload or evolve definition over time
         *        so code using this directly will be more likely to not be backward compatible in the future. Better to use adapters like InternallySynchronizedTraits
         * 
         *  \note We allow KEY_EQUALS_COMPARER to be invalid (not IEqualsComparable) - cuz ExplicitTraits maybe used as part of an expression
         *        going through temporary invalid values (defaults) later overriden by WithKeyComparerTraits - for example. We just
         *        require the final TRAITs object handed to LRUCache be VALID.
         * 
         *  \see ITraits<> above
         */
        template <IKey KEY, IValue VALUE, InternallySynchronized INTERNALLY_SYNCHRONIZED, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, Cache::Statistics::IStatsType STATS_TYPE>
        struct ExplicitTraits {
            /**
             */
            using KeyType = KEY;

            /**
             */
            using ValueType = VALUE;

            /**
             */
            using KeyEqualsCompareFunctionType = KEY_EQUALS_COMPARER;

            /**
             */
            using KeyHashFunctionType = KEY_HASH_FUNCTION; // @todo support function TYPE and VALUE

            /**
             * @brief This 'automatic synchronization' feature is off (eNotKnownInternallySynchronized) by default, but can easily
             *        be turned on with InternallySynchronizedTraits
             */
            static constexpr inline InternallySynchronized kInternallySynchronized{INTERNALLY_SYNCHRONIZED};

            /**
             * @brief Internally synchronized 'Stats' collector type (Cache::Statistics::IStatsType). Often null stats collector.
             */
            using StatsType = STATS_TYPE;
        };

        /**
         * @brief Default choices for LRUCache, if you don't specify anything else.
         * 
         * @tparam KEY 
         * @tparam VALUE 
         */
        template <IKey KEY, IValue VALUE>
        using DefaultTraits =
            ExplicitTraits<KEY, VALUE, InternallySynchronized::eNotKnownInternallySynchronized, equal_to<KEY>, nullptr_t, Statistics::StatsType_DEFAULT>;

        /**
         * @brief InternallySynchronizedTraits same as argument traits, but resetting the kInternallySynchronized to eInternallySynchronized
         * 
         * @tparam TRAITS 
         */
        template <typename TRAITS>
            requires (ITraits<TRAITS, typename TRAITS::KeyType, typename TRAITS::ValueType>)
        struct InternallySynchronizedTraits : TRAITS {
            static constexpr inline Execution::InternallySynchronized kInternallySynchronized{Execution::InternallySynchronized::eInternallySynchronized};
        };

        /**
         * @brief WithKeyComparerTraits same as argument traits, but resetting the KeyEqualsCompareFunctionType
         * 
         * @tparam TRAITS 
         */
        template <typename TRAITS, Common::IEqualsComparer<typename TRAITS::KeyType> KEY_EQUALS_COMPARER = equal_to<typename TRAITS::KeyType>>
        struct WithKeyComparerTraits : TRAITS {
            using KeyEqualsCompareFunctionType = KEY_EQUALS_COMPARER; // @todo support function TYPE and VALUE
        };

        /**
         * @brief WithKeyHashTraits same as argument traits, but resetting the KeyEqualsCompareFunctionType
         * 
         * @tparam TRAITS 
         */
        template <typename TRAITS, typename KEY_HASH_FUNCTION = hash<typename TRAITS::KeyType>>
        struct WithKeyHashTraits : TRAITS {
            using KeyHashFunctionType = KEY_HASH_FUNCTION; // @todo support function TYPE and VALUE
        };

    }

    /**
     *  \brief LRUCache implements a simple least-recently-used caching strategy, with optional hashing (of keys) to make it faster.
     *
     *  \note Comparison with TimedCache
     *        The main difference between an LRUCache and TimedCache has to do with when an element is evicted from the Cache.
     *        With a TimedCache, its evicted only when its overly aged (now - when added to cache). With an LRUCache, its more random, and depends a
     *        bit on luck (when using hashing) and how recently an item was last accessed.
     * 
     *        With a TimedCache, you track 'time' associated with each cache element. With LRUCache, no such timing association exists.
     * 
     * 
     *  \note LRUCache (soon will) support(s) IValuelessCache, but requires IKeyedCache (cuz the KEY is critical to how LRUCache works - use TimedCache for
     *        keyless cache)).
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
     *  \note <a href="Design-Overview.md#Comparisons">Comparisons</a>:
     *        o No comparison of LRUCache objects is currently supported. It might make sense, but would be of questionable use.
     * 
     *  \par Implementation Note:
     *       Private (_) routines dont hold locks - the public ones do. And in unsyncrhonized builds, the 'locks' aren't really locks
     *       but Debug::AssertExternallySyncrhonizedMutex - to detect bad usage.
     *
     *  \note   \em Thread-Safety   if (TRAITS::kInternallySynchronized == eInternallySynchronized)           <a href='#Internally-Synchronized-Thread-Safety'>Internally-Synchronized-Thread-Safety</a>
     *  \note   \em Thread-Safety   if (TRAITS::kInternallySynchronized == eNotKnownInternallySynchronized)   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     * 
     *  \note   we REQUIRE (without a way to enforce) - that the STATS object be internally synchronized, so that we can
     *          maintain statistics, without requiring the lookup method be non-const; this is only for tuning/debugging, anyhow...
     */
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS = LRUCacheSupport::DefaultTraits<KEY, VALUE>>
    class LRUCache {
    public:
        using TraitsType = TRAITS;

    public:
        /**
         */
        using KeyType = typename TRAITS::KeyType;

    public:
        /**
         */
        using ValueType = typename TRAITS::ValueType;

    public:
        /**
         */
        using KeyEqualsCompareFunctionType = typename TRAITS::KeyEqualsCompareFunctionType;

    public:
        /**
         */
        using KeyHashFunctionType = typename TRAITS::KeyHashFunctionType;

    public:
        /**
         */
        using StatsType = typename TRAITS::StatsType;

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
         *          LRUCache                 t0{Factory::LRUCache::Maker<string, string>{}(3, 3)};
         *          LRUCache                 t1{Factory::LRUCache::Maker<String, string>{}(3, 3, hashFunction)};  // types (except key/value) deducted from arguments
         *      \endcode
         * 
         *  \todo default CTOR requires no hashing, but we could make hashing work in this case with default params - just not worth it yet --LGP 2023-12-06
         */
        LRUCache ()
            requires (same_as<typename TRAITS::KeyHashFunctionType, nullptr_t> and
                      same_as<typename TRAITS::KeyEqualsCompareFunctionType, equal_to<KEY>>);
        LRUCache (size_t maxCacheSize, const typename TRAITS::KeyEqualsCompareFunctionType& keyEqualsComparer = {})
            requires (same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>);
        LRUCache (size_t maxCacheSize, const typename TRAITS::KeyEqualsCompareFunctionType& keyEqualsComparer = {}, size_t hashTableSize = 1,
                  const typename TRAITS::KeyHashFunctionType& hashFunction = typename TRAITS::KeyHashFunctionType{})
            requires (not same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>);
        LRUCache (size_t maxCacheSize, size_t hashTableSize,
                  const typename TRAITS::KeyHashFunctionType& hashFunction = typename TRAITS::KeyHashFunctionType{})
            requires (not same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>);

        LRUCache (LRUCache&& from);
        LRUCache (const LRUCache& from)
            requires (same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>);
        LRUCache (const LRUCache& from)
            requires (not same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>);

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
        nonvirtual KeyHashFunctionType GetKeyHashFunction () const;

    public:
        /**
         *  Clear all, or just the given elements from the cache.
         * 
         *      @todo unsure about best naming - TimedCache uses RemoveAll () - clear nice, but doesn't match usual meaning in STL, which is my convention for lower-case stl names...
         */
        nonvirtual void clear ();
        nonvirtual void clear (typename Common::ArgByValueType<KEY> key);
        nonvirtual void clear (function<bool (typename Common::ArgByValueType<KEY>)> clearPredicate);

    public:
        /**
         *  @todo see clear(function) and resolve...
         */
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (predicate<KEY>) PREDICATE>
        nonvirtual void RemoveAll (PREDICATE&& removeIfReturnsTrue);

    public:
        /**
         *  The value associated with KEY may not be present, so an missing optional value is returned.
         * 
         *  \note Unintuitively, Lookup () is non-const **intentionally** - because it updates internal data structures to track the most recently accessed item. This has implication for thread-safety!
         *
         *  @see LookupValue ()
         */
        template <typename V = VALUE>
            requires (not IValuelessCache<V>)
        nonvirtual optional<V> Lookup (typename Common::ArgByValueType<KEY> key);
        template <typename V = VALUE>
            requires (IValuelessCache<V>)
        nonvirtual optional<KEY> Lookup (typename Common::ArgByValueType<KEY> key);

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
        nonvirtual VALUE LookupValue (typename Common::ArgByValueType<KEY> key, const function<VALUE (typename Common::ArgByValueType<KEY>)>& valueFetcher);

    public:
        /**
         *  Add the given value to the cache. This is rarely directly used. 
         *  Typically you Lookup with something like LookupValue() which implicitly does the adds.
         */
        template <typename V = VALUE>
            requires (not IValuelessCache<V>)
        nonvirtual void Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<V> value);
        template <typename V = VALUE>
            requires (IValuelessCache<V>)
        nonvirtual void Add (typename Common::ArgByValueType<KEY> key);

    private:
        // like Add () but with no lock (assumes caller/public APIs lock)
        nonvirtual void Add_ (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<VALUE> value);

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
            pair<KEY, VALUE> ignored, size_t maxCacheSize = 1, const typename TRAITS::KeyEqualsCompareFunctionType& keyEqualsComparer = {},
            size_t hashTableSize = 1, const typename TRAITS::KeyHashFunctionType& hashFunction = typename TRAITS::KeyHashFunctionType{})
            : LRUCache{maxCacheSize, keyEqualsComparer, hashTableSize, hashFunction}
        {
        }
        [[deprecated ("Since Stroika v3.0d5 use Cache::Factory::LRUCache_WithHash or NoHash")]] LRUCache (
            pair<KEY, VALUE> ignored, size_t maxCacheSize, size_t hashTableSize,
            const typename TRAITS::KeyHashFunctionType& hashFunction = hash<KEY>{})
            : LRUCache{maxCacheSize, hashTableSize, hashFunction}
        {
        }

    private:
        const size_t fHashtableSize_{1};

    private:
        struct KeyValuePair_ {
            KEY                                          fKey;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE conditional_t<IValuelessCache<VALUE>, Common::Empty, VALUE> fValue;
        };

    private:
        // invoke selected hash function, and return number 0..fHashtableSize_
        nonvirtual size_t H_ (typename Common::ArgByValueType<KEY> k) const;

    private:
        // note if shared_mutex, it must be mutable, cuz shared locks still must be done
        using MaybeMutexType_ =
            conditional_t<TRAITS::kInternallySynchronized == Execution::InternallySynchronized::eInternallySynchronized, shared_timed_mutex, Debug::AssertExternallySynchronizedMutex>;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE mutable MaybeMutexType_ fMaybeMutex_;

    private:
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE const KeyEqualsCompareFunctionType         fKeyEqualsComparer_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE const typename TRAITS::KeyHashFunctionType fHashFunction_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE StatsType                                  fStats_;

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
        nonvirtual optional<KeyValuePair_>* AddNewButDontFillIn_ (typename Common::ArgByValueType<KeyType> item);

        /*
         * Check and see if the given element is in the cache. Return that element if its there, and nullptr otherwise.
         * Note that this routine re-orders the cache so that the most recently looked up element is first, and because
         * of this re-ordering, its illegal to do a Lookup while a @'LRUCache_<ELEMENT>::CacheIterator_' exists
         *       for this LRUCache_.
         */
        nonvirtual optional<KeyValuePair_>* LookupElement_ (typename Common::ArgByValueType<KeyType> item);

        /*
         */
        nonvirtual void ShuffleToHead_ (size_t chainIdx, CacheElement_* b);

        static constexpr size_t kPreallocatedHashtableSize_ =
            same_as<typename TRAITS::KeyHashFunctionType, nullptr_t> ? 1 : 5; // size where no memory allocation overhead for lrucache
        Memory::InlineBuffer<vector<CacheElement_>, kPreallocatedHashtableSize_> fCachedElts_BUF_{};
        Memory::InlineBuffer<CacheElement_*, kPreallocatedHashtableSize_>        fCachedElts_First_{};
        Memory::InlineBuffer<CacheElement_*, kPreallocatedHashtableSize_>        fCachedElts_Last_{};
    };

    /**
     */
    namespace Factory::LRUCache {
        using Execution::InternallySynchronized;

        /**
         * @brief Utility to make it easier to construct a LRUCache constexpr/type name from a few parameters and types.
         * 
         *  \note MAYBE replace this with deduction guides, but not clear how? 
         * 
         * @tparam KEY 
         * @tparam VALUE 
         * @param InternallySynchronized (defaults to eNotKnownInternallySynchronized) 
         * @tparam STATS_TYPE (defaults to Statistics::StatsType_DEFAULT)
         * 
         *  \par Example Usage
         *      \code
         *          auto t0{Factory::LRUCache::Maker<string, string>{}()};
         *          auto t1{Factory::LRUCache::Maker<string, string>{}(3)};
         *          LRUCache t2{Factory::LRUCache::Maker<string, string>{}(3, kStringCIComparer_)};
         *          // Add eInternallySynchronized just as 3rd template arg to Maker<>
         *          LRUCache t3{Factory::LRUCache::Maker<string, string,InternallySynchronized::eInternallySynchronized>{}(3, kStringCIComparer_)};
         *      \endcode
         * 
         *  \par Example Usage
         *      \code
         *          auto     t0{Factory::LRUCache::Maker<string, string>{}(3, 3)};
         *          auto     t1{Factory::LRUCache::Maker<String, string>{}(3, 3, hashFunction)};
         *          LRUCache t2{Factory::LRUCache::Maker<String, string>{}(3, equal_to<String>{}, 3)};
         *          LRUCache t3{Factory::LRUCache::Maker<String, string, Statistics::Stats_Basic>{}(3, equal_to<String>{}, 3)}; // throw in stats object
         *          LRUCache t4{Factory::LRUCache::Maker<String, string>{}(3, kStringCIComparer_, 3)}; // alt equality comparer
         *      \endcode
         */
        template <typename KEY, typename VALUE, InternallySynchronized internallySynchronized = InternallySynchronized::eNotKnownInternallySynchronized,
                  typename STATS_TYPE = Statistics::StatsType_DEFAULT>
        struct Maker {
            /**
             * @brief  NOHASH versions
             */
#if __cplusplus >= kStrokia_Foundation_Common_cplusplus_23 || _HAS_CXX23 /*vis studio uses _HAS_CXX23 */
            template <Common::IEqualsComparer<KEY> KEY_EQUALS_COMPARER = equal_to<KEY>>
            static auto operator() (size_t maxCacheSize = 1, KEY_EQUALS_COMPARER&& keyComparer = {});
#else
            template <Common::IEqualsComparer<KEY> KEY_EQUALS_COMPARER = equal_to<KEY>>
            auto operator() (size_t maxCacheSize = 1, KEY_EQUALS_COMPARER&& keyComparer = {}) const;
#endif

            /**
             * @brief  Hashing versions
             */
            template <typename KEY_HASH_FUNCTION = hash<KEY>>
#if __cplusplus >= kStrokia_Foundation_Common_cplusplus_23 || _HAS_CXX23 /*vis studio uses _HAS_CXX23 */
            static auto operator() (size_t maxCacheSize, size_t hashTableSize, KEY_HASH_FUNCTION&& hashFunction = {});
#else
            auto operator() (size_t maxCacheSize, size_t hashTableSize, KEY_HASH_FUNCTION&& hashFunction = {}) const;
#endif
            template <typename KEY_EQUALS_COMPARER = equal_to<KEY>, typename KEY_HASH_FUNCTION = hash<KEY>>
#if __cplusplus >= kStrokia_Foundation_Common_cplusplus_23 || _HAS_CXX23 /*vis studio uses _HAS_CXX23 */
            static auto operator() (size_t maxCacheSize, KEY_EQUALS_COMPARER&& keyComparer, size_t hashTableSize,
                                    KEY_HASH_FUNCTION&& hashFunction = {});
#else
            auto operator() (size_t maxCacheSize, KEY_EQUALS_COMPARER&& keyComparer, size_t hashTableSize, KEY_HASH_FUNCTION&& hashFunction = {}) const;
#endif
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
