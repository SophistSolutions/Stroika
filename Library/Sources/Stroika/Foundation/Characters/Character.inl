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

    /*
     ********************************************************************************
     *********************************** Character **********************************
     ********************************************************************************
     */
    constexpr inline Character::Character () noexcept
        : fCharacterCode_{'\0'}
    {
    }
    constexpr inline Character::Character (char c) noexcept
        : fCharacterCode_{static_cast<wchar_t> (c)}
    {
    }
    constexpr inline Character::Character (char16_t c) noexcept
        : fCharacterCode_{c}
    {
    }
    DISABLE_COMPILER_MSC_WARNING_START (4244)
    // NB: THIS IS A REAL BUG/ISSUE. WE MUST DO WORK TO TRANSFORM. REDESIGN REQUIRED!!!
    constexpr inline Character::Character (char32_t c) noexcept
        : fCharacterCode_{static_cast<wchar_t> (c)}
    {
    }
    DISABLE_COMPILER_MSC_WARNING_END (4244)
    constexpr inline Character::Character (wchar_t wc) noexcept
        : fCharacterCode_{wc}
    {
    }
    inline char Character::GetAsciiCode () const noexcept
    {
        Require (IsASCII ());
        return static_cast<char> (fCharacterCode_);
    }
    inline wchar_t Character::GetCharacterCode () const noexcept
    {
        return fCharacterCode_;
    }
    template <typename T>
    inline T Character::As () const noexcept
        requires (is_same_v<T, char32_t> or is_same_v<T, wchar_t>)
    {
        return GetCharacterCode ();
    }
    inline bool Character::IsASCII () const noexcept
    {
        return 0x0 <= fCharacterCode_ and fCharacterCode_ <= 0x7f;
    }
    inline bool Character::IsWhitespace () const noexcept
    {
        return !!iswspace (fCharacterCode_);
    }
    inline bool Character::IsDigit () const noexcept
    {
        return !!iswdigit (fCharacterCode_);
    }
    inline bool Character::IsHexDigit () const noexcept
    {
        return !!iswxdigit (fCharacterCode_);
    }
    inline bool Character::IsAlphabetic () const noexcept
    {
        return !!iswalpha (fCharacterCode_);
    }
    inline bool Character::IsUpperCase () const noexcept
    {
        return !!iswupper (fCharacterCode_);
    }
    inline bool Character::IsLowerCase () const noexcept
    {
        return !!iswlower (fCharacterCode_);
    }
    inline bool Character::IsAlphaNumeric () const noexcept
    {
        return !!iswalnum (fCharacterCode_);
    }
    inline bool Character::IsPunctuation () const noexcept
    {
        return !!iswpunct (fCharacterCode_);
    }
    inline bool Character::IsControl () const noexcept
    {
        return !!iswcntrl (fCharacterCode_);
    }
    inline Character Character::ToLowerCase () const noexcept
    {
        // Cannot find good spec on towlower/towupper, so not sure that this check is necessary
        if (::iswupper (fCharacterCode_)) {
            return static_cast<wchar_t> (::towlower (fCharacterCode_));
        }
        else {
            return fCharacterCode_;
        }
    }
    inline Character Character::ToUpperCase () const noexcept
    {
        if (::iswlower (fCharacterCode_)) {
            return static_cast<wchar_t> (::towupper (fCharacterCode_));
        }
        else {
            return fCharacterCode_;
        }
    }
    template <Character_Compatible CHAR_T>
    inline bool Character::IsASCII (span<const CHAR_T> fromS) noexcept
    {
        // note - tried to simplify with conditional_t but both sides evaluated
        if constexpr (is_same_v<remove_cv_t<CHAR_T>, Character>) {
            for (Character c : fromS) {
                if (c.IsASCII ()) [[unlikely]] {
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
        switch (fCompareOptions) {
            case CompareOptions::eCaseInsensitive:
                return Character::Compare (&lhs, &lhs + 1, &rhs, &rhs + 1, CompareOptions::eCaseInsensitive) == 0;
            case CompareOptions::eWithCase:
                return lhs.GetCharacterCode () == rhs.GetCharacterCode ();
            default:
                AssertNotReached ();
                return false;
        }
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
        using SIGNED_WCHART_ = make_signed_t<wchar_t>;
        switch (fCompareOptions) {
            case CompareOptions::eCaseInsensitive:
                return Character::Compare (&lhs, &lhs + 1, &rhs, &rhs + 1, CompareOptions::eCaseInsensitive);
            case CompareOptions::eWithCase:
                return static_cast<SIGNED_WCHART_> (lhs.GetCharacterCode ()) <=> static_cast<SIGNED_WCHART_> (rhs.GetCharacterCode ());
            default:
                AssertNotReached ();
                return strong_ordering::equal;
        }
    }

}

#endif /*_Stroika_Foundation_Characters_Character_inl_*/
