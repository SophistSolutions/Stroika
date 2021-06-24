
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
    {
    }
    template <typename T, typename TRAITS>
    optional<T> TableConnection<T, TRAITS>::GetByID (const typename TRAITS::IDType& id)
    {
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        Statement statement = conn->mkStatement (Schema::StandardSQLStatements{fTableSchema_}.GetByID ());
        getStatement.Execute (initializer_list<KeyValuePair<String, VariantValue>>{{fTableSchema_.GetIDField ()->fName, VariantValue{static_cast<Memory::BLOB> (id)}}});
        if constexpr (TRAITS::kTraceLogEachRequest) {
            //DbgTrace ("SQL: %s", statement.GetSQL ().c_str ());
            DbgTrace ("SQL: %s", statement.GetSQL (Statement::WhichSQLFlag::eExpanded).c_str ());
        }
        auto rows = statement.GetAllRows ()
                        .Select<T> ([] (const Statement::Row& r) {
                            return fObjectVariantMapper_.ToObject<T> (VariantValue{fTableSchema_.MapFromDB (r)});
                        });
        if (rows.empty ()) {
            return nullopt;
        }
        Ensure (rows.size () == 1); // cuz arg sb a key
        return *rows.First ();
    }
    template <typename T, typename TRAITS>
    void TableConnection<T, TRAITS>::AddNew (const T& v)
    {
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        Statement statement = conn->mkStatement (Schema::StandardSQLStatements{fTableSchema_}.Insert ());
        statement.Bind (fObjectVariantMapper_.MapToDB (fObjectVariantMapper_.FromObject (d).As<Mapping<String, VariantValue>> ()));
        if constexpr (TRAITS::kTraceLogEachRequest) {
            //DbgTrace ("SQL: %s", statement.GetSQL ().c_str ());
            DbgTrace ("SQL: %s", statement.GetSQL (Statement::WhichSQLFlag::eExpanded).c_str ());
        }
        statement.Execute ();
    }
    template <typename T, typename TRAITS>
    void TableConnection<T, TRAITS>::Update (const T& v)
    {
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        Statement statement = conn->mkStatement (Schema::StandardSQLStatements{fTableSchema_}.UpdateByID ());
        statement.Bind (fTableSchema_.MapToDB (fObjectVariantMapper_.FromObject (merged).As<Mapping<String, VariantValue>> ()));
        if constexpr (TRAITS::kTraceLogEachRequest) {
            //DbgTrace ("SQL: %s", statement.GetSQL ().c_str ());
            DbgTrace ("SQL: %s", statement.GetSQL (Statement::WhichSQLFlag::eExpanded).c_str ());
        }
        statement.Execute ();
    }

}

#endif /*_Stroika_Foundation_Database_SQL_ORM_TableConnection_inl_*/
