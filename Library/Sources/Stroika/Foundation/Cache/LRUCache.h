/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_LRUCache_h_
#define _Stroika_Foundation_Cache_LRUCache_h_  1

#include    "../StroikaPreComp.h"

#include    <vector>

#include    "../Configuration/Common.h"

/// move these to .inl file when we move code for nu_LRUCache
#include    "../Cryptography/Digest/Algorithm/Jenkins.h"
#include    "../Cryptography/Hash.h"

#include    "../Memory/Optional.h"



/**
 *      \file
 *
 * TODO:
 *
 *
 *      @todo   Change default MUTEX argument for LRUCache to AssertExternallySycnhonized, and then maybe get rid of it,
 *              cuz callers can just use 'Synchonized'
 *
 *
 *
 *
 *      @todo   Look at nu_LRUCache<> and consider possability of migrating to it, or something like that
 *              Much slower, but threadsafe, and simpler to use.
 *
 *              Probably migrate older code to class Legacy_LRUCache<>
 *
 *      @todo   TERRIBLE API!
 *                  LRUCache<string> tmp (3);
 *                  *tmp.AddNew ("x") = "x";
 *                  *tmp.AddNew ("y") = "y";
 *                  *tmp.AddNew ("z") = "z";
 *                  *tmp.AddNew ("a") = "a";
 *              So awkward to use!!! - find better - more map like - API
 *
 *      @todo   One major source of UNSAFETY (especially thread unsafety) is
 *              AddNew/Lookup() API returning internal pointers. We really want this to do copy by
 *              value. But look where its used, and see the costs are not too high of doing copy my value.
 *              Or maybe use concext update object (maybe taking a lambda) for the update).
 *
 *      @todo   NOTE - I'm NOT sure public API for this is stable yet - there are many problems (though
 *              I've used this class quite a lot - its not buggy - just not flexibly designed or good
 *              thread safety properties).
 *
 *      @todo   Test and make sure ITERATOR stuff works properly when using Hashing mode (HASTABLE SIZE > 1)
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
 *      @todo   Consider restructuring the API more like STL-MAP
 *              KEY,VALUE, intead of LRUCache<ELEMENT> and the ability to extract an element.
 *              Doing this MIGHT even allow us to make this class fit more neatly within the Stroika container
 *              pantheon. BUt it will be a bit of a PITA for all/some of the existing LRUCache<> uses...
 *
 *              Problem of doing this is that in some cases - we may want to create a cache of objects that
 *              already pre-exist, and have the 'key part' as a subobject of themselves. Not a killer as we
 *              COULD just call the KEY/ELEMENT the same type and just use a simple test traits function
 *              that just pays attention to the logical key parts?
 *
 *      @todo   Cleanup docs to reflect new TRAITS style, and document each public method (still todo) and docs for
 *              main class needs cleanup).
 *
 *              AND docs for main class need a simple example of usage (without a traits override).
 *
 *      @todo   Verify the size of  TRAITS::StatsType   fStats; is zero, or go back to old qKeepLRUCacheStats
 *              macro stuff to avoid wasted space.
 *                  o  Trouble with subobject approach is C++ seems to force all objects to be at least one byte,
 *                      so there WOULD be cost. Could avoid that by having the TRAITS
 *                      OBJECT ITSELF be what owns the counters - basically global vars. Since just
 *                      used for testing, could still be usable that way...
 *
 *      @todo   Come up with easy way to persist cache. I suppose this counts on fact that you can load/store
 *              cached elements – so persist mechanism must parameterize that).
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
                    size_t      fCachedCollected_Hits { 0 };
                    size_t      fCachedCollected_Misses { 0 };
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
                 * The DefaultTraits<> is a simple default traits implementation for building an LRUCache<>.
                 */
                template    <typename   ELEMENT, typename KEY = ELEMENT>
                struct  DefaultTraits_ {
                    using   ElementType =   ELEMENT;
                    using   KeyType     =   KEY;

                    // HASHTABLESIZE must be >= 1, but if == 1, then Hash function not used
                    DEFINE_CONSTEXPR_CONSTANT(uint8_t, HASH_TABLE_SIZE, 1);

                    static  KeyType ExtractKey (const ElementType& e);

                    // If KeyType different type than ElementType we need a hash for that too
                    static  size_t  Hash (const KeyType& e);

                    // defaults to using default CTOR for ElementType and copying over
                    static  void    Clear (ElementType* element);

                    // defaults to operator==
                    static  bool    Equal (const KeyType& lhs, const KeyType& rhs);

#if     qDebug
                    using   StatsType   =   Stats_Basic;
#else
                    using   StatsType   =   Stats_Null;
#endif
                };


            }


            /**
             *  A basic LRU (least recently used) cache mechanism. You provide a class type argument 'ELEMENT' defined roughly as follows:
             *
             *  TODO: THIS DOC IS OBSOLETE - PRE TRAITS implementation!!!
             *
             *
             *
                <br>
                <code>
                    struct  ELEMENT {
                        struct  COMPARE_ITEM;
                        nonvirtual  void    Clear ();
                        static      bool    Equal (const ELEMENT&, const COMPARE_ITEM&);
                    };
                </code>
                </p>
                    <p>The <code>COMPARE_ITEM</code> is an object which defines the attributes which make the given item UNIQUE (for Lookup purposes).
                Think of it as the KEY. The <code>Clear ()</code> method must be provided to invaliate the given item (usually by setting part of the COMPARE_ITEM
                to an invalid value) so it won't get found by a Lookup. The 'Equal' method compares an element and a COMPARE_ITEM in the Lookup method.
                    </p>
                    <p>Note that the type 'ELEMENT' must be copyable (though its rarely copied - just in response
                to a @'LRUCache<ELEMENT>::SetMaxCacheSize' call.</p>
                    <p>To iterate over the elements of the cache - use an @'LRUCache<ELEMENT>::CacheIterator'.
                    <p>Note this class is NOT THREADSAFE, and must be externally locked. This is because it returns pointers
                to internal data structures (the cached elements).
                *

                *NOTE that this the reason for the ELEMNET first arg to LRUCache template is so I can do default
                * template more easily - for second argument. Otherwise, its really not needed and must agree with
                * the elemnt in TRAITS::ElementType;
            */
            template    <typename   ELEMENT, typename TRAITS = LRUCacheSupport::DefaultTraits_<ELEMENT>>
            class   LRUCache_ {
            public:
                using   ElementType     =   typename TRAITS::ElementType;
                using   KeyType         =   typename TRAITS::KeyType;

            public:
                LRUCache_ (size_t maxCacheSize);
                LRUCache_ () = delete;
                LRUCache_ (const LRUCache_&) = delete;

            public:
                nonvirtual  LRUCache_& operator= (const LRUCache_&) = delete;

            public:
                nonvirtual  size_t  GetMaxCacheSize () const;
                nonvirtual  void    SetMaxCacheSize (size_t maxCacheSize);

            public:
                struct  CacheIterator;
            public:
                nonvirtual  CacheIterator   begin ();
                nonvirtual  CacheIterator   end ();

            public:
                nonvirtual  void    ClearCache ();

            public:
                /**
                 *  NOTE - though you can CHANGE the value of ELEMENT, it is illegal to change its KEY part/key
                 *  value if you specified HASH_TABLE_SIZE != 1 in TRAITS object.
                 */
                nonvirtual  ELEMENT*    AddNew (const KeyType& item);

            public:
                /*
                 *  NOTE - though you can CHANGE the value of ELEMENT, it is illegal to change its KEY part/key
                 *  value if you specified HASH_TABLE_SIZE != 1 in TRAITS object.
                 */
                nonvirtual  ELEMENT*    LookupElement (const KeyType& item);

            public:
                typename TRAITS::StatsType  fStats;

            private:
                struct  CacheElement_ {
                public:
                    CacheElement_ ();

                public:
                    CacheElement_*   fNext;
                    CacheElement_*   fPrev;

                public:
                    ElementType     fElement;
                };

            private:
                vector<CacheElement_>   fCachedElts_BUF_[TRAITS::HASH_TABLE_SIZE];      // we don't directly use these, but use the First_Last pointers instead which are internal to this buf
                CacheElement_*          fCachedElts_First_[TRAITS::HASH_TABLE_SIZE];
                CacheElement_*          fCachedElts_Last_[TRAITS::HASH_TABLE_SIZE];

            private:
                nonvirtual  void    ShuffleToHead_ (size_t chainIdx, CacheElement_* b);
            };


            /*
            @CLASS:         LRUCache<ELEMENT>::CacheIterator
            @DESCRIPTION:   <p>Used to iterate over elements of an @'LRUCache<ELEMENT>'</p>
                            <p>Please note that while an CacheIterator object exists for an LRUCache - it is not
                        safe to do other operations on the LRUCache - like @'LRUCache<ELEMENT>::LookupElement' or @'LRUCache<ELEMENT>::AddNew'.
                        </p>
                //TODO: Must update implementation to support BUCKETS (hashtable)
                //TODO: NOTE: UNSAFE ITERATION - UNLIKE the rest of Stroika (yes - SB fixed) - really an STL-style - not stroika style - iterator...
            */
            template    <typename   ELEMENT, typename TRAITS>
            struct  LRUCache_<ELEMENT, TRAITS>::CacheIterator {
                explicit CacheIterator (CacheElement_** start, CacheElement_** end);

            public:
                nonvirtual  CacheIterator& operator++ ();
                nonvirtual  ELEMENT& operator* ();
                nonvirtual  bool operator== (CacheIterator rhs);
                nonvirtual  bool operator!= (CacheIterator rhs);

            private:
                CacheElement_**  fCurV;
                CacheElement_**  fEndV;
                CacheElement_*   fCur;
            };





            /// MIGRATION STRATEGY FROM OLD TO NEW NAMES/DESIGN
            template    <typename   ELEMENT, typename KEY = ELEMENT>
            using  DefaultTraits_LEGACY = LRUCacheSupport::DefaultTraits_<ELEMENT, KEY>;
            template    <typename   ELEMENT, typename TRAITS = DefaultTraits_LEGACY<ELEMENT>>
            using   LRUCache_LEGACY  = LRUCache_<ELEMENT, TRAITS>;



