/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_StringUtils_inl_
#define _Stroika_Foundation_Characters_StringUtils_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Containers/Common.h"
#include    "../Memory/SmallStackBuffer.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {



#if     !qPlatformSupports_snprintf
#if _MSC_VER
            //#define snprintf _snprintf
#define snprintf _snprintf_s
#else
#error "NYI"
#endif
#endif

#if     !qPlatformSupports_wcscasecmp
#if _MSC_VER
#define wcscasecmp  _wcsicmp
#else
#error "NYI"
#endif
#endif

#if     !qPlatformSupports_wcsncasecmp
#if _MSC_VER
#define wcsncasecmp _wcsnicmp
#else
#error "NYI"
#endif
#endif


            //  TOLOWER
            inline  char    TOLOWER (char c) {
#if     qUseASCIIRangeSpeedHack
                if ('A' <= c and c <= 'Z') {
                    Ensure (std::tolower (c) == ((c - 'A') + 'a'));
                    return (c - 'A') + 'a';
                }
                else if (c <= 127) {
                    Ensure (std::tolower (c) == c);
                    return c;
                }
                else {
                    return std::tolower (c);
                }
#else
                return std::tolower (c);
#endif
            }
            inline  wchar_t TOLOWER (wchar_t c) {
#if     qUseASCIIRangeSpeedHack
                if ('A' <= c and c <= 'Z') {
                    Ensure (towlower (c) == ((c - 'A') + 'a'));
                    return (c - 'A') + 'a';
                }
                else if (c <= 127) {
                    Ensure (wchar_t (towlower (c)) == c);
                    return c;
                }
                else {
                    return towlower (c);
                }
#else
                return towlower (c);
#endif
            }

            //  TOUPPER
            inline  char    TOUPPER (char c) {
#if     qUseASCIIRangeSpeedHack
                if ('a' <= c and c <= 'z') {
                    Ensure (std::toupper (c) == ((c - 'a') + 'A'));
                    return (c - 'a') + 'A';
                }
                else if (c <= 127) {
                    Ensure (std::toupper (c) == c);
                    return c;
                }
                else {
                    return std::toupper (c);
                }
#else
                return std::toupper (c);
#endif
            }
            inline  wchar_t TOUPPER (wchar_t c) {
#if     qUseASCIIRangeSpeedHack
                if ('a' <= c and c <= 'z') {
                    Ensure (towupper (c) == ((c - 'a') + 'A'));
                    return (c - 'a') + 'A';
                }
                else if (c <= 127) {
                    Ensure (wchar_t (towupper (c)) == c);
                    return c;
                }
                else {
                    return towupper (c);
                }
#else
                return towupper (c);
#endif
            }


            //  ISSPACE
            inline  bool    ISSPACE (char c) {
#if     qUseASCIIRangeSpeedHack
                if (0 < c and c <= 127) {
                    //  space, tab, carriage return, newline, vertical tab or form feed
                    bool    r   =   (c == ' ' or c == '\t' or c == '\r' or c == '\n' or c == '\v' or c == '\f');
                    Ensure (!!isspace (c) == r);
                    return r;
                }
                else {
#pragma warning (push)
#pragma warning (4 : 4800)
                    return isspace (c);
#pragma warning (pop)
                }
#else
                return isspace (c);
#endif
            }
            inline  bool    ISSPACE (wchar_t c) {
#if     qUseASCIIRangeSpeedHack
                if (0 < c and c <= 127) {
                    //  space, tab, carriage return, newline, vertical tab or form feed
                    bool    r   =   (c == ' ' or c == '\t' or c == '\r' or c == '\n' or c == '\v' or c == '\f');
                    Ensure (!!iswspace (c) == r);
                    return r;
                }
                else {
#pragma warning (push)
#pragma warning (4 : 4800)
                    return iswspace (c);
#pragma warning (pop)
                }
#else
                return iswspace (c);
#endif
            }


            //  tolower
            inline  wstring tolower (const wstring& s) {
                wstring r   =   s;
                tolower (&r);
                return r;
            }
            inline  string  tolower (const string& s) {
                string  r   =   s;
                tolower (&r);
                return r;
            }

            //  toupper
            inline  wstring toupper (const wstring& s) {
                wstring r   =   s;
                toupper (&r);
                return r;
            }
            inline  string  toupper (const string& s) {
                string  r   =   s;
                toupper (&r);
                return r;
            }







            inline  bool    StartsWith (const string& l, const string& prefix) {
                if (l.length () < prefix.length ()) {
                    return false;
                }
                return l.substr (0, prefix.length ()) == prefix;
            }
            inline  bool    StartsWith (const wchar_t* l, const wchar_t* prefix, StringCompareOptions co) {
                // This is a good candiate for inlining because the StringCompareOptions 'co' is nearly ALWAYS a constant
                RequireNotNull (l);
                RequireNotNull (prefix);
                size_t  prefixLen   =   ::wcslen (prefix);
                switch (co) {
                    case    eWithCase_CO:
                        return wcsncmp (l, prefix, prefixLen) == 0;
                    case    eCaseInsensitive_CO:
                        return wcsncasecmp (l, prefix, prefixLen) == 0;
                }
                Assert (false);
                return false; // not reached
            }
            inline  bool    StartsWith (const wstring& l, const wstring& prefix, StringCompareOptions co) {
                // This is a good candiate for inlining because the StringCompareOptions 'co' is nearly ALWAYS a constant
                size_t  prefixLen   =   prefix.length ();
                if (l.length () < prefixLen) {
                    return false;
                }
                switch (co) {
                    case    eWithCase_CO:
                        return wcsncmp (l.c_str (), prefix.c_str (), prefixLen) == 0;
                    case    eCaseInsensitive_CO:
                        return wcsncasecmp (l.c_str (), prefix.c_str (), prefixLen) == 0;
                }
                Assert (false);
                return false; // not reached
            }



            inline  bool    EndsWith (const wstring& l, const wstring& suffix, StringCompareOptions co) {
                // This is a good candiate for inlining because the StringCompareOptions 'co' is nearly ALWAYS a constant
                size_t  lLen        =   l.length ();
                size_t  suffixLen   =   suffix.length ();
                if (lLen < suffixLen) {
                    return false;
                }
                size_t  i   =   lLen - suffixLen;
                switch (co) {
                    case    eWithCase_CO:
                        return wcsncmp (l.c_str () + i, suffix.c_str (), suffixLen) == 0;
                    case    eCaseInsensitive_CO:
                        return wcsncasecmp (l.c_str () + i, suffix.c_str (), suffixLen) == 0;
                }
                Assert (false);
                return false; // not reached
            }




            // class    CaseInsensativeLess
            inline  bool CaseInsensativeLess::operator()(const wstring& _Left, const wstring& _Right) const {
                return wcscasecmp (_Left.c_str (), _Right.c_str ()) < 0;
            }




            inline  bool    StringsCILess (const wstring& l, const wstring& r) {
                return ::wcscasecmp (l.c_str (), r.c_str ()) < 0;
            }
            inline  bool    StringsCIEqual (const wstring& l, const wstring& r) {
                return ::wcscasecmp (l.c_str (), r.c_str ()) == 0;
            }
            inline  int     StringsCICmp (const wstring& l, const wstring& r) {
                return ::wcscasecmp (l.c_str (), r.c_str ());
            }




            template    <typename TCHAR>
            basic_string<TCHAR> LTrim (const basic_string<TCHAR>& text) {
                std::locale loc1;   // default locale
                const ctype<TCHAR>& ct = use_facet<ctype<TCHAR> >(loc1);
                typename basic_string<TCHAR>::const_iterator i = text.begin ();
                for (; i != text.end () and ct.is (ctype<TCHAR>::space, *i); ++i)
                    ;
                return basic_string<TCHAR> (i, text.end ());
            }
            template    <typename TCHAR>
            basic_string<TCHAR> RTrim (const basic_string<TCHAR>& text) {
                std::locale loc1;   // default locale
                const ctype<TCHAR>& ct = use_facet<ctype<TCHAR> >(loc1);
                typename basic_string<TCHAR>::const_iterator i = text.end ();
                for (; i != text.begin () and ct.is (ctype<TCHAR>::space, *(i - 1)); --i)
                    ;
                return basic_string<TCHAR> (text.begin (), i);
            }
            template    <typename TCHAR>
            inline  basic_string<TCHAR> Trim (const basic_string<TCHAR>& text) {
                return LTrim (RTrim (text));
            }



            template    <>
            inline  size_t  Length (const char* p) {
                RequireNotNull (p);
                return ::strlen (p);
            }
            template    <>
            inline  size_t  Length (const wchar_t* p) {
                RequireNotNull (p);
                return ::wcslen (p);
            }
        }
    }
}
#endif  /*_Stroika_Foundation_Characters_StringUtils_inl_*/
