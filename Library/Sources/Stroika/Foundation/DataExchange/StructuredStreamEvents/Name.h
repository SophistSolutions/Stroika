/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_h_ 1

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Characters/String.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 */

namespace Stroika::Foundation::DataExchange::StructuredStreamEvents {

    using Characters::String;

    /**
     */
    struct Name {
        /**
         *  fNamespaceURI missing treated as 'wildcard' - matching any other URI name, and
         *  empty string ("") means no namespace (to require no namespace as ns="").
         */
        optional<String> fNamespaceURI;
        String           fLocalName;

        /**
         *  \note   This terminology differences from XML practice. Here ElementType is really like NodeType.
         *          But for now we only have 2 types of nodes (because that so far seems all that will be needed for
         *          what we do).
         *
         *  \note   Value must have an empty string localname and URI, and refers to XML 'mixed' structures, with both
         *          complex fields, and simple data.
         */
        enum NameType {
            eElement,
            eAttribute,
            eValue,

            Stroika_Define_Enum_Bounds (eElement, eValue)
        };
        NameType fType{eElement};

        /**
         *  The overload with only NameType \req type == NameType::eValue
         */
        Name ()            = delete;
        Name (const Name&) = default;
        Name (NameType type);
        Name (const String& localName, NameType type = eElement);
        Name (const String& namespaceURI, const String& localName, NameType type = eElement);

        struct ThreeWayComparer;

        /**
         *  Purely for debugging / diagnostic purposes. Don't count on this format.
         */
        nonvirtual String ToString () const;
    };

    /**
     *  Treat EITHER side missing namespace as 'wildcard' matching any namespace
     */
    struct Name::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        Common::strong_ordering operator() (const Name& lhs, const Name& rhs) const;
    };

    /**
     *  Basic operator overloads with the obivous meaning, and simply indirect to @Common::ThreeWayCompare
     */
    bool operator< (const Name& lhs, const Name& rhs);
    bool operator<= (const Name& lhs, const Name& rhs);
    bool operator== (const Name& lhs, const Name& rhs);
    bool operator!= (const Name& lhs, const Name& rhs);
    bool operator>= (const Name& lhs, const Name& rhs);
    bool operator> (const Name& lhs, const Name& rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Name.inl"

#endif /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_h_*/
