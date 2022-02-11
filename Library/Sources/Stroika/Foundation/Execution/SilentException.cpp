/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

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
