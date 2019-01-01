/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Namespace_h_
#define _Stroika_Foundation_DataExchange_XML_Namespace_h_ 1

#include "../../StroikaPreComp.h"

#include <istream>

#include "../../Configuration/Common.h"

#include "../VariantValue.h"
#include "Common.h"

namespace Stroika::Foundation::DataExchange::XML {

    /**
     */
    struct NamespaceDefinition {
        NamespaceDefinition (const wstring& uri, const wstring& prefix = wstring ());

        wstring fURI;    // required non-null
        wstring fPrefix; // can be nullptr
    };
    // order by URI, but equal requires both equal
    bool operator< (const NamespaceDefinition& lhs, const NamespaceDefinition& rhs);
    bool operator== (const NamespaceDefinition& lhs, const NamespaceDefinition& rhs);

    /**
     */
    class NamespaceDefinitionsList {
    public:
        NamespaceDefinitionsList (const vector<NamespaceDefinition>& namespaces = vector<NamespaceDefinition> ());

    public:
        nonvirtual bool empty () const;

    public:
        nonvirtual vector<NamespaceDefinition> GetNamespaces () const;
        nonvirtual void                        SetNamespaces (const vector<NamespaceDefinition>& namespaces);

    public:
        nonvirtual void Add (const wstring& uri, const wstring& prefix = wstring ());

    private:
        vector<NamespaceDefinition> fNamespaces;

    private:
        friend bool operator< (const NamespaceDefinitionsList& lhs, const NamespaceDefinitionsList& rhs);
        friend bool operator== (const NamespaceDefinitionsList& lhs, const NamespaceDefinitionsList& rhs);
    };
    bool operator< (const NamespaceDefinitionsList& lhs, const NamespaceDefinitionsList& rhs);
    bool operator== (const NamespaceDefinitionsList& lhs, const NamespaceDefinitionsList& rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Namespace.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_Namespace_h_*/
