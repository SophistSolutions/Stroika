/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_CallerStalenessCache_h_
#define _Stroika_Foundation_Cache_CallerStalenessCache_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Time/Realtime.h"

/**
 *      \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Add StatsType support (like other caches).
 *
 *      @todo   Add() overload where caller provides the time data was captured (don't assume now)
 *
 *      @todo   Consider adding way to retrieve timestamp for key 'k'. Also consider Iterable<> method (like LRUCache)
 *              so we can dump cache (including timestamps)
 *
 */

namespace Stroika::Foundation::Cache {

    /**
     * Shorthand to make code clearer - for caches that support missing key type.
     */
    template <typename KEY>
    static constexpr bool IsKeyedCache = not same_as<KEY, void>;

    /**
     */
    struct CallerStalenessCache_Traits_DEFAULT {
        using TimeStampType           = Time::TimePointSeconds; // type must support operator<()
        using TimeStampDifferenceType = Time::DurationSeconds;
        static TimeStampType GetCurrentTimestamp ();
    };

    /**
     *  The idea behind this cache is to track when something is added, and that the lookup function can avoid
     *  a costly call to compute something if its been recently enough added.
     *
     *  For example, consider a system where memory is stored across a slow bus, and several components need to read data from
     *  across that bus. But the different components have different tolerance for staleness (e.g. PID loop needs fresh temperature sensor
     *  data but GUI can use more stale data).
     *
     *  This CallerStalenessCache will store when the value is updated, and let the caller either return the
     *  value from cache, or fetch it and update the cache if needed.
     *
     *  This differs from other forms of caches in that:
     *      o   It records the timestamp when a value is last-updated
     *      o   It doesn't EXPIRE the data ever (except by explicit Clear or ClearOlderThan call)
     *      o   The lookup caller specifies its tolerance for data staleness, and refreshes the data as needed.
     *
     *  \note   Principal difference between CallerStalenessCache and TimedCache lies in where you specify the
     *          max-age for an item: with CallerStalenessCache, its specified on each lookup call (ie with the caller), and with
     *          TimedCache, the expiry is stored with each cached item.
     *
     *          Because of this, when you use either of these caches with a KEY=void (essentially to cache a single thing)
     *          they become indistinguishable.
     *
     *          N.B. the KEY=void functionality is NYI for TimedCache, so best to use CallerStalenessCache for that, at least for
     *          now.
     *
     *  \note   KEY may be 'void' - and if so, the KEY parameter to the various Add/Lookup etc functions - is omitted.
     *
     *  \note   Why take 'valid-since' argument to lookup functions, and not just 'backThisTime' - in other words, why force
     *          the use of 'Ago()'. The reason is that in a complex system (say web services) - where the final requester
     *          of the data specifies the allowed staleness, you want the 'ago' computation based on ITS time (not the time
     *          the lookup happens). This is only approximate too, since it doesn't take into account the latency of the return
     *          but its a little closer to accurate.
     *
     *  \par Example Usage
     *      \code
     *          // no key cache
     *          optional<InternetAddress> LookupExternalInternetAddress_ (optional<Time::DurationSeconds> allowedStaleness = {})
     *          {
     *              static CallerStalenessCache<void, optional<InternetAddress>> sCache_;
     *              return sCache_.Lookup (sCache_.Ago (allowedStaleness.value_or (30)), []() -> optional<InternetAddress> {
     *                  ...
     *                  return IO::Network::InternetAddress{connection.GET ().GetDataTextInputStream ().ReadAll ().Trim ()};
     *              });
     *          }
     *          optional<InternetAddress> iaddr = LookupExternalInternetAddress_ ();    // only invoke connection logic if timed out
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // keyed cache
     *          optional<int> MapValue_ (int value, optional<Time::DurationSeconds> allowedStaleness = {})
     *          {
     *              static CallerStalenessCache<int, optional<int>> sCache_;
     *              try {
     *                  return sCache_.LookupValue (value, sCache_.Ago (allowedStaleness.value_or (30)), [=](int v) -> optional<int> {
     *                      return v;   // could be more expensive computation
     *                  });
     *              }
     *              catch (...) {
     *                  // NOTE - to NEGATIVELY CACHE failure, you could call sCache_.Add (value, nullopt);
     *                  // return null here, or Execution::ReThrow ()
     *              }
     *          }
     *          EXPECT_TRUE (MapValue_ (1) == 1);  // skips 'more expensive computation' if in cache
     *          EXPECT_TRUE (MapValue_ (2) == 2);  // ''
     *      \endcode
     * 
     *  \par Example Usage
     *      \code
     *          // using KEY=void (so singleton cache)
     *          unsigned int  sCalls1_{0};
     *          optional<int> LookupExternalInternetAddress_ (optional<Time::DurationSeconds> allowedStaleness = {})
     *          {
     *              using Cache::CallerStalenessCache;
     *              static CallerStalenessCache<void, optional<int>> sCache_;
     *              return sCache_.LookupValue (sCache_.Ago (allowedStaleness.value_or (30)), [] () -> optional<int> {
     *                  ++sCalls1_;
     *                  return 1;
     *              });
     *          }
     *      \endcode
     *
     *  \note   Implementation Note - no reason to bother using Debug::AssertExternallySynchronized since uses Mapping,
     *          which does that internally.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  @see TimedCache, SynchronizedCallerStalenessCache
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
        using TimeStampDifferenceType = typename TIME_TRAITS::TimeStampDifferenceType;

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
         *          CallerStalenessCache<KEY, VALUE> cc;
         *          if (optional<VALUE> v= cc.Lookup (k, cc.Ago (5)) {
         *              // look key, but throw disregard if older than 5 seconds (from now)
         *          }
         *      \endcode
         */
        static TimeStampType Ago (TimeStampDifferenceType backThisTime);

