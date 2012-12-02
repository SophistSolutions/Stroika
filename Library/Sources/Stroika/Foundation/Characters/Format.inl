/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
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
                const ctype<TCHAR>& ct = use_facet<ctype<TCHAR> >(loc1);
                typename basic_string<TCHAR>::const_iterator i = text.begin ();
                for (; i != text.end () and ct.is (ctype<TCHAR>::space, *i); ++i)
                    ;
                return basic_string<TCHAR> (i, text.end ());
            }
            template    <typename TCHAR>
            basic_string<TCHAR> RTrim (const basic_string<TCHAR>& text)
            {
                std::locale loc1;   // default locale
                const ctype<TCHAR>& ct = use_facet<ctype<TCHAR> >(loc1);
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



        }
    }
}
#endif  /*_Stroika_Foundation_Characters_Format_inl_*/
