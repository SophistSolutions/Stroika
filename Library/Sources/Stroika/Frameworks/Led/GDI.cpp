/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <algorithm>
#include <cstdio>
#include <set>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

#include "GDI.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

#if qStroika_Platform_Windows
// Often included by <Windows.h> automaticly, but sometimes people define NOIME or VC_EXTRALEAN, and then we
// must include this manaully.
#include <windows.h>

#include <imm.h>
#endif

#if qStroika_Platform_Windows
// RTL Imaging flags
#define qUseGetCharPlacementToImage 1
#endif

#if qStroika_Platform_Windows
/*
 *  Used to use CreateCompatibleBitmap, but as of SPR#1271 try using a DIBSection (of a compatile depth) instead).
 *  This has no noticable effect on normal drawing, but greatly speeds HilightRectangle () code for some computers.
 */
#ifndef qUseDIBSectionForOffscreenBitmap
#define qUseDIBSectionForOffscreenBitmap 1
#endif
#endif

#if qStroika_Platform_Windows
inline bool operator== (PALETTEENTRY lhs, COLORREF rhs)
{
    return RGB (lhs.peRed, lhs.peGreen, lhs.peBlue) == rhs;
}
#endif

#if !qStroika_Frameworks_Led_HaveWindowsDIBDefined
#ifndef BI_BITFIELDS
#define BI_BITFIELDS 3
#endif
#endif

#if qStroika_Platform_Windows
#ifdef _UNICODE
const bool kRunning32BitGDI = true; //  UNICODE only supported on 32GDI (NT or Win2k or Later)
#else
const bool kRunning32BitGDI = ((::GetVersion () & 0x80000000) == 0); // I BELIEVE this is how we can test we are under NT!!!
#endif // Should be a better way to check for 32bit GDI!!!
#endif

#if qStroika_Platform_Windows
inline void Win32_GetTextExtentExPoint (HDC hdc, const Led_tChar* str, size_t nChars, int maxExtent, LPINT lpnFit, LPINT alpDx, LPSIZE lpSize)
{
    Require (nChars < static_cast<size_t> (numeric_limits<int>::max ()));
    Verify (::GetTextExtentExPointW (hdc, str, static_cast<int> (nChars), maxExtent, lpnFit, alpDx, lpSize));
}
inline void Win32_GetTextExtentPoint (HDC hdc, const Led_tChar* str, int nChars, LPSIZE lpSize)
{
    Verify (::GetTextExtentPointW (hdc, str, nChars, lpSize));
}
inline void Win32_TextOut (HDC hdc, int xStart, int yStart, const Led_tChar* str, int nChars)
{
    Verify (::TextOutW (hdc, xStart, yStart, str, nChars));
}
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
        unsigned short bitCount = IS_WIN30_DIB (dib) ? Led_ByteSwapFromWindows (hdr.biBitCount)
                                                     : Led_ByteSwapFromWindows (((const BITMAPCOREHEADER*)dib)->bcBitCount);

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

