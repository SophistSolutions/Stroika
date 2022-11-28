/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"

#include "Function.h"

using namespace Stroika::Foundation;

/*
 ********************************************************************************
 ************************** Execution::Private_ *********************************
 ********************************************************************************
 */
Characters::String Execution::Private_::ToString_ (Private_::FunctionObjectOrderingType_ p)
{
    if constexpr (is_same_v<Execution::Private_::FunctionObjectOrderingType_, const void*>) {
        return Characters::Format (L"%p", p);
    }
    if constexpr (is_same_v<Execution::Private_::FunctionObjectOrderingType_, uint32_t>) {
        return Characters::Format (L"%u", p);
    }
}

Execution::Private_::FunctionObjectOrderingType_ Execution::Private_::mkFunctionObjectOrderingType_ (const void* p)
{
    if constexpr (is_same_v<Execution::Private_::FunctionObjectOrderingType_, const void*>) {
        return FunctionObjectOrderingType_ (p);             // cannot use {} if type differs error
    }
    if constexpr (is_same_v<Execution::Private_::FunctionObjectOrderingType_, uint32_t>) {
        static atomic<uint32_t> sNextPtr_{1};
        return FunctionObjectOrderingType_ (++sNextPtr_);   // cannot use {} if type differs error
        #if 0
        random_device                      rd;
        mt19937                            gen{rd ()};
        uniform_int_distribution<uint32_t> dis{numeric_limits<uint32_t>::min (), numeric_limits<uint32_t>::max ()};
        return FunctionObjectOrderingType_ (dis (gen)); // cannot use {} if type differs error
        #endif
    }
}
