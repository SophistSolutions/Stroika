/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "Exceptions.h"

#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
#include    "../Debug/BackTrace.h"
#endif


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;

using   Characters::String;




/*
 ********************************************************************************
 ************************ Execution::GetCurrentUserName *************************
 ********************************************************************************
 */

#if     qStroika_Foundation_Exection_Exceptions_TraceThrowpointBacktrace
string  Private_::GetBT_s ()
{
    return Debug::BackTrace ().AsNarrowSDKString ();
}
wstring Private_::GetBT_ws ()
{
    return Debug::BackTrace ().As<wstring> ();
}
#endif