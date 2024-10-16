/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Process_h_
#define _Stroika_Foundation_Execution_Process_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>

#if qPlatform_POSIX
#include <sys/types.h>
#include <unistd.h>
#elif qPlatform_Windows
#include <Windows.h>
#include <process.h>
#endif

#include "Stroika/Foundation/Configuration/Common.h"

#if !defined(qHas_pid_t)
#error "qHas_pid_t must  be defined in StroikaConfig.h"
#endif

namespace Stroika::Foundation::Execution {

    /// TODO - maybe move this to configuraiotn module???

#if qHas_pid_t
    using pid_t = ::pid_t;
#else
#if qPlatform_Windows
    using pid_t = DWORD;
#else
    using pid_t = int;
#endif
#endif

    pid_t GetCurrentProcessID ();

    /**
     *  Return true, if the pid can be verified to be running, false if verified not running.
     *  \note   This obviously can be a race as processes can come and go quickly
     */
    bool IsProcessRunning (pid_t pid);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Process.inl"

#endif /*_Stroika_Foundation_Execution_Process_h_*/
