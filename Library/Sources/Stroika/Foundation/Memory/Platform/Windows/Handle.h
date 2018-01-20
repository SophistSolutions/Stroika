/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

namespace Stroika {
    namespace Foundation {
        namespace Memory {
            namespace Platform {
                namespace Windows {

                    using Foundation::Memory::Byte;

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
                        nonvirtual Byte* GetPointer () const;

                    public:
                        /**
                         */
                        nonvirtual size_t GetSize () const;

                    private:
                        HANDLE fHandle_;
                        Byte*  fPointer_;
                    };
                }
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Handle.inl"

#endif /*_Stroika_Foundation_Memory_OS_Win32_Handle_h_*/
