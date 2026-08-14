/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

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
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{_fAssertExternallySynchronizedChecker};
        RequireNotNull (_fRep); // Statement object moved
        return _fRep->GetColumns ();
    }
    inline auto Statement::GetParameters () const -> Sequence<ParameterDescription>
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{_fAssertExternallySynchronizedChecker};
        RequireNotNull (_fRep); // Statement object moved
        return _fRep->GetParameters ();
    }
    inline void Statement::Bind ()
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
        RequireNotNull (_fRep); // Statement object moved
        _fRep->Bind ();
    }
    inline void Statement::Bind (unsigned int parameterIndex, const VariantValue& v)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
        RequireNotNull (_fRep); // Statement object moved
        _fRep->Bind (parameterIndex, v);
    }
    inline void Statement::Bind (const String& parameterName, const VariantValue& v)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
        RequireNotNull (_fRep); // Statement object moved
        _fRep->Bind (parameterName, v);
    }
    inline String Statement::GetSQL (WhichSQLFlag whichSQL) const
    {
        Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{_fAssertExternallySynchronizedChecker};
        RequireNotNull (_fRep); // Statement object moved
        return _fRep->GetSQL (whichSQL);
    }
    inline void Statement::Reset ()
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
        RequireNotNull (_fRep); // Statement object moved
        _fRep->Reset ();
    }
    inline auto Statement::GetNextRow () -> optional<Row>
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
        RequireNotNull (_fRep); // Statement object moved
        return _fRep->GetNextRow ();
    }
    inline auto Statement::GetAllRows () -> Sequence<Row>
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
        Reset ();
        return GetAllRemainingRows ();
    }
    inline Sequence<VariantValue> Statement::GetAllRows (size_t restrictToColumn)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
        Reset ();
        return GetAllRemainingRows (restrictToColumn);
    }
    inline Sequence<tuple<VariantValue, VariantValue>> Statement::GetAllRows (size_t restrictToColumn1, size_t restrictToColumn2)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
        Reset ();
        return GetAllRemainingRows (restrictToColumn1, restrictToColumn2);
    }
    inline Sequence<tuple<VariantValue, VariantValue, VariantValue>> Statement::GetAllRows (size_t restrictToColumn1,
                                                                                            size_t restrictToColumn2, size_t restrictToColumn3)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
        Reset ();
        return GetAllRemainingRows (restrictToColumn1, restrictToColumn2, restrictToColumn3);
    }

}
