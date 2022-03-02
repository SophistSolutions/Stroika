/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Namespace_h_
#define _Stroika_Foundation_DataExchange_XML_Namespace_h_ 1

#include "../../StroikaPreComp.h"

#include <compare>
#include <istream>

#include "../../Configuration/Common.h"

#include "../VariantValue.h"
#include "Common.h"

namespace Stroika::Foundation::DataExchange::XML {

    /**
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     */
    struct NamespaceDefinition {
        NamespaceDefinition (const wstring& uri, const wstring& prefix = wstring{});

        wstring fURI;    // required non-null
        wstring fPrefix; // can be nullptr

#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif
        auto operator<=> (const NamespaceDefinition& rhs) const = default;
#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif
    };

    /**
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
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
        nonvirtual void Add (const wstring& uri, const wstring& prefix = wstring{});

    private:
        vector<NamespaceDefinition> fNamespaces;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Namespace.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_Namespace_h_*/
