/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>

#if qStroika_Foundation_Common_Platform_MacOS
#include <crt_externs.h>
#include <libproc.h>
#include <mach-o/dyld.h>
#endif
#if qStroika_Foundation_Common_Platform_POSIX && qSupport_Proc_Filesystem
#include <unistd.h>
#endif
#if qStroika_Foundation_Common_Platform_Windows
#include <windows.h>
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

#include "Module.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ***************************** Execution::GetEXEDir *****************************
 ********************************************************************************
 */
filesystem::path Execution::GetEXEDir ()
{
    return GetEXEPath ().parent_path ();
}

/*
 ********************************************************************************
 **************************** Execution::GetEXEPath *****************************
 ********************************************************************************
 */
filesystem::path Execution::GetEXEPath ()
{
// See also http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
//      Mac OS X: _NSGetExecutablePath() (man 3 dyld)
//      Linux: readlink /proc/self/exe
//      Solaris: getexecname()
//      FreeBSD: sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
//      BSD with procfs: readlink /proc/curproc/file
//      Windows: GetModuleFileName() with hModule = nullptr
//
#if qStroika_Foundation_Common_Platform_MacOS
    uint32_t bufSize = 0;
    Verify (_NSGetExecutablePath (nullptr, &bufSize) == -1);
    Assert (bufSize > 0);
    Memory::StackBuffer<char> buf{Memory::eUninitialized, bufSize};
    Verify (_NSGetExecutablePath (buf.begin (), &bufSize) == 0);
    Assert (buf[bufSize - 1] == '\0');
    return buf.begin ();
#elif qStroika_Foundation_Common_Platform_POSIX && qSupport_Proc_Filesystem
    // readlink () isn't clear about finding the right size. The only way to tell it wasn't enuf (maybe) is
    // if all the bytes passed in are used. That COULD mean it all fit, or there was more. If we get that -
    // double buf size and try again
    Memory::StackBuffer<SDKChar> buf{Memory::eUninitialized, 1024};
    ssize_t                      n;
    while ((n = ::readlink ("/proc/self/exe", buf.data (), buf.GetSize ())) == buf.GetSize ()) {
        buf.GrowToSize_uninitialized (buf.GetSize () * 2);
    }
    if (n < 0) {
        ThrowPOSIXErrNo (errno);
    }
    Assert (n <= buf.GetSize ()); // could leave no room for NUL-byte, but not needed
    return SDKString{buf.begin (), buf.begin () + n};
#elif qStroika_Foundation_Common_Platform_Windows
    SDKChar buf[MAX_PATH];
    Verify (::GetModuleFileName (nullptr, buf, static_cast<DWORD> (Memory::NEltsOf (buf))));
    buf[Memory::NEltsOf (buf) - 1] = '\0'; // cheaper and just as safe as memset() - more even. Buffer always nul-terminated, and if GetModuleFileName succeeds will be nul-terminated
    return buf;
#else
    AssertNotImplemented ();
    return filesystem::path{};
#endif
}

/*
 ********************************************************************************
 ***************************** Execution::GetEXEPath ****************************
 ********************************************************************************
 */
filesystem::path Execution::GetEXEPath ([[maybe_unused]] pid_t processID)
{
#if qStroika_Foundation_Common_Platform_MacOS
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    int  ret = ::proc_pidpath (processID, pathbuf, sizeof (pathbuf));
    if (ret <= 0) {
        Throw (Exception{"proc_pidpath failed"sv}); // @todo - horrible reporting, but not obvious what this API is? proc_pidpath?
    }
    else {
        return pathbuf;
    }
#elif qStroika_Foundation_Common_Platform_POSIX && qSupport_Proc_Filesystem
    // readlink () isn't clear about finding the right size. The only way to tell it wasn't enuf (maybe) is
    // if all the bytes passed in are used. That COULD mean it all fit, or there was more. If we get that -
    // double buf size and try again
    Memory::StackBuffer<SDKChar> buf{Memory::eUninitialized, 1024};
    ssize_t                      n;
    char                         linkNameBuf[1024];
    (void)std::snprintf (linkNameBuf, sizeof (linkNameBuf), "/proc/%ld/exe", static_cast<long> (processID));
    while ((n = ::readlink (linkNameBuf, buf.data (), buf.GetSize ())) == buf.GetSize ()) {
        buf.GrowToSize_uninitialized (buf.GetSize () * 2);
    }
    if (n < 0) {
        ThrowPOSIXErrNo (errno);
    }
    Assert (n <= buf.GetSize ()); // could leave no room for NUL-byte, but not needed
    return SDKString{buf.begin (), buf.begin () + n};
#elif qStroika_Foundation_Common_Platform_Windows
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682621(v=vs.85).aspx but a bit of work
    // not needed yet
    AssertNotImplemented ();
    return filesystem::path{};
#else
    AssertNotImplemented ();
    return filesystem::path{};
#endif
}

/*
 ********************************************************************************
 ******************************** Execution::kPath ******************************
 ********************************************************************************
 */
