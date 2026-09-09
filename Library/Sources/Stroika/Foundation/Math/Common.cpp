/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>

#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Math/Common.h"

using namespace Stroika::Foundation;

void Math::Private_::ThrowTruncOfNAN_ ()
{
    Execution::Throw (Execution::Exception<runtime_error>{"trunc (nan)"sv});
}