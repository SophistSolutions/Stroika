/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/String_Constant.h"
#include "../../../Streams/TextWriter.h"

#include "Writer.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String_Constant;

/*
 * TODO:
 *      No known issues
 */

/*
 ********************************************************************************
 *********** DataExchange::Variant::CharacterDelimitedLines::Writer *************
 ********************************************************************************
 */
class DataExchange::Variant::CharacterDelimitedLines::Writer::Rep_ : public Variant::Writer::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return String_Constant (L".txt");
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream<byte>::Ptr& out) override
    {
        TextWriter::Ptr textOut = TextWriter::New (out, TextWriter::Format::eUTF8WithoutBOM);
        AssertNotImplemented ();
        textOut.Write (L"\n"); // a single elt not LF terminated, but the entire doc SB.
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream<Character>::Ptr& out) override
    {
        AssertNotImplemented ();
    }
};

DataExchange::Variant::CharacterDelimitedLines::Writer::Writer ()
    : inherited (make_shared<Rep_> ())
{
}
