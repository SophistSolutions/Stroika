/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Thread_h_
#define _Stroika_Foundation_Execution_Thread_h_ 1

#include    "../StroikaPreComp.h"

#include    <functional>
#include    <thread>

#include    "../Configuration/Common.h"
#include    "../Time/Realtime.h"
#include    "Signals.h"

#include    "IRunnable.h"



/**
 * TODO
 *
 *  \file
 *
 *      PROGRESS NOTES GETTING THREAD INTERUPTION VIA SIGNALS WORKING ON POSIX
 *
 *      @todo   SuppressAbortInContext DTOR should CheckFor...Abort...
 *
 *      @todo   DOCUMENT:
 *              With POSIX, interuption is COMPLETELY co-operative. But with windows - we can throw from inside a handful of specail
 *              'alertable' apis. One of these - is SleepEx.
 *
 *              Turning this flag on (by default on for windoze) - means that we throw if user code calls a windows alertable API
 *              in ADDITION to the usual 'co-operative' abort situations.
 *
 *              See also maybe
 *              http://bugzilla/show_bug.cgi?id=646
 *
 *              Note we have a regtest for this case in RegressionTest5_Aborting_()
 *
 *          -- LGP 2014-01-14
 *
 *      @todo   Lose fStatusCriticalSection_ - and instead use atomics - I THINK!!! At least evalute
 *              DID - 2014-01-14 - with idefs.
 *
 *      @todo   Windows thread interuption with CRITICALSECTION/lock_gaurd<>
 *              Currently we use EnterCriticalSeciton alot, But thats not 'alertable' -
 *              which potentially causes problems with thread interuption.
 *
 *              Perhaps should use something like TryEnterCriticalSection, and loop,
 *              with SleepEx(0) or equiv, sprinkled.
 *
 *              MAYBE FIXED NOW THAT WE DIRECLTY USE STL lock_guard! - or maybe worse!
 *
 *                  run test case where one thread is blocked in a critical section, and
 *                  another thread owns that critical section,
 *                  and a third thread tries to call AbortThread(). If this doesn't work, I may need
 *                  to add my OWN 'auto-critical-section' class BACK, and have it support cancelability.
 *
 *      @todo   Consider ONLY supporting std::thread based threads. And then - provide API
 *              where we can return a reference to the underlying thread object (but probably NOT
 *              one to ADOPT an existing thread because then we couldnt hook the run-proc,
 *              which is needed for our exception stuff - I think.
 *
 *              (mostly did this - but keep todo around to update docs)
 *
 *      @todo   must define C++ static signal handler
 *
 *      @todo   must install handler when about to call Abort() - no need -
 *              I think - to install sooner - bnut maybe a good idea to???
 *
 *      @todo   in signal handler - set threadlocalstorage value (asssert in context of
 *              right thread id) - set sAborted.
 *
 *      @todo   use pthread_kill (use native_handle() from threadobj) - to send the signal.
 *              (REVIEW - but I think this is bascailly done - and obsoelte - LGP 2014-01-14)
 *
 *      @todo   MAJOR REWRITE OF THREAD DOCUMENTAITON ON INTERUPTION:
 *              >>  I THINK these are all - both posix and windows - really cooperative.
 *              >>  VEIRYF thats the case on Windows - under the debugger.
 *              >>  Changes or really clarifications on the ad interruption design. Test mt theory
 *              >>  with long loop being interopted with and without debugger on windiws
 *              >>  I THINK we can just document all cooperative interuption and clearly document
 *              >>      that deisgn choice
 *
 *      @todo   Be sure no MEMORY or other resource leak in our Thread::Rep::~Rep () handling -
 *              calling detatch when a thread is never waited for. (GNU/C+++ thread impl only)
 *
 *      @todo   Add a Method (maybe overload of Start) - which takes a new Runnable, so that
 *              the thread object can be re-run. This will be needed (or at least highly advantageous)
 *              for implementing thread pools.
 *              << turns out NOT necessary for thread pools (already draft impl not using it).
 *              Not sure if good idea - so leave this here. MAYBE>>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  OVERVIEW:
             *      Stroika Threads are built on std::thread, so can be used fully interoperably. However,
             *  Stroika threads add a number of useful features to std::threads:
             *          o   Cancelation/Interuption/Aborting
             *          o   EINTR handling (POSIX only)
             *          o   Copyability
             *          o   Better lifetime management (the thread envelope - object you create - can go away, but
             *              the underlying thread can continue running, with its memory/resources being cleaned
             *              up autoamtically.
             *              (todo docs correction - thats true for C++ thread class too...)
             *
             *  Note - this cancelation feature is very handy for building large scale applications which use
             *  worker tasks and things like thread pools, to be able to reclaim resources, cancel ongoing operations
             *  as useless, and maintain overall running system integrity.
             *
             *  \em Nomenclature Note:
             *      Thread Abort == Thread Interuption == Thread Cancelation
             *      Differnt libraries use differnt names (java uses interuption, boost uses
             *      cancelation, and .net uses Abort, as examples).
             *
             *
             *  DETAILS:
             *      Using the smartpointer wrapper Thread around a thread guarantees its reference counting
             *  will work safely - so that even when all external references go away, the fact that the thread
             *  is still running will keep the reference count non-zero.
             *
             *
             *  Thread Aborting/Interuption:
             *      The Stroika Thread class supports the idea of 'aborting' a thread. In some libraries
             *  (e.g. boost) this is called 'interuption'. In others, its called 'cancellation'.
             *
             *      The basic idea is that a thread goes off on its own, doing stuff, and an external force
             *  decides to tell it to stop.
             *
             *  Examples of this might be:
             *      (1) A search user interface, which starts searching as the user types. Once the process
             *          has recieved a certain number of characters it starts searching, but perhaps before
             *          the search is done, another character comes in, so the GUI code will want to Abort
             *          the existing search, and start a new one (with the extra character(s)).
             *      (2) A web server - which is serving up content, and it told to shut-down. It must interupt
             *          existing in process processes - some maybe handling a read/write sequence, and some
             *          perhaps doing a socket listen/accept call.
             *
             *  When a thread is aborted, it (in that thread) throws
             *      class   ThreadAbortException;
             *
             *  Thread aborting is tricky todo safely and portably. We take a number of approaches:
             *      (1) We maintain a thread-local-storage variable - saying if this thread has been aborted.
             *          Sprinkling CheckForThreadAborting throughout your code - will trigger a ThreadAbortException ()
             *          in that thread context.
             *
             *      (2) Async-injection (QueueUserAPC/Widnows)  APC functions get 'suddenly launched' in the context
             *          of a given threads when its in an 'alertable state'. This APC function can then throw -
             *          essentially ending the sleep/wait/or whatever.
             *
             *          I'M NOT sur ethis is safe - and we may want to stop doing it. Instead - do more like what
             *          I plan todo for signals
             *
             *      (3) Signal injection (POSIX) - we send a special (TDB) signal to a particular thread.
             *          It sets a 'thread-local variable - aborted' and when it returns - any (WHICH?) system
             *          calls in progress will return the error
             *
             *
             *      <<<<DOCUMENT INTERUPTION POINTS>>>> - CALLED INTERUPTION POINTS IN BOOST - MAYBE WE SHOULD CALL THEM ABORT POINTS?
             *      ??? They are placed in the code caller can ASSUME a call to CheckForThreadAborting () is called. These include:
             *          o   SLEEP()
             *          o   ANY WAIT CALLS
             *          o   anything that calls Handle_ErrNoResultInteruption ()
             *
             *
             *
             * HANDLE_EINTR_CALLER()
             *      The short of it is that you need to catch EINTR and restart the call for these system calls:
             *          o read, readv, write, writev, ioctl
             *          o open() when dealing with a fifo
             *          o wait*
             *          o Anything socket based (send*, recv*, connect, accept etc)
             *          o flock and lock control with fcntl
             *          o mq_ functions which can block
             *          o futex
             *          o sem_wait (and timed wait)
             *          o pause, sigsuspend, sigtimedwait, sigwaitinfo
             *          o poll, epoll_wait, select and 'p' versions of the same
             *          o msgrcv, msgsnd, semop, semtimedop
             *          o close (although, on Linux, EINTR won't happen here)
             *          o any sleep functions (careful, you need to handle this are restart with
             *              different arguments)
             *
             *  @see HandleEINTR
             *
             *  @todo       To make STOP code more safe - and have Stop really throw ThreadAbortException
             *              - then associate a PROGRESS object with this REP, and
             *              make sure the REP (Run method) takes that guy as arg, and call 'CheckCanceled'
             *              periodically - which can do the throw properlly!!!!
             *              (actually - above is out of date - but dont delete til I verify - but since I added the
             *              NotifyOfAbort/SleepEx/QueueUserAPC stuff - it should be pretty automatic...
             *                  -- LGP 2009-05-08
             *
             *
             */
            class   Thread {
            public:
                /**
                 * Thread::IDType is a portable representation which is a key to currently existing system threads.
                 */
                using   IDType              =   thread::id;

            public:
                /**
                 *  Thread::native_handle is the type of the underlying handle to a native thread
                 *  which can allow for using platform APIs.
                 */
                using   NativeHandleType    =   thread::native_handle_type;

            public:
                /**
                 *  No arg- constructor is available for use in applications like thread pools. Also, a variety
                 *  of cases, its handy to declare a Thread data member (and init in CTOR), but not
                 *  specify what gets run until later.
                 *
                 *  After this call, GetStatus () returns Status::eNull
                 *
                 *  fun2CallOnce is called precisely once by this thread CTOR, but called in
                 *  another thread with the arg 'arg'.
                 */
                Thread ();
                Thread (const std::function<void()>& fun2CallOnce);
                Thread (const IRunnablePtr& runnable);
                template <typename FUNCTION>
                Thread (FUNCTION f, typename is_function<FUNCTION>::type* = nullptr);

            public:
                /**
                 *  Each thread has associated an IRunnable, which gets run by the thread. It can be accessed
                 *  via GetRunnable(), but is only settable in the thread constructor.
                 */
                nonvirtual  shared_ptr<IRunnable>    GetRunnable () const;

            public:
                /**
                  */
                nonvirtual  IDType              GetID () const;

            public:
                /**
                  */
                nonvirtual  NativeHandleType    GetNativeHandle () noexcept;

            public:
                /**
                  * \req    GetStatus () == Status::eNotYetRunning
                  */
                nonvirtual  void    Start ();

            public:
                /**
                 *  Send  (ThreadAbortException) to the given thread.
                 *  This call is (generally) non-blocking (may block for critical section to update status,
                 *  but does NOT block until Stop successful).
                 *
                 *  Note that its legal to call Abort on a thread in any state - including Status::eNull.
                 *  Some may just have no effect.
                 */
                nonvirtual  void    Abort ();

            public:
                /*
                 *  Note that its legal to call Stop_Forced_Unsafe on a thread in any state -
                 *  including nullptr. Some may just have no effect
                 *
                 *  @todo   REVIEW WHERE THIS IS USED - WHY IS IT USEFUL -- LGP 2011-08-30
                 *
                 */
                nonvirtual  void    Abort_Forced_Unsafe (); // like Abort () - but less safe, and more forceful

            public:
                class   SuppressAbortInContext;

            public:
                /**
                 *  Wait until thread is done (use Abort to request termination) - throws if timeout
                 *  Note that its legal to call WaitForDone on a thread in any state - including nullptr.
                 *  Some may just have no effect
                 */
                nonvirtual  void    WaitForDone (Time::DurationSecondsType timeout = Time::kInfinite) const;

            public:
                /**
                 *  Note that its legal to call AbortAndWaitForDone on a thread in any state -
                 *  including nullptr. Some may just have no effect
                 *  An example of when this is useful is if you have a thread (performing some operation on
                 *  behalf of an object - with data pointers to that object)
                 * and must stop the thread (its no longer useful) - but must assure its done before you
                 *  destroy the rest of the data...)
                 * As for example in FileUtils - DirectoryWatcher...
                 *
                 *  throws if timeout
                 */
                nonvirtual  void    AbortAndWaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);

