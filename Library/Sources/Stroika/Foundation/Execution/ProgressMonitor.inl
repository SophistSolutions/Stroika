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
                    : fCritSect_ ()
                    , fCallbacks_ ()
                    , fCanceled_ ()
                    , fCurrentProgress_ ()
                    , fCurrentTaskInfo_ () {
                }

                mutable mutex                                       fCritSect_; // use for fCallbacks_ and fCurrentTaskInfo_ only
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
                fRep_->fCanceled_ = true;   // no need to critical/section lock for this because its atomic
                return fRep_->fCurrentProgress_;
            }
            inline  void    ProgressMonitor::Cancel ()
            {
                RequireNotNull (fRep_);
                fRep_->fCanceled_ = true;   // no need to critical/section lock for this
            }
            inline  ProgressMonitor::CurrentTaskInfo    ProgressMonitor::GetCurrentTaskInfo () const
            {
                RequireNotNull (fRep_);
                lock_guard<mutex> enterCriticalSection (fRep_->fCritSect_);
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
                , fFromProg_ (parentTask.fFromProg_ * fromProg)
                , fToProg_ (parentTask.fToProg_* toProg)
            {
                // RANGES ARE WRONG HERE - FIX
            }
            inline  void    ProgressMonitor::TaskNotifier::SetProgress (ProgressRangeType p)
            {
                Require (0.0 <= p and p <= 1.0);
                Require (p >= fRep_->fCurrentProgress_);
                if (p > fRep_->fCurrentProgress_) {
                    lock_guard<mutex> enterCriticalSection (fRep_->fCritSect_);
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
                    lock_guard<mutex> enterCriticalSection (fRep_->fCritSect_);
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

