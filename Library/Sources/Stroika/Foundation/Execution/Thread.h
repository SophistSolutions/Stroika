/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Thread_h_
#define _Stroika_Foundation_Execution_Thread_h_ 1

#include    "../StroikaPreComp.h"

#include    <functional>
#include    <thread>

#include    "../Configuration/Common.h"
#include    "../Time/Realtime.h"

#include    "Function.h"
#include    "Signals.h"



/**
 * TODO
 *
 *  \file
 *
 *
 *      @todo   DOCS and review impl/test impl of abort/thread code. Add test case for Interrupt.
 *
 *      @todo   Consider using ThrowIfDoneWithException () more thoroughly, possibly
 *              redefining the meaning of WaitForDone() or possibly adding WaitAndThrowAnyChildThreadExceptions
 *              or make optional if existing WAIT API throws child excpetions. Maybe paraemter in construction
 *              of the thread?
 *
 *      @todo   SuppressInterruptionInContext DTOR should CheckFor...Abort...
 *
 *      @todo   DOCUMENT:
 *              With POSIX, interuption is COMPLETELY co-operative. But with windows - we can throw from inside a handful of special
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
 *      @todo   Provide API where we can return a reference to the underlying thread object
 *              (but probably one to ADOPT an existing thread because then we couldnt hook the run-proc,
 *              which is needed for our exception stuff - I think.
 *
 *              (mostly did this - but keep todo around to update docs)
 *
 *      @todo   Be sure no MEMORY or other resource leak in our Thread::Rep::~Rep () handling -
 *              calling detatch when a thread is never waited for. (GNU/C+++ thread impl only)
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  OVERVIEW:
             *      Stroika Threads are built on std::thread, so can be used fully interoperably. However,
             *  Stroika threads add a number of very useful features to std::threads:
             *          o   Cancelation/Interuption/Aborting
             *          o   EINTR handling (POSIX only)
             *
             *  as well as a couple modestly helpful features (that can be done other ways directly with std::thread):
             *          o   Copyability (by reference - reference counted)
             *          o   Better lifetime management (the thread envelope - object you create - can go away, but
             *              the underlying thread can continue running, with its memory/resources being cleaned
             *              up autoamtically.
             *              (todo docs correction - thats true for C++ thread class too...)
             *
             *  Note - this cancelation feature is very handy for building large scale applications which use
             *  worker tasks and things like thread pools, to be able to reclaim resources, cancel ongoing operations
             *  as useless, and maintain overall running system integrity.
             *
             *  DETAILS:
             *      Using the smartpointer wrapper Thread around a thread guarantees its reference counting
             *  will work safely - so that even when all external references go away, the fact that the thread
             *  is still running will keep the reference count non-zero.
             *
             *  Thread Aborting/Interuption:
             *      The Stroika Thread class supports the idea of 'aborting' a thread.
             *
             *  \em Nomenclature Note:
             *       In some libraries, the term interuption, cancelation is used for thread aborting.
             *
             *              >   java uses interuption
             *              >   boost uses cancelation,
             *              >   and .net uses Interrupt and Abort
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
             *  When a thread is interrupted, it (in that thread) throws
             *      class   InterruptException;
             *
             *  When a thread is aborted, it (in that thread) throws
             *      class   AbortException;
             *
             *  The only difference between Interruption and Aborting is that Aborting is permanent, whereas
             *  Interrupt happens just once.
             *
             *  Thread interruption/aborting is tricky todo safely and portably. We take a number of approaches:
             *      (1) We maintain a thread-local-storage variable - saying if this thread has been aborted.
             *          Sprinkling CheckForThreadInterruption throughout your code - will trigger a AbortException ()
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
             *      <<<<DOCUMENT INTERUPTION POINTS>>>> - CALLED INTERUPTION POINTS IN BOOST - MAYBE WE SHOULD CALL THEM ABORT POINTS?
             *      ??? They are placed in the code caller can ASSUME a call to CheckForThreadInterruption () is called. These include:
             *          o   SLEEP()
             *          o   ANY WAIT CALLS
             *          o   anything that calls Handle_ErrNoResultInteruption ()
             *
             *  @todo   DOCUMENT IMPACT ON WaitableEvents, std::mutex, (etc), std::condition_variable, and AbortableEvent, etc.
             *
             * Handle_ErrNoResultInteruption()
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
             *      This is integrated with Stroika's thread cancelation mechanism.
             *
             *  @see Handle_ErrNoResultInteruption
             *
             *  @todo       To make STOP code more safe - and have Stop really throw AbortException
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
                enum    AutoStartFlag { eAutoStart };

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
                Thread (const Function<void()>& fun2CallOnce);
                Thread (const Function<void()>& fun2CallOnce, AutoStartFlag);
                template <typename FUNCTION>
                Thread (FUNCTION f, typename enable_if<is_function<FUNCTION>::value>::type* = nullptr);
                template <typename FUNCTION>
                Thread (FUNCTION f, AutoStartFlag, typename enable_if<is_function<FUNCTION>::value>::type* = nullptr);

            public:
                /**
                 * Generally should not be reported. It's just to help force a thread to shut itself down
                 */
                class   AbortException;

            public:
                /**
                 * Generally should not be reported. This is to support Thread::Interupt();
                 */
                class   InterruptException;

            public:
                /**
                 *  Each thread has associated an std::function, which gets run by the thread. It can be accessed
                 *  via GetFunction(), but is only settable in the thread constructor.
                 */
                nonvirtual  Function<void()>    GetFunction () const;

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
                 *  Abort gracefully shuts down and terminates the given thread.
                 *
                 *  This works by setting a flag in that thread, which is chekced at each 'cancelation point', and
                 *  can interrupt certain cancelation waiting cancelation points.
                 *
                 *  This causes the given thread to throw an AbortException whenever it reaches one of these cancelation points
                 *  (or if already at one such waitable cancelation point).
                 *
                 *  This call is (generally) non-blocking (may block for critical section to update status,
                 *  but does NOT block until Stop successful).
                 *
                 *  Note that its legal to call Abort on a thread in any state - including Status::eNull.
                 *  Some may just have no effect.
                 *
                 *  @see Interrupt
                 */
                nonvirtual  void    Abort ();

            public:
                /**
                 *  Interrupt is meant to stop what a thread is currently doing. Calling this sets a thread-local-storage variable
                 *  inside that thread, so that at the next cancelation point, it will throw the InterruptedException.
                 *
                 *  When InterruptedException is thrown, that thread local storage flag is cleared. Often code will catch and rethrow
                 *  the exception, but the Interupt state doesnt perisist past when its first handled (this is in stark contrast to Abort).
                 *
                 *  If the InterruptException is not handled, it will terminate the thread  (go to the done state).
                 *
                 *  Like Abort(), sending an Interupt() to an expired (aborted) or null thread will be ignored - simply never delivered.
                 *
                 *  Note this can be called from any thread, whether the thread object being interupted, or (more typically) from another.
                 *
                 *  A thread being Aborted can also be interrupted, but Abort() takes precedence if both are attempted.
                 *
                 *  \note   If the function associated with the thread doesn't handle the InteruptExcpetion, this will effectively
                 *          abort the thread (as would any other exception).
                 *
                 *  \note   only partly implemented and untested
                 *
                 *  @see Abort
                 */
                nonvirtual  void    Interrupt ();

            public:
                /**
                 *  Note that its legal to call Stop_Forced_Unsafe on a thread in any state -
                 *  including nullptr. Some may just have no effect
                 *
                 *  @todo   REVIEW WHERE THIS IS USED - WHY IS IT USEFUL -- LGP 2011-08-30
                 *
                 */
                nonvirtual  void    Abort_Forced_Unsafe (); // like Abort () - but less safe, and more forceful

            public:
                class   SuppressInterruptionInContext;

            public:
                /**
                 *  Wait until thread is done (use Abort to request termination) - throws if timeout
                 *  Note that its legal to call WaitForDone on a thread in any state - including nullptr.
                 *  Some may just have no effect
                 */
                nonvirtual  void    WaitForDone (Time::DurationSecondsType timeout = Time::kInfinite) const;

            public:
                /**
                 *  Wait until thread is done (use Abort to request termination) - throws if timeout
                 *  Note that its legal to call WaitForDoneUntil on a thread in any state - including nullptr.
                 *  Some may just have no effect
                 */
                nonvirtual  void    WaitForDoneUntil (Time::DurationSecondsType timeoutAt) const;

            public:
                /**
                 *  Note that its legal to call AbortAndWaitForDone on a thread in any state -
                 *  including nullptr. Some may just have no effect
                 *  An example of when this is useful is if you have a thread (performing some operation on
                 *  behalf of an object - with data pointers to that object)
                 *  and must stop the thread (its no longer useful) - but must assure its done before you
                 *  destroy the rest of the data...)
                 *  As for example in FileUtils - DirectoryWatcher...
                 *
                 *  throws if timeout
                 */
                nonvirtual  void    AbortAndWaitForDone (Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  If the thread is Done() - and completed with an exception (other than interupt),
                 *  this throws that exception, allowing an exception thrown inside a thread,
                 *  to be propagated across threads.
                 *
                 *  \note   This function intentionally omits InterruptException and AbortException from
                 *          this rule, so that if you abort or interrupt a thread and then call this method,
                 *          it will not rethrow.
                 *
                 *          The reason for this choice is that the interrupt/abort are usually generated externally
                 *          and the intent of this method is to pass information back from the thread to the caller.
                 */
                nonvirtual  void    ThrowIfDoneWithException ();

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
                enum    class   Priority {
                    eLowest,
                    eBelowNormal,
                    eNormal,
                    eAboveNormal,
                    eHighest,

                    Stroika_Define_Enum_Bounds(eLowest, eHighest)
                };

            public:
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
                enum    class   Status : uint8_t {
                    eNull,              // null thread object
                    eNotYetRunning,     // created, but start not yet called
                    eRunning,           // in the context of the 'Run' method
                    eAborting,          // Abort () called, but the thread still hasn't yet unwound
                    eCompleted,         // run has terminated (possibly by exception, possibly normally, possibly because of Abort call)

                    Stroika_Define_Enum_Bounds(eNull, eCompleted)
                };

            public:
                /**
                 *  Each Thread object has an associated state. Since this can be accessed from one thread while things
                 *  in another thread, by the time the answer is returned, the value may have changed.
                 *
                 *  But only certain transitions are legal. Status::eNull is the starting state for SOME Thread constructors (no arg).
                 *  Nothing transitions back to the Status:eNull state. From there when you specify a std::function<> to run,
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
             */
            class   Thread::InterruptException {
            public:
                InterruptException ();
            };


            /**
             */
            class   Thread::AbortException : public Thread::InterruptException {
            public:
                AbortException ();
            };


            /**
             *  This object - while in existance, blocks delivery of all Interrupt Exceptions (InterruptException and AbortException)
             *  (for this thread in which its instantiated). This blocking nest (so if you have two of them in one thread, only when the last
             *  one is destroyed does the block on Interuption be removed).
             *
             *  This is used to prevent a second abort request coming in to a thread already in the process of shutting down, which
             *  might cause a second, or incomplete cleanup.
             *
             *  Note - this does NOT prevent the delivery of Windows APC messages, nor POSIX interrupt signals. It just prevents that
             *  from doing anything (other than maybe an EINTR return).
             *
             *  It might be NICE to have the DTOR be a cancelation point, but that would violate the rule that you cannot throw
             *  from a destructor.
             *
             *  Any blocked Interrupt Exceptions will wait til the next cancelation point to be invoked (so call
             *  CheckForThreadInterruption to force that).
             */
            class   Thread::SuppressInterruptionInContext {
            public:
                SuppressInterruptionInContext ();
                SuppressInterruptionInContext (const SuppressInterruptionInContext&) = delete;
                SuppressInterruptionInContext& operator= (const SuppressInterruptionInContext&) = delete;
                ~SuppressInterruptionInContext ();
            };


            /**
             */
            Thread::IDType  GetCurrentThreadID () noexcept;


            /**
             *  Represent the thread ID for display - typically as an integer
             */
            wstring     FormatThreadID (Thread::IDType threadID);


            /**
             *  Our thread abort mechanism only throws at certain 'signalable' (alertable) spots in the code -
             *  like sleeps, most reads, etc.
             *  This function will also trigger a throw if called inside a thread which is being aborted.
             *
             *  Any call to this routine is a 'cancelation point'.
             */
            void    CheckForThreadInterruption ();
            template    <unsigned int kEveryNTimes>
            void    CheckForThreadInterruption ();


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
