/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_OS_Win32_Handle_h_
#define _Stroika_Foundation_Memory_OS_Win32_Handle_h_ 1

#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#endif

#include <memory>

#include "../../../Configuration/Common.h"
#include "../../../Memory/Common.h"

namespace Stroika::Foundation::Memory::Platform::Windows {

    using std::byte;

    /**
     */
    class StackBasedHandleLocker {
    public:
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
