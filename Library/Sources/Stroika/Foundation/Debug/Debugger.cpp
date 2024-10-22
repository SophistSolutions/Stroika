/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if __cpp_lib_debugging >= 202403L
#include <debugging>
#endif

#if qStroika_Foundation_Common_Platform_POSIX
#include <fcntl.h>
#include <string.h>
#include <string>
#include <unistd.h>
#if defined(__APPLE__) && defined(__MACH__)
#include <sys/sysctl.h>
#include <sys/types.h>
#endif
#include <cstdio>
#include <signal.h>
#include <sys/ptrace.h>
#elif qStroika_Foundation_Common_Platform_Windows
#include <Windows.h>
#endif
#include "Stroika/Foundation/Math/Common.h"

#include "Debugger.h"

using namespace Stroika::Foundation;

#ifndef __has_builtin
#define __has_builtin(x) 0 // Compatibility with non-clang compilers.
#endif

#if qStroika_Foundation_Common_Platform_Linux
namespace {
    // From https://stackoverflow.com/questions/3596781/how-to-detect-if-the-current-process-is-being-run-by-gdb
    bool DebuggerIsAttached_ ()
    {
        char    buf[4096];
        ssize_t num_read;
        {
            const int status_fd = ::open ("/proc/self/status", O_RDONLY);
            if (status_fd == -1)
                return false;
            num_read = ::read (status_fd, buf, sizeof (buf) - 1);
            ::close (status_fd);
            if (num_read <= 0)
                return false;
            buf[num_read] = '\0';
        }
        constexpr char kTacerPidString_[] = "TracerPid:";
        const auto     tracer_pid_ptr     = ::strstr (buf, kTacerPidString_);
        if (tracer_pid_ptr == nullptr)
            return false;
        for (const char* characterPtr = tracer_pid_ptr + sizeof (kTacerPidString_) - 1; characterPtr <= buf + num_read; ++characterPtr) {
            if (::isspace (*characterPtr))
                continue;
            else
                return ::isdigit (*characterPtr) != 0 && *characterPtr != '0';
        }
        return false;
    }
}
#endif

#if qStroika_Foundation_Common_Platform_MacOS
namespace {
    // https://stackoverflow.com/questions/2200277/detecting-debugger-on-mac-os-x

    // Returns true if the current process is being debugged (either
    // running under the debugger or has a debugger attached post facto).
    bool DebuggerIsAttached_ ()
    {
        // Initialize the flags so that, if sysctl fails for some bizarre
        // reason, we get a predictable result.
        kinfo_proc info{};
        // Initialize mib, which tells sysctl the info we want, in this case
        // we're looking for information about a specific process ID.
        int                  mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid ()};
        size_t               size   = sizeof (info);
        [[maybe_unused]] int junk   = sysctl (mib, sizeof (mib) / sizeof (*mib), &info, &size, NULL, 0);
        Assert (junk == 0);
        return (info.kp_proc.p_flag & P_TRACED) != 0;
    }
}
#endif

/*
 ********************************************************************************
 ************************ Debug::IsThisProcessBeingDebugged *********************
 ********************************************************************************
 */
optional<bool> Debug::IsThisProcessBeingDebugged ()
{
#if __cpp_lib_debugging >= 202403L
    return std::is_debugger_present ();
#endif
#if qStroika_Foundation_Common_Platform_Linux
    return DebuggerIsAttached_ ();
#endif
#if qStroika_Foundation_Common_Platform_MacOS
    return DebuggerIsAttached_ ();
#endif
#if qStroika_Foundation_Common_Platform_POSIX
#if 0
    // Not a good approach - see https://stackoverflow.com/questions/3596781/how-to-detect-if-the-current-process-is-being-run-by-gdb
   return ptrace (PTRACE_TRACEME, 0, NULL, 0) == -1;
#endif
#endif
#if qStroika_Foundation_Common_Platform_Windows
    return ::IsDebuggerPresent ();
#endif
    return nullopt;
}

/*
 ********************************************************************************
 ************************ Debug::DropIntoDebuggerIfPresent **********************
 ********************************************************************************
 */
void Debug::DropIntoDebuggerIfPresent ()
{
    if (IsThisProcessBeingDebugged () == true) {
#if __has_builtin(__builtin_trap) || defined(__GNUC__)
        __builtin_trap ();
#elif qStroika_Foundation_Common_Platform_POSIX
        raise (SIGTRAP);
#elif qStroika_Foundation_Common_Platform_Windows
        ::DebugBreak ();
#else
        abort ();
#endif
    }
}
