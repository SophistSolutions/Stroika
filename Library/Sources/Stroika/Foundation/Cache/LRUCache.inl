/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Cryptography/Digest/Algorithm/Jenkins.h"
#include "Stroika/Foundation/Cryptography/Digest/Hash.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ************ LRUCache<KEY, VALUE, TRAITS>::LRUCache_::CacheIterator_ ***********
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    struct LRUCache<KEY, VALUE, TRAITS>::CacheIterator_ {
        explicit CacheIterator_ (CacheElement_** start, CacheElement_** end)
            : fCurV{start}
            , fEndV{end}
            , fCur{start == end ? nullptr : *fCurV}
        {
        }
        CacheIterator_& operator++ ()
        {
            RequireNotNull (fCur);
            Require (fCurV != fEndV);
            fCur = fCur->fNext;
            if (fCur == nullptr) {
                ++fCurV;
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
        optional<KeyValuePair_>* operator->()
        {
            RequireNotNull (fCur);
            return &fCur->fElement;
        }
        bool operator== (const CacheIterator_& rhs) const
        {
            return fCur == rhs.fCur;
        }

    private:
        CacheElement_** fCurV;
        CacheElement_** fEndV;
        CacheElement_*  fCur;
    };

    /*
     ********************************************************************************
     ************* LRUCache<KEY, VALUE, TRAITS>::CacheIterator_ *********************
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    struct LRUCache<KEY, VALUE, TRAITS>::CacheElement_ {
        CacheElement_*          fNext{nullptr};
        CacheElement_*          fPrev{nullptr};
        optional<KeyValuePair_> fElement{};
    };

    /*
     ********************************************************************************
     ********************** LRUCache<KEY, VALUE, TRAITS>> ***************************
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline LRUCache<KEY, VALUE, TRAITS>::LRUCache ()
        requires (same_as<typename TRAITS::KeyHashFunctionType, nullptr_t> and same_as<typename TRAITS::KeyEqualsCompareFunctionType, equal_to<KEY>>)
        : LRUCache{1, equal_to<KEY>{}}
    {
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline LRUCache<KEY, VALUE, TRAITS>::LRUCache (size_t maxCacheSize, const typename TRAITS::KeyEqualsCompareFunctionType& keyEqualsComparer)
        requires (same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>)
        : fHashtableSize_{1}
        , fKeyEqualsComparer_{keyEqualsComparer}
        , fHashFunction_{nullptr}
        , fCachedElts_BUF_{1}
        , fCachedElts_First_{Memory::eUninitialized, 1}
        , fCachedElts_Last_{Memory::eUninitialized, 1}
    {
        Require (maxCacheSize >= 1);
        SetMaxCacheSize (maxCacheSize);
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline LRUCache<KEY, VALUE, TRAITS>::LRUCache (size_t maxCacheSize, const typename TRAITS::KeyEqualsCompareFunctionType& keyEqualsComparer,
                                                   size_t hashTableSize, const typename TRAITS::KeyHashFunctionType& hashFunction)
        requires (not same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>)
        : fHashtableSize_{hashTableSize}
        , fKeyEqualsComparer_{keyEqualsComparer}
        , fHashFunction_{hashFunction}
        , fCachedElts_BUF_{hashTableSize}
        , fCachedElts_First_{Memory::eUninitialized, hashTableSize}
        , fCachedElts_Last_{Memory::eUninitialized, hashTableSize}
    {
        Require (hashTableSize >= 1);
        WeakAssert (maxCacheSize >= hashTableSize); // plausibly a bug if violated, but no biggie since SetMaxCacheSize() adjusts
        SetMaxCacheSize (maxCacheSize);
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline LRUCache<KEY, VALUE, TRAITS>::LRUCache (size_t maxCacheSize, size_t hashTableSize, const typename TRAITS::KeyHashFunctionType& hashFunction)
        requires (not same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>)
        : LRUCache{maxCacheSize, typename TRAITS::KeyEqualsCompareFunctionType{}, hashTableSize, hashFunction}
    {
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline LRUCache<KEY, VALUE, TRAITS>::LRUCache (LRUCache&& from)
        // This is really the same as a copy, because moving is hard. This data structure contains lots of internal pointers.
        // @todo it would make sense to do a move here. Much of the memory could be just shuffled over in many cases - but
        // all the internal pointers would need to be patched. NOTE - important to not wrap from in move() for forward, cuz we want the lvalue version
        : LRUCache{from}
    {
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    LRUCache<KEY, VALUE, TRAITS>::LRUCache (const LRUCache& from)
        requires (same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>)
        : LRUCache{from.GetMaxCacheSize (), from.GetKeyEqualsCompareFunction ()}
    {
        shared_lock lockSrc{from.fMaybeMutex_}; // no need to lock THIS guy cuz CTOR, nobody could have reference to it yet
        for (CacheIterator_ i = from.begin_ (); i != from.end_ (); ++i) {
            if (*i) {
                Add_ ((*i)->fKey, (*i)->fValue);
            }
        }
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    LRUCache<KEY, VALUE, TRAITS>::LRUCache (const LRUCache& from)
        requires (not same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>)
        : LRUCache{from.GetMaxCacheSize (), from.GetKeyEqualsCompareFunction (), from.GetHashTableSize (), from.GetKeyHashFunction ()}
    {
        shared_lock lockSrc{from.fMaybeMutex_}; // no need to lock THIS guy cuz CTOR, nobody could have reference to it yet
        for (CacheIterator_ i = from.begin_ (); i != from.end_ (); ++i) {
            if (*i) {
                Add_ ((*i)->fKey, (*i)->fValue);
            }
        }
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::operator= (LRUCache&& rhs) noexcept -> LRUCache&
    {
        IgnoreExceptionsForCall (return operator= (rhs)); //  same as assign, cuz hard to move - see move constructor
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    auto LRUCache<KEY, VALUE, TRAITS>::operator= (const LRUCache& rhs) -> LRUCache&
    {
        if (this != &rhs) {
            //shared_lock lockSrc{rhs.fMaybeMutex_};    -- lock should not be needed cuz rhs.Elements() locks and makes a copy
            SetMaxCacheSize (rhs.GetMaxCacheSize ());
            scoped_lock critSec{fMaybeMutex_};
            ClearCache_ ();
            for (const auto& i : rhs.Elements ()) {
                if (i.fKey) {
                    Add_ (*i.fKey, *i.fValue);
                }
            }
        }
        return *this;
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline size_t LRUCache<KEY, VALUE, TRAITS>::GetMaxCacheSize () const
    {
        shared_lock critSec{fMaybeMutex_};
        return fHashtableSize_ * fCachedElts_BUF_[0].size ();
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::SetMaxCacheSize (size_t maxCacheSize)
    {
        Require (maxCacheSize >= 1);
        scoped_lock critSec{fMaybeMutex_};
        maxCacheSize = ((maxCacheSize + fHashtableSize_ - 1) / fHashtableSize_); // divide size over number of hash chains
        maxCacheSize = max (maxCacheSize, static_cast<size_t> (1));              // must be at least one per chain
        for (size_t hi = 0; hi < fHashtableSize_; ++hi) {
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
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::GetKeyEqualsCompareFunction () const -> KeyEqualsCompareFunctionType
    {
        shared_lock critSec{fMaybeMutex_};
        return fKeyEqualsComparer_;
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::GetStats () const -> StatsType
    {
        shared_lock critSec{fMaybeMutex_};
        return fStats_;
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::GetHashTableSize () const -> size_t
    {
        shared_lock critSec{fMaybeMutex_};
        if constexpr (same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>) {
            return 1;
        }
        else {
            return fHashtableSize_;
        }
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::GetKeyHashFunction () const -> typename TRAITS::KeyHashFunctionType
    {
        shared_lock critSec{fMaybeMutex_};
        return fHashFunction_;
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::clear ()
    {
        scoped_lock critSec{fMaybeMutex_};
        ClearCache_ ();
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::clear (typename Common::ArgByValueType<KEY> key)
    {
        scoped_lock              critSec{fMaybeMutex_};
        optional<KeyValuePair_>* v = LookupElement_ (key);
        if (v != nullptr) {
            v->clear ();
        }
        Ensure (not Lookup (key));
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::clear (function<bool (typename Common::ArgByValueType<KEY>)> clearPredicate)
    {
        scoped_lock critSec{fMaybeMutex_};
        for (auto i = begin_ (); i != end_ (); ++i) {
            if (i->has_value () and clearPredicate ((*i)->fKey)) {
                *i = nullopt;
            }
        }
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (predicate<KEY>) PREDICATE>
    void LRUCache<KEY, VALUE, TRAITS>::RemoveAll (PREDICATE&& removeIfReturnsTrue)
    {
        scoped_lock critSec{fMaybeMutex_};
        for (auto i = begin_ (); i != end_ (); ++i) {
            if (i->has_value () and forward<PREDICATE> (removeIfReturnsTrue) ((*i)->fKey)) {
                *i = nullopt;
            }
        }
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename V>
        requires (not IValuelessCache<V>)
    auto LRUCache<KEY, VALUE, TRAITS>::Lookup (typename Common::ArgByValueType<KEY> key) -> optional<V>
    {
        scoped_lock              critSec{fMaybeMutex_}; // subtle - WRITE cuz updates LRU
        optional<KeyValuePair_>* v = LookupElement_ (key);
        if (v == nullptr) {
            return optional<VALUE>{};
        }
        Ensure (fKeyEqualsComparer_ (key, (*v)->fKey));
        return (*v)->fValue;
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename V>
        requires (IValuelessCache<V>)
    auto LRUCache<KEY, VALUE, TRAITS>::Lookup (typename Common::ArgByValueType<KEY> key) -> optional<KEY>
    {
        scoped_lock              critSec{fMaybeMutex_}; // subtle - WRITE cuz updates LRU
        optional<KeyValuePair_>* v = LookupElement_ (key);
        if (v == nullptr) {
            return optional<KEY>{};
        }
        Ensure (fKeyEqualsComparer_ (key, (*v)->fKey));
        return (*v)->fKey;
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename V>
        requires (not IValuelessCache<V>)
    void LRUCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<V> value)
    {
        scoped_lock critSec{fMaybeMutex_};
        Add_ (key, value);
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename V>
        requires (IValuelessCache<V>)
    inline void LRUCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<KEY> key)
    {
        scoped_lock              critSec{fMaybeMutex_};
        optional<KeyValuePair_>* v = AddNewButDontFillIn_ (key);
        v->emplace (KeyValuePair_{.fKey = key});
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void LRUCache<KEY, VALUE, TRAITS>::Add_ (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<VALUE> value)
    {
        optional<KeyValuePair_>* v = AddNewButDontFillIn_ (key);
        v->emplace (KeyValuePair_{.fKey = key, .fValue = value});
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline size_t LRUCache<KEY, VALUE, TRAITS>::H_ ([[maybe_unused]] typename Common::ArgByValueType<KEY> k) const
    {
        Assert (fHashtableSize_ >= 1);
        if constexpr (same_as<typename TRAITS::KeyHashFunctionType, nullptr_t>) {
            return 0; // avoid referencing hash function
        }
        else {
            return fHashFunction_ (k) % fHashtableSize_;
        }
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    auto LRUCache<KEY, VALUE, TRAITS>::Elements () const -> Containers::Mapping<KEY, VALUE>
    {
        Containers::Mapping<KEY, VALUE> result;
        shared_lock                     critSec{fMaybeMutex_};
        for (CacheIterator_ i = begin_ (); i != end_ (); ++i) {
            if (*i) {
                result.Add ((*i)->fKey, (*i)->fValue);
            }
        }
        return result;
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    VALUE LRUCache<KEY, VALUE, TRAITS>::LookupValue (typename Common::ArgByValueType<KEY>                          key,
                                                     const function<VALUE (typename Common::ArgByValueType<KEY>)>& valueFetcher)
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
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::begin_ () const -> CacheIterator_
    {
        LRUCache* ncThis = const_cast<LRUCache*> (this); // http://stroika-bugs.sophists.com/browse/STK-764
        return CacheIterator_{std::begin (ncThis->fCachedElts_First_), std::end (ncThis->fCachedElts_First_)};
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline typename LRUCache<KEY, VALUE, TRAITS>::CacheIterator_ LRUCache<KEY, VALUE, TRAITS>::end_ () const
    {
        return CacheIterator_{nullptr, nullptr};
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void LRUCache<KEY, VALUE, TRAITS>::ShuffleToHead_ (size_t chainIdx, CacheElement_* b)
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
        Ensure (fCachedElts_First_[chainIdx] != nullptr and fCachedElts_First_[chainIdx] == b and
                fCachedElts_First_[chainIdx]->fPrev == nullptr and fCachedElts_First_[chainIdx]->fNext != nullptr);
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void LRUCache<KEY, VALUE, TRAITS>::ClearCache_ ()
    {
        for (size_t hi = 0; hi < fHashtableSize_; ++hi) {
            for (CacheElement_* cur = fCachedElts_First_[hi]; cur != nullptr; cur = cur->fNext) {
                cur->fElement = nullopt;
            }
        }
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::LookupElement_ (typename Common::ArgByValueType<KeyType> item) -> optional<KeyValuePair_>*
    {
        size_t chainIdx = H_ (item);
        Assert (0 <= chainIdx and chainIdx < fHashtableSize_);
        for (CacheElement_* cur = fCachedElts_First_[chainIdx]; cur != nullptr; cur = cur->fNext) {
            if (cur->fElement and fKeyEqualsComparer_ (cur->fElement->fKey, item)) {
                ShuffleToHead_ (chainIdx, cur);
                fStats_.IncrementHits ();
                return &fCachedElts_First_[chainIdx]->fElement;
            }
        }
        fStats_.IncrementMisses ();
        return nullptr;
    }
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto LRUCache<KEY, VALUE, TRAITS>::AddNewButDontFillIn_ (typename Common::ArgByValueType<KeyType> item) -> optional<KeyValuePair_>*
    {
        size_t chainIdx = H_ (item);
        Assert (0 <= chainIdx and chainIdx < fHashtableSize_);
        ShuffleToHead_ (chainIdx, fCachedElts_Last_[chainIdx]);
        return &fCachedElts_First_[chainIdx]->fElement;
    }

    /*
     ********************************************************************************
     **** Factory::LRUCache::Maker<KEY,VALUE,InternallySynchronized,STATS_TYPE> *****
     ********************************************************************************
     */
    namespace Factory::LRUCache {
        template <typename KEY, typename VALUE, InternallySynchronized internallySynchronized, typename STATS_TYPE>
        template <Common::IEqualsComparer<KEY> KEY_EQUALS_COMPARER>
#if __cplusplus >= kStrokia_Foundation_Common_cplusplus_23 || _HAS_CXX23 /*vis studio uses _HAS_CXX23 */
        inline auto Maker<KEY, VALUE, internallySynchronized, STATS_TYPE>::operator() (size_t maxCacheSize, KEY_EQUALS_COMPARER&& keyComparer)
#else
        inline auto Maker<KEY, VALUE, internallySynchronized, STATS_TYPE>::operator() (size_t maxCacheSize, KEY_EQUALS_COMPARER&& keyComparer) const
#endif
        {
            using namespace LRUCacheSupport;
            using TRAITS_ = WithKeyComparerTraits<DefaultTraits<KEY, VALUE>, KEY_EQUALS_COMPARER>;
            if constexpr (internallySynchronized == Execution::InternallySynchronized::eInternallySynchronized) {
                return Cache::LRUCache<KEY, VALUE, InternallySynchronizedTraits<TRAITS_>>{maxCacheSize, forward<KEY_EQUALS_COMPARER> (keyComparer)};
            }
            else {
                return Cache::LRUCache<KEY, VALUE, TRAITS_>{maxCacheSize, forward<KEY_EQUALS_COMPARER> (keyComparer)};
            }
        }
        template <typename KEY, typename VALUE, InternallySynchronized internallySynchronized, typename STATS_TYPE>
        template <typename KEY_HASH_FUNCTION>
#if __cplusplus >= kStrokia_Foundation_Common_cplusplus_23 || _HAS_CXX23 /*vis studio uses _HAS_CXX23 */
        inline auto Maker<KEY, VALUE, internallySynchronized, STATS_TYPE>::operator() (size_t maxCacheSize, size_t hashTableSize,
                                                                                       KEY_HASH_FUNCTION&& hashFunction)
#else
        inline auto Maker<KEY, VALUE, internallySynchronized, STATS_TYPE>::operator() (size_t maxCacheSize, size_t hashTableSize,
                                                                                       KEY_HASH_FUNCTION&& hashFunction) const
#endif
        {
            Require (maxCacheSize >= hashTableSize);
            using namespace LRUCacheSupport;
            using TRAITS_ = WithKeyHashTraits<DefaultTraits<KEY, VALUE>, KEY_HASH_FUNCTION>;
            if constexpr (internallySynchronized == InternallySynchronized::eInternallySynchronized) {
                return Cache::LRUCache<KEY, VALUE, InternallySynchronizedTraits<TRAITS_>>{maxCacheSize, equal_to<KEY>{}, hashTableSize, hashFunction};
            }
            else {
                return Cache::LRUCache<KEY, VALUE, TRAITS_>{maxCacheSize, equal_to<KEY>{}, hashTableSize, hashFunction};
            }
        }
        template <typename KEY, typename VALUE, InternallySynchronized internallySynchronized, typename STATS_TYPE>
        template <typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION>
#if __cplusplus >= kStrokia_Foundation_Common_cplusplus_23 || _HAS_CXX23 /*vis studio uses _HAS_CXX23 */
        inline auto Maker<KEY, VALUE, internallySynchronized, STATS_TYPE>::operator() (size_t maxCacheSize, KEY_EQUALS_COMPARER&& keyComparer,
                                                                                       size_t hashTableSize, KEY_HASH_FUNCTION&& hashFunction)
#else
        inline auto Maker<KEY, VALUE, internallySynchronized, STATS_TYPE>::operator() (size_t maxCacheSize, KEY_EQUALS_COMPARER&& keyComparer,
                                                                                       size_t hashTableSize, KEY_HASH_FUNCTION&& hashFunction) const
#endif
        {
            Require (maxCacheSize >= hashTableSize);
            using namespace LRUCacheSupport;
            using TRAITS_ = WithKeyHashTraits<WithKeyComparerTraits<DefaultTraits<KEY, VALUE>, KEY_EQUALS_COMPARER>, KEY_HASH_FUNCTION>;
            if constexpr (internallySynchronized == InternallySynchronized::eInternallySynchronized) {
                return Cache::LRUCache<KEY, VALUE, InternallySynchronizedTraits<TRAITS_>>{maxCacheSize, keyComparer, hashTableSize, hashFunction};
            }
            else {
                return Cache::LRUCache<KEY, VALUE, TRAITS_>{maxCacheSize, keyComparer, hashTableSize, hashFunction};
            }
        }
    }

    ///
    ///  DEPRECATED CLASS TEMPLATES
    ///
    template <typename KEY, typename VALUE, LRUCacheSupport::ITraits<KEY, VALUE> TRAITS = LRUCacheSupport::DefaultTraits<KEY, VALUE>>
    using SynchronizedLRUCache
        [[deprecated ("Since Stroika v3.0d23 - use LRUCacheSupport::InternallySynchronizedTraits or Factory::LRUCache::Maker<string, "
                      "string,InternallySynchronized::eInternallySynchronized>{}(3)")]] =
            LRUCache<KEY, VALUE, LRUCacheSupport::InternallySynchronizedTraits<TRAITS>>;

    namespace Factory {
        template <typename KEY, typename VALUE, typename STATS_TYPE = Statistics::StatsType_DEFAULT>
        struct [[deprecated ("Since Stroika v3.0d23 use Factory::LRUCache::Maker<KEY,VALUE>")]] LRUCache_NoHash {
            template <Common::IEqualsComparer<KEY> KEY_EQUALS_COMPARER = equal_to<KEY>>
            auto operator() (size_t maxCacheSize = 1, const KEY_EQUALS_COMPARER& keyComparer = {}) const
            {
                return Factory::LRUCache::Maker<KEY, VALUE, Execution::InternallySynchronized::eNotKnownInternallySynchronized, STATS_TYPE>{}(
                    maxCacheSize, keyComparer);
            }
        };
        template <typename KEY, typename VALUE, typename STATS_TYPE = Statistics::StatsType_DEFAULT, typename DEFAULT_KEY_EQUALS_COMPARER = equal_to<KEY>>
        struct [[deprecated ("Since Stroika v3.0d23 use Factory::LRUCache::Maker<KEY,VALUE>")]] LRUCache_WithHash {
            template <typename KEY_HASH_FUNCTION = hash<KEY>>
            auto operator() (size_t maxCacheSize, size_t hashTableSize, const KEY_HASH_FUNCTION& hashFunction = {}) const
            {
                return Factory::LRUCache::Maker<KEY, VALUE, Execution::InternallySynchronized::eNotKnownInternallySynchronized, STATS_TYPE>{}(
                    maxCacheSize, DEFAULT_KEY_EQUALS_COMPARER{}, hashTableSize, hashFunction);
            }
            template <typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION = hash<KEY>>
            auto operator() (size_t maxCacheSize, const KEY_EQUALS_COMPARER& keyComparer, size_t hashTableSize,
                             const KEY_HASH_FUNCTION& hashFunction = {}) const
            {
                return Factory::LRUCache::Maker<KEY, VALUE, Execution::InternallySynchronized::eNotKnownInternallySynchronized, STATS_TYPE>{}(
                    maxCacheSize, keyComparer, hashTableSize, hashFunction);
            }
        };
        template <typename KEY, typename VALUE, typename STATS_TYPE = Statistics::StatsType_DEFAULT>
        struct [[deprecated ("Since Stroika v3.0d23 use Factory::LRUCache::Maker<KEY, "
                             "VALUE,InternallySynchronized::eInternallySynchronized>")]] SynchronizedLRUCache_NoHash {
            template <Common::IEqualsComparer<KEY> KEY_EQUALS_COMPARER = equal_to<KEY>>
            auto operator() (size_t maxCacheSize = 1, const KEY_EQUALS_COMPARER& keyComparer = {}) const
            {
                return Factory::LRUCache::Maker<KEY, VALUE, Execution::InternallySynchronized::eInternallySynchronized, STATS_TYPE>{}(maxCacheSize, keyComparer);
            }
        };
        template <typename KEY, typename VALUE, typename STATS_TYPE = Statistics::StatsType_DEFAULT, typename DEFAULT_KEY_EQUALS_COMPARER = equal_to<KEY>>
        struct [[deprecated ("Since Stroika v3.0d23 use Factory::LRUCache::Maker<KEY, "
                             "VALUE,Execution::InternallySynchronized::eInternallySynchronized>")]] SynchronizedLRUCache_WithHash {
            template <typename KEY_HASH_FUNCTION = hash<KEY>>
            auto operator() (size_t maxCacheSize, size_t hashTableSize, const KEY_HASH_FUNCTION& hashFunction = {}) const
            {
                return Factory::LRUCache::Maker<KEY, VALUE, Execution::InternallySynchronized::eInternallySynchronized, STATS_TYPE>{}(
                    maxCacheSize, hashTableSize, hashFunction);
            }
            template <typename KEY_EQUALS_COMPARER, typename KEY_HASH_FUNCTION = hash<KEY>>
            auto operator() (size_t maxCacheSize, const KEY_EQUALS_COMPARER& keyComparer, size_t hashTableSize,
                             const KEY_HASH_FUNCTION& hashFunction = {}) const
            {
                return Factory::LRUCache::Maker<KEY, VALUE, Execution::InternallySynchronized::eInternallySynchronized, STATS_TYPE>{}(
                    maxCacheSize, keyComparer, hashTableSize, hashFunction);
            }
        };

    }

}