    public:
        /**
         */
        nonvirtual void ClearOlderThan (TimeStampType t);

    public:
        /**
         *  Clear () -- clear all
         *  Clear (KEY k) - clear just that key
         */
        nonvirtual void Clear ();
        template <typename K = KEY>
        nonvirtual void Clear (const K& k)
            requires (IsKeyedCache<K>);

    public:
        using AddReplaceMode = Containers::AddReplaceMode;

    public:
        /**
         *  This not only adds the association of KEY k to VALUE v, but updates the timestamp associated with k.
         */
        nonvirtual void Add (Configuration::ArgByValueType<VALUE> v)
            requires (not IsKeyedCache<KEY>);
        template <typename K = KEY>
        nonvirtual void Add (Configuration::ArgByValueType<K> k, Configuration::ArgByValueType<VALUE> v,
                             AddReplaceMode addReplaceMode = AddReplaceMode::eAddReplaces)
            requires (IsKeyedCache<K>);

        /**
         *  Usually one will use this as (cache fillter overload):
         *      VALUE v = cache.Lookup (key, ts, [this] () -> VALUE {return this->realLookup(key); });
         *
         *  However, the overload returning an optional is occasionally useful, if you don't want to fill the cache
         *  but just see if a value is present.
         *
         *  Both the overload with cacheFiller, and defaultValue will update the 'time stored' for the argument key.
         *
         *  \note   Some of these Lookup () methods are not const intentionally - as they DO generally modify the cache;
         *          but others are read-only, and are therefore const.
         *
         *  \note   Lookup (,,DEFAULT_VALUE) - doesn't fill in the cache value - so not quite the same as
         *          Lookup (...,[] () { return DEFAULT_VALUE; });
         */
        nonvirtual optional<VALUE> Lookup (TimeStampType staleIfOlderThan) const
            requires (not IsKeyedCache<KEY>);
        template <typename K = KEY>
        nonvirtual optional<VALUE> Lookup (Configuration::ArgByValueType<K> k, TimeStampType staleIfOlderThan) const
            requires (IsKeyedCache<K>);

    public:
        /**
         *  Lookup the value associated with the given key (or key omitted of KEY type is void) and always return it.
         *  In case it was stale (or missing) return the provided defaule value (or cacheFiller computed value).
         */
        nonvirtual VALUE LookupValue (TimeStampType staleIfOlderThan, const function<VALUE ()>& cacheFiller)
            requires (not IsKeyedCache<KEY>);
        template <typename F, typename K = KEY>
        nonvirtual VALUE LookupValue (Configuration::ArgByValueType<K> k, TimeStampType staleIfOlderThan, F&& cacheFiller)
            requires (IsKeyedCache<K> and is_invocable_r_v<VALUE, F, K>);
        template <typename K = KEY>
        nonvirtual VALUE LookupValue (Configuration::ArgByValueType<K> k, TimeStampType staleIfOlderThan, const VALUE& defaultValue) const
            requires (IsKeyedCache<K>);

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
                : fValue{forward<VALUE> (v)}
                , fDataCapturedAt{t}
            {
            }
        };

    private:
        // @todo see if we can clean this up (https://stackoverflow.com/questions/28432977/generic-way-of-lazily-evaluating-short-circuiting-with-stdconditional-t)
        // Point of all this mumbo jumbo is - if using void as KEY, trick, we just store an optional single item in caller stalenss cache - dont need whole Map of bogus key to value
        template <bool B, template <typename...> class TrueTemplate, template <typename...> class FalseTemplate>
        struct MyLazyConditional_;
        template <template <typename...> class TrueTemplate, template <typename...> class FalseTemplate>
        struct MyLazyConditional_<true, TrueTemplate, FalseTemplate> {
            using type = TrueTemplate<myVal_>;
        };
        template <template <typename...> class TrueTemplate, template <typename...> class FalseTemplate>
        struct MyLazyConditional_<false, TrueTemplate, FalseTemplate> {
            using type = FalseTemplate<KEY, myVal_>;
        };
        using DT_ = typename MyLazyConditional_<same_as<void, KEY>, optional, Containers::Mapping>::type;
        DT_ fData_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CallerStalenessCache.inl"

#endif /*_Stroika_Foundation_Cache_CallerStalenessCache_h_*/
