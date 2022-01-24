/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "OperationNotSupportedException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ************************ OperationNotSupportedException ************************
 ********************************************************************************
 */
OperationNotSupportedException::OperationNotSupportedException ()
    : Execution::RuntimeErrorException<>{L"Operation Not Supported"sv}
{
}

OperationNotSupportedException::OperationNotSupportedException (const String& operationName)
    : Execution::RuntimeErrorException<>{L"Operation '"sv + operationName + L"' Not Supported"sv}
    , fOperationName_{operationName}
{
}

Characters::String OperationNotSupportedException::GetOperationName () const
{
    return fOperationName_;
}
