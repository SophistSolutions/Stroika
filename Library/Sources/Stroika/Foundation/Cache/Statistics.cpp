/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"

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
    size_t total = fCachedCollected_Hits + fCachedCollected_Misses;
    if (total == 0) {
        total = 1; // avoid divide by zero
    }
    return Format (L"{ hits: %d, misses: %d, hit%%: %f }", fCachedCollected_Hits, fCachedCollected_Misses, float(fCachedCollected_Hits) / float(total));
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
