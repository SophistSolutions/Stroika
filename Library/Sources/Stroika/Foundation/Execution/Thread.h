/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Thread_h_
#define	_Stroika_Foundation_Execution_Thread_h_	1

#include	"../StroikaPreComp.h"

#if			qUseThreads_WindowsNative
	#include	<windows.h>
#endif

#include	"../Configuration/Common.h"
#include	"../Memory/SharedPtr.h"
#include	"../Time/Realtime.h"

#include	"CriticalSection.h"
#include	"Exceptions.h"
#include	"Event.h"
#include	"IRunnable.h"




/*
@CONFIGVAR:		qUseThreads_WindowsNative
@DESCRIPTION:	<p>qUseThreads_WindowsNative is true iff Stroika is built to use a native Windows SDK implementation of threads</p>
*/
#if		!defined (qUseThreads_WindowsNative)
	#error "qUseThreads_WindowsNative should normally be defined indirectly by StroikaConfig.h"
#endif

/*
@CONFIGVAR:		qUseThreads_StdCPlusPlus
@DESCRIPTION:	<p>qUseThreads_StdCPlusPlus is true iff Stroika is built to use portable standard C++ implementation of threads</p>
*/
#if		!defined (qUseThreads_StdCPlusPlus)
	#error "qUseThreads_StdCPlusPlus should normally be defined indirectly by StroikaConfig.h"
#endif



#if		qUseThreads_WindowsNative && qUseThreads_StdCPlusPlus
	#error "Configuraiton Error: cannot define both qUseThreads_WindowsNative && qUseThreads_StdCPlusPlus"
#endif




namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			using	Memory::SharedPtr;

			// Using the smartpointer wrapper Thread around a thread guarnatees its reference counting
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
			class	Thread {
				private:
					class	Rep_;
				public:
					// TODO:
					//			(o)		Either get rid of no-arg CTOR, or explain why its useful
					//
					Thread ();

					// TODO:
					//			(o)		Add more function-pointer overloads (e.g. function with no args, and function with 2 args - and maybe templated variant?
					//
					//
					// fun2CallOnce is called precisely once by this thread CTOR, but called in another thread with the arg 'arg'.
					explicit Thread (void (*fun2CallOnce) (void* arg), void* arg);
					explicit Thread (const SharedPtr<IRunnable>& runnable);

				public:
				#if			qUseThreads_WindowsNative
					nonvirtual	HANDLE			GetOSThreadHandle () const;
				#endif

				public:
					nonvirtual	SharedPtr<IRunnable>	GetRunnable () const;

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
					nonvirtual	void	WaitForDone (Time::DurationSecondsType timeout = -1.0f) const;

					// Note that its legal to call AbortAndWaitForDone on a thread in any state - including nullptr. Some may just have no effect
					// An example of when this is useful is if you have a thread (performing some operation on behalf of an object - with data pointers to that object)
					// and must stop the thread (its no longer useful) - but must assure its done before you destroy the rest of the data...)
					// As for example in FileUtils - DirectoryWatcher...
					nonvirtual	void	AbortAndWaitForDone (Time::DurationSecondsType timeout = -1.0f);	// throws if timeout

					// Look pumping messages until either time2Pump is exceeded or the thread completes. Its NOT an erorr if the
					// timeout is exceeded
					nonvirtual	void	PumpMessagesAndReturnWhenDoneOrAfterTime (Time::DurationSecondsType timeToPump = -1.0f) const;
					nonvirtual	void	WaitForDoneWhilePumpingMessages (Time::DurationSecondsType timeout = -1.0f) const;	// throws if timeout

			#if			qUseThreads_WindowsNative
				public:
					nonvirtual	void	SetThreadPriority (int nPriority = THREAD_PRIORITY_NORMAL);
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
					SharedPtr<Rep_>	fRep_;
			};



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
