/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_LRUCache_h_
#define _Stroika_Foundation_Cache_LRUCache_h_ 1

#include "../StroikaPreComp.h"

#include <vector>

#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Configuration/TypeHints.h"
#include "../Containers/Mapping.h"
#include "../Debug/AssertExternallySynchronizedLock.h"
#include "../Memory/Optional.h"

/**
 *      \file
 *
 * TODO:
 *
 *      @todo   Possible get rid of intermediate LRUCache_ object.
 *
 *      @todo   Find some reasonable/simple way to get
 *              LRUCache<PHRShortcutSpec, PHRShortcutSpec, PHRShortcutSpecNoAuthCacheTraits_>   sRecentlyUsedCache (kMaxEltsInReceltlyUsedCache_);
 *              Working with ONE T argument
 *              Add(elt2cache).
 *
 *      @todo   Hash_SFINAE_<> IS HORRIBLE HACK!!!! CLEANUP!!! Tricky... SFINAE -- SEE SerializeForHash1_
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

namespace Stroika {
    namespace Foundation {
        namespace Cache {

            namespace LRUCacheSupport {

                /**
                 *  Helper detail class for analyzing and tuning cache statistics.
                 */
                struct Stats_Basic {
                    size_t fCachedCollected_Hits{};
                    size_t fCachedCollected_Misses{};
                    void   IncrementHits ();
                    void   IncrementMisses ();
                };

                /**
                 *  Helper for DefaultTraits - when not collecting stats.
                 */
                struct Stats_Null {
                    void IncrementHits ();
                    void IncrementMisses ();
                };

/**
                 *  Helper for DefaultTraits.
                 */
#if qDebug
                using StatsType_DEFAULT = Stats_Basic;
#else
                using StatsType_DEFAULT = Stats_Null;
