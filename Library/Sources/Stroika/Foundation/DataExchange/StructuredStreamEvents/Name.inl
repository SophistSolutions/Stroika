/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::DataExchange::StructuredStreamEvents {

    /*
     ********************************************************************************
     *********************** StructuredStreamEvents::Name ***************************
     ********************************************************************************
     */
    inline Name::Name ([[maybe_unused]] NameType type)
        : fType{eValue}
    {
        Require (type == eValue);
    }
    inline Name::Name (const String& localName, NameType type)
        : fLocalName{localName}
        , fType{type}
    {
    }
    inline Name::Name (const String& namespaceURI, const String& localName, NameType type)
        : fNamespaceURI{namespaceURI}
        , fLocalName{localName}
        , fType{type}
    {
    }
    inline String Name::ToString () const
    {
        String result;
        if (fType == NameType::eAttribute) {
            result += "@"sv;
        }
        else if (fType == NameType::eValue) {
            return "{value}"sv;
        }
        if (fNamespaceURI) {
            result += *fNamespaceURI + ":"sv;
        }
        result += fLocalName;
        return result;
    }
    inline strong_ordering Name::operator<=> (const Name& rhs) const
    {
        return TWC_ (*this, rhs);
    }
    inline bool Name::operator== (const Name& rhs) const
    {
        return TWC_ (*this, rhs) == 0;
    }
    inline strong_ordering Name::TWC_ (const Name& lhs, const Name& rhs)
    {
        // Treat EITHER side missing namespace as 'wildcard' matching any namespace
        if (lhs.fNamespaceURI.has_value () and rhs.fNamespaceURI.has_value ()) {
            if (auto cmp = *lhs.fNamespaceURI <=> *rhs.fNamespaceURI; cmp != strong_ordering::equal) {
                return cmp;
            }
        }
        if (auto cmp = lhs.fLocalName <=> rhs.fLocalName; cmp != strong_ordering::equal) {
            return cmp;
        }
        return lhs.fType <=> rhs.fType;
    }

}

#endif /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_*/
