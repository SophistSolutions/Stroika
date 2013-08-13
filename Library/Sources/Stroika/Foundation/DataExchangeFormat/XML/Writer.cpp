/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Streams/iostream/BinaryOutputStreamFromOStreamAdapter.h"
#include    "../../Streams/TextOutputStreamBinaryAdapter.h"

#include    "WriterUtils.h"

#include    "Writer.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::DataExchangeFormat;
using   namespace   Stroika::Foundation::DataExchangeFormat::XML;
using   namespace   Stroika::Foundation::Streams;





/*
 ********************************************************************************
 *************** DataExchangeFormat::XML::PrettyPrint ***************************
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
    void    PrettyPrint_ (bool v, const TextOutputStream& out)
    {
        if (v) {
            out.Write (L"true");
        }
        else {
            out.Write (L"false");
        }
    }
    void    PrettyPrint_ (int v, const TextOutputStream& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%d", v);
        out.Write (buf);
    }
    void    PrettyPrint_ (Memory::VariantValue::FloatType v, const TextOutputStream& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%f", v);
        Assert (::wcslen (buf) >= 1);
        // trim trailing 0
        for (size_t i = ::wcslen (buf) - 1; buf[i] == '0'; --i) {
            if (i >= 0 and buf[i - 1] != '.') {
                buf[i] = '\0';
            }
        }
        out.Write (buf);
    }
    void    PrettyPrint_ (const String& v, const TextOutputStream& out)
    {
        // @todo need variant of QuoteForXML that ONLY quotes special cahracters, and not fancy (eg japaense, etc)
        // characters
        //
        // then can clean this up
        out.Write (String::FromAscii (QuoteForXML (v)));
    }
    void    PrettyPrint_ (const vector<Memory::VariantValue>& v, const TextOutputStream& out, int indentLevel)
    {
        for (auto i = v.begin (); i != v.end (); ++i) {
            Indent_ (out, indentLevel + 1);
            PrettyPrint_ (*i, out, indentLevel + 1);
            out.Write (L"\n");
        }
        Indent_ (out, indentLevel);
    }
    void    PrettyPrint_ (const map<wstring, Memory::VariantValue>& v, const TextOutputStream& out, int indentLevel)
    {
        //@@@@TODO - indents wrong, and must validate first legit xml elt args
        for (auto i = v.begin (); i != v.end ();) {
            Indent_ (out, indentLevel + 1);
            out.Write (L"<");
            out.Write (i->first.c_str ());
            out.Write (L">");
            PrettyPrint_ (i->second, out, indentLevel + 1);
            out.Write (L"</");
            out.Write (i->first.c_str ());
            out.Write (L">");
            ++i;
            out.Write (L"\n");
        }
        Indent_ (out, indentLevel);
    }
    void    PrettyPrint_ (const Memory::VariantValue& v, const TextOutputStream& out, int indentLevel)
    {
        switch (v.GetType ()) {
            case    Memory::VariantValue::Type::eNull:
                break;
            case    Memory::VariantValue::Type::eBoolean:
                PrettyPrint_ (v.As<bool> (), out);
                break;
            case    Memory::VariantValue::Type::eDate:
                PrettyPrint_ (v.As<String> (), out);
                break;
            case    Memory::VariantValue::Type::eDateTime:
                PrettyPrint_ (v.As<String> (), out);
                break;
            case    Memory::VariantValue::Type::eInteger:
                PrettyPrint_ (v.As<int> (), out);
                break;
            case    Memory::VariantValue::Type::eFloat:
                PrettyPrint_ (v.As<Memory::VariantValue::FloatType> (), out);
                break;
            case    Memory::VariantValue::Type::eString:
                PrettyPrint_ (v.As<String> (), out);
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

void    DataExchangeFormat::XML::PrettyPrint (const Memory::VariantValue& v, const Streams::BinaryOutputStream& out)
{
    PrettyPrint_ (v, TextOutputStreamBinaryAdapter (out, TextOutputStreamBinaryAdapter::Format::eUTF8WithoutBOM), 0);
}

void    DataExchangeFormat::XML::PrettyPrint (const Memory::VariantValue& v, const Streams::TextOutputStream& out)
{
    PrettyPrint_ (v, out, 0);
}
