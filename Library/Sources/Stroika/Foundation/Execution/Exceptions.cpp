/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Exceptions.h"

#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
#include    "../Characters/String.h"
#include    "../Debug/BackTrace.h"
#include    "../Execution/Thread.h"
#endif


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;





/*
 ********************************************************************************
 ******************** Execution::Private_::GetBT_s/GetBT_ws *********************
 ********************************************************************************
 */
#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
string  Private_::GetBT_s ()
{
    Thread::SuppressInterruptionInContext   suppressCtx;    // avoid throwing here even if we allocate memory (because messes up DoThrow() logging)
    return Debug::BackTrace ().AsNarrowSDKString ();
}
wstring Private_::GetBT_ws ()
{
    Thread::SuppressInterruptionInContext   suppressCtx;    // avoid throwing here even if we allocate memory (because messes up DoThrow() logging)
    return Debug::BackTrace ().As<wstring> ();
}
#endif