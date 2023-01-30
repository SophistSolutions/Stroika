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
#include "../Memory/Span.h"

#include "CString/Utilities.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     *************************** Characters::Private_ *******************************
     ********************************************************************************
     */
    namespace Private_ {
        template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
        static size_t StrLen_ (const CHAR_T* s)
        {
            if constexpr (is_same_v<CHAR_T, Character_Latin1>) {
                return StrLen_ (reinterpret_cast<const char*> (s));
            }
            else {
                return CString::Length (s);
            }
        }
        template <Character_UNICODECanUnambiguouslyConvertFrom SRC_T>
        inline void CopyAsASCIICharacters_ (span<const SRC_T> src, span<Character_ASCII> trg)
        {
            Require (trg.size () >= src.size ());
            Character_ASCII* outI = trg.data ();
            for (auto ii = src.begin (); ii != src.end (); ++ii) {
                if constexpr (is_same_v<SRC_T, Character>) {
                    *outI++ = ii->GetAsciiCode ();
                }
                else {
                    *outI++ = static_cast<Character_ASCII> (*ii);
                }
            }
        }
        template <Character_UNICODECanUnambiguouslyConvertFrom SRC_T>
        inline void CopyAsLatin1Characters_ (span<const SRC_T> src, span<Character_Latin1> trg)
        {
            Require (trg.size () >= src.size ());
            Character_Latin1* outI = trg.data ();
            for (auto ii = src.begin (); ii != src.end (); ++ii) {
                if constexpr (is_same_v<SRC_T, Character>) {
                    *outI++ = Character_Latin1{static_cast<unsigned char> (ii->GetCharacterCode ())};
                }
                else {
                    *outI++ = Character_Latin1{static_cast<unsigned char> (*ii)};
                }
            }
        }
        template <CanBeTreatedAsSpanOfCharacter_ USTRING>
        inline span<const Character> AsSpanOfCharacters_ (USTRING&& s, Memory::StackBuffer<Character>* mostlyIgnoredBuf)
        {
            /*
             * Genericly convert the argument to a span<const Character> object; for a string, complex and requires
             * a function call (GetData) and ESSENTIALLY optional mostlyIgnoredBuf argument. For most other types
             * mostlyIgnoredBuf is ignored.
             * 
             * This must be highly optimized as its used in critical locations, to quickly access argument data and
             * convert it into a usable form.
             */
            if constexpr (is_base_of_v<String, decay_t<USTRING>>) {
                return s.GetData (mostlyIgnoredBuf);
            }
            else if constexpr (is_same_v<decay_t<USTRING>, const char32_t*> or
                               (sizeof (wchar_t) == sizeof (Character) and is_same_v<decay_t<USTRING>, const wchar_t*>)) {
                return span{reinterpret_cast<const Character*> (s), CString::Length (s)};
            }
            else if constexpr (is_same_v<decay_t<USTRING>, u32string> or
                               (sizeof (wchar_t) == sizeof (Character) and is_same_v<decay_t<USTRING>, wstring>)) {
                return span{reinterpret_cast<const Character*> (s.c_str ()), s.length ()};
            }
            else if constexpr (is_same_v<decay_t<USTRING>, u32string_view> or
                               (sizeof (wchar_t) == sizeof (Character) and is_same_v<decay_t<USTRING>, wstring_view>)) {
                return span{reinterpret_cast<const Character*> (s.data ()), s.length ()};
            }
            else if constexpr (is_same_v<decay_t<USTRING>, const char8_t*> or is_same_v<decay_t<USTRING>, const char16_t*> or
                               is_same_v<decay_t<USTRING>, const wchar_t*>) {
                span spn{s, CString::Length (s)};
                mostlyIgnoredBuf->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<Character> (spn));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                return UTFConverter::kThe.ConvertSpan (spn, span{mostlyIgnoredBuf->data (), mostlyIgnoredBuf->size ()});
#else
                return UTFConverter::kThe.ConvertSpan (spn, span{*mostlyIgnoredBuf});
#endif
            }
            else if constexpr (is_same_v<decay_t<USTRING>, u8string> or is_same_v<decay_t<USTRING>, u16string> or is_same_v<decay_t<USTRING>, wstring>) {
                span spn{s.data (), s.size ()};
                mostlyIgnoredBuf->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<Character> (spn));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                return UTFConverter::kThe.ConvertSpan (spn, span{mostlyIgnoredBuf->data (), mostlyIgnoredBuf->size ()});
#else
                return UTFConverter::kThe.ConvertSpan (spn, span{*mostlyIgnoredBuf});
#endif
            }
            else if constexpr (is_same_v<decay_t<USTRING>, u8string_view> or is_same_v<decay_t<USTRING>, u16string_view> or
                               is_same_v<decay_t<USTRING>, wstring_view>) {
                span spn{s.data (), s.size ()};
                mostlyIgnoredBuf->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<Character> (spn));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                return UTFConverter::kThe.ConvertSpan (spn, span{mostlyIgnoredBuf->data (), mostlyIgnoredBuf->size ()});
#else
                return UTFConverter::kThe.ConvertSpan (spn, span{*mostlyIgnoredBuf});
#endif
            }
            else {
                // else must copy data to mostlyIgnoredBuf and use that, so just need a span
                span spn{s}; // tricky part
                mostlyIgnoredBuf->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<Character> (spn));
                return UTFConverter::kThe.ConvertSpan (spn);
            }
        }
    }

    /*
     ********************************************************************************
     ************************************* String ***********************************
     ********************************************************************************
     */
    // Since we don't mix spans of single/2-3-4 byte chars in a single rep (would make char indexing too expensive)
    // just specialize 3 cases - ASCII (char), utf-16, and utf-32 (others - like char8_t, wchar_t mappeed appropriately)
