/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_Sanitizer_h_
#define _Stroika_Foundation_Debug_Sanitizer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if __has_include(<sanitizer/asan_interface.h>)
#include <sanitizer/asan_interface.h>
#endif
#if __has_include(<sanitizer/lsan_interface.h>)
#include <sanitizer/lsan_interface.h>
#endif
#if __has_include(<sanitizer/tsan_interface.h>)
#include <sanitizer/tsan_interface.h>
#endif

#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 14
// @see Debug::kBuiltWithUndefinedBehaviorSanitizer
extern "C" void __attribute__ ((weak)) __ubsan_handle_builtin_unreachable ();
#endif

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  Is this built WITH a sanitizer? Debug::kBuiltWithAddressSanitizer, kBuiltWithThreadSanitizer and
 *  kBuiltWithUndefinedBehaviorSanitizer are the answers. (Which sanitizers it COULD be built with is a question about the
 *  toolchain and configuration - @see configure --sanitize.)
 *
 *  Stroika deliberately defines no macro for this. Where you need the answer at compile time - in an #if - use the test
 *  documented on each variable, which is exactly the test that variable is defined with. None of them is ISO C++:
 *      o   __SANITIZE_ADDRESS__ and __SANITIZE_THREAD__ are a g++ convention, which MSVC follows for the address sanitizer,
 *          and clang does from clang 22.
 *      o   __has_feature is a clang extension, which g++ adopted in g++ 14; MSVC has none (as of MSVC 19.51, VS 2026).
 *      o   Nothing defines __SANITIZE_UNDEFINED__ (as of g++ 16, clang 22 and MSVC 19.51).
 *
 *  \note TRAP: g++ before 14 ships a <sanitizer/common_interface_defs.h> - included by the <sanitizer/...> headers above - which
 *        defines a stand-in __has_feature(x) as 0. So after including any sanitizer header, defined(__has_feature) is true there
 *        too, and says nothing about whether the compiler has it: to single out those g++, test the version (__GNUC__ < 14).
 *        (The tests documented below still work there - they just cannot say yes, for the undefined behavior sanitizer.)
 *
 *  So a test must allow for __has_feature not existing - and "#if defined(__has_feature) && __has_feature (x)" does NOT:
 *  where it does not exist, the second half is still a syntax error. Nest the #if, as each variable's test does.
 *
 *  What each compiler provides, at compile time (as of g++ 16, clang 22, Apple clang 17 and MSVC 19.51):
 *
 *                                          address                 thread                  undefined behavior
 *      g++ before 14                       __SANITIZE_ADDRESS__    __SANITIZE_THREAD__     (nothing)
 *      g++ 14 and later                    both                    both                    __has_feature
 *      clang before 22 (and Apple clang)   __has_feature           __has_feature           __has_feature
 *      clang 22 and later                  both                    both                    __has_feature
 *      MSVC                                __SANITIZE_ADDRESS__    (no such sanitizer)     (no such sanitizer)
 *
 *  The Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_* macros are the one exception, since an attribute cannot be a
 *  variable. Each is defined for every compiler that has the attribute, whether or not built with that sanitizer - because
 *  a compiler ignores the attribute when not. (Were that not so, each definition would go inside its sanitizer's
 *  compile-time test.) All use the one generic attribute, no_sanitize ("address" / "thread" / "undefined" ...), wherever
 *  __has_attribute (no_sanitize) says the compiler has it (g++ from 8, clang) - the way boost and zstd do - plus MSVC's
 *  own __declspec (no_sanitize_address) for its one sanitizer.
 */

#if defined(__cplusplus)
namespace Stroika::Foundation::Debug {

    /**
     *  \brief true iff this is built with the address sanitizer
     *
     *  Compile-time equivalent - exactly the test this is defined with:
     *      \code
     *          #if defined(__SANITIZE_ADDRESS__)
     *              ... built with it
     *          #elif defined(__has_feature)
     *          #if __has_feature (address_sanitizer)
     *              ... built with it
     *          #endif
     *          #endif
     *      \endcode
     *
     *  @see https://clang.llvm.org/docs/AddressSanitizer.html#conditional-compilation-with-has-feature-address-sanitizer
     */
#if defined(__SANITIZE_ADDRESS__)
    constexpr bool kBuiltWithAddressSanitizer = true;
#elif defined(__has_feature)
    constexpr bool kBuiltWithAddressSanitizer = __has_feature (address_sanitizer);
#else
    constexpr bool kBuiltWithAddressSanitizer = false;
#endif

