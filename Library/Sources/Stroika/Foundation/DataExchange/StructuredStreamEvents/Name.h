/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Memory/Optional.h"

#include    "../VariantValue.h"



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
                    Optional<String>    fNamespaceURI;
                    String              fLocalName;

                    /**
                     *  \note   This terminology differences from XML practice. Here ElementType is really like NodeType.
                     *          But for now we only have 2 types of nodes (because that so far seems all that will be needed for
                     *          what we do).
                     */
                    enum    NameType {
                        eElement,
                        eAttribute,
                    };
                    NameType        fType { eElement };

                    Name () = delete;
                    Name (const String& localName, NameType type = eElement);
                    Name (const String& namespaceURI, const String& localName, NameType type = eElement);
                };


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
