/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

#include "Synchronized.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

#if qStroika_Foundation_Debug_DefaultTracingOn
void Execution::Private_::DbgTraceHelper_ (const wchar_t* w1, const optional<std::wstring>& w2)
{
    if (w2) {
        DbgTrace ("{}: {}"_f, w1, *w2);
    }
    else {
        DbgTrace ("{}"_f, w1);
    }
}
#endif

/*
 ********************************************************************************
 ************************ Execution::Private_::ThrowTimeout_ ********************
 ********************************************************************************
 */
void Execution::Private_::ThrowTimeout_ ()
{
    // Out-of-line purely so Synchronized.inl can declare it: Synchronized.h cannot include Exceptions.h, and
    // every ThrowError () overload is inline, so a forward declaration of ThrowError would not link. A .cpp has
    // no such problem. @see the declaration in Synchronized.inl.
    ThrowError (errc::timed_out);
}
