/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Characters/String_Constant.h"
#include    "../Containers/Mapping.h"
#include    "../Debug/BackTrace.h"
#include    "../Debug/Trace.h"
#include    "../Containers/Concrete/Queue_Array.h"

#include    "BlockingQueue.h"
#include    "Common.h"
#include    "Thread.h"

#include    "SignalHandlers.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::Memory;

using   Characters::String_Constant;
using   Containers::Mapping;
using   Containers::Queue;
using   Containers::Set;



// maybe useful while debugging signal code, but VERY unsafe
// and could easily be the source of bugs/deadlocks!
#ifndef qDoDbgTraceOnSignalHandlers_
#define qDoDbgTraceOnSignalHandlers_            0
#endif



// maybe useful while debugging signal code, but VERY unsafe
// and could easily be the source of bugs/deadlocks!
#ifndef qDoBacktraceOnFirstPassSignalHandler_
#define qDoBacktraceOnFirstPassSignalHandler_    0
#endif











namespace   {
    bool    IsSigIgnore_ (const Set<SignalHandler>& sigSet)
    {
        return sigSet.size () == 1 and sigSet.Contains (SignalHandlerRegistry::kIGNORED);
    }
}






/*
 ********************************************************************************
 *********** Execution::SignalHandlerRegistry::SafeSignalsManager ***************
 ********************************************************************************
 */
namespace {
    Queue<SignalID> mkSafeSignalsManagerQ_ ()
    {
        Containers::Concrete::Queue_Array<SignalID> signalQ;
        signalQ.SetCapacity (25);  // quite arbitrary - @todo make configurable somehow...
        return signalQ;
    }
}

DISABLE_COMPILER_MSC_WARNING_START(4351)
struct SignalHandlerRegistry::SafeSignalsManager::Rep_ {
    Rep_ ()
        : fIncomingSafeSignals_ (mkSafeSignalsManagerQ_ ())
    {
        fBlockingQueuePusherThread_ = Thread {
            [this] ()
            {
                // This is a safe context
                Debug::TraceContextBumper trcCtx ("Stroika::Foundation::Execution::Signals::{}::fBlockingQueueDelegatorThread_");
                while (true) {
                    Debug::TraceContextBumper trcCtx1 ("Waiting for next safe signal");
                    SignalID    i   =   fIncomingSafeSignals_.RemoveHead ();
                    Debug::TraceContextBumper trcCtx2 ("Invoking SAFE signal handlers");
                    DbgTrace (L"(signal: %s)", SignalToName (i).c_str ());
                    for (SignalHandler sh : fHandlers_.LookupValue (i)) {
                        Assert (sh.GetType () == SignalHandler::Type::eSafe);
                        IgnoreExceptionsExceptThreadAbortForCall (sh (i));
                    }
                }
            },
            Thread::eAutoStart,
            String_Constant { L"Signal Handler Safe Execution Thread" }
        };
    }
    ~Rep_ ()
    {
        Debug::TraceContextBumper trcCtx ("Stroika::Foundation::Execution::SignalHandlerRegistry::SafeSignalsManager::Rep_::~Rep_");
        Thread::SuppressInterruptionInContext  suppressInterruption;
        fBlockingQueuePusherThread_.AbortAndWaitForDone ();
    }
    Containers::Mapping<SignalID, Containers::Set<SignalHandler>>   fHandlers_;
    BlockingQueue<SignalID>                                         fIncomingSafeSignals_;
    Thread                                                          fBlockingQueuePusherThread_;
    bool                                                            fSafeSignalHanldersAvailable_[NSIG] {};         // if true post to blocking q

    void    PopulateSafeSignalHandlersCache_ (SignalID signal)
    {
        Require (0 <= signal and signal < static_cast<SignalID> (NEltsOf (fSafeSignalHanldersAvailable_)));
        fSafeSignalHanldersAvailable_[signal] = fHandlers_.Lookup (signal).IsPresent ();
    }

