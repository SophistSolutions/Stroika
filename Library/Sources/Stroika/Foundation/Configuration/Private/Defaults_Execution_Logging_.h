/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Execution_Logging_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Execution_Logging_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

/*
@CONFIGVAR:     qHas_Syslog
@DESCRIPTION:
*/
#ifndef qHas_Syslog
#define qHas_Syslog qPlatform_POSIX
#endif

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_Execution_Logging_h_*/
