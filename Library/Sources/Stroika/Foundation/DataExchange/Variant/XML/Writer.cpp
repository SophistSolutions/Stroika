/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/XML/WriterUtils.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/TextToBinary.h"

#include "Writer.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::Variant::XML;
using namespace Stroika::Foundation::Streams;

using Memory::MakeSharedPtr;

namespace {
    // XML 1.0's NameStartChar and NameChar (https://www.w3.org/TR/xml/#NT-NameStartChar), less ':' - so an NCName
    // (https://www.w3.org/TR/xml-names/#NT-NCName), as a local name and a prefix must each be
    constexpr bool IsNCNameStartChar_ (char32_t c)
    {
        return (c >= 'A' and c <= 'Z') or c == '_' or (c >= 'a' and c <= 'z') or (c >= 0xC0 and c <= 0xD6) or (c >= 0xD8 and c <= 0xF6) or
               (c >= 0xF8 and c <= 0x2FF) or (c >= 0x370 and c <= 0x37D) or (c >= 0x37F and c <= 0x1FFF) or (c >= 0x200C and c <= 0x200D) or
               (c >= 0x2070 and c <= 0x218F) or (c >= 0x2C00 and c <= 0x2FEF) or (c >= 0x3001 and c <= 0xD7FF) or
               (c >= 0xF900 and c <= 0xFDCF) or (c >= 0xFDF0 and c <= 0xFFFD) or (c >= 0x10000 and c <= 0xEFFFF);
    }
    constexpr bool IsNCNameChar_ (char32_t c)
    {
        return IsNCNameStartChar_ (c) or c == '-' or c == '.' or (c >= '0' and c <= '9') or c == 0xB7 or (c >= 0x300 and c <= 0x36F) or
               (c >= 0x203F and c <= 0x2040);
    }
    // a name comes from data - a member's key - as often as from a Binding, and one that is not an XML name would be written
    // as malformed XML, which nothing could read back
    void ThrowUnlessNCName_ (const String& name)
    {
        bool ok    = not name.empty ();
        bool first = true;
        for (Character c : name) {
            ok    = ok and (first ? IsNCNameStartChar_ (c.GetCharacterCode ()) : IsNCNameChar_ (c.GetCharacterCode ()));
            first = false;
        }
        if (not ok) {
            Execution::Throw (BadFormatException{L"'{}' is not an XML name, so cannot be written as one"_f(name)});
        }
    }

    /*
     *  A VariantValue as XML (Variant::XML::Writer's documentation has the shape): an object's member is an element, an array
     *  is repeated elements, and anything else is its element's text - each scalar as VariantValue::As<String> writes it, so a
     *  date is ISO 8601 and a BLOB base64, as in JSON.
     */
    struct Writer_ {
        OutputStream::Ptr<Character> fOut;
        bool                         fPrettyPrint;
        String                       fIndent; // one level's

        // what an element inherits from the elements it is inside
        struct Scope_ {
            optional<URI>        fDefaultNamespace; // what a name with no namespace of its own is in
            Mapping<URI, String> fPrefixes;         // a namespace written with a prefix, not as a default one
            const Binding*       fDefaultArrayItems{&Binding::kDefaultArrayItems}; // the item Binding of an array with no fItems
        };

