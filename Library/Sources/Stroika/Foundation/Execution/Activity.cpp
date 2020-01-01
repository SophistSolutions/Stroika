/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
 *********************** Activity<Characters::String> ***************************
 ********************************************************************************
 */
Characters::String Activity<Characters::String>::AsString () const
{
    return fArg_;
}

/*
 ********************************************************************************
 *************************** Activity<wstring_view> *****************************
 ********************************************************************************
 */
Characters::String Activity<wstring_view>::AsString () const
{
    return fArg_;
}

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
