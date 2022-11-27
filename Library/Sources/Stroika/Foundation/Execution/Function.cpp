/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"

#include "Function.h"

using namespace Stroika::Foundation;


/*
 ********************************************************************************
 ************************ Execution::Private_ ***********************************
 ********************************************************************************
 */
Characters::String Execution::Private_::ToString_ (const void* p)
{
    return Characters::Format (L"%p", p);
}
