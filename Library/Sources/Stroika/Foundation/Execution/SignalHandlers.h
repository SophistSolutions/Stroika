/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SignalHandlers_h_
#define _Stroika_Foundation_Execution_SignalHandlers_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>
#include <csignal>
#include <functional>
#include <mutex>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/UniqueID.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Execution/Signals.h"
#include "Stroika/Foundation/Execution/Synchronized.h"

/**
 * Description:
 *
 *      This module defines support for POSIX (and std c++ defined) Signals (not to be confused
 *  with the 'Signals and slots' design pattern which is largely unrelated). (POSIX) Signals have all sorts of tricky
 *  rules for their manipulation/control. This doesn't eliminate those, but it makes most common cases much easier
 *  to handle safely. In particular, many cases of signals can be 'handled' not as signals (safe signal handlers).
 *
 * TODO:
 *      @todo   https://github.com/SophistSolutions/Stroika/issues/1287 - a direct handler given the signal's siginfo_t (SA_SIGINFO)
 */

/**
 *  \brief qStroika_Foundation_Execution_SignalHandlers_MaxDirectPerSignal - how many direct (eDirect) handlers one signal may have
 *         at once; Stroika adds at most one to any signal
 *
 *  The signal handler reads them from a fixed table - it may not use a container (see SignalHandlerRegistry) - so this sizes it.
 *
 *  \note In a future version of Stroika, the table could grow instead - a new one published through an atomic pointer,
 *        the old freed once the handlers using it finish - but nothing has needed more than one direct handler for a signal,
 *        so it is fixed. To raise it, add to your configure line:
 *          --c-define '\#define qStroika_Foundation_Execution_SignalHandlers_MaxDirectPerSignal 4'
 */
#ifndef qStroika_Foundation_Execution_SignalHandlers_MaxDirectPerSignal
#define qStroika_Foundation_Execution_SignalHandlers_MaxDirectPerSignal 2
#endif

namespace Stroika::Foundation::Execution {

    /**
     *  \brief A function to call on a signal, with its Type - direct or safe, which decides what it may do (see Type)
     *
     *  Only its copies compare equal to it (see operator<=>) - so to remove one from SignalHandlerRegistry, keep the one added.
     */
    class SignalHandler {
    public:
        /**
         *  \brief Where a handler runs - and so what it may do
         *
         *  eDirect: in the signal's own context - called from the signal handler Stroika installs, on whichever thread the
         *  signal interrupted, at whatever point it interrupted it. So it is a signal handler as C++ means it, and may do only
         *  what the C++ standard calls signal-safe ([support.signal]: https://eel.is/c++draft/support.signal):
         *      o   plain lock-free atomic operations (atomic<T> where is_always_lock_free), and the few standard library
         *          functions the standard names signal-safe (abort, _Exit, quick_exit, memcpy, memmove, ...);
         *      o   on POSIX also its async-signal-safe functions (write, sem_post, ... - the list:
         *          https://man7.org/linux/man-pages/man7/signal-safety.7.html) - restoring errno if one may set it.
         *  Anything else is undefined behavior there: allocating, locking, any container or iterator, std::function, a
         *  shared_ptr copy, a thread_local, throwing, tracing (DbgTrace). In practice that is a deadlock, when the code the
         *  signal interrupted held the same lock (#128). So a direct handler is a noexcept function pointer that does next to
         *  nothing - sets a lock-free flag - or ends the process (a crash handler: abort ()).
         *
         *  eSafe: on SafeSignalsManager's thread, soon after: the signal handler only counts the signal's arrival (lock-free),
         *  and that thread then calls the handler as ordinary code - once per arrival counted - so it may do anything. An
         *  exception it throws is ignored (but a thread abort). The price: it runs later, and not on the thread the signal
         *  interrupted. Needs a SafeSignalsManager (see it).
         *
         *  \note POSIX itself may merge arrivals of one signal that come while it is pending, before any handler sees them.
         */
        enum class Type {
            eDirect,
            eSafe,

            eDEFAULT = eSafe,
        };
        friend constexpr Common::EnumBounds<Type> EnumBoundsOf (Type)
        {
            return {Type::eDirect, Type::eSafe};
        }

    public:
        /**
         */
        static constexpr Type eDirect = Type::eDirect;

    public:
        /**
         */
        static constexpr Type eSafe = Type::eSafe;

