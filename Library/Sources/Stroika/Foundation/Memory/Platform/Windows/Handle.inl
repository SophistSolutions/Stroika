/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_OS_Win32_Handle_inl_
#define _Stroika_Foundation_Memory_OS_Win32_Handle_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {
            namespace   Platform {
                namespace   Windows {


                    //  class   StackBasedHandleLocker
                    inline  StackBasedHandleLocker::StackBasedHandleLocker (HANDLE h):
                        fHandle (h)
                    {
                        RequireNotNull (h);
                        fPointer = reinterpret_cast<Byte*> (::GlobalLock (h));
                    }
                    inline  StackBasedHandleLocker::~StackBasedHandleLocker ()
                    {
                        ::GlobalUnlock (fHandle);
                    }
                    inline  Byte*   StackBasedHandleLocker::GetPointer () const
                    {
                        return fPointer;
                    }
                    inline  size_t  StackBasedHandleLocker::GetSize () const
                    {
                        return ::GlobalSize (fHandle);
                    }


                }
            }
        }
    }
}
#endif  /*_Stroika_Foundation_Memory_OS_Win32_Handle_inl_*/
