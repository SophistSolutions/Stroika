/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
#include    "../Execution/Common.h"


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
            inline  void    StringBuilder::Append (const Character* s, const Character* e)
            {
                static_assert (sizeof (Character) == sizeof (wchar_t), "assume wchar_t == Character roughly");  //tmphack
                Append (reinterpret_cast<const wchar_t*> (s), reinterpret_cast<const wchar_t*> (e));
            }
            inline  void    StringBuilder::Append (const wchar_t* s, const wchar_t* e)
            {
                Require (s == e or (s != nullptr and e != nullptr));
                Require (s <= e);
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
#else
                auto    critSec { Execution::make_unique_lock (fLock_) };
#endif
                size_t  i   =   fLength_;
                size_t  rhsLen  =  e - s;
                fData_.GrowToSize (i + rhsLen);
                fLength_ = i + rhsLen;
                (void)::memcpy (fData_.begin () + i, s, sizeof (wchar_t) * rhsLen);
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
            inline  void    StringBuilder::Append (wchar_t c)
            {
                Append (&c, &c + 1);
            }
            inline  void    StringBuilder::Append (Character c)
            {
                Append (c.GetCharacterCode ());
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
            inline  StringBuilder&  StringBuilder::operator+= (const Character& c)
            {
                Append (c);
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
            inline  StringBuilder&  StringBuilder::operator<< (const Character& c)
            {
                Append (c);
                return *this;
            }
            inline  void  StringBuilder::push_back (Character c)
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
#else
                auto    critSec { Execution::make_unique_lock (fLock_) };
#endif
                fData_.GrowToSize (fLength_ + 1);
                fData_[fLength_] = c.GetCharacterCode ();
                fLength_++;
            }
            inline  size_t StringBuilder::GetLength () const
            {
                return fLength_;
            }
            inline  void    StringBuilder::SetAt (Character item, size_t index)
            {
                Require (index < fLength_);
                fData_[index] = item.GetCharacterCode ();
            }
            inline  const wchar_t*  StringBuilder::c_str () const
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
#else
                auto    critSec { Execution::make_unique_lock (fLock_) };
#endif
                fData_.GrowToSize (fLength_ + 1);
                fData_[fLength_] = '\0';
                return fData_.begin ();
            }
            inline  void    StringBuilder::clear ()
            {
                fLength_ = 0;
            }
            inline  String StringBuilder::str () const
            {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
                MACRO_LOCK_GUARD_CONTEXT (fLock_);
#else
                auto    critSec { Execution::make_unique_lock (fLock_) };
#endif
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
            inline  size_t  StringBuilder::length () const
            {
                return GetLength ();
            }
            inline  size_t  StringBuilder::size () const
            {
                return GetLength ();
            }


        }
    }
}
#endif // _Stroika_Foundation_Characters_StringBuilder_inl_
