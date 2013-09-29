
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Cache_TimedCache_inl_
#define _Stroika_Foundation_Cache_TimedCache_inl_  1

#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {


            /*
             ********************************************************************************
             ************************ TimedCacheSupport::Stats_Basic ************************
             ********************************************************************************
             */
            inline  TimedCacheSupport::Stats_Basic::Stats_Basic ()
                : fCachedCollected_Hits (0)
                , fCachedCollected_Misses (0)
            {
            }
            inline  void    TimedCacheSupport::Stats_Basic::IncrementHits ()
            {
                fCachedCollected_Hits++;
            }
            inline  void    TimedCacheSupport::Stats_Basic::IncrementMisses ()
            {
                fCachedCollected_Misses++;
            }
            inline  void    TimedCacheSupport::Stats_Basic::DbgTraceStats (const SDKChar* label) const
            {
                size_t  total   =   fCachedCollected_Hits + fCachedCollected_Misses;
                if (total == 0) {
                    total = 1;  // avoid divide by zero
                }
                DbgTrace (SDKSTR ("%s stats: hits=%d, misses=%d, hit%% %f."), label, fCachedCollected_Hits, fCachedCollected_Misses, float (fCachedCollected_Hits) / (float (total)));
            }


            /*
             ********************************************************************************
             ************************ TimedCacheSupport::Stats_Null *************************
             ********************************************************************************
             */
            inline  void    TimedCacheSupport::Stats_Null::IncrementHits ()
            {
            }
            inline  void    TimedCacheSupport::Stats_Null::IncrementMisses ()
            {
            }
            inline  void    TimedCacheSupport::Stats_Null::DbgTraceStats (const SDKChar* label) const
            {
            }


            /*
             ********************************************************************************
             ************************ TimedCache<KEY,RESULT,TRAITS> *************************
             ********************************************************************************
             */
            template    <typename   KEY, typename RESULT, typename TRAITS>
            TimedCache<KEY, RESULT, TRAITS>::TimedCache (bool accessFreshensDate, Stroika::Foundation::Time::DurationSecondsType timeoutInSeconds)
                : fMap_ ()
                , fAccessFreshensDate_ (accessFreshensDate)
                , fTimeout_ (timeoutInSeconds)
                , fNextAutoClearAt_ (Time::GetTickCount () + timeoutInSeconds)
                , fStats ()
            {
                Require (fTimeout_ > 0.0f);
            }
            template    <typename   KEY, typename RESULT, typename TRAITS>
            void    TimedCache<KEY, RESULT, TRAITS>::SetTimeout (Stroika::Foundation::Time::DurationSecondsType timeoutInSeconds)
            {
                Require (timeoutInSeconds > 0.0f);
                lock_guard<mutex> critSec (fMutex_);
                if (fTimeout_ != timeoutInSeconds) {
                    ClearIfNeeded_ ();
                    fTimeout_ = timeoutInSeconds;
                    ClearIfNeeded_ ();
                }
            }
            template    <typename   KEY, typename RESULT, typename TRAITS>
            bool    TimedCache<KEY, RESULT, TRAITS>::AccessElement (const KEY& key, RESULT* result)
            {
                lock_guard<mutex> critSec (fMutex_);
                ClearIfNeeded_ ();
                typename map<KEY, MyResult_>::iterator i = fMap_.find (key);
                if (i == fMap_.end ()) {
                    fStats.IncrementMisses ();
                    return false;
                }
                else {
                    if (fAccessFreshensDate_) {
                        i->second.fLastAccessedAt = Time::GetTickCount ();
                    }
                    if (result != nullptr) {
                        *result = i->second.fResult;
                    }
                    fStats.IncrementHits ();
                    return true;
                }
            }
            template    <typename   KEY, typename RESULT, typename TRAITS>
            void    TimedCache<KEY, RESULT, TRAITS>::AddElement (const KEY& key, const RESULT& result)
            {
                lock_guard<mutex> critSec (fMutex_);
                ClearIfNeeded_ ();
                typename map<KEY, MyResult_>::iterator i = fMap_.find (key);
                if (i == fMap_.end ()) {
                    fMap_.insert (map<KEY, MyResult_>::value_type (key, MyResult_ (result)));
                }
                else {
                    i->second = MyResult_ (result);  // overwrite if its already there
                }
            }
            template    <typename   KEY, typename RESULT, typename TRAITS>
            inline  void    TimedCache<KEY, RESULT, TRAITS>::DoBookkeeping ()
            {
                lock_guard<mutex> critSec (fMutex_);
                ClearOld_ ();
            }
            template    <typename   KEY, typename RESULT, typename TRAITS>
            inline  void    TimedCache<KEY, RESULT, TRAITS>::ClearIfNeeded_ ()
            {
                if (fNextAutoClearAt_ < Time::GetTickCount ()) {
                    ClearOld_ ();
                }
            }
            template    <typename   KEY, typename RESULT, typename TRAITS>
            void    TimedCache<KEY, RESULT, TRAITS>::ClearOld_ ()
            {
                Stroika::Foundation::Time::DurationSecondsType  now =   Time::GetTickCount ();
                fNextAutoClearAt_ = now + fTimeout_ / 2.0f; // somewhat arbitrary how far into the future we do this...
                Stroika::Foundation::Time::DurationSecondsType  lastAccessThreshold =   now - fTimeout_;
                for (typename map<KEY, MyResult_>::iterator i = fMap_.begin (); i != fMap_.end (); ) {
                    if (i->second.fLastAccessedAt < lastAccessThreshold) {
                        i = fMap_.erase (i);
                    }
                    else {
                        ++i;
                    }
                }
            }


        }
    }
}


#endif  /*_Stroika_Foundation_Cache_TimedCache_inl_*/
