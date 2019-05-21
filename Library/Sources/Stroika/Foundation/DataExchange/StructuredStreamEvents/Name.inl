/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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

    /*
     ********************************************************************************
     ************** StructuredStreamEvents::Name::ThreeWayComparer ******************
     ********************************************************************************
     */
    inline int Name::ThreeWayComparer::operator() (const Name& lhs, const Name& rhs) const
    {
        // Treat EITHER side missing namespace as 'wildcard' matching any namespace
        if (lhs.fNamespaceURI.has_value () and rhs.fNamespaceURI.has_value ()) {
            if (int cmp = Common::ThreeWayCompare (*lhs.fNamespaceURI, *rhs.fNamespaceURI)) {
                return cmp;
            }
        }
        if (int cmp = Common::ThreeWayCompare (lhs.fLocalName, rhs.fLocalName)) {
            return cmp;
        }
        return Common::ThreeWayCompare (lhs.fType, rhs.fType);
    }

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

}

#endif /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_*/
