/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/String_Constant.h"

#include "Writer.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String_Constant;

/*
 ********************************************************************************
 ************************** DataExchange::INI::Writer ***************************
 ********************************************************************************
 */
class Variant::INI::Writer::Rep_ : public Variant::Writer::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return L".ini"sv;
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream<byte>::Ptr& out) override
    {
        AssertNotImplemented ();
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream<Character>::Ptr& out) override
    {
        AssertNotImplemented ();
    }
};

Variant::INI::Writer::Writer ()
    : inherited (make_shared<Rep_> ())
{
}
