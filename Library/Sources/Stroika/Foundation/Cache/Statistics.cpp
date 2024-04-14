/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"

#include "Statistics.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cache;
using namespace Stroika::Foundation::Cache::Statistics;
using namespace Stroika::Foundation::Characters;

/*
 ********************************************************************************
 ********************************** Stats_Basic *********************************
 ********************************************************************************
 */
String Stats_Basic::ToString () const
{
    size_t hits   = fCachedCollected_Hits.load (); // use local variable so if changes behind the scenes we report consistenly
    size_t misses = fCachedCollected_Misses.load ();
    size_t total  = hits + misses;
    if (total == 0) {
        total = 1; // avoid divide by zero
    }
    return Format ("{ hits: {}, misses: {}, hit%: {} }"_f, hits, misses, float (hits) / float (total));
}

/*
 ********************************************************************************
 ************************************ Stats_Null ********************************
 ********************************************************************************
 */
String Stats_Null::ToString () const
{
    return {};
}