    void    NotifyOfArrivalOfPossiblySafeSignal (SignalID signal)
    {
        Require (0 <= signal and signal < static_cast<SignalID> (NEltsOf (fSafeSignalHanldersAvailable_)));
        // harmless - but pointless - to add signals that will be ignored
        if (fSafeSignalHanldersAvailable_[signal]) {
            fIncomingSafeSignals_.AddTail (signal);
        }
    }
};
DISABLE_COMPILER_MSC_WARNING_END(4351)






/*
 ********************************************************************************
 *********** Execution::SignalHandlerRegistry::SafeSignalsManager ***************
 ********************************************************************************
 */
shared_ptr<SignalHandlerRegistry::SafeSignalsManager::Rep_>  SignalHandlerRegistry::SafeSignalsManager::sTheRep_;

SignalHandlerRegistry::SafeSignalsManager::SafeSignalsManager ()
{
    Debug::TraceContextBumper trcCtx ("Stroika::Foundation::Execution::SignalHandlerRegistry::CTOR");
    Require (sTheRep_ == nullptr);
    sTheRep_ = make_shared<SignalHandlerRegistry::SafeSignalsManager::Rep_> ();
}

SignalHandlerRegistry::SafeSignalsManager::~SafeSignalsManager ()
{
    Debug::TraceContextBumper trcCtx ("Stroika::Foundation::Execution::SignalHandlerRegistry::DTOR");
    shared_ptr<SignalHandlerRegistry::SafeSignalsManager::Rep_> tmp = SignalHandlerRegistry::SafeSignalsManager::sTheRep_;
    sTheRep_.reset ();
    // avoid slight race - after reset above - could still be processing a signal (holding refcount above zero).
    // this check and abort/wait guaratess at least no more thread running (though other stuff - like signal handling - could
    // still be running)
    if (tmp != nullptr) {
        tmp->fBlockingQueuePusherThread_.AbortAndWaitForDone ();
    }
}






/*
 ********************************************************************************
 ******************** Execution::SignalHandlerRegistry **************************
 ********************************************************************************
 */
const   SignalHandler   SignalHandlerRegistry::kIGNORED =   SignalHandler (SIG_IGN, SignalHandler::Type::eDirect);

SignalHandlerRegistry&  SignalHandlerRegistry::Get ()
{
    /*
     * Safe - even thread safe - ASSUMING never called after end of main. Then results undefined.
     */
    shared_ptr<SignalHandlerRegistry>   sp = _Stroika_Foundation_ExecutionSignalHandlers_ModuleData_.Actual ().fTheRegistry;
    if (sp == nullptr) {
        auto    critSec { make_unique_lock (_Stroika_Foundation_ExecutionSignalHandlers_ModuleData_.Actual ().fMutex) };
        sp = _Stroika_Foundation_ExecutionSignalHandlers_ModuleData_.Actual ().fTheRegistry;    // avoid race, and avoid locking unless needed
        if (sp == nullptr) {
            sp.reset (new SignalHandlerRegistry ());
            Assert (_Stroika_Foundation_ExecutionSignalHandlers_ModuleData_.Actual ().fTheRegistry == nullptr);
            _Stroika_Foundation_ExecutionSignalHandlers_ModuleData_.Actual ().fTheRegistry = sp;
        }
    }
    Assert (sp != nullptr);
    return *sp;
}

SignalHandlerRegistry::SignalHandlerRegistry ()
    : fDirectHandlers_ ()
{
#if     qDebug
    static  int nConstructed = 0;
    nConstructed++;
    Assert (nConstructed == 1);
#endif
    Debug::TraceContextBumper trcCtx ("Stroika::Foundation::Execution::SignalHandlerRegistry::CTOR");
}

SignalHandlerRegistry::~SignalHandlerRegistry ()
{
    Debug::TraceContextBumper trcCtx ("Stroika::Foundation::Execution::SignalHandlerRegistry::DTOR");
    Assert (SafeSignalsManager::sTheRep_ == nullptr);  // must be cleared first
}

