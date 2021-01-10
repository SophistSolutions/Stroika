/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Thread_inl_
#define _Stroika_Foundation_Execution_Thread_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <atomic>
#include <list>

#include "WaitableEvent.h"

namespace Stroika::Foundation::Execution {

    namespace PRIVATE_ {
        enum class InterruptFlagState_ {
            eNone,
            eInterrupted,
            eAborted,

            Stroika_Define_Enum_Bounds (eNone, eAborted)
        };
        using InterruptFlagType_ = atomic<InterruptFlagState_>;
    }

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
        static void DoCreate (const shared_ptr<Rep_>* repSharedPtr);

    public:
        nonvirtual void Start ();

    public:
        nonvirtual IDType GetID () const;

    public:
        nonvirtual NativeHandleType GetNativeHandle ();

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
        // and is just called by Thread::Abort (). It sets (indirectly) the thread-local-storage aborted
        // flag for the target thread. And if called from an aborting thread, it may throw
        nonvirtual void NotifyOfInterruptionFromAnyThread_ (bool aborting);

    private:
        static void ThreadMain_ (const shared_ptr<Rep_>* thisThreadRep) noexcept;

    private:
#if qPlatform_POSIX
        static void InterruptionSignalHandler_ (SignalID signal) noexcept;
#elif qPlatform_Windows
        static void CALLBACK CalledInRepThreadAbortProc_ (ULONG_PTR lpParameter);
#endif

    private:
        using InterruptFlagState_ = PRIVATE_::InterruptFlagState_;
        using InterruptFlagType_  = PRIVATE_::InterruptFlagType_;

    private:
        function<void ()> fRunnable_;
        // We use a global variable (thread local) to store the abort flag. But we must access it from ANOTHER thread typically - using
        // a pointer. This is that pointer - so another thread can terminate/abort this thread.
        InterruptFlagType_*              fTLSInterruptFlag_{};   // regular interrupt, abort interrupt, or none
        mutable mutex                    fAccessSTDThreadMutex_; // rarely needed but to avoid small race as we shutdown thread, while we join in one thread and call GetNativeThread() in another
        thread                           fThread_;
        atomic<Status>                   fStatus_{Status::eNotYetRunning};
        WaitableEvent                    fRefCountBumpedEvent_;
        WaitableEvent                    fOK2StartEvent_;
        WaitableEvent                    fThreadDoneAndCanJoin_;
        wstring                          fThreadName_;
        exception_ptr                    fSavedException_;
        Synchronized<optional<Priority>> fInitialPriority_; // where we store priority before start
#if qPlatform_Windows
        bool fThrowInterruptExceptionInsideUserAPC_{false};
#endif

    private:
        friend class Ptr;
    };

    /*
     ********************************************************************************
     *********************************** Thread::Rep_ *******************************
     ********************************************************************************
     */
    inline void Thread::Ptr::Rep_::Start ()
    {
        fOK2StartEvent_.Set ();
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
        : fRep_ (rep)
    {
    }
    inline Thread::Ptr::Ptr (const Ptr& src)
        : fRep_ (src.fRep_)
    {
    }
    inline Thread::Ptr::Ptr (Ptr&& src) noexcept
        : fRep_ (move (src.fRep_))
    {
    }
    inline Thread::Ptr& Thread::Ptr::operator= (const Ptr& rhs)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{rhs};
        lock_guard<const AssertExternallySynchronizedLock>  critSec{*this};
        fRep_ = rhs.fRep_;
        return *this;
    }
    inline Thread::Ptr& Thread::Ptr::operator= (Ptr&& rhs) noexcept
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec1{rhs};
        lock_guard<const AssertExternallySynchronizedLock> critSec2{*this};
        fRep_ = move (rhs.fRep_);
        return *this;
    }
    inline Thread::IDType Thread::Ptr::GetID () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        if (fRep_ == nullptr)
            [[UNLIKELY_ATTR]]
            {
                return IDType{};
            }
        return fRep_->GetID ();
    }
    inline Thread::NativeHandleType Thread::Ptr::GetNativeHandle () const noexcept
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        if (fRep_ == nullptr)
            [[UNLIKELY_ATTR]]
            {
                return NativeHandleType{};
            }
        return fRep_->GetNativeHandle ();
    }
    inline void Thread::Ptr::reset () noexcept
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this};
        fRep_.reset ();
    }
    inline function<void ()> Thread::Ptr::GetFunction () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        if (fRep_ == nullptr)
            [[UNLIKELY_ATTR]]
            {
                return nullptr;
            }
        return fRep_->fRunnable_;
    }
#if __cpp_impl_three_way_comparison >= 201907
    inline bool Thread::Ptr::operator== (const Ptr& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        shared_lock<const AssertExternallySynchronizedLock> critSec2{rhs};
        return fRep_ == rhs.fRep_;
    }
    inline bool Thread::Ptr::operator== (nullptr_t) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        return fRep_ == nullptr;
    }
    inline strong_ordering Thread::Ptr::operator<=> (const Ptr& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        shared_lock<const AssertExternallySynchronizedLock> critSec2{rhs};
#if __cpp_lib_three_way_comparison < 201907
        return Common::ThreeWayCompare (fRep_, rhs.fRep_);
#else
        return fRep_ <=> rhs.fRep_;
#endif
    }
    inline strong_ordering Thread::Ptr::operator<=> (nullptr_t) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
