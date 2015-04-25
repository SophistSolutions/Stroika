/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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


            /*
            ********************************************************************************
            ******************************** SharedStaticData<T> ***************************
            ********************************************************************************
            */
            template    <typename T>
            T*  SharedStaticData<T>::sOnceObj_;

            template    <typename T>
            SpinLock    SharedStaticData<T>::sMutex_;

            template    <typename T>
            unsigned int    SharedStaticData<T>::sCountUses_ = 0;

            template    <typename T>
            SharedStaticData<T>::SharedStaticData ()
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (sMutex_);
#else
                DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
                auto    critSec { make_unique_lock (sMutex_) };
                DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
#endif
                ++sCountUses_;
                if (sCountUses_ == 1) {
                    sOnceObj_ = new T ();
                }
            }

            template    <typename T>
            SharedStaticData<T>::~SharedStaticData ()
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (sMutex_);
#else
                DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
                auto    critSec { make_unique_lock (sMutex_) };
                DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
#endif
                --sCountUses_;
                if (sCountUses_ == 0) {
                    delete sOnceObj_;
                    sOnceObj_ = nullptr;
                }
            }

            template    <typename T>
            T*  SharedStaticData<T>::Get ()
            {
                // no need to lock since no way to destroy value while 'this' object still exists
                Ensure (sCountUses_ >= 1);
                EnsureNotNull (sOnceObj_);
                return sOnceObj_;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SharedStaticData_inl_*/
