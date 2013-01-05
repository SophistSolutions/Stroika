/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_TimedCache_h_
#define _Stroika_Foundation_Cache_TimedCache_h_    1

#include    "../StroikaPreComp.h"



#include    <map>

#include    "../Configuration/Common.h"
#include    "../Characters/TChar.h"
#include    "../Debug/Assertions.h"
#include    "../Time/Realtime.h"



/// THIS MODULE SB OBSOLETE ONCE WE GET STROIKA Cache WORKING

/// NOT TOTALLY SURE WHY - REVIEW - THIS MAYBE USEFUL - SEE WHERE USED?


namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {

            using Stroika::Foundation::Characters::TChar;


            /*
            @CONFIGVAR:     qKeepTimedCacheStats
            @DESCRIPTION:   <p>Defines whether or not we capture statistics (for debugging purposes) in @'TimedCache<KEY,RESULT>'.
                        This should be ON by default - iff @'qDebug' is true.</p>
             */
#if     !defined (qKeepTimedCacheStats)
#error "qKeepTimedCacheStats should normally be defined indirectly by StroikaConfig.h"
#endif



            // Keeps track of all items - indexed by Key - but throws away items which are any more
            // stale than given by the TIMEOUT
            //
            // The only constraint on KEY is that it must have an operator== and operator<, and the only constraint on
            // both KEY and RESULT is that they must be copyable objects.
            //
            // Note - this class doesn't employ a thread to throw away old items, so if you count on that
            // happening (e.g. because the RESULT object DTOR has a side-effect like closing a file), then
            // you may call DoBookkeeping () peridocially.
            template    <typename   KEY, typename RESULT>
            class   TimedCache {
            public:
                TimedCache (bool accessFreshensDate, Time::DurationSecondsType timeoutInSeconds);

                nonvirtual  void    SetTimeout (Time::DurationSecondsType timeoutInSeconds);

                nonvirtual  bool    AccessElement (const KEY& key, RESULT* result);
                nonvirtual  void    AddElement (const KEY& key, const RESULT& result);

                nonvirtual  void    DoBookkeeping ();   // optional - need not be called

            private:
                bool                        fAccessFreshensDate;
                Time::DurationSecondsType   fTimeout;
                Time::DurationSecondsType   fNextAutoClearAt;

            private:
                nonvirtual  void    ClearIfNeeded_ ();
                nonvirtual  void    ClearOld_ ();

            private:
                struct  MyResult {
                    MyResult (const RESULT& r):
                        fResult (r),
                        fLastAccessedAt (Time::GetTickCount ()) {
                    }
                    RESULT  fResult;
                    Time::DurationSecondsType   fLastAccessedAt;
                };
                map<KEY, MyResult>   fMap;

#if     qKeepTimedCacheStats
            public:
                void    DbgTraceStats (const Characters::TChar* label) const;
            public:
                size_t      fCachedCollected_Hits;
                size_t      fCachedCollected_Misses;
#endif
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Cache_TimedCache_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TimedCache.inl"
