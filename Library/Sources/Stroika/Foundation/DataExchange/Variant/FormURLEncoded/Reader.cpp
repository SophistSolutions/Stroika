/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/IO/Network/UniformResourceIdentification.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/StreamReader.h"

#include "Reader.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::Variant;
using namespace Stroika::Foundation::DataExchange::Variant::FormURLEncoded;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String;
using Containers::Sequence;
using Containers::Set;
using Traversal::Iterable;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1
/*
 ********************************************************************************
 ****************** DataExchange::::FormURLEncoded::Reader **********************
 ********************************************************************************
 */
class FormURLEncoded::Reader::Rep_ final : public Variant::Reader::_IRep {
public:
    Rep_ () = default;
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> ();
    }
    virtual optional<filesystem::path> GetDefaultFileSuffix () const override
    {
        return nullopt;
    }
    virtual VariantValue Read (const InputStream::Ptr<byte>& in) override
    {
        return VariantValue{ReadAssociation (in).Map<Mapping<String, VariantValue>> (
            [] (auto i) { return KeyValuePair<String, VariantValue>{i.fKey, VariantValue{i.fValue}}; })};
    }
    virtual VariantValue Read (const InputStream::Ptr<Character>& in) override
    {
        // Treat a string as mapped to UTF-8 and interpret that, not sure if thats they best thing todo - maybe disallow/throw?
        return Read (Streams::TextToBinary::Reader::New (in));
    }
    nonvirtual Association<String, String> ReadAssociation (const InputStream::Ptr<byte>& in) const
    {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"DataExchange::Variant::FormURLEncoded::Reader::Rep_::ReadMatrix"};
#endif
        Association<String, String> result;
        Streams::StreamReader<byte> streamReader{in};
        auto                        toString = [] (span<byte> bytes) {
            // https://url.spec.whatwg.org/#application/x-www-form-urlencoded says map '+' to space, but thats not part of URL-encode apparently
            for (auto&& c : bytes) {
                if (c == static_cast<byte> ('+')) {
                    c = static_cast<byte> (' ');
                }
            }
            u8string asU8Str{reinterpret_cast<const char8_t*> (bytes.data ()), bytes.size ()};
            return IO::Network::UniformResourceIdentification::PCTDecode2String (asU8Str);
        };
        while (not streamReader.IsAtEOF ()) {
            Memory::InlineBuffer<byte> lineBuf;
            span<byte>                 line = streamReader.ReadBlocking (&lineBuf, static_cast<byte> ('&'));
            if (not line.empty ()) {
                if (auto eqSign = find (line.begin (), line.end (), static_cast<byte> ('=')); eqSign != line.end ()) {
                    // add a=b
                    result.Add (toString (line.subspan (0, eqSign - line.begin ())), toString (line.subspan (eqSign + 1 - line.begin ())));
                }
                else {
                    // add a=
                    result.Add (toString (line), String{});
                }
            }
        }
        return result;
    }
};
FormURLEncoded::Reader::Reader ()
    : inherited{make_shared<Rep_> ()}
{
}

Association<String, String> FormURLEncoded::Reader::ReadAssociation (const InputStream::Ptr<byte>& in) const
{
    return Debug::UncheckedDynamicCast<const Rep_&> (_GetRep ()).ReadAssociation (in);
}
