/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Thread_h_
#define	_Stroika_Foundation_Execution_Thread_h_	1

#include	"../StroikaPreComp.h"

#if		qUseThreads_StdCPlusPlus
	#include	<thread>
#elif	qUseThreads_WindowsNative
	#include	<windows.h>
#endif

#include	"../Configuration/Common.h"
#include	"../Memory/SharedPtr.h"
#include	"../Time/Realtime.h"

#include	"IRunnable.h"





/*
@CONFIGVAR:		qUseThreads_StdCPlusPlus
@DESCRIPTION:	<p>qUseThreads_StdCPlusPlus is true iff Stroika is built to use portable standard C++ implementation of threads</p>
*/
#if		!defined (qUseThreads_StdCPlusPlus)
	#error "qUseThreads_StdCPlusPlus should normally be defined indirectly by StroikaConfig.h"
#endif

/*
@CONFIGVAR:		qUseThreads_WindowsNative
@DESCRIPTION:	<p>qUseThreads_WindowsNative is true iff Stroika is built to use a native Windows SDK implementation of threads</p>
*/
#if		!defined (qUseThreads_WindowsNative)
	#error "qUseThreads_WindowsNative should normally be defined indirectly by StroikaConfig.h"
#endif






#if		qUseThreads_StdCPlusPlus && qUseThreads_WindowsNative
	#error "Configuration Error: cannot define both qUseThreads_WindowsNative && qUseThreads_StdCPlusPlus"
#endif



/*
 * TODO:		PROGRESS NOTES GETTING THREAD INTERUPTION VIA SIGNALS WORKING ON POSIX
 *
 *		o	must define C++ static signal handler
 *
 *		o	must install handler when about to call Abort() - no need - I think - to install sooner - bnut maybe a good idea to???
 *
 *		o	in signal handler - set threadlocalstorage value (asssert in context of right thread id) - set sAborted.
 *
 *		o	use pthread_kill (use native_handle() from threadobj) - to send the signal.
 *
 *		o	
 *
 *	
 */



namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			using	Memory::SharedPtr;


			#if		qPlatform_POSIX
				// real definition in Signals.h - but redeclare here (could have separate file but not sure worth it) to avoid
				// mutual references
				typedef	int		SignalIDType;
			#endif

				
			/*
			 * TODO:
			 *		o	Add a Method (maybe overload of Start) - which takes a new Runnable, so that the thread object can be re-run.
			 *			This will be needed (or at least highly advantageous) for implementing thread pools.
			 */



			/*
			 * Thread Class:
			 *
			 *		Using the smartpointer wrapper Thread around a thread guarnatees its reference counting
			 *	will work safely - so that even when all external references go away, the fact that the thread
			 *	is still running will keep the reference count non-zero.
			 *
			 *
			 *	Thread Aborting/Interuption:
			 *		The Stroika Thread class supports the idea of 'aborting' a thread. In some libraries (e.g. boost) this is
			 *	called 'interuption'. In others, its called 'cancellation'.
			 *
			 *		The basic idea is that a thread goes off on its own, doing stuff, and an external force decides to tell it to stop.
			 *	Examples of this might be:
			 *		(1)	A search user interface, which starts searching as the user types. Once the process has recieved a certain
			 *			number of characters it starts searching, but perhaps before the search is done, another character comes in, so
			 *			the GUI code will want to Abort the existing search, and start a new one (with the extra character(s)).
			 *		(2)	A web server - which is serving up content, and it told to shut-down. It must interupt existing in process
			 *			processes - some maybe handling a read/write sequence, and some perhaps doing a socket listen/accept call.
			 *
			 *	When a thread is aborted, it (in that thread) throws
			 *		class	ThreadAbortException;
			 *
			 *	Thread aborting is tricky todo safely and portably. We take a number of approaches:
			 *		(1)	We maintain a thread-local-storage variable - saying if this thread has been aborted. Sprinkling CheckForThreadAborting
			 *			throughout your code - will trigger a ThreadAbortException () in that thread context.
			 *
			 *		(2)	Async-injection (QueueUserAPC/Widnows)	APC functions get 'suddenly launched' in the context of a given threads when its in
			 *			an 'alertable state'. This APC function can then throw - essentially ending the sleep/wait/or whatever.
			 *
			 *			I'M NOT sur ethis is safe - and we may want to stop doing it. Instead - do more like what I plan todo for signals
			 *
			 *		(3)	Signal injection (POSIX) - we send a special (TDB) signal to a particular thread. It sets a 'thread-local variable - aborted' 
			 *			and when it returns - any (WHICH?) systme calls in progress will return the error
			 *
			 *
			 *		<<<<DOCUMENT INTERUPTION POINTS>>>> - CALLED INTERUPTION POINTS IN BOOST - MAYBE WE SHOULD CALL THEM ABORT POINTS?
			 *		??? They are placed in the code caller can ASSUME a call to CheckForThreadAborting () is called. These include:
			 *			o	SLEEP()
			 *			o	ANY WAIT CALLS
			 *			o	anything that calls Handle_ErrNoResultInteruption ()
			 *
			 *
			 *
			 *
			 *
			 * HANDLE_EINTR_CALLER()
			 *		The short of it is that you need to catch EINTR and restart the call for these system calls:
			 *			o read, readv, write, writev, ioctl
			 *			o open() when dealing with a fifo
			 *			o wait*
			 *			o Anything socket based (send*, recv*, connect, accept etc)
			 *			o flock and lock control with fcntl
			 *			o mq_ functions which can block
			 *			o futex
			 *			o sem_wait (and timed wait)
			 *			o pause, sigsuspend, sigtimedwait, sigwaitinfo
			 *			o poll, epoll_wait, select and 'p' versions of the same
			 *			o msgrcv, msgsnd, semop, semtimedop
			 *			o close (although, on Linux, EINTR won't happen here)
			 *			o any sleep functions (careful, you need to handle this are restart with
 			 *				different arguments)
			 *
			 *	See HandleEINTR
			 *
			 */


