/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Platform_Common_h_
#define _Stroika_Foundation_Common_Platform_Common_h_ 1

/**
 *  Platforms (not always mutually exclusive)
 *      o   qStroika_Foundation_Common_Platform_Windows
 *      o   qStroika_Foundation_Common_Platform_Win32       (implies qStroika_Foundation_Common_Platform_Windows)
 *      o   qStroika_Foundation_Common_Platform_Win64       (implies qStroika_Foundation_Common_Platform_Windows)
 *      o   qStroika_Foundation_Common_Platform_POSIX
 *      o   qStroika_Foundation_Common_Platform_Linux       (implies qStroika_Foundation_Common_Platform_POSIX)
 *      o   qStroika_Foundation_Common_Platform_MacOS       (implies qStroika_Foundation_Common_Platform_POSIX)
 */


/**
 *  \brief qStroika_Foundation_Common_Platform_Windows true iff compiling targeting the Windows platform (Win32 or Win64)
 */
#ifndef qStroika_Foundation_Common_Platform_Windows
#if defined(_WINDOWS) || defined(_WIN64) || defined(_WIN32)
#define qStroika_Foundation_Common_Platform_Windows 1
#else
#define qStroika_Foundation_Common_Platform_Windows 0
#endif
#endif

/**
 *  \brief qStroika_Foundation_Common_Platform_Win32
 */
#if defined(_WIN32) && !defined(_WIN64)
#define qStroika_Foundation_Common_Platform_Win32 1
#else
#define qStroika_Foundation_Common_Platform_Win32 0
#endif

/**
 *  \brief qStroika_Foundation_Common_Platform_Win64
 */
#if defined(_WIN64)
#define qStroika_Foundation_Common_Platform_Win64 1
#else
#define qStroika_Foundation_Common_Platform_Win64 0
#endif

/**
 *  \brief qStroika_Foundation_Common_Platform_POSIX
 */
#ifndef qStroika_Foundation_Common_Platform_POSIX
// Not really sure which defines to look for --LGP 2011-09-13
#if defined(_POSIX_SOURCE) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__linux)
#define qStroika_Foundation_Common_Platform_POSIX 1
#else
#define qStroika_Foundation_Common_Platform_POSIX 0
#endif
#endif

/**
 *  \brief qStroika_Foundation_Common_Platform_Linux
 */
#ifndef qStroika_Foundation_Common_Platform_Linux
#if defined(__linux)
#define qStroika_Foundation_Common_Platform_Linux 1
#else
#define qStroika_Foundation_Common_Platform_Linux 0
#endif
#endif

/**
 *  \brief qStroika_Foundation_Common_Platform_MacOS - MacOS X
 */                                                                                                                                       \
#ifndef qStroika_Foundation_Common_Platform_MacOS
#if defined(__APPLE__) && defined(__MACH__)
#define qStroika_Foundation_Common_Platform_MacOS 1
#else
#define qStroika_Foundation_Common_Platform_MacOS 0
#endif
#endif

/*
 *  Sanity/consistency check for platform defines
 */
#ifndef RC_INVOKED
#if qStroika_Foundation_Common_Platform_Windows != (qStroika_Foundation_Common_Platform_Win32 || qStroika_Foundation_Common_Platform_Win64)
#warning "INCONSISTENT DEFINES"
#endif
#if qStroika_Foundation_Common_Platform_Windows && qStroika_Foundation_Common_Platform_POSIX
#warning "Shouldn't have both Windows and POSIX platform flags set"
#endif
#if qStroika_Foundation_Common_Platform_Windows && qStroika_Foundation_Common_Platform_MacOS
#warning "Shouldn't have both Windows and MacOS platform flags set"
#endif
#endif

/*
 *  Defines to tune other #includes - based on platform
 */
#ifndef RC_INVOKED
#if qStroika_Foundation_Common_Platform_Windows

// not important, but a good default
#if !defined(STRICT)
#define STRICT
#endif

// Avoid MSFT Win32 macro which interferes with several 'standard c++' things - not just the min/max templates, but
// the numeric_limits<>::max() stuff as well!
#define NOMINMAX

// Too many different components have quirky dependencies about what to include in what order.
// This define helps mitigate that problem.
// (especially see MFC vs winsock, and Stroika's use of winsock2, windows.h including winsock.h etc)
#define WIN32_LEAN_AND_MEAN 1

#endif
#endif

//? DERIVITIVE DEFINES
// @todo reconsider this define
#ifndef qHas_pid_t
#define qHas_pid_t Stroika_Foundation_Common_Platform_POSIX
#endif
#ifndef qSupport_Proc_Filesystem
#if defined(__linux__)
#define qSupport_Proc_Filesystem 1
#else
#define qSupport_Proc_Filesystem 0
#endif
#endif

// #DEPRECATED DEFINES - DEPRECATED since 3.0d11
#ifndef RC_INVOKED
#define qPlatform_Windows qStroika_Foundation_Common_Platform_Windows
#define qPlatform_Win32 qStroika_Foundation_Common_Platform_Win32
#define qPlatform_Win64 qStroika_Foundation_Common_Platform_Win64
#define qPlatform_POSIX qStroika_Foundation_Common_Platform_POSIX
#define qPlatform_Linux qStroika_Foundation_Common_Platform_Linux
#define qPlatform_MacOS qStroika_Foundation_Common_Platform_MacOS
#endif

// FOR RC_INVOKED, but defined always so can be used in mixed use code as well
#if defined(_WINDOWS) || defined(_WIN64) || defined(_WIN32)
#define qRCBWA_Platform_Windows 1
#else
#define qRCBWA_Platform_Windows 0
#endif

#endif /*_Stroika_Foundation_Common_Platform_Common_h_*/
