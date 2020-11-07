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

    namespace Private_ {
        /*
         *  Detect if each santizer is enabled, set a temporary macro flag __STK_DBG_SAN_HAS_{XXX}
         *  (for code factoring in this module) which we undefine when done.
         *
         *      __STK_DBG_SAN_HAS_ASAN
         *      __STK_DBG_SAN_HAS_TSAN
         *      __STK_DBG_SAN_HAS_UBSAN
         */

#if !defined(__STK_DBG_SAN_HAS_ASAN)
#if defined(__SANITIZE_ADDRESS__)
#define __STK_DBG_SAN_HAS_ASAN 1
#endif
#endif
#if !defined(__STK_DBG_SAN_HAS_ASAN)
#if defined(__has_feature)
// @ see https://clang.llvm.org/docs/AddressSanitizer.html#conditional-compilation-with-has-feature-address-sanitizer
#define __STK_DBG_SAN_HAS_ASAN __has_feature (address_sanitizer)
#endif
#endif

#if !defined(__STK_DBG_SAN_HAS_TSAN)
#if defined(__SANITIZE_THREAD__)
#define __STK_DBG_SAN_HAS_TSAN 1
#endif
#endif
#if !defined(__STK_DBG_SAN_HAS_TSAN)
// see https://clang.llvm.org/docs/ThreadSanitizer.html#has-feature-thread-sanitizer
#if defined(__has_feature)
#define __STK_DBG_SAN_HAS_TSAN __has_feature (thread_sanitizer)
#endif
#endif

#if !defined(__STK_DBG_SAN_HAS_UBSAN)
#if defined(__has_feature)
        // not documented - https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html - not sure if ever works
        // appears to work on apple XCode 11 clang, at least
#define __STK_DBG_SAN_HAS_UBSAN __has_feature (undefined_behavior_sanitizer)
#endif
#endif
    }

    /**
     *  \brief kBuiltWithAddressSanitizer can be checked in compiled code to see if the address sanitizer
     *         support is compiled into this executable
     */
#if __STK_DBG_SAN_HAS_ASAN
    constexpr bool kBuiltWithAddressSanitizer = true;
#else
    constexpr bool kBuiltWithAddressSanitizer           = false;
#endif

    /**
     *  \brief kBuiltWithThreadSanitizer can be checked in compiled code to see if the thread sanitizer
     *         support is compiled into this executable
     */
#if __STK_DBG_SAN_HAS_TSAN
    constexpr bool kBuiltWithThreadSanitizer = true;
#else
    constexpr bool kBuiltWithThreadSanitizer            = false;
#endif

    /**
     *  \brief kBuiltWithUndefinedBehaviorSanitizer can be checked in compiled code to see if the undfined behavior sanitizer
     *         support is compiled into this executable
     */
#if __STK_DBG_SAN_HAS_UBSAN
    constexpr bool kBuiltWithUndefinedBehaviorSanitizer = true;
#else
    constexpr bool kBuiltWithUndefinedBehaviorSanitizer = false;
#endif

/*
 *  Macro: Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
 */
#if __STK_DBG_SAN_HAS_ASAN
#if defined(__clang__)
    // using [[gnu::no_sanitize_undefined]] syntax on clang++-10 on lambdas produces warning of no_sanitize_address undefined but this seems to work?
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__ ((no_sanitize_address))
#elif defined(__GNUC__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS [[gnu::no_sanitize_address]]
#endif
#endif
#if !defined(Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif

/*
 *  Macro: Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD
 */
#if __STK_DBG_SAN_HAS_TSAN
#if defined(__clang__) || defined(__GNUC__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD [[gnu::no_sanitize_thread]]
#endif
#endif
#if !defined(Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD
#endif

/*
 *  Macro: Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED
 */
#if __STK_DBG_SAN_HAS_UBSAN
#if defined(__clang__)
#if defined(__APPLE__)
// apple clang++ 11 gives warning no_sanitize_undefined unrecognized, and also doesn't like [[]] notation
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED __attribute__ ((no_sanitize ("undefined")))
#else
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED [[gnu::no_sanitize_undefined]]
#endif
#elif defined(__GNUC__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED [[gnu::no_sanitize_undefined]]
#endif
#endif
#if !defined(Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED
#endif

    namespace Private_ {
        // REMOVE all these temporary macros, just used to simplify code inside this file
#undef __STK_DBG_SAN_HAS_ASAN
#undef __STK_DBG_SAN_HAS_TSAN
#undef __STK_DBG_SAN_HAS_UBSAN
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
