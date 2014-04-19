/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_CriticalSectionMutex_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_CriticalSectionMutex_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Platform {
                namespace   Windows {


                    /*
                     ********************************************************************************
                     ****************************** CriticalSectionMutex ****************************
                     ********************************************************************************
                     */
                    inline  CriticalSectionMutex::CriticalSectionMutex ()
                        : fCritSec_ ()
                    {
                        ::InitializeCriticalSection (&fCritSec_);
                    }
                    inline  CriticalSectionMutex::~CriticalSectionMutex ()
                    {
                        ::DeleteCriticalSection (&fCritSec_);
                    }
                    inline  void    CriticalSectionMutex::lock ()
                    {
                        ::EnterCriticalSection (&fCritSec_);
                    }
                    inline  void    CriticalSectionMutex::unlock ()
                    {
                        ::LeaveCriticalSection (&fCritSec_);
                    }

                }
            }
        }
    }
}

#endif  /*_Stroika_Foundation_Execution_Platform_Windows_CriticalSectionMutex_inl_*/