#if qStroika_Platform_Windows
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
            ++(*iP);
        }
    }

    void CreateStandardColorTable (RGBQUAD colorTable[256], COLORREF c1 = RGB (0, 0, 0), COLORREF c2 = RGB (0, 0, 0),
                                   COLORREF c3 = RGB (0, 0, 0), COLORREF c4 = RGB (0, 0, 0))
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
            for (; i < 256; ++i) {
                colorTable[i].rgbRed   = kColorSpecVals[redIdx];
                colorTable[i].rgbGreen = kColorSpecVals[greenIdx];
                colorTable[i].rgbBlue  = kColorSpecVals[blueIdx];
                ++blueIdx;
                if (blueIdx >= kColorSpecValCnt) {
                    blueIdx = 0;
                    ++greenIdx;
                    if (greenIdx >= kColorSpecValCnt) {
                        greenIdx = 0;
                        ++redIdx;
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

        --i; // don't count the WHITE color just added. It will be forced to be last.
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
                ++i;
                --nColorsLeft;
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
        Memory::StackBuffer<char> palBuf{sizeof (LOGPALETTE) + sizeof (PALETTEENTRY) * 256};
        LPLOGPALETTE              lplgPal = reinterpret_cast<LPLOGPALETTE> (static_cast<char*> (palBuf));

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
        size_t                    nInfoSize = sizeof (BITMAPINFOHEADER) + sizeof (RGBQUAD) * (1 << wBits);
        Memory::StackBuffer<char> bmiBuf{nInfoSize};
        LPBITMAPINFO              pbmi = reinterpret_cast<LPBITMAPINFO> (static_cast<char*> (bmiBuf));
        (void)::memset (pbmi, 0, nInfoSize);
        pbmi->bmiHeader.biSize        = sizeof (BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth       = dwX;
        pbmi->bmiHeader.biHeight      = dwY;
        pbmi->bmiHeader.biPlanes      = 1;
        pbmi->bmiHeader.biBitCount    = wBits;
        pbmi->bmiHeader.biCompression = BI_RGB; // OVERRIDE below for 16 and 32bpp

        for (int i = 0; i < 256; ++i) {
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
        size_t                    nInfoSize = sizeof (BITMAPINFOHEADER) + 3 * sizeof (DWORD);
        Memory::StackBuffer<char> bmiBuf{nInfoSize};
        LPBITMAPINFO              pbmi = reinterpret_cast<LPBITMAPINFO> (static_cast<char*> (bmiBuf));
        (void)::memset (pbmi, 0, nInfoSize);
        pbmi->bmiHeader.biSize     = sizeof (BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth    = dwX;
        pbmi->bmiHeader.biHeight   = dwY;
        pbmi->bmiHeader.biPlanes   = 1;
        pbmi->bmiHeader.biBitCount = 16;
        {
            // if it's 16bpp, fill in the masks and OVERRIDE the compression
            // these are the default masks - you could change them if needed
            LPDWORD pMasks                = (LPDWORD)(pbmi->bmiColors);
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
        size_t                    nInfoSize = sizeof (BITMAPINFOHEADER) + 3 * sizeof (DWORD);
        Memory::StackBuffer<char> bmiBuf{nInfoSize};
        LPBITMAPINFO              pbmi = reinterpret_cast<LPBITMAPINFO> (static_cast<char*> (bmiBuf));
        (void)::memset (pbmi, 0, nInfoSize);
        pbmi->bmiHeader.biSize     = sizeof (BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth    = dwX;
        pbmi->bmiHeader.biHeight   = dwY;
        pbmi->bmiHeader.biPlanes   = 1;
        pbmi->bmiHeader.biBitCount = 32;
        {
            // if it's 32bpp, fill in the masks and OVERRIDE the compression
            // these are the default masks - you could change them if needed
            LPDWORD pMasks                = (LPDWORD)(pbmi->bmiColors);
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

#if qStroika_Platform_Windows
/*
 ********************************************************************************
 ************************************ Bitmap ********************************
 ********************************************************************************
 */
BOOL Bitmap::CreateCompatibleBitmap (HDC hdc, DistanceType nWidth, DistanceType nHeight)
{
    Assert (m_hObject == nullptr);
    m_hObject  = ::CreateCompatibleBitmap (hdc, nWidth, nHeight);
    fImageSize = Led_Size (nHeight, nWidth);
    return (m_hObject != nullptr); // return value backward compat hack...
}

BOOL Bitmap::CreateCompatibleDIBSection (HDC hdc, DistanceType nWidth, DistanceType nHeight)
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
    return m_hObject != nullptr; // return value backward compat hack...
}

void Bitmap::LoadBitmap (HINSTANCE hInstance, LPCTSTR lpBitmapName)
{
    Require (m_hObject == nullptr);
    m_hObject = ::LoadBitmap (hInstance, lpBitmapName);
    Execution::ThrowIfNull (m_hObject);
    {
        BITMAP bm{};
        Verify (::GetObject (m_hObject, sizeof (BITMAP), (LPVOID)&bm));
        fImageSize = Led_Size (bm.bmHeight, bm.bmWidth);
    }
}
#endif

/*
 ********************************************************************************
 ************************************** Color ***********************************
 ********************************************************************************
 */
/*
 *  Color name values from page 79 of Web Design in a Nutshell, O'Reilly, table 5-2.
 */
const Color Color::kBlack     = Color (0, 0, 0);
const Color Color::kWhite     = Color (Color::kColorValueMax, Color::kColorValueMax, Color::kColorValueMax);
const Color Color::kRed       = Color (Color::kColorValueMax, 0, 0);
const Color Color::kGreen     = Color (0, Color::kColorValueMax / 2, 0);
const Color Color::kBlue      = Color (0, 0, Color::kColorValueMax);
const Color Color::kCyan      = Color (0, Color::kColorValueMax, Color::kColorValueMax);
const Color Color::kMagenta   = Color (Color::kColorValueMax, 0, Color::kColorValueMax);
const Color Color::kYellow    = Color (Color::kColorValueMax, Color::kColorValueMax, 0);
const Color Color::kMaroon    = Color (Color::kColorValueMax / 2, 0, 0);
const Color Color::kOlive     = Color (Color::kColorValueMax / 2, Color::kColorValueMax / 2, 0);
const Color Color::kNavyBlue  = Color (0, 0, Color::kColorValueMax / 2);
const Color Color::kPurple    = Color (Color::kColorValueMax / 2, 0, Color::kColorValueMax / 2);
const Color Color::kTeal      = Color (0, Color::kColorValueMax / 2, Color::kColorValueMax / 2);
const Color Color::kGray      = Color (Color::kColorValueMax / 2, Color::kColorValueMax / 2, Color::kColorValueMax / 2);
const Color Color::kSilver    = Color (Color::kColorValueMax * 3 / 4, Color::kColorValueMax * 3 / 4, Color::kColorValueMax * 3 / 4);
const Color Color::kDarkGreen = Color (0, (Color::kColorValueMax / 256) * 100, 0);
const Color Color::kLimeGreen = Color (0, Color::kColorValueMax, 0);
const Color Color::kFuchsia   = Color::kMagenta; // same according to that table
const Color Color::kAqua      = Color::kCyan;    // same according to that table

/*
 ********************************************************************************
 ****************************** FontSpecification ***************************
 ********************************************************************************
 */

/*
@METHOD:        FontSpecification::SetFontName
@DESCRIPTION:   <p>See also @'FontSpecification::GetFontName'.</p>
*/
void FontSpecification::SetFontName (const SDKString& fontName)
{
#if qStroika_Platform_Windows
    Characters::CString::Copy (fFontInfo.lfFaceName, std::size (fFontInfo.lfFaceName), fontName.c_str ());
    fFontInfo.lfCharSet = DEFAULT_CHARSET;
#endif
}

#if qStroika_Platform_Windows
FontSpecification::FontNameSpecifier::FontNameSpecifier (const Characters::SDKChar* from)
{
    Characters::CString::Copy (fName, std::size (fName), from);
}
#endif

void FontSpecification::SetFontNameSpecifier (FontNameSpecifier fontNameSpecifier)
{
#if qStroika_Platform_Windows
    Characters::CString::Copy (fFontInfo.lfFaceName, std::size (fFontInfo.lfFaceName), fontNameSpecifier.fName);
    fFontInfo.lfCharSet = DEFAULT_CHARSET;
#endif
}

#if qStroika_Platform_Windows
struct FontSelectionInfo {
    FontSelectionInfo (BYTE desiredCharset)
        : fDesiredCharset (desiredCharset)
        , fUsesBestCharset (false)
        , fIsTT (false)
        , fIsANSI_VAR_Font (false)
        , fIsGoodBackupCharset (false)
        , fIsFavoriteForCharset (false)
        , fIsVariablePitch (false)
        , fStartsWithAt (false)
    {
        memset (&fBestFont, 0, sizeof (fBestFont));
    }
    BYTE       fDesiredCharset;
    LOGFONT    fBestFont;
    bool       fUsesBestCharset;
    bool       fIsTT;
    bool       fIsANSI_VAR_Font; // a good second choice if Arial not present
    bool       fIsGoodBackupCharset;
    bool       fIsFavoriteForCharset;
    bool       fIsVariablePitch;
    bool       fStartsWithAt;
    inline int Score () const
    {
        int score = 0;
        if (fUsesBestCharset) {
            score += 10; // underweight - because (at least with Win2K) - the fCharset field almost always set to zero - so cannot be used reliably
        }
        if (fIsTT) {
            score += 20;
        }
        if (fIsANSI_VAR_Font) {
            score += 4;
        }
        if (fIsGoodBackupCharset) {
            score += 2;
        }
        if (fIsFavoriteForCharset) {
            score += 25;
        }
        if (fIsVariablePitch) {
            //unsure if this is desirable or not - good for US - but not sure about Japan?
            score += 2;
        }
        if (fStartsWithAt) {
            score -= 25; // we don't really support vertical fonts
        }
        return score;
    }
};
static int FAR PASCAL EnumFontCallback (const LOGFONT* lplf, const TEXTMETRIC* /*lpntm*/, DWORD fontType, LPARAM arg)
{
    // Score each font choice, and pick the 'best' one. Pick randomly if several are 'best'.
    RequireNotNull (lplf);
    FontSelectionInfo& result = *(FontSelectionInfo*)arg;

    static LOGFONT theANSILogFont;
    if (theANSILogFont.lfFaceName[0] == '\0') {
        HFONT xxx = HFONT (::GetStockObject (ANSI_VAR_FONT));
        Verify (::GetObject (xxx, sizeof theANSILogFont, &theANSILogFont));
    }

    FontSelectionInfo potentialResult = result;
    memcpy (&potentialResult.fBestFont, lplf, sizeof (LOGFONT));

    if (potentialResult.fDesiredCharset == DEFAULT_CHARSET) {
        potentialResult.fUsesBestCharset = bool (lplf->lfCharSet == theANSILogFont.lfCharSet);
    }
    else {
        potentialResult.fUsesBestCharset = bool (lplf->lfCharSet == potentialResult.fDesiredCharset);
    }
    potentialResult.fIsTT                = bool (fontType & TRUETYPE_FONTTYPE);
    potentialResult.fIsANSI_VAR_Font     = ::_tcscmp (lplf->lfFaceName, theANSILogFont.lfFaceName) == 0;
    potentialResult.fIsGoodBackupCharset = (lplf->lfCharSet == DEFAULT_CHARSET or lplf->lfCharSet == theANSILogFont.lfCharSet);
    {
        switch (potentialResult.fDesiredCharset) {
            case SHIFTJIS_CHARSET: {
                if (potentialResult.fBestFont.lfFaceName == SDKString{Led_SDK_TCHAROF ("MS P Gothic")} or
                    potentialResult.fBestFont.lfFaceName == SDKString{Led_SDK_TCHAROF ("MS Gothic")} or
                    potentialResult.fBestFont.lfFaceName == SDKString{Led_SDK_TCHAROF ("MS PGothic")}) {
                    potentialResult.fIsFavoriteForCharset = true;
                }
            } break;
            case CHINESEBIG5_CHARSET: {
                if (potentialResult.fBestFont.lfFaceName == SDKString{Led_SDK_TCHAROF ("MS HEI")} or
                    potentialResult.fBestFont.lfFaceName == SDKString{Led_SDK_TCHAROF ("MingLiU")}) {
                    potentialResult.fIsFavoriteForCharset = true;
                }
            } break;
        }
    }
    potentialResult.fIsVariablePitch = lplf->lfPitchAndFamily & VARIABLE_PITCH;
    potentialResult.fStartsWithAt    = lplf->lfFaceName[0] == '@';

    if (potentialResult.Score () > result.Score ()) {
        result = potentialResult;
    }
    return 1;
}
#endif
FontSpecification Led::GetStaticDefaultFont ()
{
    //  Since this can be called alot, and since its value shouldn't change during the lifetime
    //  of Led running, cache the result (and since on windows - at least - it is expensive to compute)
    static bool              sDefaultFontValid = false;
    static FontSpecification sDefaultFont;
    if (not sDefaultFontValid) {
#if qStroika_Platform_Windows
        sDefaultFont = GetStaticDefaultFont (DEFAULT_CHARSET);
#endif
#if qStroika_Platform_Windows
        sDefaultFont.SetTextColor (Led_GetTextColor ());
#endif
        sDefaultFontValid = true;
    }
    return (sDefaultFont);
}

#if qStroika_Platform_Windows
FontSpecification Led::GetStaticDefaultFont (BYTE charSet)
{
    FontSpecification defaultFont;
    //nb: import to go through the intermediary font so we don't set into the
    // LOGFONT lots of fields which are part of the chosen font but are
    // extraneous, and then mess up later choices to change to face name.
    //
    // Eg., if our default font is BOLD, that will result in a big weight# being part of
    // the logFont. But then if the user picks a different face name, we don't want it
    // to stay bold. Maybe BOLD is a bad example cuz that DOES show up in our UI (menu of
    // font attriobutes). But pick one attribute (width? or escarpment) which doesn't show
    // up in our choice lists, and yet gets caried along even after you change face names.
    //
    // See spr# 0426 for more details.
    FontSpecification fooo;
    FontSelectionInfo selectedFont (charSet);
    WindowDC          screenDC (nullptr);
#if defined(STRICT)
    ::EnumFontFamilies (screenDC.m_hDC, nullptr, EnumFontCallback, reinterpret_cast<LPARAM> (&selectedFont));
#else
    ::EnumFontFamilies (screenDC.m_hDC, nullptr, reinterpret_cast<FONTENUMPROC> (EnumFontCallback), reinterpret_cast<LPARAM> (&selectedFont));
#endif
    fooo.LightSetOSRep (selectedFont.fBestFont);

    // EnumFontFamilies seems to pick very bad sizes. Not sure why. No biggie. This works
    // pretty well. LGP 970613
    {
        static LOGFONT theANSILogFont;
        if (theANSILogFont.lfFaceName[0] == '\0') {
            HFONT xxx = HFONT (::GetStockObject (ANSI_VAR_FONT));
            Verify (::GetObject (xxx, sizeof theANSILogFont, &theANSILogFont));
        }
        fooo.SetPointSize (min (max (FontSpecification (theANSILogFont).GetPointSize (), static_cast<unsigned short> (8)),
                                static_cast<unsigned short> (14)));
    }

    defaultFont.MergeIn (fooo);
    defaultFont.SetTextColor (Led_GetTextColor ());
    return (defaultFont);
}
#endif

/*
@METHOD:        Intersection
@DESCRIPTION:   <p>Compute the subset of the two @'IncrementalFontSpecification' arguments where both parts
            are valid and identical.</p>
*/
IncrementalFontSpecification Led::Intersection (const IncrementalFontSpecification& lhs, const IncrementalFontSpecification& rhs)
{
    IncrementalFontSpecification result = lhs;

    // FontName Info
    {
        if (not lhs.GetFontNameSpecifier_Valid () or not rhs.GetFontNameSpecifier_Valid () or
            lhs.GetFontNameSpecifier () != rhs.GetFontNameSpecifier ()) {
            result.InvalidateFontNameSpecifier ();
        }
    }

    // Style Info
    {
        if (not lhs.GetStyle_Bold_Valid () or not rhs.GetStyle_Bold_Valid () or lhs.GetStyle_Bold () != rhs.GetStyle_Bold ()) {
            result.InvalidateStyle_Bold ();
        }
    }
    {
        if (not lhs.GetStyle_Italic_Valid () or not rhs.GetStyle_Italic_Valid () or lhs.GetStyle_Italic () != rhs.GetStyle_Italic ()) {
            result.InvalidateStyle_Italic ();
        }
    }
    {
        if (not lhs.GetStyle_Underline_Valid () or not rhs.GetStyle_Underline_Valid () or lhs.GetStyle_Underline () != rhs.GetStyle_Underline ()) {
            result.InvalidateStyle_Underline ();
        }
    }
    {
        if (not lhs.GetStyle_SubOrSuperScript_Valid () or not rhs.GetStyle_SubOrSuperScript_Valid () or
            lhs.GetStyle_SubOrSuperScript () != rhs.GetStyle_SubOrSuperScript ()) {
            result.InvalidateStyle_SubOrSuperScript ();
        }
    }
#if qStroika_Platform_Windows
    {
        if (not lhs.GetStyle_Strikeout_Valid () or not rhs.GetStyle_Strikeout_Valid () or lhs.GetStyle_Strikeout () != rhs.GetStyle_Strikeout ()) {
            result.InvalidateStyle_Strikeout ();
        }
    }
#endif

    // Font Color Info
    {
        if (not lhs.GetTextColor_Valid () or not rhs.GetTextColor_Valid () or lhs.GetTextColor () != rhs.GetTextColor ()) {
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
        if (lhs.GetPointSize_Valid () != rhs.GetPointSize_Valid () or (lhs.GetPointSize_Valid () and lhs.GetPointSize () != rhs.GetPointSize ())) {
            needsInval = true;
        }
        if (needsInval) {
            result.InvalidatePointSize ();
        }
    }

    return result;
}

#if qStroika_Platform_Windows
/*
 ********************************************************************************
 ******************************** Tablet::RecolorHelper *************************
 ********************************************************************************
 */
class Tablet::RecolorHelper {
public:
    RecolorHelper (HDC baseHDC, Led_Size size, Color hilightBackColor, Color hilightForeColor, Color oldBackColor, Color oldForeColor);
    ~RecolorHelper ();

public:
    static RecolorHelper* CheckCacheAndReconstructIfNeeded (RecolorHelper* _THIS_, HDC baseHDC, Led_Size size, Color hilightBackColor,
                                                            Color hilightForeColor, Color oldBackColor, Color oldForeColor);

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
COLORREF Tablet::RecolorHelper::MapColor (COLORREF c) const
{
    float fIntrp;
    fIntrp = (float)(255 - GetRValue (c)) / 256.0f;
    BYTE red = static_cast<BYTE> (GetRValue (fHilightBackColor) + fIntrp * GetRValue (fHilightForeColor) - fIntrp * GetRValue (fHilightBackColor));

    fIntrp = (float)(255 - GetGValue (c)) / 256.0f;
    BYTE green = static_cast<BYTE> (GetGValue (fHilightBackColor) + fIntrp * GetGValue (fHilightForeColor) - fIntrp * GetGValue (fHilightBackColor));

    fIntrp = (float)(255 - GetBValue (c)) / 256.0f;
    BYTE blue = static_cast<BYTE> (GetBValue (fHilightBackColor) + fIntrp * GetBValue (fHilightForeColor) - fIntrp * GetBValue (fHilightBackColor));
    return RGB (red, green, blue);
}
inline COLORREF Tablet::RecolorHelper::MapColor (RGBQUAD c) const
{
    return MapColor (RGB (c.rgbRed, c.rgbGreen, c.rgbBlue));
}

Tablet::RecolorHelper::RecolorHelper (HDC baseHDC, Led_Size size, Color hilightBackColor, Color hilightForeColor, Color oldBackColor, Color oldForeColor)
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

Tablet::RecolorHelper::~RecolorHelper ()
{
    if (fDibSection != nullptr) {
        ::SelectObject (fHMemDC, fOldBitmap);
        ::DeleteDC (fHMemDC);
        ::DeleteObject (fDibSection);
    }
}

Tablet::RecolorHelper* Tablet::RecolorHelper::CheckCacheAndReconstructIfNeeded (RecolorHelper* _THIS_, HDC baseHDC, Led_Size size, Color hilightBackColor,
                                                                                Color hilightForeColor, Color oldBackColor, Color oldForeColor)
{
    if (_THIS_ == nullptr or size.h > _THIS_->fSize.h or size.v > _THIS_->fSize.v or baseHDC != _THIS_->fBaseDC or
        hilightBackColor.GetOSRep () != _THIS_->fHilightBackColor or hilightForeColor.GetOSRep () != _THIS_->fHilightForeColor or
        oldBackColor.GetOSRep () != _THIS_->fOldBackColor or oldForeColor.GetOSRep () != _THIS_->fOldForeColor) {
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

void Tablet::RecolorHelper::MakeMappingTable ()
{
    for (size_t i = 0; i < 256; ++i) {
        fMappingTable[i] = FindClosestColorInColorTable_ (MapColor (fColorTable[i]));
    }
}

uint8_t Tablet::RecolorHelper::FindClosestColorInColorTable_ (COLORREF c) const
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

void Tablet::RecolorHelper::DoRecolor (const Led_Rect& hilightArea)
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

void Tablet::RecolorHelper::DoRecolor_SimpleDSTINVERT (const Led_Rect& hilightArea)
{
    // Does proper inverse video, but seems to ignore the TextColor/BkColor/Pen/Brush colors.
    // Really should fix this to behave like Mac - replacing the background color with the text hilight color.
    // See SPR#1271
    ::BitBlt (fBaseDC, hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (), fBaseDC, hilightArea.left,
              hilightArea.top, DSTINVERT);
}

void Tablet::RecolorHelper::DoRecolor_SimplePATINVERT (const Led_Rect& hilightArea)
{
    // Attempt at solving SPR#1271. Works decently - producing the right background - but the text is colored YELLOW instead of WHITE - and so
    // doesn't look very good (not enough contrast).
    Color   useColor = Color::kWhite - Color (fHilightBackColor);
    HGDIOBJ oldPen   = ::SelectObject (fBaseDC, ::GetStockObject (NULL_PEN));
    Brush   backgroundBrush (useColor.GetOSRep ());
    HGDIOBJ oldBrush = ::SelectObject (fBaseDC, backgroundBrush);
    ::BitBlt (fBaseDC, hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (), fBaseDC, hilightArea.left,
              hilightArea.top, PATINVERT);
    (void)::SelectObject (fBaseDC, oldPen);
    (void)::SelectObject (fBaseDC, oldBrush);
}

void Tablet::RecolorHelper::DoRecolor_CopyTo8BitManualMungePixAndBack (const Led_Rect& hilightArea)
{
    // By commenting stuff in and out - I determined that virtuall ALL the time is spent in this first
    // BitBlt () - LGP 2003-03-11
    // I also found that qUseDIBSectionForOffscreenBitmap made this BitBlt go much faster - to the point of acceptable speed
    // LGP - 2003-03-12

    // Copy the REAL image into our 8-bit DIBSECTION
    ::BitBlt (fHMemDC, 0, 0, hilightArea.GetWidth (), hilightArea.GetHeight (), fBaseDC, hilightArea.left, hilightArea.top, SRCCOPY);

    /*
     *
     *  Fiddle the bits:
     *
     *  NB: This code assumes we're pointing at an 8-bit COLOR-LOOKUP-TABLE based Image
     *
     *  Note - this also may be modifying MUCH MORE than is actually needed. It goes all the way to the
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
    ::BitBlt (fBaseDC, hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (), fHMemDC, 0, 0, SRCCOPY);
}
#endif

#if qStroika_Frameworks_Led_SupportGDI
/*
 ********************************************************************************
 *********************************** Tablet *************************************
 ********************************************************************************
 */
#if qStroika_Platform_Windows
Tablet::Tablet (HDC hdc, Tablet::OwnDCControl ownsDC)
    : m_hDC (hdc)
    , fRecolorHelper (nullptr)
    , m_hAttribDC (hdc)
    , m_bPrinting (false)
    , fOwnsDC (ownsDC)
    , fLogPixelsV (0)
    , fLogPixelsH (0)
{
}
#endif

Tablet::~Tablet ()
{
#if qStroika_Platform_Windows
    delete fRecolorHelper;
    if (m_hDC != nullptr and fOwnsDC == eOwnsDC) {
        ::DeleteDC (Detach ());
    }
#endif
}

/*
@METHOD:        Tablet::CvtFromTWIPS
@DESCRIPTION:   <p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).</p>
    <p>See also @'Tablet::CvtFromTWIPSV', @'Tablet::CvtFromTWIPSH', @'Tablet::CvtToTWIPSV', @'Tablet::CvtToTWIPSH'.</p>
*/
Led_Point Tablet::CvtFromTWIPS (TWIPS_Point from) const
{
    return Led_Point{CvtFromTWIPSV (from.v), CvtFromTWIPSH (from.h)};
}

/*
@METHOD:        Tablet::CvtToTWIPS
@DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
    <p>See also @'Tablet::CvtFromTWIPSV', @'Tablet::CvtFromTWIPSH', @'Tablet::CvtToTWIPSV', @'Tablet::CvtToTWIPSH'.</p>
*/
TWIPS_Point Tablet::CvtToTWIPS (Led_Point from) const
{
    return TWIPS_Point{CvtToTWIPSV (from.v), CvtToTWIPSH (from.h)};
}

/*
@METHOD:        Tablet::CvtFromTWIPS
@DESCRIPTION:   <p>Utility routine to convert from TWIPS to logical coordinates (usually pixels).</p>
    <p>See also @'Tablet::CvtFromTWIPSV', @'Tablet::CvtFromTWIPSH', @'Tablet::CvtToTWIPSV', @'Tablet::CvtToTWIPSH'.</p>
*/
Led_Rect Tablet::CvtFromTWIPS (TWIPS_Rect from) const
{
    return Led_Rect{CvtFromTWIPS (from.GetOrigin ()), Led_Size (CvtFromTWIPS (from.GetSize ()))};
}

/*
@METHOD:        Tablet::CvtToTWIPS
@DESCRIPTION:   <p>Utility routine to convert from logical coordinates (usually pixels) to TWIPS.</p>
    <p>See also @'Tablet::CvtFromTWIPSV', @'Tablet::CvtFromTWIPSH', @'Tablet::CvtToTWIPSV', @'Tablet::CvtToTWIPSH'.</p>
*/
TWIPS_Rect Tablet::CvtToTWIPS (Led_Rect from) const
{
    return TWIPS_Rect{CvtToTWIPS (from.GetOrigin ()), CvtToTWIPS (Led_Point (from.GetSize ()))};
}

/*
@METHOD:        Tablet::ScrollBitsAndInvalRevealed
@DESCRIPTION:   <p>Scroll the given 'windowRect' by 'scrollVBy localical units. The area of the window exposed by this
            action is invalidated (so a later update event will fix it).</p>
*/
void Tablet::ScrollBitsAndInvalRevealed (const Led_Rect& windowRect, CoordinateType scrollVBy)
{
#if qStroika_Platform_Windows
    RECT gdiMoveRect = AsRECT (windowRect);
    // NB: I used to use ScrollDC (Led 2.1 and earlier). But that code appeared to sometimes leave
    // little bits of crufy around. I never understood why. But I assume it was a windows bug.
    HWND w = GetWindow ();
    Execution::ThrowIfNull (w);
    ::ScrollWindow (w, 0, scrollVBy, &gdiMoveRect, &gdiMoveRect);
#else
    Assert (false); //NYI
#endif
}

/*
@METHOD:        Tablet::FrameRegion
@DESCRIPTION:   <p>Draw the outline of the given region 'r' in color 'c'.</p>
*/
void Tablet::FrameRegion (const Region& r, const Color& c)
{
#if qStroika_Platform_Windows
    Brush brush = Brush (c.GetOSRep ());
    (void)::FrameRgn (*this, r, brush, 1, 1);
#else
    Assert (false); // NYI
#endif
}

/*
@METHOD:        Tablet::FrameRectangle
@DESCRIPTION:   <p>Draw the outline of the given rectangle 'r' in color 'c' and with
            borderWidth (pen width) 'borderWidth'. This function does NOT use the currently selected
            pen or brush, or anything like that. It draws a border just INSIDE the rectangle specified
            by 'r'.
                </p>
*/
void Tablet::FrameRectangle (const Led_Rect& r, Color c, DistanceType borderWidth)
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
@METHOD:        Tablet::MeasureText
@DESCRIPTION:   <p>Measure the widths of the given argument @'Led_tChar's. Assign those widths into
            the array 'charLocations'. (EXPLAIN CAREFULLY REQUIREMENTS ABOUT BUFSIZE of charLocations and handling of
            zero case and offset rules, handling of tabs, etc)</p>
                <p>Note that the resulting measured text must come out in non-descreasing order (there can be zero
            character widths, but never negative).</p>
*/
void Tablet::MeasureText (const FontMetrics& precomputedFontMetrics, const Led_tChar* text, size_t nTChars, DistanceType* charLocations)
{
    RequireNotNull (text);
    RequireNotNull (charLocations);

#if qStroika_Platform_Windows
    DistanceType kMaxTextWidthResult = kRunning32BitGDI ? 0x7fffffff : 0x7fff;
    if (IsPrinting ()) {
        // See SPR#0435
        SIZE ve             = GetViewportExt ();
        SIZE we             = GetWindowExt ();
        kMaxTextWidthResult = ::MulDiv (kMaxTextWidthResult, we.cx, ve.cx) - 1;
    }
#endif
    size_t kMaxChars = kMaxTextWidthResult / precomputedFontMetrics.GetMaxCharacterWidth ();
    Assert (kMaxChars > 1);

    DistanceType runningCharCount = 0;
    for (size_t charsToGo = nTChars; charsToGo > 0;) {
        size_t i = nTChars - charsToGo;
        Assert (i < nTChars);

        while (text[i] == '\t') {
            Assert (charsToGo > 0);

            charLocations[i] = runningCharCount;
            ++i;
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

#if qStroika_Platform_Windows
        SIZE size;
        Assert (sizeof (int) == sizeof (DistanceType));

        Win32_GetTextExtentExPoint (m_hAttribDC, &text[i], charsThisTime, kMaxTextWidthResult, nullptr, (int*)&charLocations[i], &size);
        for (size_t j = 0; j < charsThisTime; ++j) {
            charLocations[i + j] += runningCharCount;
        }
#endif

        runningCharCount = charLocations[i + charsThisTime - 1];

        Assert (charsToGo >= charsThisTime);
        charsToGo -= charsThisTime;
    }

// LGP-991220 - This is generating asserts elsewhere - and seems like such a hack. Not sure why needed. Try getting rid of and see what happens?
#if qStroika_Platform_Windows && 0
    // This gross hack is cuz we do a GetTextExtent() at the end of the
    // DrawText for PC, to see how much we drew. This is the only hack
    // I could think of to assure we get consistent results (which is very important).
    // This REALLY shold be done better - fix may not be here, but in DrawCode below...
    // LGP 960509

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

    if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
        // Assure charLocations are in non-decreasing order (OK to have some zero width - but never negative).
        DistanceType d = 0;
        for (size_t i = 0; i < nTChars; ++i) {
            Ensure (d <= charLocations[i]);
            d = charLocations[i];
        }
    }
}

/*
@DESCRIPTION:   <p>Draw the given text (@'Led_tChars') to this tablet object, at the given logical coordinates. Use the given
            tabstop origin, and tabStopList object to compute where tabs go. Return the amountDrawn (number of pixels used by draw).
            (EXPLAIN CAREFULLY REQUIREMENTS ABOUT BUFSIZE of charLocations and handling of
            zero case and offset rules, handling of tabs, AND MUCH MORE etc)</p>
                <p>Note - for BIDI text - the VIRTUAL (display) text is what should be passed in. Text will be imaged
            left to right. This means that for RTL text such as Arabic or Hebrew - it should have already been
            re-ordered in the argument passed in, and any mirroring should have already been done. This routine WILL
            take care of any contextual shaping required (glyph selection based on context - as with Arabic).</p>
*/
void Tablet::TabbedTextOut ([[maybe_unused]] const FontMetrics& precomputedFontMetrics, const Led_tChar* text, size_t nBytes,
                            [[maybe_unused]] TextDirection direction, Led_Point outputAt, CoordinateType hTabOrigin,
                            const TabStopList& tabStopList, DistanceType* amountDrawn, CoordinateType hScrollOffset)
{
    DistanceType     widthSoFar = 0;
    const Led_tChar* textCursor = text;
    const Led_tChar* textEnd    = text + nBytes;
    while (textCursor < textEnd) {
        // Skip over tabs
        const Led_tChar* nextTabAt = textCursor;
        for (; nextTabAt < textEnd;) {
            if (*nextTabAt == '\t') {
                break;
            }
            ++nextTabAt;
        }

// Actually image the characters
#if qStroika_Platform_Windows
        int oldBkMode = SetBkMode (TRANSPARENT);

        if (direction == eLeftToRight) {
            Win32_TextOut (m_hDC, static_cast<int> (outputAt.h + widthSoFar - hScrollOffset), static_cast<int> (outputAt.v), textCursor,
                           static_cast<int> (nextTabAt - textCursor));

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
#if qUseGetCharPlacementToImage
            {
                size_t                       len = nextTabAt - textCursor;
                Memory::StackBuffer<wchar_t> glyphs{len};
                GCP_RESULTSW                 gcpResult;
                memset (&gcpResult, 0, sizeof (gcpResult));
                gcpResult.lStructSize = sizeof (GCP_RESULTS);
                gcpResult.lpGlyphs    = glyphs.data ();
                gcpResult.nGlyphs     = static_cast<UINT> (len);
                if (::GetCharacterPlacementW (m_hDC, textCursor, static_cast<int> (len), 0, &gcpResult, GCP_GLYPHSHAPE | GCP_LIGATE) != 0) {
                    Verify (::ExtTextOutW (m_hDC, outputAt.h + widthSoFar - hScrollOffset, outputAt.v, ETO_GLYPH_INDEX, nullptr,
                                           gcpResult.lpGlyphs, gcpResult.nGlyphs, nullptr));
                    goto Succeeded_But_Need_To_Adjust_Width;
                }
            }
#endif

            {
                size_t len = nextTabAt - textCursor;
                // Fallback - if the above fails...
                // Displays the text in the right order, but doesn't do contextual shaping (tested on WinXP and WinME) - LGP 2002-12-10
                Verify (::ExtTextOutW (m_hDC, outputAt.h + widthSoFar - hScrollOffset, outputAt.v, 0, nullptr, textCursor,
                                       static_cast<UINT> (len), nullptr));
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

        (void)SetBkMode (oldBkMode);
#endif

        // Now see if nextTab really pointing at a tab (otherwise at end of buffer)
        if (nextTabAt < textEnd) {
            DistanceType thisTabWidth;
            {
                DistanceType curOutputAtZeroBased = (outputAt.h - hTabOrigin) + widthSoFar;
                DistanceType tabStop              = tabStopList.ComputeTabStopAfterPosition (this, curOutputAtZeroBased);
                thisTabWidth                      = tabStop - curOutputAtZeroBased;
                Assert (thisTabWidth >= 0);
            }

            widthSoFar += thisTabWidth;
            ++nextTabAt; // since we processed that tab...
        }
        textCursor = nextTabAt;
    }
    if (amountDrawn != nullptr) {
        *amountDrawn = widthSoFar;
    }
}

void Tablet::SetBackColor (const Color& backColor)
{
#if qStroika_Platform_Windows
    SetBkColor (backColor.GetOSRep ());
#endif
}

void Tablet::SetForeColor (const Color& foreColor)
{
#if qStroika_Platform_Windows
    SetTextColor (foreColor.GetOSRep ());
#endif
}

/*
@METHOD:        Tablet::EraseBackground_SolidHelper
@DESCRIPTION:   <p>EraseBackground_SolidHelper () is simple helper function - usually called from subclasses which OVERRIDE
    @'TextImager::EraseBackground'.</p>
*/
void Tablet::EraseBackground_SolidHelper (const Led_Rect& eraseRect, const Color& eraseColor)
{
    if (not eraseRect.IsEmpty ()) {
#if qStroika_Platform_Windows
        Led_Rect         eraser = eraseRect;
        Brush            backgroundBrush (eraseColor.GetOSRep ());
        GDI_Obj_Selector pen (this, ::GetStockObject (NULL_PEN));
        GDI_Obj_Selector brush (this, backgroundBrush);
        ++eraser.right; // lovely - windows doesn't count last pixel... See Docs for Rectangle() and rephrase!!!
        ++eraser.bottom;
        Rectangle (AsRECT (eraser));
#endif
    }
}

/*
@METHOD:        Tablet::HilightArea_SolidHelper
@DESCRIPTION:   <p>HilightArea_SolidHelper () is simple helper function - usually called from subclasses which OVERRIDE
            @'TextImager::HilightArea'.</p>
                <p>Note the backColor and foreColor are advisory - and maybe ignored if the GDI better supports (or the
            platform UI conventionally calls for) inverting the text via a simple XOR.</p>
                <p>Note also that as of Led 3.1b4, there is new code to properly respect the hilight fore/back colors. This code
            should generally be fast enough, but may - on some hardware - NOT be fast enough. By setting the text color to black,
            the background color to while, and the hilight colors the reverse of this (fore=black/back=white), this code will revert
            to the old algorithm, and run much faster.</p>
*/
void Tablet::HilightArea_SolidHelper (const Led_Rect& hilightArea, [[maybe_unused]] Color hilightBackColor,
                                      [[maybe_unused]] Color hilightForeColor, Color oldBackColor, [[maybe_unused]] Color oldForeColor)
{
    if (not hilightArea.IsEmpty ()) {
#if qStroika_Platform_Windows
        /*
         *  SPR#1271 - major reworking using DIB sections etc, to get much better display of hilighted text.
         */
        if (hilightBackColor.GetOSRep () == Color::kBlack.GetOSRep () and hilightForeColor.GetOSRep () == Color::kWhite.GetOSRep () and
            oldBackColor.GetOSRep () == Color::kWhite.GetOSRep () and oldForeColor.GetOSRep () == Color::kBlack.GetOSRep ()) {
            // This is much faster (on some/most hardware) than the RecolorHelper algorithms. For this special case of of B&W fore/back/hilight
            // colors - this code is MUCH faster as well. So - for people for whom the default algorithm is too slow - they can just specify
            // these colors for hilight and back/fore-color, and they'll get the faster hilight.
            // See SPR#1271
            BitBlt (hilightArea.left, hilightArea.top, hilightArea.GetWidth (), hilightArea.GetHeight (), this, hilightArea.left,
                    hilightArea.top, DSTINVERT);
        }
        else {
#if 1
            fRecolorHelper = RecolorHelper::CheckCacheAndReconstructIfNeeded (fRecolorHelper, m_hDC,
                                                                              Led_Size (hilightArea.GetHeight (), hilightArea.GetWidth ()),
                                                                              hilightBackColor, hilightForeColor, oldBackColor, oldForeColor);
            fRecolorHelper->DoRecolor (hilightArea);
#else
            static RecolorHelper* recolorHelper = nullptr;
            recolorHelper = RecolorHelper::CheckCacheAndReconstructIfNeeded (recolorHelper, m_hDC,
                                                                             Led_Size (hilightArea.GetHeight (), hilightArea.GetWidth ()),
                                                                             hilightBackColor, hilightForeColor, oldBackColor, oldForeColor);
            recolorHelper->DoRecolor (hilightArea);
#endif
        }
#endif
    }
}

/*
@METHOD:        Tablet::HilightArea_SolidHelper
@DESCRIPTION:   <p>HilightArea_SolidHelper () is simple helper function - usually called from subclasses which OVERRIDE
            @'TextImager::HilightArea'.</p>
                <p>Note the backColor and foreColor are advisory - and maybe ignored if the GDI better supports (or the
            platform UI conventionally calls for) inverting the text via a simple XOR.</p>
*/
void Tablet::HilightArea_SolidHelper (const Region& hilightArea, [[maybe_unused]] Color hilightBackColor, [[maybe_unused]] Color hilightForeColor,
                                      [[maybe_unused]] Color oldBackColor, [[maybe_unused]] Color oldForeColor)
{
    if (not hilightArea.IsEmpty ()) {
#if qStroika_Platform_Windows
        Assert (false); // probably not hard - bit not totally obvious how todo and since not called yet - ignore for now... LGP 2002-12-03
#endif
    }
}

/*
@METHOD:        Tablet::GetFontMetrics
@DESCRIPTION:   <p>Retrieve the (@'FontMetrics') associated with the current tablet (based on the last SetFont call).</p>
*/
FontMetrics Tablet::GetFontMetrics () const
{
#if qStroika_Platform_Windows
    RequireNotNull (m_hAttribDC);
    TEXTMETRIC tms;
    Verify (::GetTextMetrics (m_hAttribDC, &tms) != 0);
    return tms;
#endif
}

/*
 ********************************************************************************
 ***************************** OffscreenTablet::OT ******************************
 ********************************************************************************
 */
#if qStroika_Platform_Windows
OffscreenTablet::OT::OT (HDC hdc, Tablet::OwnDCControl ownsDC)
    : inherited (hdc, ownsDC)
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
#if qStroika_Platform_Windows
    , fMemDC ()
    , fMemoryBitmap ()
    , fOldBitmapInDC (nullptr)
#endif
{
}

OffscreenTablet::~OffscreenTablet ()
{
#if qStroika_Platform_Windows
    if (fOldBitmapInDC != nullptr) {
        (void)fMemDC.SelectObject (fOldBitmapInDC);
    }
#endif
}

/*
@METHOD:        OffscreenTablet::Setup
@DESCRIPTION:   <p>Prepare a new offscreen drawing environment given the starting basis 'originalTablet' (typically from a window).</p>
                <p>Later call @'OffscreenTablet::PrepareRect' before any actual drawing can be done. This should be called once before
            calling @'OffscreenTablet::PrepareRect'.</p>
*/
void OffscreenTablet::Setup (Tablet* origTablet)
{
    Require (fOrigTablet == nullptr); // can only call once.
    RequireNotNull (origTablet);

    fOrigTablet = origTablet;
#if qStroika_Platform_Windows
    if (fMemDC.CreateCompatibleDC (fOrigTablet)) {
        fOffscreenTablet = &fMemDC;
    }
#endif
}

/*
@METHOD:        OffscreenTablet::PrepareRect
@DESCRIPTION:   <p>Prepare the offscreen drawing environment for the given 'currentRowRect'. This can only be safely called
            after the call to @'OffscreenTablet::Setup' - but can be called multiple times. Note that calls to this
            will typically 'destroy' the bits in the offscreen tablet.</p>
*/
Tablet* OffscreenTablet::PrepareRect (const Led_Rect& currentRowRect, DistanceType extraToAddToBottomOfRect)
{
    Tablet* result = fOrigTablet;
#if qStroika_Platform_Windows
    if (fOffscreenTablet != nullptr) {
        fOffscreenRect = currentRowRect;
        fOffscreenRect.bottom += extraToAddToBottomOfRect;
        // See if we need to re-allocate the bitmap
        if (fMemoryBitmap == nullptr or (fOffscreenRect.GetSize () != fMemoryBitmap.GetImageSize ())) {
            // deselect our new memory bitmap before changing its size - not sure needed, but lets be paranoid -
            // this is Windows after all ... LGP 960513
            if (fOldBitmapInDC != nullptr) {
                (void)fMemDC.SelectObject (fOldBitmapInDC);
            }
            fMemoryBitmap.DeleteObject (); // lose previous contents, if any...
#if qUseDIBSectionForOffscreenBitmap
            if (fMemoryBitmap.CreateCompatibleDIBSection (fOrigTablet->m_hDC, fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight ()) == 0) {
                fOffscreenTablet = nullptr; // OK, just don't use...
            }
#else
            if (fMemoryBitmap.CreateCompatibleBitmap (fOrigTablet->m_hDC, fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight ()) == 0) {
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
#if qStroika_Platform_Windows
        Tablet* screenDC = fOrigTablet;
        screenDC->BitBlt (fOffscreenRect.left, fOffscreenRect.top, fOffscreenRect.GetWidth (), fOffscreenRect.GetHeight (),
                          fOffscreenTablet, fOffscreenRect.left, fOffscreenRect.top, SRCCOPY);
#endif
    }
}
#endif

#if qStroika_Frameworks_Led_SupportGDI

/*
 ********************************************************************************
 ********************************* InstalledFonts ***************************
 ********************************************************************************
 */
InstalledFonts::InstalledFonts (FilterOptions filterOptions)
    : fFilterOptions (filterOptions)
    , fFontNames ()
{
#if qStroika_Platform_Windows
    LOGFONT lf;
    memset (&lf, 0, sizeof (LOGFONT));
    lf.lfCharSet = DEFAULT_CHARSET;
    WindowDC screenDC (nullptr);
    ::EnumFontFamiliesEx (screenDC.m_hDC, &lf, (FONTENUMPROC)FontFamilyAdderProc, reinterpret_cast<LPARAM> (this), 0);
    sort (fFontNames.begin (), fFontNames.end ());
    vector<SDKString>::iterator rest = unique (fFontNames.begin (), fFontNames.end ());
    fFontNames.erase (rest, fFontNames.end ()); // remove the duplicates
#else
    Assert (false); // NYI for other platforms
#endif
}

#if qStroika_Platform_Windows
BOOL FAR PASCAL InstalledFonts::FontFamilyAdderProc (ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int fontType, LPVOID pThis)
{
    InstalledFonts* thisP = reinterpret_cast<InstalledFonts*> (pThis);

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
#endif

#if qStroika_Frameworks_Led_SupportGDI

/*
 ********************************************************************************
 ********************************* Globals *******************************
 ********************************************************************************
 */

Globals* Globals::sThe = nullptr;

// Somewhat silly hack so Globals gets destroyed at end of application execution. Helpful for quitting memleak detectors.
class Globals::_Global_DESTRUCTOR_ {
public:
    ~_Global_DESTRUCTOR_ ()
    {
        delete (Globals::sThe);
        Globals::sThe = nullptr;
    }
} sTheLed_GDIGlobalsDESTRUCTOR_;

Globals::Globals ()
    : fLogPixelsH (0)
    , fLogPixelsV (0)
{
    InvalidateGlobals ();
}

void Globals::InvalidateGlobals ()
{
// From the name, it would appear we invalidated, and re-validate later. But I think this implematnion is a bit
// simpler, and should perform fine given its expected usage.
#if qStroika_Platform_Windows
    WindowDC screenDC (nullptr);
    fLogPixelsH = ::GetDeviceCaps (screenDC, LOGPIXELSX);
    fLogPixelsV = ::GetDeviceCaps (screenDC, LOGPIXELSY);
#endif
}
#endif

#if qStroika_Frameworks_Led_SupportGDI
/*
 ********************************************************************************
 ************************************ AddRectangleToRegion **********************
 ********************************************************************************
 */
void Led::AddRectangleToRegion (Led_Rect addRect, Region* toRgn)
{
    RequireNotNull (toRgn);
    *toRgn = *toRgn + Region (addRect);
}
#endif

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
#if qStroika_Platform_Windows
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
    return reinterpret_cast<const char*> (dib) + Led_ByteSwapFromWindows (hdr.biSize) + Led_GetDIBPalletByteCount (dib);
}

#if qStroika_Platform_Windows
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

    [[maybe_unused]] int nScanLinesCopied =
        ::GetDIBits (hDC, hbm, 0, dibResult->bmiHeader.biHeight,
                     reinterpret_cast<char*> (dibResult) + Led_GetDIBPalletByteCount (dibResult) + sizeof (BITMAPINFOHEADER), dibResult, DIB_RGB_COLORS);
    Assert (nScanLinesCopied == dibResult->bmiHeader.biHeight);
    return dibResult;
}
#endif

#if qStroika_Frameworks_Led_ProvideIMESupport
#include <ime.h>

/*
 ********************************************************************************
 ****************************************** IME *********************************
 ********************************************************************************
 */
IME* IME::sThe = nullptr;

#ifndef qUseNewIMECode
#define qUseNewIMECode 1
#endif

// Somewhat silly hack so IME gets destroyed at end of application execution. Helpful for quitting memleak detectors.
class IME::_Global_DESTRUCTOR_ {
public:
    ~_Global_DESTRUCTOR_ ()
    {
        delete (IME::sThe);
        IME::sThe = nullptr;
    }
} sTheIME_DESTRUCTOR_;

IME::IME ()
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

void IME::NotifyPosition (HWND hWnd, const SHORT x, const SHORT y)
{
    if (x != fLastX || y != fLastY) {
        UpdatePosition (hWnd, x, y);
    }
}

void IME::NotifyOfFontChange (HWND hWnd, const LOGFONT& lf)
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
void IME::SendSimpleMessage (HWND hWnd, UINT fnc, WPARAM wParam)
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

void IME::IMEOn (HWND hWnd)
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

void IME::IMEOff (HWND hWnd)
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

void IME::UpdatePosition (const HWND hWnd, const SHORT x, const SHORT y)
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

        // Should I redo this taking the LOGFONT as an arg to IME::UpdatePosition ()??? LGP 980714
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

wstring IME::GetCompositionResultStringW (HWND hWnd)
{
    wstring result;
    if (fImmGetCompositionStringW != nullptr and fImmGetContext != nullptr and fImmReleaseContext != nullptr) {
        HIMC hImc = 0;
        if ((hImc = fImmGetContext (hWnd)) != 0) {
            wchar_t curIMEString[2048];
            LONG    nChars = fImmGetCompositionStringW (hImc, GCS_RESULTSTR, curIMEString, static_cast<DWORD> (std::size (curIMEString)));

            nChars /= sizeof (wchar_t); // why???? LGP 991214
            if (nChars >= 0 and static_cast<size_t> (nChars) < std::size (curIMEString)) {
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
    CoordinateType xLeft = (centerIn.left + centerIn.right) / 2 - r.GetWidth () / 2;
    CoordinateType yTop  = (centerIn.top + centerIn.bottom) / 2 - r.GetHeight () / 2;
    return Led_Rect (yTop, xLeft, r.GetHeight (), r.GetWidth ());
}

#if qStroika_Platform_Windows
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
