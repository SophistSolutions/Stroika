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
#include <cwctype>
#include <type_traits>

#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Characters {

    namespace Private_ {
        template <Character_Compatible CHAR_T>
        constexpr strong_ordering Compare_CS_ (span<const CHAR_T> lhs, span<const CHAR_T> rhs)
        {
            size_t        lLen   = lhs.size ();
            size_t        rLen   = rhs.size ();
            size_t        length = min (lLen, rLen);
            const CHAR_T* li     = lhs.data ();
            const CHAR_T* ri     = rhs.data ();
            for (size_t i = 0; i < length; ++i, ++li, ++ri) {
                if constexpr (is_same_v<CHAR_T, Character>) {
                    if (li->GetCharacterCode () != ri->GetCharacterCode ()) {
                        return li->GetCharacterCode () <=> ri->GetCharacterCode ();
                    }
                }
                else {
                    if (*li != *ri) {
                        return *li <=> *ri;
                    }
                }
            }
            return Common::CompareResultNormalizer (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
        }
        template <Character_Compatible CHAR_T>
        constexpr strong_ordering Compare_CI_ (span<const CHAR_T> lhs, span<const CHAR_T> rhs)
        {
            size_t        lLen   = lhs.size ();
            size_t        rLen   = rhs.size ();
            size_t        length = min (lLen, rLen);
            const CHAR_T* li     = lhs.data ();
            const CHAR_T* ri     = rhs.data ();
            for (size_t i = 0; i < length; ++i, ++li, ++ri) {
                if constexpr (is_same_v<CHAR_T, Character>) {
                    Character lc = li->ToLowerCase ();
                    Character rc = ri->ToLowerCase ();
                    if (lc != rc) {
                        return lc <=> rc;
                    }
                }
                else {
                    // see https://en.cppreference.com/w/cpp/string/byte/tolower for rationale for this crazy casting
                    CHAR_T lc = static_cast<CHAR_T> (tolower (static_cast<unsigned char> (*li)));
                    CHAR_T rc = static_cast<CHAR_T> (tolower (static_cast<unsigned char> (*ri)));
                    if (lc != rc) {
                        return lc <=> rc;
                    }
                }
            }
            return Common::CompareResultNormalizer (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
        }
        void ThrowNotIsASCII_ ();
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
         *      0x10000 + (H - 0xD800) × 0x400 + (L - 0xDC00)
         */
        constexpr int      halfShift = 10; /* used for shifting by 10 bits */
        constexpr char32_t halfBase  = 0x0010000UL;
        if (not(UNI_SUR_HIGH_START <= hiSurrogate and hiSurrogate <= UNI_SUR_HIGH_END)) {
            Private_::ThrowSurrogatesOutOfRange_ ();
        }
        if (not(UNI_SUR_LOW_START <= lowSurrogate and lowSurrogate <= UNI_SUR_LOW_END)) {
            Private_::ThrowSurrogatesOutOfRange_ ();
        }
        fCharacterCode_ = ((hiSurrogate - UNI_SUR_HIGH_START) << halfShift) + (lowSurrogate - UNI_SUR_LOW_START) + halfBase;
    }
    inline char Character::GetAsciiCode () const noexcept
    {
        Require (IsASCII ());
        return static_cast<char> (fCharacterCode_);
    }
    constexpr char32_t Character::GetCharacterCode () const noexcept
    {
        return fCharacterCode_;
    }
    constexpr Character::operator char32_t () const noexcept
    {
        return fCharacterCode_;
    }
    template <typename T>
    inline T Character::As () const noexcept
        requires (is_same_v<T, char32_t> or (sizeof (wchar_t) == sizeof (char32_t) and is_same_v<T, wchar_t>))
    {
        return GetCharacterCode ();
    }
    constexpr bool Character::IsASCII () const noexcept
    {
        return 0x0 <= fCharacterCode_ and fCharacterCode_ <= 0x7f;
    }
    template <Character_Compatible CHAR_T>
    constexpr bool Character::IsASCII (span<const CHAR_T> fromS) noexcept
    {
        // note - tried to simplify with conditional_t but both sides evaluated
        if constexpr (is_same_v<remove_cv_t<CHAR_T>, Character>) {
            for (Character c : fromS) {
                if (not c.IsASCII ()) [[unlikely]] {
                    return false;
                }
            }
        }
        else {
            for (CHAR_T c : fromS) {
                if (static_cast<make_unsigned_t<CHAR_T>> (c) > 127) [[unlikely]] {
                    return false;
                }
            }
        }
        return true;
    }
    template <Character_Compatible CHAR_T>
    inline void Character::CheckASCII (span<const CHAR_T> s)
    {
        if (not IsASCII (s)) {
            Private_::ThrowNotIsASCII_ ();
        }
    }
    template <Character_Compatible CHAR_T>
    inline void Character::CheckASCII (span<CHAR_T> s)
    {
        CheckASCII (Memory::ConstSpan (s));
    }
    constexpr bool Character::IsWhitespace () const noexcept
    {
        bool result = false;
        if (0x09 <= fCharacterCode_ and fCharacterCode_ <= 0x0D) {
            result = true;
        }
        else if (fCharacterCode_ == 0x20) {
            result = true;
        }
        else if (fCharacterCode_ >= 0x1680) {
            // rarely get chars this big, so shortcut all the detailed tests
            if (fCharacterCode_ == 0x1680 or fCharacterCode_ == 0x180E) {
                result = true;
            }
            else if (0x2000 <= fCharacterCode_ and fCharacterCode_ <= 0x2006) {
                result = true;
            }
            else if (0x2008 <= fCharacterCode_ and fCharacterCode_ <= 0x200A) {
                result = true;
            }
            else if (fCharacterCode_ == 0x2028 or fCharacterCode_ == 0x2029 or fCharacterCode_ == 0x205F or fCharacterCode_ == 0x3000) {
                result = true;
            }
        }
        DISABLE_COMPILER_MSC_WARNING_START (5063)
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wconstant-evaluated\"");
        if constexpr (not std::is_constant_evaluated ()) {
            Ensure (result == !!iswspace (static_cast<wchar_t> (fCharacterCode_)));
        }
        DISABLE_COMPILER_MSC_WARNING_END (5063)
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wconstant-evaluated\"");
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
    inline bool Character::IsControl () const noexcept
    {
        return !!iswcntrl (static_cast<wchar_t> (fCharacterCode_));
    }
    inline Character Character::ToLowerCase () const noexcept
    {
        // https://stackoverflow.com/questions/60353945/isthing-equivalents-for-char32-t
        // Cannot find good spec on towlower/towupper, so not sure that this check is necessary
        if (::iswupper (static_cast<wchar_t> (fCharacterCode_))) {
            return static_cast<wchar_t> (::towlower (static_cast<wchar_t> (fCharacterCode_)));
        }
        else {
            return fCharacterCode_;
        }
    }
    inline Character Character::ToUpperCase () const noexcept
    {
        // https://stackoverflow.com/questions/60353945/isthing-equivalents-for-char32-t
        if (::iswlower (static_cast<wchar_t> (fCharacterCode_))) {
            return static_cast<wchar_t> (::towupper (static_cast<wchar_t> (fCharacterCode_)));
        }
        else {
            return fCharacterCode_;
        }
    }
    template <typename RESULT_T, Character_Compatible CHAR_T>
    inline bool Character::AsASCIIQuietly (span<const CHAR_T> fromS, RESULT_T* into)
        requires (is_same_v<RESULT_T, string> or is_same_v<RESULT_T, Memory::StackBuffer<char>>)
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
    template <Character_Compatible CHAR_T>
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
    inline auto Character::ThreeWayComparer::operator() (Stroika::Foundation::Characters::Character lhs, Stroika::Foundation::Characters::Character rhs) const noexcept
    {
        using namespace Stroika::Foundation::Characters;
        return Character::Compare (Memory::ConstSpan (span{&lhs, 1}), Memory::ConstSpan (span{&rhs, 1}), fCompareOptions);
    }

}

#endif /*_Stroika_Foundation_Characters_Character_inl_*/
