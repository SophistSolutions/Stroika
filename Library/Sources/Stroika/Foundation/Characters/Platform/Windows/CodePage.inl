/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Platform_Windows_CodePage_inl_
#define _Stroika_Foundation_Characters_Platform_Windows_CodePage_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../../Containers/Common.h"
#include "../../../Debug/Assertions.h"

namespace Stroika::Foundation::Characters::Platform::Windows {

    /*
     ********************************************************************************
     ********************************* WideStringToNarrow ***************************
     ********************************************************************************
     */
    inline void WideStringToNarrow (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult)
    {
        RequireNotNull (intoResult);
        Require (wsStart <= wsEnd);
        if (wsStart == wsEnd) {
            intoResult->clear ();
            return;
        }
        Assert ((wsEnd - wsStart) < numeric_limits<int>::max ()); // WideCharToMultiByte API uses int - and this impl assumes fits
        constexpr bool kUseOptimizedApporach_ = true;
        if (kUseOptimizedApporach_) {
            int wsLen            = static_cast<int> (wsEnd - wsStart);
            int guessNewSterlLen = max<int> (wsLen, 2 * 64 - 10); // for ascii, wsLen enuf. For most string, they will fit in 64 chars (apx)
            // in these cases, we solve in one pass/one call to OS/charmapper
            intoResult->resize (guessNewSterlLen); // maybe over-allocates a bit but will pare back
            int nCharsWritten =
                ::WideCharToMultiByte (codePage, 0, wsStart, wsLen, Containers::Start (*intoResult), guessNewSterlLen, nullptr, nullptr);
            if ((nCharsWritten == 0) and (::GetLastError () == ERROR_INSUFFICIENT_BUFFER)) {
                guessNewSterlLen = ::WideCharToMultiByte (codePage, 0, wsStart, static_cast<int> (wsLen), nullptr, 0, nullptr, nullptr);
                intoResult->resize (guessNewSterlLen);
                nCharsWritten =
                    ::WideCharToMultiByte (codePage, 0, wsStart, wsLen, Containers::Start (*intoResult), guessNewSterlLen, nullptr, nullptr);
            }
            Assert (nCharsWritten != 0);
            Verify (nCharsWritten != 0);
            Verify (nCharsWritten > 0);
            Verify (nCharsWritten <= guessNewSterlLen);
            if (nCharsWritten != guessNewSterlLen) {
                intoResult->resize (nCharsWritten); // shrink to fit
            }
        }
        else {
            int wsLen        = static_cast<int> (wsEnd - wsStart);
            int stringLength = ::WideCharToMultiByte (codePage, 0, wsStart, wsLen, nullptr, 0, nullptr, nullptr);
            intoResult->resize (stringLength);
            if (stringLength != 0) {
                Verify (::WideCharToMultiByte (codePage, 0, wsStart, wsLen, Containers::Start (*intoResult), stringLength, nullptr, nullptr) == stringLength);
            }
        }
    }

    /*
     ********************************************************************************
     ********************************* NarrowStringToWide ***************************
     ********************************************************************************
     */
    inline void NarrowStringToWide (const char* sStart, const char* sEnd, CodePage codePage, wstring* intoResult)
    {
        RequireNotNull (intoResult);
        Require (sStart <= sEnd);
        if (sStart == sEnd) {
            intoResult->clear ();
            return;
        }
        Assert ((sEnd - sStart) < numeric_limits<int>::max ()); // MultiByteToWideChar API uses int - and this impl assumes fits
        int sLen = static_cast<int> (sEnd - sStart);
        Assert (sLen > 0);
        const bool kUseOptimizedApporach_ = true;
        if (kUseOptimizedApporach_) {
            int guessNewSterlLen = sLen;
            Assert (guessNewSterlLen >= ::MultiByteToWideChar (codePage, 0, sStart, static_cast<int> (sLen), nullptr, 0));
            intoResult->resize (guessNewSterlLen); // maybe overallocates a little sometimes...
            int nCharsWritten = ::MultiByteToWideChar (codePage, 0, sStart, sLen, Containers::Start (*intoResult), guessNewSterlLen);
            Verify (nCharsWritten != 0);
            Verify (nCharsWritten > 0);
            Verify (nCharsWritten <= guessNewSterlLen);
            if (nCharsWritten != guessNewSterlLen) {
                intoResult->resize (nCharsWritten); // shrink to fit
            }
        }
        else {
            int newStrLen = ::MultiByteToWideChar (codePage, 0, sStart, sLen, nullptr, 0);
            intoResult->resize (newStrLen);
            if (newStrLen != 0) {
                Verify (::MultiByteToWideChar (codePage, 0, sStart, sLen, Containers::Start (*intoResult), newStrLen) == newStrLen);
            }
        }
    }

