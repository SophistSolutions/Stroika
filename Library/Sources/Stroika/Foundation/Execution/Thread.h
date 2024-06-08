/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Thread_h_
#define _Stroika_Foundation_Execution_Thread_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <functional>
#include <map>
#include <optional>
#include <thread>

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Signals.h"
#include "Stroika/Foundation/Time/Realtime.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/**
 *  \file
 *
 * TODO
 *      @todo   Probably no longer need siginterrupt () calls, since we DON'T set SA_RESTART in our call to sigaction().
 */
namespace Stroika::Foundation::Characters {
    class String;
}

namespace Stroika::Foundation::Execution {

    /**
     *  This is only meant for debugging. If true, track the running threads (and provide API to access)
     *  and DbgTrace() automatically in construction/destruction.
     */
#ifndef qStroika_Foundation_Execution_Thread_SupportThreadStatistics
#define qStroika_Foundation_Execution_Thread_SupportThreadStatistics qDebug
#endif

    /**
     *  \brief  Thread is a namespace for Stroika thread code, @see Thread::Ptr or @Thread::New
     *
     *      Stroika Threads are built on std::jthread, so can be used mostly interoperably. However,
     *  Stroika threads add a number of very useful features to std::threads:
     *          o   Simpler Cancelation/Interruption/Aborting
     *              (c++ 20 introduces thread cancelation via explicitly managed stop_tokens, but Stroika's thread cancelation
     *              uses this, but hides it - mostly).
     *          o   EINTR handling (POSIX only)
     *
     *  as well as a couple modestly helpful features (that can be done other ways directly with std::thread):
     *          o   Copyability (using Thread::Ptr)
     *          o   Better lifetime management (the thread envelope - object you create - can go away, but
     *              the underlying thread can continue running, with its memory/resources being cleaned
     *              up autoamtically.
     *          o   And several minor features like (mostly) portably setting thread priorities, names, etc and more
     *
     *  Note - the cancelation feature is very handy for building large scale applications which use
     *  worker tasks and things like thread pools, to be able to reclaim resources, cancel ongoing operations
     *  as useless, and maintain overall running system integrity.
     *
     *  Using the smartpointer wrapper Thread around a thread guarantees its reference counting
     *  will work safely - so that even when all external references go away, the fact that the thread
     *  is still running will keep the reference count non-zero.
     *
     *  Thread Aborting:
     *      The Stroika Thread class supports the idea of 'aborting' a thread (cooperatitve cancelation)
     *
     *  Thread Interruoption:
     *      Stroika v2.1 and earlier supported a feature like Thread-Abort, called 'Interrupt' - that acted like
     *      thread abort, except that it was not sticky - and once the interrupt was handled, it 'reset' to not
     *      interrupting anymore.
     * 
     *      I never really found any compelling use case for this idea (borrowed from java thread interrupt).
     * 
     *      And I think the scenarios where it MIGHT be helpful, could be handled just as easily with
     *      condition variables.
     * 
     *      Also, c++20 added support which allows thead aborting (stop_token), but that doesn't work well/easily
     *      for basic interruption.
     * 
     *      So - abandoned Thread::Interrupt(...) in Stroika v3.
     *
     *  \em Nomenclature Note:
     *       In some libraries, the term interruption, cancelation is used for thread aborting.
     *              >   java uses interruption
     *              >   boost uses cancelation,
     *              >   POSIX uses cancelation (pthread_canel)
     *              >   and .net uses Interrupt and Abort
     *
     *  The basic idea is that a thread goes off on its own, doing stuff, and an external force
     *  decides to tell it to stop.
     *
     *  Examples of this might be:
     *      (1) A search user interface, which starts searching as the user types. Once the process
     *          has received a certain number of characters it starts searching, but perhaps before
     *          the search is done, another character comes in, so the GUI code will want to Abort
     *          the existing search, and start a new one (with the extra character(s)).
     *      (2) A web server - which is serving up content, and it told to shut-down. It must interrupt
     *          existing in process processes - some maybe handling a read/write sequence, and some
     *          perhaps doing a socket listen/accept call.
     *
     *  When a thread is aborted, it (in that thread) throws
     *      class   AbortException;
     *
     *  Thread 'interruption' happens via 'cancelation points'. Cancelation points are points in the code
     *  where we check to see if the current running thread has been interrupted (or aborted) and raise
     *  the appropriate exception. 
     *
     *  This mechanism is almost completely co-operative, meaning that user
     *  code must call CheckForInterruption () in the right places to make interruption work. But Stroika
     *  is structured to make that happen automatically throughout most of its code, but having key routines (like Sleep, and WaitableEvents)
     *  automatically internally be cancelation points.
     *
     *  The only slight exception to this is on Windows, where we have the APC mechanism that will interupt a wide vareity of windows
     *  system calls (see docs below).
     *
     *  \note - its important that this 'interruption' can only happen at well defined times, because that allows
     *        for safe and reliable cleanup of whatever activitites were being done (e.g. cannot interupt in a destructor)
     *
     *  Thread interruption/aborting is tricky todo safely and portably. We take a safe, cooperative approach.
     *      (1) We maintain a thread-local-storage variable - saying if this thread has been aborted.
     *          Sprinkling CheckForInterruption throughout your code - will trigger an InterupptedException () or AbortException ()
     *          in that thread context. Note a pointer to that TLS interruption variable is also stored
     *          in the thread 'rep' object, so it can be set (by Thread::Interrupt).
     *
     *      (2) WINDOWS ONLY: Async-injection (QueueUserAPC/Windows) - Alertable state -  certain (alertable) functions get interrupted and return 
     *          WAIT_IO_COMPLETION (like POSIX EINTR).
     *
     *          @see https://msdn.microsoft.com/en-us/library/windows/desktop/aa363772(v=vs.85).aspx
     *              o   SleepEx
     *              o   WaitForSingleObjectEx
     *              o   WaitForMultipleObjectsEx
     *              o   SignalObjectAndWait
     *              o   MsgWaitForMultipleObjectsEx
     *          But also from https://msdn.microsoft.com/en-us/library/windows/desktop/ms741669(v=vs.85).aspx
     *              o   WSAPoll ... Winsock performs an alertable, similarly for recvfrom, accept, etc. Not clear how to get full list of
     *                  windows alertable functions...
     *
     *          This allows interrupting windows waiting / blocking functions (most of them).
     *
     *      (3) POSIX ONLY: Signal injection - we send a special (defaults to SIG_USR2) signal to a particular thread.
     *          This triggers an EINTR on most UNIX system calls, which are automatically restarted in most cases
     *          (@see Execution::Handle_ErrNoResultInterruption), but in case of interruption, we call
     *          CheckForInterruption ()
     *
     *          This allows interrupting UNIX waiting / blocking functions (all of them?).
     *
     *  \note @see defails on cancelation points, because many common std C++ blocking operations, like std::mutex are not
     *        std::condition_variable are not cancelation points and so can break this mechanism if not used carefully
     *
     * Handle_ErrNoResultInterruption()
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
     *  @see Handle_ErrNoResultInterruption
     *
     *  ***Thread Cancelation Points***
     *      A cancelation point is any (typically but not always blocking) function which will be interrupted (cause interruption exception) and stop blocking,
     *      when someone calls Thread::Interupt or Thread::Abort() on its thread object.
     *
     *      Roughly, these are subroutines which call 
     *          CheckForInterruption ()
     *      frequently, internally.
     *
     *      As its crucial to understand this in the API, we document each such function with ***Cancelation Point*** in its doc header.
     *      For example, the Sleep() overloads are cancelation points.
     *
     *      Equally important to understand, is when a function guarantees its NOT a cancelation point - which we will document
     *      with ***Not Cancelation Point***, and typically also noexcept. The DbgTrace () calls fall into this category.
     *
     *      \note An API marked *** Cancelation Point *** will always CheckForInterruption () at least once (or equivalent check)
     *            and will never block indefinitely without periodically checking for interruption.
     *
     *  \note   Stroika threads lifetime must NOT extend outside the lifetime of 'main'. That means they cannot
     *          be started by static constructors, and must not be left running past the end of main, to
     *          be shut down by static destructors. The reason for this is that its too hard to guarantee
     *          all the rest of the Stroika infrastructure (signals etc) work properly in that context,
     *          and its do overwhemlingly likely a softare bug waiting to happen.
     *
     *          If you must have a thread running like that, use std::thread (though I'm not sure how well that
     *          will work either).
     *
     *          Note the Thread 'smartpointer' wrapper can be constructed/destructed statically (before/after main). You
     *          just cannot start a thread before main, nor allow it to still be running (not completed) before exiting.
     *          #if qStroika_Foundation_Execution_Thread_SupportThreadStatistics (defaults true in debug builds) an attempt
     *          is made to auto-detect this and diagnose it in the tracelog and with assertions.
     *
     *  \note   Considered adding an API to return the underlying std::thread or std::thread::native_handle (), but
     *          have not needed this so far, and it might create some confusion about ownership? Detaching would be
     *          a problem because the owned thread object has hooks into the Stroika Thread object, so that kind of
     *          needs to be left around. I suppose we might want to allow an accessor to std::thread* which is like an
     *          internal pointer (thread safety questions too with that). MAYBE better to just allow construction of a 
     *          Stroika Thread from an std::thread (but then we don't have the wrapper logic for maining reference counts).
     *          No compelling need so far, and no obviously best approach.
     */
    namespace Thread {

