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
                RequireNotNull (s);
                lock_guard<Execution::ExternallySynchronizedLock> critSec (fLock_);
                size_t  i   =   fLength_;
                size_t  rhsLen  =   ::wcslen (s);
                fData_.GrowToSize (i + rhsLen);
                fLength_ = i + rhsLen;
                memcpy (fData_.begin () + i, s, sizeof (wchar_t) * rhsLen);
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
                RequireNotNull (s);
                return operator+= (s);
            }
            inline  void  StringBuilder::push_back (Character c)
            {
                lock_guard<Execution::ExternallySynchronizedLock> critSec (fLock_);
                fData_.GrowToSize (fLength_ + 1);
                fData_[fLength_] = c.GetCharacterCode ();
                fLength_++;
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
                return String (fData_.begin (), fData_.begin () + fLength_);
            }


        }
    }
}
#endif // _Stroika_Foundation_Characters_StringBuilder_inl_
