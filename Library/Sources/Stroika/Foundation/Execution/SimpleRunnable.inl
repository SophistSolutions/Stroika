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

            inline  SimpleRunnable::SimpleRunnable (void (*fun2CallOnce) ())
                : fFun2CallOnce (FakeZeroArg_)
                , fArg (reinterpret_cast<void*> (fun2CallOnce))
            {
            }
            inline  SimpleRunnable::SimpleRunnable (void (*fun2CallOnce) (void* arg), void* arg)
                : fFun2CallOnce (fun2CallOnce)
                , fArg (arg)
            {
            }
            inline  shared_ptr<IRunnable>    SimpleRunnable::MAKE (void (*fun2CallOnce) ())
            {
                return shared_ptr<IRunnable> (DEBUG_NEW SimpleRunnable (fun2CallOnce));
            }
            inline  shared_ptr<IRunnable>    SimpleRunnable::MAKE (void (*fun2CallOnce) (void* arg), void* arg)
            {
                return shared_ptr<IRunnable> (DEBUG_NEW SimpleRunnable (fun2CallOnce, arg));
            }



            template    <typename   OBJ>
            inline  SimpleObjRunnable<OBJ>::SimpleObjRunnable (void (OBJ::*ptrToMemberFunction)(), OBJ* objPtr)
                : fPtrToMemberFunction_ (ptrToMemberFunction)
                , fObjPtr_ (objPtr)
            {
            }
            template    <typename   OBJ>
            inline  shared_ptr<IRunnable>    SimpleObjRunnable<OBJ>::MAKE (void (OBJ::*ptrToMemberFunction)(), OBJ* objPtr)
            {
                return shared_ptr<IRunnable> (DEBUG_NEW SimpleObjRunnable<OBJ> (ptrToMemberFunction, objPtr));
            }
            template    <typename   OBJ>
            void    SimpleObjRunnable<OBJ>::Run () override
            {
                ((*fObjPtr_).*(fPtrToMemberFunction_)) ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SimpleRunnable_inl_*/