        /**
         * Thread::IDType is a portable representation which is a key to currently existing system threads.
         */
        using IDType = thread::id;

        /**
         *  Thread::native_handle is the type of the underlying handle to a native thread
         *  which can allow for using platform APIs.
         */
        using NativeHandleType = thread::native_handle_type;

        /**
         *  \brief  EXPERIMENTAL SUPPORT FOR THREAD STACK (and maybe other) settings
         *
         *  @todo COULD add things like CPU afinity, or thread prority.
         *
         *  These configuration describe how Stroika will construct new Stroika Thread objects.
         */
        struct Configuration {
            /**
             *      \note see https://stroika.atlassian.net/browse/STK-474 - @todo and NYI
             */
            optional<size_t> fStackSize;

            /**
             *      \note see https://stroika.atlassian.net/browse/STK-474 - @todo and NYI
             *
             *      \note   NYI - see &&&& - probably availble for POSIX, but not sure for windoze threads
             *              http://man7.org/linux/man-pages/man3/pthread_attr_setguardsize.3.html
             *              Perhaps for windows just add to end of stacksize
             */
            optional<size_t> fStackGuard;

#if qPlatform_Windows
            optional<bool> fThrowInterruptExceptionInsideUserAPC;
#endif
        };

        /**
         *  Return or Set the global default configuration for Thread object construction.
         */
        Configuration DefaultConfiguration () noexcept;
        Configuration DefaultConfiguration (const optional<Configuration>& newConfiguration);

