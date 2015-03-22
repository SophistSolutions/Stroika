/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_LRUCache_h_
#define _Stroika_Foundation_Cache_LRUCache_h_  1

#include    "../StroikaPreComp.h"

#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Characters/String.h"
#include    "../Containers/Mapping.h"
#include    "../Debug/AssertExternallySynchronizedLock.h"
#include    "../Memory/Optional.h"



/**
 *      \file
 *
 * TODO:
 *
 *      @todo   Consider if we should be using Optional_Traits_Blockallocated_Indirect_Storage.
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
 *      @todo   Verify the size of  TRAITS::StatsType   fStats; is zero, or go back to old qKeepLRUCacheStats
 *              macro stuff to avoid wasted space.
 *                  o  Trouble with subobject approach is C++ seems to force all objects to be at least one byte,
 *                      so there WOULD be cost. Could avoid that by having the TRAITS
 *                      OBJECT ITSELF be what owns the counters - basically global vars. Since just
 *                      used for testing, could still be usable that way...
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {


            namespace   LRUCacheSupport {


                /**
                 *  Helper detail class for analyzing and tuning cache statistics.
                 */
                struct  Stats_Basic {
                    size_t      fCachedCollected_Hits   { };
                    size_t      fCachedCollected_Misses { };
                    void    IncrementHits ();
                    void    IncrementMisses ();
                };


                /**
                 *  Helper for DefaultTraits - when not collecting stats.
                 */
                struct  Stats_Null {
                    void    IncrementHits ();
                    void    IncrementMisses ();
                };


                /**
                 *  Helper for DefaultTraits.
                 */
#if     qDebug
                using   StatsType_DEFAULT   =   Stats_Basic;
#else
                using   StatsType_DEFAULT   =   Stats_Null;
