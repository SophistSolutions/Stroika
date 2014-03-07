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
            inline  void    StringBuilder::Append (const wchar_t* s, const wchar_t* e)
            {
                Require (s == e or (s != nullptr and e != nullptr));
                Require (s <= e);
                lock_guard<Execution::ExternallySynchronizedLock> critSec (fLock_);
                size_t  i   =   fLength_;
                size_t  rhsLen  =  e - s;
                fData_.GrowToSize (i + rhsLen);
                fLength_ = i + rhsLen;
                memcpy (fData_.begin () + i, s, sizeof (wchar_t) * rhsLen);
            }
            inline  void    StringBuilder::Append (const wchar_t* s)
            {
                RequireNotNull (s);
                Append (s, s + ::wcslen (s));
            }
            inline  void    StringBuilder::Append (const wstring& s)
            {
                Append (s.c_str (), s.c_str () + s.length ());  // careful about s.end () if empty
            }
            inline  void    StringBuilder::Append (const String& s)
            {
                pair<const wchar_t*, const wchar_t*>    p   =   s.GetData<wchar_t> ();
                Append (p.first, p.second);
            }
            inline  StringBuilder&  StringBuilder::operator+= (const wchar_t* s)
            {
                RequireNotNull (s);
                Append (s);
                return *this;
            }
            inline  StringBuilder&  StringBuilder::operator+= (const String& s)
            {
                Append (s);
                return *this;
            }
            inline  StringBuilder&  StringBuilder::operator<< (const String& s)
            {
                Append (s);
                return *this;
            }
            inline  StringBuilder&  StringBuilder::operator<< (const wchar_t* s)
            {
                RequireNotNull (s);
                Append (s);
                return *this;
            }
            inline  void  StringBuilder::push_back (Character c)
            {
                lock_guard<Execution::ExternallySynchronizedLock> critSec (fLock_);
                fData_.GrowToSize (fLength_ + 1);
                fData_[fLength_] = c.GetCharacterCode ();
                fLength_++;
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
            template    <typename   T>
            T   StringBuilder::As () const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            template    <typename   T>
            void    StringBuilder::As (T* into) const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            template    <>
            inline  void    StringBuilder::As (String* into) const
            {
                RequireNotNull (into);
                // @todo could do more efficiently
                *into = str ();
            }
            template    <>
            inline  String StringBuilder::As () const
            {
                return str ();
            }
            template    <>
            inline  void    StringBuilder::As (wstring* into) const
            {
                RequireNotNull (into);
                // @todo could do more efficiently
                *into = str ().As<wstring> ();
            }
            template    <>
            inline  wstring StringBuilder::As () const
            {
                // @todo could do more efficiently
                return  str ().As<wstring> ();
            }
            inline  StringBuilder::operator String () const
            {
                return As<String> ();
            }
            inline  StringBuilder::operator wstring () const
            {
                return As<wstring> ();
            }


        }
    }
}
#endif // _Stroika_Foundation_Characters_StringBuilder_inl_
