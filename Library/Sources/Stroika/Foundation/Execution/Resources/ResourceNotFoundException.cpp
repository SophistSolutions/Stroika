/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/String_Constant.h"

#include "ResourceNotFoundException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ************************ OperationNotSupportedException ************************
 ********************************************************************************
 */
ResourceNotFoundException::ResourceNotFoundException ()
    : StringException (String_Constant (L"Resource Not Found"))
{
}
