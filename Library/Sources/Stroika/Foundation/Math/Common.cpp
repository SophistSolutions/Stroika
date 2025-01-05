/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdio>

#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Math/Common.h"

using namespace Stroika::Foundation;

void Math::Private_::ThrowTruncOfNAN_ ()
{
    Execution::Throw (Execution::RuntimeErrorException{"trunc (nan)"sv});
}