/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_Variant_XML_Binding_h_
#define _Stroika_Foundation_DataExchange_Variant_XML_Binding_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/XML/Namespace.h"
#include "Stroika/Foundation/IO/Network/URI.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::DataExchange::Variant::XML {

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using DataExchange::XML::NameWithNamespace;
    using IO::Network::URI;

    /**
     *  \brief How a VariantValue is to be written as XML, and read back, where the default shape (@see Variant::XML::Writer) is
     *         not what is wanted - an attribute rather than an element, a namespace, a name of its own, an order of its own
     *
     *  A Binding mirrors the shape of the value it goes with: an object's members are fMembers, **in the order XML wants them**
     *  (an element's children are an ordered list, and xs:sequence is XSD's normal idiom, while a VariantValue object has no
     *  order); each item of an array is fItems. Nothing is required - a member fMembers does not name keeps the default shape,
     *  and is written after those it does - so a Binding need say only how its value differs.
     *
     *  The Binding a Reader or Writer is given is the document's. With an fName, that names the document element, and the value
     *  is its content (reading a document element of another name throws BadFormatException). With none, the value is an object
     *  of exactly one member, and that member is the document element - named, and bound, as any other member is (writing any
     *  other value throws BadFormatException).
     *
     *  Anything may produce one: written by hand, as below; generated from an ObjectVariantMapper, which knows a type's field
     *  names and their declaration order (https://github.com/SophistSolutions/Stroika/issues/1217); or taken from a schema of
     *  someone else's, an XSD or a WSDL, where the order is normative rather than incidental.
     *
     *  \par Example Usage
     *      \code
     *          // UPnP's <stateVariable>: sendEvents an attribute, and name before dataType (a VariantValue object has no order)
     *          VariantValue v{Mapping<String, VariantValue>{{"name"sv, VariantValue{"Status"sv}},
     *                                                       {"dataType"sv, VariantValue{"boolean"sv}},
     *                                                       {"sendEvents"sv, VariantValue{"yes"sv}}}};
     *          Binding      b{.fName    = "stateVariable"sv,
     *                         .fMembers = {{"sendEvents"sv, Binding{.fKind = Binding::Kind::eAttribute}}, {"name"sv}, {"dataType"sv}}};
     *          String       xml  = Variant::XML::Writer{{.fBinding = b}}.WriteAsString (v);
     *          VariantValue back = Variant::XML::Reader{b}.Read (xml); // == v
     *
     *          // With no fName, the document element is the value's one member: its name moves from the Binding into the value.
     *          // So the same XML again, from v wrapped in a member of that name - whose Binding is b:
     *          VariantValue w{Mapping<String, VariantValue>{{"stateVariable"sv, v}}};
     *          Binding      d{.fMembers = {{"stateVariable"sv, b}}};
     *          String       xml2  = Variant::XML::Writer{{.fBinding = d}}.WriteAsString (w); // == xml
     *          VariantValue back2 = Variant::XML::Reader{d}.Read (xml2);                     // == w
     *          VariantValue plain = Variant::XML::Reader{}.Read (xml);                       // == w too: read with no Binding at all
     *      \endcode
     *      \code
     *          <stateVariable sendEvents="yes">
     *              <name>Status</name>
     *              <dataType>boolean</dataType>
     *          </stateVariable>
     *      \endcode
     */
    struct Binding {
        /**
         *  \brief One member of an object: its key, and how that member differs from the default shape (nothing, by default -
         *         naming it is enough to give it a place in the order)
         */
        struct Member {
            String              fKey;
            shared_ptr<Binding> fBinding;

            Member (const String& key);                         ///< \brief the member's own key, and the default shape
            Member (const String& key, const Binding& binding); ///< \brief the member's own key, and how it differs

            bool operator== (const Member& rhs) const;

            /**
             *  @see Characters::ToString ();
             */
            nonvirtual String ToString () const;
        };

        /**
         *  Whether the member this Binding goes with is written as a child element (the default), as an attribute of the
         *  element it is a member of, or as that element's text. The document element is always an element.
         */
        enum class Kind {
            eElement,
            eAttribute,
            eText
        };
        Kind fKind{Kind::eElement};

        /**
         *  The element's (or attribute's) name, where that is not the member's own key - and its namespace. On the document's
         *  Binding, the document element's.
         */
        optional<NameWithNamespace> fName;

        /**
         *  This object's members, in the order to write them. A member of the value that is not named here is written after
         *  those that are, in the order the value gives them.
         */
        Sequence<Member> fMembers;

        /**
         *  A Binding for each item of this array - and, reading, what says this member IS an array, however few elements the
         *  document happens to have (@see Variant::XML::Writer on that ambiguity).
         *
         *  Where that Binding has an fName, it names the item element, and the member's own element wraps them - so one
         *  document can wrap two arrays differently, as UPnP's `<actionList><action>` and `<allowedValueList><allowedValue>`:
         *      \code
         *          // {"allowedValueList": ["yes", "no"]}
         *          Binding{.fMembers = {{"allowedValueList"sv,
         *                                Binding{.fItems = make_shared<Binding> (Binding{.fName = "allowedValue"sv})}}}};
         *      \endcode
         *      \code
         *          <allowedValueList>
         *              <allowedValue>yes</allowedValue>
         *              <allowedValue>no</allowedValue>
         *          </allowedValueList>
         *      \endcode
         *  Where it has none, nothing wraps them: the member's own element is repeated, once per item.
         *
         *  An array with no fItems has fDefaultArrayItems'.
         */
        shared_ptr<Binding> fItems;

        /**
         *  \brief The Binding each item of an array is written with, where the array's own Binding has no fItems: its fName is
         *         the element each item is written as - and with none (kRepeatedArrayItems), the member's own element is
         *         repeated, once per item
         *
         *  It holds for the arrays at and inside this element, until an element inside gives its own; nullptr takes the one the
         *  element this is inside has. The document starts with kDefaultArrayItems.
         *
         *  \par Example Usage
         *      \code
         *          // v is {"serviceList": {"service": ["a", "b"]}}
         *
         *          // nothing set - kDefaultArrayItems: an <Array> per item, wrapped in <service>
         *          Variant::XML::Writer{}.WriteAsString (v);
         *          //  <serviceList>
         *          //      <service>
         *          //          <Array>a</Array>
         *          //          <Array>b</Array>
         *          //      </service>
         *          //  </serviceList>
         *
         *          // an item name: an <item> per item, wrapped in <service>
         *          Variant::XML::Writer{{.fBinding = Binding{.fDefaultArrayItems = make_shared<Binding> (Binding{.fName = "item"sv})}}}
         *              .WriteAsString (v);
         *          //  <serviceList>
         *          //      <service>
         *          //          <item>a</item>
         *          //          <item>b</item>
         *          //      </service>
         *          //  </serviceList>
         *
         *          // no item name: <service> itself, repeated
         *          Variant::XML::Writer{{.fBinding = Binding{.fDefaultArrayItems = make_shared<Binding> (Binding::kRepeatedArrayItems)}}}
         *              .WriteAsString (v);
         *          //  <serviceList>
         *          //      <service>a</service>
         *          //      <service>b</service>
         *          //  </serviceList>
         *      \endcode
         *  Each reads back as it was written, given the same Binding - except an array of one item in the third, which reads back
         *  as that one item (a single `<service>a</service>` looks like any other member), unless the member's own Binding has
         *  fItems, saying it is an array:
         *
         *  | service      | nothing set  | an item name | no item name |
         *  |--------------|--------------|--------------|--------------|
         *  | ["a", "b"]   | ["a", "b"]   | ["a", "b"]   | ["a", "b"]   |
         *  | ["a"]        | ["a"]        | ["a"]        | "a"          |
         *  | []           | ""           | ""           | absent       |
         */
        shared_ptr<Binding> fDefaultArrayItems;

        /**
         *  \brief The Binding each item of an array is written with where neither fItems nor fDefaultArrayItems gives one: an
         *         `<Array>` element, wrapped in the member's own - so an array of one reads back as an array, though nothing says
         *         it is one
         */
        static const Binding kDefaultArrayItems;

        /**
         *  \brief An item Binding with no name, so no element of the items' own: the member's own element is repeated, once per
         *         item - as a schema of someone else's usually has an array. As fDefaultArrayItems, or a single array's fItems.
         */
        static const Binding kRepeatedArrayItems;

        /**
         *  \brief A prefix for each of these namespaces, declared on this element (xmlns:p=) and used for every element and
         *         attribute at and inside it that is in that namespace: SOAP's s:Envelope, s:encodingStyle
         *
         *  A namespace with no prefix is written as a default namespace (xmlns=) on the element that has it, which an attribute
         *  cannot be: an attribute in a namespace requires a prefix, declared here on its element or one it is inside. Writing
         *  only - reading takes namespaces from the document.
         *
         *  \par Example Usage
         *      \code
         *          // SOAP's envelope: {"encodingStyle": "http://schemas.xmlsoap.org/soap/encoding/", "Body": {"GetStatus": ""}}
         *          const URI kSOAP{"http://schemas.xmlsoap.org/soap/envelope/"sv};
         *          Binding b{.fName    = NameWithNamespace{kSOAP, "Envelope"sv},
         *                    .fMembers = {{"encodingStyle"sv, Binding{.fKind = Binding::Kind::eAttribute,
         *                                                             .fName = NameWithNamespace{kSOAP, "encodingStyle"sv}}},
         *                                 {"Body"sv, Binding{.fName = NameWithNamespace{kSOAP, "Body"sv}}}},
         *                    .fNamespacePrefixes = {{kSOAP, "s"sv}}};
         *      \endcode
         *      \code
         *          <s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="...">
         *              <s:Body>
         *                  <GetStatus></GetStatus>
         *              </s:Body>
         *          </s:Envelope>
         *      \endcode
         */
        Mapping<URI, String> fNamespacePrefixes;

        /**
         *  \brief The Binding for the member of this object with that key - nullptr where it has none, so the default shape
         */
        nonvirtual const Binding* MemberBinding (const String& key) const;

        /**
         *  \brief The name that member is written with: its Binding's fName, else its own key
         */
        nonvirtual NameWithNamespace NameFor (const String& key) const;

        /**
         *  \brief That member's kind: its Binding's, else an element
         */
        nonvirtual Kind KindFor (const String& key) const;

        /**
         *  \brief Reading: the key of the member written with that element (or attribute) name - the name itself, where no
         *         member was given a name of its own
         */
        nonvirtual String KeyForName (const String& elementName) const;

        /**
         *  \brief Reading: the key of the member that is this element's text, if one is
         */
        nonvirtual optional<String> TextMemberKey () const;

        /**
         *  \brief Equal where every field is - fItems and fDefaultArrayItems by the Bindings they point to, not the pointers
         */
        bool operator== (const Binding& rhs) const;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

}

#endif /*_Stroika_Foundation_DataExchange_Variant_XML_Binding_h_*/
