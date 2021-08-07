/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_ODBCClient_h_
#define _Stroika_Foundation_Database_SQL_ODBCClient_h_ 1

#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#endif

#include <memory>
#include <string>

#include "../../Execution/Exceptions.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   ADD Sample/Regression test support for this
 *              make sure working UNIX/Windows
 *              see also maybe http://www.unixodbc.org/
 *      @todo   Refactor to be layered on top of Database::SQL::Connection/Statement etc - so works with ORM code
 *      @todo   Perhaps add functionality to read back table structure (schema) from the database
 *
 */

/*
@CONFIGVAR:     qHasLibrary_ODBC
@DESCRIPTION:   <p>Defines if Stroika is built supporting ODBC (only do if ODBC headers in -I path)/p>
*/
#ifndef qHasLibrary_ODBC
#error "qHasLibrary_ODBC should normally be defined indirectly by StroikaConfig.h"
#endif

namespace Stroika::Foundation::Database::SQL {

    using namespace std;

#if qHasLibrary_ODBC
    class DBConnection {
    private:
        class Rep;

    public:
        DBConnection (const wstring& dsn);
        virtual ~DBConnection () = default;

    public:
        nonvirtual unsigned int GetNestedTransactionCount () const;

    private:
        shared_ptr<Rep> fRep;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Database_SQL_ODBCClient_h_*/
