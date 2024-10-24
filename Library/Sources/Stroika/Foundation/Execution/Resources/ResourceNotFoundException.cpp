/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

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
    : Execution::RuntimeErrorException<>{"Resource Not Found"sv}
{
}
