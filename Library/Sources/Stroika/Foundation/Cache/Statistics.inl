/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_Statistics_inl_
#define _Stroika_Foundation_Cache_Statistics_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Cache::Statistics {

    /*
     ********************************************************************************
     ***************************** Statistics::Stats_Basic **************************
     ********************************************************************************
     */
    inline void Stats_Basic::IncrementHits ()
    {
        fCachedCollected_Hits++;
    }
    inline void Stats_Basic::IncrementMisses ()
    {
        fCachedCollected_Misses++;
    }

    /*
     ********************************************************************************
     ****************************** Statistics::Stats_Null **************************
     ********************************************************************************
     */
    inline void Stats_Null::IncrementHits ()
    {
    }
    inline void Stats_Null::IncrementMisses ()
    {
    }

}

#endif /*_Stroika_Foundation_Cache_Statistics_inl_*/
