/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_OS_Win32_Handle_h_
#define _Stroika_Foundation_Memory_OS_Win32_Handle_h_   1

#include    "../../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    <memory>

#include    "../../../Configuration/Common.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {
            namespace   Platform {
                namespace   Windows {


                    class   StackBasedHandleLocker {
                    public:
                        StackBasedHandleLocker (HANDLE h);
                        ~StackBasedHandleLocker ();
                    private:
                        StackBasedHandleLocker (const StackBasedHandleLocker&);
                        const StackBasedHandleLocker& operator= (const StackBasedHandleLocker&);

                    public:
                        nonvirtual  Byte*   GetPointer () const;
                        nonvirtual  size_t  GetSize () const;

                    private:
                        HANDLE  fHandle;
                        Byte*   fPointer;
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
#include    "Handle.inl"

#endif  /*_Stroika_Foundation_Memory_OS_Win32_Handle_h_*/
