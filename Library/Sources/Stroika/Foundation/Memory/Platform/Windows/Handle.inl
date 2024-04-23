/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Throw.h"

namespace Stroika::Foundation::Memory::Platform::Windows {

    /*
     ********************************************************************************
     ***************************** StackBasedHandleLocker ***************************
     ********************************************************************************
     */
    inline StackBasedHandleLocker::StackBasedHandleLocker (HANDLE h)
        : fHandle_{h}
        , fPointer_{reinterpret_cast<byte*> (::GlobalLock (h))}
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
