/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"

#include "Projection.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Database;

using namespace Database::Document;

/*
 ********************************************************************************
 *************************** Document::Projection *******************************
 ********************************************************************************
 */
Database::Document::Document Projection::Apply (const Database::Document::Document& d) const
{
    Mapping<String, VariantValue> vv = d;
    if (const Set<String>* i0 = get_if<static_cast<int> (eOmit)> (&fFields_)) {
        vv.RemoveAll (*i0);
    }
    else if (const Set<String>* i1 = get_if<static_cast<int> (eInclude)> (&fFields_)) {
        vv.RetainAll (*i1);
    }
    else {
        AssertNotReached ();
    }
    return vv;
}

bool Projection::Includes (const String& fieldName) const
{
    if (const Set<String>* i0 = get_if<static_cast<int> (eOmit)> (&fFields_)) {
        return not i0->Contains (fieldName);
    }
    else if (const Set<String>* i1 = get_if<static_cast<int> (eInclude)> (&fFields_)) {
        return i1->Contains (fieldName);
    }
    else {
        AssertNotReached ();
        return false;
    }
}

String Projection::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (const Set<String>* i0 = get_if<static_cast<int> (eOmit)> (&fFields_)) {
        sb << "omit: "sv << *i0;
    }
    else if (const Set<String>* i1 = get_if<static_cast<int> (eInclude)> (&fFields_)) {
        sb << "include: "sv << *i1;
    }
    sb << "}"sv;
    return sb;
}