Set<SignalID>   SignalHandlerRegistry::GetHandledSignals () const
{
    // @todo redo using Mapping<>::Keys () when implemented...
    Set<SignalID>   result;
    for (auto i : fDirectHandlers_) {
        result.Add (i.fKey);
    }
    shared_ptr<SafeSignalsManager::Rep_> tmp = SafeSignalsManager::sTheRep_;
    if (tmp != nullptr) {
        for (auto i : tmp->fHandlers_) {
            result.Add (i.fKey);
        }
    }
    return result;
}

Set<SignalHandler>  SignalHandlerRegistry::GetSignalHandlers (SignalID signal) const
{
    Set<SignalHandler>  result  =   fDirectHandlers_.LookupValue (signal);
    shared_ptr<SafeSignalsManager::Rep_> tmp = SafeSignalsManager::sTheRep_;
    if (tmp != nullptr) {
        result += tmp->fHandlers_.LookupValue (signal);
    }
    return result;
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalID signal)
{
    SetSignalHandlers (signal, Set<SignalHandler> ());
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalID signal, SignalHandler handler)
{
    SetSignalHandlers (signal, Set<SignalHandler> ({handler}));
}

void    SignalHandlerRegistry::SetSignalHandlers (SignalID signal, const Set<SignalHandler>& handlers)
{
    Debug::TraceContextBumper trcCtx ("Stroika::Foundation::Execution::SignalHandlerRegistry::{}::SetSignalHandlers");
    DbgTrace (L"(signal = %s, handlers.size () = %d, ....)", SignalToName (signal).c_str (), handlers.size ());

    shared_ptr<SignalHandlerRegistry::SafeSignalsManager::Rep_> tmp = SignalHandlerRegistry::SafeSignalsManager::sTheRep_;

    auto sigSetHandler = [] (SignalID signal) {
#if     qPlatform_POSIX
        struct  sigaction sa {};
        sa.sa_handler = FirstPassSignalHandler_;
        Verify (::sigemptyset (&sa.sa_mask) == 0);
        sa.sa_flags = 0; // important NOT to set SA_RESTART for interrupt() - but maybe for others helpful - maybe add option?
        Verify (::sigaction (signal, &sa, nullptr) == 0);
#else
        Verify (::signal (signal, FirstPassSignalHandler_) != SIG_ERR);
#endif
        DbgTrace (L"DID ::signal (%s, FirstPassSignalHandler_)", SignalToName (signal).c_str ());
    };
    auto sigSetDefault = [] (SignalID signal) {
#if     qPlatform_POSIX
        struct  sigaction sa {};
        sa.sa_handler = SIG_DFL;
        Verify (::sigemptyset (&sa.sa_mask) == 0);
        sa.sa_flags = 0; // important NOT to set SA_RESTART for interrupt() - but maybe for others helpful - maybe add option?
        Verify (::sigaction (signal, &sa, nullptr) == 0);
#else
        Verify (::signal (signal, SIG_DFL) != SIG_ERR);
#endif
        DbgTrace (L"DID ::signal (%s, SIG_DFL)", SignalToName (signal).c_str ());
    };
    auto sigSetIgnore = [] (SignalID signal) {
#if     qPlatform_POSIX
        struct  sigaction sa {};
        sa.sa_handler = SIG_IGN;
        Verify (::sigemptyset (&sa.sa_mask) == 0);
        sa.sa_flags = 0; // important NOT to set SA_RESTART for interrupt() - but maybe for others helpful - maybe add option?
        Verify (::sigaction (signal, &sa, nullptr) == 0);
#else
        Verify (::signal (signal, SIG_IGN) != SIG_ERR);
#endif
        DbgTrace (L"DID ::signal (%s, SIG_IGN)", SignalToName (signal).c_str ());
    };
    if (handlers.empty ()) {
        /*
         *  No handlers means use default.
         */
        fDirectHandlers_.Remove (signal);
        PopulateDirectSignalHandlersCache_ (signal);
        if (tmp != nullptr) {
            tmp->fHandlers_.Remove (signal);
            tmp->PopulateSafeSignalHandlersCache_ (signal);
        }
        sigSetDefault (signal);
    }
    else if (IsSigIgnore_ (handlers)) {
        Assert (handlers.size () == 1);
        fDirectHandlers_.Add (signal, handlers);
        PopulateDirectSignalHandlersCache_ (signal);
        if (tmp != nullptr) {
            tmp->fHandlers_.Remove (signal);
            tmp->PopulateSafeSignalHandlersCache_ (signal);
        }
        sigSetIgnore (signal);
    }
    else {
        Set<SignalHandler>  directHandlers;
        Set<SignalHandler>  safeHandlers;
        handlers.Apply ([&directHandlers, &safeHandlers] (SignalHandler si) {
            switch (si.GetType ()) {
                case SignalHandler::Type::eDirect: {
                        directHandlers.Add (si);
                    }
                    break;
                case SignalHandler::Type::eSafe: {
                        safeHandlers.Add (si);
                    }
                    break;
            }
        });
        Assert (directHandlers.size () + safeHandlers.size () == handlers.size ());
        fDirectHandlers_.Add (signal, directHandlers);
        PopulateDirectSignalHandlersCache_ (signal);
        if (not safeHandlers.empty ()) {
            // To use safe signal handlers, you must have a SignalHandlerRegistry::SafeSignalsManager
            // defined first. It is recommended that you define an instance of
            // SignalHandlerRegistry::SafeSignalsManager handler; should be defined in main ()
            Require (SafeSignalsManager::sTheRep_ != nullptr);
        }
        if (tmp != nullptr) {
            if (safeHandlers.empty ()) {
                tmp->fHandlers_.Remove (signal);
                tmp->PopulateSafeSignalHandlersCache_ (signal);
            }
            else {
                tmp->fHandlers_.Add (signal, safeHandlers);
                tmp->PopulateSafeSignalHandlersCache_ (signal);
            }
        }
        sigSetHandler (signal);
    }
}

