/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_ProgressMonitor_h_
#define	_Stroika_Foundation_Execution_ProgressMonitor_h_	1

#include	"../StroikaPreComp.h"

#include	<string>

#include	"../Configuration/Common.h"
#include	"../Memory/RefCntPtr.h"

#include	"CriticalSection.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			typedef	float	ProgressRangeType;

			
/*
	* This class is the basic interface used for progress tracking. Progress tracking both measures progress,
	* and supports the notion of canceling. The reason progres and cancelability are tied together is that its
	* only for 'long lived' tasks one might want to measure the progress of, that one might want to allow canceling.
<<REDO BELOW COMMENTS>>
*
* This class is not meant to be subclassed to revieve notification (again, perhaps that should
* be re-thought) - but instead - its meant to be an abstraction of the progress watcher (e.g. progress tracker).
*
* Its meant to be used by a using the tracker - and have that periodically check if its been canceled or not.
	*/
	 
			class	ProgressMontior {
				public:
					class	ICallback {
						public:
							virtual	~ICallback ();
						public:
							virtual	void	NotifyOfProgress (const ProgressMontior& pm)	=	0;
					};

				public:
					ProgressMontior ();
					ProgressMontior (const Memory::RefCntPtr<ICallback>& callback);
				public:
					~ProgressMontior ();

				public:
					nonvirtual	void	AddCallback (const Memory::RefCntPtr<ICallback>& callback);
				private:
					vector<Memory::RefCntPtr<ICallback>>	fCallbacks;

				public:
					nonvirtual	ProgressRangeType	GetProgress () const;
					nonvirtual	void				SetProgress (ProgressRangeType p);

				// Cancelability. Anyone can call SetCanceled() on this progress object. If the progress object is handed to
				// some long-lived task, that task may (at its discretion) - check the progress callback, and abort its operation
				// by throwing a CANCEED exception (GET EXACT NAME RIGHT)
				//
				public:
					nonvirtual	void	Abort ();	// causes this 'progress callback' to be marked for aborting. If already abortd, it does nothing

				public:
					// Called from the context of a thread which has been given this progress object. This method will check
					// if the thread has been aborted, or if this progress object has been aborted, and in either case, throw???? EXEDCPTIONNAME?
					nonvirtual	void	ThrowIfAborted ();
				
				public:
					virtual		wstring	GetCurrentTaskDescription () const;
					virtual		void	SetCurrentTaskDescription (const wstring& taskDescription);
			
				public:
					nonvirtual	void	SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress);

				private:
					mutable CriticalSection		fCritSect_;
					bool						fAborted_;
					ProgressRangeType			fCurrentProgress_;
					wstring						fCurrentTaskDescription_;

				private:
					nonvirtual	void	CallNotifyProgress_ () const;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_ProgressMonitor_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ProgressMonitor.inl"
