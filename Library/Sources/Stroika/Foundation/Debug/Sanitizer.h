/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Sanitizer_h_
#define _Stroika_Foundation_Debug_Sanitizer_h_ 1

#include "../StroikaPreComp.h"

#if __has_include(<sanitizer/asan_interface.h>)
#include <sanitizer/asan_interface.h>
#endif
#if __has_include(<sanitizer/lsan_interface.h>)
#include <sanitizer/lsan_interface.h>
#endif
#if __has_include(<sanitizer/tsan_interface.h>)
#include <sanitizer/tsan_interface.h>
#endif

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

#if defined(__cplusplus)
namespace Stroika::Foundation::Debug {

    /// @todo understand why [[no_sanitize_thread]] (etc) syntax not working on gcc (no error but no effect - and often warning unrecognized)

#if defined(__clang__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD [[gnu::no_sanitize_thread]]
#elif defined(__GNUC__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD __attribute__ ((no_sanitize ("thread")))
#else
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD
#endif

#if defined(__clang__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED [[gnu::no_sanitize_undefined]]
#elif defined(__GNUC__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED __attribute__ ((no_sanitize ("undefined")))
#else
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED
#endif

#if defined(__clang__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__ ((no_sanitize_address))
#elif defined(__GNUC__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__ ((no_sanitize ("address")))
#else
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif

#if defined(__has_feature)
#if __has_feature(address_sanitizer) or defined(__SANITIZE_ADDRESS__)
    constexpr bool kBuiltWithAddressSanitizer = true;
#else
    constexpr bool kBuiltWithAddressSanitizer = false;
#endif
#elif defined(__SANITIZE_ADDRESS__)
    // sadly, even gcc8 doesn't __has_feature, but defines this instead
    constexpr bool kBuiltWithAddressSanitizer = true;
#else
    constexpr bool kBuiltWithAddressSanitizer = false;
#endif

#if defined(__has_feature)
#if __has_feature(address_thread) or defined(__SANITIZE_THREAD__)
    constexpr bool kBuiltWithThreadSanitizer = true;
#else
    constexpr bool kBuiltWithThreadSanitizer  = false;
#endif
#elif defined(__SANITIZE_THREAD__)
    // sadly, even gcc8 doesn't __has_feature, but defines this instead
    constexpr bool kBuiltWithThreadSanitizer = true;
#else
    constexpr bool kBuiltWithThreadSanitizer  = false;
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