#if __cpp_lib_three_way_comparison < 201907
        return Common::ThreeWayCompare (fRep_, nullptr);
#else
        return fRep_ <=> nullptr;
#endif
    }
#else
    inline bool Thread::Ptr::operator< (const Ptr& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fRep_ < rhs.fRep_;
    }
    inline bool Thread::Ptr::operator== (const Ptr& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        shared_lock<const AssertExternallySynchronizedLock> critSec2{rhs};
        return fRep_ == rhs.fRep_;
    }
    inline bool Thread::Ptr::operator== (nullptr_t) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        return fRep_ == nullptr;
    }
    inline bool Thread::Ptr::operator!= (const Ptr& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        shared_lock<const AssertExternallySynchronizedLock> critSec2{rhs};
        return fRep_ != rhs.fRep_;
    }
    inline bool Thread::Ptr::operator!= (nullptr_t) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        return fRep_ != nullptr;
    }
#endif
    inline Thread::Ptr::operator bool () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        return fRep_ != nullptr;
    }
#if qPlatform_Windows
    inline bool Thread::Ptr::ThrowInterruptExceptionInsideUserAPC () const noexcept
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        return fRep_ == nullptr ? false : fRep_->fThrowInterruptExceptionInsideUserAPC_;
    }
    inline bool Thread::Ptr::ThrowInterruptExceptionInsideUserAPC (optional<bool> throwInterruptExceptionInsideUserAPC)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec1{*this};
        bool                                               result = fRep_ == nullptr ? false : fRep_->fThrowInterruptExceptionInsideUserAPC_;
        if (throwInterruptExceptionInsideUserAPC) {
            RequireNotNull (fRep_);
            fRep_->fThrowInterruptExceptionInsideUserAPC_ = throwInterruptExceptionInsideUserAPC.value ();
        }
        return result;
    }
#endif
    inline Thread::Status Thread::Ptr::GetStatus () const noexcept
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        if (fRep_ == nullptr)
            [[UNLIKELY_ATTR]]
            {
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
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        WaitForDoneUntil (timeoutAt);
        ThrowIfDoneWithException ();
    }
    inline void Thread::Ptr::WaitForDone (Time::DurationSecondsType timeout) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        WaitForDoneUntil (timeout + Time::GetTickCount ());
    }
    inline void Thread::Ptr::AbortAndWaitForDone (Time::DurationSecondsType timeout) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        AbortAndWaitForDoneUntil (timeout + Time::GetTickCount ());
    }

    /*
     ********************************************************************************
     ******************************** Thread::CleanupPtr ****************************
     ********************************************************************************
     */
    inline Thread::CleanupPtr::CleanupPtr (AbortFlag abortFlag, Ptr threadPtr)
        : Ptr (threadPtr)
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
    inline Thread::Ptr Thread::New (const function<void ()>& fun2CallOnce, AutoStartFlag, const optional<Configuration>& configuration)
    {
        Ptr ptr = New (fun2CallOnce, nullopt, configuration);
        ptr.Start ();
        return ptr;
    }
    inline Thread::Ptr Thread::New (const function<void ()>& fun2CallOnce, AutoStartFlag, const optional<Characters::String>& name, const optional<Configuration>& configuration)
    {
        Ptr ptr = New (fun2CallOnce, name, configuration);
        ptr.Start ();
        return ptr;
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
     ********************* Thread::CheckForInterruption ***********************
     ********************************************************************************
     */
    template <unsigned int kEveryNTimes>
    void Thread::CheckForInterruption ()
    {
        // note that it is not important that this be protected/thread safe, since the value is just advisory/hint
        static unsigned int n = 0;
        if (++n % kEveryNTimes == kEveryNTimes - 1)
            [[UNLIKELY_ATTR]]
            {
                CheckForInterruption ();
            }
    }

}

namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Execution::Thread::Status> DefaultNames<Execution::Thread::Status>::k{
        EnumNames<Execution::Thread::Status>::BasicArrayInitializer{{
            {Execution::Thread::Status::eNull, L"Null"},
            {Execution::Thread::Status::eNotYetRunning, L"Not-Yet-Running"},
            {Execution::Thread::Status::eRunning, L"Running"},
            {Execution::Thread::Status::eAborting, L"Aborting"},
            {Execution::Thread::Status::eCompleted, L"Completed"},
        }}};
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Execution::Thread::Priority> DefaultNames<Execution::Thread::Priority>::k{
        EnumNames<Execution::Thread::Priority>::BasicArrayInitializer{{
            {Execution::Thread::Priority::eLowest, L"Lowest"},
            {Execution::Thread::Priority::eBelowNormal, L"Below-Normal"},
            {Execution::Thread::Priority::eNormal, L"Normal"},
            {Execution::Thread::Priority::eAboveNormal, L"Above-Normal"},
            {Execution::Thread::Priority::eHighest, L"Highest"},
        }}};
}

#endif /*_Stroika_Foundation_Execution_Thread_inl_*/
