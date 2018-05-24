/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_SerializationConfiguration_inl_
#define _Stroika_Foundation_DataExchange_XML_SerializationConfiguration_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace XML {

                /*
                 ********************************************************************************
                 ********************* XML::SerializationConfiguration **************************
                 ********************************************************************************
                 */
                inline Optional<String> SerializationConfiguration::GetDocumentElementName () const
                {
                    return fDocumentElementName_.empty () ? Optional<String> () : fDocumentElementName_;
                }
                inline void SerializationConfiguration::SetDocumentElementName (const Optional<String>& n)
                {
                    Require (not n.has_value () or not(*n).empty ()); // should validate legit xml elt name
                    fDocumentElementName_ = n.has_value () ? *n : String ();
                }
                inline Optional<String> SerializationConfiguration::GetArrayElementName () const
                {
                    return fArrayElementName_.empty () ? Optional<String> () : fArrayElementName_;
                }
                inline void SerializationConfiguration::SetArrayElementName (const Optional<String>& n)
                {
                    Require (not n.has_value () or not(*n).empty ()); // should validate legit xml elt name
                    fArrayElementName_ = n.has_value () ? *n : String ();
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_DataExchange_XML_SerializationConfiguration_inl_*/
