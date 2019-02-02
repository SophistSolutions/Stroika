/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/String_Constant.h"

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
    : Execution::Exception<> (L"Operation Not Supported"sv)
    , fOperationName_ ()
{
}

OperationNotSupportedException::OperationNotSupportedException (const String& operationName)
    : Execution::Exception<> (L"Operation '"sv + operationName + L"' Not Supported"sv)
    , fOperationName_ (operationName)
{
}

Characters::String OperationNotSupportedException::GetOperationName () const
{
    return fOperationName_;
}
