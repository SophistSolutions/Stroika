/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Sanitizer_h_
#define _Stroika_Foundation_Debug_Sanitizer_h_ 1

#include "../StroikaPreComp.h"

#if qCompilerAndStdLib_sanitizer_annotate_contiguous_container_Buggy
#define __sanitizer_annotate_contiguous_container __BWA__sanitizer_annotate_contiguous_container
#endif

#if __has_include(<sanitizer/asan_interface.h>)
#include <sanitizer/asan_interface.h>
#endif
#if __has_include(<sanitizer/lsan_interface.h>)
#include <sanitizer/lsan_interface.h>
#endif
#if __has_include(<sanitizer/tsan_interface.h>)
#include <sanitizer/tsan_interface.h>
#endif

#if qCompilerAndStdLib_sanitizer_annotate_contiguous_container_Buggy
#undef __sanitizer_annotate_contiguous_container
#endif

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 */

#if defined(__cplusplus)
namespace Stroika::Foundation::Debug {

    /**
     *  Detect if Address Sanitizer is enabled in this compilation.
     *
     *      Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer
     *
     *  \note this is defined as a macro, since sometimes its very hard to use constexpr to disable bunchs of code
     *        BUT - use Debug::kBuiltWithAddressSanitizer in preference to this where you can.
     */
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer)
#if defined(__SANITIZE_ADDRESS__)
#define Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer 1
#endif
#endif
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer)
#if defined(__has_feature)
// @ see https://clang.llvm.org/docs/AddressSanitizer.html#conditional-compilation-with-has-feature-address-sanitizer
#define Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer __has_feature (address_sanitizer)
#endif
#endif
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer)
#define Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer 0
#endif

    /**
     *  \brief kBuiltWithAddressSanitizer can be checked in compiled code to see if the address sanitizer
     *         support is compiled into this executable
     */
    constexpr bool kBuiltWithAddressSanitizer = Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer;

    /*
     *  Macro: Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
     *
     *  \par Example Usage
     *      \code
     *          ToObjectMapperType<CLASS> toObjectMapper = [fields, preflightBeforeToObject] (const ObjectVariantMapper& mapper, const VariantValue& d, CLASS* intoObjOfTypeT) 
     *              Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS 
     *              -> void {...}
     *      \endcode
     */
#if Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer
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

    /**
     *  Detect if Thread Sanitizer is enabled in this compilation.
     *
     *      Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer
     *
     *  \note this is defined as a macro, since sometimes its very hard to use constexpr to disable bunchs of code
     *        BUT - use Debug::kBuiltWithThreadSanitizer in preference to this where you can.
     */
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer)
#if defined(__SANITIZE_THREAD__)
#define Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer 1
#endif
#endif
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer)
// see https://clang.llvm.org/docs/ThreadSanitizer.html#has-feature-thread-sanitizer
#if defined(__has_feature)
#define Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer __has_feature (thread_sanitizer)
#endif
#endif
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer)
#define Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer 0
#endif

    /**
     *  \brief kBuiltWithThreadSanitizer can be checked in compiled code to see if the thread sanitizer
     *         support is compiled into this executable
     */
    constexpr bool kBuiltWithThreadSanitizer = Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer;

    /**
     *  Macro: Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD
     *
     *  \par Example Usage
     *      \code
     *          #if qCompiler_ThreadSantizer_SPR_717_Buggy
     *              Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD
     *          #endif
     *          static void DoIt (void* ignored) {...}
     *      \endcode
     */
#if Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer
#if defined(__clang__) || defined(__GNUC__)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD [[gnu::no_sanitize_thread]]
#endif
#endif
#if !defined(Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD
#endif

    /**
     *  Detect if Undefined Behavior Sanitizer is enabled in this compilation.
     *
     *      Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer
     *
     *  \note this is defined as a macro, since sometimes its very hard to use constexpr to disable bunchs of code
     *        BUT - use Debug::kBuiltWithUndefinedBehaviorSanitizer in preference to this where you can.
     */
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer)
#if defined(__has_feature)
    // not documented - https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html - not sure if ever works
    // appears to work on apple XCode 11 clang, at least
#define Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer __has_feature (undefined_behavior_sanitizer)
#endif
#endif
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer)
#define Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer 0
#endif

    /**
     *  \brief kBuiltWithUndefinedBehaviorSanitizer can be checked in compiled code to see if the undfined behavior sanitizer
     *         support is compiled into this executable
     */
    constexpr bool kBuiltWithUndefinedBehaviorSanitizer = Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer;

    /**
     *  Macro: Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED
     *
     *  \par Example Usage
     *      \code
     *          #if qSomeBugFlag
     *              Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED
     *          #endif
     *              Interface GetInterfaces_POSIX_mkInterface_ (int sd, const ifreq* i) {...}
     *      \endcode
     */
#if Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer
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

}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Sanitizer.inl"

#endif /*_Stroika_Foundation_Debug_Sanitizer_h_*/
