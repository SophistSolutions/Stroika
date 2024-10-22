/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <algorithm>
#include <mutex>
#include <set>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

#include "CodePage.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Memory;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

#ifndef qBuildInTableDrivenCodePageBuilderProc
#define qBuildInTableDrivenCodePageBuilderProc 0
#endif

/*
 *  Hack to build 'myiswalpha' and 'myiswpunct' for SPR#1220 (revised and moved here for SPR#1306 and class
 *  'CharacterProperties').
 */
#ifndef qBuildMemoizedISXXXBuilderProc
#define qBuildMemoizedISXXXBuilderProc 0
#endif

/*
 *  Use this to test my IsWXXX functions produce the right results. Only test under WinXP,
 *  since that is the reference they are copying (SPR#1229).
 */
#ifndef qTestMyISWXXXFunctions
#define qTestMyISWXXXFunctions 0
#endif

#if qBuildInTableDrivenCodePageBuilderProc || qBuildMemoizedISXXXBuilderProc
#include <fstream>
#endif

using namespace Characters;

#if qStroika_Foundation_Common_Platform_Windows
namespace {
    inline const wchar_t* SAFE_WIN_WCHART_CAST_ (const char16_t* t)
    {
        return reinterpret_cast<const wchar_t*> (t);
    }
    inline wchar_t* SAFE_WIN_WCHART_CAST_ (char16_t* t)
    {
        return reinterpret_cast<wchar_t*> (t);
    }
    inline const char16_t* SAFE_WIN_WCHART_CAST_ (const wchar_t* t)
    {
        return reinterpret_cast<const char16_t*> (t);
    }
    inline char16_t* SAFE_WIN_WCHART_CAST_ (wchar_t* t)
    {
        return reinterpret_cast<char16_t*> (t);
    }
}
#endif

/*
 ********************************************************************************
 **************************** Characters::GetCharsetString **********************
 ********************************************************************************
 */
wstring Characters::GetCharsetString (CodePage cp)
{
    switch (cp) {
        case WellKnownCodePages::kUTF8:
            return L"utf-8";
        default:
            return Characters::CString::Format (L"CodePage %d", cp);
    }
}

