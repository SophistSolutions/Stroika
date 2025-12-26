/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"

#include "Reader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Variant;
using namespace Stroika::Foundation::DataExchange::Variant::CharacterDelimitedLines;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String;
using Containers::Sequence;
using Containers::Set;
using Memory::MakeSharedPtr;
using Traversal::Iterable;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ************** DataExchange::::CharacterDelimitedLines::Reader *****************
 ********************************************************************************
 */
class CharacterDelimitedLines::Reader::Rep_ final : public Variant::Reader::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    Set<Character> fDelimiters_;
    bool           fTrimTokens_{false};
    Rep_ (const Set<Character>& columnDelimiters, bool trimTokens)
        : fDelimiters_{columnDelimiters}
        , fTrimTokens_{trimTokens}
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return MakeSharedPtr<Rep_> (fDelimiters_, fTrimTokens_);
    }
    virtual optional<filesystem::path> GetDefaultFileSuffix () const override
    {
        return ".txt"sv;
    }
    virtual VariantValue Read (const InputStream::Ptr<byte>& in) override
    {
        return Read (Streams::BinaryToText::Reader::New (in));
    }
    virtual VariantValue Read (const InputStream::Ptr<Character>& in) override
    {
        // @todo consider if this functional style is more clear than a nested for-loop. Was harder for me to
        // write this way, but that could be my inexperience... --LGP 2022-12-04
        return VariantValue{ReadMatrix (in).Map<Sequence<VariantValue>> ([] (const Sequence<String>& line) -> VariantValue {
            return VariantValue{line.Map<Iterable<VariantValue>> ([] (const String& i) { return VariantValue{i}; })};
        })};
    }
    nonvirtual Iterable<Sequence<String>> ReadMatrix (const InputStream::Ptr<Character>& in) const
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::Variant::CharacterDelimitedLines::Reader::Rep_::ReadMatrix"};
#endif
        Sequence<Sequence<String>> result;
        for (const String& line : in.ReadLines ()) {
            Sequence<String> tokens{line.Tokenize (fDelimiters_)};
            if (fTrimTokens_) {
                tokens = tokens.Map<Sequence<String>> ([] (auto i) { return i.Trim (); });
            }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("DataExchange::Variant::CharacterDelimitedLines::Reader::ReadMatrix: line={}, tokenCount={}"_f, line, tokens.size ());
            for ([[maybe_unused]] const auto& i : tokens) {
                DbgTrace ("token='{}'"_f, i);
            }
#endif
            result.Append (tokens);
        }
        return result;
    }
};
CharacterDelimitedLines::Reader::Reader (const Set<Character>& columnDelimiters, bool trimTokens)
    : inherited{MakeSharedPtr<Rep_> (columnDelimiters, trimTokens)}
{
}

Iterable<Sequence<String>> CharacterDelimitedLines::Reader::ReadMatrix (const InputStream::Ptr<byte>& in) const
{
    return ReadMatrix (BinaryToText::Reader::New (in));
}

Iterable<Sequence<String>> CharacterDelimitedLines::Reader::ReadMatrix (const InputStream::Ptr<Character>& in) const
{
    return Debug::UncheckedDynamicCast<const Rep_&> (_GetRep ()).ReadMatrix (in);
}
