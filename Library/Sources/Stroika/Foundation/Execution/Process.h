/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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

/// TODO - maybe move this to configuraiotn module???

#if     qHas_pid_t
            using   pid_t   =   ::pid_t ;
#else
#if     qPlatform_Windows
            using   pid_t   =   DWORD;
#else
            using   pid_t   =   int;
#endif
#endif

            pid_t   GetCurrentProcessID ();


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Process.inl"

#endif  /*_Stroika_Foundation_Execution_Process_h_*/
