/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_SynchronizedTimedCache_h_
#define _Stroika_Foundation_Cache_SynchronizedTimedCache_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>
#include <shared_mutex>

#include "Stroika/Foundation/Cache/TimedCache.h"

/**
 *      \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Cache {

    /**
     *  @see TimedCache but internally synchronized. You could use Synchronized<TimedCache>, but this is simpler to use and
     *  performs better, due to not write locking until the last minute needed (you expect a cache to mostly be read
     *  from and have writes - cache misses - expensive/slow but not slow the rest of the cache (hits).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     *  @see TimedCache<> - for unsynchronized implementation
     *
     */
    template <typename KEY, typename VALUE, typename TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    class SynchronizedTimedCache : private TimedCache<KEY, VALUE, TRAITS> {
    private:
        using inherited = TimedCache<KEY, VALUE, TRAITS>;

    public:
        using LookupMarksDataAsRefreshed = typename inherited::LookupMarksDataAsRefreshed;

    public:
        using PurgeSpoiledDataFlagType = typename inherited::PurgeSpoiledDataFlagType;

    public:
        using CacheElement = typename inherited::CacheElement;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          optional<String> ReverseDNSLookup_ (const InternetAddress& inetAddr)
         *          {
         *              static const Time::Duration                                             kCacheTTL_{5min}; // @todo fix when Stroika Duration bug supports constexpr this should
         *              static Cache::SynchronizedTimedCache<InternetAddress, optional<String>> sCache_{kCacheTTL_};
         *              try {
         *                  return sCache_.LookupValue (inetAddr, [] (const InternetAddress& inetAddr) {
         *                      return DNS::kThe.ReverseLookup (inetAddr);
         *                  });
         *              }
         *              catch (...) {
         *                  // NOTE - to NEGATIVELY CACHE failure, you could call sCache_.Add (inetAddr, nullopt);
         *                  return nullopt; // if DNS is failing, just dont do this match, dont abandon all data collection
         *              }
         *          }
         *      \endcode
         *
         *  @see TimedCache constructor for more examples
         */
        SynchronizedTimedCache (const Time::Duration& minimumAllowedFreshness);
        SynchronizedTimedCache (const SynchronizedTimedCache& src);

    public:
        // support eventually, but not trivial
        nonvirtual SynchronizedTimedCache& operator= (const SynchronizedTimedCache&) = delete;

    public:
        /*
         *  Note:   We choose to not hold any lock while filling the cache (fHoldWriteLockDuringCacheFill false by default).
         *  This is because typically, filling the cache
         *  will be slow (otherwise you would be us using the SynchronizedTimedCache).
         *
         *  But this has the downside, that you could try filling the cache multiple times with the same value.
         *
         *  That's perfectly safe, but not speedy.
         *
         *  Which is better depends on the likihood the caller will make multiple requests for the same non-existent value at
         *  the same time. If yes, you should set fHoldWriteLockDuringCacheFill. If no (or if you care more about being able to
         *  read the rest of the data and not having threads block needlessly for other values) set fHoldWriteLockDuringCacheFill false (default).
         * 
         *  \note Design Note
         *        This probably should be a constructor parameter, but if its a plain bool, looks potentially confusing.
         *        Forcing explicit name probably better, and typically will be set just after construction before any threads could access
         *        so no real race risk.
         */
        bool fHoldWriteLockDuringCacheFill{false};

    public:
        /**
         * @see TimedCache::GetMinimumAllowedFreshness
         */
        nonvirtual Time::Duration GetMinimumAllowedFreshness () const;

    public:
        /**
         * @see TimedCache::SetMinimumAllowedFreshness
         */
        nonvirtual void SetMinimumAllowedFreshness (Time::Duration minimumAllowedFreshness);

    public:
        /**
         * @see TimedCache::Elements
         */
        nonvirtual Traversal::Iterable<CacheElement> Elements () const;

    public:
        /**
         * @see TimedCache::Lookup
         */
        nonvirtual optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key, Time::TimePointSeconds* lastRefreshedAt = nullptr) const;
        nonvirtual optional<VALUE> Lookup (typename Configuration::ArgByValueType<KEY> key, LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag);

    public:
        /**
         * @see TimedCache::LookupValue
         */
        nonvirtual VALUE LookupValue (typename Configuration::ArgByValueType<KEY>                          key,
                                      const function<VALUE (typename Configuration::ArgByValueType<KEY>)>& cacheFiller,
                                      LookupMarksDataAsRefreshed successfulLookupRefreshesAcceesFlag = LookupMarksDataAsRefreshed::eDontTreatFoundThroughLookupAsRefreshed,
                                      PurgeSpoiledDataFlagType purgeSpoiledData = PurgeSpoiledDataFlagType::eAutomaticallyPurgeSpoiledData);

    public:
        /**
         * @see TimedCache::Add
         */
        nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result,
                             TimedCacheSupport::PurgeSpoiledDataFlagType purgeSpoiledData = PurgeSpoiledDataFlagType::eAutomaticallyPurgeSpoiledData);
        nonvirtual void Add (typename Configuration::ArgByValueType<KEY> key, typename Configuration::ArgByValueType<VALUE> result,
                             Time::Duration freshAsOf);

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
         * @see TimedCache::PurgeSpoiledData
         */
        nonvirtual void PurgeSpoiledData ();

    public:
        [[deprecated ("Since Stroika v3.0d1, use PurgeSpoiledData or count on Add's purgeSpoiledData parameter)")]] nonvirtual void DoBookkeeping ()
        {
            PurgeSpoiledData ();
        }
        [[deprecated ("Since Stroika 3.0d1 use GetMinimumAllowedFreshness")]] Time::Duration GetTimeout () const
        {
            return GetMinimumAllowedFreshness ();
        }
        [[deprecated ("Since Stroika 3.0d1 use GetMinimumAllowedFreshness")]] void SetTimeout (Time::Duration timeout)
        {
            SetMinimumAllowedFreshness (timeout);
        }

    private:
        mutable shared_timed_mutex fMutex_; // careful this is non-recursive
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SynchronizedTimedCache.inl"

#endif /*_Stroika_Foundation_Cache_SynchronizedTimedCache_h_*/