namespace {
    template <CodePage CODEPAGE>
    class TableDrivenCodePageConverter_ {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt);
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
    };
    inline void MapToUNICODEFromTable_ (const char16_t kMap_[256], const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
    {
        size_t nCharsToCopy = min (inMBCharCnt, *outCharCnt);
        for (size_t i = 0; i < nCharsToCopy; ++i) {
            outChars[i] = kMap_[(unsigned char)inMBChars[i]];
        }
        *outCharCnt = nCharsToCopy;
    }
    inline void MapFromUNICODEFromTable_ (const char16_t kMap_[256], const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
    {
        size_t nCharsToCopy = min (inCharCnt, *outCharCnt);
        for (size_t i = 0; i < nCharsToCopy; ++i) {
            size_t j = 0;
            for (; j < 256; ++j) {
                if (kMap_[j] == inChars[i]) {
                    outChars[i] = static_cast<char> (j);
                    break;
                }
            }
            if (j == 256) {
                /*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
                outChars[i] = '?';
            }
        }
        *outCharCnt = nCharsToCopy;
    }
    template <>
    class TableDrivenCodePageConverter_<WellKnownCodePages::kANSI> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
        {
            MapToUNICODEFromTable_ (kMap_, inMBChars, inMBCharCnt, outChars, outCharCnt);
        }
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
        {
            MapFromUNICODEFromTable_ (kMap_, inChars, inCharCnt, outChars, outCharCnt);
        }

    private:
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
    };
    template <>
    class TableDrivenCodePageConverter_<WellKnownCodePages::kMAC> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
        {
            MapToUNICODEFromTable_ (kMap_, inMBChars, inMBCharCnt, outChars, outCharCnt);
        }
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
        {
            MapFromUNICODEFromTable_ (kMap_, inChars, inCharCnt, outChars, outCharCnt);
        }

    private:
        static constexpr char16_t kMap_[256] = {
            0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,    0x9,    0xa,    0xb,    0xc,    0xd,    0xe,    0xf,
            0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,   0x19,   0x1a,   0x1b,   0x1c,   0x1d,   0x1e,   0x1f,
            0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,   0x28,   0x29,   0x2a,   0x2b,   0x2c,   0x2d,   0x2e,   0x2f,
            0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,   0x38,   0x39,   0x3a,   0x3b,   0x3c,   0x3d,   0x3e,   0x3f,
            0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,   0x48,   0x49,   0x4a,   0x4b,   0x4c,   0x4d,   0x4e,   0x4f,
            0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,   0x58,   0x59,   0x5a,   0x5b,   0x5c,   0x5d,   0x5e,   0x5f,
            0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,   0x69,   0x6a,   0x6b,   0x6c,   0x6d,   0x6e,   0x6f,
            0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,   0x78,   0x79,   0x7a,   0x7b,   0x7c,   0x7d,   0x7e,   0x7f,
            0xc4,   0xc5,   0xc7,   0xc9,   0xd1,   0xd6,   0xdc,   0xe1,   0xe0,   0xe2,   0xe4,   0xe3,   0xe5,   0xe7,   0xe9,   0xe8,
            0xea,   0xeb,   0xed,   0xec,   0xee,   0xef,   0xf1,   0xf3,   0xf2,   0xf4,   0xf6,   0xf5,   0xfa,   0xf9,   0xfb,   0xfc,
            0x2020, 0xb0,   0xa2,   0xa3,   0xa7,   0x2022, 0xb6,   0xdf,   0xae,   0xa9,   0x2122, 0xb4,   0xa8,   0x2260, 0xc6,   0xd8,
            0x221e, 0xb1,   0x2264, 0x2265, 0xa5,   0xb5,   0x2202, 0x2211, 0x220f, 0x3c0,  0x222b, 0xaa,   0xba,   0x2126, 0xe6,   0xf8,
            0xbf,   0xa1,   0xac,   0x221a, 0x192,  0x2248, 0x2206, 0xab,   0xbb,   0x2026, 0xa0,   0xc0,   0xc3,   0xd5,   0x152,  0x153,
            0x2013, 0x2014, 0x201c, 0x201d, 0x2018, 0x2019, 0xf7,   0x25ca, 0xff,   0x178,  0x2044, 0x20ac, 0x2039, 0x203a, 0xfb01, 0xfb02,
            0x2021, 0xb7,   0x201a, 0x201e, 0x2030, 0xc2,   0xca,   0xc1,   0xcb,   0xc8,   0xcd,   0xce,   0xcf,   0xcc,   0xd3,   0xd4,
            0xf8ff, 0xd2,   0xda,   0xdb,   0xd9,   0x131,  0x2c6,  0x2dc,  0xaf,   0x2d8,  0x2d9,  0x2da,  0xb8,   0x2dd,  0x2db,  0x2c7,
        };
    };
    template <>
    class TableDrivenCodePageConverter_<WellKnownCodePages::kPC> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
        {
            MapToUNICODEFromTable_ (kMap_, inMBChars, inMBCharCnt, outChars, outCharCnt);
        }
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
        {
            MapFromUNICODEFromTable_ (kMap_, inChars, inCharCnt, outChars, outCharCnt);
        }

    private:
        static constexpr char16_t kMap_[256] = {
            0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,    0x9,    0xa,    0xb,    0xc,    0xd,    0xe,    0xf,
            0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,   0x19,   0x1a,   0x1b,   0x1c,   0x1d,   0x1e,   0x1f,
            0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,   0x28,   0x29,   0x2a,   0x2b,   0x2c,   0x2d,   0x2e,   0x2f,
            0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,   0x38,   0x39,   0x3a,   0x3b,   0x3c,   0x3d,   0x3e,   0x3f,
            0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,   0x48,   0x49,   0x4a,   0x4b,   0x4c,   0x4d,   0x4e,   0x4f,
            0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,   0x58,   0x59,   0x5a,   0x5b,   0x5c,   0x5d,   0x5e,   0x5f,
            0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,   0x69,   0x6a,   0x6b,   0x6c,   0x6d,   0x6e,   0x6f,
            0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,   0x78,   0x79,   0x7a,   0x7b,   0x7c,   0x7d,   0x7e,   0x7f,
            0xc7,   0xfc,   0xe9,   0xe2,   0xe4,   0xe0,   0xe5,   0xe7,   0xea,   0xeb,   0xe8,   0xef,   0xee,   0xec,   0xc4,   0xc5,
            0xc9,   0xe6,   0xc6,   0xf4,   0xf6,   0xf2,   0xfb,   0xf9,   0xff,   0xd6,   0xdc,   0xa2,   0xa3,   0xa5,   0x20a7, 0x192,
            0xe1,   0xed,   0xf3,   0xfa,   0xf1,   0xd1,   0xaa,   0xba,   0xbf,   0x2310, 0xac,   0xbd,   0xbc,   0xa1,   0xab,   0xbb,
            0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
            0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
            0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b, 0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
            0x3b1,  0xdf,   0x393,  0x3c0,  0x3a3,  0x3c3,  0xb5,   0x3c4,  0x3a6,  0x398,  0x3a9,  0x3b4,  0x221e, 0x3c6,  0x3b5,  0x2229,
            0x2261, 0xb1,   0x2265, 0x2264, 0x2320, 0x2321, 0xf7,   0x2248, 0xb0,   0x2219, 0xb7,   0x221a, 0x207f, 0xb2,   0x25a0, 0xa0,
        };
    };
    template <>
    class TableDrivenCodePageConverter_<WellKnownCodePages::kPCA> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
        {
            MapToUNICODEFromTable_ (kMap_, inMBChars, inMBCharCnt, outChars, outCharCnt);
        }
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
        {
            MapFromUNICODEFromTable_ (kMap_, inChars, inCharCnt, outChars, outCharCnt);
        }

    private:
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
    };
    template <>
    class TableDrivenCodePageConverter_<WellKnownCodePages::kGreek> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
        {
            MapToUNICODEFromTable_ (kMap_, inMBChars, inMBCharCnt, outChars, outCharCnt);
        }
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
        {
            MapFromUNICODEFromTable_ (kMap_, inChars, inCharCnt, outChars, outCharCnt);
        }

    private:
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
    };
    template <>
    class TableDrivenCodePageConverter_<WellKnownCodePages::kTurkish> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
        {
            MapToUNICODEFromTable_ (kMap_, inMBChars, inMBCharCnt, outChars, outCharCnt);
        }
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
        {
            MapFromUNICODEFromTable_ (kMap_, inChars, inCharCnt, outChars, outCharCnt);
        }

    private:
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
    };
    template <>
    class TableDrivenCodePageConverter_<WellKnownCodePages::kHebrew> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
        {
            MapToUNICODEFromTable_ (kMap_, inMBChars, inMBCharCnt, outChars, outCharCnt);
        }
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
        {
            MapFromUNICODEFromTable_ (kMap_, inChars, inCharCnt, outChars, outCharCnt);
        }

    private:
        static constexpr char16_t kMap_[256] = {
            0x0,    0x1,    0x2,    0x3,    0x4,    0x5,    0x6,    0x7,    0x8,   0x9,    0xa,    0xb,    0xc,    0xd,    0xe,    0xf,
            0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,   0x18,  0x19,   0x1a,   0x1b,   0x1c,   0x1d,   0x1e,   0x1f,
            0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,   0x28,  0x29,   0x2a,   0x2b,   0x2c,   0x2d,   0x2e,   0x2f,
            0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,   0x38,  0x39,   0x3a,   0x3b,   0x3c,   0x3d,   0x3e,   0x3f,
            0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,   0x48,  0x49,   0x4a,   0x4b,   0x4c,   0x4d,   0x4e,   0x4f,
            0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,   0x58,  0x59,   0x5a,   0x5b,   0x5c,   0x5d,   0x5e,   0x5f,
            0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,   0x68,  0x69,   0x6a,   0x6b,   0x6c,   0x6d,   0x6e,   0x6f,
            0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,   0x78,  0x79,   0x7a,   0x7b,   0x7c,   0x7d,   0x7e,   0x7f,
            0x20ac, 0x81,   0x201a, 0x192,  0x201e, 0x2026, 0x2020, 0x2021, 0x2c6, 0x2030, 0x8a,   0x2039, 0x8c,   0x8d,   0x8e,   0x8f,
            0x90,   0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x2dc, 0x2122, 0x9a,   0x203a, 0x9c,   0x9d,   0x9e,   0x9f,
            0xa0,   0xa1,   0xa2,   0xa3,   0x20aa, 0xa5,   0xa6,   0xa7,   0xa8,  0xa9,   0xd7,   0xab,   0xac,   0xad,   0xae,   0xaf,
            0xb0,   0xb1,   0xb2,   0xb3,   0xb4,   0xb5,   0xb6,   0xb7,   0xb8,  0xb9,   0xf7,   0xbb,   0xbc,   0xbd,   0xbe,   0xbf,
            0x5b0,  0x5b1,  0x5b2,  0x5b3,  0x5b4,  0x5b5,  0x5b6,  0x5b7,  0x5b8, 0x5b9,  0x5ba,  0x5bb,  0x5bc,  0x5bd,  0x5be,  0x5bf,
            0x5c0,  0x5c1,  0x5c2,  0x5c3,  0x5f0,  0x5f1,  0x5f2,  0x5f3,  0x5f4, 0xf88d, 0xf88e, 0xf88f, 0xf890, 0xf891, 0xf892, 0xf893,
            0x5d0,  0x5d1,  0x5d2,  0x5d3,  0x5d4,  0x5d5,  0x5d6,  0x5d7,  0x5d8, 0x5d9,  0x5da,  0x5db,  0x5dc,  0x5dd,  0x5de,  0x5df,
            0x5e0,  0x5e1,  0x5e2,  0x5e3,  0x5e4,  0x5e5,  0x5e6,  0x5e7,  0x5e8, 0x5e9,  0x5ea,  0xf894, 0xf895, 0x200e, 0x200f, 0xf896,
        };
    };
    template <>
    class TableDrivenCodePageConverter_<WellKnownCodePages::kArabic> {
    public:
        static void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt)
        {
            MapToUNICODEFromTable_ (kMap_, inMBChars, inMBCharCnt, outChars, outCharCnt);
        }
        static void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
        {
            MapFromUNICODEFromTable_ (kMap_, inChars, inCharCnt, outChars, outCharCnt);
        }

    private:
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
    };
}

namespace {

#if qBuildInTableDrivenCodePageBuilderProc
    static void WriteCodePageTable (CodePage codePage);

    struct DoRunIt {
        DoRunIt ()
        {
            //WriteCodePageTable (WellKnownCodePages::kANSI);
            //WriteCodePageTable (WellKnownCodePages::kMAC);
            //WriteCodePageTable (WellKnownCodePages::kPC);
            //WriteCodePageTable (WellKnownCodePages::kPCA);
            //WriteCodePageTable (WellKnownCodePages::kGreek);
            //WriteCodePageTable (WellKnownCodePages::kTurkish);
            //WriteCodePageTable (WellKnownCodePages::kHebrew);
            //WriteCodePageTable (WellKnownCodePages::kArabic);
        }
    } gRunIt;
#endif

#if qBuildMemoizedISXXXBuilderProc
    template <typename FUNCTION>
    void WriteMemoizedIsXXXProc (FUNCTION function, const string& origFunctionName, const string& functionName)
    {
        ofstream outStream ("IsXXXProc.txt");

        outStream << "bool      " << functionName << " (wchar_t c)\n";
        outStream << "{\n";

        outStream << "\t// ********** " << functionName << " (AUTOGENERATED memoize of " << origFunctionName << " - " << __DATE__ << ") ***********\n";
        outStream << "\t// Hack for SPR#1220 and SPR#1306\n";

        // SPR#1308 - the generated if/then skipchain can be somewhat long, and therefore inefficient.
        // This is needlessly so. In principle - we could break up the long skipchain into many smaller ones
        // nested hierarchically. This would involve first pre-computing the entire list of regions, and
        // then generating the if/then code recursively since before you can generate INNER code you need
        // to know the full range of codepoints contained in all contained ifs.
        //
        // Anyhow - there is a trivial implementation that gets us most of the speedup we seek - since most
        // characters looked up fall in the 0..256 range. So - just handle that specially.
        //

        const wchar_t kBoundaryForSpecialLoop1 = 255;
        const wchar_t kBoundaryForSpecialLoop2 = 5000;

        for (int j = 0; j <= 2; ++j) {

            if (j == 0) {
                outStream << "\tif (c < " << int (kBoundaryForSpecialLoop1) << ") {\n";
            }
            else if (j == 1) {
                outStream << "\telse if (c < " << int (kBoundaryForSpecialLoop2) << ") {\n";
            }
            else {
                outStream << "\telse {\n";
            }

            outStream << "\t\tif (";
            bool   firstTime         = true;
            bool   hasLastTrue       = false;
            size_t firstRangeIdxTrue = 0;
            size_t startLoop         = 0;
            size_t endLoop           = 0;
            if (j == 0) {
                endLoop = kBoundaryForSpecialLoop1;
            }
            else if (j == 1) {
                startLoop = kBoundaryForSpecialLoop1;
                endLoop   = kBoundaryForSpecialLoop2;
            }
            else {
                startLoop = kBoundaryForSpecialLoop2;
                endLoop   = 256 * 256;
            }
            for (size_t i = startLoop; i < endLoop; ++i) {
                bool isT = function (static_cast<wchar_t> (i));

                if (((not isT) or (i + 1 == endLoop)) and hasLastTrue) {
                    // then emit the range...
                    if (not firstTime) {
                        outStream << "\t\t\t||";
                    }
                    size_t rangeEnd = isT ? i : i - 1;
                    outStream << "(" << firstRangeIdxTrue << " <= c && c <= " << rangeEnd << ")";
                    firstTime = false;
                    outStream << "\n";
                    hasLastTrue = false;
                }
                if (isT and not hasLastTrue) {
                    firstRangeIdxTrue = i;
                    hasLastTrue       = true;
                }
            }
            if (firstTime) {
                outStream << "false";
            }

            outStream << "\t\t\t) {\n";
            outStream << "\t\t\t#if             qTestMyISWXXXFunctions\n";
            outStream << "\t\t\tAssert (" << origFunctionName << "(c));\n";
            outStream << "\t\t\t#endif\n";
            outStream << "\t\t\treturn true;\n";
            outStream << "\t\t}\n";

            outStream << "\t}\n";
        }
        outStream << "\t#if             qTestMyISWXXXFunctions\n";
        outStream << "\tAssert (!" << origFunctionName << "(c));\n";
        outStream << "\t#endif\n";
        outStream << "\treturn false;\n";
        outStream << "}\n";
        outStream << "\n";
    }

    struct DoRunIt {
        DoRunIt ()
        {
            WriteMemoizedIsXXXProc (iswalpha, "iswalpha", "CharacterProperties::IsAlpha_M");
            //WriteMemoizedIsXXXProc (iswalnum, "iswalnum", "CharacterProperties::Ialnum_M");
            //WriteMemoizedIsXXXProc (iswpunct, "iswpunct", "CharacterProperties::IsPunct_M");
            //WriteMemoizedIsXXXProc (iswspace, "iswspace", "CharacterProperties::IsSpace_M");
            //WriteMemoizedIsXXXProc (iswcntrl, "iswcntrl", "CharacterProperties::IsCntrl_M");
            //WriteMemoizedIsXXXProc (iswdigit, "iswdigit", "CharacterProperties::IsDigit_M");
        }
    } gRunIt;
#endif

#if qTestMyISWXXXFunctions
    class MyIsWXXXTesterFunctions {
    public:
        MyIsWXXXTesterFunctions ()
        {
            for (wchar_t c = 0; c < 0xffff; ++c) {
                Assert (CharacterProperties::IsAlpha_M (c) == !!iswalpha (c));
                Assert (CharacterProperties::IsAlnum_M (c) == !!iswalnum (c));
                Assert (CharacterProperties::IsPunct_M (c) == !!iswpunct (c));
                Assert (CharacterProperties::IsSpace_M (c) == !!iswspace (c));
                Assert (CharacterProperties::IsCntrl_M (c) == !!iswcntrl (c));
                Assert (CharacterProperties::IsDigit_M (c) == !!iswdigit (c));
            }
        }
    } sMyIsWXXXTesterFunctions;
#endif
}

/*
 ********************************************************************************
 ******************** CodePageNotSupportedException *****************************
 ********************************************************************************
 */
CodePageNotSupportedException::CodePageNotSupportedException (CodePage codePage)
    : fMsg_{Characters::Format ("Code page {} not supported"_f, codePage).AsNarrowSDKString ()}
    , fCodePage_{codePage}
{
}

const char* CodePageNotSupportedException::what () const noexcept
{
    return fMsg_.c_str ();
}

/*
 ********************************************************************************
 ******************************** CodePageConverter *****************************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (4996);
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
size_t CodePageConverter::MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* inChars, size_t inCharCnt) const
{
    size_t resultSize;
    switch (fCodePage) {
        case WellKnownCodePages::kANSI:
            resultSize = inCharCnt * 1;
            break;
        case WellKnownCodePages::kMAC:
            resultSize = inCharCnt * 1;
            break;
        case WellKnownCodePages::kPC:
            resultSize = inCharCnt * 1;
            break;
        case WellKnownCodePages::kPCA:
            resultSize = inCharCnt * 1;
            break;
        case WellKnownCodePages::kSJIS:
            resultSize = inCharCnt * 2;
            break;
            break; // ITHINK thats right... BOM appears to be 5 chars long? LGP 2001-09-11
        case WellKnownCodePages::kUTF8:
            resultSize = UTFConvert::ComputeTargetBufferSize<char8_t> (span{inChars, inChars + inCharCnt});
        default:
            resultSize = inCharCnt * 8;
            break; // I THINK that should always be enough - but who knows...
    }
    if (GetHandleBOM ()) {
        switch (fCodePage) {
            case WellKnownCodePages::kUNICODE_WIDE:
            case WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN: {
                // BOM (byte order mark)
                resultSize += 2;
            } break;
            case WellKnownCodePages::kUTF8: {
                resultSize += 3; // BOM (byte order mark)
            }
        }
    }
    return resultSize;
}

void CodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt) const
{
    Require (inMBCharCnt == 0 or inMBChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);

    if (GetHandleBOM ()) {
        size_t bytesToStrip = 0;
        if (CodePagesGuesser{}.Guess (inMBChars, inMBCharCnt, nullptr, &bytesToStrip) == fCodePage) {
            Assert (inMBCharCnt >= bytesToStrip);
            inMBChars += bytesToStrip;
            inMBCharCnt -= bytesToStrip;
        }
    }
    switch (fCodePage) {
        case WellKnownCodePages::kANSI:
            TableDrivenCodePageConverter_<WellKnownCodePages::kANSI>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kMAC:
            TableDrivenCodePageConverter_<WellKnownCodePages::kMAC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kPC:
            TableDrivenCodePageConverter_<WellKnownCodePages::kPC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kPCA:
            TableDrivenCodePageConverter_<WellKnownCodePages::kPCA>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kGreek:
            TableDrivenCodePageConverter_<WellKnownCodePages::kGreek>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kTurkish:
            TableDrivenCodePageConverter_<WellKnownCodePages::kTurkish>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kHebrew:
            TableDrivenCodePageConverter_<WellKnownCodePages::kHebrew>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kArabic:
            TableDrivenCodePageConverter_<WellKnownCodePages::kArabic>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kUNICODE_WIDE: {
            const wchar_t* inWChars   = reinterpret_cast<const wchar_t*> (inMBChars);
            size_t         inWCharCnt = (inMBCharCnt / sizeof (wchar_t));
            *outCharCnt               = inWCharCnt;
            (void)::memcpy (outChars, inWChars, inWCharCnt * sizeof (wchar_t));
        } break;
        case WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN: {
            const wchar_t* inWChars   = reinterpret_cast<const wchar_t*> (inMBChars);
            size_t         inWCharCnt = (inMBCharCnt / sizeof (wchar_t));
            *outCharCnt               = inWCharCnt;
            for (size_t i = 0; i < inWCharCnt; ++i) {
                wchar_t c = inWChars[i];
                // byteswap
                c           = ((c & 0xff) << 8) + (c >> 8);
                outChars[i] = c;
            }
        } break;
        case WellKnownCodePages::kUTF8: {
            *outCharCnt = UTFConvert::kThe.Convert (span{inMBChars, inMBChars + inMBCharCnt}, span{outChars, *outCharCnt}).fTargetProduced;
        } break;
        default: {
#if qStroika_Foundation_Common_Platform_Windows
            Characters::Platform::Windows::PlatformCodePageConverter{fCodePage}.MapToUNICODE (inMBChars, inMBCharCnt,
                                                                                              SAFE_WIN_WCHART_CAST_ (outChars), outCharCnt);
#else
            Execution::Throw (CodePageNotSupportedException (fCodePage));
#endif
        } break;
    }

#if qStroika_Foundation_Common_Platform_Windows && 0
    if constexpr (qDebug) {
        // Assure my baked tables (and UTF8 converters) perform the same as the builtin Win32 API
        size_t               tstCharCnt = *outCharCnt;
        StackBuffer<wchar_t> tstBuf{Memory::eUninitialized, *outCharCnt};
        Characters::Platform::Windows::PlatformCodePageConverter{fCodePage}.MapToUNICODE (inMBChars, inMBCharCnt, tstBuf, &tstCharCnt);
        Assert (tstCharCnt == *outCharCnt);
        Assert (memcmp (tstBuf, outChars, sizeof (wchar_t) * tstCharCnt) == 0);
    }
#endif
}

void CodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char32_t* outChars, size_t* outCharCnt) const
{
    // Not really right - but hopefully adquate for starters -- LGP 2011-09-06
    StackBuffer<char16_t> tmpBuf{Memory::eUninitialized, *outCharCnt};
    MapToUNICODE (inMBChars, inMBCharCnt, tmpBuf.data (), outCharCnt);
    for (size_t i = 0; i < *outCharCnt; ++i) {
        outChars[i] = tmpBuf[i];
    }
}

void CodePageConverter::MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
    Require (inCharCnt == 0 or inChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);

    [[maybe_unused]] size_t outBufferSize = *outCharCnt;
#if qDebug && qStroika_Foundation_Common_Platform_Windows
    size_t countOfBOMCharsAdded = 0; // just for the Windows debug check at the end
#endif

    switch (fCodePage) {
        case WellKnownCodePages::kANSI:
            TableDrivenCodePageConverter_<WellKnownCodePages::kANSI>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kMAC:
            TableDrivenCodePageConverter_<WellKnownCodePages::kMAC>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kPC:
            TableDrivenCodePageConverter_<WellKnownCodePages::kPC>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kPCA:
            TableDrivenCodePageConverter_<WellKnownCodePages::kPCA>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
            break;
        case WellKnownCodePages::kUNICODE_WIDE: {
            if (*outCharCnt >= 2) {
                wchar_t* outWBytes    = reinterpret_cast<wchar_t*> (outChars);
                size_t   outByteCount = (inCharCnt * sizeof (wchar_t));
                if (GetHandleBOM ()) {
                    ++outWBytes;       // skip BOM
                    outByteCount -= 2; // subtract for BOM
                }
                *outCharCnt = outByteCount;
                if (GetHandleBOM ()) {
                    outChars[0] = '\xff';
                    outChars[1] = '\xfe';
#if qDebug && qStroika_Foundation_Common_Platform_Windows
                    countOfBOMCharsAdded = 2;
#endif
                }
                (void)::memcpy (outWBytes, inChars, inCharCnt * sizeof (wchar_t));
            }
            else {
                *outCharCnt = 0;
            }
        } break;
        case WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN: {
            if (*outCharCnt >= 2) {
                wchar_t* outWBytes    = reinterpret_cast<wchar_t*> (outChars);
                size_t   outByteCount = (inCharCnt * sizeof (wchar_t));
                if (GetHandleBOM ()) {
                    ++outWBytes;       // skip BOM
                    outByteCount -= 2; // subtract for BOM
                }
                *outCharCnt = outByteCount;
                if (GetHandleBOM ()) {
                    outChars[0] = '\xfe';
                    outChars[1] = '\xff';
#if qDebug && qStroika_Foundation_Common_Platform_Windows
                    countOfBOMCharsAdded = 2;
#endif
                }
                for (size_t i = 0; i < inCharCnt; ++i) {
                    wchar_t c = inChars[i];
                    // byteswap
                    c            = ((c & 0xff) << 8) + (c >> 8);
                    outWBytes[i] = c;
                }
            }
            else {
                *outCharCnt = 0;
            }
        } break;
        case WellKnownCodePages::kUTF8: {
            char*  useOutChars     = outChars;
            size_t useOutCharCount = *outCharCnt;
            if (GetHandleBOM ()) {
                if (*outCharCnt >= 3) {
                    useOutChars += 3; // skip BOM
                    useOutCharCount -= 3;
                    reinterpret_cast<unsigned char*> (outChars)[0] = 0xef;
                    reinterpret_cast<unsigned char*> (outChars)[1] = 0xbb;
                    reinterpret_cast<unsigned char*> (outChars)[2] = 0xbf;
#if qDebug && qStroika_Foundation_Common_Platform_Windows
                    countOfBOMCharsAdded = 3;
#endif
                }
                else {
                    useOutCharCount = 0;
                }
            }
            useOutCharCount =
                UTFConvert::kThe.Convert (span{inChars, inCharCnt}, span{reinterpret_cast<char8_t*> (useOutChars), useOutCharCount}).fTargetProduced;
            if (GetHandleBOM ()) {
                useOutCharCount += 3;
            }
            *outCharCnt = useOutCharCount;
        } break;
        default: {
#if qStroika_Foundation_Common_Platform_Windows
            Characters::Platform::Windows::PlatformCodePageConverter{fCodePage}.MapFromUNICODE (SAFE_WIN_WCHART_CAST_ (inChars), inCharCnt,
                                                                                                outChars, outCharCnt);
#else
            Execution::Throw (CodePageNotSupportedException{fCodePage});
#endif
        }
    }

#if qStroika_Foundation_Common_Platform_Windows
    if constexpr (qDebug) {
        // Assure my baked tables perform the same as the builtin Win32 API
        size_t            win32TstCharCnt = outBufferSize;
        StackBuffer<char> win32TstBuf{Memory::eUninitialized, win32TstCharCnt};

        Characters::Platform::Windows::PlatformCodePageConverter{fCodePage}.MapFromUNICODE (SAFE_WIN_WCHART_CAST_ (inChars), inCharCnt,
                                                                                            win32TstBuf.data (), &win32TstCharCnt);

// SPR#0813 (and SPR#1277) - assert this produces the right result OR a '?' character -
// used for bad conversions. Reason is cuz for characters that don't map - our table and
// the system table can differ in how they map depending on current OS code page.
#if qDebug
        Assert ((win32TstCharCnt + countOfBOMCharsAdded) == *outCharCnt or outChars[0] == '?');
        Assert (memcmp (win32TstBuf.data (), outChars + countOfBOMCharsAdded, win32TstCharCnt) == 0 or outChars[0] == '?');
#endif
    }
#endif
}

void CodePageConverter::MapFromUNICODE (const char32_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
    char*  useOutChars     = outChars;
    size_t useOutCharCount = *outCharCnt;
    bool   addBOM          = GetHandleBOM () and useOutCharCount >= 3;
    if (addBOM) {
        useOutChars += 3; // skip BOM
        useOutCharCount -= 3;
        reinterpret_cast<unsigned char*> (outChars)[0] = 0xef;
        reinterpret_cast<unsigned char*> (outChars)[1] = 0xbb;
        reinterpret_cast<unsigned char*> (outChars)[2] = 0xbf;
    }
    *outCharCnt = UTFConvert::kThe.Convert (span{inChars, inCharCnt}, span{reinterpret_cast<char8_t*> (useOutChars), useOutCharCount}).fTargetProduced;
    if (addBOM) {
        *outCharCnt += 3;
    }
}
DISABLE_COMPILER_MSC_WARNING_END (4996);
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

/*
 ********************************************************************************
 ***************************** CodePagesInstalled *******************************
 ********************************************************************************
 */
namespace {
#if qStroika_Foundation_Common_Platform_Windows
    shared_ptr<set<CodePage>> s_EnumCodePagesProc_Accumulator_;
    BOOL FAR PASCAL           EnumCodePagesProc_ (LPTSTR lpCodePageString)
    {
        s_EnumCodePagesProc_Accumulator_->insert (_ttoi (lpCodePageString));
        return 1;
    }
#endif
}

CodePagesInstalled::CodePagesInstalled ()
{
    Assert (fCodePages_.size () == 0);

    shared_ptr<set<CodePage>> accum = make_shared<set<CodePage>> ();
#if qStroika_Foundation_Common_Platform_Windows
    static mutex sCritSec_;
    {
        [[maybe_unused]] lock_guard critSec{sCritSec_};
        Assert (s_EnumCodePagesProc_Accumulator_.get () == nullptr);
        s_EnumCodePagesProc_Accumulator_ = accum;
        ::EnumSystemCodePages (EnumCodePagesProc_, CP_INSTALLED);
        s_EnumCodePagesProc_Accumulator_.reset ();
    }
#endif
    // Add these 'fake' code pages - which I believe are always available, but never listed by EnumSystemCodePages()
    accum->insert (WellKnownCodePages::kUNICODE_WIDE);
    accum->insert (WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN);
    accum->insert (WellKnownCodePages::kUTF8);
    fCodePages_ = vector<CodePage>{accum->begin (), accum->end ()};
}

/*
 ********************************************************************************
 ********************************** CodePagesGuesser ****************************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (4996);
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
CodePage CodePagesGuesser::Guess (const void* input, size_t nBytes, Confidence* confidence, size_t* bytesFromFrontToStrip)
{
    if (confidence != nullptr) {
        *confidence = Confidence::eLow;
    }
    if (bytesFromFrontToStrip != nullptr) {
        *bytesFromFrontToStrip = 0;
    }
    if (nBytes >= 2) {
        unsigned char c0 = reinterpret_cast<const unsigned char*> (input)[0];
        unsigned char c1 = reinterpret_cast<const unsigned char*> (input)[1];
        if (c0 == 0xff and c1 == 0xfe) {
            if (confidence != nullptr) {
                *confidence = Confidence::eHigh;
            }
            if (bytesFromFrontToStrip != nullptr) {
                *bytesFromFrontToStrip = 2;
            }
            return WellKnownCodePages::kUNICODE_WIDE;
        }
        if (c0 == 0xfe and c1 == 0xff) {
            if (confidence != nullptr) {
                *confidence = Confidence::eHigh;
            }
            if (bytesFromFrontToStrip != nullptr) {
                *bytesFromFrontToStrip = 2;
            }
            return WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN;
        }
        if (nBytes >= 3) {
            unsigned char c2 = reinterpret_cast<const unsigned char*> (input)[2];
            if (c0 == 0xef and c1 == 0xbb and c2 == 0xbf) {
                if (confidence != nullptr) {
                    *confidence = Confidence::eHigh;
                }
                if (bytesFromFrontToStrip != nullptr) {
                    *bytesFromFrontToStrip = 3;
                }
                return WellKnownCodePages::kUTF8;
            }
        }
    }

    /*
     * Final ditch efforts if we don't recognize any prefix.
     */
    if (confidence != nullptr) {
        *confidence = Confidence::eLow;
    }
    return Characters::GetDefaultSDKCodePage ();
}
DISABLE_COMPILER_MSC_WARNING_END (4996);
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

/*
 ********************************************************************************
 ****************************** CodePagePrettyNameMapper ************************
 ********************************************************************************
 */
CodePagePrettyNameMapper::CodePageNames CodePagePrettyNameMapper::sCodePageNames_ = CodePagePrettyNameMapper::MakeDefaultCodePageNames ();

CodePagePrettyNameMapper::CodePageNames CodePagePrettyNameMapper::MakeDefaultCodePageNames ()
{
    CodePageNames codePageNames;
    codePageNames.fUNICODE_WIDE           = L"UNICODE {wide characters}"sv;
    codePageNames.fUNICODE_WIDE_BIGENDIAN = L"UNICODE {wide characters - big endian}"sv;
    codePageNames.fANSI                   = L"ANSI (1252)"sv;
    codePageNames.fMAC                    = L"MAC (2)"sv;
    codePageNames.fPC                     = L"IBM PC United States code page (437)"sv;
    codePageNames.fSJIS                   = L"Japanese SJIS {932}"sv;
    codePageNames.fUTF8                   = L"UNICODE {UTF-8}"sv;
    codePageNames.f850                    = L"Latin I - MS-DOS Multilingual (850)"sv;
    codePageNames.f851                    = L"Latin II - MS-DOS Slavic (850)"sv;
    codePageNames.f866                    = L"Russian - MS-DOS (866)"sv;
    codePageNames.f936                    = L"Chinese {Simplfied} (936)"sv;
    codePageNames.f949                    = L"Korean (949)"sv;
    codePageNames.f950                    = L"Chinese {Traditional} (950)"sv;
    codePageNames.f1250                   = L"Eastern European Windows (1250)"sv;
    codePageNames.f1251                   = L"Cyrilic (1251)"sv;
    codePageNames.f10000                  = L"Roman {Macintosh} (10000)"sv;
    codePageNames.f10001                  = L"Japanese {Macintosh} (10001)"sv;
    codePageNames.f50220                  = L"Japanese JIS (50220)"sv;
    return codePageNames;
}

wstring CodePagePrettyNameMapper::GetName (CodePage cp)
{
    switch (cp) {
        case WellKnownCodePages::kUNICODE_WIDE:
            return sCodePageNames_.fUNICODE_WIDE;
        case WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN:
            return sCodePageNames_.fUNICODE_WIDE_BIGENDIAN;
        case WellKnownCodePages::kANSI:
            return sCodePageNames_.fANSI;
        case WellKnownCodePages::kMAC:
            return sCodePageNames_.fMAC;
        case WellKnownCodePages::kPC:
            return sCodePageNames_.fPC;
        case WellKnownCodePages::kSJIS:
            return sCodePageNames_.fSJIS;
        case WellKnownCodePages::kUTF8:
            return sCodePageNames_.fUTF8;
        case 850:
            return sCodePageNames_.f850;
        case 851:
            return sCodePageNames_.f851;
        case 866:
            return sCodePageNames_.f866;
        case 936:
            return sCodePageNames_.f936;
        case 949:
            return sCodePageNames_.f949;
        case 950:
            return sCodePageNames_.f950;
        case 1250:
            return sCodePageNames_.f1250;
        case WellKnownCodePages::kCyrilic:
            return sCodePageNames_.f1251;
        case 10000:
            return sCodePageNames_.f10000;
        case 10001:
            return sCodePageNames_.f10001;
        case 50220:
            return sCodePageNames_.f50220;
        default: {
            return Characters::CString::Format (L"%d", cp);
        }
    }
}

/*
 ********************************************************************************
 *********************** Characters::WideStringToNarrow *************************
 ********************************************************************************
 */
void Characters::WideStringToNarrow (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult)
{
    RequireNotNull (intoResult);
    Require (wsStart <= wsEnd);
#if qStroika_Foundation_Common_Platform_Windows
    Platform::Windows::WideStringToNarrow (wsStart, wsEnd, codePage, intoResult);
#else
    *intoResult = CodeCvt<wchar_t>{codePage}.String2Bytes<string> (span{wsStart, wsEnd});
#endif
}

/*
 ********************************************************************************
 *********************** Characters::NarrowStringToWide *************************
 ********************************************************************************
 */
#if 0
namespace {
    void PortableNarrowStringToWide_ (const char* sStart, const char* sEnd, CodePage codePage, wstring* intoResult)
    {
        RequireNotNull (intoResult);
        Require (sStart <= sEnd);
        size_t            inSize = sEnd - sStart;
        CodePageConverter cc{codePage};
        // this grossly overestimates size - which is a problem for the RESIZE below!!! COULD pointlessly run out of memroy and intitialize data to good values...
        size_t outSizeBuf = cc.MapToUNICODE_QuickComputeOutBufSize (sStart, inSize);
        intoResult->resize (outSizeBuf);
        size_t actualOutSize = 0;
        if (inSize != 0) {
            actualOutSize = outSizeBuf;
            cc.MapToUNICODE (sStart, inSize, Containers::Start (*intoResult), &actualOutSize);
            if (intoResult->size () != actualOutSize) {
                // shrink
                Assert (intoResult->size () > actualOutSize);
                intoResult->resize (actualOutSize);
            }
        }
    }
}
#endif
void Characters::NarrowStringToWide (const char* sStart, const char* sEnd, CodePage codePage, wstring* intoResult)
{
    RequireNotNull (intoResult);
    Require (sStart <= sEnd);
#if qStroika_Foundation_Common_Platform_Windows
    Platform::Windows::NarrowStringToWide (sStart, sEnd, codePage, intoResult);
#else
    *intoResult =
        CodeCvt<wchar_t>{codePage}.Bytes2String<wstring> (span{reinterpret_cast<const byte*> (sStart), static_cast<size_t> (sEnd - sStart)});
#endif
}

/*
 ********************************************************************************
 *********************** MapUNICODETextWithMaybeBOMTowstring ********************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (4996);
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
wstring Characters::MapUNICODETextWithMaybeBOMTowstring (const char* start, const char* end)
{
    // THIS IMPL DEPRECATED SO GO AWAY SOON
    Require (start <= end);
    if (start == end) {
        return wstring{};
    }
    else {
        size_t               outBufSize = end - start;
        StackBuffer<wchar_t> wideBuf{Memory::eUninitialized, outBufSize};
        size_t               outCharCount = outBufSize;
        MapSBUnicodeTextWithMaybeBOMToUNICODE (start, end - start, wideBuf.data (), &outCharCount);
        Assert (outCharCount <= outBufSize);
        if (outCharCount == 0) {
            return wstring{};
        }

        // The wideBuf may be NUL-terminated or not (depending on whether the input was NUL-terminated or not).
        // Be sure to construct the resuting string with the right end-of-string pointer (the length doesn't include
        // the NUL-char)
        return wstring{wideBuf.data (), wideBuf[outCharCount - 1] == '\0' ? (outCharCount - 1) : outCharCount};
    }
}
DISABLE_COMPILER_MSC_WARNING_END (4996);
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

/*
 ********************************************************************************
 *********************** MapUNICODETextWithMaybeBOMTowstring ********************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (4996);
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
vector<byte> Characters::MapUNICODETextToSerializedFormat (const wchar_t* start, const wchar_t* end, CodePage useCP)
{
    // THIS IMPL DEPRECATED SO GO AWAY SOON
    CodePageConverter cpc{useCP, CodePageConverter::eHandleBOM};
    size_t            outCharCount = cpc.MapFromUNICODE_QuickComputeOutBufSize (start, end - start);
    StackBuffer<char> buf{Memory::eUninitialized, outCharCount};
    cpc.MapFromUNICODE (start, end - start, buf.data (), &outCharCount);
    const byte* bs = reinterpret_cast<const byte*> (static_cast<const char*> (buf));
    return vector<byte>{bs, bs + outCharCount};
}
DISABLE_COMPILER_MSC_WARNING_END (4996);
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