#endif

                /**
                 */
                template    <typename KEY, size_t HASH_TABLE_SIZE = 1, typename KEY_EQUALS_COMPARER = Common::ComparerWithEquals<KEY>>
                struct  DefaultTraits {
                    using   KeyType     =   KEY;

                    /**
                     *  HASHTABLESIZE must be >= 1, but if == 1, then Hash function not used
                     */
                    DEFINE_CONSTEXPR_CONSTANT(size_t, kHashTableSize, HASH_TABLE_SIZE);

                    //tmphack - SHOUDL do smarter defaults!!!!
                    template    <typename SFINAE>
                    static  size_t  Hash_SFINAE_ (const KEY& e, typename enable_if < is_arithmetic<SFINAE>::value || is_convertible<SFINAE, string>::value || is_convertible<SFINAE, Characters::String>::value, void >::type* = nullptr);
                    template    <typename SFINAE>
                    static  size_t  Hash_SFINAE_ (const KEY& e, typename enable_if < not (is_arithmetic<SFINAE>::value || is_convertible<SFINAE, string>::value || is_convertible<SFINAE, Characters::String>::value), void >::type* = nullptr);
                    static  size_t  Hash (const KEY& e);

                    /**
                     */
                    using   KeyEqualsCompareFunctionType    =   KEY_EQUALS_COMPARER;

                    /**
                     */
                    using   StatsType   =   LRUCacheSupport::StatsType_DEFAULT;
                };


            }


            /**
             *  LRUCache is NOT threadsafe (checks usage with Debug::AssertExternallySynchronizedLock), so typical uses would use Execution::Synchonized.
             *
             *  \note   LRUCache destroys objects when they are cleared from the cache. This guarantee is
             *          relevant only in case where the objects use significant resources, or where their lifetime has
             *          externally visible (e.g. lockfiles) impact.
             *
             *  EXAMPLE USAGE:
             *      Execution::Synchronized<LRUCache<DetailsID, Details_>>      sDetailsCache_; // caches often helpful in multithreaded situations
             */
            template    <typename KEY, typename VALUE, typename TRAITS = LRUCacheSupport::DefaultTraits<KEY>>
            class   LRUCache : /*private*/private Debug::AssertExternallySynchronizedLock {
            public:
                using   TraitsType  =   TRAITS;

            public:
                using   KeyType     =   KEY;

            public:
                using   KeyEqualsCompareFunctionType =  typename TRAITS::KeyEqualsCompareFunctionType;

            public:
                LRUCache (size_t size = 1);
                LRUCache (const LRUCache& from);

            public:
                nonvirtual  const LRUCache& operator= (const LRUCache& rhs);

            public:
                /**
                 */
                nonvirtual  size_t  GetMaxCacheSize () const;

            public:
                /**
                 */
                nonvirtual  void    SetMaxCacheSize (size_t maxCacheSize);

            public:
                /**
                 */
                nonvirtual  typename TRAITS::StatsType  GetStats () const;

            public:
                /**
                 */
                nonvirtual  void    clear ();
                nonvirtual  void    clear (const KEY& key);
                nonvirtual  void    clear (function<bool(const KEY&)> clearPredicate);

            public:
                /**
                 *  The value associated with KEY may not be present, so an Optional is returned.
                 *
                 *  \note  experimental as of 2015-02-01 - use of Optional_Traits_Blockallocated_Indirect_Storage - maybe use regular optional traits?
                 *
                 *  @see LookupValue ()
                 */
                nonvirtual  Memory::Optional<VALUE, Memory::Optional_Traits_Blockallocated_Indirect_Storage<VALUE>> Lookup (const KEY& key) const;

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
                 *  EXAMPLE USAGE:
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
                 */
                nonvirtual  VALUE   LookupValue (const KEY& key, const function<VALUE(KEY)>& valueFetcher);

            public:
                /**
                 */
                nonvirtual  void    Add (const KEY& key, const VALUE& value);

            public:
                /**
                 */
                nonvirtual  Containers::Mapping<KEY, VALUE, Containers::Mapping_DefaultTraits<KEY, VALUE, KeyEqualsCompareFunctionType>>     Elements () const;

            private:
                struct  KeyValuePair_ {
                    KEY     fKey;
                    VALUE   fValue;
                };
                using   OptKeyValuePair_ = Memory::Optional<KeyValuePair_, Memory::Optional_Traits_Blockallocated_Indirect_Storage<KeyValuePair_>>;

                template    <typename SFINAE = KEY>
                static  size_t  H_ (const SFINAE& k);
                static  size_t  Hash_ (const KEY& e);

            private:
                struct      LRUCache_ {
                    LRUCache_ (size_t maxCacheSize);
                    LRUCache_ () = delete;
                    LRUCache_ (const LRUCache_&) = delete;
                    nonvirtual  LRUCache_& operator= (const LRUCache_&) = delete;

                    nonvirtual  size_t  GetMaxCacheSize () const;
                    nonvirtual  void    SetMaxCacheSize (size_t maxCacheSize);

                    struct  CacheElement_;
                    struct  CacheIterator;

                    nonvirtual  CacheIterator   begin ();
                    nonvirtual  CacheIterator   end ();

                    nonvirtual  void        ClearCache ();
                    nonvirtual  OptKeyValuePair_*    AddNew (const KeyType& item);
                    nonvirtual  OptKeyValuePair_*    LookupElement (const KeyType& item);

                    typename TraitsType::StatsType  fStats;

                    vector<CacheElement_>   fCachedElts_BUF_[TRAITS::kHashTableSize];      // we don't directly use these, but use the First_Last pointers instead which are internal to this buf
                    CacheElement_*          fCachedElts_First_[TRAITS::kHashTableSize];
                    CacheElement_*          fCachedElts_Last_[TRAITS::kHashTableSize];

                    nonvirtual  void    ShuffleToHead_ (size_t chainIdx, CacheElement_* b);
                };

            private:
                mutable LRUCache_  fRealCache_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "LRUCache.inl"

#endif  /*_Stroika_Foundation_Cache_LRUCache_h_*/
