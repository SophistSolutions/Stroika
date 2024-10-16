/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

#include "Stroika/Foundation/Characters/CharacterEncodingException.h"

#include "CodeCvt.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

// #define qGenTableDumper_ 1

#if qGenTableDumper_
#include <fstream>
#endif

/*
 ********************************************************************************
 ************ Private_::ThrowErrorConvertingBytes2Characters_ *******************
 ********************************************************************************
 */
void Characters::Private_::ThrowErrorConvertingBytes2Characters_ (size_t nSrcCharsWhereError)
{
    Execution::Throw (CharacterEncodingException{CharacterEncodingException::eDecoding, nSrcCharsWhereError});
}

/*
 ********************************************************************************
 ************ Private_::ThrowErrorConvertingCharacters2Bytes_ *******************
 ********************************************************************************
 */
void Characters::Private_::ThrowErrorConvertingCharacters2Bytes_ (size_t nSrcCharsWhereError)
{
    Execution::Throw (CharacterEncodingException{CharacterEncodingException::eEncoding, nSrcCharsWhereError});
}

/*
 ********************************************************************************
 ************** Private_::ThrowCodePageNotSupportedException_ *******************
 ********************************************************************************
 */
void Characters::Private_::ThrowCodePageNotSupportedException_ (CodePage cp)
{
    Execution::Throw (CodePageNotSupportedException{cp});
}

/*
 ********************************************************************************
 *************** Private_::ThrowCharsetNotSupportedException_ *******************
 ********************************************************************************
 */
void Characters::Private_::ThrowCharsetNotSupportedException_ (const Charset& charset)
{
    Execution::Throw (Execution::RuntimeErrorException{"Cannot construct CodeCvt with provided charset': "sv + static_cast<String> (charset)});
}

/*
 ********************************************************************************
 **** Private_::ThrowInvalidCharacterProvidedDoesntFitWithProvidedCodeCvt_ ******
 ********************************************************************************
 */
void Characters::Private_::ThrowInvalidCharacterProvidedDoesntFitWithProvidedCodeCvt_ ()
{
    static const auto kException_ =
        Execution::RuntimeErrorException{"Cannot construct CodeCvt with provided std::code_cvt and provided 'invalid character'"sv};
    Execution::Throw (kException_);
}

/*
 ********************************************************************************
 ********************** Private_::AsNarrowSDKString_ ****************************
 ********************************************************************************
 */
string Characters::Private_::AsNarrowSDKString_ (const String& s)
{
    return s.AsNarrowSDKString ();
}

/*
 ********************************************************************************
 ************** Private_::BuiltinSingleByteTableCodePageRep_ ********************
 ********************************************************************************
 */
#if qGenTableDumper_
void dumpTable (CodePage cp, std::filesystem::path p)
{
    ofstream o{p, ios::out};
    o << " static constexpr char16_t kMap_[256] = {" << endl;
    for (int i = 0; i < 256; ++i) {
        wchar_t wc{};
        Verify (MultiByteToWideChar (cp, 0, (char*)&i, 1, &wc, 1));
        o << hex << "0x" << static_cast<int> (wc) << ", ";
        if (i % 16 == 15) {
            o << endl;
        }
    }
    o << endl << " };" << endl;
};
#endif

#if qGenTableDumper_
//const int ignored1 = (dumpTable (WellKnownCodePages::kANSI, "kCodePage_ANSI.txt"), 1);
//const int ignored2 = (dumpTable (WellKnownCodePages::kMAC, "kCodePage_MAC.txt"), 1);
//const int ignored3 = (dumpTable (WellKnownCodePages::kPC, "kCodePage_PC.txt"), 1);
//const int ignored4 = (dumpTable (kCodePage_PCA, "kCodePage_PCA.txt"), 1);
//const int ignored5 = (dumpTable (WellKnownCodePages::kGreek, "kCodePage_GREEK.txt"), 1);
//const int ignored6 = (dumpTable (WellKnownCodePages::kTurkish, "kCodePage_Turkish.txt"), 1);
//const int ignored7 = (dumpTable (WellKnownCodePages::kHebrew, "kCodePage_HEBREW.txt"), 1);
//const int ignored8 = (dumpTable (kCodePage_ARABIC, "kCodePage_ARABIC.txt"), 1);
#endif

