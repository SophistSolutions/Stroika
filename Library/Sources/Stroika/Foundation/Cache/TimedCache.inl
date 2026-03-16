/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Common.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ************************* TimedCache<KEY,VALUE,TRAITS> *************************
     ********************************************************************************
     */
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    TimedCache<KEY, VALUE, TRAITS>::TimedCache (const Time::Duration& minimumAllowedFreshness)
        : fMinimumAllowedFreshness_{minimumAllowedFreshness}
        , fNextAutoClearAt_{Time::GetTickCount () + minimumAllowedFreshness}
    {
        Require (fMinimumAllowedFreshness_ > 0.0s);
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline TimedCache<KEY, VALUE, TRAITS>::TimedCache (TimedCache&& src) noexcept
        : fMinimumAllowedFreshness_{src.fMinimumAllowedFreshness_}
        , fNextAutoClearAt_{src.fNextAutoClearAt_}
    {
        [[maybe_unused]] auto&& srcLock = scoped_lock{src.fMaybeMutex_};
        fMap_                           = move (src.fMap_);
        fStats_                         = move (src.fStats_);
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline TimedCache<KEY, VALUE, TRAITS>::TimedCache (const TimedCache& src)
        : fMinimumAllowedFreshness_{src.fMinimumAllowedFreshness_}
        , fNextAutoClearAt_{src.fNextAutoClearAt_}
    {
        [[maybe_unused]] auto&& srcLock = shared_lock{src.fMaybeMutex_};
        fMap_                           = src.fMap_;
        fStats_                         = src.fStats_;
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto TimedCache<KEY, VALUE, TRAITS>::operator= (TimedCache&& rhs) noexcept -> TimedCache&
    {
        [[maybe_unused]] auto&& locks = scoped_lock{rhs.fMaybeMutex_, fMaybeMutex_};
        fMinimumAllowedFreshness_     = rhs.fMinimumAllowedFreshness_;
        fNextAutoClearAt_             = rhs.fNextAutoClearAt_;
        fMap_                         = move (rhs.fMap_);
        fStats_                       = move (rhs.fStats_);
        return *this;
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto TimedCache<KEY, VALUE, TRAITS>::operator= (const TimedCache& rhs) -> TimedCache&
    {
        [[maybe_unused]] auto&& locks = scoped_lock{rhs.fMaybeMutex_, fMaybeMutex_};
        fMinimumAllowedFreshness_     = rhs.fMinimumAllowedFreshness_;
        fNextAutoClearAt_             = rhs.fNextAutoClearAt_;
        fMap_                         = rhs.fMap_;
        fStats_                       = rhs.fStats_;
        return *this;
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline Time::Duration TimedCache<KEY, VALUE, TRAITS>::GetMinimumAllowedFreshness () const
    {
        shared_lock critSec{fMaybeMutex_};
        return Time::Duration{fMinimumAllowedFreshness_};
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::SetMinimumAllowedFreshness (Time::Duration minimumAllowedFreshness)
    {
        Require (minimumAllowedFreshness > 0.0s);
        scoped_lock critSec{fMaybeMutex_};
        if (fMinimumAllowedFreshness_ != minimumAllowedFreshness) {
            fMinimumAllowedFreshness_ = minimumAllowedFreshness;
            ClearOld_ (); // ClearOld_ not AutomaticallyPurgeExpiredDataSometimes_ to force auto-update of fNextAutoClearAt_, and cuz moderately likely items interestingly out of date after adjust of min allowed freshness
        }
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    auto TimedCache<KEY, VALUE, TRAITS>::Elements () const -> Traversal::Iterable<CacheElement>
    {
        shared_lock          critSec{fMaybeMutex_};
        vector<CacheElement> r;
        r.reserve (fMap_.size ());
        Time::TimePointSeconds lastAccessThreshold = Time::GetTickCount () - fMinimumAllowedFreshness_;
        for (const auto& i : fMap_) {
            if (i.second.fLastRefreshedAt >= lastAccessThreshold) {
                r.push_back (CacheElement{i.first, i.second.fResult, i.second.fLastRefreshedAt});
            }
        }
        return Traversal::Iterable<CacheElement>{move (r)};
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup (typename Common::ArgByValueType<KEY> key, Time::TimePointSeconds* lastRefreshedAt) const
    {
        shared_lock                         critSec{fMaybeMutex_};
        typename MyMapType_::const_iterator i   = fMap_.find (key);
        Time::TimePointSeconds              now = Time::GetTickCount ();
        if (i == fMap_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            Time::TimePointSeconds lastAccessThreshold = now - fMinimumAllowedFreshness_;
            if (i->second.fLastRefreshedAt < lastAccessThreshold) {
                /**
                 *  Cannot update fMap_ to indicate item expired const constant overload
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            if (lastRefreshedAt != nullptr) {
                *lastRefreshedAt = i->second.fLastRefreshedAt;
            }
            return i->second.fResult;
        }
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup (typename Common::ArgByValueType<KEY> key, Time::TimePointSeconds* lastRefreshedAt)
    {
        shared_lock                   critSec{fMaybeMutex_};
        typename MyMapType_::iterator i   = fMap_.find (key);
        Time::TimePointSeconds        now = Time::GetTickCount ();
        if (i == fMap_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            Time::TimePointSeconds lastAccessThreshold = now - fMinimumAllowedFreshness_;
            if constexpr (TRAITS::kAutomaticallyMarkDataAsRefreshedEachTimeAccessed) {
                i->second.fLastRefreshedAt = Time::GetTickCount ();
            }
            if (i->second.fLastRefreshedAt < lastAccessThreshold) {
                /**
                 *  since expired, remove from cache
                 */
                (void)fMap_.erase (i);
                fStats_.IncrementMisses ();
                return nullopt;
            }
            fStats_.IncrementHits ();
            if (lastRefreshedAt != nullptr) {
                *lastRefreshedAt = i->second.fLastRefreshedAt;
            }
            return i->second.fResult;
        }
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    template <Common::invocable_r<VALUE, KEY> CACHE_FILLTER_T>
    VALUE TimedCache<KEY, VALUE, TRAITS>::LookupValue (typename Common::ArgByValueType<KEY> key, CACHE_FILLTER_T&& cacheFiller)
    {
        auto&& readLock = shared_lock{fMaybeMutex_}; // try shared_lock for case where present, and then lose it if we need to update object
        if (optional<VALUE> o = Lookup (key)) {
            return *o;
        }
        else {
            /**
             *  unlocking the shared lock while fetching the new value (optionally with a write lock).
             */
            readLock.unlock (); // don't hold read lock, upgrade to write, and condition when we hold the write lock

            constexpr bool kHoldWriteLockDuringCacheFill = false;
            // never used true, and caused some trouble- need to invesigate
            // possibly add to TRAITS and retry
#if 0
            *  Note:   We choose to not hold any lock while filling the cache (fHoldWriteLockDuringCacheFill false by default).
            *  This is because typically, filling the cache
            *  will be slow (otherwise you would be us using the SynchronizedTimedCache).
            *
            *  But this has the downside, that you could try filling the cache multiple times with the same value.
            *
            *  Thats perfectly safe, but not speedy.
            *
            *  Which is better depends on the likihood the caller will make multiple requests for the same non-existent value at
            *  the same time. If yes, you should set fHoldWriteLockDuringCacheFill. If no (or if you care more about being able to
            *  read the rest of the data and not having threads block needlessly for other values) set fHoldWriteLockDuringCacheFill false (default).
#endif
            if constexpr (kHoldWriteLockDuringCacheFill) {
                // Avoid two threds calling cache for same key value at the same time
                [[maybe_unused]] auto&& newRWLock = scoped_lock{fMaybeMutex_};
                VALUE                   v         = cacheFiller (key);
                newRWLock.unlock ();
                Add (key, v); // if purgeSpoiledData must be done, do while holding lock
                return v;
            }
            else {
                VALUE v = cacheFiller (key);
                Add (key, v);
                return v;
            }
        }
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<VALUE> result)
    {
        scoped_lock critSec{fMaybeMutex_};
        AutomaticallyPurgeExpiredDataSometimes_ ();
        typename MyMapType_::iterator i = fMap_.find (key);
        if (i == fMap_.end ()) {
            fMap_.insert ({key, MyResult_{result}});
        }
        else {
            i->second = MyResult_{result}; // overwrite if its already there
        }
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<VALUE> result,
                                              Time::TimePointSeconds freshAsOf)
    {
        scoped_lock critSec{fMaybeMutex_};
        AutomaticallyPurgeExpiredDataSometimes_ ();
        fMap_.insert ({key, MyResult_{result, freshAsOf}});
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::Remove (typename Common::ArgByValueType<KEY> key)
    {
        scoped_lock critSec{fMaybeMutex_};
        fMap_.erase (key);
        AutomaticallyPurgeExpiredDataSometimes_ ();
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::clear ()
    {
        scoped_lock critSec{fMaybeMutex_};
        fMap_.clear ();
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::PurgeExpiredData ()
    {
        scoped_lock critSec{fMaybeMutex_};
        ClearOld_ ();
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::AutomaticallyPurgeExpiredDataSometimes_ ()
    {
        if constexpr (TRAITS::kAutomaticPurgeFrequency != Time::DurationSeconds{TimedCacheSupport::kNoAutomaticPurgeSentinal}) {
            if (fNextAutoClearAt_ < Time::GetTickCount ()) {
                ClearOld_ ();
                WeakAssert (fNextAutoClearAt_ > Time::GetTickCount ()); // note internally resets fNextAutoClearAt_
            }
        }
    }
    template <copyable KEY, copyable VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::ClearOld_ ()
    {
        Time::TimePointSeconds now                 = Time::GetTickCount ();
        Time::TimePointSeconds lastAccessThreshold = now - fMinimumAllowedFreshness_;
        for (typename MyMapType_::iterator i = fMap_.begin (); i != fMap_.end ();) {
            if (i->second.fLastRefreshedAt < lastAccessThreshold) {
                i = fMap_.erase (i);
            }
            else {
                ++i;
            }
        }
        if constexpr (TRAITS::kAutomaticPurgeFrequency != Time::DurationSeconds{TimedCacheSupport::kNoAutomaticPurgeSentinal}) {
            fNextAutoClearAt_ = now + TRAITS::kAutomaticPurgeFrequency;
        }
    }

}
