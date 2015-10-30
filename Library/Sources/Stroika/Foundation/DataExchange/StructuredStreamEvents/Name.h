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
                    bool                fIsAttribute { false };

                    Name () = delete;
                    Name (const String& localName, bool isAttribute = false);
                    Name (const String& namespaceURI, const String& localName, bool isAttribute = false);
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
