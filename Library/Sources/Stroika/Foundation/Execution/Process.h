/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Process_h_
#define _Stroika_Foundation_Execution_Process_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <mutex>

#if qStroika_Platform_POSIX
#include <sys/types.h>
#include <unistd.h>
#elif qStroika_Platform_Windows
#include <Windows.h>
#include <process.h>
#endif

#include "Stroika/Foundation/Common/Common.h"

namespace Stroika::Foundation::Execution {

#if qStroika_Platform_POSIX
    using pid_t = ::pid_t;
#elif qStroika_Platform_Windows
    using pid_t = DWORD;
#else
    using pid_t = int;
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
