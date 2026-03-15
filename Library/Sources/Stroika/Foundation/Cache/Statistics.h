/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_Statistics_h_
#define _Stroika_Foundation_Cache_Statistics_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <type_traits>

#include "Stroika/Foundation/Characters/String.h"

/**
 *      \file
 */

namespace Stroika::Foundation::Cache::Statistics {

    /**
     *  Helper detail class for analyzing and tuning cache statistics.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    template <typename STATS_TYPE>
    concept IStatsType = requires (STATS_TYPE s) {
        { s.IncrementHits () };
        { s.IncrementMisses () };
        { s.ToString () } -> same_as<Characters::String>;
    };

    /**
     *  Helper detail class for analyzing and tuning cache statistics.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    struct Stats_Basic {
        Stats_Basic () = default;
        Stats_Basic (Stats_Basic&& src) noexcept;
        Stats_Basic (const Stats_Basic& src);

        nonvirtual Stats_Basic& operator= (Stats_Basic&& rhs) noexcept;
        nonvirtual Stats_Basic& operator= (const Stats_Basic& rhs);

        nonvirtual void IncrementHits ();
        nonvirtual void IncrementMisses ();

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;

        atomic<unsigned int> fCachedCollected_Hits{};
        atomic<unsigned int> fCachedCollected_Misses{};
    };
    static_assert (IStatsType<Stats_Basic>);

    /**
     *  Helper for DefaultTraits - when not collecting stats.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    struct Stats_Null {
        void IncrementHits ();
        void IncrementMisses ();

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;
    };
    static_assert (IStatsType<Stats_Null>);

    /**
     *  Helper for DefaultTraits.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    using StatsType_DEFAULT = conditional_t<qStroika_Foundation_Debug_AssertionsChecked, Stats_Basic, Stats_Null>;
    static_assert (IStatsType<StatsType_DEFAULT>);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Statistics.inl"

#endif /*_Stroika_Foundation_Cache_Statistics_h_*/
