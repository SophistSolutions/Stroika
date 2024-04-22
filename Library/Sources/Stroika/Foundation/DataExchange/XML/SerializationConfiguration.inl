/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::DataExchange::XML {

    /*
     ********************************************************************************
     ********************* XML::SerializationConfiguration **************************
     ********************************************************************************
     */
    inline optional<String> SerializationConfiguration::GetDocumentElementName () const
    {
        return fDocumentElementName_.empty () ? optional<String> () : fDocumentElementName_;
    }
    inline void SerializationConfiguration::SetDocumentElementName (const optional<String>& n)
    {
        Require (not n.has_value () or not(*n).empty ()); // should validate legit xml elt name
        fDocumentElementName_ = n.has_value () ? *n : String{};
    }
    inline optional<String> SerializationConfiguration::GetArrayElementName () const
    {
        return fArrayElementName_.empty () ? optional<String> () : fArrayElementName_;
    }
    inline void SerializationConfiguration::SetArrayElementName (const optional<String>& n)
    {
        Require (not n.has_value () or not(*n).empty ()); // should validate legit xml elt name
        fArrayElementName_ = n.has_value () ? *n : String{};
    }

}