        /**
         * optional flag for constructing new threads
         */
        enum AutoStartFlag {
            eAutoStart
        };

        /**
         *  Configuration::DefaultNames<> is defined for this enumeration.
         */
        enum class Status : uint8_t {
            eNotYetRunning, // created, but start not yet called
            eRunning,       // in the context of the 'Run' method
            eAborting,      // Abort () called, but the thread still hasn't yet unwound
            eCompleted,     // run has terminated (possibly by exception, possibly normally, possibly because of Abort call)

            Stroika_Define_Enum_Bounds (eNotYetRunning, eCompleted)
        };

        /**
         *
         *  \note   Configuration::DefaultNames<> supported
         */
        enum class Priority {
            eLowest,
            eBelowNormal,
            eNormal,
            eAboveNormal,
            eHighest,

            Stroika_Define_Enum_Bounds (eLowest, eHighest)
        };

        /**
         *
         */
        struct ConfigurationStatus : Configuration {
            /**
             *  Stack starts at base, and grows towards limit - could be up or down.
             */
            optional<const byte*> fStackBase;

            /**
             *  @see fStackBase
             */
            optional<const byte*> fStackLimit;

            /**
             *  @see fStackBase, fStackLimit
             */
            optional<const byte*> fCurrentStackAt;

            /**
             *  Return current stack used, if available
             *
             *  \note NYI
             */
            optional<size_t> GetStackUsed () const;

            /**
             *  Return current stack available, if available
             *
             *  \note NYI
             */
            optional<size_t> GetStackAvailable () const;
        };

        /**
         *  \brief  Thread::Ptr is a (unsynchronized) smart pointer referencing an internally synchronized std::thread object (rep), with special feautres, including cancelation
         *
         *  \note Since this is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
         *        the underlying thread object.
         *
         *  \see    Thread::CleanupPtr
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
         */
        class Ptr {
        private:
            class Rep_;

        public:
            /**
             *  \par Example Usage
             *      \code
             *          Thread::Ptr t   = Thread::New ([r]() { r->Run (); }, Thread::eAutoStart, "Thread Name");
             *          Thread::Ptr t2  = t;
             *      \endcode
             */
            Ptr () = default;
            Ptr (nullptr_t);
            Ptr (const Ptr& src);
            Ptr (Ptr&& src) noexcept;

        protected:
            Ptr (const shared_ptr<Rep_>& rep);

        public:
            /**
            */
            nonvirtual Ptr& operator= (const Ptr& rhs);
            nonvirtual Ptr& operator= (Ptr&& rhs) noexcept;

        public:
            /**
             *  \brief Clear the pointer this Thread smart pointer refers to. This does nothing to affect the state of the underlying thread.
             *
             *  \Ensure *this == nullptr
             */
            nonvirtual void reset () noexcept;

        public:
            /**
             *  Each thread has associated an std::function, which gets run by the thread. It can be accessed
             *  via GetFunction(), but is only settable in the thread constructor.
             */
            nonvirtual function<void ()> GetFunction () const;

        public:
            /**
             *  \note   GetID () may return a default-constructed IDType if the thread is not running (has not been started, or has terminated)
             */
            nonvirtual IDType GetID () const;

        public:
            /**
             *  \note   GetNativeHandle () may return a default-constructed NativeHandleType if the thread is not running (has not been started, or has terminated)
             */
            nonvirtual NativeHandleType GetNativeHandle () const noexcept;

#if __cpp_lib_jthread >= 201911
        public:
            /**
             */
            nonvirtual stop_token GetStopToken () const;
#endif

        public:
            /**
             *  For start method
             */
            enum WaitUntilStarted {
                eWaitUntilStarted
            };

        public:
            /**
             * \req    GetStatus () == Status::eNotYetRunning or Status::eAborting
             * \req    never called before on this thread object
             *
             *  Typically you won't call this directly, but instead pass the eStart parameter to the Thread constructor.
             *  But you can explicitly call if you prefer.
             * 
             *  A thread can only be started once. When it is constructed (without eStart) - it starts in the NotYetRunning state. And can
             *  only be started when its in that state.
             * 
             *  The zero argument Start overload returns, with the thread in any possible state.
             *  This function returns quickly, and the state of the thread may not have changed by the time it ru
             *  ensures when it returns the state is running, or completed, but cannot (yet) be aborted?? or null or not yet running
             * 
             *  The Start (WaitUntilStarted) overload waits for the thread to have started (or throws if it cannot start thread). Note the thread
             *  can still run to completion, or be aborted somewhere along the lines. So in this case:
             *      Ensure (s == Status::eRunning or s == Status::eAborting or s == Status::eCompleted );
             */
            nonvirtual void Start () const;
            nonvirtual void Start (WaitUntilStarted) const;

#if qPlatform_Windows
        public:
            /**
             *  CalledInRepThreadAbortProc_ USED TO (until Stroika 2.0a234) - call CheckForThreadInterupption () in most cases. But that appeared to cause some trouble
             *  problably because of Windows library code calling an altertable function without being prepared for it to throw. So we adopted 
             *  a safer apporach, and just follow all these alertable calls with CheckForInterruption().
             *
             *  However, occasionally you use a library (like gsoap) that makes this difficult, so for those cases, enable this throw from APC feature.
             *
             *  \note Get function CAN be called with *this == nullptr, but 
             *  \req if throwInterruptExceptionInsideUserAPC then *this != nullptr;
             */
            nonvirtual bool ThrowInterruptExceptionInsideUserAPC () const noexcept;
            nonvirtual bool ThrowInterruptExceptionInsideUserAPC (optional<bool> throwInterruptExceptionInsideUserAPC);
#endif

