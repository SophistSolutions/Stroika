/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Format_inl_
#define _Stroika_Foundation_Characters_Format_inl_  1


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


            namespace Private_ {
                long long int           String2Int_ (const string& s);
                long long int           String2Int_ (const String& s);
                long long int           String2Int_ (const char* s);
                long long int           String2Int_ (const wchar_t* s);
                long long int           String2Int_ (const wstring& s);
                unsigned long long int  String2UInt_ (const string& s);
                unsigned long long int  String2UInt_ (const String& s);
                unsigned long long int  String2UInt_ (const char* s);
                unsigned long long int  String2UInt_ (const wchar_t* s);
                unsigned long long int  String2UInt_ (const wstring& s);

                inline  long long int     String2Int_ (const char* s)
                {
                    return String2Int_ (string (s));
                }
                inline  long long int     String2Int (const wchar_t* s)
                {
                    return String2Int_ (String (s));
                }
                inline  long long int     String2Int (const wstring& s)
                {
                    return String2Int_ (String (s));
                }


                inline  unsigned long long int     String2UInt (const char* s)
                {
                    return String2UInt_ (string (s));
                }
                inline  unsigned long long int     String2UInt (const wchar_t* s)
                {
                    return String2UInt_ (String (s));
                }
                inline  unsigned long long int     String2UInt (const wstring& s)
                {
                    return String2UInt_ (String (s));
                }

                template    <typename T, typename STRING_ARG>
                unsigned long long int     String2IntOrUInt_ (STRING_ARG s)
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
            template    <typename T>
            inline  T     String2Int (const String& s)
            {
                return Private_::String2IntOrUInt_<T, const String&> (s);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Characters_Format_inl_*/
