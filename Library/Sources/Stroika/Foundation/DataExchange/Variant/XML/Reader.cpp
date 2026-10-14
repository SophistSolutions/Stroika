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
#include "Stroika/Foundation/DataExchange/StructuredStreamEvents/IConsumer.h"
#include "Stroika/Foundation/DataExchange/XML/SAXReader.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/TextToBinary.h"

#include "Reader.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;
using namespace Stroika::Foundation::DataExchange::Variant::XML;

using Characters::Character;
using Memory::MakeSharedPtr;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#if qStroika_Foundation_DataExchange_XML_SupportParsing
namespace {
    /*
     *  The inverse of Variant::XML::Writer (its documentation has the shape): an element with child elements is an object, one
     *  with only text is that text (a String - XML says nothing of types, so the reader types nothing), and children of one
     *  name are an array. An attribute reads as a member too, by its name, so that reading a document of someone else's does
     *  not silently drop it.
     */
    struct Builder_ : StructuredStreamEvents::IConsumer {
        using Name = StructuredStreamEvents::Name;

        Builder_ (const Binding& binding)
            : fBinding{binding}
        {
        }
        const Binding& fBinding; // the document's - with an fName, the document element's own (@see Binding)

        struct Frame_ {
            String           fKey;              // the member it is: its own name, unless a Binding renamed it
            const Binding*   fBinding{nullptr}; // how its content differs from the default shape
            const Binding*   fDefaultArrayItems{&Binding::kDefaultArrayItems}; // the item Binding, here, of an array with no fItems
            bool             fIsItem{false};           // an item of the array the element it is in wraps - so not a member of it
            bool             fIsRepeatedItem{false};   // one of a member's elements, repeated: a Binding says the member is an array
            bool             fIsBoundArray{false};     // a Binding says it wraps an array - so it is one, however few items it has
            bool             fHasChildElements{false}; // any child element - an array item or a member - not merely attributes
            optional<String> fItemName;                // what the items of an array it wraps are called, where it may wrap one
            const Binding*   fItemBinding{nullptr};    // ... and their Binding
            StringBuilder<>  fText;
            Mapping<String, VariantValue> fChildren;
            Set<String>                   fRepeated;   // a name seen more than once, so fChildren holds its array
            Sequence<VariantValue>        fArrayItems; // its children named fItemName
        };
        vector<Frame_>         fStack;
        String                 fRootKey;
        optional<VariantValue> fRootValue;

