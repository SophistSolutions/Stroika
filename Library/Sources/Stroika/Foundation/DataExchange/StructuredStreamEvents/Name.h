/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Memory/Optional.h"



/**
 *  \file
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {


                using   Characters::String;
                using   Memory::Optional;

                /**
                 */
                struct Name {
                    /**
                     *  fNamespaceURI missing treated as 'wildcard' - matching any other URI name, and
                     *  empty string ("") means no namespace (to require no namespace as ns="").
                     */
                    Optional<String>    fNamespaceURI;
                    String              fLocalName;

                    /**
                     *  \note   This terminology differences from XML practice. Here ElementType is really like NodeType.
                     *          But for now we only have 2 types of nodes (because that so far seems all that will be needed for
                     *          what we do).
                     *
                     *  \note   Value must have an empty string localname and URI, and refers to XML 'mixed' structures, with both
                     *          complex fields, and simple data.
                     */
                    enum    NameType {
                        eElement,
                        eAttribute,
                        eValue
                    };
                    NameType        fType { eElement };

                    /**
                     *  The overload with only NameType \req type == NameType::eValue
                     */
                    Name () = delete;
                    Name (NameType type);
                    Name (const String& localName, NameType type = eElement);
                    Name (const String& namespaceURI, const String& localName, NameType type = eElement);

                    /**
                     *  Purely for debugging / diagnostic purposes. Don't count on this format.
                     */
                    nonvirtual  String  ToString () const;
                };


                /**
                 *  Basic operator< with the obivous meaning.
                 */
                bool    operator< (const Name& lhs, const Name& rhs);

                /**
                 *  Basic operator<= with the obivous meaning
                 */
                bool    operator<= (const Name& lhs, const Name& rhs);

                /**
                 *  Basic operator== with the obivous meaning
                 */
                bool    operator== (const Name& lhs, const Name& rhs);

                /**
                 *  Basic operator!= with the obivous meaning
                 */
                bool    operator!= (const Name& lhs, const Name& rhs);

                /**
                 *  Basic operator>= with the obivous meaning
                 */
                bool    operator>= (const Name& lhs, const Name& rhs);

                /**
                 *  Basic operator> with the obivous meaning
                 */
                bool    operator> (const Name& lhs, const Name& rhs);


            }
        }
    }
}





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Name.inl"

#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_h_*/
