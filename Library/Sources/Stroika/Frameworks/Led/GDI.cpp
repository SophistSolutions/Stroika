/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include <algorithm>
#include <cstdio>
#include <set>

#include "../../Foundation/Characters/CString/Utilities.h"
#include "../../Foundation/Characters/CodePage.h"
#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Execution/Throw.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

#include "GDI.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

#if qPlatform_Windows
// Often included by <Windows.h> automaticly, but sometimes people define NOIME or VC_EXTRALEAN, and then we
// must include this manaully.
#include <windows.h>

#include <imm.h>
#endif

#if qPlatform_Windows
// RTL Imaging flags
#define qUseUniscribeToImage qUniscribeAvailableWithSDK&& qWideCharacters
#define qUseFakeTTGetWPlacementToImage 1
#define qUseGetCharPlacementToImage 1
#endif

#if qUseUniscribeToImage
#include <Usp10.h>
#endif

/*
 *  Short term debugging crap to debug X-Windows font issues.
 */
#ifndef qDebugFontDetails
#define qDebugFontDetails qDebug&& qStroika_FeatureSupported_XWindows
#endif

// Suggestion from Greg Binkerd [gregb@microsoft.com] about SRX021206603127 - LGP 2003-01-02
#if qUniscribeAvailableWithSDK
#define qTryScriptToCPX 0
//#define   qTryScriptToCPX 1

#ifndef qTryToOptimizeLongUNISCRIBEScriptOutCalls
#define qTryToOptimizeLongUNISCRIBEScriptOutCalls 1
#endif

#endif

#if qPlatform_Windows
/*
 *  Used to use CreateCompatibleBitmap, but as of SPR#1271 try using a DIBSection (of a compatile depth) instead).
 *  This has no noticable effect on normal drawing, but greatly speeds HilightRectangle () code for some computers.
 */
#ifndef qUseDIBSectionForOffscreenBitmap
#define qUseDIBSectionForOffscreenBitmap 1
#endif
#endif

#if 0
//Hack to occasionally have some reliable test to display in Arabic
static  const wchar_t   kHiInArabic[]   =   L"\xfe7d\xfe8d\xfe91\xfea3\xfead\xfedb";    // IN DISPLAY ORDER - NOT LOGICAL ORDER
#endif

#if qPlatform_Windows
inline bool operator== (PALETTEENTRY lhs, COLORREF rhs)
{
    return RGB (lhs.peRed, lhs.peGreen, lhs.peBlue) == rhs;
}
#endif

#if !qHaveWindowsDIBDefined
#ifndef BI_BITFIELDS
#define BI_BITFIELDS 3
#endif
#endif

#if qPlatform_Windows
#ifdef _UNICODE
const bool kRunning32BitGDI = true; //  UNICODE only supported on 32GDI (NT or Win2k or Later)
#else
const bool kRunning32BitGDI = ((::GetVersion () & 0x80000000) == 0); // I BELIEVE this is how we can test we are under NT!!!
#endif // Should be a better way to check for 32bit GDI!!!
#endif

#if qPlatform_MacOS
inline QDErr SafeNewGWorld (GWorldPtr* offscreenGWorld, short pixelDepth, const Rect* boundsRect,
                            CTabHandle cTable, GDHandle aGDevice, GWorldFlags flags)
{
    // NewGWorld seems to crash with 7.5.3 when we are low on memory in our app heap.
    // So just treat this as a failure result from NewGWorld, and avoid the crash.
    // LGP 960524
    try {
        Led_CheckSomeLocalHeapRAMAvailable (8 * 1024);
    }
    catch (...) {
        return memFullErr;
    }
    return ::NewGWorld (offscreenGWorld, pixelDepth, boundsRect, cTable, aGDevice, flags);
}
inline GWorldFlags SafeUpdateGWorld (GWorldPtr* offscreenGWorld, short pixelDepth,
                                     const Rect* boundsRect, CTabHandle cTable,
                                     GDHandle aGDevice, GWorldFlags flags)
{
    // UpdateGWorld seems to crash with 7.5.3 when we are low on memory in our app heap.
    // So just treat this as a failure result from UpdateGWorld, and avoid the crash.
    // LGP 960524
    try {
        Led_CheckSomeLocalHeapRAMAvailable (8 * 1024);
    }
    catch (...) {
        return -1; // <0 implies error
    }
    return ::UpdateGWorld (offscreenGWorld, pixelDepth, boundsRect, cTable, aGDevice, flags);
}
#endif

#if qPlatform_Windows
inline void Win32_GetTextExtentExPoint (HDC hdc, const Led_tChar* str, size_t nChars, int maxExtent, LPINT lpnFit, LPINT alpDx, LPSIZE lpSize)
{
    Require (nChars < static_cast<size_t> (numeric_limits<int>::max ()));
#if qWideCharacters
    Verify (::GetTextExtentExPointW (hdc, str, static_cast<int> (nChars), maxExtent, lpnFit, alpDx, lpSize));
#else
    Verify (::GetTextExtentExPointA (hdc, str, static_cast<int> (nChars), maxExtent, lpnFit, alpDx, lpSize));
#endif
}
inline void Win32_GetTextExtentPoint (HDC hdc, const Led_tChar* str, int nChars, LPSIZE lpSize)
{
#if qWideCharacters
    Verify (::GetTextExtentPointW (hdc, str, nChars, lpSize));
#else
    Verify (::GetTextExtentPointA (hdc, str, nChars, lpSize));
#endif
}
inline void Win32_TextOut (HDC hdc, int xStart, int yStart, const Led_tChar* str, int nChars)
{
#if qWideCharacters
    Verify (::TextOutW (hdc, xStart, yStart, str, nChars));
#else
    Verify (::TextOutA (hdc, xStart, yStart, str, nChars));
#endif
}
#endif

#if qPlatform_Windows && qWideCharacters && qUseUniscribeToImage

const size_t kMaxUNISCRIBECharacters = 30000;

/*
 *  Use LoadLibrary/GetProcAddress instead of direct call to avoid having to link with
 *  Usp10.lib. This avoidance allows us to run on systems that don't it installed.
 */
struct UniscribeDLL {
    UniscribeDLL ()
        : fDLL (::LoadLibrary (_T ("Usp10.dll")))
        , fScriptItemize (nullptr)
        , fScriptShape (nullptr)
        , fScriptPlace (nullptr)
        , fScriptStringAnalyse (nullptr)
        , fScriptStringOut (nullptr)
        , fScriptStringFree (nullptr)
        , fScriptStringGetLogicalWidths (nullptr)
        , fScriptString_pcOutChars (nullptr)
        , fScriptString_pSize (nullptr)
        , fScriptStringCPtoX (nullptr)
    {
        if (fDLL != nullptr) {
            fScriptItemize                = (HRESULT (WINAPI*) (const WCHAR*, int, int, const SCRIPT_CONTROL*, const SCRIPT_STATE*, SCRIPT_ITEM*, int*)) (::GetProcAddress (fDLL, "ScriptItemize"));
            fScriptShape                  = (HRESULT (WINAPI*) (HDC, SCRIPT_CACHE*, const WCHAR*, int, int, SCRIPT_ANALYSIS*, WORD*, WORD*, SCRIPT_VISATTR*, int*)) (::GetProcAddress (fDLL, "ScriptShape"));
            fScriptPlace                  = (HRESULT (WINAPI*) (HDC, SCRIPT_CACHE*, const WORD*, int, const SCRIPT_VISATTR*, SCRIPT_ANALYSIS*, int*, GOFFSET*, ABC*)) (::GetProcAddress (fDLL, "ScriptPlace"));
            fScriptStringAnalyse          = (HRESULT (WINAPI*) (HDC, const void*, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS*)) (::GetProcAddress (fDLL, "ScriptStringAnalyse"));
            fScriptStringOut              = (HRESULT (WINAPI*) (SCRIPT_STRING_ANALYSIS, int, int, UINT, const RECT*, int, int, BOOL)) (::GetProcAddress (fDLL, "ScriptStringOut"));
            fScriptStringFree             = (HRESULT (WINAPI*) (SCRIPT_STRING_ANALYSIS*)) (::GetProcAddress (fDLL, "ScriptStringFree"));
            fScriptStringGetLogicalWidths = (HRESULT (WINAPI*) (SCRIPT_STRING_ANALYSIS, int*)) (::GetProcAddress (fDLL, "ScriptStringGetLogicalWidths"));
            fScriptString_pcOutChars      = (const int*(WINAPI*)(SCRIPT_STRING_ANALYSIS)) (::GetProcAddress (fDLL, "ScriptString_pcOutChars"));
            fScriptString_pSize           = (const SIZE*(WINAPI*)(SCRIPT_STRING_ANALYSIS)) (::GetProcAddress (fDLL, "ScriptString_pSize"));
            fScriptStringCPtoX            = (HRESULT (WINAPI*) (SCRIPT_STRING_ANALYSIS, int, BOOL, int*)) (::GetProcAddress (fDLL, "ScriptStringCPtoX"));
        }
    }
    ~UniscribeDLL ()
    {
        if (fDLL != nullptr) {
            Verify (::FreeLibrary (fDLL));
        }
    }

    nonvirtual bool IsAvail () const
    {
        return fDLL != nullptr;
    }

    HRESULT WINAPI ScriptItemize (const WCHAR* pwcInChars, int cInChars, int cMaxItems, const SCRIPT_CONTROL* psControl, const SCRIPT_STATE* psState, SCRIPT_ITEM* pItems, int* pcItems)
    {
        if (fScriptItemize == nullptr) {
            return E_FAIL;
        }
        return (*fScriptItemize) (pwcInChars, cInChars, cMaxItems, psControl, psState, pItems, pcItems);
    }

    HRESULT WINAPI ScriptShape (HDC hdc, SCRIPT_CACHE* psc, const WCHAR* pwcChars, int cChars, int cMaxGlyphs, SCRIPT_ANALYSIS* psa, WORD* pwOutGlyphs, WORD* pwLogClust, SCRIPT_VISATTR* psva, int* pcGlyphs)
    {
        if (fScriptShape == nullptr) {
            return E_FAIL;
        }
        return (*fScriptShape) (hdc, psc, pwcChars, cChars, cMaxGlyphs, psa, pwOutGlyphs, pwLogClust, psva, pcGlyphs);
    }

    HRESULT WINAPI ScriptPlace (HDC hdc, SCRIPT_CACHE* psc, const WORD* pwGlyphs, int cGlyphs, const SCRIPT_VISATTR* psva, SCRIPT_ANALYSIS* psa, int* piAdvance, GOFFSET* pGoffset, ABC* pABC)
    {
        if (fScriptPlace == nullptr) {
            return E_FAIL;
        }
        return (*fScriptPlace) (hdc, psc, pwGlyphs, cGlyphs, psva, psa, piAdvance, pGoffset, pABC);
    }

    HRESULT WINAPI ScriptStringAnalyse (HDC hdc, const void* pString, int cString, int cGlyphs, int iCharset, DWORD dwFlags, int iReqWidth, SCRIPT_CONTROL* psControl, SCRIPT_STATE* psState, const int* piDx, SCRIPT_TABDEF* pTabdef, const BYTE* pbInClass, SCRIPT_STRING_ANALYSIS* pssa)
    {
        if (fScriptStringAnalyse == nullptr) {
            return E_FAIL;
        }
        return (*fScriptStringAnalyse) (hdc, pString, cString, cGlyphs, iCharset, dwFlags, iReqWidth, psControl, psState, piDx, pTabdef, pbInClass, pssa);
    }

    HRESULT WINAPI ScriptStringOut (SCRIPT_STRING_ANALYSIS ssa, int iX, int iY, UINT uOptions, const RECT* prc, int iMinSel, int iMaxSel, BOOL fDisabled)
    {
        if (fScriptStringOut == nullptr) {
            return E_FAIL;
        }
        return (*fScriptStringOut) (ssa, iX, iY, uOptions, prc, iMinSel, iMaxSel, fDisabled);
    }

    HRESULT WINAPI ScriptStringFree (SCRIPT_STRING_ANALYSIS* pssa)
    {
        if (fScriptStringFree == nullptr) {
            return E_FAIL;
        }
        return (*fScriptStringFree) (pssa);
    }

    HRESULT WINAPI ScriptStringGetLogicalWidths (SCRIPT_STRING_ANALYSIS ssa, int* piDx)
    {
        if (fScriptStringGetLogicalWidths == nullptr) {
            return E_FAIL;
        }
        return (*fScriptStringGetLogicalWidths) (ssa, piDx);
    }

    const int* WINAPI ScriptString_pcOutChars (SCRIPT_STRING_ANALYSIS ssa)
    {
        if (fScriptString_pcOutChars == nullptr) {
            return nullptr;
        }
        return (*fScriptString_pcOutChars) (ssa);
    }

    const SIZE* WINAPI ScriptString_pSize (SCRIPT_STRING_ANALYSIS ssa)
    {
        if (fScriptString_pSize == nullptr) {
            return nullptr;
        }
        return (*fScriptString_pSize) (ssa);
    }

    HRESULT WINAPI ScriptStringCPtoX (SCRIPT_STRING_ANALYSIS ssa, int icp, BOOL fTrailing, int* pX)
    {
        if (fScriptStringCPtoX == nullptr) {
            return E_FAIL;
        }
        return (*fScriptStringCPtoX) (ssa, icp, fTrailing, pX);
    }

    HINSTANCE fDLL;
    HRESULT (WINAPI* fScriptItemize)
    (const WCHAR*, int, int, const SCRIPT_CONTROL*, const SCRIPT_STATE*, SCRIPT_ITEM*, int*);
    HRESULT (WINAPI* fScriptShape)
    (HDC, SCRIPT_CACHE*, const WCHAR*, int, int, SCRIPT_ANALYSIS*, WORD*, WORD*, SCRIPT_VISATTR*, int*);
    HRESULT (WINAPI* fScriptPlace)
    (HDC, SCRIPT_CACHE*, const WORD*, int, const SCRIPT_VISATTR*, SCRIPT_ANALYSIS*, int*, GOFFSET*, ABC*);
    HRESULT (WINAPI* fScriptStringAnalyse)
    (HDC, const void*, int, int, int, DWORD, int, SCRIPT_CONTROL*, SCRIPT_STATE*, const int*, SCRIPT_TABDEF*, const BYTE*, SCRIPT_STRING_ANALYSIS*);
    HRESULT (WINAPI* fScriptStringOut)
    (SCRIPT_STRING_ANALYSIS, int, int, UINT, const RECT*, int, int, BOOL);
    HRESULT (WINAPI* fScriptStringFree)
    (SCRIPT_STRING_ANALYSIS*);
    HRESULT (WINAPI* fScriptStringGetLogicalWidths)
    (SCRIPT_STRING_ANALYSIS, int*);
    const int*(WINAPI* fScriptString_pcOutChars) (SCRIPT_STRING_ANALYSIS);
    const SIZE*(WINAPI* fScriptString_pSize) (SCRIPT_STRING_ANALYSIS);
    HRESULT (WINAPI* fScriptStringCPtoX)
    (SCRIPT_STRING_ANALYSIS, int, BOOL, int*);
};
static UniscribeDLL sUniscribeDLL;
#endif

namespace {
    inline bool IS_WIN30_DIB (const Led_DIB* dib)
    {
        // Logic from MSFT DibLook sample in MSVC.Net 2003
        RequireNotNull (dib);
        const BITMAPINFOHEADER& hdr = dib->bmiHeader;
        return Led_ByteSwapFromWindows (hdr.biSize) == sizeof (BITMAPINFOHEADER);
    }
    inline size_t DIBNumColors (const Led_DIB* dib)
    {
        // Logic from MSFT DibLook sample in MSVC.Net 2003
        RequireNotNull (dib);
        const BITMAPINFOHEADER& hdr = dib->bmiHeader;

        /*  If this is a Windows-style DIB, the number of colors in the
         *  color table can be less than the number of bits per pixel
         *  allows for (i.e. lpbi->biClrUsed can be set to some value).
         *  If this is the case, return the appropriate value.
         */
        if (IS_WIN30_DIB (dib)) {
            unsigned long clrUsed = Led_ByteSwapFromWindows (hdr.biClrUsed);
            if (clrUsed != 0) {
                return clrUsed;
            }
        }

        /*  Calculate the number of colors in the color table based on
         *  the number of bits per pixel for the DIB.
         */
        unsigned short bitCount = IS_WIN30_DIB (dib) ? Led_ByteSwapFromWindows (hdr.biBitCount) : Led_ByteSwapFromWindows (((const BITMAPCOREHEADER*)dib)->bcBitCount);

        /* return number of colors based on bits per pixel */
        switch (bitCount) {
            case 1:
                return 2;
            case 4:
                return 16;
            case 8:
                return 256;
            default:
                return 0;
        }
    }
}

#if qPlatform_Windows
namespace {

    inline RGBQUAD mkRGBQuad (COLORREF c)
    {
        RGBQUAD r;
        r.rgbBlue     = GetBValue (c);
        r.rgbGreen    = GetGValue (c);
        r.rgbRed      = GetRValue (c);
        r.rgbReserved = 0;
        return r;
    }
    inline void MaybeAddColorRefToTable_ (RGBQUAD colorTable[256], size_t* iP, COLORREF c)
    {
        Assert (sizeof (RGBQUAD) == sizeof (COLORREF));
        COLORREF* ct = reinterpret_cast<COLORREF*> (colorTable); // use COLORREF instead of RGBQUAD cuz same size but COLOREF has op== defined
        if (find (ct, ct + *iP, c) == ct + *iP and c != RGB (255, 255, 255)) {
            colorTable[*iP] = mkRGBQuad (c);
            (*iP)++;
        }
    }

    void CreateStandardColorTable (RGBQUAD colorTable[256], COLORREF c1 = RGB (0, 0, 0), COLORREF c2 = RGB (0, 0, 0), COLORREF c3 = RGB (0, 0, 0), COLORREF c4 = RGB (0, 0, 0))
    {
        /*
         *  Cannot use ::GetStockObject (DEFAULT_PALETTE) - because - believe it or not - it returns a 20-entry pallete.
         */
        (void)::memset (colorTable, 0, sizeof (RGBQUAD) * 256);

        const BYTE   kColorSpecVals[] = {0, 32, 64, 128, 192, 255};
        const size_t kColorSpecValCnt = sizeof (kColorSpecVals) / sizeof (kColorSpecVals[0]);

        size_t i = 0;
        {
            /*
             *  Fill in the color table with all X x X x X entries where X varies over the
             *  kColorSpecVals. This gives us a pretty good coverage of colors.
             */
            size_t redIdx   = 0;
            size_t greenIdx = 0;
            size_t blueIdx  = 0;
            for (; i < 256; i++) {
                colorTable[i].rgbRed   = kColorSpecVals[redIdx];
                colorTable[i].rgbGreen = kColorSpecVals[greenIdx];
                colorTable[i].rgbBlue  = kColorSpecVals[blueIdx];
                blueIdx++;
                if (blueIdx >= kColorSpecValCnt) {
                    blueIdx = 0;
                    greenIdx++;
                    if (greenIdx >= kColorSpecValCnt) {
                        greenIdx = 0;
                        redIdx++;
                        if (redIdx >= kColorSpecValCnt) {
                            Assert (i == kColorSpecValCnt * kColorSpecValCnt * kColorSpecValCnt - 1);
                            break;
                        }
                    }
                }
            }
        }
        /*
         * Above algorithm wrote out WHITE at the end (RGB (255,255,255)). We will OVERWRITE that item with other stuff,
         * (shades of gray) to save WHITE for the very end.
         */

        i--; // don't count the WHITE color just added. It will be forced to be last.
        size_t nColorsLeft = 255 - i;
        Assert (nColorsLeft == 41);

        // Check each color and see if needs to be added. Do this BEFORE removing white so we don't need to check
        // that specially
        MaybeAddColorRefToTable_ (colorTable, &i, c1);
        MaybeAddColorRefToTable_ (colorTable, &i, c2);
        MaybeAddColorRefToTable_ (colorTable, &i, c3);
        MaybeAddColorRefToTable_ (colorTable, &i, c4);

        nColorsLeft = 255 - i;
        Assert (nColorsLeft > 0);

        size_t aveSpace = 255 / nColorsLeft;

        BYTE startAt = static_cast<BYTE> (aveSpace);
        while (i < 254 and startAt < 255) {
            COLORREF c = RGB (startAt, startAt, startAt);
            Assert (sizeof (RGBQUAD) == sizeof (COLORREF));
            COLORREF* ct = reinterpret_cast<COLORREF*> (colorTable); // use COLORREF instead of RGBQUAD cuz same size but COLOREF has op== defined
            if (find (ct, ct + i, c) == ct + i) {
                colorTable[i] = mkRGBQuad (c);
                i++;
                nColorsLeft--;
            }
            startAt += static_cast<BYTE> (aveSpace);
        }

        Assert (nColorsLeft == 255 - i);

        Assert (nColorsLeft < 5);  // I'm pretty sure this has to get us under 5 - or pretty close...
        Assert (nColorsLeft >= 1); // I'm pretty sure this has to get us under 5 - or pretty close...

        for (; i <= 255; ++i) {
            colorTable[i].rgbRed   = static_cast<BYTE> (i);
            colorTable[i].rgbGreen = static_cast<BYTE> (i);
            colorTable[i].rgbBlue  = static_cast<BYTE> (i);
        }
    }

