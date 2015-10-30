/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {


                /*
                 ********************************************************************************
                 *********************** StructuredStreamEvents::Name ***************************
                 ********************************************************************************
                 */
                inline  Name::Name (const String& localName, bool isAttribute)
                    : fLocalName { localName }
                , fIsAttribute { isAttribute } {
                }
                inline  Name::Name (const String& namespaceURI, const String& localName, bool isAttribute)
                    : fNamespaceURI { namespaceURI }
                , fLocalName { localName }
                , fIsAttribute { isAttribute } {
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_*/