Characters::Private_::BuiltinSingleByteTableCodePageRep_::BuiltinSingleByteTableCodePageRep_ (CodePage cp, optional<Character> invalidCharacterReplacement)
{
    switch (cp) {
        // Tables generated with qGenTableDumper_ on Windows - 2023-07-23
        case WellKnownCodePages::kANSI: {
            static constexpr char16_t kMap_[256] = {
                0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,   0x9,    0xa,   0xb,    0xc,   0xd,  0xe,   0xf,
                0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,  0x19,   0x1a,  0x1b,   0x1c,  0x1d, 0x1e,  0x1f,
                0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,   0x28,  0x29,   0x2a,  0x2b,   0x2c,  0x2d, 0x2e,  0x2f,
                0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,   0x38,  0x39,   0x3a,  0x3b,   0x3c,  0x3d, 0x3e,  0x3f,
                0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,   0x48,  0x49,   0x4a,  0x4b,   0x4c,  0x4d, 0x4e,  0x4f,
                0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,   0x58,  0x59,   0x5a,  0x5b,   0x5c,  0x5d, 0x5e,  0x5f,
                0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,  0x69,   0x6a,  0x6b,   0x6c,  0x6d, 0x6e,  0x6f,
                0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,   0x78,  0x79,   0x7a,  0x7b,   0x7c,  0x7d, 0x7e,  0x7f,
                0x20ac, 0x81,   0x201a, 0x192,  0x201e, 0x2026, 0x2020, 0x2021, 0x2c6, 0x2030, 0x160, 0x2039, 0x152, 0x8d, 0x17d, 0x8f,
                0x90,   0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x2dc, 0x2122, 0x161, 0x203a, 0x153, 0x9d, 0x17e, 0x178,
                0xa0,   0xa1,   0xa2,   0xa3,   0xa4,   0xa5,   0xa6,   0xa7,   0xa8,  0xa9,   0xaa,  0xab,   0xac,  0xad, 0xae,  0xaf,
                0xb0,   0xb1,   0xb2,   0xb3,   0xb4,   0xb5,   0xb6,   0xb7,   0xb8,  0xb9,   0xba,  0xbb,   0xbc,  0xbd, 0xbe,  0xbf,
                0xc0,   0xc1,   0xc2,   0xc3,   0xc4,   0xc5,   0xc6,   0xc7,   0xc8,  0xc9,   0xca,  0xcb,   0xcc,  0xcd, 0xce,  0xcf,
                0xd0,   0xd1,   0xd2,   0xd3,   0xd4,   0xd5,   0xd6,   0xd7,   0xd8,  0xd9,   0xda,  0xdb,   0xdc,  0xdd, 0xde,  0xdf,
                0xe0,   0xe1,   0xe2,   0xe3,   0xe4,   0xe5,   0xe6,   0xe7,   0xe8,  0xe9,   0xea,  0xeb,   0xec,  0xed, 0xee,  0xef,
                0xf0,   0xf1,   0xf2,   0xf3,   0xf4,   0xf5,   0xf6,   0xf7,   0xf8,  0xf9,   0xfa,  0xfb,   0xfc,  0xfd, 0xfe,  0xff,
            };
            fMap_ = &kMap_[0];
        } break;
        case WellKnownCodePages::kMAC: {
            static constexpr char16_t kMap_[256] = {
                0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,   0x8,    0x9,    0xa,    0xb,    0xc,    0xd,    0xe,
                0xf,    0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,  0x17,   0x18,   0x19,   0x1a,   0x1b,   0x1c,   0x1d,
                0x1e,   0x1f,   0x20,   0x21,   0x22,   0x23,   0x24,   0x25,  0x26,   0x27,   0x28,   0x29,   0x2a,   0x2b,   0x2c,
                0x2d,   0x2e,   0x2f,   0x30,   0x31,   0x32,   0x33,   0x34,  0x35,   0x36,   0x37,   0x38,   0x39,   0x3a,   0x3b,
                0x3c,   0x3d,   0x3e,   0x3f,   0x40,   0x41,   0x42,   0x43,  0x44,   0x45,   0x46,   0x47,   0x48,   0x49,   0x4a,
                0x4b,   0x4c,   0x4d,   0x4e,   0x4f,   0x50,   0x51,   0x52,  0x53,   0x54,   0x55,   0x56,   0x57,   0x58,   0x59,
                0x5a,   0x5b,   0x5c,   0x5d,   0x5e,   0x5f,   0x60,   0x61,  0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,
                0x69,   0x6a,   0x6b,   0x6c,   0x6d,   0x6e,   0x6f,   0x70,  0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,
                0x78,   0x79,   0x7a,   0x7b,   0x7c,   0x7d,   0x7e,   0x7f,  0xc4,   0xc5,   0xc7,   0xc9,   0xd1,   0xd6,   0xdc,
                0xe1,   0xe0,   0xe2,   0xe4,   0xe3,   0xe5,   0xe7,   0xe9,  0xe8,   0xea,   0xeb,   0xed,   0xec,   0xee,   0xef,
                0xf1,   0xf3,   0xf2,   0xf4,   0xf6,   0xf5,   0xfa,   0xf9,  0xfb,   0xfc,   0x2020, 0xb0,   0xa2,   0xa3,   0xa7,
                0x2022, 0xb6,   0xdf,   0xae,   0xa9,   0x2122, 0xb4,   0xa8,  0x2260, 0xc6,   0xd8,   0x221e, 0xb1,   0x2264, 0x2265,
                0xa5,   0xb5,   0x2202, 0x2211, 0x220f, 0x3c0,  0x222b, 0xaa,  0xba,   0x2126, 0xe6,   0xf8,   0xbf,   0xa1,   0xac,
                0x221a, 0x192,  0x2248, 0x2206, 0xab,   0xbb,   0x2026, 0xa0,  0xc0,   0xc3,   0xd5,   0x152,  0x153,  0x2013, 0x2014,
                0x201c, 0x201d, 0x2018, 0x2019, 0xf7,   0x25ca, 0xff,   0x178, 0x2044, 0x20ac, 0x2039, 0x203a, 0xfb01, 0xfb02, 0x2021,
                0xb7,   0x201a, 0x201e, 0x2030, 0xc2,   0xca,   0xc1,   0xcb,  0xc8,   0xcd,   0xce,   0xcf,   0xcc,   0xd3,   0xd4,
                0xf8ff, 0xd2,   0xda,   0xdb,   0xd9,   0x131,  0x2c6,  0x2dc, 0xaf,   0x2d8,  0x2d9,  0x2da,  0xb8,   0x2dd,  0x2db,
                0x2c7,
            };
            fMap_ = &kMap_[0];
        } break;
        case WellKnownCodePages::kPC: {
            static constexpr char16_t kMap_[256] = {
                0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,    0x9,    0xa,    0xb,    0xc,    0xd,    0xe,
                0xf,    0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,   0x19,   0x1a,   0x1b,   0x1c,   0x1d,
                0x1e,   0x1f,   0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,   0x28,   0x29,   0x2a,   0x2b,   0x2c,
                0x2d,   0x2e,   0x2f,   0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,   0x38,   0x39,   0x3a,   0x3b,
                0x3c,   0x3d,   0x3e,   0x3f,   0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,   0x48,   0x49,   0x4a,
                0x4b,   0x4c,   0x4d,   0x4e,   0x4f,   0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,   0x58,   0x59,
                0x5a,   0x5b,   0x5c,   0x5d,   0x5e,   0x5f,   0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,
                0x69,   0x6a,   0x6b,   0x6c,   0x6d,   0x6e,   0x6f,   0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,
                0x78,   0x79,   0x7a,   0x7b,   0x7c,   0x7d,   0x7e,   0x7f,   0xc7,   0xfc,   0xe9,   0xe2,   0xe4,   0xe0,   0xe5,
                0xe7,   0xea,   0xeb,   0xe8,   0xef,   0xee,   0xec,   0xc4,   0xc5,   0xc9,   0xe6,   0xc6,   0xf4,   0xf6,   0xf2,
                0xfb,   0xf9,   0xff,   0xd6,   0xdc,   0xa2,   0xa3,   0xa5,   0x20a7, 0x192,  0xe1,   0xed,   0xf3,   0xfa,   0xf1,
                0xd1,   0xaa,   0xba,   0xbf,   0x2310, 0xac,   0xbd,   0xbc,   0xa1,   0xab,   0xbb,   0x2591, 0x2592, 0x2593, 0x2502,
                0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510, 0x2514, 0x2534, 0x252c,
                0x251c, 0x2500, 0x253c, 0x255e, 0x255f, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567, 0x2568, 0x2564,
                0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b, 0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580, 0x3b1,
                0xdf,   0x393,  0x3c0,  0x3a3,  0x3c3,  0xb5,   0x3c4,  0x3a6,  0x398,  0x3a9,  0x3b4,  0x221e, 0x3c6,  0x3b5,  0x2229,
                0x2261, 0xb1,   0x2265, 0x2264, 0x2320, 0x2321, 0xf7,   0x2248, 0xb0,   0x2219, 0xb7,   0x221a, 0x207f, 0xb2,   0x25a0,
                0xa0,
            };
            fMap_ = &kMap_[0];
        } break;
        case WellKnownCodePages::kPCA: {
            static constexpr char16_t kMap_[256] = {
                0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,  0x7,  0x8,    0x9,    0xa,    0xb,    0xc,    0xd,    0xe,    0xf,
                0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16, 0x17, 0x18,   0x19,   0x1a,   0x1b,   0x1c,   0x1d,   0x1e,   0x1f,
                0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26, 0x27, 0x28,   0x29,   0x2a,   0x2b,   0x2c,   0x2d,   0x2e,   0x2f,
                0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36, 0x37, 0x38,   0x39,   0x3a,   0x3b,   0x3c,   0x3d,   0x3e,   0x3f,
                0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46, 0x47, 0x48,   0x49,   0x4a,   0x4b,   0x4c,   0x4d,   0x4e,   0x4f,
                0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56, 0x57, 0x58,   0x59,   0x5a,   0x5b,   0x5c,   0x5d,   0x5e,   0x5f,
                0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66, 0x67, 0x68,   0x69,   0x6a,   0x6b,   0x6c,   0x6d,   0x6e,   0x6f,
                0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76, 0x77, 0x78,   0x79,   0x7a,   0x7b,   0x7c,   0x7d,   0x7e,   0x7f,
                0xc7,   0xfc,   0xe9,   0xe2,   0xe4,   0xe0,   0xe5, 0xe7, 0xea,   0xeb,   0xe8,   0xef,   0xee,   0xec,   0xc4,   0xc5,
                0xc9,   0xe6,   0xc6,   0xf4,   0xf6,   0xf2,   0xfb, 0xf9, 0xff,   0xd6,   0xdc,   0xf8,   0xa3,   0xd8,   0xd7,   0x192,
                0xe1,   0xed,   0xf3,   0xfa,   0xf1,   0xd1,   0xaa, 0xba, 0xbf,   0xae,   0xac,   0xbd,   0xbc,   0xa1,   0xab,   0xbb,
                0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0xc1,   0xc2, 0xc0, 0xa9,   0x2563, 0x2551, 0x2557, 0x255d, 0xa2,   0xa5,   0x2510,
                0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0xe3, 0xc3, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0xa4,
                0xf0,   0xd0,   0xca,   0xcb,   0xc8,   0x131,  0xcd, 0xce, 0xcf,   0x2518, 0x250c, 0x2588, 0x2584, 0xa6,   0xcc,   0x2580,
                0xd3,   0xdf,   0xd4,   0xd2,   0xf5,   0xd5,   0xb5, 0xfe, 0xde,   0xda,   0xdb,   0xd9,   0xfd,   0xdd,   0xaf,   0xb4,
                0xad,   0xb1,   0x2017, 0xbe,   0xb6,   0xa7,   0xf7, 0xb8, 0xb0,   0xa8,   0xb7,   0xb9,   0xb3,   0xb2,   0x25a0, 0xa0,
            };
            fMap_ = &kMap_[0];
        } break;
        case WellKnownCodePages::kGreek: {
            static constexpr char16_t kMap_[256] = {
                0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,   0x9,    0xa,    0xb,    0xc,   0xd,   0xe,   0xf,
                0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,  0x19,   0x1a,   0x1b,   0x1c,  0x1d,  0x1e,  0x1f,
                0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,   0x28,  0x29,   0x2a,   0x2b,   0x2c,  0x2d,  0x2e,  0x2f,
                0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,   0x38,  0x39,   0x3a,   0x3b,   0x3c,  0x3d,  0x3e,  0x3f,
                0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,   0x48,  0x49,   0x4a,   0x4b,   0x4c,  0x4d,  0x4e,  0x4f,
                0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,   0x58,  0x59,   0x5a,   0x5b,   0x5c,  0x5d,  0x5e,  0x5f,
                0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,  0x69,   0x6a,   0x6b,   0x6c,  0x6d,  0x6e,  0x6f,
                0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,   0x78,  0x79,   0x7a,   0x7b,   0x7c,  0x7d,  0x7e,  0x7f,
                0x20ac, 0x81,   0x201a, 0x192,  0x201e, 0x2026, 0x2020, 0x2021, 0x88,  0x2030, 0x8a,   0x2039, 0x8c,  0x8d,  0x8e,  0x8f,
                0x90,   0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x98,  0x2122, 0x9a,   0x203a, 0x9c,  0x9d,  0x9e,  0x9f,
                0xa0,   0x385,  0x386,  0xa3,   0xa4,   0xa5,   0xa6,   0xa7,   0xa8,  0xa9,   0xf8f9, 0xab,   0xac,  0xad,  0xae,  0x2015,
                0xb0,   0xb1,   0xb2,   0xb3,   0x384,  0xb5,   0xb6,   0xb7,   0x388, 0x389,  0x38a,  0xbb,   0x38c, 0xbd,  0x38e, 0x38f,
                0x390,  0x391,  0x392,  0x393,  0x394,  0x395,  0x396,  0x397,  0x398, 0x399,  0x39a,  0x39b,  0x39c, 0x39d, 0x39e, 0x39f,
                0x3a0,  0x3a1,  0xf8fa, 0x3a3,  0x3a4,  0x3a5,  0x3a6,  0x3a7,  0x3a8, 0x3a9,  0x3aa,  0x3ab,  0x3ac, 0x3ad, 0x3ae, 0x3af,
                0x3b0,  0x3b1,  0x3b2,  0x3b3,  0x3b4,  0x3b5,  0x3b6,  0x3b7,  0x3b8, 0x3b9,  0x3ba,  0x3bb,  0x3bc, 0x3bd, 0x3be, 0x3bf,
                0x3c0,  0x3c1,  0x3c2,  0x3c3,  0x3c4,  0x3c5,  0x3c6,  0x3c7,  0x3c8, 0x3c9,  0x3ca,  0x3cb,  0x3cc, 0x3cd, 0x3ce, 0xf8fb,
            };
            fMap_ = &kMap_[0];
        } break;
        case WellKnownCodePages::kTurkish: {
            static constexpr char16_t kMap_[256] = {
                0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,   0x9,    0xa,   0xb,    0xc,   0xd,   0xe,   0xf,
                0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,  0x19,   0x1a,  0x1b,   0x1c,  0x1d,  0x1e,  0x1f,
                0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,   0x28,  0x29,   0x2a,  0x2b,   0x2c,  0x2d,  0x2e,  0x2f,
                0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,   0x38,  0x39,   0x3a,  0x3b,   0x3c,  0x3d,  0x3e,  0x3f,
                0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,   0x48,  0x49,   0x4a,  0x4b,   0x4c,  0x4d,  0x4e,  0x4f,
                0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,   0x58,  0x59,   0x5a,  0x5b,   0x5c,  0x5d,  0x5e,  0x5f,
                0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,  0x69,   0x6a,  0x6b,   0x6c,  0x6d,  0x6e,  0x6f,
                0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,   0x78,  0x79,   0x7a,  0x7b,   0x7c,  0x7d,  0x7e,  0x7f,
                0x20ac, 0x81,   0x201a, 0x192,  0x201e, 0x2026, 0x2020, 0x2021, 0x2c6, 0x2030, 0x160, 0x2039, 0x152, 0x8d,  0x8e,  0x8f,
                0x90,   0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x2dc, 0x2122, 0x161, 0x203a, 0x153, 0x9d,  0x9e,  0x178,
                0xa0,   0xa1,   0xa2,   0xa3,   0xa4,   0xa5,   0xa6,   0xa7,   0xa8,  0xa9,   0xaa,  0xab,   0xac,  0xad,  0xae,  0xaf,
                0xb0,   0xb1,   0xb2,   0xb3,   0xb4,   0xb5,   0xb6,   0xb7,   0xb8,  0xb9,   0xba,  0xbb,   0xbc,  0xbd,  0xbe,  0xbf,
                0xc0,   0xc1,   0xc2,   0xc3,   0xc4,   0xc5,   0xc6,   0xc7,   0xc8,  0xc9,   0xca,  0xcb,   0xcc,  0xcd,  0xce,  0xcf,
                0x11e,  0xd1,   0xd2,   0xd3,   0xd4,   0xd5,   0xd6,   0xd7,   0xd8,  0xd9,   0xda,  0xdb,   0xdc,  0x130, 0x15e, 0xdf,
                0xe0,   0xe1,   0xe2,   0xe3,   0xe4,   0xe5,   0xe6,   0xe7,   0xe8,  0xe9,   0xea,  0xeb,   0xec,  0xed,  0xee,  0xef,
                0x11f,  0xf1,   0xf2,   0xf3,   0xf4,   0xf5,   0xf6,   0xf7,   0xf8,  0xf9,   0xfa,  0xfb,   0xfc,  0x131, 0x15f, 0xff,
            };
            fMap_ = &kMap_[0];
        } break;
        case WellKnownCodePages::kHebrew: {
            static constexpr char16_t kMap_[256] = {
                0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,   0x7,    0x8,    0x9,    0xa,    0xb,    0xc,    0xd,    0xe,
                0xf,    0x10,   0x11,   0x12,   0x13,   0x14,   0x15,  0x16,   0x17,   0x18,   0x19,   0x1a,   0x1b,   0x1c,   0x1d,
                0x1e,   0x1f,   0x20,   0x21,   0x22,   0x23,   0x24,  0x25,   0x26,   0x27,   0x28,   0x29,   0x2a,   0x2b,   0x2c,
                0x2d,   0x2e,   0x2f,   0x30,   0x31,   0x32,   0x33,  0x34,   0x35,   0x36,   0x37,   0x38,   0x39,   0x3a,   0x3b,
                0x3c,   0x3d,   0x3e,   0x3f,   0x40,   0x41,   0x42,  0x43,   0x44,   0x45,   0x46,   0x47,   0x48,   0x49,   0x4a,
                0x4b,   0x4c,   0x4d,   0x4e,   0x4f,   0x50,   0x51,  0x52,   0x53,   0x54,   0x55,   0x56,   0x57,   0x58,   0x59,
                0x5a,   0x5b,   0x5c,   0x5d,   0x5e,   0x5f,   0x60,  0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,
                0x69,   0x6a,   0x6b,   0x6c,   0x6d,   0x6e,   0x6f,  0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,
                0x78,   0x79,   0x7a,   0x7b,   0x7c,   0x7d,   0x7e,  0x7f,   0x20ac, 0x81,   0x201a, 0x192,  0x201e, 0x2026, 0x2020,
                0x2021, 0x2c6,  0x2030, 0x8a,   0x2039, 0x8c,   0x8d,  0x8e,   0x8f,   0x90,   0x2018, 0x2019, 0x201c, 0x201d, 0x2022,
                0x2013, 0x2014, 0x2dc,  0x2122, 0x9a,   0x203a, 0x9c,  0x9d,   0x9e,   0x9f,   0xa0,   0xa1,   0xa2,   0xa3,   0x20aa,
                0xa5,   0xa6,   0xa7,   0xa8,   0xa9,   0xd7,   0xab,  0xac,   0xad,   0xae,   0xaf,   0xb0,   0xb1,   0xb2,   0xb3,
                0xb4,   0xb5,   0xb6,   0xb7,   0xb8,   0xb9,   0xf7,  0xbb,   0xbc,   0xbd,   0xbe,   0xbf,   0x5b0,  0x5b1,  0x5b2,
                0x5b3,  0x5b4,  0x5b5,  0x5b6,  0x5b7,  0x5b8,  0x5b9, 0x5ba,  0x5bb,  0x5bc,  0x5bd,  0x5be,  0x5bf,  0x5c0,  0x5c1,
                0x5c2,  0x5c3,  0x5f0,  0x5f1,  0x5f2,  0x5f3,  0x5f4, 0xf88d, 0xf88e, 0xf88f, 0xf890, 0xf891, 0xf892, 0xf893, 0x5d0,
                0x5d1,  0x5d2,  0x5d3,  0x5d4,  0x5d5,  0x5d6,  0x5d7, 0x5d8,  0x5d9,  0x5da,  0x5db,  0x5dc,  0x5dd,  0x5de,  0x5df,
                0x5e0,  0x5e1,  0x5e2,  0x5e3,  0x5e4,  0x5e5,  0x5e6, 0x5e7,  0x5e8,  0x5e9,  0x5ea,  0xf894, 0xf895, 0x200e, 0x200f,
                0xf896,
            };
            fMap_ = &kMap_[0];
        } break;
        case WellKnownCodePages::kArabic: {
            static constexpr char16_t kMap_[256] = {
                0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,   0x9,    0xa,   0xb,    0xc,   0xd,    0xe,    0xf,
                0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,  0x19,   0x1a,  0x1b,   0x1c,  0x1d,   0x1e,   0x1f,
                0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,   0x28,  0x29,   0x2a,  0x2b,   0x2c,  0x2d,   0x2e,   0x2f,
                0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,   0x38,  0x39,   0x3a,  0x3b,   0x3c,  0x3d,   0x3e,   0x3f,
                0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,   0x48,  0x49,   0x4a,  0x4b,   0x4c,  0x4d,   0x4e,   0x4f,
                0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,   0x58,  0x59,   0x5a,  0x5b,   0x5c,  0x5d,   0x5e,   0x5f,
                0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,  0x69,   0x6a,  0x6b,   0x6c,  0x6d,   0x6e,   0x6f,
                0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,   0x78,  0x79,   0x7a,  0x7b,   0x7c,  0x7d,   0x7e,   0x7f,
                0x20ac, 0x67e,  0x201a, 0x192,  0x201e, 0x2026, 0x2020, 0x2021, 0x2c6, 0x2030, 0x679, 0x2039, 0x152, 0x686,  0x698,  0x688,
                0x6af,  0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x6a9, 0x2122, 0x691, 0x203a, 0x153, 0x200c, 0x200d, 0x6ba,
                0xa0,   0x60c,  0xa2,   0xa3,   0xa4,   0xa5,   0xa6,   0xa7,   0xa8,  0xa9,   0x6be, 0xab,   0xac,  0xad,   0xae,   0xaf,
                0xb0,   0xb1,   0xb2,   0xb3,   0xb4,   0xb5,   0xb6,   0xb7,   0xb8,  0xb9,   0x61b, 0xbb,   0xbc,  0xbd,   0xbe,   0x61f,
                0x6c1,  0x621,  0x622,  0x623,  0x624,  0x625,  0x626,  0x627,  0x628, 0x629,  0x62a, 0x62b,  0x62c, 0x62d,  0x62e,  0x62f,
                0x630,  0x631,  0x632,  0x633,  0x634,  0x635,  0x636,  0xd7,   0x637, 0x638,  0x639, 0x63a,  0x640, 0x641,  0x642,  0x643,
                0xe0,   0x644,  0xe2,   0x645,  0x646,  0x647,  0x648,  0xe7,   0xe8,  0xe9,   0xea,  0xeb,   0x649, 0x64a,  0xee,   0xef,
                0x64b,  0x64c,  0x64d,  0x64e,  0xf4,   0x64f,  0x650,  0xf7,   0x651, 0xf9,   0x652, 0xfb,   0xfc,  0x200e, 0x200f, 0x6d2,
            };
            fMap_ = &kMap_[0];
        } break;
        default: {
            Execution::Throw (CodePageNotSupportedException{cp});
        }
    }
    AssertNotNull (fMap_);
    if (invalidCharacterReplacement) {
        if (auto pi = std::find (fMap_, fMap_ + 256, invalidCharacterReplacement->As<char32_t> ()); pi != fMap_ + 256) {
            fInvalidCharacterReplacementByte_ = static_cast<byte> (pi - fMap_);
        }
    }
}

