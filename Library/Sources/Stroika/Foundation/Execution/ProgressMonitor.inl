/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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

            inline  ProgressRangeType   ProgressMontior::GetProgress () const {
                return fCurrentProgress_;
            }
            inline  void    ProgressMontior::SetProgress (ProgressRangeType p) {
                Require (0.0 <= p and p <= 1.0);
                Require (p >= fCurrentProgress_);
                if (p > fCurrentProgress_) {
                    fCurrentProgress_ = p;
                    CallNotifyProgress_ ();
                }
            }
            inline  void    ProgressMontior::Cancel () {
                fCanceled_ = true;
            }
            inline  void    ProgressMontior::ThrowIfCanceled () {
                if (fCanceled_) {
                    DoThrow (UserCanceledException ());
                    //DoThrow<ThreadAbortException> (ThreadAbortException ());
                }
            }
            inline  ProgressMontior::CurrentTaskInfo    ProgressMontior::GetCurrentTaskInfo () const {
                return fCurrentTaskInfo_;
            }
            inline  void    ProgressMontior::SetCurrentTaskInfo (const CurrentTaskInfo& taskInfo) {
                fCurrentTaskInfo_ = taskInfo;
            }
            inline  void    ProgressMontior::SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress) {
                SetProgress (currentProgress);
                ThrowIfCanceled ();
            }
            inline  void    ProgressMontior::SetCurrentProgressAndThrowIfCanceled (ProgressMontior* objOrNull, ProgressRangeType currentProgress) {
                if (objOrNull != nullptr) {
                    objOrNull->SetCurrentProgressAndThrowIfCanceled (currentProgress);
                }
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_ProgressMonitor_inl_*/

