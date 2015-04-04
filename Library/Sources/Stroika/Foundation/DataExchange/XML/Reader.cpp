/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/Format.h"
#include    "../../Characters/String_Constant.h"
#include    "../../Streams/TextInputStreamBinaryAdapter.h"

#include    "../BadFormatException.h"

#include    "Reader.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::XML;

using   Characters::String_Constant;




class   DataExchange::XML::Reader::Rep_ : public DataExchange::Reader::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION (Rep_);
public:
    Rep_ (const SerializationConfiguration& config)
        : fSerializationConfiguration_ (config)
    {
    }
public:
    virtual _SharedPtrIRep  Clone () const override
    {
        return _SharedPtrIRep (new Rep_ (fSerializationConfiguration_));
    }
    virtual String          GetDefaultFileSuffix () const override
    {
        return String_Constant (L".xml");
    }
    virtual VariantValue    Read (const Streams::BinaryInputStream& in) override
    {
        // not sure about this - we may want to led xerces read raw binary bytes!!
        return Read (Streams::TextInputStreamBinaryAdapter (in));
    }
    virtual VariantValue    Read (const Streams::TextInputStream& in) override
    {

        // TODO - USE SAXREADER HERE!!!
#if     qHasFeature_Xerces
        AssertNotImplemented ();
        return VariantValue ();
#else
        AssertNotImplemented ();
        return VariantValue ();
#endif
    }
    nonvirtual  SerializationConfiguration GetConfiguration () const
    {
        return fSerializationConfiguration_;
    }
    nonvirtual  void    SetConfiguration (const SerializationConfiguration& config)
    {
        fSerializationConfiguration_ = config;
    }

private:
    SerializationConfiguration  fSerializationConfiguration_;
};




/*
 ********************************************************************************
 ************************** DataExchange::XML::Reader ***************************
 ********************************************************************************
 */
DataExchange::XML::Reader::Reader (const SerializationConfiguration& config)
    : inherited (shared_ptr<_IRep> (new Rep_ (config)))
{
}

DataExchange::XML::Reader::Rep_&   DataExchange::XML::Reader::GetRep_ ()
{
    EnsureMember (&inherited::_GetRep (), Rep_);
    return reinterpret_cast<Rep_&> (inherited::_GetRep ());
}

const DataExchange::XML::Reader::Rep_&   DataExchange::XML::Reader::GetRep_ () const
{
    EnsureMember (&inherited::_GetRep (), Rep_);
    return reinterpret_cast<const Rep_&> (inherited::_GetRep ());
}

SerializationConfiguration DataExchange::XML::Reader::GetConfiguration () const
{
    return GetRep_ ().GetConfiguration ();
}

void    DataExchange::XML::Reader::SetConfiguration (const SerializationConfiguration& config)
{
    GetRep_ ().SetConfiguration (config);
}
