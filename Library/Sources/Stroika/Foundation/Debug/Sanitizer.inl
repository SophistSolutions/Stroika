/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#if defined(__cplusplus)
namespace Stroika::Foundation::Debug {
}
#endif

/*
 ********************************************************************************
 ***************************** DEPRECATED (v3.0d25) *****************************
 ********************************************************************************
 */

/*
 *  Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer, ..._HAS_ThreadSanitizer, ..._HAS_UndefinedBehaviorSanitizer
 *      *** DEPRECATED IN STROIKA 3.0d25 *** (to be removed at the end of the 3.0d development period)
 *
 *  Use Debug::kBuiltWithAddressSanitizer, kBuiltWithThreadSanitizer or kBuiltWithUndefinedBehaviorSanitizer instead; where an
 *  #if needs the answer, copy the compile-time test documented on each (@see Sanitizer.h).
 *
 *  \note Stroika no longer uses these, so defining them (e.g. with -D) no longer affects anything else.
 *  \note Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer is 0 on g++ before 14 even in an undefined behavior
 *        sanitizer build - there is no compile-time way to tell there (Debug::kBuiltWithUndefinedBehaviorSanitizer is right).
 */
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer)
#if defined(__SANITIZE_ADDRESS__)
#define Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer 1
#elif defined(__has_feature)
#define Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer __has_feature (address_sanitizer)
#else
#define Stroika_Foundation_Debug_Sanitizer_HAS_AddressSanitizer 0
#endif
#endif
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer)
#if defined(__SANITIZE_THREAD__)
#define Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer 1
#elif defined(__has_feature)
#define Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer __has_feature (thread_sanitizer)
#else
#define Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer 0
#endif
#endif
#if !defined(Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer)
#if defined(__has_feature)
#define Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer __has_feature (undefined_behavior_sanitizer)
#else
#define Stroika_Foundation_Debug_Sanitizer_HAS_UndefinedBehaviorSanitizer 0
#endif
#endif
