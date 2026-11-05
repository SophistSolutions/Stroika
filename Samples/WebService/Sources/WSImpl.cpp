/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Execution/Synchronized.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"

#include "WSImpl.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

using IO ::Network::HTTP::ClientErrorException;

using namespace StroikaSample::WebServices;

namespace {
    RWSynchronized<Mapping<String, Model::Number>> sVariables_;
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

Model::Number WSImpl::Variables_GET (const String& variable) const
{
    if (auto o = sVariables_.cget ()->Lookup (variable)) {
        return *o;
    }
    Execution::Throw (ClientErrorException{"no such variable"sv});
}

void WSImpl::Variables_DELETE (const String& variable) const
{
    sVariables_.rwget ()->Remove (variable);
}

void WSImpl::Variables_SET (const String& variable, const Model::Number& value)
{
    sVariables_.rwget ()->Add (variable, value);
}

Model::Number WSImpl::plus (Model::Number lhs, Model::Number rhs) const
{
    return lhs + rhs;
}

Model::Number WSImpl::minus (Model::Number lhs, Model::Number rhs) const
{
    return lhs - rhs;
}

Model::Number WSImpl::times (Model::Number lhs, Model::Number rhs) const
{
    return lhs * rhs;
}

Model::Number WSImpl::divide (Model::Number lhs, Model::Number rhs) const
{
    if (rhs == Model::Number{0}) {
        // Note - important to use ClientErrorException so web-server returns HTTP status 400, instead of 500
        Execution::Throw (ClientErrorException{"divide by zero"sv});
    }
    return lhs / rhs;
}
