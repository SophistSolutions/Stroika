
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
        : fConnection_{conn}
        , fTableSchema_{tableSchema}
        , fObjectVariantMapper_{objectVariantMapper}
        , fGetByID_Statement_{conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.GetByID ()}
        , fGetAll_Statement_ {conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.GetAllElements ())}
        , fAddNew_Statement_ {conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.Insert ())}
        , fUpdate_Statement_ {conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.UpdateByID ())}
    {
        Require (conn != nullptr);  // too late, but good docs
    }
    template <typename T, typename TRAITS>
    optional<T> TableConnection<T, TRAITS>::GetByID (const typename TRAITS::IDType& id)
    {
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        fGetByID_Statement_.Reset ();
        fGetByID_Statement_.Bind (initializer_list<KeyValuePair<String, VariantValue>>{{fTableSchema_.GetIDField ()->fName, VariantValue{static_cast<Memory::BLOB> (id)}}});
        if constexpr (TRAITS::kTraceLogEachRequest) {
            DbgTrace ("SQL: %s", fGetByID_Statement_.GetSQL (Statement::WhichSQLFlag::eExpanded).c_str ());
        }
        auto rows = fGetByID_Statement_.GetAllRows ()
                        .Select<T> ([this] (const Statement::Row& r) {
                            return fObjectVariantMapper_.ToObject<T> (VariantValue{fTableSchema_.MapFromDB (r)});
                        });
        if (rows.empty ()) {
            return nullopt;
        }
        Ensure (rows.size () == 1); // cuz arg sb a key
        return *rows.First ();
    }
    template <typename T, typename TRAITS>
    Sequence<T> TableConnection<T, TRAITS>::GetAll ()
    {
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        if constexpr (TRAITS::kTraceLogEachRequest) {
            DbgTrace ("SQL: %s", fGetAll_Statement_.GetSQL ().c_str ());
        }
        auto rows = fGetAll_Statement_.GetAllRows ()
                        .Select<T> ([this] (const Statement::Row& r) {
                            return fObjectVariantMapper_.ToObject<T> (VariantValue{fTableSchema_.MapFromDB (r)});
                        });
        return rows;
    }
    template <typename T, typename TRAITS>
    void TableConnection<T, TRAITS>::AddNew (const T& v)
    {
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        fAddNew_Statement_.Reset ();
        fAddNew_Statement_.Bind (fObjectVariantMapper_.MapToDB (fObjectVariantMapper_.FromObject (v).As<Mapping<String, VariantValue>> ()));
        if constexpr (TRAITS::kTraceLogEachRequest) {
            DbgTrace ("SQL: %s", statement.GetSQL (Statement::WhichSQLFlag::eExpanded).c_str ());
        }
        fAddNew_Statement_.Execute ();
    }
    template <typename T, typename TRAITS>
    void TableConnection<T, TRAITS>::Update (const T& v)
    {
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        fUpdate_Statement_.Reset ();
        fUpdate_Statement_.Bind (fTableSchema_.MapToDB (fObjectVariantMapper_.FromObject (v).As<Mapping<String, VariantValue>> ()));
        if constexpr (TRAITS::kTraceLogEachRequest) {
            DbgTrace ("SQL: %s", statement.GetSQL (Statement::WhichSQLFlag::eExpanded).c_str ());
        }
        fUpdate_Statement_.Execute ();
    }

}

#endif /*_Stroika_Foundation_Database_SQL_ORM_TableConnection_inl_*/
