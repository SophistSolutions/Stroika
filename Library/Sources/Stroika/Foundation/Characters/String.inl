/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_inl_
#define _Stroika_Foundation_Characters_String_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"
#include "../Execution/ModuleInit.h"
#include "../Execution/Throw.h"

namespace Stroika::Foundation::Characters {

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
    inline Character String::_IRep::GetAt (size_t index) const
    {
        Assert (_fStart <= _fEnd);
        Require (index < GetLength ());
        return _fStart[index];
    }
#if 0
    inline const Character* String::_IRep::Peek () const
    {
        Assert (_fStart <= _fEnd);
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        return (reinterpret_cast<const Character*> (_fStart));
    }
#endif
    inline pair<const Character*, const Character*> String::_IRep::GetData () const
    {
        Assert (_fStart <= _fEnd);
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        return pair<const Character*, const Character*> (reinterpret_cast<const Character*> (_fStart), reinterpret_cast<const Character*> (_fEnd));
    }
    inline void String::_IRep::CopyTo (Character* bufFrom, [[maybe_unused]] Character* bufTo) const
    {
        RequireNotNull (bufFrom);
        Require (bufFrom + _GetLength () >= bufTo);
        size_t nChars = _GetLength ();
        (void)::memcpy (bufFrom, _Peek (), nChars * sizeof (Character));
    }
    inline void String::_IRep::CopyTo (wchar_t* bufFrom, [[maybe_unused]] wchar_t* bufTo) const
    {
        RequireNotNull (bufFrom);
        Require (bufFrom + _GetLength () >= bufTo);
        size_t nChars = _GetLength ();
        (void)::memcpy (bufFrom, _Peek (), nChars * sizeof (Character));
    }
    inline void String::_IRep::_SetData (const wchar_t* start, const wchar_t* end)
    {
        Require (_fStart <= _fEnd);
        Require (*end == '\0');
        _fStart = start;
        _fEnd   = end;
    }
    inline size_t String::_IRep::_GetLength () const
    {
        Assert (_fStart <= _fEnd);
        Assert (*_fEnd == '\0');
        return _fEnd - _fStart;
    }
    inline Character String::_IRep::_GetAt (size_t index) const
    {
        Assert (_fStart <= _fEnd);
        Require (index < _GetLength ());
        return _fStart[index];
    }
    inline const Character* String::_IRep::_Peek () const
    {
        Assert (_fStart <= _fEnd);
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        return ((const Character*)_fStart);
    }

