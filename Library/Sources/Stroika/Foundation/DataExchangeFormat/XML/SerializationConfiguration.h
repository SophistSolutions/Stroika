/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_XML_SerializationConfiguration_h_
#define _Stroika_Foundation_DataExchangeFormat_XML_SerializationConfiguration_h_    1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Memory/Optional.h"

#include    "Common.h"



/*
 * TODO:
 *      @todo   Add support for namespaces
 *
 *      @todo   Add support for Schema validation
 *				and optionally more stuff � for how to map between variant structure and schema. 
 *				Maybe have virtual API to make certain decisions?  
 *				Will need experience/experimation to get right
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {
            namespace   XML {


                using   Characters::String;
                using   Memory::Optional;

                /**
				 *	@see XML::Reader, and @see XML::Writer
                 */
                class   SerializationConfiguration {
                public:
                    SerializationConfiguration ();

                public:
                    // NOTE - defaults to "Document"
                    // if empty - then Write requiers
                    //              Require (v.GetType () == Memory::VariantValue::Type::eMap);
                    //
                    // If provided - must be a valid xml element name (ncname?)
                    nonvirtual  Optional<String>    GetDocumentElementName () const;
                    nonvirtual  void                SetDocumentElementName (const Optional<String>& n);

                public:
                    // NOTE - defaults to "Array"
                    // if empty - then Write requiers
                    //              Require (v.GetType () == Memory::VariantValue::Type::eMap);
                    //
                    // If provided - must be a valid xml element name (ncname?)
                    //
                    // DOCUMENT CONSEQUENCES OF MISSING (cannot deseriualize without heko?? but how)
                    nonvirtual  Optional<String>    GetArrayElementName () const;
                    nonvirtual  void                SetArrayElementName (const Optional<String>& n);

                private:
                    String  fDocumentElementName_;  // internally empty string for missing
                    String  fArrayElementName_;     // internally empty string for missing
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
#include    "SerializationConfiguration.inl"

#endif  /*_Stroika_Foundation_DataExchangeFormat_XML_SerializationConfiguration_h_*/
