/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ThreadPool_inl_
#define _Stroika_Foundation_Execution_ThreadPool_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Math/Common.h"

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
    inline ThreadPool::TaskType ThreadPool::AddTask (const TaskType& task, const optional<Characters::String>& name)
    {
        if (fDefaultQMax_) {
            return AddTask (task, *fDefaultQMax_, name);
        }
        else {
            return AddTask_ (task, name);
        }
    }
    inline void ThreadPool::WaitForTasksDone (const Traversal::Iterable<TaskType>& tasks, Time::DurationSeconds timeout) const
    {
        WaitForTasksDoneUntil (tasks, Time::GetTickCount () + timeout);
    }
    inline void ThreadPool::WaitForTasksDone (Time::DurationSeconds timeout) const
    {
        WaitForTasksDoneUntil (Time::GetTickCount () + timeout);
    }
    inline bool ThreadPool::GetCollectingStatistics () const
    {
        return fCollectingStatistics_;
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
    inline Time::DurationSeconds ThreadPool::Statistics::GetMeanTimeConsumed () const
    {
        if (fNumberOfTasksReporting == 0) {
            return Math::nan<Time::DurationSeconds> ();
        }
        return fTotalTimeConsumed / fNumberOfTasksReporting;
    }

}
#endif /*_Stroika_Foundation_Execution_ThreadPool_inl_*/
