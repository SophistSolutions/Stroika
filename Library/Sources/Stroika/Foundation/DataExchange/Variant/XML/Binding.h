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
#include "Stroika/Foundation/DataExchange/VariantValue.h"
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
     *         not what is wanted (e.g. an attribute rather than an element, a namespace, a name of its own, an order of its own)
     *
     *  A Binding mirrors the shape of the value it goes with: an object's members are fMembers, **in the order XML wants them**
     *  (an element's children are an ordered list, and xs:sequence is XSD's normal idiom, while a VariantValue object has no
     *  order); each item of an array is fItems. Nothing is required - a member fMembers does not name keeps the default shape,
     *  and is written after those it does - so a Binding need say only how its value differs.
     *
     *  **Declaring a value's type (fType) changes what the Writer and Reader do with it, a great deal** - fType's table says
     *  how: with no type declared they take any value, and guess at any document; with one, they read back that type, and throw
     *  on anything else.
     *
     *  **Given a complete Binding (IsComplete), the Writer and Reader round-trip a VariantValue faithfully, or throw
     *  BadFormatException because the value is not one the Binding describes - never anything in between**: nothing guessed,
     *  nothing silently lost, whatever values it holds - as an XSD or a WSDL does. A complete Binding declares every value's
     *  type, and leaves nothing to the default shape. The one exception is what XML writes as nothing, read back absent (@see
     *  IsComplete). A Binding that leaves things to defaults is shorter, and particular values can then collide with them (a
     *  member keyed "Array" reads back as an array's item) - the price of the short Binding, not a bug.
     *
     *  **The top level Binding** - the one a Reader or Writer is given - describes the whole document; those inside it (fMembers,
     *  fItems) each describe a part of it. With an fName, the top level Binding names the document element, and the value is its
     *  content (reading a document element of another name throws BadFormatException). With none, the value is an object of
     *  exactly one member, and that member is the document element - named, and bound, as any other member is (writing any other
     *  value throws BadFormatException).
     *
     *  Anything may produce a Binding: written by hand, as below; generated from an ObjectVariantMapper, which knows a type's field
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
         *  The element's (or attribute's) name, where that is not the member's own key - and its namespace. On the top level Binding,
         *  the document element's.
         */
        optional<NameWithNamespace> fName;

        /**
         *  The type of the value this Binding goes with - nullopt (the default) leaving it to the value.
         *
         *  **Declaring it changes what the Writer and Reader do, and a great deal.** With none, a Binding says only how a value
         *  is written: the Writer takes any value, and the Reader any document, guessing its shape. Declared, it also says which
         *  values there may be - as an XSD does - and the Writer and Reader throw BadFormatException on any other:
         *
         *  |                               | fType nullopt - the default         | fType declared                     |
         *  |-------------------------------|-------------------------------------|------------------------------------|
         *  | a leaf, read back             | a String: 3 as "3"                  | that type: 3 as 3                  |
         *  | text not of its type, read    | - (every leaf reads as a String)    | throws                             |
         *  | an empty element, read back   | "" (but [] where fItems is set)     | {} for eMap, [] for eArray         |
         *  | a null member, written        | as an empty element: read back ""   | as none: read back absent          |
         *  | another type's value, written | as it is (a leaf as its As<String>) | throws                             |
         *  | a member it does not name     | written after those it names; read  | throws, writing and reading        |
         *  | a child named <Array>, read   | an item, unless fMembers names it   | an item only where fType is eArray |
         *
         *  A null member is written as none as ObjectVariantMapper omits a null field. Per Binding, so per value: a member of a
         *  declared object whose own Binding declares no type is still written and read the first way. A Binding declaring the
         *  type of every value in it, and leaving nothing else to the default shape, is complete (@see IsComplete).
         *
         *  @todo An eMap OPEN to members it does not declare (XSD's xs:any, JSON Schema's additionalProperties), if a need
         *        appears. Extra members left to the default shape could never be complete - their types lost, their names free
         *        to collide. The likely need is a dictionary (a Mapping<String, T> field, which a Binding generated from an
         *        ObjectVariantMapper will meet - https://github.com/SophistSolutions/Stroika/issues/1217): every extra member of
         *        ONE declared Binding. That can be complete only with its keys written as data, not as element names (a key
         *        need not be an XML name) - entries, as <e key="k">v</e>.
         */
        optional<VariantValue::Type> fType;

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
         *
         *  Reading, an element with the item name is an item - so is a member written with that name (a key "Array", in the
         *  default shape), unless the Binding of the object it is in names it in fMembers.
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
         *  \brief Reading: the key of the member of that kind (an element or an attribute) written with that local name - the
         *         name itself, where no such member is named
         *
         *  By kind, as an element and an attribute may have the same name.
         */
        nonvirtual String KeyForName (Kind kind, const String& name) const;

        /**
         *  \brief Reading: the key of the member that is this element's text, if one is
         */
        nonvirtual optional<String> TextMemberKey () const;

        /**
         *  \brief Whether, given this Binding as the top level Binding (the one a Reader or Writer is given), the Writer and Reader
         *         round-trip every VariantValue faithfully, or throw - never anything in between
         *
         *  **Given a complete Binding, writing a VariantValue and reading it back gives that value - or throws
         *  BadFormatException, the value not being one the Binding describes.** The Writer throws on a value of another type,
         *  or with a member the Binding does not declare; the Reader, on a document not of the Binding's type (an element or
         *  attribute it does not declare, a member repeated that it declares as one value, text that is not of its type).
         *  Nothing is guessed, and nothing silently lost - but for what XML writes as nothing (below).
         *
         *  Complete means it leaves nothing to the default shape, or to the value:
         *      - it names the document element (fName), and each Binding in it declares fType
         *      - each member of an object (fMembers) has a Binding, and is written with an XML name no other element member - or,
         *        for an attribute, no other attribute - of that object has (by local name, as the Reader matches them)
         *      - each array has fItems; an array whose items are its member's element repeated is an object's member (not an
         *        array's item, nor the document element)
         *      - attributes and text are leaves, a text member is beside attributes only, and an attribute in a namespace has a
         *        prefix (fNamespacePrefixes) in scope
         *
         *  The one exception, by design: XML has no way to write a null member, an empty array of repeated elements, or a text
         *  member (Kind::eText) whose text is empty - an empty string or BLOB - but as nothing, so each reads back absent. (As
         *  ObjectVariantMapper omits a null field anyway, and reads an absent one as its default.)
         *
         *  A Binding generated from a type's declaration (an ObjectVariantMapper's -
         *  https://github.com/SophistSolutions/Stroika/issues/1217) can be complete by construction; one written by hand may
         *  instead be short, leaving the rest to the default shape. ToString () says which - and, where it is not complete, why.
         *
         *  @todo NOT YET SO of a string holding a character the XML escaping gets wrong - a bug, silently changing it: CR (read
         *        back as LF), tab or LF in an attribute (read back as a space). And the Reader, not the Writer, throws on another
         *        control character (written as a reference XML 1.0 does not allow), and on Windows on a character outside the BMP
         *        (written as two surrogate references) - which should round-trip.
         */
        nonvirtual bool IsComplete () const;

        /**
         *  \brief Equal where every field is - fItems and fDefaultArrayItems by the Bindings they point to, not the pointers
         */
        bool operator== (const Binding& rhs) const;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        // why this Binding, as the top level Binding, is not complete - the first thing found; nullopt where it is
        nonvirtual optional<String> WhyIncomplete_ () const;
        // with whether it is complete only where top: a Binding inside another is not the top level one
        nonvirtual String ToString_ (bool top) const;
    };

}

#endif /*_Stroika_Foundation_DataExchange_Variant_XML_Binding_h_*/
