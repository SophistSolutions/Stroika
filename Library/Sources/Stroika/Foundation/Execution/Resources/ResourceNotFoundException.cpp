/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/String_Constant.h"

#include "ResourceNotFoundException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ************************** ResourceNotFoundException ***************************
 ********************************************************************************
 */
const ResourceNotFoundException ResourceNotFoundException::kThe;

ResourceNotFoundException::ResourceNotFoundException ()
    : Execution::RuntimeErrorException<> (L"Resource Not Found"sv)
{
}
