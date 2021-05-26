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
VariantValue ORM::Schema::Table::MapToDB (const VariantValue& vv) const
{
    Mapping<String, VariantValue> actualFields = vv.As<Mapping<String, VariantValue>> ();
    Mapping<String, VariantValue> resultFields;
    for (const auto& fi : fNamedFields) {
        if (auto oFieldVal = actualFields.Lookup (fi.fName)) {
            if (fi.fVariantType) {
                oFieldVal->CheckConvertibleTo (*fi.fVariantType);
            }
            resultFields.Add (fi.fName, *oFieldVal);
        }
        else if (fi.fRequired) {
            // throw or assert?
        }
    }
    // now fold remaining fields into special 'extra' field (for structured non-indexed/non-searchable data)
    Set<String> fields2Accumulate = Set<String>{actualFields.Keys ()} - Set<String>{resultFields.Keys ()};
    if (fSpecialCatchAll.has_value () and not fields2Accumulate.empty ()) {
        Mapping<String, VariantValue> extraFields;
        for (auto i : fields2Accumulate) {
            resultFields.Add (i, *actualFields.Lookup (i));
        }
        // convert to JSON and store as string
        auto mapperFieldsToCombinedField = fSpecialCatchAll->fMapRawFieldsToCombinedField != nullptr ? fSpecialCatchAll->fMapRawFieldsToCombinedField : CatchAllField::kDefaultMapper_RawToCombined;
        resultFields.Add (fSpecialCatchAll->fName, mapperFieldsToCombinedField (extraFields));
    }
    else {
        Require (fields2Accumulate.empty () or fSpecialCatchAll.has_value ());
    }
    return VariantValue{resultFields};
}

VariantValue ORM::Schema::Table::MapFromDB (const VariantValue& vv) const
{
    Mapping<String, VariantValue> actualFields = vv.As<Mapping<String, VariantValue>> ();
    Mapping<String, VariantValue> resultFields;
    for (const auto& fi : fNamedFields) {
        if (auto oFieldVal = actualFields.Lookup (fi.fName)) {
            if (fi.fVariantType) {
                oFieldVal->CheckConvertibleTo (*fi.fVariantType);
            }
            resultFields.Add (fi.fName, *oFieldVal);
        }
        else if (fi.fRequired) {
            // throw or assert?
        }
    }
    // now fold remaining fields into special 'extra' field (for structured non-indexed/non-searchable data)
    if (fSpecialCatchAll.has_value ()) {
        if (auto o = actualFields.Lookup (fSpecialCatchAll->fName)) {
            auto mapperCombinedToRawFields = fSpecialCatchAll->fMapCombinedFieldToRawFields != nullptr ? fSpecialCatchAll->fMapCombinedFieldToRawFields : CatchAllField::kDefaultMapper_CombinedToRaw;
            resultFields.AddAll (mapperCombinedToRawFields (*o));
        }
    }
    else {
        // @todo maybe check fNamedFields contains all the actual fields??? Maybe OK to not check
    }
    return VariantValue{resultFields};
}

namespace {
    String GetSQLiteFldName_ (const ORM::Schema::Field& f)
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
        sb += GetSQLiteFldName_ (fi) + L" ";
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