        public:
            /**
             *  \brief Abort gracefully shuts down and terminates the given thread (using cooperative multitasking).
             *
             *  \note Since Stroika v3.0 (and C++20), this is accompished internally via std::stop_token. 
             *
             *  Cancelation works by setting a flag in that thread, which is checked at each 'cancelation point' (plus a little
             *  extra magic).
             *
             *  This causes the given thread to throw an AbortException whenever it reaches one of these cancelation points
             *  (or if already at one such waitable cancelation point).
             *
             *  This call is (generally) non-blocking (may block briefly for critical section to update status,
             *  but does NOT block until Stop successful). See AbortAndWaitUntilDone() to abort and wait for completion.
             *
             *  This can be called on a thread object at any time (reps are internally syncrhonized), and in any state (except nullptr, which can only happen in the thread
             *  was created with nullptr/default CTOR and never assigned from Thread::New).
             *
             *  \note   This counts on Stroika's semi-cooperative multitasking (to be safe). This means if you call libraries that don't
             *          check for thread interruption, those threads may not BE interruptible during that region of code.
             *          @see Thread::GetThrowInterruptExceptionInsideUserAPC()
             * 
             *  \note It IS possible to have two DIFFERNT Ptr objects being called in different threads, one doing a Start ()
             *        and the other doing an Abort (); because of this, its allowed to call Start() in the 'aborted' state.
             *
             *  \req *this != nullptr
             */
            nonvirtual void Abort () const;

        public:
            /**
             *  \brief  Wait for the pointed-to thread to be done. If the thread completed with an exception (other than thread abort exception)
             *          that exception will be re-thrown in the calling thread.
             *
             *  This API is frequently preferred over WaitForDone () - because you frequently will do work in a thread which when failing - you want
             *  reported somehow. But this is not ALWAYS desirable - like when you have worker threads are just trying to clean up, and don't care that
             *  parts of it may have failed.
             *
             *  JoinUntil (timeout + Time::GetTickCount ());
             *
             *  @see WaitForDone ()
             *  @see ThrowIfDoneWithException ()
             *
             *  \note ***Cancelation Point***
             *
             *  \req *this != nullptr
             */
            nonvirtual void Join (Time::DurationSeconds timeout = Time::kInfinity) const;

        public:
            /**
             *  \brief  Wait for the pointed-to thread to be done. If the thread completed with an exception (other than thread abort exception)
             *          that exception will be re-thrown in the calling thread.
             *
             *  This API is frequently preferred over WaitForDone () - because you frequently will do work in a thread which when failing - you want
             *  reported somehow. But this is not ALWAYS desirable - like when you have worker threads are just trying to clean up, and don't care that
             *  parts of it may have failed.
             *
             *  WaitForDoneUntil () followed up ThrowIfDoneWithException ()
             *
             *  \note   Considered using ThrowIfDoneWithException () always in WaitForDone() 
             *          or make optional if existing WAIT API throws child exceptions. Maybe parameter in construction
             *          of the thread?
             *
             *          Decided against always re-throwing because sometimes you just don't care for a given thread what sort of errors
             *          it had (maybe cuz you've given up on the larger task).
             *
             *          Decided against encoding that choice in the Thread::New () function (making it a property) and then always doing in WaitForDone () - because
             *          then the the choice would be far away from where the waiting is done - and that Waiting - really needs to be setup to KNOW its likely
             *          to get an exception. So that seemed the wrong place.
             *
             *          Considered making this just an overload of WaitForDone () - with an extra param - reThrow - and that's really close to what I ended up 
             *          with. But overloading would force me to pick a default, which would cause a struggle between backward compatabilitiy and what
             *          I think is the more common case. Or maybe there is no clearly more common case. This name seems fine.
             *
             *  \note   Possible aliases WaitAndThrowAnyChildThreadExceptions, WaitForDoneUntilAndPropagateExceptions ()
             *
             *  @see WaitForDoneUntil ()
             *  @see ThrowIfDoneWithException ()
             *
             *  \note ***Cancelation Point***
             *
             *  \req *this != nullptr
             */
            nonvirtual void JoinUntil (Time::TimePointSeconds timeoutAt) const;

        public:
            /**
             *  Wait until thread is done (use Abort to request termination) - throws if timeout
             *  Note that its legal to call WaitForDone on a thread in any state.
             *  Some may just have no effect
             *
             *  @see WaitForDoneUntil (), @see Join
             *
             *  \note ***Cancelation Point***
             *
             *  \req *this != nullptr
             */
            nonvirtual void WaitForDone (Time::DurationSeconds timeout = Time::kInfinity) const;

