/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/StringBuilder.h"
#include "../../../DataExchange/Variant/JSON/Reader.h"
#include "../../../DataExchange/Variant/JSON/Writer.h"
#include "../../../Debug/Trace.h"

#include "../../Exception.h"

#include "Schema.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::Database::SQL;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 ********************************************************************************
 *************************** ORM::Schema::CatchAllField *************************
 ********************************************************************************
 */
ORM::Schema::CatchAllField::CatchAllField ()
{
    fName             = "_other_fields_"sv;
    fVariantValueType = VariantValue::Type::eBLOB; // can be BLOB or String, but BLOB more compact/efficient
}

function<VariantValue (const Mapping<String, VariantValue>& fields2Map)> ORM::Schema::CatchAllField::GetEffectiveRawToCombined () const
{
    if (fMapRawFieldsToCombinedField != nullptr) {
        return fMapRawFieldsToCombinedField;
    }
    Require (fVariantValueType);
    switch (*fVariantValueType) {
        case VariantValue::eBLOB:
#if qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy
            if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
                return [] (const Mapping<String, VariantValue>& fields2Map) { return kDefaultMapper_RawToCombined_BLOB (fields2Map); };
            }
#endif

            return kDefaultMapper_RawToCombined_BLOB;
        case VariantValue::eString:
#if qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy
            if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
                return [] (const Mapping<String, VariantValue>& fields2Map) { return kDefaultMapper_RawToCombined_String (fields2Map); };
            }
#endif
            return kDefaultMapper_RawToCombined_String;
        default:
            RequireNotReached ();
            return nullptr;
    }
}

function<Mapping<String, VariantValue> (const VariantValue& map2Fields)> ORM::Schema::CatchAllField::GetEffectiveCombinedToRaw () const
{
    if (fMapCombinedFieldToRawFields != nullptr) {
        return fMapCombinedFieldToRawFields;
    }
    Require (fVariantValueType);
    switch (*fVariantValueType) {
        case VariantValue::eBLOB:
#if qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy
            if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
                return [] (const VariantValue& map2Fields) { return kDefaultMapper_CombinedToRaw_BLOB (map2Fields); };
            }
#endif
            return kDefaultMapper_CombinedToRaw_BLOB;
        case VariantValue::eString:
#if qCompilerAndStdLib_arm_ubsan_callDirectFunInsteadOfThruLamdba_Buggy
            if (Debug::kBuiltWithUndefinedBehaviorSanitizer) {
                return [] (const VariantValue& map2Fields) { return kDefaultMapper_CombinedToRaw_String (map2Fields); };
            }
#endif
            return kDefaultMapper_CombinedToRaw_String;
        default:
            RequireNotReached ();
            return nullptr;
    }
}

VariantValue ORM::Schema::CatchAllField::kDefaultMapper_RawToCombined_BLOB (const Mapping<String, VariantValue>& fields2Map)
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsBLOB (VariantValue{fields2Map});
}

VariantValue ORM::Schema::CatchAllField::kDefaultMapper_RawToCombined_String (const Mapping<String, VariantValue>& fields2Map)
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (VariantValue{fields2Map});
}

Mapping<String, VariantValue> ORM::Schema::CatchAllField::kDefaultMapper_CombinedToRaw_BLOB (const VariantValue& map2Fields)
{
    if (map2Fields.empty ()) {
        return Mapping<String, VariantValue>{};
    }
    return DataExchange::Variant::JSON::Reader{}.Read (map2Fields.As<Memory::BLOB> ()).As<Mapping<String, VariantValue>> ();
}

Mapping<String, VariantValue> ORM::Schema::CatchAllField::kDefaultMapper_CombinedToRaw_String (const VariantValue& map2Fields)
{
    if (map2Fields.empty ()) {
        return Mapping<String, VariantValue>{};
    }
    return DataExchange::Variant::JSON::Reader{}.Read (map2Fields.As<String> ()).As<Mapping<String, VariantValue>> ();
}
using namespace std;

/*
 ********************************************************************************
 ****************************** ORM::Schema::Table ******************************
 ********************************************************************************
 */
Mapping<String, VariantValue> ORM::Schema::Table::MapToDB (const Mapping<String, VariantValue>& fields) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{L"ORM::Schema::Table::MapToDB",
                           Stroika_Foundation_Debug_OptionalizeTraceArgs (L"fields=%s", Characters::ToString (fields).c_str ())};
