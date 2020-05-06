/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
            result += L"@";
        }
        else if (fType == NameType::eValue) {
            return L"{value}";
        }
        if (fNamespaceURI) {
            result += *fNamespaceURI + L":";
        }
        result += fLocalName;
        return result;
    }
#if __cpp_impl_three_way_comparison >= 201907
    inline strong_ordering Name::operator<=> (const Name& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs);
    }
    inline bool Name::operator== (const Name& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs) == 0;
    }
#endif

    /*
     ********************************************************************************
     ************** StructuredStreamEvents::Name::ThreeWayComparer ******************
     ********************************************************************************
     */
    inline Common::strong_ordering Name::ThreeWayComparer::operator() (const Name& lhs, const Name& rhs) const
    {
        // Treat EITHER side missing namespace as 'wildcard' matching any namespace
        if (lhs.fNamespaceURI.has_value () and rhs.fNamespaceURI.has_value ()) {
            Common::strong_ordering cmp = Common::ThreeWayCompare (*lhs.fNamespaceURI, *rhs.fNamespaceURI);
            if (cmp != Common::kEqual) {
                return cmp;
            }
        }
        Common::strong_ordering cmp = Common::ThreeWayCompare (lhs.fLocalName, rhs.fLocalName);
        if (cmp != Common::kEqual) {
            return cmp;
        }
        return Common::ThreeWayCompare (lhs.fType, rhs.fType);
    }

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ************** StructuredStreamEvents::Name comparison operators ***************
     ********************************************************************************
     */
    inline bool operator< (const Name& lhs, const Name& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) < 0;
    }
    inline bool operator<= (const Name& lhs, const Name& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) <= 0;
    }
    inline bool operator== (const Name& lhs, const Name& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) == 0;
    }
    inline bool operator!= (const Name& lhs, const Name& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) != 0;
    }
    inline bool operator>= (const Name& lhs, const Name& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) >= 0;
    }
    inline bool operator> (const Name& lhs, const Name& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) > 0;
    }
#endif

}

#endif /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_*/