#endif

                /**
                 */
                template <typename KEY, typename VALUE, size_t HASH_TABLE_SIZE = 1, typename KEY_EQUALS_COMPARER = Common::DefaultEqualsComparer<KEY>>
                struct DefaultTraits {
                    using KeyType = KEY;

                    /**
                     *  HASHTABLESIZE must be >= 1, but if == 1, then Hash function not used
                     */
                    static constexpr size_t kHashTableSize = HASH_TABLE_SIZE;

                    static_assert (kHashTableSize >= 1, "HASH_TABLE_SIZE template parameter must be >= 1");

                    //tmphack - SHOUDL do smarter defaults!!!!
                    template <typename SFINAE>
                    static size_t Hash_SFINAE_ (typename Configuration::ArgByValueType<KEY> e, typename enable_if<is_arithmetic<SFINAE>::value or is_convertible<SFINAE, string>::value or is_convertible<SFINAE, Characters::String>::value, void>::type* = nullptr);
                    template <typename SFINAE>
                    static size_t Hash_SFINAE_ (typename Configuration::ArgByValueType<KEY> e, typename enable_if<not(is_arithmetic<SFINAE>::value or is_convertible<SFINAE, string>::value or is_convertible<SFINAE, Characters::String>::value), void>::type* = nullptr);
                    static size_t Hash (typename Configuration::ArgByValueType<KEY> e);

                    /**
                     */
                    using KeyEqualsCompareFunctionType = KEY_EQUALS_COMPARER;

                    /**
                     */
                    using StatsType = LRUCacheSupport::StatsType_DEFAULT;

                    /**
                     *      \note   Replace OptionalValueType value with Memory::Optional<VALUE, Memory::Optional_Traits_Blockallocated_Indirect_Storage<VALUE>>
                     *              in your traits subclass if you have compiler errors becacuse you define or LRUCache before the type you
                     *              are caching.
                     */
                    using OptionalValueType = Memory::Optional<VALUE>;
                };
            }

            /**
             *  LRUCache is NOT threadsafe (checks usage with Debug::AssertExternallySynchronizedLock), so typical uses would use Execution::Synchronized.
             *
             *  \note   LRUCache destroys objects when they are cleared from the cache. This guarantee is
             *          relevant only in case where the objects use significant resources, or where their lifetime has
             *          externally visible (e.g. lockfiles) impact.
             *
             *  \par Example Usage
             *      \code
             *      Execution::Synchronized<LRUCache<DetailsID, Details_>>      sDetailsCache_; // caches often helpful in multithreaded situations
             *      \endcode
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#ExternallySynchronized">ExternallySynchronized</a>
             *
             */
            template <typename KEY, typename VALUE, typename TRAITS = LRUCacheSupport::DefaultTraits<KEY, VALUE>>
            class LRUCache : private Debug::AssertExternallySynchronizedLock {
            public:
                using TraitsType = TRAITS;

            public:
                using KeyType = KEY;

            public:
                using KeyEqualsCompareFunctionType = typename TRAITS::KeyEqualsCompareFunctionType;

            public:
                using OptionalValueType = typename TRAITS::OptionalValueType;

            public:
                LRUCache (size_t size = 1);
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
                nonvirtual typename TRAITS::StatsType GetStats () const;

            public:
                /**
                 */
                nonvirtual void clear ();
                nonvirtual void clear (typename Configuration::ArgByValueType<KEY> key);
                nonvirtual void clear (function<bool(typename Configuration::ArgByValueType<KEY>)> clearPredicate);

            public:
                /**
                 *  The value associated with KEY may not be present, so an Optional is returned.
                 *
                 *  @see LookupValue ()
                 */
                nonvirtual OptionalValueType Lookup (typename Configuration::ArgByValueType<KEY> key) const;

            public:
                /**
                 *  LookupValue () finds the value in the cache, and returns it, or if not present, uses the argument valueFetcher to retrieve it.
                 *
                 *  So LookupValue (v) is equivilent to:
                 *          if (auto o = Lookup (k)) {
                 *              return o;
                 *          }
                 *          else {
                 *              auto v = valueFetcher (k);
                 *              Add (k, v);
                 *              return v;
                 *          }
                 *
                 *  \par Example Usage
                 *      \code
                 *      struct Details_ {
                 *      };
                 *      using DetailsID = int;
                 *      Details_ ReadDetailsFromFile_ (DetailsID id);
                 *
                 *      Execution::Synchronized<LRUCache<DetailsID, Details_>>      fDetailsCache_; // caches often helpful in multithreaded situations
                 *
                 *      // returns the value from LRUCache, or automatically pages it in from file
                 *      Details_    GetDetails (DetailsID id) {
                 *          return
                 *              fDetailsCache_->LookupValue (
                 *                  id,
                 *                  [] (DetailsID id) -> Details_ { return ReadDetailsFromFile_ (id); }
                 *              );
                 *      }
                 *      \endcode
                 */
                nonvirtual VALUE LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& valueFetcher);

            public:
                /**
                 */
                nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> value);

            public:
                /**
                 */
                nonvirtual Containers::Mapping<KEY, VALUE, Containers::DefaultTraits::Mapping<KEY, VALUE, KeyEqualsCompareFunctionType>> Elements () const;

            private:
                struct KeyValuePair_ {
                    KEY   fKey;
                    VALUE fValue;
                };
                using OptKeyValuePair_ = Memory::Optional<KeyValuePair_, Memory::Optional_Traits_Blockallocated_Indirect_Storage<KeyValuePair_>>;

                template <typename SFINAE = KEY>
                static size_t H_ (typename Configuration::ArgByValueType<SFINAE> k);
                static size_t Hash_ (typename Configuration::ArgByValueType<KEY> e);

            private:
                // nb: inherit from TraitsType::StatsType for zero-size base class sub-object rule
                struct LRUCache_ : TraitsType::StatsType {
                    LRUCache_ (size_t maxCacheSize);
                    LRUCache_ ()                  = delete;
                    LRUCache_ (const LRUCache_&)  = delete;
                    nonvirtual LRUCache_& operator= (const LRUCache_&) = delete;

                    nonvirtual size_t GetMaxCacheSize () const;
                    nonvirtual void   SetMaxCacheSize (size_t maxCacheSize);

                    struct CacheElement_;
                    struct CacheIterator;

                    nonvirtual CacheIterator begin ();
                    nonvirtual CacheIterator end ();

                    nonvirtual void ClearCache ();
                    nonvirtual OptKeyValuePair_* AddNew (typename Configuration::ArgByValueType<KeyType> item);
                    nonvirtual OptKeyValuePair_* LookupElement (typename Configuration::ArgByValueType<KeyType> item);

                    vector<CacheElement_> fCachedElts_BUF_[TRAITS::kHashTableSize]; // we don't directly use these, but use the First_Last pointers instead which are internal to this buf
                    CacheElement_*        fCachedElts_First_[TRAITS::kHashTableSize] = {};
                    CacheElement_*        fCachedElts_Last_[TRAITS::kHashTableSize]  = {};

                    nonvirtual void ShuffleToHead_ (size_t chainIdx, CacheElement_* b);
                };

            private:
                mutable LRUCache_ fRealCache_;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LRUCache.inl"

#endif /*_Stroika_Foundation_Cache_LRUCache_h_*/
