/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CString_Utilities_inl_
#define _Stroika_Foundation_Characters_CString_Utilities_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {
            namespace   CString {

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

            }
        }
    }
}
#endif  /*_Stroika_Foundation_Characters_CString_Utilities_inl_*/
