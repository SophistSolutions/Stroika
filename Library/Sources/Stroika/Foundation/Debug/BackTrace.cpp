/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#if     qPlatform_Linux
#include    <execinfo.h>
#include    <unistd.h>
#elif   qPlatform_Windows
#include    <Windows.h>
#endif

#include    "../Characters/LineEndings.h"
#include    "../Characters/StringBuilder.h"
#include    "../Execution/Finally.h"

#include    "BackTrace.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;




/*
 ********************************************************************************
 ********************************* Debug::BackTrace *****************************
 ********************************************************************************
 */
String    Debug::BackTrace (unsigned int maxFrames)
{
#if     qPlatform_Linux
    // @see http://man7.org/linux/man-pages/man3/backtrace.3.html
    constexpr   size_t  kMaxStackSize_  =   100;        // could look at return size and re-run if equals exactly...
    // @todo combine maxFrames with trial and error on backtrace() calls
    void* stackTraceBuf[kMaxStackSize_] {};
    int nptrs = ::backtrace (stackTraceBuf, NEltsOf (stackTraceBuf));
    //DbgTrace ("backtrace() returned %d addresses\n", nptrs);
    char**   syms = ::backtrace_symbols (stackTraceBuf, nptrs);
    if (syms == NULL) {
        DbgTrace ("%d errno", errno); // perror("backtrace_symbols");
        return String {};
    }
    Execution::Finally cleanup ([syms] () { if (syms != nullptr) ::free (syms); });
    StringBuilder    out;
    for (int j = 0; j < nptrs; j++) {
        out.Append (String::FromNarrowSDKString (syms[j]) + Characters::GetEOL<wchar_t> ());
    }
    return out.str ();
#else
    return Characters::String {};
#endif
}
