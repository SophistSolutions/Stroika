/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/StringBuilder.h"
#include    "../../Characters/String_Constant.h"
#include    "../../Streams/iostream/BinaryOutputStreamFromOStreamAdapter.h"
#include    "../../Streams/TextWriter.h"

#include    "Writer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Streams;

using   Characters::Character;
using   Characters::String_Constant;
using   Memory::Byte;



/*
 * TODO:
 *      No known issues
 */





/*
 ********************************************************************************
 ********************* DataExchange::JSON::PrettyPrint **************************
 ********************************************************************************
 */
namespace   {
    void    Indent_ (const OutputStream<Character>& out, int indentLevel)
    {
        Characters::StringBuilder   sb; // String builder for performance
        for (int i = 0; i < indentLevel; ++i) {
            sb.Append (L"    ");
        }
        out.Write (sb.begin (), sb.end ());
    }
}
namespace   {
    void    PrettyPrint_ (const VariantValue& v, const OutputStream<Character>& out, int indentLevel);
    void    PrettyPrint_Null_ (const OutputStream<Character>& out)
    {
        out.Write (L"null");
    }
    void    PrettyPrint_ (bool v, const OutputStream<Character>& out)
    {
        if (v) {
            out.Write (L"true");
        }
        else {
            out.Write (L"false");
        }
    }
    void    PrettyPrint_ (long long int v, const OutputStream<Character>& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%lld", v);
        out.Write (buf);
    }
    void    PrettyPrint_ (unsigned long long int v, const OutputStream<Character>& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%llu", v);
        out.Write (buf);
    }
    void    PrettyPrint_ (long double v, const OutputStream<Character>& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%Lf", v);
        Assert (::wcslen (buf) >= 1);
        // trim trailing 0
        for (size_t i = ::wcslen (buf) - 1; buf[i] == '0'; --i) {
            if (i != 0 and buf[i - 1] != '.') {
                buf[i] = '\0';
            }
        }
        out.Write (buf);
    }
    template    <typename CHARITERATOR>
    void    PrettyPrint_ (CHARITERATOR start, CHARITERATOR end, const OutputStream<Character>& out)
    {
        Characters::StringBuilder   sb; // String builder for performance
        sb.Append (L"\"");
        for (auto i = start; i != end; ++i) {
            switch (*i) {
                case '\"':
                    sb.Append (L"\\\"");
                    break;
                case '\\':
                    sb.Append (L"\\\\");
                    break;
                case '\n':
                    sb.Append (L"\\n");
                    break;
                case '\r':
                    sb.Append (L"\\r");
                    break;
// unclear if we need to quote other chars such as \f\t\b\ but probably not...
                default:
                    wchar_t c = *i;
                    sb.Append (&c, &c + 1);
                    break;
            }
        }
        sb.Append (L"\"");
        out.Write (sb.begin (), sb.end ());
    }
    void    PrettyPrint_ (const wstring& v, const OutputStream<Character>& out)
    {
        PrettyPrint_ (v.begin (), v.end (), out);
    }
    void    PrettyPrint_ (const String& v, const OutputStream<Character>& out)
    {
        pair<const wchar_t*, const wchar_t*>    p =  v.GetData<wchar_t> ();
        static_assert(sizeof(Character) == sizeof(wchar_t), "sizeof(Character) == sizeof(wchar_t)");
        PrettyPrint_ (p.first, p.second, out);
    }
    void    PrettyPrint_ (const vector<VariantValue>& v, const OutputStream<Character>& out, int indentLevel)
    {
        out.Write (L"[\n");
        for (auto i = v.begin (); i != v.end (); ++i) {
            Indent_ (out, indentLevel + 1);
            PrettyPrint_ (*i, out, indentLevel + 1);
            if (i + 1 != v.end ()) {
                out.Write (L",");
            }
            out.Write (L"\n");
        }
        Indent_ (out, indentLevel);
        out.Write (L"]");
    }
    void    PrettyPrint_ (const Mapping<String, VariantValue>& v, const OutputStream<Character>& out, int indentLevel)
    {
        out.Write (L"{\n");
        for (auto i = v.begin (); i != v.end ();) {
            Indent_ (out, indentLevel + 1);
            PrettyPrint_ (i->fKey, out, indentLevel + 1);
            out.Write (L" : ");
            PrettyPrint_ (i->fValue, out, indentLevel + 1);
            ++i;
            if (i != v.end ()) {
                out.Write (L",");
            }
            out.Write (L"\n");
        }
        Indent_ (out, indentLevel);
        out.Write (L"}");
    }
    void    PrettyPrint_ (const VariantValue& v, const OutputStream<Character>& out, int indentLevel)
    {
        switch (v.GetType ()) {
            case    VariantValue::Type::eNull:
                PrettyPrint_Null_ (out);
                break;
            case    VariantValue::Type::eBoolean:
                PrettyPrint_ (v.As<bool> (), out);
                break;
            case    VariantValue::Type::eDate:
                PrettyPrint_ (v.As<wstring> (), out);
                break;
            case    VariantValue::Type::eDateTime:
                PrettyPrint_ (v.As<wstring> (), out);
                break;
            case    VariantValue::Type::eInteger:
                PrettyPrint_ (v.As<long long int> (), out);
                break;
            case    VariantValue::Type::eUnsignedInteger:
                PrettyPrint_ (v.As<unsigned long long int> (), out);
                break;
            case    VariantValue::Type::eFloat:
                PrettyPrint_ (v.As<long double> (), out);
                break;
            case    VariantValue::Type::eString:
                PrettyPrint_ (v.As<wstring> (), out);
                break;
            case    VariantValue::Type::eMap:
                PrettyPrint_ (v.As<Mapping<String, VariantValue>> (), out, indentLevel);
                break;
            case    VariantValue::Type::eArray:
                PrettyPrint_ (v.As<vector<VariantValue>> (), out, indentLevel);
                break;
            default:
                RequireNotReached ();       // only certain types allowed
        }
    }
}




/*
 ********************************************************************************
 ************************** DataExchange::JSON::Writer **************************
 ********************************************************************************
 */
class   DataExchange::JSON::Writer::Rep_ : public DataExchange::Writer::_IRep {
public:
    virtual _SharedPtrIRep  Clone () const override
    {
        return make_shared<Rep_> ();        // no instance data
    }
    virtual String          GetDefaultFileSuffix () const override
    {
        return String_Constant (L".json");
    }
    virtual void    Write (const VariantValue& v, const Streams::OutputStream<Byte>& out) override
    {
        TextWriter textOut (out, TextWriter::Format::eUTF8WithoutBOM);
        PrettyPrint_ (v, textOut, 0);
        textOut.Write (L"\n");      // a single elt not LF terminated, but the entire doc SB.
    }
    virtual void    Write (const VariantValue& v, const Streams::OutputStream<Character>& out) override
    {
        PrettyPrint_ (v, out, 0);
    }
};


DataExchange::JSON::Writer::Writer ()
    : inherited (make_shared<Rep_> ())
{
}
