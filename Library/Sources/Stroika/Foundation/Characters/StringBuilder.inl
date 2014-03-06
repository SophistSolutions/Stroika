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
                , fLength_ (0)
            {
            }
            inline StringBuilder::StringBuilder (const String& initialValue)
                : fData_ (0)
                , fLength_ (0)
            {
                operator+= (initialValue);
            }
            inline  StringBuilder&  StringBuilder::operator+= (const wchar_t* s)
            {
                lock_guard<Execution::ExternallySynchronizedLock> critSec (fLock_);
                size_t  i   =   fLength_;
                size_t  l   =   ::wcslen (s);
                fData_.GrowToSize (i + l);
                fLength_ = i + i;
                memcpy (fData_.begin () + i, s, sizeof (wchar_t) * l);
                return *this;
            }
            inline  StringBuilder&  StringBuilder::operator+= (const String& s)
            {
                return operator+= (s.c_str ());
            }
            inline  StringBuilder&  StringBuilder::operator<< (const String& s)
            {
                return operator+= (s.c_str ());
            }
            inline  StringBuilder&  StringBuilder::operator<< (const wchar_t* s)
            {
                return operator+= (s);
            }
            inline  StringBuilder::operator String () const
            {
                return str ();
            }
            inline  const wchar_t*  StringBuilder::c_str () const
            {
                lock_guard<Execution::ExternallySynchronizedLock> critSec (fLock_);
                fData_.GrowToSize (fLength_ + 1);
                fData_[fLength_] = '\0';
                return fData_.begin ();
            }
            inline  String StringBuilder::str () const
            {
                lock_guard<Execution::ExternallySynchronizedLock> critSec (fLock_);
                size_t  l   =   fLength_;
                return String (fData_.begin (), fData_.begin () + l);
            }


        }
    }
}
#endif // _Stroika_Foundation_Characters_StringBuilder_inl_
