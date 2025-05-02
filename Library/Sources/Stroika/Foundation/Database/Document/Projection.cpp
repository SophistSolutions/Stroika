/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"

#include "Projection.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Database;
using namespace Database::Document;

/*
 ********************************************************************************
 *************************** Document::Projection *******************************
 ********************************************************************************
 */
Database::Document::Document Projection::Apply (const Database::Document::Document& d) const
{
    Mapping<String, VariantValue> vv = d;
    if (const Set<String>* i0 = get_if<eOmit> (&this->fFields_)) {
        vv.RemoveAll (*i0);
    }
    else if (const Set<String>* i1 = get_if<eInclude> (&this->fFields_)) {
        vv.RetainAll (*i1);
    }
    else {
        AssertNotReached ();
    }
    return vv;
}

bool Projection::Includes (const String& fieldName) const
{
    if (const Set<String>* i0 = get_if<eOmit> (&this->fFields_)) {
        return not i0->Contains (fieldName);
    }
    else if (const Set<String>* i1 = get_if<eInclude> (&this->fFields_)) {
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
    sb << "{ "sv;
    if (const Set<String>* i0 = get_if<eOmit> (&this->fFields_)) {
        sb << "omit: "sv << *i0;
    }
    else if (const Set<String>* i1 = get_if<eInclude> (&this->fFields_)) {
        sb << "include: "sv << *i1;
    }
    sb << "}"sv;
    return sb;
}