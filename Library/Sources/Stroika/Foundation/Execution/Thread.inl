/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Thread_inl_
#define	_Stroika_Foundation_Execution_Thread_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ThreadAbortException.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			class	Thread::Rep_ {
				public:
					Rep_ (const SharedPtr<IRunnable>& runnable);
					~Rep_ ();

				public:
					static	void	DoCreate (SharedPtr<Rep_>* repSharedPtr);

				public:
					nonvirtual	void	Start ();

				protected:
					nonvirtual	void	Run ();

				protected:
					// Called - typically from ANOTHER thread (but could  be this thread). By default this does nothing,
					// and is just called by Thread::Abort (). It CAN be hooked by subclassses to do soemthing to
					// force a quicker abort.
					//
					// BUT BEWARE WHEN OVERRIDING - WORKS ON ANOTHER THREAD!!!!
					nonvirtual	void	NotifyOfAbort ();

				protected:
					// Called from WITHIN this thread (asserts thats true), and does throw of ThreadAbortException if in eAborting state
					nonvirtual	void	ThrowAbortIfNeeded () const;

			#if			qUseThreads_WindowsNative
				private:
					static	unsigned int	__stdcall	ThreadProc (void* lpParameter);

				private:
					static	void	CALLBACK	AbortProc_ (ULONG_PTR lpParameter);
			#endif

				public:
					SharedPtr<IRunnable>	fRunnable;

				private:
					nonvirtual	int	MyGetThreadId_ () const;

				private:
					friend class	Thread;

				private:
				#if			qUseThreads_WindowsNative
					HANDLE					fThread;
				#endif
					mutable	CriticalSection	fStatusCriticalSection;
					Status					fStatus;
					Event					fRefCountBumpedEvent;
					Event					fOK2StartEvent;
					wstring					fThreadName;
			};


		// class	Thread::Rep_
			inline	void	Thread::Rep_::Start ()
				{
					fOK2StartEvent.Set ();
				}
			inline	void	Thread::Rep_::ThrowAbortIfNeeded () const
				{
				#if			qUseThreads_WindowsNative
					Require (::GetCurrentThreadId () == MyGetThreadId_ ());
				#endif
					AutoCriticalSection enterCritcalSection (fStatusCriticalSection);
					if (fStatus == eAborting) {
						DoThrow (ThreadAbortException ());
					}
				}


		// class	Thread
		#if			qUseThreads_WindowsNative
			inline	HANDLE	Thread::GetOSThreadHandle () const
				{
					return fRep_->fThread;
				}
		#endif
			inline	SharedPtr<IRunnable>	Thread::GetRunnable () const
				{
					if (fRep_.IsNull ()) {
						return SharedPtr<IRunnable> ();
					}
					return fRep_->fRunnable;
				}
			inline	bool	Thread::operator< (const Thread& rhs) const
				{
					return fRep_ < rhs.fRep_;
				}
			inline	void	Thread::AbortAndWaitForDone (Time::DurationSecondsType timeout)
				{
					Abort ();
					WaitForDone (timeout);
				}
			inline	wstring	Thread::GetThreadName () const
				{
					return fRep_->fThreadName;
				}
			inline	Thread::Status	Thread::GetStatus () const
				{
					if (fRep_.IsNull ()) {
						return eNull;
					}
					return GetStatus_ ();
				}



		// class	CheckForThreadAborting
		template	<unsigned int kEveryNTimes>
			void	CheckForThreadAborting ()
				{
					static	unsigned int	n	=	0;
					if (++n % kEveryNTimes == kEveryNTimes-1) {
						CheckForThreadAborting ();
					}
				}


		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Thread_inl_*/
