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

#if defined(__clang__) || defined(__GNUC__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__((no_sanitize("address")))
#else
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif

#if defined(__clang__) || defined(__GNUC__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINEDBEHAVIOR __attribute__((no_sanitize("undefined")))
#else
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINEDBEHAVIOR
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
