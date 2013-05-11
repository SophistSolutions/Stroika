/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CString_Utilities_h_
#define _Stroika_Foundation_Characters_CString_Utilities_h_   1

#include    "../../StroikaPreComp.h"

#include    <cstring>
#include    <string>

#include    "../../Configuration/Common.h"


/**
 *  \file
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /*
             * Legacy "C_String" string utilities.
             */
            namespace   CString {


                using   std::string;
                using   std::wstring;
                using   std::basic_string;


                /**
                 *  \brief  Measure the length of the argument c-string (NUL-terminated string).
                 *
                 * Measure the length of the argument c-string (NUL-terminated string). Overloaded version of strlen/wcslen.
                 *
                 * Only implemented for char/wchar_t. Reason for this is so code using old-style C++ strings can leverage overloading!
                 */
                template    <typename T>
                size_t  Length (const T* p);
                template    <>
                size_t  Length (const char* p);
                template    <>
                size_t  Length (const wchar_t* p);


                /**
                 *  \brief  Safe variant of strncpy() - which always NUL-terminates the string
                 *
                 * Copy the C-string pointed to by 'src' to the location pointed to by 'dest'. 'nEltsInDest' -
                 * as the name suggests - is the number of elements in the array pointed to by 'dest' - not
                 * the max string length for 'dest.
                 *
                 * This function will always nul-terminate, and it is illegal to call it with a value
                 * of zero for nEltsInDest.
                 *
                 * This is basically like strcpy/strncpy, except with better logic for nul-termination.
                 *
                 * Note - the arguments are not in exactly the same order as strncpy() - with the length of the buffer for the first string before
                 * the second string.
                 */
                template    <typename T>
                void    Copy (T* dest, size_t nEltsInDest, const T* src);
                template    <>
                void    Copy (char* dest, size_t nEltsInDest, const char* src);
                template    <>
                void    Copy (wchar_t* dest, size_t nEltsInDest, const wchar_t* src);


                /**
                 *  \brief  Safe variant of strncat() - which always NUL-terminates the string. DIFFERNT arguments however, so not safe direct substitution.
                 *
                 * Note - the arguments are not in exactly the same order as strncpy() - with the length of the buffer for the first string before
                 * the second string.
                 */
                template    <typename T>
                void    Cat (T* dest, size_t nEltsInDest, const T* src2Append);
                template    <>
                void    Cat (char* dest, size_t nEltsInDest, const char* src2Append);
                template    <>
                void    Cat (wchar_t* dest, size_t nEltsInDest, const wchar_t* src2Append);


            }
        }
    }
}





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Utilities.inl"

#endif  /*_Stroika_Foundation_Characters_CString_Utilities_h_*/
