/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace DataExchange {
            namespace StructuredStreamEvents {

                /*
                 ********************************************************************************
                 *********************** StructuredStreamEvents::Name ***************************
                 ********************************************************************************
                 */
                inline Name::Name (NameType type)
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
                 ******************** StructuredStreamEvents::Name operators ********************
                 ********************************************************************************
                 */
                inline bool operator< (const Name& lhs, const Name& rhs)
                {
                    if (lhs.fNamespaceURI.has_value () and rhs.fNamespaceURI.has_value ()) {
                        int cmp = lhs.fNamespaceURI->Compare (*rhs.fNamespaceURI);
                        if (cmp < 0) {
                            return true;
                        }
                        else if (cmp > 0) {
                            return false;
                        }
                    }
                    {
                        int cmp = lhs.fLocalName.Compare (rhs.fLocalName);
                        if (cmp < 0) {
                            return true;
                        }
                        else if (cmp > 0) {
                            return false;
                        }
                    }
                    if (lhs.fType < rhs.fType) {
                        return true;
                    }
                    return false;
                }
                inline bool operator<= (const Name& lhs, const Name& rhs)
                {
                    if (lhs.fNamespaceURI.has_value () and rhs.fNamespaceURI.has_value ()) {
                        int cmp = lhs.fNamespaceURI->Compare (*rhs.fNamespaceURI);
                        if (cmp < 0) {
                            return true;
                        }
                        else if (cmp > 0) {
                            return false;
                        }
                    }
                    {
                        int cmp = lhs.fLocalName.Compare (rhs.fLocalName);
                        if (cmp < 0) {
                            return true;
                        }
                        else if (cmp > 0) {
                            return false;
                        }
                    }
                    if (lhs.fType <= rhs.fType) {
                        return true;
                    }
                    return false;
                }
                inline bool operator== (const Name& lhs, const Name& rhs)
                {
                    if (lhs.fNamespaceURI.has_value () and rhs.fNamespaceURI.has_value () and lhs.fNamespaceURI != rhs.fNamespaceURI) {
                        return false;
                    }
                    if (lhs.fLocalName != rhs.fLocalName) {
                        return false;
                    }
                    if (lhs.fType != rhs.fType) {
                        return false;
                    }
                    return true;
                }
                inline bool operator!= (const Name& lhs, const Name& rhs)
                {
                    return not(lhs == rhs);
                }
                inline bool operator>= (const Name& lhs, const Name& rhs)
                {
                    return not(lhs < rhs);
                }
                inline bool operator> (const Name& lhs, const Name& rhs)
                {
                    return not(lhs <= rhs);
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_Name_inl_*/
