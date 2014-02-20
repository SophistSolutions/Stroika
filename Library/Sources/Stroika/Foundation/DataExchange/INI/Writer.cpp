/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Streams/iostream/BinaryOutputStreamFromOStreamAdapter.h"
#include    "../../Streams/TextOutputStreamBinaryAdapter.h"

#include    "Writer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Streams;


/*
 ********************************************************************************
 ************************** DataExchange::INI::Writer **************************
 ********************************************************************************
 */
class   DataExchange::INI::Writer::Rep_ : public DataExchange::Writer::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION (Rep_);
public:
    virtual _SharedPtrIRep  Clone () const override
    {
        return _SharedPtrIRep (new Rep_ ());    // no instance data
    }
    virtual void    Write (const VariantValue& v, const Streams::BinaryOutputStream& out) override
    {
        AssertNotImplemented ();
    }
    virtual void    Write (const VariantValue& v, const Streams::TextOutputStream& out) override
    {
        AssertNotImplemented ();
    }
};


DataExchange::INI::Writer::Writer ()
    : inherited (shared_ptr<_IRep> (new Rep_ ()))
{
}
