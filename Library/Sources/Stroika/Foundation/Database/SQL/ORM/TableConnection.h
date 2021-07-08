/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_ORM_TableConnection_h_
#define _Stroika_Foundation_Database_SQL_ORM_TableConnection_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Common/GUID.h"
#include "../../../DataExchange/ObjectVariantMapper.h"
#include "../../../Debug/AssertExternallySynchronizedLock.h"

#include "../Connection.h"
#include "../Statement.h"

#include "Schema.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 *
 */

namespace Stroika::Foundation::Database::SQL::ORM {

    using namespace Containers;
    using namespace DataExchange;

    /**
     */
    template <typename T, typename ID_TYPE = Common::GUID, bool TRACE_LOG_EACH_REQUEST = false>
    struct TableConnectionTraits {
        using IDType                               = ID_TYPE;
        static constexpr bool kTraceLogEachRequest = TRACE_LOG_EACH_REQUEST;
    };

    /**
     *  \brief TableConnection<T> wraps a database Connection::Ptr with information to map c++ objects to/from SQL database objects, and provides a series of common, building block queries (CRUD)
     *
     *  Generally, these functions use the (constructor provided) tableSchema to know the layout/shape of the SQL data
     *  and the (constructor provided) ObjectVariantMapper to map arguments and results between c++ T objects and
     *  VariantValue objects.
     * 
     *  Together, this means you can simple Read, Write, Update, etc C++ objects directly from a SQL database,
     *  with all intervening mapping of data handled inside the TableConnection (via the Schema::Table and
     *  ObjectVariantMapper objects provided in the TableConnection::CTOR).
     * 
     *  \note we choose to create/cache the statements in the constructor and re-use them. We COULD
     *        lazy create (which would work better if you use only a small subset of the methods). But then
     *        we would incurr a cost checking each time (locking), so unclear if lazy creation is worth it.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter">C++-Standard-Thread-Safety-For-Envelope-But-Ambiguous-Thread-Safety-For-Letter/a>
     *          But though each connection can only be accessed from a single thread at a time, the underlying database may be
     *          threadsafe (even if accessed across processes).
     */
    template <typename T, typename TRAITS = TableConnectionTraits<T>>
    class TableConnection : private Debug::AssertExternallySynchronizedLock {
    public:
        TableConnection (const Connection::Ptr& conn, const Schema::Table& tableSchema, const ObjectVariantMapper& objectVariantMapper);
        TableConnection (TableConnection&& src) = default;

    public:
        /**
         *  Lookup the given object by ID. This does a select * from table where id=id, and maps the various
         *  SQL parameters that come back to a C++ object.
         */
        nonvirtual optional<T> GetByID (const typename TRAITS::IDType& id);

    public:
        /**
         *  Get ALL the c++ objects in this table. This does a select * from table, and maps the various
         *  SQL parameters that come back to C++ objects.
         */
        nonvirtual Sequence<T> GetAll ();

    public:
        /**
         *  Convert the argument object to database form, and write it as a new row to the database. The ID is
         *  generally not provided as part of T (though it can be), as you generally supply a default creation rule for
         *  IDs in the Schema::Table.
         */
        nonvirtual void AddNew (const T& v);

    public:
        /**
         *  Use the ID field from the argument object to update all the OTHER fields of that object in the database.
         *  The ID field is known because of the Table::Schema, and must be valid (else this will fail).
         */
        nonvirtual void Update (const T& v);

    private:
        Connection::Ptr     fConnection_;
        Schema::Table       fTableSchema_;
        ObjectVariantMapper fObjectVariantMapper_;
        Statement           fGetByID_Statement_;
        Statement           fGetAll_Statement_;
        Statement           fAddNew_Statement_;
        Statement           fUpdate_Statement_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TableConnection.inl"

#endif /*_Stroika_Foundation_Database_SQL_ORM_TableConnection_h_*/
