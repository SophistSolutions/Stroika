/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SignalHandlers_h_
#define _Stroika_Foundation_Execution_SignalHandlers_h_    1

#include    "../StroikaPreComp.h"

#include    <csignal>
#include    <mutex>

#include    "../Configuration/Common.h"
#include    "../Containers/Mapping.h"
#include    "../Containers/Set.h"
#include    "Function.h"
#include    "Signals.h"



/**
 * Description:
 *
 *      This module defines support for POSIX (and std c++ defined) Signals (not to be confused
 *  with the 'Signals and slots' design pattern which is largely unrelated).
 *
 *
 * TODO:
 *
 *      @todo   REDO operator== etc using non-member functions
 *              (see See coding conventions document about operator usage: Compare () and operator<, operator>, etc comments)
 *
 *      @todo   SignalHandlerRegistry::FirstPassSignalHandler_ is NOT signal safe. Its close (esp given
 *              the block allocation). But its not fully safe. SO MUST FIX!!! But can be done without
 *              changing API (just store vector of actual direct handlers).
 *
 *      @todo   Small issue - AddSignalHandler versus SetSignalHandler (). This can be confusing. I had a bug
 *              which was we setup DEFAULT signal handlers, and then in the BasicUNIX Serviced code - did
 *              AddSignalHandler (SIGINT). Issue is that we process BOTH hanlders - one to set an event
 *              object to cleanly shutodown and the other to HARD ABORT!
 *
 *              This API encourages that mistake. I changed the Service code to use SetSignalHandler - but
 *              That has the default of being hard to debug/non-modular. Maybe have a "SetDefaultHanlder"
 *              or "SetFallbackHandler" - and that is invoked ONLY if no others? Or maybe a property of all
 *              handlers?
 *
 *      @todo   Consider use of threadpool for SafeSignalsManager.
 *
 *      @todo   Do overload (or some such) for (sa_sigaction)(int, siginfo_t *, void *); Allow
 *              these to be (more or less) interchangable with regular SignalHandler.
 *
 *      @todo   Consider adding "ThreadSignalHandler" faciltiy - where we register a set of handlers
 *              that ONLY apply when the signal is sent to the given (argument with register) thread.
 *              If we do this - we will want to write cooperating code with the thread start/end
 *              stuff so these get cleared out appropriately.
 *              Consider how this might be useful for stuff like SIGPIPE handling?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  The key feature of SignalHandler versus function<void(SignalID)> is that you can compare them.
             *
             *  Note that to do so, you must save the original SignalHandler you create to later remove it by value:
             *  creating another SignalHandler (even with the same arguments) may not compare as equal.
             *
             *  \note   BEWARE - these are copied during exception handling, which is a dangerous, finicky place.
             *          Copy must not do operations (like allocate memory) which would be unsafe during signal handling.
             *
             *          I THINK Function (based on shared_ptr) is safe with respect to this, but this is probably worth
             *          double checking.
             */
            class   SignalHandler {
            public:
                /**
                 *  A direct (eDirect) signal handler is invoked in the stack context in which the
                 *  signal is delivered.
                 *
                 *  A 'safe' (eSafe) signal handler is run in a separate thread context.
                 *
                 *  @see SignalHandlerRegistry::SafeSignalsManager.
                 */
                enum    class   Type {
                    eDirect,
                    eSafe,

                    eDEFAULT = eSafe,

                    Stroika_Define_Enum_Bounds (eDirect, eSafe)
                };

            public:
                SignalHandler (void (*signalHandler)(SignalID), Type type = Type::eDEFAULT);
                SignalHandler (const Function<void(SignalID)>& signalHandler, Type type = Type::eDEFAULT);

            public:
                nonvirtual  Type    GetType () const;

            public:
                /**
                 *  Invoke the actual signal handler.
                 */
                nonvirtual  void operator () (SignalID i) const;

            public:
                nonvirtual  bool operator== (const SignalHandler& rhs) const;
                nonvirtual  bool operator!= (const SignalHandler& rhs) const;
                nonvirtual  bool operator< (const SignalHandler& rhs) const;

            private:
                Type                        fType_;
                Function<void(SignalID)>    fCall_;
            };


            /**
             *  SignalHandlerRegistry is a singleton object. If used - it itself registers signal handlers
             *  for each supported signal.
             *
             *  The user can then add in their own 'handlers' for those signals, and they are ALL called -
             *  one after the other (TDB how threads work with this).
             *
             *  When an platform signal-handler is installed (via 'sigaction' for example) -
             *  and then later UNINSTALLED (due to changes in GetHandledSignals) - this code resets the
             *  signal handler to SIG_DFL (not the previous value).
             *
             *  To use 'safe' signal handlers, be sure to read about and use
             *      @see SignalHandlerRegistry::SafeSignalsManager
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            class   SignalHandlerRegistry {
            public:
                /**
                 *  If this handler is set to the the ONLY handler for a given signal, then that signal handler is
                 *  effectively ignored.
                 *
                 *  To get the signal to be handled the DEFAULT way - remove all signal handlers.
                 */
                static  const   SignalHandler   kIGNORED;

            public:
                /**
                 * Access singleton implementation. None exists until this is called.
                 */
                static  SignalHandlerRegistry&  Get ();

            private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
            public:
