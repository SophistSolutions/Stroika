/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_CriticalSectionMutex_h_
#define _Stroika_Foundation_Execution_Platform_Windows_CriticalSectionMutex_h_   1

#include    "../../../StroikaPreComp.h"

#if     qPlatform_Windows
#include    <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include    "../../../Configuration/Common.h"


/**
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Platform {
                namespace   Windows {


                    /**
                     *  (reason is to test if perofrmance better than msvc mutex)
                     */
                    class   CriticalSectionMutex {
                    public:
                        CriticalSectionMutex ();
                        CriticalSectionMutex (const CriticalSectionMutex&) = delete;

                    public:
                        ~CriticalSectionMutex ();

                    public:
                        CriticalSectionMutex& operator= (const CriticalSectionMutex&) = delete;

                    public:
                        nonvirtual  void    lock ();
                        nonvirtual  void    unlock ();

                    private:
                        CRITICAL_SECTION    fCritSec_;
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
#include    "CriticalSectionMutex.inl"

#endif  /*_Stroika_Foundation_Execution_Platform_Windows_CriticalSectionMutex_h_*/
