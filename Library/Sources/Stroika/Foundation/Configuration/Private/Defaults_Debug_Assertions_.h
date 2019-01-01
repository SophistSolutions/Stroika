/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Execution_Assertions_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Execution_Assertions_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

/*
 *  Assume the define _DEBUG is used throughout the code to indicate DEBUG mode (assertions on). Assure NDEBUG flag
 *  is set consistently (even if its not explicitly checked).
 */
#if !defined(qDebug)
#if defined(_DEBUG)
#define qDebug 1
#elif defined(NDEBUG)
#define qDebug 0
#else
// The <cassert> convention is that if NDEBUG is defined, we turn off debugging, but if nothing like it is defined, we
// turn on assertions by default. The caller can simply specify qDebug to prevent this defaulting if desired.
#define qDebug 1
#endif
#endif

// Check for consistent defines
#if qDebug
#if defined(NDEBUG)
// NB #warning is a non-standard extension - maybe we shouldnt use?
static_assert (false, "INCONSISTENT DEFINES (NDEBUG and qDebug=1)");
#endif
#else
#if defined(_DEBUG)
// NB #warning is a non-standard extension - maybe we shouldnt use?
static_assert (false, "INCONSISTENT DEFINES (_DEBUG and qDebug=0)");
#endif
#endif

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_Execution_Assertions_h_*/
