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

#include "../../Characters/StringBuilder.h"
#include "../../Characters/ToString.h"

namespace Stroika::Foundation::DataExchange::XML {

    /*
     ********************************************************************************
     ************************** XML::NamespaceDefinitions ***************************
     ********************************************************************************
     */
    inline NamespaceDefinitions::NamespaceDefinitions (const optional<URI>& defaultNamespace, const Mapping<String, URI>& prefixedNamespaces)
        : fDefaultNamespace_{defaultNamespace}
        , fPrefixedNS_{prefixedNamespaces}
    {
    }
    inline NamespaceDefinitions::NamespaceDefinitions (const Mapping<String, URI>& prefixedNamespaces)
        : fPrefixedNS_{prefixedNamespaces}
    {
    }
    inline optional<URI> NamespaceDefinitions::GetDefaultNamespace () const
    {
        return fDefaultNamespace_;
    }
    inline Mapping<String, URI> NamespaceDefinitions::GetPrefixedNamespaces () const
    {
        return fPrefixedNS_;
    }
    inline String NamespaceDefinitions::ToString () const
    {
        Characters::StringBuilder sb;
        sb << "{";
        if (fDefaultNamespace_) {
            sb << "defaultNS: " << Characters::ToString (*fDefaultNamespace_) << ", ";
        }
        if (fDefaultNamespace_) {
            sb << "prefixedNamespaces: " << Characters::ToString (fPrefixedNS_);
        }
        sb << "}";
        return sb.str ();
    }

    /*
     ********************************************************************************
     ************************* XML::NameWithNamespace *******************************
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
    inline String NameWithNamespace::ToString () const
    {
        Characters::StringBuilder sb;
        if (fNamespace) {
            sb << fNamespace->As<String> () << ":";
        }
        sb << fName;
        return Characters::ToString (sb.str ());
    }

}

#endif /*_Stroika_Foundation_DataExchange_XML_Namespace_inl_*/
