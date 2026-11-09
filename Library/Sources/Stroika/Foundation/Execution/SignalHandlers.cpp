/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <algorithm>
#if qStroika_Platform_Windows
#include <windows.h>
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/BackTrace.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#if qStroika_Platform_POSIX
#include "Stroika/Foundation/Execution/Platform/POSIX/SemWaitableEvent.h"
#endif
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"

#include "SignalHandlers.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using Containers::Mapping;
using Containers::Set;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

// maybe useful while debugging signal code, but VERY unsafe
// and could easily be the source of bugs/deadlocks!
#ifndef qDoDbgTraceOnSignalHandlers_
#define qDoDbgTraceOnSignalHandlers_ 0
#endif

// maybe useful while debugging signal code, but VERY unsafe
// and could easily be the source of bugs/deadlocks!
#ifndef qDoBacktraceOnFirstPassSignalHandler_
#define qDoBacktraceOnFirstPassSignalHandler_ 0
#endif

#if qStroika_Platform_Windows
namespace {
    // What SetSignalHandlers last gave the OS for each signal: the CRT resets a signal to SIG_DFL before calling its handler (all
    // but SIGFPE - https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/signal), so FirstPassSignalHandler_ puts it back
    atomic<void (*) (int)> sInstalledForSignal_[NSIG]{};
}
#endif

/*
 ********************************************************************************
 *************************** Execution::SignalHandler ***************************
 ********************************************************************************
 */