    public:
        /**
         *  A direct (eDirect) handler must be a noexcept function pointer - calling a std::function is not signal-safe (see
         *  Type). A safe (eSafe) handler may be any of these.
         *
         *  \note   The function overload makes a safe handler; before Stroika v3.0d26 it took a Type too (now deprecated).
         */
        SignalHandler ()                     = delete;
        SignalHandler (const SignalHandler&) = default;
        SignalHandler (void (*signalHandler) (SignalID) noexcept, Type type = Type::eDEFAULT); ///< \brief A direct or a safe handler
        SignalHandler (void (*signalHandler) (SignalID), Type type = Type::eDEFAULT); ///< \pre type == eSafe, or signalHandler is SIG_IGN
        SignalHandler (const function<void (SignalID)>& signalHandler);               ///< \brief A safe (eSafe) handler
        [[deprecated ("Since Stroika v3.0d26 - a function makes only a safe handler: drop the Type; a direct one is a noexcept "
                      "function pointer")]] SignalHandler (const function<void (SignalID)>& signalHandler, Type type);

    public:
        nonvirtual Type GetType () const;

    public:
        /**
         *  Invoke the actual signal handler.
         */
        nonvirtual void operator() (SignalID i) const;

    public:
        /**
         */
        nonvirtual Characters::String ToString () const;

    public:
        /**
         *  \brief Each SignalHandler constructed is its own, kept by its copies: another made the same way - even from the same
         *         function - is not equal to it. So to remove one, keep the one added.
         */
        nonvirtual strong_ordering operator<=> (const SignalHandler& rhs) const;

    public:
        /**
         *  \brief True only for the same SignalHandler, or a copy of it - @see operator<=>
         */
        nonvirtual bool operator== (const SignalHandler& rhs) const;

    private:
        Type                      fType_;
        function<void (SignalID)> fCall_;
        void (*fDirectCall_) (SignalID){nullptr}; // the same function, as the signal handler can call it - set if made from one
        Common::UniqueID<SignalHandler> fIdentity_{Common::UniqueID<SignalHandler>::New ()}; // a copy keeps it

    private:
        friend class SignalHandlerRegistry;
    };

    /**
     *  \brief The singleton (sThe) through which signal handlers are added and removed
     *
     *  It registers nothing itself: it installs its handler with the OS (sigaction, signal) for a signal once a SignalHandler
     *  is added for it, and then calls each of that signal's handlers, one after the other - the direct ones on the thread the
     *  signal interrupted, the safe ones on SafeSignalsManager's thread (see SignalHandler::Type).
     *
     *  When the last of a signal's handlers is removed, the signal reverts to SIG_DFL (not to what it was before).
     *
     *  \note   The SignalHandlerRegistry must only be accessed after the start of main, and must not
     *          be accessed after main has completed.
     *
     *  To use 'safe' signal handlers, be sure to read about and use
     *      @see SignalHandlerRegistry::SafeSignalsManager
     *
     *  \note   Why not Execution::CallbackRegistry, and an ID to remove a handler by, as Stroika's other callbacks are: a signal's
     *          handlers are a set of values - got and set whole (GetSignalHandlers, SetSignalHandlers), with kIGNORED among them
     *          meaning 'ignore it' - which an ID-based registry does not model. So a SignalHandler stays a value with an
     *          identity, which serves as its ID: to remove one, keep the one added. (The signal handler itself never reads the
     *          registry, so the registry's own locks are no concern there: it reads a table of its own - see SignalHandler::Type.)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     */
    class SignalHandlerRegistry {
    public:
        /**
         *  If this handler is set to the the ONLY handler for a given signal, then that signal handler is
         *  effectively ignored.
         *
         *  To get the signal to be handled the DEFAULT way - remove all signal handlers.
         */
        static const SignalHandler kIGNORED;

    public:
        /**
         * Access singleton implementation. None exists until this is called.
         */
        static SignalHandlerRegistry sThe;

    private:
        SignalHandlerRegistry ();

    public:
        SignalHandlerRegistry (const SignalHandlerRegistry&) = delete;
        ~SignalHandlerRegistry ();

    public:
        nonvirtual SignalHandlerRegistry& operator= (const SignalHandlerRegistry&) = delete;

    public:
        class SafeSignalsManager;

    public:
        /**
         * Returns the set of signals trapped by the SignalHandlerRegistry registry. Note - if not 'Installed ()' - these
         * are tracked internally by Stroika but not actually installed in the OS.
         */
        nonvirtual Containers::Set<SignalID> GetHandledSignals () const;

    public:
        /**
         * Returns the set of signals trapped by the SignalHandlerRegistry registry. This doesn't imply there is a handler.
         * NB: A signal handler must be registered for a given signal number AND the signal number must be in GetHandledSignals () AND
         * the SignalHandlerRegistry must be Installed () - to get the signal called.
         *
         * It is NOT an error to have a signal handler registered for a signal not in the set of GetHandledSignals () - or vice versa.
         * Signals in the list of GetHandledSignals() with no handlers are effectively ignored.
         */
        nonvirtual Containers::Set<SignalHandler> GetSignalHandlers (SignalID signal) const;

