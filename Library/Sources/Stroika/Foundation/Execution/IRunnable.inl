/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_IRunnable_inl_
#define _Stroika_Foundation_Execution_IRunnable_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ******************************** Execution::IRunnable **************************
             ********************************************************************************
             */
            inline  IRunnable::IRunnable ()
            {
            }
            inline  IRunnable::~IRunnable ()
            {
            }


            /*
             ********************************************************************************
             *************************** Execution::mkIRunnablePtr **************************
             ********************************************************************************
             */
            inline  IRunnablePtr    mkIRunnablePtr (const IRunnablePtr& fun2CallOnce)
            {
                return fun2CallOnce;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_IRunnable_inl_*/
