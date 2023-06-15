/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_SynchronizedCallerStalenessCache_h_
#define _Stroika_Foundation_Cache_SynchronizedCallerStalenessCache_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <shared_mutex>

#include "CallerStalenessCache.h"

/**
 *      \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *
 */

namespace Stroika::Foundation::Cache {

    /**
     *  \brief simple wrapper on CallerStalenessCache (with the same API) - but internally synchronized in a way that is
     *         more performant than using RWSyncrhonzied<CallerStalenessCache<...>>
     *
     *  @see CallerStalenessCache<> - for unsynchronized base version
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    template <typename KEY, typename VALUE, typename TIME_TRAITS = CallerStalenessCache_Traits_DEFAULT>
    class SynchronizedCallerStalenessCache : private CallerStalenessCache<KEY, VALUE, TIME_TRAITS> {
    private:
        using inherited = CallerStalenessCache<KEY, VALUE, TIME_TRAITS>;

    public:
        /*
         *  Note:   We choose to not hold any lock while filling the cache (fHoldWriteLockDuringCacheFill false by default).
         *  This is because typically, filling the cache will be slow (otherwise you wouldn't be us using a cache).
         *
         *  But this has the downside, that you could try filling the cache multiple times with the same value.
         *
         *  That's perfectly safe, but not speedy.
         *
         *  Which is better depends on the likihood the caller will make multiple requests for the same non-existent value at
         *  the same time. If yes, you should set fHoldWriteLockDuringCacheFill. If no (or if you care more about being able to
         *  read the rest of the data and not having threads block needlessly for other values) set fHoldWriteLockDuringCacheFill false (default).
         * 
         *  \note Another thing to be aware of, if setting fHoldWriteLockDuringCacheFill true, is that it increases the risk
         *        of Deadlock (if what you don the fill code is complicated and difficult to analyze for what locks it might acquire).
         * 
         *  \note See https://stroika.atlassian.net/browse/STK-906 - possible enhancement to this configuration to work better avoiding
         *        deadlocks
         *  \note See https://stroika.atlassian.net/browse/STK-907 - about needing some new mechanism in Stroika for deadlock detection/avoidance.
         * 
         */
        bool fHoldWriteLockDuringCacheFill{false};

    public:
        /**
         * @see CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::TimeStampType;
         */
        using TimeStampType = typename inherited::TimeStampType;

    public:
        /**
         * @see CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::GetCurrentTimestamp;
         */
        using inherited::GetCurrentTimestamp;

    public:
        /**
         * @see CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Ago;
         */
        using inherited::Ago;

    public:
        /**
         * @see CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::ClearOlderThan;
         */
        nonvirtual void ClearOlderThan (TimeStampType t);

    public:
        /**
         * @see CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Clear;
         */
        nonvirtual void Clear ();
        template <typename K1 = KEY>
        nonvirtual void Clear (Configuration::ArgByValueType<K1> k)
            requires (IsKeyedCache<K1>);

    public:
        using AddReplaceMode = Containers::AddReplaceMode;

    public:
        /**
         * @see CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Add;
         */
        nonvirtual void Add (Configuration::ArgByValueType<VALUE> v)
            requires (not IsKeyedCache<KEY>);
        template <typename K = KEY>
        nonvirtual void Add (Configuration::ArgByValueType<K> k, Configuration::ArgByValueType<VALUE> v,
                             AddReplaceMode addReplaceMode = AddReplaceMode::eAddReplaces)
            requires (IsKeyedCache<K>);

    public:
        /**
         * @see CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::Lookup;
         */
        nonvirtual optional<VALUE> Lookup (TimeStampType staleIfOlderThan) const
            requires (not IsKeyedCache<KEY>);
        template <typename K = KEY>
        nonvirtual optional<VALUE> Lookup (Configuration::ArgByValueType<K> k, TimeStampType staleIfOlderThan) const
            requires (IsKeyedCache<K>);
        nonvirtual VALUE LookupValue (TimeStampType staleIfOlderThan, const function<VALUE ()>& cacheFiller)
            requires (not IsKeyedCache<KEY>);
        template <typename F, typename K = KEY>
        nonvirtual VALUE LookupValue (Configuration::ArgByValueType<K> k, TimeStampType staleIfOlderThan, F&& cacheFiller)
            requires (IsKeyedCache<KEY> and is_invocable_r_v<VALUE, F, KEY>);
        template <typename K = KEY>
        nonvirtual VALUE LookupValue (Configuration::ArgByValueType<K> k, TimeStampType staleIfOlderThan, const VALUE& defaultValue) const
            requires (IsKeyedCache<KEY>);

    public:
        /**
         * @see CallerStalenessCache<KEY, VALUE, TIME_TRAITS>::clear;
         */
        nonvirtual void clear ();

    private:
        mutable shared_timed_mutex fMutex_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SynchronizedCallerStalenessCache.inl"

#endif /*_Stroika_Foundation_Cache_SynchronizedCallerStalenessCache_h_*/
