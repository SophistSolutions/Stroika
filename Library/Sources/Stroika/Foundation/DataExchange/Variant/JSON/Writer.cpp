/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/FloatConversion.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Characters/String_Constant.h"
#include "../../../Streams/TextWriter.h"

#include "Writer.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Streams;

using Characters::Character;
using Characters::String_Constant;
using Memory::Byte;

/*
 * TODO:
 *      No known issues
 */

namespace {
    // Just like Writer::Options but without opionals... fill those in
    struct Options_ {
        Options_ (const Variant::JSON::Writer::Options& o)
            : fFloatOptions{o.fFloatOptions.Value ()}
            , fSpacesPerIndent{o.fSpacesPerIndent.Value (4)}
        {
            if (fSpacesPerIndent != 0) {
                fIndentSpace = String_Constant{L" "}.Repeat (fSpacesPerIndent);
            }
        }
        Characters::Float2StringOptions fFloatOptions;
        unsigned int                    fSpacesPerIndent;
        String                          fIndentSpace;
    };
}

/*
 ********************************************************************************
 ********************* DataExchange::JSON::PrettyPrint **************************
 ********************************************************************************
 */
namespace {
    void Indent_ (const Options_& options, const OutputStream<Character>& out, int indentLevel)
    {
#if 1
        // if test not needed, but speed tweak
        if (options.fSpacesPerIndent != 0) {
            out.Write (options.fIndentSpace.Repeat (indentLevel));
        }
#else
        Characters::StringBuilder sb; // String builder for performance
        for (int i = 0; i < indentLevel; ++i) {
            sb.Append (options.fSpacesPerIndent);
        }
        out.Write (sb.begin (), sb.end ());
#endif
    }
}
namespace {
    void PrettyPrint_ (const Options_& options, const VariantValue& v, const OutputStream<Character>& out, int indentLevel);
    void PrettyPrint_Null_ (const Options_& options, const OutputStream<Character>& out)
    {
        out.Write (L"null");
    }
    void PrettyPrint_ (const Options_& options, bool v, const OutputStream<Character>& out)
    {
        if (v) {
            out.Write (L"true");
        }
        else {
            out.Write (L"false");
        }
    }
    void PrettyPrint_ (const Options_& options, long long int v, const OutputStream<Character>& out)
    {
        wchar_t buf[1024];
        (void)::swprintf (buf, NEltsOf (buf), L"%lld", v);
        out.Write (buf);
    }
    void PrettyPrint_ (const Options_& options, unsigned long long int v, const OutputStream<Character>& out)
    {
        wchar_t buf[1024];
        (void)::swprintf (buf, NEltsOf (buf), L"%llu", v);
        out.Write (buf);
    }
    void PrettyPrint_ (const Options_& options, const String& v, const OutputStream<Character>& out);
    void PrettyPrint_ (const Options_& options, long double v, const OutputStream<Character>& out)
    {
#if 1
        String tmp{Characters::Float2String (v, options.fFloatOptions)};
        if (isnan (v) or isinf (v)) {
            PrettyPrint_ (options, tmp, out);
        }
        else {
            out.Write (tmp);
        }
#else
        if (isnan (v) or isinf (v)) {
            PrettyPrint_ (options, Characters::Float2String (v), out);
        }
        else {
            wchar_t buf[1024];
            bool    useScientificNotation = abs (v) >= 1000 or (v != 0 and abs (v) < 0.001); // scientific preserves more precision - but non-scientific looks better
            if (useScientificNotation) {
                (void)::swprintf (buf, NEltsOf (buf), L"%Le", v);
            }
            else {
                (void)::swprintf (buf, NEltsOf (buf), L"%Lf", v);
                Assert (::wcslen (buf) >= 1);
                // trim trailing 0
                for (size_t i = ::wcslen (buf) - 1; buf[i] == '0'; --i) {
                    if (i != 0 and buf[i - 1] != '.') {
                        buf[i] = '\0';
                    }
                }
            }
            out.Write (buf);
        }
#endif
    }
    template <typename CHARITERATOR>
    void PrettyPrint_ (const Options_& options, CHARITERATOR start, CHARITERATOR end, const OutputStream<Character>& out)
    {
        // A backslash can be followed by "\/bfnrtu (@ see http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf)
        Characters::StringBuilder sb;
        sb.Append (L"\"");
        for (auto i = start; i != end; ++i) {
            switch (*i) {
                case '\"':
                    sb.Append (L"\\\"");
                    break;
                case '\b':
                    sb.Append (L"\\b");
                    break;
                case '\f':
                    sb.Append (L"\\f");
                    break;
                case '\n':
                    sb.Append (L"\\n");
                    break;
                case '\r':
                    sb.Append (L"\\r");
                    break;
                case '\t':
                    sb.Append (L"\\t");
                    break;
                case '\\':
                    sb.Append (L"\\\\");
                    break;
                default:
                    // JSON rule is Any code point except " or \ or control character. So OK to emit most large unicode chars - just not control chars
                    wchar_t c = *i;
                    if (std::iswcntrl (c)) {
                        wchar_t buf[10];
                        (void)::swprintf (buf, NEltsOf (buf), L"\\u%04x", static_cast<char16_t> (c));
                        sb.Append (buf);
                    }
                    else {
                        sb.Append (&c, &c + 1);
                    }
                    break;
            }
        }
        sb.Append (L"\"");
        out.Write (sb.begin (), sb.end ());
    }
    void PrettyPrint_ (const Options_& options, const wstring& v, const OutputStream<Character>& out)
    {
        PrettyPrint_ (options, v.begin (), v.end (), out);
    }
    void PrettyPrint_ (const Options_& options, const String& v, const OutputStream<Character>& out)
    {
        pair<const wchar_t*, const wchar_t*> p = v.GetData<wchar_t> ();
        static_assert (sizeof (Character) == sizeof (wchar_t), "sizeof(Character) == sizeof(wchar_t)");
        PrettyPrint_ (options, p.first, p.second, out);
    }
    void PrettyPrint_ (const Options_& options, const vector<VariantValue>& v, const OutputStream<Character>& out, int indentLevel)
    {
        out.Write (L"[\n");
        for (auto i = v.begin (); i != v.end (); ++i) {
            Indent_ (options, out, indentLevel + 1);
            PrettyPrint_ (options, *i, out, indentLevel + 1);
            if (i + 1 != v.end ()) {
                out.Write (L",");
            }
            out.Write (L"\n");
        }
        Indent_ (options, out, indentLevel);
        out.Write (L"]");
    }
    void PrettyPrint_ (const Options_& options, const Mapping<String, VariantValue>& v, const OutputStream<Character>& out, int indentLevel)
    {
        out.Write (L"{\n");
        for (auto i = v.begin (); i != v.end ();) {
            Indent_ (options, out, indentLevel + 1);
            PrettyPrint_ (options, i->fKey, out, indentLevel + 1);
            out.Write (L" : ");
            PrettyPrint_ (options, i->fValue, out, indentLevel + 1);
            ++i;
            if (i != v.end ()) {
                out.Write (L",");
            }
            out.Write (L"\n");
        }
        Indent_ (options, out, indentLevel);
        out.Write (L"}");
    }
    void PrettyPrint_ (const Options_& options, const VariantValue& v, const OutputStream<Character>& out, int indentLevel)
    {
        switch (v.GetType ()) {
            case VariantValue::Type::eNull:
                PrettyPrint_Null_ (options, out);
                break;
            case VariantValue::Type::eBoolean:
                PrettyPrint_ (options, v.As<bool> (), out);
                break;
            case VariantValue::Type::eBLOB:
                PrettyPrint_ (options, v.As<String> (), out);
                break;
            case VariantValue::Type::eDate:
                PrettyPrint_ (options, v.As<wstring> (), out);
                break;
            case VariantValue::Type::eDateTime:
                PrettyPrint_ (options, v.As<wstring> (), out);
                break;
            case VariantValue::Type::eInteger:
                PrettyPrint_ (options, v.As<long long int> (), out);
                break;
            case VariantValue::Type::eUnsignedInteger:
                PrettyPrint_ (options, v.As<unsigned long long int> (), out);
                break;
            case VariantValue::Type::eFloat:
                PrettyPrint_ (options, v.As<long double> (), out);
                break;
            case VariantValue::Type::eString:
                PrettyPrint_ (options, v.As<wstring> (), out);
                break;
            case VariantValue::Type::eMap:
                PrettyPrint_ (options, v.As<Mapping<String, VariantValue>> (), out, indentLevel);
                break;
            case VariantValue::Type::eArray:
                PrettyPrint_ (options, v.As<vector<VariantValue>> (), out, indentLevel);
                break;
            default:
                RequireNotReached (); // only certain types allowed
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
    Options_ fOptions_;
    Rep_ (const Options& options)
        : fOptions_ (options)
    {
    }
    Rep_ (const Options_& options)
        : fOptions_ (options)
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (fOptions_); // no instance data
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return String_Constant (L".json");
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream<Byte>& out) override
    {
        TextWriter textOut (out, TextWriter::Format::eUTF8WithoutBOM);
        PrettyPrint_ (fOptions_, v, textOut, 0);
        textOut.Write (L"\n"); // a single elt not LF terminated, but the entire doc SB.
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream<Character>& out) override
    {
        PrettyPrint_ (fOptions_, v, out, 0);
    }
};

Variant::JSON::Writer::Writer (const Options& options)
    : inherited (make_shared<Rep_> (options))
{
}
