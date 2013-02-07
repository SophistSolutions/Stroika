/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CopyableCriticalSection_h_
#define _Stroika_Foundation_Execution_CopyableCriticalSection_h_    1

#include    "../StroikaPreComp.h"

#include    <memory>
#include    <mutex>

#if     qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../Configuration/Common.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            //PROBABLY OBSOLETE - NO NEW USES... WILL BE GOING AWAY
            class   CopyableCriticalSection {
            private:
                shared_ptr<std::recursive_mutex>  fCritSec;
            public:
                CopyableCriticalSection ();

            public:
                nonvirtual  void    Lock ();
                nonvirtual  void    Unlock ();
                nonvirtual  void    lock () { Lock (); }
                nonvirtual  void    unlock () { Unlock (); }
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_CopyableCriticalSection_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CopyableCriticalSection.inl"
