/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdlib> // to force __GLIBCXX__ define reference

#if defined(__GNUC__) && defined(__GLIBCXX__)
#include <cxxabi.h>
#elif qStroika_Foundation_Common_Platform_Windows
#include <Windows.h>

#include <Dbghelp.h>
#endif

#include "Stroika/Foundation/Execution/Finally.h"

#include "Demangle.h"

using namespace Stroika::Foundation;

#if qStroika_Foundation_Common_Platform_Windows
// otherwise modules linking with this code will tend to get link errors without explicitly linking
// to this module...
#pragma comment(lib, "Dbghelp.lib")
#endif

/*
 ********************************************************************************
 ************************ Debug::DropIntoDebuggerIfPresent **********************
 ********************************************************************************
 */
Characters::String Debug::Demangle (const Characters::String& originalName)
{
#if defined(__GNUC__) && defined(__GLIBCXX__)
    int   status{};
    char* realname = abi::__cxa_demangle (originalName.AsNarrowSDKString (Characters::eIgnoreErrors).c_str (), 0, 0, &status);
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([&realname] () noexcept {
        if (realname != nullptr) {
            ::free (realname);
        }
    });
    if (status == 0) {
        return Characters::String::FromNarrowSDKString (realname);
    }
#elif qStroika_Foundation_Common_Platform_Windows
    char resultBuf[10 * 1024];
    if (::UnDecorateSymbolName (originalName.AsNarrowSDKString (Characters::eIgnoreErrors).c_str (), resultBuf, sizeof (resultBuf), UNDNAME_COMPLETE) != 0) {
        return Characters::String::FromNarrowSDKString (resultBuf);
    }
#endif
    return originalName;
}
