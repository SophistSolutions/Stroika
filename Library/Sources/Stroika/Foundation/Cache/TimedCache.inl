/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Common.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ************************* TimedCache<KEY,VALUE,TRAITS> *************************
     ********************************************************************************
     */
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
    inline auto TimedCache<KEY, VALUE, TRAITS>::MyResult_::MakeCacheElement (const K& key) const -> CacheElement
    {
        if constexpr (kTrackExpiration) {
            if constexpr (same_as<void, VALUE>) {
                return CacheElement{.fKey = key, .fExpiresAt = fExpiresAt};
            }
            else {
                return CacheElement{.fKey = key, .fValue = fResult, .fExpiresAt = fExpiresAt};
            }
        }
        else if constexpr (kTrackFreshness) {
            if constexpr (same_as<void, VALUE>) {
                return CacheElement{.fKey = key, .fLastRefreshedAt = fLastRefreshedAt};
            }
            else {
                return CacheElement{.fKey = key, .fValue = fResult, .fLastRefreshedAt = fLastRefreshedAt};
            }
        }
    }

    /*
     ********************************************************************************
     ************************* TimedCache<KEY,VALUE,TRAITS> *************************
     ********************************************************************************
     */
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    TimedCache<KEY, VALUE, TRAITS>::TimedCache ()
        requires (not is_empty_v<decltype (TRAITS::kDefaultMaxAge)>)
    {
        if constexpr (TRAITS::kPerCacheMaxAge) {
            fMaxAge_ = TRAITS::kDefaultMaxAge;
        }
        if constexpr (TRAITS::kAutomaticPurgeFrequency != TimeStampDifferenceType{TimedCacheSupport::kNoAutomaticPurgeSentinal}) {
            fNextAutoClearAt_ = TRAITS::GetCurrentTimestamp () + TRAITS::kAutomaticPurgeFrequency;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    TimedCache<KEY, VALUE, TRAITS>::TimedCache (TimeStampDifferenceType maxAge)
        requires (TRAITS::kPerCacheMaxAge)
        : fMaxAge_{maxAge}
        , fNextAutoClearAt_{TRAITS::GetCurrentTimestamp () + TRAITS::kAutomaticPurgeFrequency} // hopefully optimized away if kNoAutomaticPurgeSentinal
    {
        Require (fMaxAge_ > 0.0s);
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline TimedCache<KEY, VALUE, TRAITS>::TimedCache (TimedCache&& src) noexcept
        : fMaxAge_{src.fMaxAge_}
        , fNextAutoClearAt_{src.fNextAutoClearAt_}
    {
        [[maybe_unused]] auto&& srcLock = scoped_lock{src.fMaybeMutex_};
        fData_                          = move (src.fData_);
        fStats_                         = move (src.fStats_);
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline TimedCache<KEY, VALUE, TRAITS>::TimedCache (const TimedCache& src)
        : fMaxAge_{src.fMaxAge_}
        , fNextAutoClearAt_{src.fNextAutoClearAt_}
    {
        [[maybe_unused]] auto&& srcLock = shared_lock{src.fMaybeMutex_};
        fData_                          = src.fData_;
        fStats_                         = src.fStats_;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto TimedCache<KEY, VALUE, TRAITS>::operator= (TimedCache&& rhs) noexcept -> TimedCache&
    {
        [[maybe_unused]] auto&& locks = scoped_lock{rhs.fMaybeMutex_, fMaybeMutex_};
        fMaxAge_                      = rhs.fMaxAge_;
        fNextAutoClearAt_             = rhs.fNextAutoClearAt_;
        fData_                        = move (rhs.fData_);
        fStats_                       = move (rhs.fStats_);
        return *this;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto TimedCache<KEY, VALUE, TRAITS>::operator= (const TimedCache& rhs) -> TimedCache&
    {
        [[maybe_unused]] auto&& locks = scoped_lock{rhs.fMaybeMutex_, fMaybeMutex_};
        fMaxAge_                      = rhs.fMaxAge_;
        fNextAutoClearAt_             = rhs.fNextAutoClearAt_;
        fData_                        = rhs.fData_;
        fStats_                       = rhs.fStats_;
        return *this;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline bool TimedCache<KEY, VALUE, TRAITS>::Expired_ (const MyResult_& r, TimeStampType now) const
    {
        qStroika_ATTRIBUTE_INDETERMINATE TimeStampType expiresAt;
        if constexpr (kTrackExpiration) {
            expiresAt = r.fExpiresAt;
        }
        else if constexpr (kTrackFreshness) {
            expiresAt = r.fLastRefreshedAt + GetMaxAge_ ();
        }
        return now > expiresAt;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    constexpr auto TimedCache<KEY, VALUE, TRAITS>::GetMaxAge_ () const -> TimeStampDifferenceType
    {
        if constexpr (TRAITS::kPerCacheMaxAge) {
            return fMaxAge_;
        }
        else {
            return TRAITS::kDefaultMaxAge;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto TimedCache<KEY, VALUE, TRAITS>::GetMaxAge () const -> TimeStampDifferenceType
    {
        if constexpr (TRAITS::kPerCacheMaxAge) {
            shared_lock critSec{fMaybeMutex_};
            return GetMaxAge_ ();
        }
        else {
            return TRAITS::kDefaultMaxAge;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::SetMaxAge (TimeStampDifferenceType maxAge)
        requires (TRAITS::kPerCacheMaxAge)
    {
        Require (maxAge > 0.0s);
        scoped_lock critSec{fMaybeMutex_};
        if (fMaxAge_ != maxAge) {
            fMaxAge_ = maxAge;
            ClearExpired_ (); // ClearExpired_ not AutomaticallyClearExpiredDataSometimes_ to force auto-update of fNextAutoClearAt_, and cuz moderately likely items interestingly out of date after adjust of min allowed freshness
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    auto TimedCache<KEY, VALUE, TRAITS>::Elements () const -> Traversal::Iterable<CacheElement>
    {
        shared_lock          critSec{fMaybeMutex_};
        vector<CacheElement> r;
        r.reserve (fData_.size ());
        TimeStampType now = TRAITS::GetCurrentTimestamp ();
        for (const auto& i : fData_) {
            if (not Expired_ (i.second, now)) {
                r.push_back (i.second.MakeCacheElement (i.first));
            }
        }
        return Traversal::Iterable<CacheElement>{move (r)};
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
    auto TimedCache<KEY, VALUE, TRAITS>::Keys () const -> Traversal::Iterable<K>
        requires (IKeyedCache<K>)
    {
        vector<KEY> r;
        shared_lock critSec{fMaybeMutex_};
        r.reserve (fData_.size ());
        TimeStampType now = TRAITS::GetCurrentTimestamp ();
        for (const auto& i : fData_) {
            if (not Expired_ (i.second, now)) {
                r.push_back (i.first);
            }
        }
        return Traversal::Iterable<KEY>{move (r)};
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
        requires (IKeyedCache<K>)
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup (typename Common::ArgByValueType<K> key) const
    {
        shared_lock                         critSec{fMaybeMutex_};
        typename MyMapType_::const_iterator i = fData_.find (key);
        if (i == fData_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            if (Expired_ (i->second)) {
                /*
                 *  Cannot update fData_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            return i->second.fResult;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
        requires (IKeyedCache<K>)
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup (typename Common::ArgByValueType<K> key)
    {
        shared_lock                   critSec{fMaybeMutex_};
        typename MyMapType_::iterator i = fData_.find (key);
        if (i == fData_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            if (Expired_ (i->second)) {
                /*
                 * since expired, remove from cache
                 */
                (void)fData_.erase (i);
                fStats_.IncrementMisses ();
                return nullopt;
            }
            // unclear if we should do this test before or after expired check above???
            if constexpr (TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed) {
                i->second.fLastRefreshedAt = TRAITS::GetCurrentTimestamp ();
            }
            fStats_.IncrementHits ();
            return i->second.fResult;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
        requires (IKeyedCache<K> and TRAITS::kTrackFreshness)
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup (typename Common::ArgByValueType<K> key, TimeStampDifferenceType maxAge) const
    {
        shared_lock                         critSec{fMaybeMutex_};
        typename MyMapType_::const_iterator i   = fData_.find (key);
        TimeStampType                       now = TRAITS::GetCurrentTimestamp ();
        if (i == fData_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            if (i->second.fLastRefreshedAt + maxAge <= now) {
                /*
                 *  Cannot update fData_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            return i->second.fResult;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
        requires (not IKeyedCache<K>)
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup () const
    {
        shared_lock critSec{fMaybeMutex_};
        if (optional<VALUE> ov = fData_) {
            if (Expired_ (*ov)) {
                /*
                 *  Cannot update fData_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            return ov->fResult;
        }
        return nullopt;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
        requires (not IKeyedCache<K>)
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup ()
    {
        scoped_lock critSec{fMaybeMutex_};
        if (optional<VALUE> ov = fData_) {
            if (Expired_ (*ov)) {
                /*
                 *  since expired, remove from cache
                 */
                fData_ = nullopt;
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            return ov->fResult;
        }
        return nullopt;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
        requires (not IKeyedCache<K> and TRAITS::kTrackFreshness)
    inline optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup (TimeStampDifferenceType maxAge) const
    {
        if (optional<MyResult_> ov = fData_) {
            TimeStampType now = TRAITS::GetCurrentTimestamp ();
            if (ov->fLastRefreshedAt + maxAge <= now) {
                /*
                 *  Cannot update fData_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            return ov->fResult;
        }
        return nullopt;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (IKeyedCache<K> and IValuelessCache<V>)
    optional<KEY> TimedCache<KEY, VALUE, TRAITS>::Lookup (typename Common::ArgByValueType<K> key) const
    {
        static_assert (not IKeyedCache<KEY>); // cuz cannot be both unkeyed and valueless
        shared_lock                         critSec{fMaybeMutex_};
        typename MyMapType_::const_iterator i = fData_.find (key);
        if (i == fData_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            if (Expired_ (*i)) {
                /*
                 *  Cannot update fData_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            // return the STORED key object, which could contain more fields/data than the argument KEY
            return i->first;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
    auto TimedCache<KEY, VALUE, TRAITS>::LookupDetails (typename Common::ArgByValueType<K> key) const -> optional<tuple<VALUE, TimeStampType>>
        requires (kTrackExpiration and IKeyedCache<K>)
    {
        shared_lock                         critSec{fMaybeMutex_};
        typename MyMapType_::const_iterator i = fData_.find (key);
        if (i == fData_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            if (Expired_ (i->second)) {
                /*
                 *  Cannot update fData_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            return make_tuple (i->second.fResult, i->second.fExpiresAt);
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
    auto TimedCache<KEY, VALUE, TRAITS>::LookupDetails (typename Common::ArgByValueType<K> key) -> optional<tuple<VALUE, TimeStampType>>
        requires (kTrackExpiration and IKeyedCache<K>)
    {
        shared_lock                   critSec{fMaybeMutex_};
        typename MyMapType_::iterator i = fData_.find (key);
        if (i == fData_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            if (Expired_ (i->second)) {
                /*
                 *  since expired, remove from cache
                 */
                (void)fData_.erase (i);
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            return make_tuple (i->second.fResult, i->second.fExpiresAt);
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
    auto TimedCache<KEY, VALUE, TRAITS>::LookupDetails (typename Common::ArgByValueType<K> key) const -> optional<tuple<VALUE, TimeStampType>>
        requires (kTrackFreshness and IKeyedCache<K>)
    {
        shared_lock                         critSec{fMaybeMutex_};
        typename MyMapType_::const_iterator i = fData_.find (key);
        if (i == fData_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            if (Expired_ (i->second)) {
                /*
                 *  Cannot update fData_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            return make_tuple (i->second.fResult, i->second.fLastRefreshedAt);
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
    inline auto TimedCache<KEY, VALUE, TRAITS>::LookupDetails (typename Common::ArgByValueType<K> key) -> optional<tuple<VALUE, TimeStampType>>
        requires (kTrackFreshness and IKeyedCache<K>)
    {
        shared_lock                   critSec{fMaybeMutex_};
        typename MyMapType_::iterator i = fData_.find (key);
        if (i == fData_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            if (Expired_ (i->second)) {
                /*
                 * since expired, remove from cache
                 */
                (void)fData_.erase (i);
                fStats_.IncrementMisses ();
                return nullopt;
            }
            // unclear if we should do this test before or after expired check above???
            if constexpr (TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed) {
                i->second.fLastRefreshedAt = TRAITS::GetCurrentTimestamp ();
            }
            fStats_.IncrementHits ();
            return make_tuple (i->second.fResult, i->second.fLastRefreshedAt);
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
    auto TimedCache<KEY, VALUE, TRAITS>::LookupDetails (typename Common::ArgByValueType<K> key, TimeStampDifferenceType maxAge) const
        -> optional<tuple<VALUE, TimeStampType>>
        requires (kTrackFreshness and IKeyedCache<K>)
    {
        shared_lock                         critSec{fMaybeMutex_};
        typename MyMapType_::const_iterator i   = fData_.find (key);
        TimeStampType                       now = TRAITS::GetCurrentTimestamp ();
        if (i == fData_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            if (i->second.fLastRefreshedAt + maxAge <= now) {
                /*
                 *  Cannot update fData_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            return make_tuple (i->second.fResult, i->second.fLastRefreshedAt);
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
    auto TimedCache<KEY, VALUE, TRAITS>::LookupDetails (TimeStampDifferenceType maxAge) const -> optional<tuple<VALUE, TimeStampType>>
        requires (not IKeyedCache<K>)
    {
        AssertNotImplemented (); // but easy
        return nullopt;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (IKeyedCache<K> and IValuelessCache<V>)
    auto TimedCache<KEY, VALUE, TRAITS>::LookupDetails (typename Common::ArgByValueType<K> key) const -> optional<tuple<KEY, TimeStampType>>
    {
        static_assert (not IKeyedCache<KEY>); // cuz cannot be both unkeyed and valueless
        shared_lock                         critSec{fMaybeMutex_};
        typename MyMapType_::const_iterator i = fData_.find (key);
        if (i == fData_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            if (Expired_ (*i)) {
                /*
                 *  Cannot update fData_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            // return the STORED key object, which could contain more fields/data than the argument KEY
            return i->first;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
    auto TimedCache<KEY, VALUE, TRAITS>::GetExpiration () const -> optional<TimeStampType>
        requires (not IKeyedCache<K>)
    {
        if (optional<MyResult_> ov = fData_) {
            if (Expired_ (*ov)) {
                /*
                 *  Cannot update fData_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            return ov->fResult;
        }
        return nullopt;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
    auto TimedCache<KEY, VALUE, TRAITS>::GetExpiration (typename Common::ArgByValueType<K> key) const -> optional<TimeStampType>
        requires (IKeyedCache<K>)
    {
        typename MyMapType_::const_iterator i = fData_.find (key);
        if (i == fData_.end ()) {
            return nullopt;
        }
        if (Expired_ (i->second)) {
            return nullopt;
        }
        if constexpr (kTrackExpiration) {
            return i->second.fExpiresAt;
        }
        else if constexpr (kTrackFreshness) {
            return i->second.fLastRefreshedAt + GetMaxAge_ ();
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, Common::invocable_r<VALUE, KEY> CACHE_FILLTER_T>
        requires (IKeyedCache<K>)
    VALUE TimedCache<KEY, VALUE, TRAITS>::LookupValue (typename Common::ArgByValueType<K> key, CACHE_FILLTER_T&& cacheFiller)
    {
        if (optional<VALUE> o = Lookup (key)) {
            return *o;
        }
        else {
            return LockingLookupValueAdder_ (key, forward<CACHE_FILLTER_T> (cacheFiller));
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, Common::invocable_r<VALUE, KEY> CACHE_FILLTER_T>
        requires (IKeyedCache<K> and TRAITS::kTrackFreshness)
    VALUE TimedCache<KEY, VALUE, TRAITS>::LookupValue (typename Common::ArgByValueType<K> key, TimeStampDifferenceType maxAge, CACHE_FILLTER_T&& cacheFiller)
    {
        if (optional<VALUE> o = Lookup (key, maxAge)) {
            return *o;
        }
        else {
            return LockingLookupValueAdder_ (key, forward<CACHE_FILLTER_T> (cacheFiller));
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, Common::invocable_r<VALUE> CACHE_FILLTER_T>
        requires (not IKeyedCache<K>)
    VALUE TimedCache<KEY, VALUE, TRAITS>::LookupValue (CACHE_FILLTER_T&& cacheFiller)
    {
        if (optional<VALUE> ov = Lookup ()) {
            return *ov;
        }
        else {
            return LockingLookupValueAdder_ (forward<CACHE_FILLTER_T> (cacheFiller));
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, Common::invocable_r<VALUE> CACHE_FILLTER_T>
        requires (not IKeyedCache<K> and TRAITS::kTrackFreshness)
    VALUE TimedCache<KEY, VALUE, TRAITS>::LookupValue (TimeStampDifferenceType maxAge, CACHE_FILLTER_T&& cacheFiller)
    {
        if (optional<VALUE> ov = Lookup (maxAge)) {
            return *ov;
        }
        else {
            return LockingLookupValueAdder_ (forward<CACHE_FILLTER_T> (cacheFiller));
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V, Common::invocable_r<K, K> CACHE_FILLTER_T>
        requires (IKeyedCache<K> and IValuelessCache<V>)
    KEY TimedCache<KEY, VALUE, TRAITS>::LookupValue (typename Common::ArgByValueType<K> key, CACHE_FILLTER_T&& cacheFiller)
    {
        if (optional<KEY> ok = Lookup (key)) {
            return *ok;
        }
        else {
            KEY kr = forward<CACHE_FILLTER_T> (cacheFiller) (key);
            Add (kr);
            return kr;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, Common::invocable_r<VALUE> CACHE_FILLTER_T>
        requires (not IKeyedCache<K>)
    inline VALUE TimedCache<KEY, VALUE, TRAITS>::LockingLookupValueAdder_ (CACHE_FILLTER_T&& cacheFiller)
    {
        VALUE r = forward<CACHE_FILLTER_T> (cacheFiller) ();
        Add (r);
        return r;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, Common::invocable_r<VALUE, KEY> CACHE_FILLTER_T>
        requires (IKeyedCache<K>)
    inline VALUE TimedCache<KEY, VALUE, TRAITS>::LockingLookupValueAdder_ (typename Common::ArgByValueType<K> key, CACHE_FILLTER_T&& cacheFiller)
    {
        if constexpr (TRAITS::kHoldWriteLockDuringCacheFill) {
            scoped_lock critSec{fMaybeMutex_};
            VALUE       v = forward<CACHE_FILLTER_T> (cacheFiller) (key);
            AutomaticallyClearExpiredDataSometimes_ ();
            Add_ (key, v);
            return v;
        }
        else {
            VALUE v = forward<CACHE_FILLTER_T> (cacheFiller) (key);
            Add (key, v); // public API so will acquire lock
            return v;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
        requires (IKeyedCache<K> and IValuelessCache<VALUE>)
    void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<K> key)
    {
        scoped_lock critSec{fMaybeMutex_};
        // nb skip AutomaticallyClearExpiredDataSometimes_ cuz Valueless has no map, and add will just overwrite
        TimeStampType now = TRAITS::GetCurrentTimestamp ();
        if constexpr (kTrackExpiration) {
            Add_ (key, now + GetMaxAge_ ());
        }
        else if constexpr (kTrackFreshness) {
            Add_ (key, now);
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (IKeyedCache<K> and not IValuelessCache<V>)
    void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<K> key, typename Common::ArgByValueType<V> result)
    {
        scoped_lock critSec{fMaybeMutex_};
        AutomaticallyClearExpiredDataSometimes_ ();
        TimeStampType now = TRAITS::GetCurrentTimestamp ();
        if constexpr (kTrackExpiration) {
            Add_ (key, result, now + GetMaxAge_ ());
        }
        else if constexpr (kTrackFreshness) {
            Add_ (key, result, now);
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (IKeyedCache<K> and not IValuelessCache<V> and TRAITS::kTrackFreshness)
    void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<K> key, typename Common::ArgByValueType<V> result, TimeStampType freshAsOf)
    {
        scoped_lock critSec{fMaybeMutex_};
        AutomaticallyClearExpiredDataSometimes_ ();
        Add_ (key, result, freshAsOf);
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (IKeyedCache<K> and not IValuelessCache<V> and TRAITS::kTrackExpiration)
    void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<K> key, typename Common::ArgByValueType<V> result, TimeStampType expiresAt)
    {
        scoped_lock critSec{fMaybeMutex_};
        AutomaticallyClearExpiredDataSometimes_ ();
        Add_ (key, result, expiresAt);
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (not IKeyedCache<K> and not IValuelessCache<V>)
    inline void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<V> result)
    {
        TimeStampType now = TRAITS::GetCurrentTimestamp ();
        if constexpr (kTrackExpiration) {
            Add (result, now + GetMaxAge_ ());
        }
        else if constexpr (kTrackFreshness) {
            Add (result, now);
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (not IKeyedCache<K> and not IValuelessCache<V> and TRAITS::kTrackFreshness)
    inline void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<V> result, TimeStampType freshAsOf)
    {
        scoped_lock critSec{fMaybeMutex_};
        // no AutomaticallyClearExpiredDataSometimes_ for not keyed cache
        Add_ (result, freshAsOf);
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (not IKeyedCache<K> and not IValuelessCache<V> and TRAITS::kTrackExpiration)
    inline void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<V> result, TimeStampType expiresAt)
    {
        scoped_lock critSec{fMaybeMutex_};
        AutomaticallyClearExpiredDataSometimes_ ();
        Add_ (result, expiresAt);
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
        requires (IKeyedCache<K> and IValuelessCache<VALUE> and TRAITS::kTrackExpiration)
    inline void TimedCache<KEY, VALUE, TRAITS>::Add_ (typename Common::ArgByValueType<K> key, TimeStampType expiresAt)
    {
        fData_.insert_or_assign (key, MyResult_{.fExpiresAt = expiresAt});
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
        requires (IKeyedCache<K> and IValuelessCache<VALUE> and TRAITS::kTrackFreshness)
    inline void TimedCache<KEY, VALUE, TRAITS>::Add_ (typename Common::ArgByValueType<K> key, TimeStampType freshAsOf)
    {
        fData_.insert_or_assign (key, MyResult_{.fLastRefreshedAt = freshAsOf});
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (IKeyedCache<K> and not IValuelessCache<V> and TRAITS::kTrackFreshness)
    inline void TimedCache<KEY, VALUE, TRAITS>::Add_ (typename Common::ArgByValueType<K> key, typename Common::ArgByValueType<V> result, TimeStampType freshAsOf)
    {
        fData_.insert_or_assign (key, MyResult_{.fResult = result, .fLastRefreshedAt = freshAsOf});
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (IKeyedCache<K> and not IValuelessCache<V> and TRAITS::kTrackExpiration)
    inline void TimedCache<KEY, VALUE, TRAITS>::Add_ (typename Common::ArgByValueType<K> key, typename Common::ArgByValueType<V> result, TimeStampType expiresAt)
    {
        fData_.insert_or_assign (key, MyResult_{.fResult = result, .fExpiresAt = expiresAt});
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (not IKeyedCache<K> and not IValuelessCache<V> and TRAITS::kTrackFreshness)
    inline void TimedCache<KEY, VALUE, TRAITS>::Add_ (typename Common::ArgByValueType<V> result, TimeStampType freshAsOf)
    {
        fData_ = MyResult_{.fResult = result, .fLastRefreshedAt = freshAsOf};
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K, typename V>
        requires (not IKeyedCache<K> and not IValuelessCache<V> and TRAITS::kTrackExpiration)
    inline void TimedCache<KEY, VALUE, TRAITS>::Add_ (typename Common::ArgByValueType<V> result, TimeStampType expiresAt)
    {
        fData_ = MyResult_{.fResult = result, .fExpiresAt = expiresAt};
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <typename K>
        requires (IKeyedCache<K>)
    inline void TimedCache<KEY, VALUE, TRAITS>::Remove (typename Common::ArgByValueType<K> key)
    {
        scoped_lock critSec{fMaybeMutex_};
        fData_.erase (key);
        AutomaticallyClearExpiredDataSometimes_ ();
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (predicate<typename TimedCache<KEY, VALUE, TRAITS>::CacheElement>) PREDICATE>
    void TimedCache<KEY, VALUE, TRAITS>::RemoveAll (PREDICATE&& p)
    {
        scoped_lock critSec{fMaybeMutex_};
        for (auto i = fData_.begin (); i != fData_.end ();) {
            if (p (i->second.MakeCacheElement (i->first))) {
                i = fData_.erase (i);
            }
            else {
                ++i;
            }
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <Traversal::IIterableOfTo<KEY> ITERABLE_OF_KEY_TYPE>
    void TimedCache<KEY, VALUE, TRAITS>::RetainAll (const ITERABLE_OF_KEY_TYPE& items)
    {
        scoped_lock critSec{fMaybeMutex_};
        // quickie inefficient implementation
        Containers::Mapping<KEY, MyResult_> tmp{this->fData_};
        tmp.RetainAll (items);
        fData_ = tmp.template As<MyMapType_> ();
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::clear ()
    {
        scoped_lock critSec{fMaybeMutex_};
        fData_.clear ();
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::ClearExpiredData ()
    {
        scoped_lock critSec{fMaybeMutex_};
        ClearExpired_ ();
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::ClearExpiredData (TimeStampDifferenceType maxAge)
        requires (TRAITS::kTrackFreshness)
    {
        scoped_lock critSec{fMaybeMutex_};
        ClearExpired_ (maxAge);
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline typename TRAITS::StatsType TimedCache<KEY, VALUE, TRAITS>::GetStats () const
    {
        return fStats_;
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::AutomaticallyClearExpiredDataSometimes_ ()
        requires (IKeyedCache<KEY>)
    {
        if constexpr (TRAITS::kAutomaticPurgeFrequency != TimeStampDifferenceType{TimedCacheSupport::kNoAutomaticPurgeSentinal}) {
            if constexpr (TRAITS::kAutomaticPurgeFrequency != TimeStampDifferenceType{TimedCacheSupport::kNoAutomaticPurgeSentinal}) {
                if (fNextAutoClearAt_ < TRAITS::GetCurrentTimestamp ()) [[unlikely]] {
                    ClearExpired_ ();
                    WeakAssert (fNextAutoClearAt_ > TRAITS::GetCurrentTimestamp ()); // note internally resets fNextAutoClearAt_
                }
            }
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::ClearExpired_ ()
        requires (IKeyedCache<KEY>)
    {
        TimeStampType now = TRAITS::GetCurrentTimestamp ();
        for (typename MyMapType_::iterator i = fData_.begin (); i != fData_.end ();) {
            if (Expired_ (i->second, now)) {
                i = fData_.erase (i);
            }
            else {
                ++i;
            }
        }
        if constexpr (TRAITS::kAutomaticPurgeFrequency != TimeStampDifferenceType{TimedCacheSupport::kNoAutomaticPurgeSentinal}) {
            fNextAutoClearAt_ = now + TRAITS::kAutomaticPurgeFrequency;
        }
    }
    template <IKey KEY, IValue VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::ClearExpired_ (TimeStampDifferenceType maxAge)
        requires (IKeyedCache<KEY> and TRAITS::kTrackFreshness)
    {
        TimeStampType now = TRAITS::GetCurrentTimestamp ();
        for (typename MyMapType_::iterator i = fData_.begin (); i != fData_.end ();) {
            if (i->second.fLastRefreshedAt + maxAge <= now) {
                i = fData_.erase (i);
            }
            else {
                ++i;
            }
        }
        if constexpr (TRAITS::kAutomaticPurgeFrequency != TimeStampDifferenceType{TimedCacheSupport::kNoAutomaticPurgeSentinal}) {
            fNextAutoClearAt_ = now + TRAITS::kAutomaticPurgeFrequency;
        }
    }

    ///////////////////
    // DEPRECATED APIS
    ///////////////////
    template <typename KEY, typename VALUE, typename TIME_TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    class [[deprecated ("Since Stroika v3.0d23 - TimedCache has the same functionality and mostly the same names")]] CallerStalenessCache
        : public TimedCache<KEY, VALUE, TimedCacheSupport::DefaultTraits<KEY, VALUE>> {
    private:
        using inherited = TimedCache<KEY, VALUE, TimedCacheSupport::DefaultTraits<KEY, VALUE>>;

    public:
        using TraitsType              = typename inherited::TraitsType;
        using TimeStampType           = typename inherited::TimeStampType;
        using TimeStampDifferenceType = typename inherited::TimeStampDifferenceType;

    public:
        [[deprecated ("Since Stroika v3.0d23, usually just get rid of call and argument can be used directly in situ")]]
        static TimeStampType Ago (TimeStampDifferenceType backThisTime)
        {
            Require (backThisTime >= 0s);
            return TraitsType::GetCurrentTimestamp () - backThisTime;
        }
        [[deprecated ("Since Stroika v3.0d23, use TraitsType::GetCurrentStamp")]]
        static TimeStampType GetCurrentTimestamp ()
        {
            return TraitsType::GetCurrentTimestamp ();
        }
    };
    template <typename KEY, typename VALUE, typename TIME_TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    class [[deprecated ("Since Stroika v3.0d23 - TimedCache (or SyncrhonizedTimedCache) has the same functionality and mostly the same "
                        "names")]] SynchronizedCallerStalenessCache
        : public TimedCache<KEY, VALUE, TimedCacheSupport::InternallySynchronizedTraits<TimedCacheSupport::DefaultTraits<KEY, VALUE>>> {
    private:
        using inherited = TimedCache<KEY, VALUE, TimedCacheSupport::InternallySynchronizedTraits<TimedCacheSupport::DefaultTraits<KEY, VALUE>>>;

    public:
        using TraitsType              = typename inherited::TraitsType;
        using TimeStampType           = typename inherited::TimeStampType;
        using TimeStampDifferenceType = typename inherited::TimeStampDifferenceType;

    public:
        [[deprecated ("Since Stroika v3.0d23, usually just get rid of call and argument can be used directly in situ")]]
        static TimeStampType Ago (TimeStampDifferenceType backThisTime)
        {
            Require (backThisTime >= 0s);
            return TraitsType::GetCurrentTimestamp () - backThisTime;
        }
        [[deprecated ("Since Stroika v3.0d23, use TraitsType::GetCurrentStamp")]]
        static TimeStampType GetCurrentTimestamp ()
        {
            return TraitsType::GetCurrentTimestamp ();
        }
    };

}
