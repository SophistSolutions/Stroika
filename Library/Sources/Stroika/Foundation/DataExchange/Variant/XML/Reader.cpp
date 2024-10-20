/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Reader.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;

using Characters::Character;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

class Variant::XML::Reader::Rep_ : public Variant::Reader::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    Rep_ (const SerializationConfiguration& config)
        : fSerializationConfiguration_{config}
    {
    }

public:
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (fSerializationConfiguration_);
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return ".xml"sv;
    }
    virtual VariantValue Read (const Streams::InputStream::Ptr<byte>& in) override
    {
        // not sure about this - we may want to led xerces read raw binary bytes!!
        return Read (Streams::TextReader::New (in));
    }
    virtual VariantValue Read ([[maybe_unused]] const Streams::InputStream::Ptr<Character>& in) override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::XML::Reader::Rep_::Read"};
#endif
// TODO - USE SAXREADER HERE!!!
#if qHasFeature_Xerces
        AssertNotImplemented ();
        return VariantValue{};
#else
        AssertNotImplemented ();
        return VariantValue{};
#endif
    }
    nonvirtual SerializationConfiguration GetConfiguration () const
    {
        return fSerializationConfiguration_;
    }
    nonvirtual void SetConfiguration (const SerializationConfiguration& config)
    {
        fSerializationConfiguration_ = config;
    }

private:
    SerializationConfiguration fSerializationConfiguration_;
};

/*
 ********************************************************************************
 ************************** DataExchange::XML::Reader ***************************
 ********************************************************************************
 */
Variant::XML::Reader::Reader (const SerializationConfiguration& config)
    : inherited{make_shared<Rep_> (config)}
{
}

Variant::XML::Reader::Rep_& Variant::XML::Reader::GetRep_ ()
{
    EnsureMember (&inherited::_GetRep (), Rep_);
    return reinterpret_cast<Rep_&> (inherited::_GetRep ());
}

const Variant::XML::Reader::Rep_& Variant::XML::Reader::GetRep_ () const
{
    EnsureMember (&inherited::_GetRep (), Rep_);
    return reinterpret_cast<const Rep_&> (inherited::_GetRep ());
}

SerializationConfiguration Variant::XML::Reader::GetConfiguration () const
{
    return GetRep_ ().GetConfiguration ();
}

void Variant::XML::Reader::SetConfiguration (const SerializationConfiguration& config)
{
    GetRep_ ().SetConfiguration (config);
}
