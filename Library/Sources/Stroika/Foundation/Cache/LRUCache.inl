/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_LRUCache_inl_
#define _Stroika_Foundation_Cache_LRUCache_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Containers/Common.h"
#include "../Cryptography/Digest/Algorithm/Jenkins.h"
#include "../Cryptography/Hash.h"
#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     * LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::LRUCache_::CacheIterator_ *
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    struct LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::CacheIterator_ {
        explicit CacheIterator_ (CacheElement_** start, CacheElement_** end)
            : fCurV (start)
            , fEndV (end)
            , fCur (start == end ? nullptr : *fCurV)
        {
        }
        CacheIterator_& operator++ ()
        {
            RequireNotNull (fCur);
            Require (fCurV != fEndV);
            fCur = fCur->fNext;
            if (fCur == nullptr) {
                fCurV++;
                if (fCurV != fEndV) {
                    fCur = *fCurV;
                }
            }
            return *this;
        }
        optional<KeyValuePair_>& operator* ()
        {
            RequireNotNull (fCur);
            return fCur->fElement;
        }
        optional<KeyValuePair_>* operator-> ()
        {
            RequireNotNull (fCur);
            return &fCur->fElement;
        }
        bool operator== (CacheIterator_ rhs)
        {
            return fCur == rhs.fCur;
        }
        bool operator!= (CacheIterator_ rhs)
        {
            return fCur != rhs.fCur;
        }

    private:
        CacheElement_** fCurV;
        CacheElement_** fEndV;
        CacheElement_*  fCur;
    };

    /*
     ********************************************************************************
     * LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::CacheIterator_ *
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    struct LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::CacheElement_ {
        CacheElement_*          fNext{nullptr};
        CacheElement_*          fPrev{nullptr};
        optional<KeyValuePair_> fElement{};
    };

    /*
     ********************************************************************************
     ** LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>> ***
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::LRUCache (size_t maxCacheSize, const KeyEqualsCompareFunctionType& keyEqualsComparer, size_t hashTableSize, KEY_HASH_FUNCTION hashFunction)
        : fHashtableSize_{hashTableSize}
        , fKeyEqualsComparer_ (keyEqualsComparer)
        , fHashFunction_ (hashFunction)
        , fCachedElts_BUF_{hashTableSize}
        , fCachedElts_First_{hashTableSize}
        , fCachedElts_Last_{hashTableSize}
    {
        SetMaxCacheSize (maxCacheSize);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::LRUCache (pair<KEY, VALUE> ignored, size_t maxCacheSize, const KeyEqualsCompareFunctionType& keyEqualsComparer, size_t hashTableSize, KEY_HASH_FUNCTION hashFunction)
        : LRUCache (maxCacheSize, keyEqualsComparer, hashTableSize, hashFunction)
    {
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::LRUCache (size_t maxCacheSize, size_t hashTableSize, KEY_HASH_FUNCTION hashFunction)
        : LRUCache (maxCacheSize, KEY_EQUALS_COMPARER{}, hashTableSize, hashFunction)
    {
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::LRUCache (pair<KEY, VALUE> ignored, size_t maxCacheSize, size_t hashTableSize, KEY_HASH_FUNCTION hashFunction)
        : LRUCache (maxCacheSize, hashTableSize, hashFunction)
    {
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::LRUCache (const LRUCache& from)
        : LRUCache (from.GetMaxCacheSize (), from.fKeyEqualsComparer_, from.fHashtableSize_, from.fHashFunction_)
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> fromCritSec{from}; // after above getMaxCacheSize to avoid recursive access
        for (CacheIterator_ i = from.begin_ (); i != from.end_ (); ++i) {
            if (*i) {
                Add ((*i)->fKey, (*i)->fValue);
            }
        }
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    auto LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::operator= (const LRUCache& rhs) -> const LRUCache&
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this}; // after above SetMaxCacheSize to avoid recursive access
        if (this != &rhs) {
            SetMaxCacheSize (rhs.GetMaxCacheSize ());
            ClearCache_ ();
            for (auto i : rhs) {
                if (i.fKey) {
                    Add (*i.fKey, *i.fValue);
                }
            }
        }
        return *this;
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline size_t LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::GetMaxCacheSize () const
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        return fHashtableSize_ * fCachedElts_BUF_[0].size ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    void LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::SetMaxCacheSize (size_t maxCacheSize)
    {
        Require (maxCacheSize >= 1);
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        maxCacheSize = ((maxCacheSize + fHashtableSize_ - 1) / fHashtableSize_); // divide size over number of hash chains
        maxCacheSize = max (maxCacheSize, static_cast<size_t> (1));              // must be at least one per chain
        for (size_t hi = 0; hi < fHashtableSize_; hi++) {
            if (maxCacheSize != fCachedElts_BUF_[hi].size ()) {
                fCachedElts_BUF_[hi].resize (maxCacheSize);
                // Initially link LRU together.
                fCachedElts_First_[hi]        = Containers::Start (fCachedElts_BUF_[hi]);
                fCachedElts_Last_[hi]         = fCachedElts_First_[hi] + maxCacheSize - 1;
                fCachedElts_BUF_[hi][0].fPrev = nullptr;
                for (size_t i = 0; i < maxCacheSize - 1; ++i) {
                    fCachedElts_BUF_[hi][i].fNext     = fCachedElts_First_[hi] + (i + 1);
                    fCachedElts_BUF_[hi][i + 1].fPrev = fCachedElts_First_[hi] + (i);
                }
                fCachedElts_BUF_[hi][maxCacheSize - 1].fNext = nullptr;
            }
        }
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline auto LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::GetStats () const -> StatsType
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        return *this;
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    void LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::clear ()
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this};
        ClearCache_ ();
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    void LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::clear (typename Configuration::ArgByValueType<KEY> key)
    {
        {
            // Scope for assert because AssertExternallySynchronizedLock is not recursive
            lock_guard<AssertExternallySynchronizedLock> critSec{*this};
            optional<KeyValuePair_>*                     v = LookupElement_ (key);
            if (v != nullptr) {
                v->clear ();
            }
        }
        Ensure (not Lookup (key));
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    void LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::clear (function<bool(typename Configuration::ArgByValueType<KEY>)> clearPredicate)
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this};
        for (auto i = begin_ (); i != end_ (); ++i) {
            if (i->has_value () and clearPredicate ((*i)->fKey)) {
                *i = nullopt;
            }
        }
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    auto LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::Lookup (typename Configuration::ArgByValueType<KEY> key) const -> optional<VALUE>
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        optional<KeyValuePair_>*                                  v = LookupElement_ (key);
        if (v == nullptr) {
            return optional<VALUE>{};
        }
        Ensure (fKeyEqualsComparer_ (key, (*v)->fKey));
        return (*v)->fValue;
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    void LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> value)
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this};
        optional<KeyValuePair_>*                     v = AddNew_ (key);
        *v                                             = KeyValuePair_{key, value};
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline size_t LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::H_ ([[maybe_unused]] typename Configuration::ArgByValueType<KEY> k) const
    {
        Assert (fHashtableSize_ >= 1);
        if constexpr (is_same_v<KEY_HASH_FUNCTION, nullptr_t>) {
            return 0; // avoid referencing hash function
        }
        else {
            return fHashFunction_ (k) % fHashtableSize_;
        }
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    auto LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::Elements () const -> Containers::Mapping<KEY, VALUE>
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        Containers::Mapping<KEY, VALUE>                           result;
        for (CacheIterator_ i = begin_ (); i != end_ (); ++i) {
            if (*i) {
                result.Add ((*i)->fKey, (*i)->fValue);
            }
        }
        return result;
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    VALUE LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& valueFetcher)
    {
        auto v = Lookup (key);
        if (v.has_value ()) {
            return *v;
        }
        else {
            VALUE newV = valueFetcher (key);
            Add (key, newV);
            return newV;
        }
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline auto LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::begin_ () const -> CacheIterator_
    {
        LRUCache* ncThis = const_cast<LRUCache*> (this);
        return CacheIterator_ (std::begin (ncThis->fCachedElts_First_), std::end (ncThis->fCachedElts_First_));
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline typename LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::CacheIterator_ LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::end_ () const
    {
        return CacheIterator_ (nullptr, nullptr);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>

    inline void LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::ShuffleToHead_ (size_t chainIdx, CacheElement_* b)
    {
        Require (chainIdx < fHashtableSize_);
        RequireNotNull (b);
        if (b == fCachedElts_First_[chainIdx]) {
            Assert (b->fPrev == nullptr);
            return; // already at head
        }
        CacheElement_* prev = b->fPrev;
        AssertNotNull (prev); // don't call this if already at head
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
        CacheElement_* oldFirst = fCachedElts_First_[chainIdx];
        AssertNotNull (oldFirst);
        b->fNext                     = oldFirst;
        oldFirst->fPrev              = b;
        b->fPrev                     = nullptr;
        fCachedElts_First_[chainIdx] = b;

        Ensure (fCachedElts_Last_[chainIdx] != nullptr and fCachedElts_Last_[chainIdx]->fNext == nullptr);
        Ensure (fCachedElts_First_[chainIdx] != nullptr and fCachedElts_First_[chainIdx] == b and fCachedElts_First_[chainIdx]->fPrev == nullptr and fCachedElts_First_[chainIdx]->fNext != nullptr);
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline void LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::ClearCache_ ()
    {
        for (size_t hi = 0; hi < fHashtableSize_; hi++) {
            for (CacheElement_* cur = fCachedElts_First_[hi]; cur != nullptr; cur = cur->fNext) {
                cur->fElement = nullopt;
            }
        }
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline auto LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::LookupElement_ (typename Configuration::ArgByValueType<KeyType> item) const -> optional<KeyValuePair_>*
    {
        size_t chainIdx = H_ (item);
        Assert (0 <= chainIdx and chainIdx < fHashtableSize_);
        for (CacheElement_* cur = fCachedElts_First_[chainIdx]; cur != nullptr; cur = cur->fNext) {
            if (cur->fElement and fKeyEqualsComparer_ (cur->fElement->fKey, item)) {
                const_cast<LRUCache*> (this)->ShuffleToHead_ (chainIdx, cur);
                const_cast<LRUCache*> (this)->IncrementHits ();
                return &fCachedElts_First_[chainIdx]->fElement;
            }
        }
        const_cast<LRUCache*> (this)->IncrementMisses ();
        return nullptr;
    }
    template <typename KEY, typename VALUE, typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION, typename STATS_TYPE>
    inline auto LRUCache<KEY, VALUE, KEY_EQUALS_COMPARER, KEY_HASH_FUNCTION, STATS_TYPE>::AddNew_ (typename Configuration::ArgByValueType<KeyType> item) -> optional<KeyValuePair_>*
    {
        size_t chainIdx = H_ (item);
        Assert (0 <= chainIdx and chainIdx < fHashtableSize_);
        ShuffleToHead_ (chainIdx, fCachedElts_Last_[chainIdx]);
        return &fCachedElts_First_[chainIdx]->fElement;
    }

}

#endif /*_Stroika_Foundation_Cache_LRUCache_inl_*/
