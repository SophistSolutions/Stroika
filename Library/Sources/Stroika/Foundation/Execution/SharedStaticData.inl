/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SharedStaticData_inl_
#define _Stroika_Foundation_Execution_SharedStaticData_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "Common.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            namespace Private_ {
                // hack to avoid #include of Thread.h for Thread::SuppressInterruptionInContext
                bool    SharedStaticData_DTORHelper_ (
#if     qStroika_Foundation_Execution_SpinLock_IsFasterThan_mutex
                    SpinLock* m,
#else
                    mutex* m,
#endif
                    unsigned int* cu
                );
            }


            /*
            ********************************************************************************
            ******************************** SharedStaticData<T> ***************************
            ********************************************************************************
            */
            template    <typename T>
            T*  SharedStaticData<T>::sOnceObj_;

            template    <typename T>
#if     qStroika_Foundation_Execution_SpinLock_IsFasterThan_mutex
            SpinLock    SharedStaticData<T>::sMutex_;
#else
            mutex    SharedStaticData<T>::sMutex_;
#endif

            template    <typename T>
            unsigned int    SharedStaticData<T>::sCountUses_ = 0;

            template    <typename T>
            SharedStaticData<T>::SharedStaticData ()
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (sMutex_);
#else
                auto    critSec { make_unique_lock (sMutex_) };
#endif
                ++sCountUses_;
                if (sCountUses_ == 1) {
                    sOnceObj_ = new T {};
                }
            }
            template    <typename T>
            SharedStaticData<T>::~SharedStaticData ()
            {
                if (Private_::SharedStaticData_DTORHelper_ (&sMutex_, &sCountUses_)) {
                    Assert (sCountUses_ == 0);
                    delete sOnceObj_;
                    sOnceObj_ = nullptr;
                }
            }
            template    <typename T>
            inline  T&  SharedStaticData<T>::Get ()
            {
                // no need to lock since no way to destroy value while 'this' object still exists
                Ensure (sCountUses_ >= 1);
                EnsureNotNull (sOnceObj_);
                return *sOnceObj_;
            }
            template    <typename T>
            inline  const T&  SharedStaticData<T>::Get () const
            {
                // no need to lock since no way to destroy value while 'this' object still exists
                Ensure (sCountUses_ >= 1);
                EnsureNotNull (sOnceObj_);
                return *sOnceObj_;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SharedStaticData_inl_*/
