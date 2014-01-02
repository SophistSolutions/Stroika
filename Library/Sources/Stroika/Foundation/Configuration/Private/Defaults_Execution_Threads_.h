/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Execution_Exceptions_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Execution_Exceptions_h_  1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */



#if     defined(__cplusplus)


#ifndef qHas_pid_t
#define qHas_pid_t  qPlatform_POSIX
#endif



/*
 * See DOCS in Stroika/Foundation/Execution/Thread.h
 */
#if     !defined (qUseThreads_WindowsNative)
// On Windows - its quite unclear if one is better off using the stdc++ implementation (since it doesnt yet exist its hard to tell what features it
// will have) or the stdc++ implemeentation. Luckily - there is only one for unix & one for windoze now, so its clear in the very short term...
//      --LGP 2011-09-01
//
// force still using native til I have tim eto test more...
//      -- LGP 2013-03-24
//#define   qUseThreads_WindowsNative   qPlatform_Windows
#define qUseThreads_WindowsNative   0
#endif
#if     !defined (qUseThreads_StdCPlusPlus)
#define qUseThreads_StdCPlusPlus    !qUseThreads_WindowsNative
#endif


#endif


#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_Execution_Exceptions_h_*/
