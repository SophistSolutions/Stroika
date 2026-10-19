/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <charconv>
#include <cmath>

#include "Stroika/Foundation/Characters/FloatConversion.h"
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
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/TextToBinary.h"
#include "Stroika/Foundation/Time/Date.h"
#include "Stroika/Foundation/Time/DateTime.h"

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
    // whether the Binding names a member written as an element (or attribute) of that name - which the element then is, whatever
    // else its name could make it (an item of an array). A presence marker is an element
    bool NamesMember_ (const Binding* binding, Binding::Kind kind, const String& name)
    {
        auto isElement = [] (Binding::Kind k) { return k == Binding::Kind::eElement or k == Binding::Kind::ePresenceMarker; };
        return binding != nullptr and binding->fMembers.Any ([&] (const Binding::Member& m) {
            Binding::Kind k = binding->KindFor (m.fKey);
            return (isElement (kind) ? isElement (k) : k == kind) and binding->NameFor (m.fKey).fName == name;
        });
    }

    // what reading does with an element or attribute the Binding of the element it is in does not declare - and an array or a
    // leaf the Binding declares has no members to declare, so none to keep: an array's Binding can say to skip what is not its
    // item, and otherwise it is refused
    Binding::UndeclaredMembers UndeclaredIn_ (const Binding* binding)
    {
        if (binding == nullptr) {
            return Binding::UndeclaredMembers::eKeep;
        }
        if (binding->fType and binding->fType != VariantValue::eMap) {
            if (binding->fType == VariantValue::eArray and binding->fUndeclaredMembers == Binding::UndeclaredMembers::eSkip) {
                return Binding::UndeclaredMembers::eSkip;
            }
            return Binding::UndeclaredMembers::eReject;
        }
        return binding->GetUndeclaredMembers ();
    }

    template <typename INT>
    optional<INT> ParseInteger_ (const String& text)
    {
        if (optional<string> s = text.AsASCIIQuietly ()) {
            INT r{};
            auto [ptr, ec] = from_chars (s->data (), s->data () + s->size (), r);
            if (ec == errc{} and ptr == s->data () + s->size ()) {
                return r;
            }
        }
        return nullopt;
    }

    // text read back as the leaf type a Binding declares - throwing where it is not one, the document then not being what the
    // Binding describes. Whitespace around a number, date or BLOB is dropped, as XML Schema collapses it
    VariantValue AsDeclaredType_ (const String& text, VariantValue::Type type)
    {
        String trimmed = text.Trim ();
        switch (type) {
            case VariantValue::eString:
                return VariantValue{text};
            case VariantValue::eBoolean:
                // as XML Schema's xs:boolean
                if (trimmed == "true"sv or trimmed == "1"sv) {
                    return VariantValue{true};
                }
                if (trimmed == "false"sv or trimmed == "0"sv) {
                    return VariantValue{false};
                }
                break;
            case VariantValue::eInteger:
                if (optional<long long> i = ParseInteger_<long long> (trimmed)) {
                    return VariantValue{*i};
                }
                break;
            case VariantValue::eUnsignedInteger:
                if (optional<unsigned long long> i = ParseInteger_<unsigned long long> (trimmed)) {
                    return VariantValue{*i};
                }
                break;
            case VariantValue::eFloat: {
                long double d = FloatConversion::ToFloat<long double> (trimmed); // nan where it is not a number at all
                if (not isnan (d) or trimmed.ToLowerCase ().Find ("nan"sv)) {
                    return VariantValue{d};
                }
            } break;
            case VariantValue::eDate:
                return VariantValue{Time::Date::Parse (trimmed, Time::Date::kISO8601Format)};
            case VariantValue::eDateTime:
                return VariantValue{Time::DateTime::Parse (trimmed, Time::DateTime::kISO8601Format)};
            case VariantValue::eBLOB:
                return VariantValue{VariantValue{trimmed}.As<Memory::BLOB> ()}; // base64, as the Writer writes it
            default:
                break; // not a leaf
        }
        Execution::Throw (BadFormatException{"'{}' is not {}"_f(text, type)});
    }

    /*
     *  The inverse of Variant::XML::Writer (its documentation has the shape): an element with child elements is an object, one
     *  with only text is that text (a String - XML says nothing of types, so the reader types nothing), and children of one
     *  name are an array. An attribute reads as a member too, by its name, so that reading a document of someone else's does
     *  not silently drop it.
     */
    struct Builder_ : StructuredStreamEvents::IConsumer {
        using Name = StructuredStreamEvents::Name;

        Builder_ (const Binding& binding, bool lenient)
            : fBinding{binding}
            , fLenient{lenient}
        {
        }
        const Binding& fBinding; // the top level Binding - with an fName, the document element's own (@see Binding)
        const bool     fLenient; // Reader::Options::fLenient: what the Binding cannot hold is passed over, not thrown

        // text as the type a Binding declares - or, read leniently, nullopt where it is not one: absent, rather than thrown
        optional<VariantValue> Declared_ (const String& text, VariantValue::Type type) const
        {
            if (fLenient) {
                try {
                    return AsDeclaredType_ (text, type);
                }
                catch (...) {
                    return nullopt;
                }
            }
            return AsDeclaredType_ (text, type);
        }

        struct Frame_ {
            String         fElement;                                         // its local name
            String         fKey;                                             // the member it is: its own name, unless a Binding renamed it
            const Binding* fBinding{nullptr};                                // how its content differs from the default shape
            const Binding* fDefaultArrayItems{&Binding::kDefaultArrayItems}; // the item Binding, here, of an array with no fItems
            bool           fIsItem{false};           // an item of the array the element it is in wraps - so not a member of it
            bool           fIsRepeatedItem{false};   // one of a member's elements, repeated: a Binding says the member is an array
            bool           fIsBoundArray{false};     // a Binding says it wraps an array - so it is one, however few items it has
            bool           fHasChildElements{false}; // any child element - an array item or a member - not merely attributes
            bool fIsPresenceMarker{false}; // a presence marker (Binding::Kind::ePresenceMarker): an empty element, true by being there
            optional<String>              fItemName;             // what the items of an array it wraps are called, where it may wrap one
            const Binding*                fItemBinding{nullptr}; // ... and their Binding
            StringBuilder<>               fText;
            Mapping<String, VariantValue> fChildren;
            Set<String>                   fRepeated;   // a name seen more than once, so fChildren holds its array
            Sequence<VariantValue>        fArrayItems; // its children named fItemName
        };
        vector<Frame_>         fStack;
        String                 fRootKey;
        optional<VariantValue> fRootValue;
        unsigned int           fSkipDepth_{0}; // inside an element being skipped (Binding::UndeclaredMembers::eSkip): how deep

        virtual void StartElement (const Name& name, const Mapping<Name, String>& attributes) override
        {
            if (fSkipDepth_ > 0) {
                ++fSkipDepth_; // inside an element being skipped
                return;
            }
            // what an element is, its own Binding says - and what it is inside says which Binding that is
            Frame_         f;
            const Binding* ownBinding = nullptr;
            f.fElement                = name.fLocalName;
            if (fStack.empty ()) {
                if (fBinding.fName) {
                    // it names the document element, so it is that element's - and a document element of another name is not what
                    // it describes. By local name, as every member is matched
                    if (name.fLocalName != fBinding.fName->fName) {
                        Execution::Throw (BadFormatException{
                            "the document element is <{}>, where the Binding names <{}>"_f(name.fLocalName, fBinding.fName->fName)});
                    }
                    ownBinding = &fBinding;
                }
                else {
                    // the document element is the value's one member, so its key and Binding are that member's
                    f.fKey     = fBinding.KeyForName (Binding::Kind::eElement, name.fLocalName);
                    ownBinding = fBinding.MemberBinding (f.fKey);
                    if (fBinding.fDefaultArrayItems != nullptr) {
                        f.fDefaultArrayItems = fBinding.fDefaultArrayItems.get ();
                    }
                }
            }
            else {
                const Frame_& parent = fStack.back ();
                f.fDefaultArrayItems = parent.fDefaultArrayItems;
                if (parent.fItemName and name.fLocalName == *parent.fItemName and
                    not NamesMember_ (parent.fBinding, Binding::Kind::eElement, name.fLocalName)) {
                    f.fIsItem  = true;
                    ownBinding = parent.fItemBinding;
                }
                else {
                    // one its parent's Binding does not declare (an array's items are, above): as that Binding says
                    if (not NamesMember_ (parent.fBinding, Binding::Kind::eElement, name.fLocalName)) {
                        switch (UndeclaredIn_ (parent.fBinding)) {
                            case Binding::UndeclaredMembers::eReject:
                                if (not fLenient) {
                                    Execution::Throw (BadFormatException{"<{}> is not an element its parent's Binding declares"_f(name.fLocalName)});
                                }
                                [[fallthrough]]; // read leniently, skipped - even inside a leaf, as markup in its text
                            case Binding::UndeclaredMembers::eSkip:
                                fSkipDepth_ = 1; // it, and everything in it, unread
                                return;
                            case Binding::UndeclaredMembers::eKeep:
                                break;
                        }
                    }
                    f.fKey = parent.fBinding == nullptr ? name.fLocalName : parent.fBinding->KeyForName (Binding::Kind::eElement, name.fLocalName);
                    ownBinding = parent.fBinding == nullptr ? nullptr : parent.fBinding->MemberBinding (f.fKey);
                }
            }
            if (ownBinding != nullptr and ownBinding->fDefaultArrayItems != nullptr) {
                f.fDefaultArrayItems = ownBinding->fDefaultArrayItems.get ();
            }
            // its items' Binding, where its own says it is an array: fItems - else, where it declares an array, the default items
            // in scope, as the Writer takes them
            const Binding* items = nullptr;
            if (ownBinding != nullptr) {
                items = ownBinding->fItems != nullptr               ? ownBinding->fItems.get ()
                        : ownBinding->fType == VariantValue::eArray ? f.fDefaultArrayItems
                                                                    : nullptr;
            }
            f.fBinding          = ownBinding;
            f.fIsPresenceMarker = ownBinding != nullptr and ownBinding->fKind == Binding::Kind::ePresenceMarker;
            if (items != nullptr and not items->fName) {
                // items with no name of their own are the member's element, repeated: this is one of them, so the item
                // Binding is what its content has
                f.fIsRepeatedItem = true;
                f.fBinding        = items;
            }
            if (f.fBinding != ownBinding and f.fBinding->fDefaultArrayItems != nullptr) {
                f.fDefaultArrayItems = f.fBinding->fDefaultArrayItems.get ();
            }
            // what the items of an array it wraps are called: as its Binding says, else as the default array items in scope do -
            // and none, where its Binding declares an object or a leaf
            bool declaresNoArray = f.fBinding != nullptr and f.fBinding->fType and f.fBinding->fType != VariantValue::eArray;
            if (f.fBinding != nullptr and f.fBinding->fItems != nullptr) {
                if (f.fBinding->fItems->fName) {
                    f.fIsBoundArray = true;
                    f.fItemName     = f.fBinding->fItems->fName->fName;
                    f.fItemBinding  = f.fBinding->fItems.get ();
                }
            }
            else if (f.fDefaultArrayItems->fName and not declaresNoArray) {
                f.fItemName    = f.fDefaultArrayItems->fName->fName;
                f.fItemBinding = f.fDefaultArrayItems;
            }
            fStack.push_back (move (f));
            Frame_& top = fStack.back ();
            for (const auto& i : attributes) {
                if (not NamesMember_ (top.fBinding, Binding::Kind::eAttribute, i.fKey.fLocalName)) {
                    switch (UndeclaredIn_ (top.fBinding)) {
                        case Binding::UndeclaredMembers::eReject:
                            if (not fLenient) {
                                Execution::Throw (BadFormatException{
                                    "<{}>'s attribute '{}' is not one its Binding declares"_f(name.fLocalName, i.fKey.fLocalName)});
                            }
                            continue; // read leniently, skipped
                        case Binding::UndeclaredMembers::eSkip:
                            continue;
                        case Binding::UndeclaredMembers::eKeep:
                            break;
                    }
                }
                String key = top.fBinding == nullptr ? i.fKey.fLocalName : top.fBinding->KeyForName (Binding::Kind::eAttribute, i.fKey.fLocalName);
                const Binding* attrBinding = top.fBinding == nullptr ? nullptr : top.fBinding->MemberBinding (key);
                if (attrBinding == nullptr or not attrBinding->fType) {
                    AddMember_ (top, key, VariantValue{i.fValue});
                }
                else if (optional<VariantValue> v = Declared_ (i.fValue, *attrBinding->fType)) {
                    AddMember_ (top, key, *v);
                }
            }
        }
        virtual void TextInsideElement (const String& text) override
        {
            if (not fStack.empty () and fSkipDepth_ == 0) {
                fStack.back ().fText << text;
            }
        }
        virtual void EndElement ([[maybe_unused]] const Name& name) override
        {
            if (fSkipDepth_ > 0) {
                --fSkipDepth_;
                return;
            }
            Require (not fStack.empty ());
            Frame_ f = move (fStack.back ());
            fStack.pop_back ();
            optional<VariantValue> v = ValueOf_ (f);
            if (fStack.empty ()) {
                fRootKey          = f.fKey;
                VariantValue root = v.value_or (VariantValue{});
                fRootValue = f.fIsRepeatedItem ? VariantValue{Sequence<VariantValue>{root}} : root; // a document has one root element
                return;
            }
            Frame_& parent           = fStack.back ();
            parent.fHasChildElements = true;
            if (not v) {
                return; // read leniently, a value not of its declared type: absent
            }
            if (f.fIsItem) {
                parent.fArrayItems.Append (*v);
            }
            else if (f.fIsRepeatedItem) {
                // a Binding says the member is an array, so it is one however few elements the document has
                Sequence<VariantValue> items;
                if (optional<VariantValue> already = parent.fChildren.Lookup (f.fKey)) {
                    items = already->As<Sequence<VariantValue>> ();
                }
                items.Append (*v);
                parent.fChildren.Add (f.fKey, VariantValue{items});
            }
            else {
                AddMember_ (parent, f.fKey, *v);
            }
        }
        // a member - and a name seen again makes it the array of them, in document order; but not where the element's Binding
        // declares its type, which says what each member is (one its own Binding says is an array, its element repeated, is not
        // added here: EndElement)
        void AddMember_ (Frame_& f, const String& key, const VariantValue& v)
        {
            if (optional<VariantValue> already = f.fChildren.Lookup (key)) {
                if (f.fBinding != nullptr and f.fBinding->fType) {
                    if (fLenient) {
                        f.fChildren.Add (key, v); // read leniently, the last
                        return;
                    }
                    Execution::Throw (BadFormatException{"<{}> has '{}' more than once, where its Binding declares one"_f(f.fElement, key)});
                }
                Sequence<VariantValue> items = f.fRepeated.Contains (key) ? already->As<Sequence<VariantValue>> () : Sequence<VariantValue>{*already};
                items.Append (v);
                f.fChildren.Add (key, VariantValue{items});
                f.fRepeated.Add (key);
            }
            else {
                f.fChildren.Add (key, v);
            }
        }
        // its value - nullopt only read leniently, where it is not of its declared type, so is absent
        optional<VariantValue> ValueOf_ (Frame_& f)
        {
            String                       text      = f.fText.str ();
            bool                         textBlank = text.Trim ().empty ();
            optional<VariantValue::Type> declared  = f.fBinding == nullptr ? nullopt : f.fBinding->fType;
            if (f.fIsPresenceMarker) {
                // there, so true: it says so by being there, and says nothing else - read leniently, whatever is in it passed over
                if (not fLenient and (not f.fChildren.empty () or f.fHasChildElements or not textBlank)) {
                    Execution::Throw (BadFormatException{"<{}> is a presence marker (Binding::Kind::ePresenceMarker), so an empty element"_f(f.fElement)});
                }
                return VariantValue{true};
            }
            if (declared and *declared != VariantValue::eMap) {
                // its Binding says what it is - and StartElement has let in no child element or attribute it does not declare
                Assert (*declared == VariantValue::eArray or f.fChildren.empty ());
                if (*declared == VariantValue::eArray) {
                    if (not fLenient and (not f.fChildren.empty () or not textBlank)) {
                        Execution::Throw (BadFormatException{"<{}> is not an array, as its Binding declares"_f(f.fElement)});
                    }
                    return VariantValue{f.fArrayItems}; // [] where it is empty - read leniently, text beside its items passed over
                }
                return Declared_ (text, *declared);
            }
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
            // a presence marker its Binding declares, absent, is false - as a false one is written: as nothing
            if (f.fBinding != nullptr) {
                for (const Binding::Member& m : f.fBinding->fMembers) {
                    if (f.fBinding->KindFor (m.fKey) == Binding::Kind::ePresenceMarker and not f.fChildren.ContainsKey (m.fKey)) {
                        f.fChildren.Add (m.fKey, VariantValue{false});
                    }
                }
            }
            static const auto kMixedContent_ = BadFormatException{"mixed content (text beside child elements) cannot be read into a VariantValue"sv};
            // a Binding may say this element's text is one of its members, beside its attributes - an empty one, where that member's
            // Binding declares its type, written as none, so absent
            if (optional<String> textKey = f.fBinding == nullptr ? nullopt : f.fBinding->TextMemberKey ()) {
                if (f.fHasChildElements) {
                    // ... but not beside child elements, which is mixed content - the whitespace of a pretty-printed document aside,
                    // which is no text at all - read leniently, passed over
                    if (not textBlank and not fLenient) {
                        Execution::Throw (kMixedContent_);
                    }
                    return VariantValue{f.fChildren};
                }
                const Binding* textBinding = f.fBinding->MemberBinding (*textKey);
                if (textBinding == nullptr or not textBinding->fType) {
                    f.fChildren.Add (*textKey, VariantValue{text});
                }
                else if (not text.empty ()) {
                    if (optional<VariantValue> v = Declared_ (text, *textBinding->fType)) {
                        f.fChildren.Add (*textKey, *v);
                    }
                }
                return VariantValue{f.fChildren};
            }
            if (f.fChildren.empty () and declared != VariantValue::eMap) {
                return VariantValue{text}; // text, kept as it came: XML whitespace is significant
            }
            if (fLenient) {
                return VariantValue{f.fChildren}; // read leniently, text it has nowhere for passed over
            }
            if (f.fChildren.empty () and not textBlank) {
                Execution::Throw (BadFormatException{"<{}> has text, where its Binding declares an object"_f(f.fElement)});
            }
            if (not textBlank) {
                // text this shape cannot hold, which reading on would silently drop - so say so, rather than lose it (it comes
                // from the document, not from any caller's mistake). Beside attributes alone, a Binding could have held it
                if (f.fHasChildElements) {
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
    Rep_ (const Options& options)
        : fOptions_{options}
    {
    }

public:
    virtual optional<filesystem::path> GetDefaultFileSuffix () const override
    {
        return ".xml"sv;
    }
    virtual VariantValue Read ([[maybe_unused]] const Streams::InputStream::Ptr<byte>& in) const override
    {
#if qStroika_Foundation_DataExchange_XML_SupportParsing
        Builder_ builder{fOptions_.fBinding, fOptions_.fLenient};
        SAXParse (in, &builder, fOptions_.fValidateAgainst, nullptr, fOptions_.fXMLOptions);
        if (not builder.fRootValue) {
            static const auto kException_ = BadFormatException{"no root element"sv};
            Execution::Throw (kException_);
        }
        // as the Writer has it: with the document element named, its content is the value; else the value is the object it is
        // the one member of
        return fOptions_.fBinding.fName ? *builder.fRootValue : VariantValue{Mapping<String, VariantValue> {
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

private:
    Options fOptions_;
};

Variant::XML::Reader::Reader ()
    : Reader{Options{}}
{
}

Variant::XML::Reader::Reader (const Options& options)
    : inherited{MakeSharedPtr<Rep_> (options)}
{
}

Variant::XML::Reader::Reader (const Binding& binding)
    : Reader{Options{.fBinding = binding}}
{
}
