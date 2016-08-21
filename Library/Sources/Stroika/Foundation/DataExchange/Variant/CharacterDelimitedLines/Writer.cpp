/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../../../Characters/String_Constant.h"
#include    "../../../Streams/TextWriter.h"

#include    "Writer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Streams;

using   Characters::Character;
using   Characters::String_Constant;
using   Memory::Byte;



/*
 * TODO:
 *      No known issues
 */





/*
 ********************************************************************************
 *********** DataExchange::Variant::CharacterDelimitedLines::Writer *************
 ********************************************************************************
 */
class   DataExchange::Variant::CharacterDelimitedLines::Writer::Rep_ : public DataExchange::Variant::VariantWriter::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION (Rep_);
public:
    virtual _SharedPtrIRep  Clone () const override
    {
        return make_shared<Rep_> ();    // no instance data
    }
    virtual String          GetDefaultFileSuffix () const override
    {
        return String_Constant (L".txt");
    }
    virtual void    Write (const VariantValue& v, const Streams::OutputStream<Byte>& out) override
    {
        TextWriter textOut (out, TextWriter::Format::eUTF8WithoutBOM);
        AssertNotImplemented ();
        textOut.Write (L"\n");      // a single elt not LF terminated, but the entire doc SB.
    }
    virtual void    Write (const VariantValue& v, const Streams::OutputStream<Character>& out) override
    {
        AssertNotImplemented ();
    }
};


DataExchange::Variant::CharacterDelimitedLines::Writer::Writer ()
    : inherited (make_shared<Rep_> ())
{
}
