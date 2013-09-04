/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Streams/iostream/BinaryOutputStreamFromOStreamAdapter.h"
#include    "../../Streams/TextOutputStreamBinaryAdapter.h"

#include    "Writer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;
using   namespace   Stroika::Foundation::Streams;


/*
 * TODO:
 *      No known issues
 */





/*
 ********************************************************************************
 *************** DataExchangeFormat::JSON::PrettyPrint **************************
 ********************************************************************************
 */
namespace   {
    void    Indent_ (const TextOutputStream& out, int indentLevel)
    {
        for (int i = 0; i < indentLevel; ++i) {
            out.Write (L"    ");
        }
    }
}
namespace   {
    void    PrettyPrint_ (const Memory::VariantValue& v, const TextOutputStream& out, int indentLevel);
    void    PrettyPrint_Null_ (const TextOutputStream& out)
    {
        out.Write (L"null");
    }
    void    PrettyPrint_ (bool v, const TextOutputStream& out)
    {
        if (v) {
            out.Write (L"true");
        }
        else {
            out.Write (L"false");
        }
    }
    void    PrettyPrint_ (long long int v, const TextOutputStream& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%lld", v);
        out.Write (buf);
    }
    void    PrettyPrint_ (unsigned long long int v, const TextOutputStream& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%llu", v);
        out.Write (buf);
    }
    void    PrettyPrint_ (long double v, const TextOutputStream& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%Lf", v);
        Assert (::wcslen (buf) >= 1);
        // trim trailing 0
        for (size_t i = ::wcslen (buf) - 1; buf[i] == '0'; --i) {
            if (i >= 0 and buf[i - 1] != '.') {
                buf[i] = '\0';
            }
        }
        out.Write (buf);
    }
    void    PrettyPrint_ (const wstring& v, const TextOutputStream& out)
    {
        out.Write (L"\"");
        for (auto i = v.begin (); i != v.end (); ++i) {
            switch (*i) {
                case '\"':
                    out.Write (L"\\\"");
                    break;
                case '\\':
                    out.Write (L"\\\\");
                    break;
                case '\n':
                    out.Write (L"\\n");
                    break;
                case '\r':
                    out.Write (L"\\r");
                    break;
// unclear if we need to quote other chars such as \f\t\b\ but probably not...
                default:
                    wchar_t c = *i;
                    out.Write (&c, &c + 1);
                    break;
            }
        }
        out.Write (L"\"");
    }
    void    PrettyPrint_ (const vector<Memory::VariantValue>& v, const TextOutputStream& out, int indentLevel)
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
    void    PrettyPrint_ (const map<wstring, Memory::VariantValue>& v, const TextOutputStream& out, int indentLevel)
    {
        out.Write (L"{\n");
        for (auto i = v.begin (); i != v.end ();) {
            Indent_ (out, indentLevel + 1);
            PrettyPrint_ (i->first, out, indentLevel + 1);
            out.Write (L" : ");
            PrettyPrint_ (i->second, out, indentLevel + 1);
            ++i;
            if (i != v.end ()) {
                out.Write (L",");
            }
            out.Write (L"\n");
        }
        Indent_ (out, indentLevel);
        out.Write (L"}");
    }
    void    PrettyPrint_ (const Memory::VariantValue& v, const TextOutputStream& out, int indentLevel)
    {
        switch (v.GetType ()) {
            case    Memory::VariantValue::Type::eNull:
                PrettyPrint_Null_ (out);
                break;
            case    Memory::VariantValue::Type::eBoolean:
                PrettyPrint_ (v.As<bool> (), out);
                break;
            case    Memory::VariantValue::Type::eDate:
                PrettyPrint_ (v.As<wstring> (), out);
                break;
            case    Memory::VariantValue::Type::eDateTime:
                PrettyPrint_ (v.As<wstring> (), out);
                break;
            case    Memory::VariantValue::Type::eInteger:
                PrettyPrint_ (v.As<long long int> (), out);
                break;
            case    Memory::VariantValue::Type::eUnsignedInteger:
                PrettyPrint_ (v.As<unsigned long long int> (), out);
                break;
            case    Memory::VariantValue::Type::eFloat:
                PrettyPrint_ (v.As<long double> (), out);
                break;
            case    Memory::VariantValue::Type::eString:
                PrettyPrint_ (v.As<wstring> (), out);
                break;
            case    Memory::VariantValue::Type::eMap:
                PrettyPrint_ (v.As<map<wstring, Memory::VariantValue>> (), out, indentLevel);
                break;
            case    Memory::VariantValue::Type::eArray:
                PrettyPrint_ (v.As<vector<Memory::VariantValue>> (), out, indentLevel);
                break;
            default:
                RequireNotReached ();       // only certain types allowed
        }
    }
}




/*
 ********************************************************************************
 ******************** DataExchangeFormat::JSON::Writer **************************
 ********************************************************************************
 */
class   DataExchangeFormat::JSON::Writer::Rep_ : public DataExchangeFormat::Writer::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION (Rep_);
public:
    virtual _SharedPtrIRep  Clone () const override {
        return _SharedPtrIRep (new Rep_ ());    // no instance data
    }
    virtual void    Write (const Memory::VariantValue& v, const Streams::BinaryOutputStream& out) override {
        TextOutputStreamBinaryAdapter textOut (out, TextOutputStreamBinaryAdapter::Format::eUTF8WithoutBOM);
        PrettyPrint_ (v, textOut, 0);
        textOut.Write (L"\n");      // a single elt not LF terminated, but the entire doc SB.
    }
    virtual void    Write (const Memory::VariantValue& v, const Streams::TextOutputStream& out) override {
        PrettyPrint_ (v, out, 0);
    }
};


DataExchangeFormat::JSON::Writer::Writer ()
    : inherited (shared_ptr<_IRep> (new Rep_ ()))
{
}
