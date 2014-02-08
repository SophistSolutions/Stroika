/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_inl_
#define _Stroika_Foundation_Characters_String_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"
#include    "../Execution/ModuleInit.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /*
            ********************************************************************************
            ********************************* String::_IRep ********************************
            ********************************************************************************
            */
            inline  String::_IRep::_IRep ()
            {
            }
            inline  String::_IRep::~_IRep ()
            {
            }
            inline  void    String::_IRep::CopyTo (Character* bufFrom, Character* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + GetLength () >= bufTo);
                size_t  nChars = GetLength ();
                (void)::memcpy (bufFrom, Peek (), nChars * sizeof (Character));
            }
            inline  void    String::_IRep::CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + GetLength () >= bufTo);
                size_t  nChars = GetLength ();
                (void)::memcpy (bufFrom, Peek (), nChars * sizeof (Character));
            }


            /*
            ********************************************************************************
            ************************************* String ***********************************
            ********************************************************************************
            */
            inline  String::String (const String& from) noexcept
:
            inherited (from)
            {
            }
            inline  String::String (String&& from) noexcept
:
            inherited (std::move (from))
            {
            }
            inline  String& String::operator= (const String& newString)
            {
                inherited::operator= (newString);
                return *this;
            }
            inline  String::~String ()
            {
            }
            inline  const String::_IRep&    String::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), String::_IRep);
                return static_cast<const String::_IRep&> (inherited::_GetRep ());   // static cast for performance sake - dynamic cast in Ensure
            }
            inline  String::_IRep&          String::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), String::_IRep);
                return static_cast<String::_IRep&> (inherited::_GetRep ());         // static cast for performance sake - dynamic cast in Ensure
            }
            inline  void    String::CopyTo (Character* bufFrom, Character* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + GetLength () >= bufTo);
                _GetRep ().CopyTo (bufFrom, bufTo);
            }
            inline  void    String::CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + GetLength () >= bufTo);
                _GetRep ().CopyTo (bufFrom, bufTo);
            }
            inline  size_t  String::GetLength () const
            {
                return (_GetRep ().GetLength ());
            }
            inline  void    String::RemoveAt (size_t charAt)
            {
                RemoveAt (charAt, charAt + 1);
            }
            inline  bool    String::empty () const
            {
                return _GetRep ().GetLength () == 0;
            }
            inline  void    String::clear ()
            {
                *this = String ();
            }
            inline  size_t  String::Find (Character c, CompareOptions co) const
            {
                return Find (c, 0, co);
            }
            inline  size_t  String::Find (const String& subString, CompareOptions co) const
            {
                return Find (subString, 0, co);
            }
            inline  bool    String::Contains (Character c, CompareOptions co) const
            {
                return bool (Find (c, co) != kBadIndex);
            }
            inline  bool    String::Contains (const String& subString, CompareOptions co) const
            {
                return bool (Find (subString, co) != kBadIndex);
            }
            inline  void    String::InsertAt (Character c, size_t at)
            {
                InsertAt (&c, &c + 1, at);
            }
            inline  void    String::InsertAt (const String& s, size_t at)
            {
                // NB: I don't THINK we need be careful if s.fRep == this->fRep because when we first derefence this->fRep it will force a CLONE, so OUR fRep will be unique
                // And no need to worry about lifetime of 'p' because we don't allow changes to 's' from two different threads at a time, and the rep would rep if accessed from
                // another thread could only change that other envelopes copy
                pair<const Character*, const Character*> d = s._GetRep ().GetData ();
                InsertAt (d.first, d.second, at);
            }
            inline  void    String::InsertAt (const wchar_t* from, const wchar_t* to, size_t at)
            {
                InsertAt (reinterpret_cast<const Character*> (from), reinterpret_cast<const Character*> (to), at);
            }
            inline  void    String::Append (Character c)
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                InsertAt (c, GetLength ());
            }
            inline  void    String::Append (const String& s)
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                InsertAt (s, GetLength ());
            }
            inline  void    String::Append (const wchar_t* from, const wchar_t* to)
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                InsertAt (from, to, GetLength ());
            }
            inline  void    String::Append (const Character* from, const Character* to)
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                InsertAt (from, to, GetLength ());
            }
            inline  String& String::operator+= (Character appendage)
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                InsertAt (appendage, GetLength ());
                return (*this);
            }
            inline  String& String::operator+= (const String& appendage)
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                InsertAt (appendage, GetLength ());
                return (*this);
            }
            inline  Character   String::operator[] (size_t i) const
            {
                Require (i >= 0);
                Require (i < GetLength ());
                return (_GetRep ().GetAt (i));
            }
            template    <typename   T>
            T   String::As () const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            template    <typename   T>
            void    String::As (T* into) const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            template    <typename   T>
            T   String::AsUTF8 () const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            template    <typename   T>
            void    String::AsUTF8 (T* into) const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            template    <typename   T>
            T   String::AsASCII () const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            template    <typename   T>
            void    String::AsASCII (T* into) const
            {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                RequireNotReached ();
#else
                static_assert (false, "Only specifically specialized variants are supported");
#endif
            }
            template    <>
            inline  void    String::As (wstring* into) const
            {
                RequireNotNull (into);
                String  threadSafeCopy  =   *this;
                size_t  n   =   threadSafeCopy.GetLength ();
                const Character* cp = threadSafeCopy._GetRep ().Peek ();
                Assert (sizeof (Character) == sizeof (wchar_t));        // going to want to clean this up!!!    --LGP 2011-09-01
                const wchar_t* wcp  =   (const wchar_t*)cp;
                into->assign (wcp, wcp + n);
            }
            template    <>
            inline  wstring String::As () const
            {
                wstring r;
                As (&r);
                return r;
            }
            template    <>
            inline  const wchar_t*  String::As () const
            {
// I'm not sure of the Peek() semantics, so I'm not sure this is right, but document Peek() better so this is safe!!!   -- LGP 2011-09-01
                return (const wchar_t*)_GetRep ().Peek ();
            }
            template    <>
            inline  const Character*    String::As () const
            {
// I'm not sure of the Peek() semantics, so I'm not sure this is right, but document Peek() better so this is safe!!!   -- LGP 2011-09-01
                return (const Character*)_GetRep ().Peek ();
            }
            template    <>
            inline  string  String::AsUTF8 () const
            {
                string  r;
                AsUTF8 (&r);
                return r;
            }
            inline  std::string String::AsUTF8 () const
            {
                return AsUTF8<std::string> ();
            }
            inline  void        String::AsUTF8 (std::string* into) const
            {
                AsUTF8<std::string> (into);
            }
            template    <>
            inline  string  String::AsASCII () const
            {
                string  r;
                AsASCII (&r);
                return r;
            }
            inline  std::string String::AsASCII () const
            {
                return AsASCII<std::string> ();
            }
            inline  void    String::AsASCII (std::string* into) const
            {
                AsASCII<std::string> (into);
            }
            inline  size_t  String::length () const
            {
                return GetLength ();
            }
            inline  size_t  String::size () const
            {
                return GetLength ();
            }
            inline  const wchar_t*  String::data () const
            {
                return As<const wchar_t*> ();
            }
            inline  size_t String::find (wchar_t c, size_t startAt) const
            {
                return Find (c, startAt, CompareOptions::eWithCase);
            }
            inline  size_t String::rfind (wchar_t c) const
            {
                return RFind (c);
            }
            inline  void    String::push_back (wchar_t c)
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                InsertAt (c, GetLength ());
            }
            inline  void    String::push_back (Character c)
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                InsertAt (c, GetLength ());
            }
            inline  int String::Compare (const String& rhs, CompareOptions co) const
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                pair<const Character*, const Character*> l = _GetRep ().GetData ();
                pair<const Character*, const Character*> r = rhs._GetRep ().GetData ();
                return Character::Compare (l.first, l.second, r.first, r.second, co);
            }
            inline  int String::Compare (const Character* rhs, CompareOptions co) const
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                pair<const Character*, const Character*> l = _GetRep ().GetData ();
                return Character::Compare (l.first, l.second, reinterpret_cast<const Character*> (rhs), reinterpret_cast<const Character*> (rhs) +::wcslen (reinterpret_cast<const wchar_t*> (rhs)), co);
            }
            inline  int String::Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co) const
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                pair<const Character*, const Character*> l = _GetRep ().GetData ();
                return Character::Compare (l.first, l.second, rhsStart, rhsEnd, co);
            }
            inline  int String::Compare (const wchar_t* rhs, CompareOptions co) const
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                pair<const Character*, const Character*> l = _GetRep ().GetData ();
                return Character::Compare (l.first, l.second, reinterpret_cast<const Character*> (rhs), reinterpret_cast<const Character*> (rhs) +::wcslen (rhs), co);
            }
            inline  int String::Compare (const wchar_t* rhsStart, const wchar_t* rhsEnd, CompareOptions co) const
            {
                // @todo - FIX - NOT ENVELOPE THREADSAFE
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                pair<const Character*, const Character*> l = _GetRep ().GetData ();
                return Character::Compare (l.first, l.second, reinterpret_cast<const Character*> (rhsStart), reinterpret_cast<const Character*> (rhsEnd), co);
            }
            inline  bool String::Equals (const String& rhs, CompareOptions co) const
            {
                return Compare (rhs, co) == 0;
            }
            inline  bool String::operator<= (const String& rhs) const
            {
                return Compare (rhs) <= 0;
            }
            inline  bool String::operator< (const String& rhs) const
            {
                return Compare (rhs) < 0;
            }
            inline  bool String::operator> (const String& rhs) const
            {
                return Compare (rhs) > 0;
            }
            inline  bool String::operator>= (const String& rhs) const
            {
                return Compare (rhs) >= 0;
            }
            inline  bool String::operator== (const String& rhs) const
            {
                return Compare (rhs) == 0;
            }
            inline  bool String::operator!= (const String& rhs) const
            {
                return Compare (rhs) != 0;
            }
            inline  bool String::operator<= (const Character* rhs) const
            {
                return Compare (rhs) <= 0;
            }
            inline  bool String::operator< (const Character* rhs) const
            {
                return Compare (rhs) < 0;
            }
            inline  bool String::operator> (const Character* rhs) const
            {
                return Compare (rhs) > 0;
            }
            inline  bool String::operator>= (const Character* rhs) const
            {
                return Compare (rhs) >= 0;
            }
            inline  bool String::operator== (const Character* rhs) const
            {
                return Compare (rhs) == 0;
            }
            inline  bool String::operator!= (const Character* rhs) const
            {
                return Compare (rhs) != 0;
            }
            inline  bool String::operator<= (const wchar_t* rhs) const
            {
                return Compare (rhs) <= 0;
            }
            inline  bool String::operator< (const wchar_t* rhs) const
            {
                return Compare (rhs) < 0;
            }
            inline  bool String::operator> (const wchar_t* rhs) const
            {
                return Compare (rhs) > 0;
            }
            inline  bool String::operator>= (const wchar_t* rhs) const
            {
                return Compare (rhs) >= 0;
            }
            inline  bool String::operator== (const wchar_t* rhs) const
            {
                return Compare (rhs) == 0;
            }
            inline  bool String::operator!= (const wchar_t* rhs) const
            {
                return Compare (rhs) != 0;
            }


            /*
            ********************************************************************************
            ********************************* String_ModuleInit_ ***************************
            ********************************************************************************
            */
            struct  String_ModuleInit_ {
                String_ModuleInit_ ();
                Execution::ModuleDependency fBlockAllocationDependency;
            };


        }
    }
}


namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Characters::String_ModuleInit_>   _Stroika_Foundation_Characters_String_ModuleInit_;   // this object constructed for the CTOR/DTOR per-module side-effects
}

#endif // _Stroika_Foundation_Characters_String_inl_
