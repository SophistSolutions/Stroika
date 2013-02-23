/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_LRUCache_h_
#define _Stroika_Foundation_Cache_LRUCache_h_  1

#include    "../StroikaPreComp.h"

#include    <vector>

#include    "../Configuration/Common.h"


/**
 *      \file
 *
 * TODO:
 *
 *          o   Test and make sure ITERATOR stuff works properly when using Hashing mode (HASTABLE SIZE > 1)
 *
 *          o   Currently we have redundant storage - _Buf, and _First, and _Last (really just need _Buf cuz
 *              has first/last, or do our own storage managemnet with secondary array? - we do the mallocs/frees.
 *              To re-free, even though order distorted by shuffles, we can always figure out which was
 *              the original array head by which has the lowest address!
 *
 *              Also somewhat related, _Last usage is C++ unconvnetional - though maybe OK. If not more awkward
 *              in impl, consider using _fEnd? Or if it is (I think last maybe better then document clearly why its better.
 *
 *          o   Consider restructuring the API more like STL-MAP
 *              KEY,VALUE, intead of LRUCache<ELEMENT> and the ability to extract an element.
 *              Doing this MIGHT even allow us to make this class fit more neatly within the Stroika container pantheon.
 *              BUt it will be a bit of a PITA for all/some of the existing LRUCache<> uses...
 *
 *              Problem of doing this is that in some cases - we may want to create a cache of objects that
 *              already pre-exist, and have the 'key part' as a subobject of themselves. Not a killer as we
 *              COULD just call the KEY/ELEMENT the same type and just use a simple test traits function that just pays
 *              attention to the logical key parts?
 *
 *          o   Cleanup docs to reflect new TRAITS style
 *
 *          o   PERHAPS get rid of qKeepLRUCacheStats - and instead have INCREMTEN_HITS()/INCREMNT_REQUESTS_ methods
 *              in TRAITS, and STATS subobject in traits So no cost. Trouble with subobject approach is C++ seems to
 *              force all objects to be at least one byte, so there WOULD be cost. Could avoid that by having the TRAITS
 *              OBJECT ITSELF be what owns the counters - basically global vars. Since just used for testing, could still
 *              be usable that way...
 */







namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {


            /*
            @CONFIGVAR:     qKeepLRUCacheStats
            @DESCRIPTION:   <p>Defines whether or not we capture statistics (for debugging purposes) in @'LRUCache<ELEMENT>'.
             */
#if     !defined (qKeepLRUCacheStats)
#error "qKeepLRUCacheStats should normally be defined indirectly by StroikaConfig.h"
#endif




            /*
             * The LRUCacheDefaultTraits<> is a simple default traits implementation for building an LRUCache<>.
             */
            template    <typename   ELEMENT, typename KEY = ELEMENT>
            struct  LRUCacheDefaultTraits {
                typedef ELEMENT     ElementType;
                typedef KEY         KeyType;
                // HASHTABLESIZE must be >= 1, but if == 1, then Hash function not used
                DEFINE_CONSTEXPR_CONSTANT(uint8_t, HASH_TABLE_SIZE, 1);
                static  KeyType ExtractKey (const ElementType& e);
                // If KeyType different type than ElementType we need a hash for that too
                static  size_t  Hash (const KeyType& e);
                // defaults to using default CTOR for ElementType and copying over
                static  void    Clear (ElementType* element);
                // defaults to operator==
                static  bool    Equal (const KeyType& lhs, const KeyType& rhs);
            };




            /**
             *  A basic LRU (least recently used) cache mechanism. You provide a class type argument 'ELEMENT' defined roughly as follows:
             *
             *  TODO: THIS DOC IS OBSOLETE - PRE TRAITS IMPLEMENTAITON!!!
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
            */
            template    <typename   ELEMENT, typename TRAITS = LRUCacheDefaultTraits<ELEMENT> >
            class   LRUCache {
            public:
                typedef typename TRAITS::ElementType    ElementType;
                typedef typename TRAITS::KeyType        KeyType;

            public:
                LRUCache (size_t maxCacheSize);

            public:
                nonvirtual  size_t  GetMaxCacheSize () const;
                nonvirtual  void    SetMaxCacheSize (size_t maxCacheSize);

            public:
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

            public:
                struct  CacheIterator;
                nonvirtual  CacheIterator   begin ();
                nonvirtual  CacheIterator   end ();

            public:
                nonvirtual  void    ClearCache ();

            public:
                // NOTE - though you can CHANGE the value of ELEMENT, it is illegal to change its KEY part/key value if you specified HASH_TABLE_SIZE != 1
                // In TRAITS object.
                nonvirtual  ELEMENT*    AddNew (const KeyType& item);
                // NOTE - though you can CHANGE the value of ELEMENT, it is illegal to change its KEY part/key value if you specified HASH_TABLE_SIZE != 1
                // In TRAITS object.
                nonvirtual  ELEMENT*    LookupElement (const KeyType& item);

#if     qKeepLRUCacheStats
            public:
                size_t      fCachedCollected_Hits;
                size_t      fCachedCollected_Misses;
#endif

            private:
                vector<CacheElement_>   fCachedElts_BUF_[TRAITS::HASH_TABLE_SIZE];      // we don't directly use these, but use the First_Last pointers instead which are internal to this buf
                CacheElement_*          fCachedElts_First_[TRAITS::HASH_TABLE_SIZE];
                CacheElement_*          fCachedElts_fLast_[TRAITS::HASH_TABLE_SIZE];

            private:
                nonvirtual  void    ShuffleToHead_ (size_t chainIdx, CacheElement_* b);
            };


            /*
            @CLASS:         LRUCache<ELEMENT>::CacheIterator
            @DESCRIPTION:   <p>Used to iterate over elements of an @'LRUCache<ELEMENT>'</p>
                            <p>Please note that while an CacheIterator object exists for an LRUCache - it is not
                        safe to do other operations on the LRUCache - like @'LRUCache<ELEMENT>::LookupElement' or @'LRUCache<ELEMENT>::AddNew'.
                        </p>
            */
//TODO: Must update implementation to support BUCKETS (hashtable)
            template    <typename   ELEMENT, typename TRAITS>
            struct  LRUCache<ELEMENT, TRAITS>::CacheIterator {
                explicit CacheIterator (CacheElement_** start, CacheElement_** end);

                CacheElement_**  fCurV;
                CacheElement_**  fEndV;
                CacheElement_*   fCur;

                nonvirtual  CacheIterator& operator++ ();
                nonvirtual  ELEMENT& operator* ();
                nonvirtual  bool operator== (CacheIterator rhs);
                nonvirtual  bool operator!= (CacheIterator rhs);
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Cache_LRUCache_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "LRUCache.inl"
