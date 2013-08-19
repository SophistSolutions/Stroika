/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/StringUtils.h"
#include    "../../Characters/Format.h"
#include    "../../Streams/TextInputStreamBinaryAdapter.h"

#include    "../BadFormatException.h"

#include    "Reader.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;
using   namespace   Stroika::Foundation::DataExchangeFormat::XML;


class   DataExchangeFormat::XML::Reader::Rep_ : public DataExchangeFormat::Reader::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION (Rep_);
public:
    Rep_ (const SerializationConfiguration& config)
        : fSerializationConfiguration_ (config) {
    }
public:
    virtual Memory::VariantValue    Read (const Streams::BinaryInputStream& in) override {
        // not sure about this - we may want to led xerces read raw binary bytes!!
        return Read (Streams::TextInputStreamBinaryAdapter (in));
    }
    virtual Memory::VariantValue    Read (const Streams::TextInputStream& in) override {

        // TODO - USE SAXREADER HERE!!!
#if     qHasLibrary_Xerces
        AssertNotImplemented ();
        return Memory::VariantValue ();
#else
        AssertNotImplemented ();
        return Memory::VariantValue ();
#endif
    }
    nonvirtual  SerializationConfiguration GetConfiguration () const {
        return fSerializationConfiguration_;
    }
    nonvirtual  void    SetConfiguration (const SerializationConfiguration& config) {
        fSerializationConfiguration_ = config;
    }

private:
    SerializationConfiguration  fSerializationConfiguration_;
};




/*
 ********************************************************************************
 ******************** DataExchangeFormat::XML::Reader ***************************
 ********************************************************************************
 */
DataExchangeFormat::XML::Reader::Reader (const SerializationConfiguration& config)
    : inherited (shared_ptr<_IRep> (new Rep_ (config)))
{
}

shared_ptr<DataExchangeFormat::XML::Reader::Rep_>   DataExchangeFormat::XML::Reader::GetRep_ () const
{
    // no need for this exactly since cannot throw...
    return dynamic_pointer_cast<DataExchangeFormat::XML::Reader::Rep_> (inherited::_GetRep ());
}

SerializationConfiguration DataExchangeFormat::XML::Reader::GetConfiguration () const
{
    return GetRep_ ()->GetConfiguration ();
}

void    DataExchangeFormat::XML::Reader::SetConfiguration (const SerializationConfiguration& config)
{
    GetRep_ ()->SetConfiguration (config);
}
