/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Thread_inl_
#define _Stroika_Foundation_Execution_Thread_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <atomic>

#include "WaitableEvent.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ********************************* Thread::Ptr::Rep_ ****************************
     ********************************************************************************
     */
    /**
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    class Thread::Ptr::Rep_ {
    public:
        Rep_ (const function<void ()>& runnable, const optional<Configuration>& configuration);
        ~Rep_ ();

    public:
        nonvirtual IDType GetID () const;

    public:
        nonvirtual NativeHandleType GetNativeHandle ();

#if __cpp_lib_jthread >= 201911
    public:
        nonvirtual stop_token GetStopToken () const;
#endif

    public:
        nonvirtual Characters::String ToString () const;

    public:
        nonvirtual void ApplyThreadName2OSThreadObject ();

    public:
        nonvirtual void ApplyPriority (Priority priority);

    private:
        nonvirtual void Run_ ();

    private:
        // Called - typically from ANOTHER thread (but could  be this thread). By default this does nothing,
        // and is just called by Thread::Abort (). It sets (indirectly) the thread-local-storage interrupted
        // flag for the target thread. And if called from an aborting thread, it may throw
        nonvirtual void NotifyOfInterruptionFromAnyThread_ ();

    private:
        static void ThreadMain_ (const shared_ptr<Rep_> thisThreadRep) noexcept;

    private:
#if qPlatform_POSIX
        static void InterruptionSignalHandler_ (SignalID signal) noexcept;
#elif qPlatform_Windows
        static void CALLBACK CalledInRepThreadAbortProc_ (ULONG_PTR lpParameter);
#endif

    private:
        function<void ()> fRunnable_;
        atomic<bool>      fInterruptionState_{false}; // regular interrupt, abort interrupt, or none
        // @todo lose this mutex. We read fID from thread object, but sometimes call join. want to allow access to some attributes to read while doing a join..
        /// MAYBE reviseit - not sure thats safe...
        //     mutable mutex fAccessSTDThreadMutex_; // rarely needed but to avoid small race as we shutdown thread, while we join in one thread and call GetNativeThread() in another
#if __cpp_lib_jthread >= 201911
        stop_source fStopSource_;
        stop_token  fStopToken_; // initialized in Ptr::Start() before ThreadMain_ called
        jthread     fThread_;
#else
        thread               fThread_;
#endif
        atomic<Status>                   fStatus_{Status::eNotYetRunning};
        WaitableEvent                    fRefCountBumpedInsideThreadMainEvent_;
        WaitableEvent                    fStartReadyToTransitionToRunningEvent_;
        WaitableEvent                    fThreadDoneAndCanJoin_;
        wstring                          fThreadName_;
        Synchronized<exception_ptr>      fSavedException_;    // really no logical need for Syncrhonized<>, except when used from ToString() for debugging
        Synchronized<optional<Priority>> fInitialPriority_; // where we store priority before start
#if qPlatform_Windows
        bool fThrowInterruptExceptionInsideUserAPC_{false};
#endif

    private:
        friend class Ptr;
        friend void CheckForInterruption ();
    };

    /*
     ********************************************************************************
     *********************************** Thread::Rep_ *******************************
     ********************************************************************************
     */
#if __cpp_lib_jthread >= 201911
    inline stop_token Thread::Ptr::Rep_::GetStopToken () const
    {
        return this->fStopToken_;
    }
#endif
    inline Thread::IDType Thread::Ptr::Rep_::GetID () const
    {
        return fThread_.get_id ();
    }
    inline Thread::NativeHandleType Thread::Ptr::Rep_::GetNativeHandle ()
    {
        return fThread_.native_handle ();
    }

    /*
     ********************************************************************************
     ******************************** Thread::Ptr ***********************************
     ********************************************************************************
     */
    inline Thread::Ptr::Ptr (nullptr_t)
    {
    }
    inline Thread::Ptr::Ptr (const shared_ptr<Rep_>& rep)
        : fRep_{rep}
    {
    }
    inline Thread::Ptr::Ptr (const Ptr& src)
        : fRep_{src.fRep_}
    {
    }
    inline Thread::Ptr::Ptr (Ptr&& src) noexcept
        : fRep_{move (src.fRep_)}
    {
    }
    inline Thread::Ptr& Thread::Ptr::operator= (const Ptr& rhs)
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext  readLock1{rhs.fThisAssertExternallySynchronized_};
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fRep_ = rhs.fRep_;
        return *this;
    }
    inline Thread::Ptr& Thread::Ptr::operator= (Ptr&& rhs) noexcept
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareWriteContext1{rhs.fThisAssertExternallySynchronized_};
        Debug::AssertExternallySynchronizedMutex::WriteContext declareWriteContext2{fThisAssertExternallySynchronized_};
        fRep_ = move (rhs.fRep_);
        return *this;
    }
    inline Thread::IDType Thread::Ptr::GetID () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareReadContext{fThisAssertExternallySynchronized_};
        if (fRep_ == nullptr) [[unlikely]] {
            return IDType{};
        }
        return fRep_->GetID ();
    }
    inline Thread::NativeHandleType Thread::Ptr::GetNativeHandle () const noexcept
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareReadContext{fThisAssertExternallySynchronized_};
        if (fRep_ == nullptr) [[unlikely]] {
            return NativeHandleType{};
        }
        return fRep_->GetNativeHandle ();
    }
