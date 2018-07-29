/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_ODBCClient_h_
#define _Stroika_Foundation_Database_ODBCClient_h_ 1

#include "../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#endif

#include <memory>
#include <string>

#include "../Execution/StringException.h"

/*
* TODO:
*          ADO + ORM!!!
*
*      o   First - in the Stroika Foundation - add a layer - like ADO - which provides database connectivity, and simple variant-record objects
*          to return values of rows from queries, and to provide safe arguments to queries (auto quoting etc).
*          > This layer must ALSO define METACLASS inforamtion about a target database. It need not (at least initially) provide and modifciation
*              functions, but must be able to read back table structure and types of fields etc.
*
*
*      o   THEN - as a totally separate (layered on top) matter, provide a FRAMEWORK LAYER "ORM" - which provides mapping to C++ objects, in a
*          maner vaguely similar to SQLAlchemy.
*          >   Define MAPPERS - which specify a correspondence between C++ struct and database 'variant objects' from above.
*
*          >   Use virtual class with SharedByValue<> rep popinters to actual C++ objects, so C++-side objects all GC'd, and can be constructed by
*              mapping layer.
*
*          >   Avoid circular dependencies by having 'set' relationships managed by central object (or faked trough mtohod calls on smart object),
*              but not throgh direct pointers (just ids).
*
*          >   SharedByValue<> important, for update semantics - cuz core mapping layer can cache/store orig objects whcih can be copied, and then 'updated'
*              by save operation.
*
*          >   COULD autogenerate templates from SQL schema, but also can use user-provided ones to generate a schema, or 'hook up' and dynamcially gneerate
*              runtime error of C++ template specification of related types doesn't match that read back from DB dynamically.
*
*
*/

/*
@CONFIGVAR:     qHasLibrary_ODBC
@DESCRIPTION:   <p>Defines if Stroika is built supporting ODBC (only do if ODBC headers in -I path)/p>
*/
#ifndef qHasLibrary_ODBC
#error "qHasLibrary_ODBC should normally be defined indirectly by StroikaConfig.h"
#endif

namespace Stroika::Foundation::Database {

    using namespace std;
    using namespace Stroika::Foundation;
    //using namespace Stroika::Foundation::Execution;
    //using namespace Stroika::Foundation::Memory;

    using Characters::String;

    class Exception : public Execution::StringException {
    public:
        Exception (const String& message);
    };
    class NoDataException : public Exception {
    public:
        NoDataException ();
    };

#if qHasLibrary_ODBC
    class DBConnection {
    private:
        class Rep;

    public:
        DBConnection (const wstring& dsn);
        virtual ~DBConnection ();

    public:
        nonvirtual unsigned int GetNestedTransactionCount () const;

    private:
        shared_ptr<Rep> fRep;
    };
#endif

#if qHasLibrary_ODBC
    // Maybe pattern this more after an 'iterator'?
    class Query {
    public:
        class AbstractColumn;

    private:
        class Rep;

    public:
        Query (DBConnection database);

    public:
        void Bind (shared_ptr<AbstractColumn>* columns, size_t numberColumns);

    public:
        void Execute (const wstring& sqlQuery);

    public:
        bool FetchRow ();

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

#endif /*_Stroika_Foundation_Database_ODBCClient_h_*/
