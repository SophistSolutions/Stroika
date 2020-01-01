/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ProgressMonitor_inl_
#define _Stroika_Foundation_Execution_ProgressMonitor_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <atomic>

#include "../Debug/Assertions.h"
#include "../Math/Common.h"
#include "../Memory/BlockAllocated.h"

#include "Common.h"
#include "Thread.h"
#include "UserCanceledException.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ***************************** ProgressMonitor::Rep_ ****************************
     ********************************************************************************
     */
    class ProgressMonitor::Rep_ : public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    public:
        Rep_ ()
            : fCurTaskInfo_CritSect_ ()
            , fCallbacks_ ()
            , fCanceled_ (false)
            , fCurrentProgress_ (0.0)
            , fCurrentTaskInfo_ ()
            , fWorkThread_ ()
        {
        }

        mutable mutex                             fCurTaskInfo_CritSect_; // needed because VariantValue is not threadsafe
        Containers::Sequence<ChangedCallbackType> fCallbacks_;
        atomic<bool>                              fCanceled_;
        atomic<ProgressRangeType>                 fCurrentProgress_;
        CurrentTaskInfo                           fCurrentTaskInfo_;
        Thread::Ptr                               fWorkThread_; // optional - ignore if empty
    };

    /*
     ********************************************************************************
     ***************************** ProgressMontitor *********************************
     ********************************************************************************
     */
    inline ProgressMonitor::ProgressMonitor (const shared_ptr<Rep_>& rep)
        : fRep_ (rep)
    {
    }
    inline ProgressMonitor::ProgressRangeType ProgressMonitor::GetProgress () const
    {
        RequireNotNull (fRep_);
        Ensure (0.0 <= fRep_->fCurrentProgress_ and fRep_->fCurrentProgress_ <= 1.0);
        fRep_->fCanceled_ = true;
        return fRep_->fCurrentProgress_;
    }
    inline ProgressMonitor::CurrentTaskInfo ProgressMonitor::GetCurrentTaskInfo () const
    {
        RequireNotNull (fRep_);
        [[maybe_unused]] auto&& critSec = lock_guard{fRep_->fCurTaskInfo_CritSect_};
        return fRep_->fCurrentTaskInfo_;
    }

    /*
     ********************************************************************************
     *************************** ProgressMonitor::Updater ***************************
     ********************************************************************************
     */
    inline ProgressMonitor::Updater::Updater ()
        : fRep_ ()
        , fFromProg_ (0.0)
        , fToProg_ (1.0)
    {
    }
    inline ProgressMonitor::Updater::Updater (nullptr_t)
        : fRep_ ()
        , fFromProg_ (0.0)
        , fToProg_ (1.0)
    {
    }
    inline ProgressMonitor::Updater::Updater (const Updater& parentTask, ProgressRangeType fromProg, ProgressRangeType toProg)
        : fRep_ (parentTask.fRep_)
        , fFromProg_ (parentTask.fFromProg_ + fromProg * (parentTask.fToProg_ - parentTask.fFromProg_))
        , fToProg_ (parentTask.fFromProg_ + toProg * (parentTask.fToProg_ - parentTask.fFromProg_))
    {
        fFromProg_ = Math::PinToSpecialPoint (fFromProg_, parentTask.fFromProg_);
        fToProg_   = Math::PinToSpecialPoint (fToProg_, parentTask.fToProg_);
        Require ((0.0f <= parentTask.fFromProg_) and (parentTask.fFromProg_ <= fFromProg_) and (fFromProg_ < fToProg_) and (fToProg_ <= parentTask.fToProg_) and (parentTask.fToProg_ <= 1.0f));
    }
    inline void ProgressMonitor::Updater::SetProgress (ProgressRangeType p)
    {
        p = Math::PinToSpecialPoint (Math::PinToSpecialPoint (p, 1.0f), 0.0f);
        Require (0.0 <= p and p <= 1.0);
        p = fFromProg_ + p * (fToProg_ - fFromProg_);
        p = Math::PinToSpecialPoint (Math::PinToSpecialPoint (p, 1.0f), 0.0f);
        Require (0.0 <= p and p <= 1.0);
        // pin-to-special-point to avoid floating point rounding errors triggering bogus assertions/progress changes
        p = Math::PinToSpecialPoint (p, fRep_->fCurrentProgress_.load ());
        // disallow moving progress backwards because it is nearly always a bug, and not terribly useful
        Require (p >= fRep_->fCurrentProgress_);
        if (fRep_->fCurrentProgress_.exchange (p) != p) {
            // only call if value changed - but always write so atomic update
            CallNotifyProgress_ ();
        }
    }
    inline void ProgressMonitor::Updater::ThrowIfCanceled ()
    {
        if (fRep_.get () != nullptr and fRep_->fCanceled_) {
            if (fRep_->fWorkThread_ != nullptr) {
                Throw (Thread::AbortException::kThe);
            }
            Throw (UserCanceledException::kThe);
        }
    }
    inline void ProgressMonitor::Updater::SetCurrentTaskInfo (const CurrentTaskInfo& taskInfo)
    {
        if (fRep_.get () != nullptr) {
            [[maybe_unused]] auto&& critSec = lock_guard{fRep_->fCurTaskInfo_CritSect_};
            fRep_->fCurrentTaskInfo_        = taskInfo;
        }
    }
    inline void ProgressMonitor::Updater::SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress)
    {
        if (fRep_.get () != nullptr) {
            SetProgress (currentProgress);
            ThrowIfCanceled ();
        }
    }

}

#endif /*_Stroika_Foundation_Execution_ProgressMonitor_inl_*/
