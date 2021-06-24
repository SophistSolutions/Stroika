/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Database_SQL_ORM_TableConnection_h_
#define _Stroika_Foundation_Database_SQL_ORM_TableConnection_h_ 1

#include "../../../StroikaPreComp.h"

#include "../../../DataExchange/ObjectVariantMapper.h"

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
     */
    template <typename T, typename TRAITS = TableConnectionTraits<T>>
    class TableConnection {
    public:
        TableConnection (const Connection::Ptr& conn, const Schema::Table& tableSchema, const ObjectVariantMapper& objectVariantMapper);

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
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TableConnection.inl"

#endif /*_Stroika_Foundation_Database_SQL_ORM_TableConnection_h_*/
