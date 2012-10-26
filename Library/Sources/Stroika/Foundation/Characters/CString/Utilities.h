/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CString_Utilities_h_
#define _Stroika_Foundation_Characters_CString_Utilities_h_   1

#include    "../../StroikaPreComp.h"

#include    <string>


#include    "../../Configuration/Common.h"




/*
 * TODO:
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
                 * Copy the C-string pointed to by 'src' to the location pointed to by 'dest'. This function will always
                 * nul-terminate, and it is illegal to call it with a value of zero for nEltsInDest.
                 *
                 * This is basically like strcpy/strncpy, except with better logic for nul-termination.
                 */
                template    <typename T>
                void    Copy (T* dest, const T* src, size_t nEltsInDest);
                template    <>
                void    Copy (char* dest, const char* src, size_t nEltsInDest);
                template    <>
                void    Copy (wchar_t* dest, const wchar_t* src, size_t nEltsInDest);

	        }
        }
    }
}
#endif  /*_Stroika_Foundation_Characters_CString_Utilities_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Utilities.inl"

