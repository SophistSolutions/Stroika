/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

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
Containers::Sequence< Activity<>>	Execution::CaptureCurrentActivities ()
{
	Containers::Sequence< Activity<>>	result;
	// no locks needed because thread local
	for (Private_::Activities_::StackElt_* s = Private_::Activities_::sTop_; s != nullptr; s = s->fPrev) {
		result += Activity<> { s->fActivity->AsString ()};
	}
	return result;
}
