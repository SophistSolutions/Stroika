/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_SimpleThread_h_
#define	_Stroika_Foundation_Execution_SimpleThread_h_	1

#include	"../StroikaPreComp.h"

#include	<windows.h>

#include	"../Configuration/Basics.h"
#include	"../Memory/RefCntPtr.h"

#include	"CriticalSection.h"
#include	"Exceptions.h"
#include	"Event.h"
#include	"IRunnable.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			using	Memory::RefCntPtr;


			// ADD THREADPOOL (Thread Pool) SUPPORT:
			//
			//		Basic idea is to separate out this existing class into two parts - the part that has
			//		NotifyOfAbort/MainLoop () - and the surrounding code that does the ThreadMain etc stuff.
			//
			//		Then do one backend impl that does just what we have here, and one that pools the thread objects
			//		(handles) - and keeps them sleeping when not used.
			//
			//		LOW PRIORITY - but a modest change. Its mainly useful for servicing lost-cost calls/threads, where
			//		the overhead of constructing the thread is significant compared to the cost of performing the
			//		action, and where the priority & stacksize can all be predeterimed and 'shared'.
			//


			// Using the smartpointer wrapper SimpleThread around a thread guarnatees its reference counting
			// will work safely - so that even when all external references go away, the fact that the thread
			// is still running will keep the reference count non-zero.
			//
			//
			// To make STOP code more safe - and have Stop really throw ThreadAbortException - then associate a PROGRESS object with this REP, and
			// make sure the REP (Run method) takes that guy as arg, and call 'CheckCanceled' periodically - which can do the throw properlly!!!!
		// (actually - above is out of date - but dont delete til I verify - but since I added the 
		// NotifyOfAbort/SleepEx/QueueUserAPC stuff - it should be pretty automatic...
		// -- LGP 2009-05-08
			//
			class	SimpleThread {
				public:
					class	Rep;
				public:
					SimpleThread ();
					explicit SimpleThread (const RefCntPtr<Rep>& threadObj);
					explicit SimpleThread (Rep* newThreadObj);
					// fun2CallOnce is called precisely once by this thread CTOR, but called in another thread with the arg 'arg'.
					explicit SimpleThread (void (*fun2CallOnce) (void* arg), void* arg);
					explicit SimpleThread (const RefCntPtr<IRunnable>& runnable);

				public:
					nonvirtual	HANDLE			GetOSThreadHandle () const;
					nonvirtual	RefCntPtr<Rep>	GetRep () const;

				public:
					nonvirtual	void	Start ();				// only legal if status is eNotYetRunning
					nonvirtual	void	Stop ();				// send ThreadAbortException if not forced, and TerminateThread if forced - does NOT block until Stop successful
					nonvirtual	void	Stop_Forced_Unsafe ();	// like Stop () - but less safe, and more forceful
					nonvirtual	void	WaitForDone (float timeout = -1.0f) const;	// wait until thread is done (use Stop to request termination) - throws if timeout
					nonvirtual	void	StopAndWaitForDone (float timeout = -1.0f);	// throws if timeout
					// Look pumping messages until either time2Pump is exceeded or the thread completes. Its NOT an erorr if the
					// timeout is exceeded
					nonvirtual	void	PumpMessagesAndReturnWhenDoneOrAfterTime (float timeToPump = -1.0f) const;
					nonvirtual	void	WaitForDoneWhilePumpingMessages (float timeout = -1.0f) const;	// throws if timeout

				public:
					nonvirtual	void	SetThreadPriority (int nPriority = THREAD_PRIORITY_NORMAL);

				public:
					enum Status { 
						eNull,				// null thread object
						eNotYetRunning,		// created, but start not yet called
						eRunning,			// in the context of the 'Run' method
						eAborting,			// Stop () called, but the thread still hasn't yet unwound
						eCompleted,			// run has terminated (possibly by exception, possibly normally, possibly because of Stop call)
					};
					nonvirtual	Status	GetStatus () const;

				// Thread name does NOT need to be unique and defaults to '', but can be used on advisory basis for
				//
				public:
					nonvirtual	wstring	GetThreadName () const;
					nonvirtual	void	SetThreadName (const wstring& threadName);

				public:
					nonvirtual	bool	operator< (const SimpleThread& rhs) const;

				private:
					RefCntPtr<Rep>	fRep;
			};
			class	SimpleThread::Rep : public virtual Memory::RefCntPtrBase {
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
					// and is just called by SimpleThread::Stop (). It CAN be hooked by subclassses to do soemthing to
					// force a quicker abort.
					//
					// BUT BEWARE WHEN OVERRIDING - WORKS ON ANOTHER THREAD!!!!
					virtual	void	NotifyOfAbort ();

				protected:
					// Called from WITHIN this thread (asserts thats true), and does throw of ThreadAbortException if in eAborting state
					nonvirtual	void	ThrowAbortIfNeeded () const;

				public:
					override	void	DO_DELETE_REF_CNT ();

				private:
					static	unsigned int	__stdcall	ThreadProc (void* lpParameter);

				private:
					static	void	CALLBACK	AbortProc_ (ULONG_PTR lpParameter);

				private:
					nonvirtual	int	MyGetThreadId_ () const;

				private:
					friend class	SimpleThread;

				private:
					HANDLE					fThread;
					mutable	CriticalSection	fStatusCriticalSection;
					Status					fStatus;
					Event					fRefCountBumpedEvent;
					Event					fOK2StartEvent;
					wstring					fThreadName;
			};




			// Our thread abort mechanism only throws at certain 'signalable' spots in the code - like sleeps, most reads, etc.
			// This function will also trigger a throw if called inside a thread which is being aborted.
			void	CheckForThreadAborting ();

			template	<unsigned int kEveryNTimes>
				void	CheckForThreadAborting ();



		}
	}
}
#endif	/*_Stroika_Foundation_Execution_SimpleThread_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"SimpleThread.inl"
