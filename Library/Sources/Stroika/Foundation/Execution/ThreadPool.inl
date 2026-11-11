/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     *********************************** ThreadPool *********************************
     ********************************************************************************
     */
    inline ThreadPool::ThreadPool ()
        : ThreadPool{ThreadPool::Options{}}
    {
    }
    inline auto ThreadPool::GetOptions () const -> Options
    {
        return Options{.fThreadCount = GetPoolSize (), .fThreadPoolName = fThreadPoolName_, .fQMax = fDefaultQMax_, .fCollectStatistics = this->fCollectStatistics_};
    }
    inline auto ThreadPool::AddTask (const TaskType& task, const optional<Characters::String>& name) -> TaskID
    {
        return AddTask_ (task, fDefaultQMax_, name);
    }
    template <invocable F>
    inline auto ThreadPool::AddTaskWithResult (F&& task, const optional<Characters::String>& name) -> TaskWithResult<invoke_result_t<F>>
    {
        return AddTaskWithResult_ (forward<F> (task), fDefaultQMax_, name);
    }
    template <invocable F>
    inline auto ThreadPool::AddTaskWithResult (F&& task, QMax qmax, const optional<Characters::String>& name)
        -> TaskWithResult<invoke_result_t<F>>
    {
        return AddTaskWithResult_ (forward<F> (task), qmax, name);
    }
    template <invocable F>
    auto ThreadPool::AddTaskWithResult_ (F&& task, const optional<QMax>& qmax, const optional<Characters::String>& name)
        -> TaskWithResult<invoke_result_t<F>>
    {
        using R = invoke_result_t<F>;
        static_assert (not is_reference_v<R>, "a task for AddTaskWithResult returns a value (or a pointer), not a reference");
        using Rep = typename TaskWithResult<R>::Rep_;
        /*
         *  TaskType is a copyable function<void ()>, and the task may be move-only: so it lives in one shared Task_, which the
         *  TaskType - however many copies of it there are - only points to. Whatever leaves the result unset (an abort, or the task
         *  removed before it ran) sets it, once the task is gone, to operation_canceled.
         */
        struct Task_ {
            shared_ptr<Rep> fResult;
            decay_t<F>      fTask;
            bool            fRan{false}; // set only by the task's run, and ~Task_ runs after that is done
            Task_ (const shared_ptr<Rep>& result, F&& t)
                : fResult{result}
                , fTask{forward<F> (t)}
            {
            }
            ~Task_ ()
            {
                if (not fRan) {
                    fResult->SetDone (nullopt, make_exception_ptr (SystemErrorException{make_error_code (errc::operation_canceled)}));
                }
            }
        };
        shared_ptr<Rep>   rep = Memory::MakeSharedPtr<Rep> ();
        shared_ptr<Task_> t   = Memory::MakeSharedPtr<Task_> (rep, forward<F> (task));
        TaskID            id  = AddTask_ (
            [t] () {
                try {
                    if constexpr (is_void_v<R>) {
                        t->fTask ();
                        t->fResult->SetDone (true, nullptr);
                    }
                    else {
                        t->fResult->SetDone (t->fTask (), nullptr);
                    }
                }
                catch (const Thread::AbortException&) {
                    t->fResult->SetDone (nullopt, make_exception_ptr (SystemErrorException{make_error_code (errc::operation_canceled)}));
                    t->fRan = true;
                    throw; // so the pool's thread still ends
                }
                catch (...) {
                    t->fResult->SetDone (nullopt, current_exception ());
                }
                t->fRan = true;
            },
            qmax, name);
        return TaskWithResult<R>{id, rep};
    }

    /*
     ********************************************************************************
     ********************** ThreadPool::TaskWithResult<R> ***************************
     ********************************************************************************
     */
    template <typename R>
    struct ThreadPool::TaskWithResult<R>::Rep_ {
        using ValueType_ = conditional_t<is_void_v<R>, bool, R>; // a void task's 'value' is just that it returned

        // fDoneChanged.fMutex guards the fields below it; set once, by SetDone
        mutable ConditionVariable<> fDoneChanged;
        optional<ValueType_>        fValue;
        exception_ptr               fException;
        bool                        fDone{false};

        void SetDone (optional<ValueType_>&& value, const exception_ptr& e)
        {
            ConditionVariable<>::LockType lock{fDoneChanged.fMutex};
            Require (not fDone);
            fValue     = move (value);
            fException = e;
            fDone      = true;
            fDoneChanged.release_and_notify_all (lock);
        }
    };
    template <typename R>
    inline ThreadPool::TaskWithResult<R>::TaskWithResult (TaskID id, const shared_ptr<Rep_>& rep)
        : fID_{id}
        , fRep_{rep}
    {
    }
    template <typename R>
    inline auto ThreadPool::TaskWithResult<R>::GetID () const -> TaskID
    {
        return fID_;
    }
    template <typename R>
    bool ThreadPool::TaskWithResult<R>::IsDone () const
    {
        typename ConditionVariable<>::QuickLockType lock{fRep_->fDoneChanged.fMutex};
        return fRep_->fDone;
    }
    template <typename R>
    inline auto ThreadPool::TaskWithResult<R>::GetResult (Time::DurationSeconds timeout) const -> ResultType
    {
        return GetResultUntil (Time::GetTickCount () + timeout);
    }
    template <typename R>
    auto ThreadPool::TaskWithResult<R>::GetResultUntil (Time::TimePointSeconds timeoutAt) const -> ResultType
    {
        ConditionVariable<>::LockType lock{fRep_->fDoneChanged.fMutex};
        if (not fRep_->fDoneChanged.wait_until (lock, timeoutAt, [this] () { return fRep_->fDone; })) {
            ThrowError (errc::timed_out);
        }
        if (fRep_->fException != nullptr) {
            rethrow_exception (fRep_->fException);
        }
        if constexpr (is_void_v<R>) {
            return;
        }
        else {
            return *fRep_->fValue; // set once and never changed after, so safe to read once the lock is released
        }
    }
    inline void ThreadPool::WaitForTasksDone (const Traversal::Iterable<TaskID>& tasks, Time::DurationSeconds timeout) const
    {
        WaitForTasksDoneUntil (tasks, Time::GetTickCount () + timeout);
    }
    inline void ThreadPool::WaitForTasksDone (Time::DurationSeconds timeout) const
    {
        WaitForTasksDoneUntil (Time::GetTickCount () + timeout);
    }

    /*
     ********************************************************************************
     *************************** ThreadPool::TaskInfo *******************************
     ********************************************************************************
     */
    inline bool ThreadPool::TaskInfo::IsRunning () const
    {
        return fRunningSince.has_value ();
    }

    /*
     ********************************************************************************
     *************************** ThreadPool::Statistics *****************************
     ********************************************************************************
     */
    inline optional<Time::DurationSeconds> ThreadPool::Statistics::GetMeanTimeConsumed () const
    {
        if (fNumberOfTasksReporting == 0) {
            return nullopt;
        }
        return fTotalTimeConsumed / fNumberOfTasksReporting;
    }

}