    /*
     *  Macro: Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
     *
     *  \note Defined whether or not built with the address sanitizer (a compiler ignores it when not) - @see the file comment
     *
     *  \par Example Usage
     *      \code
     *          #if qCompilerAndStdLib_arm_asan_FaultStackUseAfterScope_Buggy
     *              Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
     *          #endif
     *          void Debug::Private_::Emitter::DoEmit_ (const wchar_t* p, const wchar_t* e) noexcept
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // OLD EXAMPLE - NOT SURE THIS SYNTAX WORKS - MAYBE HAS TO GO BEFORE [] not after function()??
     *          ToObjectMapperType<CLASS> toObjectMapper = [fields, preflightBeforeToObject] (const ObjectVariantMapper& mapper, const VariantValue& d, CLASS* intoObjOfTypeT)
     *              Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
     *              -> void {...}
     *      \endcode
     */
#if defined(__has_attribute)
#if __has_attribute(no_sanitize)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS __attribute__ ((no_sanitize ("address")))
#endif
#endif
#if !defined(Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS)
#if defined(_MSC_VER)
// The one exception to the no_sanitize ("...") spelling: MSVC has no __has_attribute and no generic no_sanitize attribute
// (as of MSVC 19.51, VS 2026), but it does have an address sanitizer, and this is its spelling. (clang-cl has
// __has_attribute, so takes the branch above.)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS __declspec (no_sanitize_address)
#else
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_ADDRESS
#endif
#endif

    /**
     *  \brief true iff this is built with the thread sanitizer
     *
     *  Compile-time equivalent - exactly the test this is defined with:
     *      \code
     *          #if defined(__SANITIZE_THREAD__)
     *              ... built with it
     *          #elif defined(__has_feature)
     *          #if __has_feature (thread_sanitizer)
     *              ... built with it
     *          #endif
     *          #endif
     *      \endcode
     *
     *  @see https://clang.llvm.org/docs/ThreadSanitizer.html#has-feature-thread-sanitizer
     */
#if defined(__SANITIZE_THREAD__)
    constexpr bool kBuiltWithThreadSanitizer = true;
#elif defined(__has_feature)
    constexpr bool kBuiltWithThreadSanitizer = __has_feature (thread_sanitizer);
#else
    constexpr bool kBuiltWithThreadSanitizer = false;
#endif

    /**
     *  Macro: Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD
     *
     *  \note Defined whether or not built with the thread sanitizer (a compiler ignores it when not) - @see the file comment
     *
     *  \par Example Usage
     *      \code
     *          #if qCompiler_ThreadSantizer_SPR_717_Buggy
     *              Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD
     *          #endif
     *          static void DoIt (void* ignored) {...}
     *      \endcode
     */
#if defined(__has_attribute)
#if __has_attribute(no_sanitize)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD __attribute__ ((no_sanitize ("thread")))
#endif
#endif
#if !defined(Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_THREAD
#endif

    /**
     *  \brief true iff this is built with the undefined behavior sanitizer
     *
     *  Compile-time equivalent, where there is one:
     *      \code
     *          #if defined(__has_feature)
     *          #if __has_feature (undefined_behavior_sanitizer)
     *              ... built with it
     *          #endif
     *          #endif
     *      \endcode
     *
     *  But g++ before 14 has NO compile-time way to tell: nothing defines __SANITIZE_UNDEFINED__ (as of g++ 16 and clang 22)
     *  (https://github.com/google/sanitizers/issues/765), and those g++ have no real __has_feature (@see the TRAP in the file
     *  comment - there the test above just says no). There, this is found at run time, from whether the sanitizer's runtime is
     *  linked in - reliable on g++, whose asan and tsan runtimes do not include it as of g++ 16 (clang's do, but clang always has
     *  __has_feature). So there it is a const, not a constexpr: test it with if, not if constexpr.
     *
     *  \note HAZARD before main: where it is found at run time, it is set by dynamic initialization. That comes before a
     *        namespace-scope object's initialization only if the object's translation unit includes this header before
     *        defining it; otherwise that object's initializer can read this too early, and see false.
     */
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 14
    inline const bool kBuiltWithUndefinedBehaviorSanitizer = &__ubsan_handle_builtin_unreachable != nullptr;
#elif defined(__has_feature)
    constexpr bool kBuiltWithUndefinedBehaviorSanitizer = __has_feature (undefined_behavior_sanitizer);
#else
    constexpr bool kBuiltWithUndefinedBehaviorSanitizer = false; // MSVC has no undefined behavior sanitizer (as of MSVC 19.51)
#endif

    /**
     *  Macro: Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED
     *
     *  \note Defined whether or not built with the undefined behavior sanitizer (a compiler ignores it when not) - @see the
     *        file comment. Which also means it applies on g++ before 14, where it cannot be told at compile time.
     *
     *  \par Example Usage
     *      \code
     *          #if qSomeBugFlag
     *              Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED
     *          #endif
     *              Interface GetInterfaces_POSIX_mkInterface_ (int sd, const ifreq* i) {...}
     *      \endcode
     */
#if defined(__has_attribute)
#if __has_attribute(no_sanitize)
#define Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE_UNDEFINED __attribute__ ((no_sanitize ("undefined")))
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