span<char16_t> Characters::Private_::BuiltinSingleByteTableCodePageRep_::Bytes2Characters (span<const byte>* from, span<char16_t> to) const
{
    RequireNotNull (from);
    Require (from->size () <= to.size ());
    // all bytes DEFINED to map to some unicode character
    char16_t* oi = to.data ();
    for (byte i : *from) {
        *oi++ = fMap_[(int)i];
    }
    *from = span<const byte>{}; // always consume all bytes - no partial characters
    return to.subspan (oi - to.data ());
}

span<byte> Characters::Private_::BuiltinSingleByteTableCodePageRep_::Characters2Bytes (span<const char16_t> from, span<byte> to) const
{
    // very simple, but stageringly inefficient algorithm for this case... LGP 2023-07-22
    // Note easy to fix for most characters with reverse direction lookup table (all bit unicode characters back to their binary rep and rest in smaller lookaside table)
    byte* oi = to.data ();
    for (char16_t i : from) {
        if (auto pi = std::find (fMap_, fMap_ + 256, i); pi != fMap_ + 256) {
            *oi++ = static_cast<byte> (pi - fMap_);
        }
        else {
            if (fInvalidCharacterReplacementByte_) {
                *oi++ = *fInvalidCharacterReplacementByte_;
            }
            else {
                size_t nCharsConsumed = oi - to.data (); // one char at a time on both so same and avoids counting or using explicit iterator
                Execution::Throw (CharacterEncodingException{CharacterEncodingException::eEncoding, nCharsConsumed}); // @todo COULD safe/capture the encoding name as well here easy enuf...
            }
        }
    }
    return to.subspan (oi - to.data ());
}

