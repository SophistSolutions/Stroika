/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Namespace_h_
#define _Stroika_Foundation_DataExchange_XML_Namespace_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Containers/Sequence.h"
#include "../../IO/Network/URI.h"

#include "Common.h"

namespace Stroika::Foundation::DataExchange::XML {

    using Characters::String;
    using Containers::Sequence;
    using IO::Network::URI;

    /**
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     * 
     *  \note similar to NameWithNamespace, the 'meaning' of prefixes used in xml, and their associated namepace (or when missing prefix the default namespace)
     */
    struct NamespaceDefinition {
        NamespaceDefinition (const URI& uri, const optional<String>& prefix = {});

        URI              fURI; // required non-null
        optional<String> fPrefix;

#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif
        auto operator<=> (const NamespaceDefinition& rhs) const = default;
#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif
    };

    /**
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     * 
     *  @todo Enforce that only ONE namespace DEF can have an empty prefix!
     *  REally - sb not VECTOR - but MAPPING from prefix (whci cna be empty) to namespace string.
     * 
     *  REALLY - to be clear - this sin't ncesariyl all naemapces, just all prefixes...
     * 
     *      @todo use Sequnce not vector
     */
    class NamespaceDefinitionsList {
    public:
        NamespaceDefinitionsList (const vector<NamespaceDefinition>& namespaces = {});

    public:
        nonvirtual bool empty () const;

    public:
        nonvirtual Sequence<NamespaceDefinition> GetNamespaces () const;
        nonvirtual void                          SetNamespaces (const Sequence<NamespaceDefinition>& namespaces);

    public:
        nonvirtual void Add (const URI& uri, const optional<String>& prefix = {});

    private: // sb mapping<prefix -> URI>
        Sequence<NamespaceDefinition> fNamespaces;
    };

    /**
     *  Note name argument slightly more flexible than just String so double conversion works ("" can be assigned to NameWithNamespace)
     * 
     *  \note similar to NamespaceDefinition, but this refers to elements which may or may not have an associated namespace.
     */
    struct NameWithNamespace {
        String        fName;
        optional<URI> fNamespace;

        template <Characters::IConvertibleToString NAME_TYPE>
        NameWithNamespace (NAME_TYPE&& name);
        NameWithNamespace (const optional<URI>& ns, const String& name);

        bool operator== (const NameWithNamespace& rhs) const = default;
#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif
        auto operator<=> (const NameWithNamespace& rhs) const = default;
#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Namespace.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_Namespace_h_*/
