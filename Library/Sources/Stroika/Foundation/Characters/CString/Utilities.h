/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CString_Utilities_h_
#define _Stroika_Foundation_Characters_CString_Utilities_h_ 1

#include "../../StroikaPreComp.h"

#include <cstdarg>
#include <cstring>
#include <string>

#include "../../Configuration/Common.h"

/**
 *  \file
 *
 *  TODO:
 *      @todo   Consider losing StringToFloat() or making it look/act like String/Format version.
 *
 *      @todo   Many of these CString utilities should be removed? Review each one and its use. See if
 *              Stroika-string is just better? Maybe leave - but review.
 *
 *      @todo   DOCUMENT BEHAVIOR OF STRING2INT() for bad strings. What does it do?
 *              AND SIMILARPT FOR hexString2Int. And for btoh – probably rewrite to use strtoul/strtol etc
 *
 *      @todo   Same changes to HexString2Int() as we did with String2Int()
 *
 *      @todo   Consdier if we should have variants of these funtions taking a locale, or
 *              always using C/currnet locale. For the most part - I find it best to use the C locale.
 *              But DOCUMENT in all cases!!! And maybe implement variants...
 */

namespace Stroika {
    namespace Foundation {
        namespace Characters {

            /*
             * Legacy "C_String" string utilities.
             */
            namespace CString {

                using std::basic_string;
                using std::string;
                using std::wstring;

                /**
                 *  \brief  Measure the length of the argument c-string (NUL-terminated string).
                 *
                 * Measure the length of the argument c-string (NUL-terminated string). Overloaded version of strlen/wcslen.
                 *
                 * Only implemented for char/wchar_t. Reason for this is so code using old-style C++ strings can leverage overloading!
                 */
                template <typename T>
                size_t Length (const T* p);
                template <>
                size_t Length (const char* p);
                template <>
                size_t Length (const char16_t* p);
                template <>
                size_t Length (const char32_t* p);
                template <>
                size_t Length (const wchar_t* p);

                /**
                 *  \brief  strcmp or wsccmp() as appropriate == 0
                 */
                template <typename T>
                bool Equals (const T* lhs, const T* rhs);
                template <>
                bool Equals (const char* lhs, const char* rhs);
                template <>
                bool Equals (const char16_t* lhs, const char16_t* rhs);
                template <>
                bool Equals (const char32_t* lhs, const char32_t* rhs);
                template <>
                bool Equals (const wchar_t* lhs, const wchar_t* rhs);

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
                 *
                 *  \par Example Usage
                 *      \code
                 *      char    urlBuf[1024];
                 *      CString::Copy (urlBuf, NEltsOf (urlBuf), url.c_str ());
                 *      \endcode
                 */
                template <typename T>
                void Copy (T* dest, size_t nEltsInDest, const T* src);
                template <>
                void Copy (char* dest, size_t nEltsInDest, const char* src);
                template <>
                void Copy (wchar_t* dest, size_t nEltsInDest, const wchar_t* src);

                /**
                 *  \brief  Safe variant of strncat() - which always NUL-terminates the string. DIFFERNT arguments however, so not safe direct substitution.
                 *
                 * Note - the arguments are not in exactly the same order as strncpy() - with the length of the buffer for the first string before
                 * the second string.
                 */
                template <typename T>
                void Cat (T* dest, size_t nEltsInDest, const T* src2Append);
                template <>
                void Cat (char* dest, size_t nEltsInDest, const char* src2Append);
                template <>
                void Cat (wchar_t* dest, size_t nEltsInDest, const wchar_t* src2Append);

                /*
                 * Format is the Stroika wrapper on sprintf().
                 * The main differences between sprintf () and Format are:
                 *      (1)     No need for buffer management. ANY sized buffer will be automatically allocated internally and returned as a
                 *              string (eventually probably using Stroika::String - but for now std::string).
                 *
                 *      (2)     This version of format has a SUBTLE - but important difference from std::c++ / sprintf() in the interpretation of
                 *              %s format strings in Format(const wchar_t*).  The interpretation of Format (const char*,...) is the same
                 *              but for Format (const wchar_t* format, ...) - %s is assumed to match a const wchar_t* string in the variable
                 *              argument list.
                 *
                 *              This deviation from the c++ standard (technically not a deviation because Stroika::Foundation::Characters::Format() isn't
                 *              covered by the stdc++ ;-)) - is because of two reasons:
                 *                  a)  Microsoft has this interpretation.
                 *                  b)  Its a BETTER interpretation (allowing for stuff like (Format (SDKSTR("time%s"), count==1? SDKSTR (""): SDKSTR ("s"));
                 */
                string  FormatV (const char* format, va_list argsList);
                wstring FormatV (const wchar_t* format, va_list argsList);
                string  Format (const char* format, ...);
                wstring Format (const wchar_t* format, ...);

