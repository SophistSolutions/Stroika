/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_Connection_h_
#define _Stroika_Foundation_Database_SQL_Connection_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Characters/String.h"
#include "../../Common/Property.h"
#include "../../Debug/AssertExternallySynchronizedMutex.h"

#include "EngineProperties.h"

/**
 *  \file
 * 
 *  The point of this module is to define a Connection abtraction that can be used for different kinds
 *  of connections (e.g. SQLite local database object, and ODBC remote database object). This generic API
 *  can then be used in places where either backend database might be in use.
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 */

namespace Stroika::Foundation::Database::SQL {

    using Characters::String;

    class Transaction;
    class Statement;

    /**
     *  'Connection' is a quasi-namespace.
     */
    class Connection {
    public:
        class IRep;

    public:
        class Ptr;

    public:
        /**
         *  Quasi-namespace class - don't construct - construct a SOME_CONNECTION_SUBCLASS::New ()
         * 
         *  \see SQLite::Connection::New ()
         */
        Connection () = delete;
    };

    /**
     *  Connection::IRep provides an (abstract) API for accessing an SQL database.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *          But though each connection can only be accessed from a single thread at a time, the underlying database may be
     *          threadsafe (even if accessed across processes).
     */
    class Connection::IRep : protected Debug::AssertExternallySynchronizedMutex, public enable_shared_from_this<Connection::IRep> {
    public:
        /**
         */
        virtual ~IRep () = default;

    public:
        /**
         *  The return the EngineProperties of the database being talked to via the Connection::Ptr.
         * 
         *  \note for some Connections, this can change dynamically (like ODBC depending on connection state)
         */
        virtual shared_ptr<const EngineProperties> GetEngineProperties () const = 0;

    public:
        /**
         *  Statement object factory
         */
        virtual Statement mkStatement (const String& sql) = 0;

    public:
        /**
         *  Transaction object factory
         */

        virtual Transaction mkTransaction () = 0;

    public:
        /**
         *  This returns nothing, but raises exceptions on errors.
         */
        virtual void Exec (const String& sql) = 0;

    private:
        friend class Ptr;
    };

    /**
     *  Connection::Ptr provides an API for accessing an SQLite database.
     * 
     *  A new Connection::Ptr is typically created with SOME_SERVICE::Connection::New () (e..g SQLite::Connection::New())
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *          But though each connection can only be accessed from a single thread at a time, the underlying database may be
     *          threadsafe (even if accessed across processes).
     *
     *          The Connection::Ptr itself is standardC++ thread safety. The thread-safety of the underlying database depends on how the underlying
     *          shared_ptr<IRep> was created.
     */
    class Connection::Ptr : private Debug::AssertExternallySynchronizedMutex {
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

    public:
        /**
         *  The return the EngineProperties of the database being talked to via the Connection::Ptr.
         * 
         *  \note for some Connections, this can change dynamically (like ODBC depending on connection state)
         */
        nonvirtual shared_ptr<const EngineProperties> GetEngineProperties () const;

    public:
        /**
         *  Statement object factory
         * 
         *  The reason you might use this instead of SQLite::Statement{} - is in writing generic code
         *  that doesn't depend on the particular kind of SQL database you are connected to (e.g. that
         *  might be used for ODBC or SQLite).
         */
        nonvirtual Statement mkStatement (const String& sql);

    public:
        /**
         *  Transaction object factory
         * 
         *  The reason you might use this instead of SQLite::Transaction{} - is in writing generic code
         *  that doesn't depend on the particular kind of SQL database you are connected to (e.g. that
         *  might be used for ODBC or SQLite).
         */
        nonvirtual Transaction mkTransaction ();

    public:
        /**
         *  This returns nothing, but raises exceptions on errors.
         *
         *  \todo - EXTEND this to write the RESPONSE (use the callback) to DbgTrace () calls - perhaps optionally?)
         */
        nonvirtual void Exec (const String& sql) const;

#if qDebug
    public:
        /**
         */
        nonvirtual shared_ptr<SharedContext> GetSharedContext () const;
#endif

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    public:
        nonvirtual auto operator== (const Ptr& rhs) const;
        nonvirtual bool operator== (nullptr_t) const noexcept;

    protected:
        shared_ptr<IRep> _fRep;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Connection.inl"

#endif /*_Stroika_Foundation_Database_SQL_Connection_h_*/
