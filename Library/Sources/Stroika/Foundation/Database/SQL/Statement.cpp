/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/Format.h"
#include "../../Characters/StringBuilder.h"
#include "../../Characters/ToString.h"
#include "../../Debug/Trace.h"

#include "Statement.h"

using std::byte;

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
    sb += L"{";
    sb += L"name: " + Characters::ToString (fName) + L", ";
    sb += L"type: " + Characters::ToString (fType);
    sb += L"}";
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
    sb += L"{";
    sb += L"name: " + Characters::ToString (fName) + L", ";
    sb += L"value: " + Characters::ToString (fValue);
    sb += L"}";
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
    lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
    Sequence<Row>                                              result;
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
    lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
    Sequence<VariantValue>                                     result;
    ColumnDescription                                          col0 = GetColumns ()[restrictToColumn];
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
    lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
    Sequence<tuple<VariantValue, VariantValue>>                result;
    ColumnDescription                                          col0 = GetColumns ()[restrictToColumn1];
    ColumnDescription                                          col1 = GetColumns ()[restrictToColumn2];
    while (auto o = GetNextRow ()) {
        result += make_tuple (*o->Lookup (col0.fName), *o->Lookup (col1.fName));
    }
    return result;
}

Sequence<tuple<VariantValue, VariantValue, VariantValue>> Statement::GetAllRemainingRows (size_t restrictToColumn1, size_t restrictToColumn2, size_t restrictToColumn3)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::GetAllRemainingRows"};
#endif
    lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
    Sequence<tuple<VariantValue, VariantValue, VariantValue>>  result;
    ColumnDescription                                          col0 = GetColumns ()[restrictToColumn1];
    ColumnDescription                                          col1 = GetColumns ()[restrictToColumn2];
    ColumnDescription                                          col2 = GetColumns ()[restrictToColumn3];
    while (auto o = GetNextRow ()) {
        result += make_tuple (*o->Lookup (col0.fName), *o->Lookup (col1.fName), *o->Lookup (col2.fName));
    }
    return result;
}

void Statement::Bind (const Traversal::Iterable<ParameterDescription>& parameters)
{
    lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
    int                                                        idx = 0;
    for (auto i : parameters) {
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
    lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
    for (auto i : parameters) {
        Bind (i.fKey, i.fValue);
    }
}

void Statement::Execute ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::Execute"};
#endif
    lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
    Reset ();
    (void)_fRep->GetNextRow ();
}

void Statement::Execute (const Traversal::Iterable<ParameterDescription>& parameters)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{"SQL::Statement::Execute"};
#endif
    lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
    Reset ();
    Bind (parameters);
    (void)_fRep->GetNextRow ();
}

void Statement::Execute (const Traversal::Iterable<Common::KeyValuePair<String, VariantValue>>& parameters)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"SQL::Statement::Execute", L"parameters=%s", Characters::ToString (parameters).c_str ())};
#endif
    lock_guard<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
    Reset ();
    Bind (parameters);
    (void)_fRep->GetNextRow ();
}

String Statement::ToString () const
{
    shared_lock<const Debug::AssertExternallySynchronizedMutex> critSec{*this};
    StringBuilder                                               sb;
    sb += L"{";
    sb += L"Parameter-Bindings: " + Characters::ToString (GetParameters ()) + L", ";
    sb += L"Column-Descriptions: " + Characters::ToString (GetColumns ()) + L", ";
    sb += L"Original-SQL: " + Characters::ToString (GetSQL ());
    sb += L"}";
    return sb.str ();
}