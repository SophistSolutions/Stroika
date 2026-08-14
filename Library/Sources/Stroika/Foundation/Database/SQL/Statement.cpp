/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Statement.h"

using namespace Stroika::Foundation;

using namespace Characters;
using namespace Debug;
using namespace Database;
using namespace Database::SQL;
using namespace Execution;
using namespace Time;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************* SQL::Statement::ColumnDescription ************************
 ********************************************************************************
 */
String Statement::ColumnDescription::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "name: "sv << fName;
    sb << ", type: "sv << fType;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ****************** SQL::Statement::ParameterDescription ************************
 ********************************************************************************
 */
String Statement::ParameterDescription::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    sb << "name: "sv << fName;
    sb << ", value: "sv << fValue;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ****************************** SQL::Statement **********************************
 ********************************************************************************
 */
auto Statement::GetAllRemainingRows () -> Sequence<Row>
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::GetAllRemainingRows"};
#endif
    AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
    Sequence<Row>                                   result;
    while (auto o = GetNextRow ()) {
        result += *o;
    }
    return result;
}

Sequence<VariantValue> Statement::GetAllRemainingRows (size_t restrictToColumn)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::GetAllRemainingRows"};
#endif
    AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
    Sequence<VariantValue>                          result;
    ColumnDescription                               col0 = GetColumns ()[restrictToColumn];
    while (auto o = GetNextRow ()) {
        result += *o->Lookup (col0.fName);
    }
    return result;
}

Sequence<tuple<VariantValue, VariantValue>> Statement::GetAllRemainingRows (size_t restrictToColumn1, size_t restrictToColumn2)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::GetAllRemainingRows"};
#endif
    AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
    Sequence<tuple<VariantValue, VariantValue>>     result;
    ColumnDescription                               col0 = GetColumns ()[restrictToColumn1];
    ColumnDescription                               col1 = GetColumns ()[restrictToColumn2];
    while (auto o = GetNextRow ()) {
        result += make_tuple (*o->Lookup (col0.fName), *o->Lookup (col1.fName));
    }
    return result;
}

Sequence<tuple<VariantValue, VariantValue, VariantValue>> Statement::GetAllRemainingRows (size_t restrictToColumn1,
                                                                                          size_t restrictToColumn2, size_t restrictToColumn3)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::GetAllRemainingRows"};
#endif
    AssertExternallySynchronizedChecker::WriteContext           critSec{_fAssertExternallySynchronizedChecker};
    Sequence<tuple<VariantValue, VariantValue, VariantValue>> result;
    ColumnDescription                                         col0 = GetColumns ()[restrictToColumn1];
    ColumnDescription                                         col1 = GetColumns ()[restrictToColumn2];
    ColumnDescription                                         col2 = GetColumns ()[restrictToColumn3];
    while (auto o = GetNextRow ()) {
        result += make_tuple (*o->Lookup (col0.fName), *o->Lookup (col1.fName), *o->Lookup (col2.fName));
    }
    return result;
}

void Statement::Bind (const Traversal::Iterable<ParameterDescription>& parameters)
{
    AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
    int                                             idx = 0;
    Bind ();
    for (const auto& i : parameters) {
        if (i.fName) {
            Bind (*i.fName, i.fValue);
        }
        else {
            Bind (idx, i.fValue);
        }
        ++idx;
    }
}

void Statement::Bind (const Traversal::Iterable<Common::KeyValuePair<String, VariantValue>>& parameters)
{
    AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
    Bind ();
    for (const auto& i : parameters) {
        Bind (i.fKey, i.fValue);
    }
}

void Statement::Execute ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::Execute"};
#endif
    AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
    Reset ();
    (void)_fRep->GetNextRow ();
}

void Statement::Execute (const Traversal::Iterable<ParameterDescription>& parameters)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::Execute"};
#endif
    AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
    Reset ();
    Bind (parameters);
    (void)_fRep->GetNextRow ();
}

void Statement::Execute (const Traversal::Iterable<Common::KeyValuePair<String, VariantValue>>& parameters)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::Execute", "parameters={}"_f, parameters};
#endif
    AssertExternallySynchronizedChecker::WriteContext declareContext{_fAssertExternallySynchronizedChecker};
    Reset ();
    Bind (parameters);
    (void)_fRep->GetNextRow ();
}

String Statement::ToString () const
{
    AssertExternallySynchronizedChecker::ReadContext declareContext{_fAssertExternallySynchronizedChecker};
    StringBuilder                                  sb;
    sb << "{"sv;
    sb << "parameterBindings: "sv << GetParameters ();
    sb << ", columnDescriptions: "sv << GetColumns ();
    sb << ", originalSQL: "sv << GetSQL ();
    sb << "}"sv;
    return sb;
}