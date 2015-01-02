/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ThreadPool_inl_
#define _Stroika_Foundation_Execution_ThreadPool_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ThreadPool.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             *********************************** ThreadPool *********************************
             ********************************************************************************
             */
            inline  void    ThreadPool::WaitForDone (Time::DurationSecondsType timeout) const
            {
                WaitForDoneUntil (timeout + Time::GetTickCount ());
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_ThreadPool_inl_*/
