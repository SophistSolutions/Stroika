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


            /*
             ********************************************************************************
             ********** LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheIterator **************
             ********************************************************************************
             */
            template    <typename KEY, typename VALUE, typename TRAITS>
            struct  LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheIterator {
                explicit CacheIterator (CacheElement_** start, CacheElement_** end)
                    : fCurV (start)
                    , fEndV (end)
                    , fCur (start == end ? nullptr : *fCurV)
                {
                }
                CacheIterator& operator++ ()
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
                ELEMENT& operator* ()
                {
                    RequireNotNull (fCur);
                    return fCur->fElement;
                }
                ELEMENT* operator-> ()
                {
                    RequireNotNull (fCur);
                    return &fCur->fElement;
                }
                bool operator== (CacheIterator rhs)
                {
                    return fCur == rhs.fCur;
                }
                bool operator!= (CacheIterator rhs)
                {
                    return fCur != rhs.fCur;
                }

            private:
                CacheElement_**  fCurV;
                CacheElement_**  fEndV;
                CacheElement_*   fCur;
            };


            /*
             ********************************************************************************
             ********** LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheIterator **************
             ********************************************************************************
             */
            template    <typename KEY, typename VALUE, typename TRAITS>
            struct  LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheElement_ {
                CacheElement_*      fNext      { nullptr };
                CacheElement_*      fPrev      { nullptr };
                LEGACYLRUCACHEOBJ_  fElement   {};
            };


            /*
             ********************************************************************************
             ***************************** LRUCache_<ELEMENT,TRAITS> ************************
             ********************************************************************************
             */
            template    <typename KEY, typename VALUE, typename TRAITS>
            LRUCache<KEY, VALUE, TRAITS>::LRUCache_::LRUCache_ (size_t maxCacheSize)
                : fStats ()
            {
                // TODO: Find more elegant but equally efficent way to initailize and say these are all initialized to zero
                // (INCLUDING fCachedElts_BUF_)
                (void)::memset (&fCachedElts_First_, 0, sizeof (fCachedElts_First_));
                (void)::memset (&fCachedElts_Last_, 0, sizeof (fCachedElts_Last_));

                SetMaxCacheSize (maxCacheSize);
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            inline  size_t  LRUCache<KEY, VALUE, TRAITS>::LRUCache_::GetMaxCacheSize () const
            {
                return TraitsType::kHashTableSize * fCachedElts_BUF_[0].size ();
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            void    LRUCache<KEY, VALUE, TRAITS>::LRUCache_::SetMaxCacheSize (size_t maxCacheSize)
            {
                Require (maxCacheSize >= 1);
                maxCacheSize =  ((maxCacheSize + TraitsType::kHashTableSize - 1) / TraitsType::kHashTableSize);   // divide size over number of hash chains
                maxCacheSize = max (maxCacheSize, static_cast<size_t> (1)); // must be at least one per chain
                for (size_t hi = 0; hi < TraitsType::kHashTableSize; hi++) {
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
            template    <typename KEY, typename VALUE, typename TRAITS>
            inline  auto LRUCache<KEY, VALUE, TRAITS>::LRUCache_::begin () -> CacheIterator {
                return CacheIterator (std::begin (fCachedElts_First_), std::end (fCachedElts_First_));
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            inline  typename    LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheIterator LRUCache<KEY, VALUE, TRAITS>::LRUCache_::end ()
            {
                return CacheIterator (nullptr, nullptr);
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            inline  void    LRUCache<KEY, VALUE, TRAITS>::LRUCache_::ShuffleToHead_ (size_t chainIdx, CacheElement_* b)
            {
                Require (chainIdx < TraitsType::kHashTableSize);
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
            template    <typename KEY, typename VALUE, typename TRAITS>
            inline  void    LRUCache<KEY, VALUE, TRAITS>::LRUCache_::ClearCache ()
            {
                for (size_t hi = 0; hi < TraitsType::kHashTableSize; hi++) {
                    for (CacheElement_* cur = fCachedElts_First_[hi]; cur != nullptr; cur = cur->fNext) {
                        LEGACYLRUCACHEOBJ_TRAITS_::Clear (&cur->fElement);
                    }
                }
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            /*
            @METHOD:        LRUCache_<ELEMENT>::LookupElement
            @DESCRIPTION:   <p>Check and see if the given element is in the cache. Return that element if its tehre, and nullptr otherwise.
                        Note that this routine re-orders the cache so that the most recently looked up element is first, and because
                        of this re-ordering, its illegal to do a Lookup while a @'LRUCache_<ELEMENT>::CacheIterator' exists
                        for this LRUCache_.</p>
            */
            inline  auto LRUCache<KEY, VALUE, TRAITS>::LRUCache_::LookupElement (const KeyType& item) -> ELEMENT* {
                size_t      chainIdx    =   LEGACYLRUCACHEOBJ_TRAITS_::Hash (item) % TraitsType::kHashTableSize;
                Assert (0 <= chainIdx and chainIdx < TraitsType::kHashTableSize);
                for (CacheElement_* cur = fCachedElts_First_[chainIdx]; cur != nullptr; cur = cur->fNext)
                {
                    if (LEGACYLRUCACHEOBJ_TRAITS_::Equal (LEGACYLRUCACHEOBJ_TRAITS_::ExtractKey (cur->fElement), item)) {
                        ShuffleToHead_ (chainIdx, cur);
                        fStats.IncrementHits ();
                        return &fCachedElts_First_[chainIdx]->fElement;
                    }
                }
                fStats.IncrementMisses ();
                return nullptr;
            }
            template    <typename KEY, typename VALUE, typename TRAITS>
            /*
            @METHOD:        LRUCache_<ELEMENT>::AddNew
            @DESCRIPTION:   <p>Create a new LRUCache_ element (potentially bumping some old element out of the cache). This new element
                        will be considered most recently used. Note that this routine re-orders the cache so that the most recently looked
                        up element is first, and because of this re-ordering, its illegal to do a Lookup while
                        a @'LRUCache_<ELEMENT>::CacheIterator' exists for this LRUCache_.</p>
            */
            inline  auto LRUCache<KEY, VALUE, TRAITS>::LRUCache_::AddNew (const KeyType& item) -> ELEMENT* {
                size_t      chainIdx    =   TRAITS::Hash (item) % TraitsType::kHashTableSize;
                Assert (0 <= chainIdx and chainIdx < TraitsType::kHashTableSize);
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
                Ensure (TRAITS::KeyEqualsCompareFunctionType::Equals (key, *v->fKey));
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
            auto     LRUCache<KEY, VALUE, TRAITS>::Elements () const -> Containers::Mapping<KEY, VALUE, Containers::Mapping_DefaultTraits<KEY, VALUE, KeyEqualsCompareFunctionType>> {
                Containers::Mapping<KEY, VALUE, Containers::Mapping_DefaultTraits<KEY, VALUE, KeyEqualsCompareFunctionType>>  result;
                auto    critSec { Execution::make_unique_lock (*this) };
                for (auto i : fRealCache_)
                {
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
