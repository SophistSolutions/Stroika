/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/XML/Binding.h"
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
        // an object's members written as child elements, in the Binding's order - then any it does not name, as the value
        // gives them
        Sequence<Common::KeyValuePair<String, VariantValue>> ElementMembers_ (const Mapping<String, VariantValue>& members, const Binding* binding)
        {
            Sequence<Common::KeyValuePair<String, VariantValue>> result;
            Containers::Set<String>                              named;
            if (binding != nullptr) {
                for (const BindingMember& i : binding->fMembers) {
                    named.Add (i.fKey);
                    if (binding->KindFor (i.fKey) == Binding::Kind::eElement) {
                        if (optional<VariantValue> v = members.Lookup (i.fKey)) {
                            result.Append (Common::KeyValuePair<String, VariantValue>{i.fKey, *v});
                        }
                    }
                }
            }
            for (const auto& i : members) {
                if (not named.Contains (i.fKey)) {
                    result.Append (i);
                }
            }
            return result;
        }
        // a default namespace (xmlns=) where this element names one, and it is not the one it sits in already; a name with none
        // inherits the namespace it is in - saying nothing is not the same as saying 'no namespace'. A prefixed namespace is
        // not expressible yet. Returns the namespace this element's children are in
        optional<URI> WriteNamespaceIfNew_ (const NameWithNamespace& name, const optional<URI>& inheritedNamespace)
        {
            if (not name.fNamespace or name.fNamespace == inheritedNamespace) {
                return inheritedNamespace;
            }
            fOut.Write (" xmlns=\""sv +
                        String{QuoteForXMLAttribute (name.fNamespace->As<String> (IO::Network::URI::StringPCTEncodedFlag::eDecoded))} + "\""sv);
            return name.fNamespace;
        }
        void WriteElement (const NameWithNamespace& name, const VariantValue& v, const Binding* binding,
                           const optional<URI>& inheritedNamespace, int indentLevel)
        {
            if (v.GetType () == VariantValue::eArray) {
                Sequence<VariantValue> items       = v.As<Sequence<VariantValue>> ();
                const Binding*         itemBinding = binding == nullptr ? nullptr : binding->fItems.get ();
                if (fArrayItemElementName.empty ()) {
                    // the member's own name, once per item - how a schema of someone else's usually has it (UPnP's <service>);
                    // then an array of one reads back as a scalar, unless a Binding says that member is an array
                    for (const VariantValue& i : items) {
                        WriteElement (name, i, itemBinding, inheritedNamespace, indentLevel);
                    }
                    return;
                }
                Indent_ (indentLevel);
                fOut.Write ("<"sv + name.fName);
                optional<URI> childNamespace = WriteNamespaceIfNew_ (name, inheritedNamespace);
                fOut.Write (">"sv);
                if (not items.empty ()) {
                    fOut.Write ("\n"sv);
                    for (const VariantValue& i : items) {
                        WriteElement (NameWithNamespace{fArrayItemElementName}, i, itemBinding, childNamespace, indentLevel + 1);
                    }
                    Indent_ (indentLevel);
                }
                fOut.Write ("</"sv + name.fName + ">\n"sv);
                return;
            }
            Indent_ (indentLevel);
            fOut.Write ("<"sv + name.fName);
            optional<URI>                 childNamespace = WriteNamespaceIfNew_ (name, inheritedNamespace);
            Mapping<String, VariantValue> members;
            optional<VariantValue>        textMember; // a member the Binding says is this element's text, not a child element
            if (v.GetType () == VariantValue::eMap) {
                members = v.As<Mapping<String, VariantValue>> ();
                if (binding != nullptr) {
                    for (const BindingMember& i : binding->fMembers) {
                        optional<VariantValue> mv = members.Lookup (i.fKey);
                        if (not mv) {
                            continue;
                        }
                        switch (binding->KindFor (i.fKey)) {
                            case Binding::Kind::eAttribute:
                                // its local name: an attribute in a namespace would need a prefix, not expressible yet
                                fOut.Write (" "sv + binding->NameFor (i.fKey).fName + "=\""sv + String{QuoteForXMLAttribute (mv->As<String> ())} + "\""sv);
                                break;
                            case Binding::Kind::eText:
                                textMember = mv;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            fOut.Write (">"sv);
            if (textMember) {
                fOut.Write (String{QuoteForXML (textMember->As<String> ())});
            }
            else {
                WriteContent (v, members, binding, childNamespace, indentLevel);
            }
            fOut.Write ("</"sv + name.fName + ">\n"sv);
        }
        void WriteContent (const VariantValue& v, const Mapping<String, VariantValue>& members, const Binding* binding,
                           const optional<URI>& inheritedNamespace, int indentLevel)
        {
            switch (v.GetType ()) {
                case VariantValue::eNull:
                    break; // an empty element: XML has no null
                case VariantValue::eMap: {
                    Sequence<Common::KeyValuePair<String, VariantValue>> elements = ElementMembers_ (members, binding);
                    if (not elements.empty ()) {
                        fOut.Write ("\n"sv);
                        for (const auto& i : elements) {
                            const Binding*    memberBinding = binding == nullptr ? nullptr : binding->MemberBinding (i.fKey);
                            NameWithNamespace memberName    = binding == nullptr ? NameWithNamespace{i.fKey} : binding->NameFor (i.fKey);
                            WriteElement (memberName, i.fValue, memberBinding, inheritedNamespace, indentLevel + 1);
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
        , fBinding_{config.GetBinding ()}
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
        Writer_        w{out, fArrayItemElementName_};
        const Binding* binding = fBinding_ ? &*fBinding_ : nullptr;
        if (fDocumentElementName_.empty ()) {
            Require (v.GetType () == VariantValue::eMap);
            Mapping<String, VariantValue> members = v.As<Mapping<String, VariantValue>> ();
            Require (members.size () == 1); // a document has exactly one root element - else name one (SetDocumentElementName)
            for (const auto& i : members) {
                // the root element is a member of the value, so its name and Binding come from the Binding's member of that key
                const Binding*    rootBinding = binding == nullptr ? nullptr : binding->MemberBinding (i.fKey);
                NameWithNamespace rootName    = binding == nullptr ? NameWithNamespace{i.fKey} : binding->NameFor (i.fKey);
                w.WriteElement (rootName, i.fValue, rootBinding, nullopt, 0);
            }
        }
        else {
            // the document element names the root, so the Binding IS the root element's
            NameWithNamespace rootName = (binding != nullptr and binding->fName) ? *binding->fName : NameWithNamespace{fDocumentElementName_};
            w.WriteElement (rootName, v, binding, nullopt, 0);
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
        fBinding_                    = config.GetBinding ();
    }

private:
    SerializationConfiguration fSerializationConfiguration_;
    String                     fDocumentElementName_;
    String                     fArrayItemElementName_;
    optional<Binding>          fBinding_;
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
