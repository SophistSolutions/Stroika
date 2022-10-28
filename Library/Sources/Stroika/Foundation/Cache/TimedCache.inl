
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Cache_TimedCache_inl_
#define _Stroika_Foundation_Cache_TimedCache_inl_ 1

#include "../Containers/STL/Compare.h"
#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"
#include "../Execution/Common.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ************************* TimedCache<KEY,VALUE,TRAITS> *************************
     ********************************************************************************
     */
    template <typename KEY, typename VALUE, typename TRAITS>
    TimedCache<KEY, VALUE, TRAITS>::TimedCache (const Time::Duration& timeout)
        : fTimeout_{timeout.As<Time::DurationSecondsType> ()}
        , fNextAutoClearAt_{Time::GetTickCount () + timeout.As<Time::DurationSecondsType> ()}
    {
        Require (fTimeout_ > 0.0f);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    Time::Duration TimedCache<KEY, VALUE, TRAITS>::GetMinimumAllowedFreshness () const
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*this};
        return fTimeout_;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::SetMinimumAllowedFreshness (Stroika::Foundation::Time::Duration timeoutInSeconds)
    {
        Require (timeoutInSeconds > 0.0s);
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        if (fTimeout_ != timeoutInSeconds) {
            fTimeout_ = timeoutInSeconds.As<Time::DurationSecondsType> ();
            ClearIfNeeded_ ();
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key, Time::DurationSecondsType* lastRefreshedAt) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*this};
        typename MyMapType_::const_iterator                  i   = fMap_.find (key);
        Time::DurationSecondsType                            now = Time::GetTickCount ();
        if (i == fMap_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            Stroika::Foundation::Time::DurationSecondsType lastAccessThreshold = now - fTimeout_;
            if (i->second.fLastAccessedAt < lastAccessThreshold) {
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
                *lastRefreshedAt = i->second.fLastAccessedAt;
            }
            return i->second.fResult;
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key, LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag)
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        typename MyMapType_::iterator                       i   = fMap_.find (key);
        Time::DurationSecondsType                           now = Time::GetTickCount ();
        if (i == fMap_.end ()) {
            fStats_.IncrementMisses ();
            return nullopt;
        }
        else {
            Stroika::Foundation::Time::DurationSecondsType lastAccessThreshold = now - fTimeout_;
            if (i->second.fLastAccessedAt < lastAccessThreshold) {
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
                i->second.fLastAccessedAt = Time::GetTickCount ();
            }
            fStats_.IncrementHits ();
            return i->second.fResult;
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    VALUE TimedCache<KEY, VALUE, TRAITS>::LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller, LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag)
    {
        if (optional<VALUE> o = Lookup (key, successfulLookupRefreshesAcceesFlag)) {
            return *o;
        }
        else {
            VALUE v = cacheFiller (key);
            Add (key, v);
            return v;
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result, PurgeSpoiledDataFlagType prgeSpoiledData)
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        if (prgeSpoiledData == PurgeSpoiledDataFlagType::eAutomaticallyPurgeSpoiledData) {
            ClearIfNeeded_ ();
        }
        typename MyMapType_::iterator i = fMap_.find (key);
        if (i == fMap_.end ()) {
            fMap_.insert (typename MyMapType_::value_type (key, MyResult_ (result)));
        }
        else {
            i->second = MyResult_ (result); // overwrite if its already there
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::Remove (typename Configuration::ArgByValueType<KEY> key)
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        fMap_.erase (key);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::clear ()
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        fMap_.clear ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::PurgeSpoiledData ()
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        ClearOld_ ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::ClearIfNeeded_ ()
    {
        if (fNextAutoClearAt_ < Time::GetTickCount ()) {
            ClearOld_ ();
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::ClearOld_ ()
    {
        Stroika::Foundation::Time::DurationSecondsType now                 = Time::GetTickCount ();
        fNextAutoClearAt_                                                  = now + fTimeout_ * 0.75; // somewhat arbitrary how far into the future we do this...
        Stroika::Foundation::Time::DurationSecondsType lastAccessThreshold = now - fTimeout_;
        for (typename MyMapType_::iterator i = fMap_.begin (); i != fMap_.end ();) {
            if (i->second.fLastAccessedAt < lastAccessThreshold) {
                i = fMap_.erase (i);
            }
            else {
                ++i;
            }
        }
    }

}

#endif /*_Stroika_Foundation_Cache_TimedCache_inl_*/