#if __cpp_lib_jthread >= 201911
    inline stop_token Thread::Ptr::GetStopToken () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareReadContext{fThisAssertExternallySynchronized_};
        RequireNotNull (fRep_);
        return fRep_->GetStopToken ();
    }
#endif
    inline void Thread::Ptr::reset () noexcept
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        fRep_.reset ();
    }
    inline function<void ()> Thread::Ptr::GetFunction () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareReadContext{fThisAssertExternallySynchronized_};
        if (fRep_ == nullptr) [[unlikely]] {
            return nullptr;
        }
        return fRep_->fRunnable_;
    }
    inline bool Thread::Ptr::operator== (const Ptr& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock1{fThisAssertExternallySynchronized_};
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock2{rhs.fThisAssertExternallySynchronized_};
        return fRep_ == rhs.fRep_;
    }
    inline bool Thread::Ptr::operator== (nullptr_t) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock1{fThisAssertExternallySynchronized_};
        return fRep_ == nullptr;
    }
    inline strong_ordering Thread::Ptr::operator<=> (const Ptr& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock1{fThisAssertExternallySynchronized_};
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock2{rhs.fThisAssertExternallySynchronized_};
#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy or qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
        return Common::compare_three_way_BWA{}(fRep_, rhs.fRep_);
#else
        return fRep_ <=> rhs.fRep_;
#endif
    }
    inline strong_ordering Thread::Ptr::operator<=> (nullptr_t) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock1{fThisAssertExternallySynchronized_};
#if qCompilerAndStdLib_stdlib_compare_three_way_present_but_Buggy or qCompilerAndStdLib_stdlib_compare_three_way_missing_Buggy
        return Common::compare_three_way_BWA{}(fRep_, nullptr);
#else
        return fRep_ <=> nullptr;
#endif
    }
    inline Thread::Ptr::operator bool () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareReadContext{fThisAssertExternallySynchronized_};
        return fRep_ != nullptr;
    }
#if qPlatform_Windows
    inline bool Thread::Ptr::ThrowInterruptExceptionInsideUserAPC () const noexcept
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareReadContext{fThisAssertExternallySynchronized_};
        return fRep_ == nullptr ? false : fRep_->fThrowInterruptExceptionInsideUserAPC_;
    }
    inline bool Thread::Ptr::ThrowInterruptExceptionInsideUserAPC (optional<bool> throwInterruptExceptionInsideUserAPC)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext critSec1{fThisAssertExternallySynchronized_};
        bool result = fRep_ == nullptr ? false : fRep_->fThrowInterruptExceptionInsideUserAPC_;
        if (throwInterruptExceptionInsideUserAPC) {
            RequireNotNull (fRep_);
            fRep_->fThrowInterruptExceptionInsideUserAPC_ = throwInterruptExceptionInsideUserAPC.value ();
        }
        return result;
    }
