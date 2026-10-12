/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Namespace_h_
#define _Stroika_Foundation_DataExchange_XML_Namespace_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/DataExchange/XML/Common.h"
#include "Stroika/Foundation/IO/Network/URI.h"

namespace Stroika::Foundation::DataExchange::XML {

    using Characters::String;
    using Containers::Mapping;
    using Containers::Sequence;
    using IO::Network::URI;

    /**
     *  \brief used to specify default namespace, and any n: prefixes applicable to elements.
     * 
     *  \note default namespace does NOT apply to attributes, only element names...
     */
    struct NamespaceDefinitions {
    public:
        NamespaceDefinitions () = default;
        NamespaceDefinitions (const optional<URI>& defaultNamespace, const Mapping<String, URI>& prefixedNamespaces = {});
        NamespaceDefinitions (const Mapping<String, URI>& prefixedNamespaces);

    public:
        nonvirtual bool operator== (const NamespaceDefinitions&) const = default;

    public:
        nonvirtual optional<URI> GetDefaultNamespace () const;

    public:
        nonvirtual Mapping<String, URI> GetPrefixedNamespaces () const;

    public:
        /**
         */
        nonvirtual String ToString () const;

    private:
        optional<URI>        fDefaultNamespace_;
        Mapping<String, URI> fPrefixedNS_;
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
        /**
         */
        nonvirtual String ToString () const;
    };

    /**
     *  Common 'name/namespace' combo used internally in XML.
     */
    inline const NameWithNamespace kXMLNS{"http://www.w3.org/2000/xmlns/"sv, "xmlns"sv};

    /**
     *  \brief NameWithNamespace::fNamespace for a name in NO namespace - which XML spells as an empty namespace name, and
     *         a document writes as xmlns="" where a default namespace would otherwise be in scope
     *
     *  Distinct from kInheritNamespace, which these two are named apart from each other to make unmissable: an element
     *  saying nothing about its namespace takes its parent's, where one saying kNoNamespace takes none at all. Say which
     *  with these, never with the optional<URI> underneath - which of them is nullopt is only history (@see
     *  kInheritNamespace), and neither reading of a bare nullopt would be obvious.
     *
     *  \par Example Usage
     *      \code
     *          // SOAP 1.1 has a Fault's own children unqualified (section 4.4), inside an Envelope that is not
     *          faultElt.Append (NameWithNamespace{kNoNamespace, "faultcode"sv}, "s:Client"sv);
     *      \endcode
     *
     *  \note An ATTRIBUTE is in no namespace unless it carries a prefix, whatever default is in scope (XML Namespaces,
     *        section 6.2) - so for an attribute this is what an unprefixed name means anyway.
     */
    inline const optional<IO::Network::URI> kNoNamespace{IO::Network::URI{}};

    /**
     *  \brief NameWithNamespace::fNamespace for a name in whatever namespace the element it is created under is in - the
     *         default, and what a name constructed without a namespace means
     *
     *  Unlike kNoNamespace this is not an XML notion at all: a document only ever says a namespace or none. It is a
     *  convenience for BUILDING one, most of whose elements are in the same namespace as their parent, and it is nullopt
     *  only because that is what every name built without a namespace has always meant.
     */
    inline const optional<IO::Network::URI> kInheritNamespace{};

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Namespace.inl"

#endif /*_Stroika_Foundation_DataExchange_XML_Namespace_h_*/
