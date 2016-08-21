/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Characters/String_Constant.h"

#include    "Writer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Streams;

using   Characters::Character;
using   Characters::String_Constant;
using   Memory::Byte;


/*
 ********************************************************************************
 ************************** DataExchange::INI::Writer ***************************
 ********************************************************************************
 */
class   Variant::INI::Writer::Rep_ : public Variant::VariantWriter::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION (Rep_);
public:
    virtual _SharedPtrIRep  Clone () const override
    {
        return make_shared<Rep_> ();        // no instance data
    }
    virtual String          GetDefaultFileSuffix () const override
    {
        return String_Constant (L".ini");
    }
    virtual void    Write (const VariantValue& v, const Streams::OutputStream<Byte>& out) override
    {
        AssertNotImplemented ();
    }
    virtual void    Write (const VariantValue& v, const Streams::OutputStream<Character>& out) override
    {
        AssertNotImplemented ();
    }
};


Variant::INI::Writer::Writer ()
    : inherited (make_shared<Rep_> ())
{
}
