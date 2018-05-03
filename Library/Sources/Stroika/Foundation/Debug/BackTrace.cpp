/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#if qPlatform_Linux
#include <execinfo.h>
#include <unistd.h>
#if defined(__GNUC__) && defined(__GLIBCXX__)
#include <cxxabi.h>
#endif
#elif qPlatform_Windows
#include <Windows.h>
#endif

#include "../Characters/LineEndings.h"
#include "../Characters/StringBuilder.h"
#include "../Execution/Finally.h"

#include "BackTrace.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

/*
 ********************************************************************************
 ********************************* Debug::BackTrace *****************************
 ********************************************************************************
 */
wstring Debug::BackTrace (unsigned int maxFrames)
{
#if qPlatform_Linux
    // @see http://man7.org/linux/man-pages/man3/backtrace.3.html
    constexpr size_t kMaxStackSize_ = 100; // could look at return size and re-run if equals exactly...
    // @todo combine maxFrames with trial and error on backtrace() calls
    void*  stackTraceBuf[kMaxStackSize_]{};
    int    nptrs = ::backtrace (stackTraceBuf, NEltsOf (stackTraceBuf));
    char** syms  = ::backtrace_symbols (stackTraceBuf, nptrs);
    if (syms == NULL) {
        //DbgTrace ("%d errno", errno); // perror("backtrace_symbols");
        return wstring{};
    }
    auto narrow2Wide = [](const char* s) -> wstring {
        wstring symStr;
        for (const char* p = s; *p != '\0'; ++p) {
            symStr += *p;
        }
        return symStr;
    };
    auto&&  cleanup = Execution::Finally ([syms]() noexcept { if (syms != nullptr) ::free (syms); });
    wstring out;
    for (int j = 0; j < nptrs; j++) {
        wstring symStr = narrow2Wide (syms[j]);
#if defined(__GNUC__) && defined(__GLIBCXX__)
        //
        // Example output from backtrace_symbols:
        //      /media/Sandbox/lewis-Sandbox/Stroika-DevRoot/Builds/Debug/Samples_SimpleService(_ZN7Stroika10Foundation9Execution8Private_8GetBT_wsEv+0x21) [0x7fc677]
        //
        //  abi::__cxa_demangle requires just the _ZN7Stroika10Foundation9Execution8Private_8GetBT_wsEv part, so extract it.
        //
        const char* beginOfName = ::strchr (syms[j], '(');
        if (beginOfName != nullptr) {
            beginOfName++;
        }
        const char* endOfName = (beginOfName == nullptr) ? nullptr : ::strchr (beginOfName, '+');
        //if (endOfName == nullptr and beginOfName != nullptr) {
        //  endOfName = ::strchr (beginOfName, ')');    // maybe no + sometimes?
        //}
        if (beginOfName != nullptr and endOfName != nullptr) {
            int    status = -1;
            string tmp{beginOfName, endOfName};
            char*  realname = abi::__cxa_demangle (tmp.c_str (), 0, 0, &status);
            if (status == 0) {
                symStr = narrow2Wide ((string{static_cast<const char*> (syms[j]), beginOfName} + realname + endOfName).c_str ());
            }
            if (realname != nullptr) {
                ::free (realname);
            }
        }
#endif
        out += symStr + L";" + Characters::GetEOL<wchar_t> ();
    }
    return out;
#else
    return wstring{};
#endif
}
