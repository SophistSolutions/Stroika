/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Character_h_
#define _Stroika_Foundation_Characters_Character_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Enumeration.h"



/**
 * TODO:
 *
 *      @todo   REDO THIS SO ALWAYS USES char32_t - NOT wchar_t!!!! But DONT DO until I have a STRING class
 *              implementation based on UTF-8, to minimize the performance costs...
 *
 *      @todo       KEY ISSUES TO DECIDE:
 *          o   Can we use a class with no loss of performance (or must we use typdef wchar_t Character)
 *
 *          o   How do we handle char16_t versus char32_t - Windows uses 16bit, UNIX 32-bit. UNCLEAR how to
 *              handle here. (LEANING TOWARDS FORCING USE OF 16bit char??) - maybe irrlevelnt if our STRING
 *              class internally stores stuff as utf8
 *
 *      @todo   Biggest thing todo is to work out 'surrogates' - and whether or not they are needed
 *              (depending on the size of wchar_t - which right now - we PRESUME is the same as the size
 *              of Character.
 *
 *      @todo   ToLower ('GERMAN ES-ZETT' or 'SHARP S') returns two esses ('ss') - and we return a single chararcter.
 *              We COULD change return value, or simply document that issue here and define ToLower() of STRING todo
 *              the right thing for queer cases like this, and use this API for the most common cases.
 */



/*
@CONFIGVAR:     qNBytesPerWCharT
@DESCRIPTION:   <p>2 or 4</p>
*/
#if     !defined (qNBytesPerWCharT)
#error "qNBytesPerWCharT should normally be defined indirectly by StroikaConfig.h"
#endif
#if     qNBytesPerWCharT != 2 && qNBytesPerWCharT != 4
#error "qNBytesPerWCharT invalid value"
#endif



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /**
             */
            enum    class   CompareOptions : uint8_t {
                eWithCase,
                eCaseInsensitive,

                Stroika_Define_Enum_Bounds(eWithCase, eCaseInsensitive)
            };


            /**
             */
            class   Character   {
            public:
                Character ();
                Character (char c);
                Character (char16_t c);
                Character (char32_t c);   // @todo decide how to handle surrogates
                Character (wchar_t wc);

            public:
                /**
                 *  \req IsASCII()
                 */
                nonvirtual  char    GetAsciiCode () const;

            public:
                nonvirtual  wchar_t GetCharacterCode () const;

            public:
                /*
                 * @todo    NOT SURE WE WANT THIS FOR wchar_t etc - maybe just get rid of this!!! TRICKYYY;
                 *          IF we go with design based on char32_t - then thats all we can ever safely return.
                 *          We need diff API to return up to 2 wchar_t's!!!
                 */
                template    <typename T>
                nonvirtual  T   As () const;

            public:
                nonvirtual  bool    IsASCII () const;

            public:
                _DeprecatedFunction_ (inline  bool    IsAscii () const , "Instead use IsASCII() - to be removed after v2.0a53") {return IsASCII ();};

            public:
                nonvirtual  bool    IsWhitespace () const;

            public:
                nonvirtual  bool    IsDigit () const;

            public:
                nonvirtual  bool    IsHexDigit () const;

            public:
                nonvirtual  bool    IsAlphabetic () const;

            public:
                // Checks if the given character is uppper case. Can be called on any character.
                // Returns false if not alphabetic
                nonvirtual  bool    IsUpperCase () const;

            public:
                // Checks if the given character is lower case. Can be called on any character.
                // Returns false if not alphabetic
                nonvirtual  bool    IsLowerCase () const;

            public:
                nonvirtual  bool    IsAlphaNumeric () const;

            public:
                nonvirtual  bool    IsPunctuation () const;

            public:
                nonvirtual  bool    IsControl () const;

            public:
                /**
                 *      Note that this does NOT modify the character in place but returns the new desired
                 * character.
                 *
                 *      It is not necessary to first check
                 * if the argument character is uppercase or alpabetic. ToLowerCase () just returns the
                 * original character if there is no sensible conversion.
                 */
                nonvirtual  Character   ToLowerCase () const;

            public:
                /**
                 *      Note that this does NOT modify the character in place but returns the new desired
                 * character.
                 *
                 *      It is not necessary to first check
                 * if the argument character is lowercase or alpabetic. ToUpperCase () just returns the
                 * original character if there is no sensible conversion.
                 */
                nonvirtual  Character   ToUpperCase () const;


            public:
                /**
                *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                */
                nonvirtual  int Compare (Character rhs) const;
                nonvirtual  int Compare (Character rhs, CompareOptions co) const;

            public:
                static int      Compare (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd, CompareOptions co);

            public:
                /**
                *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const String& rhs)
                */
                nonvirtual  bool operator< (Character rhs) const;
                nonvirtual  bool operator<= (Character rhs) const;
                nonvirtual  bool operator> (Character rhs) const;
                nonvirtual  bool operator>= (Character rhs) const;
                nonvirtual  bool operator== (Character rhs) const;
                nonvirtual  bool operator!= (Character rhs) const;

            private:
                wchar_t     fCharacterCode_;
            };


            template<>
            wchar_t Character::As () const;


            /// NOT GOOD IDEA/NOT GOOD PRACTICE - BUT AT LEAST MODULARIZE THE BAD PRACTICE
            /// SO I CAN SEARCH FOR IT AND FIX IT WHEN I HAVE A GOOD IDEA HOW.
            //
            // ASSUME sizeof(wchar_t) same as sizeof (Character) everwhere so the cast between
            // them is safe
            inline  const wchar_t*  CVT_CHARACTER_2_wchar_t (const Character* c)
            {
                return reinterpret_cast<const wchar_t*> (c);
            }


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Character.inl"

#endif  /*__Character__*/
