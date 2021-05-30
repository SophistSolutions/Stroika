/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/StringBuilder.h"
#include "../../DataExchange/Variant/JSON/Reader.h"
#include "../../DataExchange/Variant/JSON/Writer.h"

#include "Schema.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::DataExchange;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 *************************** ORM::Schema::CatchAllField *************************
 ********************************************************************************
 */
ORM::Schema::CatchAllField::CatchAllField ()
{
    fName        = L"_other_fields_"sv;
    fVariantType = VariantValue::Type::eString; // sb BLOB probably
}

VariantValue ORM::Schema::CatchAllField::kDefaultMapper_RawToCombined (const Mapping<String, VariantValue>& fields2Map)
{
    return DataExchange::Variant::JSON::Writer{}.WriteAsString (VariantValue{fields2Map});
}

Mapping<String, VariantValue> ORM::Schema::CatchAllField::kDefaultMapper_CombinedToRaw (const VariantValue& fields2Map)
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
    TraceContextBumper ctx{"ORM::Schema::Table::MapToDB", Stroika_Foundation_Debug_OptionalizeTraceArgs (L"fields=%s", Characters::ToString (fields).c_str ())};
#endif
    Mapping<String, VariantValue> resultFields;
    Set<String>                   usedFields; // must track outside of resultFields.Keys () cuz input key could differ from output
    for (const auto& fi : fNamedFields) {
        String srcKey = fi.GetVariantValueFieldName ();
        if (auto oFieldVal = fields.Lookup (srcKey)) {
            if (fi.fVariantType) {
                resultFields.Add (fi.fName, oFieldVal->ConvertTo (*fi.fVariantType));
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
    if (fSpecialCatchAll.has_value () and not fields2Accumulate.empty ()) {
        Mapping<String, VariantValue> extraFields;
        for (auto i : fields2Accumulate) {
            extraFields.Add (i, *fields.Lookup (i));
        }
        // convert to JSON and store as string
        auto mapperFieldsToCombinedField = fSpecialCatchAll->fMapRawFieldsToCombinedField != nullptr ? fSpecialCatchAll->fMapRawFieldsToCombinedField : CatchAllField::kDefaultMapper_RawToCombined;
        resultFields.Add (fSpecialCatchAll->fName, mapperFieldsToCombinedField (extraFields));
    }
    else {
        Require (fields2Accumulate.empty () or fSpecialCatchAll.has_value ());
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"returning: %s", Characters::ToString (vv).c_str ());
#endif
    return resultFields;
}

Mapping<String, VariantValue> ORM::Schema::Table::MapFromDB (const Mapping<String, VariantValue>& fields) const
{
    Mapping<String, VariantValue> resultFields;
    for (const auto& fi : fNamedFields) {
        if (auto oFieldVal = fields.Lookup (fi.fName)) {
            String toName = fi.GetVariantValueFieldName ();
            if (fi.fVariantType) {
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
            auto mapperCombinedToRawFields = fSpecialCatchAll->fMapCombinedFieldToRawFields != nullptr ? fSpecialCatchAll->fMapCombinedFieldToRawFields : CatchAllField::kDefaultMapper_CombinedToRaw;
            // no need to map names here because that is for DBRep to/from VariantValue rep name mapping and there is none for these fields
            resultFields.AddAll (mapperCombinedToRawFields (*o));
        }
    }
    else {
        // @todo maybe check fNamedFields contains all the actual fields??? Maybe OK to not check
    }
    return resultFields;
}

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
                    return L"INT"sv;
                case VariantValue::eFloat:
                    return L"REAL"sv;
            }
        }
        return L"TEXT"sv; // @todo better
    }
}

String ORM::Schema::Table::GetSQLToCreateTable () const
{
    StringBuilder sb;

    /*
     *   CREATE TABLE DEVICES(
     *      ID BLOB PRIMARY KEY DEFAULT(randomblob(16)),
     *      NAME           TEXT    NOT NULL
     *     );
     */
    sb += L"CREATE TABLE " + fName + L" (";
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
        if (fi.fNotNull == true) {
            sb += L" NOT NULL";
        }
    };
    for (const Field& i : fNamedFields) {
        addField (i);
    }
    if (fSpecialCatchAll) {
        addField (*fSpecialCatchAll);
    }
    sb += L");";

    return sb.str ();
}

String ORM::Schema::Table::GetSQLToInsert () const
{
    StringBuilder sb;

    /*
     *   INSERT INTO DEVICES (name) values (:NAME);
     */
    sb += L"INSERT INTO " + fName + L" (";
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
    for (const Field& i : fNamedFields) {
        addFieldName (i);
    }
    if (fSpecialCatchAll) {
        addFieldName (*fSpecialCatchAll);
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
    for (const Field& i : fNamedFields) {
        addFieldValueVariableName (i);
    }
    if (fSpecialCatchAll) {
        addFieldValueVariableName (*fSpecialCatchAll);
    }
    sb += L");";
    return sb.str ();
}

String ORM::Schema::Table::GetSQLToDelete () const
{
    StringBuilder sb;
    /*
     *   Delete from DEVICES (ID) where ID=:ID;
     */
    Field indexField = Memory::ValueOf (fNamedFields.First<Field> ([] (const Field& fi) { if (fi.fIsKeyField) { return fi; } }));
    sb += L"DELETE FROM " + fName + L" WHERE " + indexField.fName + L"=:" + indexField.fName + L";";
    return sb.str ();
}
