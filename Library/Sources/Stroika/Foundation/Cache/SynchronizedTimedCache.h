/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_SynchronizedTimedCache_h_
#define _Stroika_Foundation_Cache_SynchronizedTimedCache_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Cache/TimedCache.h"

/**
 *      \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

_DeprecatedFile_ ("DEPRECATED in v2.1d23 - use TimedCacheSupport::ExplicitTraits directly");
namespace Stroika::Foundation::Cache {

    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS>
    struct TCTRAITS_ : TRAITS {
        static constexpr inline Execution::InternallySynchronized kInternallySynchronized{Execution::InternallySynchronized::eInternallySynchronized};
    };
    template <typename KEY, typename VALUE, TimedCacheSupport::ITraits<KEY, VALUE> TRAITS = TimedCacheSupport::DefaultTraits<KEY, VALUE>>
    using SynchronizedTimedCache
        [[deprecated ("Since Stroika v3.0d23 use INTERNALLY_SYNCRHONIZED arg to TimedCacheSupport::ExplicitTraits")]] =
            TimedCache<KEY, VALUE, TCTRAITS_<KEY, VALUE, TRAITS>>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cache_SynchronizedTimedCache_h_*/