        virtual void StartElement (const Name& name, const Mapping<Name, String>& attributes) override
        {
            // what an element is, its own Binding says - and what it is inside says which Binding that is
            Frame_         f;
            const Binding* ownBinding = nullptr;
            if (fStack.empty ()) {
                if (fBinding.fName) {
                    // it names the document element, so it is that element's - and a document element of another name is not what
                    // it describes. By local name, as every member is matched
                    if (name.fLocalName != fBinding.fName->fName) {
                        Execution::Throw (BadFormatException{
                            L"the document element is <{}>, where the Binding names <{}>"_f(name.fLocalName, fBinding.fName->fName)});
                    }
                    ownBinding = &fBinding;
                }
                else {
                    // the document element is the value's one member, so its key and Binding are that member's
                    f.fKey     = fBinding.KeyForName (name.fLocalName);
                    ownBinding = fBinding.MemberBinding (f.fKey);
                    if (fBinding.fDefaultArrayItems != nullptr) {
                        f.fDefaultArrayItems = fBinding.fDefaultArrayItems.get ();
                    }
                }
            }
            else {
                const Frame_& parent = fStack.back ();
                f.fDefaultArrayItems = parent.fDefaultArrayItems;
                if (parent.fItemName and name.fLocalName == *parent.fItemName) {
                    f.fIsItem  = true;
                    ownBinding = parent.fItemBinding;
                }
                else {
                    f.fKey     = parent.fBinding == nullptr ? name.fLocalName : parent.fBinding->KeyForName (name.fLocalName);
                    ownBinding = parent.fBinding == nullptr ? nullptr : parent.fBinding->MemberBinding (f.fKey);
                }
            }
            f.fBinding = ownBinding;
            if (ownBinding != nullptr and ownBinding->fItems != nullptr and not ownBinding->fItems->fName) {
                // items with no name of their own are the member's element, repeated: this is one of them, so the item
                // Binding is what its content has
                f.fIsRepeatedItem = true;
                f.fBinding        = ownBinding->fItems.get ();
            }
            if (ownBinding != nullptr and ownBinding->fDefaultArrayItems != nullptr) {
                f.fDefaultArrayItems = ownBinding->fDefaultArrayItems.get ();
            }
            if (f.fBinding != ownBinding and f.fBinding->fDefaultArrayItems != nullptr) {
                f.fDefaultArrayItems = f.fBinding->fDefaultArrayItems.get ();
            }
            // what the items of an array it wraps are called: as its Binding says, else as the default array items in scope do
            if (f.fBinding != nullptr and f.fBinding->fItems != nullptr) {
                if (f.fBinding->fItems->fName) {
                    f.fIsBoundArray = true;
                    f.fItemName     = f.fBinding->fItems->fName->fName;
                    f.fItemBinding  = f.fBinding->fItems.get ();
                }
            }
            else if (f.fDefaultArrayItems->fName) {
                f.fItemName    = f.fDefaultArrayItems->fName->fName;
                f.fItemBinding = f.fDefaultArrayItems;
            }
            fStack.push_back (move (f));
            Frame_& top = fStack.back ();
            for (const auto& i : attributes) {
                AddMember_ (top, top.fBinding == nullptr ? i.fKey.fLocalName : top.fBinding->KeyForName (i.fKey.fLocalName), VariantValue{i.fValue});
            }
        }
        virtual void TextInsideElement (const String& text) override
        {
            if (not fStack.empty ()) {
                fStack.back ().fText << text;
            }
        }
        virtual void EndElement ([[maybe_unused]] const Name& name) override
        {
            Require (not fStack.empty ());
            Frame_ f = move (fStack.back ());
            fStack.pop_back ();
            VariantValue v = ValueOf_ (f);
            if (fStack.empty ()) {
                fRootKey   = f.fKey;
                fRootValue = f.fIsRepeatedItem ? VariantValue{Sequence<VariantValue>{v}} : v; // a document has one root element
                return;
            }
            Frame_& parent           = fStack.back ();
            parent.fHasChildElements = true;
            if (f.fIsItem) {
                parent.fArrayItems.Append (v);
            }
            else if (f.fIsRepeatedItem) {
                // a Binding says the member is an array, so it is one however few elements the document has
                Sequence<VariantValue> items;
                if (optional<VariantValue> already = parent.fChildren.Lookup (f.fKey)) {
                    items = already->As<Sequence<VariantValue>> ();
                }
                items.Append (v);
                parent.fChildren.Add (f.fKey, VariantValue{items});
            }
            else {
                AddMember_ (parent, f.fKey, v);
            }
        }
        // a member - and a name seen again makes it the array of them, in document order
        void AddMember_ (Frame_& f, const String& key, const VariantValue& v)
        {
            if (optional<VariantValue> already = f.fChildren.Lookup (key)) {
                Sequence<VariantValue> items = f.fRepeated.Contains (key) ? already->As<Sequence<VariantValue>> () : Sequence<VariantValue>{*already};
                items.Append (v);
                f.fChildren.Add (key, VariantValue{items});
                f.fRepeated.Add (key);
            }
            else {
                f.fChildren.Add (key, v);
            }
        }
        VariantValue ValueOf_ (Frame_& f)
        {
            bool textBlank = f.fText.str ().Trim ().empty ();
            if (f.fIsBoundArray and f.fChildren.empty () and textBlank) {
                return VariantValue{f.fArrayItems}; // a Binding says it is an array, so it is one - of no items, if it is empty
            }
            if (not f.fArrayItems.empty () and f.fChildren.empty () and textBlank) {
                return VariantValue{f.fArrayItems}; // all its children were array items, so the element IS the array
            }
            if (not f.fArrayItems.empty ()) {
                // ... beside other members, or text: just a member
                AddMember_ (f, *f.fItemName, VariantValue{f.fArrayItems});
                f.fArrayItems.clear ();
            }
            // a Binding may say this element's text is one of its members, beside its attributes
            if (optional<String> textKey = f.fBinding == nullptr ? nullopt : f.fBinding->TextMemberKey ()) {
                f.fChildren.Add (*textKey, VariantValue{f.fText.str ()});
                return VariantValue{f.fChildren};
            }
            if (f.fChildren.empty ()) {
                return VariantValue{f.fText.str ()}; // text, kept as it came: XML whitespace is significant
            }
            if (not textBlank) {
                // text this shape cannot hold, which reading on would silently drop - so say so, rather than lose it (it comes
                // from the document, not from any caller's mistake). Beside attributes alone, a Binding could have held it
                if (f.fHasChildElements) {
                    static const auto kMixedContent_ =
                        BadFormatException{"mixed content (text beside child elements) cannot be read into a VariantValue"sv};
                    Execution::Throw (kMixedContent_);
                }
                static const auto kTextBesideAttributes_ = BadFormatException{
                    "text beside attributes cannot be read into a VariantValue with no Binding member for it (Binding::Kind::eText)"sv};
                Execution::Throw (kTextBesideAttributes_);
            }
            return VariantValue{f.fChildren};
        }
    };
}
#endif

