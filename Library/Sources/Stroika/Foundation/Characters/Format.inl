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


            inline  long long int     String2Int (const char* s)
            {
                return String2Int (string (s));
            }
            inline  long long int     String2Int (const wchar_t* s)
            {
                return String2Int (String (s));
            }
            inline  long long int     String2Int (const wstring& s)
            {
                return String2Int (String (s));
            }
            template    <typename T>
            inline  T     String2Integer (const string& s)
            {
                using   std::numeric_limits;
                long long int    l   =   String2Int (s);
                if (l <= numeric_limits<T>::min ()) {
                    return numeric_limits<T>::min ();
                }
                if (l >= numeric_limits<T>::max ()) {
                    return numeric_limits<T>::max ();
                }
                return static_cast<T> (l);
            }
            template    <typename T>
            inline  T     String2Integer (const wchar_t* s)
            {
                using   std::numeric_limits;
                long long int    l   =   String2Int (s);
                if (l <= numeric_limits<T>::min ()) {
                    return numeric_limits<T>::min ();
                }
                if (l >= numeric_limits<T>::max ()) {
                    return numeric_limits<T>::max ();
                }
                return static_cast<T> (l);
            }
            template    <typename T>
            inline  T     String2Integer (const wstring& s)
            {
                using   std::numeric_limits;
                long long int    l   =   String2Int (s);
                if (l <= numeric_limits<T>::min ()) {
                    return numeric_limits<T>::min ();
                }
                if (l >= numeric_limits<T>::max ()) {
                    return numeric_limits<T>::max ();
                }
                return static_cast<T> (l);
            }
            template    <typename T>
            inline  T     String2Integer (const String& s)
            {
                using   std::numeric_limits;
                long long int    l   =   String2Int (s);
                if (l <= numeric_limits<T>::min ()) {
                    return numeric_limits<T>::min ();
                }
                if (l >= numeric_limits<T>::max ()) {
                    return numeric_limits<T>::max ();
                }
                return static_cast<T> (l);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Characters_Format_inl_*/
