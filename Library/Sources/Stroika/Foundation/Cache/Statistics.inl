/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Cache::Statistics {

    /*
     ********************************************************************************
     ***************************** Statistics::Stats_Basic **************************
     ********************************************************************************
     */
    inline Stats_Basic::Stats_Basic (Stats_Basic&& src)
        : fCachedCollected_Hits{src.fCachedCollected_Hits.load ()}
        , fCachedCollected_Misses{src.fCachedCollected_Misses.load ()}
    {
    }
    inline Stats_Basic::Stats_Basic (const Stats_Basic& src)
        : fCachedCollected_Hits{src.fCachedCollected_Hits.load ()}
        , fCachedCollected_Misses{src.fCachedCollected_Misses.load ()}
    {
    }
    inline Stats_Basic& Stats_Basic::operator= (Stats_Basic&& rhs) noexcept
    {
        fCachedCollected_Hits   = rhs.fCachedCollected_Hits.load ();
        fCachedCollected_Misses = rhs.fCachedCollected_Misses.load ();
        return *this;
    }
    inline Stats_Basic& Stats_Basic::operator= (const Stats_Basic& rhs)
    {
        fCachedCollected_Hits   = rhs.fCachedCollected_Hits.load ();
        fCachedCollected_Misses = rhs.fCachedCollected_Misses.load ();
        return *this;
    }
    inline void Stats_Basic::IncrementHits ()
    {
        ++fCachedCollected_Hits;
    }
    inline void Stats_Basic::IncrementMisses ()
    {
        ++fCachedCollected_Misses;
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
