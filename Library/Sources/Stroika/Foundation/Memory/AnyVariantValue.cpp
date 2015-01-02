/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "AnyVariantValue.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;



/*
 ********************************************************************************
 ****************************** Memory::AnyVariantValue *************************
 ********************************************************************************
 */
bool    AnyVariantValue::Equals (const AnyVariantValue& rhs) const
{
    if (empty ()) {
        return rhs.empty ();
    }
    AssertNotImplemented ();    // I don't need this for now - add virtual method to TIRep_!!!
    return false;
}
