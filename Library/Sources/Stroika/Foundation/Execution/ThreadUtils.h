/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__ThreadUtils_h__
#define	__ThreadUtils_h__	1

#include	"../StroikaPreComp.h"

#include	<windows.h>

#include	"../Configuration/Basics.h"
#include	"../Memory/RefCntPtr.h"

#include	"Exceptions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			using	Memory::RefCntPtr;


			#ifndef	qTrack_ThreadUtils_HandleCounts
			#define	qTrack_ThreadUtils_HandleCounts		defined (_DEBUG)
			#endif



			/// More TODO - Improve use of RUNNABLE CLASS - SEE http://bugzilla/show_bug.cgi?id=746
			class	IRunnable {
				public:
					virtual ~IRunnable ();
				public:
					virtual	void	Run () = 0;
			};




			class	WaitAbandonedException : public StringException {
				public:
					WaitAbandonedException ();
			};
			class	WaitTimedOutException : public StringException {
				public:
					WaitTimedOutException ();
			};


			class	CriticalSection {
				public:
					CriticalSection () throw ();
					~CriticalSection ();

				private:
					CriticalSection (const CriticalSection&);					// not allowed
					const CriticalSection& operator= (const CriticalSection&);	// not allowed

				public:
					nonvirtual	void	Lock (); 
					nonvirtual	void	Unlock ();

					operator CRITICAL_SECTION& ();

				private:
					CRITICAL_SECTION fCritSec;
			};



			class	CopyableCriticalSection {
				private:
					Memory::RefCntPtr<CriticalSection>	fCritSec;
				public:
					CopyableCriticalSection ();

				public:
					nonvirtual	void	Lock (); 
					nonvirtual	void	Unlock ();
					nonvirtual	operator CRITICAL_SECTION& ();
			};


			class	Event {
			#if		qTrack_ThreadUtils_HandleCounts
				public:
					static	LONG	sCurAllocatedHandleCount;
			#endif
				public:
					Event (bool manualReset, bool initialState);
					~Event ();

				private:
					Event (const Event&);						// NOT IMPL
					const Event operator= (const Event&);		// NOT IMPL

				public:
					// Pulse the event (signals waiting objects, then resets)
					nonvirtual	void	Pulse () throw ();

					// Set the event to the non-signaled state
					nonvirtual	void	Reset () throw ();

					// Set the event to the signaled state
					nonvirtual	void	Set () throw ();

					// Simple wait. Can use operator HANDLE() to do fancier waits
					nonvirtual	void	Wait (float timeout = float (INFINITE)/1000) const;

				public:
					operator HANDLE () const;

				private:
					HANDLE	fEventHandle;
			};




			// enter  in CTOR and LEAVE in DTOR
			class	AutoCriticalSection {
				public:
					explicit AutoCriticalSection (CRITICAL_SECTION& critSec);
					~AutoCriticalSection ();

				private:
					CRITICAL_SECTION&	fCritSec;
			};


			// MAIN reason to use this - is it sets the 'alertable' flag on the sleep, so the QueueUserAPC () stuff works!
			// which allows SimpleThread::Stop () to work properly...
			// -- LGP 2009-04-28
			void	Sleep (float seconds2Wait);



			// Our thread abort mechanism only throws at certain 'signalable' spots in the code - like sleeps, most reads, etc.
			// This function will also trigger a throw if called inside a thread which is being aborted.
			void	CheckForThreadAborting ();

			template	<unsigned int kEveryNTimes>
				void	CheckForThreadAborting ();


			// Generally should not be reported. It's just to help force a thread to shut itself down
			class	ThreadAbortException {
				public:
					ThreadAbortException ();
			};


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

		}
	}
}
#endif	/*__ThreadUtils_h__*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include	"ThreadUtils.inl"
