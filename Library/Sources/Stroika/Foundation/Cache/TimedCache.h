/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_TimedCache_h_
#define _Stroika_Foundation_Cache_TimedCache_h_    1

#include    "../StroikaPreComp.h"

#include    <map>
#include    <mutex>

#include    "../Configuration/Common.h"
#include    "../Characters/TChar.h"
#include    "../Debug/Assertions.h"
#include    "../Time/Realtime.h"


/**
 *      \file
 *
 * TODO:
 *      @todo   Use Concepts or other such constraint on T/ELEMENT declarations (and docs)
 *
 *      @todo   Add regression test for this class
 *
 *      @todo   Add mutex and docs about thread safety
 *
 *      @todo   Move accessFreshensDate into DefaultTraits
 *
 *      @todo   add bool option to TRAITS for 'accessDoesBookkeeping' - if false, then this ASSUMES/REQUIRES
 *              caller respponsability to arragen for periodic call of bookkeeping task.
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {

            using Stroika::Foundation::Characters::TChar;


            namespace   TimedCacheSupport {


                /**
                 *  Helper detail class for analyzing and tuning cache statistics.
                 */
                struct  Stats_Basic {
                    Stats_Basic ();
                    size_t      fCachedCollected_Hits;
                    size_t      fCachedCollected_Misses;

                    nonvirtual  void    IncrementHits ();
                    nonvirtual  void    IncrementMisses ();
                    nonvirtual  void    DbgTraceStats (const Characters::TChar* label) const;
                };


                /**
                 *  Helper for DefaultTraits - when not collecting stats.
                 */
                struct  Stats_Null {
                    nonvirtual  void    IncrementHits ();
                    nonvirtual  void    IncrementMisses ();
                    nonvirtual  void    DbgTraceStats (const Characters::TChar* label) const;
                };


                /**
                 * The DefaultTraits<> is a simple default traits implementation for building an TimedCache<>.
                 */
                template    <typename   KEY, typename RESULT>
                struct  DefaultTraits {
                    typedef KEY         KeyType;
                    typedef RESULT      ResultType;

#if     qDebug
                    typedef Stats_Basic     StatsType;
#else
                    typedef Stats_Null      StatsType;
#endif
                };


            }






            /**
             *
             *  Keeps track of all items - indexed by Key - but throws away items which are any more
             *  stale than given by the TIMEOUT
             *
             *  The only constraint on KEY is that it must have an operator== and operator<, and the only constraint on
             *  both KEY and RESULT is that they must be copyable objects.
             *
             *  Note - this class doesn't employ a thread to throw away old items, so if you count on that
             *  happening (e.g. because the RESULT object DTOR has a side-effect like closing a file), then
             *  you may call DoBookkeeping () peridocially.
             */
            template    <typename   KEY, typename RESULT, typename TRAITS = TimedCacheSupport::DefaultTraits<KEY, RESULT>>
            class   TimedCache {
            public:
                TimedCache (bool accessFreshensDate, Time::DurationSecondsType timeoutInSeconds);

            public:
                nonvirtual  void    SetTimeout (Time::DurationSecondsType timeoutInSeconds);

            public:
                nonvirtual  bool    AccessElement (const KEY& key, RESULT* result);

            public:
                nonvirtual  void    AddElement (const KEY& key, const RESULT& result);

            public:
                nonvirtual  void    DoBookkeeping ();   // optional - need not be called

            public:
                typename TRAITS::StatsType  fStats;

            private:
                mutable mutex               fMutex_;
                bool                        fAccessFreshensDate_;
                Time::DurationSecondsType   fTimeout_;
                Time::DurationSecondsType   fNextAutoClearAt_;

            private:
                nonvirtual  void    ClearIfNeeded_ ();
                nonvirtual  void    ClearOld_ ();

            private:
                struct  MyResult_ {
                    MyResult_ (const RESULT& r)
                        : fResult (r)
                        , fLastAccessedAt (Time::GetTickCount ()) {
                    }
                    RESULT  fResult;
                    Time::DurationSecondsType   fLastAccessedAt;
                };
                map<KEY, MyResult_>   fMap_;
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
