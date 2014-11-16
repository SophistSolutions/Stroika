/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/Format.h"
#include    "../../Characters/String_Constant.h"
#include    "../../Characters/String2Float.h"
#include    "../../Characters/String2Int.h"
#include    "../../Streams/TextInputStreamBinaryAdapter.h"
#include    "../BadFormatException.h"

#include    "Reader.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;

using   Characters::String_Constant;
using   Containers::Sequence;
using   Containers::Set;
using   Characters::Character;
using   Characters::String;
using   Traversal::Iterable;



// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1




/*
 ********************************************************************************
 *********** DataExchange::CharacterDelimitedLines::Reader **********************
 ********************************************************************************
 */
class   DataExchange::CharacterDelimitedLines::Reader::Rep_ : public DataExchange::Reader::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION (Rep_);
public:
    Set<Character>  fDelimiters_;
    Rep_ (const Set<Character>& columnDelimiters)
        : fDelimiters_ (columnDelimiters)
    {
    }
    virtual _SharedPtrIRep  Clone () const override
    {
        return _SharedPtrIRep (new Rep_ (fDelimiters_));    // no instance data
    }
    virtual String          GetDefaultFileSuffix () const override
    {
        return String_Constant (L".txt");
    }
    virtual VariantValue    Read (const Streams::BinaryInputStream& in) override
    {
        return Read (Streams::TextInputStreamBinaryAdapter (in));
    }
    virtual VariantValue    Read (const Streams::TextInputStream& in) override
    {
        AssertNotImplemented ();
        return VariantValue ();
    }
    nonvirtual  Iterable<Sequence<String>>  ReadMatrix (const Streams::TextInputStream& in) const
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
    nonvirtual  Mapping<String, String>  ReadAsMapping (const Streams::TextInputStream& in) const
    {
        //tmphack
        return Mapping<String, String> ();
    }
#endif

};
DataExchange::CharacterDelimitedLines::Reader::Reader (const Set<Character>& columnDelimiters)
    : inherited (shared_ptr<_IRep> (new Rep_ (columnDelimiters)))
{
}

Iterable<Sequence<String>>  DataExchange::CharacterDelimitedLines::Reader::ReadMatrix (const Streams::BinaryInputStream& in) const
{
    return ReadMatrix (Streams::TextInputStreamBinaryAdapter (in));
}

Iterable<Sequence<String>>  DataExchange::CharacterDelimitedLines::Reader::ReadMatrix (const Streams::TextInputStream& in) const
{
    const _IRep& baseRep = _GetRep ();
    AssertMember (&baseRep, Rep_);
    return reinterpret_cast<const Rep_*> (&baseRep)->ReadMatrix (in);
}