size_t Characters::Private_::BuiltinSingleByteTableCodePageRep_::ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const
{
    if (const size_t* i = get_if<size_t> (&src)) {
        return *i;
    }
    else {
        return get<span<const byte>> (src).size ();
    }
}

size_t Characters::Private_::BuiltinSingleByteTableCodePageRep_::ComputeTargetByteBufferSize (variant<span<const char16_t>, size_t> src) const
{
    if (const size_t* i = get_if<size_t> (&src)) {
        return *i;
    }
    else {
        return get<span<const char16_t>> (src).size ();
    }
}

#if qPlatform_Windows
/*
 ********************************************************************************
 ***************** Characters::Private_::WindowsNative_ *************************
 ********************************************************************************
 */
span<char16_t> Characters::Private_::WindowsNative_::Bytes2Characters (span<const byte>* from, span<char16_t> to) const
{
    RequireNotNull (from);
    Require (ComputeTargetCharacterBufferSize (*from) <= to.size ());
    static constexpr DWORD kFLAGS_ = MB_ERR_INVALID_CHARS;
    int r = ::MultiByteToWideChar (fCodePage_, kFLAGS_, reinterpret_cast<LPCCH> (from->data ()), static_cast<int> (from->size ()),
                                   reinterpret_cast<LPWSTR> (to.data ()), static_cast<int> (to.size ()));
    if (r == 0) {
        if (from->empty ()) {
            return span<char16_t>{}; // OK - empty from produces empty to
        }
        else {
            Execution::ThrowSystemErrNo ();
        }
    }
    else {
        return to.subspan (0, static_cast<size_t> (r));
    }
}

