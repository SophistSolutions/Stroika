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
    if (const Set<String>* i0 = get_if<0> (&this->fFields_)) { // eOmit
        vv.RemoveAll (*i0);
    }
    else if (const Set<String>* i1 = get_if<1> (&this->fFields_)) { // eInclude
        vv.RetainAll (*i1);
    }
    return vv;
}

String Projection::ToString () const
{
    StringBuilder sb;
    sb << "{ "sv;
    if (const Set<String>* i0 = get_if<0> (&this->fFields_)) { // eOmit
        sb << "omit: "sv << *i0;
    }
    else if (const Set<String>* i1 = get_if<1> (&this->fFields_)) { // eInclude
        sb << "include: "sv << *i1;
    }
    sb << "}"sv;
    return sb;
}