#if     qPlatform_Windows
            public:
                /**
                 *  Look pumping messages until either time2Pump is exceeded or the thread completes.
                 *  Its NOT an erorr if the timeout is exceeded.
                 *
                 *  throws if timeout
                 */
                nonvirtual  void    WaitForDoneWhilePumpingMessages (Time::DurationSecondsType timeout = Time::kInfinite) const;
#endif


            public:
                enum class Priority {
                    eLowest,
                    eBelowNormal,
                    eNormal,
                    eAboveNormal,
                    eHighest,

                    Stroika_Define_Enum_Bounds(eLowest, eHighest)
                };
                /**
                 *  This is a portable wrapper on setting thread priorities. It has fewer knobs than direct or low level
                 *  APIs. You can always directly call low-level native APIs using GetNativeHandle().
                 */
                nonvirtual  void    SetThreadPriority (Priority priority = Priority::eNormal);


#if     qPlatform_POSIX
            public:
                static  SignalID        GetSignalUsedForThreadAbort ();
                static  void            SetSignalUsedForThreadAbort (SignalID signalNumber);
            private:
                static  SignalID        sSignalUsedForThreadAbort_;
#endif


            public:
                enum class Status : uint8_t {
                    eNull,              // null thread object
                    eNotYetRunning,     // created, but start not yet called
                    eRunning,           // in the context of the 'Run' method
                    eAborting,          // Abort () called, but the thread still hasn't yet unwound
                    eCompleted,         // run has terminated (possibly by exception, possibly normally, possibly because of Abort call)

                    Stroika_Define_Enum_Bounds(eNull, eCompleted)
                };
                /**
                 *  Each Thread object has an associated state. Since this can be accessed from one thread while things
                 *  in another thread, by the time the answer is returned, the value may have changed.
                 *
                 *  But only certain transitions are legal. Status::eNull is the starting state for SOME Thread constructors (no arg).
                 *  Nothing transitions back to the Status:eNull state. From there when you specify an IRunnable to run,
                 *  then you transition to Status::eNotYetRunning.
                 *
                 *  From eNotYetRunning, you can transition to eRunning with Start ().
                 *
                 *  From eRunning (or eNotYetRunning) you can transition to eAborting or eCompleted (or from eAborting to eCompleted).
                 *
                 *  A thread object can never transition back (by this I mean the underlying pointed to rep - the container of
                 *  course can transition back by being assigned another ThreadRep).
                 */
                nonvirtual  Status  GetStatus () const noexcept;

            private:
                nonvirtual  Status  GetStatus_ () const noexcept;

            public:
                /**
                 *  \req GetStatus () != Status::eNull
                 *
                 *  Thread name does NOT need to be unique and defaults to '', but can be used on advisory
                 *  basis for debugging. Also - setting the thread name, and then getting it back, may not
                 *  produce the same result. Some OSes may munge the name to make it unique, or conform
                 *  to some naming convention.
                 *
                 *  These names should not be counted on for program logic.
                 */
                nonvirtual  wstring GetThreadName () const;

            public:
                /**
                 *  \req GetStatus () != Status::eNull
                 *
                 *  @see GetThreadName ();
                 */
                nonvirtual  void    SetThreadName (const wstring& threadName);

            public:
                nonvirtual  bool    operator< (const Thread& rhs) const;

            private:
                class   Rep_;
                shared_ptr<Rep_> fRep_;
            };


            /**
             *  This object - while in existance, blocks delivery of all abort signals (for this thread in which its instantiated).
             *  These objects nest.
             */
            class   Thread::SuppressAbortInContext {
            public:
                SuppressAbortInContext ();
                SuppressAbortInContext (const SuppressAbortInContext&) = delete;
                const SuppressAbortInContext& operator= (const SuppressAbortInContext&) = delete;
                ~SuppressAbortInContext ();
            };


            /**
             */
            Thread::IDType  GetCurrentThreadID () noexcept;


            /**
             *  Represent the thread ID for display - typically as an integer
             */
            wstring FormatThreadID (Thread::IDType threadID);


            /**
             *  Our thread abort mechanism only throws at certain 'signalable' spots in the code -
             *  like sleeps, most reads, etc.
             *  This function will also trigger a throw if called inside a thread which is being aborted.
             */
            void    CheckForThreadAborting ();

            template    <unsigned int kEveryNTimes>
            void    CheckForThreadAborting ();


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Thread.inl"

#endif  /*_Stroika_Foundation_Execution_Thread_h_*/