    HPALETTE CreatePaletteForColorTable (const RGBQUAD colorTable[256])
    {
        /*
         *  Cannot use ::GetStockObject (DEFAULT_PALETTE) - because - believe it or not - it returns a 20-entry pallete.
         */
        Memory::SmallStackBuffer<char> palBuf (sizeof (LOGPALETTE) + sizeof (PALETTEENTRY) * 256);
        LPLOGPALETTE                   lplgPal = reinterpret_cast<LPLOGPALETTE> (static_cast<char*> (palBuf));

        lplgPal->palVersion    = 0x300;
        lplgPal->palNumEntries = 256;

        for (size_t i = 0; i <= 255; ++i) {
            lplgPal->palPalEntry[i].peRed   = colorTable[i].rgbRed;
            lplgPal->palPalEntry[i].peGreen = colorTable[i].rgbGreen;
            lplgPal->palPalEntry[i].peBlue  = colorTable[i].rgbBlue;
            lplgPal->palPalEntry[i].peFlags = 0;
        }
        return ::CreatePalette (lplgPal);
    }

    HPALETTE CreateStandardPalette (COLORREF c1, COLORREF c2, COLORREF c3, COLORREF c4)
    {
        RGBQUAD colorTable[256];
        CreateStandardColorTable (colorTable, c1, c2, c3, c4);
        return CreatePaletteForColorTable (colorTable);
    }

