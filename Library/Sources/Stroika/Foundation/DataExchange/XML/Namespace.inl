/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
    inline NamespaceDefinition::NamespaceDefinition (const wstring& uri, const wstring& prefix)
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
    inline vector<NamespaceDefinition> NamespaceDefinitionsList::GetNamespaces () const
    {
        return fNamespaces;
    }
    inline void NamespaceDefinitionsList::SetNamespaces (const vector<NamespaceDefinition>& namespaces)
    {
        fNamespaces = namespaces;
    }

}

#endif /*_Stroika_Foundation_DataExchange_XML_Namespace_inl_*/
