/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ThreadPool_inl_
#define _Stroika_Foundation_Execution_ThreadPool_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ThreadPool.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             *********************************** ThreadPool *********************************
             ********************************************************************************
             */
            inline void ThreadPool::WaitForTasksDone (const Traversal::Iterable<TaskType>& tasks, Time::DurationSecondsType timeout) const
            {
                WaitForTasksDoneUntil (tasks, Time::GetTickCount () + timeout);
            }
            inline void ThreadPool::WaitForTasksDone (Time::DurationSecondsType timeout) const
            {
                WaitForTasksDoneUntil (Time::GetTickCount () + timeout);
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_ThreadPool_inl_*/