#endif
    Mapping<String, VariantValue> resultFields;
    Set<String>                   usedFields; // must track outside of resultFields.Keys () cuz input key could differ from output
    for (const auto& fi : fNamedFields) {
        String srcKey = fi.GetVariantValueFieldName ();
        if (optional<VariantValue> oFieldVal = fields.Lookup (srcKey)) {
            if (oFieldVal->GetType () == VariantValue::eNull) {
                // special case - don't write a NULL value - treat as not specified (but don't roll into fSpecialCatchAll)
                usedFields += srcKey;
                continue;
            }
            if (fi.fVariantValueType) {
                try {
                    resultFields.Add (fi.fName, oFieldVal->ConvertTo (*fi.fVariantValueType));
                }
                catch (...) {
                    DbgTrace (L"IN ORM::Schema::Table::MapToDB for field %s: %s", fi.fName.As<wstring> ().c_str (),
                              Characters::ToString (current_exception ()).c_str ());
                    throw; // dont call Execution::ReThrow () to avoid extra log entry - above enuf
                }
            }
            else {
                resultFields.Add (fi.fName, *oFieldVal);
            }
            usedFields += srcKey;
        }
        else if (fi.fRequired) {
            // throw or assert?
            DbgTrace (L"IN ORM::Schema::Table::MapToDB for field %s: field required, but not present in the argument list to Map function",
                      fi.fName.As<wstring> ().c_str ());
            AssertNotReached ();
        }
    }
    // now fold remaining fields into special 'extra' field (for structured non-indexed/non-searchable data)
    Set<String> fields2Accumulate = Set<String>{fields.Keys ()} - usedFields;
    if (fSpecialCatchAll.has_value ()) {
        Mapping<String, VariantValue> extraFields;
        for (auto i : fields2Accumulate) {
            extraFields.Add (i, *fields.Lookup (i));
        }
        // Combine fields into a new variant value (typically json string)
        resultFields.Add (fSpecialCatchAll->fName, fSpecialCatchAll->GetEffectiveRawToCombined () (extraFields));
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"returning: %s", Characters::ToString (resultFields).As<wstring> ().c_str ());
#endif
    return resultFields;
}

Mapping<String, VariantValue> ORM::Schema::Table::MapFromDB (const Mapping<String, VariantValue>& fields) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{L"ORM::Schema::Table::MapFromDB",
                           Stroika_Foundation_Debug_OptionalizeTraceArgs (L"fields=%s", Characters::ToString (fields).c_str ())};
#endif
    Mapping<String, VariantValue> resultFields;
    for (const auto& fi : fNamedFields) {
        if (auto oFieldVal = fields.Lookup (fi.fName)) {
            String toName = fi.GetVariantValueFieldName ();
            if (not fi.fRequired and oFieldVal->GetType () == VariantValue::eNull) {
                // @todo consider just not adding it if its null (not in DB) and nullable
                resultFields.Add (toName, *oFieldVal);
            }
            else if (fi.fVariantValueType) {
                resultFields.Add (toName, oFieldVal->ConvertTo (*fi.fVariantValueType));
            }
            else {
                resultFields.Add (toName, *oFieldVal);
            }
        }
        else if (fi.fRequired) {
            static const auto kException_ = Exception{"missing required field"};
            Execution::Throw (kException_);
        }
    }
    // now fold remaining fields into special 'extra' field (for structured non-indexed/non-searchable data)
    if (fSpecialCatchAll.has_value ()) {
        if (auto o = fields.Lookup (fSpecialCatchAll->fName)) {
            // no need to map names here because that is for DBRep to/from VariantValue rep name mapping and there is none for these fields
            resultFields.AddAll (fSpecialCatchAll->GetEffectiveCombinedToRaw () (*o));
        }
    }
    else {
        // @todo maybe check fNamedFields contains all the actual fields??? Maybe OK to not check
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"resultFields=%s", Characters::ToString (resultFields).c_str ());
#endif
    return resultFields;
}

auto ORM::Schema::Table::GetIDField () const -> optional<Field>
{
    return fNamedFields.First<Field> ([] (const Field& fi) -> optional<Field> {
        if (fi.fIsKeyField) {
            return fi;
        }
        return nullopt;
    });
}

/*
 ********************************************************************************
 ************************* ORM::Schema::StandardSQLStatements *******************
 ********************************************************************************
 */
namespace {
    string_view GetSQLiteFldType_ (const ORM::Schema::Field& f)
    {
        // @todo - this must be factored through SQL::EngineProperties
        if (f.fVariantValueType) {
            switch (*f.fVariantValueType) {
                case VariantValue::eBLOB:
                    return "BLOB"sv;
                case VariantValue::eDate:
                case VariantValue::eDateTime:
                case VariantValue::eString:
                    return "TEXT"sv;
                case VariantValue::eBoolean:
                case VariantValue::eInteger:
                case VariantValue::eUnsignedInteger:
                    if (f.fIsKeyField) {
                        // see https://stackoverflow.com/questions/20289410/difference-between-int-primary-key-and-integer-primary-key-sqlite/20289487#:~:text=Yes%2C%20there%20is%20a%20difference,separate%20primary%20key%20is%20created.
                        return "INTEGER"sv;
                    }
                    return "INT"sv;
                case VariantValue::eFloat:
                    return "REAL"sv;
            }
        }
        return "TEXT"sv; // @todo better
    }
}

