/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_StringBuilder_inl_
#define _Stroika_Foundation_Characters_StringBuilder_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <memory>
#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
            inline  StringBuilder::StringBuilder ()
                : fData_ (0)
            {
            }
            inline StringBuilder::StringBuilder (const String& initialValue)
                : fData_ (0)
            {
                operator+= (initialValue);
            }
            inline  StringBuilder&  StringBuilder::operator+= (const wchar_t* s)
            {
                size_t  i   =   fData_.GetSize ();
                size_t  l   =   ::wcslen (s);
                fData_.GrowToSize (i + l);
                memcpy (fData_.begin () + i, s, sizeof (wchar_t) * l);
            }
            inline  StringBuilder&  StringBuilder::operator+= (const String& s)
            {
                return operator+= (s.c_str ());
            }
            inline  StringBuilder::operator String () const
            {
                return str ();
            }
            inline  String StringBuilder::str () const
            {
                size_t  l   =   fData_.GetSize ();
                return String (fData_.begin (), fData_.begin () + l);
            }


        }
    }
}
#endif // _Stroika_Foundation_Characters_StringBuilder_inl_
