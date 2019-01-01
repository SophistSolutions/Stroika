/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/String_Constant.h"
#include "../../../Streams/TextWriter.h"

#include "../../XML/WriterUtils.h"

#include "Writer.h"

using std::byte;

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::Streams;

namespace {
    void Indent_ (const OutputStream<Character>::Ptr& out, int indentLevel)
    {
        for (int i = 0; i < indentLevel; ++i) {
            out.Write (String_Constant (L"    "));
        }
    }
}
namespace {
    static const String kTrueLbl_  = String_Constant{L"true"};
    static const String kFalseLbl_ = String_Constant{L"false"};
    void                PrettyPrint_ (const VariantValue& v, const OutputStream<Character>::Ptr& out, int indentLevel);
    void                PrettyPrint_ (bool v, const OutputStream<Character>::Ptr& out)
    {
        if (v) {
            out.Write (kTrueLbl_);
        }
        else {
            out.Write (kFalseLbl_);
        }
    }
    void PrettyPrint_ (long long int v, const OutputStream<Character>::Ptr& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%lld", v);
        out.Write (buf);
    }
    void PrettyPrint_ (unsigned long long int v, const OutputStream<Character>::Ptr& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%llu", v);
        out.Write (buf);
    }
    void PrettyPrint_ (long double v, const OutputStream<Character>::Ptr& out)
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
    void PrettyPrint_ (const String& v, const OutputStream<Character>::Ptr& out)
    {
        // @todo need variant of QuoteForXML that ONLY quotes special cahracters, and not fancy (eg japaense, etc)
        // characters
        //
        // then can clean this up
        out.Write (String::FromASCII (QuoteForXML (v)));
    }
    void PrettyPrint_ (const vector<VariantValue>& v, const OutputStream<Character>::Ptr& out, int indentLevel)
    {
        for (auto i = v.begin (); i != v.end (); ++i) {
            PrettyPrint_ (*i, out, indentLevel + 1);
            out.Write (L"\n");
        }
        Indent_ (out, indentLevel);
    }
    void PrettyPrint_ (const map<wstring, VariantValue>& v, const OutputStream<Character>::Ptr& out, int indentLevel)
    {
        //@@@@TODO - must validate first legit xml elt args
        out.Write (L"\n");
        for (auto i = v.begin (); i != v.end ();) {
            Indent_ (out, indentLevel);
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
    }
    void PrettyPrint_ (const VariantValue& v, const OutputStream<Character>::Ptr& out, int indentLevel)
    {
        switch (v.GetType ()) {
            case VariantValue::eNull:
                break;
            case VariantValue::eBoolean:
                PrettyPrint_ (v.As<bool> (), out);
                break;
            case VariantValue::eDate:
                PrettyPrint_ (v.As<String> (), out);
                break;
            case VariantValue::eDateTime:
                PrettyPrint_ (v.As<String> (), out);
                break;
            case VariantValue::eInteger:
                PrettyPrint_ (v.As<long long int> (), out);
                break;
            case VariantValue::eUnsignedInteger:
                PrettyPrint_ (v.As<unsigned long long int> (), out);
                break;
            case VariantValue::eFloat:
                PrettyPrint_ (v.As<long double> (), out);
                break;
            case VariantValue::eString:
                PrettyPrint_ (v.As<String> (), out);
                break;
            case VariantValue::eMap:
                PrettyPrint_ (v.As<map<wstring, VariantValue>> (), out, indentLevel);
                break;
            case VariantValue::eArray:
                PrettyPrint_ (v.As<vector<VariantValue>> (), out, indentLevel);
                break;
            default:
                RequireNotReached (); // only certain types allowed
        }
    }
}

/*
 ********************************************************************************
 ************************** Variant::XML::Writer ********************************
 ********************************************************************************
 */
class Variant::XML::Writer::Rep_ : public Variant::Writer::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    Rep_ (const SerializationConfiguration& config)
        : fSerializationConfiguration_ (config)
        , fDocumentElementName_ (config.GetDocumentElementName ().value_or (String{}))
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return make_shared<Rep_> (fSerializationConfiguration_);
    }
    virtual String GetDefaultFileSuffix () const override
    {
        return String_Constant (L".xml");
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream<byte>::Ptr& out) override
    {
        if (fDocumentElementName_.empty ()) {
            Require (v.GetType () == VariantValue::eMap);
            PrettyPrint_ (v, TextWriter::New (out, TextWriter::Format::eUTF8WithoutBOM), 0);
        }
        else {
            Containers::Mapping<String, VariantValue> v2;
            v2.Add (fDocumentElementName_, v);
            PrettyPrint_ (v2, TextWriter::New (out, TextWriter::Format::eUTF8WithoutBOM), 0);
        }
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream<Character>::Ptr& out) override
    {
        if (fDocumentElementName_.empty ()) {
            Require (v.GetType () == VariantValue::eMap);
            PrettyPrint_ (v, out, 0);
        }
        else {
            Containers::Mapping<String, VariantValue> v2;
            v2.Add (fDocumentElementName_, v);
            PrettyPrint_ (v2, out, 0);
        }
    }
    nonvirtual SerializationConfiguration GetConfiguration () const
    {
        return fSerializationConfiguration_;
    }
    nonvirtual void SetConfiguration (const SerializationConfiguration& config)
    {
        fSerializationConfiguration_ = config;
        fDocumentElementName_        = config.GetDocumentElementName ().value_or (String{});
    }

private:
    SerializationConfiguration fSerializationConfiguration_;
    String                     fDocumentElementName_;
};

Variant::XML::Writer::Writer (const SerializationConfiguration& config)
    : inherited (make_shared<Rep_> (config))
{
}

Variant::XML::Writer::Rep_& Variant::XML::Writer::GetRep_ ()
{
    EnsureMember (&inherited::_GetRep (), Rep_);
    return reinterpret_cast<Rep_&> (inherited::_GetRep ());
}

const Variant::XML::Writer::Rep_& Variant::XML::Writer::GetRep_ () const
{
    EnsureMember (&inherited::_GetRep (), Rep_);
    return reinterpret_cast<const Rep_&> (inherited::_GetRep ());
}

SerializationConfiguration Variant::XML::Writer::GetConfiguration () const
{
    return GetRep_ ().GetConfiguration ();
}

void Variant::XML::Writer::SetConfiguration (const SerializationConfiguration& config)
{
    GetRep_ ().SetConfiguration (config);
}
