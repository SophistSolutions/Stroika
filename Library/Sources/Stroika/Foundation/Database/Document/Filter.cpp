/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"

#include "Filter.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Database;
using namespace Database::Document;

/*
 ********************************************************************************
 ********************** Document::FilterElements::Equals ************************
 ********************************************************************************
 */
bool FilterElements::Equals::Matches (const Database::Document::Document& doc) const
{
    if (optional<VariantValue> elt = doc.Lookup (fLHS)) {
        // RHS could be value, or another lookup
        optional<VariantValue> rhsValue = get_if<Value> (&fRHS); // intentionally object slice
        if (!rhsValue) {
            // then fetch from document
            rhsValue = doc.Lookup (get<FieldName> (fRHS));
        }
        return *elt == rhsValue;
    }
    return false;
}

String FilterElements::Equals::ToString () const
{
    StringBuilder sb;
    sb << "{ "sv;
    sb << ", op: EQUALS "sv;
    sb << ", lhs:  "sv << fLHS;
    sb << ", rhs:  "sv << fRHS;
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ********************** Document::FilterElements::Matches ***********************
 ********************************************************************************
 */
bool FilterElements::Matches (const Operation& op, const Database::Document::Document& doc)
{
    if (auto ei = get_if<Equals> (&op)) {
        return ei->Matches (doc);
    }
    AssertNotReached (); // so far only variant supported
    return false;
}

/*
 ********************************************************************************
 ******************************* Document::Filter *******************************
 ********************************************************************************
 */
bool Filter::Matches (const Database::Document::Document& doc) const
{
    for (FilterElements::Operation op : fAndedOperations_) {
        if (not FilterElements::Matches (op, doc)) {
            return false;
        }
    }
    return true;
}

String Filter::ToString () const
{
    StringBuilder sb;
    sb << "{ "sv;
    sb << ", disjunction: "sv << fAndedOperations_;
    sb << "}"sv;
    return sb;
}
