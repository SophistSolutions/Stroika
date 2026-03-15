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
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    TimedCache<KEY, VALUE, TRAITS>::TimedCache (const Time::Duration& minimumAllowedFreshness)
        : fMinimumAllowedFreshness_{minimumAllowedFreshness}
        , fNextAutoClearAt_{Time::GetTickCount () + minimumAllowedFreshness}
    {
        Require (fMinimumAllowedFreshness_ > 0.0s);
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline TimedCache<KEY, VALUE, TRAITS>::TimedCache (TimedCache&& src) noexcept
        : fMinimumAllowedFreshness_{src.fMinimumAllowedFreshness_}
        , fNextAutoClearAt_{src.fNextAutoClearAt_}
        , fMap_{move (src.fMap_)}
        , fStats_{move (src.fStats_)}
    {
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline TimedCache<KEY, VALUE, TRAITS>::TimedCache (const TimedCache& src)
        : fMinimumAllowedFreshness_{src.fMinimumAllowedFreshness_}
        , fNextAutoClearAt_{src.fNextAutoClearAt_}
        , fMap_{src.fMap_}
        , fStats_{src.fStats_}
    {
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto TimedCache<KEY, VALUE, TRAITS>::operator= (TimedCache&& rhs) noexcept -> TimedCache&
    {
        fMinimumAllowedFreshness_ = rhs.fMinimumAllowedFreshness_;
        fNextAutoClearAt_         = rhs.fNextAutoClearAt_;
        fMap_                     = move (rhs.fMap_);
        fStats_                   = move (rhs.fStats_);
        return *this;
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline auto TimedCache<KEY, VALUE, TRAITS>::operator= (const TimedCache& rhs) -> TimedCache&
    {
        fMinimumAllowedFreshness_ = rhs.fMinimumAllowedFreshness_;
        fNextAutoClearAt_         = rhs.fNextAutoClearAt_;
        fMap_                     = rhs.fMap_;
        fStats_                   = rhs.fStats_;
        return *this;
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline Time::Duration TimedCache<KEY, VALUE, TRAITS>::GetMinimumAllowedFreshness () const
    {
        shared_lock critSec{fMaybeMutex_};
        return Time::Duration{fMinimumAllowedFreshness_};
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::SetMinimumAllowedFreshness (Time::Duration minimumAllowedFreshness)
    {
        Require (minimumAllowedFreshness > 0.0s);
        scoped_lock critSec{fMaybeMutex_};
        if (fMinimumAllowedFreshness_ != minimumAllowedFreshness) {
            fMinimumAllowedFreshness_ = minimumAllowedFreshness;
            ClearOld_ (); // ClearOld_ not ClearIfNeeded_ to force auto-update of fNextAutoClearAt_, and cuz moderately likely items interestingly out of date after adjust of min allowed freshness
        }
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    auto TimedCache<KEY, VALUE, TRAITS>::Elements () const -> Traversal::Iterable<CacheElement>
    {
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
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
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
                 *  Before Stroika 3.0d1, we used to remove the entry from the list (an optimization). But
                 * that required Lookup to be non-const (with synchronization in mind probably a pessimization).
                 * So instead, count on PurgeUnusedData being called automatically on future adds,
                 * explicit user calls to purge unused data.
                 *
                 *      i = fMap_.erase (i);
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
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup (typename Common::ArgByValueType<KEY> key, LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag)
    {
        scoped_lock                   critSec{fMaybeMutex_};
        typename MyMapType_::iterator i   = fMap_.find (key);
        Time::TimePointSeconds        now = Time::GetTickCount ();
        if (i == fMap_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            Time::TimePointSeconds lastAccessThreshold = now - fMinimumAllowedFreshness_;
            if (i->second.fLastRefreshedAt < lastAccessThreshold) {
                /**
                 *  Before Stroika 3.0d1, we used to remove the entry from the list (an optimization). But
                 * that required Lookup to be non-const (with synchronization in mind probably a pessimization).
                 * So instead, count on PurgeUnusedData being called automatically on future adds,
                 * explicit user calls to purge unused data.
                 *
                 *      i = fMap_.erase (i);
                 */
                fStats_.IncrementMisses ();
                return nullopt;
            }
            if (successfulLookupRefreshesAcceesFlag == LookupMarksDataAsRefreshed::eTreatFoundThroughLookupAsRefreshed) {
                i->second.fLastRefreshedAt = Time::GetTickCount ();
            }
            fStats_.IncrementHits ();
            return i->second.fResult;
        }
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    VALUE TimedCache<KEY, VALUE, TRAITS>::LookupValue (typename Common::ArgByValueType<KEY>                          key,
                                                       const function<VALUE (typename Common::ArgByValueType<KEY>)>& cacheFiller,
                                                       LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag,
                                                       PurgeSpoiledDataFlagType   purgeSpoiledData)
    {
        if (optional<VALUE> o = Lookup (key, successfulLookupRefreshesAcceesFlag)) {
            return *o;
        }
        else {
            VALUE v = cacheFiller (key);
            Add (key, v, purgeSpoiledData);
            return v;
        }
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<VALUE> result,
                                              PurgeSpoiledDataFlagType prgeSpoiledData)
    {
        scoped_lock critSec{fMaybeMutex_};
        if (prgeSpoiledData == PurgeSpoiledDataFlagType::eAutomaticallyPurgeSpoiledData) {
            ClearIfNeeded_ ();
        }
        typename MyMapType_::iterator i = fMap_.find (key);
        if (i == fMap_.end ()) {
            fMap_.insert ({key, MyResult_{result}});
        }
        else {
            i->second = MyResult_{result}; // overwrite if its already there
        }
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::Add (typename Common::ArgByValueType<KEY> key, typename Common::ArgByValueType<VALUE> result,
                                              Time::TimePointSeconds freshAsOf)
    {
        scoped_lock critSec{fMaybeMutex_};
        fMap_.insert ({key, MyResult_{result, freshAsOf}});
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::Remove (typename Common::ArgByValueType<KEY> key)
    {
        scoped_lock critSec{fMaybeMutex_};
        fMap_.erase (key);
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::clear ()
    {
        scoped_lock critSec{fMaybeMutex_};
        fMap_.clear ();
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::PurgeSpoiledData ()
    {
        scoped_lock critSec{fMaybeMutex_};
        ClearOld_ ();
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::ClearIfNeeded_ ()
    {
        if (fNextAutoClearAt_ < Time::GetTickCount ()) {
            ClearOld_ ();
        }
    }
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::ClearOld_ ()
    {
        Time::TimePointSeconds now = Time::GetTickCount ();
        fNextAutoClearAt_          = now + fMinimumAllowedFreshness_ * 0.75; // somewhat arbitrary how far into the future we do this...
        Time::TimePointSeconds lastAccessThreshold = now - fMinimumAllowedFreshness_;
        for (typename MyMapType_::iterator i = fMap_.begin (); i != fMap_.end ();) {
            if (i->second.fLastRefreshedAt < lastAccessThreshold) {
                i = fMap_.erase (i);
            }
            else {
                ++i;
            }
        }
    }

}
