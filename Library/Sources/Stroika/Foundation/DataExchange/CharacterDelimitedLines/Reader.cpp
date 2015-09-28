/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/FloatConversion.h"
#include    "../../Characters/Format.h"
#include    "../../Characters/String_Constant.h"
#include    "../../Characters/String2Int.h"
#include    "../../Streams/TextReader.h"
#include    "../BadFormatException.h"

#include    "Reader.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;

using   Characters::String_Constant;
using   Containers::Sequence;
using   Containers::Set;
using   Characters::Character;
using   Characters::String;
using   Memory::Byte;
using   Traversal::Iterable;



// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1




/*
 ********************************************************************************
 *********** DataExchange::CharacterDelimitedLines::Reader **********************
 ********************************************************************************
 */
class   DataExchange::CharacterDelimitedLines::Reader::Rep_ : public DataExchange::VariantReader::_IRep {
public:
    Set<Character>  fDelimiters_;
    Rep_ (const Set<Character>& columnDelimiters)
        : fDelimiters_ (columnDelimiters)
    {
    }
    virtual _SharedPtrIRep  Clone () const override
    {
        return make_shared<Rep_> (fDelimiters_);
    }
    virtual String          GetDefaultFileSuffix () const override
    {
        return String_Constant (L".txt");
    }
    virtual VariantValue    Read (const Streams::InputStream<Byte>& in) override
    {
        return Read (Streams::TextReader (in));
    }
    virtual VariantValue    Read (const Streams::InputStream<Character>& in) override
    {
        AssertNotImplemented ();
        return VariantValue ();
    }
    nonvirtual  Iterable<Sequence<String>>  ReadMatrix (const Streams::InputStream<Character>& in) const
    {
        Sequence<Sequence<String>>  result;
        for (String line : in.ReadLines ()) {
            Sequence<String>    tokens  { line.Tokenize (fDelimiters_) };
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"DataExchange::CharacterDelimitedLines::Reader::ReadMatrix: line=%s, tokenCount=%d", line.c_str (), tokens.size ());
            for (auto i : tokens) {
                DbgTrace (L"token='%s'", i.c_str ());
            }
#endif
            result.Append (tokens);
        }
        return result;
    }
#if 0
    nonvirtual  Mapping<String, String>  ReadAsMapping (const Streams::InputStream<Character>& in) const
    {
        //tmphack
        return Mapping<String, String> ();
    }
#endif

};
DataExchange::CharacterDelimitedLines::Reader::Reader (const Set<Character>& columnDelimiters)
    : inherited (make_shared<Rep_> (columnDelimiters))
{
}

Iterable<Sequence<String>>  DataExchange::CharacterDelimitedLines::Reader::ReadMatrix (const Streams::InputStream<Byte>& in) const
{
    return ReadMatrix (Streams::TextReader (in));
}

Iterable<Sequence<String>>  DataExchange::CharacterDelimitedLines::Reader::ReadMatrix (const Streams::InputStream<Character>& in) const
{
    const _IRep& baseRep = _GetRep ();
    AssertMember (&baseRep, Rep_);
    return reinterpret_cast<const Rep_*> (&baseRep)->ReadMatrix (in);
}
