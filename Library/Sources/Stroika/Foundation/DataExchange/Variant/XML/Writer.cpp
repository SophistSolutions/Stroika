/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/XML/WriterUtils.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/TextToBinary.h"

#include "Writer.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::Streams;

using Memory::MakeSharedPtr;

namespace {
    /*
     *  A VariantValue as XML (Variant::XML::Writer's documentation has the shape): an object's member is an element, an array
     *  is repeated elements, and anything else is its element's text - each scalar as VariantValue::As<String> writes it, so a
     *  date is ISO 8601 and a BLOB base64, as in JSON.
     */
    struct Writer_ {
        OutputStream::Ptr<Character> fOut;
        String                       fArrayItemElementName; // each item wrapped in this; empty: the member's own name repeated instead

        void Indent_ (int indentLevel)
        {
            for (int i = 0; i < indentLevel; ++i) {
                fOut.Write ("    "sv);
            }
        }
        void WriteElement (const String& name, const VariantValue& v, int indentLevel)
        {
            if (v.GetType () == VariantValue::eArray) {
                Sequence<VariantValue> items = v.As<Sequence<VariantValue>> ();
                if (fArrayItemElementName.empty ()) {
                    // the member's own name, once per item - how a schema of someone else's usually has it (UPnP's <service>);
                    // then an array of one reads back as a scalar, which only a binding (or the object it is read into) can tell
                    for (const VariantValue& i : items) {
                        WriteElement (name, i, indentLevel);
                    }
                    return;
                }
                Indent_ (indentLevel);
                fOut.Write ("<"sv + name + ">"sv);
                if (not items.empty ()) {
                    fOut.Write ("\n"sv);
                    for (const VariantValue& i : items) {
                        WriteElement (fArrayItemElementName, i, indentLevel + 1);
                    }
                    Indent_ (indentLevel);
                }
                fOut.Write ("</"sv + name + ">\n"sv);
                return;
            }
            Indent_ (indentLevel);
            fOut.Write ("<"sv + name + ">"sv);
            WriteContent (v, indentLevel);
            fOut.Write ("</"sv + name + ">\n"sv);
        }
        void WriteContent (const VariantValue& v, int indentLevel)
        {
            switch (v.GetType ()) {
                case VariantValue::eNull:
                    break; // an empty element: XML has no null
                case VariantValue::eMap: {
                    Mapping<String, VariantValue> members = v.As<Mapping<String, VariantValue>> ();
                    if (not members.empty ()) {
                        fOut.Write ("\n"sv);
                        for (const auto& i : members) {
                            WriteElement (i.fKey, i.fValue, indentLevel + 1);
                        }
                        Indent_ (indentLevel);
                    }
                } break;
                case VariantValue::eArray:
                    AssertNotReached (); // WriteElement handles an array, needing its element's name
                    break;
                default:
                    fOut.Write (String{QuoteForXML (v.As<String> ())});
                    break;
            }
        }
    };
}

/*
 ********************************************************************************
 ************************** Variant::XML::Writer ********************************
 ********************************************************************************
 */
class Variant::XML::Writer::Rep_ final : public Variant::Writer::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    Rep_ (const SerializationConfiguration& config)
        : fSerializationConfiguration_{config}
        , fDocumentElementName_{config.GetDocumentElementName ().value_or (String{})}
        , fArrayItemElementName_{config.GetArrayElementName ().value_or (String{})}
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return MakeSharedPtr<Rep_> (fSerializationConfiguration_);
    }
    virtual optional<filesystem::path> GetDefaultFileSuffix () const override
    {
        return ".xml"sv;
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream::Ptr<byte>& out) const override
    {
        // the byte overload writes a document, so it declares the encoding it is writing in
        OutputStream::Ptr<Character> useOut = TextToBinary::Writer::New (out, UnicodeExternalEncodings::eUTF8, ByteOrderMark::eDontInclude);
        useOut.Write ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"sv);
        Write_ (v, useOut);
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream::Ptr<Character>& out) const override
    {
        // no declaration here: the caller owns the encoding, and may be writing this into a document of its own
        Write_ (v, out);
    }
    nonvirtual void Write_ (const VariantValue& v, const Streams::OutputStream::Ptr<Character>& out) const
    {
        Writer_ w{out, fArrayItemElementName_};
        if (fDocumentElementName_.empty ()) {
            Require (v.GetType () == VariantValue::eMap);
            Mapping<String, VariantValue> members = v.As<Mapping<String, VariantValue>> ();
            Require (members.size () == 1); // a document has exactly one root element - else name one (SetDocumentElementName)
            for (const auto& i : members) {
                w.WriteElement (i.fKey, i.fValue, 0);
            }
        }
        else {
            w.WriteElement (fDocumentElementName_, v, 0);
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
        fArrayItemElementName_       = config.GetArrayElementName ().value_or (String{});
    }

private:
    SerializationConfiguration fSerializationConfiguration_;
    String                     fDocumentElementName_;
    String                     fArrayItemElementName_;
};

Variant::XML::Writer::Writer (const SerializationConfiguration& config)
    : inherited{MakeSharedPtr<Rep_> (config)}
{
}

Variant::XML::Writer::Rep_& Variant::XML::Writer::GetRep_ ()
{
    EnsureMember (&inherited::_GetRep (), Rep_);
    return reinterpret_cast<Rep_&> (inherited::_GetRep ());
}

const Variant::XML::Writer::Rep_& Variant::XML::Writer::GetRep_ () const
{
    EnsureMember (&inherited::_ConstGetRep (), Rep_);
    return reinterpret_cast<const Rep_&> (inherited::_ConstGetRep ());
}

SerializationConfiguration Variant::XML::Writer::GetConfiguration () const
{
    return GetRep_ ().GetConfiguration ();
}

void Variant::XML::Writer::SetConfiguration (const SerializationConfiguration& config)
{
    GetRep_ ().SetConfiguration (config);
}
