/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Private_Defaults_Database_ODBCClient_h_
#define _Stroika_Foundation_Configuration_Private_Defaults_Database_ODBCClient_h_ 1

/*
 * This file is a private Stroika implementation detail, and shouldn't be included directly.
 * Its used by StroikaConfig.h - optionally included there -
 * to help implement the set of Stroika public configuration defines.
 */

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

#endif /*_Stroika_Foundation_Configuration_Private_Defaults_Database_ODBCClient_h_*/