    public:
        /**
         * @see GetSignalHandlers().
         *
         * SetSignalHandlers () with NO arguments uninstalls all Stroika signal handlers for this signal.
         * SetSignalHandlers () with ONE argument makes Stroika take-over the signal handling - and sets the set of hanlders to be
         * exactly the one given (effectively removing any others previously added).
         * SetSignalHandlers () with ONE a set of handlers registers all the given handlers.
         *
         * Note - if through ANY combination of set/add/remove - you have NO signal handler - this reverts to SIG_DFL, and if you have
         * exactly ONE signal handler - and its kIGNORED- the signal will be ignored.
         *
         *  \note Setting any 'Safe' signal handlers requires that SafeSignalsManager has been created.
         *
         *  \note Once this returns, a safe handler it removed is not running, and is not called again - not even for a signal that
         *        arrived before - so what the handler uses may then go: it waits for a call of it running on SafeSignalsManager's
         *        thread to end. Except on that thread (from a safe handler), where the handler running is the caller's - as
         *        std::stop_callback's destructor does. A direct handler, a function, may still be finishing a call on a thread the
         *        signal interrupted.
         *
         *  \note SetSignalHandlers, AddSignalHandler and RemoveSignalHandler change a signal's handlers one at a time, so calls
         *        on several threads at once lose none of each other's changes.
         */
        nonvirtual void SetSignalHandlers (SignalID signal);
        nonvirtual void SetSignalHandlers (SignalID signal, const SignalHandler& handler);
        nonvirtual void SetSignalHandlers (SignalID signal, const Containers::Set<SignalHandler>& handlers);

    public:
        /**
         * @see GetSignalHandlers().
         *
         *  \note - subtlety - if you wish to later call RemoveSignalHandler, save the signalhandler in a static const of type
         *          SignalHandler, and re-use that value.
         *
         *  \note Adding any 'Safe' signal handlers requires that SafeSignalsManager has been created.
         */
        nonvirtual void AddSignalHandler (SignalID signal, const SignalHandler& handler);

    public:
        /**
         * @see GetSignalHandlers()
         *
         *  \note Once this returns, a safe handler removed is not running, and is not called again: @see SetSignalHandlers ()
         */
        nonvirtual void RemoveSignalHandler (SignalID signal, const SignalHandler& handler);

    public:
        /**
         * This signal handler simply prints error to the trace log, and calls 'abort' - which on most operating systems will allow the
         * debugger to examine the errant code.
         */
        static void DefaultCrashSignalHandler (SignalID signal) noexcept;

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
        static Containers::Set<SignalID> GetStandardCrashSignals ();

    public:
        /**
         *  The set of signals given (by default GetStandardCrashSignals) will be set to the given handler
         *  (by default DefaultCrashSignalHandler).
         *
         *  The only exception is SIGABRT will be intentionally ignored from this call because it prevents abort()
         *  from functioning properly. We COULD disable SIGABRT upon receipt of that signal (SIG_DFL) but that
         *  would be different than other signals handled, raise re-entrancy issues etc. Didn't seem worth while.
         *
         *  \note   By default these are created as 'unsafe' signal handlers, meaning that they are run
         *          on the thread that triggered the error. This is mostly because we (currently) have no other
         *          way to capture what thread we were running on at that point.
         *
         *          This is bad, because we could deadlock in handling the crash. But realistically, we were crashing
         *          anyhow, so the better stack trace maybe worth it.
         *
         *          @todo https://github.com/SophistSolutions/Stroika/issues/1287 - pass the handler the signal's siginfo_t
         *
         *  After a stack overflow the handler cannot run - unless the thread has an alternate signal stack: @see
         *  InstallAlternateSignalStack (POSIX).
         */
        nonvirtual void SetStandardCrashHandlerSignals (SignalHandler handler = SignalHandler{DefaultCrashSignalHandler, SignalHandler::Type::eDirect},
                                                        const Containers::Set<SignalID>& forSignals = GetStandardCrashSignals ());

#if qStroika_Platform_POSIX
    public:
        /**
         *  \brief Give the calling thread an alternate signal stack (sigaltstack), so a signal handler runs even when the
         *         thread's own stack is used up - after a stack overflow
         *
         *  Stroika installs its signal handler with SA_ONSTACK, so on a thread with an alternate signal stack, signal handlers
         *  run there. Opt in, per thread: nothing calls this for you (TestHarness::Setup does, for a test program's main thread).
         *  Each call's thread holds the stack - about SIGSTKSZ + 64KB, 77KB on Linux x86_64 - until it exits; a thread that has
         *  an alternate signal stack already keeps it (a sanitizer sets its own). Most crashes are not stack overflows, and
         *  without it a stack overflow still ends the program (SIGSEGV), just without the crash handler's report.
         *
         *  POSIX only: on Windows a stack overflow is not a signal (it is an SEH exception), so no signal handler runs for it.
         */
        static void InstallAlternateSignalStack ();
#endif