                /**
                 *  @see String::LimitLength()
                 */
                string  LimitLength (const string& str, size_t maxLen, bool keepLeft = true);
                wstring LimitLength (const wstring& str, size_t maxLen, bool keepLeft = true);

                /**
                 *  @see String::LTrim()
                 *  @see String::RTrim()
                 *  @see String::Trim()
                 */
                template <typename TCHAR>
                basic_string<TCHAR> LTrim (const basic_string<TCHAR>& text);
                template <typename TCHAR>
                basic_string<TCHAR> RTrim (const basic_string<TCHAR>& text);
                template <typename TCHAR>
                basic_string<TCHAR> Trim (const basic_string<TCHAR>& text);

                /**
                 *  This could be generalized to accomodate TRIM/merge with TRIM, but it sometimes used to trim other
                 *  characters (like trailing CRLF, or trailing '.').
                 */
                string  StripTrailingCharIfAny (const string& s, char c);
                wstring StripTrailingCharIfAny (const wstring& s, wchar_t c);

                /**
                 *  Convert the given hex-format string to an unsigned integer.
                 *  String2Int will return 0 if no valid parse, and UINT_MAX on overflow.
                 *
                 *  @see strtoul(), or @see wcstol (). This is a simple wrapper on strtoul() or wcstoul().
                 *  strtoul() etc are more flexible. This is merely meant to be an often convenient wrapper.
                 *  Use strtoul etc directly to see if the string parsed properly.
                 */
                unsigned int HexString2Int (const string& s);
                unsigned int HexString2Int (const wchar_t* s);
                unsigned int HexString2Int (const wstring& s);

                /**
                 *  Convert the given decimal-format integral string to any integer type
                 *  ( e.g. signed char, unsigned short int, long long int, uint32_t etc).
                 *
                 *  String2Int will return 0 if no valid parse, and numeric_limits<T>::min on underflow,
                 *  numeric_limits<T>::max on overflow.
                 *      @todo consider if this is unwise - it seems we ought to throw? Or have a variant
                 *              perhaps that does no throw?
                 *
                 *              CONSIDER!
                 *
                 *  @see strtoll(), or @see wcstoll (). This is a simple wrapper on strtoll() / wcstoll ().
                 *  strtoll() is more flexible. This is merely meant to be an often convenient wrapper.
                 *  Use strtoll etc directly to see if the string parsed properly.
                 */
                template <typename T>
                T String2Int (const string& s);
                template <typename T>
                T String2Int (const wchar_t* s);
                template <typename T>
                T String2Int (const wstring& s);

                /**
                 *  Convert the given decimal-format floating point string to an double.
                 *  String2Float will return nan () if no valid parse.
                 *
                 *  @see strtod(), or @see wcstod (). This is a simple wrapper on strtod() / wcstod ()
                 *  except that it returns nan() on invalid data, instead of zero.
                 *
                 *  strtod() /etc are more flexible. This is merely meant to be an often convenient wrapper.
                 *  Use strtod etc directly to see if the string parsed properly.
                 */
                double String2Float (const string& s);
                double String2Float (const wchar_t* s);
                double String2Float (const wstring& s);

                /**
                 *  Note - this routine ignores the current locale settings, and always uses the 'C' locale.
                 *
                 *  @see Characters::Float2String
                 */
                wstring Float2String (double f);

                /*
                 @CONFIGVAR:     qPlatformSupports_snprintf
                 @DESCRIPTION:   <p>Defines if the compiler stdC++ library supports the std::snprintf() function</p>
                */
#ifndef qPlatformSupports_snprintf
#error "qPlatformSupports_snprintf should normally be defined indirectly by StroikaConfig.h"
#endif

                /*
                @CONFIGVAR:     qPlatformSupports_wcscasecmp
                @DESCRIPTION:   <p>Defines if the compiler supports the wcscasecmp function/p>
                */
#ifndef qPlatformSupports_wcscasecmp
#error "qPlatformSupports_wcscasecmp should normally be defined indirectly by StroikaConfig.h"
#endif

                /*
                @CONFIGVAR:     qPlatformSupports_wcsncasecmp
                @DESCRIPTION:   <p>Defines if the compiler supports the wcscasecmp function/p>
                */
#ifndef qPlatformSupports_wcsncasecmp
#error "qPlatformSupports_wcsncasecmp should normally be defined indirectly by StroikaConfig.h"
#endif

#if !qPlatformSupports_snprintf
#if _MSC_VER
//#define snprintf _snprintf
#define snprintf _snprintf_s
#else
#error "NYI"
#endif
#endif

#if !qPlatformSupports_wcscasecmp
#if _MSC_VER
#define wcscasecmp _wcsicmp
#else
#error "NYI"
#endif
#endif

#if !qPlatformSupports_wcsncasecmp
#if _MSC_VER
#define wcsncasecmp _wcsnicmp
#else
#error "NYI"
#endif
#endif
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Utilities.inl"

#endif /*_Stroika_Foundation_Characters_CString_Utilities_h_*/
