/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Streams/TextWriter.h"

#include "Writer.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;

using Characters::Character;

/*
 * TODO:
 *      No known issues
 */

namespace {
    // Just like Writer::Options but without optionals... fill those in
    struct OptionValues_ {
        OptionValues_ (const Variant::JSON::Writer::Options& o)
            : fFloatOptions{o.fFloatOptions.value_or (Characters::FloatConversion::ToStringOptions{})}
            , fJSONPrettyPrint{o.fJSONPrettyPrint.value_or (true)}
            , fSpacesPerIndent{o.fSpacesPerIndent.value_or (4)}
            , fAllowNANInf{o.fAllowNANInf.value_or (true)}
            , fLineTermination{o.fLineTermination.value_or (Characters::kEOL<char>)}
        {
            if (not fJSONPrettyPrint) {
                fSpacesPerIndent = 0;
            }
            if (fSpacesPerIndent != 0) {
                fIndentSpace = String{" "sv}.Repeat (fSpacesPerIndent);
            }
        }
        Characters::FloatConversion::ToStringOptions fFloatOptions;
        bool                                         fJSONPrettyPrint;
        unsigned int                                 fSpacesPerIndent;
        String                                       fIndentSpace;
        bool                                         fAllowNANInf;
        String                                       fLineTermination;
    };
}

/*
 ********************************************************************************
 ********************* DataExchange::JSON::PrettyPrint **************************
 ********************************************************************************
 */