    /*
     ********************************************************************************
     ************************************* String ***********************************
     ********************************************************************************
     */
    inline String::String (const String& from) noexcept
        : inherited (from)
    {
    }
    inline String::String (String&& from) noexcept
        : inherited (move (from))
    {
    }
    inline String::String (const _SharedPtrIRep& rep) noexcept
        : inherited (rep)
    {
        _AssertRepValidType ();
    }
    inline String::String (_SharedPtrIRep&& rep) noexcept
        : inherited (move (rep))
    {
        //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
        _AssertRepValidType ();
    }
    inline String::String ()
        : inherited (mkEmpty_ ())
    {
        _AssertRepValidType ();
    }
    inline String::String (const wchar_t* cString)
        : inherited (cString[0] == '\0' ? mkEmpty_ () : mk_ (cString, cString + wcslen (cString)))
    {
        RequireNotNull (cString);
        _AssertRepValidType ();
    }
    inline String::String (const wchar_t* from, const wchar_t* to)
        : inherited ((from == to) ? mkEmpty_ () : mk_ (from, to))
    {
        Require (from <= to);
        Require (from != nullptr or from == to);
        _AssertRepValidType ();
    }
    inline String::String (const Character* from, const Character* to)
        : inherited ((from == to) ? mkEmpty_ () : mk_ (reinterpret_cast<const wchar_t*> (from), reinterpret_cast<const wchar_t*> (to)))
    {
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        Require (from <= to);
        Require (from != nullptr or from == to);
        _AssertRepValidType ();
    }
    inline String::String (const wstring& r)
        : inherited (r.empty () ? mkEmpty_ () : mk_ (r.data (), r.data () + r.length ()))
    {
        _AssertRepValidType ();
    }
    inline String::String (const u16string& r)
        : inherited (r.empty () ? mkEmpty_ () : mk_ (r.data (), r.data () + r.length ()))
    {
        _AssertRepValidType ();
    }
    inline String::String (const u32string& r)
        : inherited (r.empty () ? mkEmpty_ () : mk_ (r.data (), r.data () + r.length ()))
    {
        _AssertRepValidType ();
    }
#if __cpp_char8_t >= 201811L
    inline String::String (const char8_t* from, const char8_t* to)
        : inherited ((from == to) ? mkEmpty_ () : FromUTF8 (from, to))
    {
        Require ((from == nullptr) == (to == nullptr));
        Require (from <= to);
        _AssertRepValidType (); // just make sure non-null and right type
    }
#endif
    inline String::String (const char16_t* from, const char16_t* to)
        : inherited ((from == to) ? mkEmpty_ () : mk_ (from, to))
    {
        Require ((from == nullptr) == (to == nullptr));
        Require (from <= to);
        _AssertRepValidType (); // just make sure non-null and right type
    }
    inline String::String (const char32_t* from, const char32_t* to)
        : inherited ((from == to) ? mkEmpty_ () : mk_ (from, to))
    {
        Require ((from == nullptr) == (to == nullptr));
        Require (from <= to);
        _AssertRepValidType (); // just make sure non-null and right type
    }
    inline String String::FromNarrowString (const char* from, const locale& l)
    {
        RequireNotNull (from);
        return FromNarrowString (from, from + ::strlen (from), l);
    }
    inline String String::FromNarrowString (const string& from, const locale& l)
    {
        return FromNarrowString (from.c_str (), from.c_str () + from.length (), l);
    }
    inline String String::FromASCII (const char* from)
    {
        RequireNotNull (from);
        return FromASCII (from, from + ::strlen (from));
    }
    inline String String::FromASCII (const string& from)
    {
        return FromASCII (from.c_str (), from.c_str () + from.length ());
    }
    inline String String::FromUTF8 (const char* from)
    {
        RequireNotNull (from);
        return FromUTF8 (from, from + ::strlen (from));
    }
    inline String String::FromUTF8 (const string& from)
    {
        return FromUTF8 (from.c_str (), from.c_str () + from.length ());
    }
    inline String String::FromISOLatin1 (const char* from)
    {
        return FromISOLatin1 (from, from + ::strlen (from));
    }
    inline String String::FromISOLatin1 (const string& from)
    {
        return FromISOLatin1 (from.c_str (), from.c_str () + from.length ());
    }
    inline void String::_AssertRepValidType () const
    {
        EnsureMember (&_SafeReadRepAccessor{this}._ConstGetRep (), String::_IRep);
    }
    inline void String::CopyTo (Character* bufFrom, Character* bufTo) const
    {
        _SafeReadRepAccessor accessor{this};
        RequireNotNull (bufFrom);
        Require (bufFrom + accessor._ConstGetRep ()._GetLength () >= bufTo);
        accessor._ConstGetRep ().CopyTo (bufFrom, bufTo);
    }
    inline void String::CopyTo (wchar_t* bufFrom, wchar_t* bufTo) const
    {
        _SafeReadRepAccessor accessor{this};
        RequireNotNull (bufFrom);
        Require (bufFrom + accessor._ConstGetRep ()._GetLength () >= bufTo);
        accessor._ConstGetRep ().CopyTo (bufFrom, bufTo);
    }
    inline size_t String::GetLength () const
    {
        return _SafeReadRepAccessor{this}._ConstGetRep ()._GetLength ();
    }
    inline size_t String::SubString_adjust_ (unsigned int from, [[maybe_unused]] size_t myLength) const
    {
        return static_cast<size_t> (from);
    }
    inline size_t String::SubString_adjust_ (unsigned long from, [[maybe_unused]] size_t myLength) const
    {
        return static_cast<size_t> (from);
    }
    inline size_t String::SubString_adjust_ (unsigned long long from, [[maybe_unused]] size_t myLength) const
    {
        return static_cast<size_t> (from);
    }
    inline size_t String::SubString_adjust_ (int from, size_t myLength) const
    {
        return static_cast<size_t> (from < 0 ? (myLength + from) : from);
    }
    inline size_t String::SubString_adjust_ (long from, size_t myLength) const
    {
        return static_cast<size_t> (from < 0 ? (myLength + from) : from);
    }
    inline size_t String::SubString_adjust_ (long long from, size_t myLength) const
    {
        return static_cast<size_t> (from < 0 ? (myLength + from) : from);
    }
    template <typename SZ>
    inline String String::SubString (SZ from) const
    {
        _SafeReadRepAccessor accessor{this};
        size_t               myLength{accessor._ConstGetRep ()._GetLength ()};
        size_t               f = SubString_adjust_ (from, myLength);
        size_t               t = myLength;
        Require (f <= myLength);
        return SubString_ (accessor, myLength, f, t);
    }
    template <typename SZ1, typename SZ2>
    inline String String::SubString (SZ1 from, SZ2 to) const
    {
        _SafeReadRepAccessor accessor{this};
        size_t               myLength{accessor._ConstGetRep ()._GetLength ()};
        size_t               f = SubString_adjust_ (from, myLength);
        size_t               t = SubString_adjust_ (to, myLength);
        Require (f <= t);
        Require (t <= myLength);
        return SubString_ (accessor, myLength, f, t);
    }
    template <typename SZ>
    inline String String::SafeSubString (SZ from) const
    {
        _SafeReadRepAccessor accessor{this};
        size_t               myLength{accessor._ConstGetRep ()._GetLength ()};
        size_t               f = SubString_adjust_ (from, myLength);
        size_t               t = myLength;
        f                      = min (f, myLength);
        Assert (f <= myLength);
        size_t useLength{myLength - f};
        return SubString_ (accessor, myLength, f, f + useLength);
    }
    template <typename SZ1, typename SZ2>
    inline String String::SafeSubString (SZ1 from, SZ2 to) const
    {
        _SafeReadRepAccessor accessor{this};
        size_t               myLength{accessor._ConstGetRep ()._GetLength ()};
        size_t               f = SubString_adjust_ (from, myLength);
        size_t               t = SubString_adjust_ (to, myLength);
        f                      = min (f, myLength);
        t                      = min (t, myLength);
        t                      = max (t, f);
        Assert (f <= t);
        Assert (t <= myLength);
        size_t useLength = (t - f);
        return SubString_ (accessor, myLength, f, f + useLength);
    }
    inline String String::RemoveAt (size_t charAt) const
    {
        return RemoveAt (charAt, charAt + 1);
    }
    inline String String::RemoveAt (pair<size_t, size_t> fromTo) const
    {
        return RemoveAt (fromTo.first, fromTo.second);
    }
    inline bool String::empty () const
    {
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ()._GetLength () == 0;
    }
    inline void String::clear ()
    {
        *this = String ();
    }
    inline optional<size_t> String::Find (Character c, CompareOptions co) const
    {
        return Find (c, 0, co);
    }
    inline optional<size_t> String::Find (const String& subString, CompareOptions co) const
    {
        return Find (subString, 0, co);
    }
    inline bool String::Contains (Character c, CompareOptions co) const
    {
        return static_cast<bool> (Find (c, co));
    }
    inline bool String::Contains (const String& subString, CompareOptions co) const
    {
        return static_cast<bool> (Find (subString, co));
    }
    inline String String::Replace (pair<size_t, size_t> fromTo, const String& replacement) const
    {
        return Replace (fromTo.first, fromTo.second, replacement);
    }
    inline String String::InsertAt (Character c, size_t at) const
    {
        return InsertAt (&c, &c + 1, at);
    }
    inline String String::InsertAt (const String& s, size_t at) const
    {
        _SafeReadRepAccessor                     copyAccessor{&s};
        pair<const Character*, const Character*> d = copyAccessor._ConstGetRep ().GetData ();
        return InsertAt (d.first, d.second, at);
    }
    inline String String::InsertAt (const wchar_t* from, const wchar_t* to, size_t at) const
    {
        return InsertAt (reinterpret_cast<const Character*> (from), reinterpret_cast<const Character*> (to), at);
    }
    inline void String::Append (const wchar_t* from, const wchar_t* to)
    {
        Require (from <= to);
        if (from != to) {
            _SafeReadRepAccessor                     thisAccessor{this};
            pair<const Character*, const Character*> lhsD = thisAccessor._ConstGetRep ().GetData ();
            *this                                         = String (
                mk_ (
                    reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second),
                    from, to));
        }
    }
    inline void String::Append (Character c)
    {
        Append (&c, &c + 1);
    }
    inline void String::Append (const String& s)
    {
        _SafeReadRepAccessor                     rhsAccessor{&s};
        pair<const Character*, const Character*> rhsD = rhsAccessor._ConstGetRep ().GetData ();
        Append (reinterpret_cast<const wchar_t*> (rhsD.first), reinterpret_cast<const wchar_t*> (rhsD.second));
    }
    inline void String::Append (const wchar_t* s)
    {
        Append (s, s + ::wcslen (s));
    }
    inline void String::Append (const Character* from, const Character* to)
    {
        Append (reinterpret_cast<const wchar_t*> (from), reinterpret_cast<const wchar_t*> (to));
    }
    inline String& String::operator+= (Character appendage)
    {
        Append (appendage);
        return *this;
    }
    inline String& String::operator+= (const String& appendage)
    {
        Append (appendage);
        return *this;
    }
    inline String& String::operator+= (const wchar_t* appendageCStr)
    {
        Append (appendageCStr);
        return *this;
    }
    inline const Character String::GetCharAt (size_t i) const
    {
        _SafeReadRepAccessor accessor{this};
        Require (i >= 0);
        Require (i < accessor._ConstGetRep ()._GetLength ());
        return accessor._ConstGetRep ().GetAt (i);
    }
    inline const Character String::operator[] (size_t i) const
    {
        Require (i >= 0);
        Require (i < GetLength ());
        return GetCharAt (i);
    }
    template <>
    inline void String::As (wstring* into) const
    {
        RequireNotNull (into);
        _SafeReadRepAccessor accessor{this};
        size_t               n{accessor._ConstGetRep ()._GetLength ()};
        const Character*     cp = accessor._ConstGetRep ()._Peek ();
        Assert (sizeof (Character) == sizeof (wchar_t)); // going to want to clean this up!!!    --LGP 2011-09-01
        const wchar_t* wcp = (const wchar_t*)cp;
        into->assign (wcp, wcp + n);
    }
    template <>
    inline wstring String::As () const
    {
        wstring r;
        As (&r);
        return r;
    }
    template <>
    inline u16string String::As () const
    {
        return AsUTF16 ();
    }
    template <>
    inline u32string String::As () const
    {
        return AsUTF32 ();
    }
    template <>
    inline const wchar_t* String::As () const
    {
        _SafeReadRepAccessor accessor{this};
        return (const wchar_t*)accessor._ConstGetRep ()._Peek ();
    }
    template <>
    inline const Character* String::As () const
    {
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ()._Peek ();
    }
    template <>
    inline string String::AsUTF8 () const
    {
        string r;
        AsUTF8 (&r);
        return r;
    }
    inline string String::AsUTF8 () const
    {
        return AsUTF8<string> ();
    }
    inline void String::AsUTF8 (string* into) const
    {
        RequireNotNull (into);
        AsUTF8<string> (into);
    }
    inline u16string String::AsUTF16 () const
    {
        u16string r;
        AsUTF16 (&r);
        return r;
    }
    inline u32string String::AsUTF32 () const
    {
        u32string r;
        AsUTF32 (&r);
        return r;
    }
    template <>
    inline string String::AsASCII () const
    {
        string r;
        AsASCII (&r);
        return r;
    }
    inline string String::AsASCII () const
    {
        return AsASCII<string> ();
    }
    inline void String::AsASCII (string* into) const
    {
        RequireNotNull (into);
        AsASCII<string> (into);
    }
    inline string String::AsNarrowString (const locale& l) const
    {
        string result;
        AsNarrowString (l, &result);
        return result;
    }
    inline SDKString String::AsSDKString () const
    {
        SDKString result;
        AsSDKString (&result);
        return result;
    }
    inline string String::AsNarrowSDKString () const
    {
        string result;
        AsNarrowSDKString (&result);
        return result;
    }
    template <>
    inline pair<const Character*, const Character*> String::GetData () const
    {
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ().GetData ();
    }
    template <>
    inline pair<const wchar_t*, const wchar_t*> String::GetData () const
    {
        _SafeReadRepAccessor                     accessor{this};
        pair<const Character*, const Character*> p = accessor._ConstGetRep ().GetData ();
        return pair<const wchar_t*, const wchar_t*> (reinterpret_cast<const wchar_t*> (p.first), reinterpret_cast<const wchar_t*> (p.second));
    }
    inline size_t String::length () const
    {
        return GetLength ();
    }
    inline size_t String::size () const
    {
        return GetLength ();
    }
    inline const wchar_t* String::data () const
    {
        return As<const wchar_t*> ();
    }
    inline const wchar_t* String::c_str () const noexcept
    {
        const wchar_t* result = _SafeReadRepAccessor{this}._ConstGetRep ().c_str_peek ();
        EnsureNotNull (result);
        Ensure (result[GetLength ()] == '\0');
        return result;
    }
    inline size_t String::find (wchar_t c, size_t startAt) const
    {
        return Find (c, startAt, CompareOptions::eWithCase).value_or (npos);
    }
    inline size_t String::rfind (wchar_t c) const
    {
        return RFind (c).value_or (npos);
    }
    inline void String::push_back (wchar_t c)
    {
        Append (Character (c));
    }
    inline void String::push_back (Character c)
    {
        Append (c);
    }
    inline String String::substr (size_t from, size_t count) const
    {
        _SafeReadRepAccessor accessor{this};
        size_t               thisLen = accessor._ConstGetRep ()._GetLength ();
        if (from > thisLen)
            [[UNLIKELY_ATTR]]
            {
                Execution::Throw (out_of_range ("string index out of range"));
            }
        // @todo
        // Not QUITE correct - due to overflow issues, but pragmaitcally this is probably close enough
        size_t to = (count == npos) ? thisLen : (from + min (thisLen, count));
        return SubString_ (accessor, thisLen, from, to);
    }
    inline int String::Compare (const String& rhs, CompareOptions co) const
    {
        _SafeReadRepAccessor                     accessor{this};
        pair<const Character*, const Character*> l = accessor._ConstGetRep ().GetData ();
        _SafeReadRepAccessor                     rhsAccessor{&rhs};
        pair<const Character*, const Character*> r = rhsAccessor._ConstGetRep ().GetData ();
        return Character::Compare (l.first, l.second, r.first, r.second, co);
    }
    inline int String::Compare (const Character* rhs, CompareOptions co) const
    {
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        _SafeReadRepAccessor                     accessor{this};
        pair<const Character*, const Character*> l = accessor._ConstGetRep ().GetData ();
        return Character::Compare (l.first, l.second, reinterpret_cast<const Character*> (rhs), reinterpret_cast<const Character*> (rhs) + ::wcslen (reinterpret_cast<const wchar_t*> (rhs)), co);
    }
    inline int String::Compare (const Character* rhsStart, const Character* rhsEnd, CompareOptions co) const
    {
        _SafeReadRepAccessor                     accessor{this};
        pair<const Character*, const Character*> l = accessor._ConstGetRep ().GetData ();
        return Character::Compare (l.first, l.second, rhsStart, rhsEnd, co);
    }
    inline int String::Compare (const wchar_t* rhs, CompareOptions co) const
    {
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        _SafeReadRepAccessor                     accessor{this};
        pair<const Character*, const Character*> l = accessor._ConstGetRep ().GetData ();
        return Character::Compare (l.first, l.second, reinterpret_cast<const Character*> (rhs), reinterpret_cast<const Character*> (rhs) + ::wcslen (rhs), co);
    }
    inline int String::Compare (const wchar_t* rhsStart, const wchar_t* rhsEnd, CompareOptions co) const
    {
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        _SafeReadRepAccessor                     accessor{this};
        pair<const Character*, const Character*> l = accessor._ConstGetRep ().GetData ();
        return Character::Compare (l.first, l.second, reinterpret_cast<const Character*> (rhsStart), reinterpret_cast<const Character*> (rhsEnd), co);
    }
    inline bool String::Equals (const String& rhs, CompareOptions co) const
    {
        // OK in two steps because length test doesn't affect correctness - just performance
        //
        // @todo - reconsider and test - not sure this helps performance
        if (GetLength () != rhs.GetLength ()) {
            return false;
        }
        return Compare (rhs, co) == 0;
    }
    inline bool String::Equals (const wchar_t* rhs, CompareOptions co) const
    {
        return Compare (rhs, co) == 0;
    }

    /*
     ********************************************************************************
     ******************************* String::EqualToCI ******************************
     ********************************************************************************
     */
    inline bool String::EqualToCI::operator() (const String& lhs, const String& rhs) const
    {
        return lhs.Compare (rhs, CompareOptions::eCaseInsensitive) == 0;
    }

    /*
     ********************************************************************************
     ******************************* String::LessCI *********************************
     ********************************************************************************
     */
    inline bool String::LessCI::operator() (const String& lhs, const String& rhs) const
    {
        return lhs.Compare (rhs, CompareOptions::eCaseInsensitive) < 0;
    }

    /*
     ********************************************************************************
     *********************************** operator< **********************************
     ********************************************************************************
     */
    inline bool operator< (const String& lhs, const String& rhs)
    {
        return lhs.Compare (rhs) < 0;
    }
    inline bool operator< (const String& lhs, const wchar_t* rhs)
    {
        return lhs.Compare (rhs) < 0;
    }
    inline bool operator< (const wchar_t* lhs, const String& rhs)
    {
        RequireNotNull (lhs);
        bool result{rhs.Compare (lhs) > 0}; // reversed order to avoid construction of extra object
        Assert ((String (lhs).Compare (rhs) < 0) == result);
        return result;
    }

    /*
     ********************************************************************************
     *********************************** operator<= *********************************
     ********************************************************************************
     */
    inline bool operator<= (const String& lhs, const String& rhs)
    {
        return lhs.Compare (rhs) <= 0;
    }
    inline bool operator<= (const String& lhs, const wchar_t* rhs)
    {
        return lhs.Compare (rhs) <= 0;
    }
    inline bool operator<= (const wchar_t* lhs, const String& rhs)
    {
        RequireNotNull (lhs);
        bool result{rhs.Compare (lhs) >= 0}; // reversed order to avoid construction of extra object
        Assert ((String (lhs).Compare (rhs) <= 0) == result);
        return result;
    }

    /*
     ********************************************************************************
     *********************************** operator== *********************************
     ********************************************************************************
     */
    inline bool operator== (const String& lhs, const String& rhs)
    {
        return lhs.Equals (rhs);
    }
    inline bool operator== (const String& lhs, const wchar_t* rhs)
    {
        return lhs.Equals (rhs);
    }
    inline bool operator== (const wchar_t* lhs, const String& rhs)
    {
        RequireNotNull (lhs);
        return rhs.Equals (lhs); // revsered to avoid construction and because operator== is commutative
    }

    /*
     ********************************************************************************
     *********************************** operator!= *********************************
     ********************************************************************************
     */
    inline bool operator!= (const String& lhs, const String& rhs)
    {
        return not lhs.Equals (rhs);
    }
    inline bool operator!= (const String& lhs, const wchar_t* rhs)
    {
        return not lhs.Equals (rhs);
    }
    inline bool operator!= (const wchar_t* lhs, const String& rhs)
    {
        RequireNotNull (lhs);
        return not rhs.Equals (lhs); // revsered to avoid construction and because operator== is commutative
    }

    /*
     ********************************************************************************
     *********************************** operator>= *********************************
     ********************************************************************************
     */
    inline bool operator>= (const String& lhs, const String& rhs)
    {
        return lhs.Compare (rhs) >= 0;
    }
    inline bool operator>= (const String& lhs, const wchar_t* rhs)
    {
        return lhs.Compare (rhs) >= 0;
    }
    inline bool operator>= (const wchar_t* lhs, const String& rhs)
    {
        RequireNotNull (lhs);
        bool result{rhs.Compare (lhs) <= 0}; // reversed order to avoid construction of extra object
        Assert ((String (lhs).Compare (rhs) >= 0) == result);
        return result;
    }

    /*
     ********************************************************************************
     *********************************** operator> **********************************
     ********************************************************************************
     */
    inline bool operator> (const String& lhs, const String& rhs)
    {
        return lhs.Compare (rhs) > 0;
    }
    inline bool operator> (const String& lhs, const wchar_t* rhs)
    {
        return lhs.Compare (rhs) > 0;
    }
    inline bool operator> (const wchar_t* lhs, const String& rhs)
    {
        RequireNotNull (lhs);
        bool result{rhs.Compare (lhs) < 0}; // reversed order to avoid construction of extra object
        Assert ((String (lhs).Compare (rhs) > 0) == result);
        return result;
    }

    /*
     ********************************************************************************
     *********************************** operator+ **********************************
     ********************************************************************************
     */
    inline String operator+ (const String& lhs, const wchar_t* rhs)
    {
        RequireNotNull (rhs);
        return lhs.Concatenate (rhs);
    }
    inline String operator+ (const String& lhs, const String& rhs)
    {
        return lhs.Concatenate (rhs);
    }

    /*
     ********************************************************************************
     ********************************* String_ModuleInit_ ***************************
     ********************************************************************************
     */
    struct String_ModuleInit_ {
        String_ModuleInit_ ();
        Execution::ModuleDependency fBlockAllocationDependency;
    };
}

namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Characters::String_ModuleInit_> _Stroika_Foundation_Characters_String_ModuleInit_; // this object constructed for the CTOR/DTOR per-module side-effects
}

#endif // _Stroika_Foundation_Characters_String_inl_
