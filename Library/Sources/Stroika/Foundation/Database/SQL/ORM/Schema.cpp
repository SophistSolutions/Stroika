/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/StringBuilder.h"
#include "../../../DataExchange/Variant/JSON/Reader.h"
#include "../../../DataExchange/Variant/JSON/Writer.h"
#include "../../../Debug/Trace.h"

#include "Schema.h"

using std::byte;

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
    fName        = L"_other_fields_"sv;
    fVariantType = VariantValue::Type::eBLOB; // can be BLOB or String, but BLOB more compact/efficient
}

function<VariantValue (const Mapping<String, VariantValue>& fields2Map)> ORM::Schema::CatchAllField::GetEffectiveRawToCombined () const
{
    if (fMapRawFieldsToCombinedField != nullptr) {
        return fMapRawFieldsToCombinedField;
    }
    Require (fVariantType);
    switch (*fVariantType) {
        case VariantValue::eBLOB:
            return kDefaultMapper_RawToCombined_BLOB;
        case VariantValue::eString:
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
    Require (fVariantType);
    switch (*fVariantType) {
        case VariantValue::eBLOB:
            return kDefaultMapper_CombinedToRaw_BLOB;
        case VariantValue::eString:
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

Mapping<String, VariantValue> ORM::Schema::CatchAllField::kDefaultMapper_CombinedToRaw_BLOB (const VariantValue& fields2Map)
{
    return DataExchange::Variant::JSON::Reader{}.Read (fields2Map.As<Memory::BLOB> ()).As<Mapping<String, VariantValue>> ();
}

Mapping<String, VariantValue> ORM::Schema::CatchAllField::kDefaultMapper_CombinedToRaw_String (const VariantValue& fields2Map)
{
    return DataExchange::Variant::JSON::Reader{}.Read (fields2Map.As<String> ()).As<Mapping<String, VariantValue>> ();
}

/*
 ********************************************************************************
 ****************************** ORM::Schema::Table ******************************
 ********************************************************************************
 */
Mapping<String, VariantValue> ORM::Schema::Table::MapToDB (const Mapping<String, VariantValue>& fields) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{L"ORM::Schema::Table::MapToDB", Stroika_Foundation_Debug_OptionalizeTraceArgs (L"fields=%s", Characters::ToString (fields).c_str ())};
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
            if (fi.fVariantType) {
                try {
                    resultFields.Add (fi.fName, oFieldVal->ConvertTo (*fi.fVariantType));
                }
                catch (...) {
                    DbgTrace (L"IN ORM::Schema::Table::MapToDB for field %s: %s", fi.fName.c_str (), Characters::ToString (current_exception ()).c_str ());
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
    DbgTrace (L"returning: %s", Characters::ToString (resultFields).c_str ());
#endif
    return resultFields;
}

Mapping<String, VariantValue> ORM::Schema::Table::MapFromDB (const Mapping<String, VariantValue>& fields) const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    TraceContextBumper ctx{L"ORM::Schema::Table::MapFromDB", Stroika_Foundation_Debug_OptionalizeTraceArgs (L"fields=%s", Characters::ToString (fields).c_str ())};
#endif
    Mapping<String, VariantValue> resultFields;
    for (const auto& fi : fNamedFields) {
        if (auto oFieldVal = fields.Lookup (fi.fName)) {
            String toName = fi.GetVariantValueFieldName ();
            if (fi.IsNullable () and oFieldVal->GetType () == VariantValue::eNull) {
                // @todo consider just not adding it if its null (not in DB) and nullable
                resultFields.Add (toName, *oFieldVal);
            }
            else if (fi.fVariantType) {
                resultFields.Add (toName, oFieldVal->ConvertTo (*fi.fVariantType));
            }
            else {
                resultFields.Add (toName, *oFieldVal);
            }
        }
        else if (fi.fRequired) {
            // throw or assert?
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
    String GetSQLiteFldType_ (const ORM::Schema::Field& f)
    {
        if (f.fVariantType) {
            switch (*f.fVariantType) {
                case VariantValue::eBLOB:
                    return L"BLOB"sv;
                case VariantValue::eDate:
                case VariantValue::eDateTime:
                case VariantValue::eString:
                    return L"TEXT"sv;
                case VariantValue::eBoolean:
                case VariantValue::eInteger:
                case VariantValue::eUnsignedInteger:
                    if (f.fIsKeyField) {
                        // see https://stackoverflow.com/questions/20289410/difference-between-int-primary-key-and-integer-primary-key-sqlite/20289487#:~:text=Yes%2C%20there%20is%20a%20difference,separate%20primary%20key%20is%20created.
                        return L"INTEGER"sv;
                    }
                    return L"INT"sv;
                case VariantValue::eFloat:
                    return L"REAL"sv;
            }
        }
        return L"TEXT"sv; // @todo better
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
    sb += L"CREATE TABLE " + fTable.fName + L" (";
    bool firstField = true;
    auto addField   = [&] (const Field& fi) {
        if (firstField) {
            firstField = false;
        }
        else {
            sb += L", ";
        }
        sb += fi.fName + L" ";
        sb += GetSQLiteFldType_ (fi) + L" ";
        if (fi.fIsKeyField == true) {
            sb += L" PRIMARY KEY";
        }
        if (fi.fDefaultExpression) {
            sb += L" DEFAULT(" + *fi.fDefaultExpression + L")";
        }
        if (fi.fNotNull) {
            sb += L" NOT NULL";
        }
        if (fi.fAutoIncrement) {
            sb += L" AUTOINCREMENT";
        }
    };
    for (const Field& i : fTable.fNamedFields) {
        addField (i);
    }
    if (fTable.fSpecialCatchAll) {
        addField (*fTable.fSpecialCatchAll);
    }
    sb += L");";

    return sb.str ();
}

String ORM::Schema::StandardSQLStatements::Insert () const
{
    StringBuilder sb;

    /*
     *   INSERT INTO DEVICES (name) values (:NAME);
     */
    sb += L"INSERT INTO " + fTable.fName + L" (";
    bool firstField   = true;
    auto addFieldName = [&] (const Field& fi) {
        if (firstField) {
            firstField = false;
        }
        else {
            sb += L", ";
        }
        sb += fi.fName;
    };
    for (const Field& i : fTable.fNamedFields) {
        addFieldName (i);
    }
    if (fTable.fSpecialCatchAll) {
        addFieldName (*fTable.fSpecialCatchAll);
    }
    sb += L") VALUES (";
    firstField                     = true;
    auto addFieldValueVariableName = [&] (const Field& fi) {
        if (firstField) {
            firstField = false;
        }
        else {
            sb += L", ";
        }
        sb += L":" + fi.fName;
    };
    for (const Field& i : fTable.fNamedFields) {
        addFieldValueVariableName (i);
    }
    if (fTable.fSpecialCatchAll) {
        addFieldValueVariableName (*fTable.fSpecialCatchAll);
    }
    sb += L");";
    return sb.str ();
}

String ORM::Schema::StandardSQLStatements::DeleteByID () const
{
    StringBuilder sb;
    /*
     *   Delete from DEVICES (ID) where ID=:ID;
     */
    Field indexField = Memory::ValueOf (fTable.GetIDField ());
    sb += L"DELETE FROM " + fTable.fName + L" WHERE " + indexField.fName + L"=:" + indexField.fName + L";";
    return sb.str ();
}

String ORM::Schema::StandardSQLStatements::GetByID () const
{
    StringBuilder sb;
    /*
     *   SELECT * FROM DEVICES where ID=:ID;
     */
    Field indexField = Memory::ValueOf (fTable.GetIDField ());
    sb += L"SELECT * FROM " + fTable.fName + L" WHERE " + indexField.fName + L"=:" + indexField.fName + L";";
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
    sb += L"UPDATE " + fTable.fName;
    bool firstField  = true;
    auto addSetField = [&] (const Field& fi) {
        if (firstField) {
            firstField = false;
            sb += L" SET ";
        }
        else {
            sb += L", ";
        }
        sb += fi.fName + L"=:" + fi.fName;
    };
    for (const Field& i : fTable.fNamedFields) {
        if (not i.fIsKeyField) {
            addSetField (i);
        }
    }
    if (fTable.fSpecialCatchAll) {
        addSetField (*fTable.fSpecialCatchAll);
    }
    sb += L" WHERE " + indexField.fName + L"=:" + indexField.fName + L";";
    return sb.str ();
}

String ORM::Schema::StandardSQLStatements::GetAllElements () const
{
    StringBuilder sb;
    /*
     *   Select * from DEVICES;
     */
    sb += L"SELECT * FROM " + fTable.fName + L";";
    return sb.str ();
}
