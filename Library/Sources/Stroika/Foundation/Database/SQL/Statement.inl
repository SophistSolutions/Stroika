
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Database_SQL_Statement_inl_
#define _Stroika_Foundation_Database_SQL_Statement_inl_ 1

namespace Stroika::Foundation::Database::SQL {

    /*
     ********************************************************************************
     ****************************** SQL::Statement **********************************
     ********************************************************************************
     */
    inline Statement::Statement (unique_ptr<IRep>&& rep)
        : _fRep{move (rep)}
    {
    }
    inline auto Statement::GetColumns () const -> Sequence<ColumnDescription>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fAssertExternallySynchronizedMutex};
        RequireNotNull (_fRep); // Statement object moved
        return _fRep->GetColumns ();
    }
    inline auto Statement::GetParameters () const -> Sequence<ParameterDescription>
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fAssertExternallySynchronizedMutex};
        RequireNotNull (_fRep); // Statement object moved
        return _fRep->GetParameters ();
    }
    inline void Statement::Bind ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
        RequireNotNull (_fRep); // Statement object moved
        _fRep->Bind ();
    }
    inline void Statement::Bind (unsigned int parameterIndex, const VariantValue& v)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
        RequireNotNull (_fRep); // Statement object moved
        _fRep->Bind (parameterIndex, v);
    }
    inline void Statement::Bind (const String& parameterName, const VariantValue& v)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
        RequireNotNull (_fRep); // Statement object moved
        _fRep->Bind (parameterName, v);
    }
    inline String Statement::GetSQL (WhichSQLFlag whichSQL) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{_fAssertExternallySynchronizedMutex};
        RequireNotNull (_fRep); // Statement object moved
        return _fRep->GetSQL (whichSQL);
    }
    inline void Statement::Reset ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
        RequireNotNull (_fRep); // Statement object moved
        _fRep->Reset ();
    }
    inline auto Statement::GetNextRow () -> optional<Row>
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
        RequireNotNull (_fRep); // Statement object moved
        return _fRep->GetNextRow ();
    }
    inline auto Statement::GetAllRows () -> Sequence<Row>
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
        Reset ();
        return GetAllRemainingRows ();
    }
    inline Sequence<VariantValue> Statement::GetAllRows (size_t restrictToColumn)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
        Reset ();
        return GetAllRemainingRows (restrictToColumn);
    }
    inline Sequence<tuple<VariantValue, VariantValue>> Statement::GetAllRows (size_t restrictToColumn1, size_t restrictToColumn2)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
        Reset ();
        return GetAllRemainingRows (restrictToColumn1, restrictToColumn2);
    }
    inline Sequence<tuple<VariantValue, VariantValue, VariantValue>> Statement::GetAllRows (size_t restrictToColumn1,
                                                                                            size_t restrictToColumn2, size_t restrictToColumn3)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
        Reset ();
        return GetAllRemainingRows (restrictToColumn1, restrictToColumn2, restrictToColumn3);
    }

}

#endif /*_Stroika_Foundation_Database_SQL_Statement_inl_*/
