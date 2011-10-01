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

			#if		qUseThreads

// SOON TO BE PRIVATE
			class	Thread::Rep : public virtual Memory::SharedPtrBase {
				protected:
					Rep ();
				public:
					virtual ~Rep ();

				public:
					nonvirtual	void	Start ();

				protected:
					virtual	void	Run ()	=	0;

				protected:
					// Called - typically from ANOTHER thread (but could  be this thread). By default this does nothing,
					// and is just called by Thread::Abort (). It CAN be hooked by subclassses to do soemthing to
					// force a quicker abort.
					//
					// BUT BEWARE WHEN OVERRIDING - WORKS ON ANOTHER THREAD!!!!
					virtual	void	NotifyOfAbort ();

				protected:
					// Called from WITHIN this thread (asserts thats true), and does throw of ThreadAbortException if in eAborting state
					nonvirtual	void	ThrowAbortIfNeeded () const;

				public:
					virtual	void	DO_DELETE_REF_CNT () override;

			#if			qUseThreads_WindowsNative
				private:
					static	unsigned int	__stdcall	ThreadProc (void* lpParameter);

				private:
					static	void	CALLBACK	AbortProc_ (ULONG_PTR lpParameter);
			#endif


				public:
					SharedPtr<IRunnable>	fRunnable;		//// 



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
			#endif


		// class	Thread::Rep
			inline	void	Thread::Rep::Start ()
				{
					fOK2StartEvent.Set ();
				}
			inline	void	Thread::Rep::ThrowAbortIfNeeded () const
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
					return fRep->fThread;
				}
		#endif
			inline	SharedPtr<Thread::Rep>	Thread::GetRep () const
				{
					return fRep;
				}
			inline	SharedPtr<IRunnable>	Thread::GetRunnable () const
				{
					if (fRep.IsNull ()) {
						return SharedPtr<IRunnable> ();
					}
					return fRep->fRunnable;
				}
			inline	bool	Thread::operator< (const Thread& rhs) const
				{
					return fRep < rhs.fRep;
				}
			inline	void	Thread::AbortAndWaitForDone (Time::DurationSecondsType timeout)
				{
					Abort ();
					WaitForDone (timeout);
				}
			inline	wstring	Thread::GetThreadName () const
				{
					return fRep->fThreadName;
				}
			inline	Thread::Status	Thread::GetStatus () const
				{
					if (fRep.IsNull ()) {
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