#endif
    inline Thread::Status Thread::Ptr::GetStatus () const noexcept
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareReadContext{fThisAssertExternallySynchronized_};
        if (fRep_ == nullptr) [[unlikely]] {
            return Status::eNull;
        }
        return GetStatus_ ();
    }
    inline void Thread::Ptr::Join (Time::DurationSecondsType timeout) const
    {
        JoinUntil (timeout + Time::GetTickCount ());
    }
    inline void Thread::Ptr::JoinUntil (Time::DurationSecondsType timeoutAt) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareReadContext{fThisAssertExternallySynchronized_};
        WaitForDoneUntil (timeoutAt);
        ThrowIfDoneWithException ();
    }
    inline void Thread::Ptr::WaitForDone (Time::DurationSecondsType timeout) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareReadContext{fThisAssertExternallySynchronized_};
        WaitForDoneUntil (timeout + Time::GetTickCount ());
    }
    inline void Thread::Ptr::AbortAndWaitForDone (Time::DurationSecondsType timeout) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareReadContext{fThisAssertExternallySynchronized_};
        AbortAndWaitForDoneUntil (timeout + Time::GetTickCount ());
    }

    /*
     ********************************************************************************
     ******************************** Thread::CleanupPtr ****************************
     ********************************************************************************
     */
    inline Thread::CleanupPtr::CleanupPtr (AbortFlag abortFlag, Ptr threadPtr)
        : Ptr{threadPtr}
        , fAbort_{abortFlag == AbortFlag::eAbortBeforeWaiting}
    {
    }
    inline Thread::CleanupPtr& Thread::CleanupPtr::operator= (const Ptr& rhs)
    {
        // preserve constructed with fAbort flag
        Ptr::operator= (rhs);
        return *this;
    }

    /*
     ********************************************************************************
     ************************************* Thread ***********************************
     ********************************************************************************
     */
    inline Thread::Ptr Thread::New (const function<void ()>& fun2CallOnce, const optional<Configuration>& configuration)
    {
        return New (fun2CallOnce, nullopt, configuration);
    }
    inline Thread::Ptr Thread::New (const function<void ()>& fun2CallOnce, const Characters::String& name, const optional<Configuration>& configuration)
    {
        return New (fun2CallOnce, optional<Characters::String>{name}, configuration);
    }
    inline auto Thread::New (const function<void ()>& fun2CallOnce, AutoStartFlag, const optional<Characters::String>& name,
                             const optional<Configuration>& configuration) -> Ptr
    {
        Ptr ptr = New (fun2CallOnce, name, configuration);
        ptr.Start ();
        return ptr;
    }
    inline auto Thread::New (const function<void ()>& fun2CallOnce, AutoStartFlag, const Characters::String& name,
                             const optional<Configuration>& configuration) -> Ptr
    {
        return New (fun2CallOnce, AutoStartFlag::eAutoStart, optional<Characters::String>{name}, configuration);
    }
    inline auto Thread::New (const function<void ()>& fun2CallOnce, AutoStartFlag, const optional<Configuration>& configuration) -> Ptr
    {
        return New (fun2CallOnce, AutoStartFlag::eAutoStart, nullopt, configuration);
    }
    inline void Thread::AbortAndWaitForDone (const Traversal::Iterable<Ptr>& threads, Time::DurationSecondsType timeout)
    {
        AbortAndWaitForDoneUntil (threads, timeout + Time::GetTickCount ());
    }
    inline void Thread::Start (const Traversal::Iterable<Ptr>& threads)
    {
        threads.Apply ([] (const Ptr& p) { p.Start (); });
    }
    inline void Thread::WaitForDone (const Traversal::Iterable<Ptr>& threads, Time::DurationSecondsType timeout)
    {
        WaitForDoneUntil (threads, timeout + Time::GetTickCount ());
    }

    /*
     ********************************************************************************
     ************************ Thread::GetCurrentThreadID ****************************
     ********************************************************************************
     */
    inline Thread::IDType Thread::GetCurrentThreadID () noexcept
    {
        return this_thread::get_id ();
    }

    /*
     ********************************************************************************
     ***************************** Thread::GetCurrent *******************************
     ********************************************************************************
     */
    inline Thread::Ptr Thread::GetCurrent ()
    {
        return Ptr{Ptr::sCurrentThreadRep_.lock ()};
    }

#if !qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy
#if __cpp_lib_jthread >= 201911
    /*
     ********************************************************************************
     ************************ Thread::GetCurrentThreadStopToken *********************
     ********************************************************************************
     */
    inline optional<stop_token> Thread::GetCurrentThreadStopToken ()
    {
        if (Ptr curThread = GetCurrent ()) {
            return curThread.GetStopToken ();
        }
        else {
            return nullopt;
        }
    }
#endif
#endif

#if !qCompilerAndStdLib_ThreadLocalInlineDupSymbol_Buggy
    /*
     ********************************************************************************
     ******************* Thread::IsCurrentThreadInterruptible ***********************
     ********************************************************************************
     */
    inline bool Thread::IsCurrentThreadInterruptible ()
    {
        return Ptr::sCurrentThreadRep_.lock () != nullptr;
    }
#endif

}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<Execution::Thread::Status> DefaultNames<Execution::Thread::Status>::k{EnumNames<Execution::Thread::Status>::BasicArrayInitializer{{
        {Execution::Thread::Status::eNull, L"Null"},
        {Execution::Thread::Status::eNotYetRunning, L"Not-Yet-Running"},
        {Execution::Thread::Status::eRunning, L"Running"},
        {Execution::Thread::Status::eAborting, L"Aborting"},
        {Execution::Thread::Status::eCompleted, L"Completed"},
    }}};
    template <>
    constexpr EnumNames<Execution::Thread::Priority> DefaultNames<Execution::Thread::Priority>::k{EnumNames<Execution::Thread::Priority>::BasicArrayInitializer{{
        {Execution::Thread::Priority::eLowest, L"Lowest"},
        {Execution::Thread::Priority::eBelowNormal, L"Below-Normal"},
        {Execution::Thread::Priority::eNormal, L"Normal"},
        {Execution::Thread::Priority::eAboveNormal, L"Above-Normal"},
        {Execution::Thread::Priority::eHighest, L"Highest"},
    }}};
}

#endif /*_Stroika_Foundation_Execution_Thread_inl_*/
