/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <list>

#include "../Debug/Trace.h"

#include "Activity.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ****************** Execution::CaptureCurrentActivities *************************
 ********************************************************************************
 */
Containers::Stack<Activity<>> Execution::CaptureCurrentActivities ()
{
    list<Activity<>> rv;
    // no locks needed because thread local
    for (const Private_::Activities_::StackElt_* si = Private_::Activities_::sTop_; si != nullptr; si = si->fPrev) {
        AssertNotNull (si->fActivity);
        rv.push_front (Activity<>{si->fActivity->AsString ()});
    }
    Containers::Stack<Activity<>> result;
    for (auto i = rv.rbegin (); i != rv.rend (); ++i) {
        result.Push (*i);
    }
    return result;
}
