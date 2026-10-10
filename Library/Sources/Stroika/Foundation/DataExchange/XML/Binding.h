/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Binding_h_
#define _Stroika_Foundation_DataExchange_XML_Binding_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/XML/Namespace.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::DataExchange::XML {

    using Characters::String;
    using Containers::Sequence;

    struct Binding;

    /**
     *  \brief One member of an object, in a Binding: its key, and how that member differs from the default shape (nothing, by
     *         default - naming it is enough to give it a place in the order)
     */
    struct BindingMember {
        String              fKey;
        shared_ptr<Binding> fBinding;

        BindingMember (const String& key);                         ///< \brief the member's own key, and the default shape
        BindingMember (const String& key, const Binding& binding); ///< \brief the member's own key, and how it differs

        bool operator== (const BindingMember& rhs) const;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

    /**
     *  \brief How a VariantValue is to be written as XML, and read back, where the default shape (@see Variant::XML::Writer) is
     *         not what is wanted - an attribute rather than an element, a namespace, a name of its own, an order of its own
     *
     *  A Binding mirrors the shape of the value it goes with: an object's members are fMembers, **in the order XML wants them**
     *  (an element's children are an ordered list, and xs:sequence is XSD's normal idiom, while a VariantValue object has no
     *  order); each item of an array is fItems. Nothing is required - a member fMembers does not name keeps the default shape,
     *  and is written after those it does - so a Binding need say only how its value differs.
     *
     *  Anything may produce one: written by hand, as below; generated from an ObjectVariantMapper, which knows a type's field
     *  names and their declaration order (https://github.com/SophistSolutions/Stroika/issues/1217); or taken from a schema of
     *  someone else's, an XSD or a WSDL, where the order is normative rather than incidental.
     *
     *  \par Example Usage
     *      \code
     *          // UPnP's <stateVariable sendEvents="yes"><name>..</name><dataType>..</dataType></stateVariable>
     *          Binding b{.fMembers = {{"sendEvents"sv, Binding{.fKind = Binding::Kind::eAttribute}},
     *                                 {"name"sv},
     *                                 {"dataType"sv}}};
     *      \endcode
     *
     *  \note A namespace is written as a default namespace (xmlns="...") on the element that has one. A prefixed namespace -
     *        and so an attribute in a namespace, which cannot be written without a prefix - is not expressible yet.
     *        @see https://github.com/SophistSolutions/Stroika/issues/954
     */
    struct Binding {
        /**
         *  Whether the member this Binding goes with is written as a child element (the default), as an attribute of the
         *  element it is a member of, or as that element's text.
         */
        enum class Kind {
            eElement,
            eAttribute,
            eText
        };
        Kind fKind{Kind::eElement};

        /**
         *  The element's (or attribute's) name, where that is not the member's own key - and its namespace.
         */
        optional<NameWithNamespace> fName;

        /**
         *  This object's members, in the order to write them. A member of the value that is not named here is written after
         *  those that are, in the order the value gives them.
         */
        Sequence<BindingMember> fMembers;

        /**
         *  A Binding for each item of this array - and, reading, what says this member IS an array, however few elements the
         *  document happens to have (@see Variant::XML::Writer on that ambiguity).
         */
        shared_ptr<Binding> fItems;

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

        bool operator== (const Binding& rhs) const = default;

        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Binding.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_Binding_h_*/
