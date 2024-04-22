/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_OS_Win32_Handle_h_
#define _Stroika_Foundation_Memory_OS_Win32_Handle_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#endif

#include <memory>

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Memory/Common.h"

namespace Stroika::Foundation::Memory::Platform::Windows {

    /**
     */
    class StackBasedHandleLocker {
    public:
        StackBasedHandleLocker () = delete;
        StackBasedHandleLocker (HANDLE h);
        StackBasedHandleLocker (const StackBasedHandleLocker&) = delete;

    public:
        ~StackBasedHandleLocker ();

    public:
        const StackBasedHandleLocker& operator= (const StackBasedHandleLocker&) = delete;

    public:
        /**
         */
        nonvirtual byte* GetPointer () const;

    public:
        /**
         */
        nonvirtual size_t GetSize () const;

    private:
        HANDLE fHandle_;
        byte*  fPointer_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Handle.inl"

#endif /*_Stroika_Foundation_Memory_OS_Win32_Handle_h_*/