#endif
                SignalHandlerRegistry ();

            public:
                SignalHandlerRegistry (const SignalHandlerRegistry&) = delete;
                ~SignalHandlerRegistry ();

            public:
                nonvirtual  SignalHandlerRegistry&    operator= (const SignalHandlerRegistry&) = delete;

            public:
                class   SafeSignalsManager;

            public:
                /**
                 * Returns the set of signals trapped by the SignalHandlerRegistry registry. Note - if not 'Installed ()' - these
                 * are tracked internally by Stroika but not actually installed in the OS.
                 */
                nonvirtual  Containers::Set<SignalID>   GetHandledSignals () const;

            public:
                /**
                 * Returns the set of signals trapped by the SignalHandlerRegistry registry. This doesn't imply there is a handler.
                 * NB: A signal handler must be registered for a given signal number AND the signal number must be in GetHandledSignals () AND
                 * the SignalHandlerRegistry must be Installed () - to get the signal called.
                 *
                 * It is NOT an error to have a signal handler registered for a signal not in the set of GetHandledSignals () - or vice versa.
                 * Signals in the list of GetHandledSignals() with no handlers are effectively ignored.
                 */
                nonvirtual  Containers::Set<SignalHandler>  GetSignalHandlers (SignalID signal) const;

            public:
                /**
                 * @see GetSignalHandlers().
                 *
                 * SetSignalHandlers () with NO arguments uninstalls all Stroika signal handlers for this signal.
                 * SetSignalHandlers () with ONE argument makes Stroika take-over the signal handling - and sets the set of hanlders to be
                 * exactly the one given (effectively removing any others previously added).
                 * SetSignalHandlers () with ONE a set of handlers registers all the given handlers.
                 *
                 * Note - if through ANY combination of set/add/remvoe - you have NO signal handler - this reverts to SIG_DFL, and if you have
                 * exactly ONE signal handler - and its kIGNORED- the signal will be ignored.
                 */
                nonvirtual  void    SetSignalHandlers (SignalID signal);
                nonvirtual  void    SetSignalHandlers (SignalID signal, SignalHandler handler);
                nonvirtual  void    SetSignalHandlers (SignalID signal, const Containers::Set<SignalHandler>& handlers);

            public:
                /**
                 * @see GetSignalHandlers()
                 */
                nonvirtual  void    AddSignalHandler (SignalID signal, SignalHandler handler);

            public:
                /**
                 * @see GetSignalHandlers()
                 */
                nonvirtual  void    RemoveSignalHandler (SignalID signal, SignalHandler handler);

            public:
                /**
                 * This signal handler simply prints error to the trace log, and calls 'abort' - which on most operating systems will allow the
                 * debugger to examine the errant code.
                 */
                static  void    DefaultCrashSignalHandler (SignalID signal);

            public:
                /**
                 *  These signals are generally associated with a programming error or bug, and these signals
                 *  should generally be treated as a crash and terminate the program with a core-dump file.
                 *      o   SIGABRT
                 *      o   SIGILL
                 *      o   SIGFPE
                 *      o   SIGSEGV
                 *      o   SIGSYS      (POSIX ONLY)
                 *      o   SIGBUS      (POSIX ONLY)
                 *      o   SIGQUIT     (POSIX ONLY)
                 */
                static  Containers::Set<SignalID>    GetStandardCrashSignals ();

            public:
                /**
                 *      o   SIGABRT
                 *      o   SIGILL
                 *      o   SIGINT
                 *      o   SIGFPE
                 *      o   SIGSEGV
                 *      o   SIGTERM
                 *      o   SIGBUS      (POSIX ONLY)
                 *      o   SIGHUP      (POSIX ONLY)
                 *      o   SIGPIPE     (POSIX ONLY)
                 *      o   SIGQUIT     (POSIX ONLY)
                 *      o   SIGSYS      (POSIX ONLY)
                 *      o   SIGXCPU     (POSIX ONLY)
                 *      o   SIGXFSZ     (POSIX ONLY)
                 *
                 *  These  signals - by default (without any coding) - will generally (depends on OS, and environment) terminate the
                 *  process.
                 */
                static  Containers::Set<SignalID>    GetStandardTerminationSignals ();

            public:
                /**
                 *  The set of signals given (by default GetStandardCrashSignals) will be set to the given handler
                 *  (by default DefaultCrashSignalHandler).
                 *
                 *  The only exception is SIGABRT will be intentionally ignored from this call because it prevents abort()
                 *  from functioning properly. We COULD disable SIGABRT upon receipt of that signal (SIG_DFL) but that
                 *  would be different than other signals handled, raise re-entrancy issues etc. Didn't seem owrh while.
                 */
                nonvirtual  void    SetStandardCrashHandlerSignals (SignalHandler handler = DefaultCrashSignalHandler, const Containers::Set<SignalID>& forSignals = GetStandardCrashSignals ());

            private:
                Containers::Mapping<SignalID, Containers::Set<SignalHandler>>   fDirectHandlers_;

            private:
                static      void    FirstPassSignalHandler_ (SignalID signal);
            };


            /**
             *  A direct (eDirect) signal handler is invoked in the stack context in which the
             *  signal is delivered.
             *
             *  A 'safe' (eSafe) signal handler is run in a separate thread context.
             *
             *  Direct is more performant, has lower latency. However, since many signals are
             *  delivered on an arbitrary thread, this can easily lead to deadlocks!
             *              See http://stackoverflow.com/questions/3366307/why-is-malloc-not-async-signal-safe
             *
             *  'safe' signal handlers avoid this by automatically moving the signal delivery to a specaial
             *  thread that handles the signals one at a time, and careful to avoid any memory allocation
             *  in the direct signal handling thread.
             *
             *  To use this feature, you must construct a SignalHandlerRegistry::SafeSignalsManager. This
             *  must be constructed BEFORE adding any safe signal handler, and must be destroyed before
             *  exiting main (so it can shutdown its own threads).
             *
             *  The easiest (and recommened) way to do this is to add the line:
             *      SignalHandlerRegistry::SafeSignalsManager   safeSignalsMgr;
             *  to the beginning of main (int argc, const char* argv[])...
             *
             */
            class   SignalHandlerRegistry::SafeSignalsManager {
            public:
                SafeSignalsManager ();
                SafeSignalsManager (const SafeSignalsManager&) = delete;

            public:
                ~SafeSignalsManager ();

            public:
                nonvirtual  SafeSignalsManager& operator= (const SafeSignalsManager&) = delete;

            private:
                struct Rep_;
                static  shared_ptr<Rep_>    sTheRep_;

            private:
                friend  class   SignalHandlerRegistry;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SignalHandlers.inl"

#endif  /*_Stroika_Foundation_Execution_SignalHandlers_h_*/