        public:
            /**
             *  Wait until thread is done (use Abort to request termination) - throws if timeout
             *  Note that its legal to call WaitForDoneUntil on a thread in any state.
             *  Some may just have no effect.
             *
             *  @see WaitForDone (), @see Join
             *
             *  \note   This does a tiny bit more than waiting for the done state to be set - it also
             *          'joins' (frees memory for) underlying thread if still allocated. This should not be visible/noticed
             *          except for in a debugger or #if qStroika_Foundation_Execution_Thread_SupportThreadStatistics
             *
             *  \note ***Cancelation Point***
             *
             *  \req *this != nullptr
             */
            nonvirtual void WaitForDoneUntil (Time::TimePointSeconds timeoutAt) const;

        public:
            /**
             *  Wait until thread is done (use Abort to request termination) or timeoutAt expires
             *  
             *  Returns:    true if thread done, and false if timeout
             *
             *  Note that its legal to call WaitForDoneUntilQuietly on a thread in any state - including nullptr.
             *  Some may just have no effect.
             *
             *  \note   This does a tiny bit more than waiting for the done state to be set - it also
             *          'joins' (frees memory for) underlying thread if still allocated. This should not be visible/noticed
             *          except for in a debugger or #if qStroika_Foundation_Execution_Thread_SupportThreadStatistics
             *
             *  \note ***Cancelation Point***
             *
             *  \req *this != nullptr
             */
            nonvirtual bool WaitForDoneUntilQuietly (Time::TimePointSeconds timeoutAt) const;

        public:
            /**
             *  \brief  Abort () the thread, and then WaitForDone () - but if doesn't finish fast enough, send extra aborts (aka AbortAndWaitForDoneUntil (timeout + GetTickCount))
             *
             *  \note   This frequently (and nearly always in a destructor) - should be preceded by:
             *      \code
             *           Execution::Thread::SuppressInterruptionInContext  suppressInterruption;  // critical to prohibit this thread from interruption until its killed owned threads
             *      \endcode
             *
             *  @see AbortAndWaitForDoneUntil ()
             *
             *  \note ***Cancelation Point***
             *
             *  \req *this != nullptr
             */
            nonvirtual void AbortAndWaitForDone (Time::DurationSeconds timeout = Time::kInfinity) const;

        public:
            /**
             *  \brief  Abort () the thread, and then WaitForDone () - but if doesn't finish fast enough, send extra aborts
             *
             *   \note  Note that its legal to call AbortAndWaitForDone on a thread in any state.
             *          Some may just have no effect
             *
             *  An example of when this is useful is if you have a thread (performing some operation on
             *  behalf of an object - with data pointers to that object)
             *  and must stop the thread (its no longer useful) - but must assure its done before you
             *  destroy the rest of the data...)
             *  As for example in FileUtils - DirectoryWatcher...
             *
             *  throws if timeout
             *
             *   \note   This frequently (and nearly always in a destructor) - should be preceded by:
             *      \code
             *            Execution::Thread::SuppressInterruptionInContext  suppressInterruption;  // critical to prohibit this thread from interruption until its killed owned threads
             *      \endcode
             *
             *   \note ***Cancelation Point***
             *
             *  @see Abort ()
             *  @see WaitForDoneUntil ()
             *  @see AbortAndWaitForDone ()
             *
             *  \req *this != nullptr
             */
            nonvirtual void AbortAndWaitForDoneUntil (Time::TimePointSeconds timeoutAt) const;

        public:
            /**
             *  If the thread is Done() - and completed with an exception (other than interrupt),
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
            nonvirtual void ThrowIfDoneWithException () const;

#if qPlatform_Windows
        public:
            /**
             *  Look pumping messages until either time2Pump is exceeded or the thread completes.
             *  Its NOT an error if the timeout is exceeded.
             *
             *  throws if timeout
             *
             *  @see WaitForDoneUntil ()
             *
             *  \note   ***Cancelation Point***
             *
             *  \req *this != nullptr
             */
            nonvirtual void WaitForDoneWhilePumpingMessages (Time::DurationSeconds timeout = Time::kInfinity) const;
#endif

        public:
            /**
             *  This is a portable wrapper on setting thread priorities. It has fewer knobs than direct or low level
             *  APIs. You can always directly call low-level native APIs using GetNativeHandle().
             * 
             *  \note - no GetPriority API, cuz cannot do something vaguely like that portably. For one thing, priority
             *          is more complex than a single number (like this argument) at runtime for reading on Linux.
             */
            nonvirtual void SetThreadPriority (Priority priority = Priority::eNormal) const;

        public:
            /**
             *  Each Thread object has an associated state. Since this can be accessed from one thread while things
             *  in another thread, by the time the answer is returned, the value may have changed.
             *
             *  But only certain transitions are legal. Status::eNull is the starting state for SOME Thread constructors (no arg).
             *  Nothing transitions back to the Status:eNull state. From there when you specify a std::function<> to run,
             *  then you transition to Status::eNotYetRunning.
             *
             *  From eNotYetRunning, you can transition to eRunning with Start () (or the starting thread CTOR). Or you can transition from
             *  eNotYetRunning to eAborting, if you call Abort() before Start().
             *
             *  From eRunning (or eNotYetRunning) you can transition to eAborting or eCompleted (or from eAborting to eCompleted).
             *
             *  A thread object can never transition back (by this I mean the underlying pointed to rep - the container of
             *  course can transition back by being assigned another ThreadRep).
             *
             *  \req *this != nullptr
             *
             *  \note - Calling GetStatus () is generally not a good idea, except for debugging purposes. Generally, you will
             *          want to call WaitForDone (), and perhaps Abort, or AbortAndWaitForDone (). GetStatus () gives you a clue
             *          what the thread was doing, but by the time you look it could have changed.
             */
            nonvirtual Status GetStatus () const noexcept;

