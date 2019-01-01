/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_OS_Win32_Handle_inl_
#define _Stroika_Foundation_Memory_OS_Win32_Handle_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../../Debug/Assertions.h"
#include "../../../Execution/Exceptions.h"

namespace Stroika::Foundation::Memory::Platform::Windows {

    /*
     ********************************************************************************
     ***************************** StackBasedHandleLocker ***************************
     ********************************************************************************
     */
    inline StackBasedHandleLocker::StackBasedHandleLocker (HANDLE h)
        : fHandle_ (h)
        , fPointer_ (reinterpret_cast<byte*> (::GlobalLock (h)))
    {
        RequireNotNull (h);
        Execution::ThrowIfNull (fPointer_);
    }
    inline StackBasedHandleLocker::~StackBasedHandleLocker ()
    {
        AssertNotNull (fHandle_);
        ::GlobalUnlock (fHandle_);
    }
    inline byte* StackBasedHandleLocker::GetPointer () const
    {
        return fPointer_;
    }
    inline size_t StackBasedHandleLocker::GetSize () const
    {
        return ::GlobalSize (fHandle_);
    }

}

#endif /*_Stroika_Foundation_Memory_OS_Win32_Handle_inl_*/
