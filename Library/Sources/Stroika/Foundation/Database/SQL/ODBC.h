/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_ODBCClient_h_
#define _Stroika_Foundation_Database_SQL_ODBCClient_h_ 1

#include "../../StroikaPreComp.h"

#include "Connection.h"
#include "Statement.h"
#include "Transaction.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   ADD Sample/Regression test support for this
 *              make sure working UNIX/Windows
 *              see also maybe http://www.unixodbc.org/
 *      @todo   Largely NYI (need statement code to do much of anything)
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

namespace Stroika::Foundation::Database::SQL::ODBC {

    using namespace std;

#if qHasLibrary_ODBC
    /**
     *  These are options used to create a database Connection::Ptr object (with Connection::New).
     *
     *  Since this is also how you create a database, in a sense, its those options too.
     */
    struct Options final {
        /**
         */
        optional<String> fDSN;
    };

    /**
     *  'Connection' is a quasi-namespace.
     */
    class Connection : SQL::Connection {
    public:
        class IRep;

    public:
        class Ptr;

    private:
        struct Rep_;

    public:
        /**
         *  Quasi-namespace class - don't construct - construct a Connection:::Ptr class with Connection::New ()
         */
        Connection () = delete;

    public:
        /**
         *  The dbInitializer is called IFF the New () call results in a newly created database (@todo RECONSIDER).
         */
        static Ptr New (const Options& options);
    };

    /**
     *  Connection provides an API for accessing an ODBC database.
     * 
     *  Typically don't use this directly, but use Connecion::Ptr, a smart ptr wrapper on this interface.
     */
    class Connection::IRep : public SQL::Connection::IRep {
    private:
        friend class Ptr;
    };

    /**
     *  Connection provides an API for accessing an ODBC database.
     *
     *  A new Connection::Ptr is typically created ODBC::Connection::New()
     */
    class Connection::Ptr : public SQL::Connection::Ptr {
    private:
        using inherited = SQL::Connection::Ptr;

    public:
        /**
         */
        Ptr (const Ptr& src);
        Ptr (const shared_ptr<IRep>& src = nullptr);

    public:
        ~Ptr () = default;

    public:
        /**
         */
        nonvirtual Ptr& operator= (const Ptr& src);
        nonvirtual Ptr& operator= (Ptr&& src) noexcept;

    public:
        /**
         */
        nonvirtual IRep* operator-> () const noexcept;
    };

    /**
     */
    class Statement : public SQL::Statement {
    private:
        using inherited = SQL::Statement;

    public:
        /**
         */
        Statement () = delete;
        Statement (const Connection::Ptr& db, const String& query);
        Statement (const Statement&) = delete;

    private:
        struct MyRep_;
    };

    /**
     */
    class Transaction : public SQL::Transaction {
    private:
        using inherited = SQL::Transaction;

    public:
        /**
         */
        Transaction () = delete;
        Transaction (const Connection::Ptr& db);
        Transaction (const Transaction&) = delete;

    private:
        struct MyRep_;
    };
#endif

#if qHasLibrary_ODBC
    class [[deprecated ("Since Stroika 2.1b13 use Connection::New")]] DBConnection
    {
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
#include "ODBC.inl"

#endif /*_Stroika_Foundation_Database_SQL_ODBCClient_h_*/