class Variant::XML::Reader::Rep_ final : public Variant::Reader::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    Rep_ (const Binding& binding)
        : fBinding_{binding}
    {
    }

public:
    virtual _SharedPtrIRep Clone () const override
    {
        return MakeSharedPtr<Rep_> (fBinding_);
    }
    virtual optional<filesystem::path> GetDefaultFileSuffix () const override
    {
        return ".xml"sv;
    }
    virtual VariantValue Read ([[maybe_unused]] const Streams::InputStream::Ptr<byte>& in) const override
    {
#if qStroika_Foundation_DataExchange_XML_SupportParsing
        Builder_ builder{fBinding_};
        SAXParse (in, &builder, nullptr);
        if (not builder.fRootValue) {
            static const auto kException_ = BadFormatException{"no root element"sv};
            Execution::Throw (kException_);
        }
        // as the Writer has it: with the document element named, its content is the value; else the value is the object it is
        // the one member of
        return fBinding_.fName ? *builder.fRootValue : VariantValue{Mapping<String, VariantValue> {
            { builder.fRootKey,
              *builder.fRootValue }
        }};
#else
        static const Execution::Exception<runtime_error> kException_{
            "Variant::XML::Reader requires an XML parser (qStroika_Foundation_DataExchange_XML_SupportParsing)"sv};
        Execution::Throw (kException_);
#endif
    }
    virtual VariantValue Read (const Streams::InputStream::Ptr<Character>& in) const override
    {
        // SAXParse reads bytes - so write the characters back out as UTF-8 for it
        return Read (Streams::TextToBinary::Reader::New (in));
    }
    nonvirtual Binding GetBinding () const
    {
        return fBinding_;
    }
    nonvirtual void SetBinding (const Binding& binding)
    {
        fBinding_ = binding;
    }

private:
    Binding fBinding_;
};

Variant::XML::Reader::Reader (const Binding& binding)
    : inherited{MakeSharedPtr<Rep_> (binding)}
{
}

Variant::XML::Reader::Rep_& Variant::XML::Reader::GetRep_ ()
{
    EnsureMember (&inherited::_GetRep (), Rep_);
    return reinterpret_cast<Rep_&> (inherited::_GetRep ());
}

const Variant::XML::Reader::Rep_& Variant::XML::Reader::GetRep_ () const
{
    EnsureMember (&inherited::_ConstGetRep (), Rep_);
    return reinterpret_cast<const Rep_&> (inherited::_ConstGetRep ());
}

Binding Variant::XML::Reader::GetBinding () const
{
    return GetRep_ ().GetBinding ();
}

void Variant::XML::Reader::SetBinding (const Binding& binding)
{
    GetRep_ ().SetBinding (binding);
}
