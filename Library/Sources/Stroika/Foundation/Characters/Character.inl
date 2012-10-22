/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Character_inl_
#define _Stroika_Foundation_Characters_Character_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <cwctype>

#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {

            inline  Character::Character ()
                : fCharacterCode_ ('\0')
            {
            }
            inline  Character::Character (char c)
                : fCharacterCode_ (c)
            {
            }
            inline  Character::Character (char16_t c)
                : fCharacterCode_ (c)
            {
            }
            inline  Character::Character (wchar_t wc)
                : fCharacterCode_ (wc)
            {
                static_assert (
                    qNBytesPerWCharT == sizeof (wchar_t),
                    "wchar_t size must agree with qNBytesPerWCharT setting"
                );
            }
            inline  char    Character::GetAsciiCode () const
            {
                // ASSERT IS ASCII
                // not sure this is right range...
                Require ('0' <= fCharacterCode_ and fCharacterCode_ <= 0x8f);
                return static_cast<char> (fCharacterCode_);
            }
            inline  wchar_t Character::GetCharacterCode () const
            {
                return (fCharacterCode_);
            }
            template<>
            inline  wchar_t Character::As () const
            {
                return GetCharacterCode ();
            }
            inline  bool    Character::IsAscii () const
            {
                return fCharacterCode_ <= 0x7f;
            }
            inline  bool    Character::IsWhitespace () const
            {
                return !!iswspace (fCharacterCode_);
            }
            inline  bool    Character::IsDigit () const
            {
                return !!iswdigit (fCharacterCode_);
            }
            inline  bool    Character::IsHexDigit () const
            {
                return !!iswxdigit (fCharacterCode_);
            }
            inline  bool    Character::IsAlphabetic () const
            {
                return !!iswalpha (fCharacterCode_);
            }
            inline  bool    Character::IsUpperCase () const
            {
                return !!iswupper (fCharacterCode_);
            }
            inline  bool    Character::IsLowerCase () const
            {
                return !!iswlower (fCharacterCode_);
            }
            inline  bool    Character::IsAlphaNumeric () const
            {
                return !!iswalnum (fCharacterCode_);
            }
            inline  bool    Character::IsPunctuation () const
            {
                return !!iswpunct (fCharacterCode_);
            }
			inline  bool    Character::IsControl () const
            {
                return !!iswcntrl (fCharacterCode_);
            }
            inline  Character   Character::ToLowerCase () const
            {
                // Cannot find good spec on towlower/towupper, so not sure this cehck is necessary
                if (::iswupper (fCharacterCode_)) {
                    return static_cast<wchar_t> (::towlower (fCharacterCode_));
                }
                else {
                    return fCharacterCode_;
                }
            }
            inline  Character   Character::ToUpperCase () const
            {
                if (::iswlower (fCharacterCode_)) {
                    return static_cast<wchar_t> (::towupper(fCharacterCode_));
                }
                else {
                    return fCharacterCode_;
                }
            }






            inline  bool    operator== (Character lhs, Character rhs)
            {
                // not sure what do do if characer sets differ?
                return (lhs.GetCharacterCode () == rhs.GetCharacterCode ());
            }
            inline  bool    operator!= (Character lhs, Character rhs)
            {
                // not sure what do do if characer sets differ?
                return (lhs.GetCharacterCode () != rhs.GetCharacterCode ());
            }
            inline  bool    operator>= (Character lhs, Character rhs)
            {
                // not sure what do do if characer sets differ?
                return (lhs.GetCharacterCode () >= rhs.GetCharacterCode ());
            }
            inline  bool    operator> (Character lhs, Character rhs)
            {
                // not sure what do do if characer sets differ?
                return (lhs.GetCharacterCode () > rhs.GetCharacterCode ());
            }
            inline  bool    operator<= (Character lhs, Character rhs)
            {
                // not sure what do do if characer sets differ?
                return (lhs.GetCharacterCode () <= rhs.GetCharacterCode ());
            }
            inline  bool    operator< (Character lhs, Character rhs)
            {
                // not sure what do do if characer sets differ?
                return (lhs.GetCharacterCode () < rhs.GetCharacterCode ());
            }

        }
    }
}
#endif  /*_Stroika_Foundation_Characters_Character_inl_*/
