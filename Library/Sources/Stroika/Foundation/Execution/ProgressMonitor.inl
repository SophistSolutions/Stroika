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
#include    "../Debug/Assertions.h"

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
                IRep_ ()
                    : fCritSect_ ()
                    , fCallbacks_ ()
                    , fCanceled_ ()
                    , fCurrentProgress_ ()
                    , fCurrentTaskInfo_ () {
                }

                mutable recursive_mutex                             fCritSect_;
                Containers::Sequence<ProgressChangedCallbackType>   fCallbacks_;
                bool                                                fCanceled_;
                ProgressRangeType                                   fCurrentProgress_;
                CurrentTaskInfo                                     fCurrentTaskInfo_;
            };



            /*
             ********************************************************************************
             ***************************** ProgressMontitor *********************************
             ********************************************************************************
             */
            inline  ProgressMonitor::ProgressRangeType   ProgressMonitor::GetProgress () const
            {
                RequireNotNull (fRep_);
                Ensure (0.0 <= fRep_->fCurrentProgress_ and fRep_->fCurrentProgress_ <= 1.0);
                return fRep_->fCurrentProgress_;
            }
            inline  void    ProgressMonitor::Cancel ()
            {
                RequireNotNull (fRep_);
                fRep_->fCanceled_ = true;
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
                , fFromProg_ (parentTask.fFromProg_)
                , fToProg_ (parentTask.fToProg_)
            {
            }
            inline  void    ProgressMonitor::TaskNotifier::SetProgress (ProgressRangeType p)
            {
                Require (0.0 <= p and p <= 1.0);
                Require (p >= fRep_->fCurrentProgress_);
                if (p > fRep_->fCurrentProgress_) {
                    lock_guard<recursive_mutex> enterCriticalSection (fRep_->fCritSect_);
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
            inline  void    ProgressMonitor::TaskNotifier::SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress)
            {
                if (fRep_.get () != nullptr) {
                    SetProgress (currentProgress);
                    ThrowIfCanceled ();
                }
            }

#if 0
            inline  void    ProgressMonitor::Cancel ()
            {
                fCanceled_ = true;
            }
            inline  ProgressMonitor::CurrentTaskInfo    ProgressMonitor::GetCurrentTaskInfo () const
            {
                return fCurrentTaskInfo_;
            }
            inline  void    ProgressMonitor::SetCurrentTaskInfo (const CurrentTaskInfo& taskInfo)
            {
                fCurrentTaskInfo_ = taskInfo;
            }
            inline  void    ProgressMonitor::SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress)
            {
                SetProgress (currentProgress);
                ThrowIfCanceled ();
            }
            inline  void    ProgressMonitor::SetCurrentProgressAndThrowIfCanceled (ProgressMonitor* objOrNull, ProgressRangeType currentProgress)
            {
                if (objOrNull != nullptr) {
                    objOrNull->SetCurrentProgressAndThrowIfCanceled (currentProgress);
                }
            }
#endif

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_ProgressMonitor_inl_*/