// To make STOP code more safe - and have Stop really throw ThreadAbortException - then associate a PROGRESS object with this REP, and
// make sure the REP (Run method) takes that guy as arg, and call 'CheckCanceled' periodically - which can do the throw properlly!!!!
// (actually - above is out of date - but dont delete til I verify - but since I added the 
// NotifyOfAbort/SleepEx/QueueUserAPC stuff - it should be pretty automatic...
// -- LGP 2009-05-08
//
			class	Thread {
				public:
					/*
					 * Thread::IDType is a portable representation which is a key to currently existing system threads.
					 */
					#if		qUseThreads_StdCPlusPlus
						typedef	thread::id	IDType;
					#elif	qUseThreads_WindowsNative
						typedef	DWORD		IDType;
					#endif

				public:
					/*
					 * Thread::native_handle is the type of the underlying handle to a native thread which can allow for using platform APIs.
					 */
					#if		qUseThreads_StdCPlusPlus
						typedef	thread::native_handle_type 	NativeHandleType;
					#elif	qUseThreads_WindowsNative
						typedef	HANDLE						NativeHandleType;
					#endif

				public:
					//
					// No arg- constructor is available for use in applications like thread pools.
					//
					Thread ();

					// TODO:
					//			(o)		Add more function-pointer overloads (e.g. function with no args, and function with 2 args - and maybe templated variant?
					//
					//
					// fun2CallOnce is called precisely once by this thread CTOR, but called in another thread with the arg 'arg'.
					explicit Thread (void (*fun2CallOnce) ());
					explicit Thread (void (*fun2CallOnce) (void* arg), void* arg);
					explicit Thread (const SharedPtr<IRunnable>& runnable);

				#if			qUseThreads_WindowsNative
				public:
					nonvirtual	HANDLE		GetOSThreadHandle () const;
				#endif

				public:
					nonvirtual	SharedPtr<IRunnable>	GetRunnable () const;

				public:
					nonvirtual	IDType				GetID () const;
					nonvirtual	NativeHandleType	GetNativeHandle ();

				public:
					nonvirtual	void	Start ();				// only legal if status is eNotYetRunning
					
					// send ThreadAbortException to the given thread.
					// This call is (generally) non-blocking (may block for critical section to update status, but does NOT block until Stop successful).
					//
					// Note that its legal to call Abort on a thread in any state - including nullptr. Some may just have no effect
					nonvirtual	void	Abort ();

// LGP - REVIEW WHERE THIS IS USED - WHY IS IT USEFUL -- LGP 2011-08-30
					// Note that its legal to call Stop_Forced_Unsafe on a thread in any state - including nullptr. Some may just have no effect
					nonvirtual	void	Abort_Forced_Unsafe ();	// like Abort () - but less safe, and more forceful

					// wait until thread is done (use Abort to request termination) - throws if timeout
					// Note that its legal to call WaitForDone on a thread in any state - including nullptr. Some may just have no effect
					nonvirtual	void	WaitForDone (Time::DurationSecondsType timeout = Time::kInfinite) const;

					// Note that its legal to call AbortAndWaitForDone on a thread in any state - including nullptr. Some may just have no effect
					// An example of when this is useful is if you have a thread (performing some operation on behalf of an object - with data pointers to that object)
					// and must stop the thread (its no longer useful) - but must assure its done before you destroy the rest of the data...)
					// As for example in FileUtils - DirectoryWatcher...
					nonvirtual	void	AbortAndWaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);	// throws if timeout

			#if		qPlatform_Windows
				public:
					// Look pumping messages until either time2Pump is exceeded or the thread completes. Its NOT an erorr if the
					// timeout is exceeded
					nonvirtual	void	PumpMessagesAndReturnWhenDoneOrAfterTime (Time::DurationSecondsType timeToPump = Time::kInfinite) const;
			#endif

			#if		qPlatform_Windows
				public:
					// throws if timeout
					nonvirtual	void	WaitForDoneWhilePumpingMessages (Time::DurationSecondsType timeout = Time::kInfinite) const;
			#endif

			#if			qUseThreads_WindowsNative
				public:
					nonvirtual	void	SetThreadPriority (int nPriority = THREAD_PRIORITY_NORMAL);
			#endif


				#if		qPlatform_POSIX
				public:
					static	SignalIDType		GetSignalUsedForThreadAbort ();
					static	void				SetSignalUsedForThreadAbort (SignalIDType signalNumber);
				private:
					static	int	sSignalUsedForThreadAbort_;
				#endif

				public:
					enum Status { 
						eNull,				// null thread object
						eNotYetRunning,		// created, but start not yet called
						eRunning,			// in the context of the 'Run' method
						eAborting,			// Abort () called, but the thread still hasn't yet unwound
						eCompleted,			// run has terminated (possibly by exception, possibly normally, possibly because of Abort call)
					};
					nonvirtual	Status	GetStatus () const;

				private:
					nonvirtual	Status	GetStatus_ () const;

				// Thread name does NOT need to be unique and defaults to '', but can be used on advisory basis for
				//
				public:
					nonvirtual	wstring	GetThreadName () const;
					nonvirtual	void	SetThreadName (const wstring& threadName);

				public:
					nonvirtual	bool	operator< (const Thread& rhs) const;

				private:
					class	Rep_;
					SharedPtr<Rep_>	fRep_;
			};


			Thread::IDType	GetCurrentThreadID ();


			// Represent the thread ID for display - typically as an integer
			wstring	FormatThreadID (Thread::IDType threadID);


			// Our thread abort mechanism only throws at certain 'signalable' spots in the code - like sleeps, most reads, etc.
			// This function will also trigger a throw if called inside a thread which is being aborted.
			void	CheckForThreadAborting ();

			template	<unsigned int kEveryNTimes>
				void	CheckForThreadAborting ();




		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Thread_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Thread.inl"
