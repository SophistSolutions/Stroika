/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Configuration_Common_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Configuration_Common_h_  1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */


// We should automate detecing this, but I don't know any portable way todo so at compile time - just runtime.
#if     !defined (qEndian_Little) && !defined (qEndian_Big)
#if     defined (_LITTLE_ENDIAN_)
#define qEndian_Little      1
#elif   defined (_BIG_ENDIAN_)
#define qEndian_Big     1
#elif   defined(__hppa__)
#define qEndian_Big     1
#elif   defined(__m68k__) || defined(mc68000) || defined(_M_M68K)
#define qEndian_Big     1
#elif   defined(__MIPS__) && defined(__MISPEB__)
#define qEndian_Big     1
#elif   defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC)
#define qEndian_Big     1
#elif   defined(__sparc__)
#define qEndian_Big     1
#else
// DEFAULT
#define qEndian_Little      1
#endif

// Be sure other defined
#if     qEndian_Little
#define qEndian_Big         0
#elif   qEndian_Big
#define qEndian_Little      0
#else
#error  INCONSISTENT DEFINES
#endif
#endif


// Define qPlatform_Windows (by inference from other defines)
#if     !defined (qPlatform_Windows)
#if     defined (_WINDOWS)
#define qPlatform_Windows   1
#endif
#endif

// Define qPlatform_Win32 and qPlatform_Win64 (by inference from other defines)
#if     !defined (qPlatform_Win32) || !defined (qPlatform_Win64)
#if     defined (_WIN64)
#define qPlatform_Win64     1
#define qPlatform_Win32     0
// in case only _WIN32 defined
#define qPlatform_Windows   1
#elif       defined (_WIN32)
#define qPlatform_Win64     0
#define qPlatform_Win32     1
// in case only _WIN32 defined
#define qPlatform_Windows   1
#endif
#endif


#if     qPlatform_Win32
#if     !qPlatform_Windows
#error  INCONSISTENT DEFINES
#endif
#endif



#if     qPlatform_Windows

// not important, but a good default
#if     !defined (STRICT)
#define STRICT
#endif

/*
 *  See http://msdn2.microsoft.com/en-us/library/aa383745.aspx
 *
 *  ...
 *      Windows Vista/Windows Server 2008   _WIN32_WINNT>=0x0600
 *                  WINVER>=0x0600
 *
 *  ...
 *      Windows Server 2003     _WIN32_WINNT>=0x0502
 *                  WINVER>=0x0502
 *  ...
 *      Windows XP  _WIN32_WINNT>=0x0501
 *                  WINVER>=0x0501
 *  ...
 *      Windows Me  _WIN32_WINDOWS=0x0500
 *                  WINVER>=0x0500
 *
 *      Internet Explorer 6.0
 *                  _WIN32_IE>=0x0600
 */

// We COULD easily use an earlier build (was using 0x501 until 2011-02-22) - but use 0x502 because
// MyGetThreadId () can be more efficient using builtin version if we define 0x502 - and no real reason not to...
//      -- LGP 2011-02-22
//
// I THINK above comment is wrong - cuz then we break running on WinXP. Instead - just define 0x0502 as min for 64bit where we need its GetThreadID
#if     qPlatform_Win32
#if     !defined (WINVER)
#define WINVER          0x0501
#endif
#if     !defined (_WIN32_WINNT)
#define _WIN32_WINNT    0x0501
#endif
#if     !defined (_WIN32_WINDOWS)
#define _WIN32_WINDOWS  0x0501
#endif
#if     !defined (_WIN32_IE)
#define _WIN32_IE       0x0600
#endif
#elif   qPlatform_Win64
#if     !defined (WINVER)
#define WINVER          0x0502
#endif
#if     !defined (_WIN32_WINNT)
#define _WIN32_WINNT    0x0502
#endif
#if     !defined (_WIN32_WINDOWS)
#define _WIN32_WINDOWS  0x0502
#endif
#if     !defined (_WIN32_IE)
#define _WIN32_IE       0x0600
#endif
#endif


// Avoid MSFT Win32 macro which interferes with several 'standard c++' things - not just the min/max templates, but
// the numeric_limits<>::max() stuff as well!
#define NOMINMAX

#endif


#ifndef qPlatform_MacOS
#define qPlatform_MacOS      defined (macintosh)
#endif



// POSIX support
#if     !defined (qPlatform_POSIX)
// not sure
#if     qPlatform_Windows
#define qPlatform_POSIX 0
#elif   defined (_POSIX_SOURCE) || defined (__unix__)
// Not really sure which defines to look for --LGP 2011-09-13
#define qPlatform_POSIX 1
#else
#define qPlatform_POSIX 0
#endif
#endif


#if     qPlatform_Windows && qPlatform_POSIX
#warning "Shouldn't have both Windows and POSIX platform flags set"
#endif

#if     qPlatform_Windows && qPlatform_MacOS
#warning "Shouldn't have both Windows and MacOS platform flags set"
#endif




#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_Configuration_Common_h_*/
