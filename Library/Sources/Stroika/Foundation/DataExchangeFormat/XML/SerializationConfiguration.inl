/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchangeFormat_XML_SerializationConfiguration_inl_
#define _Stroika_Foundation_DataExchangeFormat_XML_SerializationConfiguration_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchangeFormat {
            namespace   XML {


                /*
                 ********************************************************************************
                 ********************* XML::SerializationConfiguration **************************
                 ********************************************************************************
                 */
                inline  Optional<String>    SerializationConfiguration::GetDocumentElementName () const
                {
                    return fDocumentElementName_.empty () ? Optional<String> () : fDocumentElementName_;
                }
                inline  void    SerializationConfiguration::SetDocumentElementName (const Optional<String>& n)
                {
                    Require (n.empty () or not (*n).empty ());  // should validate legit xml elt name
                    fDocumentElementName_ = n.empty () ? String () : *n;
                }
                inline  Optional<String>    SerializationConfiguration::GetArrayElementName () const
                {
                    return fArrayElementName_.empty () ? Optional<String> () : fArrayElementName_;
                }
                inline  void    SerializationConfiguration::SetArrayElementName (const Optional<String>& n)
                {
                    Require (n.empty () or not (*n).empty ());  // should validate legit xml elt name
                    fArrayElementName_ = n.empty () ? String () : *n;
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchangeFormat_XML_SerializationConfiguration_inl_*/
