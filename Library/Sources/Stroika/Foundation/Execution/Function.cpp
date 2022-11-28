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
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wint-to-pointer-cast\""); // if constexpr should mean not compiled
        return FunctionObjectOrderingType_ (p);                                                  // cannot use {} if type differs error
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wint-to-pointer-cast\"");
    }
    if constexpr (is_same_v<Execution::Private_::FunctionObjectOrderingType_, uint32_t>) {
        static atomic<uint32_t> sNextPtr_{1};             // could use uniform_int_distribution<uint32_t> but this is faster & more reliable
        return FunctionObjectOrderingType_ (++sNextPtr_); // cannot use {} if type differs error
    }
}
