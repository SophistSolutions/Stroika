/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Namespace_h_
#define _Stroika_Foundation_DataExchange_XML_Namespace_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Characters/String.h"
#include "../../Containers/Mapping.h"
#include "../../Containers/Sequence.h"
#include "../../IO/Network/URI.h"

#include "Common.h"

namespace Stroika::Foundation::DataExchange::XML {

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using IO::Network::URI;

    /**
     *  \brief used to specify default namespace, and any n: prefixes applicable to elements.
     */
    struct NamespaceDefinitions {
    public:
        constexpr NamespaceDefinitions () = default;
        NamespaceDefinitions (const optional<URI>& defaultNamespace, const Mapping<String, URI>& prefixedNamepsaces = {});
        NamespaceDefinitions (const Mapping<String, URI>& prefixedNamepsaces);

    public:
        nonvirtual optional<URI> GetDefaultNamespace () const;

    public:
        nonvirtual Mapping<String, URI> GetPrefixedNamespaces () const;

    private:
        optional<URI>                  fDefaultNamespace_;
        optional<Mapping<String, URI>> fPrefixedNS_; // optional so can do constexpr
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
