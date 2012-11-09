/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SimpleRunnable_inl_
#define _Stroika_Foundation_Execution_SimpleRunnable_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            inline  SimpleRunnable::SimpleRunnable (const std::function<void()>& fun2CallOnce)
                : fCall_ (fun2CallOnce)
            {
            }
            inline  IRunnablePtr    SimpleRunnable::MAKE (const std::function<void()>& fun2CallOnce)
            {
                return IRunnablePtr (DEBUG_NEW SimpleRunnable (fun2CallOnce));
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SimpleRunnable_inl_*/