    HBITMAP Create8BitDIBSection (HDC refDC, DWORD dwX, DWORD dwY, const RGBQUAD* colorTable = nullptr, LPBYTE* ppBits = nullptr)
    {
        LPBYTE ignored = nullptr;
        if (ppBits == nullptr) {
            ppBits = &ignored;
        }

        const WORD wBits = 8;

        RGBQUAD colorTableBuf[256];
        if (colorTable == nullptr) {
            CreateStandardColorTable (colorTableBuf);
            colorTable = colorTableBuf;
        }

        // Create the header big enough to contain color table and bitmasks if needed
        size_t                         nInfoSize = sizeof (BITMAPINFOHEADER) + sizeof (RGBQUAD) * (1 << wBits);
        Memory::SmallStackBuffer<char> bmiBuf (nInfoSize);
        LPBITMAPINFO                   pbmi = reinterpret_cast<LPBITMAPINFO> (static_cast<char*> (bmiBuf));
        (void)::memset (pbmi, 0, nInfoSize);
        pbmi->bmiHeader.biSize        = sizeof (BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth       = dwX;
        pbmi->bmiHeader.biHeight      = dwY;
        pbmi->bmiHeader.biPlanes      = 1;
        pbmi->bmiHeader.biBitCount    = wBits;
        pbmi->bmiHeader.biCompression = BI_RGB; // OVERRIDE below for 16 and 32bpp

        for (int i = 0; i < 256; i++) {
            pbmi->bmiColors[i].rgbRed      = colorTable[i].rgbRed;
            pbmi->bmiColors[i].rgbGreen    = colorTable[i].rgbGreen;
            pbmi->bmiColors[i].rgbBlue     = colorTable[i].rgbBlue;
            pbmi->bmiColors[i].rgbReserved = 0;
        }
        pbmi->bmiHeader.biClrUsed = 256;

        return ::CreateDIBSection (refDC, pbmi, DIB_RGB_COLORS, (void**)ppBits, nullptr, 0);
    }

    HBITMAP Create16BitDIBSection (HDC refDC, DWORD dwX, DWORD dwY)
    {
        // Create the header big enough to contain color table and bitmasks if needed
        size_t                         nInfoSize = sizeof (BITMAPINFOHEADER) + 3 * sizeof (DWORD);
        Memory::SmallStackBuffer<char> bmiBuf (nInfoSize);
        LPBITMAPINFO                   pbmi = reinterpret_cast<LPBITMAPINFO> (static_cast<char*> (bmiBuf));
        (void)::memset (pbmi, 0, nInfoSize);
        pbmi->bmiHeader.biSize     = sizeof (BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth    = dwX;
        pbmi->bmiHeader.biHeight   = dwY;
        pbmi->bmiHeader.biPlanes   = 1;
        pbmi->bmiHeader.biBitCount = 16;
        {
            // if it's 16bpp, fill in the masks and OVERRIDE the compression
            // these are the default masks - you could change them if needed
            LPDWORD pMasks                = (LPDWORD) (pbmi->bmiColors);
            pMasks[0]                     = 0x00007c00;
            pMasks[1]                     = 0x000003e0;
            pMasks[2]                     = 0x0000001f;
            pbmi->bmiHeader.biCompression = BI_BITFIELDS;
        }
        LPBYTE pBits = 0;
        return ::CreateDIBSection (refDC, pbmi, DIB_RGB_COLORS, (void**)&pBits, nullptr, 0);
    }

    HBITMAP Create32BitDIBSection (HDC refDC, DWORD dwX, DWORD dwY)
    {
        // Create the header big enough to contain color table and bitmasks if needed
        size_t                         nInfoSize = sizeof (BITMAPINFOHEADER) + 3 * sizeof (DWORD);
        Memory::SmallStackBuffer<char> bmiBuf (nInfoSize);
        LPBITMAPINFO                   pbmi = reinterpret_cast<LPBITMAPINFO> (static_cast<char*> (bmiBuf));
        (void)::memset (pbmi, 0, nInfoSize);
        pbmi->bmiHeader.biSize     = sizeof (BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth    = dwX;
        pbmi->bmiHeader.biHeight   = dwY;
        pbmi->bmiHeader.biPlanes   = 1;
        pbmi->bmiHeader.biBitCount = 32;
        {
            // if it's 32bpp, fill in the masks and OVERRIDE the compression
            // these are the default masks - you could change them if needed
            LPDWORD pMasks                = (LPDWORD) (pbmi->bmiColors);
            pMasks[0]                     = 0x00ff0000;
            pMasks[1]                     = 0x0000ff00;
            pMasks[2]                     = 0x000000ff;
            pbmi->bmiHeader.biCompression = BI_BITFIELDS;
        }
        LPBYTE pBits;
        return ::CreateDIBSection (refDC, pbmi, DIB_RGB_COLORS, (void**)&pBits, nullptr, 0);
    }
}
#endif

#if qPlatform_Windows && qWideCharacters && qUseFakeTTGetWPlacementToImage
static bool Win9x_Workaround_GetCharPlacementFunction (HDC hdc, const wchar_t* srcText, size_t len, wchar_t* glyphImagesOut);
#endif

#if qPlatform_Windows
/*
 ********************************************************************************
 ************************************ Led_Bitmap ********************************
 ********************************************************************************
 */
BOOL Led_Bitmap::CreateCompatibleBitmap (HDC hdc, Led_Distance nWidth, Led_Distance nHeight)
{
    Assert (m_hObject == nullptr);
    m_hObject  = ::CreateCompatibleBitmap (hdc, nWidth, nHeight);
    fImageSize = Led_Size (nHeight, nWidth);
    return (m_hObject != nullptr); // return value backward compat hack...
}

BOOL Led_Bitmap::CreateCompatibleDIBSection (HDC hdc, Led_Distance nWidth, Led_Distance nHeight)
{
    RequireNotNull (hdc);
    Require (m_hObject == nullptr);
    int useDepth = 16; // default to DIBSection depth - seems to work pretty well in most cases

    fImageSize = Led_Size (nHeight, nWidth);

    int deviceDepth = ::GetDeviceCaps (hdc, BITSPIXEL) * ::GetDeviceCaps (hdc, PLANES);
    if (deviceDepth > 16) {
        useDepth = 32;
    }
    else if (deviceDepth <= 8) {
        useDepth = 8;
    }
    switch (useDepth) {
        case 8:
            m_hObject = Create8BitDIBSection (hdc, nWidth, nHeight);
            break;
        case 16:
            m_hObject = Create16BitDIBSection (hdc, nWidth, nHeight);
            break;
        case 32:
            m_hObject = Create32BitDIBSection (hdc, nWidth, nHeight);
            break;
        default:
            Assert (false); //NotReached
    }
    return (m_hObject != nullptr); // return value backward compat hack...
}

void Led_Bitmap::LoadBitmap (HINSTANCE hInstance, LPCTSTR lpBitmapName)
{
    Require (m_hObject == nullptr);
    m_hObject = ::LoadBitmap (hInstance, lpBitmapName);
    Led_ThrowIfNull (m_hObject);
    {
        BITMAP bm;
        (void)::memset (&bm, 0, sizeof (bm));
        Verify (::GetObject (m_hObject, sizeof (BITMAP), (LPVOID)&bm));
        fImageSize = Led_Size (bm.bmHeight, bm.bmWidth);
    }
}
#endif

/*
 ********************************************************************************
 ************************************ Led_TWIPS *********************************
 ********************************************************************************
 */
const Led_TWIPS Led_TWIPS::kPoint   = Led_TWIPS (20); // a printers 'point' (1/72 of an inch)
const Led_TWIPS Led_TWIPS::kInch    = Led_TWIPS (1440);
const Led_TWIPS Led_TWIPS::kOneInch = Led_TWIPS (1440);

/*
 ********************************************************************************
 ************************************ Led_Color *********************************
 ********************************************************************************
 */
/*
 *  Color name values from page 79 of Web Design in a Nutshell, O'Reilly, table 5-2.
 */
const Led_Color Led_Color::kBlack     = Led_Color (0, 0, 0);
const Led_Color Led_Color::kWhite     = Led_Color (Led_Color::kColorValueMax, Led_Color::kColorValueMax, Led_Color::kColorValueMax);
const Led_Color Led_Color::kRed       = Led_Color (Led_Color::kColorValueMax, 0, 0);
const Led_Color Led_Color::kGreen     = Led_Color (0, Led_Color::kColorValueMax / 2, 0);
const Led_Color Led_Color::kBlue      = Led_Color (0, 0, Led_Color::kColorValueMax);
const Led_Color Led_Color::kCyan      = Led_Color (0, Led_Color::kColorValueMax, Led_Color::kColorValueMax);
const Led_Color Led_Color::kMagenta   = Led_Color (Led_Color::kColorValueMax, 0, Led_Color::kColorValueMax);
const Led_Color Led_Color::kYellow    = Led_Color (Led_Color::kColorValueMax, Led_Color::kColorValueMax, 0);
const Led_Color Led_Color::kMaroon    = Led_Color (Led_Color::kColorValueMax / 2, 0, 0);
const Led_Color Led_Color::kOlive     = Led_Color (Led_Color::kColorValueMax / 2, Led_Color::kColorValueMax / 2, 0);
const Led_Color Led_Color::kNavyBlue  = Led_Color (0, 0, Led_Color::kColorValueMax / 2);
const Led_Color Led_Color::kPurple    = Led_Color (Led_Color::kColorValueMax / 2, 0, Led_Color::kColorValueMax / 2);
const Led_Color Led_Color::kTeal      = Led_Color (0, Led_Color::kColorValueMax / 2, Led_Color::kColorValueMax / 2);
const Led_Color Led_Color::kGray      = Led_Color (Led_Color::kColorValueMax / 2, Led_Color::kColorValueMax / 2, Led_Color::kColorValueMax / 2);
const Led_Color Led_Color::kSilver    = Led_Color (Led_Color::kColorValueMax * 3 / 4, Led_Color::kColorValueMax * 3 / 4, Led_Color::kColorValueMax * 3 / 4);
const Led_Color Led_Color::kDarkGreen = Led_Color (0, (Led_Color::kColorValueMax / 256) * 100, 0);
const Led_Color Led_Color::kLimeGreen = Led_Color (0, Led_Color::kColorValueMax, 0);
const Led_Color Led_Color::kFuchsia   = Led_Color::kMagenta; // same according to that table
const Led_Color Led_Color::kAqua      = Led_Color::kCyan;    // same according to that table

/*
 ********************************************************************************
 ************************************** Led_Pen *********************************
 ********************************************************************************
 */
#if qPlatform_MacOS
const Pattern Led_Pen::kWhitePattern = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};
const Pattern Led_Pen::kLightGrayPattern = {
    0x88,
    0x22,
    0x88,
    0x22,
    0x88,
    0x22,
    0x88,
    0x22,
};
const Pattern Led_Pen::kGrayPattern = {
    0xaa,
    0x55,
    0xaa,
    0x55,
    0xaa,
    0x55,
    0xaa,
    0x55,
};
const Pattern Led_Pen::kDarkGrayPattern = {
    0x77,
    0xdd,
    0x77,
    0xdd,
    0x77,
    0xdd,
    0x77,
    0xdd,
};
const Pattern Led_Pen::kBlackPattern = {
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
    0xff,
};
#endif

/*
 ********************************************************************************
 ****************************** Led_FontSpecification ***************************
 ********************************************************************************
 */
#if qStroika_FeatureSupported_XWindows
string Led_FontSpecification::mkOSRep (const string& foundry, const string& family, const string& weight, const string& slant, const string& pointSize)
{
    char hRes[1024];
    (void)::sprintf (hRes, "%d", Led_GDIGlobals::Get ().GetMainScreenLogPixelsH ());
    char vRes[1024];
    (void)::sprintf (vRes, "%d", Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ());
    string result = "-" + foundry + "-" + family + "-" + weight + "-" + slant + "-*-*-*-" + pointSize + "-" + hRes + "-" + vRes + "-*-*-*-*";
    return result;
}

string Led_FontSpecification::GetOSRep () const
{
    string foundry = "*";
    string weight  = fBold ? "bold" : "medium";
    string slant   = fItalics ? "i" : "r";
    char   pointSize[1024];
    (void)::sprintf (pointSize, "%d", GetPointSize () * 10);
    return mkOSRep (foundry, fFontFamily, weight, slant, pointSize);
}

void Led_FontSpecification::SetFromOSRep (const string& osRep)
{
    Led_SDK_String familyName;
    Led_SDK_String fontSize;
    Led_SDK_String fontWeight;
    Led_SDK_String fontSlant;
    Led_Tablet_::ParseFontName (osRep, &familyName, &fontSize, &fontWeight, &fontSlant);
    SetFontName (familyName);
    if (fontSlant == "i") {
        SetStyle_Italic (true);
    }
    else if (fontSlant == "r") {
        SetStyle_Italic (false);
    }
    if (fontWeight == "bold") {
        SetStyle_Bold (true);
    }
    else if (fontWeight == "medium") {
        SetStyle_Bold (false);
    }
    int fspPointSize = GetPointSize ();
    if (::sscanf (fontSize.c_str (), "%d", &fspPointSize) == 1) {
        fspPointSize /= 10;
        if (fspPointSize < 5) {
            fspPointSize = 5;
        }
        if (fspPointSize > 100) {
            fspPointSize = 100;
        }
        SetPointSize (fspPointSize);
    }
}
#endif

/*
@METHOD:        Led_FontSpecification::SetFontName
@DESCRIPTION:   <p>See also @'Led_FontSpecification::GetFontName'.</p>
*/
void Led_FontSpecification::SetFontName (const Led_SDK_String& fontName)
{
#if qPlatform_MacOS
    Str255 pFontName;
    pFontName[0] = fontName.length ();
    memcpy (&pFontName[1], fontName.c_str (), pFontName[0]);
    short fontNum = 0;
    ::GetFNum (pFontName, &fontNum);
    // Alas, the Mac font Manager returns ZERO as the font number if it really
    // has no idea about the font. This is NOT what we want. But unsure what we can do better at this point!
    fFontSpecifier = fontNum;
#elif qPlatform_Windows
    Characters::CString::Copy (fFontInfo.lfFaceName, NEltsOf (fFontInfo.lfFaceName), fontName.c_str ());
    fFontInfo.lfCharSet = DEFAULT_CHARSET;
#elif qStroika_FeatureSupported_XWindows
    fFontFamily = fontName;
#endif
}

#if qPlatform_Windows
Led_FontSpecification::FontNameSpecifier::FontNameSpecifier (const Led_SDK_Char* from)
{
    Characters::CString::Copy (fName, NEltsOf (fName), from);
}
#endif

void Led_FontSpecification::SetFontNameSpecifier (FontNameSpecifier fontNameSpecifier)
{
#if qPlatform_MacOS
    fFontSpecifier = fontNameSpecifier;
#elif qPlatform_Windows
    Characters::CString::Copy (fFontInfo.lfFaceName, NEltsOf (fFontInfo.lfFaceName), fontNameSpecifier.fName);
    fFontInfo.lfCharSet = DEFAULT_CHARSET;
#elif qStroika_FeatureSupported_XWindows
    fFontFamily = fontNameSpecifier;
#endif
}

/*
 ********************************************************************************
 ************************ Led_IncrementalFontSpecification **********************
 ********************************************************************************
 */

/*
@METHOD:        Intersection
@DESCRIPTION:   <p>Compute the subset of the two @'Led_IncrementalFontSpecification' arguments where both parts
            are valid and identical.</p>
*/
Led_IncrementalFontSpecification Led::Intersection (const Led_IncrementalFontSpecification& lhs, const Led_IncrementalFontSpecification& rhs)
{
    Led_IncrementalFontSpecification result = lhs;

    // FontName Info
    {
        if (not lhs.GetFontNameSpecifier_Valid () or not rhs.GetFontNameSpecifier_Valid () or
            lhs.GetFontNameSpecifier () != rhs.GetFontNameSpecifier ()) {
            result.InvalidateFontNameSpecifier ();
        }
    }

    // Style Info
    {
        if (not lhs.GetStyle_Bold_Valid () or not rhs.GetStyle_Bold_Valid () or
            lhs.GetStyle_Bold () != rhs.GetStyle_Bold ()) {
            result.InvalidateStyle_Bold ();
        }
    }
    {
        if (not lhs.GetStyle_Italic_Valid () or not rhs.GetStyle_Italic_Valid () or
            lhs.GetStyle_Italic () != rhs.GetStyle_Italic ()) {
            result.InvalidateStyle_Italic ();
        }
    }
    {
        if (not lhs.GetStyle_Underline_Valid () or not rhs.GetStyle_Underline_Valid () or
            lhs.GetStyle_Underline () != rhs.GetStyle_Underline ()) {
            result.InvalidateStyle_Underline ();
        }
    }
    {
        if (not lhs.GetStyle_SubOrSuperScript_Valid () or not rhs.GetStyle_SubOrSuperScript_Valid () or
            lhs.GetStyle_SubOrSuperScript () != rhs.GetStyle_SubOrSuperScript ()) {
            result.InvalidateStyle_SubOrSuperScript ();
        }
    }
#if qPlatform_MacOS
    {
        if (not lhs.GetStyle_Outline_Valid () or not rhs.GetStyle_Outline_Valid () or
            lhs.GetStyle_Outline () != rhs.GetStyle_Outline ()) {
            result.InvalidateStyle_Outline ();
        }
    }
    {
        if (not lhs.GetStyle_Shadow_Valid () or not rhs.GetStyle_Shadow_Valid () or
            lhs.GetStyle_Shadow () != rhs.GetStyle_Shadow ()) {
            result.InvalidateStyle_Shadow ();
        }
    }
    {
        if (not lhs.GetStyle_Condensed_Valid () or not rhs.GetStyle_Condensed_Valid () or
            lhs.GetStyle_Condensed () != rhs.GetStyle_Condensed ()) {
            result.InvalidateStyle_Condensed ();
        }
    }
    {
        if (not lhs.GetStyle_Extended_Valid () or not rhs.GetStyle_Extended_Valid () or
            lhs.GetStyle_Extended () != rhs.GetStyle_Extended ()) {
            result.InvalidateStyle_Extended ();
        }
    }
#elif qPlatform_Windows
    {
        if (not lhs.GetStyle_Strikeout_Valid () or not rhs.GetStyle_Strikeout_Valid () or
            lhs.GetStyle_Strikeout () != rhs.GetStyle_Strikeout ()) {
            result.InvalidateStyle_Strikeout ();
        }
    }
#endif

    // Font Color Info
    {
        if (not lhs.GetTextColor_Valid () or not rhs.GetTextColor_Valid () or
            lhs.GetTextColor () != rhs.GetTextColor ()) {
            result.InvalidateTextColor ();
        }
    }

    // Size Info
    {
        // careful - cuz InvalidatePointSizeIncrement and InvalidatePointSize both
        // invalidate the same thing...
        // Must check that if ANY specification - its the same on both sides (lhs & rhs)
        bool needsInval = false;
        if (lhs.GetPointSizeIncrement_Valid () != rhs.GetPointSizeIncrement_Valid () or
            (lhs.GetPointSizeIncrement_Valid () and lhs.GetPointSizeIncrement () != rhs.GetPointSizeIncrement ())) {
            needsInval = true;
        }
        if (lhs.GetPointSize_Valid () != rhs.GetPointSize_Valid () or
            (lhs.GetPointSize_Valid () and lhs.GetPointSize () != rhs.GetPointSize ())) {
            needsInval = true;
        }
        if (needsInval) {
            result.InvalidatePointSize ();
        }
    }

    return result;
}

#if qPlatform_Windows
/*
 ********************************************************************************
 *************************** Led_Tablet_::RecolorHelper *************************
 ********************************************************************************
 */
class Led_Tablet_::RecolorHelper {
public:
    RecolorHelper (HDC baseHDC, Led_Size size, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor);
    ~RecolorHelper ();

public:
    static RecolorHelper* CheckCacheAndReconstructIfNeeded (RecolorHelper* _THIS_, HDC baseHDC, Led_Size size, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor);

public:
    nonvirtual void DoRecolor (const Led_Rect& hilightArea);

private:
    nonvirtual void DoRecolor_SimpleDSTINVERT (const Led_Rect& hilightArea);
    nonvirtual void DoRecolor_SimplePATINVERT (const Led_Rect& hilightArea);

protected:
    nonvirtual void DoRecolor_CopyTo8BitManualMungePixAndBack (const Led_Rect& hilightArea);

private:
    nonvirtual void MakeMappingTable ();

private:
    uint8_t fMappingTable[256];

private:
    nonvirtual uint8_t FindClosestColorInColorTable_ (COLORREF c) const;

private:
    nonvirtual COLORREF MapColor (COLORREF c) const; // use fHilightBackColor etc to map color
    nonvirtual COLORREF MapColor (RGBQUAD c) const;

private:
    HDC      fBaseDC;
    Led_Size fSize;
    RGBQUAD  fColorTable[256];
    HBITMAP  fDibSection;
    LPBYTE   fDibData;
    size_t   fDibDataByteCount;
    HDC      fHMemDC;
    HBITMAP  fOldBitmap;
    COLORREF fHilightBackColor;
    COLORREF fHilightForeColor;
    COLORREF fOldBackColor;
    COLORREF fOldForeColor;
};
inline COLORREF Led_Tablet_::RecolorHelper::MapColor (COLORREF c) const
{
    float fIntrp;
    fIntrp   = (float)(255 - GetRValue (c)) / 256.0f;
    BYTE red = static_cast<BYTE> (
        GetRValue (fHilightBackColor) +
        fIntrp * GetRValue (fHilightForeColor) -
        fIntrp * GetRValue (fHilightBackColor));

    fIntrp     = (float)(255 - GetGValue (c)) / 256.0f;
    BYTE green = static_cast<BYTE> (
        GetGValue (fHilightBackColor) +
        fIntrp * GetGValue (fHilightForeColor) -
        fIntrp * GetGValue (fHilightBackColor));

    fIntrp    = (float)(255 - GetBValue (c)) / 256.0f;
    BYTE blue = static_cast<BYTE> (
        GetBValue (fHilightBackColor) +
        fIntrp * GetBValue (fHilightForeColor) -
        fIntrp * GetBValue (fHilightBackColor));
    return RGB (red, green, blue);
}
inline COLORREF Led_Tablet_::RecolorHelper::MapColor (RGBQUAD c) const
{
    return MapColor (RGB (c.rgbRed, c.rgbGreen, c.rgbBlue));
}

Led_Tablet_::RecolorHelper::RecolorHelper (HDC baseHDC, Led_Size size, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor)
    : fDibData (nullptr)
    //fMappingTable ()
    , fDibDataByteCount (0)
    , fHMemDC (nullptr)
    , fBaseDC (baseHDC)
    , fSize (size)
    , fColorTable ()
    , fDibSection (nullptr)
    , fOldBitmap (nullptr)
    , fHilightBackColor (hilightBackColor.GetOSRep ())
    , fHilightForeColor (hilightForeColor.GetOSRep ())
    , fOldBackColor (oldBackColor.GetOSRep ())
    , fOldForeColor (oldForeColor.GetOSRep ())
{
    CreateStandardColorTable (fColorTable, fHilightBackColor, fHilightForeColor, fOldBackColor, fOldForeColor);
    fDibSection = Create8BitDIBSection (baseHDC, size.h, size.v, fColorTable, &fDibData);
    BITMAP bm;
    Verify (::GetObject (fDibSection, sizeof (BITMAP), &bm) == sizeof (BITMAP));
    fDibDataByteCount = bm.bmWidthBytes * bm.bmHeight;
    fHMemDC           = ::CreateCompatibleDC (fBaseDC);
    fOldBitmap        = reinterpret_cast<HBITMAP> (::SelectObject (fHMemDC, fDibSection));
    MakeMappingTable ();
}

Led_Tablet_::RecolorHelper::~RecolorHelper ()
{
    if (fDibSection != nullptr) {
        ::SelectObject (fHMemDC, fOldBitmap);
        ::DeleteDC (fHMemDC);
        ::DeleteObject (fDibSection);
    }
}

Led_Tablet_::RecolorHelper* Led_Tablet_::RecolorHelper::CheckCacheAndReconstructIfNeeded (RecolorHelper* _THIS_, HDC baseHDC, Led_Size size, Led_Color hilightBackColor, Led_Color hilightForeColor, Led_Color oldBackColor, Led_Color oldForeColor)
{
    if (_THIS_ == nullptr or
        size.h > _THIS_->fSize.h or
        size.v > _THIS_->fSize.v or
        baseHDC != _THIS_->fBaseDC or
        hilightBackColor.GetOSRep () != _THIS_->fHilightBackColor or
        hilightForeColor.GetOSRep () != _THIS_->fHilightForeColor or
        oldBackColor.GetOSRep () != _THIS_->fOldBackColor or
        oldForeColor.GetOSRep () != _THIS_->fOldForeColor) {
        Led_Size areaSize = size;
        if (_THIS_ != nullptr) {
            areaSize.v = max (size.v, _THIS_->fSize.v);
            areaSize.h = max (size.h, _THIS_->fSize.h);
        }
        RecolorHelper* tmp = new RecolorHelper (baseHDC, areaSize, hilightBackColor, hilightForeColor, oldBackColor, oldForeColor);
        delete _THIS_;
        return tmp;
    }
    return _THIS_;
}

void Led_Tablet_::RecolorHelper::MakeMappingTable ()
{
    for (size_t i = 0; i < 256; ++i) {
        fMappingTable[i] = FindClosestColorInColorTable_ (MapColor (fColorTable[i]));
    }
}

uint8_t Led_Tablet_::RecolorHelper::FindClosestColorInColorTable_ (COLORREF c) const
{
    // walk through the color table and see which color is closest to 'c'
    uint8_t      closest         = 0;
    unsigned int closestDistance = 0xffffffff; // big distance
    for (size_t i = 0; i < 256; ++i) {
        unsigned int thisDist = Distance_Squared (c, RGB (fColorTable[i].rgbRed, fColorTable[i].rgbGreen, fColorTable[i].rgbBlue));
        if (thisDist < closestDistance) {
            closest         = static_cast<uint8_t> (i);
            closestDistance = thisDist;
            if (closestDistance == 0) {
                break; // not needed, but COULD be a slight speed tweek
            }
        }
    }
    return closest;
}

void Led_Tablet_::RecolorHelper::DoRecolor (const Led_Rect& hilightArea)
{
    HPALETTE hPal            = nullptr;
    HPALETTE hOldPal         = nullptr;
    bool     isPaletteDevice = !!(::GetDeviceCaps (fBaseDC, RASTERCAPS) & RC_PALETTE);
    if (isPaletteDevice) {
        // if it's a palette device, select and realize a palette
        // as a background palette (won't cause a problem is the
        // palette was not selected in the foreground in the main app
        hPal = CreatePaletteForColorTable (fColorTable);
        Execution::ThrowIfNull (hPal);
        hOldPal = ::SelectPalette (fBaseDC, hPal, TRUE);
        ::RealizePalette (fBaseDC);
    }

    DoRecolor_CopyTo8BitManualMungePixAndBack (hilightArea);

    if (isPaletteDevice) {
        if (hOldPal != nullptr) {
            ::SelectPalette (fBaseDC, hOldPal, TRUE);
        }
        ::DeleteObject (hPal);
    }
}

void Led_Tablet_::RecolorHelper::DoRecolor_SimpleDSTINVERT (const Led_Rect& hilightArea)
{
    // Does proper inverse video, but seems to ignore the TextColor/BkColor/Pen/Brush colors.
    // Really should fix this to behave like Mac - replacing the background color with the text hilight color.
    // See SPR#1271
    ::BitBlt (fBaseDC, hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (),
              fBaseDC, hilightArea.left, hilightArea.top, DSTINVERT);
}

void Led_Tablet_::RecolorHelper::DoRecolor_SimplePATINVERT (const Led_Rect& hilightArea)
{
    // Attempt at solving SPR#1271. Works decently - producing the right background - but the text is colored YELLOW instead of WHITE - and so
    // doesn't look very good (not enough contrast).
    Led_Color useColor = Led_Color::kWhite - Led_Color (fHilightBackColor);
    HGDIOBJ   oldPen   = ::SelectObject (fBaseDC, ::GetStockObject (NULL_PEN));
    Led_Brush backgroundBrush (useColor.GetOSRep ());
    HGDIOBJ   oldBrush = ::SelectObject (fBaseDC, backgroundBrush);
    ::BitBlt (fBaseDC, hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (),
              fBaseDC, hilightArea.left, hilightArea.top, PATINVERT);
    (void)::SelectObject (fBaseDC, oldPen);
    (void)::SelectObject (fBaseDC, oldBrush);
}

void Led_Tablet_::RecolorHelper::DoRecolor_CopyTo8BitManualMungePixAndBack (const Led_Rect& hilightArea)
{
    // By commenting stuff in and out - I determined that virtuall ALL the time is spent in this first
    // BitBlt () - LGP 2003-03-11
    // I also found that qUseDIBSectionForOffscreenBitmap made this BitBlt go much faster - to the point of acceptable speed
    // LGP - 2003-03-12

    // Copy the REAL image into our 8-bit DIBSECTION
    ::BitBlt (fHMemDC, 0, 0, hilightArea.GetWidth (), hilightArea.GetHeight (),
              fBaseDC, hilightArea.left, hilightArea.top, SRCCOPY);

    /*
     *
     *  Fiddle the bits:
     *
     *  NB: This code assumes we're pointing at an 8-bit COLOR-LOOKUP-TABLE based Image
     *
     *  Note - this also may be modifying MUCH MORE than is actaully needed. It goes all the way to the
     *  end of the ROW of pixels - but we could be using much less.
     *
     *  In order to remedy that - I tried constructing the DIBSECTION on the fly - instead of caching it. That turned
     *  out to be quite slow (on DELL Precision Workstation 420 (800mz Dual Processor)). This muning code has never shown
     *  up as taking significant time. Its all that BitBlt above.
     *
     *  Anyhow - if this does someday look slow - it can easily be fixed. We can just break the loop into two nested loops,
     *  the outer one over rows, and the inner row loop stopping NOT at the end of the REAL row - but just at the end
     *  of the subset we are using (easy cuz we always start at 0,0).
     */
    Verify (::GdiFlush ()); // make sure bits in sync... - not SURE if this is needed?
    {
        const unsigned char* kMappingTable = fMappingTable;
        unsigned char*       dataStart     = fDibData;
        unsigned char*       dataEnd       = dataStart + fDibDataByteCount;
        for (unsigned char* i = dataStart; i < dataEnd; ++i) {
            *i = kMappingTable[*i];
        }
    }

    // Copy them back
    ::BitBlt (fBaseDC, hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (),
              fHMemDC, 0, 0, SRCCOPY);
}
#endif

/*
 ********************************************************************************
 *********************************** Led_Tablet_ ********************************
 ********************************************************************************
 */
#if qPlatform_MacOS
Led_Tablet_::Led_Tablet_ (GrafPtr gp)
    : fGrafPort (gp)
{
    RequireNotNull (gp);
}
#elif qPlatform_Windows
Led_Tablet_::Led_Tablet_ (HDC hdc, Led_Tablet_::OwnDCControl ownsDC)
    : m_hDC (hdc)
    , fRecolorHelper (nullptr)
    , m_hAttribDC (hdc)
    , m_bPrinting (false)
    , fOwnsDC (ownsDC)
    , fLogPixelsV (0)
    , fLogPixelsH (0)
{
}
#elif qStroika_FeatureSupported_XWindows
Led_Tablet_::Led_Tablet_ (Display* display, Drawable drawable)
    : fDrawableOrigin (Led_Point (0, 0))
    , fFontCache ()
    , fCurDrawLineLoc (Led_Point (0, 0))
    , fDisplay (display)
    , fDrawable (drawable)
    , fGC (nullptr)
    , fColormap (0)
    , fCachedFontInfo (nullptr)
    , fFontMappingCache ()
{
    int screen = DefaultScreen (display);
    fGC = ::XCreateGC (display, drawable, 0, nullptr);
    ::XSetForeground (display, fGC, BlackPixel (display, screen));
    ::XSetBackground (display, fGC, WhitePixel (display, screen));
    XSetGraphicsExposures (display, fGC, true);
    XWindowAttributes wa;
    (void)::memset (&wa, 0, sizeof (wa));
    /*
     *  Since we don't know for sure the drawable is a window - catch the error and ignore it. Don't let
     *  XErrorHandler do anything bad.
     */
    int (*oldErrHandler) (Display*, XErrorEvent*) = ::XSetErrorHandler (IgnoreXErrorHandler);
    Status s = ::XGetWindowAttributes (display, drawable, &wa);
    ::XSetErrorHandler (oldErrHandler);
    if (s != 0 and wa.map_installed) {
        fColormap = wa.colormap;
    }
    else {
        fColormap = DefaultColormap (fDisplay, DefaultScreen (fDisplay));
        //          Assert (false);//???
        // make new colormap...call XGetWMColormap ()...
        // CALL XSetWindowColormap ().... if not gotten
        // .
    }
}
#endif

Led_Tablet_::~Led_Tablet_ ()
{
#if qPlatform_Windows
    delete fRecolorHelper;
    if (m_hDC != nullptr and fOwnsDC == eOwnsDC) {
        ::DeleteDC (Detach ());
    }
#elif qStroika_FeatureSupported_XWindows
    ::XFreeGC (fDisplay, fGC);
    for (auto i = fFontCache.begin (); i != fFontCache.end (); ++i) {
        ::XFreeFont (fDisplay, i->second);
    }
#endif
}

/*
@METHOD:        Led_Tablet_::CvtFromTWIPS
@DESCRIPTION:   <p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).</p>
    <p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
*/
Led_Point Led_Tablet_::CvtFromTWIPS (Led_TWIPS_Point from) const
{
    return Led_Point (CvtFromTWIPSV (from.v), CvtFromTWIPSH (from.h));
}

/*
@METHOD:        Led_Tablet_::CvtToTWIPS
@DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
    <p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
*/
Led_TWIPS_Point Led_Tablet_::CvtToTWIPS (Led_Point from) const
{
    return Led_TWIPS_Point (CvtToTWIPSV (from.v), CvtToTWIPSH (from.h));
}

/*
@METHOD:        Led_Tablet_::CvtFromTWIPS
@DESCRIPTION:   <p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).</p>
    <p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
*/
Led_Rect Led_Tablet_::CvtFromTWIPS (Led_TWIPS_Rect from) const
{
    return Led_Rect (CvtFromTWIPS (from.GetOrigin ()), Led_Size (CvtFromTWIPS (from.GetSize ())));
}

/*
@METHOD:        Led_Tablet_::CvtToTWIPS
@DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
    <p>See also @'Led_Tablet_::CvtFromTWIPSV', @'Led_Tablet_::CvtFromTWIPSH', @'Led_Tablet_::CvtToTWIPSV', @'Led_Tablet_::CvtToTWIPSH'.</p>
*/
Led_TWIPS_Rect Led_Tablet_::CvtToTWIPS (Led_Rect from) const
{
    return Led_TWIPS_Rect (CvtToTWIPS (from.GetOrigin ()), CvtToTWIPS (Led_Point (from.GetSize ())));
}

/*
@METHOD:        Led_Tablet_::ScrollBitsAndInvalRevealed
@DESCRIPTION:   <p>Scroll the given 'windowRect' by 'scrollVBy localical units. The area of the window exposed by this
            action is invalidated (so a later update event will fix it).</p>
*/
void Led_Tablet_::ScrollBitsAndInvalRevealed (const Led_Rect& windowRect, Led_Coordinate scrollVBy)
{
#if qPlatform_MacOS
    Rect      qdMoveRect = AsQDRect (windowRect);
    RgnHandle updateRgn  = ::NewRgn ();
    Led_ThrowIfNull (updateRgn);
    SetPort ();
    ::ScrollRect (&qdMoveRect, 0, scrollVBy, updateRgn);
#if TARGET_CARBON
    ::InvalWindowRgn (::GetWindowFromPort (fGrafPort), updateRgn);
#else
    ::InvalRgn (updateRgn);
#endif
    ::DisposeRgn (updateRgn);
#elif qPlatform_Windows
    RECT gdiMoveRect = AsRECT (windowRect);
    // NB: I used to use ScrollDC (Led 2.1 and earlier). But that code appeared to sometimes leave
    // little bits of crufy around. I never understood why. But I assume it was a windows bug.
    HWND w = GetWindow ();
    Led_ThrowIfNull (w);
    ::ScrollWindow (w, 0, scrollVBy, &gdiMoveRect, &gdiMoveRect);
#elif qStroika_FeatureSupported_XWindows
    if (scrollVBy != 0) {
        {
            /*
             *  We cannot do a scrollbits if there are any pending update events. Ideally - we would PREVENT
             *  this situation by having Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Update_ () work properly.
             *  But - alas - after a day or two's efforts - I've been unable to get that code working.
             *  Sigh. Luckily - this seems to prevent any drawing bugs, and only results in an occasional
             *  drawing (scrolling) slowdown. I guess we can live with that. -- LGP 2001-05-18
             */
            XEvent e;
            if (::XCheckTypedEvent (fDisplay, Expose, &e) or ::XCheckTypedEvent (fDisplay, GraphicsExpose, &e)) {
                ::XPutBackEvent (fDisplay, &e);
                Led_ThrowIfNull (0);
            }
        }
        Led_Rect srcMoveRect = windowRect;
        Led_Rect exposedRect = windowRect;
        if (scrollVBy > 0) {
            // moving bits down (up scrollbar button)
            srcMoveRect.bottom -= scrollVBy;
            exposedRect.bottom = scrollVBy;
        }
        else {
            srcMoveRect.top -= scrollVBy;
            exposedRect.top = exposedRect.bottom + scrollVBy;
        }
        XGCValues prevValues;
        const unsigned long kSavedAttrs = GCGraphicsExposures;
        (void)::memset (&prevValues, 0, sizeof (prevValues));
        ::XGetGCValues (fDisplay, fGC, kSavedAttrs, &prevValues);
        ::XSetClipMask (fDisplay, fGC, None);
        ::XSetGraphicsExposures (fDisplay, fGC, true);
        ::XCopyArea (fDisplay, fDrawable, fDrawable, fGC,
                     srcMoveRect.GetLeft (), srcMoveRect.GetTop (),
                     srcMoveRect.GetWidth (), srcMoveRect.GetHeight (),
                     srcMoveRect.GetLeft (), srcMoveRect.top + scrollVBy);
        ::XChangeGC (fDisplay, fGC, kSavedAttrs, &prevValues);

/*
         *  After the scrollbits - we leave a little rectangle exposed. We must mark that as needing drawing.
         */
#if 1
        XEvent event;
        (void)::memset (&event, 0, sizeof (event));
        event.type = Expose;
        event.xexpose.send_event = true;
        event.xexpose.display = fDisplay;
        event.xexpose.window = fDrawable;
        event.xexpose.x = (int)exposedRect.GetLeft ();
        event.xexpose.y = (int)exposedRect.GetTop ();
        event.xexpose.width = (int)exposedRect.GetWidth ();
        event.xexpose.height = (int)exposedRect.GetHeight ();
        event.xexpose.count = 0;
        Verify (::XSendEvent (fDisplay, fDrawable, false, ExposureMask, &event) != 0);
#else
        ::XClearArea (fDisplay, fDrawable, (int)exposedRect.GetLeft (), (int)exposedRect.GetTop (),
                      (unsigned int)exposedRect.GetWidth (), (unsigned int)exposedRect.GetHeight (), true);
#endif
    }
#else
    Assert (false); //NYI
#endif
}

/*
@METHOD:        Led_Tablet_::FrameRegion
@DESCRIPTION:   <p>Draw the outline of the given region 'r' in color 'c'.</p>
*/
void Led_Tablet_::FrameRegion (const Led_Region& r, const Led_Color& c)
{
#if qPlatform_MacOS
    Led_MacPortAndClipRegionEtcSaver saver; // unclear if this is useful/needed?
    SetPort ();
    PenMode (srcCopy); // ???
    GDI_RGBForeColor (c.GetOSRep ());
    ::FrameRgn (r.GetOSRep ());
#elif qPlatform_Windows
    Led_Brush brush = Led_Brush (c.GetOSRep ());
    (void)::FrameRgn (*this, r, brush, 1, 1);
#else
    Assert (false);                                  // NYI
#endif
}

/*
@METHOD:        Led_Tablet_::FrameRectangle
@DESCRIPTION:   <p>Draw the outline of the given rectangle 'r' in color 'c' and with
            borderWidth (pen width) 'borderWidth'. This function does NOT use the currently selected
            pen or brush, or anything like that. It draws a border just INSIDE the rectangle specified
            by 'r'.
                </p>
*/
void Led_Tablet_::FrameRectangle (const Led_Rect& r, Led_Color c, Led_Distance borderWidth)
{
    /*
     *  Almost certainly can implement much more efficiently, but leave like this for now to assure pixel-for-pixel
     *  equiv across GDIs.
     */
    Led_Rect topBar  = Led_Rect (r.top, r.left, borderWidth, r.GetWidth ());
    Led_Rect leftBar = Led_Rect (r.top, r.left, r.GetHeight (), borderWidth);
    EraseBackground_SolidHelper (topBar, c);                                               // TOP
    EraseBackground_SolidHelper (leftBar, c);                                              // LEFT
    EraseBackground_SolidHelper (topBar + Led_Point (r.GetHeight () - borderWidth, 0), c); // BOTTOM
    EraseBackground_SolidHelper (leftBar + Led_Point (0, r.GetWidth () - borderWidth), c); // RIGHT
}

/*
@METHOD:        Led_Tablet_::MeasureText
@DESCRIPTION:   <p>Measure the widths of the given argument @'Led_tChar's. Assign those widths into
            the array 'charLocations'. (EXPLAIN CAREFULLY REQUIREMENTS ABOUT BUFSIZE of charLocations and handling of
            zero case and offset rules, handling of tabs, etc)</p>
                <p>Note that the resulting measured text must come out in non-descreasing order (there can be zero
            character widths, but never negative).</p>
*/
void Led_Tablet_::MeasureText (const Led_FontMetrics& precomputedFontMetrics,
                               const Led_tChar* text, size_t nTChars, Led_Distance* charLocations)
{
    RequireNotNull (text);
    RequireNotNull (charLocations);
#if qPlatform_MacOS
    SetPort ();
#endif

#if qPlatform_MacOS
    const Led_Distance kMaxTextWidthResult = 0x7fff;
#elif qPlatform_Windows
    Led_Distance kMaxTextWidthResult = kRunning32BitGDI ? 0x7fffffff : 0x7fff;
    if (IsPrinting ()) {
        // See SPR#0435
        SIZE ve = GetViewportExt ();
        SIZE we = GetWindowExt ();
        kMaxTextWidthResult = ::MulDiv (kMaxTextWidthResult, we.cx, ve.cx) - 1;
    }
#elif qStroika_FeatureSupported_XWindows
    const Led_Distance kMaxTextWidthResult = 0x7fff; //X-TMP-HACK-LGP991213
#endif
    size_t kMaxChars = kMaxTextWidthResult / precomputedFontMetrics.GetMaxCharacterWidth ();
#if qUseUniscribeToImage
    if (kMaxChars > kMaxUNISCRIBECharacters) {
        kMaxChars = kMaxUNISCRIBECharacters;
    }
#endif
    Assert (kMaxChars > 1);

    Led_Distance runningCharCount = 0;
    for (size_t charsToGo = nTChars; charsToGo > 0;) {
        size_t i = nTChars - charsToGo;
        Assert (i < nTChars);

        while (text[i] == '\t') {
            Assert (charsToGo > 0);

            charLocations[i] = runningCharCount;
            i++;
            if (--charsToGo == 0) {
                break;
            }
        }
        if (charsToGo == 0) {
            break;
        }

        size_t charsThisTime = min (charsToGo, kMaxChars);
        for (size_t tabIndex = 1; tabIndex < charsThisTime; tabIndex++) {
            if (text[i + tabIndex] == '\t') {
                charLocations[i + tabIndex] = runningCharCount;
                charsThisTime               = tabIndex;
                break;
            }
        }

#if qMultiByteCharacters
        /*
         *  See if i+charsThisTime is on a real character boundary - and if not - then step back one so it is.
         */
        if (Led_FindPrevOrEqualCharBoundary (&text[0], &text[i + charsThisTime]) != &text[i + charsThisTime]) {
            Assert (charsThisTime > 0);
            charsThisTime--;
        }
#endif

#if qPlatform_MacOS
        Memory::SmallStackBuffer<short> shortOffsets (charsThisTime + 1);
        Assert (Led_GetCurrentGDIPort () == *this);
        ::MeasureText (charsThisTime, &text[i], shortOffsets);
        for (size_t j = 0; j < charsThisTime; j++) {
            charLocations[i + j] = shortOffsets[j + 1] + runningCharCount; // Silly Apple defines shortOffsets[0] always to be zero!
        }
#elif qPlatform_Windows
        SIZE size;
        Assert (sizeof (int) == sizeof (Led_Distance));
#if qUseUniscribeToImage && qWideCharacters
        {
            if (sUniscribeDLL.IsAvail ()) {
                SCRIPT_CONTROL scriptControl;
                memset (&scriptControl, 0, sizeof (scriptControl));

                SCRIPT_STATE scriptState;
                memset (&scriptState, 0, sizeof (scriptState));
                // Important to ALLOW ScriptStringAnalyse to REORDER (so don't set this true) cuz otherwise it won't get right measurements
                // for arabic font substition (shaping)---LGP 2003-01-02

                // MAYBE THIS IS WRONG - AND COVERING UP ANOTHER BUG??? DUNNO? MUST BE CAREFUL ABOUT MIRRORING (SYMSWAP). If done HERE,
                // then I must NOT inhibit symswap. I DON'T THINK I CAN DO it here cuz the draw code gets done in RUNS... HMMM
                // LGP 2003-01-02...
                //  scriptState.fOverrideDirection  = true;     // I THINK This is how I say already in display order
                scriptState.fInhibitSymSwap = true;

                SCRIPT_STRING_ANALYSIS ssa;
                memset (&ssa, 0, sizeof (ssa));

                Verify (sUniscribeDLL.ScriptStringAnalyse (m_hAttribDC, &text[i], charsThisTime, 0, -1, SSA_GLYPHS | SSA_FALLBACK, -1, &scriptControl, &scriptState, nullptr, nullptr, nullptr, &ssa) == S_OK);

#if qTryScriptToCPX
                for (size_t j = 0; j < charsThisTime; j++) {
                    int leadingEdge = 0;
                    int trailingEdge = 0;
                    Verify (sUniscribeDLL.ScriptStringCPtoX (ssa, j, false, &leadingEdge) == S_OK);
                    Verify (sUniscribeDLL.ScriptStringCPtoX (ssa, j, true, &trailingEdge) == S_OK);

                    int logicalWidth = abs (trailingEdge - leadingEdge); // can be zero-width - but never negative...
                    if (j == 0) {
                        charLocations[i + j] = runningCharCount + logicalWidth;
                    }
                    else {
                        charLocations[i + j] = charLocations[i + j - 1] + logicalWidth;
                    }
                }
#else
                Memory::SmallStackBuffer<int> logicalWidths (charsThisTime);
                Verify (sUniscribeDLL.ScriptStringGetLogicalWidths (ssa, logicalWidths) == S_OK);

                Assert (charsThisTime > 0);
                Assert (logicalWidths[0] >= 0); // can be zero-width - but never negative...
                charLocations[i] = runningCharCount + logicalWidths[0];
                for (size_t j = 1; j < charsThisTime; j++) {
                    Assert (logicalWidths[j] >= 0); // can be zero-width - but never negative...
                    charLocations[i + j] = charLocations[i + j - 1] + logicalWidths[j];
                }
#endif
                Verify (sUniscribeDLL.ScriptStringFree (&ssa) == S_OK);
                goto Succeeded;
            }
        }
#endif

        // Default code - if UNISCRIBE not compiled for or not dynamically loaded
        Win32_GetTextExtentExPoint (m_hAttribDC, &text[i], charsThisTime, kMaxTextWidthResult, nullptr, (int*)&charLocations[i], &size);
        for (size_t j = 0; j < charsThisTime; j++) {
            charLocations[i + j] += runningCharCount;
        }

#if qUseUniscribeToImage && qWideCharacters
    Succeeded:
#endif
#elif qStroika_FeatureSupported_XWindows
        Led_ThrowIfNull (fCachedFontInfo);
        // Gross hack - sloppy implementation (SLOW). But I'm not sure what in the X SDK allows this to be done faster! -- LGP 2000-09-05
        // Actually - not TOO bad since whole computation is done client-side. Seems to be working OK - at least for now - LGP 2001-05-05
        for (size_t j = 0; j < charsThisTime; ++j) {
            charLocations[i + j] = runningCharCount + ::XTextWidth (const_cast<XFontStruct*> (fCachedFontInfo), &text[i], j + 1);
        }
#endif

        runningCharCount = charLocations[i + charsThisTime - 1];

        Assert (charsToGo >= charsThisTime);
        charsToGo -= charsThisTime;
    }

// LGP-991220 - This is generating asserts elsewhere - and seems like such a hack. Not sure why needed. Try getting rid of and see what happens?
#if qPlatform_Windows && 0
    // This gross hack is cuz we do a GetTextExtent() at the end of the
    // DrawText for PC, to see how much we drew. This is the only hack
    // I could think of to assure we get consistent results (which is very important).
    // This REALLY shold be done better - fix may not be here, but in DrawCode below...
    // LGP 960509

    // Now, special hack- assert no qMultiByteCharacters
    if (nTChars > 0) {
        int  lastWidth = (nTChars == 1) ? charLocations[0] : (charLocations[nTChars - 1] - charLocations[nTChars - 2]);
        SIZE size;
        AssertNotNull (m_hAttribDC);
        Win32_GetTextExtentPoint (m_hAttribDC, &text[nTChars - 1], 1, &size);
        int sbWidth = size.cx;
        if (sbWidth != lastWidth) {
            charLocations[nTChars - 1] = ((nTChars == 1) ? 0 : charLocations[nTChars - 2]) + sbWidth;
        }
    }
#endif

#if qDebug
    {
        // Assure charLocations are in non-decreasing order (OK to have some zero width - but never negative).
        Led_Distance d = 0;
        for (size_t i = 0; i < nTChars; ++i) {
            Ensure (d <= charLocations[i]);
            d = charLocations[i];
        }
    }
#endif
}

/*
@METHOD:        Led_Tablet_::TabbedTextOut
@DESCRIPTION:   <p>Draw the given text (@'Led_tChars') to this tablet object, at the given logical coordinates. Use the given
            tabstop origin, and tabStopList object to compute where tabs go. Return the amountDrawn (number of pixels used by draw).
            (EXPLAIN CAREFULLY REQUIREMENTS ABOUT BUFSIZE of charLocations and handling of
            zero case and offset rules, handling of tabs, AND MUCH MORE etc)</p>
                <p>Note - for BIDI text - the VIRTUAL (display) text is what should be passed in. Text will be imaged
            left to right. This means that for RTL text such as Arabic or Hebrew - it should have already been
            re-ordered in the argument passed in, and any mirroring should have already been done. This routine WILL
            take care of any contextual shaping required (glyph selection based on context - as with Arabic).</p>
*/
void Led_Tablet_::TabbedTextOut ([[maybe_unused]] const Led_FontMetrics& precomputedFontMetrics, const Led_tChar* text, size_t nBytes,
                                 [[maybe_unused]] TextDirection direction,
                                 Led_Point outputAt, Led_Coordinate hTabOrigin, const Led_TabStopList& tabStopList,
                                 Led_Distance* amountDrawn, Led_Coordinate hScrollOffset)
{
#if qPlatform_MacOS
    SetPort ();
#endif

    Led_Distance     widthSoFar = 0;
    const Led_tChar* textCursor = text;
    const Led_tChar* textEnd    = text + nBytes;
    while (textCursor < textEnd) {
        // Skip over tabs
        const Led_tChar* nextTabAt = textCursor;
        for (; nextTabAt < textEnd;) {
            if (*nextTabAt == '\t') {
                break;
            }

// We can get away with nextTabAt++ even under SJIS so long as...
#if qMultiByteCharacters
            Assert (not Led_IsValidSecondByte ('\t'));
#endif
            nextTabAt++;
        }

// Actually image the characters
#if qPlatform_MacOS
        Assert (Led_GetCurrentGDIPort () == *this);
        Led_Point cursor = Led_Point (outputAt.v + precomputedFontMetrics.GetAscent (), outputAt.h - hScrollOffset); // ascent - goto baseline...
        ::MoveTo (cursor.h + widthSoFar, cursor.v);
        ::TextMode (srcOr);
        ::DrawText (textCursor, 0, nextTabAt - textCursor);
#if TARGET_CARBON
        {
            Point junk;
            widthSoFar = ::GetPortPenLocation (Led_GetCurrentGDIPort (), &junk)->h - cursor.h;
        }
#else
        widthSoFar                      = Led_GetCurrentGDIPort ()->pnLoc.h - cursor.h;
#endif
#elif qPlatform_Windows
        int oldBkMode = SetBkMode (TRANSPARENT);

#if qUseUniscribeToImage && qWideCharacters
        {
#if qTryToOptimizeLongUNISCRIBEScriptOutCalls
            const size_t kMaxCharsToDrawAtATime = 500;
#endif
            size_t len = nextTabAt - textCursor;
            if (len == 0) {
                goto Succeeded; // UNISCRIBE barfs on zero-length strings. Nothing todo anyhow...
            }
            if (sUniscribeDLL.IsAvail ()) {
                SCRIPT_CONTROL scriptControl;
                memset (&scriptControl, 0, sizeof (scriptControl));

                SCRIPT_STATE scriptState;
                memset (&scriptState, 0, sizeof (scriptState));
                scriptState.fOverrideDirection = true; // I THINK This is how I say already in display order
                scriptState.fInhibitSymSwap = true;

                const Led_tChar* thisChunkPtr = textCursor;
                for (size_t thisChunkLen = len; thisChunkLen > 0;) {
                    if (thisChunkLen > kMaxUNISCRIBECharacters) {
                        thisChunkLen = kMaxUNISCRIBECharacters;
                    }
#if qTryToOptimizeLongUNISCRIBEScriptOutCalls
                    if (thisChunkLen > kMaxCharsToDrawAtATime) {
                        thisChunkLen = kMaxCharsToDrawAtATime;
                    }
#endif
                    {
                        SCRIPT_STRING_ANALYSIS ssa;
                        memset (&ssa, 0, sizeof (ssa));
                        if (not SUCCEEDED (sUniscribeDLL.ScriptStringAnalyse (m_hDC, thisChunkPtr, thisChunkLen, 0, -1, SSA_GLYPHS | SSA_FALLBACK, -1, &scriptControl, &scriptState, nullptr, nullptr, nullptr, &ssa))) {
                            goto UniscribeFailure; // Can happen - for example - during ColeControl::DrawMetaFile ()
                            // call - see SPR#1447 - fallback on older draw code...
                        }
                        Verify (sUniscribeDLL.ScriptStringOut (ssa, outputAt.h + int (widthSoFar) - hScrollOffset, outputAt.v, 0, nullptr, 0, 0, false) == S_OK);
                        const SIZE* sizep = sUniscribeDLL.ScriptString_pSize (ssa);
                        AssertNotNull (sizep);
                        widthSoFar += sizep->cx;
                        Verify (sUniscribeDLL.ScriptStringFree (&ssa) == S_OK);
                    }

#if qTryToOptimizeLongUNISCRIBEScriptOutCalls
                    // only rarely (tune this) - check if we've already drawn past the end of the HDC.
                    // (not REALLY doing a great/reliable test for that either???
                    if (len > kMaxCharsToDrawAtATime) {
                        POINT vpOrg;
                        Verify (::GetViewportOrgEx (m_hAttribDC, &vpOrg));
                        POINT wOrg;
                        Verify (::GetWindowOrgEx (m_hAttribDC, &wOrg));
                        int deviceWidth = GetDeviceCaps (HORZRES);
                        POINT x = vpOrg;
                        x.x += deviceWidth;
                        Verify (::DPtoLP (m_hAttribDC, &x, 1));
                        if (x.x < outputAt.h + int (widthSoFar) - hScrollOffset) {
                            // assume we're done - and can break out...
                            break;
                        }
                    }
#endif

                    thisChunkPtr += thisChunkLen;
                    thisChunkLen = (textCursor + len - thisChunkPtr); // set length left to go to be end of REAL buf minus new start ptr
                    // at TOP of loop - it will be trimmed down to kMaxUNISCRIBECharacters
                }
                goto Succeeded;
            }
        }
    UniscribeFailure:
#endif

        if (direction == eLeftToRight) {
            Win32_TextOut (m_hDC, static_cast<int> (outputAt.h + widthSoFar - hScrollOffset), static_cast<int> (outputAt.v), textCursor, static_cast<int> (nextTabAt - textCursor));

            // Geez! There must be SOME API within Win32 to give me this info (like SetTextAlign (UPDATE_CP))
            // without recomputing it. But there doesn't appear to be. So we must recompute!
            // LGP 960503

            // A SLIGHT optimization is now possible here - we don't need to compute the last GetTextExtent() if amountDrawn
            // is nullptr (typically TRUE) - LGP 960521
            if (amountDrawn != nullptr or (nextTabAt < textEnd)) {
                SIZE size;
                Win32_GetTextExtentPoint (m_hAttribDC, textCursor, static_cast<int> (nextTabAt - textCursor), &size);
                widthSoFar += size.cx;
            }
        }
        else {
            /*
             *  A bunch of different ways to get the RTL code emitted. Try them each in order (some ifdefed out). When
             *  one succeeds - just to the succcess label.
             */
#if qUseGetCharPlacementToImage && qWideCharacters
            {
                size_t len = nextTabAt - textCursor;
                Memory::SmallStackBuffer<wchar_t> glyphs (len);
                GCP_RESULTSW gcpResult;
                memset (&gcpResult, 0, sizeof (gcpResult));
                gcpResult.lStructSize = sizeof (GCP_RESULTS);
                gcpResult.lpGlyphs = glyphs;
                gcpResult.nGlyphs = static_cast<UINT> (len);
                if (::GetCharacterPlacementW (m_hDC, textCursor, static_cast<int> (len), 0, &gcpResult, GCP_GLYPHSHAPE | GCP_LIGATE) != 0) {
                    Verify (::ExtTextOutW (m_hDC, outputAt.h + widthSoFar - hScrollOffset, outputAt.v, ETO_GLYPH_INDEX, nullptr, gcpResult.lpGlyphs, gcpResult.nGlyphs, nullptr));
                    goto Succeeded_But_Need_To_Adjust_Width;
                }
            }
#endif

#if qUseFakeTTGetWPlacementToImage && qWideCharacters
            {
                size_t len = nextTabAt - textCursor;
                Memory::SmallStackBuffer<wchar_t> glyphs (len);
                if (Win9x_Workaround_GetCharPlacementFunction (m_hDC, textCursor, len, glyphs) != 0) {
                    Verify (::ExtTextOutW (m_hDC, outputAt.h + widthSoFar - hScrollOffset, outputAt.v, ETO_GLYPH_INDEX, nullptr, glyphs, static_cast<UINT> (len), nullptr));
                    goto Succeeded_But_Need_To_Adjust_Width;
                }
            }
#endif

            {
                size_t len = nextTabAt - textCursor;
                // Fallback - if the above fails...
                // Displays the text in the right order, but doesn't do contextual shaping (tested on WinXP and WinME) - LGP 2002-12-10
#if qWideCharacters
                Verify (::ExtTextOutW (m_hDC, outputAt.h + widthSoFar - hScrollOffset, outputAt.v, 0, nullptr, textCursor, static_cast<UINT> (len), nullptr));
#else
                Verify (::ExtTextOutA (m_hDC, outputAt.h + widthSoFar - hScrollOffset, outputAt.v, 0, nullptr, textCursor, static_cast<UINT> (len), nullptr));
#endif
            }

        Succeeded_But_Need_To_Adjust_Width:
            // Geez! There must be SOME API within Win32 to give me this info (like SetTextAlign (UPDATE_CP))
            // without recomputing it. But there doesn't appear to be. So we must recompute!
            // LGP 960503

            // A SLIGHT optimization is now possible here - we don't need to compute the last GetTextExtent() if amountDrawn
            // is nullptr (typically TRUE) - LGP 960521
            if (amountDrawn != nullptr or (nextTabAt < textEnd)) {
                SIZE size;
                Win32_GetTextExtentPoint (m_hAttribDC, textCursor, static_cast<int> (nextTabAt - textCursor), &size);
                widthSoFar += size.cx;
            }
        }
#if qUseUniscribeToImage && qWideCharacters
    Succeeded:;
#endif

        (void)SetBkMode (oldBkMode);
#elif qStroika_FeatureSupported_XWindows
        Led_Point cursor = Led_Point (outputAt.v + precomputedFontMetrics.GetAscent (), outputAt.h - hScrollOffset) - fDrawableOrigin; // ascent - goto baseline...
        XTextItem item;
        memset (&item, 0, sizeof (item));
        item.chars = const_cast<char*> (textCursor);
        item.nchars = nextTabAt - textCursor;
        item.delta = 0;
        item.font = None;
        ::XDrawText (fDisplay, fDrawable, fGC, cursor.h + widthSoFar, cursor.v, &item, 1);
        Led_ThrowIfNull (fCachedFontInfo);
        widthSoFar += ::XTextWidth (const_cast<XFontStruct*> (fCachedFontInfo), item.chars, item.nchars);
#endif

        // Now see if nextTab really pointing at a tab (otherwise at end of buffer)
        if (nextTabAt < textEnd) {
            Led_Distance thisTabWidth;
            {
                Led_Distance curOutputAtZeroBased = (outputAt.h - hTabOrigin) + widthSoFar;
                Led_Distance tabStop              = tabStopList.ComputeTabStopAfterPosition (this, curOutputAtZeroBased);
                thisTabWidth                      = tabStop - curOutputAtZeroBased;
                Assert (thisTabWidth >= 0);
            }

            widthSoFar += thisTabWidth;
            nextTabAt++; // since we processed that tab...
        }
        textCursor = nextTabAt;
    }
    if (amountDrawn != nullptr) {
        *amountDrawn = widthSoFar;
    }
}

void Led_Tablet_::SetBackColor (const Led_Color& backColor)
{
#if qPlatform_MacOS
    SetPort ();
    GDI_RGBBackColor (backColor.GetOSRep ());
#elif qPlatform_Windows
    SetBkColor (backColor.GetOSRep ());
#elif qStroika_FeatureSupported_XWindows
    if (backColor == Led_Color::kWhite) {
        ::XSetBackground (fDisplay, fGC, WhitePixel (fDisplay, DefaultScreen (fDisplay)));
    }
    else if (backColor == Led_Color::kBlack) {
        ::XSetBackground (fDisplay, fGC, BlackPixel (fDisplay, DefaultScreen (fDisplay)));
    }
    else {
        XColor bgColorDef;
        memset (&bgColorDef, 0, sizeof (bgColorDef));
        bgColorDef.red = backColor.GetRed ();
        bgColorDef.green = backColor.GetGreen ();
        bgColorDef.blue = backColor.GetBlue ();
        Colormap cmap = DefaultColormap (fDisplay, DefaultScreen (fDisplay));
        Status s = XAllocColor (fDisplay, cmap, &bgColorDef);
        if (s == 0) {
            ::XSetBackground (fDisplay, fGC, WhitePixel (fDisplay, DefaultScreen (fDisplay)));
        }
        else {
            ::XSetBackground (fDisplay, fGC, bgColorDef.pixel);
        }
    }
#endif
}

void Led_Tablet_::SetForeColor (const Led_Color& foreColor)
{
#if qPlatform_MacOS
    SetPort ();
    GDI_RGBForeColor (foreColor.GetOSRep ());
#elif qPlatform_Windows
    SetTextColor (foreColor.GetOSRep ());
#elif qStroika_FeatureSupported_XWindows
    if (foreColor == Led_Color::kWhite) {
        ::XSetForeground (fDisplay, fGC, WhitePixel (fDisplay, DefaultScreen (fDisplay)));
    }
    else if (foreColor == Led_Color::kBlack) {
        ::XSetForeground (fDisplay, fGC, BlackPixel (fDisplay, DefaultScreen (fDisplay)));
    }
    else {
        XColor fgColorDef;
        memset (&fgColorDef, 0, sizeof (fgColorDef));
        fgColorDef.red = foreColor.GetRed ();
        fgColorDef.green = foreColor.GetGreen ();
        fgColorDef.blue = foreColor.GetBlue ();
        Colormap cmap = DefaultColormap (fDisplay, DefaultScreen (fDisplay));
        Status s = ::XAllocColor (fDisplay, cmap, &fgColorDef);
        if (s == 0) {
            ::XSetForeground (fDisplay, fGC, BlackPixel (fDisplay, DefaultScreen (fDisplay)));
        }
        else {
            ::XSetForeground (fDisplay, fGC, fgColorDef.pixel);
        }
    }
#endif
}

/*
@METHOD:        Led_Tablet_::EraseBackground_SolidHelper
@DESCRIPTION:   <p>EraseBackground_SolidHelper () is simple helper function - usually called from subclasses which OVERRIDE
    @'TextImager::EraseBackground'.</p>
*/
void Led_Tablet_::EraseBackground_SolidHelper (const Led_Rect& eraseRect, const Led_Color& eraseColor)
{
    if (not eraseRect.IsEmpty ()) {
#if qPlatform_MacOS
        SetPort ();
        Rect qdEraser = AsQDRect (eraseRect);
        GDI_RGBForeColor (eraseColor.GetOSRep ());
        ::FillRect (&qdEraser, &Led_Pen::kBlackPattern);
#elif qPlatform_Windows
        Led_Rect eraser = eraseRect;
        Led_Brush backgroundBrush (eraseColor.GetOSRep ());
        Led_Win_Obj_Selector pen (this, ::GetStockObject (NULL_PEN));
        Led_Win_Obj_Selector brush (this, backgroundBrush);
        eraser.right++; // lovely - windows doesn't count last pixel... See Docs for Rectangle() and rephrase!!!
        eraser.bottom++;
        Rectangle (AsRECT (eraser));
#elif qStroika_FeatureSupported_XWindows
        XGCValues prevValues;
        const unsigned long kSavedAttrs = GCForeground;
        Colormap cmap = DefaultColormap (fDisplay, 0);
        XColor fgColorDef;
        memset (&fgColorDef, 0, sizeof (fgColorDef));
        fgColorDef.red = eraseColor.GetRed ();
        fgColorDef.green = eraseColor.GetGreen ();
        fgColorDef.blue = eraseColor.GetBlue ();
        Status s = ::XAllocColor (fDisplay, cmap, &fgColorDef);
        if (s != 0) {
            ::XSetForeground (fDisplay, fGC, fgColorDef.pixel);
        }
        Led_Rect adjustedEraseRect = eraseRect - fDrawableOrigin;
        ::XFillRectangle (fDisplay, fDrawable, fGC, adjustedEraseRect.GetLeft (), adjustedEraseRect.GetTop (), adjustedEraseRect.GetWidth (), adjustedEraseRect.GetHeight ());
        ::XChangeGC (fDisplay, fGC, kSavedAttrs, &prevValues);
#endif
    }
}

/*
@METHOD:        Led_Tablet_::HilightArea_SolidHelper
@DESCRIPTION:   <p>HilightArea_SolidHelper () is simple helper function - usually called from subclasses which OVERRIDE
            @'TextImager::HilightArea'.</p>
                <p>Note the backColor and foreColor are advisory - and maybe ignored if the GDI better supports (or the
            platform UI conventionally calls for) inverting the text via a simple XOR.</p>
                <p>Note also that as of Led 3.1b4, there is new code to properly respect the hilight fore/back colors. This code
            should generally be fast enough, but may - on some hardware - NOT be fast enough. By setting the text color to black,
            the background color to while, and the hilight colors the reverse of this (fore=black/back=white), this code will revert
            to the old algorithm, and run much faster.</p>
*/
void Led_Tablet_::HilightArea_SolidHelper (const Led_Rect& hilightArea, [[maybe_unused]] Led_Color hilightBackColor, [[maybe_unused]] Led_Color hilightForeColor, Led_Color oldBackColor, [[maybe_unused]] Led_Color oldForeColor)
{
    if (not hilightArea.IsEmpty ()) {
#if qPlatform_MacOS
        SetPort ();
        LMSetHiliteMode (LMGetHiliteMode () & 0x7F);
        GDI_RGBBackColor (oldBackColor.GetOSRep ()); // Mac HilightMode code already knows the hilightBackColor - and exchanges it with the given backColor
        //  GDI_RGBForeColor (foreColor.GetOSRep ());       // See IM V-61- docs on "The Hilite Mode".
        Rect qdHiliteRect = AsQDRect (hilightArea);
        ::InvertRect (&qdHiliteRect);
#elif qPlatform_Windows
        /*
         *  SPR#1271 - major reworking using DIB sections etc, to get much better display of hilighted text.
         */
        if (hilightBackColor.GetOSRep () == Led_Color::kBlack.GetOSRep () and
            hilightForeColor.GetOSRep () == Led_Color::kWhite.GetOSRep () and
            oldBackColor.GetOSRep () == Led_Color::kWhite.GetOSRep () and
            oldForeColor.GetOSRep () == Led_Color::kBlack.GetOSRep ()) {
            // This is much faster (on some/most hardware) than the RecolorHelper algorithms. For this special case of of B&W fore/back/hilight
            // colors - this code is MUCH faster as well. So - for people for whom the default algorithm is too slow - they can just specify
            // these colors for hilight and back/fore-color, and they'll get the faster hilight.
            // See SPR#1271
            BitBlt (hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (),
                    this, hilightArea.left, hilightArea.top, DSTINVERT);
        }
        else {
#if 1
            fRecolorHelper = RecolorHelper::CheckCacheAndReconstructIfNeeded (fRecolorHelper,
                                                                              m_hDC, Led_Size (hilightArea.GetHeight (), hilightArea.GetWidth ()),
                                                                              hilightBackColor, hilightForeColor, oldBackColor, oldForeColor);
            fRecolorHelper->DoRecolor (hilightArea);
#else
            static RecolorHelper* recolorHelper = nullptr;
            recolorHelper = RecolorHelper::CheckCacheAndReconstructIfNeeded (recolorHelper,
                                                                             m_hDC, Led_Size (hilightArea.GetHeight (), hilightArea.GetWidth ()),
                                                                             hilightBackColor, hilightForeColor, oldBackColor, oldForeColor);
            recolorHelper->DoRecolor (hilightArea);
#endif
        }
#elif qStroika_FeatureSupported_XWindows
        /*
         *  Quick and dirty primitive version. Should probably take into account backColor/foreColor args.
         *          --  LGP 2001-04-30
         */
        XGCValues prevValues;
        const unsigned long kSavedAttrs = GCFunction | GCForeground | GCBackground;
        (void)::memset (&prevValues, 0, sizeof (prevValues));
        ::XGetGCValues (fDisplay, fGC, kSavedAttrs, &prevValues);
        ::XSetFunction (fDisplay, fGC, GXxor);
        long whiteP = WhitePixel (fDisplay, DefaultScreen (fDisplay));
        long blackP = BlackPixel (fDisplay, DefaultScreen (fDisplay)) ^ whiteP;
        ::XSetBackground (fDisplay, fGC, whiteP);
        ::XSetForeground (fDisplay, fGC, blackP);
        Led_Rect adjustedRect = hilightArea - fDrawableOrigin;
        ::XFillRectangle (fDisplay, fDrawable, fGC, adjustedRect.GetLeft (), adjustedRect.GetTop (), adjustedRect.GetWidth (), adjustedRect.GetHeight ());
        ::XChangeGC (fDisplay, fGC, kSavedAttrs, &prevValues);
#endif
    }
}

/*
@METHOD:        Led_Tablet_::HilightArea_SolidHelper
@DESCRIPTION:   <p>HilightArea_SolidHelper () is simple helper function - usually called from subclasses which OVERRIDE
            @'TextImager::HilightArea'.</p>
                <p>Note the backColor and foreColor are advisory - and maybe ignored if the GDI better supports (or the
            platform UI conventionally calls for) inverting the text via a simple XOR.</p>
*/
void Led_Tablet_::HilightArea_SolidHelper (const Led_Region& hilightArea, [[maybe_unused]] Led_Color hilightBackColor, [[maybe_unused]] Led_Color hilightForeColor, [[maybe_unused]] Led_Color oldBackColor, [[maybe_unused]] Led_Color oldForeColor)
{
    if (not hilightArea.IsEmpty ()) {
#if qPlatform_MacOS
        SetPort ();
        LMSetHiliteMode (LMGetHiliteMode () & 0x7F);
        GDI_RGBBackColor (oldBackColor.GetOSRep ()); // Mac HilightMode code already knows the hilightBackColor - and exchanges it with the given backColor
        //  GDI_RGBForeColor (foreColor.GetOSRep ());       // See IM V-61- docs on "The Hilite Mode".
        ::InvertRgn (hilightArea.GetOSRep ());
#elif qPlatform_Windows
        Assert (false); // probably not hard - bit not totally obvious how todo and since not called yet - ignore for now... LGP 2002-12-03
#elif qStroika_FeatureSupported_XWindows
        Assert (false); // I have no XWin region implementation yet... LGP 2002-12-03
#endif
    }
}

/*
@METHOD:        Led_Tablet_::GetFontMetrics
@DESCRIPTION:   <p>Retrieve the (@'Led_FontMetrics') associated with the current tablet (based on the last SetFont call).</p>
*/
Led_FontMetrics Led_Tablet_::GetFontMetrics () const
{
#if qPlatform_MacOS
    FontInfo fontInfo;
    ::GetFontInfo (&fontInfo);
    return (fontInfo);
#elif qPlatform_Windows
    RequireNotNull (m_hAttribDC);
    TEXTMETRIC tms;
    Verify (::GetTextMetrics (m_hAttribDC, &tms) != 0);
    return tms;
#elif qStroika_FeatureSupported_XWindows
    Led_FontMetrics::PlatformSpecific result;
    memset (&result, 0, sizeof (result));
    Led_ThrowIfNull (fCachedFontInfo);
    result.fAscent = fCachedFontInfo->ascent;
    result.fDescent = fCachedFontInfo->descent;
    result.fLeading = 0; // NOT SURE WHAT THIS IS in X-terminology. Maybe just not supported in XFonts? - LGP 2001-05-07
    result.fMaxCharWidth = fCachedFontInfo->max_bounds.width;
    return result;
#endif
}

#if qStroika_FeatureSupported_XWindows
void Led_Tablet_::SetFont (const Led_FontSpecification& fontSpec)
{
    /*
     * First, see if the XFontStruct* is already cached. If so - all we need todo is (maybe) an XSetFont call.
     */
    {
        map<string, XFontStruct*>::const_iterator i = fFontCache.find (fontSpec.GetOSRep ());
        if (i != fFontCache.end ()) {
            XFontStruct* newFontStruct = i->second;
            if (newFontStruct != fCachedFontInfo) {
                fCachedFontInfo = i->second;
                AssertNotNull (fCachedFontInfo);
                ::XSetFont (fDisplay, fGC, fCachedFontInfo->fid);
            }
            return;
        }
        /*
         *  If a cache miss, then assure cache not too big.
         */
        if (fFontCache.size () >= kMaxFontCacheSize) {
            // remove a random elt
            ::XFreeFont (fDisplay, fFontCache.begin ()->second);
            fFontCache.erase (fFontCache.begin ());
        }
    }

    /*
     *  The font is not already cached. We must try to find it (maybe finding the name in the
     *  fFontMappingCache cache, maybe not.
     */
    fCachedFontInfo = nullptr;
    fCachedFontInfo = ::XLoadQueryFont (fDisplay, fontSpec.GetOSRep ().c_str ());
    if (fCachedFontInfo == nullptr) {
        /*
         *  Look and see if the font is in the cache.
         */
        map<string, string>::const_iterator i = fFontMappingCache.find (fontSpec.GetOSRep ());

        string useFontName;
        if (i != fFontMappingCache.end ()) {
            useFontName = i->second;
            Assert (not useFontName.empty ());
        }
        else {
            // try font-matching algorithm...
            char pointSize[1024];
            (void)::sprintf (pointSize, "%d", fontSpec.GetPointSize () * 10);
            const string kMatchAny  = "*";
            string       tryFontRep = fontSpec.mkOSRep (kMatchAny, fontSpec.GetFontNameSpecifier (), kMatchAny, kMatchAny, kMatchAny);
            int          nFonts     = 0;
            char**       fontList   = ::XListFonts (fDisplay, tryFontRep.c_str (), 100000, &nFonts);
#if qDebugFontDetails
            bool nameMatchFailure = false;
#endif
            if (fontList == nullptr) {
#if qDebugFontDetails
                nameMatchFailure = true;
#endif
                // Try a few name mappings/aliases (apx equal fonts - generalize this!!!)
                if (fontSpec.GetFontNameSpecifier () == "Times New Roman") {
                    tryFontRep = fontSpec.mkOSRep (kMatchAny, "times", kMatchAny, kMatchAny, kMatchAny);
                    fontList   = ::XListFonts (fDisplay, tryFontRep.c_str (), 100000, &nFonts);
                }
            }
            if (fontList == nullptr) {
#if qDebugFontDetails
                nameMatchFailure = true;
#endif
                tryFontRep = fontSpec.mkOSRep (kMatchAny, kMatchAny, kMatchAny, kMatchAny, kMatchAny);
                fontList   = ::XListFonts (fDisplay, tryFontRep.c_str (), 100000, &nFonts);
            }
            Led_ThrowIfNull (fontList);
            vector<string> vFontList;
            {
                vFontList.reserve (nFonts);
                for (size_t i = 0; i < nFonts; ++i) {
                    vFontList.push_back (fontList[i]);
                }
            }
            ::XFreeFontNames (fontList);
            fontList                = nullptr;
            string bestMatchingName = BestMatchFont (fontSpec, vFontList);
#if qDebugFontDetails
            if (nameMatchFailure) {
                fprintf (stderr, "Couldn't find fontName '%s'- using BestMatchSpec = '%s'\r\n", fontSpec.GetFontNameSpecifier ().c_str (), bestMatchingName.c_str ());
            }
#endif
            useFontName = bestMatchingName;
            Assert (not useFontName.empty ());
#if qDebugFontDetails
            fprintf (stderr, "Adding mapping to  fFontMappingCache: '%s'- ==> '%s'\r\n", fontSpec.GetOSRep ().c_str (), useFontName.c_str ());
#endif
            fFontMappingCache.insert (map<string, string>::value_type (fontSpec.GetOSRep (), useFontName));
        }
        fCachedFontInfo = ::XLoadQueryFont (fDisplay, useFontName.c_str ());
        Led_ThrowIfNull (fCachedFontInfo);
    }
    fFontCache.insert (map<string, XFontStruct*>::value_type (fontSpec.GetOSRep (), fCachedFontInfo));
    AssertNotNull (fCachedFontInfo);
    ::XSetFont (fDisplay, fGC, fCachedFontInfo->fid);
}

void Led_Tablet_::SetDrawableOrigin (const Led_Point& origin)
{
    fDrawableOrigin = origin;
}
#endif

#if qStroika_FeatureSupported_XWindows
static bool FontNamesEqual (const string& lhs, const string& rhs)
{
    if (lhs.length () != rhs.length ()) {
        return false;
    }
    for (size_t i = 0; i < lhs.length (); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}
Led_SDK_String Led_Tablet_::BestMatchFont (const Led_FontSpecification& fsp, const vector<Led_SDK_String>& fontsList)
{
    Led_SDK_String bestAnswer;
    float          bestScore    = 0.0f;
    Led_SDK_String fspName      = fsp.GetFontName ();
    int            fspPointSize = fsp.GetPointSize ();
    Led_SDK_String fspWeight    = fsp.GetStyle_Bold () ? "bold" : "medium";
    Led_SDK_String fspItalics   = fsp.GetStyle_Italic () ? "i" : "r";
    const string   kMatchAny    = "*";
    for (auto i = fontsList.begin (); i != fontsList.end (); ++i) {
        Led_SDK_String name;
        Led_SDK_String size;
        Led_SDK_String weight;
        Led_SDK_String slant;
        ParseFontName (*i, &name, &size, &weight, &slant);
        bool rightFontName = (FontNamesEqual (fspName, name));

        float thisScore = 1;
        if (rightFontName) {
            thisScore += 10;
        }
        int thisPointSize = 0;
        if (::sscanf (size.c_str (), "%d", &thisPointSize) == 1) {
            int   pointSizeDiff = abs (thisPointSize - (fspPointSize * 10));
            float scoreAdj      = (100.0f - (pointSizeDiff / 10.0f)) / 10.0f;
            scoreAdj            = max (0.0f, scoreAdj);
            thisScore += scoreAdj;
        }
        if (weight == fspWeight) {
            thisScore += 5.0f;
        }
        if (slant == fspItalics) {
            thisScore += 4.0f;
        }

        if (thisScore > bestScore) {
            bestScore  = thisScore;
            bestAnswer = Led_FontSpecification::mkOSRep (kMatchAny, name, weight, slant, size);
        }
    }
    return bestAnswer;
}

int Led_Tablet_::IgnoreXErrorHandler (Display* /*display*/, XErrorEvent* /*error*/)
{
    return 0;
}

void Led_Tablet_::ParseFontName (const Led_SDK_String& fontName, Led_SDK_String* familyName, Led_SDK_String* fontSize, Led_SDK_String* fontWeight, Led_SDK_String* fontSlant)
{
    RequireNotNull (familyName);
    RequireNotNull (fontSize);
    RequireNotNull (fontWeight);

    Led_SDK_String foundry;
    Led_SDK_String family;
    Led_SDK_String weight;
    Led_SDK_String slant;
    Led_SDK_String setwidth;
    Led_SDK_String pixels;
    Led_SDK_String points;
    Led_SDK_String hRes;
    Led_SDK_String vRes;
    Led_SDK_String spacing;
    Led_SDK_String aveWidth;
    Led_SDK_String charset;

    size_t start = 1;
    size_t end   = fontName.find ('-', start);
    foundry      = fontName.substr (start, end - start);

    start  = end + 1;
    end    = fontName.find ('-', start);
    family = fontName.substr (start, end - start);

    start  = end + 1;
    end    = fontName.find ('-', start);
    weight = fontName.substr (start, end - start);

    start = end + 1;
    end   = fontName.find ('-', start);
    slant = fontName.substr (start, end - start);

    start    = end + 1;
    end      = fontName.find ('-', start);
    setwidth = fontName.substr (start, end - start);

    start          = end + 1;
    end            = fontName.find ('-', start);
    string ignored = fontName.substr (start, end - start);

    start  = end + 1;
    end    = fontName.find ('-', start);
    pixels = fontName.substr (start, end - start);

    start  = end + 1;
    end    = fontName.find ('-', start);
    points = fontName.substr (start, end - start);

    start = end + 1;
    end   = fontName.find ('-', start);
    hRes  = fontName.substr (start, end - start);

    start = end + 1;
    end   = fontName.find ('-', start);
    vRes  = fontName.substr (start, end - start);

    *familyName = family;
    *fontSize   = points;
    *fontWeight = weight;
    *fontSlant  = slant;
}
#endif

/*
 ********************************************************************************
 ***************************** OffscreenTablet::OT ******************************
 ********************************************************************************
 */
#if qPlatform_MacOS
OffscreenTablet::OT::OT (GrafPtr gp)
    : inherited (gp)
{
}
#elif qPlatform_Windows
OffscreenTablet::OT::OT (HDC hdc, Led_Tablet_::OwnDCControl ownsDC)
    : inherited (hdc, ownsDC)
{
}
#elif qStroika_FeatureSupported_XWindows
OffscreenTablet::OT::OT (Display* display, Drawable drawable)
    : inherited (display, drawable)
{
}
#endif

/*
 ********************************************************************************
 ********************************* OffscreenTablet ******************************
 ********************************************************************************
 */
OffscreenTablet::OffscreenTablet ()
    : fOrigTablet (nullptr)
    , fOffscreenRect (Led_Rect (0, 0, 0, 0))
    , fOffscreenTablet (nullptr)
#if qPlatform_MacOS
    , fOrigDevice (nullptr)
    , fOrigPort (nullptr)
    , fOffscreenGWorld (nullptr)
#elif qPlatform_Windows
    , fMemDC ()
    , fMemoryBitmap ()
    , fOldBitmapInDC (nullptr)
#elif qStroika_FeatureSupported_XWindows
    , fPixmap (0)
#endif
{
}

OffscreenTablet::~OffscreenTablet ()
{
#if qPlatform_MacOS
    if (fOrigPort != nullptr) {
        ::SetGWorld (fOrigPort, fOrigDevice); // restore gworld
    }
    if (fOffscreenGWorld != nullptr) {
        ::DisposeGWorld (fOffscreenGWorld);
    }
    delete fOffscreenTablet;
#elif qPlatform_Windows
    if (fOldBitmapInDC != nullptr) {
        (void)fMemDC.SelectObject (fOldBitmapInDC);
    }
#elif qStroika_FeatureSupported_XWindows
    if (fPixmap != 0) {
        ::XFreePixmap (fOrigTablet->fDisplay, fPixmap);
    }
#endif
}

/*
@METHOD:        OffscreenTablet::Setup
@DESCRIPTION:   <p>Prepare a new offscreen drawing environment given the starting basis 'originalTablet' (typically from a window).</p>
                <p>Later call @'OffscreenTablet::PrepareRect' before any actual drawing can be done. This should be called once before
            calling @'OffscreenTablet::PrepareRect'.</p>
*/
void OffscreenTablet::Setup (Led_Tablet origTablet)
{
    Require (fOrigTablet == nullptr); // can only call once.
    RequireNotNull (origTablet);

    fOrigTablet = origTablet;
#if qPlatform_MacOS
    // Save the old gworld info
    Assert (fOrigPort == nullptr);
    Assert (fOrigDevice == nullptr);
    ::GetGWorld (&fOrigPort, &fOrigDevice);

    // Create our gworld (may have to cache this if it turns out to be expensive to re-create...
    Assert (fOffscreenGWorld == nullptr);
    {
        Rect  bounds = AsQDRect (Led_Rect (0, 0, 1, 1)); // size appropriately on a row-by-row basis below...
        OSErr theErr = SafeNewGWorld (&fOffscreenGWorld, 0, &bounds, nullptr, nullptr, noNewDevice | useTempMem);
        if (theErr != noErr) {
            fOffscreenGWorld = nullptr; // no biggie, we just don't use it...
        }
    }
    if (fOffscreenGWorld != nullptr) {
        fOffscreenTablet = new OT (reinterpret_cast<GrafPtr> (fOffscreenGWorld));
    }
#elif qPlatform_Windows
    if (fMemDC.CreateCompatibleDC (fOrigTablet)) {
        fOffscreenTablet = &fMemDC;
    }
#elif qStroika_FeatureSupported_XWindows
    Assert (fPixmap == 0);
// Nothing todo yet - create the pixmap when we know the RowRect.
#endif
}

/*
@METHOD:        OffscreenTablet::PrepareRect
@DESCRIPTION:   <p>Prepare the offscreen drawing environment for the given 'currentRowRect'. This can only be safely called
            after the call to @'OffscreenTablet::Setup' - but can be called multiple times. Note that calls to this
            will typically 'destroy' the bits in the offscreen tablet.</p>
*/
Led_Tablet OffscreenTablet::PrepareRect (const Led_Rect& currentRowRect, Led_Distance extraToAddToBottomOfRect)
{
    Led_Tablet result = fOrigTablet;
#if qPlatform_MacOS
    if (fOffscreenTablet != nullptr) {
        fOffscreenRect = currentRowRect;
        fOffscreenRect.bottom += extraToAddToBottomOfRect;
        Rect bounds = AsQDRect (fOffscreenRect);
        ::OffsetRect (&bounds, -bounds.left, -bounds.top);
#if TARGET_CARBON
        Led_Size curOffscreenGWorldSize;
        {
            Rect junk;
            curOffscreenGWorldSize = AsLedSize (GetRectSize (*::GetPixBounds (::GetPortPixMap (fOffscreenGWorld), &junk)));
        }
#else
        Led_Size curOffscreenGWorldSize = AsLedSize (GetRectSize ((*fOffscreenGWorld->portPixMap)->bounds));
#endif
        if ((fOffscreenRect.GetSize () == curOffscreenGWorldSize) or
            SafeUpdateGWorld (&fOffscreenGWorld, 0, &bounds, nullptr, nullptr, 0) >= 0) {
            AssertNotNull (::GetGWorldPixMap (fOffscreenGWorld));
            if (::LockPixels (::GetGWorldPixMap (fOffscreenGWorld))) {
                // UpdateGWorld () can change grafPortPTR!
                delete fOffscreenTablet;
                fOffscreenTablet = new OT (reinterpret_cast<GrafPtr> (fOffscreenGWorld));
                result           = fOffscreenTablet;
                ::SetGWorld (fOffscreenGWorld, nullptr);
                ::SetOrigin (fOffscreenRect.left, fOffscreenRect.top);
                goto good;
            }
        }
    bad:
        ::SetGWorld (fOrigPort, fOrigDevice); // restore gworld
        if (fOffscreenGWorld != nullptr) {
            ::DisposeGWorld (fOffscreenGWorld);
            fOffscreenGWorld = nullptr;
        }
        delete fOffscreenTablet;
        fOffscreenTablet = nullptr;
    good:;
    }
#elif qPlatform_Windows
    if (fOffscreenTablet != nullptr) {
        fOffscreenRect = currentRowRect;
        fOffscreenRect.bottom += extraToAddToBottomOfRect;
        // See if we need to re-allocate the bitmap
        if (fMemoryBitmap == nullptr or
            (fOffscreenRect.GetSize () != fMemoryBitmap.GetImageSize ())) {
            // deselect our new memory bitmap before changing its size - not sure needed, but lets be paranoid -
            // this is Windows after all ... LGP 960513
            if (fOldBitmapInDC != nullptr) {
                (void)fMemDC.SelectObject (fOldBitmapInDC);
            }
            fMemoryBitmap.DeleteObject (); // lose previous contents, if any...
#if qUseDIBSectionForOffscreenBitmap
            if (fMemoryBitmap.CreateCompatibleDIBSection (Led_Tablet (fOrigTablet)->m_hDC, fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight ()) == 0) {
                fOffscreenTablet = nullptr; // OK, just don't use...
            }
#else
            if (fMemoryBitmap.CreateCompatibleBitmap (Led_Tablet (fOrigTablet)->m_hDC, fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight ()) == 0) {
                fOffscreenTablet = nullptr; // OK, just don't use...
            }
#endif
            if (fOffscreenTablet != nullptr) {
                fOldBitmapInDC = fMemDC.SelectObject (fMemoryBitmap);
                if (fOldBitmapInDC == nullptr) {
                    fOffscreenTablet = nullptr; // OK, just don't use...
                }
            }
        }
        if (fOffscreenTablet != nullptr) {
            result = fOffscreenTablet; // Draw into offscreen bitmap
        }
        if (fOffscreenTablet != nullptr) {
            fMemDC.SetWindowOrg (fOffscreenRect.left, fOffscreenRect.top);
        }
    }
#elif qStroika_FeatureSupported_XWindows
    Led_Size pixmapSize = fOffscreenRect.GetSize ();
    fOffscreenRect = currentRowRect;
    fOffscreenRect.bottom += extraToAddToBottomOfRect;
    if (fPixmap == 0 or pixmapSize != fOffscreenRect.GetSize ()) {
        // Destroy old pixmap, and create new one
        delete fOffscreenTablet;
        fOffscreenTablet = nullptr;
        if (fPixmap != 0) {
            ::XFreePixmap (fOrigTablet->fDisplay, fPixmap);
            fPixmap = 0;
        }
        unsigned int depth = 1; // default - cuz should always be supported
        {
            // Try to get it from the drawable. Only works (I believe) if the drawable is a window.
            XWindowAttributes winAttrs;
            (void)::memset (&winAttrs, 0, sizeof (winAttrs));
            /*
             *  Since we don't know for sure the drawable is a window - catch the error and ignore it. Don't let
             *  XErrorHandler do anything bad.
             */
            int (*oldErrHandler) (Display*, XErrorEvent*) = ::XSetErrorHandler (Led_Tablet_::IgnoreXErrorHandler);
            Status s = ::XGetWindowAttributes (fOrigTablet->fDisplay, fOrigTablet->fDrawable, &winAttrs);
            ::XSetErrorHandler (oldErrHandler);
            if (s == 0) {
                // if call failed - no biggie. Just pick the DefaultDepthOfScreen (could have used XListDepths ()?).
                depth = ::XDefaultDepthOfScreen (::XScreenOfDisplay (fOrigTablet->fDisplay, DefaultScreen (fOrigTablet->fDisplay)));
            }
            else {
                depth = winAttrs.depth;
            }
        }
        fPixmap = ::XCreatePixmap (fOrigTablet->fDisplay, fOrigTablet->fDrawable,
                                   fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight (), depth);
        Assert (fPixmap != 0);
        try {
            fOffscreenTablet = new OT (fOrigTablet->fDisplay, fPixmap);
            fOffscreenTablet->fColormap = fOrigTablet->fColormap;
            fOffscreenTablet->fFontMappingCache = fOrigTablet->fFontMappingCache;
        }
        catch (...) {
            delete fOffscreenTablet;
            fOffscreenTablet = nullptr;
            throw;
        }
    }
    if (fOffscreenTablet != nullptr) {
        fOffscreenTablet->SetDrawableOrigin (fOffscreenRect.GetTopLeft ());
        result = fOffscreenTablet; // Draw into offscreen bitmap
    }
#endif
    return result;
}

/*
@METHOD:        OffscreenTablet::BlastBitmapToOrigTablet
@DESCRIPTION:   <p>Copy the bits which have been saved away into this offscreen tablet back to the original tablet specified in
            @'OffscreenTablet::Setup' and to coordinates specified in the last call to @'OffscreenTablet::PrepareRect'.</p>
*/
void OffscreenTablet::BlastBitmapToOrigTablet ()
{
    if (fOffscreenTablet != nullptr) {
#if qPlatform_MacOS
        Rect bounds = AsQDRect (fOffscreenRect);
        ::SetGWorld (fOrigPort, fOrigDevice); // restore gworld
        GDI_RGBForeColor (Led_Color::kBlack.GetOSRep ());
        GDI_RGBBackColor (Led_Color::kWhite.GetOSRep ());
        GrafPtr tabletGrafPort = *fOffscreenTablet;
#if TARGET_CARBON
        {
            Rect tmp;
            ::CopyBits (::GetPortBitMapForCopyBits (tabletGrafPort), ::GetPortBitMapForCopyBits (fOrigPort), ::GetPortBounds (tabletGrafPort, &tmp), &bounds, srcCopy, nullptr);
        }
#else
        ::CopyBits (&tabletGrafPort->portBits, &((GrafPtr)fOrigPort)->portBits, &tabletGrafPort->portRect, &bounds, srcCopy, nullptr);
#endif
        ::UnlockPixels (::GetGWorldPixMap (fOffscreenGWorld));
#elif qPlatform_Windows
        Led_Tablet screenDC = fOrigTablet;
        screenDC->BitBlt (fOffscreenRect.left, fOffscreenRect.top, fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight (),
                          fOffscreenTablet, fOffscreenRect.left, fOffscreenRect.top, SRCCOPY);
#elif qStroika_FeatureSupported_XWindows
        Assert (fPixmap != 0);
        ::XCopyArea (fOrigTablet->fDisplay, fOffscreenTablet->fDrawable, fOrigTablet->fDrawable, fOrigTablet->fGC,
                     0, 0,
                     fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight (),
                     (int)fOffscreenRect.GetLeft (), (int)fOffscreenRect.GetTop ());
#endif
    }
}

/*
 ********************************************************************************
 ********************************* Led_InstalledFonts ***************************
 ********************************************************************************
 */
Led_InstalledFonts::Led_InstalledFonts (
#if qStroika_FeatureSupported_XWindows
    Display* display,
#endif
    FilterOptions filterOptions)
    : fFilterOptions (filterOptions)
    , fFontNames ()
{
#if qPlatform_Windows
    LOGFONT lf;
    memset (&lf, 0, sizeof (LOGFONT));
    lf.lfCharSet = DEFAULT_CHARSET;
    Led_WindowDC screenDC (nullptr);
    ::EnumFontFamiliesEx (screenDC.m_hDC, &lf, (FONTENUMPROC)FontFamilyAdderProc, reinterpret_cast<LPARAM> (this), 0);
    sort (fFontNames.begin (), fFontNames.end ());
    vector<Led_SDK_String>::iterator rest = unique (fFontNames.begin (), fFontNames.end ());
    fFontNames.erase (rest, fFontNames.end ()); // remove the duplicates
#elif qStroika_FeatureSupported_XWindows
    int fontListSize = 0;
    char** fontList = ::XListFonts (display, "*", 200000, &fontListSize);
    set<string> fontNames;
    for (int i = 0; i < fontListSize; ++i) {
        string longFontName = fontList[i];
        string tmp = longFontName;
        if (tmp.length () > 0 and tmp[0] == '-') {
            size_t nextDash = tmp.find ('-', 1);
            if (nextDash != string::npos and nextDash > 1) {
                tmp = tmp.substr (nextDash + 1);
            }
            nextDash = tmp.find ('-'); // OK - even if end of string
            string fontFamilyName = tmp.substr (0, nextDash);
            if (not fontFamilyName.empty ()) {
                fontNames.insert (fontFamilyName);
            }
        }
    }
    ::XFreeFontNames (fontList);
    fontList = nullptr;
    fFontNames = vector<string> (fontNames.begin (), fontNames.end ());
#else
    Assert (false); // NYI for other platforms
#endif
}

#if qPlatform_Windows
BOOL FAR PASCAL Led_InstalledFonts::FontFamilyAdderProc (ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int fontType, LPVOID pThis)
{
    Led_InstalledFonts* thisP = reinterpret_cast<Led_InstalledFonts*> (pThis);

    if (thisP->fFilterOptions & eSkipRasterFonts) {
        // don't put in non-printer raster fonts (cuz WordPad doesn't and CFontDialog doesn't appear to -
        // LGP 971215)
        if (fontType & RASTER_FONTTYPE)
            return 1;
    }
    if (thisP->fFilterOptions & eSkipAtSignFonts) {
        if (pelf->elfLogFont.lfFaceName[0] == '@')
            return 1;
    }
    thisP->fFontNames.push_back (pelf->elfLogFont.lfFaceName);
    return 1;
}
#endif

/*
 ********************************************************************************
 ********************************* Led_GDIGlobals *******************************
 ********************************************************************************
 */

Led_GDIGlobals* Led_GDIGlobals::sThe = nullptr;

// Somewhat silly hack so Led_GDIGlobals gets destroyed at end of application execution. Helpful for quitting memleak detectors.
class Led_GDIGlobals::_Global_DESTRUCTOR_ {
public:
    ~_Global_DESTRUCTOR_ ()
    {
        delete (Led_GDIGlobals::sThe);
        Led_GDIGlobals::sThe = nullptr;
    }
} sTheLed_GDIGlobalsDESTRUCTOR_;

Led_GDIGlobals::Led_GDIGlobals ()
    : fLogPixelsH (0)
    , fLogPixelsV (0)
{
    InvalidateGlobals ();
}

void Led_GDIGlobals::InvalidateGlobals ()
{
// From the name, it would appear we invalidated, and re-validate later. But I think this implematnion is a bit
// simpler, and should perform fine given its expected usage.
#if qPlatform_MacOS
    fLogPixelsH = 72;
    fLogPixelsV = 72;
#elif qPlatform_Windows
    Led_WindowDC screenDC (nullptr);
    fLogPixelsH = ::GetDeviceCaps (screenDC, LOGPIXELSX);
    fLogPixelsV = ::GetDeviceCaps (screenDC, LOGPIXELSY);
#elif qStroika_FeatureSupported_XWindows
    /*
     *  Either 75 or 100??? Not sure which is best
     *
     *  AbiWord has comments (in gr_UnixGraphix.cpp) that though most X-Servers return a resolution of 75, 100 seems to
     *  look best. I tried on XWinPro 5.1 (a Win32-based X-Server) and on the one that comes with RedHat Linux 6.1, and both
     *  looked better when I set this to 100. So try that for now...
     */
    //const int kResToUse   =   75;
    const int kResToUse = 100;
    fLogPixelsH = kResToUse;
    fLogPixelsV = kResToUse;
#endif
}

/*
 ********************************************************************************
 ************************************ AddRectangleToRegion **********************
 ********************************************************************************
 */
void Led::AddRectangleToRegion (Led_Rect addRect, Led_Region* toRgn)
{
    RequireNotNull (toRgn);
    *toRgn = *toRgn + Led_Region (addRect);
}

/*
 ********************************************************************************
 ********************************* Led_GetDIBImageSize **************************
 ********************************************************************************
 */

/*
@METHOD:        Led_GetDIBImageSize
@DESCRIPTION:   <p>Return the size in pixels of the given argument DIB</p>
*/
Led_Size Led::Led_GetDIBImageSize (const Led_DIB* dib)
{
    RequireNotNull (dib);
    Assert (sizeof (BITMAPINFOHEADER) == 40); // just to make sure we have these defined right on other platforms
    Assert (sizeof (BITMAPCOREHEADER) == 12); // ''
    Assert (sizeof (RGBTRIPLE) == 3);         // ''

    if (IS_WIN30_DIB (dib)) {
        const BITMAPINFOHEADER& hdr = dib->bmiHeader;
        return (Led_Size (abs (Led_ByteSwapFromWindows (hdr.biHeight)), abs (Led_ByteSwapFromWindows (hdr.biWidth))));
    }
    else {
        const BITMAPCOREHEADER& hdr = *(reinterpret_cast<const BITMAPCOREHEADER*> (dib));
        return (Led_Size (Led_ByteSwapFromWindows (hdr.bcHeight), Led_ByteSwapFromWindows (hdr.bcWidth)));
    }
}

/*
 ********************************************************************************
 *************************** Led_GetDIBPalletByteCount **************************
 ********************************************************************************
 */
size_t Led::Led_GetDIBPalletByteCount (const Led_DIB* dib)
{
    RequireNotNull (dib);
    /*
     *  Logic from MSFT DibLook sample in MSVC.Net 2003, plus:
     *      MSVC.Net 2003 SDK docs mention this case - that:
     *          BI_BITFIELDS:   Specifies that the bitmap is not compressed and that the
     *                          color table consists of three DWORD color masks that specify
     *                          the red, green, and blue components, respectively, of each pixel.
     *                          This is valid when used with 16- and 32-bpp bitmaps
     */
    if (IS_WIN30_DIB (dib)) {
        size_t                  byteCount = DIBNumColors (dib) * sizeof (RGBQUAD);
        const BITMAPINFOHEADER& hdr       = dib->bmiHeader;
        //unsigned short          bitCount  = Led_ByteSwapFromWindows (hdr.biBitCount);
        if (Led_ByteSwapFromWindows (hdr.biCompression) == BI_BITFIELDS) {
#if qPlatform_Windows
            Assert (sizeof (DWORD) == sizeof (unsigned int));
#endif
            Assert (4 == sizeof (unsigned int));
            byteCount += 3 * sizeof (unsigned int);
        }
        return byteCount;
    }
    else {
        Assert (sizeof (RGBTRIPLE) == 3); // make sure we have this defined right on each platform
        return (DIBNumColors (dib) * sizeof (RGBTRIPLE));
    }
}

/*
 ********************************************************************************
 ************************* Led_GetDIBImageRowByteCount **************************
 ********************************************************************************
 */
/*
@METHOD:        Led_GetDIBImageRowByteCount
@DESCRIPTION:   <p>Return the size in bytes of a single ROW of pixels in the given argument DIB.</p>
*/
size_t Led::Led_GetDIBImageRowByteCount (const Led_DIB* dib)
{
    RequireNotNull (dib);
    Led_Size                imageSize = Led_GetDIBImageSize (dib);
    const BITMAPINFOHEADER& hdr       = dib->bmiHeader;

    unsigned short bitCount = Led_ByteSwapFromWindows (hdr.biBitCount);
    return (((imageSize.h * bitCount + 31) & ~31) >> 3);
}

/*
 ********************************************************************************
 *************************** Led_GetDIBImageByteCount ***************************
 ********************************************************************************
 */
/*
@METHOD:        Led_GetDIBImageByteCount
@DESCRIPTION:   <p>Return the size in bytes of the given argument DIB. DIBs are contiguous chunks of RAM.</p>
*/
size_t Led::Led_GetDIBImageByteCount (const Led_DIB* dib)
{
    RequireNotNull (dib);
    Led_Size                imageSize = Led_GetDIBImageSize (dib);
    const BITMAPINFOHEADER& hdr       = dib->bmiHeader;
    size_t                  byteCount = Led_ByteSwapFromWindows (hdr.biSize);

    byteCount += Led_GetDIBPalletByteCount (dib);

    unsigned long imageByteSize = Led_ByteSwapFromWindows (hdr.biSizeImage);
    if (imageByteSize == 0) {
        unsigned short bitCount = Led_ByteSwapFromWindows (hdr.biBitCount);
        // often zero for uncompressed images, so we compute ourselves...
        //imageByteSize = imageSize.v * ((imageSize.h * bitCount + 31)/32)*4;
        imageByteSize = imageSize.v * (((imageSize.h * bitCount + 31) & ~31) >> 3);
    }
    byteCount += imageByteSize;
    return byteCount;
}

/*
 ********************************************************************************
 ********************************* Led_CloneDIB *********************************
 ********************************************************************************
 */
/*
@METHOD:        Led_CloneDIB
@DESCRIPTION:   <p>Make a copy of the given @'Led_DIB' object using ::operator new (). Just use normal C++ ::operator delete ()
            to destroy the result.</p>
*/
Led_DIB* Led::Led_CloneDIB (const Led_DIB* dib)
{
    RequireNotNull (dib);
    size_t   nBytes = Led_GetDIBImageByteCount (dib);
    Led_DIB* newDIB = reinterpret_cast<Led_DIB*> (new char[nBytes]);
    (void)::memcpy (newDIB, dib, nBytes);
    return newDIB;
}

/*
 ********************************************************************************
 ***************************** Led_GetDIBBitsPointer ****************************
 ********************************************************************************
 */
/*
@METHOD:        Led_GetDIBBitsPointer
@DESCRIPTION:   <p></p>
*/
const void* Led::Led_GetDIBBitsPointer (const Led_DIB* dib)
{
    RequireNotNull (dib);
    const BITMAPINFOHEADER& hdr = dib->bmiHeader;
    return reinterpret_cast<const char*> (dib) +
           Led_ByteSwapFromWindows (hdr.biSize) +
           Led_GetDIBPalletByteCount (dib);
}

#if qPlatform_Windows
/*
 ********************************************************************************
 ****************************** Led_DIBFromHBITMAP ******************************
 ********************************************************************************
 */
Led_DIB* Led::Led_DIBFromHBITMAP (HDC hDC, HBITMAP hbm)
{
    RequireNotNull (hbm);
    BITMAP bm;
    Verify (::GetObject (hbm, sizeof (BITMAP), (LPVOID)&bm));

    Led_DIB* dibResult = nullptr;
    {
        BITMAPINFOHEADER bmiHdr;
        memset (&bmiHdr, 0, sizeof (bmiHdr));
        bmiHdr.biSize        = sizeof (BITMAPINFOHEADER);
        bmiHdr.biWidth       = bm.bmWidth;
        bmiHdr.biHeight      = bm.bmHeight;
        bmiHdr.biPlanes      = 1;
        bmiHdr.biBitCount    = 24;
        bmiHdr.biCompression = BI_RGB;
        bmiHdr.biSizeImage   = ((((bmiHdr.biWidth * bmiHdr.biBitCount) + 31) & ~31) >> 3) * bmiHdr.biHeight;
        size_t nBytes        = Led_GetDIBImageByteCount (reinterpret_cast<Led_DIB*> (&bmiHdr));
        dibResult            = reinterpret_cast<Led_DIB*> (new char[nBytes]);
        Assert (nBytes > sizeof (BITMAPINFOHEADER));
        DISABLE_COMPILER_MSC_WARNING_START (6386)
        (void)::memcpy (dibResult, &bmiHdr, sizeof (bmiHdr));
        DISABLE_COMPILER_MSC_WARNING_END (6386)
    }

    [[maybe_unused]] int nScanLinesCopied = ::GetDIBits (hDC, hbm, 0, dibResult->bmiHeader.biHeight, reinterpret_cast<char*> (dibResult) + Led_GetDIBPalletByteCount (dibResult) + sizeof (BITMAPINFOHEADER), dibResult, DIB_RGB_COLORS);
    Assert (nScanLinesCopied == dibResult->bmiHeader.biHeight);
    return dibResult;
}
#endif

#if qProvideIMESupport
#include <ime.h>

/*
 ********************************************************************************
 ************************************** Led_IME *********************************
 ********************************************************************************
 */
Led_IME* Led_IME::sThe = nullptr;

#ifndef qUseNewIMECode
#define qUseNewIMECode 1
#endif

// Somewhat silly hack so Led_IME gets destroyed at end of application execution. Helpful for quitting memleak detectors.
class Led_IME::_Global_DESTRUCTOR_ {
public:
    ~_Global_DESTRUCTOR_ ()
    {
        delete (Led_IME::sThe);
        Led_IME::sThe = nullptr;
    }
} sTheLed_IME_DESTRUCTOR_;

Led_IME::Led_IME ()
    : fSendIMEMessageProc (nullptr)
    , fIMEEnableProc (nullptr)
    , fImmGetContext (nullptr)
    , fImmSetCompositionFont (nullptr)
    , fImmReleaseContext (nullptr)
    , fImmGetCompositionStringW (nullptr)
    , fImmSetCompositionWindow (nullptr)
    , fImmSetOpenStatus (nullptr)
    , fWinNlsAvailable (false)
    , fLastX (-1)
    , fLastY (-1)
{
    Assert (sThe == nullptr);
    sThe = this;

#ifdef _UNICODE
    const char IMEPROCNAME[] = "SendIMEMessageExW";
#else
    const char IMEPROCNAME[] = "SendIMEMessageExA";
#endif
    HINSTANCE hNLS = ::GetModuleHandle (_T ("USER32.DLL"));
    if (hNLS != nullptr) {
        fSendIMEMessageProc = (LRESULT (FAR PASCAL*) (HWND, DWORD))::GetProcAddress (hNLS, IMEPROCNAME);
        fIMEEnableProc      = (BOOL (FAR PASCAL*) (HWND, BOOL))::GetProcAddress (hNLS, "WINNLSEnableIME");
    }
    fWinNlsAvailable = fSendIMEMessageProc != nullptr and fIMEEnableProc != nullptr;

    HINSTANCE hIMM = ::GetModuleHandle (_T ("IMM32.DLL"));
    if (hIMM != nullptr) {
#ifdef _UNICODE
        constexpr char ImmSetCompositionFontNAME[] = "ImmSetCompositionFontW";
#else
        constexpr char ImmSetCompositionFontNAME[] = "ImmSetCompositionFontA";
#endif
        fImmGetContext            = (HIMC (FAR PASCAL*) (HWND))::GetProcAddress (hIMM, "ImmGetContext");
        fImmSetCompositionFont    = (BOOL (FAR PASCAL*) (HIMC, const LOGFONT*))::GetProcAddress (hIMM, ImmSetCompositionFontNAME);
        fImmReleaseContext        = (BOOL (FAR PASCAL*) (HWND, HIMC))::GetProcAddress (hIMM, "ImmReleaseContext");
        fImmGetCompositionStringW = (LONG (FAR PASCAL*) (HIMC, DWORD, LPVOID, DWORD))::GetProcAddress (hIMM, "ImmGetCompositionStringW");
        fImmSetCompositionWindow  = (BOOL (FAR PASCAL*) (HIMC, const void*))::GetProcAddress (hIMM, "ImmSetCompositionWindow");
        fImmSetOpenStatus         = (BOOL (FAR PASCAL*) (HIMC, BOOL))::GetProcAddress (hIMM, "ImmSetOpenStatus");
    }
}

void Led_IME::NotifyPosition (HWND hWnd, const SHORT x, const SHORT y)
{
    if (x != fLastX || y != fLastY) {
        UpdatePosition (hWnd, x, y);
    }
}

void Led_IME::NotifyOfFontChange (HWND hWnd, const LOGFONT& lf)
{
    if (fImmGetContext != nullptr and fImmSetCompositionFont != nullptr and fImmReleaseContext != nullptr) {
        HIMC hImc = NULL;
        if ((hImc = fImmGetContext (hWnd)) != NULL) {
            fImmSetCompositionFont (hImc, &lf);
            fImmReleaseContext (hWnd, hImc);
        }
    }
}

#if !qUseNewIMECode
void Led_IME::SendSimpleMessage (HWND hWnd, UINT fnc, WPARAM wParam)
{
    if (fSendIMEMessageProc != nullptr) {
        HANDLE      hime = ::GlobalAlloc (GMEM_MOVEABLE | GMEM_LOWER | GMEM_DDESHARE, (DWORD)sizeof (IMESTRUCT));
        LPIMESTRUCT lpime;
        if (hime)
            lpime = (LPIMESTRUCT)GlobalLock (hime);
        else
            return;

        if (lpime == nullptr) {
            GlobalFree (hime);
            return;
        }
        lpime->fnc    = fnc;
        lpime->wParam = wParam;
        fSendIMEMessageProc (hWnd, (LONG)hime);
        wParam = lpime->wParam;
        ::GlobalUnlock (hime);
        ::GlobalFree (hime);
    }
}
#endif

void Led_IME::IMEOn (HWND hWnd)
{
#if qUseNewIMECode
    if (fImmGetContext != nullptr and fImmSetOpenStatus != nullptr and fImmReleaseContext != nullptr) {
        HIMC hImc = NULL;
        if ((hImc = fImmGetContext (hWnd)) != NULL) {
            Verify (fImmSetOpenStatus (hImc, true));
            fImmReleaseContext (hWnd, hImc);
        }
    }
#else
    SendSimpleMessage (hWnd, IME_SETOPEN, 1);
#endif
}

void Led_IME::IMEOff (HWND hWnd)
{
#if qUseNewIMECode
    if (fImmGetContext != nullptr and fImmSetOpenStatus != nullptr and fImmReleaseContext != nullptr) {
        HIMC hImc = NULL;
        if ((hImc = fImmGetContext (hWnd)) != NULL) {
            Verify (fImmSetOpenStatus (hImc, false));
            fImmReleaseContext (hWnd, hImc);
        }
    }
#else
    SendSimpleMessage (hWnd, IME_SETOPEN, 0);
#endif
}

void Led_IME::UpdatePosition (const HWND hWnd, const SHORT x, const SHORT y)
{
    if (fSendIMEMessageProc != nullptr) {
#if qUseNewIMECode
        if (fImmGetContext != nullptr and fImmSetCompositionWindow != nullptr and fImmReleaseContext != nullptr) {
            HIMC hImc = NULL;
            if ((hImc = fImmGetContext (hWnd)) != NULL) {
                COMPOSITIONFORM compForm;
                memset (&compForm, 0, sizeof (compForm));
                compForm.dwStyle        = CFS_FORCE_POSITION;
                compForm.ptCurrentPos.x = x;
                compForm.ptCurrentPos.y = y;
                Verify (fImmSetCompositionWindow (hImc, &compForm));
                fImmReleaseContext (hWnd, hImc);
                fLastX = x;
                fLastY = y;
            }
        }
#else
        HANDLE      hime  = ::GlobalAlloc (GMEM_MOVEABLE | GMEM_LOWER | GMEM_DDESHARE, (DWORD)sizeof (IMESTRUCT));
        LPIMESTRUCT lpime = nullptr;
        if (hime != nullptr) {
            lpime = (LPIMESTRUCT)GlobalLock (hime);
        }
        else {
            return;
        }

        if (lpime == nullptr) {
            return;
        }

        lpime->fnc       = IME_SETCONVERSIONWINDOW; // called IME_MOVECONVERTWINDOW in Win3.1
        lpime->wParam    = MCW_WINDOW;
        lpime->wCount    = 0;
        lpime->dchSource = 0;
        lpime->dchDest   = 0;
        lpime->lParam1   = MAKELONG (x, y);
        lpime->lParam2   = 0L;
        lpime->lParam3   = 0L;

        // SendIMEMessageProc returns 0 if there is an error, in which case
        // the error code is returned in IMESTRUCT.wParam;
        short ret = fSendIMEMessageProc (hWnd, (LONG)hime);
        ret       = ret ? 0 : (short)lpime->wParam;

        ::GlobalUnlock (hime);
        ::GlobalFree (hime);
        if (!ret) {
            fLastX = x;
            fLastY = y;
        }
#endif

        // Should I redo this taking the LOGFONT as an arg to Led_IME::UpdatePosition ()??? LGP 980714
        if (fImmGetContext != nullptr and fImmSetCompositionFont != nullptr and fImmReleaseContext != nullptr) {
            HFONT hFont = nullptr;
            if ((hFont = (HFONT)::SendMessage (hWnd, WM_GETFONT, 0, 0L)) != nullptr) {
                LOGFONT lFont;
                if (::GetObject (hFont, sizeof (LOGFONT), &lFont)) {
                    HIMC hImc = NULL;
                    if ((hImc = fImmGetContext (hWnd)) != NULL) {
                        fImmSetCompositionFont (hImc, &lFont);
                        fImmReleaseContext (hWnd, hImc);
                    }
                }
            }
        }
    }
}

wstring Led_IME::GetCompositionResultStringW (HWND hWnd)
{
    wstring result;
    if (fImmGetCompositionStringW != nullptr and fImmGetContext != nullptr and fImmReleaseContext != nullptr) {
        HIMC hImc = 0;
        if ((hImc = fImmGetContext (hWnd)) != 0) {
            wchar_t curIMEString[2048];
            LONG    nChars = fImmGetCompositionStringW (hImc, GCS_RESULTSTR, curIMEString, static_cast<DWORD> (NEltsOf (curIMEString)));

            nChars /= sizeof (wchar_t); // why???? LGP 991214
            if (nChars >= 0 and static_cast<size_t> (nChars) < NEltsOf (curIMEString)) {
                curIMEString[nChars] = '\0';
            }
            else {
                curIMEString[0] = '\0';
            }
            result = curIMEString;
            fImmReleaseContext (hWnd, hImc);
        }
    }
    return result;
}
#endif

Led_Rect Led::CenterRectInRect (const Led_Rect& r, const Led_Rect& centerIn)
{
    Led_Coordinate xLeft = (centerIn.left + centerIn.right) / 2 - r.GetWidth () / 2;
    Led_Coordinate yTop  = (centerIn.top + centerIn.bottom) / 2 - r.GetHeight () / 2;
    return Led_Rect (yTop, xLeft, r.GetHeight (), r.GetWidth ());
}

#if qPlatform_Windows
void Led::Led_CenterWindowInParent (HWND w)
{
    Assert (::IsWindow (w));
    HWND hWndCenter = ::GetWindow (w, GW_OWNER);
    if (hWndCenter == nullptr) {
        hWndCenter = ::GetDesktopWindow ();
    }
    Assert (::IsWindow (hWndCenter));

    // get coordinates of the window relative to its parent
    RECT rcDlg;
    ::GetWindowRect (w, &rcDlg);
    RECT rcCenter;
    ::GetWindowRect (hWndCenter, &rcCenter);

    // find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.left + rcCenter.right) / 2 - AsLedRect (rcDlg).GetWidth () / 2;
    int yTop  = (rcCenter.top + rcCenter.bottom) / 2 - AsLedRect (rcDlg).GetHeight () / 2;

    // map screen coordinates to child coordinates
    ::SetWindowPos (w, nullptr, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
#endif

////////////////////////////// PRIVATE UTILITIES

#if qPlatform_Windows && qWideCharacters && qUseFakeTTGetWPlacementToImage

///////////////////////////////////////////////////////////////////////////////////
////////////// CODE FROM Microsoft Knowledge Base Article - 241020  ///////////////
///////////////////////////////////////////////////////////////////////////////////
///////// http://support.microsoft.com/default.aspx?scid=kb;en-us;241020 //////////
///////////////////////////////////////////////////////////////////////////////////

#pragma pack(1) // for byte alignment
// We need byte alignment to be structure compatible with the
// contents of a TrueType font file

// Macros to swap from Big Endian to Little Endian
#define SWAPWORD(x) MAKEWORD ( \
    HIBYTE (x),                \
    LOBYTE (x))
#define SWAPLONG(x) MAKELONG ( \
    SWAPWORD (HIWORD (x)),     \
    SWAPWORD (LOWORD (x)))

struct CMAP4 {            // From the TrueType Spec. revision 1.66
    USHORT format;        // Format number is set to 4.
    USHORT length;        // Length in bytes.
    USHORT version;       // Version number (starts at 0).
    USHORT segCountX2;    // 2 x segCount.
    USHORT searchRange;   // 2 x (2**floor(log2(segCount)))
    USHORT entrySelector; // log2(searchRange/2)
    USHORT rangeShift;    // 2 x segCount - searchRange

    USHORT Arrays[1]; // Placeholder symbol for address of arrays following
};
using LPCMAP4 = CMAP4*;

/*  CMAP table Data
    From the TrueType Spec revision 1.66

    USHORT  Table Version #
    USHORT  Number of encoding tables
*/
#define CMAPHEADERSIZE (sizeof (USHORT) * 2)

/*  ENCODING entry Data aka CMAPENCODING
    From the TrueType Spec revision 1.66

    USHORT  Platform Id
    USHORT  Platform Specific Encoding Id
    ULONG   Byte Offset from beginning of table
*/
#define ENCODINGSIZE (sizeof (USHORT) * 2 + sizeof (ULONG))

struct CMAPENCODING {
    USHORT PlatformId;
    USHORT EncodingId;
    ULONG  Offset;
};

// Macro to pack a TrueType table name into a DWORD
#define MAKETABLENAME(ch1, ch2, ch3, ch4) ( \
    (((DWORD) (ch4)) << 24) |               \
    (((DWORD) (ch3)) << 16) |               \
    (((DWORD) (ch2)) << 8) |                \
    ((DWORD) (ch1)))

/* public functions */
static USHORT GetTTUnicodeGlyphIndex (HDC hdc, USHORT ch);

// DWORD packed four letter table name for each GetFontData()
// function call when working with the CMAP TrueType table
static DWORD dwCmapName = MAKETABLENAME ('c', 'm', 'a', 'p');

static USHORT* GetEndCountArray (LPBYTE pBuff)
{
    return (USHORT*)(pBuff + 7 * sizeof (USHORT)); // Per TT spec
}

static USHORT* GetStartCountArray (LPBYTE pBuff)
{
    DWORD segCount = ((LPCMAP4)pBuff)->segCountX2 / 2;
    return (USHORT*)(pBuff +
                     8 * sizeof (USHORT) +        // 7 header + 1 reserved USHORT
                     segCount * sizeof (USHORT)); // Per TT spec
}

static USHORT* GetIdDeltaArray (LPBYTE pBuff)
{
    DWORD segCount = ((LPCMAP4)pBuff)->segCountX2 / 2;
    return (USHORT*)(pBuff +
                     8 * sizeof (USHORT) +            // 7 header + 1 reserved USHORT
                     segCount * 2 * sizeof (USHORT)); // Per TT spec
}

static USHORT* GetIdRangeOffsetArray (LPBYTE pBuff)
{
    DWORD segCount = ((LPCMAP4)pBuff)->segCountX2 / 2;
    return (USHORT*)(pBuff +
                     8 * sizeof (USHORT) +            // 7 header + 1 reserved USHORT
                     segCount * 3 * sizeof (USHORT)); // Per TT spec
}

static void SwapArrays (LPCMAP4 pFormat4)
{
    DWORD   segCount = pFormat4->segCountX2 / 2; // Per TT Spec
    DWORD   i;
    USHORT *pGlyphId,
        *pEndOfBuffer,
        *pstartCount    = GetStartCountArray ((LPBYTE)pFormat4),
        *pidDelta       = GetIdDeltaArray ((LPBYTE)pFormat4),
        *pidRangeOffset = GetIdRangeOffsetArray ((LPBYTE)pFormat4),
        *pendCount      = GetEndCountArray ((LPBYTE)pFormat4);

    // Swap the array elements for Intel.
    for (i = 0; i < segCount; i++) {
        pendCount[i]      = SWAPWORD (pendCount[i]);
        pstartCount[i]    = SWAPWORD (pstartCount[i]);
        pidDelta[i]       = SWAPWORD (pidDelta[i]);
        pidRangeOffset[i] = SWAPWORD (pidRangeOffset[i]);
    }

    // Swap the Glyph Id array
    pGlyphId     = pidRangeOffset + segCount; // Per TT spec
    pEndOfBuffer = (USHORT*)((LPBYTE)pFormat4 + pFormat4->length);
    for (; pGlyphId < pEndOfBuffer; pGlyphId++) {
        *pGlyphId = SWAPWORD (*pGlyphId);
    }
} /* end of function SwapArrays */

static BOOL GetFontEncoding (
    HDC           hdc,
    CMAPENCODING* pEncoding,
    int           iEncoding)
/*
    Note for this function to work correctly, structures must
    have byte alignment.
*/
{
    DWORD dwResult;
    BOOL  fSuccess = TRUE;

    // Get the structure data from the TrueType font
    dwResult = GetFontData (
        hdc,
        dwCmapName,
        CMAPHEADERSIZE + ENCODINGSIZE * iEncoding,
        pEncoding,
        sizeof (CMAPENCODING));
    fSuccess = (dwResult == sizeof (CMAPENCODING));

    // swap the Platform Id for Intel
    pEncoding->PlatformId = SWAPWORD (pEncoding->PlatformId);

    // swap the Specific Id for Intel
    pEncoding->EncodingId = SWAPWORD (pEncoding->EncodingId);

    // swap the subtable offset for Intel
    pEncoding->Offset = SWAPLONG (pEncoding->Offset);

    return fSuccess;

} /* end of function GetFontEncoding */

static BOOL GetFontFormat4Header (
    HDC     hdc,
    LPCMAP4 pFormat4,
    DWORD   dwOffset)
/*
    Note for this function to work correctly, structures must
    have byte alignment.
*/
{
    BOOL    fSuccess = TRUE;
    DWORD   dwResult;
    int     i;
    USHORT* pField;

    // Loop and Alias a writeable pointer to the field of interest
    pField = (USHORT*)pFormat4;

    for (i = 0; i < 7; i++) {
        // Get the field from the subtable
        dwResult = GetFontData (
            hdc,
            dwCmapName,
            dwOffset + sizeof (USHORT) * i,
            pField,
            sizeof (USHORT));

        // swap it to make it right for Intel.
        *pField = SWAPWORD (*pField);
        // move on to the next
        pField++;
        // accumulate our success
        fSuccess = (dwResult == sizeof (USHORT)) && fSuccess;
    }

    return fSuccess;

} /* end of function GetFontFormat4Header */

static BOOL GetFontFormat4Subtable (
    HDC     hdc,              // DC with TrueType font
    LPCMAP4 pFormat4Subtable, // destination buffer
    DWORD   dwOffset          // Offset within font
)
{
    DWORD  dwResult;
    USHORT length;

    // Retrieve the header values in swapped order
    if (!GetFontFormat4Header (hdc,
                               pFormat4Subtable,
                               dwOffset)) {
        return FALSE;
    }

    // Get the rest of the table
    length   = pFormat4Subtable->length - (7 * sizeof (USHORT));
    dwResult = GetFontData (hdc,
                            dwCmapName,
                            dwOffset + 7 * sizeof (USHORT),   // pos of arrays
                            (LPBYTE)pFormat4Subtable->Arrays, // destination
                            length);

    if (dwResult != length) {
        // We really shouldn't ever get here
        return FALSE;
    }

    // Swamp the arrays
    SwapArrays (pFormat4Subtable);

    return TRUE;
}

static BOOL GetTTUnicodeCoverage (
    HDC     hdc,      // DC with TT font
    LPCMAP4 pBuffer,  // Properly allocated buffer
    DWORD   cbSize,   // Size of properly allocated buffer
    DWORD*  pcbNeeded // size of buffer needed
)
/*
    if cbSize is to small or zero, or if pBuffer is nullptr the function
    will fail and return the required buffer size in *pcbNeeded.

    if another error occurs, the function will fail and *pcbNeeded will
    be zero.

    When the function succeeds, *pcbNeeded contains the number of bytes
    copied to pBuffer.
*/
{
    USHORT       nEncodings; // # of encoding in the TT font
    CMAPENCODING Encoding{}; // The current encoding
    DWORD        dwResult;
    DWORD        i,
        iUnicode;             // The Unicode encoding
    CMAP4   Format4;          // Unicode subtable format
    LPCMAP4 pFormat4Subtable; // Working buffer for subtable

    // Get the number of subtables in the CMAP table from the CMAP header
    // The # of subtables is the second USHORT in the CMAP table, per the TT Spec.
    dwResult   = GetFontData (hdc, dwCmapName, sizeof (USHORT), &nEncodings, sizeof (USHORT));
    nEncodings = SWAPWORD (nEncodings);

    if (dwResult != sizeof (USHORT)) {
        // Something is wrong, we probably got GDI_ERROR back
        // Probably this means that the Device Context does not have
        // a TrueType font selected into it.
        return FALSE;
    }

    // Get the encodings and look for a Unicode Encoding
    iUnicode = nEncodings;
    for (i = 0; i < nEncodings; i++) {
        // Get the encoding entry for each encoding
        if (!GetFontEncoding (hdc, &Encoding, i)) {
            *pcbNeeded = 0;
            return FALSE;
        }

        // Take note of the Unicode encoding.
        //
        // A Unicode encoding per the TrueType specification has a
        // Platform Id of 3 and a Platform specific encoding id of 1
        // Note that Symbol fonts are supposed to have a Platform Id of 3
        // and a specific id of 0. If the TrueType spec. suggestions were
        // followed then the Symbol font's Format 4 encoding could also
        // be considered Unicode because the mapping would be in the
        // Private Use Area of Unicode. We assume this here and allow
        // Symbol fonts to be interpreted. If they do not contain a
        // Format 4, we bail later. If they do not have a Unicode
        // character mapping, we'll get wrong results.
        // Code could infer from the coverage whether 3-0 fonts are
        // Unicode or not by examining the segments for placement within
        // the Private Use Area Subrange.
        if (Encoding.PlatformId == 3 &&
            (Encoding.EncodingId == 1 || Encoding.EncodingId == 0)) {
            iUnicode = i; // Set the index to the Unicode encoding
        }
    }

    // index out of range means failure to find a Unicode mapping
    if (iUnicode >= nEncodings) {
        // No Unicode encoding found.
        *pcbNeeded = 0;
        return FALSE;
    }

    // Get the header entries(first 7 USHORTs) for the Unicode encoding.
    if (!GetFontFormat4Header (hdc, &Format4, Encoding.Offset)) {
        *pcbNeeded = 0;
        return FALSE;
    }

    // Check to see if we retrieved a Format 4 table
    if (Format4.format != 4) {
        // Bad, subtable is not format 4, bail.
        // This could happen if the font is corrupt
        // It could also happen if there is a new font format we
        // don't understand.
        *pcbNeeded = 0;
        return FALSE;
    }

    // Figure buffer size and tell caller if buffer to small
    *pcbNeeded = Format4.length;
    if (*pcbNeeded > cbSize || pBuffer == nullptr) {
        // Either test indicates caller needs to know
        // the buffer size and the parameters are not setup
        // to continue.
        return FALSE;
    }

    // allocate a full working buffer
    pFormat4Subtable = (LPCMAP4)malloc (Format4.length);
    if (pFormat4Subtable == nullptr) {
        // Bad things happening if we can't allocate memory
        *pcbNeeded = 0;
        return FALSE;
    }

    // get the entire subtable
    if (!GetFontFormat4Subtable (hdc, pFormat4Subtable, Encoding.Offset)) {
        // Bad things happening if we can't allocate memory
        *pcbNeeded = 0;
        return FALSE;
    }

    // Copy the retrieved table into the buffer
    CopyMemory (pBuffer,
                pFormat4Subtable,
                pFormat4Subtable->length);

    free (pFormat4Subtable);
    return TRUE;
} /* end of function GetTTUnicodeCoverage */

static BOOL FindFormat4Segment (
    LPCMAP4 pTable, // a valid Format4 subtable buffer
    USHORT  ch,     // Unicode character to search for
    USHORT* piSeg   // out: index of segment containing ch
)
/*
    if the Unicode character ch is not contained in one of the
    segments the function returns FALSE.

    if the Unicode character ch is found in a segment, the index
    of the segment is placed in*piSeg and the function returns
    TRUE.
*/
{
    USHORT i,
        segCount        = pTable->segCountX2 / 2;
    USHORT* pendCount   = GetEndCountArray ((LPBYTE)pTable);
    USHORT* pstartCount = GetStartCountArray ((LPBYTE)pTable);

    // Find segment that could contain the Unicode character code
    for (i = 0; i < segCount && pendCount[i] < ch; i++)
        ;

    // We looked in them all, ch not there
    if (i >= segCount)
        return FALSE;

    // character code not within the range of the segment
    if (pstartCount[i] > ch)
        return FALSE;

    // this segment contains the character code
    *piSeg = i;
    return TRUE;
} /* end of function FindFormat4Segment */

static USHORT GetTTUnicodeGlyphIndex (
    HDC    hdc, // DC with a TrueType font selected
    USHORT ch   // Unicode character to convert to Index
)
/*
    When the TrueType font contains a glyph for ch, the
    function returns the glyph index for that character.

    If an error occurs, or there is no glyph for ch, the
    function will return the missing glyph index of zero.
*/
{
    LPCMAP4 pUnicodeCMapTable;
    DWORD   dwSize;
    USHORT  iSegment;
    USHORT* idRangeOffset;
    USHORT* idDelta;
    USHORT* startCount;
    USHORT  GlyphIndex = 0; // Initialize to missing glyph

    // How big a buffer do we need for Unicode CMAP?
    GetTTUnicodeCoverage (hdc, nullptr, 0, &dwSize);
    pUnicodeCMapTable = (LPCMAP4)malloc (dwSize);
    if (!GetTTUnicodeCoverage (hdc, pUnicodeCMapTable, dwSize, &dwSize)) {
        // Either no Unicode cmap, or some other error occurred
        // like font in DC is not TT.
        free (pUnicodeCMapTable);
        return 0; // return missing glyph on error
    }

    // Find the cmap segment that has the character code.
    if (!FindFormat4Segment (pUnicodeCMapTable, ch, &iSegment)) {
        free (pUnicodeCMapTable);
        return 0; // ch not in cmap, return missing glyph
    }

    // Get pointers to the cmap data
    idRangeOffset = GetIdRangeOffsetArray ((LPBYTE)pUnicodeCMapTable);
    idDelta       = GetIdDeltaArray ((LPBYTE)pUnicodeCMapTable);
    startCount    = GetStartCountArray ((LPBYTE)pUnicodeCMapTable);

    // Per TT spec, if the RangeOffset is zero,
    if (idRangeOffset[iSegment] == 0) {
        // calculate the glyph index directly
        GlyphIndex = (idDelta[iSegment] + ch) % 65536;
    }
    else {
        // otherwise, use the glyph id array to get the index
        USHORT idResult; //Intermediate id calc.

        idResult = *(
            idRangeOffset[iSegment] / 2 +
            (ch - startCount[iSegment]) +
            &idRangeOffset[iSegment]); // indexing equation from TT spec
        if (idResult)
            // Per TT spec, nonzero means there is a glyph
            GlyphIndex = (idDelta[iSegment] + idResult) % 65536;
        else
            // otherwise, return the missing glyph
            GlyphIndex = 0;
    }

    free (pUnicodeCMapTable);
    return GlyphIndex;
} /* end of function GetTTUnicodeGlyphIndex */

static bool Win9x_Workaround_GetCharPlacementFunction (HDC hdc, const wchar_t* srcText, size_t len, wchar_t* glyphImagesOut)
{
    // Should check if really using a true-type font and as the doc "Microsoft Knowledge Base Article - 241020" says:
    // This sample code was written for clarity of explanation. It is not optimized for repeated use because
    // it allocates and retrieves TrueType tables each time a public function is called. For real applications,
    // a good optimization would be to cache the Unicode encoding for the TrueType font file as long as it remained
    // in the DC. An application can compare to see whether the font selected into a DC is the same TrueType font file
    // by caching and comparing the checksum value of the font file. This checksum is located in the Table Directory
    // of the TrueType font file at the beginning of the file and can be retrieved by using the GetFontData function.
    // See the TrueType specification's discussion of "The Table Directory" under the Data Types chapter to locate
    // the checksum of a font file.
    for (size_t i = 0; i < len; ++i) {
        glyphImagesOut[i] = GetTTUnicodeGlyphIndex (hdc, srcText[i]);
    }
    return true;
}
#endif
