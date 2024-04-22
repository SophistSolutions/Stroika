/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SharedStaticData_inl_
#define _Stroika_Foundation_Execution_SharedStaticData_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Execution {

    namespace Private_ {
        // hack to avoid #include of Thread.h for Thread::SuppressInterruptionInContext
        bool SharedStaticData_DTORHelper_ (conditional_t<kSpinLock_IsFasterThan_mutex, SpinLock, mutex>* m, unsigned int* cu) noexcept;
    }

    /*
     ********************************************************************************
     ******************************** SharedStaticData<T> ***************************
     ********************************************************************************
     */
    template <typename T>
    SharedStaticData<T>::SharedStaticData ()
    {
        [[maybe_unused]] lock_guard critSec{sMutex_};
        ++sCountUses_;
        if (sCountUses_ == 1) {
            sOnceObj_ = new T{};
        }
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

#endif /*_Stroika_Foundation_Execution_SharedStaticData_inl_*/
