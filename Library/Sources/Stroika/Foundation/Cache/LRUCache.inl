/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_LRUCache_inl_
#define _Stroika_Foundation_Cache_LRUCache_inl_    1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {


            /*
             ********************************************************************************
             ************************ LRUCacheSupport::Stats_Basic **************************
             ********************************************************************************
             */
            inline  void    LRUCacheSupport::Stats_Basic::IncrementHits ()
            {
                fCachedCollected_Hits++;
            }
            inline  void    LRUCacheSupport::Stats_Basic::IncrementMisses ()
            {
                fCachedCollected_Misses++;
            }


            /*
             ********************************************************************************
             ************************ LRUCacheSupport::Stats_Null **************************
             ********************************************************************************
             */
            inline  void    LRUCacheSupport::Stats_Null::IncrementHits ()
            {
            }
            inline  void    LRUCacheSupport::Stats_Null::IncrementMisses ()
            {
            }






            // MOVE TO NESTED SOON
            template    <typename   ELEMENT, typename TRAITS>
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


            public:
                /*
                @CLASS:         LRUCache<ELEMENT>::CacheIterator
                @DESCRIPTION:   <p>Used to iterate over elements of an @'LRUCache<ELEMENT>'</p>
                                <p>Please note that while an CacheIterator object exists for an LRUCache - it is not
                            safe to do other operations on the LRUCache - like @'LRUCache<ELEMENT>::LookupElement' or @'LRUCache<ELEMENT>::AddNew'.
                            </p>
                    //TODO: Must update implementation to support BUCKETS (hashtable)
                    //TODO: NOTE: UNSAFE ITERATION - UNLIKE the rest of Stroika (yes - SB fixed) - really an STL-style - not stroika style - iterator...
                */
                struct  CacheIterator {
                    explicit CacheIterator (CacheElement_** start, CacheElement_** end);

                public:
                    nonvirtual  CacheIterator& operator++ ();
                    nonvirtual  ELEMENT& operator* ();
                    nonvirtual  ELEMENT* operator-> ();
                    nonvirtual  bool operator== (CacheIterator rhs);
                    nonvirtual  bool operator!= (CacheIterator rhs);

                private:
                    CacheElement_**  fCurV;
                    CacheElement_**  fEndV;
                    CacheElement_*   fCur;
                };

            private:
                vector<CacheElement_>   fCachedElts_BUF_[TRAITS::HASH_TABLE_SIZE];      // we don't directly use these, but use the First_Last pointers instead which are internal to this buf
                CacheElement_*          fCachedElts_First_[TRAITS::HASH_TABLE_SIZE];
                CacheElement_*          fCachedElts_Last_[TRAITS::HASH_TABLE_SIZE];

            private:
                nonvirtual  void    ShuffleToHead_ (size_t chainIdx, CacheElement_* b);
            };



            /*
             ********************************************************************************
             ************** LRUCache<ELEMENT, TRAITS>::CacheElement_ ************************
             ********************************************************************************
             */
            template    <typename   ELEMENT, typename TRAITS>
            inline  LRUCache_<ELEMENT, TRAITS>::CacheElement_::CacheElement_ ()
                : fNext (nullptr)
                , fPrev (nullptr)
                , fElement ()
            {
            }


            /*
             ********************************************************************************
             *************** LRUCache_<ELEMENT,TRAITS>::CacheIterator ************************
             ********************************************************************************
             */
            template    <typename   ELEMENT, typename TRAITS>
            inline  LRUCache_<ELEMENT, TRAITS>::CacheIterator::CacheIterator (CacheElement_** start, CacheElement_** end)
                : fCurV (start)
                , fEndV (end)
                , fCur (start == end ? nullptr : *fCurV)
            {
            }
            template    <typename   ELEMENT, typename TRAITS>
            inline  typename    LRUCache_<ELEMENT, TRAITS>::CacheIterator&    LRUCache_<ELEMENT, TRAITS>::CacheIterator::operator++ ()
            {
                RequireNotNull (fCur);
                Require (fCurV != fEndV);
                fCur = fCur->fNext;
                if (fCur == nullptr) {
                    fCurV++;
                    if (fCurV != fEndV) {
                        fCur  = *fCurV;
                    }
                }
                return *this;
            }
            template    <typename   ELEMENT, typename TRAITS>
            inline  ELEMENT&    LRUCache_<ELEMENT, TRAITS>::CacheIterator::operator* ()
            {
                RequireNotNull (fCur);
                return fCur->fElement;
            }
            template    <typename   ELEMENT, typename TRAITS>
            inline  ELEMENT*    LRUCache_<ELEMENT, TRAITS>::CacheIterator::operator-> ()
            {
                RequireNotNull (fCur);
                return &fCur->fElement;
            }
            template    <typename   ELEMENT, typename TRAITS>
            inline  bool LRUCache_<ELEMENT, TRAITS>::CacheIterator::operator== (CacheIterator rhs)
            {
                return fCur == rhs.fCur;
            }
            template    <typename   ELEMENT, typename TRAITS>
            inline  bool LRUCache_<ELEMENT, TRAITS>::CacheIterator::operator!= (CacheIterator rhs)
            {
                return fCur != rhs.fCur;
            }


            /*
             ********************************************************************************
             ***************************** LRUCache_<ELEMENT,TRAITS> ************************
             ********************************************************************************
             */
            template    <typename   ELEMENT, typename TRAITS>
            LRUCache_<ELEMENT, TRAITS>::LRUCache_ (size_t maxCacheSize)
                : fStats ()
            {
                // TODO: Find more elegant but equally efficent way to initailize and say these are all initialized to zero
                // (INCLUDING fCachedElts_BUF_)
                (void)::memset (&fCachedElts_First_, 0, sizeof (fCachedElts_First_));
                (void)::memset (&fCachedElts_Last_, 0, sizeof (fCachedElts_Last_));

                SetMaxCacheSize (maxCacheSize);
            }
            template    <typename   ELEMENT, typename TRAITS>
            inline  size_t  LRUCache_<ELEMENT, TRAITS>::GetMaxCacheSize () const
            {
                return TRAITS::HASH_TABLE_SIZE * fCachedElts_BUF_[0].size ();
            }
            template    <typename   ELEMENT, typename TRAITS>
            void    LRUCache_<ELEMENT, TRAITS>::SetMaxCacheSize (size_t maxCacheSize)
            {
                Require (maxCacheSize >= 1);
                maxCacheSize =  ((maxCacheSize + TRAITS::HASH_TABLE_SIZE - 1) / TRAITS::HASH_TABLE_SIZE);   // divide size over number of hash chains
                maxCacheSize = max (maxCacheSize, static_cast<size_t> (1)); // must be at least one per chain
                for (size_t hi = 0; hi < TRAITS::HASH_TABLE_SIZE; hi++) {
                    if (maxCacheSize != fCachedElts_BUF_[hi].size ()) {
                        fCachedElts_BUF_[hi].resize (maxCacheSize);
                        // Initially link LRU together.
                        fCachedElts_First_[hi] = Containers::Start (fCachedElts_BUF_[hi]);
                        fCachedElts_Last_[hi] = fCachedElts_First_[hi] + maxCacheSize - 1;
                        fCachedElts_BUF_[hi][0].fPrev = nullptr;
                        for (size_t i = 0; i < maxCacheSize - 1; ++i) {
                            fCachedElts_BUF_[hi][i].fNext = fCachedElts_First_[hi] + (i + 1);
                            fCachedElts_BUF_[hi][i + 1].fPrev = fCachedElts_First_[hi] + (i);
                        }
                        fCachedElts_BUF_[hi][maxCacheSize - 1].fNext = nullptr;
                    }
                }
            }
            template    <typename   ELEMENT, typename TRAITS>
            inline  typename    LRUCache_<ELEMENT, TRAITS>::CacheIterator LRUCache_<ELEMENT, TRAITS>::begin ()
            {
                return CacheIterator (std::begin (fCachedElts_First_), std::end (fCachedElts_First_));
            }
            template    <typename   ELEMENT, typename TRAITS>
            inline  typename    LRUCache_<ELEMENT, TRAITS>::CacheIterator LRUCache_<ELEMENT, TRAITS>::end ()
            {
                return CacheIterator (nullptr, nullptr);
            }
            template    <typename   ELEMENT, typename TRAITS>
            inline  void    LRUCache_<ELEMENT, TRAITS>::ShuffleToHead_ (size_t chainIdx, CacheElement_* b)
            {
                Require (chainIdx < TRAITS::HASH_TABLE_SIZE);
                RequireNotNull (b);
                if (b == fCachedElts_First_[chainIdx]) {
                    Assert (b->fPrev == nullptr);
                    return; // already at head
                }
                CacheElement_*   prev    =   b->fPrev;
                AssertNotNull (prev);                   // don't call this if already at head
                // patch following and preceeding blocks to point to each other
                prev->fNext = b->fNext;
                if (b->fNext == nullptr) {
                    Assert (b == fCachedElts_Last_[chainIdx]);
                    fCachedElts_Last_[chainIdx] = b->fPrev;
                }
                else {
                    b->fNext->fPrev = prev;
                }

                // Now patch us into the head of the list
                CacheElement_*   oldFirst    =   fCachedElts_First_[chainIdx];
                AssertNotNull (oldFirst);
                b->fNext = oldFirst;
                oldFirst->fPrev = b;
                b->fPrev = nullptr;
                fCachedElts_First_[chainIdx] = b;

                Ensure (fCachedElts_Last_[chainIdx] != nullptr and fCachedElts_Last_[chainIdx]->fNext == nullptr);
                Ensure (fCachedElts_First_[chainIdx] != nullptr and fCachedElts_First_[chainIdx] == b and fCachedElts_First_[chainIdx]->fPrev == nullptr and fCachedElts_First_[chainIdx]->fNext != nullptr);
            }
            template    <typename   ELEMENT, typename TRAITS>
            inline  void    LRUCache_<ELEMENT, TRAITS>::ClearCache ()
            {
                for (size_t hi = 0; hi < TRAITS::HASH_TABLE_SIZE; hi++) {
                    for (CacheElement_* cur = fCachedElts_First_[hi]; cur != nullptr; cur = cur->fNext) {
                        TRAITS::Clear (&cur->fElement);
                    }
                }
            }
            template    <typename   ELEMENT, typename TRAITS>
            /*
            @METHOD:        LRUCache_<ELEMENT>::LookupElement
            @DESCRIPTION:   <p>Check and see if the given element is in the cache. Return that element if its tehre, and nullptr otherwise.
                        Note that this routine re-orders the cache so that the most recently looked up element is first, and because
                        of this re-ordering, its illegal to do a Lookup while a @'LRUCache_<ELEMENT>::CacheIterator' exists
                        for this LRUCache_.</p>
            */
            inline  ELEMENT*    LRUCache_<ELEMENT, TRAITS>::LookupElement (const KeyType& item)
            {
                size_t      chainIdx    =   TRAITS::Hash (item) % TRAITS::HASH_TABLE_SIZE;
                Assert (0 <= chainIdx and chainIdx < TRAITS::HASH_TABLE_SIZE);
                for (CacheElement_* cur = fCachedElts_First_[chainIdx]; cur != nullptr; cur = cur->fNext) {
                    if (TRAITS::Equal (TRAITS::ExtractKey (cur->fElement), item)) {
                        ShuffleToHead_ (chainIdx, cur);
                        fStats.IncrementHits ();
                        return &fCachedElts_First_[chainIdx]->fElement;
                    }
                }
                fStats.IncrementMisses ();
                return nullptr;
            }
            template    <typename   ELEMENT, typename TRAITS>
            /*
            @METHOD:        LRUCache_<ELEMENT>::AddNew
            @DESCRIPTION:   <p>Create a new LRUCache_ element (potentially bumping some old element out of the cache). This new element
                        will be considered most recently used. Note that this routine re-orders the cache so that the most recently looked
                        up element is first, and because of this re-ordering, its illegal to do a Lookup while
                        a @'LRUCache_<ELEMENT>::CacheIterator' exists for this LRUCache_.</p>
            */
            inline  ELEMENT*    LRUCache_<ELEMENT, TRAITS>::AddNew (const KeyType& item)
            {
                size_t      chainIdx    =   TRAITS::Hash (item) % TRAITS::HASH_TABLE_SIZE;
                Assert (0 <= chainIdx and chainIdx < TRAITS::HASH_TABLE_SIZE);
                ShuffleToHead_ (chainIdx, fCachedElts_Last_[chainIdx]);
                return &fCachedElts_First_[chainIdx]->fElement;
            }





            /*
             ********************************************************************************
             ************************** LRUCache<KEY, VALUE, TRAITS> ************************
             ********************************************************************************
             */
            template    <typename KEY, typename VALUE, typename TRAITS>
            LRUCache<KEY, VALUE, TRAITS>::LRUCache (size_t size)
                : fRealCache_ (size)
            {
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            LRUCache<KEY, VALUE, TRAITS>::LRUCache (const LRUCache& from)
                : fRealCache_ (1)
            {
                fRealCache_.SetMaxCacheSize (from.GetMaxCacheSize ());
                auto    critSec { Execution::make_unique_lock (from) };
                for (auto i : from.fRealCache_) {
                    Assert (i.fKey.IsMissing () == i.fValue.IsMissing ());
                    if (i.fKey) {
                        Add (*i.fKey, *i.fValue);
                    }
                }
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            auto    LRUCache<KEY, VALUE, TRAITS>::operator= (const LRUCache& rhs) -> const LRUCache&
            {
                if (this != &rhs) {
                    SetMaxCacheSize (rhs.GetMaxCacheSize ());
                    auto    critSec { Execution::make_unique_lock (rhs) };
                    fRealCache_.ClearCache ();
                    for (auto i : rhs.fRealCache_) {
                        if (i.fKey) {
                            Add (*i.fKey, *i.fValue);
                        }
                    }
                }
                return *this;
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            inline  size_t  LRUCache<KEY, VALUE, TRAITS>::GetMaxCacheSize () const
            {
                auto    critSec { Execution::make_unique_lock (*this) };
                return fRealCache_.GetMaxCacheSize ();
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            void    LRUCache<KEY, VALUE, TRAITS>::SetMaxCacheSize (size_t maxCacheSize)
            {
                auto    critSec { Execution::make_unique_lock (*this) };
                fRealCache_.SetMaxCacheSize (maxCacheSize);
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            inline  typename TRAITS::StatsType  LRUCache<KEY, VALUE, TRAITS>::GetStats () const
            {
                auto    critSec { Execution::make_unique_lock (*this) };
                return fRealCache_.fStats;
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            void    LRUCache<KEY, VALUE, TRAITS>::clear ()
            {
                auto    critSec { Execution::make_unique_lock (*this) };
                fRealCache_.ClearCache ();
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            void    LRUCache<KEY, VALUE, TRAITS>::clear (const KEY& key)
            {
                auto    critSec { Execution::make_unique_lock (*this) };
                LEGACYLRUCACHEOBJ_*  v   =   fRealCache_.LookupElement (key);
                if (v != nullptr) {
                    v->fKey.clear ();
                    v->fValue.clear ();
                }
                Ensure (not Lookup (key));
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            void    LRUCache<KEY, VALUE, TRAITS>::clear (function<bool(const KEY&)> clearPredicate)
            {
                auto    critSec { Execution::make_unique_lock (*this) };
                for (auto i = fRealCache_.begin (); i != fRealCache_.end (); ++i) {
                    if (i->fKey and clearPredicate (*i->fKey)) {
                        i->fKey.clear ();
                        i->fValue.clear ();
                    }
                }
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            auto     LRUCache<KEY, VALUE, TRAITS>::Lookup (const KEY& key) const -> OptionalValue
            {
                auto    critSec { Execution::make_unique_lock (*this) };
                LEGACYLRUCACHEOBJ_*  v   =   fRealCache_.LookupElement (key);
                if (v == nullptr) {
                    return OptionalValue ();
                }
                Ensure (TRAITS::Equals (key, *v->fKey));
                return *v->fValue;
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            void    LRUCache<KEY, VALUE, TRAITS>::Add (const KEY& key, const VALUE& value)
            {
                auto    critSec { Execution::make_unique_lock (*this) };
                LEGACYLRUCACHEOBJ_*  v   =   fRealCache_.AddNew (key);
                v->fKey = key;
                v->fValue = value;
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            Containers::Mapping<KEY, VALUE>     LRUCache<KEY, VALUE, TRAITS>::Elements () const
            {
                Containers::Mapping<KEY, VALUE>  result;
                auto    critSec { Execution::make_unique_lock (*this) };
                for (auto i : fRealCache_) {
                    if (i.fKey) {
                        result.Add (*i.fKey, *i.fValue);
                    }
                }
                return result;
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            VALUE   LRUCache<KEY, VALUE, TRAITS>::LookupValue (const KEY& key, const function<VALUE(KEY)>& valueFetcher)
            {
                OptionalValue v = Lookup (key);
                if (v.IsMissing ()) {
                    VALUE   newV = valueFetcher (key);
                    Add (key, newV);
                    return newV;
                }
                else {
                    return *v;
                }
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Cache_LRUCache_inl_*/
