/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_ORM_TableConnection_h_
#define _Stroika_Foundation_Database_SQL_ORM_TableConnection_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../Common/GUID.h"
#include "../../../Common/Property.h"
#include "../../../DataExchange/ObjectVariantMapper.h"
#include "../../../Debug/AssertExternallySynchronizedMutex.h"

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
    template <typename T, typename ID_TYPE = Common::GUID>
    struct TableConnectionTraits {
        using IDType = ID_TYPE;
        static VariantValue ID2VariantValue (const IDType& id)
        {
            if constexpr (is_convertible_v<IDType, Memory::BLOB> or is_same_v<IDType, Common::GUID>) {
                return VariantValue{static_cast<Memory::BLOB> (id)};
            }
            else if constexpr (is_same_v<IDType, IO::Network::URI>) {
                return VariantValue{id.template As<Characters::String> ()};
            }
            else if constexpr (is_convertible_v<IDType, Characters::String>) {
                return VariantValue{static_cast<Characters::String> (id)};
            }
            else {
                //static_assert (false, "specify your own ID2VariantValue function for this type");
                AssertNotReached ();
                return VariantValue{};
            }
        }
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
    class TableConnection : private Debug::AssertExternallySynchronizedMutex {
    public:
        /**
         * Optionally passed to TableConnection for the purpose of logging
         */
        enum Operation { eStartingRead,
                         eCompletedRead,
                         eStartingWrite,
                         eCompletedWrite,
                         eNotifyError };

    public:
        /**
         * Optionally passed to TableConnection for the purpose of logging; 
         * note exception_ptr is only provided for eNotifyError, and is typically current_exception () but can be nullptr
         */
        using OpertionCallbackPtr = function<void (Operation op, const TableConnection* tableConn, const Statement* s, const exception_ptr& e)>;

    public:
        TableConnection () = delete;
        TableConnection (const Connection::Ptr& conn, const Schema::Table& tableSchema, const ObjectVariantMapper& objectVariantMapper, const OpertionCallbackPtr& operationCallback = nullptr);
        TableConnection (const TableConnection& src);

    public:
        /**
         */
        Common::ReadOnlyProperty<Connection::Ptr> pConnection;

    public:
        /**
         */
        Common::ReadOnlyProperty<Schema::Table> pTableSchema;

    public:
        /**
         */
        Common::ReadOnlyProperty<ObjectVariantMapper> pObjectVariantMapper;

    public:
        /**
         */
        Common::ReadOnlyProperty<OpertionCallbackPtr> pOperationCallback;

    public:
        static const OpertionCallbackPtr kDefaultTracingOpertionCallback;

    public:
        /**
         *  Lookup the given object by ID. This does a select * from table where id=id, and maps the various
         *  SQL parameters that come back to a C++ object.
         */
        nonvirtual optional<T> GetByID (const VariantValue& id);
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
         *  Equivilent to checking GetByID () to see if present, and either calling AddNew or Update().
         */
        nonvirtual void AddOrUpdate (const T& v);

    public:
        /**
         *  Use the ID field from the argument object to update all the OTHER fields of that object in the database.
         *  The ID field is known because of the Table::Schema, and must be valid (else this will fail).
         */
        nonvirtual void Update (const T& v);

    private:
        Connection::Ptr                    fConnection_;
        shared_ptr<const EngineProperties> fEngineProperties_;
        Schema::Table                      fTableSchema_;
        ObjectVariantMapper                fObjectVariantMapper_;
        OpertionCallbackPtr                fTableOpertionCallback_;
        Statement                          fGetByID_Statement_;
        Statement                          fGetAll_Statement_;
        Statement                          fAddNew_Statement_;
        Statement                          fUpdate_Statement_;

    private:
        template <typename FUN>
        nonvirtual void DoExecute_ (FUN&& f, Statement& s, bool write);
        nonvirtual void DoExecute_ (Statement& s, bool write);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TableConnection.inl"

#endif /*_Stroika_Foundation_Database_SQL_ORM_TableConnection_h_*/
