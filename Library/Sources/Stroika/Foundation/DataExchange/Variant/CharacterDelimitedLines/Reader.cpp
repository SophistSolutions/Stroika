/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/FloatConversion.h"
#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/String_Constant.h"
#include "../../../Streams/TextReader.h"
#include "../../BadFormatException.h"

#include "Reader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using Characters::Character;
using Characters::String;
using Characters::String_Constant;
using Containers::Sequence;
using Containers::Set;
using Memory::Byte;
using Traversal::Iterable;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 *********** DataExchange::::CharacterDelimitedLines::Reader ********************
 ********************************************************************************
 */
class Variant::CharacterDelimitedLines::Reader::Rep_ : public Variant::Reader::_IRep {
public:
    Set<Character> fDelimiters_;
    Rep_ (const Set<Character>& columnDelimiters)
        : fDelimiters_ (columnDelimiters)
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (fDelimiters_);
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return String_Constant (L".txt");
    }
    virtual VariantValue Read (const Streams::InputStream<Byte>::Ptr& in) override
    {
        return Read (Streams::TextReader::New (in));
    }
    virtual VariantValue Read (const Streams::InputStream<Character>::Ptr& /*in*/) override
    {
        AssertNotImplemented ();
        return VariantValue ();
    }
    nonvirtual Iterable<Sequence<String>> ReadMatrix (const Streams::InputStream<Character>::Ptr& in) const
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx ("DataExchange::Variant::CharacterDelimitedLines::Reader::Rep_::ReadMatrix");
#endif
        Sequence<Sequence<String>> result;
        for (String line : in.ReadLines ()) {
            Sequence<String> tokens{line.Tokenize (fDelimiters_)};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"DataExchange::Variant::CharacterDelimitedLines::Reader::ReadMatrix: line=%s, tokenCount=%d", line.c_str (), tokens.size ());
            for (auto i : tokens) {
                DbgTrace (L"token='%s'", i.c_str ());
            }
#endif
            result.Append (tokens);
        }
        return result;
    }
#if 0
    nonvirtual  Mapping<String, String>  ReadAsMapping (const Streams::InputStream<Character>::Ptr& in) const
    {
        //tmphack
        return Mapping<String, String> ();
    }
#endif
};
Variant::CharacterDelimitedLines::Reader::Reader (const Set<Character>& columnDelimiters)
    : inherited (make_shared<Rep_> (columnDelimiters))
{
}

Iterable<Sequence<String>> Variant::CharacterDelimitedLines::Reader::ReadMatrix (const Streams::InputStream<Byte>::Ptr& in) const
{
    return ReadMatrix (Streams::TextReader::New (in));
}

Iterable<Sequence<String>> Variant::CharacterDelimitedLines::Reader::ReadMatrix (const Streams::InputStream<Character>::Ptr& in) const
{
    const _IRep& baseRep = _GetRep ();
    AssertMember (&baseRep, Rep_);
    return reinterpret_cast<const Rep_*> (&baseRep)->ReadMatrix (in);
}