    private:
        // Sets a signal's handlers to what change makes of its current ones, then waits until no safe handler that removed is
        // running - after releasing fUpdating_, as a running safe handler may itself change handlers
        nonvirtual void Update_ (SignalID signal, const function<Containers::Set<SignalHandler> (const Containers::Set<SignalHandler>&)>& change);

    private:
        // Sets a signal's handlers - with fUpdating_ held
        nonvirtual void SetSignalHandlers_ (SignalID signal, const Containers::Set<SignalHandler>& handlers);

    private:
        // One change of the handlers at a time, so a change computed from the handlers it read (AddSignalHandler,
        // RemoveSignalHandler) loses no change made meanwhile
        mutex fUpdating_;

    private:
        RWSynchronized<Containers::Mapping<SignalID, Containers::Set<SignalHandler>>> fDirectHandlers_;

    private:
        /*
         *  Each signal's direct handlers, as the signal handler (FirstPassSignalHandler_) reads them - it may use no container
         *  (see SignalHandler::Type) - so a fixed table of function pointers, each slot a lock-free atomic; an empty one is
         *  nullptr. SetSignalHandlers changes one slot at a time and never moves a handler, so a signal arriving meanwhile calls
         *  each handler at most once.
         */
        static constexpr size_t     kMaxDirectHandlersPerSignal_ = qStroika_Foundation_Execution_SignalHandlers_MaxDirectPerSignal;
        atomic<void (*) (SignalID)> fDirectSignalHandlersCache_[NSIG][kMaxDirectHandlersPerSignal_]{};
        static_assert (atomic<void (*) (SignalID)>::is_always_lock_free);

    private:
        static void FirstPassSignalHandler_ (SignalID signal);

    public:
        // DEPRECATED - kept only so code written for older Stroika versions keeps compiling
        [[deprecated ("Since Stroika v3.0d21, use SignalHandlerRegistry::sThe")]] static SignalHandlerRegistry& Get ()
        {
            return sThe;
        }
    };
    inline SignalHandlerRegistry SignalHandlerRegistry::sThe;

    /**
     *  \brief The thread safe (eSafe) signal handlers run on - see SignalHandler::Type for what that means
     *
     *  To use safe handlers, you must construct a SignalHandlerRegistry::SafeSignalsManager.
     *
     *  \note   This must be constructed BEFORE adding any safe signal handler, and
     *          must be destroyed before exiting main (so it can shutdown its own threads).
     *
     *          And only one SafeSignalsManager instance can exist at a time.
     *
     *  Destroying it removes the safe handlers it runs: each signal keeps its direct handlers, or with none reverts to its
     *  default.
     *
     *  \par Example Usage
     *      \code
     *          int main (int argc, const char* argv[])
     *          {
     *              SignalHandlerRegistry::SafeSignalsManager safeSignals; // first in main, so it outlives the handlers
     *              atomic<bool>                              quit{false};
     *              SignalHandlerRegistry::sThe.AddSignalHandler (SIGINT, SignalHandler{[&quit] (SignalID) { quit = true; }}); // safe
     *              ...
     *          }
     *      \endcode
     */
    class SignalHandlerRegistry::SafeSignalsManager {
    public:
        /**
         */
        SafeSignalsManager ();
        SafeSignalsManager (const SafeSignalsManager&) = delete;

    public:
        ~SafeSignalsManager ();

    public:
        nonvirtual SafeSignalsManager& operator= (const SafeSignalsManager&) = delete;

    private:
        class Rep_;
#if defined(__cpp_lib_atomic_shared_ptr)
        static inline atomic<shared_ptr<Rep_>> sTheRep_{nullptr};
#else
        static inline shared_ptr<Rep_> sTheRep_{nullptr};
#endif
        // The signal handler's way to it - a shared_ptr copy is not signal-safe (see SignalHandler::Type): a raw pointer, and how
        // many handlers are using it, which ~SafeSignalsManager waits to drain before letting it go
        static inline atomic<Rep_*>        sTheRepForSignalHandler_{nullptr};
        static inline atomic<unsigned int> sSignalHandlersUsingRep_{0};
        static_assert (atomic<Rep_*>::is_always_lock_free and atomic<unsigned int>::is_always_lock_free);

    private:
        friend class SignalHandlerRegistry;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SignalHandlers.inl"

#endif /*_Stroika_Foundation_Execution_SignalHandlers_h_*/
