/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/ToString.h"

#include "Throw.h"

#if qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
#include "../Characters/String.h"
#include "../Debug/BackTrace.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ******************** Execution::Private_::GetBT_s/GetBT_ws *********************
 ********************************************************************************
 */
#if qStroika_Foundation_Exection_Throw_TraceThrowpointBacktrace
string Private_::GetBT_s ()
{
    // KISS, and don't use string if you don't want truncation
    wstring tmp = Debug::BackTrace::Capture (Debug::BackTrace::Options{1}); // skip this frame from stacktrace
    string  result;
    for (wchar_t c : tmp) {
        result += static_cast<char> (c);
    }
    return result;
}
wstring Private_::GetBT_ws ()
{
    // no need to use Thread::SuppressInterruptionInContext since not using any Stroika code that can throw
    return Debug::BackTrace::Capture (Debug::BackTrace::Options{1}); // skip this frame from stacktrace
}
#endif

string Private_::ToString_s (const type_info& s)
{
    return Characters::ToString (s).AsNarrowSDKString ();
}
