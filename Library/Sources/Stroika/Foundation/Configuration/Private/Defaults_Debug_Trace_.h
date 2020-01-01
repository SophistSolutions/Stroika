/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Debug_Trace_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Debug_Trace_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

#ifndef qTraceToFile
#define qTraceToFile 0
#endif

/*
 * See DOCS in Stroika/Foundation/Debug/Trace.h
 */
#ifndef qDefaultTracingOn
#if qDebug
#define qDefaultTracingOn 1
#else
#define qDefaultTracingOn 0
#endif
#endif

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_Debug_Trace_h_*/
