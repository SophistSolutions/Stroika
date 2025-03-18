/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Transaction.h"

#include "Connection.h"

using namespace Stroika::Foundation;

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
        optional<VariantValue> rhsValue = get_if<VariantValue> (&fRHS);
        if (!rhsValue) {
            // then fetch from document
            rhsValue = doc.Lookup (get<FieldName> (fRHS));
        }
        return *elt == rhsValue;
    }
    return false;
}

/*
 ********************************************************************************
 ********************** Document::FilterElements::Matches ***********************
 ********************************************************************************
 */
bool FilterElements::Matches (const Operatation& op, const Database::Document::Document& doc)
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
    for (auto op : fAndedOperations) {
        if (not FilterElements::Matches (op, doc)) {
            return false;
        }
    }
    return true;
}