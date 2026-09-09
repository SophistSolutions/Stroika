/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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
    : Execution::Exception<runtime_error>{"Resource Not Found"sv}
{
}
