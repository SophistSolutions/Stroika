/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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

    [[deprecated ("Since v3.0d1 - use String{s}.AsNarrowSDKString ()")]] inline std::string WideStringToNarrowSDKString (std::wstring s)
    {
        return String{s}.AsNarrowSDKString ();
    }

    /*
     ********************************************************************************
     *************************** Characters::Private_ *******************************
     ********************************************************************************
     */
    namespace Private_ {
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static size_t StrLen_ (const CHAR_T* s)
        {
            if constexpr (is_same_v<CHAR_T, Latin1>) {
                return StrLen_ (reinterpret_cast<const char*> (s));
            }
            else {
                return CString::Length (s);
            }
        }
        template <IUNICODECanUnambiguouslyConvertFrom SRC_T>
        inline void CopyAsASCIICharacters_ (span<const SRC_T> src, span<ASCII> trg)
        {
            Require (trg.size () >= src.size ());
            ASCII* outI = trg.data ();
            for (auto ii = src.begin (); ii != src.end (); ++ii) {
                if constexpr (is_same_v<SRC_T, Character>) {
                    *outI++ = ii->GetAsciiCode ();
                }
                else {
                    *outI++ = static_cast<ASCII> (*ii);
                }
            }
        }
        template <IUNICODECanUnambiguouslyConvertFrom SRC_T>
        inline void CopyAsLatin1Characters_ (span<const SRC_T> src, span<Latin1> trg)
        {
            Require (trg.size () >= src.size ());
            Latin1* outI = trg.data ();
            for (auto ii = src.begin (); ii != src.end (); ++ii) {
                if constexpr (is_same_v<SRC_T, Character>) {
                    *outI++ = Latin1{static_cast<unsigned char> (ii->GetCharacterCode ())};
                }
                else {
                    *outI++ = Latin1{static_cast<unsigned char> (*ii)};
                }
            }
        }
        template <ICanBeTreatedAsSpanOfCharacter_ USTRING, size_t STACK_BUFFER_SZ>
        inline span<const Character> AsSpanOfCharacters_ (USTRING&& s, Memory::StackBuffer<Character, STACK_BUFFER_SZ>* mostlyIgnoredBuf)
        {
            /*
             * Genericly convert the argument to a span<const Character> object; for a string, complex and requires
             * a function call (GetData) and ESSENTIALLY optional mostlyIgnoredBuf argument. For most other types
             * mostlyIgnoredBuf is ignored.
             * 
             * This must be highly optimized as its used in critical locations, to quickly access argument data and
             * convert it into a usable form.
             */
            if constexpr (derived_from<remove_cvref_t<USTRING>, String>) {
                return s.GetData (mostlyIgnoredBuf);
            }
            else if constexpr (is_same_v<remove_cvref_t<USTRING>, const char32_t*> or
                               (sizeof (wchar_t) == sizeof (Character) and is_same_v<remove_cvref_t<USTRING>, const wchar_t*>)) {
                return span{reinterpret_cast<const Character*> (s), CString::Length (s)};
            }
            else if constexpr (is_same_v<remove_cvref_t<USTRING>, u32string> or
                               (sizeof (wchar_t) == sizeof (Character) and is_same_v<remove_cvref_t<USTRING>, wstring>)) {
                return span{reinterpret_cast<const Character*> (s.c_str ()), s.length ()};
            }
            else if constexpr (is_same_v<remove_cvref_t<USTRING>, u32string_view> or
                               (sizeof (wchar_t) == sizeof (Character) and is_same_v<remove_cvref_t<USTRING>, wstring_view>)) {
                return span{reinterpret_cast<const Character*> (s.data ()), s.length ()};
            }
            else if constexpr (is_same_v<remove_cvref_t<USTRING>, const char8_t*> or is_same_v<remove_cvref_t<USTRING>, const char16_t*> or
                               is_same_v<remove_cvref_t<USTRING>, const wchar_t*>) {
                span spn{s, CString::Length (s)};
                mostlyIgnoredBuf->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<Character> (spn));
                return UTFConvert::kThe.ConvertSpan (spn, span{*mostlyIgnoredBuf});
            }
            else if constexpr (is_same_v<remove_cvref_t<USTRING>, u8string> or is_same_v<remove_cvref_t<USTRING>, u16string> or
                               is_same_v<remove_cvref_t<USTRING>, wstring>) {
                span spn{s.data (), s.size ()};
                mostlyIgnoredBuf->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<Character> (spn));
                return UTFConvert::kThe.ConvertSpan (spn, span{*mostlyIgnoredBuf});
            }
            else if constexpr (is_same_v<remove_cvref_t<USTRING>, u8string_view> or is_same_v<remove_cvref_t<USTRING>, u16string_view> or
                               is_same_v<remove_cvref_t<USTRING>, wstring_view>) {
                span spn{s.data (), s.size ()};
                mostlyIgnoredBuf->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<Character> (spn));
                return UTFConvert::kThe.ConvertSpan (spn, span{*mostlyIgnoredBuf});
            }
            else {
                // else must copy data to mostlyIgnoredBuf and use that, so just need a span
                span spn{s}; // tricky part
                mostlyIgnoredBuf->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<Character> (spn));
                return UTFConvert::kThe.ConvertSpan (spn);
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
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    auto String::mk_ (span<const CHAR_T> s) -> shared_ptr<_IRep>
    {
        if (s.empty ()) {
            return mkEmpty_ ();
        }
        if constexpr (is_same_v<CHAR_T, ASCII>) {
            Character::CheckASCII (s);
            return mk_nocheck_ (s);
        }
        else if constexpr (is_same_v<CHAR_T, Latin1>) {
            Character::CheckLatin1 (s);
            return mk_nocheck_ (s);
        }
        // NOTE: StackBuffer <,SIZE> parameters heuristic - to avoid _chkstk calls and performance impact - for most common string cases.
        switch (Character::IsASCIIOrLatin1 (s)) {
            case Character::ASCIIOrLatin1Result::eASCII: {
                if constexpr (sizeof (CHAR_T) == 1) {
                    return mk_nocheck_ (span<const ASCII>{reinterpret_cast<const ASCII*> (s.data ()), s.size ()});
                }
                else {
                    // Copy to smaller buffer (e.g. utf16_t to char)
                    Memory::StackBuffer<ASCII, Memory::kStackBuffer_SizeIfLargerStackGuardCalled / 4 - 20> buf{Memory::eUninitialized, s.size ()};
                    Private_::CopyAsASCIICharacters_ (s, span{buf});
                    return mk_nocheck_ (span<const ASCII>{buf});
                }
            }
            case Character::ASCIIOrLatin1Result::eLatin1: {
                if constexpr (sizeof (CHAR_T) == 1) {
                    if constexpr (same_as<remove_cv_t<CHAR_T>, ASCII>) {
                        RequireNotReached (); // if marked as ASCII, better not contain non-ascii characters!
                    }
                    else if constexpr (same_as<remove_cv_t<CHAR_T>, Latin1>) {
                        return mk_nocheck_ (s);
                    }
                    else if constexpr (same_as<remove_cv_t<CHAR_T>, char8_t>) {
                        // Lat1in1 CAN fit in a single byte, but when encoded as UTF-8, it generally does NOT. So we must map to its one byte
                        // representation, by doing UTF decoding.
                        //
                        // EXAMPLE:
                        //      https://www.utf8-chartable.de/
                        //              U+00C2  �   c3 82   LATIN CAPITAL LETTER A WITH CIRCUMFLEX
                        //
                        // However, a quirk (reasonable) of UTFConvert::kThe.ConvertSpan is that it CANNOT convert to Latin1 becuase
                        // the concept IUNICODECanUnambiguouslyConvertTo<Latin1> evaluated to false.
                        //
                        // COULD lift that restriction, (@todo consider), and just check/assert/require input is indeeded all Latin1. Or could
                        // do what I do here, and do a two step copy.
                        //
                        // OR could fix CopyAsLatin1Characters_ () to handle input of char8_t differently/correctly;
                        //
                        //  Suspect this case is rare enuf to be good enuf for now ]--LGP 2023-12-02
                        Memory::StackBuffer<char16_t, Memory::kStackBuffer_SizeIfLargerStackGuardCalled / 16 - 20> c16buf{Memory::eUninitialized,
                                                                                                                          s.size ()};
                        span<const char16_t> s16 = UTFConvert::kThe.ConvertSpan (s, span{c16buf});
                        Memory::StackBuffer<Latin1, Memory::kStackBuffer_SizeIfLargerStackGuardCalled / 16 - 20> buf{Memory::eUninitialized,
                                                                                                                     s16.size ()};
                        Private_::CopyAsLatin1Characters_ (s16, span{buf});
                        return mk_nocheck_ (span<const Latin1>{buf});
                    }
                    else {
                        AssertNotReached (); // no other 1-byte case
                    }
                }
                else {
                    // Copy to smaller buffer (e.g. utf32_t to Latin1)
                    Memory::StackBuffer<Latin1, Memory::kStackBuffer_SizeIfLargerStackGuardCalled / 8 - 20> buf{Memory::eUninitialized, s.size ()};
                    Private_::CopyAsLatin1Characters_ (s, span{buf});
                    return mk_nocheck_ (span<const Latin1>{buf});
                }
            }
        }
        // at this point, we know the text must be encoded as utf16 or utf32 (but source code still be single byte, like utf8)
        if (UTFConvert::AllFitsInTwoByteEncoding (s)) {
            if constexpr (sizeof (CHAR_T) == 2) {
                // no transcode needed UTF16->UTF16
                return mk_nocheck_ (span<const char16_t>{reinterpret_cast<const char16_t*> (s.data ()), s.size ()});
            }
            else {
                // complex case - could be utf8 src, utf16, or utf32, so must transcode to char16_t
                Memory::StackBuffer<char16_t, Memory::kStackBuffer_SizeIfLargerStackGuardCalled / 16> wideUnicodeBuf{
                    Memory::eUninitialized, UTFConvert::ComputeTargetBufferSize<char16_t> (s)};
                return mk_nocheck_ (Memory::ConstSpan (UTFConvert::kThe.ConvertSpan (s, span{wideUnicodeBuf})));
            }
        }
        // So at this point - definitely converting to UTF-32
        if constexpr (sizeof (CHAR_T) == 4) {
            // Easy, just cast
            return mk_nocheck_ (span<const char32_t>{reinterpret_cast<const char32_t*> (s.data ()), s.size ()});
        }
        else {
            // converting utf8 or utf16 with surrogates to utf32
            Memory::StackBuffer<char32_t, Memory::kStackBuffer_SizeIfLargerStackGuardCalled / 32> wideUnicodeBuf{
                Memory::eUninitialized, UTFConvert::ComputeTargetBufferSize<char32_t> (s)};
            return mk_nocheck_ (Memory::ConstSpan (UTFConvert::kThe.ConvertSpan (s, span{wideUnicodeBuf})));
        }
    }
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    auto String::mk_ (span<CHAR_T> s) -> shared_ptr<_IRep>
    {
        // weird and unfortunate overload needed for non-const spans, not automatically promoted to const
        return mk_ (Memory::ConstSpan (s));
    }
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    auto String::mk_ (Iterable<CHAR_T> it) -> shared_ptr<_IRep>
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
    auto String::mk_ (basic_string<char>&& s) -> shared_ptr<_IRep>;
    template <>
    auto String::mk_ (basic_string<char16_t>&& s) -> shared_ptr<_IRep>;
    template <>
    auto String::mk_ (basic_string<char32_t>&& s) -> shared_ptr<_IRep>;
    template <>
    auto String::mk_ (basic_string<wchar_t>&& s) -> shared_ptr<_IRep>;
    template <IStdBasicStringCompatibleCharacter CHAR_T>
    inline auto String::mk_ (basic_string<CHAR_T>&& s) -> shared_ptr<_IRep>
    {
        // by default, except for maybe a few special cases, just copy the data - don't move
        return mk_ (span{s.begin (), s.size ()});
    }
    inline String::String (const shared_ptr<_IRep>& rep) noexcept
        : inherited{rep}
    {
        _AssertRepValidType ();
    }
    inline String::String (shared_ptr<_IRep>&& rep) noexcept
        : inherited{(RequireExpression (rep != nullptr), move (rep))}
    {
        _AssertRepValidType ();
    }
    inline String::String ()
        : inherited{mkEmpty_ ()}
    {
        _AssertRepValidType ();
    }
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline String::String (const CHAR_T* cString)
        : inherited{mk_ (span{cString, CString::Length (cString)})}
    {
        RequireNotNull (cString);
        _AssertRepValidType ();
    }
    template <Memory::ISpan SPAN_OF_CHAR_T>
    inline String::String (SPAN_OF_CHAR_T s)
        requires (IUNICODECanUnambiguouslyConvertFrom<typename SPAN_OF_CHAR_T::value_type>)
        : inherited{mk_ (span<const typename SPAN_OF_CHAR_T::value_type>{s})}
    {
        _AssertRepValidType ();
    }
    template <IStdBasicStringCompatibleCharacter CHAR_T>
    inline String::String (const basic_string<CHAR_T>& s)
        : inherited{mk_ (span<const CHAR_T>{s.data (), s.size ()})}
    {
    }
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline String::String (const Iterable<CHAR_T>& src)
        requires (not Memory::ISpan<CHAR_T>)
        : inherited{mk_ (src)}
    {
    }
    inline String::String (Character c)
        : String{span{&c, 1}}
    {
    }
    template <IStdBasicStringCompatibleCharacter CHAR_T>
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
    template <IStdBasicStringCompatibleCharacter CHAR_T>
    inline String String::FromLatin1 (const basic_string<CHAR_T>& s)
    {
        return FromLatin1 (span{s.data (), s.size ()});
    }
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline String String::FromLatin1 (const CHAR_T* cString)
    {
        RequireNotNull (cString);
        return FromLatin1 (span{cString, Private_::StrLen_ (cString)});
    }
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline String String::FromLatin1 (span<const CHAR_T> s)
    {
        /*
         *  From http://unicodebook.readthedocs.io/encodings.html
         *      "For example, ISO-8859-1 are the first 256 UNICODE code points (U+0000-U+00FF)."
         */
        if constexpr (sizeof (CHAR_T) == 1) {
            return mk_ (span<const Latin1>{reinterpret_cast<const Latin1*> (s.data ()), s.size ()});
        }
        else {
            const CHAR_T*               b = reinterpret_cast<const CHAR_T*> (s.data ());
            const CHAR_T*               e = b + s.size ();
            Memory::StackBuffer<Latin1> buf{Memory::eUninitialized, static_cast<size_t> (e - b)};
            Latin1*                     pOut = buf.begin ();
            for (const CHAR_T* i = b; i != e; ++i, ++pOut) {
                if (*i >= 256) {
                    static const auto kException_ = out_of_range{"Error converting non-iso-latin-1 text to String"};
                    Execution::Throw (kException_);
                }
                *pOut = *i;
            }
            return mk_ (span<const Latin1>{buf.begin (), pOut});
        }
    }
    template <size_t SIZE>
    inline String String::FromStringConstant (const ASCII (&cString)[SIZE])
    {
        return FromStringConstant (span<const ASCII>{cString, SIZE - 1}); // -1 because a literal array SIZE includes the NUL-character at the end
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
        if (Character::IsASCII (s)) [[likely]] {
            return mk_ (span<const char>{reinterpret_cast<const char*> (s.data ()), s.size ()});
        }
        else if (UTFConvert::AllFitsInTwoByteEncoding (s)) [[likely]] {
            Memory::StackBuffer<char16_t> buf{Memory::eUninitialized, UTFConvert::kThe.ComputeTargetBufferSize<char16_t> (s)};
            return String{UTFConvert::kThe.ConvertSpan (s, span{buf})};
        }
        else {
            Memory::StackBuffer<char32_t> buf{Memory::eUninitialized, UTFConvert::kThe.ComputeTargetBufferSize<char32_t> (s)};
            return String{UTFConvert::kThe.ConvertSpan (s, span{buf})};
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
        if constexpr (same_as<SDKChar, wchar_t>) {
            return String{s};
        }
        else {
            return String{SDK2Wide (s)};
        }
    }
    inline String String::FromSDKString (const SDKString& from)
    {
        if constexpr (same_as<SDKString, wstring>) {
            return String{from};
        }
        else {
            return FromSDKString (span{from.c_str (), from.length ()});
        }
    }
    inline String String::FromNarrowSDKString (const char* from)
    {
        RequireNotNull (from);
        return FromNarrowSDKString (span{from, ::strlen (from)});
    }
    inline String String::FromNarrowSDKString (span<const char> s)
    {
        return String{NarrowSDK2Wide (s)};
    }
    inline String String::FromNarrowSDKString (const string& from)
    {
        return FromNarrowSDKString (span{from.c_str (), from.length ()});
    }
    template <typename T>
    inline String String::Concatenate (T&& rhs) const
        requires (is_convertible_v<T, String>)
    {
        // @todo more work needed optimizing this - esp for other arguments like const char*, and string_view etc...
        // and even can optimize for case where storing char16_t or LATIN1 types...
        PeekSpanData lhsPSD = GetPeekSpanData<ASCII> ();
        // OPTIMIZED PATHS: Common case(s) and should be fast
        if (lhsPSD.fInCP == PeekSpanData::StorageCodePointType::eAscii) {
            if constexpr (derived_from<remove_cvref_t<T>, String>) {
                PeekSpanData rhsPSD = rhs.template GetPeekSpanData<ASCII> ();
                if (rhsPSD.fInCP == PeekSpanData::StorageCodePointType::eAscii) {
                    Memory::StackBuffer<ASCII, 512> buf{Memory::eUninitialized, lhsPSD.fAscii.size () + rhsPSD.fAscii.size ()};
                    copy (lhsPSD.fAscii.begin (), lhsPSD.fAscii.end (), buf.data ());
                    copy (rhsPSD.fAscii.begin (), rhsPSD.fAscii.end (), buf.data () + lhsPSD.fAscii.size ());
                    return this->mk_nocheck_ (span<const ASCII>{buf}); // no check needed cuz combining all ASCII sources
                }
            }
            // @todo lots of other easy cases to optimize, but this came up first...
        }
        // simple default fallthru implementation
        return Concatenate_ (forward<T> (rhs));
    }
    inline void String::_AssertRepValidType () const
    {
        EnsureMember (&_SafeReadRepAccessor{this}._ConstGetRep (), String::_IRep);
    }
    template <IUNICODECanAlwaysConvertTo CHAR_T>
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
                    return UTFConvert::kThe.ConvertSpan (psd.fSingleByteLatin1, s);
                case PeekSpanData::StorageCodePointType::eChar16:
                    return UTFConvert::kThe.ConvertSpan (psd.fChar16, s);
                case PeekSpanData::StorageCodePointType::eChar32:
                    return UTFConvert::kThe.ConvertSpan (psd.fChar32, s);
                default:
                    AssertNotReached ();
                    return span<CHAR_T>{};
            }
        }
    }
    inline size_t String::size () const noexcept
    {
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ().size ();
    }
    template <unsigned_integral T>
    inline size_t String::SubString_adjust_ (T fromOrTo, [[maybe_unused]] size_t myLength) const
    {
        Require (fromOrTo <= numeric_limits<size_t>::max ());
        return static_cast<size_t> (fromOrTo);
    }
    template <signed_integral T>
    inline size_t String::SubString_adjust_ (T fromOrTo, size_t myLength) const
    {
        if (fromOrTo >= 0) [[likely]] {
            Require (fromOrTo <= numeric_limits<ptrdiff_t>::max ());
            return static_cast<size_t> (fromOrTo);
        }
        else {
            Require (fromOrTo >= numeric_limits<ptrdiff_t>::min ());
            return static_cast<size_t> (myLength + static_cast<ptrdiff_t> (fromOrTo));
        }
    }
    template <typename SZ>
    inline String String::SubString (SZ from) const
    {
        _SafeReadRepAccessor accessor{this};
        size_t               myLength{accessor._ConstGetRep ().size ()};
        size_t               f = SubString_adjust_ (from, myLength);
        size_t               t = myLength;
        Require (f <= myLength);
        return SubString_ (accessor, f, t);
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
        return SubString_ (accessor, f, t);
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
        return SubString_ (accessor, f, f + useLength);
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
        return SubString_ (accessor, f, f + useLength);
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
        return accessor._ConstGetRep ().size () == 0;
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
        return InsertAt (span<const Character>{&c, 1}, at);
    }
    inline String String::InsertAt (const String& s, size_t at) const
    {
        Memory::StackBuffer<Character> ignored1;
        return InsertAt (s.GetData (&ignored1), at);
    }
    inline String String::InsertAt (span<Character> s, size_t at) const
    {
        return InsertAt (Memory::ConstSpan (s), at);
    }
    template <typename CHAR_T>
    inline void String::Append (span<const CHAR_T> s)
        requires (is_same_v<CHAR_T, Character> or is_same_v<CHAR_T, char32_t>)
    {
        if (not s.empty ()) {
            Memory::StackBuffer<char32_t> ignored1;
            span<const char32_t>          thisSpan = this->GetData (&ignored1);
            Memory::StackBuffer<char32_t> combinedBuf{Memory::eUninitialized, thisSpan.size () + s.size ()};
            Memory::CopySpanData (thisSpan, span{combinedBuf});
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
            *this = mk_ (span{combinedBuf});
        }
    }
    inline void String::Append (const wchar_t* from, const wchar_t* to)
    {
        Require (from <= to);
        if (from != to) {
            Memory::StackBuffer<wchar_t> ignored1;
            span<const wchar_t>          thisSpan = this->GetData (&ignored1);
            Memory::StackBuffer<wchar_t> buf{Memory::eUninitialized, thisSpan.size () + (to - from)};
            span<wchar_t>                bufSpan{buf};
            Memory::CopySpanData (thisSpan, bufSpan);
            Memory::CopySpanData (span{from, to}, bufSpan.subspan (thisSpan.size ()));
            *this = mk_ (bufSpan);
        }
    }
    inline void String::Append (Character c)
    {
        Append (&c, &c + 1);
    }
    inline void String::Append (const String& s)
    {
        Memory::StackBuffer<char32_t> ignored1;
        auto                          rhsSpan = s.GetData (&ignored1);
        Append (rhsSpan);
    }
    inline void String::Append (const wchar_t* s)
    {
        Append (s, s + ::wcslen (s));
    }
    inline void String::Append (const Character* from, const Character* to)
    {
        Append (span{from, to});
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
        Require (i < accessor._ConstGetRep ().size ());
        return accessor._ConstGetRep ().GetAt (i);
    }
    inline const Character String::operator[] (size_t i) const noexcept
    {
        Require (i >= 0);
        Require (i < size ());
        return GetCharAt (i);
    }
    inline String String::LimitLength (size_t maxLen, StringShorteningPreference keepPref) const
    {
#if qCompiler_vswprintf_on_elispisStr_Buggy
        static const String kELIPSIS_{"..."_k};
#else
        static const String          kELIPSIS_{u"\u2026"sv}; // OR "..."
#endif
        return LimitLength (maxLen, keepPref, kELIPSIS_);
    }
    template <typename T>
    inline T String::As () const
        requires (IBasicUNICODEStdString<T> or is_same_v<T, String>)
    {
        if constexpr (is_same_v<T, u8string>) {
            return AsUTF8<T> ();
        }
        else if constexpr (is_same_v<T, u16string>) {
            return AsUTF16<T> ();
        }
        else if constexpr (is_same_v<T, u32string>) {
            return AsUTF32<T> ();
        }
        else if constexpr (is_same_v<T, wstring>) {
            if constexpr (sizeof (wchar_t) == 2) {
                return AsUTF16<T> ();
            }
            else {
                return AsUTF32<T> ();
            }
        }
        else if constexpr (is_same_v<T, String>) {
            return *this;
        }
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
    inline T String::AsUTF16 () const
        requires (is_same_v<T, u16string> or (sizeof (wchar_t) == sizeof (char16_t) and is_same_v<T, wstring>))
    {
        Memory::StackBuffer<char16_t> maybeIgnoreBuf1;
        span<const char16_t>          thisData = GetData (&maybeIgnoreBuf1);
        return T{reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ()};
    }
    template <typename T>
    inline T String::AsUTF32 () const
        requires (is_same_v<T, u32string> or (sizeof (wchar_t) == sizeof (char32_t) and is_same_v<T, wstring>))
    {
        Memory::StackBuffer<char32_t> maybeIgnoreBuf1;
        span<const char32_t>          thisData = GetData (&maybeIgnoreBuf1);
        return T{reinterpret_cast<const typename T::value_type*> (thisData.data ()), thisData.size ()};
    }
    inline SDKString String::AsSDKString () const
    {
#if qTargetPlatformSDKUseswchar_t
        Memory::StackBuffer<wchar_t> maybeIgnoreBuf1;
        span<const wchar_t>          thisData = GetData (&maybeIgnoreBuf1);
        return SDKString{thisData.begin (), thisData.end ()};
#elif qPlatform_MacOS
        Memory::StackBuffer<char8_t> maybeIgnoreBuf1;
        span<const char8_t>          thisData = GetData (&maybeIgnoreBuf1);
        return SDKString{thisData.begin (), thisData.end ()}; // @todo DOCUMENT THAT MACOS USES UTF8 - SRC - LOGIC/RATIONALE
#else
        return AsNarrowString (locale{});                // @todo document why - linux one rationale - default - similar
#endif
    }
    inline SDKString String::AsSDKString (AllowMissingCharacterErrorsFlag) const
    {
#if qTargetPlatformSDKUseswchar_t
        Memory::StackBuffer<wchar_t> maybeIgnoreBuf1;
        span<const wchar_t>          thisData = GetData (&maybeIgnoreBuf1);
        return SDKString{thisData.begin (), thisData.end ()};
#elif qPlatform_MacOS
        Memory::StackBuffer<char8_t> maybeIgnoreBuf1;
        span<const char8_t> thisData = GetData (&maybeIgnoreBuf1); // Note this always works, since we can always map to UTF-8 any Stroika string
        return SDKString{thisData.begin (), thisData.end ()};      // @todo DOCUMENT THAT MACOS USES UTF8 - SRC - LOGIC/RATIONALE
#else
        return AsNarrowString (locale{}, eIgnoreErrors); // @todo document why - linux one rationale - default - similar
#endif
    }
    inline string String::AsNarrowSDKString () const
    {
        return SDK2Narrow (AsSDKString ());
    }
    inline string String::AsNarrowSDKString (AllowMissingCharacterErrorsFlag) const
    {
        return SDK2Narrow (AsSDKString (eIgnoreErrors), AllowMissingCharacterErrorsFlag::eIgnoreErrors);
    }
    template <typename T>
    inline T String::AsASCII () const
        requires requires (T* into) {
            {
                into->empty ()
            } -> same_as<bool>;
            {
                into->push_back (ASCII{0})
            };
        }
    {
        // @todo possibly rewrite/inline impl to avoid map to optional<T> which involves copying
        if (auto p = AsASCIIQuietly<T> ()) {
            return *p;
        }
        else {
            ThrowInvalidAsciiException_ ();
        }
    }
    template <typename T>
    inline optional<T> String::AsASCIIQuietly () const
        requires requires (T* into) {
            {
                into->empty ()
            } -> same_as<bool>;
            {
                into->push_back (ASCII{0})
            };
        }
    {
        // @todo OPTIMIZE - PeekSpanData - may already be ASCII - OPTIMIZE THAT CASE!!!
        Memory::StackBuffer<wchar_t> ignored1;
        auto                         thisSpan = GetData (&ignored1);
        T                            s;
        return Character::AsASCIIQuietly<T> (thisSpan, &s) ? s : optional<T>{};
    }
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_TYPE>
    inline String::PeekSpanData String::GetPeekSpanData () const
    {
        using StorageCodePointType = PeekSpanData::StorageCodePointType;
        StorageCodePointType preferredSCP{};
        if constexpr (is_same_v<remove_cv_t<CHAR_TYPE>, ASCII>) {
            preferredSCP = StorageCodePointType::eAscii;
        }
        else if constexpr (is_same_v<remove_cv_t<CHAR_TYPE>, Latin1>) {
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
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_TYPE>
    inline optional<span<const CHAR_TYPE>> String::PeekData (const PeekSpanData& pds)
    {
        using StorageCodePointType = PeekSpanData::StorageCodePointType;
        if constexpr (is_same_v<CHAR_TYPE, ASCII>) {
            if (pds.fInCP == StorageCodePointType::eAscii) {
                return pds.fAscii;
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, Latin1>) {
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
    template <IUNICODECanUnambiguouslyConvertFrom CHAR_TYPE>
    inline optional<span<const CHAR_TYPE>> String::PeekData () const
    {
        return PeekData<CHAR_TYPE> (GetPeekSpanData<CHAR_TYPE> ());
    }
    template <IUNICODECanAlwaysConvertTo CHAR_TYPE, size_t STACK_BUFFER_SZ>
    span<const CHAR_TYPE> String::GetData (const PeekSpanData& pds, Memory::StackBuffer<CHAR_TYPE, STACK_BUFFER_SZ>* possiblyUsedBuffer)
    {
        RequireNotNull (possiblyUsedBuffer);
        using StorageCodePointType = PeekSpanData::StorageCodePointType;
        if constexpr (is_same_v<CHAR_TYPE, wchar_t>) {
            if constexpr (sizeof (CHAR_TYPE) == 2) {
                auto p = GetData (pds, reinterpret_cast<Memory::StackBuffer<char16_t, STACK_BUFFER_SZ>*> (possiblyUsedBuffer));
                return span<const CHAR_TYPE>{reinterpret_cast<const CHAR_TYPE*> (p.data ()), p.size ()};
            }
            else if constexpr (sizeof (wchar_t) == 4) {
                auto p = GetData (pds, reinterpret_cast<Memory::StackBuffer<char32_t, STACK_BUFFER_SZ>*> (possiblyUsedBuffer));
                return span<const CHAR_TYPE>{reinterpret_cast<const CHAR_TYPE*> (p.data ()), p.size ()};
            }
        }
        else if constexpr (is_same_v<CHAR_TYPE, Character>) {
            auto p = GetData (pds, reinterpret_cast<Memory::StackBuffer<char32_t, STACK_BUFFER_SZ>*> (possiblyUsedBuffer));
            return span<const CHAR_TYPE>{reinterpret_cast<const CHAR_TYPE*> (p.data ()), p.size ()};
        }
        if constexpr (is_same_v<CHAR_TYPE, char8_t>) {
            switch (pds.fInCP) {
                case StorageCodePointType::eAscii:
                    // ASCII chars are subset of char8_t so any span of ascii is legit span of char8_t
                    return span{reinterpret_cast<const char8_t*> (pds.fAscii.data ()), pds.fAscii.size ()};
                case StorageCodePointType::eSingleByteLatin1: {
                    // Convert ISO-Latin to UTF8 requires a little work sadly
                    possiblyUsedBuffer->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<CHAR_TYPE> (pds.fSingleByteLatin1));
                    return UTFConvert::kThe.ConvertSpan (pds.fSingleByteLatin1, span{*possiblyUsedBuffer});
                }
                case StorageCodePointType::eChar16: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar16));
                    return UTFConvert::kThe.ConvertSpan (pds.fChar16, span{*possiblyUsedBuffer});
                }
                case StorageCodePointType::eChar32: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar32));
                    return UTFConvert::kThe.ConvertSpan (pds.fChar32, span{*possiblyUsedBuffer});
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
                    possiblyUsedBuffer->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<CHAR_TYPE> (pds.fSingleByteLatin1));
                    return UTFConvert::kThe.ConvertSpan (pds.fSingleByteLatin1, span{*possiblyUsedBuffer});
                }
                case StorageCodePointType::eChar16:
                    return pds.fChar16;
                case StorageCodePointType::eChar32: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar32));
                    return UTFConvert::kThe.ConvertSpan (pds.fChar32, span{*possiblyUsedBuffer});
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
                    possiblyUsedBuffer->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<CHAR_TYPE> (pds.fSingleByteLatin1));
                    return UTFConvert::kThe.ConvertSpan (pds.fSingleByteLatin1, span{*possiblyUsedBuffer});
                }
                case StorageCodePointType::eChar16: {
                    possiblyUsedBuffer->resize_uninitialized (UTFConvert::ComputeTargetBufferSize<CHAR_TYPE> (pds.fChar16));
                    return UTFConvert::kThe.ConvertSpan (pds.fChar16, span{*possiblyUsedBuffer});
                }
                case StorageCodePointType::eChar32:
                    return pds.fChar32;
                default:
                    AssertNotReached ();
                    return span<const CHAR_TYPE>{};
            }
        }
    }
    template <IUNICODECanAlwaysConvertTo CHAR_TYPE, size_t STACK_BUFFER_SZ>
    inline span<const CHAR_TYPE> String::GetData (Memory::StackBuffer<CHAR_TYPE, STACK_BUFFER_SZ>* possiblyUsedBuffer) const
    {
        RequireNotNull (possiblyUsedBuffer);
        return GetData (GetPeekSpanData<CHAR_TYPE> (), possiblyUsedBuffer);
    }
    inline size_t String::length () const noexcept
    {
        return size ();
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
    inline size_t String::find (Character c, size_t startAt) const
    {
        return Find (c, startAt, CompareOptions::eWithCase).value_or (npos);
    }
    inline size_t String::find (const String& s, size_t startAt) const
    {
        return Find (s, startAt, CompareOptions::eWithCase).value_or (npos);
    }
    inline size_t String::rfind (Character c) const
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
    inline Character String::back () const
    {
        Require (not empty ());
        _SafeReadRepAccessor accessor{this};
        size_t               thisLen = accessor._ConstGetRep ().size ();
        return accessor._ConstGetRep ().GetAt (thisLen - 1);
    }
    inline Character String::front () const
    {
        Require (not empty ());
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ().GetAt (0);
    }
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
        return SubString_ (accessor, from, to);
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
     ******************************** operator"" _k *********************************
     ********************************************************************************
     */
    inline String operator"" _k (const char* s, size_t len)
    {
        return String::FromStringConstant (span<const char>{s, len});
    }
    inline String operator"" _k (const wchar_t* s, size_t len)
    {
        return String::FromStringConstant (span<const wchar_t>{s, len});
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
    template <Private_::ICanBeTreatedAsSpanOfCharacter_ LT, Private_::ICanBeTreatedAsSpanOfCharacter_ RT>
    inline bool String::EqualsComparer::Cmp_ (LT&& lhs, RT&& rhs) const
    {
        // optimize very common case of ASCII String vs ASCII String
        if constexpr (is_same_v<remove_cvref_t<LT>, String> and is_same_v<remove_cvref_t<RT>, String>) {
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
    template <Private_::ICanBeTreatedAsSpanOfCharacter_ LT, Private_::ICanBeTreatedAsSpanOfCharacter_ RT>
    bool String::EqualsComparer::Cmp_Generic_ (LT&& lhs, RT&& rhs) const
    {
        // separate function - cuz large stackframe and on windows generates chkstk calls, so dont have in
        // same frame where we do optimizations
        // and use smaller 'stackbuffer' size to avoid invoking _chkstk on VisualStudio (could do patform specific, but not clear there is a need) --LGP 2023-09-15
        Memory::StackBuffer<Character, 256> ignore1;
        Memory::StackBuffer<Character, 256> ignore2;
        return Character::Compare (Private_::AsSpanOfCharacters_ (forward<LT> (lhs), &ignore1),
                                   Private_::AsSpanOfCharacters_ (forward<RT> (rhs), &ignore2), fCompareOptions) == 0;
    }
    template <IConvertibleToString LT, IConvertibleToString RT>
    inline bool String::EqualsComparer::operator() (LT&& lhs, RT&& rhs) const
    {
        if constexpr (
            requires { lhs.size (); } and requires { rhs.size (); }) {
            if (lhs.size () != rhs.size ()) {
                return false; // performance tweak
            }
        }
        if constexpr (Private_::ICanBeTreatedAsSpanOfCharacter_<LT> and Private_::ICanBeTreatedAsSpanOfCharacter_<RT>) {
            return Cmp_ (forward<LT> (lhs), forward<RT> (rhs));
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
    template <Private_::ICanBeTreatedAsSpanOfCharacter_ LT, Private_::ICanBeTreatedAsSpanOfCharacter_ RT>
    inline strong_ordering String::ThreeWayComparer::Cmp_ (LT&& lhs, RT&& rhs) const
    {
        // optimize very common case of ASCII String vs ASCII String
        if constexpr (is_same_v<remove_cvref_t<LT>, String> and is_same_v<remove_cvref_t<RT>, String>) {
            if (auto lhsAsciiSpan = lhs.template PeekData<ASCII> ()) {
                if (auto rhsAsciiSpan = rhs.template PeekData<ASCII> ()) {
                    return Character::Compare (*lhsAsciiSpan, *rhsAsciiSpan, fCompareOptions);
                }
            }
        }
        return Cmp_Generic_ (forward<LT> (lhs), forward<RT> (rhs));
    }
    template <Private_::ICanBeTreatedAsSpanOfCharacter_ LT, Private_::ICanBeTreatedAsSpanOfCharacter_ RT>
    strong_ordering String::ThreeWayComparer::Cmp_Generic_ (LT&& lhs, RT&& rhs) const
    {
        // separate function - cuz large stackframe and on windows generates chkstk calls, so dont have in
        // same frame where we do optimizations
        // and use smaller 'stackbuffer' size to avoid invoking _chkstk on VisualStudio (could do patform specific, but not clear there is a need) --LGP 2023-09-15
        Memory::StackBuffer<Character, 256> ignore1;
        Memory::StackBuffer<Character, 256> ignore2;
        return Character::Compare (Private_::AsSpanOfCharacters_ (forward<LT> (lhs), &ignore1),
                                   Private_::AsSpanOfCharacters_ (forward<RT> (rhs), &ignore2), fCompareOptions);
    }
    template <IConvertibleToString LT, IConvertibleToString RT>
    inline strong_ordering String::ThreeWayComparer::operator() (LT&& lhs, RT&& rhs) const
    {
        if constexpr (Private_::ICanBeTreatedAsSpanOfCharacter_<LT> and Private_::ICanBeTreatedAsSpanOfCharacter_<RT>) {
            return Cmp_ (forward<LT> (lhs), forward<RT> (rhs));
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
    template <IConvertibleToString LHS_T, IConvertibleToString RHS_T>
    inline String operator+ (LHS_T&& lhs, RHS_T&& rhs)
        requires (derived_from<remove_cvref_t<LHS_T>, String> or derived_from<remove_cvref_t<RHS_T>, String>)
    {
        if constexpr (derived_from<remove_cvref_t<LHS_T>, String>) {
            return lhs.Concatenate (forward<RHS_T> (rhs));
        }
#if 0
        else if constexpr (Private_::ICanBeTreatedAsSpanOfCharacter_<LHS_T> and Private_::ICanBeTreatedAsSpanOfCharacter_<RHS_T>) {
            // maybe always true?
            Memory::StackBuffer<Character, 256> ignored1;
            span<const Character>               lSpan = Private_::AsSpanOfCharacters_ (forward<LHS_T> (lhs), &ignored1);
            Memory::StackBuffer<Character, 256> ignored2;
            span<const Character>               rSpan = Private_::AsSpanOfCharacters_ (forward<RHS_T> (rhs), &ignored2);
            Memory::StackBuffer<Character, 512> buf{Memory::eUninitialized, lSpan.size () + rSpan.size ()};
            span                                bufSpan{buf};
            Memory::CopySpanData (lSpan, bufSpan);
            Memory::CopySpanData (rSpan, bufSpan.subspan (lSpan.size ()));
            return String{bufSpan};
        }
#endif
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
    class [[deprecated ("Since Stroika v3.0 - just use String::FromStringConstant")]] String_Constant : public String {
    public:
        template <size_t SIZE>
        explicit String_Constant (const wchar_t (&cString)[SIZE])
            : String{String::FromStringConstant (std::basic_string_view<wchar_t>{cString, SIZE - 1})}
        {
        }

        String_Constant (const wchar_t* start, const wchar_t* end)
            : String{String::FromStringConstant (std::basic_string_view<wchar_t>{start, static_cast<size_t> (end - start)})}
        {
        }

        String_Constant (const std::basic_string_view<wchar_t>& str)
            : String{String::FromStringConstant (str)}
        {
        }
    };
}
namespace Stroika::Foundation::Characters::Concrete {
    class [[deprecated ("Since Stroika v3.0 - just use String::FromStringConstant")]] String_ExternalMemoryOwnership_ApplicationLifetime : public String {
    public:
        template <size_t SIZE>
        explicit String_ExternalMemoryOwnership_ApplicationLifetime (const wchar_t (&cString)[SIZE - 1])
            : String{String::FromStringConstant (basic_string_view<wchar_t>{cString, SIZE})}
        {
        }

        String_ExternalMemoryOwnership_ApplicationLifetime (const wchar_t* start, const wchar_t* end)
            : String{String::FromStringConstant (basic_string_view<wchar_t>{start, static_cast<size_t> (end - start)})}
        {
        }

        String_ExternalMemoryOwnership_ApplicationLifetime (const basic_string_view<wchar_t>& str)
            : String{String::FromStringConstant (str)}
        {
        }
    };
}

#if qCompilerAndStdLib_clangWithLibStdCPPStringConstexpr_Buggy
namespace {
    inline std::u8string clang_string_BWA_ (const char8_t* a, const char8_t* b)
    {
        return {a, b};
    }
}
#endif

#endif // _Stroika_Foundation_Characters_String_inl_
