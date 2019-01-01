/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Configuration_Common_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Configuration_Common_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

// Define qPlatform_Windows (by inference from other defines)
#if !defined(qPlatform_Windows)
#if defined(_WINDOWS)
#define qPlatform_Windows 1
#endif
#endif

// Define qPlatform_Win32 and qPlatform_Win64 (by inference from other defines)
#if !defined(qPlatform_Win32) || !defined(qPlatform_Win64)
#if defined(_WIN64)
#define qPlatform_Win64 1
#define qPlatform_Win32 0
// in case only _WIN32 defined
#define qPlatform_Windows 1
#elif defined(_WIN32)
#define qPlatform_Win64 0
#define qPlatform_Win32 1
// in case only _WIN32 defined
#define qPlatform_Windows 1
#endif
#endif

#if qPlatform_Win32
#if !qPlatform_Windows
#error INCONSISTENT DEFINES
#endif
#endif

#if qPlatform_Windows

// not important, but a good default
#if !defined(STRICT)
#define STRICT
#endif

// Avoid MSFT Win32 macro which interferes with several 'standard c++' things - not just the min/max templates, but
// the numeric_limits<>::max() stuff as well!
#define NOMINMAX

#endif

// POSIX support
#if !defined(qPlatform_POSIX)
// not sure
#if qPlatform_Windows
#define qPlatform_POSIX 0
#elif defined(_POSIX_SOURCE) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// Not really sure which defines to look for --LGP 2011-09-13
#define qPlatform_POSIX 1
#else
#define qPlatform_POSIX 0
#endif
#endif

// LINUX support
#if !defined(qPlatform_Linux)
// not sure
#if qPlatform_POSIX && defined(__linux)
#define qPlatform_Linux 1
#else
#define qPlatform_Linux 0
#endif
#endif

// MacOS support (MacOSX) -
#if !defined(qPlatform_MacOS)
#if defined(__APPLE__) && defined(__MACH__)
#define qPlatform_MacOS 1
#else
#define qPlatform_MacOS 0
#endif
#endif

#if qPlatform_Windows && qPlatform_POSIX
#warning "Shouldn't have both Windows and POSIX platform flags set"
#endif

#if qPlatform_Windows && qPlatform_MacOS
#warning "Shouldn't have both Windows and MacOS platform flags set"
#endif

/*
@CONFIGVAR:     qSilenceAnnoyingCompilerWarnings
 *
 *  &note   I looked into doing a simple set of macros to hide the
 *          #if qSilenceAnnoyingCompilerWarnings && __MSVCVER \n#pragma stuff, but VC11 didn't like having
 *          the pragmas that disable warnings inside of a macro - so that won't work (I think by definition
 *          of how the C++ spec is written, that should have worked - macros should work as if separate phase
 *          before compiler ahnd then these are compiler pragmas). But if it doesn't work, cannot use it ;-)
 *          -- LGP 2012-11-14
 */
#ifndef qSilenceAnnoyingCompilerWarnings
#define qSilenceAnnoyingCompilerWarnings 1
#endif

#if qPlatform_Windows
// Too many differnt compoents have quirky dependencies about what to include in what order.
// This define helps mittigate that problem.
// (especially see MFC vs winsock, and stroikas use of winsock2, windows.h including winsock.h etc)
#define WIN32_LEAN_AND_MEAN 1
#endif // qPlatform_Windows

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_Configuration_Common_h_*/
