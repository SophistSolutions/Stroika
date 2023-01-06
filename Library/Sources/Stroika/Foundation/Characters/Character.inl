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
