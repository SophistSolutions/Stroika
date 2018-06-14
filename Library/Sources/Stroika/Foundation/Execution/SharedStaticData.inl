/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SharedStaticData_inl_
#define _Stroika_Foundation_Execution_SharedStaticData_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"
#include "Common.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            namespace Private_ {
                // hack to avoid #include of Thread.h for Thread::SuppressInterruptionInContext
                bool SharedStaticData_DTORHelper_ (conditional_t<kSpinLock_IsFasterThan_mutex, SpinLock, mutex>* m, unsigned int* cu);
            }

            /*
             ********************************************************************************
             ******************************** SharedStaticData<T> ***************************
             ********************************************************************************
             */
            template <typename T>
            T* SharedStaticData<T>::sOnceObj_;

            template <typename T>
            conditional_t<kSpinLock_IsFasterThan_mutex, SpinLock, mutex> SharedStaticData<T>::sMutex_;

            template <typename T>
            unsigned int SharedStaticData<T>::sCountUses_ = 0;

            template <typename T>
            SharedStaticData<T>::SharedStaticData ()
            {
                [[maybe_unused]] auto&& critSec = lock_guard{sMutex_};
                ++sCountUses_;
                if (sCountUses_ == 1) {
                    sOnceObj_ = new T{};
                }
#if qDebug
                Stroika_Foundation_Debug_ValgrindDisableHelgrind (sCountUses_); // beacuse use in asserts - read there - safe/OK even without lock
                Stroika_Foundation_Debug_ValgrindDisableHelgrind (sOnceObj_);   // maybe not non-const get, but count on other later to provide interlock/checking
#endif
            }
            template <typename T>
            SharedStaticData<T>::~SharedStaticData ()
            {
                if (Private_::SharedStaticData_DTORHelper_ (&sMutex_, &sCountUses_)) {
                    Assert (sCountUses_ == 0);
                    delete sOnceObj_;
                    sOnceObj_ = nullptr;
                }
            }
            template <typename T>
            inline T& SharedStaticData<T>::Get ()
            {
                // no need to lock since no way to destroy value while 'this' object still exists
                Ensure (sCountUses_ >= 1);
                EnsureNotNull (sOnceObj_);
                return *sOnceObj_;
            }
            template <typename T>
            inline const T& SharedStaticData<T>::Get () const
            {
                // no need to lock since no way to destroy value while 'this' object still exists
                Ensure (sCountUses_ >= 1);
                EnsureNotNull (sOnceObj_);
                return *sOnceObj_;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_SharedStaticData_inl_*/
