/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cctype>

#include "../../Foundation/Characters/Character.h"
#include "../../Foundation/Characters/CodeCvt.h"
#include "../../Foundation/Characters/CodePage.h"

#include "Config.h"

#include "TextBreaks.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

// These SHOULD work for UNICODE, MBYTE and SingleByte case...
inline bool IsASCIISpace (Led_tChar c)
{
    return isascii (c) and isspace (c);
}
inline bool IsASCIIAlnum (Led_tChar c)
{
    return isascii (c) and isalnum (c);
}
inline bool IsASCIIAlpha (Led_tChar c)
{
    return isascii (c) and isalpha (c);
}
inline bool IsASCIIDigit (Led_tChar c)
{
    return isascii (c) and isdigit (c);
}

static bool SJIS_IsLeadByte (unsigned char c)
{
    // Based on code from LEC - mtcdef.h
    return ((c >= 0x81 and c <= 0x9f) or (c >= 0xe0 and c <= 0xfc));
}
static bool SJIS_IsBOLChar (const char* mbChar)
{
    AssertNotNull (mbChar);
    unsigned char byte0 = (unsigned char)mbChar[0];
    unsigned char byte1 = (unsigned char)mbChar[1];
    // Based on code from LEC - jwrap.c
    static constexpr unsigned char yBits[8]         = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
    static constexpr unsigned char yBOLTable[4][32] = {
        {0x0, 0x0, 0x0, 0x0, 0x2,  0x52, 0x0, 0xcc, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 0x20,
         0x0, 0x0, 0x0, 0x0, 0x98, 0xff, 0x0, 0x0,  0x0, 0x0, 0x0, 0xc0, 0x0, 0x0, 0x0, 0x0},
        {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xdf, 0x3, 0x3c, 0x1, 0x40, 0x55, 0x55, 0x5,
         0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,  0x0, 0x0,  0x0, 0x0,  0x0,  0x0,  0x0},
        {0x0, 0x0, 0x0, 0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,  0x0,  0x0, 0x0,
         0x0, 0x0, 0x0, 0x80, 0xa2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2a, 0x10, 0x0, 0x0},
        {0x0,  0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x55, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
         0xa8, 0x40, 0x60, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}};
    unsigned char hi;
    unsigned char lo;
    if (SJIS_IsLeadByte (byte0)) {
        if (byte0 < (unsigned char)0x81 || byte0 > (unsigned char)0x83)
            return false;
        else {
            hi = byte0;
            hi -= 0x80;
            lo = byte1;
        }
    }
    else {
        hi = 0;
        lo = byte0;
    }
    bool isBOLChar = (yBOLTable[hi][lo / 8] & yBits[lo & 7]);
    return (isBOLChar);
}
static bool SJIS_IsEOLChar (const char* mbChar)
{
    AssertNotNull (mbChar);
    unsigned char byte0 = (unsigned char)mbChar[0];
    unsigned char byte1 = (unsigned char)mbChar[1];
    // Based on code from LEC - jwrap.c
    static unsigned char yBits[8]         = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
    static unsigned char yEOLTable[2][32] = {{0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x10, 0x0, 0x0, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8,
                                              0x0, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
                                             {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xa0, 0xaa, 0xaa, 0x2,
                                              0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,  0x0,  0x0,  0x0}};
    unsigned char        hi;
    unsigned char        lo;
    if (SJIS_IsLeadByte (byte0)) {
        if (byte0 != 0x81) {
            return false;
        }
        else {
            hi = 1;
            lo = byte1;
        }
    }
    else {
        hi = 0;
        lo = byte0;
    }
    bool isEOLChar = bool (yEOLTable[hi][lo / 8] & yBits[lo & 7]);
    return isEOLChar;
}

// CONSIDER USING CodePageConverter class here!!!
inline unsigned SJIS_To_Kuten_Row (const char mbChar[2])
{
    // See alec@lec.com's 950111 email message/SPEC
    unsigned char c1        = mbChar[0];
    unsigned char c2        = mbChar[1];
    int           adjust    = (c2 < 159 ? 1 : 0);
    int           rowOffset = (c1 < 160 ? 112 : 176);
    int           result    = ((c1 - rowOffset) << 1) - adjust - 32;
    Assert (result >= 0);
    return (result);
}
inline bool IsJapaneseBOLChar (wchar_t c)
{
    char   mbyteChars[2];
    size_t nBytesInThisChar = 2;
    char16_t useC = static_cast<char16_t> (c); // this code was originally written for wchar_t == char16_t, so that explains unfortunate casts for now
    nBytesInThisChar = CodeCvt<char16_t>{Characters::WellKnownCodePages::kSJIS}
                           .Characters2Bytes (span{&useC, 1}, Memory::SpanReInterpretCast<byte, char> (span{mbyteChars}))
                           .size ();
    Assert (nBytesInThisChar >= 0 and nBytesInThisChar <= 2);
    if (nBytesInThisChar == 0) {
        return 0; // if No SJIS code page, not much we can do!
    }
    return SJIS_IsBOLChar (mbyteChars);
}
inline bool IsJapaneseEOLChar (wchar_t c)
{
    char   mbyteChars[2];
    size_t nBytesInThisChar = 2;
    char16_t useC = static_cast<char16_t> (c); // this code was originally written for wchar_t == char16_t, so that explains unfortunate casts for now
    nBytesInThisChar = CodeCvt<char16_t>{Characters::WellKnownCodePages::kSJIS}
                           .Characters2Bytes (span{&useC, 1}, Memory::SpanReInterpretCast<byte, char> (span{mbyteChars}))
                           .size ();
    Assert (nBytesInThisChar >= 0 and nBytesInThisChar <= 2);
    if (nBytesInThisChar == 0) {
        return 0; // if No SJIS code page, not much we can do!
    }
    return SJIS_IsEOLChar (mbyteChars);
}
inline unsigned GetJapaneseKutenRow (wchar_t c)
{
    char   mbyteChars[2];
    size_t nBytesInThisChar = 2;
    char16_t useC = static_cast<char16_t> (c); // this code was originally written for wchar_t == char16_t, so that explains unfortunate casts for now
    nBytesInThisChar = CodeCvt<char16_t>{Characters::WellKnownCodePages::kSJIS}
                           .Characters2Bytes (span{&useC, 1}, Memory::SpanReInterpretCast<byte, char> (span{mbyteChars}))
                           .size ();
    Assert (nBytesInThisChar >= 0 and nBytesInThisChar <= 2);
    if (nBytesInThisChar == 0) {
        return 0; // if No SJIS code page, not much we can do!
    }
    if (SJIS_IsLeadByte (mbyteChars[0])) {
        return SJIS_To_Kuten_Row (mbyteChars);
    }
    else {
        return 0;
    }
}

/*
 ********************************************************************************
 ******************************** TextBreaks_Basic ******************************
 ********************************************************************************
 */
TextBreaks_Basic::TextBreaks_Basic ()
{
#if qDebug
    // NB: since this is called in this CTOR - it doesn't capture (or pay attention to) subclass overrides of CharToCharacterClass
    // That fact is important - since subclasses might change its result in a way to voilate this regression test. Thats fine - if its
    // desired by the subclass. This is just a test to make sure logical changes we make to this code have intended
    // consequences... LGP 2003-11-24
    RegressionTest ();
#endif
}

void TextBreaks_Basic::FindWordBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
                                       size_t* wordStartResult, size_t* wordEndResult, bool* wordReal) const
{
    AssertNotNull (startOfText);
    AssertNotNull (wordStartResult);
    AssertNotNull (wordEndResult);
    AssertNotNull (wordReal);
    Assert (textOffsetToStartLookingForWord <= lengthOfText);

    if (textOffsetToStartLookingForWord == lengthOfText) {
        *wordStartResult = textOffsetToStartLookingForWord;
        *wordEndResult   = textOffsetToStartLookingForWord;
        *wordReal        = false;
        return;
    }
    /*
     *  First figure out the class of characters we are looking at. Then - scan backwards
     *  looking for the same class of characters. Then scan forwards (from our starting point)
     *  looking for the same class of characters. Return a signal if the class of characters is
     *  space or not (treat all other word-classes as the same for this purpose).
     *  Except that there is a special sentinel class which always breaks anything.
     */
    Assert (textOffsetToStartLookingForWord < lengthOfText); // cuz we checked at start - and returned if equal

    CharacterClasses charClass = CharToCharacterClass (startOfText, lengthOfText, &startOfText[textOffsetToStartLookingForWord]);

    if (charClass == eSentinelClass) {
        *wordStartResult = textOffsetToStartLookingForWord;
        *wordEndResult   = textOffsetToStartLookingForWord + 1;
        *wordReal        = true;
        return;
    }

    // Scan back - while character of the same class
    {
        const Led_tChar* cur = &startOfText[textOffsetToStartLookingForWord];
        if (charClass != eOtherCharacterClass and textOffsetToStartLookingForWord != 0) {
            for (const Led_tChar* maybeCur = Led_PreviousChar (startOfText, cur);; maybeCur = Led_PreviousChar (startOfText, maybeCur)) {
                if (*maybeCur == '\n') { //SPR#0354 - don't cross lines
                    break;
                }
                CharacterClasses curCharClass = CharToCharacterClass (startOfText, lengthOfText, maybeCur);
                if (charClass == curCharClass) {
                    cur = maybeCur;
                }
                else {
                    break;
                }

                if (maybeCur == startOfText) {
                    break;
                }
            }
        }
        *wordStartResult = cur - startOfText;
    }
    // Scan forward - while character of the same class
    {
        const Led_tChar* cur = &startOfText[textOffsetToStartLookingForWord];
        for (; cur < &startOfText[lengthOfText]; cur = Led_NextChar (cur)) {
            if (*cur == '\n') { //SPR#0354
                break;
            }
            CharacterClasses curCharClass = CharToCharacterClass (startOfText, lengthOfText, cur);
            /*
             *  On a change of char-class break. Except for the case of the special class OTHER - in which case we break
             *  return a single char as result.
             */
            if (charClass != curCharClass or (cur != &startOfText[textOffsetToStartLookingForWord] and charClass == eOtherCharacterClass)) {
                break;
            }
        }
        *wordEndResult = cur - startOfText;
    }
    *wordReal = not(charClass == eSpaceClass) and (*wordStartResult != *wordEndResult);
}

