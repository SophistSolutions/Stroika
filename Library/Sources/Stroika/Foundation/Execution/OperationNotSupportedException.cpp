/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
    : StringException (String_Constant (L"Operation Not Supported"))
    , fOperationName_ ()
{
}

OperationNotSupportedException::OperationNotSupportedException (const String& operationName)
    : StringException (String_Constant (L"Operation '") + operationName + String_Constant (L"' Not Supported"))
    , fOperationName_ (operationName)
{
}

Characters::String OperationNotSupportedException::GetOperationName () const
{
    return fOperationName_;
}