        public:
            /**
             *  Return true iff WaitForDone () would return immediately;
             * 
             *  \req not == nullptr
             *  
             *  This will return true iff GetStatus() would return eCompleted.
             * 
             *  Note - some internal traces of the thread object may still be running at this point, but its safe to call join at this point without significant blockage.
             */
            nonvirtual bool IsDone () const;

        public:
            /**
             *  Thread name does NOT need to be unique and defaults to '', but can be used on advisory
             *  basis for debugging. Also - setting the thread name, and then getting it back, may not
             *  produce the same result. Some OSes may munge the name to make it unique, or conform
             *  to some naming convention.
             *
             *  These names should not be counted on for program logic.
             *
             *  Thread names appear frequently in debug trace messages.
             */
            nonvirtual Characters::String GetThreadName () const;

        public:
            /**
             *  \req GetStatus () != Status::eNull
             *
             *  @see GetThreadName ();
             */
            nonvirtual void SetThreadName (const Characters::String& threadName) const;

        public:
            /**
             *  Return a snapshot of the current stack settings - configured - and dynamic. Much/most of this
             *  may not be available on your system.
             *
             *  \note   @todo - IMPLEMENT ON SOME SYSTEM ;-) Not sure if/where we can do this. Tricky...
             *  \note   NYI - see https://stroika.atlassian.net/browse/STK-475
             */
            nonvirtual ConfigurationStatus GetConfigurationStatus () const;

        public:
            /**
             *  @see Characters::ToString ()
             */
            nonvirtual Characters::String ToString () const;

        public:
            /**
             */
            nonvirtual bool operator== (const Ptr& rhs) const;
            nonvirtual bool operator== (nullptr_t) const;

        public:
            /**
             */
            nonvirtual strong_ordering operator<=> (const Ptr& rhs) const;
            nonvirtual strong_ordering operator<=> (nullptr_t) const;

        public:
            /**
             *  \brief return true iff *this != nullptr
             */
            nonvirtual explicit operator bool () const;

        private:
            shared_ptr<Rep_>                                               fRep_;
            [[no_unique_address]] Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;
            static inline thread_local weak_ptr<Rep_>                      sCurrentThreadRep_;

        private:
            friend Ptr New (const function<void ()>& fun2CallOnce, const optional<Characters::String>& name, const optional<Configuration>& configuration);
            friend Ptr  GetCurrent ();
            friend bool IsCurrentThreadInterruptible ();
            friend void CheckForInterruption ();
        };

        /**
         *  Handy wrapper around a Thread::Ptr, when you want to assure the thread is terminated before the CleanupPtr goes out of scope.
         */
        class CleanupPtr : public Ptr {
        public:
            /**
             */
            enum class AbortFlag {
                eAbortBeforeWaiting,
                eDirectlyWait
            };
            static constexpr AbortFlag eAbortBeforeWaiting = AbortFlag::eAbortBeforeWaiting;
            static constexpr AbortFlag eDirectlyWait       = AbortFlag::eDirectlyWait;

        public:
            /**
             *  \brief  in destructor, wait for the thread to terminate (optionally aborting it first - depending on CleahupPtr arg/flag)
             *
             *  \note it is OK to pass  a nullptr, or to otherwise stop/abort the thread. This class wraps safe checking around the thread ptr to make sure
             *        it gets cleaned up.
             */
            CleanupPtr (const CleanupPtr&) = delete;
            CleanupPtr (AbortFlag abortFlag, Ptr threadPtr = nullptr);

        public:
            ~CleanupPtr ();

        public:
            /**
             *  Assigning a new Thread::Ptr object just replaces the thread that will be 'cleaned up' in the
             *  CleanupPtr destructor. It doesn't force the previous thread to be terminated.
             */
            nonvirtual CleanupPtr& operator= (const Ptr&);
            nonvirtual CleanupPtr& operator= (const CleanupPtr&) = delete;

        private:
            bool fAbort_{false};
        };

        /**
         */
        class AbortException : public Exception<> {
        public:
            AbortException ();

        public:
            /*
             *  Handy constant you can use to avoid construction.
             *
             *  Statically allocate because:
             *      o   Performance
             *      o   Only legal to throw these while main active (so safe to use in that context)
             *      o   Avoids issue with re-throwing while constructing one
             */
            static const AbortException kThe;
        };
        inline const AbortException AbortException::kThe;

        using InterruptException [[deprecated ("Since Stroika v3.0d4 - use AbortException")]] = AbortException;