    /*
     ********************************************************************************
     ********************************* BSTR2wstring *********************************
     ********************************************************************************
     */
    inline wstring BSTR2wstring (BSTR b)
    {
        if (b == nullptr) {
            return wstring{};
        }
        else {
            return wstring{b};
        }
    }

    /*
     ********************************************************************************
     ****************************** Win32CharSetToCodePage **************************
     ********************************************************************************
     */
    inline CodePage Win32CharSetToCodePage (uint8_t lfCharSet)
    {
// See MSFT info article Q165478. No routine to map from these charset values - just this table... Sigh...
// LGP 2001-04-30

// In the file http://msdn.microsoft.com/library/specs/rtfspec_6.htm - there are many more of these
// magic#s documented. But not how to get the numbers back from a font. May want to support more
// of these? But I think not - I think they are now redudundantly specified via better mecahnims,
// like UNICODE or \cpg
// -- LGP 2000/04/29
//
// For SPR#1184 I grabbed a few more numbers. The mapping to codepages can be roughly guestimated by looking
// in the registry around HKEY_LOCAL_MACHINE\SOFTWARE\Classes\MIME\Database\Charset\
        //  -- LGP 2002-11-29
#ifdef ANSI_CHARSET
        Assert (ANSI_CHARSET == 0);
#else
        const unsigned char ANSI_CHARSET        = 0;
#endif
#ifdef DEFAULT_CHARSET
        Assert (DEFAULT_CHARSET == 1);
#else
        const unsigned char DEFAULT_CHARSET     = 1;
#endif
#ifdef MAC_CHARSET
        Assert (MAC_CHARSET == 77);
#else
        const unsigned char MAC_CHARSET         = 77;
#endif
#ifdef SHIFTJIS_CHARSET
        Assert (SHIFTJIS_CHARSET == 128);
#else
        const unsigned char SHIFTJIS_CHARSET    = 128;
#endif
#ifdef HANGEUL_CHARSET
        Assert (HANGEUL_CHARSET == 129);
#else
        const unsigned char HANGEUL_CHARSET     = 129;
#endif
#ifdef JOHAB_CHARSET
        Assert (JOHAB_CHARSET == 130);
#else
        const unsigned char JOHAB_CHARSET       = 130;
#endif
#ifdef GB2312_CHARSET
        Assert (GB2312_CHARSET == 134);
#else
        const unsigned char GB2312_CHARSET      = 134;
#endif
#ifdef CHINESEBIG5_CHARSET
        Assert (CHINESEBIG5_CHARSET == 136);
#else
        const unsigned char CHINESEBIG5_CHARSET = 136;
#endif
#ifdef GREEK_CHARSET
        Assert (GREEK_CHARSET == 161);
#else
        const unsigned char GREEK_CHARSET       = 161;
#endif
#ifdef TURKISH_CHARSET
        Assert (TURKISH_CHARSET == 162);
#else
        const unsigned char TURKISH_CHARSET     = 162;
#endif
#ifdef VIETNAMESE_CHARSET
        Assert (VIETNAMESE_CHARSET == 163);
#else
        const unsigned char VIETNAMESE_CHARSET  = 163;
#endif
#ifdef HEBREW_CHARSET
        Assert (HEBREW_CHARSET == 177);
#else
        const unsigned char HEBREW_CHARSET      = 177;
#endif
#ifdef ARABIC_CHARSET
        Assert (ARABIC_CHARSET == 178);
#else
        const unsigned char ARABIC_CHARSET      = 178;
#endif
#ifdef BALTIC_CHARSET
        Assert (BALTIC_CHARSET == 186);
#else
        const unsigned char BALTIC_CHARSET      = 186;
#endif
#ifdef RUSSIAN_CHARSET
        Assert (RUSSIAN_CHARSET == 204);
#else
        const unsigned char RUSSIAN_CHARSET     = 204;
#endif
#ifdef THAI_CHARSET
        Assert (THAI_CHARSET == 222);
#else
        const unsigned char THAI_CHARSET        = 222;
#endif
#ifdef EASTEUROPE_CHARSET
        Assert (EASTEUROPE_CHARSET == 238);
#else
        const unsigned char EASTEUROPE_CHARSET  = 238;
#endif
#ifdef OEM_CHARSET
        Assert (OEM_CHARSET == 255);
#else
        const unsigned char OEM_CHARSET         = 255;
#endif

        switch (lfCharSet) {
            case ANSI_CHARSET:
                return WellKnownCodePages::kANSI; // right? Maybe SB? WellKnownCodePages::kANSI (1252)???
            case MAC_CHARSET:
                return WellKnownCodePages::kMAC;
            case SHIFTJIS_CHARSET:
                return WellKnownCodePages::kSJIS; //  Japanese (SJIS)
            case HANGEUL_CHARSET:
                return WellKnownCodePages::kKorean; //  Hangul
            case GB2312_CHARSET:
                return WellKnownCodePages::kGB2312; //  Chinese
            case CHINESEBIG5_CHARSET:
                return WellKnownCodePages::kBIG5; //  Chinese
            case GREEK_CHARSET:
                return WellKnownCodePages::kGreek; //  Greek
            case TURKISH_CHARSET:
                return WellKnownCodePages::kTurkish; //  Turkish
            case VIETNAMESE_CHARSET:
                return WellKnownCodePages::kVietnamese; //  Vietnamese
            case HEBREW_CHARSET:
                return WellKnownCodePages::kHebrew; //  Hebrew
            case ARABIC_CHARSET:
                return WellKnownCodePages::kArabic; //  Arabic
            case 179:
                return WellKnownCodePages::kArabic; //  Arabic Traditional
            case 180:
                return WellKnownCodePages::kArabic; //  Arabic user
            case 181:
                return WellKnownCodePages::kHebrew; //  Hebrew user
            case BALTIC_CHARSET:
                return WellKnownCodePages::kBaltic; //  Baltic
            case RUSSIAN_CHARSET:
                return WellKnownCodePages::kCyrilic; //  Russian/Cyrilic
            case THAI_CHARSET:
                return WellKnownCodePages::kThai; //  Thai
            case EASTEUROPE_CHARSET:
                return WellKnownCodePages::kEasternEuropean; //  aka 'central european'?
            case 254:
                return WellKnownCodePages::kPC;
                break;
            case OEM_CHARSET:
                return WellKnownCodePages::kPCA;
                break;
            default:
                return GetDefaultSDKCodePage ();
        }
    }

