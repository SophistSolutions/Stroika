/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_Statistics_h_
#define _Stroika_Foundation_Cache_Statistics_h_ 1

#include "../StroikaPreComp.h"

#include <type_traits>

#include "../Characters/String.h"

/**
 *      \file
 */

namespace Stroika::Foundation::Cache::Statistics {

    /**
     *  Helper detail class for analyzing and tuning cache statistics.
     */
    struct Stats_Basic {
        size_t fCachedCollected_Hits{};
        size_t fCachedCollected_Misses{};
        void   IncrementHits ();
        void   IncrementMisses ();

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;
    };

    /**
     *  Helper for DefaultTraits - when not collecting stats.
     */
    struct Stats_Null {
        void IncrementHits ();
        void IncrementMisses ();

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString () const;
    };

    /**
     *  Helper for DefaultTraits.
     */
    using StatsType_DEFAULT = conditional_t<qDebug, Stats_Basic, Stats_Null>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Statistics.inl"

#endif /*_Stroika_Foundation_Cache_Statistics_h_*/
