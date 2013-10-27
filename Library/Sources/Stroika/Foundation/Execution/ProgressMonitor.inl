/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ProgressMonitor_inl_
#define _Stroika_Foundation_Execution_ProgressMonitor_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <atomic>

#include    "../Debug/Assertions.h"
#include    "../Math/Common.h"
#include    "../Memory/BlockAllocated.h"

#include    "UserCanceledException.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ***************************** ProgressMonitor::IRep_ ***************************
             ********************************************************************************
             */
            class   ProgressMonitor::IRep_ {
            public:
                DECLARE_USE_BLOCK_ALLOCATION (IRep_);
                IRep_ ()
                    : fCurTaskInfo_CritSect_ ()
                    , fCallbacks_ ()
                    , fCanceled_ (false)
                    , fCurrentProgress_ (0.0)
                    , fCurrentTaskInfo_ () {
                }

                mutable mutex                                       fCurTaskInfo_CritSect_; // needed because Memory::VariantValue is not threadsafe
                Containers::Sequence<ProgressChangedCallbackType>   fCallbacks_;
                std::atomic<bool>                                   fCanceled_;
                std::atomic<ProgressRangeType>                      fCurrentProgress_;
                CurrentTaskInfo                                     fCurrentTaskInfo_;
            };



            /*
             ********************************************************************************
             ***************************** ProgressMontitor *********************************
             ********************************************************************************
             */
            inline ProgressMonitor::ProgressMonitor (const shared_ptr<IRep_>& rep)
                : fRep_ (rep)
            {
            }
            inline  ProgressMonitor::ProgressRangeType   ProgressMonitor::GetProgress () const
            {
                RequireNotNull (fRep_);
                Ensure (0.0 <= fRep_->fCurrentProgress_ and fRep_->fCurrentProgress_ <= 1.0);
                fRep_->fCanceled_ = true;
                return fRep_->fCurrentProgress_;
            }
            inline  void    ProgressMonitor::Cancel ()
            {
                RequireNotNull (fRep_);
                fRep_->fCanceled_ = true;
            }
            inline  ProgressMonitor::CurrentTaskInfo    ProgressMonitor::GetCurrentTaskInfo () const
            {
                RequireNotNull (fRep_);
                lock_guard<mutex> enterCriticalSection (fRep_->fCurTaskInfo_CritSect_);
                return fRep_->fCurrentTaskInfo_;
            }


            /*
             ********************************************************************************
             ********************** ProgressMonitor::TaskNotifier ***************************
             ********************************************************************************
             */
            inline ProgressMonitor::TaskNotifier::TaskNotifier ()
                : fRep_ ()
                , fFromProg_ (0.0)
                , fToProg_ (1.0)
            {
            }
            inline ProgressMonitor::TaskNotifier::TaskNotifier (nullptr_t)
                : fRep_ ()
                , fFromProg_ (0.0)
                , fToProg_ (1.0)
            {
            }
            inline ProgressMonitor::TaskNotifier::TaskNotifier (const TaskNotifier& parentTask, ProgressRangeType fromProg, ProgressRangeType toProg)
                : fRep_ (parentTask.fRep_)
                , fFromProg_ (parentTask.fFromProg_ + fromProg * (parentTask.fToProg_ - parentTask.fFromProg_))
                , fToProg_ (parentTask.fFromProg_ + toProg * (parentTask.fToProg_ - parentTask.fFromProg_))
            {
                fFromProg_ = Math::PinToSpecialPoint (fFromProg_, parentTask.fFromProg_);
                fToProg_ = Math::PinToSpecialPoint (fToProg_, parentTask.fToProg_);
                Require ((0.0f <= parentTask.fFromProg_) and (parentTask.fFromProg_ <= fFromProg_) and (fFromProg_ < fToProg_) and (fToProg_ <= parentTask.fToProg_) and (parentTask.fToProg_ <= 1.0f));
            }
            inline  void    ProgressMonitor::TaskNotifier::SetProgress (ProgressRangeType p)
            {
                p = Math::PinToSpecialPoint (Math::PinToSpecialPoint (p, 1.0f), 0.0f) ;
                Require (0.0 <= p and p <= 1.0);
                p = fFromProg_ + p * (fToProg_ - fFromProg_);
                p = Math::PinToSpecialPoint (Math::PinToSpecialPoint (p, 1.0f), 0.0f) ;
                Require (0.0 <= p and p <= 1.0);
                p = Math::PinToSpecialPoint (p, fRep_->fCurrentProgress_.load ());  // pin-to-special-point to avoid floating point rounding errors triggering bogus assertions/progress changes
                Require (p >= fRep_->fCurrentProgress_);        // disallow moving progress backwards
                // Note this is done because it is nearly always a bug, and not terribly useful
                if (p > fRep_->fCurrentProgress_) {
                    fRep_->fCurrentProgress_ = p;
                    CallNotifyProgress_ ();
                }
            }
            inline  void    ProgressMonitor::TaskNotifier::ThrowIfCanceled ()
            {
                if (fRep_.get () != nullptr and fRep_->fCanceled_) {
                    DoThrow (UserCanceledException ());
                    //DoThrow<ThreadAbortException> (ThreadAbortException ());
                }
            }
            inline  void    ProgressMonitor::TaskNotifier::SetCurrentTaskInfo (const CurrentTaskInfo& taskInfo)
            {
                if (fRep_.get () != nullptr) {
                    lock_guard<mutex> enterCriticalSection (fRep_->fCurTaskInfo_CritSect_);
                    fRep_->fCurrentTaskInfo_ = taskInfo;
                }
            }
            inline  void    ProgressMonitor::TaskNotifier::SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress)
            {
                if (fRep_.get () != nullptr) {
                    SetProgress (currentProgress);
                    ThrowIfCanceled ();
                }
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_ProgressMonitor_inl_*/

