/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Reader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Variant;
using namespace Stroika::Foundation::DataExchange::Variant::CharacterDelimitedLines;

using Characters::Character;
using Characters::String;
using Containers::Sequence;
using Containers::Set;
using Traversal::Iterable;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ************** DataExchange::::CharacterDelimitedLines::Reader *****************
 ********************************************************************************
 */
class CharacterDelimitedLines::Reader::Rep_ : public Variant::Reader::_IRep {
public:
    Set<Character> fDelimiters_;
    Rep_ (const Set<Character>& columnDelimiters)
        : fDelimiters_{columnDelimiters}
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (fDelimiters_);
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return ".txt"sv;
    }
    virtual VariantValue Read (const Streams::InputStream::Ptr<byte>& in) override
    {
        return Read (Streams::TextReader::New (in));
    }
    virtual VariantValue Read (const Streams::InputStream::Ptr<Character>& in) override
    {
        // @todo consider if this functional style is more clear than a nested for-loop. Was harder for me to
        // write this way, but that could be my inexpereince... --LGP 2022-12-04
        return VariantValue{ReadMatrix (in).Map<Sequence<VariantValue>> ([] (const Sequence<String>& line) -> VariantValue {
            return VariantValue{line.Map<Iterable<VariantValue>> ([] (const String& i) { return VariantValue{i}; })};
        })};
    }
    nonvirtual Iterable<Sequence<String>> ReadMatrix (const Streams::InputStream::Ptr<Character>& in) const
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::Variant::CharacterDelimitedLines::Reader::Rep_::ReadMatrix"};
#endif
        Sequence<Sequence<String>> result;
        for (const String& line : in.ReadLines ()) {
            Sequence<String> tokens{line.Tokenize (fDelimiters_)};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("DataExchange::Variant::CharacterDelimitedLines::Reader::ReadMatrix: line={}, tokenCount={}"_f, line, tokens.size ());
            for ([[maybe_unused]] const auto& i : tokens) {
                DbgTrace ("token='{}'"_f, i);
            }
#endif
            result.Append (tokens);
        }
        return move (result);
    }
};
CharacterDelimitedLines::Reader::Reader (const Set<Character>& columnDelimiters)
    : inherited{make_shared<Rep_> (columnDelimiters)}
{
}

Iterable<Sequence<String>> CharacterDelimitedLines::Reader::ReadMatrix (const Streams::InputStream::Ptr<byte>& in) const
{
    return ReadMatrix (Streams::TextReader::New (in));
}

Iterable<Sequence<String>> CharacterDelimitedLines::Reader::ReadMatrix (const Streams::InputStream::Ptr<Character>& in) const
{
    return Debug::UncheckedDynamicCast<const Rep_&> (_GetRep ()).ReadMatrix (in);
}
