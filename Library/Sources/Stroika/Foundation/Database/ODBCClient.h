/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_ODBCClient_h_
#define _Stroika_Foundation_Database_ODBCClient_h_ 1

#include "../StroikaPreComp.h"

#include "Exception.h"
#include "SQL/ODBC.h"

#pragma message("Warning: Database deprecated - use Database::SQL::ODBC::Connection")
namespace Stroika::Foundation::Database {
#if qHasLibrary_ODBC
    using Database::SQL::ODBC::DBConnection;
#endif

}

#endif /*_Stroika_Foundation_Database_ODBCClient_h_*/
