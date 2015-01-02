/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CString_Utilities_inl_
#define _Stroika_Foundation_Characters_CString_Utilities_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <limits>
#include    <locale>

#include    "../../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   CString {


                /*
                 ********************************************************************************
                 *************************************** Length *********************************
                 ********************************************************************************
                 */
                template    <>
                inline  size_t  Length (const char* p)
                {
                    RequireNotNull (p);
                    return ::strlen (p);
                }
                template    <>
                inline  size_t  Length (const wchar_t* p)
                {
                    RequireNotNull (p);
                    return ::wcslen (p);
                }


                /*
                 ********************************************************************************
                 *************************************** Copy ***********************************
                 ********************************************************************************
                 */
                template    <typename T>
                void    Copy (T* dest, size_t nEltsInDest, const T* src)
                {
                    // Only provide template specializations - but this variant so we get syntax error compiling instead of
                    // link error if someone calls with other types...
                    void* fail;
                    dest = fail;
                }
                template    <>
                inline  void    Copy (char* dest, size_t nEltsInDest, const char* src)
                {
                    RequireNotNull (dest);
                    RequireNotNull (src);
                    Require (nEltsInDest >= 1);
                    char*       destEnd =   dest + nEltsInDest;
                    char*       di      =   dest;
                    const char* si      =   src;
                    while ((*di++ = *si++) != '\0') {
                        Assert (di <= destEnd);
                        if (di == destEnd) {
                            *(di - 1) = '\0';
                            break;
                        }
                    }
                    Ensure (Length (dest) < nEltsInDest);
                }
                template    <>
                inline  void    Copy (wchar_t* dest, size_t nEltsInDest, const wchar_t* src)
                {
                    RequireNotNull (dest);
                    RequireNotNull (src);
                    Require (nEltsInDest >= 1);
                    wchar_t*        destEnd =   dest + nEltsInDest;
                    wchar_t*        di      =   dest;
                    const wchar_t*  si      =   src;
                    while ((*di++ = *si++) != '\0') {
                        Assert (di <= destEnd);
                        if (di == destEnd) {
                            *(di - 1) = '\0';
                            break;
                        }
                    }
                    Ensure (Length (dest) < nEltsInDest);
                }


                /*
                 ********************************************************************************
                 *************************************** Cat ************************************
                 ********************************************************************************
                 */
                template    <typename T>
                void    Cat (T* dest, size_t nEltsInDest, const T* src)
                {
                    // Only provide template specializations - but this variant so we get syntax error compiling instead of
                    // link error if someone calls with other types...
                    void* fail;
                    dest = fail;
                }
                template    <>
                inline  void    Cat (char* dest, size_t nEltsInDest, const char* src)
                {
                    RequireNotNull (dest);
                    RequireNotNull (src);
                    Require (nEltsInDest >= 1);
                    char*       destEnd =   dest + nEltsInDest;
                    char*       di      =   dest + strlen (dest);
                    const char* si      =   src;
                    while ((*di++ = *si++) != '\0') {
                        Assert (di <= destEnd);
                        if (di == destEnd) {
                            *(di - 1) = '\0';
                            break;
                        }
                    }
                    Ensure (Length (dest) < nEltsInDest);
                }
                template    <>
                inline  void    Cat (wchar_t* dest, size_t nEltsInDest, const wchar_t* src)
                {
                    RequireNotNull (dest);
                    RequireNotNull (src);
                    Require (nEltsInDest >= 1);
                    wchar_t*        destEnd =   dest + nEltsInDest;
                    wchar_t*        di      =   dest + ::wcslen (dest);
                    const wchar_t*  si      =   src;
                    while ((*di++ = *si++) != '\0') {
                        Assert (di <= destEnd);
                        if (di == destEnd) {
                            *(di - 1) = '\0';
                            break;
                        }
                    }
                    Ensure (Length (dest) < nEltsInDest);
                }


                /*
                 ********************************************************************************
                 ************************** LTrim/RTrim/Trim ************************************
                 ********************************************************************************
                 */
                template    <typename TCHAR>
                basic_string<TCHAR> LTrim (const basic_string<TCHAR>& text)
                {
                    std::locale loc1;   // default locale
                    const ctype<TCHAR>& ct = use_facet<ctype<TCHAR>>(loc1);
                    typename basic_string<TCHAR>::const_iterator i = text.begin ();
                    for (; i != text.end () and ct.is (ctype<TCHAR>::space, *i); ++i)
                        ;
                    return basic_string<TCHAR> (i, text.end ());
                }
                template    <typename TCHAR>
                basic_string<TCHAR> RTrim (const basic_string<TCHAR>& text)
                {
                    std::locale loc1;   // default locale
                    const ctype<TCHAR>& ct = use_facet<ctype<TCHAR>>(loc1);
                    typename basic_string<TCHAR>::const_iterator i = text.end ();
                    for (; i != text.begin () and ct.is (ctype<TCHAR>::space, *(i - 1)); --i)
                        ;
                    return basic_string<TCHAR> (text.begin (), i);
                }
                template    <typename TCHAR>
                inline  basic_string<TCHAR> Trim (const basic_string<TCHAR>& text)
                {
                    return LTrim (RTrim (text));
                }


                /*
                 ********************************************************************************
                 ******************************* String2Int *************************************
                 ********************************************************************************
                 */
                namespace Private_ {
                    long long int           String2Int_ (const string& s);
                    long long int           String2Int_ (const char* s);
                    long long int           String2Int_ (const wchar_t* s);
                    long long int           String2Int_ (const wstring& s);
                    unsigned long long int  String2UInt_ (const string& s);
                    unsigned long long int  String2UInt_ (const char* s);
                    unsigned long long int  String2UInt_ (const wchar_t* s);
                    unsigned long long int  String2UInt_ (const wstring& s);
                    inline  long long int     String2Int_ (const char* s)
                    {
                        return String2Int_ (string (s));
                    }
                    inline  long long int     String2Int_ (const wchar_t* s)
                    {
                        return String2Int_ (wstring (s));
                    }
                    inline  unsigned long long int     String2UInt_ (const char* s)
                    {
                        return String2UInt_ (string (s));
                    }
                    inline  unsigned long long int     String2UInt_ (const wchar_t* s)
                    {
                        return String2UInt_ (wstring (s));
                    }
                    DISABLE_COMPILER_MSC_WARNING_START(4018)
                    template    <typename T, typename STRING_ARG>
                    T     String2IntOrUInt_ (STRING_ARG s)
                    {
                        using   std::numeric_limits;
                        if (numeric_limits<T>::is_signed) {
                            long long int    l   =   String2Int_ (s);
                            if (l <= numeric_limits<T>::min ()) {
                                return numeric_limits<T>::min ();
                            }
                            if (l >= numeric_limits<T>::max ()) {
                                return numeric_limits<T>::max ();
                            }
                            return static_cast<T> (l);
                        }
                        else {
                            unsigned long long int    l   =   String2UInt_ (s);
                            if (l >= numeric_limits<T>::max ()) {
                                return numeric_limits<T>::max ();
                            }
                            return static_cast<T> (l);
                        }
                    }
                }
                DISABLE_COMPILER_MSC_WARNING_END(4018)

                template    <typename T>
                inline  T     String2Int (const string& s)
                {
                    return Private_::String2IntOrUInt_<T, const string&> (s);
                }
                template    <typename T>
                inline  T     String2Int (const wchar_t* s)
                {
                    return Private_::String2IntOrUInt_<T, const wchar_t*> (s);
                }
                template    <typename T>
                inline  T     String2Int (const wstring& s)
                {
                    return Private_::String2IntOrUInt_<T, const wstring&> (s);
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Characters_CString_Utilities_inl_*/
