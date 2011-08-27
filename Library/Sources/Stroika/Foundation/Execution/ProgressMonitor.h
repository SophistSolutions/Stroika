/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_ProgressMonitor_h_
#define	_Stroika_Foundation_Execution_ProgressMonitor_h_	1

#include	"../StroikaPreComp.h"

#include	<string>

#include	"../Configuration/Common.h"
#include	"../Memory/RefCntPtr.h"
#include	"../Memory/VariantValue.h"

#include	"CriticalSection.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			typedef	float	ProgressRangeType;

			
			/*
			 * This class is the basic interface used for progress tracking. Progress tracking both measures progress,
			 * and supports the notion of canceling. The reason progres and cancelability are tied together is that its
			 * only for 'long lived' tasks one might want to measure the progress of, that one might want to allow canceling.
			 *
			 * An instance of ProgressMontior can be passed to any function which supports progress measurement (by pointer typically,
			 * or reference). That function can then run off and do a long-lived computation (often in another thread), and the user
			 * of this class can monitor the progress (e.g. updating a ui display), and can Cancel it at any point.
			 *
			 * Though this CAN all be used from a single threaded application (using some sort of co-operative multitasking) its intended
			 * use is with threads: one working, and the other controlling/monitoring (e.g. ui).
			 *
			 * The caller (in the control thread) can call GetProgress() repeatedly - to find out whats changed, or can register a callback,
			 * that will be notified on any change. But NOTE - the callback maybe called on a different thread than the ProgressMontitor was
			 * originally created on!
			 */
			class	ProgressMontior {
//ADD NO-COPY-NO-MOVE DECLARATIONMS!!! AND NO ASSIGN-OVER(?)
				public:
					class	ICallback {
						public:
							virtual	~ICallback ();
						public:
							// WARNING: this progress notification call will typically be invoked from a different thread than the one which created
							// the ProgressMonitor (typically from the running thread performing the action). So overrides must be careful about
							// updating data structures. Probably best not to do too much in this override, because of the thread issue and because
							// this call is blocking progress of that other thread doing the actual work
							//
							// Probaly best practice to have a shared event, and just have this call event.Set() and have the other (GUI/monitoring)
							// thread do a timed wait on that event.
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

				// Cancelability. Anyone can call Cancel () on this progress object. If the progress object is handed to
				// some long-lived task, that task may (at its discretion) - check the progress callback, and cancel its operation
				// by throwing a UserCanceledException.
				//
				public:
					nonvirtual	void	Cancel ();	// causes this 'progress callback' to be marked for canceling (aborting). If already canceled, it does nothing

				public:
					// Called from the context of a thread which has been given this progress object. This method will check
					// if this progress object has been canceled, and in if so throw UserCanceledException.
					// 
					// Note - this function does NOT check if the itself thread has been aborted (as that is usually taken care of automatically or via 
					// CheckForThreadAborting)
					nonvirtual	void	ThrowIfCanceled ();
				
				// Often in displaying progress, its useful to have a notion of what the system is doing, and thats usually displayed far away
				// from where the notion of progress stage resides. This API is usually called by the bit of code performing actions (to set the current task)
				// and by the calling GUI to Get the current task description.
				//
				// Note also - for reasons of localization - its often helpful to pass back specific information about the task in progress (like file 1 of 4).
				// Using the 'fExtraData' field of the 
				public:
					struct	CurrentTaskInfo {
						wstring					fName;
						Memory::VariantValue	fDetails;
					};
					nonvirtual	CurrentTaskInfo	GetCurrentTaskInfo () const;
					nonvirtual	void			SetCurrentTaskInfo (const CurrentTaskInfo& taskInfo);
			
				/*
				 * SetCurrentProgressAndThrowIfCanceled () overloads are handy helpers for code performing long-lived tasks to deal with being passed a
				 * null progress object (often) and do that check, and if non-null, then update the progress and check for cancelations.
				 */
				public:
					nonvirtual	void	SetCurrentProgressAndThrowIfCanceled (ProgressRangeType currentProgress);
					static		void	SetCurrentProgressAndThrowIfCanceled (ProgressMontior* objOrNull, ProgressRangeType currentProgress);

				private:
					mutable CriticalSection		fCritSect_;
					bool						fCanceled_;
					ProgressRangeType			fCurrentProgress_;
					CurrentTaskInfo				fCurrentTaskInfo_;

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
