/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Characters_TChar_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Characters_TChar_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

// First DEFINE qTargetPlatformSDKUseswchar_t
#ifndef qTargetPlatformSDKUseswchar_t
#if (defined(_UNICODE) || defined(UNICODE))
#define qTargetPlatformSDKUseswchar_t 1
#else
#define qTargetPlatformSDKUseswchar_t 0
#endif
#endif

// Then VALIDATE qTargetPlatformSDKUseswchar_t with respect to other common defines
#if defined(_WINDOWS)
#if qTargetPlatformSDKUseswchar_t
#if !defined(_UNICODE) || !defined(UNICODE)
#error "INCONSITENT VALS"
#endif
#else
#if defined(_UNICODE) || defined(UNICODE)
#error "INCONSITENT VALS"
#endif
#endif
#endif

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_Characters_TChar_h_*/
