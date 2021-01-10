/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Namespace_inl_
#define _Stroika_Foundation_DataExchange_XML_Namespace_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::DataExchange::XML {

    //  class   NamespaceDefinition
    inline NamespaceDefinition::NamespaceDefinition (const wstring& uri, const wstring& prefix)
        : fURI{uri}
        , fPrefix{prefix}
    {
    }
#if __cpp_impl_three_way_comparison < 201907
    inline bool operator< (const NamespaceDefinition& lhs, const NamespaceDefinition& rhs)
    {
        if (lhs.fURI == rhs.fURI) {
            return lhs.fPrefix < rhs.fPrefix;
        }
        return lhs.fURI < rhs.fURI;
    }
    inline bool operator== (const NamespaceDefinition& lhs, const NamespaceDefinition& rhs)
    {
        return lhs.fURI == rhs.fURI and lhs.fPrefix == rhs.fPrefix;
    }
#endif

    //  class   NamespaceDefinitionsList
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