void TextBreaks_Basic::FindLineBreaks (const Led_tChar* startOfText, size_t lengthOfText, size_t textOffsetToStartLookingForWord,
                                       size_t* wordEndResult, bool* wordReal) const
{
    AssertNotNull (startOfText);
    AssertNotNull (wordEndResult);
    AssertNotNull (wordReal);
    Assert (textOffsetToStartLookingForWord <= lengthOfText); // Cannot look at characters

    if (textOffsetToStartLookingForWord == lengthOfText) {
        *wordEndResult = textOffsetToStartLookingForWord;
        *wordReal      = false;
        return;
    }

    Led_tChar thisChar = startOfText[textOffsetToStartLookingForWord];

    bool             isSpaceChar    = IsASCIISpace (thisChar);
    CharacterClasses startCharClass = CharToCharacterClass (startOfText, lengthOfText, &startOfText[textOffsetToStartLookingForWord]);

    // Scan forward - while character of the same class
    if (startCharClass == eSentinelClass) {
        *wordEndResult = textOffsetToStartLookingForWord + 1;
        Assert (not isspace (thisChar)); // else we need to cleanup the wordReal logic below...
    }
    else {
        Led_tChar prevChar = thisChar; // for Kinsoku rule - need to keep track of previous character...
                                       // But since we skip first char at start of loop, initialize with
                                       // first char!

        CharacterClasses prevCharWordClass = startCharClass;

        const Led_tChar* end = &startOfText[lengthOfText];
        const Led_tChar* cur = Led_NextChar (&startOfText[textOffsetToStartLookingForWord]);
        for (; cur < end; cur = Led_NextChar (cur)) {
            Led_tChar thisLoopCurChar = *cur;

            CharacterClasses charClass = CharToCharacterClass (startOfText, lengthOfText, cur);
            if (charClass == eSentinelClass) {
                break;
            }

            /*
             *  On a change of char-class break (space-ness) - we return a possible row break.
             */
            bool curCharSpaceChar = IsASCIISpace (thisLoopCurChar);
            if (isSpaceChar != curCharSpaceChar) {
                break;
            }
            //  FROM CHARLESVIEW EDITOR - (Basically) ALL I COPIED WAS THE COMMENT!
            //
            // Here is the Kinsoku rule:
            // The following character combinations cannot be broken:
            //   an EOL character followed by any character.
            //   any character followed by an BOL character.
            //   any non-white space english characters.
            if (not isSpaceChar) {
                if ((charClass != eWordClass or prevCharWordClass != eWordClass or IsASCIISpace (thisLoopCurChar)) and
                    not IsJapaneseEOLChar (prevChar) and not IsJapaneseBOLChar (thisLoopCurChar)) {
                    break;
                }
            }
            prevChar          = thisLoopCurChar;
            prevCharWordClass = charClass;
        }
        *wordEndResult = cur - startOfText;
    }
    *wordReal = (not(IsASCIISpace (thisChar))) and (textOffsetToStartLookingForWord != *wordEndResult);
    Assert (*wordEndResult <= lengthOfText); // LGP added 950208 - in response to Alecs email message of same date - not
                                             // sure this assert is right, but might help debugging later...
}

