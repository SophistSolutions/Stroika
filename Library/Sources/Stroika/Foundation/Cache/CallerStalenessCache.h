/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_CallerStalenessCache_h_
#define _Stroika_Foundation_Cache_CallerStalenessCache_h_ 1

#include "../StroikaPreComp.h"

#include "../Containers/Mapping.h"
#include "../Time/Realtime.h"

/**
 *      \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-449 and Cache::CallerStalenessCache should support
 *              second template argument VALUE=void
 *
 *      @todo   Add Debug::AssertExternallySynchronizedLock usage.
 *
 *      @todo   Consider defect - easy to misinterpret 'TimeStampType staleIfOlderThan' arg to Lookup()
 *              as offset (see Ago() API), instead of timestamp to compare with timestamp on data.
 *
 *      @todo   Consider adding way to retreive timestamp for key 'k'. Also consider Iterable<> method (like LRUCache)
 *              so we can dump cache (including timestamps)
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Cache {

            /**
             */
            struct CallerStalenessCache_Traits_DEFAULT {
                using TimeStampType = Time::DurationSecondsType; // type must support operator<()
                static TimeStampType GetCurrentTimestamp ();
            };

            /**
             *  The idea behind this cache is to track when something is added, and that the lookup function can avoid
             *  a costly call to compute something if its been recently enough added.
             *
             *  For example, consider a system where memory is stored across a slow bus, and several components need to read data from
             *  across that bus. But the different components have different tolerance for staleness.
             *
             *  This CallerStalenessCache will store when the value is updated, and let the caller either return the
             *  value from cache, or fetch it and update the cache if needed.
             *
             *  This differs from other forms of caches in that:
             *      o   It records the timestamp when a value is last-updated
             *      o   It doesn't EXPIRE the data ever (except by explicit Clear or ClearOlderThan call)
             *      o   The lookup caller specifies its tollerance for data staleness, and refreshes the data as needed.
             *
             *  \par Example usage:
             *      Hardware connected via a (slow) serial bus with a bunch of 'registers' that can be read. Sometimes
             *      the caller really needs an up to date value (say for a PID loop), and sometimes the caller can accept older data 
             *      (say to power a remote UI). This lets the caller share the latest values, and only force a call across the bus (slow load)
             *      when the caller needs
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
             *
             *  @see TimedCache
             */
            template <typename KEY, typename VALUE, typename TIME_TRAITS = CallerStalenessCache_Traits_DEFAULT>
            class CallerStalenessCache {
            public:
                /**
                 */
                using TimeStampType = typename TIME_TRAITS::TimeStampType;

            public:
                /**
                 */
                static TimeStampType GetCurrentTimestamp ();

            public:
                /**
                 *  Return the timestamp backwards the given timestamp.
                 *
                 *  \req backThisTime >= 0
                 *
                 *  \par Example Usage
                 *      \code
                 *      CallerStalenessCache<> cc;
                 *      if (Optional<VALUE> v= cc.Lookup (k, cc.Ago (5)) {
                 *          // look key, but throw disregard if older than 5 seconds (from now)
                 *      }
                 *      \endcode
                 */
                static TimeStampType Ago (TimeStampType backThisTime);

            public:
                /**
                 */
                nonvirtual void ClearOlderThan (TimeStampType t);

            public:
                /**
                 */
                nonvirtual void Clear ();
                nonvirtual void Clear (KEY k);

            public:
                /**
                 *  This not only adds the association of KEY k to VALUE v, but updates the timestamp associated with k.
                 */
                nonvirtual void Add (KEY k, VALUE v);

            public:
                /**
                 *  Usually one will use this as
                 *      VALUE v = cache.Lookup (key, ts, [this] () -> VALUE {return this->realLookup(key); });
                 *
                 *  However, the overload returing an optional is occasionally useful, if you dont want to fill the cache
                 *  but just see if a value is present.
                 *
                 *  Both the overload with cacheFiller, and defaultValue will update the 'time stored' for the argument key.
                 *
                 *  \note   These Lookup () methods are not const intentionally - as they DO generally modify the cache.
                 */
                nonvirtual Memory::Optional<VALUE> Lookup (KEY k, TimeStampType staleIfOlderThan);
                nonvirtual VALUE Lookup (KEY k, TimeStampType staleIfOlderThan, const std::function<VALUE ()>& cacheFiller);
                nonvirtual VALUE Lookup (KEY k, TimeStampType staleIfOlderThan, const VALUE& defaultValue);

            public:
                /**
                 * \brief STL-ish alias for Clear/RemoveAll ().
                 */
                nonvirtual void clear ();

            private:
                struct myVal_ {
                    VALUE         fValue;
                    TimeStampType fDataCapturedAt;
                    myVal_ (VALUE&& v, TimeStampType t)
                        : fValue (std::move (v))
                        , fDataCapturedAt (t)
                    {
                    }
                };

            private:
                Containers::Mapping<KEY, myVal_> fMap_;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CallerStalenessCache.inl"

#endif /*_Stroika_Foundation_Cache_CallerStalenessCache_h_*/
