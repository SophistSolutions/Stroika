/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SignalHandlers_h_
#define _Stroika_Foundation_Execution_SignalHandlers_h_    1

#include    "../StroikaPreComp.h"

#include    <mutex>
#include    <csignal>

#include    "../Configuration/Common.h"
#include    "../Containers/Mapping.h"
#include    "../Containers/Set.h"
#include    "BlockingQueue.h"
#include    "Signals.h"
#include    "Thread.h"



/**
 * Description:
 *
 *      This module defines support for POSIX (and std c++ defined) Signals (not to be confused
 *  with the 'Signals and slots' design pattern which is largely unrelated).
 *
 *
 * TODO:
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
 *      @todo   ADD SAFE SIGNAL HANDLER (DEADLOCK FREE) SUPPORT
 *
 *              Much signal handling - like in Frameworks::Service::Main - unsafe anyhow cuz of malloc in signal handler)
 *
 *              Must provide SOME way to safely delegate the signal call
 *              to another handler - perhaps by having a queue (with all ram preallocated), and a special handler thread
 *              and some configuraiton mechanism so signals get sent to the hander via the extra level of indirection.
 *
 *              <<<< HAVE ANSWER>>>>
 *              I THINK
 *
 *              Create thread to RUN signal handlers on. Then put my ACUTUAL signal handlers registered to do nothing but
 *              event::Signal() - and have the thread that runs signal hanlders wait on that notification. OR - use
 *              BLOCKING QUUE (probably better). MEans only one signal handler runs at a time (probably OK - could extned
 *              with threadpool). But key is that actual signal handlers only on safe mode -  and do no memory allocaiton or
 *              risk blocking. Must do in a way that others can do what they want - but make super easiy for (slightly lower
 *              perofrmance hihgher latency approach) to work SAFELY.
 *
 *              Maybe when registering signalhandler have enum {SAFE, or LOW-LATENCY}
 *
 *      @todo   Becaues Stroika Mapping class is so safe, I may not need the critical section
 *              protecting sHandlers_ any longer.
 *
 *      @todo   THINK OUT AND DESCRIBE IN DETAIL HOW WE HANDLE THREADS
 *              (review / test above plan about using signal hanlder sep  thread and blocking queue - maybe good anser)
 *
 *      @todo   Do overload (or some such) for (sa_sigaction)(int, siginfo_t *, void *); Allow these to be (more or less) interchangable with
 *              regular SignalHandler.
 *
 *      @todo   Consider adding "ThreadSignalHandler" faciltiy - where we register a set of handlers that ONLY apply when the signal
 *              is sent to the given (argument with register) thread. If we do this - we will want to write cooperating code with the thread start/end
 *              stuff so these get cleared out appropriately.
 *              Consider how this might be useful for stuff like SIGPIPE handling?
 *
 *      @todo   DOCUMENT that its UNSAFE to call malloc during signal handlers.
 *              See http://stackoverflow.com/questions/3366307/why-is-malloc-not-async-signal-safe
 *
 *              ((yes - BUT see above note about SAFE/LOW-LATENCY))
 *
 *      @todo   Consider adding some automatic mechansim in signal handler callback (wrappers) - to WRAP cals to malloc/operator new,
 *              to assert out (just int eh context of a sginal handler - just to aid in debugging this issue)
 *
 *              ((yes - BUT see above note about SAFE/LOW-LATENCY))
 *
 *              >   CERTAINLY make sure none of our sginal handler stuff violates that principle.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             */
            typedef int     SignalID;


            /**
             *  The key feature of SignalHandler versus function<void(SignalID)> is that you can compare them.
             *  Note - todo so - you must save the original SignalHandler you create to later remove it by value.
            */
            class   SignalHandler {
            public:

                /**
                 *  @todo EXPLAIN
                  *     SAFE runs on separate thread
                  * eDirect runs directly but greatly risks deadlocks!
                 */
                enum    class   Type {
                    eDirect,
                    eSafe,

                    eDEFAULT = eSafe,

                    Stroika_Define_Enum_Bounds (eDirect, eSafe)
                };

            public:
                SignalHandler (void (*signalHandler)(SignalID), Type type = Type::eDEFAULT);
                SignalHandler (const function<void(SignalID)>& signalHandler, Type type = Type::eDEFAULT);

            public:
                nonvirtual  Type    GetType () const;

            public:
                nonvirtual  void operator () (SignalID i) const;

            public:
                nonvirtual  bool operator== (const SignalHandler& rhs) const;
                nonvirtual  bool operator!= (const SignalHandler& rhs) const;
                nonvirtual  bool operator< (const SignalHandler& rhs) const;

            private:
                Type                                    fType_;
                shared_ptr<function<void(SignalID)>>    fCall_;
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
             *  @todo EXPLAIN ABOUT SignalHandler::Type::eSafe - and how they are run on separate
             *  thread which means no deadlock, but also means SERIALIZED!!!! COULD have mutliple threads (config option?)
             *
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
                SignalHandlerRegistry ();
                ~SignalHandlerRegistry ();
                SignalHandlerRegistry (const SignalHandlerRegistry&) = delete;

            public:
                nonvirtual  const SignalHandlerRegistry& operator= (const SignalHandlerRegistry&) = delete;


            public:
                /*
                 *  Needs lots of docs
                 */
                class   SafeSignalsManager {
                public:
                    SafeSignalsManager ();
                    ~SafeSignalsManager ();

                private:
                    //tmphack
                    static  SafeSignalsManager* sThe;

                private:
                    BlockingQueue<SignalID>     fIncomingSafeSignals_;
                    Thread                      fBlockingQueuePusherThread_;

                private:
                    friend  class   SignalHandlerRegistry;
                };

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
                **** REMOVES ALL HANDLERRS. CAN BE CALLED BEFORE existing main to assure threads shutdown first, but
                *   sbhoudktn be needed. Removes all handlers.
                 */
                nonvirtual  void    Shutdown ();

            public:
                /**
                 * This signal handler simply prints error to the trace log, and calls 'abort' - which on most operating systems will allow the
                 * debugger to examine the errant code.
                 */
                static  void    DefaultCrashSignalHandler (SignalID signal);

            public:
                /**
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
                nonvirtual  void    SetStandardCrashHandlerSignals (SignalHandler handler = DefaultCrashSignalHandler, const Containers::Set<SignalID>& excludedSignals = Containers::Set<SignalID> ());


            private:
                nonvirtual  void    UpdateDirectSignalHandlers_ (SignalID forSignal);

            private:
                // Note - we use vector<> isntead of Stroika class to assure no memory allocation in iteration
                static  mutex                                       sDirectSignalHandlers_CritSection_;
                static  vector<pair<SignalID, SignalHandler>>       sDirectSignalHandlers_;

            private:
                static    void    FirstPassSignalHandler_ (SignalID signal);
            private:
                static    void    SecondPassDelegationSignalHandler_ (SignalID signal);

            private:
                Containers::Mapping<SignalID, Containers::Set<SignalHandler>>    fHandlers_;

            private:
                static  SignalHandlerRegistry   sThe_;
            };



#if     qPlatform_POSIX
            /**
             * For the lifetime of this object - save the initial signal block state for the given signal, and then block the given signal.
             */
            class   ScopedBlockCurrentThreadSignal {
            public:
                ScopedBlockCurrentThreadSignal (SignalID signal);
                ScopedBlockCurrentThreadSignal () = delete;
                ScopedBlockCurrentThreadSignal (const ScopedBlockCurrentThreadSignal&) = delete;
                ~ScopedBlockCurrentThreadSignal ();

            public:
                nonvirtual  const ScopedBlockCurrentThreadSignal& operator= (const ScopedBlockCurrentThreadSignal&) = delete;

            private:
                SignalID    fSignal_;
                sigset_t    fRestoreMask_;
            };
#endif


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