TextBreaks_Basic::CharacterClasses TextBreaks_Basic::CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText,
                                                                           const Led_tChar* charToExamine) const
{
    Led_tChar c = *charToExamine;

    if (c == 0) {
        return eSentinelClass;
    }
    /*
        *   Return appropriate class for all characters we are SURE of. Some deepend on
        *   context (like decimal point). For those - we return eOtherCharacterClass, and
        *   let the calling software check those special cases.
        */
    if (IsASCIISpace (c)) {
        return (eSpaceClass);
    }
    if (IsASCIIAlnum (c)) {
        return (eWordClass);
    }
    {
        unsigned kutenRow = GetJapaneseKutenRow (c);
        switch (kutenRow) {
            case 3:
                return (eRomanjiOrDigitClass);
            case 4:
                return (eHiraganaClass);
            case 5:
                return (eKatakanaClass);
            default: {
                if (kutenRow >= 16 and kutenRow <= 84) {
                    return (eKanjiClass);
                }
            }
        }
    }

    switch (c) {
        case '.': { // PERIOD before digits
            if (charToExamine > startOfText and charToExamine < &startOfText[lengthOfText]) {
                const Led_tChar* nextChar = charToExamine + 1; // cuz we KNOW we are single-byte...

                if (IsASCIIDigit (*nextChar)) {
                    return (eWordClass);
                }
            }
        } break;
    }

    if (not Character (c).IsPunctuation ()) {
        return eWordClass;
    }

    return eOtherCharacterClass;
}

