/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_String_inl_
#define _Stroika_Foundation_Characters_String_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <regex>

#include "../Debug/Assertions.h"
#include "../Execution/Throw.h"

#include "CString/Utilities.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ********************************* String::_IRep ********************************
     ********************************************************************************
     */
    inline String::_IRep::_IRep (const pair<const wchar_t*, const wchar_t*>& span)
        : _fStart{span.first}
        , _fEnd{span.second}
    {
    }
    inline String::_IRep::_IRep (const wchar_t* start, const wchar_t* end)
        : _fStart{start}
        , _fEnd{end}
    {
    }
    inline Character String::_IRep::GetAt (size_t index) const
    {
        Assert (_fStart <= _fEnd);
        Require (index < size ());
        return _fStart[index];
    }
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
        return (const Character*)_fStart;
    }
    inline auto String::_IRep::Find_equal_to (const Configuration::ArgByValueType<value_type>& v) const -> Traversal::Iterator<value_type>
    {
        return this->_Find_equal_to_default_implementation (v);
    }

    /*
     ********************************************************************************
     ************************************* String ***********************************
     ********************************************************************************
     */
    namespace Private_ {
        template <Character_Compatible SRC_T, Character_Compatible TRG_T>
        inline void CopyAsASCIICharacters_ (span<const SRC_T> src, span<TRG_T> trg)
        {
            Require (trg.size () >= src.size ());
            TRG_T* outI = trg.data ();
            for (auto ii = src.begin (); ii != src.end (); ++ii) {
                if constexpr (is_same_v<SRC_T, Character>) {
                    *outI++ = ii->GetAsciiCode ();
                }
                else {
                    *outI++ = static_cast<TRG_T> (*ii);
                }
            }
        }
    }
    template <Character_Compatible CHAR_T>
    auto String::mk_ (span<const CHAR_T> s) -> _SharedPtrIRep
    {
        if (s.empty ()) {
            return mkEmpty_ ();
        }
        if (Character::IsASCII (s)) {
            // if we already have ascii, just copy into a buffer that can be used for now with the legacy API, and
            // later specialized into something we construct a special rep for
            Memory::StackBuffer<wchar_t> buf{s.size ()};
#if qCompilerAndStdLib_spanOfContainer_Buggy
            Private_::CopyAsASCIICharacters_ (s, span{buf.data (), buf.size ()});
            return mk_ (span<const wchar_t>{buf.data (), buf.size ()});
#else
            Private_::CopyAsASCIICharacters_ (s, span{buf});
            return mk_ (span<const wchar_t>{buf}); // this case specialized
#endif
        }
        else {
            Memory::StackBuffer<wchar_t> buf{UTFConverter::ComputeTargetBufferSize<wchar_t> (s)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
            auto len = UTFConverter::kThe.Convert (s, span{buf.data (), buf.size ()}).fTargetProduced;
#else
            auto len = UTFConverter::kThe.Convert (s, span{buf}).fTargetProduced;
#endif
            Assert (len <= buf.size ());
            return mk_ (span<const wchar_t>{buf.data (), len}); // this case specialized
        }
    }
    DISABLE_COMPILER_MSC_WARNING_START (4244) // just logically needed around copy () call inside, but compiler spews warnings unless out here
    template <Character_Compatible CHAR_T>
    auto String::mk_ (span<const CHAR_T> s1, span<const CHAR_T> s2) -> _SharedPtrIRep
    {
        // Simplistic implementation, probably not too bad for most strings that fit in stack buffer, though involves
        // extra copy
        if (Character::IsASCII (s1) and Character::IsASCII (s2)) {
            Memory::StackBuffer<char> buf{s1.size () + s2.size ()};
            copy (s1.begin (), s1.end (), buf.data ());
            copy (s2.begin (), s2.end (), buf.data () + s1.size ()); // append
#if qCompilerAndStdLib_spanOfContainer_Buggy
            Private_::CopyAsASCIICharacters_ (s1, span{buf.data (), buf.size ()});
            Private_::CopyAsASCIICharacters_ (s2, span{buf.data (), buf.size ()}.subspan (s1.size ()));
            return mk_ (span<const char>{buf.data (), buf.size ()});
#else
            Private_::CopyAsASCIICharacters_ (s1, span{buf});
            Private_::CopyAsASCIICharacters_ (s2, span{buf}.subspan (s1.size ()));
            return mk_ (span<const char>{buf});
#endif
        }
        else {
            Memory::StackBuffer<wchar_t> buf{UTFConverter::ComputeTargetBufferSize<wchar_t> (s1) + UTFConverter::ComputeTargetBufferSize<wchar_t> (s2)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
            size_t len1 = UTFConverter::kThe.Convert (s1, span<wchar_t>{buf.data (), buf.size ()}).fTargetProduced;
            size_t len2 = UTFConverter::kThe.Convert (s2, span<wchar_t>{buf.data (), buf.size ()}.subspan (len1)).fTargetProduced;
#else
            size_t len1 = UTFConverter::kThe.Convert (s1, span{buf}).fTargetProduced;
            size_t len2 = UTFConverter::kThe.Convert (s2, span{buf}.subspan (len1)).fTargetProduced;
#endif
            return mk_ (span<const wchar_t>{buf.data (), len1 + len2});
        }
    }
    DISABLE_COMPILER_MSC_WARNING_END (4244)
    template <Character_Compatible CHAR_T>
    auto String::mk_ (Iterable<CHAR_T> it) -> _SharedPtrIRep
    {
        // redo with small stackbuffer (character and dont do iterable<Characer> do Iterable<CHAR_T> where t is Characer_Compiabple)
        // then unoicode covert and use other mk_ existing overloads
        Memory::StackBuffer<char32_t> r;
        it.Apply ([&r] (CHAR_T c) {
            if constexpr (is_same_v<CHAR_T, Character>) {
                r.push_back (static_cast<char32_t> (c)); // explicit operator char32_t to avoid ambiguities elsewhere
            }
            else {
                r.push_back (c);
            }
        });
        return mk_ (span<const char32_t>{r.data (), r.size ()});
    }
    // FOR NOW - INITIALLY - but later specialize for char and char32_t and probably lose this one
    template <>
    auto String::mk_ (span<const wchar_t> s) -> _SharedPtrIRep;

    inline String::String (const _SharedPtrIRep& rep) noexcept
        : inherited{rep}
    {
        _AssertRepValidType ();
    }
    inline String::String (_SharedPtrIRep&& rep) noexcept
        : inherited{move (rep)}
    {
        //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
        _AssertRepValidType ();
    }
    inline String::String ()
        : inherited{mkEmpty_ ()}
    {
        _AssertRepValidType ();
    }
    template <Character_SafelyCompatible CHAR_T>
    inline String::String (const CHAR_T* cString)
        : inherited{mk_ (span{cString, CString::Length (cString)})}
    {
        RequireNotNull (cString);
        _AssertRepValidType ();
    }
    template <Character_SafelyCompatible CHAR_T>
    inline String::String (span<CHAR_T> s)
        : inherited{mk_ (span<const CHAR_T>{s})}
    {
        _AssertRepValidType ();
    }
    template <Character_SafelyCompatible CHAR_T>
    inline String::String (const CHAR_T* from, const CHAR_T* to)
        : inherited{mk_ (span<const CHAR_T>{from, to})}
    {
    }
    template <Character_IsUnicodeCodePoint CHAR_T>
    inline String::String (const basic_string<CHAR_T>& s)
        : inherited{mk_ (span<const CHAR_T>{s.data (), s.size ()})}
    {
    }
    template <Character_SafelyCompatible CHAR_T>
    inline String::String (const Iterable<CHAR_T>& src)
        : inherited{mk_ (src)}
    {
    }
    inline String::String (const Character& c)
        : String{span{&c, 1}}
    {
    }
    inline String String::FromNarrowString (const char* from, const locale& l)
    {
        RequireNotNull (from);
        return FromNarrowString (span{from, ::strlen (from)}, l);
    }
    inline String String::FromNarrowString (const string& from, const locale& l)
    {
        return FromNarrowString (span{from.c_str (), from.length ()}, l);
    }
    template <Character_Compatible CHAR_T>
    inline String String::FromASCII (const CHAR_T* cString)
    {
        RequireNotNull (cString);
        return FromASCII (span{cString, CString::Length (cString)});
    }
    template <Character_Compatible CHAR_T>
    inline String String::FromASCII (span<const CHAR_T> s)
    {
        if (not Character::IsASCII (s)) {
            static const auto kException_ = out_of_range{"Error converting non-ascii text to String"};
            Execution::Throw (kException_);
        }
        // @todo redo using different rep
        Memory::StackBuffer<wchar_t> buf{Memory::eUninitialized, UTFConverter::kThe.ComputeTargetBufferSize<wchar_t> (s)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
        return String{span<const wchar_t>{buf.data (), UTFConverter::kThe.Convert (s, span{buf.data (), buf.size ()}).fTargetProduced}};
#else
        return String{span<const wchar_t>{buf.data (), UTFConverter::kThe.Convert (s, span{buf}).fTargetProduced}};
#endif
    }
    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    inline String String::FromISOLatin1 (const basic_string<CHAR_T>& s)
    {
        return FromISOLatin1 (span{s.data (), s.size ()});
    }
    template <Character_Compatible CHAR_T>
    inline String String::FromISOLatin1 (const CHAR_T* cString)
    {
        RequireNotNull (cString);
        return FromISOLatin1 (span{cString, CString::Length (cString)});
    }
    template <Character_Compatible CHAR_T>
    inline String String::FromISOLatin1 (span<const CHAR_T> s)
    {
        // @todo redo using different rep
        /*
         *  From http://unicodebook.readthedocs.io/encodings.html
         *      "For example, ISO-8859-1 are the first 256 Unicode code points (U+0000-U+00FF)."
         */
        const CHAR_T*                e = s.data () + s.size ();
        Memory::StackBuffer<wchar_t> buf{Memory::eUninitialized, static_cast<size_t> (e - s.data ())};
        wchar_t*                     pOut = buf.begin ();
        for (const CHAR_T* i = s.data (); i != e; ++i, ++pOut) {
            if constexpr (sizeof (CHAR_T) > 1) {
                if (*i >= 256) {
                    static const auto kException_ = out_of_range{"Error converting non-iso-latin-1 text to String"};
                    Execution::Throw (kException_);
                }
            }
            *pOut = *i;
        }
        return String{span<const wchar_t>{buf.begin (), pOut}};
    }
    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    inline String String::FromASCII (const basic_string<CHAR_T>& s)
    {
        return FromASCII (span{s.data (), s.size ()});
    }
    template <typename CHAR_T>
    String String::FromUTF8 (span<CHAR_T> s)
        requires (
            is_same_v<remove_cv_t<CHAR_T>, char8_t> or is_same_v<remove_cv_t<CHAR_T>, char>)
    {
        Memory::StackBuffer<wchar_t> buf{Memory::eUninitialized, UTFConverter::kThe.ComputeTargetBufferSize<wchar_t> (s)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
        return String{span<const wchar_t>{buf.data (), UTFConverter::kThe.Convert (s, span{buf.data (), buf.size ()}).fTargetProduced}};
#else
        return String{span<const wchar_t>{buf.data (), UTFConverter::kThe.Convert (s, span{buf}).fTargetProduced}};
#endif
    }
    template <typename CHAR_T>
    inline String String::FromUTF8 (basic_string<CHAR_T> from)
        requires (is_same_v<remove_cv_t<CHAR_T>, char8_t> or is_same_v<remove_cv_t<CHAR_T>, char>)
    {
        return FromUTF8 (span{from.c_str (), from.length ()});
    }
    template <typename CHAR_T>
    inline String String::FromUTF8 (const CHAR_T* from)
        requires (is_same_v<remove_cv_t<CHAR_T>, char8_t> or is_same_v<remove_cv_t<CHAR_T>, char>)
    {
        return FromUTF8 (span{from, ::strlen (reinterpret_cast<const char*> (from))});
    }
    inline String String::FromSDKString (const SDKChar* from)
    {
        RequireNotNull (from);
        return FromSDKString (span{from, CString::Length (from)});
    }
    inline String String::FromSDKString (span<const SDKChar> s)
    {
#if qTargetPlatformSDKUseswchar_t
        return String{s};
#else
        wstring w;
        NarrowStringToWide (s.data (), s.data () + s.size (), GetDefaultSDKCodePage (), &w);
        return String{move (w)};
#endif
    }
    inline String String::FromSDKString (const SDKString& from)
    {
        return FromSDKString (span{from.c_str (), from.length ()});
    }
    inline String String::FromNarrowSDKString (const char* from)
    {
        RequireNotNull (from);
        return FromNarrowSDKString (span{from, ::strlen (from)});
    }
    inline String String::FromNarrowSDKString (span<const char> s)
    {
        // @todo FIX PERFORMANCE
        wstring tmp;
        NarrowStringToWide (s.data (), s.data () + s.size (), GetDefaultSDKCodePage (), &tmp);
        return String{tmp};
    }
    inline String String::FromNarrowSDKString (const string& from)
    {
        return FromNarrowSDKString (span{from.c_str (), from.length ()});
    }
    template <typename T>
    String String::Concatenate (T&& rhs) const
        requires (is_convertible_v<T, String>)
    {
        // KISS for now - but this can and should be much more complex, dealing with ascii cases, etc...
        String                        rrhs = rhs;
        Memory::StackBuffer<char32_t> ignoredA;
        Memory::StackBuffer<char32_t> ignoredB;
        return mk_ (GetData<char32_t> (&ignoredA), rrhs.GetData<char32_t> (&ignoredB));
    }
    inline void String::_AssertRepValidType () const
    {
        EnsureMember (&_SafeReadRepAccessor{this}._ConstGetRep (), String::_IRep);
    }
    template <Character_Compatible CHAR_T>
    inline span<CHAR_T> String::CopyTo (span<CHAR_T> s) const
        requires (not is_const_v<CHAR_T>)
    {
        PeekSpanData psd = GetPeekSpanData<CHAR_T> ();
        if (auto p = PeekData<CHAR_T> (psd)) {
            Require (s.size_bytes () >= p->size_bytes ());
            (void)::memcpy (s.data (), p->data (), p->size_bytes ());
            return span<CHAR_T>{s.data (), p->size ()};
        }
        else {
            // OK, we need to UTF convert from the actual size we have to what the caller asked for
            switch (psd.fInCP) {
                case PeekSpanData::StorageCodePointType::eAscii: // maybe could optimize this case too
                case PeekSpanData::StorageCodePointType::eChar8:
                    return span<CHAR_T>{s.data (), UTFConverter::kThe.Convert (psd.fChar8, s).fTargetProduced};
                case PeekSpanData::StorageCodePointType::eChar16:
                    return span<CHAR_T>{s.data (), UTFConverter::kThe.Convert (psd.fChar16, s).fTargetProduced};
                case PeekSpanData::StorageCodePointType::eChar32:
                    return span<CHAR_T>{s.data (), UTFConverter::kThe.Convert (psd.fChar32, s).fTargetProduced};
                default:
                    AssertNotReached ();
                    return span<CHAR_T>{};
            }
        }
    }
    inline size_t String::size () const noexcept
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
    inline bool String::empty () const noexcept
    {
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ()._GetLength () == 0;
    }
    inline void String::clear ()
    {
        *this = String{};
    }
    namespace Private_ {
        // match index starts with 1 (and requres match.size () >=2)
        inline void ExtractMatches_ ([[maybe_unused]] const wsmatch& base_match, [[maybe_unused]] size_t currentUnpackIndex)
        {
        }
        template <typename... OPTIONAL_STRINGS>
        void ExtractMatches_ (const wsmatch& base_match, size_t currentUnpackIndex, optional<String>* subMatchI, OPTIONAL_STRINGS&&... remainingSubmatches)
        {
            if (currentUnpackIndex < base_match.size ()) {
                if (subMatchI != nullptr) {
                    *subMatchI = base_match[currentUnpackIndex].str ();
                }
                ExtractMatches_ (base_match, currentUnpackIndex + 1, forward<OPTIONAL_STRINGS> (remainingSubmatches)...);
            }
        }
        const wregex& RegularExpression_GetCompiled (const RegularExpression& regExp);
    }
    template <typename... OPTIONAL_STRINGS>
    bool String::Matches (const RegularExpression& regEx, OPTIONAL_STRINGS&&... subMatches) const
    {
        wstring tmp{As<wstring> ()};
        wsmatch base_match;
        if (regex_match (tmp, base_match, Private_::RegularExpression_GetCompiled (regEx))) {
            Private_::ExtractMatches_ (base_match, 1, forward<OPTIONAL_STRINGS> (subMatches)...);
            return true;
        }
        return false;
    }
    inline optional<size_t> String::Find (Character c, CompareOptions co) const
    {
        return Find (c, 0, co);
    }
    inline optional<size_t> String::Find (const String& subString, CompareOptions co) const
    {
        return Find (subString, 0, co);
    }
    inline Traversal::Iterator<Character> String::Find (const function<bool (Character item)>& that) const
    {
        return inherited::Find (that);
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
            *this                                         = String{mk_ (reinterpret_cast<const wchar_t*> (lhsD.first), reinterpret_cast<const wchar_t*> (lhsD.second), from, to)};
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
    inline const Character String::GetCharAt (size_t i) const noexcept
    {
        _SafeReadRepAccessor accessor{this};
        Require (i >= 0);
        Require (i < accessor._ConstGetRep ()._GetLength ());
        return accessor._ConstGetRep ().GetAt (i);
    }
    inline const Character String::operator[] (size_t i) const noexcept
    {
        Require (i >= 0);
        Require (i < size ());
        return GetCharAt (i);
    }

    template <typename T>
    inline T String::As () const
        requires (BasicUnicodeString<T> or is_same_v<T, String>)
    {
        T r{}; // for now - KISS, but this can be optimized
        As (&r);
        return r;
    }
    template <typename T>
    inline void String::As (T* into) const
        requires (BasicUnicodeString<T> or is_same_v<T, String>)
    {
        if constexpr (is_same_v<T, u8string>) {
            AsUTF8 (into);
        }
        else if constexpr (is_same_v<T, u16string>) {
            AsUTF16 (into);
        }
        else if constexpr (is_same_v<T, u32string>) {
            AsUTF32 (into);
        }
        else if constexpr (is_same_v<T, wstring>) {
            // @todo rewrite for new design
            RequireNotNull (into);
            _SafeReadRepAccessor accessor{this};
            size_t               n{accessor._ConstGetRep ()._GetLength ()};
            const Character*     cp = accessor._ConstGetRep ()._Peek ();
            Assert (sizeof (Character) == sizeof (wchar_t)); // going to want to clean this up!!!    --LGP 2011-09-01
            const wchar_t* wcp = (const wchar_t*)cp;
            into->assign (wcp, wcp + n);
        }
        else if constexpr (is_same_v<T, String>) {
            if (into != this) [[likely]] {
                *into = *this;
            }
        }
    }
    inline string String::AsNarrowString (const locale& l) const
    {
        string result;
        AsNarrowString (l, &result);
        return result;
    }
    template <typename T>
    inline T String::AsUTF8 () const
        requires (is_same_v<T, string> or is_same_v<T, u8string>)
    {
        Memory::StackBuffer<char8_t> maybeIgnoreBuf1;
        span<const char8_t>          thisData = GetData (&maybeIgnoreBuf1);
        return T{reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ()};
    }
    template <typename T>
    inline void String::AsUTF8 (T* into) const
        requires (is_same_v<T, string> or is_same_v<T, u8string>)
    {
        RequireNotNull (into);
        Memory::StackBuffer<char8_t> maybeIgnoreBuf1;
        span<const char8_t>          thisData = GetData (&maybeIgnoreBuf1);
        into->assign (reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ());
    }
    template <typename T>
    inline T String::AsUTF16 () const
        requires (is_same_v<T, u16string>)
    {
        Memory::StackBuffer<char16_t> maybeIgnoreBuf1;
        span<const char16_t>          thisData = GetData (&maybeIgnoreBuf1);
        return T{reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ()};
    }
    template <typename T>
    inline void String::AsUTF16 (T* into) const
        requires (is_same_v<T, u16string>)
    {
        RequireNotNull (into);
        Memory::StackBuffer<char16_t> maybeIgnoreBuf1;
        span<const char16_t>          thisData = GetData (&maybeIgnoreBuf1);
        into->assign (reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ());
    }
    template <typename T>
    inline T String::AsUTF32 () const
        requires (is_same_v<T, u32string>)
    {
        Memory::StackBuffer<char32_t> maybeIgnoreBuf1;
        span<const char32_t>          thisData = GetData (&maybeIgnoreBuf1);
        return T{reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ()};
    }
    template <typename T>
    inline void String::AsUTF32 (T* into) const
        requires (is_same_v<T, u32string>)
    {
        RequireNotNull (into);
        Memory::StackBuffer<char32_t> maybeIgnoreBuf1;
        span<const char32_t>          thisData = GetData (&maybeIgnoreBuf1);
        into->assign (reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ());
    }
    inline void String::AsSDKString (SDKString* into) const
    {
        RequireNotNull (into);
        Memory::StackBuffer<wchar_t> maybeIgnoreBuf1;
        span<const wchar_t>          thisData = GetData<wchar_t> (&maybeIgnoreBuf1);
        into->assign (thisData.begin (), thisData.end ());
#if qTargetPlatformSDKUseswchar_t
        into->assign (thisData.begin (), thisData.end ());
#else
        WideStringToNarrow (thisData.data (), thisData.data () + thisData.size (), GetDefaultSDKCodePage (), into);
#endif
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
    inline void String::AsNarrowSDKString (string* into) const
    {
        RequireNotNull (into);
        Memory::StackBuffer<wchar_t> maybeIgnoreBuf1;
        span<const wchar_t>          thisData = GetData<wchar_t> (&maybeIgnoreBuf1);
        WideStringToNarrow (thisData.data (), thisData.data () + thisData.size (), GetDefaultSDKCodePage (), into);
    }
    template <typename T>
    inline T String::AsASCII () const
        requires (is_same_v<T, string> or is_same_v<T, Memory::StackBuffer<char>>)
    {
        T r;
        AsASCII (&r);
        return r;
    }
    template <typename T>
    inline void String::AsASCII (T* into) const
        requires (is_same_v<T, string> or is_same_v<T, Memory::StackBuffer<char>>)
    {
        if (not AsASCIIQuietly (into)) {
            ThrowInvalidAsciiException_ ();
        }
    }
    template <typename T>
    inline bool String::AsASCIIQuietly (T* into) const
        requires (is_same_v<T, string> or is_same_v<T, Memory::StackBuffer<char>>)
    {
        RequireNotNull (into);
        String::_SafeReadRepAccessor             thisAccessor{this};
        pair<const Character*, const Character*> p = thisAccessor._ConstGetRep ().GetData ();
        return Character::AsASCIIQuietly (span<const Character>{p.first, p.second}, into);
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
    template <Character_Compatible CHAR_TYPE>
    inline String::PeekSpanData String::GetPeekSpanData () const
    {
        using StorageCodePointType = PeekSpanData::StorageCodePointType;
        StorageCodePointType preferredSCP{};
        if constexpr (is_same_v<remove_cv_t<CHAR_TYPE>, char8_t>) {
            preferredSCP = StorageCodePointType::eChar8;
        }
        else if constexpr (is_same_v<remove_cv_t<CHAR_TYPE>, char16_t>) {
            preferredSCP = StorageCodePointType::eChar16;
        }
        else if constexpr (is_same_v<remove_cv_t<CHAR_TYPE>, char32_t> or is_same_v<remove_cv_t<CHAR_TYPE>, Character>) {
            preferredSCP = StorageCodePointType::eChar32;
        }
        if constexpr (is_same_v<remove_cv_t<CHAR_TYPE>, wchar_t>) {
            if constexpr (sizeof (wchar_t) == 2) {
                preferredSCP = StorageCodePointType::eChar16;
            }
            else if constexpr (sizeof (wchar_t) == 4) {
                preferredSCP = StorageCodePointType::eChar32;
            }
        }
        else if constexpr (is_same_v<remove_cv_t<CHAR_TYPE>, Character>) {
            // later will map to char32_t, but for now same as wchar_t
            if constexpr (sizeof (wchar_t) == 2) {
                preferredSCP = StorageCodePointType::eChar16;
            }
            else if constexpr (sizeof (wchar_t) == 4) {
                preferredSCP = StorageCodePointType::eChar32;
            }
        }
        return _SafeReadRepAccessor{this}._ConstGetRep ().PeekData (preferredSCP);
    }
    template <Character_SafelyCompatible CHAR_TYPE>
    inline optional<span<const CHAR_TYPE>> String::PeekData (const PeekSpanData& pds)
    {
        using StorageCodePointType = PeekSpanData::StorageCodePointType;
        if constexpr (is_same_v<CHAR_TYPE, char>) {
            if (pds.fInCP == StorageCodePointType::eAscii) {
                return pds.fAscii;
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, char8_t>) {
            if (pds.fInCP == StorageCodePointType::eAscii or pds.fInCP == StorageCodePointType::eChar8) {
                return pds.fChar8;
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, char16_t>) {
            if (pds.fInCP == StorageCodePointType::eChar16) {
                return pds.fChar16;
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, char32_t>) {
            if (pds.fInCP == StorageCodePointType::eChar32) {
                return pds.fChar32;
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, wchar_t>) {
            if constexpr (sizeof (wchar_t) == 2) {
                if (pds.fInCP == StorageCodePointType::eChar16) {
                    if (pds.fChar16.empty ()) {
                        return span<const wchar_t>{};
                    }
                    return span<const wchar_t>{reinterpret_cast<const wchar_t*> (&*pds.fChar16.begin ()), pds.fChar16.size ()};
                }
            }
            else if constexpr (sizeof (wchar_t) == 4) {
                if (pds.fInCP == StorageCodePointType::eChar32) {
                    if (pds.fChar32.empty ()) {
                        return span<const wchar_t>{};
                    }
                    return span<const wchar_t>{reinterpret_cast<const wchar_t*> (&*pds.fChar32.begin ()), pds.fChar32.size ()};
                }
            }
            return span<const wchar_t>{};
        }
        else if constexpr (is_same_v<CHAR_TYPE, Character>) {
            // later will map to char32_t, but for now same as wchar_t
            if constexpr (sizeof (wchar_t) == 2) {
                if (pds.fInCP == StorageCodePointType::eChar16) {
                    if (pds.fChar16.empty ()) {
                        return span<const wchar_t>{};
                    }
                    return span<const Character>{reinterpret_cast<const Character*> (&*pds.fChar16.begin ()), pds.fChar16.size ()};
                }
            }
            else if constexpr (sizeof (wchar_t) == 4) {
                if (pds.fInCP == StorageCodePointType::eChar32) {
                    if (pds.fChar32.empty ()) {
                        return span<const wchar_t>{};
                    }
                    return span<const Character>{reinterpret_cast<const Character*> (&*pds.fChar32.begin ()), pds.fChar32.size ()};
                }
            }
            return span<const Character>{};
        }
        return nullopt; // can easily happen if you request a type that is not stored in the rep
    }
    template <Character_Compatible CHAR_TYPE>
    inline optional<span<const CHAR_TYPE>> String::PeekData () const
    {
        return PeekData<CHAR_TYPE> (GetPeekSpanData<CHAR_TYPE> ());
    }
    template <Character_SafelyCompatible CHAR_TYPE>
    span<const CHAR_TYPE> String::GetData (const PeekSpanData& pds, Memory::StackBuffer<CHAR_TYPE>* possiblyUsedBuffer)
    {
        RequireNotNull (possiblyUsedBuffer);
        using StorageCodePointType = PeekSpanData::StorageCodePointType;
        if constexpr (is_same_v<CHAR_TYPE, wchar_t>) {
            if constexpr (sizeof (CHAR_TYPE) == 2) {
                auto p = GetData<char16_t> (pds, reinterpret_cast<Memory::StackBuffer<char16_t>*> (possiblyUsedBuffer));
                if (p.empty ()) {
                    return span<const CHAR_TYPE>{};
                }
                return span<const CHAR_TYPE>{reinterpret_cast<const CHAR_TYPE*> (&*p.begin ()), p.size ()};
            }
            else if constexpr (sizeof (wchar_t) == 4) {
                auto p = GetData<char32_t> (pds, reinterpret_cast<Memory::StackBuffer<char32_t>*> (possiblyUsedBuffer));
                if (p.empty ()) {
                    return span<const CHAR_TYPE>{};
                }
                return span<const CHAR_TYPE>{reinterpret_cast<const CHAR_TYPE*> (&*p.begin ()), p.size ()};
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, Character>) {
            // later will map to char32_t, but for now same as wchar_t
            if constexpr (sizeof (wchar_t) == 2) {
                auto p = GetData<char16_t> (pds, reinterpret_cast<Memory::StackBuffer<char16_t>*> (possiblyUsedBuffer));
                if (p.empty ()) {
                    return span<const CHAR_TYPE>{};
                }
                return span<const CHAR_TYPE>{reinterpret_cast<const CHAR_TYPE*> (&*p.begin ()), p.size ()};
            }
            else if constexpr (sizeof (wchar_t) == 4) {
                auto p = GetData<char32_t> (pds, reinterpret_cast<Memory::StackBuffer<char32_t>*> (possiblyUsedBuffer));
                if (p.empty ()) {
                    return span<const CHAR_TYPE>{};
                }
                return span<const CHAR_TYPE>{reinterpret_cast<const CHAR_TYPE*> (&*p.begin ()), p.size ()};
            }
        }
        if constexpr (is_same_v<CHAR_TYPE, char8_t>) {
            switch (pds.fInCP) {
                case StorageCodePointType::eAscii:
                case StorageCodePointType::eChar8:
                    return pds.fChar8;
                case StorageCodePointType::eChar16: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar16));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    auto r = UTFConverter::kThe.Convert (pds.fChar16, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    auto r = UTFConverter::kThe.Convert (pds.fChar16, span{*possiblyUsedBuffer});
#endif
                    return span{possiblyUsedBuffer->data (), r.fTargetProduced};
                }
                case StorageCodePointType::eChar32: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar32));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    auto r = UTFConverter::kThe.Convert (pds.fChar32, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    auto r = UTFConverter::kThe.Convert (pds.fChar32, span{*possiblyUsedBuffer});
#endif
                    return span{possiblyUsedBuffer->data (), r.fTargetProduced};
                }
                default:
                    AssertNotReached ();
                    return span<const CHAR_TYPE>{};
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, char16_t>) {
            switch (pds.fInCP) {
                case StorageCodePointType::eAscii:
                case StorageCodePointType::eChar8: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar8));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    auto r = UTFConverter::kThe.Convert (pds.fChar8, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    auto r = UTFConverter::kThe.Convert (pds.fChar8, span{*possiblyUsedBuffer});
#endif
                    return span{possiblyUsedBuffer->data (), r.fTargetProduced};
                }
                case StorageCodePointType::eChar16:
                    return pds.fChar16;
                case StorageCodePointType::eChar32: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar32));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    auto r = UTFConverter::kThe.Convert (pds.fChar32, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    auto r = UTFConverter::kThe.Convert (pds.fChar32, span{*possiblyUsedBuffer});
#endif
                    return span{possiblyUsedBuffer->data (), r.fTargetProduced};
                }
                default:
                    AssertNotReached ();
                    return span<const CHAR_TYPE>{};
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, char32_t>) {
            switch (pds.fInCP) {
                case StorageCodePointType::eAscii:
                case StorageCodePointType::eChar8: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar8));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    auto r = UTFConverter::kThe.Convert (pds.fChar8, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    auto r = UTFConverter::kThe.Convert (pds.fChar8, span{*possiblyUsedBuffer});
#endif
                    return span{possiblyUsedBuffer->data (), r.fTargetProduced};
                }
                case StorageCodePointType::eChar16: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar16));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    auto r = UTFConverter::kThe.Convert (pds.fChar16, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    auto r = UTFConverter::kThe.Convert (pds.fChar16, span{*possiblyUsedBuffer});
#endif
                    return span{possiblyUsedBuffer->data (), r.fTargetProduced};
                }
                case StorageCodePointType::eChar32:
                    return pds.fChar32;
                default:
                    AssertNotReached ();
                    return span<const CHAR_TYPE>{};
            }
        }
    }
    template <Character_SafelyCompatible CHAR_TYPE>
    inline span<const CHAR_TYPE> String::GetData (Memory::StackBuffer<CHAR_TYPE>* possiblyUsedBuffer) const
    {
        RequireNotNull (possiblyUsedBuffer);
        return GetData (GetPeekSpanData<CHAR_TYPE> (), possiblyUsedBuffer);
    }
    inline size_t String::length () const noexcept
    {
        return size ();
    }
    inline const wchar_t* String::c_str () const noexcept
    {
        const wchar_t* result = _SafeReadRepAccessor{this}._ConstGetRep ().c_str_peek ();
        EnsureNotNull (result);
        Ensure (result[size ()] == '\0');
        return result;
    }
    inline const wchar_t* String::c_str ()
    {
        // @todo must totally re-impelemnt use switches rep!!!
        const wchar_t* result = (wchar_t*)_SafeReadRepAccessor{this}._ConstGetRep ().c_str_peek ();
        EnsureNotNull (result);
        Ensure (result[size ()] == '\0');
        return result;
    }
    inline tuple<const wchar_t*, wstring_view> String::c_str (Memory::StackBuffer<wchar_t>* possibleBackingStore) const
    {
        // @todo FIRST check if default impl already returns c_str () and just use it if we can. ONLY if that fails, do we
        // convert, and write to possibleBackingStore
        RequireNotNull (possibleBackingStore);
        // quickie weak implementation
        wstring tmp{As<wstring> ()};
        possibleBackingStore->resize_uninitialized (tmp.size () + 1);
        copy (tmp.begin (), tmp.end (), possibleBackingStore->begin ());
        (*possibleBackingStore)[tmp.length ()] = '\0'; // assure NUL-terminated
        return make_tuple (possibleBackingStore->begin (), wstring_view{possibleBackingStore->begin (), tmp.length ()});
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
        if (from > thisLen) [[unlikely]] {
            Execution::Throw (out_of_range{"string index out of range"});
        }
        // @todo
        // Not QUITE correct - due to overflow issues, but pragmaitcally this is probably close enough
        size_t to = (count == npos) ? thisLen : (from + min (thisLen, count));
        return SubString_ (accessor, thisLen, from, to);
    }
    inline strong_ordering String::operator<=> (const String& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs);
    }
    inline strong_ordering String::operator<=> (const wchar_t* rhs) const
    {
        return ThreeWayComparer{}(*this, rhs);
    }
    inline strong_ordering String::operator<=> (const wstring& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs);
    }
    inline strong_ordering String::operator<=> (const wstring_view& rhs) const
    {
        return ThreeWayComparer{}(*this, rhs);
    }
    inline bool String::operator== (const String& rhs) const
    {
        return EqualsComparer{}(*this, rhs);
    }
    inline bool String::operator== (const wchar_t* rhs) const
    {
        return EqualsComparer{}(*this, rhs);
    }
    inline bool String::operator== (const wstring& rhs) const
    {
        return EqualsComparer{}(*this, rhs);
    }
    inline bool String::operator== (const wstring_view& rhs) const
    {
        return EqualsComparer{}(*this, rhs);
    }

    /*
     ********************************************************************************
     **************************** String::EqualsComparer ****************************
     ********************************************************************************
     */
    constexpr String::EqualsComparer::EqualsComparer (CompareOptions co)
        : fCompareOptions{co}
    {
    }
    inline pair<const Character*, const Character*> String::EqualsComparer::Access_ (const wstring& s)
    {
        using namespace Stroika::Foundation::Traversal;
        return s.empty () ? make_pair<const Character*, const Character*> (nullptr, nullptr) : make_pair (reinterpret_cast<const Character*> (Iterator2Pointer (s.begin ())), reinterpret_cast<const Character*> (Iterator2Pointer (s.begin ()) + s.size ()));
    }
    inline pair<const Character*, const Character*> String::EqualsComparer::Access_ (const wstring_view& s)
    {
        using namespace Stroika::Foundation::Traversal;
        return s.empty () ? make_pair<const Character*, const Character*> (nullptr, nullptr) : make_pair (reinterpret_cast<const Character*> (Iterator2Pointer (s.begin ())), reinterpret_cast<const Character*> (Iterator2Pointer (s.begin ()) + s.size ()));
    }
    inline pair<const Character*, const Character*> String::EqualsComparer::Access_ (const Character* s)
    {
        using namespace Stroika::Foundation::Characters;
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        RequireNotNull (s);
        return make_pair (s, s + ::wcslen (reinterpret_cast<const wchar_t*> (s)));
    }
    inline pair<const Character*, const Character*> String::EqualsComparer::Access_ (const wchar_t* s)
    {
        using namespace Stroika::Foundation::Characters;
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        RequireNotNull (s);
        return make_pair (reinterpret_cast<const Character*> (s), reinterpret_cast<const Character*> (s) + ::wcslen (s));
    }
    inline pair<const Character*, const Character*> String::EqualsComparer::Access_ (const String& s)
    {
        using namespace Stroika::Foundation::Characters;
        return String::_SafeReadRepAccessor{&s}._ConstGetRep ().GetData ();
    }
    template <typename LT, typename RT>
    inline bool String::EqualsComparer::Cmp_ (LT lhs, RT rhs) const
    {
        using namespace Stroika::Foundation::Characters;
        pair<const Character*, const Character*> l = Access_ (lhs);
        pair<const Character*, const Character*> r = Access_ (rhs);
        return Character::Compare (l.first, l.second, r.first, r.second, fCompareOptions) == 0;
    }
    inline bool String::EqualsComparer::operator() (const String& lhs, const String& rhs) const
    {
        if (lhs.size () != rhs.size ()) {
            return false; // performance tweak
        }
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const String& lhs, const wstring& rhs) const
    {
        if (lhs.size () != rhs.size ()) {
            return false; // performance tweak
        }
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const String& lhs, const wstring_view& rhs) const
    {
        if (lhs.size () != rhs.size ()) {
            return false; // performance tweak
        }
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const String& lhs, const Character* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const String& lhs, const wchar_t* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wstring& lhs, const String& rhs) const
    {
        if (lhs.size () != rhs.size ()) {
            return false; // performance tweak
        }
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wstring_view& lhs, const String& rhs) const
    {
        if (lhs.size () != rhs.size ()) {
            return false; // performance tweak
        }
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wstring& lhs, const wstring_view& rhs) const
    {
        if (lhs.size () != rhs.size ()) {
            return false; // performance tweak
        }
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wstring_view& lhs, const wstring_view& rhs) const
    {
        if (lhs.size () != rhs.size ()) {
            return false; // performance tweak
        }
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wstring& lhs, const Character* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wstring_view& lhs, const Character* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wstring& lhs, const wchar_t* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wstring_view& lhs, const wchar_t* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const Character* lhs, const String& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const Character* lhs, const wstring& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const Character* lhs, const wstring_view& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const Character* lhs, const Character* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const Character* lhs, const wchar_t* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wchar_t* lhs, const String& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wchar_t* lhs, const wstring& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wchar_t* lhs, const wstring_view& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wchar_t* lhs, const Character* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline bool String::EqualsComparer::operator() (const wchar_t* lhs, const wchar_t* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }

    /*
     ********************************************************************************
     **************************** String::ThreeWayComparer **************************
     ********************************************************************************
     */
    constexpr String::ThreeWayComparer::ThreeWayComparer (CompareOptions co)
        : fCompareOptions{co}
    {
    }
    inline pair<const Character*, const Character*> String::ThreeWayComparer::Access_ (const wstring_view& s)
    {
        using namespace Stroika::Foundation::Characters;
        return s.empty ()
                   ? make_pair<const Character*, const Character*> (nullptr, nullptr)
                   : make_pair (reinterpret_cast<const Character*> (Traversal::Iterator2Pointer (s.begin ())), reinterpret_cast<const Character*> (Traversal::Iterator2Pointer (s.begin ()) + s.size ()));
    }
    inline pair<const Character*, const Character*> String::ThreeWayComparer::Access_ (const Character* s)
    {
        using namespace Stroika::Foundation::Characters;
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        RequireNotNull (s);
        return make_pair (s, s + ::wcslen (reinterpret_cast<const wchar_t*> (s)));
    }
    inline pair<const Character*, const Character*> String::ThreeWayComparer::Access_ (const wchar_t* s)
    {
        using namespace Stroika::Foundation::Characters;
        static_assert (sizeof (Character) == sizeof (wchar_t), "Character and wchar_t must be same size");
        RequireNotNull (s);
        return make_pair (reinterpret_cast<const Character*> (s), reinterpret_cast<const Character*> (s) + ::wcslen (s));
    }
    inline pair<const Character*, const Character*> String::ThreeWayComparer::Access_ (const String& s)
    {
        using namespace Stroika::Foundation::Characters;
        return String::_SafeReadRepAccessor{&s}._ConstGetRep ().GetData ();
    }
    template <typename LT, typename RT>
    inline strong_ordering String::ThreeWayComparer::Cmp_ (LT lhs, RT rhs) const
    {
        using namespace Stroika::Foundation::Characters;
        pair<const Character*, const Character*> l = Access_ (forward<LT> (lhs));
        pair<const Character*, const Character*> r = Access_ (forward<RT> (rhs));
        return Character::Compare (l.first, l.second, r.first, r.second, fCompareOptions);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const String& lhs, const String& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const wstring_view& lhs, const wstring_view& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const Character* lhs, const String& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const Character* lhs, const wstring_view& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const String& lhs, const Character* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const wstring_view& lhs, const Character* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const Character* lhs, const Stroika::Foundation::Characters::Character* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const wchar_t* lhs, const Stroika::Foundation::Characters::String& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const wchar_t* lhs, const wstring_view& rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const Stroika::Foundation::Characters::String& lhs, const wchar_t* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const wstring_view& lhs, const wchar_t* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }
    inline strong_ordering String::ThreeWayComparer::operator() (const wchar_t* lhs, const wchar_t* rhs) const
    {
        return Cmp_ (lhs, rhs);
    }

    /*
     ********************************************************************************
     **************************** String::LessComparer ******************************
     ********************************************************************************
     */
    constexpr String::LessComparer::LessComparer (CompareOptions co)
        : fComparer_{co}
    {
    }
    template <typename T1, typename T2>
    inline bool String::LessComparer::operator() (T1 lhs, T2 rhs) const
    {
        return fComparer_ (lhs, rhs) < 0;
    }

    /*
     ********************************************************************************
     *********************************** operator"" *********************************
     ********************************************************************************
     */
    inline String operator"" _ASCII (const char* str, size_t len)
    {
        // a future verison of this API may do something like String_Constant, which is why this API requires its arg is a
        // forever-lifetime C++ constant.
        return String::FromASCII (span{str, len});
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
}

namespace Stroika::Foundation::Traversal {
    /*
     *  LOGICALLY, we should put Iterable<T>::Join () String methods in Iterable.inl, but they must be defined in
     *  String.inl to avoid #include problems (that includes this anyhow, and this cannot include that because that includes this)
     */
    template <typename T>
    inline Characters::String Iterable<T>::Join (const function<Characters::String (const T&)>& convertToT, const function<Characters::String (const Characters::String&, const Characters::String&)>& combine) const
    {
        return Join<Characters::String> (convertToT, combine);
    }
    template <typename T>
    inline Characters::String Iterable<T>::Join (const function<Characters::String (const T&)>& convertToT, const Characters::String& separator) const
    {
        return Join (convertToT, [=] (const Characters::String& l, const Characters::String& r) { return l + separator + r; });
    }
    template <typename T>
    inline Characters::String Iterable<T>::Join (const function<Characters::String (const T&)>& convertToT) const
    {
        return Join (convertToT, L", "sv);
    }
    template <typename T>
    inline Characters::String Iterable<T>::Join (const Characters::String& separator) const
    {
        //return Join ([] (const auto& i) { return Characters::ToString (i); });    @todo figure out trick to be able to use Characters::ToString () here...
        return Join ([] (const auto& i) { return i.ToString (); }, [=] (const Characters::String& l, const Characters::String& r) { return l + separator + r; });
    }
    template <typename T>
    inline Characters::String Iterable<T>::Join () const
    {
        return Join (L", "sv);
    }
}

#endif // _Stroika_Foundation_Characters_String_inl_
