/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Process_h_
#define _Stroika_Foundation_Execution_Process_h_    1

#include    "../StroikaPreComp.h"

#include    <mutex>

#if         qPlatform_Windows
#include    <process.h>
#elif   qPlatform_POSIX
#include    <sys/types.h>
#include    <unistd.h>
#endif

#include    "../Configuration/Common.h"

#include    "Exceptions.h"


#if     !defined (qHas_pid_t)
#error  "qHas_pid_t must  be defined in StroikaConfig.h"
#endif



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

#if     !qHas_pid_t
#if     qPlatform_Windows
            typedef DWORD   pid_t;
#else
            typedef int pid_t;
#endif
#endif

            pid_t   GetCurrentProcessID ();

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Process_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Process.inl"