#if qDebug
void TextBreaks_Basic::RegressionTest ()
{
    {
        const Led_tChar* kTest           = LED_TCHAR_OF ("This is a good test");
        size_t           wordStartResult = 0;
        size_t           wordEndResult   = 0;
        bool             wordReal        = 0;

        FindWordBreaks (kTest, Led_tStrlen (kTest), 1, &wordStartResult, &wordEndResult, &wordReal);
        Assert (wordEndResult == 4);
        Assert (wordReal == true);

        FindWordBreaks (kTest, Led_tStrlen (kTest), 4, &wordStartResult, &wordEndResult, &wordReal);
        Assert (wordEndResult == 5);
        Assert (wordReal == false);
    }
}
#endif

/*
 ********************************************************************************
 ***************************** TextBreaks_Basic_WP ******************************
 ********************************************************************************
 */
TextBreaks_Basic_WP::TextBreaks_Basic_WP ()
{
#if qDebug
    // NB: since this is called in this CTOR - it doesn't capture (or pay attention to) subclass overrides of CharToCharacterClass
    // That fact is important - since subclasses might change its result in a way to voilate this regression test. Thats fine - if its
    // desired by the subclass. This is just a test to make sure logical changes we make to this code have intended
    // consequences... LGP 2003-11-24
    RegressionTest ();
#endif
}

