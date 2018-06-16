/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Sanitizer_h_
#define _Stroika_Foundation_Debug_Sanitizer_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \file
 *

 *
 */

#if defined(__cplusplus)
namespace Stroika {
    namespace Foundation {
        namespace Debug {

#if qCompiler_noSanitizeAttribute_Buggy
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE(ARGS)
#else
#if qCompiler_noSanitizeAttributeMustUseOldStyleAttr_Buggy
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE(ARGS) __attribute__ ((no_sanitize (ARGS)))
#else
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE(ARGS) [[no_sanitize (ARGS)]]
#endif
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
