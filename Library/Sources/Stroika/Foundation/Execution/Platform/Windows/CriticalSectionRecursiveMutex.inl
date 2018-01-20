/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_CriticalSectionRecursiveMutex_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_CriticalSectionRecursiveMutex_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace Execution {
            namespace Platform {
                namespace Windows {

                    /*
                     ********************************************************************************
                     ************************ CriticalSectionRecursiveMutex *************************
                     ********************************************************************************
                     */
                    inline CriticalSectionRecursiveMutex::CriticalSectionRecursiveMutex ()
                        : fCritSec_ ()
                    {
                        ::InitializeCriticalSection (&fCritSec_);
                    }
                    inline CriticalSectionRecursiveMutex::~CriticalSectionRecursiveMutex ()
                    {
                        ::DeleteCriticalSection (&fCritSec_);
                    }
                    inline void CriticalSectionRecursiveMutex::lock ()
                    {
                        ::EnterCriticalSection (&fCritSec_);
                    }
                    inline void CriticalSectionRecursiveMutex::unlock ()
                    {
                        ::LeaveCriticalSection (&fCritSec_);
                    }
                }
            }
        }
    }
}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_CriticalSectionRecursiveMutex_inl_*/
