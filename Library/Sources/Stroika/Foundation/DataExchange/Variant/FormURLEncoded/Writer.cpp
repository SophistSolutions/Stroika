/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/IO/Network/UniformResourceIdentification.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/TextToBinary.h"

#include "Writer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Traversal;

using namespace DataExchange::Variant;

/*
 ********************************************************************************
 ************** DataExchange::Variant::FormURLEncoded::Writer *******************
 ********************************************************************************
 */
class FormURLEncoded::Writer::Rep_ : public Variant::Writer::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    Rep_ ()
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (); // no instance data
    }
    virtual optional<filesystem::path> GetDefaultFileSuffix () const override
    {
        return nullopt;
    }
    virtual void Write (const VariantValue& v, const OutputStream::Ptr<byte>& out) const override
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::Variant::FormURLEncoded::Reader::Rep_::Write"};
#endif
        Association<String, String> assoc = v.As<Mapping<String, VariantValue>> ().Map<Association<String, String>> (
            [] (auto i) { return KeyValuePair<String, String>{i.fKey, i.fValue.template As<String> ()}; });
        Write (assoc, out);
    }
    virtual void Write ([[maybe_unused]] const VariantValue& v, [[maybe_unused]] const OutputStream::Ptr<Character>& out) const override
    {
        AssertNotImplemented (); // not sure if this makes sense ---
    }
    nonvirtual void Write (const Association<String, String>& m, const OutputStream::Ptr<byte>& out) const
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::Variant::FormURLEncoded::Reader::Rep_::Write"};
#endif
        // @todo this would be a good place to have a StreamWriter - instead of having to use a buffer manually
        Memory::StackBuffer<byte> buf;
        for (const KeyValuePair<String, String>& kvp : m) {
            using namespace IO::Network::UniformResourceIdentification;
            constexpr auto kOpts_ = PCTEncodeOptions{};
            if (!buf.empty ()) {
                buf.push_back (static_cast<byte> ('&'));
            }
            buf.push_back (Memory::SpanBytesCast<span<const byte>> (span<const char8_t>{PCTEncode (kvp.fKey, kOpts_)}));
            buf.push_back (static_cast<byte> ('='));
            buf.push_back (Memory::SpanBytesCast<span<const byte>> (span<const char8_t>{PCTEncode (kvp.fValue, kOpts_)}));
        }
        out.Write (buf);
    }
    /*nonvirtual void Write (const Iterable<String>& line, const OutputStream::Ptr<Characters::Character>& out) const
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::Variant::FormURLEncoded::Reader::Rep_::Write"};
#endif
        StringBuilder sb;
        for (Iterator<String> i = line.begin (); i != line.end (); ++i) {
            sb << *i;
            if (i + 1 != line.end ()) {
                sb << fOptions_.fSeparator;
                if (fOptions_.fSpaceSeparate) {
                    sb << " "sv;
                }
            }
        }
        out.WriteLn (sb);
    }*/
};

FormURLEncoded::Writer::Writer ()
    : inherited{make_shared<Rep_> ()}
{
}

void FormURLEncoded::Writer::Write (const Association<String, String>& m, const OutputStream::Ptr<byte>& out)
{
    Debug::UncheckedDynamicCast<Rep_&> (_GetRep ()).Write (m, out);
}

Memory::BLOB FormURLEncoded::Writer::WriteAsBLOB (const Association<String, String>& m)
{
    return _WriteAsBLOBHelper ([&m, this] (const OutputStream::Ptr<byte>& out) { Write (m, out); });
}