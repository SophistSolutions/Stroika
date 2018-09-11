/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
     ********************************* Thread::Rep_ *********************************
     ********************************************************************************
     */
    /**
     *  \note   \em Thread-Safety   <a href="thread_safety.html#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     */
    class Thread::Rep_ {
    public:
        Rep_ (const Function<void()>& runnable, const optional<Configuration>& configuration);
        ~Rep_ ();

    public:
        static void DoCreate (const shared_ptr<Rep_>* repSharedPtr);

    public:
        nonvirtual void Start ();

    public:
        nonvirtual Thread::IDType GetID () const;

    public:
        nonvirtual Thread::NativeHandleType GetNativeHandle ();

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
        static void InterruptionSignalHandler_ (SignalID signal);
#elif qPlatform_Windows
        static void CALLBACK CalledInRepThreadAbortProc_ (ULONG_PTR lpParameter);
#endif

    private:
        using InterruptFlagState_ = PRIVATE_::InterruptFlagState_;
        using InterruptFlagType_  = PRIVATE_::InterruptFlagType_;

    private:
        Function<void()> fRunnable_;
        // We use a global variable (thread local) to store the abort flag. But we must access it from ANOTHER thread typically - using
        // a pointer. This is that pointer - so another thread can terminate/abort this thread.
        InterruptFlagType_*              fTLSInterruptFlag_{};   // regular interrupt, abort interrupt, or none
        mutable mutex                    fAccessSTDThreadMutex_; // rarely needed but to avoid small race as we shutdown thread, while we join in one thread and call GetNativeThread() in another
        thread                           fThread_;
        atomic<Status>                   fStatus_;
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
        friend class Thread;
    };

    /*
     ********************************************************************************
     *********************************** Thread::Rep_ *******************************
     ********************************************************************************
     */
    inline void Thread::Rep_::Start ()
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
                return Thread::IDType{};
            }
        return fRep_->GetID ();
    }
    inline Thread::NativeHandleType Thread::Ptr::GetNativeHandle () const noexcept
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        if (fRep_ == nullptr)
            [[UNLIKELY_ATTR]]
            {
                return Thread::NativeHandleType{};
            }
        return fRep_->GetNativeHandle ();
    }
    inline void Thread::Ptr::reset () noexcept
    {
        lock_guard<AssertExternallySynchronizedLock> critSec{*this};
        fRep_.reset ();
    }
    inline Function<void()> Thread::Ptr::GetFunction () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        if (fRep_ == nullptr)
            [[UNLIKELY_ATTR]]
            {
                return nullptr;
            }
        return fRep_->fRunnable_;
    }
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
    inline Thread::Ptr::operator bool () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        return fRep_ != nullptr;
    }
#if qPlatform_Windows
    bool Thread::Ptr::GetThrowInterruptExceptionInsideUserAPC () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{*this};
        return fRep_ == nullptr ? false : fRep_->fThrowInterruptExceptionInsideUserAPC_;
    }
    inline void Thread::Ptr::SetThrowInterruptExceptionInsideUserAPC (bool throwInterruptExceptionInsideUserAPC)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec1{*this};
        RequireNotNull (fRep_);
        fRep_->fThrowInterruptExceptionInsideUserAPC_ = throwInterruptExceptionInsideUserAPC;
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
    template <typename FUNCTION>
    inline Thread::Ptr Thread::New (FUNCTION f, const optional<Characters::String>& name, const optional<Configuration>& configuration, enable_if_t<is_function_v<FUNCTION>>*)
    {
        return New (Function<void()> (f), name, configuration);
    }
    template <typename FUNCTION>
    inline Thread::Ptr Thread::New (FUNCTION f, AutoStartFlag flag, const optional<Characters::String>& name, const optional<Configuration>& configuration, enable_if_t<is_function_v<FUNCTION>>*)
    {
        return New (Function<void()> (f), flag, name, configuration);
    }
    inline void Thread::AbortAndWaitForDone (const Traversal::Iterable<Thread::Ptr>& threads, Time::DurationSecondsType timeout)
    {
        AbortAndWaitForDoneUntil (threads, timeout + Time::GetTickCount ());
    }
    inline void Thread::Start (const Traversal::Iterable<Thread::Ptr>& threads)
    {
        threads.Apply ([](const Thread::Ptr& p) { p.Start (); });
    }
    inline void Thread::WaitForDone (const Traversal::Iterable<Thread::Ptr>& threads, Time::DurationSecondsType timeout)
    {
        WaitForDoneUntil (threads, timeout + Time::GetTickCount ());
    }
#if qPlatform_POSIX
    inline SignalID Thread::GetSignalUsedForThreadInterrupt ()
    {
        return sSignalUsedForThreadInterrupt_;
    }
#endif

    /*
     ********************************************************************************
     *************************** GetCurrentThreadID *********************************
     ********************************************************************************
     */
    inline Thread::IDType GetCurrentThreadID () noexcept
    {
        return this_thread::get_id ();
    }

    /*
     ********************************************************************************
     ************************* CheckForThreadInterruption ***************************
     ********************************************************************************
     */
    template <unsigned int kEveryNTimes>
    void CheckForThreadInterruption ()
    {
        // note that it is not important that this be protected/thread safe, since the value is just advisory/hint
        static unsigned int n = 0;
        if (++n % kEveryNTimes == kEveryNTimes - 1)
            [[UNLIKELY_ATTR]]
            {
                CheckForThreadInterruption ();
            }
    }
}

namespace Stroika::Foundation::Configuration {
    template <>
    struct DefaultNames<Execution::Thread::Status> : EnumNames<Execution::Thread::Status> {
        static constexpr EnumNames<Execution::Thread::Status> k{
            EnumNames<Execution::Thread::Status>::BasicArrayInitializer{
                {
                    {Execution::Thread::Status::eNull, L"Null"},
                    {Execution::Thread::Status::eNotYetRunning, L"Not-Yet-Running"},
                    {Execution::Thread::Status::eRunning, L"Running"},
                    {Execution::Thread::Status::eAborting, L"Aborting"},
                    {Execution::Thread::Status::eCompleted, L"Completed"},
                }}};
        DefaultNames ()
            : EnumNames<Execution::Thread::Status> (k)
        {
        }
    };
}

#endif /*_Stroika_Foundation_Execution_Thread_inl_*/
