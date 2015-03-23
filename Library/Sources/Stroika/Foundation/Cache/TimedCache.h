/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_TimedCache_h_
#define _Stroika_Foundation_Cache_TimedCache_h_    1

#include    "../StroikaPreComp.h"

#include    <map>
#include    <mutex>

#include    "../Configuration/Common.h"
#include    "../Characters/SDKChar.h"
#include    "../Debug/Assertions.h"
#include    "../Debug/AssertExternallySynchronizedLock.h"
#include    "../Memory/Optional.h"
#include    "../Time/Realtime.h"



/**
 *      \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 *      @todo   This class is logically a map. But you may want to have individual values with timed cache!
 *              Basically - KEY=RESULT? And then the arg to add/lookup dont take key? Maybe key is void?
 *
 *              That maybe best. Template specialization where KEY=void?
 *
 *              THEN - maybe reverse order of template params? VALUE/KEY - so then we can have KEY=void as default
 *              arg?
 *
 *      @todo   Use Concepts or other such constraint on T/ELEMENT declarations (and docs)
 *
 *      @todo   Add regression test for this class
 *
 *      @todo   Perhaps use Stroika Mapping<> instead of std::map<> - and in that way - we can use aribtrary externally
 *              specified map impl - so can use HASHING or BTREE, based on passed in arg. So we dont ahve problem with
 *              creating the default, specify default type to create in the TRAITS object (so for example, if using Hash,
 *              we dont force having operator< for BTREE map).
 *
 *      @todo   Move accessFreshensDate into DefaultTraits
 *
 *      @todo   add bool option to TRAITS for 'accessDoesBookkeeping' - if false, then this ASSUMES/REQUIRES
 *              caller respponsability to arragen for periodic call of bookkeeping task.
 *
 *  Implementation Note:
 *
 *      This module uses stl:map<> instead of a Stroika Mapping since we are comfortable with
 *      the current implementation using btree's, and to avoid any dependencies between
 *      Caching and Containers. We may want to re-think that, and just  use Mapping here.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {

            using Stroika::Foundation::Characters::SDKChar;


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
                    nonvirtual  void    DbgTraceStats (const Characters::SDKChar* label) const;
                };


                /**
                 *  Helper for DefaultTraits - when not collecting stats.
                 */
                struct  Stats_Null {
                    nonvirtual  void    IncrementHits ();
                    nonvirtual  void    IncrementMisses ();
                    nonvirtual  void    DbgTraceStats (const Characters::SDKChar* label) const;
                };


                /**
                 * The DefaultTraits<> is a simple default traits implementation for building an TimedCache<>.
                 */
                template    <typename   KEY, typename RESULT>
                struct  DefaultTraits {
                    using   KeyType     =   KEY;
                    using   ResultType  =   RESULT;

#if     qDebug
                    using   StatsType   =   Stats_Basic;
#else
                    using   StatsType   =   Stats_Null;
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
             *
             *  EXAMPLE - USE DNS CACHE... - or current use for LDAP lookups
             *
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename   KEY, typename RESULT, typename TRAITS = TimedCacheSupport::DefaultTraits<KEY, RESULT>>
            class   TimedCache : private Debug::AssertExternallySynchronizedLock {
            public:
                TimedCache (bool accessFreshensDate, Time::DurationSecondsType timeoutInSeconds);
                TimedCache (const TimedCache&) = delete;

            public:
                nonvirtual  TimedCache& operator= (const TimedCache&) = delete;

            public:
                nonvirtual  void    SetTimeout (Time::DurationSecondsType timeoutInSeconds);

            public:
                /**
                 *  Lookup the given value and return it if its in the Cache.
                 */
                nonvirtual  Memory::Optional<RESULT>    AccessElement (const KEY& key);
                nonvirtual  bool                        AccessElement (const KEY& key, RESULT* result);

            public:
                nonvirtual  void    AddElement (const KEY& key, const RESULT& result);

            public:
                nonvirtual  void    Remove (const KEY& key);

            public:
                nonvirtual  void    DoBookkeeping ();   // optional - need not be called

            public:
                /**
                 *  This can be peeked at (or reset). This is purely advisory.
                 */
                typename TRAITS::StatsType  fStats;

            private:
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
                        , fLastAccessedAt (Time::GetTickCount ())
                    {
                    }
                    RESULT                      fResult;
                    Time::DurationSecondsType   fLastAccessedAt;
                };
                map<KEY, MyResult_>   fMap_;
            };


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TimedCache.inl"

#endif  /*_Stroika_Foundation_Cache_TimedCache_h_*/
