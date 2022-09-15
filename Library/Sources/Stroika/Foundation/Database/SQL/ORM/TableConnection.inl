
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQL_ORM_TableConnection_inl_
#define _Stroika_Foundation_Database_SQL_ORM_TableConnection_inl_ 1

#include "../../../Debug/Trace.h"
#include "../../../Execution/Finally.h"

namespace Stroika::Foundation::Database::SQL::ORM {

    /*
     ********************************************************************************
     ******************************** TableConnection *******************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    TableConnection<T, TRAITS>::TableConnection (const Connection::Ptr& conn, const Schema::Table& tableSchema, const ObjectVariantMapper& objectVariantMapper, const OpertionCallbackPtr& operationCallback)
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
        , pOperationCallback{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) {
            const TableConnection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &TableConnection::pOperationCallback);
            return thisObj->fTableOpertionCallback_;
        }}
        , fConnection_{conn}
        , fEngineProperties_{conn->GetEngineProperties ()}
        , fTableSchema_{tableSchema}
        , fObjectVariantMapper_{objectVariantMapper}
        , fTableOpertionCallback_{operationCallback}
        , fGetByID_Statement_{conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.GetByID ())}
        , fGetAll_Statement_{conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.GetAllElements ())}
        , fAddNew_Statement_{conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.Insert ())}
        , fUpdate_Statement_{conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.UpdateByID ())}
        , fDeleteByID_Statement_{conn->mkStatement (Schema::StandardSQLStatements{tableSchema}.DeleteByID ())}
    {
        Require (conn != nullptr); // too late, but good docs
    }
    template <typename T, typename TRAITS>
    inline TableConnection<T, TRAITS>::TableConnection (const TableConnection& src)
        : TableConnection{src.fConnection_, src.fTableSchema_, src.fObjectVariantMapper_, src.fTableOpertionCallback_}
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
        optional<Statement::Row> row;
        DoExecute_ ([&] (Statement& s) { row = s.GetNextRow (); }, fGetByID_Statement_, false);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () {
            if (fEngineProperties_->RequireStatementResetAfterModifyingStatmentToCompleteTransaction ()) {
                // could potentially avoid this if I added way to track if existing transaction object, but not clearly any point
                fGetByID_Statement_.Reset ();
            }
        });
        if (row) {
            return fObjectVariantMapper_.ToObject<T> (VariantValue{fTableSchema_.MapFromDB (*row)});
        }
        return nullopt;
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
        Sequence<Statement::Row> rows;
        DoExecute_ ([&] (Statement& s) { rows = s.GetAllRows (); }, fGetAll_Statement_, false);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () {
            if (fEngineProperties_->RequireStatementResetAfterModifyingStatmentToCompleteTransaction ()) {
                // could potentially avoid this if I added way to track if existing transaction object, but not clearly any point
                fGetAll_Statement_.Reset ();
            }
        });
        return Sequence<T>{rows.template Select<T> ([this] (const Statement::Row& r) {
            return fObjectVariantMapper_.ToObject<T> (VariantValue{fTableSchema_.MapFromDB (r)});
        })};
    }
    template <typename T, typename TRAITS>
    Sequence<T> TableConnection<T, TRAITS>::GetAll (const function<optional<T> (const Statement::Row&, const exception_ptr&)>& onItemException)
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        Sequence<Statement::Row> rows;
        DoExecute_ ([&] (Statement& s) { rows = s.GetAllRows (); }, fGetAll_Statement_, false);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () {
            if (fEngineProperties_->RequireStatementResetAfterModifyingStatmentToCompleteTransaction ()) {
                // could potentially avoid this if I added way to track if existing transaction object, but not clearly any point
                fGetAll_Statement_.Reset ();
            }
        });
        return Sequence<T>{rows.template Select<T> ([this, &onItemException] (const Statement::Row& r) -> optional<T> {
            try {
                return fObjectVariantMapper_.ToObject<T> (VariantValue{fTableSchema_.MapFromDB (r)});
            }
            catch (...) {
                return onItemException (r, current_exception ());
            }
        })};
    }
    template <typename T, typename TRAITS>
    void TableConnection<T, TRAITS>::AddNew (const T& v)
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        using DataExchange::VariantValue;
        fAddNew_Statement_.Reset ();
        fAddNew_Statement_.Bind (fTableSchema_.MapToDB (fObjectVariantMapper_.FromObject (v).template As<Mapping<String, VariantValue>> ()));
        DoExecute_ (fAddNew_Statement_, true);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () {
            if (fEngineProperties_->RequireStatementResetAfterModifyingStatmentToCompleteTransaction ()) {
                // could potentially avoid this if I added way to track if existing transaction object, but not clearly any point
                fAddNew_Statement_.Reset ();
            }
        });
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
        DoExecute_ (fUpdate_Statement_, true);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () {
            if (fEngineProperties_->RequireStatementResetAfterModifyingStatmentToCompleteTransaction ()) {
                // could potentially avoid this if I added way to track if existing transaction object, but not clearly any point
                fUpdate_Statement_.Reset ();
            }
        });
    }
    template <typename T, typename TRAITS>
    void TableConnection<T, TRAITS>::DeleteByID (const VariantValue& id)
    {
        lock_guard<const AssertExternallySynchronizedMutex> critSec{*this};
        using DataExchange::VariantValue;
        using Stroika::Foundation::Common::KeyValuePair;
        fDeleteByID_Statement_.Reset ();
        fDeleteByID_Statement_.Bind (initializer_list<KeyValuePair<String, VariantValue>>{{fTableSchema_.GetIDField ()->fName, id}});
        DoExecute_ (fDeleteByID_Statement_, true);
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () {
            if (fEngineProperties_->RequireStatementResetAfterModifyingStatmentToCompleteTransaction ()) {
                // could potentially avoid this if I added way to track if existing transaction object, but not clearly any point
                fDeleteByID_Statement_.Reset ();
            }
        });
    }
    template <typename T, typename TRAITS>
    inline void TableConnection<T, TRAITS>::DeleteByID (const typename TRAITS::IDType& id)
    {
        DeleteByID (TRAITS::ID2VariantValue (id));
    }
    template <typename T, typename TRAITS>
    template <typename FUN>
    void TableConnection<T, TRAITS>::DoExecute_ (FUN&& f, Statement& s, bool write)
    {
        if (fTableOpertionCallback_ == nullptr) {
            f (s);
        }
        else {
            fTableOpertionCallback_ (write ? Operation::eStartingWrite : Operation::eStartingRead, this, &s, nullptr);
            try {
                f (s);
                fTableOpertionCallback_ (write ? Operation::eCompletedWrite : Operation::eCompletedRead, this, &s, nullptr);
            }
            catch (...) {
                fTableOpertionCallback_ (Operation::eNotifyError, this, &s, current_exception ());
                fTableOpertionCallback_ (write ? Operation::eCompletedWrite : Operation::eCompletedRead, this, &fUpdate_Statement_, nullptr);
                Execution::ReThrow ();
            }
        }
    }
    template <typename T, typename TRAITS>
    inline void TableConnection<T, TRAITS>::DoExecute_ (Statement& s, bool write)
    {
        DoExecute_ ([] (Statement& s) { s.Execute (); }, s, write);
    }
    template <typename T, typename TRAITS>
    const typename TableConnection<T, TRAITS>::OpertionCallbackPtr TableConnection<T, TRAITS>::kDefaultTracingOpertionCallback = [] (Operation op, const TableConnection* tableConn, const Statement* s) {
        if (op == Operation::eStartingRead or op == Operation::eStartingWrite) {
            if (s != nullptr) {
                DbgTrace (L"SQL: %s", s->GetSQL (Statement::WhichSQLFlag::eExpanded).c_str ());
            }
        }
    };

}

#endif /*_Stroika_Foundation_Database_SQL_ORM_TableConnection_inl_*/
