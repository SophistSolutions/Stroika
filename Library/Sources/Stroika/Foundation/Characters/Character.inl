/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

namespace Stroika::Foundation {
    namespace Characters {

        /*
         ********************************************************************************
         *********************************** Character **********************************
         ********************************************************************************
         */
        constexpr inline Character::Character ()
            : fCharacterCode_ ('\0')
        {
        }
        constexpr inline Character::Character (char c)
            : fCharacterCode_ (c)
        {
        }
        constexpr inline Character::Character (char16_t c)
            : fCharacterCode_ (c)
        {
        }
        DISABLE_COMPILER_MSC_WARNING_START (4244)
        // NB: THIS IS A REAL BUG/ISSUE. WE MUST DO WORK TO TRANSFORM. REDESIGN REQUIRED!!!
        constexpr inline Character::Character (char32_t c)
            : fCharacterCode_ (c)
        {
        }
        DISABLE_COMPILER_MSC_WARNING_END (4244)
        constexpr inline Character::Character (wchar_t wc)
            : fCharacterCode_ (wc)
        {
        }
        inline char Character::GetAsciiCode () const
        {
            Require (IsASCII ());
            return static_cast<char> (fCharacterCode_);
        }
        inline wchar_t Character::GetCharacterCode () const
        {
            return (fCharacterCode_);
        }
        template <>
        inline wchar_t Character::As () const
        {
            return GetCharacterCode ();
        }
        template <>
        inline char32_t Character::As () const
        {
            return GetCharacterCode ();
        }
        inline bool Character::IsASCII () const
        {
            return 0x0 <= fCharacterCode_ and fCharacterCode_ <= 0x7f;
        }
        inline bool Character::IsWhitespace () const
        {
            return !!iswspace (fCharacterCode_);
        }
        inline bool Character::IsDigit () const
        {
            return !!iswdigit (fCharacterCode_);
        }
        inline bool Character::IsHexDigit () const
        {
            return !!iswxdigit (fCharacterCode_);
        }
        inline bool Character::IsAlphabetic () const
        {
            return !!iswalpha (fCharacterCode_);
        }
        inline bool Character::IsUpperCase () const
        {
            return !!iswupper (fCharacterCode_);
        }
        inline bool Character::IsLowerCase () const
        {
            return !!iswlower (fCharacterCode_);
        }
        inline bool Character::IsAlphaNumeric () const
        {
            return !!iswalnum (fCharacterCode_);
        }
        inline bool Character::IsPunctuation () const
        {
            return !!iswpunct (fCharacterCode_);
        }
        inline bool Character::IsControl () const
        {
            return !!iswcntrl (fCharacterCode_);
        }
        inline Character Character::ToLowerCase () const
        {
            // Cannot find good spec on towlower/towupper, so not sure that this check is necessary
            if (::iswupper (fCharacterCode_)) {
                return static_cast<wchar_t> (::towlower (fCharacterCode_));
            }
            else {
                return fCharacterCode_;
            }
        }
        inline Character Character::ToUpperCase () const
        {
            if (::iswlower (fCharacterCode_)) {
                return static_cast<wchar_t> (::towupper (fCharacterCode_));
            }
            else {
                return fCharacterCode_;
            }
        }
        inline int Character::Compare (Character rhs) const
        {
            using SIGNED_WCHART_ = make_signed_t<wchar_t>;
            return static_cast<SIGNED_WCHART_> (GetCharacterCode ()) - static_cast<SIGNED_WCHART_> (rhs.GetCharacterCode ());
        }
        inline int Character::Compare (Character rhs, CompareOptions co) const
        {
            return Compare (this, this + 1, &rhs, &rhs + 1, co);
        }

        /*
            ********************************************************************************
            ************************* Character operators **********************************
            ********************************************************************************
            */
        inline bool operator< (Character lhs, Character rhs)
        {
            return lhs.Compare (rhs) < 0;
        }
        inline bool operator<= (Character lhs, Character rhs)
        {
            return lhs.Compare (rhs) <= 0;
        }
        inline bool operator== (Character lhs, Character rhs)
        {
            return lhs.Compare (rhs) == 0;
        }
        inline bool operator!= (Character lhs, Character rhs)
        {
            return lhs.Compare (rhs) != 0;
        }
        inline bool operator>= (Character lhs, Character rhs)
        {
            return lhs.Compare (rhs) >= 0;
        }
        inline bool operator> (Character lhs, Character rhs)
        {
            return lhs.Compare (rhs) > 0;
        }
    }
}
#endif /*_Stroika_Foundation_Characters_Character_inl_*/
