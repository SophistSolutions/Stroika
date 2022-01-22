/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "ResourceNotFoundException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ************************** ResourceNotFoundException ***************************
 ********************************************************************************
 */
ResourceNotFoundException::ResourceNotFoundException ()
    : Execution::RuntimeErrorException<>{L"Resource Not Found"sv}
{
}