span<byte> Characters::Private_::WindowsNative_::Characters2Bytes (span<const char16_t> from, span<byte> to) const
{
    Require (ComputeTargetByteBufferSize (from) <= to.size ());
    static constexpr DWORD kFLAGS_ = 0; // WC_ERR_INVALID_CHARS doesn't work (https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte), so must use lpUsedDefaultChar
    BOOL usedDefaultChar{false};
    int  r = ::WideCharToMultiByte (fCodePage_, kFLAGS_, reinterpret_cast<LPCWCH> (from.data ()), static_cast<int> (from.size ()),
                                    reinterpret_cast<LPSTR> (to.data ()), static_cast<int> (to.size ()), nullptr, &usedDefaultChar);
    if (usedDefaultChar) {
        Execution::ThrowSystemErrNo (ERROR_NO_UNICODE_TRANSLATION);
    }
    if (r == 0) {
        if (from.empty ()) {
            return span<byte>{}; // OK - empty from produces empty to
        }
        else {
            Execution::ThrowSystemErrNo ();
        }
    }
    else {
        Assert (r > 0);
        return to.subspan (0, static_cast<size_t> (r));
    }
}

size_t Characters::Private_::WindowsNative_::ComputeTargetCharacterBufferSize (variant<span<const byte>, size_t> src) const
{
    if (const size_t* i = get_if<size_t> (&src)) {
        return *i;
    }
    else {
        auto                   s       = get<span<const byte>> (src);
        static constexpr DWORD kFLAGS_ = MB_ERR_INVALID_CHARS;
        int r = ::MultiByteToWideChar (fCodePage_, kFLAGS_, reinterpret_cast<LPCCH> (s.data ()), static_cast<int> (s.size ()), nullptr, 0);
        Assert (r >= 0);
        if (r == 0) {
            if (s.size () == 0) {
                return 0;
            }
            else {
                Execution::ThrowSystemErrNo ();
            }
        }
        else {
            return static_cast<size_t> (r);
        }
    }
}

size_t Characters::Private_::WindowsNative_::ComputeTargetByteBufferSize (variant<span<const char16_t>, size_t> src) const
{
    if (const size_t* i = get_if<size_t> (&src)) {
        constexpr size_t kMaxBytesPerCharWAG_ = 6; // @todo improve this WAG, or find some reference/basis, but I think for UNICODE (excluding UTF-7) - I think this number is 4
        return *i * kMaxBytesPerCharWAG_;
    }
    else {
        auto s = get<span<const char16_t>> (src);
        static constexpr DWORD kFLAGS_ = 0; // WC_ERR_INVALID_CHARS doesn't work (https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte)
        int r = ::WideCharToMultiByte (fCodePage_, kFLAGS_, reinterpret_cast<LPCWCH> (s.data ()), static_cast<int> (s.size ()), nullptr, 0,
                                       nullptr, nullptr);
        Assert (r >= 0);
        if (r == 0) {
            if (s.size () == 0) {
                return 0;
            }
            else {
                Execution::ThrowSystemErrNo ();
            }
        }
        else {
            Assert (r > 0);
            return static_cast<size_t> (r);
        }
    }
}
#endif
