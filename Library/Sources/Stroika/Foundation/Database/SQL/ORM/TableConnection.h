/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_ORM_TableConnection_h_
#define _Stroika_Foundation_Database_SQL_ORM_TableConnection_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../DataExchange/ObjectVariantMapper.h"
#include "../../../Debug/AssertExternallySynchronizedLock.h"

#include "../Connection.h"

#include "Schema.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 *
 */

#include "../../../Common/GUID.h"

namespace Stroika::Foundation::Database::SQL::ORM {

    using namespace Containers;
    using namespace DataExchange;

    /**
     */
    template <typename T>
    struct TableConnectionTraits {
        using IDType                               = Common::GUID;
        static constexpr bool kTraceLogEachRequest = false;
    };

    /**
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
        nonvirtual optional<T> GetByID (const typename TRAITS::IDType& id);

    public:
        nonvirtual Sequence<T> GetAll ();

    public:
        nonvirtual void AddNew (const T& v);

    public:
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
