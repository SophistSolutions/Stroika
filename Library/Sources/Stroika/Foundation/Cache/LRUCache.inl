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
     *** LRUCacheSupport::DefaultTraits<KEY, HASH_TABLE_SIZE, KEY_EQUALS_COMPARER> **
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, size_t HASH_TABLE_SIZE, typename KEY_EQUALS_COMPARER, typename STATS_TYPE>
    template <typename SFINAE>
    size_t LRUCacheSupport::DefaultTraits<KEY, VALUE, HASH_TABLE_SIZE, KEY_EQUALS_COMPARER, STATS_TYPE>::Hash_SFINAE_ (typename Configuration::ArgByValueType<KEY> e, enable_if_t<is_arithmetic_v<SFINAE> or is_convertible_v<SFINAE, string> or is_convertible_v<SFINAE, Characters::String>, void>*)
    {
        using Cryptography::Digest::Digester;
        using Cryptography::Digest::Algorithm::Jenkins;
        using USE_DIGESTER_ = Digester<Jenkins>;
        return Cryptography::Hash<USE_DIGESTER_, KEY, size_t> (e);
    }
    template <typename KEY, typename VALUE, size_t HASH_TABLE_SIZE, typename KEY_EQUALS_COMPARER, typename STATS_TYPE>
    template <typename SFINAE>
    inline size_t LRUCacheSupport::DefaultTraits<KEY, VALUE, HASH_TABLE_SIZE, KEY_EQUALS_COMPARER, STATS_TYPE>::Hash_SFINAE_ ([[maybe_unused]] typename Configuration::ArgByValueType<KEY> e, enable_if_t<not(is_arithmetic_v<SFINAE> or is_convertible_v<SFINAE, string> or is_convertible_v<SFINAE, Characters::String>), void>*)
    {
        return 0;
    }
    template <typename KEY, typename VALUE, size_t HASH_TABLE_SIZE, typename KEY_EQUALS_COMPARER, typename STATS_TYPE>
    inline size_t LRUCacheSupport::DefaultTraits<KEY, VALUE, HASH_TABLE_SIZE, KEY_EQUALS_COMPARER, STATS_TYPE>::Hash (typename Configuration::ArgByValueType<KEY> e)
    {
        return Hash_SFINAE_<KEY> (e);
    }

    /*
     ********************************************************************************
     ********** LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheIterator **************
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename TRAITS>
    struct LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheIterator {
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
                    fCur = *fCurV;
                }
            }
            return *this;
        }
        OptKeyValuePair_& operator* ()
        {
            RequireNotNull (fCur);
            return fCur->fElement;
        }
        OptKeyValuePair_* operator-> ()
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
        CacheElement_** fCurV;
        CacheElement_** fEndV;
        CacheElement_*  fCur;
    };

    /*
     ********************************************************************************
     ********** LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheIterator **************
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename TRAITS>
    struct LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheElement_ {
        CacheElement_*   fNext{nullptr};
        CacheElement_*   fPrev{nullptr};
        OptKeyValuePair_ fElement{};
    };

    /*
     ********************************************************************************
     ***************************** LRUCache_<ELEMENT,TRAITS> ************************
     ********************************************************************************
     */
    DISABLE_COMPILER_MSC_WARNING_START (4351)
    template <typename KEY, typename VALUE, typename TRAITS>
    LRUCache<KEY, VALUE, TRAITS>::LRUCache_::LRUCache_ (size_t maxCacheSize)
    {
        SetMaxCacheSize (maxCacheSize);
    }
    DISABLE_COMPILER_MSC_WARNING_END (4351)
    template <typename KEY, typename VALUE, typename TRAITS>
    inline size_t LRUCache<KEY, VALUE, TRAITS>::LRUCache_::GetMaxCacheSize () const
    {
        return TraitsType::kHashTableSize * fCachedElts_BUF_[0].size ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::LRUCache_::SetMaxCacheSize (size_t maxCacheSize)
    {
        Require (maxCacheSize >= 1);
        maxCacheSize = ((maxCacheSize + TraitsType::kHashTableSize - 1) / TraitsType::kHashTableSize); // divide size over number of hash chains
        maxCacheSize = max (maxCacheSize, static_cast<size_t> (1));                                    // must be at least one per chain
        for (size_t hi = 0; hi < TraitsType::kHashTableSize; hi++) {
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
    template <typename KEY, typename VALUE, typename TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::LRUCache_::begin () -> CacheIterator
    {
        return CacheIterator (std::begin (fCachedElts_First_), std::end (fCachedElts_First_));
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline typename LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheIterator LRUCache<KEY, VALUE, TRAITS>::LRUCache_::end ()
    {
        return CacheIterator (nullptr, nullptr);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void LRUCache<KEY, VALUE, TRAITS>::LRUCache_::ShuffleToHead_ (size_t chainIdx, CacheElement_* b)
    {
        Require (chainIdx < TraitsType::kHashTableSize);
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
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void LRUCache<KEY, VALUE, TRAITS>::LRUCache_::ClearCache ()
    {
        for (size_t hi = 0; hi < TraitsType::kHashTableSize; hi++) {
            for (CacheElement_* cur = fCachedElts_First_[hi]; cur != nullptr; cur = cur->fNext) {
                cur->fElement = nullopt;
            }
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::LRUCache_::LookupElement (typename Configuration::ArgByValueType<KeyType> item) -> OptKeyValuePair_*
    {
        size_t chainIdx = Hash_ (item) % TraitsType::kHashTableSize;
        Assert (0 <= chainIdx and chainIdx < TraitsType::kHashTableSize);
        for (CacheElement_* cur = fCachedElts_First_[chainIdx]; cur != nullptr; cur = cur->fNext) {
            if (cur->fElement and fKeyEqualsComparer_ (cur->fElement->fKey, item)) {
                ShuffleToHead_ (chainIdx, cur);
                this->IncrementHits ();
                return &fCachedElts_First_[chainIdx]->fElement;
            }
        }
        this->IncrementMisses ();
        return nullptr;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::LRUCache_::AddNew (typename Configuration::ArgByValueType<KeyType> item) -> OptKeyValuePair_*
    {
        size_t chainIdx = TRAITS::Hash (item) % TraitsType::kHashTableSize;
        Assert (0 <= chainIdx and chainIdx < TraitsType::kHashTableSize);
        ShuffleToHead_ (chainIdx, fCachedElts_Last_[chainIdx]);
        return &fCachedElts_First_[chainIdx]->fElement;
    }

    /*
     ********************************************************************************
     ************************** LRUCache<KEY, VALUE, TRAITS> ************************
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename TRAITS>
    inline LRUCache<KEY, VALUE, TRAITS>::LRUCache (size_t size, const KeyEqualsCompareFunctionType& keyEqualsComparer)
        : fRealCache_ (size)
    {
        fRealCache_.fKeyEqualsComparer_ = keyEqualsComparer;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    LRUCache<KEY, VALUE, TRAITS>::LRUCache (const LRUCache& from)
        : fRealCache_ (1)
    {
        fRealCache_.fKeyEqualsComparer_ = from.fRealCache_.fKeyEqualsComparer_;
        fRealCache_.SetMaxCacheSize (from.GetMaxCacheSize ());
        lock_guard<const Debug::AssertExternallySynchronizedLock> fromCritSec{from}; // after above getMaxCacheSize to avoid recursive access
        for (auto i : from.fRealCache_) {
            if (i) {
                Add (i->fKey, i->fValue);
            }
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    auto LRUCache<KEY, VALUE, TRAITS>::operator= (const LRUCache& rhs) -> const LRUCache&
    {
        if (this != &rhs) {
            SetMaxCacheSize (rhs.GetMaxCacheSize ());
            lock_guard<AssertExternallySynchronizedLock> critSec{*this}; // after above SetMaxCacheSize to avoid recursive access
            fRealCache_.ClearCache ();
            for (auto i : rhs.fRealCache_) {
                if (i.fKey) {
                    Add (*i.fKey, *i.fValue);
                }
            }
        }
        return *this;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline size_t LRUCache<KEY, VALUE, TRAITS>::GetMaxCacheSize () const
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        return fRealCache_.GetMaxCacheSize ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::SetMaxCacheSize (size_t maxCacheSize)
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this};
        fRealCache_.SetMaxCacheSize (maxCacheSize);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline typename TRAITS::StatsType LRUCache<KEY, VALUE, TRAITS>::GetStats () const
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        return fRealCache_;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::clear ()
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this};
        fRealCache_.ClearCache ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::clear (typename Configuration::ArgByValueType<KEY> key)
    {
        {
            // Scope for assert because AssertExternallySynchronizedLock is not recursive
            lock_guard<AssertExternallySynchronizedLock> critSec{*this};
            OptKeyValuePair_*                            v = fRealCache_.LookupElement (key);
            if (v != nullptr) {
                v->clear ();
            }
        }
        Ensure (not Lookup (key));
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::clear (function<bool(typename Configuration::ArgByValueType<KEY>)> clearPredicate)
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this};
        for (auto i = fRealCache_.begin (); i != fRealCache_.end (); ++i) {
            if (i->has_value () and clearPredicate ((*i)->fKey)) {
                *i = nullopt;
            }
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    auto LRUCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key) const -> OptionalValueType
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        OptKeyValuePair_*                                         v = fRealCache_.LookupElement (key);
        if (v == nullptr) {
            return OptionalValueType ();
        }
        Ensure (fRealCache_.fKeyEqualsComparer_ (key, (*v)->fKey));
        return (*v)->fValue;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> value)
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this};
        OptKeyValuePair_*                            v = fRealCache_.AddNew (key);
        *v                                             = KeyValuePair_{key, value};
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    template <typename SFINAE>
    inline size_t LRUCache<KEY, VALUE, TRAITS>::H_ (typename Configuration::ArgByValueType<SFINAE> k)
    {
        return TRAITS::Hash (k);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline size_t LRUCache<KEY, VALUE, TRAITS>::Hash_ ([[maybe_unused]] typename Configuration::ArgByValueType<KEY> e)
    {
        static_assert (TraitsType::kHashTableSize >= 1, "TraitsType::kHashTableSize >= 1");
        if constexpr (TRAITS::kHashTableSize == 1) {
            return 0; // avoid referencing hash function
        }
        else {
            // use template indirection so we dont force compiling this if its not needed cuz TRAITS::kHashTableSize == 1
            // may not work, but trying...
            return H_ (e);
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    auto LRUCache<KEY, VALUE, TRAITS>::Elements () const -> Containers::Mapping<KEY, VALUE>
    {
        lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{*this};
        Containers::Mapping<KEY, VALUE>                           result;
        for (auto i : fRealCache_) {
            if (i) {
                result.Add (i->fKey, i->fValue);
            }
        }
        return result;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    VALUE LRUCache<KEY, VALUE, TRAITS>::LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& valueFetcher)
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

}

#endif /*_Stroika_Foundation_Cache_LRUCache_inl_*/