String ORM::Schema::StandardSQLStatements::CreateTable () const
{
    StringBuilder sb;

    /*
     *   CREATE TABLE DEVICES(
     *      ID BLOB PRIMARY KEY DEFAULT(randomblob(16)),
     *      NAME           TEXT    NOT NULL
     *     );
     */
    sb << "CREATE TABLE "sv << fTable.fName << " ("sv;
    bool firstField = true;
    auto addField   = [&] (const Field& fi) {
        if (firstField) {
            firstField = false;
        }
        else {
            sb << ", "sv;
        }
        sb << fi.fName << " "sv;
        sb << GetSQLiteFldType_ (fi) << " "sv;
        if (fi.fIsKeyField == true) {
            sb << " PRIMARY KEY"sv;
        }
        if (fi.fDefaultExpression) {
            if (*fi.fDefaultExpression == Field::kDefaultExpression_AutoIncrement) {
                sb << " AUTOINCREMENT"sv;
            }
            else {
                sb << " DEFAULT("sv << *fi.fDefaultExpression << ")"sv;
            }
        }
        if (fi.fRequired) {
            sb << " NOT NULL"sv;
        }
    };
    for (const Field& i : fTable.fNamedFields) {
        addField (i);
    }
    if (fTable.fSpecialCatchAll) {
        addField (*fTable.fSpecialCatchAll);
    }
    sb << ");"sv;
    return sb.str ();
}

String ORM::Schema::StandardSQLStatements::Insert () const
{
    StringBuilder sb;

    /*
     *   INSERT INTO DEVICES (name) values (:NAME);
     */
    sb << "INSERT INTO "sv + fTable.fName << " ("sv;
    bool firstField   = true;
    auto addFieldName = [&] (const Field& fi) {
        if (firstField) {
            firstField = false;
        }
        else {
            sb << ", "sv;
        }
        sb << fi.fName;
    };
    for (const Field& i : fTable.fNamedFields) {
        addFieldName (i);
    }
    if (fTable.fSpecialCatchAll) {
        addFieldName (*fTable.fSpecialCatchAll);
    }
    sb << ") VALUES ("sv;
    firstField                     = true;
    auto addFieldValueVariableName = [&] (const Field& fi) {
        if (firstField) {
            firstField = false;
        }
        else {
            sb << ", "sv;
        }
        sb << ":"sv << fi.fName;
    };
    for (const Field& i : fTable.fNamedFields) {
        addFieldValueVariableName (i);
    }
    if (fTable.fSpecialCatchAll) {
        addFieldValueVariableName (*fTable.fSpecialCatchAll);
    }
    sb << ");"sv;
    return sb.str ();
}

String ORM::Schema::StandardSQLStatements::DeleteByID () const
{
    StringBuilder sb;
    /*
     *   Delete from DEVICES (ID) where ID=:ID;
     */
    Field indexField = Memory::ValueOf (fTable.GetIDField ());
    sb << "DELETE FROM "sv + fTable.fName << " WHERE "sv << indexField.fName << "=:"sv << indexField.fName << ";"sv;
    return sb.str ();
}

String ORM::Schema::StandardSQLStatements::GetByID () const
{
    StringBuilder sb;
    /*
     *   SELECT * FROM DEVICES where ID=:ID;
     */
    Field indexField = Memory::ValueOf (fTable.GetIDField ());
    sb << "SELECT * FROM "sv + fTable.fName << " WHERE "sv + indexField.fName << "=:"sv << indexField.fName << ";"sv;
    return sb.str ();
}

String ORM::Schema::StandardSQLStatements::UpdateByID () const
{
    StringBuilder sb;
    /*
     *   UPDATE Customers
     *   SET ContactName = 'Alfred Schmidt', City= 'Frankfurt'
     *   WHERE ID=:ID;
     */
    Field indexField = Memory::ValueOf (fTable.GetIDField ());
    sb << "UPDATE "sv << fTable.fName;
    bool firstField  = true;
    auto addSetField = [&] (const Field& fi) {
        if (firstField) {
            firstField = false;
            sb << " SET "sv;
        }
        else {
            sb << ", "sv;
        }
        sb << fi.fName << "=:"sv << fi.fName;
    };
    for (const Field& i : fTable.fNamedFields) {
        if (not i.fIsKeyField) {
            addSetField (i);
        }
    }
    if (fTable.fSpecialCatchAll) {
        addSetField (*fTable.fSpecialCatchAll);
    }
    sb << " WHERE "sv << indexField.fName << "=:"sv << indexField.fName << ";"sv;
    return sb.str ();
}

String ORM::Schema::StandardSQLStatements::GetAllElements () const
{
    StringBuilder sb;
    /*
     *   Select * from DEVICES;
     */
    sb << "SELECT * FROM "sv + fTable.fName << ";"sv;
    return sb.str ();
}
