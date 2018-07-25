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
namespace Stroika::Foundation::Debug {

#if qCompiler_noSanitizeAttribute_Buggy
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE(ARGS)
#else
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE(ARGS) [[no_sanitize (ARGS)]]
#endif
}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Sanitizer.inl"

#endif /*_Stroika_Foundation_Debug_Sanitizer_h_*/
