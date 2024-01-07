/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Namespace_inl_
#define _Stroika_Foundation_DataExchange_XML_Namespace_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::DataExchange::XML {

    /*
     ********************************************************************************
     ******************************** NamespaceDefinition ***************************
     ********************************************************************************
     */
    inline NamespaceDefinition::NamespaceDefinition (const URI& uri, const optional<String>& prefix)
        : fURI{uri}
        , fPrefix{prefix}
    {
    }

    /*
     ********************************************************************************
     *************************** NamespaceDefinitionsList ***************************
     ********************************************************************************
     */
    inline NamespaceDefinitionsList::NamespaceDefinitionsList (const vector<NamespaceDefinition>& namespaces)
        : fNamespaces (namespaces)
    {
    }
    inline bool NamespaceDefinitionsList::empty () const
    {
        return fNamespaces.empty ();
    }
    inline Sequence<NamespaceDefinition> NamespaceDefinitionsList::GetNamespaces () const
    {
        return fNamespaces;
    }
    inline void NamespaceDefinitionsList::SetNamespaces (const Sequence<NamespaceDefinition>& namespaces)
    {
        fNamespaces = namespaces;
    }


    
    /*
     ********************************************************************************
     *********************** XML::NameWithNamespace *********************************
     ********************************************************************************
     */
    template <Characters::IConvertibleToString NAME_TYPE>
    inline NameWithNamespace::NameWithNamespace (NAME_TYPE&& name)
        : fName{name}
    {
    }
    inline NameWithNamespace::NameWithNamespace (const optional<URI>& ns, const String& name)
        : fName{name}
        , fNamespace{ns}
    {
    }

}

#endif /*_Stroika_Foundation_DataExchange_XML_Namespace_inl_*/
