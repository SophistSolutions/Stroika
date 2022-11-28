/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <random>

#include "../Characters/Format.h"

#include "Function.h"

using namespace Stroika::Foundation;

/*
 ********************************************************************************
 ************************** Execution::Private_ *********************************
 ********************************************************************************
 */
Characters::String Execution::Private_::ToString_ (Private_::FunctionObjectOrderingType p)
{
    if constexpr (is_same_v<Execution::Private_::FunctionObjectOrderingType, const void*>) {
        return Characters::Format (L"%p", p);
    }
    if constexpr (is_same_v<Execution::Private_::FunctionObjectOrderingType, uint32_t>) {
        return Characters::Format (L"%u", p);
    }
}

Execution::Private_::FunctionObjectOrderingType Execution::Private_::mkFunctionObjectOrderingType (const void* p)
{
    if constexpr (is_same_v<Execution::Private_::FunctionObjectOrderingType, const void*>) {
        return p;
    }
    if constexpr (is_same_v<Execution::Private_::FunctionObjectOrderingType, uint32_t>) {
        random_device                      rd;
        mt19937                            gen{rd ()};
        uniform_int_distribution<uint32_t> dis{numeric_limits<uint32_t>::min (), numeric_limits<uint32_t>::max ()};
        return dis (gen);
    }
}
