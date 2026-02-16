/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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
     ****************** OperationalStatisticsMgr::ProcessDBCmd **********************
     ********************************************************************************
     */
    inline OperationalStatisticsMgr::ProcessDBCmd::ProcessDBCmd (DBCommandType cmdType)
        : fStart_{Time::GetTickCount ()}
    {
        switch (cmdType) {
            case DBCommandType::eRead:
                fKind_ = Rec_::Kind::eDBRead;
                break;
            case DBCommandType::eWrite:
                fKind_ = Rec_::Kind::eDBWrite;
                break;
            default:
                RequireNotReached ();
        }
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
        if (fNextHistory_ == std::size (fRollingHistory_)) {
            fNextHistory_ = 0;
        }
        Assert (fNextHistory_ < std::size (fRollingHistory_));
        fRollingHistory_[fNextHistory_] = r;
    }

}
