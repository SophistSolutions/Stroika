/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Projection.h"

using namespace Stroika::Foundation;

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
    if (const Set<String>* i0 = get_if<0> (&this->fFields)) {
        vv.RetainAll (*i0);
    }
    else if (const Set<String>* i1 = get_if<1> (&this->fFields)) {
        vv.RemoveAll (*i1);
    }
    return vv;
}