void    SignalHandlerRegistry::AddSignalHandler (SignalID signal, SignalHandler handler)
{
    Set<SignalHandler>  s   =   GetSignalHandlers (signal);
    s.Add (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::RemoveSignalHandler (SignalID signal, SignalHandler handler)
{
    Set<SignalHandler>  s   =   GetSignalHandlers (signal);
    Require (s.Contains (handler));
    s.Remove (handler);
    SetSignalHandlers (signal, s);
}

void    SignalHandlerRegistry::DefaultCrashSignalHandler (SignalID signal)
{
    DbgTrace (L"Serious Signal Error trapped: %s ... Aborting", SignalToName (signal).c_str ());
    abort ();
}

Containers::Set<SignalID>   SignalHandlerRegistry::GetStandardCrashSignals ()
{
    Containers::Set<SignalID>   results;
    results.Add (SIGABRT);
    results.Add (SIGILL);
    results.Add (SIGFPE);
    results.Add (SIGSEGV);
#if     qPlatform_POSIX
    results.Add (SIGSYS);
    results.Add (SIGBUS);
#endif
    return results;
}

Containers::Set<SignalID>    SignalHandlerRegistry::GetStandardTerminationSignals ()
{
    Containers::Set<SignalID>   results;
    results.Add (SIGABRT);
    results.Add (SIGINT);
    results.Add (SIGTERM);
    results.Add (SIGILL);
    results.Add (SIGFPE);
    results.Add (SIGSEGV);
#if     qPlatform_POSIX
    results.Add (SIGHUP);
    results.Add (SIGQUIT);
    results.Add (SIGSYS);
    results.Add (SIGBUS);
    results.Add (SIGPIPE);
    results.Add (SIGXCPU);
    results.Add (SIGXFSZ);
#endif
    return results;
}

void    SignalHandlerRegistry::SetStandardCrashHandlerSignals (SignalHandler handler, const Containers::Set<SignalID>& forSignals)
{
    for (SignalID s : forSignals) {
        if (s != SIGABRT) {
            SetSignalHandlers (s, handler);
        }
    }
}

void    SignalHandlerRegistry::FirstPassSignalHandler_ (SignalID signal)
{
    /*
     *  Important example / stack backtrace to bear in mind:
     *
     *
                #0  __lll_lock_wait_private () at ../sysdeps/unix/sysv/linux/x86_64/lowlevellock.S:95
                #1  0x00007f4edcd924fc in __GI___libc_malloc (bytes=139975802748960) at malloc.c:2891
                #2  0x0000000000540168 in operator new(unsigned long) ()
                #3  0x000000000042581e in __gnu_cxx::new_allocator<std::_Rb_tree_node<std::thread::id> >::allocate (this=0x7f4eb7ffd380, __n=1) at /usr/include/c++/4.9/ext/new_allocator.h:104
                #4  0x0000000000423e54 in std::allocator_traits<std::allocator<std::_Rb_tree_node<std::thread::id> > >::allocate (__a=..., __n=1) at /usr/include/c++/4.9/bits/alloc_traits.h:357
                #5  0x000000000042161b in std::_Rb_tree<std::thread::id, std::thread::id, std::_Identity<std::thread::id>, std::less<std::thread::id>, std::allocator<std::thread::id> >::_M_get_node (
                    this=0x7f4eb7ffd380) at /usr/include/c++/4.9/bits/stl_tree.h:385
                #6  0x000000000041eae1 in std::_Rb_tree<std::thread::id, std::thread::id, std::_Identity<std::thread::id>, std::less<std::thread::id>, std::allocator<std::thread::id> >::_M_create_node<std::thread::id>(std::thread::id&&) (this=0x7f4eb7ffd380) at /usr/include/c++/4.9/bits/stl_tree.h:417
                #7  0x000000000041c348 in std::_Rb_tree<std::thread::id, std::thread::id, std::_Identity<std::thread::id>, std::less<std::thread::id>, std::allocator<std::thread::id> >::_M_insert_<std::thread::id>(std::_Rb_tree_node_base*, std::_Rb_tree_node_base*, std::thread::id&&) (this=0x7f4eb7ffd380, __x=0x0, __p=0x7f4eb7ffd388,
                    __v=<unknown type in /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/gcc-4.9-debug-no-TPC/Test35, CU 0x0, DIE 0x5e560>) at /usr/include/c++/4.9/bits/stl_tree.h:1143
                #8  0x000000000041858f in std::_Rb_tree<std::thread::id, std::thread::id, std::_Identity<std::thread::id>, std::less<std::thread::id>, std::allocator<std::thread::id> >::_M_insert_equal<std::thread::id>(std::thread::id&&) (this=0x7f4eb7ffd380, __v=<unknown type in /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/gcc-4.9-debug-no-TPC/Test35, CU 0x0, DIE 0x5944e>)
                    at /usr/include/c++/4.9/bits/stl_tree.h:1523
                #9  0x0000000000414a37 in std::__cxx1998::multiset<std::thread::id, std::less<std::thread::id>, std::allocator<std::thread::id> >::insert(std::thread::id&&) (this=0x7f4eb7ffd380,
                    __x=<unknown type in /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/gcc-4.9-debug-no-TPC/Test35, CU 0x0, DIE 0x54234>) at /usr/include/c++/4.9/bits/stl_multiset.h:498
                #10 0x0000000000412303 in std::__debug::multiset<std::thread::id, std::less<std::thread::id>, std::allocator<std::thread::id> >::insert(std::thread::id&&) (this=0x7f4eb7ffd380,
                    __x=<unknown type in /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/gcc-4.9-debug-no-TPC/Test35, CU 0x0, DIE 0x507d4>) at /usr/include/c++/4.9/debug/multiset.h:257
                #11 0x0000000000410f61 in Stroika::Foundation::Debug::AssertExternallySynchronizedLock::lock_shared (this=0x7f4eb7ffd370)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Containers/../Memory/../Debug/AssertExternallySynchronizedLock.inl:92
                #12 0x00000000004157d8 in std::shared_lock<Stroika::Foundation::Debug::AssertExternallySynchronizedLock const>::shared_lock (this=0x7f4eb7ffd270, m=...)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Configuration/Private/Defaults_CompilerAndStdLib_.h:2123
                #13 0x000000000048ce64 in Stroika::Foundation::Traversal::Iterable<Stroika::Foundation::Execution::SignalHandler>::_SafeReadRepAccessor<Stroika::Foundation::Containers::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::_IRep>::_SafeReadRepAccessor (this=0x7f4eb7ffd270, it=0x7f4eb7ffd370)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/../Traversal/Iterable.inl:88
                #14 0x000000000048dd70 in Stroika::Foundation::Containers::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::_AssertRepValidType (this=0x7f4eb7ffd370) at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Set.inl:282
                #15 0x0000000000496803 in Stroika::Foundation::Containers::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::Set(Stroika::Foundation::Memory::SharedPtr<Stroika::Foundation::Containers::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::_IRep>&&) (this=0x7f4eb7ffd370, src=<unknown type in /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/gcc-4.9-debug-no-TPC/Test35, CU 0x267e27, DIE 0x2d5751>)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Set.inl:67
                #16 0x0000000000494ff0 in Stroika::Foundation::Containers::Concrete::Set_stdset<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::Concrete::Set_stdset_DefaultTraits<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> >, Stroika::Foundation::Common::ComparerWithWellOrder<Stroika::Foundation::Execution::SignalHandler> > >::Set_stdset (this=0x7f4eb7ffd370)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Factory/../Concrete/Set_stdset.inl:244
                #17 0x0000000000493c39 in Stroika::Foundation::Containers::Concrete::Set_Factory<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::Default_SFINAE_<Stroika::Foundation::Execution::SignalHandler>(Stroika::Foundation::Execution::SignalHandler*, std::enable_if<Stroika::Foundation::Configuration::has_lt<Stroika::Foundation::Execution::SignalHandler>::value&&std::is_same<Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > >, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, std::conditional<Stroika::Foundation::Configuration::has_eq<Stroika::Foundation::Execution::SignalHandler>::value&&std::is_convertible<Stroika::Foundation::Configuration::Private_::eq_result_impl<Stroika::Foundation::Execution::SignalHandler>::type, bool>::value, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler>, std::conditional<Stroika::Foundation::Configuration::has_lt<Stroika::Foundation::Execution::SignalHandler>::value&&std::is_convertible<Stroika::Foundation::Configuration::Private_::lt_result_impl<Stroika::Foundation::Execution::SignalHandler>::type, bool>::value, Stroika::Foundation::Common::ComparerWithWellOrder<Stroika::Foundation::Execution::SignalHandler>, std::shared_ptr<int> >::type>::type> > >::value, void>::type*) ()
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Factory/Set_Factory.inl:63
                #18 0x0000000000491336 in Stroika::Foundation::Containers::Concrete::Set_Factory<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::Default_ () at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Factory/Set_Factory.inl:57
                #19 0x000000000048dbf1 in Stroika::Foundation::Containers::Concrete::Set_Factory<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::mk () at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Factory/Set_Factory.inl:44
                #20 0x000000000048abbf in Stroika::Foundation::Containers::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Containers::DefaultTraits::Set<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::DefaultEqualsComparer<Stroika::Foundation::Execution::SignalHandler, Stroika::Foundation::Common::ComparerWithEquals<Stroika::Foundation::Execution::SignalHandler> > > >::Set (this=0x7f4eb7ffd5e0) at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/Concrete/../../Containers/Set.inl:23
                #21 0x0000000000488de6 in Stroika::Foundation::Execution::SignalHandlerRegistry::FirstPassSignalHandler_ (signal=12)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/SignalHandlers.cpp:422
                #22 <signal handler called>
                #23 _int_malloc (av=av@entry=0x7f4ea8000020, bytes=bytes@entry=8160) at malloc.c:3769
                #24 0x00007f4edcd9250e in __GI___libc_malloc (bytes=8160) at malloc.c:2895
                #25 0x0000000000540168 in operator new(unsigned long) ()
                #26 0x000000000043b180 in __gnu_cxx::new_allocator<char>::allocate (this=0x7f4eb7ffdf3f, __n=8160) at /usr/include/c++/4.9/ext/new_allocator.h:104
                #27 0x000000000043b023 in std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >::_Rep::_S_create (__capacity=2033, __old_capacity=816, __alloc=...)
                    at /usr/include/c++/4.9/bits/basic_string.tcc:607
                #28 0x000000000044236e in std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >::_Rep::_M_clone (this=0x7f4ea8002450, __alloc=..., __res=51)
                    at /usr/include/c++/4.9/bits/basic_string.tcc:629
                #29 0x0000000000443e42 in std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >::reserve (this=0x7f4eb7ffe0c0, __res=867) at /usr/include/c++/4.9/bits/basic_string.tcc:510
                #30 0x0000000000480971 in std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >::append (this=0x7f4eb7ffe0c0, __str=...) at /usr/include/c++/4.9/bits/basic_string.tcc:332
                #31 0x0000000000527955 in std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >::operator+= (this=0x7f4eb7ffe0c0, __str=...) at /usr/include/c++/4.9/bits/basic_string.h:950
                #32 0x0000000000527342 in Stroika::Foundation::Debug::BackTrace (maxFrames=4294967295) at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Debug/BackTrace.cpp:50
                #33 0x0000000000485f61 in Stroika::Foundation::Execution::Private_::GetBT_ws () at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Exceptions.cpp:40
                #34 0x0000000000485e6e in Stroika::Foundation::Execution::Private_::GetBT_s () at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Exceptions.cpp:30
                #35 0x00000000004c56a8 in Stroika::Foundation::Execution::Throw<Stroika::Foundation::Execution::Thread::AbortException> (e2Throw=...)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Characters/../Memory/../Execution/Exceptions.inl:40
                #36 0x00000000004c2afc in Stroika::Foundation::Execution::CheckForThreadInterruption () at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Thread.cpp:993
                #37 0x00000000004f036c in Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntilQuietly (this=0x7ffe1997bf80, timeoutAt=1.7976931348623157e+308)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/WaitableEvent.cpp:83
                #38 0x00000000004f02bd in Stroika::Foundation::Execution::WaitableEvent::WE_::WaitUntil (this=0x7ffe1997bf80, timeoutAt=1.7976931348623157e+308)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/WaitableEvent.cpp:52
                #39 0x000000000041169b in Stroika::Foundation::Execution::WaitableEvent::Wait (this=0x7ffe1997bf80, timeout=1.7976931348623157e+308)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/WaitableEvent.inl:100
                #40 0x00000000004d4578 in Stroika::Foundation::Execution::ThreadPool::WaitForNextTask_(Stroika::Foundation::Execution::Function<void ()>*) (this=0x7ffe1997beb0, result=0x1841390)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/ThreadPool.cpp:444
                #41 0x00000000004d5288 in Stroika::Foundation::Execution::ThreadPool::MyRunnable_::Run (this=0x1841350)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/ThreadPool.cpp:57
                #42 0x00000000004d45db in Stroika::Foundation::Execution::ThreadPool::<lambda()>::operator()(void) const (__closure=0x1840ee0)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/ThreadPool.cpp:454
                #43 0x00000000004d4a14 in std::_Function_handler<void(), Stroika::Foundation::Execution::ThreadPool::mkThread_()::<lambda()> >::_M_invoke(const std::_Any_data &) (__functor=...)
                    at /usr/include/c++/4.9/functional:2039
                #44 0x000000000041298a in std::function<void ()>::operator()() const (this=0x18413d0) at /usr/include/c++/4.9/functional:2439
                #45 0x00000000004c4c43 in Stroika::Foundation::Execution::Function<void ()>::operator()<>() const (this=0x1841410)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/../Execution/Function.inl:48
                #46 0x00000000004c06b6 in Stroika::Foundation::Execution::Thread::Rep_::Run_ (this=0x1841410) at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Thread.cpp:317
                #47 0x00000000004c0c2f in Stroika::Foundation::Execution::Thread::Rep_::ThreadMain_ (thisThreadRep=0x7ffe1997b820)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Thread.cpp:425
                #48 0x00000000004c03ee in Stroika::Foundation::Execution::Thread::Rep_::<lambda()>::operator()(void) const (__closure=0x1840c68)
                    at /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Library/Sources/Stroika/Foundation/Execution/Thread.cpp:286
                #49 0x00000000004c420c in std::_Bind_simple<Stroika::Foundation::Execution::Thread::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Rep_>*)::<lambda()>()>::_M_invoke<>(std::_Index_tuple<>) (this=0x1840c68) at /usr/include/c++/4.9/functional:1700
                #50 0x00000000004c413e in std::_Bind_simple<Stroika::Foundation::Execution::Thread::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Rep_>*)::<lambda()>()>::operator()(void) (
                    this=0x1840c68) at /usr/include/c++/4.9/functional:1688
                #51 0x00000000004c40ac in std::thread::_Impl<std::_Bind_simple<Stroika::Foundation::Execution::Thread::Rep_::DoCreate(std::shared_ptr<Stroika::Foundation::Execution::Thread::Rep_>*)::<lambda()>()> >::_M_run(void) (this=0x1840c50) at /usr/include/c++/4.9/thread:115
                #52 0x0000000000589630 in execute_native_thread_routine ()
                #53 0x00007f4edd8056aa in start_thread (arg=0x7f4eb7fff700) at pthread_create.c:333
                #54 0x00007f4edce14eed in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:109

        *ANALYSIS OF ABOVE:
        *       At frame 22 (#22 <signal handler called>) - we have AT LEAST the malloc lock held (we could have more in principle)
        *       We receive a signal
        *       Deeper down, at frame 11 (#11 0x0000000000410f61) we acquire an unrelated lock, and AssertExternallySynchronizedLock
        *       which does more memory allocations.
        *       deadlocking at frame 0 (#0  __lll_lock_wait_private)
        *
        *       BUT - worse than deadlocking this thread (thats bad enuf) - we acquired a lock on AssertExternallySynchronizedLock
        *       which we will never give up, and kill any other thread doing much of anything.
        *
        *
        *   SUMAMRY:
        *       >   THIS CODE - CAN NEVER SAFELY CALL MALLOC!!!!
        */
#if     qDoDbgTraceOnSignalHandlers_
    Debug::TraceContextBumper trcCtx ("Stroika::Foundation::Execution::SignalHandlerRegistry::FirstPassSignalHandler_");
    DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
#endif
#if     qDoBacktraceOnFirstPassSignalHandler_
    DbgTrace (L"BT=%s", Debug::BackTrace ().c_str ());
#endif

    /*
     *  This (SignalHandlerRegistry::Get () SHOULD be safe. It can allocate memory, but only the first time
     *  called, and this cannot be our first call.
     */
    SignalHandlerRegistry&  SHR =   Get ();

#if 1
    {
        /*
         *  Pretty sure this all allocates no memory, so should be safe/lock free
         */
        const   vector<SignalHandler>*  shs = SHR.PeekAtSignalHandlersForSignal_ (signal);
        auto    cleanup { mkFinally ([signal, &SHR] () { SHR.ReleaseSignalHandlersForSignalLock_ (signal); }) };
        //Execution::FinallyT<> cleanup { [signal, &SHR] () { SHR.ReleaseSignalHandlersForSignalLock_ (signal); } };
        for (auto shi = shs->begin (); shi != shs->end (); ++shi) {
            (*shi) (signal);
        }
    }
#else
    {
        // Copy out of Stroika-based containers, because these may throw thread-abort exceptions.
        // The Set<> and Mapping<> code I'm thinking of here. Just use stl code (stuff that wont throw abort exceptions)
        vector<SignalHandler>   shs;
        {
            Thread::SuppressInterruptionInContext  suppressContext;
            for (SignalHandler sh : SHR.fDirectHandlers_.LookupValue (signal)) {
                shs.push_back (sh);
            }
        }
        for (SignalHandler sh : shs) {
            sh (signal);
        }
    }
#endif

    shared_ptr<SignalHandlerRegistry::SafeSignalsManager::Rep_> tmp = SignalHandlerRegistry::SafeSignalsManager::sTheRep_;
    if (tmp != nullptr) {
        /// @todo STILL VERY VERY UNSAFE IN SIGNAL HANLDER CALLBACK
        tmp->NotifyOfArrivalOfPossiblySafeSignal (signal);
    }
}
