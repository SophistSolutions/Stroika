
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_TimedCache_inl_
#define _Stroika_Foundation_Containers_TimedCache_inl_  1

#include    "../Debug/Assertions.h"
#include    "../Debug/Trace.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            template    <typename   KEY, typename RESULT>
            TimedCache<KEY, RESULT>::TimedCache (bool accessFreshensDate, Stroika::Foundation::Time::DurationSecondsType timeoutInSeconds):
                fMap (),
                fAccessFreshensDate (accessFreshensDate),
                fTimeout (timeoutInSeconds),
                fNextAutoClearAt (Time::GetTickCount () + timeoutInSeconds)
#if     qKeepTimedCacheStats
                , fCachedCollected_Hits (0)
                , fCachedCollected_Misses (0)
#endif
            {
                Require (fTimeout > 0.0f);
            }
            template    <typename   KEY, typename RESULT>
            void    TimedCache<KEY, RESULT>::SetTimeout (Stroika::Foundation::Time::DurationSecondsType timeoutInSeconds) {
                Require (timeoutInSeconds > 0.0f);
                if (fTimeout != timeoutInSeconds) {
                    ClearIfNeeded_ ();
                    fTimeout = timeoutInSeconds;
                    ClearIfNeeded_ ();
                }
            }
            template    <typename   KEY, typename RESULT>
            bool    TimedCache<KEY, RESULT>::AccessElement (const KEY& key, RESULT* result) {
                ClearIfNeeded_ ();
                typename map<KEY, MyResult>::iterator i = fMap.find (key);
                if (i == fMap.end ()) {
#if     qKeepTimedCacheStats
                    fCachedCollected_Misses++;
#endif
                    return false;
                }
                else {
                    if (fAccessFreshensDate) {
                        i->second.fLastAccessedAt = Time::GetTickCount ();
                    }
                    if (result != nullptr) {
                        *result = i->second.fResult;
                    }
#if     qKeepTimedCacheStats
                    fCachedCollected_Hits++;
#endif
                    return true;
                }
            }
            template    <typename   KEY, typename RESULT>
            void    TimedCache<KEY, RESULT>::AddElement (const KEY& key, const RESULT& result) {
                ClearIfNeeded_ ();
                typename map<KEY, MyResult>::iterator i = fMap.find (key);
                if (i == fMap.end ()) {
                    fMap.insert (map<KEY, MyResult>::value_type (key, MyResult (result)));
                }
                else {
                    i->second = MyResult (result);  // overwrite if its already there
                }
            }
            template    <typename   KEY, typename RESULT>
            inline  void    TimedCache<KEY, RESULT>::DoBookkeeping () {
                ClearOld_ ();
            }
            template    <typename   KEY, typename RESULT>
            inline  void    TimedCache<KEY, RESULT>::ClearIfNeeded_ () {
                if (fNextAutoClearAt < Time::GetTickCount ()) {
                    ClearOld_ ();
                }
            }
            template    <typename   KEY, typename RESULT>
            void    TimedCache<KEY, RESULT>::ClearOld_ () {
                Stroika::Foundation::Time::DurationSecondsType  now =   Time::GetTickCount ();
                fNextAutoClearAt = now + fTimeout / 2.0f; // somewhat arbitrary how far into the future we do this...
                Stroika::Foundation::Time::DurationSecondsType  lastAccessThreshold =   now - fTimeout;
                for (typename map<KEY, MyResult>::iterator i = fMap.begin (); i != fMap.end (); ) {
                    if (i->second.fLastAccessedAt < lastAccessThreshold) {
                        i = fMap.erase (i);
                    }
                    else {
                        ++i;
                    }
                }
            }
#if     qKeepTimedCacheStats
            template    <typename   KEY, typename RESULT>
            void    TimedCache<KEY, RESULT>::DbgTraceStats (const TChar* label) const {
                size_t  total   =   fCachedCollected_Hits + fCachedCollected_Misses;
                if (total == 0) {
                    total = 1;  // avoid divide by zero
                }
                DbgTrace (TSTR ("%s stats: hits=%d, misses=%d, hit%% %f."), label, fCachedCollected_Hits, fCachedCollected_Misses, float (fCachedCollected_Hits) / (float (total)));
            }
#endif


        }
    }
}


#endif  /*_Stroika_Foundation_Containers_TimedCache_inl_*/
