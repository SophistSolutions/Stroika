/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstdlib> // to force __GLIBCXX__ define reference
#include <sstream>

#if qPlatform_Linux
#include <execinfo.h>
#include <unistd.h>
#if defined(__GNUC__) && defined(__GLIBCXX__)
#include <cxxabi.h>
#endif
#elif qPlatform_MacOS
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED 1
#elif qPlatform_Windows
#include <Windows.h>
// BOOST_STACKTRACE_USE_WINDBG seems to be default on windows, and very slow
#define BOOST_STACKTRACE_USE_WINDBG_CACHED 1
#endif

#if qHasFeature_boost
#include <boost/stacktrace.hpp>
#endif

#include "../Characters/LineEndings.h"
#include "../Characters/StringBuilder.h"
#include "../Execution/Finally.h"

#include "BackTrace.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;

/*
 ********************************************************************************
 ************************ Debug::BackTrace::Capture *****************************
 ********************************************************************************
 */
wstring Debug::BackTrace::Capture ([[maybe_unused]] const BackTrace::Options& options)
{
    [[maybe_unused]] unsigned int useSkipFrames = options.fSkipFrames.value_or (BackTrace::Options::sDefault_SkipFrames);

    useSkipFrames += 1; // always skip this frame, because anyone calling BackTrace() doens't care to see its implementation in the trace

    [[maybe_unused]] unsigned usingMaxFrames = options.fMaxFrames.value_or (BackTrace::Options::sDefault_MaxFrames);
#if qHasFeature_boost
    using namespace boost;

    auto bt = stacktrace::stacktrace ();

    //
    // Simple default usage with boost
    // auto x = String::FromNarrowSDKString (stacktrace::to_string (bt)).As<wstring> ();
    //

    wstringstream result; // avoid use of StringBuild to avoid dependencies on the rest of stroika
    streamsize    w      = result.width ();
    size_t        frames = bt.size ();

    useSkipFrames += 2; // boost (as checking on windows as of 2020-03-01) appears to leave in two layers of its own

    if (useSkipFrames != 0 and frames != 0) {
        result << L"..." << Characters::GetEOL<wchar_t> ();
    }

    bool includeSrcLines = options.fIncludeSourceLines.value_or (BackTrace::Options::sDefault_IncludeSourceLines);
    for (size_t i = 0; i < frames; ++i) {
        if (i < useSkipFrames) {
            continue;
        }
        result.width (2);
        result << i;
        result.width (w);
        result << L"# ";
        if (includeSrcLines) {
            result << Characters::NarrowSDKStringToWide (boost::stacktrace::to_string (bt[i]));
        }
        else {
            result << Characters::NarrowSDKStringToWide (bt[i].name ());
        }
        result << L";" << Characters::GetEOL<wchar_t> ();
        if (i - useSkipFrames >= usingMaxFrames) {
            break;
        }
    }
    return result.str ();
#elif qPlatform_Linux
    /*
     *  @see http://man7.org/linux/man-pages/man3/backtrace.3.html
     */
    constexpr size_t kMaxStackSize_ = 100; // could look at return size and re-run if equals exactly...
    // @todo combine maxFrames with trial and error on backtrace() calls
    void*  stackTraceBuf[kMaxStackSize_]{};
    int    nptrs = ::backtrace (stackTraceBuf, Memory::NEltsOf (stackTraceBuf));
    char** syms  = ::backtrace_symbols (stackTraceBuf, nptrs);
    if (syms == NULL) {
        //DbgTrace ("%d errno", errno); // perror("backtrace_symbols");
        return wstring{};
    }
    auto narrow2Wide = [] (const char* s) -> wstring {
        wstring symStr;
        for (const char* p = s; *p != '\0'; ++p) {
            symStr += *p;
        }
        return symStr;
    };
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([syms] () noexcept { if (syms != nullptr) ::free (syms); });
    wstring                 out;
    if (useSkipFrames != 0 and nptrs != 0) {
        out += wstring{L"..."} + Characters::GetEOL<wchar_t> ();
    }
    for (int j = 0; j < nptrs; j++) {
        if (j < useSkipFrames) {
            continue;
        }
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
        if (j - useSkipFrames >= usingMaxFrames) {
            break;
        }
    }
    return out;
#elif qPlatform_Windows
    // No real need todo this because boost does so well, but could be done pretty easily - see
    // http://www.debuginfo.com/examples/src/SymFromAddr.cpp -- LGP 2020-03-01
    return wstring{};
#else
    return wstring{};
#endif
}