        void NewLine_ ()
        {
            if (fPrettyPrint) {
                fOut.Write ("\n"sv);
            }
        }
        void Indent_ (int indentLevel)
        {
            if (fPrettyPrint) {
                for (int i = 0; i < indentLevel; ++i) {
                    fOut.Write (fIndent);
                }
            }
        }
        // a prefix names one namespace at a time, so binding it to this one unbinds any other it named
        static void BindPrefix_ (Mapping<URI, String>* prefixes, const URI& ns, const String& prefix)
        {
            prefixes->RemoveAll ([&] (const Common::KeyValuePair<URI, String>& i) { return i.fValue == prefix; });
            prefixes->Add (ns, prefix);
        }
        // an object's members written as child elements, in the Binding's order - then any it does not name, as the value
        // gives them
        Sequence<Common::KeyValuePair<String, VariantValue>> ElementMembers_ (const Mapping<String, VariantValue>& members, const Binding* binding)
        {
            Sequence<Common::KeyValuePair<String, VariantValue>> result;
            Containers::Set<String>                              named;
            if (binding != nullptr) {
                for (const Binding::Member& i : binding->fMembers) {
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
        // prefix:local, for a namespace given a prefix; else its local name
        void WriteName_ (const NameWithNamespace& name, const Scope_& scope)
        {
            ThrowUnlessNCName_ (name.fName);
            if (name.fNamespace) {
                if (optional<String> prefix = scope.fPrefixes.Lookup (*name.fNamespace)) {
                    fOut.Write (*prefix);
                    fOut.Write (':');
                }
            }
            fOut.Write (name.fName);
        }
        // a default namespace (xmlns=) where this element names one, and it is not the one it sits in already; a name with none
        // inherits the namespace it is in - saying nothing is not the same as saying 'no namespace'. Returns the namespace
        // this element's children are in
        optional<URI> WriteNamespaceIfNew_ (const NameWithNamespace& name, const Scope_& scope)
        {
            if (not name.fNamespace or name.fNamespace == scope.fDefaultNamespace) {
                return scope.fDefaultNamespace;
            }
            if (scope.fPrefixes.ContainsKey (*name.fNamespace)) {
                return scope.fDefaultNamespace; // written prefix:local, so the default namespace is untouched
            }
            fOut.Write (" xmlns=\""sv);
            fOut.Write (String{QuoteForXMLAttribute (name.fNamespace->As<String> (IO::Network::URI::StringPCTEncodedFlag::eDecoded))});
            fOut.Write ('"');
            return name.fNamespace;
        }
        // '<', the name, the prefixes this element declares (xmlns:p=, and now in scope) where they are not in scope already,
        // and the default namespace where it starts one - all but the attributes and the '>'
        void StartTag_ (const NameWithNamespace& name, const Mapping<URI, String>& declare, Scope_* scope, int indentLevel)
        {
            Mapping<URI, String> newPrefixes;
            for (const auto& i : declare) {
                if (scope->fPrefixes.Lookup (i.fKey) != i.fValue) {
                    ThrowUnlessNCName_ (i.fValue);
                    newPrefixes.Add (i.fKey, i.fValue);
                    BindPrefix_ (&scope->fPrefixes, i.fKey, i.fValue);
                }
            }
            Indent_ (indentLevel);
            fOut.Write ('<');
            WriteName_ (name, *scope);
            for (const auto& i : newPrefixes) {
                fOut.Write (" xmlns:"sv);
                fOut.Write (i.fValue);
                fOut.Write ("=\""sv);
                fOut.Write (String{QuoteForXMLAttribute (i.fKey.As<String> (IO::Network::URI::StringPCTEncodedFlag::eDecoded))});
                fOut.Write ('"');
            }
            scope->fDefaultNamespace = WriteNamespaceIfNew_ (name, *scope);
        }
        void EndTag_ (const NameWithNamespace& name, const Scope_& scope)
        {
            fOut.Write ("</"sv);
            WriteName_ (name, scope);
            fOut.Write ('>');
            NewLine_ ();
        }
        // the element for a member (or the document's) - binding nullptr for the default shape. outer is what the element it
        // is inside has in scope, and alsoDeclare prefixes in scope that no element has declared yet
        void WriteElement (const NameWithNamespace& name, const VariantValue& v, const Binding* binding, const Scope_& outer,
                           int indentLevel, const Mapping<URI, String>& alsoDeclare = {})
        {
            Scope_               scope   = outer;
            Mapping<URI, String> declare = alsoDeclare;
            if (binding != nullptr) {
                if (binding->fDefaultArrayItems != nullptr) {
                    scope.fDefaultArrayItems = binding->fDefaultArrayItems.get ();
                }
                for (const auto& i : binding->fNamespacePrefixes) {
                    BindPrefix_ (&declare, i.fKey, i.fValue);
                }
            }
            if (v.GetType () == VariantValue::eArray) {
                Sequence<VariantValue> items = v.As<Sequence<VariantValue>> ();
                const Binding* itemBinding = (binding != nullptr and binding->fItems != nullptr) ? binding->fItems.get () : scope.fDefaultArrayItems;
                if (not itemBinding->fName) {
                    // items with no name of their own are this element, repeated - how a schema of someone else's usually has
                    // it (UPnP's <service>); then an array of one reads back as a scalar, unless a Binding says it is an array
                    if (indentLevel == 0 and items.size () != 1) {
                        static const auto kException_ =
                            BadFormatException{"a document has exactly one root element, so cannot be an array of items with no name of their own"sv};
                        Execution::Throw (kException_);
                    }
                    for (const VariantValue& i : items) {
                        WriteElement (name, i, itemBinding, scope, indentLevel, declare);
                    }
                    return;
                }
                // items with a name of their own are wrapped in this element: one array of a document can then be wrapped
                // differently from another, as a schema of someone else's wants (UPnP's <actionList><action>, beside its
                // <allowedValueList><allowedValue>)
                StartTag_ (name, declare, &scope, indentLevel);
                fOut.Write ('>');
                if (not items.empty ()) {
                    NewLine_ ();
                    for (const VariantValue& i : items) {
                        WriteElement (*itemBinding->fName, i, itemBinding, scope, indentLevel + 1);
                    }
                    Indent_ (indentLevel);
                }
                EndTag_ (name, scope);
                return;
            }
            StartTag_ (name, declare, &scope, indentLevel);
            Mapping<String, VariantValue> members;
            optional<VariantValue>        textMember; // a member the Binding says is this element's text, not a child element
            if (v.GetType () == VariantValue::eMap) {
                members = v.As<Mapping<String, VariantValue>> ();
                if (binding != nullptr) {
                    for (const Binding::Member& i : binding->fMembers) {
                        optional<VariantValue> mv = members.Lookup (i.fKey);
                        if (not mv) {
                            continue;
                        }
                        switch (binding->KindFor (i.fKey)) {
                            case Binding::Kind::eAttribute: {
                                NameWithNamespace attrName = binding->NameFor (i.fKey);
                                // an attribute in a namespace is writable only with a prefix: the Binding and its prefixes are
                                // both the caller's, so this is its contract, not anything the value could do
                                Require (not attrName.fNamespace or scope.fPrefixes.ContainsKey (*attrName.fNamespace));
                                fOut.Write (' ');
                                WriteName_ (attrName, scope);
                                fOut.Write ("=\""sv);
                                fOut.Write (String{QuoteForXMLAttribute (mv->As<String> ())});
                                fOut.Write ('"');
                            } break;
                            case Binding::Kind::eText:
                                textMember = mv;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            fOut.Write ('>');
            if (textMember) {
                fOut.Write (String{QuoteForXML (textMember->As<String> ())});
            }
            else {
                WriteContent_ (v, members, binding, scope, indentLevel);
            }
            EndTag_ (name, scope);
        }
        void WriteContent_ (const VariantValue& v, const Mapping<String, VariantValue>& members, const Binding* binding, const Scope_& scope, int indentLevel)
        {
            switch (v.GetType ()) {
                case VariantValue::eNull:
                    break; // an empty element: XML has no null
                case VariantValue::eMap: {
                    Sequence<Common::KeyValuePair<String, VariantValue>> elements = ElementMembers_ (members, binding);
                    if (not elements.empty ()) {
                        NewLine_ ();
                        for (const auto& i : elements) {
                            const Binding*    memberBinding = binding == nullptr ? nullptr : binding->MemberBinding (i.fKey);
                            NameWithNamespace memberName    = binding == nullptr ? NameWithNamespace{i.fKey} : binding->NameFor (i.fKey);
                            WriteElement (memberName, i.fValue, memberBinding, scope, indentLevel + 1);
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
    Rep_ (const Options& options)
        : fOptions_{options}
    {
    }
    virtual _SharedPtrIRep Clone () const override
    {
        return MakeSharedPtr<Rep_> (fOptions_);
    }
    virtual optional<filesystem::path> GetDefaultFileSuffix () const override
    {
        return ".xml"sv;
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream::Ptr<byte>& out) const override
    {
        // the byte overload writes a document, so it declares the encoding it is writing in
        OutputStream::Ptr<Character> useOut = TextToBinary::Writer::New (out, UnicodeExternalEncodings::eUTF8, ByteOrderMark::eDontInclude);
        useOut.Write ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"sv);
        if (fOptions_.fPrettyPrint) {
            useOut.Write ("\n"sv);
        }
        Write_ (v, useOut);
    }
    virtual void Write (const VariantValue& v, const Streams::OutputStream::Ptr<Character>& out) const override
    {
        // no declaration here: the caller owns the encoding, and may be writing this into a document of its own
        Write_ (v, out);
    }
    nonvirtual void Write_ (const VariantValue& v, const Streams::OutputStream::Ptr<Character>& out) const
    {
        StringBuilder indent;
        for (unsigned int i = 0; i < fOptions_.fSpacesPerIndent; ++i) {
            indent << " "sv;
        }
        Writer_         w{out, fOptions_.fPrettyPrint, indent.str ()};
        Writer_::Scope_ document;
        const Binding&  binding = fOptions_.fBinding;
        if (binding.fName) {
            // the Binding names the document element, so it IS that element's, and the value is its content
            w.WriteElement (*binding.fName, v, &binding, document, 0);
        }
        else {
            // no name: the value's one member is the document element - so the Binding is the document's, its default array items
            // in scope there, and its prefixes declared there. A document has exactly one root element; a value with no single
            // member to be it throws, as any value that cannot be written does - it is data, not code
            Mapping<String, VariantValue> members;
            if (v.GetType () == VariantValue::eMap) {
                members = v.As<Mapping<String, VariantValue>> ();
            }
            if (members.size () != 1) {
                static const auto kException_ =
                    BadFormatException{"a document has exactly one root element: give the Binding an fName, or write an object of exactly one member"sv};
                Execution::Throw (kException_);
            }
            if (binding.fDefaultArrayItems != nullptr) {
                document.fDefaultArrayItems = binding.fDefaultArrayItems.get ();
            }
            for (const auto& i : members) {
                w.WriteElement (binding.NameFor (i.fKey), i.fValue, binding.MemberBinding (i.fKey), document, 0, binding.fNamespacePrefixes);
            }
        }
    }
    nonvirtual Options GetOptions () const
    {
        return fOptions_;
    }
    nonvirtual void SetOptions (const Options& options)
    {
        fOptions_ = options;
    }

private:
    Options fOptions_;
};

Variant::XML::Writer::Writer ()
    : Writer{Options{}}
{
}

Variant::XML::Writer::Writer (const Options& options)
    : inherited{MakeSharedPtr<Rep_> (options)}
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

auto Variant::XML::Writer::GetOptions () const -> Options
{
    return GetRep_ ().GetOptions ();
}

void Variant::XML::Writer::SetOptions (const Options& options)
{
    GetRep_ ().SetOptions (options);
}
