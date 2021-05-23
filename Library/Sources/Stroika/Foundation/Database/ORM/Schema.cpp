/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/StringBuilder.h"

#include "Schema.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;
using namespace Stroika::Foundation::DataExchange;

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
            // @todo type check...
            resultFields.Add (fi.fName, *oFieldVal);
        }
        else if (fi.fRequired) {
            // throw or assert?
        }
    }
    // now fold remaining fields into special 'extra' field (for structured non-indexed/non-searchable data)
    Set<String> fields2Accumulate = Set<String>{actualFields.Keys ()} - Set<String>{resultFields.Keys ()};
    if (not fields2Accumulate.empty ()) {
        Mapping<String, VariantValue> extraFields;
        for (auto i : fields2Accumulate) {
            resultFields.Add (i, *actualFields.Lookup (i));
        }
        // convert to JSON and store as string
    }
    return VariantValue{resultFields};
}

VariantValue ORM::Schema::Table::MapFromDB (const VariantValue& vv) const
{
    return VariantValue{};
}

namespace {
    String GetSQLiteFldName_ (const ORM::Schema::Field& f)
    {
        if (f.fVariantType) {
            switch (*f.fVariantType) {
                case VariantValue::Type::eBLOB:
                    return L"BLOB"sv;
                case VariantValue::Type::eString:
                    return L"TEXT"sv;
                case VariantValue::Type::eInteger:
                    return L"INT"sv;
                case VariantValue::Type::eFloat:
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
    for (const Field& i : fNamedFields) {
        if (firstField) {
            firstField = false;
        }
        else {
            sb += L", ";
        }
        sb += i.fName + L" ";
        sb += GetSQLiteFldName_ (i) + L" ";
        if (i.fIsKeyField == true) {
            sb += L" PRIMARY KEY";
        }
        if (i.fNotNull == true) {
            sb += L" NOT NULL";
        }
    }
    //@todo add EXTRA FIELD
    sb += L");";

    return sb.str ();
}
