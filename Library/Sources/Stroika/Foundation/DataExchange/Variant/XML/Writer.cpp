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
    // a name comes from data - a member's key - as often as from a Binding, and one that is not an XML name would be written
    // as malformed XML, which nothing could read back
    void ThrowUnlessNCName_ (const String& name)
    {
        if (not IsNCName (name)) {
            Execution::Throw (BadFormatException{"'{}' is not an XML name, so cannot be written as one"_f(name)});
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
        // a member whose Binding declares its type is written as no member at all where it is null (as ObjectVariantMapper omits
        // a null field) - so it reads back absent
        static bool OmitNull_ (const VariantValue& v, const Binding* memberBinding)
        {
            return v.GetType () == VariantValue::eNull and memberBinding != nullptr and memberBinding->fType;
        }
        // where a Binding declares a type, the value has that type - v is data, not code, so this throws
        static void ThrowUnlessDeclaredType_ (const String& name, const VariantValue& v, const Binding* binding)
        {
            if (binding != nullptr and binding->fType and v.GetType () != *binding->fType) {
                Execution::Throw (BadFormatException{"'{}' is {}, where its Binding declares {}"_f(name, v.GetType (), *binding->fType)});
            }
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
                    if (Binding::Kind k = binding->KindFor (i.fKey); k == Binding::Kind::eElement or k == Binding::Kind::ePresenceMarker) {
                        // a presence marker is written only where true (as an empty element): false, there is nothing to write
                        if (optional<VariantValue> v = members.Lookup (i.fKey); v and not OmitNull_ (*v, binding->MemberBinding (i.fKey)) and
                                                                                (k != Binding::Kind::ePresenceMarker or v->As<bool> ())) {
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
        // the element for a member (or the document element) - binding nullptr for the default shape. outer is what the element it
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
                ThrowUnlessDeclaredType_ (name.fName, v, binding);
                if (v.GetType () == VariantValue::eMap and binding->GetUndeclaredMembers () != Binding::UndeclaredMembers::eKeep) {
                    // the members it declares are all the object has - one it does not would not read back
                    for (const auto& i : v.As<Mapping<String, VariantValue>> ()) {
                        if (not binding->fMembers.Any ([&] (const Binding::Member& m) { return m.fKey == i.fKey; })) {
                            Execution::Throw (BadFormatException{"'{}' has a member '{}', which its Binding does not declare"_f(name.fName, i.fKey)});
                        }
                    }
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
                        if (not mv or OmitNull_ (*mv, i.fBinding.get ())) {
                            continue;
                        }
                        if (binding->KindFor (i.fKey) != Binding::Kind::eElement) {
                            ThrowUnlessDeclaredType_ (i.fKey, *mv, i.fBinding.get ()); // an element member's, WriteElement checks
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
            // a text member is this element's text beside its attributes - an empty one written as nothing - not beside child
            // elements: that is mixed content, which the Reader cannot read back, so v throws rather than lose them
            String text = textMember ? textMember->As<String> () : String{};
            if (text.empty ()) {
                WriteContent_ (v, members, binding, scope, indentLevel);
            }
            else {
                if (not ElementMembers_ (members, binding).empty ()) {
                    Execution::Throw (BadFormatException{
                        "'{}' has text (Binding::Kind::eText) beside child elements - mixed content, which cannot be read back"_f(name.fName)});
                }
                fOut.Write (String{QuoteForXML (text)});
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
                            if (memberBinding != nullptr and memberBinding->fKind == Binding::Kind::ePresenceMarker) {
                                // a presence marker ElementMembers_ gave is true: an empty element says so by being there
                                Scope_ markerScope = scope;
                                StartTag_ (memberName, {}, &markerScope, indentLevel + 1);
                                fOut.Write ('>');
                                EndTag_ (memberName, markerScope);
                                continue;
                            }
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
            // no name: the value's one member is the document element, and the top level Binding describes the value - so its default
            // array items are in scope there, and its prefixes declared there. A document has exactly one root element; a value
            // with no single member to be it throws, as any value that cannot be written does - it is data, not code
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