Characters::String SignalHandler::ToString () const
{
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "type: "sv << GetType ();
    // rough guess what to print...
    if (fCall_.target_type () == typeid (void (*) (SignalID))) {
        sb << ", target: "sv << "{}"_f(reinterpret_cast<const void*> (fCall_.target<void (*) (SignalID)> ()));
    }
    else {
        // type only/mainly interesting if not one of the above so we're printing nullptr
        sb << ", targetType: "sv << fCall_.target_type ();
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 *********** Execution::SignalHandlerRegistry::SafeSignalsManager ***************
 ********************************************************************************
 */
/*
 *  Design note:
 *      Though it would be logical to use a BlockQueue<> here to implement the signal forwarding,
 *      we cannot since that code allocates memory, and could deadlock.
 */
class SignalHandlerRegistry::SafeSignalsManager::Rep_ final {
private:
    void waitForNextSig_ ()
    {
#if qStroika_Platform_POSIX
        fRecievedSig_.Wait ();
#elif qStroika_Platform_Windows
        Verify (::WaitForSingleObject (fRecievedSig_, INFINITE) == WAIT_OBJECT_0);
#endif
    }
    // Called by the signal handler (NotifyOfArrivalOfPossiblySafeSignal), so it takes no lock (see SignalHandler::Type): sem_post,
    // or a Win32 event - not a mutex and condition_variable
    void tell2WakeAfterDataUpdate_ ()
    {
        fWorkMaybeAvailable_ = true;
#if qStroika_Platform_POSIX
        fRecievedSig_.Set ();
#elif qStroika_Platform_Windows
        (void)::SetEvent (fRecievedSig_);
#endif
    }

public:
    Rep_ ()
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper trcCtx{L"Stroika::Foundation::Execution::SignalHandlerRegistry::SafeSignalsManager::Rep_::CTOR",
                                         Stroika_Foundation_Debug_OptionalizeTraceArgs (L"this=%p", this)};
#endif
        fBlockingQueuePusherThread_ = Thread::New (
            [this] () {
                // This is a safe context
                Debug::TraceContextBumper trcCtx{"Stroika::Foundation::Execution::Signals::{}::fBlockingQueueDelegatorThread_"};
                while (true) {
                    Debug::TraceContextBumper trcCtx1{"Waiting for next safe signal"};
                    Thread::CheckForInterruption ();
                    waitForNextSig_ ();
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("fRecievedSig_ wait complete (either arrival or timeout): fLastSignalRecieved_ = {}"_f, fLastSignalRecieved_.load ());
#endif
                    if (fLastSignalRecieved_ < NSIG) {
                    Again:
                        for (int i = 0; i < NSIG; ++i) {
                            while (fIncomingSignalCounts_[i] > 0) {
                                DbgTrace ("fIncomingSignalCounts_[{}] = {}"_f, i, fIncomingSignalCounts_[i].load ());
                                fIncomingSignalCounts_[i]--;
                                Debug::TraceContextBumper trcCtx2{"Invoking SAFE signal handlers"};
                                // a copy, so no lock is held while the handlers run - one may add or remove handlers (since C++23 a
                                // temporary in a range-for's initializer - as a lock - lives until the loop ends)
                                Set<SignalHandler> handlers = fHandlers_.cget ()->LookupValue (i);
                                for (const SignalHandler& sh : handlers) {
                                    Assert (sh.GetType () == SignalHandler::Type::eSafe);
                                    IgnoreExceptionsExceptThreadAbortForCall (sh (i));
                                }
                            }
                        }

                        // reset fLastSignalRecieved_ as a shortcut
                        fLastSignalRecieved_ = NSIG;
                        // but check for races, and reset
                        for (int i = 0; i < NSIG; ++i) {
                            while (fIncomingSignalCounts_[i] > 0) {
                                fLastSignalRecieved_ = i;
                                DbgTrace ("Rare, but possible race avoidance"_f);
                                goto Again;
                            }
                        }
                    }
                    // When we set fWorkMaybeAvailable_ false, do one more time around loop so no race - if we set from true to false
                    // we always recehck protected data (and mutex not signal safe)
                    if (fWorkMaybeAvailable_.exchange (false)) {
                        goto Again;
                    }
                }
            },
            Thread::eAutoStart, "Signal Handler Safe Execution Thread"sv);
    }

public:
    ~Rep_ ()
    {
        Debug::TraceContextBumper trcCtx{"Stroika::Foundation::Execution::SignalHandlerRegistry::SafeSignalsManager::Rep_::~Rep_",
                                         Stroika_Foundation_Debug_OptionalizeTraceArgs ("this={}"_f, static_cast<const void*> (this))};
        Thread::SuppressInterruptionInContext suppressInterruption;
        fBlockingQueuePusherThread_.Abort ();
        tell2WakeAfterDataUpdate_ ();
        fBlockingQueuePusherThread_.AbortAndWaitForDone ();
#if qStroika_Platform_Windows
        Verify (::CloseHandle (fRecievedSig_));
#endif
    }

public:
    /*
     *  Called at VERY UNSAFE TIME. NEVER allocates memory, or use locks.
     */
    void NotifyOfArrivalOfPossiblySafeSignal (SignalID signal)
    {
        Require (0 <= signal and signal < static_cast<SignalID> (std::size (fIncomingSignalCounts_)));
        // Check fHanlderAvailable_ [] as a performance optimizaiton. This gets called by direct-sginals, even when there are no safe signals to
        // be delegated to
        if (fHanlderAvailable_[signal]) {
            fIncomingSignalCounts_[signal]++;
            fLastSignalRecieved_ = signal; // used as a quick check
            tell2WakeAfterDataUpdate_ ();
        }
    }

public:
    Set<SignalID> GetHandledSignals () const
    {
        return Set<SignalID>{fHandlers_.cget ()->Keys ()};
    }

public:
    Set<SignalHandler> GetSignalHandlers (SignalID signal) const
    {
        return fHandlers_.cget ()->LookupValue (signal);
    }

public:
    void Remove (SignalID signal)
    {
        fHandlers_.rwget ()->RemoveIf (signal);
        PopulateSafeSignalHandlersCache_ (signal);
    }

public:
    void Add (SignalID signal, const Containers::Set<SignalHandler>& safeHandlers)
    {
        fHandlers_.rwget ()->Add (signal, safeHandlers);
        PopulateSafeSignalHandlersCache_ (signal);
    }

private:
    void PopulateSafeSignalHandlersCache_ (SignalID signal)
    {
        Require (0 <= signal and signal < static_cast<SignalID> (std::size (fHanlderAvailable_)));
        fHanlderAvailable_[signal] = fHandlers_.rwget ()->Lookup (signal).has_value ();
    }

private:
    Synchronized<Mapping<SignalID, Set<SignalHandler>>> fHandlers_;
    atomic<bool> fHanlderAvailable_[NSIG]{}; // if true post to blocking q; the signal handler reads it
private:
    /*
     *  Instead of maintaining an acutal Q, just maintain a count of number of signals recieved for each signal number.
     *  This means signals not necessarily delivered in order, but this appraoch has the advantage of using a small amount of memory to
     *  essentially guarantee no overflow
     */
    atomic<unsigned int> fIncomingSignalCounts_[NSIG]{};
    atomic<SignalID>     fLastSignalRecieved_{NSIG};
    Thread::Ptr fBlockingQueuePusherThread_; // no need to synchonize cuz only called from thread which constructs/destroys safetymfg
private:
    atomic<bool> fWorkMaybeAvailable_{false};
#if qStroika_Platform_POSIX
    Execution::Platform::POSIX::SemWaitableEvent fRecievedSig_;
#elif qStroika_Platform_Windows
    HANDLE fRecievedSig_{::CreateEvent (nullptr, FALSE, FALSE, nullptr)}; // auto-reset
#endif
};

/*
 ********************************************************************************
 *********** Execution::SignalHandlerRegistry::SafeSignalsManager ***************
 ********************************************************************************
 */
SignalHandlerRegistry::SafeSignalsManager::SafeSignalsManager ()
{
    Debug::TraceContextBumper trcCtx{"Stroika::Foundation::Execution::SignalHandlerRegistry::SafeSignalsManager::CTOR"};
    shared_ptr<Rep_>          rep = MakeSharedPtr<SignalHandlerRegistry::SafeSignalsManager::Rep_> ();
#if defined(__cpp_lib_atomic_shared_ptr)
    Require (sTheRep_.load () == nullptr);
    sTheRep_.store (rep);
#else
    Require (atomic_load (&sTheRep_) == nullptr);
    atomic_store (&sTheRep_, rep);
#endif
    sTheRepForSignalHandler_.store (rep.get ());
}

SignalHandlerRegistry::SafeSignalsManager::~SafeSignalsManager ()
{
    Debug::TraceContextBumper trcCtx{"Stroika::Foundation::Execution::SignalHandlerRegistry::SafeSignalsManager::DTOR"};
    // The safe handlers it runs go with it - each signal keeps its direct handlers, or with none reverts to its default: else a
    // signal arriving later would find safe handlers with no thread to run them, and be lost
#if defined(__cpp_lib_atomic_shared_ptr)
    if (shared_ptr<Rep_> rep = sTheRep_.load ()) {
#else
    if (shared_ptr<Rep_> rep = atomic_load (&sTheRep_)) {
#endif
        for (SignalID s : rep->GetHandledSignals ()) {
            // a copy, so the read lock is gone before SetSignalHandlers takes the write lock
            Set<SignalHandler> directHandlers = sThe.fDirectHandlers_.cget ()->LookupValue (s);
            sThe.SetSignalHandlers (s, directHandlers);
        }
    }
    // No signal handler gets the Rep_ once this is null; those already using it finish before it is let go
    sTheRepForSignalHandler_.store (nullptr);
    while (sSignalHandlersUsingRep_.load () != 0) {
        this_thread::yield ();
    }
#if defined(__cpp_lib_atomic_shared_ptr)
    SignalHandlerRegistry::SafeSignalsManager::sTheRep_.store (shared_ptr<Rep_>{}); // this will wait for shutdown of safe processing thread to shut down
#else
    atomic_store (&SignalHandlerRegistry::SafeSignalsManager::sTheRep_, shared_ptr<Rep_>{}); // this will wait for shutdown of safe processing thread to shut down
#endif
}

/*
 ********************************************************************************
 ******************** Execution::SignalHandlerRegistry **************************
 ********************************************************************************
 */
const SignalHandler SignalHandlerRegistry::kIGNORED = SignalHandler{SIG_IGN, SignalHandler::Type::eDirect};

SignalHandlerRegistry::SignalHandlerRegistry ()
{
    if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
        [[maybe_unused]] static int nConstructed = 0;
        ++nConstructed;
        Assert (nConstructed == 1);
    }
    Debug::TraceContextBumper trcCtx{"Stroika::Foundation::Execution::SignalHandlerRegistry::CTOR"};
}

SignalHandlerRegistry::~SignalHandlerRegistry ()
{
    Debug::TraceContextBumper trcCtx{"Stroika::Foundation::Execution::SignalHandlerRegistry::DTOR"};
#if defined(__cpp_lib_atomic_shared_ptr)
    Assert (SafeSignalsManager::sTheRep_.load () == nullptr); // must be cleared first
#else
    Assert (atomic_load (&SafeSignalsManager::sTheRep_) == nullptr); // must be cleared first
#endif
}

Set<SignalID> SignalHandlerRegistry::GetHandledSignals () const
{
    Set<SignalID> result{fDirectHandlers_.cget ()->Keys ()};
#if defined(__cpp_lib_atomic_shared_ptr)
    if (shared_ptr<SafeSignalsManager::Rep_> tmp = SafeSignalsManager::sTheRep_.load ()) {
#else
    if (shared_ptr<SafeSignalsManager::Rep_> tmp = atomic_load (&SafeSignalsManager::sTheRep_)) {
#endif
        result += tmp->GetHandledSignals ();
    }
    return result;
}

Set<SignalHandler> SignalHandlerRegistry::GetSignalHandlers (SignalID signal) const
{
    Set<SignalHandler> result = fDirectHandlers_.cget ()->LookupValue (signal);
#if defined(__cpp_lib_atomic_shared_ptr)
    if (shared_ptr<SafeSignalsManager::Rep_> tmp = SafeSignalsManager::sTheRep_.load ()) {
#else
    if (shared_ptr<SafeSignalsManager::Rep_> tmp = atomic_load (&SafeSignalsManager::sTheRep_)) {
#endif

        result += tmp->GetSignalHandlers (signal);
    }
    return result;
}

void SignalHandlerRegistry::SetSignalHandlers (SignalID signal)
{
    SetSignalHandlers (signal, Set<SignalHandler>{});
}

void SignalHandlerRegistry::SetSignalHandlers (SignalID signal, const SignalHandler& handler)
{
    SetSignalHandlers (signal, Set<SignalHandler> ({handler}));
}

void SignalHandlerRegistry::SetSignalHandlers (SignalID signal, const Set<SignalHandler>& handlers)
{
    Debug::TraceContextBumper trcCtx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
        "Stroika::Foundation::Execution::SignalHandlerRegistry::{}::SetSignalHandlers", "signal: {}, handlers: {}"_f, SignalToName (signal), handlers)};

    Set<SignalHandler> directHandlers;
    Set<SignalHandler> safeHandlers;
    handlers.Apply ([&directHandlers, &safeHandlers] (SignalHandler si) {
        switch (si.GetType ()) {
            case SignalHandler::Type::eDirect: {
                directHandlers.Add (si);
            } break;
            case SignalHandler::Type::eSafe: {
                safeHandlers.Add (si);
            } break;
        }
    });
    Assert (directHandlers.size () + safeHandlers.size () == handlers.size ());

    shared_ptr<SignalHandlerRegistry::SafeSignalsManager::Rep_> tmp = SignalHandlerRegistry::SafeSignalsManager::sTheRep_;

    if (not safeHandlers.empty ()) {
        // To use safe signal handlers, you must have a SignalHandlerRegistry::SafeSignalsManager
        // defined first. It is recommended that you define an instance of
        // SignalHandlerRegistry::SafeSignalsManager handler; should be defined in main ()
#if defined(__cpp_lib_atomic_shared_ptr)
        Require (SafeSignalsManager::sTheRep_.load () != nullptr);
#else
        Require (atomic_load (&SafeSignalsManager::sTheRep_) != nullptr);
#endif
    }

    auto sigSetHandler = [] (SignalID signal, void (*fun) (int)) {
#if qStroika_Platform_POSIX
        struct sigaction sa{};
        sa.sa_handler = fun;
        Verify (sigemptyset (&sa.sa_mask) == 0); // nb: cannot use :: on macos - macro - LGP 2016-12-30
        // SA_ONSTACK: on the thread's alternate signal stack where it has one (InstallAlternateSignalStack), so a crash handler runs
        // after a stack overflow too. Important NOT to set SA_RESTART for interrupt() - but maybe for others helpful - maybe add option?
        sa.sa_flags = SA_ONSTACK;
        Verify (::sigaction (signal, &sa, nullptr) == 0);
#elif qStroika_Platform_Windows
        sInstalledForSignal_[signal].store (fun);
        Verify (::signal (signal, fun) != SIG_ERR);
#endif
    };

    // Held to the end - the OS's handler set too - so calls for one signal from several threads at once leave the OS's handler
    // matching the handlers set last
    auto l = fDirectHandlers_.rwget ();
    if (directHandlers.empty ()) {
        l->RemoveIf (signal);
    }
    else {
        l->Add (signal, directHandlers);
    }
    /*
     *  Update the signal handler's table (fDirectSignalHandlersCache_) - under fDirectHandlers_'s lock, so one update at a time:
     *  empty the slots of handlers no longer wanted, then put new ones in empty slots - each slot stored on its own, and no
     *  handler moved, so a signal arriving meanwhile sees each handler at most once. kIGNORED (SIG_IGN) is no function to call.
     */
    Require (0 <= signal and signal < static_cast<SignalID> (std::size (fDirectSignalHandlersCache_)));
    vector<void (*) (SignalID)> wanted;
    for (const SignalHandler& sh : l->LookupValue (signal)) {
        Assert (sh.fDirectCall_ != nullptr); // a direct handler is a function pointer - see SignalHandler's constructors
        if (sh.fDirectCall_ != SIG_IGN) {
            wanted.push_back (sh.fDirectCall_);
        }
    }
    auto& slots = fDirectSignalHandlersCache_[signal];
    for (atomic<void (*) (SignalID)>& slot : slots) {
        if (void (*h) (SignalID) = slot.load ()) {
            if (auto i = find (wanted.begin (), wanted.end (), h); i != wanted.end ()) {
                wanted.erase (i); // still wanted - stays where it is
            }
            else {
                slot.store (nullptr);
            }
        }
    }
    for (void (*h) (SignalID) : wanted) {
        auto i = find_if (begin (slots), end (slots), [] (const atomic<void (*) (SignalID)>& slot) { return slot.load () == nullptr; });
        Require (i != end (slots)); // else raise qStroika_Foundation_Execution_SignalHandlers_MaxDirectPerSignal
        i->store (h);
    }

    if (tmp != nullptr) {
        if (safeHandlers.empty ()) {
            tmp->Remove (signal);
        }
        else {
            tmp->Add (signal, safeHandlers);
        }
    }

    // And set the actual signal handlers
    if (handlers.empty ()) {
        sigSetHandler (signal, SIG_DFL);
    }
    else if (handlers.size () == 1 and handlers.Contains (SignalHandlerRegistry::kIGNORED)) {
        sigSetHandler (signal, SIG_IGN);
    }
    else {
        sigSetHandler (signal, FirstPassSignalHandler_);
    }
}

void SignalHandlerRegistry::AddSignalHandler (SignalID signal, const SignalHandler& handler)
{
    Set<SignalHandler> s = GetSignalHandlers (signal);
    s.Add (handler);
    SetSignalHandlers (signal, s);
}

void SignalHandlerRegistry::RemoveSignalHandler (SignalID signal, const SignalHandler& handler)
{
    Set<SignalHandler> s = GetSignalHandlers (signal);
    Require (s.Contains (handler));
    s.Remove (handler);
    SetSignalHandlers (signal, s);
}

void SignalHandlerRegistry::DefaultCrashSignalHandler ([[maybe_unused]] SignalID signal) noexcept
{
    DbgTrace ("Serious Signal Error trapped: {} ... Aborting"_f, SignalToName (signal));
    abort ();
}

Containers::Set<SignalID> SignalHandlerRegistry::GetStandardCrashSignals ()
{
    Containers::Set<SignalID> results;
    results.Add (SIGABRT);
    results.Add (SIGILL);
    results.Add (SIGFPE);
    results.Add (SIGSEGV);
#if qStroika_Platform_POSIX
    results.Add (SIGSYS);
    results.Add (SIGBUS);
#endif
    return results;
}

void SignalHandlerRegistry::SetStandardCrashHandlerSignals (SignalHandler handler, const Containers::Set<SignalID>& forSignals)
{
    for (SignalID s : forSignals) {
        if (s != SIGABRT) {
            SetSignalHandlers (s, handler);
        }
    }
}

#if qStroika_Platform_POSIX
void SignalHandlerRegistry::InstallAlternateSignalStack ()
{
    /*
     *  One per thread, made on the thread's first call (thread_local), and removed then freed as the thread exits. Left alone
     *  where the thread has one already. Room for DefaultCrashSignalHandler's DbgTrace, beyond the minimum (SIGSTKSZ).
     */
    struct AlternateStack_ {
        unique_ptr<char[]> fStack_;
        AlternateStack_ ()
        {
            stack_t current{};
            Verify (::sigaltstack (nullptr, &current) == 0);
            if ((current.ss_flags & SS_DISABLE) != 0) {
                size_t size = SIGSTKSZ + 64 * 1024;
                fStack_.reset (new char[size]);
                stack_t ss{};
                ss.ss_sp   = fStack_.get ();
                ss.ss_size = size;
                Verify (::sigaltstack (&ss, nullptr) == 0);
            }
        }
        ~AlternateStack_ ()
        {
            if (fStack_ != nullptr) {
                stack_t ss{};
                ss.ss_flags = SS_DISABLE;
                Verify (::sigaltstack (&ss, nullptr) == 0);
            }
        }
    };
    [[maybe_unused]] static thread_local AlternateStack_ tAlternateStack_;
}
#endif

// use no_sanitize(thread) to workaround https://github.com/SophistSolutions/Stroika/issues/812 (STK-677)
Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD void SignalHandlerRegistry::FirstPassSignalHandler_ (SignalID signal)
{
    /*
     *  Important example / stack backtrace to bear in mind:
     *          #0  __lll_lock_wait_private () at ../sysdeps/unix/sysv/linux/x86_64/lowlevellock.S:95
     *          #1  0x00007f4edcd924fc in __GI___libc_malloc (bytes=139975802748960) at malloc.c:2891
     *          #2  0x0000000000540168 in operator new(unsigned long) ()
     *          #3  0x000000000042581e in __gnu_cxx::new_allocator<std::_Rb_tree_node<std::thread::id> >::allocate (this=0x7f4eb7ffd380, __n=1) at /usr/include/c++/4.9/ext/new_allocator.h:104
     *          #4  0x0000000000423e54 in std::allocator_traits<std::allocator<std::_Rb_tree_node<std::thread::id> > >::allocate (__a=..., __n=1) at /usr/include/c++/4.9/bits/alloc_traits.h:357
     *          #5  0x000000000042161b in std::_Rb_tree<std::thread::id, std::thread::id, std::_Identity<std::thread::id>, std::less<std::thread::id>, std::allocator<std::thread::id> >::_M_get_node (
     *              this=0x7f4eb7ffd380) at /usr/include/c++/4.9/bits/stl_tree.h:385
     *          #6  0x000000000041eae1 in std::_Rb_tree<std::thread::id, std::thread::id, std::_Identity<std::thread::id>, std::less<std::thread::id>, std::allocator<std::thread::id> >::_M_create_node<std::thread::id>(std::thread::id&&) (this=0x7f4eb7ffd380) at /usr/include/c++/4.9/bits/stl_tree.h:417
     *          #7  0x000000000041c348 in std::_Rb_tree<std::thread::id, std::thread::id, std::_Identity<std::thread::id>, std::less<std::thread::id>, std::allocator<std::thread::id> >::_M_insert_<std::thread::id>(std::_Rb_tree_node_base*, std::_Rb_tree_node_base*, std::thread::id&&) (this=0x7f4eb7ffd380, __x=0x0, __p=0x7f4eb7ffd388,
     *              __v=<unknown type in /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/gcc-4.9-debug-no-TPC/Test35, CU 0x0, DIE 0x5e560>) at /usr/include/c++/4.9/bits/stl_tree.h:1143
     *          #8  0x000000000041858f in std::_Rb_tree<std::thread::id, std::thread::id, std::_Identity<std::thread::id>, std::less<std::thread::id>, std::allocator<std::thread::id> >::_M_insert_equal<std::thread::id>(std::thread::id&&) (this=0x7f4eb7ffd380, __v=<unknown type in /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/gcc-4.9-debug-no-TPC/Test35, CU 0x0, DIE 0x5944e>)
     *              at /usr/include/c++/4.9/bits/stl_tree.h:1523
     *          #9  0x0000000000414a37 in std::__cxx1998::multiset<std::thread::id, std::less<std::thread::id>, std::allocator<std::thread::id> >::insert(std::thread::id&&) (this=0x7f4eb7ffd380,
     *              __x=<unknown type in /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/gcc-4.9-debug-no-TPC/Test35, CU 0x0, DIE 0x54234>) at /usr/include/c++/4.9/bits/stl_multiset.h:498
     *          #10 0x0000000000412303 in std::__debug::multiset<std::thread::id, std::less<std::thread::id>, std::allocator<std::thread::id> >::insert(std::thread::id&&) (this=0x7f4eb7ffd380,
     *              __x=<unknown type in /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/gcc-4.9-debug-no-TPC/Test35, CU 0x0, DIE 0x507d4>) at /usr/include/c++/4.9/debug/multiset.h:257
     *          #11 0x0000000000410f61 in Stroika::Foundation::Debug::AssertExternallySynchronizedChecker::lock_shared (this=0x7f4eb7ffd370)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Containers/../Memory/../Debug/AssertExternallySynchronizedChecker.inl:92
     *          #12 0x00000000004157d8 in std::shared_lock<Stroika::Foundation::Debug::AssertExternallySynchronizedChecker const>::shared_lock (this=0x7f4eb7ffd270, m=...)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Configuration/Private/Defaults_CompilerAndStdLib_.h:2123
     *          #13 0x000000000048ce64 in Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::SignalHandler>::_SafeReadRepAccessor<Stroika::Foundation::Containers::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::_IRep>::_SafeReadRepAccessor (this=0x7f4eb7ffd270, it=0x7f4eb7ffd370)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/../Traversal/Iterable.inl:88
     *          #14 0x000000000048dd70 in Stroika::Foundation::Containers::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::_AssertRepValidType (this=0x7f4eb7ffd370) at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Set.inl:282
     *          #15 0x0000000000496803 in Stroika::Foundation::Containers::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::Set(Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::Containers::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::_IRep>&&) (this=0x7f4eb7ffd370, src=<unknown type in /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/gcc-4.9-debug-no-TPC/Test35, CU 0x267e27, DIE 0x2d5751>)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Set.inl:67
     *          #16 0x0000000000494ff0 in Stroika::Foundation::Containers::Concrete::Set_stdset<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::Concrete::Set_stdset_DefaultTraits<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> >, Stroika::Foundation::Common::ComparerWithWellOrder<Stroika::Foundation::Execution::SignalHandler> > >::Set_stdset (this=0x7f4eb7ffd370)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Factory/../Concrete/Set_stdset.inl:244
     *          #17 0x0000000000493c39 in Stroika::Foundation::Containers::Concrete::Set_Factory<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::Default_SFINAE_<Stroika::Foundation::Execution::SignalHandler>(Stroika::Foundation::Execution::SignalHandler*, std::enable_if<Stroika::Foundation::Configuration::has_lt<Stroika::Foundation::Execution::SignalHandler>::value&&std::is_same<Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > >, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, std::conditional<Stroika::Foundation::Configuration::has_eq<Stroika::Foundation::Execution::SignalHandler>::value&&std::is_convertible<Stroika::Foundation::Configuration::Private_::eq_result_impl<Stroika::Foundation::Execution::SignalHandler>::type, bool>::value, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler>, std::conditional<Stroika::Foundation::Configuration::has_lt<Stroika::Foundation::Execution::SignalHandler>::value&&std::is_convertible<Stroika::Foundation::Configuration::Private_::lt_result_impl<Stroika::Foundation::Execution::SignalHandler>::type, bool>::value, Stroika::Foundation::Common::ComparerWithWellOrder<Stroika::Foundation::Execution::SignalHandler>, std::shared_ptr<int> >::type>::type> > >::value, void>::type*) ()
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Factory/Set_Factory.inl:63
     *          #18 0x0000000000491336 in Stroika::Foundation::Containers::Concrete::Set_Factory<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::Default_ () at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Factory/Set_Factory.inl:57
     *          #19 0x000000000048dbf1 in Stroika::Foundation::Containers::Concrete::Set_Factory<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::mk () at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Factory/Set_Factory.inl:44
     *          #20 0x000000000048abbf in Stroika::Foundation::Containers::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::Set (this=0x7f4eb7ffd5e0) at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Set.inl:23
     *          #21 0x0000000000488de6 in Stroika::Foundation::Execution::SignalHandlerRegistry::FirstPassSignalHandler_ (signal=12)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/SignalHandlers.cpp:422
     *          #22 <signal handler called>
     *          #23 _int_malloc (av=av@entry=0x7f4ea8000020, bytes=bytes@entry=8160) at malloc.c:3769
     *          #24 0x00007f4edcd9250e in __GI___libc_malloc (bytes=8160) at malloc.c:2895
     *          #25 0x0000000000540168 in operator new(unsigned long) ()
     *          #26 0x000000000043b180 in __gnu_cxx::new_allocator<char>::allocate (this=0x7f4eb7ffdf3f, __n=8160) at /usr/include/c++/4.9/ext/new_allocator.h:104
     *          #27 0x000000000043b023 in std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >::_Rep::_S_create (__capacity=2033, __old_capacity=816, __alloc=...)
     *              at /usr/include/c++/4.9/bits/basic_string.tcc:607
     *          #28 0x000000000044236e in std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >::_Rep::_M_clone (this=0x7f4ea8002450, __alloc=..., __res=51)
     *              at /usr/include/c++/4.9/bits/basic_string.tcc:629
     *          #29 0x0000000000443e42 in std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >::reserve (this=0x7f4eb7ffe0c0, __res=867) at /usr/include/c++/4.9/bits/basic_string.tcc:510
     *          #30 0x0000000000480971 in std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >::append (this=0x7f4eb7ffe0c0, __str=...) at /usr/include/c++/4.9/bits/basic_string.tcc:332
     *          #31 0x0000000000527955 in std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >::operator+= (this=0x7f4eb7ffe0c0, __str=...) at /usr/include/c++/4.9/bits/basic_string.h:950
     *          #32 0x0000000000527342 in Stroika::Foundation::Debug::BackTrace (maxFrames=4294967295) at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Debug/BackTrace.cpp:50
     *          #33 0x0000000000485f61 in Stroika::Foundation::Execution::Private_::GetBT_ws () at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Exceptions.cpp:40
     *          #34 0x0000000000485e6e in Stroika::Foundation::Execution::Private_::GetBT_s () at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Exceptions.cpp:30
     *          #35 0x00000000004c56a8 in Stroika::Foundation::Execution::Throw<Stroika::Foundation::Execution::Thread::AbortException> (e2Throw=...)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/../Memory/../Execution/Exceptions.inl:40
     *          #36 0x00000000004c2afc in Stroika::Foundation::Execution::CheckForInterruption () at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Thread.cpp:993
     *          #37 0x00000000004f036c in Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntilQuietly (this=0x7ffe1997bf80, timeoutAt=1.7976931348623157e+308)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/WaitableEvent.cpp:83
     *          #38 0x00000000004f02bd in Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntil (this=0x7ffe1997bf80, timeoutAt=1.7976931348623157e+308)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/WaitableEvent.cpp:52
     *          #39 0x000000000041169b in Stroika::Foundation::Execution::WaitableEvent::Wait (this=0x7ffe1997bf80, timeout=1.7976931348623157e+308)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/WaitableEvent.inl:100
     *          #40 0x00000000004d4578 in Stroika::Foundation::Execution::ThreadPool::WaitForNextTask_(Stroika::Foundation::Execution::Function<void ()>*) (this=0x7ffe1997beb0, result=0x1841390)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/ThreadPool.cpp:444
     *          #41 0x00000000004d5288 in Stroika::Foundation::Execution::ThreadPool::MyRunnable_::Run (this=0x1841350)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/ThreadPool.cpp:57
     *          #42 0x00000000004d45db in Stroika::Foundation::Execution::ThreadPool::<lambda()>::operator()(void) const (__closure=0x1840ee0)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/ThreadPool.cpp:454
     *          #43 0x00000000004d4a14 in std::_Function_handler<void(), Stroika::Foundation::Execution::ThreadPool::mkThread_()::<lambda()> >::_M_invoke(const std::_Any_data &) (__functor=...)
     *              at /usr/include/c++/4.9/functional:2039
     *          #44 0x000000000041298a in std::function<void ()>::operator()() const (this=0x18413d0) at /usr/include/c++/4.9/functional:2439
     *          #45 0x00000000004c4c43 in Stroika::Foundation::Execution::Function<void ()>::operator()<>() const (this=0x1841410)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Execution/Function.inl:48
     *          #46 0x00000000004c06b6 in Stroika::Foundation::Execution::Thread::Rep_::Run_ (this=0x1841410) at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Thread.cpp:317
     *          #47 0x00000000004c0c2f in Stroika::Foundation::Execution::Thread::Rep_::ThreadMain_ (thisThreadRep=0x7ffe1997b820)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Thread.cpp:425
     *          #48 0x00000000004c03ee in Stroika::Foundation::Execution::Thread::Rep_::<lambda()>::operator()(void) const (__closure=0x1840c68)
     *              at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Thread.cpp:286
     *          #49 0x00000000004c420c in std::_Bind_simple<Stroika::Foundation::Execution::Thread::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Rep_>*)::<lambda()>()>::_M_invoke<>(std::_Index_tuple<>) (this=0x1840c68) at /usr/include/c++/4.9/functional:1700
     *          #50 0x00000000004c413e in std::_Bind_simple<Stroika::Foundation::Execution::Thread::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Rep_>*)::<lambda()>()>::operator()(void) (
     *              this=0x1840c68) at /usr/include/c++/4.9/functional:1688
     *          #51 0x00000000004c40ac in std::thread::_Impl<std::_Bind_simple<Stroika::Foundation::Execution::Thread::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Rep_>*)::<lambda()>()> >::_M_run(void) (this=0x1840c50) at /usr/include/c++/4.9/thread:115
     *          #52 0x0000000000589630 in execute_native_thread_routine ()
     *          #53 0x00007f4edd8056aa in start_thread (arg=0x7f4eb7fff700) at pthread_create.c:333
     *          #54 0x00007f4edce14eed in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:109
     *
     *   ANALYSIS OF ABOVE:
     *       At frame 22 (#22 <signal handler called>) - we have AT LEAST the malloc lock held (we could have more in principle)
     *       We receive a signal
     *       Deeper down, at frame 11 (#11 0x0000000000410f61) we acquire an unrelated lock, and AssertExternallySynchronizedChecker
     *       which does more memory allocations.
     *       deadlocking at frame 0 (#0  __lll_lock_wait_private)
     *
     *       BUT - worse than deadlocking this thread (thats bad enuf) - we acquired a lock on AssertExternallySynchronizedChecker
     *       which we will never give up, and kill any other thread doing much of anything.
     *
     *   SUMMARY:
     *      >   THIS CODE - CAN NEVER SAFELY CALL MALLOC!!!!
     *      >   Be CAREFUL to do as little as possible here.
     */
#if qDoDbgTraceOnSignalHandlers_
    Debug::TraceContextBumper trcCtx{L"Stroika::Foundation::Execution::SignalHandlerRegistry::FirstPassSignalHandler_", L"signal = %s",
                                     SignalToName (signal).c_str ()};
#endif
#if qDoBacktraceOnFirstPassSignalHandler_ and qStroika_Foundation_Debug_DefaultTracingOn
    {
        wstring tmp{Debug::BackTrace ()};
        if (not tmp.empty ()) {
            DbgTrace (L"BackTrace: %s", tmp.c_str ());
        }
    }
#endif

    /*
     *  This is the signal handler: it may do only what SignalHandler::Type allows a direct one (in SignalHandlers.h). So only
     *  plain arrays, function pointers and lock-free atomics here - and sem_post (the safe handlers' wakeup, which keeps errno
     *  as it was), or on Windows SetEvent, and signal () for this signal. For instance with libstdc++ a debug-mode iterator, or
     *  (before libstdc++ 12) a shared_ptr atomic, takes one of a pool of mutexes - so used here, a signal arriving while its thread
     *  held that mutex would never return (#128).
     */
    Require (0 <= signal and signal < NSIG);
#if qStroika_Platform_Windows
    // The CRT set the signal to SIG_DFL before calling this: put back what SetSignalHandlers installed - this handler, unless a
    // SetSignalHandlers since changed it, so that is read after re-installing. Calling signal () for the signal being handled is
    // signal-safe ([support.signal] p4)
    (void)::signal (signal, FirstPassSignalHandler_);
    if (void (*installed) (int) = sInstalledForSignal_[signal].load (); installed != FirstPassSignalHandler_) {
        (void)::signal (signal, installed);
    }
#endif
    for (const atomic<void (*) (SignalID)>& slot : sThe.fDirectSignalHandlersCache_[signal]) {
        if (void (*h) (SignalID) = slot.load ()) {
            h (signal);
        }
    }
    ++SafeSignalsManager::sSignalHandlersUsingRep_;
    if (SafeSignalsManager::Rep_* rep = SafeSignalsManager::sTheRepForSignalHandler_.load ()) {
        rep->NotifyOfArrivalOfPossiblySafeSignal (signal);
    }
    --SafeSignalsManager::sSignalHandlersUsingRep_;
}
