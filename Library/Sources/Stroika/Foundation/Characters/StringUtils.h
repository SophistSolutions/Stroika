/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_StringUtils_h_
#define _Stroika_Foundation_Characters_StringUtils_h_   1

#include    "../StroikaPreComp.h"

#include    <cctype>
#include    <locale>
#include    <string>
#include    <vector>


#include    "../Configuration/Common.h"



/// THIS MODULE SB OBSOLETE ONCE WE GET STROIKA CONTAINERS WORKING


/*
 * TODO:
 *      >> Probably move LineEdnings related stutff to Characters/LineEndings module
 */




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






namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {

            using   std::string;
            using   std::wstring;
            using   std::basic_string;


//See what this is used for? Seems too specific... See String::Trim()
            string  StripTrailingCharIfAny (const string& s, char c);
            wstring StripTrailingCharIfAny (const wstring& s, wchar_t c);




            // Speed-tweek versions of these ANSI-standard routines. NB: These assume a congtiguous range encoding of upper/lower characters,
            // as with ASCII, and UNICODE character encodings
            //
            // NB: we should revisit if this speedtweek is a good idea with each new compiler release, since in principal, a good compiler
            // and runtime lib would do better than what we've coded.
            //
            //          -- LGP 2006-01-17
            //
#ifndef qUseASCIIRangeSpeedHack
#define qUseASCIIRangeSpeedHack     1
#endif

            // use qUseASCIIRangeSpeedHack - if turned on...
            char    TOLOWER (char c);
            wchar_t TOLOWER (wchar_t c);
            char    TOUPPER (char c);
            wchar_t TOUPPER (wchar_t c);
            bool    ISSPACE (char c);
            bool    ISSPACE (wchar_t c);


            string  tolower (const string& s);
            void    tolower (string* s);
            wstring tolower (const wstring& s);
            void    tolower (wstring* s);

            string  toupper (const string& s);
            void    toupper (string* s);
            wstring toupper (const wstring& s);
            void    toupper (wstring* s);

            // for now, only do wstring version since for narrow-strings, case compare is ambiguous (need to specify codeset)
            bool    StringsCIEqual (const wstring& l, const wstring& r);
            bool    StringsCILess (const wstring& l, const wstring& r);
            // like strcmp() < 0 impless less, = 0 implies equal, and > 0 implies >
            int     StringsCICmp (const wstring& l, const wstring& r);

            enum    StringCompareOptions {
                eWithCase_CO,
                eCaseInsensitive_CO,
            };
            bool    StartsWith (const string& l, const string& prefix);
            bool    StartsWith (const wchar_t* l, const wchar_t* prefix, StringCompareOptions co = eWithCase_CO);
            bool    StartsWith (const wstring& l, const wstring& prefix, StringCompareOptions co = eWithCase_CO);
            bool    EndsWith (const wstring& l, const wstring& suffix, StringCompareOptions co = eWithCase_CO);
            bool    Contains (const wstring& string2Search, const wstring& substr, StringCompareOptions co = eWithCase_CO);

            struct  CaseInsensativeLess : public binary_function<wstring, wstring, bool> {
                bool operator()(const wstring& _Left, const wstring& _Right) const;
            };

#if 0
            // Only implemented for char/wchar_t. Reason for this is so code using old-style C++ strings can leverage overloading!
            template    <typename T>
            size_t  Length (const T* p);
#endif


            string  LimitLength (const string& str, size_t maxLen, bool keepLeft = true);
            wstring LimitLength (const wstring& str, size_t maxLen, bool keepLeft = true);


            template    <typename TCHAR>
            basic_string<TCHAR> LTrim (const basic_string<TCHAR>& text);
            template    <typename TCHAR>
            basic_string<TCHAR> RTrim (const basic_string<TCHAR>& text);
            template    <typename TCHAR>
            basic_string<TCHAR> Trim (const basic_string<TCHAR>& text);




            namespace   StringVectorEncoding {
                /*
                 *  Don't write this to a file, as the format of the encoding is subject to change. This is a
                 *  work-in-progress, and just intended to solve - for now - an immediate need (list of ids passed through javascript)
                 *
                 *      -- LGP 2006-04-15
                 */
                wstring         Encode (const vector<wstring>& v);
                vector<wstring> Decode (const wstring& encodedString);
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Characters_StringUtils_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "StringUtils.inl"

