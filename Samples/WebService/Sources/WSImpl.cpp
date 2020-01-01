/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"

#include "WSImpl.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

using IO ::Network::HTTP::ClientErrorException;

using namespace StroikaSample::WebServices;

namespace {
    RWSynchronized<Mapping<String, Number>> sVariables_;
}

/*
 ********************************************************************************
 ************************************* WSImpl ***********************************
 ********************************************************************************
 */
Collection<String> WSImpl::Variables_GET () const
{
    return sVariables_.cget ()->Keys ();
}

Number WSImpl::Variables_GET (const String& variable) const
{
    if (auto o = sVariables_.cget ()->Lookup (variable)) {
        return *o;
    }
    Execution::Throw (ClientErrorException (L"no such variable"sv));
}

void WSImpl::Variables_DELETE (const String& variable) const
{
    sVariables_.rwget ()->Remove (variable);
}

void WSImpl::Variables_SET (const String& variable, const Number& value)
{
    sVariables_.rwget ()->Add (variable, value);
}

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
    if (rhs == Number{0}) {
        // Note - important to use ClientErrorException so web-server returns HTTP status 400, instead of 500
        Execution::Throw (ClientErrorException (L"divide by zero"sv));
    }
    return lhs / rhs;
}