const LazyInitialized<Sequence<filesystem::path>> Execution::kPath{[] () -> Sequence<filesystem::path> {
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    if (const char* env_p = std::getenv ("PATH")) {
        String pathVar = String::FromNarrowSDKString (env_p);
#if qStroika_Foundation_Common_Platform_POSIX
        return pathVar.Tokenize ({':'}).Map<Sequence<filesystem::path>> ([] (auto i) { return i.template As<filesystem::path> (); });
#elif qStroika_Foundation_Common_Platform_Windows
        return pathVar.Tokenize ({';'}).Map<Sequence<filesystem::path>> ([] (auto i) { return i.template As<filesystem::path> (); });
#endif
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    return {};
}};

#if qStroika_Foundation_Common_Platform_Windows
/*
 ********************************************************************************
 ***************************** Execution::kPathEXT ******************************
 ********************************************************************************
 */
const LazyInitialized<Sequence<filesystem::path>> Execution::kPathEXT{[] () -> Sequence<filesystem::path> {
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    if (const char* env_p = std::getenv ("PATHEXT")) {
        String pathVar = String::FromNarrowSDKString (env_p);
        return pathVar.Tokenize ({';'}).Map<Sequence<filesystem::path>> ([] (auto i) { return i.template As<filesystem::path> (); });
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996)
    return {};
}};
#endif

/*
 ********************************************************************************
 ************************ Execution::kRawEnvironment ****************************
 ********************************************************************************
 */
const LazyInitialized<Mapping<SDKString, SDKString>> Execution::kRawEnvironment{[] () -> Mapping<SDKString, SDKString> {
    Mapping<SDKString, SDKString> r;
    const SDKChar* const*         envHead = nullptr;
#if qStroika_Foundation_Common_Platform_Windows
    // documented in https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getenv-wgetenv?view=msvc-170 can happen, and how to workaround
    DISABLE_COMPILER_MSC_WARNING_START (4996)
    if constexpr (same_as<SDKChar, wchar_t>) {
        if (_wenviron == nullptr) {
            [[maybe_unused]] auto ignored = ::_wgetenv (L"PATH");
        }
    }
    else {
        if (environ == nullptr) {
            [[maybe_unused]] auto ignored = ::getenv ("PATH");
        }
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996)
#endif
#if qStroika_Foundation_Common_Platform_Windows
    if constexpr (same_as<SDKChar, wchar_t>) {
        envHead = _wenviron;
    }
    else
#endif
    {
#if qStroika_Foundation_Common_Platform_MacOS
        envHead = (*_NSGetEnviron ());
#else
        envHead = environ;
#endif
    }
    // NULL-terminated array of NUL-terminated strings
    AssertNotNull (envHead);
    for (const SDKChar* const* p = envHead; *p; ++p) {
        SDKString eltStr = *p;
        size_t    i      = eltStr.find ('=');
        if (i == SDKString::npos) {
            DbgTrace ("bad env elt: {}"_f, String::FromSDKString (eltStr));
            WeakAssertNotReached ();
        }
        else {
            r.Add (eltStr.substr (0, i), eltStr.substr (i + 1));
        }
    }
    return r;
}};

/*
 ********************************************************************************
 *************************** Execution::kEnvironment ****************************
 ********************************************************************************
 */
const LazyInitialized<Mapping<String, String>> Execution::kEnvironment{[] () -> Mapping<String, String> {
    Mapping<String, String> r;
    for (auto i : kRawEnvironment ()) {
        r.Add (String::FromSDKString (i.fKey), String::FromSDKString (i.fValue));
    }
    return r;
}};

/*
 ********************************************************************************
 ********************** Execution::FindExecutableInPath *************************
 ********************************************************************************
 */
optional<filesystem::path> Execution::FindExecutableInPath (const filesystem::path& fn)
{
    auto checkExists = [] (const filesystem::path& exe) {
        // better to use 'access' api?
        return filesystem::exists (exe) and filesystem::is_regular_file (exe) and
               static_cast<bool> (filesystem::status (exe).permissions () & filesystem::perms::owner_exec);
    };
    if (fn.is_absolute ()) {
        if (checkExists (fn)) {
            return fn;
        }
#if qStroika_Foundation_Common_Platform_Windows
        if (fn.extension ().empty ()) {
            filesystem::path exe = fn;
            for (auto exeExt : kPathEXT ()) {
                exe.replace_extension (exeExt);
                if (checkExists (exe)) {
                    return exe;
                }
            }
        }
#endif
    }
    else {
        // if not absolute, try relative to each element of the path (and CWD? not for now??)
        for (filesystem::path d : kPath ()) {
            filesystem::path exe = d / fn;
            if (checkExists (exe)) {
                return exe;
            }
#if qStroika_Foundation_Common_Platform_Windows
            if (fn.extension ().empty ()) {
                for (auto exeExt : kPathEXT ()) {
                    exe.replace_extension (exeExt);
                    if (checkExists (exe)) {
                        return exe;
                    }
                }
            }
#endif
        }
    }
    return nullopt;
}