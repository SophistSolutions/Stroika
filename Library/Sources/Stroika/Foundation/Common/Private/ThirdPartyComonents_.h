/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Private_ThirdPartyComponents_h_
#define _Stroika_Foundation_Common_Private_ThirdPartyComponents_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */
#ifndef qHasFeature_Xerces
#define qHasFeature_Xerces 0
#endif

/*
@CONFIGVAR:     qHas_Syslog
@DESCRIPTION:
*/
#ifndef qHas_Syslog
#define qHas_Syslog qStroika_Foundation_Common_Platform_POSIX
#endif

/*
 * See DOCS in Stroika/Foundation/Cryptography/Common.h
 */
#if !defined(qHasFeature_OpenSSL)
#define qHasFeature_OpenSSL 0
#endif

/*
 * See DOCS in Stroika/Foundation/Database/ODBCClient.h
 */
#ifndef qHasLibrary_ODBC
#if defined(_WINDOWS)
#define qHasLibrary_ODBC 1
#else
#define qHasLibrary_ODBC 0
#endif
#endif

#endif /*_Stroika_Foundation_Common_Private_ThirdPartyComponents_h_*/