/// DRAFT NEW API
            namespace   LRUCacheSupport {
                template    <typename KEY, size_t HASH_TABLE_SIZE = 1, typename MUTEX = mutex>
                struct  DefaultTraits {
                    // HASHTABLESIZE must be >= 1, but if == 1, then Hash function not used
                    DEFINE_CONSTEXPR_CONSTANT(size_t, kHashTableSize, HASH_TABLE_SIZE);

                    // If KeyType different type than ElementType we need a hash for that too
                    static  size_t  Hash (const KEY& e)
                    {
                        using   Cryptography::Hash;
                        using   Cryptography::Digest::Digester;
                        using   Cryptography::Digest::Algorithm::Jenkins;
                        using   USE_DIGESTER_     =   Digester<Jenkins>;
                        return Hash<USE_DIGESTER_, KEY, size_t> (e);
                    }

                    // defaults to operator==
                    static  bool    Equals (const KEY& lhs, const KEY& rhs)
                    {
                        return lhs == rhs;
                    }

                    using   MutexType   =   MUTEX;

#if     qDebug
                    using   StatsType   =   LRUCacheSupport::Stats_Basic;
#else
                    using   StatsType   =   LRUCacheSupport::Stats_Null;
#endif
                };


                //@todo LOSE/DEPRECATE THIS NAME
                template    <typename KEY, size_t HASH_TABLE_SIZE = 1>
                using  LRUCache_DefaultTraits  = DefaultTraits<KEY, HASH_TABLE_SIZE>;

            }

            template    <typename KEY, typename VALUE, typename TRAITS = LRUCacheSupport::DefaultTraits<KEY>>
            class   LRUCache {
            private:
                struct  LEGACYLRUCACHEOBJ_ {
                    KEY     fKey;
                    VALUE   fValue;
                };
                struct  LEGACYLRUCACHEOBJ_TRAITS_ : Cache::LRUCacheSupport::DefaultTraits_<LEGACYLRUCACHEOBJ_, KEY> {
                    static  KEY ExtractKey (const LEGACYLRUCACHEOBJ_& e)
                    {
                        return e.fKey;
                    }
                    DEFINE_CONSTEXPR_CONSTANT(size_t, HASH_TABLE_SIZE, TRAITS::kHashTableSize);
                    static  size_t  Hash (const KEY& e)
                    {
                        return TRAITS::Hash (e);
                    }
                    static  bool    Equal (const KEY& lhs, const KEY& rhs)
                    {
                        return TRAITS::Equals (lhs, rhs);
                    }
                };
                mutable Cache::LRUCache_<LEGACYLRUCACHEOBJ_, LEGACYLRUCACHEOBJ_TRAITS_>  fRealCache_;

                using MutexType = typename TRAITS::MutexType;

                mutable MutexType   fLock_;
            public:
                LRUCache (size_t size = 1)
                    : fRealCache_ (size)
                    , fLock_ ()
                {
                }
                LRUCache (const LRUCache& from)
                    : fRealCache_ (1)
                    , fLock_ ()
                {
                    fRealCache_.SetMaxCacheSize (from.GetMaxCacheSize ());
                    auto    critSec { Execution::make_unique_lock (from.fLock_) };
                    for (auto i : from.fRealCache_) {
                        Add (i.fKey, i.fValue);
                    }
                }
                const LRUCache& operator= (const LRUCache& rhs)
                {
                    if (this != &rhs) {
                        SetMaxCacheSize (rhs.GetMaxCacheSize ());
                        auto    critSec { Execution::make_unique_lock (rhs.fLock_) };
                        for (auto i : rhs.fRealCache_) {
                            Add (i.fKey, i.fValue);
                        }
                    }
                    return *this;
                }

            public:
                nonvirtual  size_t  GetMaxCacheSize () const
                {
                    auto    critSec { Execution::make_unique_lock (fLock_) };
                    return fRealCache_.GetMaxCacheSize ();
                }

            public:
                nonvirtual  void    SetMaxCacheSize (size_t maxCacheSize)
                {
                    auto    critSec { Execution::make_unique_lock (fLock_) };
                    fRealCache_.SetMaxCacheSize (maxCacheSize);
                }

            public:
                nonvirtual  typename TRAITS::StatsType  GetStats () const
                {
                    auto    critSec { Execution::make_unique_lock (fLock_) };
                    return fRealCache_.fStats;
                }

            public:
                nonvirtual  void    clear ()
                {
                    auto    critSec { Execution::make_unique_lock (fLock_) };
                    fRealCache_.ClearCache ();
                }

            public:
                Memory::Optional<VALUE> Lookup (const KEY& key) const
                {
                    auto    critSec { Execution::make_unique_lock (fLock_) };
                    LEGACYLRUCACHEOBJ_*  v   =   fRealCache_.LookupElement (key);
                    if (v == nullptr) {
                        return Memory::Optional<VALUE> ();
                    }
                    Ensure (TRAITS::Equals (key, v->fKey));
                    return v->fValue;
                }
            public:
                void Add (const KEY& key, const VALUE& value)
                {
                    auto    critSec { Execution::make_unique_lock (fLock_) };
                    LEGACYLRUCACHEOBJ_*  v   =   fRealCache_.AddNew (key);
                    v->fKey = key;
                    v->fValue = value;
                }

            public:
                VALUE   LookupValue (const KEY& key, const function<VALUE(KEY)>& valueFetcher)
                {
                    Memory::Optional<VALUE> v = Lookup (key);
                    if (v.IsMissing ()) {
                        VALUE   newV = valueFetcher (key);
                        Add (key, newV);
                        return newV;
                    }
                    else {
                        return *v;
                    }
                }
            };




#if 0
            template    <typename KEY, size_t HASH_TABLE_SIZE = 1>
            using   nu_LRUCache_DefaultTraits  = LRUCacheSupport::LRUCache_DefaultTraits<KEY, HASH_TABLE_SIZE>;
            template    <typename KEY, typename VALUE, typename TRAITS = nu_LRUCache_DefaultTraits<KEY>>
            using   nu_LRUCache = LRUCache<KEY, VALUE, TRAITS>;
#endif



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