TextBreaks_Basic_WP::CharacterClasses TextBreaks_Basic_WP::CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText,
                                                                                 const Led_tChar* charToExamine) const
{
    switch (*charToExamine) {
        case ',': { // COMMA between digits
            if (charToExamine > startOfText and charToExamine < &startOfText[lengthOfText]) {
                const Led_tChar* prevChar = Led_PreviousChar (startOfText, charToExamine);
                const Led_tChar* nextChar = charToExamine + 1; // cuz we KNOW we are single-byte...

                if (IsASCIIDigit (*prevChar) and IsASCIIDigit (*nextChar)) {
                    return (eWordClass);
                }
            }
        } break;

        case 0x2019: // curly apostrophe
        case '\'': {
            // APOSTROPHE between digits or letters
            if (charToExamine > startOfText and charToExamine < &startOfText[lengthOfText]) {
                const Led_tChar* prevChar = Led_PreviousChar (startOfText, charToExamine);
                const Led_tChar* nextChar = charToExamine + 1; // cuz we KNOW we are single-byte...

                // E.g.: Fred's or Lewis', but not Jim'
                if ((IsASCIIAlnum (*prevChar) and *nextChar == 's') or (*prevChar == 's' and IsASCIISpace (*nextChar))) {
                    return (eWordClass);
                }
            }
        } break;
    }

    // Mimic what we did for MacOS (Inside-Mac : Text (Appendix A-6 thru A-15))
    if (*charToExamine == kNonBreakingSpace or *charToExamine == kPoundSign or *charToExamine == kYenSign or *charToExamine == kCentSign) {
        return eWordClass;
    }
    if (*charToExamine == '$' or *charToExamine == '%' or *charToExamine == '-') {
        return (eWordClass);
    }

    return inherited::CharToCharacterClass (startOfText, lengthOfText, charToExamine);
}

#if qDebug
void TextBreaks_Basic_WP::RegressionTest ()
{
    {
        const Led_tChar* kTest           = LED_TCHAR_OF ("This is a good test");
        size_t           wordStartResult = 0;
        size_t           wordEndResult   = 0;
        bool             wordReal        = 0;

        FindWordBreaks (kTest, Led_tStrlen (kTest), 1, &wordStartResult, &wordEndResult, &wordReal);
        Assert (wordEndResult == 4);
        Assert (wordReal == true);

        FindWordBreaks (kTest, Led_tStrlen (kTest), 4, &wordStartResult, &wordEndResult, &wordReal);
        Assert (wordEndResult == 5);
        Assert (wordReal == false);
    }

    {
        const Led_tChar* kTest           = LED_TCHAR_OF ("This is a good test of Simone's bug with the 'word'.");
        size_t           wordStartResult = 0;
        size_t           wordEndResult   = 0;
        bool             wordReal        = 0;

        FindWordBreaks (kTest, Led_tStrlen (kTest), 25, &wordStartResult, &wordEndResult, &wordReal);
        Assert (wordEndResult == 31);
        Assert (wordReal == true);
    }
}
#endif

/*
 ********************************************************************************
 ************************* TextBreaks_Basic_TextEditor **************************
 ********************************************************************************
 */
TextBreaks_Basic_TextEditor::TextBreaks_Basic_TextEditor ()
{
#if qDebug
    // NB: since this is called in this CTOR - it doesn't capture (or pay attention to) subclass overrides of CharToCharacterClass
    // That fact is important - since subclasses might change its result in a way to voilate this regression test. Thats fine - if its
    // desired by the subclass. This is just a test to make sure logical changes we make to this code have intended
    // consequences... LGP 2003-11-24
    RegressionTest ();
#endif
}

TextBreaks_Basic_TextEditor::CharacterClasses TextBreaks_Basic_TextEditor::CharToCharacterClass (const Led_tChar* startOfText, size_t lengthOfText,
                                                                                                 const Led_tChar* charToExamine) const
{
    if (*charToExamine == '$' or *charToExamine == '%') {
        return (eWordClass);
    }
    if (*charToExamine == '_') {
        return (eWordClass); // SPR#1309 - I think this works a little better in text word selection
    }
    return inherited::CharToCharacterClass (startOfText, lengthOfText, charToExamine);
}

#if qDebug
void TextBreaks_Basic_TextEditor::RegressionTest ()
{
    {
        const Led_tChar* kTest           = LED_TCHAR_OF ("This is a good test of Simone's bug with the 'word'.");
        size_t           wordStartResult = 0;
        size_t           wordEndResult   = 0;
        bool             wordReal        = 0;

        FindWordBreaks (kTest, Led_tStrlen (kTest), 25, &wordStartResult, &wordEndResult, &wordReal);
        Assert (wordEndResult == 29);
        Assert (wordReal == true);
    }
}
#endif
