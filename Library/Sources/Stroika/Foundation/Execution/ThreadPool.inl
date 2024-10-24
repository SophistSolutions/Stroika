/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Math/Common.h"

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
        return Options{.fThreadCount       = static_cast<unsigned int> (fThreads_.size ()),
                       .fThreadPoolName    = fThreadPoolName_,
                       .fQMax              = fDefaultQMax_,
                       .fCollectStatistics = this->fCollectStatistics_};
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