        /**
         *  Thread IDs tend to be long and not easy to read in trace output. This utility class just maps these long
         *  ids to a short index. Again - ONLY for easier / brevity in logging. But a common registry is maintained here
         *  to allow all the logging across Stroika to use the same shortened IDs.
         * 
         *  Note you can only reference the singleton sThe. This can be referenced before main, or after main safely,
         *  but in those cases, there can be no threads, so it just returns zero (GetIndex).
         * 
         *  NOTE - this class is slightly queer, in that it guarantees behavior of one of its methods BEFORE its
         *  constructed and AFTER its been destroyed. This cannot be 100% true in general and might cause some problems
         *  with sanitizers and the like. But I think this path will generally work and certainly tries to give reasonable
         *  answers when called outside its lifetime (and should work due to only having a single singleton instance).
         *
         *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
         */
        class IndexRegistrar {
        protected:
            IndexRegistrar (const IndexRegistrar&) = delete;
            IndexRegistrar ();

        public:
            ~IndexRegistrar ();

        public:
            nonvirtual IndexRegistrar& operator= (const IndexRegistrar&) = delete;

        public:
            /**
             */
            nonvirtual unsigned int GetIndex (const IDType& threadID, bool* wasNew = nullptr);

        public:
            /**
             */
            static IndexRegistrar sThe;

        private:
            bool                      fInitialized_{false};
            mutex                     fMutex_;          // for read/update the fShownThreadIDs_ map
            map<IDType, unsigned int> fShownThreadIDs_; // use std::map instead of Stroika to avoid module deadly embrace
        };
        inline IndexRegistrar IndexRegistrar::sThe;

        /**
         *  This object - while in existence, blocks delivery of all Thread::AbortException's
         *  (for this thread in which its instantiated). This blocking nest (so if you have two of them in one thread, only when the last
         *  one is destroyed does the block on Interruption be removed).
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
         *  CheckForInterruption to force that). The destructor of this suppress (even when count hits zero)
         *  will not throw.
         */
        class SuppressInterruptionInContext {
        public:
            SuppressInterruptionInContext ();
            SuppressInterruptionInContext (const SuppressInterruptionInContext&)            = delete;
            SuppressInterruptionInContext& operator= (const SuppressInterruptionInContext&) = delete;
            ~SuppressInterruptionInContext ();
        };

        /**
         *  No arg- constructor is available for use in applications like thread pools. Also, a variety
         *  of cases, its handy to declare a Thread data member (and init in CTOR), but not
         *  specify what gets run until later.
         *
         *  After this call, GetStatus () returns Status::eNull
         *
         *  fun2CallOnce is called precisely once by this thread CTOR, but called in
         *  another thread with the arg 'arg'.
         *
         *  \note   about the 'configuration' parameter. If missing (or parts missing) - the whole are parts from from
         *          the static GetDefaultConfiguration () function. If defaults are missing there too, the OS / system defaults
         *          are relied upon.
         *
         *          It's not expected one would need to use this often, but when you need it you need it early - before the thread has
         *          been constructed (generally) - or at least before started (sucks swapping stacks out on a running thread ;-))
         *
         *  \note   Unlike std::thread, a Stroika Thread is not started automatically (unless you pass eAutoStart as a constructor argument),
         *          and it can run in the background after the Thread has gone out of scope (std::thread you can do this but must call detach).
         *
         *  \req    Debug::AppearsDuringMainLifetime() at all points during the threads lifetime. It must be stopped
         *          before Debug::AppearsDuringMainLifetime() becomes untrue. This is somewhat checked by the Stroika
         *          thread infrastructure, but may not be fully reliably asserted (see AllThreadsDeadDetector_)
         *
         *  \par Example Usage
         *      \code
         *          Thread::New ([r]() { r->Run (); }, Thread::eAutoStart);     // runs arg to completion then thread destroyed. New returns once thread created
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Thread::Ptr t = Thread::New ([r]() { r->Run (); }, Thread::eAutoStart, "Thread Name");
         *      \endcode
         *
         *  \par Example Usage
         *      \code
         *          Thread::Ptr t = Thread::New ([r]() { r->Run (); });
         *          t.Start ();
         *          t.WaitForDone ();
         *      \endcode
         */
        Ptr New (const function<void ()>& fun2CallOnce, const optional<Characters::String>& name, const optional<Configuration>& configuration);
        Ptr New (const function<void ()>& fun2CallOnce, const Characters::String& name, const optional<Configuration>& configuration = nullopt);
        Ptr New (const function<void ()>& fun2CallOnce, const optional<Configuration>& configuration = nullopt);
        Ptr New (const function<void ()>& fun2CallOnce, AutoStartFlag, const optional<Characters::String>& name,
                 const optional<Configuration>& configuration);
        Ptr New (const function<void ()>& fun2CallOnce, AutoStartFlag, const Characters::String& name,
                 const optional<Configuration>& configuration = nullopt);
        Ptr New (const function<void ()>& fun2CallOnce, AutoStartFlag, const optional<Configuration>& configuration = nullopt);

        /**
         * \req    foreach Thread t: t.GetStatus () == Status::eNotYetRunning
         */
        void Start (const Traversal::Iterable<Ptr>& threads);

        /**
         *    \brief foreach Thread t: t.Abort ()
         *
         * \req    foreach Thread t: t != nullptr
         *
         *  \see Thread::Ptr::Abort
         */
        void Abort (const Traversal::Iterable<Ptr>& threads);

