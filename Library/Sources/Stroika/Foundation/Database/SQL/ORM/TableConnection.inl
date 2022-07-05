
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQL_ORM_TableConnection_inl_
#define _Stroika_Foundation_Database_SQL_ORM_TableConnection_inl_ 1

#include "../../../Debug/Trace.h"

namespace Stroika::Foundation::Database::SQL::ORM {

    /*
     ********************************************************************************
     ******************************** TableConnection *******************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    TableConnection<T, TRAITS>::TableConnection (const Connection::Ptr& conn, const Schema::Table& tableSchema, const ObjectVariantMapper& objectVariantMapper)
        : pConnection{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
            const TableConnection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &TableConnection::pConnection);
            return thisObj->fConnection_;
        }}
        , pTableSchema{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
            const TableConnection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &TableConnection::pTableSchema);
            return thisObj->fTableSchema_;
        }}
        , pObjectVariantMapper{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
            const TableConnection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &TableConnection::pObjectVariantMapper);
            return thisObj->fObjectVariantMapper_;
        }}
        , fConnection_{conn}
        , fTableSchema_{tableSchema}
        , fObjectVariantMapper_{objectVariantMapper}
        , fGetByID_Statement_{conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.GetByID ())}
        , fGetAll_Statement_{conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.GetAllElements ())}
        , fAddNew_Statement_{conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.Insert ())}
        , fUpdate_Statement_{conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.UpdateByID ())}
    {
        Require (conn != nullptr); // too late, but good docs
    }
    template <typename T, typename TRAITS>
    inline TableConnection<T, TRAITS>::TableConnection (const TableConnection& src)
        : TableConnection{src.fConnection_, src.fTableSchema_, src.fObjectVariantMapper_}
    {
    }
    template <typename T, typename TRAITS>
    optional<T> TableConnection<T, TRAITS>::GetByID (const VariantValue& id)
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        fGetByID_Statement_.Reset ();
        fGetByID_Statement_.Bind (initializer_list<KeyValuePair<String, VariantValue>>{{fTableSchema_.GetIDField ()->fName, id}});
        if constexpr (TRAITS::kTraceLogEachRequest) {
            DbgTrace (L"SQL: %s", fGetByID_Statement_.GetSQL (Statement::WhichSQLFlag::eExpanded).c_str ());
        }
        auto rows = fGetByID_Statement_.GetAllRows ()
                        .template Select<T> ([this] (const Statement::Row& r) {
                            return fObjectVariantMapper_.ToObject<T> (VariantValue{fTableSchema_.MapFromDB (r)});
                        });
        if (rows.empty ()) {
            return nullopt;
        }
        Ensure (rows.size () == 1); // cuz arg sb a key
        return *rows.First ();
    }
    template <typename T, typename TRAITS>
    optional<T> TableConnection<T, TRAITS>::GetByID (const typename TRAITS::IDType& id)
    {
        return GetByID (TRAITS::ID2VariantValue (id));
    }
    template <typename T, typename TRAITS>
    Sequence<T> TableConnection<T, TRAITS>::GetAll ()
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        if constexpr (TRAITS::kTraceLogEachRequest) {
            DbgTrace (L"SQL: %s", fGetAll_Statement_.GetSQL ().c_str ());
        }
        auto rows = fGetAll_Statement_.GetAllRows ()
                        .template Select<T> ([this] (const Statement::Row& r) {
                            return fObjectVariantMapper_.ToObject<T> (VariantValue{fTableSchema_.MapFromDB (r)});
                        });
        return Sequence<T>{rows};
    }
    template <typename T, typename TRAITS>
    void TableConnection<T, TRAITS>::AddNew (const T& v)
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        using DataExchange::VariantValue;
        fAddNew_Statement_.Reset ();
        fAddNew_Statement_.Bind (fTableSchema_.MapToDB (fObjectVariantMapper_.FromObject (v).template As<Mapping<String, VariantValue>> ()));
        if constexpr (TRAITS::kTraceLogEachRequest) {
            DbgTrace (L"SQL: %s", fAddNew_Statement_.GetSQL (Statement::WhichSQLFlag::eExpanded).c_str ());
        }
        fAddNew_Statement_.Execute ();
    }
    template <typename T, typename TRAITS>
    void TableConnection<T, TRAITS>::AddOrUpdate (const T& v)
    {
        // @todo: this can and should be done much more efficiently
        Mapping<String, VariantValue> dbV = fTableSchema_.MapToDB (fObjectVariantMapper_.FromObject (v).template As<Mapping<String, VariantValue>> ());
        if (GetByID (dbV[fTableSchema_.GetIDField ()->fName])) {
            Update (v);
        }
        else {
            AddNew (v);
        }
    }
    template <typename T, typename TRAITS>
    void TableConnection<T, TRAITS>::Update (const T& v)
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        fUpdate_Statement_.Reset ();
        fUpdate_Statement_.Bind (fTableSchema_.MapToDB (fObjectVariantMapper_.FromObject (v).template As<Mapping<String, VariantValue>> ()));
        if constexpr (TRAITS::kTraceLogEachRequest) {
            DbgTrace (L"SQL: %s", fUpdate_Statement_.GetSQL (Statement::WhichSQLFlag::eExpanded).c_str ());
        }
        fUpdate_Statement_.Execute ();
    }

}

#endif /*_Stroika_Foundation_Database_SQL_ORM_TableConnection_inl_*/
