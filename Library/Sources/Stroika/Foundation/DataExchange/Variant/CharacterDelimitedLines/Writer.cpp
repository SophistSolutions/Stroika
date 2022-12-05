/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Streams/TextWriter.h"

#include "Writer.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Traversal::Iterable;
using Traversal::Iterator;
using namespace DataExchange::Variant;

/*
 ********************************************************************************
 *********** DataExchange::Variant::CharacterDelimitedLines::Writer *************
 ********************************************************************************
 */
class CharacterDelimitedLines::Writer::Rep_ : public Variant::Writer::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    Rep_ (const Options& options)
        : fOptions_{options}
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (fOptions_); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return L".txt"sv;
    }
    virtual void Write (const VariantValue& v, const OutputStream<byte>::Ptr& out) override
    {
        Write (v, TextWriter::New (out, TextWriter::Format::eUTF8WithoutBOM));
    }
    virtual void Write (const VariantValue& v, const OutputStream<Character>::Ptr& out) override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::Variant::CharacterDelimitedLines::Reader::Rep_::Write"};
#endif
        for (VariantValue line : v.As<Sequence<VariantValue>> ()) {
            Write (line.As<Sequence<VariantValue>> ().Map<String> ([] (const VariantValue& i) { return i.As<String> (); }), out);
        }
    }
    nonvirtual void Write (const Iterable<Sequence<String>>& m, const OutputStream<Characters::Character>::Ptr& out) const
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::Variant::CharacterDelimitedLines::Reader::Rep_::Write"};
#endif
        for (const Sequence<String>& seq : m) {
            Write (seq, out);
        }
    }
    nonvirtual void Write (const Iterable<String>& line, const OutputStream<Characters::Character>::Ptr& out) const
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::Variant::CharacterDelimitedLines::Reader::Rep_::Write"};
#endif
        StringBuilder sb;
        for (Iterator<String> i = line.begin (); i != line.end (); ++i) {
            sb += *i;
            if (i + 1 != line.end ()) {
                sb += fOptions_.fSeparator;
                if (fOptions_.fSpaceSeparate) {
                    sb += L" "sv;
                }
            }
        }
        out.WriteLn (sb.str ());
    }

private:
    Options fOptions_;
};

CharacterDelimitedLines::Writer::Writer (const Options& options)
    : inherited{make_shared<Rep_> (options)}
{
}

void CharacterDelimitedLines::Writer::Write (const Iterable<Sequence<String>>& m, const OutputStream<std::byte>::Ptr& out)
{
    return Write (m, Streams::TextWriter::New (out));
}

void CharacterDelimitedLines::Writer::Write (const Iterable<Sequence<String>>& m, const OutputStream<Characters::Character>::Ptr& out)
{
    Debug::UncheckedDynamicCast<Rep_&> (_GetRep ()).Write (m, out);
}

Memory::BLOB CharacterDelimitedLines::Writer::WriteAsBLOB (const Iterable<Sequence<String>>& m)
{
    return _WriteAsBLOBHelper ([&m, this] (const OutputStream<std::byte>::Ptr& out) { Write (m, out); });
}

String CharacterDelimitedLines::Writer::WriteAsString (const Iterable<Sequence<String>>& m)
{
    return _WriteAsStringHelper ([&m, this] (const OutputStream<Characters::Character>::Ptr& out) { Write (m, out); });
}
