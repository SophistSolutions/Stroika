/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Private_ThirdPartyComponents_h_
#define _Stroika_Foundation_Common_Private_ThirdPartyComponents_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - to help implement the set of Stroika public configuration defines.
 */

#ifndef qStroika_HasComponent_xerces
#define qStroika_HasComponent_xerces 0
#endif

/*
@CONFIGVAR:     qStroika_HasComponent_syslog
@DESCRIPTION:
*/
#ifndef qStroika_HasComponent_syslog
#define qStroika_HasComponent_syslog qStroika_Foundation_Common_Platform_POSIX
#endif

/*
 * See DOCS in Stroika/Foundation/Cryptography/Common.h
 */
#if !defined(qStroika_HasComponent_OpenSSL)
#define qStroika_HasComponent_OpenSSL 0
#endif

/*
 * See DOCS in Stroika/Foundation/Database/ODBCClient.h
 */
#ifndef qStroika_HasComponent_ODBC
#if defined(_WINDOWS)
#define qStroika_HasComponent_ODBC 1
#else
#define qStroika_HasComponent_ODBC 0
#endif
#endif

// DEPRECATED NAMES - BACKWARD COMPAT FROM PRE v3.0d11 - until v3.0a1 --LGP 2024-10-23
#define qHas_Syslog qStroika_HasComponent_syslog
#define qHasLibrary_ODBC qStroika_HasComponent_ODBC
#define qHasFeature_sqlite qStroika_HasComponent_sqlite
#define qHasFeature_OpenSSL qStroika_HasComponent_OpenSSL
#define qHasFeature_Xerces qStroika_HasComponent_xerces
#define qHasFeature_ATLMFC qStroika_HasComponent_ATLMFC
#define qHasFeature_boost qStroika_HasComponent_boost
#define qHasFeature_LibCurl qStroika_HasComponent_libcurl
#define qHasFeature_fmtlib qStroika_HasComponent_fmtlib
#define qHasFeature_GoogleTest qStroika_HasComponent_googletest
#define qHasFeature_LZMA qStroika_HasComponent_LZMA
#define qHasFeature_WinHTTP qStroika_HasComponent_WinHTTP
#define qHasFeature_libxml2 qStroika_HasComponent_libxml2

#endif /*_Stroika_Foundation_Common_Private_ThirdPartyComponents_h_*/
