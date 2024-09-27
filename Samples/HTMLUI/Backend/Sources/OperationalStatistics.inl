/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */

namespace Stroika::Samples::HTMLUI {

    /*
     ********************************************************************************
     ***************** OperationalStatisticsMgr::ProcessAPICmd **********************
     ********************************************************************************
     */
    inline OperationalStatisticsMgr::ProcessAPICmd::ProcessAPICmd ()
        : fStart_{Time::GetTickCount ()}
    {
    }

    /*
     ********************************************************************************
     ************************** OperationalStatisticsMgr ****************************
     ********************************************************************************
     */
    inline void OperationalStatisticsMgr::Add_ (const Rec_& r)
    {
        lock_guard lk{fMutex_};
        ++fNextHistory_;
        if (fNextHistory_ == Memory::NEltsOf (fRollingHistory_)) {
            fNextHistory_ = 0;
        }
        Assert (fNextHistory_ < Memory::NEltsOf (fRollingHistory_));
        fRollingHistory_[fNextHistory_] = r;
    }

}