#if !qCompilerAndStdLib_template_requresDefNeededonSpecializations_Buggy
    template <>
    auto String::mk_nocheck_ (span<const Character_ASCII> s) -> _SharedPtrIRep;
    template <>
    auto String::mk_nocheck_ (span<const Character_Latin1> s) -> _SharedPtrIRep;
    template <>
    auto String::mk_nocheck_ (span<const char16_t> s) -> _SharedPtrIRep;
    template <>
    auto String::mk_nocheck_ (span<const char32_t> s) -> _SharedPtrIRep;
#endif
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    auto String::mk_ (span<const CHAR_T> s) -> _SharedPtrIRep
    {
        if (s.empty ()) {
            return mkEmpty_ ();
        }
        if constexpr (is_same_v<CHAR_T, Character_ASCII>) {
            Character::CheckASCII (s);
            return mk_nocheck_ (s);
        }
        else if constexpr (is_same_v<CHAR_T, Character_Latin1>) {
            Character::CheckLatin1 (s);
            return mk_nocheck_ (s);
        }
        switch (Character::IsASCIIOrLatin1 (s)) {
            case Character::ASCIIOrLatin1Result::eASCII: {
                if constexpr (sizeof (CHAR_T) == 1) {
                    return mk_nocheck_ (span<const Character_ASCII>{reinterpret_cast<const Character_ASCII*> (s.data ()), s.size ()});
                }
                else {
                    // Copy to smaller buffer (e.g. utf16_t to char)
                    Memory::StackBuffer<Character_ASCII> buf{Memory::eUninitialized, s.size ()};
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    Private_::CopyAsASCIICharacters_ (s, span{buf.data (), buf.size ()});
                    return mk_nocheck_ (span<const Character_ASCII>{buf.data (), buf.size ()});
#else
                    Private_::CopyAsASCIICharacters_ (s, span{buf});
                    return mk_nocheck_ (span<const Character_ASCII>{buf});
#endif
                }
            }
            case Character::ASCIIOrLatin1Result::eLatin1: {
                if constexpr (sizeof (CHAR_T) == 1) {
                    return mk_nocheck_ (span<const Character_Latin1>{reinterpret_cast<const Character_Latin1*> (s.data ()), s.size ()});
                }
                else {
                    // Copy to smaller buffer (e.g. utf32_t to Character_Latin1)
                    Memory::StackBuffer<Character_Latin1> buf{Memory::eUninitialized, s.size ()};
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    Private_::CopyAsLatin1Characters_ (s, span{buf.data (), buf.size ()});
                    return mk_nocheck_ (span<const Character_Latin1>{buf.data (), buf.size ()});
#else
                    Private_::CopyAsLatin1Characters_ (s, span{buf});
                    return mk_nocheck_ (span<const Character_Latin1>{buf});
#endif
                }
            }
        }
        // at this point, we know the text must be encoded as utf16 or utf32 (but source code still be single byte, like utf8)
        if (UTFConverter::AllFitsInTwoByteEncoding (s)) {
            if constexpr (sizeof (CHAR_T) == 2) {
                // no transcode needed UTF16->UTF16
                return mk_nocheck_ (span<const char16_t>{reinterpret_cast<const char16_t*> (s.data ()), s.size ()});
            }
            else {
                // complex case - could be utf8 src, utf16, or utf32, so must transcode to char16_t
                Memory::StackBuffer<char16_t> wideUnicodeBuf{Memory::eUninitialized, UTFConverter::ComputeTargetBufferSize<char16_t> (s)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
                return mk_nocheck_ (Memory::ConstSpan (UTFConverter::kThe.ConvertSpan (s, span{wideUnicodeBuf.data (), wideUnicodeBuf.size ()})));
#else
                return mk_nocheck_ (Memory::ConstSpan (UTFConverter::kThe.ConvertSpan (s, span{wideUnicodeBuf})));
#endif
            }
        }
        // So at this point - definitely converting to UTF-32
        if constexpr (sizeof (CHAR_T) == 4) {
            // Easy, just cast
            return mk_nocheck_ (span<const char32_t>{reinterpret_cast<const char32_t*> (s.data ()), s.size ()});
        }
        else {
            // converting utf8 or utf16 with surrogates to utf32
            Memory::StackBuffer<char32_t> wideUnicodeBuf{Memory::eUninitialized, UTFConverter::ComputeTargetBufferSize<char32_t> (s)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
            return mk_nocheck_ (Memory::ConstSpan (UTFConverter::kThe.ConvertSpan (s, span{wideUnicodeBuf.data (), wideUnicodeBuf.size ()})));
#else
            return mk_nocheck_ (Memory::ConstSpan (UTFConverter::kThe.ConvertSpan (s, span{wideUnicodeBuf})));
#endif
        }
    }
    template <Character_Compatible CHAR_T>
    auto String::mk_ (span<CHAR_T> s) -> _SharedPtrIRep
    {
        // weird and unfortunate overload needed for non-const spans, not automatically promoted to const
        return mk_ (Memory::ConstSpan (s));
    }
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
        return mk_ (span{r.data (), r.size ()});
    }
    template <>
    auto String::mk_ (basic_string<char>&& s) -> _SharedPtrIRep;
    template <>
    auto String::mk_ (basic_string<char16_t>&& s) -> _SharedPtrIRep;
    template <>
    auto String::mk_ (basic_string<char32_t>&& s) -> _SharedPtrIRep;
    template <>
    auto String::mk_ (basic_string<wchar_t>&& s) -> _SharedPtrIRep;
    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    inline auto String::mk_ (basic_string<CHAR_T>&& s) -> _SharedPtrIRep
    {
        // by default, except for maybe a few special cases, just copy the data - don't move
        return mk_ (span{s.begin (), s.size ()});
    }
    inline String::String (const _SharedPtrIRep& rep) noexcept
        : inherited{rep}
    {
        _AssertRepValidType ();
    }
    inline String::String (_SharedPtrIRep&& rep) noexcept
        : inherited{(RequireNotNull (rep), move (rep))}
    {
        _AssertRepValidType ();
    }
    inline String::String ()
        : inherited{mkEmpty_ ()}
    {
        _AssertRepValidType ();
    }
    template <Character_Compatible CHAR_T>
    inline String::String (const CHAR_T* cString)
        : inherited{mk_ (span{cString, CString::Length (cString)})}
    {
        RequireNotNull (cString);
        _AssertRepValidType ();
    }
    template <Character_Compatible CHAR_T>
    inline String::String (span<const CHAR_T> s)
        : inherited{mk_ (span<const CHAR_T>{s})}
    {
        _AssertRepValidType ();
    }
    template <Character_Compatible CHAR_T>
    inline String::String (span<CHAR_T> s)
        : inherited{mk_ (Memory::ConstSpan (s))}
    {
        _AssertRepValidType ();
    }
    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    inline String::String (const basic_string<CHAR_T>& s)
        : inherited{mk_ (span<const CHAR_T>{s.data (), s.size ()})}
    {
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_T>
    inline String::String (const Iterable<CHAR_T>& src)
        : inherited{mk_ (src)}
    {
    }
    inline String::String (const Character& c)
        : String{span{&c, 1}}
    {
    }
    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    inline String::String (basic_string<CHAR_T>&& s)
        : inherited{mk_ (forward<basic_string<CHAR_T>> (s))}
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
    template <Character_IsUnicodeCodePointOrPlainChar CHAR_T>
    inline String String::FromLatin1 (const basic_string<CHAR_T>& s)
    {
        return FromLatin1 (span{s.data (), s.size ()});
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline String String::FromLatin1 (const CHAR_T* cString)
    {
        RequireNotNull (cString);
        return FromLatin1 (span{cString, Private_::StrLen_ (cString)});
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline String String::FromLatin1 (span<const CHAR_T> s)
    {
        /*
         *  From http://unicodebook.readthedocs.io/encodings.html
         *      "For example, ISO-8859-1 are the first 256 Unicode code points (U+0000-U+00FF)."
         */
        if constexpr (sizeof (CHAR_T) == 1) {
            return mk_ (span<const Character_Latin1>{reinterpret_cast<const Character_Latin1*> (s.data ()), s.size ()});
        }
        else {
            const CHAR_T*                         b = reinterpret_cast<const CHAR_T*> (s.data ());
            const CHAR_T*                         e = b + s.size ();
            Memory::StackBuffer<Character_Latin1> buf{Memory::eUninitialized, static_cast<size_t> (e - b)};
            Character_Latin1*                     pOut = buf.begin ();
            for (const CHAR_T* i = b; i != e; ++i, ++pOut) {
                if (*i >= 256) {
                    static const auto kException_ = out_of_range{"Error converting non-iso-latin-1 text to String"};
                    Execution::Throw (kException_);
                }
                *pOut = *i;
            }
            return mk_ (span<const Character_Latin1>{buf.begin (), pOut});
        }
    }
    template <size_t SIZE>
    inline String String::FromStringConstant (const Character_ASCII (&cString)[SIZE])
    {
        return FromStringConstant (span<const Character_ASCII>{cString, SIZE - 1}); // -1 because a literal array SIZE includes the NUL-character at the end
    }
    template <size_t SIZE>
    inline String String::FromStringConstant (const wchar_t (&cString)[SIZE])
    {
        return FromStringConstant (span<const wchar_t>{cString, SIZE - 1}); // -1 because a literal array SIZE includes the NUL-character at the end
    }
    inline String String::FromStringConstant (const basic_string_view<char>& str)
    {
        return FromStringConstant (span<const char>{str.data (), str.size ()});
    }
    inline String String::FromStringConstant (const basic_string_view<wchar_t>& str)
    {
        return FromStringConstant (span<const wchar_t>{str.data (), str.size ()});
    }
    inline String String::FromStringConstant (const basic_string_view<char32_t>& str)
    {
        return FromStringConstant (span<const char32_t>{str.data (), str.size ()});
    }
    template <typename CHAR_T>
    inline String String::FromUTF8 (span<CHAR_T> s)
        requires (is_same_v<remove_cv_t<CHAR_T>, char8_t> or is_same_v<remove_cv_t<CHAR_T>, char>)
    {
        if (Character::IsASCII (s)) {
            return mk_ (span<const char>{reinterpret_cast<const char*> (s.data ()), s.size ()});
        }
        if (UTFConverter::AllFitsInTwoByteEncoding (s)) {
            Memory::StackBuffer<char16_t> buf{Memory::eUninitialized, UTFConverter::kThe.ComputeTargetBufferSize<char16_t> (s)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
            return String{UTFConverter::kThe.ConvertSpan (s, span{buf.data (), buf.size ()})};
#else
            return String{UTFConverter::kThe.ConvertSpan (s, span{buf})};
#endif
        }
        else {
            Memory::StackBuffer<char32_t> buf{Memory::eUninitialized, UTFConverter::kThe.ComputeTargetBufferSize<char32_t> (s)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
            return String{UTFConverter::kThe.ConvertSpan (s, span{buf.data (), buf.size ()})};
#else
            return String{UTFConverter::kThe.ConvertSpan (s, span{buf})};
#endif
        }
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
    inline String String::FromSDKString (const SDKString& from) { return FromSDKString (span{from.c_str (), from.length ()}); }
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
    inline String String::FromNarrowSDKString (const string& from) { return FromNarrowSDKString (span{from.c_str (), from.length ()}); }
    template <typename T>
    String String::Concatenate (T&& rhs) const
        requires (is_convertible_v<T, String>)
    {
        // KISS for now - but this can and should be much more complex, dealing with ascii cases, etc...
        String                        rrhs = rhs;
        Memory::StackBuffer<char32_t> ignoredA;
        span                          leftSpan = GetData (&ignoredA);
        Memory::StackBuffer<char32_t> ignoredB;
        span                          rightSpan = rrhs.GetData (&ignoredB);
        Memory::StackBuffer<char32_t> buf{Memory::eUninitialized, leftSpan.size () + rightSpan.size ()};
        copy (leftSpan.begin (), leftSpan.end (), buf.data ());
        copy (rightSpan.begin (), rightSpan.end (), buf.data () + leftSpan.size ());
#if qCompilerAndStdLib_spanOfContainer_Buggy
        return mk_ (span{buf.data (), buf.size ()});
#else
        return mk_ (span{buf});
#endif
    }
    inline void String::_AssertRepValidType () const { EnsureMember (&_SafeReadRepAccessor{this}._ConstGetRep (), String::_IRep); }
    template <Character_Compatible CHAR_T>
    inline span<CHAR_T> String::CopyTo (span<CHAR_T> s) const
        requires (not is_const_v<CHAR_T>)
    {
        PeekSpanData psd = GetPeekSpanData<CHAR_T> ();
        if (auto p = PeekData<CHAR_T> (psd)) {
            return Memory::CopySpanData (*p, s);
        }
        else {
            // OK, we need to UTF convert from the actual size we have to what the caller asked for
            switch (psd.fInCP) {
                case PeekSpanData::StorageCodePointType::eAscii: // maybe could optimize this case too
                case PeekSpanData::StorageCodePointType::eSingleByteLatin1:
                    return UTFConverter::kThe.ConvertSpan (psd.fSingleByteLatin1, s);
                case PeekSpanData::StorageCodePointType::eChar16:
                    return UTFConverter::kThe.ConvertSpan (psd.fChar16, s);
                case PeekSpanData::StorageCodePointType::eChar32:
                    return UTFConverter::kThe.ConvertSpan (psd.fChar32, s);
                default:
                    AssertNotReached ();
                    return span<CHAR_T>{};
            }
        }
    }
    inline size_t String::size () const noexcept { return _SafeReadRepAccessor{this}._ConstGetRep ().size (); }
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
        size_t               myLength{accessor._ConstGetRep ().size ()};
        size_t               f = SubString_adjust_ (from, myLength);
        size_t               t = myLength;
        Require (f <= myLength);
        return SubString_ (accessor, myLength, f, t);
    }
    template <typename SZ1, typename SZ2>
    inline String String::SubString (SZ1 from, SZ2 to) const
    {
        _SafeReadRepAccessor accessor{this};
        size_t               myLength{accessor._ConstGetRep ().size ()};
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
        size_t               myLength{accessor._ConstGetRep ().size ()};
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
        size_t               myLength{accessor._ConstGetRep ().size ()};
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
    inline String String::RemoveAt (size_t charAt) const { return RemoveAt (charAt, charAt + 1); }
    inline String String::RemoveAt (pair<size_t, size_t> fromTo) const { return RemoveAt (fromTo.first, fromTo.second); }
    inline bool   String::empty () const noexcept
    {
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ().size () == 0;
    }
    inline void String::clear () { *this = String{}; }
    namespace Private_ {
        // match index starts with 1 (and requres match.size () >=2)
        inline void ExtractMatches_ ([[maybe_unused]] const wsmatch& base_match, [[maybe_unused]] size_t currentUnpackIndex) {}
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
    inline optional<size_t> String::Find (Character c, CompareOptions co) const { return Find (c, 0, co); }
    inline optional<size_t> String::Find (const String& subString, CompareOptions co) const { return Find (subString, 0, co); }
    inline Traversal::Iterator<Character> String::Find (const function<bool (Character item)>& that) const
    {
        return inherited::Find (that);
    }
    inline bool   String::Contains (Character c, CompareOptions co) const { return static_cast<bool> (Find (c, co)); }
    inline bool   String::Contains (const String& subString, CompareOptions co) const { return static_cast<bool> (Find (subString, co)); }
    inline String String::Replace (pair<size_t, size_t> fromTo, const String& replacement) const
    {
        return Replace (fromTo.first, fromTo.second, replacement);
    }
    inline String String::InsertAt (Character c, size_t at) const { return InsertAt (span<const Character>{&c, 1}, at); }
    inline String String::InsertAt (const String& s, size_t at) const
    {
        Memory::StackBuffer<Character> ignored1;
        return InsertAt (s.GetData (&ignored1), at);
    }
    inline String String::InsertAt (span<Character> s, size_t at) const { return InsertAt (Memory::ConstSpan (s), at); }
    template <typename CHAR_T>
    inline void String::Append (span<const CHAR_T> s)
        requires (is_same_v<CHAR_T, Character> or is_same_v<CHAR_T, char32_t>)
    {
        if (not s.empty ()) {
            Memory::StackBuffer<char32_t> ignored1;
            span<const char32_t>          thisSpan = this->GetData (&ignored1);
            Memory::StackBuffer<char32_t> combinedBuf{Memory::eUninitialized, thisSpan.size () + s.size ()};
#if qCompilerAndStdLib_spanOfContainer_Buggy
            Memory::CopySpanData (thisSpan, span{combinedBuf.data (), combinedBuf.size ()});
#else
            Memory::CopySpanData (thisSpan, span{combinedBuf});
#endif
            char32_t* write2Buf = combinedBuf.data () + thisSpan.size ();
            for (auto i : s) {
                if constexpr (is_same_v<CHAR_T, Character>) {
                    *write2Buf = i.template As<char32_t> ();
                }
                else {
                    *write2Buf = i;
                }
                ++write2Buf;
            }
#if qCompilerAndStdLib_spanOfContainer_Buggy
            *this = mk_ (span{combinedBuf.data (), combinedBuf.size ()});
#else
            *this = mk_ (span{combinedBuf});
#endif
        }
    }
    inline void String::Append (const wchar_t* from, const wchar_t* to)
    {
        Require (from <= to);
        if (from != to) {
            Memory::StackBuffer<wchar_t> ignored1;
            span<const wchar_t>          thisSpan = this->GetData (&ignored1);
            Memory::StackBuffer<wchar_t> buf{Memory::eUninitialized, thisSpan.size () + (to - from)};
#if qCompilerAndStdLib_spanOfContainer_Buggy
            span<wchar_t> bufSpan{buf.data (), buf.size ()};
#else
            span<wchar_t> bufSpan{buf};
#endif
            Memory::CopySpanData (thisSpan, bufSpan);
            Memory::CopySpanData (span{from, to}, bufSpan.subspan (thisSpan.size ()));
            *this = mk_ (bufSpan);
        }
    }
    inline void String::Append (Character c) { Append (&c, &c + 1); }
    inline void String::Append (const String& s)
    {
        Memory::StackBuffer<char32_t> ignored1;
        auto                          rhsSpan = s.GetData (&ignored1);
        Append (rhsSpan);
    }
    inline void    String::Append (const wchar_t* s) { Append (s, s + ::wcslen (s)); }
    inline void    String::Append (const Character* from, const Character* to) { Append (span{from, to}); }
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
        Require (i < accessor._ConstGetRep ().size ());
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
        requires (BasicUnicodeStdString<T> or is_same_v<T, String>)
    {
        T r{}; // for now - KISS, but this can be optimized
        As (&r);
        return r;
    }
    template <typename T>
    inline void String::As (T* into) const
        requires (BasicUnicodeStdString<T> or is_same_v<T, String>)
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
            if constexpr (sizeof (wchar_t) == 2) {
                AsUTF16 (into);
            }
            else {
                AsUTF32 (into);
            }
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
        requires (is_same_v<T, u16string> or (sizeof (wchar_t) == sizeof (char16_t) and is_same_v<T, wstring>))
    {
        Memory::StackBuffer<char16_t> maybeIgnoreBuf1;
        span<const char16_t>          thisData = GetData (&maybeIgnoreBuf1);
        return T{reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ()};
    }
    template <typename T>
    inline void String::AsUTF16 (T* into) const
        requires (is_same_v<T, u16string> or (sizeof (wchar_t) == sizeof (char16_t) and is_same_v<T, wstring>))
    {
        RequireNotNull (into);
        Memory::StackBuffer<char16_t> maybeIgnoreBuf1;
        span<const char16_t>          thisData = GetData (&maybeIgnoreBuf1);
        into->assign (reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ());
    }
    template <typename T>
    inline T String::AsUTF32 () const
        requires (is_same_v<T, u32string> or (sizeof (wchar_t) == sizeof (char32_t) and is_same_v<T, wstring>))
    {
        Memory::StackBuffer<char32_t> maybeIgnoreBuf1;
        span<const char32_t>          thisData = GetData (&maybeIgnoreBuf1);
        return T{reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ()};
    }
    template <typename T>
    inline void String::AsUTF32 (T* into) const
        requires (is_same_v<T, u32string> or (sizeof (wchar_t) == sizeof (char32_t) and is_same_v<T, wstring>))
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
        span<const wchar_t>          thisData = GetData (&maybeIgnoreBuf1);
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
        span<const wchar_t>          thisData = GetData (&maybeIgnoreBuf1);
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
        Memory::StackBuffer<wchar_t> ignored1;
        auto                         thisSpan = GetData (&ignored1);
        return Character::AsASCIIQuietly (thisSpan, into);
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_TYPE>
    inline String::PeekSpanData String::GetPeekSpanData () const
    {
        using StorageCodePointType = PeekSpanData::StorageCodePointType;
        StorageCodePointType preferredSCP{};
        if constexpr (is_same_v<remove_cv_t<CHAR_TYPE>, Character_ASCII>) {
            preferredSCP = StorageCodePointType::eAscii;
        }
        else if constexpr (is_same_v<remove_cv_t<CHAR_TYPE>, Character_Latin1>) {
            preferredSCP = StorageCodePointType::eSingleByteLatin1;
        }
        else if constexpr (is_same_v<remove_cv_t<CHAR_TYPE>, char8_t>) {
            preferredSCP = StorageCodePointType::eAscii; // not clear what's best in this case but probably doesn't matter
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
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_TYPE>
    inline optional<span<const CHAR_TYPE>> String::PeekData (const PeekSpanData& pds)
    {
        using StorageCodePointType = PeekSpanData::StorageCodePointType;
        if constexpr (is_same_v<CHAR_TYPE, Character_ASCII>) {
            if (pds.fInCP == StorageCodePointType::eAscii) {
                return pds.fAscii;
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, Character_Latin1>) {
            if (pds.fInCP == StorageCodePointType::eSingleByteLatin1) {
                return pds.fSingleByteLatin1;
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, char8_t>) {
            if (pds.fInCP == StorageCodePointType::eAscii) { // single-byte-latin1 not legal char8_t format
                return pds.fAscii;
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
                    return span<const wchar_t>{reinterpret_cast<const wchar_t*> (pds.fChar16.data ()), pds.fChar16.size ()};
                }
            }
            else if constexpr (sizeof (wchar_t) == 4) {
                if (pds.fInCP == StorageCodePointType::eChar32) {
                    return span<const wchar_t>{reinterpret_cast<const wchar_t*> (pds.fChar32.data ()), pds.fChar32.size ()};
                }
            }
            return span<const wchar_t>{};
        }
        else if constexpr (is_same_v<CHAR_TYPE, Character>) {
            if (pds.fInCP == StorageCodePointType::eChar32) {
                return span<const Character>{reinterpret_cast<const Character*> (pds.fChar32.data ()), pds.fChar32.size ()};
            }
            return span<const Character>{};
        }
        return nullopt; // can easily happen if you request a type that is not stored in the rep
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_TYPE>
    inline optional<span<const CHAR_TYPE>> String::PeekData () const
    {
        return PeekData<CHAR_TYPE> (GetPeekSpanData<CHAR_TYPE> ());
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_TYPE>
    span<const CHAR_TYPE> String::GetData (const PeekSpanData& pds, Memory::StackBuffer<CHAR_TYPE>* possiblyUsedBuffer)
    {
        RequireNotNull (possiblyUsedBuffer);
        using StorageCodePointType = PeekSpanData::StorageCodePointType;
        if constexpr (is_same_v<CHAR_TYPE, wchar_t>) {
            if constexpr (sizeof (CHAR_TYPE) == 2) {
                auto p = GetData (pds, reinterpret_cast<Memory::StackBuffer<char16_t>*> (possiblyUsedBuffer));
                return span<const CHAR_TYPE>{reinterpret_cast<const CHAR_TYPE*> (p.data ()), p.size ()};
            }
            else if constexpr (sizeof (wchar_t) == 4) {
                auto p = GetData (pds, reinterpret_cast<Memory::StackBuffer<char32_t>*> (possiblyUsedBuffer));
                return span<const CHAR_TYPE>{reinterpret_cast<const CHAR_TYPE*> (p.data ()), p.size ()};
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, Character>) {
            auto p = GetData (pds, reinterpret_cast<Memory::StackBuffer<char32_t>*> (possiblyUsedBuffer));
            return span<const CHAR_TYPE>{reinterpret_cast<const CHAR_TYPE*> (p.data ()), p.size ()};
        }
        if constexpr (is_same_v<CHAR_TYPE, char8_t>) {
            switch (pds.fInCP) {
                case StorageCodePointType::eAscii:
                    // ASCII chars are subset of char8_t so any span of ascii is legit span of char8_t
                    return span{reinterpret_cast<const char8_t*> (pds.fAscii.data ()), pds.fAscii.size ()};
                case StorageCodePointType::eSingleByteLatin1: {
                    // Convert ISO-Latin to UTF8 requires a little work sadly
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fSingleByteLatin1));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    return UTFConverter::kThe.ConvertSpan (pds.fSingleByteLatin1, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    return UTFConverter::kThe.ConvertSpan (pds.fSingleByteLatin1, span{*possiblyUsedBuffer});
#endif
                }
                case StorageCodePointType::eChar16: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar16));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    return UTFConverter::kThe.ConvertSpan (pds.fChar16, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    return UTFConverter::kThe.ConvertSpan (pds.fChar16, span{*possiblyUsedBuffer});
#endif
                }
                case StorageCodePointType::eChar32: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar32));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    return UTFConverter::kThe.ConvertSpan (pds.fChar32, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    return UTFConverter::kThe.ConvertSpan (pds.fChar32, span{*possiblyUsedBuffer});
#endif
                }
                default:
                    AssertNotReached ();
                    return span<const CHAR_TYPE>{};
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, char16_t>) {
            switch (pds.fInCP) {
                case StorageCodePointType::eAscii:
                case StorageCodePointType::eSingleByteLatin1: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fSingleByteLatin1));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    return UTFConverter::kThe.ConvertSpan (pds.fSingleByteLatin1, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    return UTFConverter::kThe.ConvertSpan (pds.fSingleByteLatin1, span{*possiblyUsedBuffer});
#endif
                }
                case StorageCodePointType::eChar16:
                    return pds.fChar16;
                case StorageCodePointType::eChar32: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar32));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    return UTFConverter::kThe.ConvertSpan (pds.fChar32, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    return UTFConverter::kThe.ConvertSpan (pds.fChar32, span{*possiblyUsedBuffer});
#endif
                }
                default:
                    AssertNotReached ();
                    return span<const CHAR_TYPE>{};
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, char32_t>) {
            switch (pds.fInCP) {
                case StorageCodePointType::eAscii:
                case StorageCodePointType::eSingleByteLatin1: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fSingleByteLatin1));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    return UTFConverter::kThe.ConvertSpan (pds.fSingleByteLatin1, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    return UTFConverter::kThe.ConvertSpan (pds.fSingleByteLatin1, span{*possiblyUsedBuffer});
#endif
                }
                case StorageCodePointType::eChar16: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConverter::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar16));
#if qCompilerAndStdLib_spanOfContainer_Buggy
                    return UTFConverter::kThe.ConvertSpan (pds.fChar16, span{possiblyUsedBuffer->data (), possiblyUsedBuffer->size ()});
#else
                    return UTFConverter::kThe.ConvertSpan (pds.fChar16, span{*possiblyUsedBuffer});
#endif
                }
                case StorageCodePointType::eChar32:
                    return pds.fChar32;
                default:
                    AssertNotReached ();
                    return span<const CHAR_TYPE>{};
            }
        }
    }
    template <Character_UNICODECanAlwaysConvertTo CHAR_TYPE>
    inline span<const CHAR_TYPE> String::GetData (Memory::StackBuffer<CHAR_TYPE>* possiblyUsedBuffer) const
    {
        RequireNotNull (possiblyUsedBuffer);
        return GetData (GetPeekSpanData<CHAR_TYPE> (), possiblyUsedBuffer);
    }
    inline size_t                              String::length () const noexcept { return size (); }
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
    inline size_t String::find (Character c, size_t startAt) const { return Find (c, startAt, CompareOptions::eWithCase).value_or (npos); }
    inline size_t String::rfind (Character c) const { return RFind (c).value_or (npos); }
    inline void   String::push_back (wchar_t c) { Append (Character (c)); }
    inline void   String::push_back (Character c) { Append (c); }
    inline String String::substr (size_t from, size_t count) const
    {
        _SafeReadRepAccessor accessor{this};
        size_t               thisLen = accessor._ConstGetRep ().size ();
        if (from > thisLen) [[unlikely]] {
            Execution::Throw (out_of_range{"string index out of range"});
        }
        // @todo
        // Not QUITE correct - due to overflow issues, but pragmaitcally this is probably close enough
        size_t to = (count == npos) ? thisLen : (from + min (thisLen, count));
        return SubString_ (accessor, thisLen, from, to);
    }
    inline strong_ordering String::operator<=> (const String& rhs) const { return ThreeWayComparer{}(*this, rhs); }
    inline strong_ordering String::operator<=> (const wchar_t* rhs) const { return ThreeWayComparer{}(*this, rhs); }
    inline strong_ordering String::operator<=> (const wstring& rhs) const { return ThreeWayComparer{}(*this, rhs); }
    inline strong_ordering String::operator<=> (const wstring_view& rhs) const { return ThreeWayComparer{}(*this, rhs); }
    inline bool String::operator== (const String& rhs) const { return EqualsComparer{}(*this, rhs); }
    inline bool String::operator== (const wchar_t* rhs) const { return EqualsComparer{}(*this, rhs); }
    inline bool String::operator== (const wstring& rhs) const { return EqualsComparer{}(*this, rhs); }
    inline bool String::operator== (const wstring_view& rhs) const { return EqualsComparer{}(*this, rhs); }

    /*
     ********************************************************************************
     ******************************** operator"" _k *********************************
     ********************************************************************************
     */
    inline String operator"" _k (const char* s, size_t len) { return String::FromStringConstant (span<const char>{s, len}); }
    inline String operator"" _k (const wchar_t* s, size_t len) { return String::FromStringConstant (span<const wchar_t>{s, len}); }

    /*
     ********************************************************************************
     **************************** String::EqualsComparer ****************************
     ********************************************************************************
     */
    constexpr String::EqualsComparer::EqualsComparer (CompareOptions co)
        : fCompareOptions{co}
    {
    }
    template <Private_::CanBeTreatedAsSpanOfCharacter_ LT, Private_::CanBeTreatedAsSpanOfCharacter_ RT>
    inline bool String::EqualsComparer::Cmp_ (LT&& lhs, RT&& rhs) const
    {
        // optimize very common case of ASCII String vs ASCII String
        if constexpr (is_same_v<decay_t<LT>, String> and is_same_v<decay_t<RT>, String>) {
            if (auto lhsAsciiSpan = lhs.template PeekData<char> ()) {
                if (auto rhsAsciiSpan = rhs.template PeekData<char> ()) {
                    if (fCompareOptions == CompareOptions::eWithCase) {
                        if (lhsAsciiSpan->size () != rhsAsciiSpan->size ()) {
                            return false;
                        }
                        return Memory::MemCmp (lhsAsciiSpan->data (), rhsAsciiSpan->data (), lhsAsciiSpan->size ()) == 0;
                    }
                    else {
                        return Character::Compare (*lhsAsciiSpan, *rhsAsciiSpan, CompareOptions::eCaseInsensitive) == 0;
                    }
                }
            }
        }
        return Cmp_Generic_ (forward<LT> (lhs), forward<RT> (rhs));
    }
    template <Private_::CanBeTreatedAsSpanOfCharacter_ LT, Private_::CanBeTreatedAsSpanOfCharacter_ RT>
    bool String::EqualsComparer::Cmp_Generic_ (LT&& lhs, RT&& rhs) const
    {
        // separate function - cuz large stackframe and on windows generates chkstk calls, so dont have in
        // same frame where we do optimizations
        Memory::StackBuffer<Character> ignore1;
        Memory::StackBuffer<Character> ignore2;
        return Character::Compare (Private_::AsSpanOfCharacters_ (forward<LT> (lhs), &ignore1),
                                   Private_::AsSpanOfCharacters_ (forward<RT> (rhs), &ignore2), fCompareOptions) == 0;
    }
    template <ConvertibleToString LT, ConvertibleToString RT>
    inline bool String::EqualsComparer::operator() (LT&& lhs, RT&& rhs) const
    {
        if constexpr (
            requires { lhs.size (); } and requires { rhs.size (); }) {
            if (lhs.size () != rhs.size ()) {
                return false; // performance tweak
            }
        }
        if constexpr (Private_::CanBeTreatedAsSpanOfCharacter_<LT> and Private_::CanBeTreatedAsSpanOfCharacter_<RT>) {
            return Cmp_ (lhs, rhs);
        }
        else {
            // should almost never happen, but if it does, fall back on using String
            return operator() (String{forward<LT> (lhs)}, String{forward<RT> (rhs)});
        }
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
    template <Private_::CanBeTreatedAsSpanOfCharacter_ LT, Private_::CanBeTreatedAsSpanOfCharacter_ RT>
    inline strong_ordering String::ThreeWayComparer::Cmp_ (LT&& lhs, RT&& rhs) const
    {
        // optimize very common case of ASCII String vs ASCII String
        if constexpr (is_same_v<decay_t<LT>, String> and is_same_v<decay_t<RT>, String>) {
            if (auto lhsAsciiSpan = lhs.template PeekData<char> ()) {
                if (auto rhsAsciiSpan = rhs.template PeekData<char> ()) {
                    return Character::Compare (*lhsAsciiSpan, *rhsAsciiSpan, fCompareOptions);
                }
            }
        }
        return Cmp_Generic_ (forward<LT> (lhs), forward<RT> (rhs));
    }
    template <Private_::CanBeTreatedAsSpanOfCharacter_ LT, Private_::CanBeTreatedAsSpanOfCharacter_ RT>
    strong_ordering String::ThreeWayComparer::Cmp_Generic_ (LT&& lhs, RT&& rhs) const
    {
        // separate function - cuz large stackframe and on windows generates chkstk calls, so dont have in
        // same frame where we do optimizations
        Memory::StackBuffer<Character> ignore1;
        Memory::StackBuffer<Character> ignore2;
        return Character::Compare (Private_::AsSpanOfCharacters_ (forward<LT> (lhs), &ignore1),
                                   Private_::AsSpanOfCharacters_ (forward<RT> (rhs), &ignore2), fCompareOptions);
    }
    template <ConvertibleToString LT, ConvertibleToString RT>
    inline strong_ordering String::ThreeWayComparer::operator() (LT&& lhs, RT&& rhs) const
    {
        if constexpr (Private_::CanBeTreatedAsSpanOfCharacter_<LT> and Private_::CanBeTreatedAsSpanOfCharacter_<RT>) {
            return Cmp_ (lhs, rhs);
        }
        else {
            // should almost never happen, but if it does, fall back on using String
            return operator() (String{forward<LT> (lhs)}, String{forward<RT> (rhs)});
        }
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
     *********************************** operator+ **********************************
     ********************************************************************************
     */
    template <ConvertibleToString LHS_T, ConvertibleToString RHS_T>
    String operator+ (LHS_T&& lhs, RHS_T&& rhs)
        requires (is_base_of_v<String, decay_t<LHS_T>> or is_base_of_v<String, decay_t<RHS_T>>)
    {
        if constexpr (Private_::CanBeTreatedAsSpanOfCharacter_<LHS_T> and Private_::CanBeTreatedAsSpanOfCharacter_<RHS_T>) {
            // maybe always true?
            Memory::StackBuffer<Character> ignored1;
            span<const Character>          lSpan = Private_::AsSpanOfCharacters_ (forward<LHS_T> (lhs), &ignored1);
            Memory::StackBuffer<Character> ignored2;
            span<const Character>          rSpan = Private_::AsSpanOfCharacters_ (forward<RHS_T> (rhs), &ignored2);
            Memory::StackBuffer<Character> buf{Memory::eUninitialized, lSpan.size () + rSpan.size ()};
#if qCompilerAndStdLib_spanOfContainer_Buggy
            span bufSpan{buf.data (), buf.size ()};
#else
            span bufSpan{buf};
#endif
            Memory::CopySpanData (lSpan, bufSpan);
            Memory::CopySpanData (rSpan, bufSpan.subspan (lSpan.size ()));
            return String{bufSpan};
        }
        else {
            return String{forward<LHS_T> (lhs)}.Concatenate (forward<RHS_T> (rhs));
        }
    }

}

namespace Stroika::Foundation::Traversal {
    /*
     *  LOGICALLY, we should put Iterable<T>::Join () String methods in Iterable.inl, but they must be defined in
     *  String.inl to avoid #include problems (that includes this anyhow, and this cannot include that because that includes this)
     */
    template <typename T>
    inline Characters::String Iterable<T>::Join (const function<Characters::String (const T&)>& convertToT,
                                                 const function<Characters::String (const Characters::String&, const Characters::String&)>& combine) const
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
        return Join (convertToT, ", "sv);
    }
    template <typename T>
    inline Characters::String Iterable<T>::Join (const Characters::String& separator) const
    {
        //return Join ([] (const auto& i) { return Characters::ToString (i); });    @todo figure out trick to be able to use Characters::ToString () here...
        return Join ([] (const auto& i) { return i.ToString (); },
                     [=] (const Characters::String& l, const Characters::String& r) { return l + separator + r; });
    }
    template <typename T>
    inline Characters::String Iterable<T>::Join () const
    {
        return Join (", "sv);
    }
}

namespace Stroika::Foundation::Characters {
    [[deprecated ("Since Stroika v3.0d1 - just use _k, sv, or nothing")]] inline String operator"" _ASCII (const char* str, size_t len)
    {
        return String{span{str, len}};
    }
    class [[deprecated ("Since Stroika v3.0 - just use String::FromStringConstant")]] String_Constant : public String{
        public : template <size_t SIZE>
        explicit String_Constant (const wchar_t (&cString)[SIZE]) :
            String{String::FromStringConstant (std::basic_string_view<wchar_t>{cString, SIZE - 1})} {}

        String_Constant (const wchar_t* start, const wchar_t* end) :
            String{String::FromStringConstant (std::basic_string_view<wchar_t>{start, static_cast<size_t> (end - start)})} {}

        String_Constant (const std::basic_string_view<wchar_t>& str) : String{String::FromStringConstant (str)} {}
    };
}
namespace Stroika::Foundation::Characters::Concrete {
    class [[deprecated ("Since Stroika v3.0 - just use String::FromStringConstant")]] String_ExternalMemoryOwnership_ApplicationLifetime : public String{
        public : template <size_t SIZE>
        explicit String_ExternalMemoryOwnership_ApplicationLifetime (const wchar_t (&cString)[SIZE - 1]) :
            String{String::FromStringConstant (basic_string_view<wchar_t>{cString, SIZE})} {}

        String_ExternalMemoryOwnership_ApplicationLifetime (const wchar_t* start, const wchar_t* end) :
            String{String::FromStringConstant (basic_string_view<wchar_t>{start, static_cast<size_t> (end - start)})} {}

        String_ExternalMemoryOwnership_ApplicationLifetime (const basic_string_view<wchar_t>& str) : String{String::FromStringConstant (str)} {}
    };
}

#if qCompilerAndStdLib_clangWithLibStdCPPStringConstexpr_Buggy
namespace {
    inline std::u8string clang_string_BWA_ (const char8_t* a, const char8_t* b) { return {a, b}; }
}
#endif

#endif // _Stroika_Foundation_Characters_String_inl_
