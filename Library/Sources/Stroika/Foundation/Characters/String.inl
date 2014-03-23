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
            inline String::_IRep::_IRep (const wchar_t* start, const wchar_t* end)
                : _fStart (start)
                , _fEnd (end)
            {
            }
            inline   Character   String::_IRep::GetAt (size_t index) const
            {
                Assert (_fStart <= _fEnd);
                Require (index < GetLength ());
                return _fStart[index];
            }
            inline  const Character*    String::_IRep::Peek () const
            {
                Assert (_fStart <= _fEnd);
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                return ((const Character*)_fStart);
            }
            inline  pair<const Character*, const Character*> String::_IRep::GetData () const
            {
                Assert (_fStart <= _fEnd);
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                return pair<const Character*, const Character*> ((const Character*)_fStart, (const Character*)_fEnd);
            }
            inline  void    String::_IRep::CopyTo (Character* bufFrom, Character* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + _GetLength () >= bufTo);
                size_t  nChars = _GetLength ();
                (void)::memcpy (bufFrom, Peek (), nChars * sizeof (Character));
            }
            inline  void    String::_IRep::CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const
            {
                RequireNotNull (bufFrom);
                Require (bufFrom + _GetLength () >= bufTo);
                size_t  nChars = _GetLength ();
                (void)::memcpy (bufFrom, Peek (), nChars * sizeof (Character));
            }
            inline  void    String::_IRep::_SetData (const wchar_t* start, const wchar_t* end)
            {
                Require (_fStart <= _fEnd);
                _fStart = start;
                _fEnd = end;
            }
            inline     size_t  String::_IRep::_GetLength () const
            {
                Assert (_fStart <= _fEnd);
                return _fEnd - _fStart;
            }
            inline     Character   String::_IRep::_GetAt (size_t index) const
            {
                Assert (_fStart <= _fEnd);
                Require (index < _GetLength ());
                return _fStart[index];
            }
            inline     const Character*    String::_IRep::_Peek () const
            {
                Assert (_fStart <= _fEnd);
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                return ((const Character*)_fStart);
            }


            /*
            ********************************************************************************
            *********************** String::_SafeRepAccessor *******************************
            ********************************************************************************
            */
            inline   String::_SafeRepAccessor::_SafeRepAccessor (const String& s)
                : fAccessor (s._GetReadOnlyIterableIRepReference ())
            {
            }
            inline  const String::_IRep&    String::_SafeRepAccessor::_ConstGetRep () const
            {
                EnsureMember (fAccessor.get (), String::_IRep);
                return static_cast<const String::_IRep&> (*fAccessor.get ());   // static cast for performance sake - dynamic cast in Ensure
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
            inline  String::String (const _SharedPtrIRep& rep) noexcept
:
            inherited (rep)
            {
#if     qDebug
                _ConstGetRep (); // just make sure non-null and right type
#endif
            }
            inline  String::String (_SharedPtrIRep&& rep) noexcept
:
            inherited (std::move (rep))
            {
#if     qDebug
                _ConstGetRep (); // just make sure non-null and right type
#endif
            }
            inline  String::String ()
                : inherited (mkEmpty_ ())
            {
#if     qDebug
                _ConstGetRep (); // just make sure non-null and right type
#endif
            }
            inline  String::String (const wchar_t* cString)
                : inherited (cString[0] == '\0' ? mkEmpty_ () : mk_ (cString, cString + wcslen (cString)))
            {
                RequireNotNull (cString);
#if     qDebug
                _ConstGetRep (); // just make sure non-null and right type
#endif
            }
            inline  String::String (const wchar_t* from, const wchar_t* to)
                : inherited ((from == to) ? mkEmpty_ () : mk_ (from, to))
            {
                Require (from <= to);
                Require (from != nullptr or from == to);
#if     qDebug
                _ConstGetRep (); // just make sure non-null and right type
#endif
            }
            inline  String::String (const Character* from, const Character* to)
                : inherited ((from == to) ? mkEmpty_ () : mk_ (reinterpret_cast<const wchar_t*> (from), reinterpret_cast<const wchar_t*> (to)))
            {
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                Require (from <= to);
                Require (from != nullptr or from == to);
#if     qDebug
                _ConstGetRep (); // just make sure non-null and right type
#endif
            }
            inline  String::String (const std::wstring& r)
                : inherited (r.empty () ? mkEmpty_ () : mk_ (r.data (), r.data () + r.length ()))
            {
#if     qDebug
                _ConstGetRep (); // just make sure non-null and right type
#endif
            }
            inline  const String::_IRep&    String::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), String::_IRep);
                return static_cast<const String::_IRep&> (inherited::_ConstGetRep ());   // static cast for performance sake - dynamic cast in Ensure
            }
            inline  void    String::CopyTo (Character* bufFrom, Character* bufTo) const
            {
                _SafeRepAccessor accessor { *this };
                RequireNotNull (bufFrom);
                Require (bufFrom + accessor._ConstGetRep ()._GetLength () >= bufTo);
                accessor._ConstGetRep ().CopyTo (bufFrom, bufTo);
            }
            inline  void    String::CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const
            {
                _SafeRepAccessor accessor { *this };
                RequireNotNull (bufFrom);
                Require (bufFrom + accessor._ConstGetRep ()._GetLength () >= bufTo);
                accessor._ConstGetRep ().CopyTo (bufFrom, bufTo);
            }
            inline  size_t  String::GetLength () const
            {
                _SafeRepAccessor accessor { *this };
                return accessor._ConstGetRep ()._GetLength ();
            }
            inline  String  String::SubString (size_t from) const
            {
                _SafeRepAccessor    accessor { *this };
                size_t              myLength { accessor._ConstGetRep ()._GetLength () };
                Require (from <= myLength);
                size_t  useLength  { myLength - from };
                if (useLength == 0) {
                    return String ();
                }
                if ((from == 0) and (useLength == myLength)) {
                    ////@TODO - FIX - SHOULD convert _SafeRepAccessor to String instance??? then return that!
                    return *this;       // just bump reference count
                }
                return SubString_ (accessor, myLength, from, from + useLength);
            }
            inline  String  String::SubString (size_t from, size_t to) const
            {
                _SafeRepAccessor    accessor { *this };
                size_t              myLength { accessor._ConstGetRep ()._GetLength () };
                Require (from <= to);
                Require (to <= myLength);
                size_t  useLength  =   (to - from);
                if (useLength == 0) {
                    return String ();
                }
                if ((from == 0) and (useLength == myLength)) {
                    ////@TODO - FIX - SHOULD convert _SafeRepAccessor to String instance??? then return that!
                    return *this;       // just bump reference count
                }
                return SubString_ (accessor, myLength, from, from + useLength);
            }
            inline  String      String::CircularSubString (ptrdiff_t from, ptrdiff_t to) const
            {
                _SafeRepAccessor    accessor { *this };
                size_t              myLength { accessor._ConstGetRep ()._GetLength () };
                size_t  f = from < 0 ? (myLength + from) : from;
                size_t  t = to < 0 ? (myLength + to) : to;
                Require (f != kBadIndex);
                Require (t != kBadIndex);
                Require (f <= t);
                Require (t <= myLength);
                return SubString_ (accessor, myLength, f, t);
            }
            inline  String    String::RemoveAt (size_t charAt) const
            {
                return RemoveAt (charAt, charAt + 1);
            }
            inline  bool    String::empty () const
            {
                _SafeRepAccessor accessor (*this);
                return accessor._ConstGetRep ()._GetLength () == 0;
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
            inline  String    String::InsertAt_nu (Character c, size_t at) const
            {
                return InsertAt_nu (&c, &c + 1, at);
            }
            inline  String    String::InsertAt_nu (const String& s, size_t at) const
            {
                _SafeRepAccessor copyAccessor { *this };
                pair<const Character*, const Character*> d = copyAccessor._ConstGetRep ().GetData ();
                return InsertAt_nu (d.first, d.second, at);
            }
            inline  String    String::InsertAt_nu (const wchar_t* from, const wchar_t* to, size_t at) const
            {
                return InsertAt_nu (reinterpret_cast<const Character*> (from), reinterpret_cast<const Character*> (to), at);
            }
            inline  void    String::Append (const wchar_t* from, const wchar_t* to)
            {
                Require (from <= to);
                if (from != to) {
                    _SafeRepAccessor    thisAccessor { *this };
                    pair<const Character*, const Character*> lhsD   =   thisAccessor._ConstGetRep ().GetData ();
                    *this = String (
                                mk_ (
                                    reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second),
                                    from, to
                                )
                            );
                }
            }
            inline  void    String::Append (Character c)
            {
                Append (&c, &c + 1);
            }
            inline  void    String::Append (const String& s)
            {
                _SafeRepAccessor    rhsAccessor { s };
                pair<const Character*, const Character*> rhsD   =   rhsAccessor._ConstGetRep ().GetData ();
                Append (reinterpret_cast<const wchar_t*> (rhsD.first), reinterpret_cast<const wchar_t*> (rhsD.second));
            }
            inline  void    String::Append (const wchar_t* s)
            {
                Append (s, s + ::wcslen (s));
            }
            inline  void    String::Append (const Character* from, const Character* to)
            {
                Append (reinterpret_cast<const wchar_t*> (from), reinterpret_cast<const wchar_t*> (to));
            }
            inline  String& String::operator+= (Character appendage)
            {
                Append (appendage);
                return *this;
            }
            inline  String& String::operator+= (const String& appendage)
            {
                Append (appendage);
                return *this;
            }
            inline  String& String::operator+= (const wchar_t* appendageCStr)
            {
                Append (appendageCStr);
                return *this;
            }
            inline  const Character   String::GetCharAt (size_t i) const
            {
                _SafeRepAccessor    accessor { *this };
                Require (i >= 0);
                Require (i < accessor._ConstGetRep ()._GetLength ());
                return accessor._ConstGetRep ().GetAt (i);
            }
            inline  const Character   String::operator[] (size_t i) const
            {
                Require (i >= 0);
                Require (i < GetLength ());
                return GetCharAt (i);
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
            inline  String::operator T () const
            {
                return As<T> ();
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
                _SafeRepAccessor    accessor { *this };
                size_t              n       {   accessor._ConstGetRep ()._GetLength () };
                const Character* cp = accessor._ConstGetRep ().Peek ();
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
                _SafeRepAccessor    accessor { *this };
// I'm not sure of the Peek() semantics, so I'm not sure this is right, but document Peek() better so this is safe!!!   -- LGP 2011-09-01
                return (const wchar_t*)accessor._ConstGetRep ().Peek ();
            }
            template    <>
            inline  const Character*    String::As () const
            {
                _SafeRepAccessor    accessor { *this };
// I'm not sure of the Peek() semantics, so I'm not sure this is right, but document Peek() better so this is safe!!!   -- LGP 2011-09-01
                return (const Character*)accessor._ConstGetRep ().Peek ();
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
            inline  SDKString String::AsSDKString () const
            {
                SDKString result;
                AsSDKString (&result);
                return result;
            }
            inline  string  String::AsNarrowSDKString () const
            {
                string result;
                AsNarrowSDKString (&result);
                return result;
            }
            template    <>
            inline  pair<const Character*, const Character*> String::GetData () const
            {
                _SafeRepAccessor    accessor { *this };
                return accessor._ConstGetRep ().GetData ();
            }
            template    <>
            inline  pair<const wchar_t*, const wchar_t*> String::GetData () const
            {
                _SafeRepAccessor    accessor { *this };
                pair<const Character*, const Character*>    p   = accessor._ConstGetRep ().GetData ();
                return pair<const wchar_t*, const wchar_t*> (reinterpret_cast<const wchar_t*> (p.first), reinterpret_cast<const wchar_t*> (p.second));
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
            inline  const wchar_t*  String::c_str () const
            {
                // Since this is intrinsically un-threadsafe anyhow, dont bother making a threadsafe
                // copy (_SafeRepAccessor) of the shared_ptr
                if (const wchar_t* result = _ConstGetRep ().c_str_peek ()) {
                    return result;
                }
                else {
                    return c_str_ ();
                }
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
                Append (Character (c));
            }
            inline  void    String::push_back (Character c)
            {
                Append (c);
            }
            inline  String      String::substr (size_t from, size_t count) const
            {
                _SafeRepAccessor    accessor { *this };
                size_t              thisLen = accessor._ConstGetRep ()._GetLength ();
                if (from > thisLen) {
                    Execution::DoThrow (std::out_of_range ("string index out of range"));
                }
                // @todo
                // Not QUITE correct - due to overflow issues, but pragmaitcally this is probably close enough
                size_t  to  =   (count == kBadIndex) ? thisLen : (from + min (thisLen, count));
                return SubString_ (accessor, thisLen, from, to);
            }
            inline  int String::Compare (const String& rhs, CompareOptions co) const
            {
                _SafeRepAccessor    accessor { *this };
                pair<const Character*, const Character*> l = accessor._ConstGetRep ().GetData ();
                _SafeRepAccessor    rhsAccessor { rhs };
                pair<const Character*, const Character*> r = rhsAccessor._ConstGetRep ().GetData ();
                return Character::Compare (l.first, l.second, r.first, r.second, co);
            }
            inline  int String::Compare (const Character* rhs, CompareOptions co) const
            {
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                _SafeRepAccessor    accessor { *this };
                pair<const Character*, const Character*> l = accessor._ConstGetRep ().GetData ();
                return Character::Compare (l.first, l.second, reinterpret_cast<const Character*> (rhs), reinterpret_cast<const Character*> (rhs) +::wcslen (reinterpret_cast<const wchar_t*> (rhs)), co);
            }
            inline  int String::Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co) const
            {
                _SafeRepAccessor    accessor { *this };
                pair<const Character*, const Character*> l = accessor._ConstGetRep ().GetData ();
                return Character::Compare (l.first, l.second, rhsStart, rhsEnd, co);
            }
            inline  int String::Compare (const wchar_t* rhs, CompareOptions co) const
            {
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                _SafeRepAccessor    accessor { *this };
                pair<const Character*, const Character*> l = accessor._ConstGetRep ().GetData ();
                return Character::Compare (l.first, l.second, reinterpret_cast<const Character*> (rhs), reinterpret_cast<const Character*> (rhs) +::wcslen (rhs), co);
            }
            inline  int String::Compare (const wchar_t* rhsStart, const wchar_t* rhsEnd, CompareOptions co) const
            {
                static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
                _SafeRepAccessor    accessor { *this };
                pair<const Character*, const Character*> l = accessor._ConstGetRep ().GetData ();
                return Character::Compare (l.first, l.second, reinterpret_cast<const Character*> (rhsStart), reinterpret_cast<const Character*> (rhsEnd), co);
            }
            inline  bool String::Equals (const String& rhs, CompareOptions co) const
            {
                // OK in two steps because length test doesnt affect correctness - just performance
                if (GetLength () != rhs.GetLength ()) {
                    return false;
                }
                return Compare (rhs, co) == 0;
            }
            inline  bool String::Equals (const wchar_t* rhs, CompareOptions co) const
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
                return Equals (rhs);
            }
            inline  bool String::operator!= (const wchar_t* rhs) const
            {
                return not Equals (rhs);
            }


            /*
            ********************************************************************************
            ********************************** operator<< **********************************
            ********************************************************************************
            */
            inline  wostream&    operator<< (wostream& out, const String& s)
            {
                // Tried two impls, but first empirically appears quicker.
                // HOWERVER - THIS IS INTRINSICALLY NOT THREASDAFE (if s changed while another thread writin it)
                String::_SafeRepAccessor    thisAccessor { s };
                pair<const Character*, const Character*> p   =   thisAccessor._ConstGetRep ().GetData ();
                out.write (reinterpret_cast<const wchar_t*> (p.first), p.second - p.first);
                return out;
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
