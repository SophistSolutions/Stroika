/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_ProgressMonitor_inl_
#define	_Stroika_Foundation_Execution_ProgressMonitor_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"

#include	"Exceptions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			inline	ProgressRangeType	ProgressMontior::GetProgress () const
				{
					return fCurrentProgress_;
				}
			inline	void	ProgressMontior::SetProgress (ProgressRangeType p)
				{
					Require (0.0 <= p and p <= 1.0);
					Require (p >= fCurrentProgress_);
					if (p > fCurrentProgress_) {
						fCurrentProgress_ = p;
						CallNotifyProgress_ ();
					}
				}
			inline	void	ProgressMontior::Abort ()
				{
					fAborted_ = true;
				}
			inline	void	ProgressMontior::ThrowIfAborted ()
				{
					if (fAborted_) {
						DoThrow (UserCanceledException ());
						//DoThrow<ThreadAbortException> (ThreadAbortException ());
					}
				}
			inline	wstring	ProgressMontior::GetCurrentTaskDescription () const
				{
					return fCurrentTaskDescription_;
				}
			inline	void	ProgressMontior::SetCurrentTaskDescription (const wstring& taskDescription)
				{
					fCurrentTaskDescription_ = taskDescription;
				}
			inline	void	ProgressMontior::SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress)
				{
					SetProgress (currentProgress);
					ThrowIfAborted ();
				}

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_ProgressMonitor_inl_*/

