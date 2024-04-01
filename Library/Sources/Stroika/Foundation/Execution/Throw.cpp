/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/ToString.h"

#include "Throw.h"

#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
#include "../Characters/String.h"
#include "../Debug/BackTrace.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;

#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
/*
 ********************************************************************************
 ********************* Execution::Private_::JustDbgTrace_ ***********************
 ********************************************************************************
 */
void Execution::Private_::JustDbgTrace_ (const string& msg)
{
    DbgTrace ("{}"_f, String::FromNarrowSDKString (msg));
}
void Execution::Private_::JustDbgTrace_ (const wstring& msg)
{
    DbgTrace ("{}"_f, msg);
}

/*
 ********************************************************************************
 ************* Execution::Private_::ThrowingExceptionDbgTrace_ ******************
 ********************************************************************************
 */
void Execution::Private_::ThrowingExceptionDbgTrace_ (const string& msg)
{
    if constexpr (qStroika_Foundation_Execution_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
        DbgTrace ("Throwing exception: {} from {}"_f, String::FromNarrowSDKString (msg), Private_::GetBT_ws ());
#else
        DbgTrace ("Throwing exception: {}"_f, String{msg});
#endif
    }
}
void Execution::Private_::ThrowingExceptionDbgTrace_ (const wstring& msg)
{
    if constexpr (qStroika_Foundation_Execution_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
        DbgTrace ("Throwing exception: {} from {}"_f, msg, Private_::GetBT_ws ());
#else
        DbgTrace ("Throwing exception: {}"_f, msg);
#endif
    }
}

/*
 ********************************************************************************
 ************ Execution::Private_::ReThrowingExceptionDbgTrace_ *****************
 ********************************************************************************
 */
void Execution::Private_::ReThrowingExceptionDbgTrace_ (const string& msg)
{
    if constexpr (qStroika_Foundation_Execution_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
        DbgTrace ("ReThrow: {} from {}"_f, String::FromNarrowSDKString (msg), Private_::GetBT_ws ());
#else
        DbgTrace ("ReThrow: {}"_f, String{msg});
#endif
    }
}
void Execution::Private_::ReThrowingExceptionDbgTrace_ (const wstring& msg)
{
    if constexpr (qStroika_Foundation_Execution_Throw_TraceThrowpoint) {
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
        DbgTrace ("ReThrow: {} from {}"_f, msg, Private_::GetBT_ws ());
#else
        DbgTrace ("ReThrow: {}"_f, msg);
#endif
    }
}
#endif

/*
 ********************************************************************************
 ******************** Execution::Private_::GetBT_s/GetBT_ws *********************
 ********************************************************************************
 */
#if qStroika_Foundation_Execution_Throw_TraceThrowpointBacktrace
string Execution::Private_::GetBT_s ()
{
    // KISS, and don't use string if you don't want truncation
    wstring tmp = Debug::BackTrace::Capture (Debug::BackTrace::Options{1}); // skip this frame from stacktrace
    string  result;
    for (wchar_t c : tmp) {
        result += static_cast<char> (c);
    }
    return result;
}
wstring Execution::Private_::GetBT_ws ()
{
    // no need to use Thread::SuppressInterruptionInContext since not using any Stroika code that can throw
    return Debug::BackTrace::Capture (Debug::BackTrace::Options{1}); // skip this frame from stacktrace
}
#endif

string Execution::Private_::ToString_ (const type_info& s)
{
    return Characters::ToString (s).AsNarrowSDKString (Characters::eIgnoreErrors);
}
