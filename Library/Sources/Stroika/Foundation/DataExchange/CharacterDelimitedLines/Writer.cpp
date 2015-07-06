/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/String_Constant.h"
#include    "../../Streams/iostream/BinaryOutputStreamFromOStreamAdapter.h"
#include    "../../Streams/TextWriter.h"

#include    "Writer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Streams;

using   Characters::String_Constant;
using   Memory::Byte;



/*
 * TODO:
 *      No known issues
 */





/*
 ********************************************************************************
 *********** DataExchange::CharacterDelimitedLines::Writer **********************
 ********************************************************************************
 */
class   DataExchange::CharacterDelimitedLines::Writer::Rep_ : public DataExchange::Writer::_IRep {
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
    virtual void    Write (const VariantValue& v, const Streams::TextOutputStream& out) override
    {
        AssertNotImplemented ();
    }
};


DataExchange::CharacterDelimitedLines::Writer::Writer ()
    : inherited (make_shared<Rep_> ())
{
}
