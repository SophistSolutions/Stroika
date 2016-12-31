/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Execution_Threads_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Execution_Threads_h_  1

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
 */
#ifndef qHas_pthread_setschedprio

#if     defined (__clang__) && defined (__APPLE__)
#define qHas_pthread_setschedprio       0
#else
#define qHas_pthread_setschedprio       qPlatform_POSIX
#endif

#endif  /*qHas_pthread_setschedprio*/


#endif  /*defined(__cplusplus)*/


#endif  /*_Stroika_Foundation_Configuration_Private_Defaults_Execution_Threads_h_*/
