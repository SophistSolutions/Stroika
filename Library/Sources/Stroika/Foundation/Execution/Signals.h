/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Signals_h_
#define _Stroika_Foundation_Execution_Signals_h_    1

#include    "../StroikaPreComp.h"

#include    <csignal>
#include    <set>

#include    "../Configuration/Common.h"
#include    "Thread.h"

/*
 * Description:
 *
 *      This module defines support for POSIX (and std c++ defined) Signals (not to be confused with the 'Signals and slots'
 *  design pattern which is largely unrelated).
 *
 *
 */


/*
 * TODO:
 *      @todo   Becaues Stroika Mapping class is so safe, I may not need the critical section
 *              protecting sHandlers_ any longer.
 *
 *      @todo   THINK OUT AND DESCRIBE IN DETAIL HOW WE HANDLE THREADS
 *
 *      @todo   Do overload (or some such) for (sa_sigaction)(int, siginfo_t *, void *); Allow these to be (more or less) interchangable with
 *              regular SignalHandlerType.
 *
 *      @todo   Consider adding "ThreadSignalHandler" faciltiy - where we register a set of handlers that ONLY apply when the signal
 *              is sent to the given (argument with register) thread. If we do this - we will want to write cooperating code with the thread start/end
 *              stuff so these get cleared out appropriately.
 *              Consider how this might be useful for stuff like SIGPIPE handling?
 *
 *      @todo   DOCUMENT that its UNSAFE to call malloc during signal handlers.
 *              See http://stackoverflow.com/questions/3366307/why-is-malloc-not-async-signal-safe
 *
 *      @todo   Consider adding some automatic mechansim in signal handler callback (wrappers) - to WRAP cals to malloc/operator new,
 *              to assert out (just int eh context of a sginal handler - just to aid in debugging this issue)
 *
 *      @todo   CERTAINLY make sure none of our sginal handler stuff violates that principle.
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            typedef int     SignalIDType;
            typedef void    (*SignalHandlerType) (SignalIDType);

            /*
             *  Description:
             *      SignalHandlerRegistry is a singleton object. If used - it itself registers signal handlers for each supported signal.
             *
             *      The user can then add in their own 'handlers' for those signals, and they are ALL called - one after the other (TDB how threads
             *      work with this).
             *
             *      When an platform signal-handler is installed (via 'sigaction' for example) - and then later UNINSTALLED (due to
             *      changes in GetHandledSignals) - this code resets the signal handler to SIG_DFL (not the previous value).
             *
             *      This object is thread-safe.
             */
            class   SignalHandlerRegistry {
            public:
                /*
                 * If this handler is set to the the ONLY handler for a given signal, then that signal handler is effectively ignored.
                 *
                 * To get the signal to be handled the DEFAULT way - remove all signal handlers.
                 */
                static  const   SignalHandlerType   kIGNORED;

            public:
                // Access singleton implementation. None exists until this is called.
                static  SignalHandlerRegistry&  Get ();

            private:
                SignalHandlerRegistry ();
                NO_COPY_CONSTRUCTOR (SignalHandlerRegistry);
                NO_ASSIGNMENT_OPERATOR (SignalHandlerRegistry);

            public:
                /*
                 * Returns the set of signals trapped by the SignalHandlerRegistry registry. Note - if not 'Installed ()' - these
                 * are tracked internally by Stroika but not actually installed in the OS.
                 */
                nonvirtual  set<SignalIDType>   GetHandledSignals () const;

            public:
                /*
                 * Returns the set of signals trapped by the SignalHandlerRegistry registry. This doesn't imply there is a handler.
                 * NB: A signal handler must be registered for a given signal number AND the signal number must be in GetHandledSignals () AND
                 * the SignalHandlerRegistry must be Installed () - to get the signal called.
                 *
                 * It is NOT an error to have a signal handler registered for a signal not in the set of GetHandledSignals () - or vice versa.
                 * Signals in the list of GetHandledSignals() with no handlers are effectively ignored.
                 */
                nonvirtual  set<SignalHandlerType>  GetSignalHandlers (SignalIDType signal) const;
                /*
                 * See GetSignalHandlers().
                 *
                 * SetSignalHandlers () with NO arguments uninstalls all Stroika signal handlers for this signal.
                 * SetSignalHandlers () with ONE argument makes Stroika take-over the signal handling - and sets the set of hanlders to be
                 * exactly the one given (effectively removing any others previously added).
                 * SetSignalHandlers () with ONE a set of handlers registers all the given handlers.
                 *
                 * Note - if through ANY combination of set/add/remvoe - you have NO signal handler - this reverts to SIG_DFL, and if you have
                 * exactly ONE signal handler - and its kIGNORED- the signal will be ignored.
                 */
                nonvirtual  void                    SetSignalHandlers (SignalIDType signal);
                nonvirtual  void                    SetSignalHandlers (SignalIDType signal, SignalHandlerType handler);
                nonvirtual  void                    SetSignalHandlers (SignalIDType signal, const set<SignalHandlerType>& handlers);
                /*
                 * See GetSignalHandlers()
                 */
                nonvirtual  void                    AddSignalHandler (SignalIDType signal, SignalHandlerType handler);
                /*
                 * See GetSignalHandlers()
                 */
                nonvirtual  void                    RemoveSignalHandler (SignalIDType signal, SignalHandlerType handler);

            public:
                /*
                 * This signal handler simply prints error to the trace log, and calls 'abort' - which on most operating systems will allow the
                 * debugger to examine the errant code.
                 */
                static  void    DefaultCrashSignalHandler (SignalIDType signal);

                /*
                 * Install the given signal handler for
                 *      o   SIGINT
                 *      o   SIGILL
                 *      o   SIGFPE
                 *      o   SIGSEGV
                 *      o   SIGTERM
                 *      o   SIGSYS      (POSIX ONLY)
                 *      o   SIGBUS      (POSIX ONLY)
                 *      o   SIGQUIT     (POSIX ONLY)
                 *      o   SIGPIPE     (POSIX ONLY)
                 *      o   SIGHUP      (POSIX ONLY)
                 *      o   SIGXCPU     (POSIX ONLY)
                 *      o   SIGXFSZ     (POSIX ONLY)
                 * signals, so that errors get neatly logged. A common use is to provide a handler that uses the LogMgr to record the crash.
                 *
                 * NB:  SIGABRT intentionally omitted from this list because it prevents abort() from functioning properly. We COULD
                 *      disable SIGABRT upon receipt of that signal (SIG_DFL) but that would be different than other signals handled, raise
                 *      re-entrancy issues etc. Didn't seem owrh while.
                 */
                nonvirtual  void                    SetStandardCrashHandlerSignals (SignalHandlerType handler = DefaultCrashSignalHandler, const set<SignalIDType>& excludedSignals = set<SignalIDType> ());
            };

            wstring SignalToName (SignalIDType signal);

            /*
             * Send the given signal to a specific thread (within this process)
             */
            void    SendSignal (Thread::NativeHandleType h, SignalIDType signal);


#if     qPlatform_POSIX
            /*
             * For the lifetime of this object - save the initial signal block state for the given signal, and then block the given signal.
             */
            class   ScopedBlockCurrentThreadSignal {
            public:
                ScopedBlockCurrentThreadSignal (SignalIDType signal);
                ~ScopedBlockCurrentThreadSignal ();
            private:
                NO_DEFAULT_CONSTRUCTOR (ScopedBlockCurrentThreadSignal);
                NO_COPY_CONSTRUCTOR (ScopedBlockCurrentThreadSignal);
                NO_ASSIGNMENT_OPERATOR (ScopedBlockCurrentThreadSignal);

            private:
                SignalIDType    fSignal_;
                sigset_t        fRestoreMask_;
            };
#endif
        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Signals_h_*/


