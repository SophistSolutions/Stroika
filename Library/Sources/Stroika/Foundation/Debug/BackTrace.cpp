/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdlib> // to force __GLIBCXX__ define reference
#include <sstream>

#if qStroika_Platform_POSIX
#include <dlfcn.h>
#endif

#if qStroika_Platform_Linux
#include <execinfo.h>
#include <unistd.h>
#if defined(__GNUC__) && defined(__GLIBCXX__)
#include <cxxabi.h>
#endif
#elif qStroika_Platform_MacOS
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED 1
#elif qStroika_Platform_Windows
#include <Windows.h>
// BOOST_STACKTRACE_USE_WINDBG seems to be default on windows, and very slow
#define BOOST_STACKTRACE_USE_WINDBG_CACHED 1
#endif

#if defined(__cpp_lib_stacktrace)
#include <stacktrace>
#endif

#if qStroika_HasComponent_boost
#include <boost/stacktrace.hpp>
#endif

#include "Stroika/Foundation/Characters/LineEndings.h"
#include "Stroika/Foundation/Characters/SDKString.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Thread.h"

#include "BackTrace.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Debug;

namespace {
#if qStroika_Platform_POSIX
    // Where a frame's call lies - "[<module>+0x<offset>]", the arguments addr2line takes - for a frame with no name; "" if
    // dladdr cannot tell. A frame's address is where its call returns to, just past the call: one byte back is in the call,
    // whose line - and function - addr2line should give.
    [[maybe_unused]] string ModuleAndOffset_ (const void* returnAddress)
    {
        const void* call = static_cast<const std::byte*> (returnAddress) - 1;
        Dl_info     info{};
        if (::dladdr (call, &info) == 0 or info.dli_fbase == nullptr) {
            return string{};
        }
        stringstream o;
        o << "[" << (info.dli_fname == nullptr ? "?" : info.dli_fname) << "+0x" << hex
          << reinterpret_cast<uintptr_t> (call) - reinterpret_cast<uintptr_t> (info.dli_fbase) << "]";
        return o.str ();
    }
#endif
}

/*
 ********************************************************************************
 ************************ Debug::BackTrace::Capture *****************************
 ********************************************************************************
 */
wstring Debug::BackTrace::Capture ([[maybe_unused]] const BackTrace::Options& options)
{
    // Throw () logs a Capture (), and Capture () calls code that may throw (character conversion, allocation) - so a
    // Capture () inside a Capture () returns nothing, rather than recursing
    static thread_local bool tCapturing_{false};
    if (tCapturing_) {
        return wstring{};
    }
    tCapturing_                   = true;
    [[maybe_unused]] auto&& clear = Execution::Finally ([] () noexcept { tCapturing_ = false; });

    Execution::Thread::SuppressInterruptionInContext suppressAborts;
    [[maybe_unused]] unsigned int                    useSkipFrames = options.fSkipFrames.value_or (BackTrace::Options::sDefault_SkipFrames);

    useSkipFrames += 1; // always skip this frame, because anyone calling BackTrace() doens't care to see its implementation in the trace

    [[maybe_unused]] unsigned usingMaxFrames = options.fMaxFrames.value_or (BackTrace::Options::sDefault_MaxFrames);

#if defined(__cpp_lib_stacktrace)
    // current () requires skip + max_depth to fit its size_type - just 16 bits in libstdc++, and 32 for MSVC x86 - and the
    // default usingMaxFrames is UINT_MAX
    auto st = std::stacktrace::current (useSkipFrames, min<size_t> (usingMaxFrames, numeric_limits<std::stacktrace::size_type>::max () - useSkipFrames));
    stringstream o;
    for (const stacktrace_entry& entry : st) {
        string eText = entry.description ();
        if (eText.empty ()) {
            // no name: its source file and line, where debug information gives them; else where its code lies
            eText = entry.source_file ();
            if (not eText.empty ()) {
                eText += "#" + to_string (entry.source_line ());
            }
#if qStroika_Platform_POSIX
            else {
                eText = ModuleAndOffset_ (reinterpret_cast<const void*> (entry.native_handle ()));
            }
#endif
        }
        else if (options.fIncludeSourceLines.value_or (BackTrace::Options::sDefault_IncludeSourceLines)) {
            eText += "#" + to_string (entry.source_line ());
        }
        o << eText << ";" << Characters::kEOL<char>;
    }
    return Characters::NarrowSDK2Wide (o.str (), eIgnoreErrors);
#elif qStroika_HasComponent_boost
    using namespace boost;

    // boost skips its own frames (its constructors are force-inlined; init and collect skip themselves), in Debug or
    // Release - so skip just ours, and let it apply the limit too
    auto bt = stacktrace::stacktrace (useSkipFrames, usingMaxFrames);

    wstringstream result; // avoid use of StringBuild to avoid dependencies on the rest of stroika
    streamsize    w      = result.width ();
    size_t        frames = bt.size ();

    if (useSkipFrames != 0 and frames != 0) {
        result << L"..." << Characters::kEOL<wchar_t>;
    }

    bool includeSrcLines = options.fIncludeSourceLines.value_or (BackTrace::Options::sDefault_IncludeSourceLines);
    for (size_t i = 0; i < frames; ++i) {
        result.width (2);
        result << i;
        result.width (w);
        result << L"# ";
        string name  = bt[i].name ();
        string frame = includeSrcLines ? boost::stacktrace::to_string (bt[i]) : name;
#if qStroika_Platform_POSIX
        if (name.empty ()) {
            string where = ModuleAndOffset_ (bt[i].address ());
            frame += (frame.empty () or where.empty () ? "" : " ") + where;
        }
#endif
        result << String::FromNarrowSDKString (frame).As<wstring> ();
        result << L";" << Characters::kEOL<wchar_t>;
    }
    return result.str ();
#elif qStroika_Platform_Linux
    /*
     *  @see http://man7.org/linux/man-pages/man3/backtrace.3.html
     */
    constexpr size_t kMaxStackSize_ = 100; // could look at return size and re-run if equals exactly...
    // @todo combine maxFrames with trial and error on backtrace() calls
    if constexpr (Debug::kBuiltWithAddressSanitizer or Debug::kBuiltWithThreadSanitizer) {
        ++useSkipFrames; // ASan and TSan interpose on backtrace (), so their interceptor is its first frame
    }
    void*  stackTraceBuf[kMaxStackSize_]{};
    int    nptrs = ::backtrace (stackTraceBuf, std::size (stackTraceBuf));
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
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([syms] () noexcept {
        if (syms != nullptr)
            ::free (syms);
    });
    wstring                 out;
    if (useSkipFrames != 0 and nptrs != 0) {
        out += wstring{L"..."} + Characters::kEOL<wchar_t>;
    }
    for (int j = 0; j < nptrs; j++) {
        if (j < useSkipFrames) {
            continue;
        }
        if (j - useSkipFrames >= usingMaxFrames) {
            break;
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
        out += symStr + L";" + Characters::kEOL<wchar_t>;
    }
    return out;
#elif qStroika_Platform_Windows
    // No real need todo this because boost does so well, but could be done pretty easily - see
    // http://www.debuginfo.com/examples/src/SymFromAddr.cpp -- LGP 2020-03-01
    return wstring{};
#else
    return wstring{};
#endif
}
