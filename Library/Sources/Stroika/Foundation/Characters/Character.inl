/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Character_inl_
#define _Stroika_Foundation_Characters_Character_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <bit>
#include <cwctype>
#include <ranges>
#include <type_traits>

#include "../Debug/Assertions.h"
#include "../Memory/Span.h"

namespace Stroika::Foundation::Characters {

    namespace Private_ {
        template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
        constexpr strong_ordering Compare_CS_ (span<const CHAR_T> lhs, span<const CHAR_T> rhs)
        {
            size_t        lLen   = lhs.size ();
            size_t        rLen   = rhs.size ();
            size_t        length = min (lLen, rLen);
            const CHAR_T* li     = lhs.data ();
            const CHAR_T* ri     = rhs.data ();

            // when can we use memcmp() instead of a loop comparing?
            // for sizeof CHAR_T == 1, sure.
            // for bigger CHAR_T, we have to worry about endianness
            // |HI-1|LO-1|HI-2|LO-2... - for this case, we are all set, because
            // this will have the same 'ordering' when we compare characters as when we compare as bytes
            constexpr bool kCanUseMemCmpOptimization_ = sizeof (CHAR_T) == 1 or (std::endian::native == std::endian::big);

            // tested on windows, and no obvious difference
            constexpr bool kUseStdTraitsCompare_ = true;

            if constexpr (kUseStdTraitsCompare_) {
                using TRAITS_CHAR_T = conditional_t<sizeof (CHAR_T) == 4, char32_t, CHAR_T>;
                int r               = std::char_traits<TRAITS_CHAR_T>::compare (reinterpret_cast<const TRAITS_CHAR_T*> (li),
                                                                                reinterpret_cast<const TRAITS_CHAR_T*> (ri), length);
                if (r != 0) [[likely]] {
                    return Common::CompareResultNormalizer (r);
                }
            }
            else if constexpr (kCanUseMemCmpOptimization_) {
                int r = std::memcmp (li, ri, length);
                if (r != 0) [[likely]] {
                    return Common::CompareResultNormalizer (r);
                }
            }
            else {
                const CHAR_T* lend = li + length; // just end of what we are comparing in this loop
                for (; li != lend; ++li, ++ri) {
                    if (*li != *ri) [[likely]] {
                        return *li <=> *ri;
                    }
                }
            }
            return Common::CompareResultNormalizer (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
        }
        template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
        constexpr strong_ordering Compare_CI_ (span<const CHAR_T> lhs, span<const CHAR_T> rhs)
        {
            size_t        lLen   = lhs.size ();
            size_t        rLen   = rhs.size ();
            size_t        length = min (lLen, rLen);
            const CHAR_T* li     = lhs.data ();
            const CHAR_T* ri     = rhs.data ();
            const CHAR_T* lend   = li + length; // just end of what we are comparing in this loop
            for (; li != lend; ++li, ++ri) {
                CHAR_T lc; // intentionally uninitialized
                CHAR_T rc; // ""
                if constexpr (is_same_v<CHAR_T, Character>) {
                    lc = li->ToLowerCase ();
                    rc = ri->ToLowerCase ();
                }
                else if constexpr (is_same_v<CHAR_T, Character_ASCII>) {
                    // @todo NOT SURE if this works for Character_Latin1...
                    // see https://en.cppreference.com/w/cpp/string/byte/tolower for rationale for this crazy casting
                    lc = static_cast<CHAR_T> (std::tolower (static_cast<unsigned char> (*li)));
                    rc = static_cast<CHAR_T> (std::tolower (static_cast<unsigned char> (*ri)));
                }
                else {
                    // see https://en.cppreference.com/w/cpp/string/byte/tolower for rationale for this crazy casting
                    lc = static_cast<CHAR_T> (std::towlower (static_cast<wchar_t> (*li)));
                    rc = static_cast<CHAR_T> (std::towlower (static_cast<wchar_t> (*ri)));
                }
                if (lc != rc) [[likely]] {
                    return lc <=> rc;
                }
            }
            return Common::CompareResultNormalizer (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
        }
        void ThrowNotIsASCII_ ();
        void ThrowNotIsLatin1_ ();
        void ThrowSurrogatesOutOfRange_ ();
    }

    /*
     ********************************************************************************
     *********************************** Character **********************************
     ********************************************************************************
     */
    constexpr inline Character::Character () noexcept
        : fCharacterCode_{'\0'}
    {
    }
    constexpr inline Character::Character (char32_t c) noexcept
        : fCharacterCode_{c}
    {
    }
    constexpr Character::Character (char16_t hiSurrogate, char16_t lowSurrogate)
    {
        /*
         * See https://en.wikipedia.org/wiki/Universal_Character_Set_characters#Surrogates
         * 
         * A surrogate pair denotes the code point
         *      0x10000 + (H - 0xD800) x 0x400 + (L - 0xDC00)
         */
        constexpr int      halfShift = 10; /* used for shifting by 10 bits */
        constexpr char32_t halfBase  = 0x0010000UL;
        if (not IsSurrogatePair_Hi (hiSurrogate)) [[unlikely]] {
            Private_::ThrowSurrogatesOutOfRange_ ();
        }
        if (not IsSurrogatePair_Lo (lowSurrogate)) [[unlikely]] {
            Private_::ThrowSurrogatesOutOfRange_ ();
        }
        fCharacterCode_ = ((hiSurrogate - UNI_SUR_HIGH_START) << halfShift) + (lowSurrogate - UNI_SUR_LOW_START) + halfBase;
    }
    inline char Character::GetAsciiCode () const noexcept
    {
        Require (IsASCII ());
        return static_cast<char> (fCharacterCode_);
    }
    constexpr char32_t   Character::GetCharacterCode () const noexcept { return fCharacterCode_; }
    constexpr Character::operator char32_t () const noexcept { return fCharacterCode_; }
    template <typename T>
    inline T Character::As () const noexcept
        requires (is_same_v<T, char32_t> or (sizeof (wchar_t) == sizeof (char32_t) and is_same_v<T, wchar_t>))
    {
        return GetCharacterCode ();
    }
    constexpr bool Character::IsASCII () const noexcept { return 0x0 <= fCharacterCode_ and fCharacterCode_ <= 0x7f; }
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    constexpr bool Character::IsASCII (span<const CHAR_T> fromS) noexcept
    {
        constexpr auto charComparer = [] () noexcept {
            if constexpr (is_same_v<remove_cv_t<CHAR_T>, Character>) {
                return [] (Character c) noexcept { return c.IsASCII (); };
            }
            else if constexpr (is_same_v<remove_cv_t<CHAR_T>, Character_Latin1>) {
                return [] (Character_Latin1 c) noexcept { return static_cast<uint8_t> (c) <= 127; };
            }
            else {
                return [] (CHAR_T c) noexcept { return static_cast<make_unsigned_t<CHAR_T>> (c) <= 127; };
            }
        }();
        return ranges::all_of (fromS, charComparer);
    }
    template <Character_Compatible CHAR_T>
    inline void Character::CheckASCII (span<const CHAR_T> s)
    {
        if (not IsASCII (s)) [[unlikely]] {
            Private_::ThrowNotIsASCII_ ();
        }
    }
    template <Character_Compatible CHAR_T>
    inline void Character::CheckASCII (span<CHAR_T> s)
    {
        CheckASCII (Memory::ConstSpan (s));
    }
    constexpr bool Character::IsLatin1 () const noexcept { return 0x0 <= fCharacterCode_ and fCharacterCode_ <= 0xff; }
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    constexpr bool Character::IsLatin1 (span<const CHAR_T> fromS) noexcept
    {
        if constexpr (is_same_v<CHAR_T, Character_ASCII> or is_same_v<CHAR_T, Character_Latin1>) {
            // then data must be ascii or latin1, since any byte is latin1
            return true;
        }
        else if constexpr (is_same_v<CHAR_T, char8_t>) {
            // For the special case of UTF-8, tricky to tell if its latin1 or not. Must iterate through the remaining
            // two-byte pairs, and make sure they are 110xxxxx followed by 10xxxxxx where the xxx's get unpacked in to <= 0xff
            if (fromS.size () % 2 == 0) {
                for (auto i = fromS.begin (); i < fromS.end (); ++i) {
                    char8_t c1 = *i++;
                    char8_t c2 = *i;
                    // Check letgit high order bits for first and second byte
                    // and to check RANGE of values being latin1, means bit pattern only
                    // 8 bits. Low order bits in second byte, so dont need to examine those.
                    // Just make sure at most two bits set in upper (first) byte
                    if ((c1 & 0b11100000) == 0b11000000 and ((c2 & 0b11000000) == 0b10000000 and (c1 & 0b00111111) <= 0b011)) [[likely]] {
                        // OK
                    }
                    else {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }
        else {
            static_assert (2 <= sizeof (CHAR_T) and sizeof (CHAR_T) <= 4);
            constexpr auto charComparer = [] () noexcept {
                if constexpr (is_same_v<remove_cv_t<CHAR_T>, Character>) {
                    return [] (Character c) noexcept { return c.IsLatin1 (); };
                }
                else {
                    return [] (CHAR_T c) noexcept { return static_cast<make_unsigned_t<CHAR_T>> (c) <= 0xff; };
                }
            }();
            return ranges::all_of (fromS, charComparer);
        }
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline void Character::CheckLatin1 (span<const CHAR_T> s)
    {
        if (not IsLatin1 (s)) [[unlikely]] {
            Private_::ThrowNotIsLatin1_ ();
        }
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    inline void Character::CheckLatin1 (span<CHAR_T> s)
    {
        CheckLatin1 (Memory::ConstSpan (s));
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    constexpr auto Character::IsASCIIOrLatin1 (span<const CHAR_T> s) noexcept -> ASCIIOrLatin1Result
    {
        constexpr auto eNone   = ASCIIOrLatin1Result::eNone;
        constexpr auto eLatin1 = ASCIIOrLatin1Result::eLatin1;
        constexpr auto eASCII  = ASCIIOrLatin1Result::eASCII;
        if constexpr (is_same_v<CHAR_T, Character_ASCII> or is_same_v<CHAR_T, Character_Latin1>) {
            // then data must be ascii or latin1, since any byte is latin1
            return IsASCII (s) ? eASCII : eLatin1;
        }
        else {
            constexpr auto isASCII = [] () noexcept {
                if constexpr (is_same_v<remove_cv_t<CHAR_T>, Character>) {
                    return [] (Character c) noexcept { return c.IsASCII (); };
                }
                else {
                    return [] (CHAR_T c) noexcept { return static_cast<make_unsigned_t<CHAR_T>> (c) <= 0x7f; };
                }
            }();
            auto   leadingASCIISpan    = ranges::take_while_view (s, isASCII);
            size_t leadingAsciiCharCnt = static_cast<size_t> (ranges::distance (leadingASCIISpan));
            if (leadingAsciiCharCnt == s.size ()) [[likely]] {
                return eASCII;
            }
            span remainingInputSpan = s.subspan (leadingAsciiCharCnt);
            if constexpr (is_same_v<CHAR_T, char8_t>) {
                // special case - we need different algorithm looking at pairs of entries, to see if IsLatin1 with utf8
                return IsLatin1 (remainingInputSpan) ? eLatin1 : eNone;
            }
            constexpr auto isLatin1 = [] () noexcept {
                if constexpr (is_same_v<remove_cv_t<CHAR_T>, Character>) {
                    return [] (Character c) noexcept { return c.IsLatin1 (); };
                }
                else {
                    return [] (CHAR_T c) noexcept { return static_cast<make_unsigned_t<CHAR_T>> (c) <= 0xff; };
                }
            }();
            auto remainingLatin1 = ranges::take_while_view (remainingInputSpan, isLatin1);
            if (static_cast<size_t> (ranges::distance (remainingLatin1)) == remainingInputSpan.size ()) [[likely]] {
                return eLatin1;
            }
            return eNone;
        }
    }
    constexpr bool Character::IsWhitespace () const noexcept
    {
        bool result = false;
        if (0x09 <= fCharacterCode_ and fCharacterCode_ <= 0x0D) [[unlikely]] {
            result = true;
        }
        else if (fCharacterCode_ == 0x20) [[unlikely]] {
            result = true;
        }
        else if (fCharacterCode_ >= 0x1680) [[unlikely]] {
            // rarely get chars this big, so shortcut all the detailed tests
            if (fCharacterCode_ == 0x1680 or fCharacterCode_ == 0x180E) [[unlikely]] {
                result = true;
            }
            else if (0x2000 <= fCharacterCode_ and fCharacterCode_ <= 0x2006) [[unlikely]] {
                result = true;
            }
            else if (0x2008 <= fCharacterCode_ and fCharacterCode_ <= 0x200A) [[unlikely]] {
                result = true;
            }
            else if (fCharacterCode_ == 0x2028 or fCharacterCode_ == 0x2029 or fCharacterCode_ == 0x205F or fCharacterCode_ == 0x3000) [[unlikely]] {
                result = true;
            }
        }
        DISABLE_COMPILER_MSC_WARNING_START (5063)
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wconstant-evaluated\"");
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wtautological-compare\"");
        if constexpr (not std::is_constant_evaluated ()) {
            Ensure (result == !!iswspace (static_cast<wchar_t> (fCharacterCode_)));
        }
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wtautological-compare\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wconstant-evaluated\"");
        DISABLE_COMPILER_MSC_WARNING_END (5063)
        return result;
    }
    inline bool Character::IsDigit () const noexcept
    {
        // https://stackoverflow.com/questions/60353945/isthing-equivalents-for-char32-t
        // @todo RECONSIDER IF THIS IS RIGHT FOR char32_t?
        return !!iswdigit (static_cast<wchar_t> (fCharacterCode_));
    }
    inline bool Character::IsHexDigit () const noexcept
    {
        // https://stackoverflow.com/questions/60353945/isthing-equivalents-for-char32-t
        return !!iswxdigit (static_cast<wchar_t> (fCharacterCode_));
    }
    inline bool Character::IsAlphabetic () const noexcept
    {
        // https://stackoverflow.com/questions/60353945/isthing-equivalents-for-char32-t
        return !!iswalpha (static_cast<wchar_t> (fCharacterCode_));
    }
    inline bool Character::IsUpperCase () const noexcept
    {
        // https://stackoverflow.com/questions/60353945/isthing-equivalents-for-char32-t
        return !!iswupper (static_cast<wchar_t> (fCharacterCode_));
    }
    inline bool Character::IsLowerCase () const noexcept
    {
        // https://stackoverflow.com/questions/60353945/isthing-equivalents-for-char32-t
        return !!iswlower (static_cast<wchar_t> (fCharacterCode_));
    }
    inline bool Character::IsAlphaNumeric () const noexcept
    {
        // https://stackoverflow.com/questions/60353945/isthing-equivalents-for-char32-t
        return !!iswalnum (static_cast<wchar_t> (fCharacterCode_));
    }
    inline bool Character::IsPunctuation () const noexcept
    {
        // https://stackoverflow.com/questions/60353945/isthing-equivalents-for-char32-t
        return !!iswpunct (static_cast<wchar_t> (fCharacterCode_));
    }
    constexpr bool Character::IsControl () const noexcept
    {
        /*
         *  According to https://en.cppreference.com/w/cpp/string/wide/iswcntrl
         * 
         *  ISO 30112 defines POSIX control characters as Unicode characters U+0000..U+001F, 
         *  U+007F..U+009F, U+2028, and U+2029 (Unicode classes Cc, Zl, and Zp)
         * 
         *  Be explicit here so can use constexpr
         *
         *  WAS:    return !!iswcntrl (static_cast<wchar_t> (fCharacterCode_));
         */
        if (0 <= fCharacterCode_ and fCharacterCode_ <= 0x1f) [[unlikely]] {
            return true;
        }
        if (0x7f <= fCharacterCode_ and fCharacterCode_ <= 0x9f) [[unlikely]] {
            return true;
        }
        if (0x2028 == fCharacterCode_ or 0x2029 == fCharacterCode_) [[unlikely]] {
            return true;
        }
        return false;
    }
    inline Character Character::ToLowerCase () const noexcept
    {
        // https://stackoverflow.com/questions/60353945/isthing-equivalents-for-char32-t
        // Cannot find good spec on towlower/towupper, so not sure that this check is necessary
        //
        // before Stroika v3.0d1 - we used to check iswupper first, but according to https://en.cppreference.com/w/cpp/string/wide/towlower
        // that appears unnecessary
        return static_cast<wchar_t> (::towlower (static_cast<wchar_t> (fCharacterCode_)));
    }
    inline Character Character::ToUpperCase () const noexcept
    {
        // See ToLowerCase() for implementation comments
        return static_cast<wchar_t> (::towupper (static_cast<wchar_t> (fCharacterCode_)));
    }
    template <typename RESULT_T, Character_Compatible CHAR_T>
    inline bool Character::AsASCIIQuietly (span<const CHAR_T> fromS, RESULT_T* into)
        requires (is_same_v<RESULT_T, string> or is_same_v<RESULT_T, Memory::StackBuffer<Character_ASCII>>)
    {
        RequireNotNull (into);
        into->clear ();
        // note - tried to simplify with conditional_t but both sides evaluated
        if constexpr (is_same_v<remove_cv_t<CHAR_T>, Character>) {
            for (Character c : fromS) {
                if (c.IsASCII ()) [[likely]] {
                    into->push_back (c.GetAsciiCode ());
                }
                else {
                    return false;
                }
            }
        }
        else {
            for (CHAR_T c : fromS) {
                if (static_cast<make_unsigned_t<CHAR_T>> (c) <= 127) [[likely]] {
                    into->push_back (static_cast<char> (c));
                }
                else {
                    return false;
                }
            }
        }
        return true;
    }
    template <Character_UNICODECanUnambiguouslyConvertFrom CHAR_T>
    constexpr strong_ordering Character::Compare (span<const CHAR_T> lhs, span<const CHAR_T> rhs, CompareOptions co) noexcept
    {
        Require (co == CompareOptions::eWithCase or co == CompareOptions::eCaseInsensitive);
        switch (co) {
            case CompareOptions::eWithCase:
                return Private_::Compare_CS_ (lhs, rhs);
            case CompareOptions::eCaseInsensitive:
                return Private_::Compare_CI_ (lhs, rhs);
            default:
                AssertNotReached ();
                return strong_ordering::equal;
        }
    }
    constexpr bool Character::IsSurrogatePair () const
    {
        /*
         * See https://en.wikipedia.org/wiki/Universal_Character_Set_characters#Surrogates
         * 
         * A surrogate pair denotes the code point
         *      0x10000 + (H - 0xD800) x 0x400 + (L - 0xDC00)
         */
        constexpr char32_t kMinCode_ = 0x10000;
        constexpr char32_t kMaxCode_ = kMinCode_ + (UNI_SUR_HIGH_END - UNI_SUR_HIGH_START) * 0x400 + (UNI_SUR_LOW_END - UNI_SUR_LOW_START);
        return kMinCode_ <= fCharacterCode_ and fCharacterCode_ <= kMaxCode_;
    }
    constexpr bool Character::IsSurrogatePair (char16_t hiSurrogate, char16_t lowSurrogate)
    {
        return IsSurrogatePair_Hi (hiSurrogate) and IsSurrogatePair_Lo (lowSurrogate);
    }
    constexpr bool Character::IsSurrogatePair_Hi (char16_t hiSurrogate)
    {
        return UNI_SUR_HIGH_START <= hiSurrogate and hiSurrogate <= UNI_SUR_HIGH_END;
    }
    constexpr bool Character::IsSurrogatePair_Lo (char16_t lowSurrogate)
    {
        return UNI_SUR_LOW_START <= lowSurrogate and lowSurrogate <= UNI_SUR_LOW_END;
    }

    /*
     ********************************************************************************
     *************************** Character::EqualsComparer **************************
     ********************************************************************************
     */
    constexpr Character::EqualsComparer::EqualsComparer (CompareOptions co) noexcept
        : fCompareOptions{co}
    {
    }
    constexpr bool Character::EqualsComparer::operator() (Character lhs, Character rhs) const noexcept
    {
        using namespace Stroika::Foundation::Characters;
        return Character::Compare (Memory::ConstSpan (span{&lhs, 1}), Memory::ConstSpan (span{&rhs, 1}), fCompareOptions) == 0;
    }

    /*
     ********************************************************************************
     ************************* Character::ThreeWayComparer **************************
     ********************************************************************************
     */
    constexpr Character::ThreeWayComparer::ThreeWayComparer (Stroika::Foundation::Characters::CompareOptions co) noexcept
        : fCompareOptions{co}
    {
    }
    inline auto Character::ThreeWayComparer::operator() (Stroika::Foundation::Characters::Character lhs,
                                                         Stroika::Foundation::Characters::Character rhs) const noexcept
    {
        using namespace Stroika::Foundation::Characters;
        return Character::Compare (Memory::ConstSpan (span{&lhs, 1}), Memory::ConstSpan (span{&rhs, 1}), fCompareOptions);
    }

}

#endif /*_Stroika_Foundation_Characters_Character_inl_*/
