/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Sanitizer_h_
#define _Stroika_Foundation_Debug_Sanitizer_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

#if defined(__cplusplus)
namespace Stroika {
    namespace Foundation {
        namespace Debug {

#if defined(__clang__)
// at least clang++4 requires this older attr syntax - retest later with clang5
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE(ARGS) __attribute__ ((no_sanitize (ARGS)))
#elif defined(__GNUC__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE(ARGS) [[no_sanitize (ARGS)]]
#else
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE(ARGS)
#endif
        }
    }
}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Sanitizer.inl"

#endif /*_Stroika_Foundation_Debug_Sanitizer_h_*/
