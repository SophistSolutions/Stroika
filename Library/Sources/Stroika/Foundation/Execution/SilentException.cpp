/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "SilentException.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

/*
 ********************************************************************************
 ******************************** SilentException *******************************
 ********************************************************************************
 */
const char* SilentException::what () const noexcept
{
    return "Silent Exception";
}
