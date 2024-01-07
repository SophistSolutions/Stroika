/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Characters/StringBuilder.h"
#include "../../Characters/ToString.h"
#include "../../Debug/Trace.h"

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
    sb << "name: "sv << Characters::ToString (fName) << ", "sv;
    sb << "type: "sv << Characters::ToString (fType);
    sb << "}"sv;
    return sb.str ();
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
    sb << "name: "sv << Characters::ToString (fName) << ", "sv;
    sb << "value: "sv << Characters::ToString (fValue);
    sb << "}"sv;
    return sb.str ();
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
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
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
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
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
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
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
    AssertExternallySynchronizedMutex::WriteContext           critSec{_fAssertExternallySynchronizedMutex};
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
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
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
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
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
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
    Reset ();
    (void)_fRep->GetNextRow ();
}

void Statement::Execute (const Traversal::Iterable<ParameterDescription>& parameters)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::Execute"};
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
    Reset ();
    Bind (parameters);
    (void)_fRep->GetNextRow ();
}

void Statement::Execute (const Traversal::Iterable<Common::KeyValuePair<String, VariantValue>>& parameters)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"SQL::Statement::Execute", L"parameters=%s",
                                                                          Characters::ToString (parameters).c_str ())};
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{_fAssertExternallySynchronizedMutex};
    Reset ();
    Bind (parameters);
    (void)_fRep->GetNextRow ();
}

String Statement::ToString () const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{_fAssertExternallySynchronizedMutex};
    StringBuilder                                  sb;
    sb << "{"sv;
    sb << "Parameter-Bindings: "sv << Characters::ToString (GetParameters ()) << ", "sv;
    sb << "Column-Descriptions: "sv << Characters::ToString (GetColumns ()) << ", "sv;
    sb << "Original-SQL: "sv << Characters::ToString (GetSQL ());
    sb << "}"sv;
    return sb.str ();
}