namespace {
    void Indent_ (const OptionValues_& options, const OutputStream::Ptr<Character>& out, int indentLevel)
    {
        // if test not needed, but speed tweak (especially since incorporates options.fJSONPrettyPrint check
        if (options.fSpacesPerIndent != 0) {
            constexpr bool kSpeedTweak_ = true;
            if constexpr (kSpeedTweak_) {
                for (int i = 0; i < indentLevel; ++i) {
                    out.Write (options.fIndentSpace);
                }
            }
            else {
                out.Write (options.fIndentSpace.Repeat (indentLevel));
            }
        }
    }
}
namespace {
    void PrettyPrint_ (const OptionValues_& options, const VariantValue& v, const OutputStream::Ptr<Character>& out, int indentLevel);
    void PrettyPrint_Null_ (const OptionValues_& /*options*/, const OutputStream::Ptr<Character>& out)
    {
        out.Write ("null"sv);
    }
    void PrettyPrint_ (const OptionValues_& /*options*/, bool v, const OutputStream::Ptr<Character>& out)
    {
        if (v) {
            out.Write ("true"sv);
        }
        else {
            out.Write ("false"sv);
        }
    }
    void PrettyPrint_ (const OptionValues_& /*options*/, long long int v, const OutputStream::Ptr<Character>& out)
    {
        wchar_t buf[1024];
        (void)::swprintf (buf, Memory::NEltsOf (buf), L"%lld", v);
        out.Write (buf);
    }
    void PrettyPrint_ (const OptionValues_& /*options*/, unsigned long long int v, const OutputStream::Ptr<Character>& out)
    {
        wchar_t buf[1024];
        (void)::swprintf (buf, Memory::NEltsOf (buf), L"%llu", v);
        out.Write (buf);
    }
    void PrettyPrint_ (const OptionValues_& options, const String& v, const OutputStream::Ptr<Character>& out);
    void PrettyPrint_ (const OptionValues_& options, long double v, const OutputStream::Ptr<Character>& out)
    {
        String tmp{Characters::FloatConversion::ToString (v, options.fFloatOptions)};
        if (isnan (v) or isinf (v)) {
            Require (options.fAllowNANInf);
            PrettyPrint_ (options, tmp, out);
        }
        else {
            out.Write (tmp);
        }
    }
    template <typename CHARITERATOR>
    void PrettyPrint_ (const OptionValues_& /*options*/, CHARITERATOR start, CHARITERATOR end, const OutputStream::Ptr<Character>& out)
    {
        // A backslash can be followed by "\/bfnrtu (@ see http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf)
        Characters::StringBuilder sb;
        sb.Append ('\"');
        for (auto i = start; i != end; ++i) {
            switch (*i) {
                case '\"':
                    sb.Append ("\\\""sv);
                    break;
                case '\b':
                    sb.Append ("\\b"sv);
                    break;
                case '\f':
                    sb.Append ("\\f"sv);
                    break;
                case '\n':
                    sb.Append ("\\n"sv);
                    break;
                case '\r':
                    sb.Append ("\\r"sv);
                    break;
                case '\t':
                    sb.Append ("\\t"sv);
                    break;
                case '\\':
                    sb.Append ("\\\\"sv);
                    break;
                default:
                    // JSON rule is Any code point except " or \ or control character. So OK to emit most large unicode chars - just not control chars
                    Character c = *i;
                    if (c.IsSurrogatePair ()) [[unlikely]] {
                        wchar_t buf[20];
                        (void)::swprintf (buf, Memory::NEltsOf (buf), L"\\u%04x\\u%04x", c.GetSurrogatePair ().first, c.GetSurrogatePair ().second);
                        sb.Append (buf);
                    }
                    else if (c.IsControl ()) [[unlikely]] {
                        wchar_t buf[10];
                        (void)::swprintf (buf, Memory::NEltsOf (buf), L"\\u%04x", static_cast<char16_t> (c.GetCharacterCode ()));
                        sb.Append (buf);
                    }
                    else {
                        sb.Append (c);
                    }
                    break;
            }
        }
        sb.Append ('\"');
        Memory::StackBuffer<wchar_t> probablyIgnoredBuf;
        out.Write (sb.GetData (&probablyIgnoredBuf));
    }
    void PrettyPrint_ (const OptionValues_& options, const wstring& v, const OutputStream::Ptr<Character>& out)
    {
        PrettyPrint_ (options, v.begin (), v.end (), out);
    }
    void PrettyPrint_ (const OptionValues_& options, const String& v, const OutputStream::Ptr<Character>& out)
    {
        Memory::StackBuffer<char32_t> ignored;
        span<const char32_t>          p = v.GetData (&ignored);
        PrettyPrint_ (options, p.data (), p.data () + p.size (), out);
    }
    void PrettyPrint_ (const OptionValues_& options, const vector<VariantValue>& v, const OutputStream::Ptr<Character>& out, int indentLevel)
    {
        out.Write ('[');
        if (options.fJSONPrettyPrint) {
            out.Write (options.fLineTermination);
        }
        for (auto i = v.begin (); i != v.end (); ++i) {
            Indent_ (options, out, indentLevel + 1);
            PrettyPrint_ (options, *i, out, indentLevel + 1);
            if (i + 1 != v.end ()) {
                out.Write (',');
            }
            if (options.fJSONPrettyPrint) {
                out.Write (options.fLineTermination);
            }
        }
        Indent_ (options, out, indentLevel);
        out.Write (']');
    }
    void PrettyPrint_ (const OptionValues_& options, const Mapping<String, VariantValue>& v, const OutputStream::Ptr<Character>& out, int indentLevel)
    {
        out.Write ('{');
        if (options.fJSONPrettyPrint) {
            out.Write (options.fLineTermination);
        }
        for (auto i = v.begin (); i != v.end ();) {
            Indent_ (options, out, indentLevel + 1);
            PrettyPrint_ (options, i->fKey, out, indentLevel + 1);
            out.Write (" : "sv);
            PrettyPrint_ (options, i->fValue, out, indentLevel + 1);
            ++i;
            if (i != v.end ()) {
                out.Write (',');
            }
            if (options.fJSONPrettyPrint) {
                out.Write (options.fLineTermination);
            }
        }
        Indent_ (options, out, indentLevel);
        out.Write ('}');
    }
    void PrettyPrint_ (const OptionValues_& options, const VariantValue& v, const OutputStream::Ptr<Character>& out, int indentLevel)
    {
        switch (v.GetType ()) {
            case VariantValue::eNull:
                PrettyPrint_Null_ (options, out);
                break;
            case VariantValue::eBoolean:
                PrettyPrint_ (options, v.As<bool> (), out);
                break;
            case VariantValue::eInteger:
                PrettyPrint_ (options, v.As<long long int> (), out);
                break;
            case VariantValue::eUnsignedInteger:
                PrettyPrint_ (options, v.As<unsigned long long int> (), out);
                break;
            case VariantValue::eFloat:
                PrettyPrint_ (options, v.As<long double> (), out);
                break;
            case VariantValue::eMap:
                PrettyPrint_ (options, v.As<Mapping<String, VariantValue>> (), out, indentLevel);
                break;
            case VariantValue::eArray:
                PrettyPrint_ (options, v.As<vector<VariantValue>> (), out, indentLevel);
                break;
            default:
                PrettyPrint_ (options, v.As<String> (), out);
        }
    }
}

/*
 ********************************************************************************
 ************************** DataExchange::JSON::Writer **************************
 ********************************************************************************
 */
class Variant::JSON::Writer::Rep_ : public Variant::Writer::_IRep {
public:
    OptionValues_ fOptions_;
    Rep_ (const Options& options)
        : fOptions_{options}
    {
    }
    Rep_ (const OptionValues_& options)
        : fOptions_{options}
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (fOptions_); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return ".json"sv;
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream::Ptr<byte>& out) const override
    {
        TextWriter::Ptr textOut = TextWriter::New (out, UnicodeExternalEncodings::eUTF8, ByteOrderMark::eDontInclude);
        PrettyPrint_ (fOptions_, v, textOut, 0);
        if (fOptions_.fJSONPrettyPrint) {
            textOut.Write (fOptions_.fLineTermination); // a single elt not LF terminated, but the entire doc should be.
        }
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream::Ptr<Character>& out) const override
    {
        PrettyPrint_ (fOptions_, v, out, 0);
    }
};

Variant::JSON::Writer::Writer (const Options& options)
    : inherited{make_shared<Rep_> (options)}
{
}
