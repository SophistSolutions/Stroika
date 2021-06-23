/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_ODBCClient_h_
#define _Stroika_Foundation_Database_ODBCClient_h_ 1

#include "../StroikaPreComp.h"

#include "SQL/ODBCClient.h"

#pragma message("Warning: Database deprecated - use Database::SQL")
namespace Stroika::Foundation::Database {
    using Database::SQL::Exception;
#if qHasLibrary_ODBC
    using Database::SQL::DBConnection;
#endif

}

#endif /*_Stroika_Foundation_Database_ODBCClient_h_*/
