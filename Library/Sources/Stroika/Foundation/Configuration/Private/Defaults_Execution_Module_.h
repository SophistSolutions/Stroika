/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Execution_Module_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Execution_Module_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

#ifndef qSupport_Proc_Filesystem
#if defined(__linux__)
#define qSupport_Proc_Filesystem 1
#else
#define qSupport_Proc_Filesystem 0
#endif
#endif

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_Execution_Module_h_*/
