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


                template    <>
                inline  void    Copy (char* dest, const char* src, size_t nEltsInDest)
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
                inline  void    Copy (wchar_t* dest, const wchar_t* src, size_t nEltsInDest)
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

            }
        }
    }
}
#endif  /*_Stroika_Foundation_Characters_CString_Utilities_inl_*/
