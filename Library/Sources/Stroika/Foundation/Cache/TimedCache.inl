
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
    TimedCache<KEY, VALUE, TRAITS>::TimedCache (Stroika::Foundation::Time::DurationSecondsType timeoutInSeconds)
        : fTimeout_ (timeoutInSeconds)
        , fNextAutoClearAt_ (Time::GetTickCount () + timeoutInSeconds)
    {
        Require (fTimeout_ > 0.0f);
    }
	template <typename KEY, typename VALUE, typename TRAITS>
	Time::DurationSecondsType TimedCache<KEY, VALUE, TRAITS>::GetTimeout () const
	{
		shared_lock<const AssertExternallySynchronizedLock> critSec{ *this };
		return fTimeout_;
	}

    template <typename KEY, typename VALUE, typename TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::SetTimeout (Stroika::Foundation::Time::DurationSecondsType timeoutInSeconds)
    {
        Require (timeoutInSeconds > 0.0f);
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        if (fTimeout_ != timeoutInSeconds) {
            ClearIfNeeded_ ();
            fTimeout_ = timeoutInSeconds;
            ClearIfNeeded_ ();
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    optional<VALUE> TimedCache<KEY, VALUE, TRAITS>::Lookup (typename Configuration::ArgByValueType<KEY> key)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        ClearIfNeeded_ ();
        typename MyMapType_::iterator i   = fMap_.find (key);
        Time::DurationSecondsType     now = Time::GetTickCount ();
        if (i == fMap_.end ()) {
            this->IncrementMisses ();
            return nullopt;
        }
        else {
            Stroika::Foundation::Time::DurationSecondsType lastAccessThreshold = now - fTimeout_;
            if (i->second.fLastAccessedAt < lastAccessThreshold) {
                i = fMap_.erase (i);
                this->IncrementMisses ();
                return nullopt;
            }
            if (TraitsType::kTrackReadAccess) {
                i->second.fLastAccessedAt = Time::GetTickCount ();
            }
            this->IncrementHits ();
            return optional<VALUE> (i->second.fResult);
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    VALUE TimedCache<KEY, VALUE, TRAITS>::LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller)
    {
        if (optional<VALUE> o = Lookup (key)) {
            return *o;
        }
        else {
            VALUE v = cacheFiller (key);
            Add (key, v);
            return move (v);
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void TimedCache<KEY, VALUE, TRAITS>::Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        ClearIfNeeded_ ();
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
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fMap_.erase (key);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::clear ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fMap_.clear ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline void TimedCache<KEY, VALUE, TRAITS>::DoBookkeeping ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
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
