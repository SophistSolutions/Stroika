/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Private_Platform_h_
#define _Stroika_Foundation_Common_Private_Platform_h_ 1

// @todo PROBABLY MOVE MOST OF THIS FILE TO Common/Platform/Common.h
// and document/segarate/rename qPlatform defines...
// qStroika_Foundation_Common_Platform_Windows etc...


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
#endif
#endif

// LINUX support
#if !defined(qPlatform_Linux)
// not sure
#if qPlatform_POSIX && defined(__linux)
#define qPlatform_Linux 1
#endif
#endif

// MacOS support (MacOSX) -
#if !defined(qPlatform_MacOS)
#if defined(__APPLE__) && defined(__MACH__)
#define qPlatform_MacOS 1
#endif
#endif

// Have all these default to 0, if not otherwise defined, so you can use if constexpr (qPlatform_Linux), etc
#if !qPlatform_MacOS
#define qPlatform_MacOS 0
#endif
#if !qPlatform_Windows
#define qPlatform_Windows 0
#endif
#if !qPlatform_Linux
#define qPlatform_Linux 0
#endif
#if !qPlatform_POSIX
#define qPlatform_POSIX 0
#endif

#if qPlatform_Windows && qPlatform_POSIX
#warning "Shouldn't have both Windows and POSIX platform flags set"
#endif

#if qPlatform_Windows && qPlatform_MacOS
#warning "Shouldn't have both Windows and MacOS platform flags set"
#endif

#if qPlatform_Windows
// Too many different components have quirky dependencies about what to include in what order.
// This define helps mitigate that problem.
// (especially see MFC vs winsock, and stroikas use of winsock2, windows.h including winsock.h etc)
#define WIN32_LEAN_AND_MEAN 1
#endif // qPlatform_Windows

#ifndef qSupport_Proc_Filesystem
#if defined(__linux__)
#define qSupport_Proc_Filesystem 1
#else
#define qSupport_Proc_Filesystem 0
#endif
#endif

#ifndef qHas_pid_t
#define qHas_pid_t qPlatform_POSIX
#endif

#endif /*_Stroika_Foundation_Common_Private_Platform_h_*/
