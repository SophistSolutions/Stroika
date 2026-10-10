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
     *  not silently drop it - which the writer, having no attributes yet, writes back as an element.
     */
    struct Builder_ : StructuredStreamEvents::IConsumer {
        using Name = StructuredStreamEvents::Name;

        Builder_ (const String& arrayItemElementName)
            : fArrayItemElementName{arrayItemElementName}
        {
        }
        String fArrayItemElementName; // children of this name are the element's array; empty: any one name repeated is

        struct Frame_ {
            String                        fName;
            StringBuilder<>               fText;
            Mapping<String, VariantValue> fChildren;
            Set<String>                   fRepeated;   // a name seen more than once, so fChildren holds its array
            Sequence<VariantValue>        fArrayItems; // children named fArrayItemElementName
        };
        vector<Frame_> fStack;
        String         fRootName;
        VariantValue   fRootValue;

        virtual void StartElement (const Name& name, const Mapping<Name, String>& attributes) override
        {
            fStack.push_back (Frame_{.fName = name.fLocalName});
            for (const auto& i : attributes) {
                AddChild_ (fStack.back (), i.fKey.fLocalName, VariantValue{i.fValue});
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
            Frame_ f = fStack.back ();
            fStack.pop_back ();
            VariantValue v = ValueOf_ (f);
            if (fStack.empty ()) {
                fRootName  = f.fName;
                fRootValue = v;
            }
            else {
                AddChild_ (fStack.back (), f.fName, v);
            }
        }
        void AddChild_ (Frame_& f, const String& name, const VariantValue& v)
        {
            if (not fArrayItemElementName.empty () and name == fArrayItemElementName) {
                f.fArrayItems.Append (v);
                return;
            }
            if (optional<VariantValue> already = f.fChildren.Lookup (name)) {
                // a name seen again: the member is the array of them, in document order
                Sequence<VariantValue> items = f.fRepeated.Contains (name) ? already->As<Sequence<VariantValue>> () : Sequence<VariantValue>{*already};
                items.Append (v);
                f.fChildren.Add (name, VariantValue{items});
                f.fRepeated.Add (name);
            }
            else {
                f.fChildren.Add (name, v);
            }
        }
        VariantValue ValueOf_ (Frame_& f)
        {
            if (not f.fArrayItems.empty () and f.fChildren.empty ()) {
                return VariantValue{f.fArrayItems}; // all its children were array items, so the element IS the array
            }
            if (not f.fArrayItems.empty ()) {
                AddChild_ (f, fArrayItemElementName, VariantValue{f.fArrayItems}); // ... beside other members: just a member
                f.fArrayItems.clear ();
            }
            if (f.fChildren.empty ()) {
                return VariantValue{f.fText.str ()}; // text, kept as it came: XML whitespace is significant
            }
            WeakAssert (f.fText.str ().Trim ().empty ()); // mixed content, which this shape cannot hold - the text is dropped
            return VariantValue{f.fChildren};
        }
    };
}
#endif

class Variant::XML::Reader::Rep_ final : public Variant::Reader::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
public:
    Rep_ (const SerializationConfiguration& config)
        : fSerializationConfiguration_{config}
    {
    }

public:
    virtual _SharedPtrIRep Clone () const override
    {
        return MakeSharedPtr<Rep_> (fSerializationConfiguration_);
    }
    virtual optional<filesystem::path> GetDefaultFileSuffix () const override
    {
        return ".xml"sv;
    }
    virtual VariantValue Read ([[maybe_unused]] const Streams::InputStream::Ptr<byte>& in) const override
    {
#if qStroika_Foundation_DataExchange_XML_SupportParsing
        Builder_ builder{fSerializationConfiguration_.GetArrayElementName ().value_or (String{})};
        SAXParse (in, &builder, nullptr);
        if (builder.fRootName.empty ()) {
            static const auto kException_ = BadFormatException{"no root element"sv};
            Execution::Throw (kException_);
        }
        // as the Writer does it: with a document element named, that element is the value; else the value is the object it is in
        return fSerializationConfiguration_.GetDocumentElementName ().has_value () ? builder.fRootValue : VariantValue{Mapping<String, VariantValue> {
            { builder.fRootName,
              builder.fRootValue }
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
    nonvirtual SerializationConfiguration GetConfiguration () const
    {
        return fSerializationConfiguration_;
    }
    nonvirtual void SetConfiguration (const SerializationConfiguration& config)
    {
        fSerializationConfiguration_ = config;
    }

private:
    SerializationConfiguration fSerializationConfiguration_;
};

Variant::XML::Reader::Reader (const SerializationConfiguration& config)
    : inherited{MakeSharedPtr<Rep_> (config)}
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

SerializationConfiguration Variant::XML::Reader::GetConfiguration () const
{
    return GetRep_ ().GetConfiguration ();
}

void Variant::XML::Reader::SetConfiguration (const SerializationConfiguration& config)
{
    GetRep_ ().SetConfiguration (config);
}