    /*
     ********************************************************************************
     ************************ Win32PrimaryLangIDToCodePage **************************
     ********************************************************************************
     */
    inline CodePage Win32PrimaryLangIDToCodePage (USHORT languageIdenifier)
    {
        /*
         *  I haven't found this stuff documented anyplace. Its hard to imagine that MSFT doesn't provide their own
         *  mapping routines! Anyhow - I got a start on this from some UNISCRIBE sample code, and have since
         *  added on from educated guesswork.       -- LGP 2003-01-30
         */
        switch (PRIMARYLANGID (languageIdenifier)) {
            case LANG_ARABIC:
                return WellKnownCodePages::kArabic;
            case LANG_ENGLISH:
                return WellKnownCodePages::kANSI;
            case LANG_FRENCH:
                return WellKnownCodePages::kANSI;
            case LANG_GERMAN:
                return WellKnownCodePages::kANSI;
            case LANG_GREEK:
                return WellKnownCodePages::kGreek;
            case LANG_HEBREW:
                return WellKnownCodePages::kHebrew;
            case LANG_ICELANDIC:
                return WellKnownCodePages::kANSI; // guess? - 2003-01-30
            case LANG_ITALIAN:
                return WellKnownCodePages::kANSI;
            case LANG_KOREAN:
                return WellKnownCodePages::kKorean;
            case LANG_POLISH:
                return WellKnownCodePages::kEasternEuropean; // a bit of a guess - LGP 2003-01-30
            case LANG_PORTUGUESE:
                return WellKnownCodePages::kANSI;
            case LANG_RUSSIAN:
                return WellKnownCodePages::kCyrilic;
            case LANG_SPANISH:
                return WellKnownCodePages::kANSI;
            case LANG_SWEDISH:
                return WellKnownCodePages::kANSI; // guess? - 2003-01-30
            case LANG_THAI:
                return WellKnownCodePages::kThai;
            case LANG_TURKISH:
                return WellKnownCodePages::kTurkish;
            case LANG_UKRAINIAN:
                return WellKnownCodePages::kCyrilic; // guess? - 2003-01-30
            case LANG_VIETNAMESE:
                return WellKnownCodePages::kVietnamese;
            default:
                return GetDefaultSDKCodePage ();
        }
    }

    /*
     ********************************************************************************
     ************************ PlatformCodePageConverter *****************************
     ********************************************************************************
     */
    inline PlatformCodePageConverter::PlatformCodePageConverter (CodePage codePage)
        : fCodePage_{codePage}
    {
    }

}
#endif /*_Stroika_Foundation_Characters_Platform_Windows_CodePage_inl_*/
