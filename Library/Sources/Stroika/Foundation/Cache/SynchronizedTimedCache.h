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
     *  @see TimedCache but internally synchronized. You could use Synchronized<TimedCache>, but this is simpler to use and
     *  performs better, due to not write locking until the last minute needed (you expect a cache to mostly be read
     *  from and have writes - cache misses - expensive/slow but not slow the rest of the cache (hits).
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     *  @see TimedCache<> - for unsynchonized implementation
     *
     */
    template <typename KEY, typename VALUE, typename TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    class SynchronizedTimedCache : private TimedCache<KEY, VALUE, TRAITS> {
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
          * @see TimedCache::SetTimeout
          */
        nonvirtual void SetTimeout (Time::DurationSecondsType timeoutInSeconds);

    public:
        /**
          * @see TimedCache::Lookup
         */
        nonvirtual optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key);

    public:
        nonvirtual VALUE LookupValue (typename Configuration::ArgByValueType<KEY> key, const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller);

    public:
        /**
          * @see TimedCache::Add
         */
        nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result);

    public:
        /**
          * @see TimedCache::Remove
         */
        nonvirtual void Remove (typename Configuration::ArgByValueType<KEY> key);

    public:
        /**
          * @see TimedCache::clear
         */
        nonvirtual void clear ();

    public:
        /**
         * @see TimedCache::DoBookkeeping
         */
        nonvirtual void DoBookkeeping ();

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
