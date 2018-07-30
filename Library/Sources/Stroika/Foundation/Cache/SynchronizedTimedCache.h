/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_SynchronizedTimedCache_h_
#define _Stroika_Foundation_Cache_SynchronizedTimedCache_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <shared_mutex>

#include "TimedCache.h"

/**
 *      \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   Add 'overrides' for Add/Remove methods - so those are safe too!
 *              and add more overloads (from base class) of Lookup
 */

namespace Stroika::Foundation::Cache {

    /**
     *  @see TimedCache but internally syncrhonized. You could use Syncrhonized<TimedCache>, but this is simpler to use and
     *  performs better, due to not write locking until the last minute needed (you expect a cache to mostly be read
     *  from and have writes - cache misses - expensive/slow but not slow the rest of the cache (hits).
     */
    template <typename KEY, typename VALUE, typename TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    class SynchronizedTimedCache : public TimedCache<KEY, VALUE, TRAITS> {
        using inherited = TimedCache<KEY, VALUE, TRAITS>;

    public:
        /**
         *  @see TimedCache constructor for examples
         */
        SynchronizedTimedCache (Time::DurationSecondsType timeoutInSeconds);
        // support eventually, but not trivial
        SynchronizedTimedCache (const SynchronizedTimedCache&) = delete;

    public:
        // support eventually, but not trivial
        nonvirtual SynchronizedTimedCache& operator= (const SynchronizedTimedCache&) = delete;

    public:
        /*
         *  Note:   We choose to not hold any lock while filling the cache (fHoldWriteLockDuringCacheFill false by default).
         *  This is because typically, filling the cache
         *  will be slow (otherwise you wouldbe us using the SynchronizedTimedCache).
         *
         *  But this has the downside, that you could try filling the cache multiple times with the same value.
         *
         *  That's perfectly safe, but not speedy.
         *
         *  Which is better depends on the likihood the caller will make multiple requests for the same non-existent value at
         *  the same time. If yes, you should set fHoldWriteLockDuringCacheFill. If no (or if you care more about being able to
         *  read the rest of the data and not having threads block needlessly for other values) set fHoldWriteLockDuringCacheFill false (default).
         */
        bool fHoldWriteLockDuringCacheFill{false};

    public:
        /**
         *  Lookup the given value and return it. But if its not found, use argument 'cacheFiller' function to fetch the real value.
         *  This supports (and propagates) exceptions, and is threadsafe (can be called multiple times from different threads safely.
         */
        nonvirtual VALUE Lookup (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller);

    private:
        mutable shared_timed_mutex fMutex_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SynchronizedTimedCache.inl"

#endif /*_Stroika_Foundation_Cache_SynchronizedTimedCache_h_*/