        /**
         *  \note ***Cancelation Point***
         *
         * \req    foreach Thread t: t != nullptr
         */
        void WaitForDone (const Traversal::Iterable<Ptr>& threads, Time::DurationSeconds timeout = Time::kInfinity);

        /**
         *  \note ***Cancelation Point***
         *
         * \req    foreach Thread t: t != nullptr
         */
        void WaitForDoneUntil (const Traversal::Iterable<Ptr>& threads, Time::TimePointSeconds timeoutAt);

        /**
         *  \brief - shorthand for AbortAndWaitForDoneUntil (Time::GetTickCount () + timeout)
         *
         *  \note ***Cancelation Point***
         *
         *  @see Thread::Ptr::AbortAndWaitForDone
         *
         *  @see Thread::AbortAndWaitForDoneUntil
         *
         *   \note   This frequently (and nearly always in a destructor) - should be preceded by:
         *      \code
         *            Execution::Thread::SuppressInterruptionInContext  suppressInterruption;  // critical to prohibit this thread from interruption until its killed owned threads
         *      \endcode
         *
         * \req    foreach Thread t: t != nullptr
         */
        void AbortAndWaitForDone (const Traversal::Iterable<Ptr>& threads, Time::DurationSeconds timeout = Time::kInfinity);

        /**
         *   \note ***Cancelation Point***
         *
         *  @see Thread::Ptr::AbortAndWaitForDoneUntil
         *
         *   \note   This frequently (and nearly always in a destructor) - should be preceded by:
         *      \code
         *            Execution::Thread::SuppressInterruptionInContext  suppressInterruption;  // critical to prohibit this thread from interruption until its killed owned threads
         *      \endcode
         *
         * \req    foreach Thread t: t != nullptr
         */
        void AbortAndWaitForDoneUntil (const Traversal::Iterable<Ptr>& threads, Time::TimePointSeconds timeoutAt);

        /**
         * Generally should not be reported. It's just to help force a thread to shut itself down
         */
        class AbortException;

        class SuppressInterruptionInContext;

#if qStroika_Foundation_Execution_Thread_SupportThreadStatistics
        /**
         */
        struct Statistics {
            /**
             *  These are the thread objects in the status 'running'. It doesn't count ones that exist,
             *  or Thread objects (which could be null or completed)
             */
            Traversal::Iterable<IDType> fRunningThreads;
        };

        /**
         *  This does not return statistics about this thread (its a static method) - but about all thread allocations (through
         *  the Stroika thread API).
         */
        Statistics GetStatistics ();
#endif

#if qPlatform_POSIX
        /**
         *  Unsafe to change this while threads running - at least if you could be interupting threads during this time.
         *  If argument given, this resets the signalNumber.
         *  Either way, this returns the previous value.
         */
        SignalID SignalUsedForThreadInterrupt () noexcept;
        SignalID SignalUsedForThreadInterrupt (optional<SignalID> signalNumber);
#endif

        struct FormatThreadInfo {
            bool fIncludeLeadingZeros{false};
        };

        /**
         *  Represent the thread ID for display - typically as an integer.
         *
         *  \note this function is NOT a cancelation point
         *
         *  \note   this returns an ASCII string (not using String class library) so easier to use from code expecting no cancelation
         *
         *  @see Characters::ToString (Thread::IDType threadID)
         */
        wstring FormatThreadID (Thread::IDType threadID, const FormatThreadInfo& formatInfo = {});
        string  FormatThreadID_A (Thread::IDType threadID, const FormatThreadInfo& formatInfo = {});

        /**
         */
        IDType GetCurrentThreadID () noexcept;

        /**
         *  If the current (calling) thread is a 'Stroika thread' - that thread Ptr is returned. If the current thread is NOT a stroika thread (e.g. the main thread)
         *  this will return a nullptr.
         * 
         *  \note SEE https://stroika.atlassian.net/browse/STK-994 - for the idea of creating a special Ptr - just for the main thread. But really
         *        no reason I can see --LGP 2023-10-18
         */
        Ptr GetCurrent ();

#if __cpp_lib_jthread >= 201911
        /**
         */
        optional<stop_token> GetCurrentThreadStopToken ();
#endif

        /**
         *  Returns true iff it is potentially useful to call CheckForInterruption.
         */
        bool IsCurrentThreadInterruptible ();

        /**
         *  Our thread interruption (and abort) mechanism only throws at certain 'signalable' (alertable/cancelable)
         *  spots in the code - like sleeps, most reads, etc.
         *  This function will also trigger a throw if called inside a thread which is being aborted.
         *
         *  Any call to this routine is a 'cancelation point'.
         *
         *  \note   ***Cancelation Point***
         * 
         *  \note   This name is somewhat historical, but still reasonable. But now might be better called CheckForThreadAbort, since thats all it does now.
         */
        void CheckForInterruption ();

        /*
         *  Avoid interference with Windows SDK headers. I hate needless C macros (with short, common names)
         */
#ifdef Yield
#undef Yield
#endif

        /**
         *  \brief calls CheckForInterruption, and std::this_thread::yield ()
         *
         *  \note   ***Cancelation Point***
         *          To avoid cancelation point, directly call std::this_thread::yield ()
         */
        dont_inline void Yield ();

    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Thread.inl"

#endif /*_Stroika_Foundation_Execution_Thread_h_*/
