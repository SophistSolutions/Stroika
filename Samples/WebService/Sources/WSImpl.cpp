/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/StringException.h"

#include "WSImpl.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

using namespace Stroika::Foundation;

using namespace StroikaSample::WebServices;

/*
 ********************************************************************************
 ************************************* WSImpl ***********************************
 ********************************************************************************
 */
Number WSImpl::plus (Number lhs, Number rhs) const
{
    return lhs + rhs;
}

Number WSImpl::minus (Number lhs, Number rhs) const
{
    return lhs - rhs;
}

Number WSImpl::times (Number lhs, Number rhs) const
{
    return lhs * rhs;
}

Number WSImpl::divide (Number lhs, Number rhs) const
{
    if (rhs == 0) {
        Execution::Throw (Execution::StringException (L"divide by zero"sv));
    }
    return lhs / rhs;
}
