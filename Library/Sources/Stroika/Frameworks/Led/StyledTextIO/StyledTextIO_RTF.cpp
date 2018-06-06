/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../Foundation/StroikaPreComp.h"

#include <cctype>
#include <climits>
#include <cstdio> // for a couple sprintf() calls - could pretty easily be avoided

#include "../../../Foundation/Characters/CodePage.h"
#if qPlatform_Windows
#include "../../../Foundation/Characters/Platform/Windows/CodePage.h"
#endif
#include "../../../Foundation/Characters/CString/Utilities.h"
#include "../../../Foundation/Characters/String.h"

#include "../Config.h"

#include "../Support.h"

#include "StyledTextIO_RTF.h"

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(4 : 4800) //qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#pragma warning(4 : 4786) //qQuiteAnnoyingDebugSymbolTruncationWarnings
#endif

/**
 *  @todo   Must fix to properly support 32-bit and 64-bit safety
 */
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(4 : 4267)
#endif

// Include moved down here cuz of GCC lib bug...
#if qBitSetTemplateAvailable
#include <bitset>
#endif

#ifndef qUseCompiledSetHack
#if qBitSetTemplateAvailable
// Preliminary testing on Mac indicates this compiled set hack doesn't help. Test on PC, as well...
// LGP 970326
#define qUseCompiledSetHack 1
#else
#define qUseCompiledSetHack 0
#endif
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

inline int ConvertReadSingleHexDigit (char digit)
{
    if (isupper (digit)) {
        digit = static_cast<char> (tolower (digit));
    }
    if (isdigit (digit)) {
        return digit - '0';
    }
    else if (islower (digit)) {
        return 10 + (digit - 'a');
    }
    else {
        Led_ThrowBadFormatDataException ();
        Assert (false);
        return 0; // not reached
    }
}
inline char ConvertWriteSingleHexDigit (int numZeroToFifteen)
{
    Require (numZeroToFifteen >= 0);
    Require (numZeroToFifteen <= 15);
    if (numZeroToFifteen < 10) {
        return static_cast<char> ('0' + numZeroToFifteen);
    }
    else {
        return static_cast<char> ('a' + (numZeroToFifteen - 10));
    }
}

// RTF / Led_LineSpacing support
inline static Led_LineSpacing mkLineSpacing_From_RTFValues (Led_Coordinate sl, bool multi)
{
    Led_LineSpacing result; // defaults to single line...
    if (sl != 1000) {
        if (multi) {
            /*
             *  This / 12 is total guesswork. The RTF 1.5 Spec is TOTALLY VAGUE. Much of this was just guestimated
             *  and infered from the Win32 Docs on PARAFORMAT2, plus the trial and error - which yielded that
             *  dividing by 12 got the right answer!
             *      -- LGP 2000/06/12
             */
            if (sl < 0 or sl > 2000) {
                sl = 240; // will work out to single line - don't let bogus values in the RTF file make trouble for us...
            }
            result = Led_LineSpacing (Led_LineSpacing::eExactLinesSpacing, sl / 12);
        }
        else {
            if (sl < 0) {
                result = Led_LineSpacing (Led_LineSpacing::eExactTWIPSSpacing, Led_TWIPS (-sl));
            }
            else {
                result = Led_LineSpacing (Led_LineSpacing::eAtLeastTWIPSSpacing, Led_TWIPS (sl));
            }
        }
    }
    return result;
}
inline static void mkRTFValues_From_LineSpacing (Led_LineSpacing inLS, Led_Coordinate* sl, bool* multi)
{
    const int kOneLinesWorth = 240;
    switch (inLS.fRule) {
        case Led_LineSpacing::eOnePointFiveSpace:
            *sl    = static_cast<Led_Coordinate> (kOneLinesWorth * 1.5);
            *multi = true;
            break;
        case Led_LineSpacing::eDoubleSpace:
            *sl    = kOneLinesWorth * 2;
            *multi = true;
            break;
        case Led_LineSpacing::eAtLeastTWIPSSpacing:
            *sl    = inLS.fArg;
            *multi = false;
            break;
        case Led_LineSpacing::eExactTWIPSSpacing:
            *sl    = -static_cast<Led_Coordinate> (inLS.fArg);
            *multi = false;
            break;
        case Led_LineSpacing::eExactLinesSpacing:
            *sl    = inLS.fArg * 12;
            *multi = true;
            break;

        default: // Treat as Single space
        case Led_LineSpacing::eSingleSpace:
            *sl    = 1000;
            *multi = true;
            break;
    }
}

#if !qWideCharacters

/*
 ********************************************************************************
 ******************* RTFIO::SingleByteCharsetToCharsetMappingTable **************
 ********************************************************************************
 */
RTFIO::SingleByteCharsetToCharsetMappingTable::SingleByteCharsetToCharsetMappingTable (CodePage srcEncoding, CodePage dstEncoding, char bogusChar)
//:fMappingTable ()
{
    /*
     *  Walk the two (input and output) tables, and compute the table which would convert from src to dst.
     *  This means simply linearly walking the input table, and for all valid input characters, find the corresponding
     *  number (by doing a name lookup) in the output table.
     *
     *  Any unmappable combinations are replaced with the 'bogusChar'.
     */
    CodePageConverter srcCvt (srcEncoding);
    CodePageConverter dstCvt (dstEncoding);

    for (size_t i = 0; i < 256; i++) {
        wchar_t unicodeChar = '\0';

        {
            size_t outCharCnt = 1;
            char   srcChar    = i;
            srcCvt.MapToUNICODE (&srcChar, 1, &unicodeChar, &outCharCnt);
            if (outCharCnt != 1) {
                unicodeChar = '\0';
            }
        }

        if (unicodeChar == '\0') {
            fMappingTable[i] = bogusChar;
        }
        else {
            char   outChar;
            size_t outCharCnt = 1;
            dstCvt.MapFromUNICODE (&unicodeChar, 1, &outChar, &outCharCnt);
            fMappingTable[i] = (outCharCnt == 1) ? outChar : bogusChar;
        }
    }
}
#endif

/*
 ********************************************************************************
 ******************** RTFIO::FontTableEntry::FontTableEntry *********************
 ********************************************************************************
 */
RTFIO::FontTableEntry::FontTableEntry ()
    : fFontName ()
    , fFNum (-1)
    , fFamily (eNil)
    , fCharSet (static_cast<Byte> (-1))
    , fPitch (0)
    , fCodePage (0)
{
}

/*
 ********************************************************************************
 ********************************** RTFIO::FontTable ****************************
 ********************************************************************************
 */
using FontTableEntry = RTFIO::FontTableEntry;
using FontTable      = RTFIO::FontTable;
FontTable::FontTable ()
    : fEntries ()
{
}

FontTable::FontTable (const vector<FontTableEntry>& fontTable)
    : fEntries (fontTable)
{
}

Led_IncrementalFontSpecification FontTable::GetFontSpec (int fontNumber)
{
    const FontTableEntry* ftep = LookupEntryByNumber (fontNumber);
    if (ftep == nullptr) {
        return Led_IncrementalFontSpecification (); // See spr#0696 0 some docs leave bad \font#s - so don't blow up - just no font spec!
    }
    const FontTableEntry&            fte = *ftep;
    Led_IncrementalFontSpecification fontSpec;
#if qPlatform_MacOS
    Str255 fontName;
    fontName[0] = fte.fFontName.length ();
    memcpy (&fontName[1], fte.fFontName.c_str (), fontName[0]);
    short fontNum = 0;
    ::GetFNum (fontName, &fontNum);
    if (fontNum == 0) {
        // Alas, the Mac font Manager returns ZERO as the font number if it really
        // has no idea about the font. This is NOT what we want. So instead,
        // use the other information - like font family etc - to make a best guess
        // about what font # to return. Of course, be careful. Maybe the user REALLY
        // DID specify font# 0 - system font.
        Str255 realName;
        ::GetFontName (0, realName);
        if (memcmp (realName, fontName, realName[0]) == 0) {
            // then the user REALLY said Chicago/System font, and so
            // we just use that...
        }
        else {
            // Just guesses at what we should use. Maybe reverse engineer what
            // MS Word does on mac?
            switch (fte.fFamily) {
                case FontTableEntry::eNil:
                    fontNum = applFont;
                    break;
                case FontTableEntry::eRoman:
                    fontNum = kFontIDTimes;
                    break;
                case FontTableEntry::eSwiss:
                    fontNum = kFontIDGeneva;
                    break;
                case FontTableEntry::eModern:
                    fontNum = kFontIDMonaco;
                    break;
                case FontTableEntry::eScript:
                    fontNum = kFontIDLondon;
                    break;
                case FontTableEntry::eDecor:
                    fontNum = kFontIDLondon;
                    break;
                case FontTableEntry::eTech:
                    fontNum = kFontIDSymbol;
                    break;
                case FontTableEntry::eBidi:
                default: {
                    // no idea. Just us application font I guess?
                    fontNum = applFont;
                } break;
            }
        }
    }
    fontSpec.SetFontNameSpecifier (fontNum);
#elif qPlatform_Windows || qXWindows
    fontSpec.SetFontNameSpecifier (fte.fFontName.c_str ());
#endif
    return fontSpec;
}

const FontTableEntry* FontTable::LookupEntryByNumber (int fontNumber)
{
    for (size_t i = 0; i < fEntries.size (); i++) {
        const FontTableEntry& fte = fEntries[i];
        if (fte.fFNum == fontNumber) {
            return &fte;
        }
    }
    return nullptr;
}

const FontTableEntry* FontTable::LookupEntryByName (const Led_SDK_String& name)
{
    for (size_t i = 0; i < fEntries.size (); i++) {
        const FontTableEntry& fte = fEntries[i];
        if (fte.fFontName == name) {
            return &fte;
        }
    }
    return nullptr;
}

FontTableEntry FontTable::Add (const FontTableEntry& newEntry)
{
    int            newFontNumber = FindSmallestUnusedFontNumber ();
    FontTableEntry newerEntry    = newEntry;
    newerEntry.fFNum             = newFontNumber;
    fEntries.push_back (newerEntry);
    return newerEntry;
}

int FontTable::FindSmallestUnusedFontNumber () const
{
    // Not terribly efficient, but OK for small numbers... LGP 960825
    int tryThis = 0;
Again:
    for (size_t i = 0; i < fEntries.size (); i++) {
        const FontTableEntry& fte = fEntries[i];
        if (fte.fFNum == tryThis) {
            tryThis++;
            goto Again;
        }
    }
    return tryThis;
}

/*
 ********************************************************************************
 ******************************** RTFIO::ColorTable *****************************
 ********************************************************************************
 */
RTFIO::ColorTable::ColorTable ()
    : fEntries ()
{
}

RTFIO::ColorTable::ColorTable (const vector<Led_Color>& colorTable)
    : fEntries (colorTable)
{
}

Led_Color RTFIO::ColorTable::LookupColor (size_t colorNumber) const
{
    if (colorNumber < 0 or colorNumber >= fEntries.size ()) {
        Led_ThrowBadFormatDataException (); // font number not found!
    }
    return fEntries[colorNumber];
}

size_t RTFIO::ColorTable::LookupColor (const Led_Color& color) const
{
    for (size_t i = 0; i < fEntries.size (); i++) {
        const Led_Color& c = fEntries[i];
        if (c == color) {
            return i;
        }
    }
    Ensure (false);
    return 0;
}

int RTFIO::ColorTable::EnterColor (const Led_Color& color)
{
    for (size_t i = 0; i < fEntries.size (); i++) {
        const Led_Color& c = fEntries[i];
        if (c == color) {
            return i;
        }
    }
    fEntries.push_back (color);
    return fEntries.size () - 1;
}

/*
 ********************************************************************************
 ****************************** RTFIO::ListTableEntry ***************************
 ********************************************************************************
 */

RTFIO::ListTableEntry::ListTableEntry ()
    : fListID (0)
    , fListTemplateID (0)
    , fListStyle (eListStyle_None)
    , fFontID (0)
{
}

/*
 ********************************************************************************
 ***************************** RTFIO::ListOverrideTableEntry ********************
 ********************************************************************************
 */
RTFIO::ListOverrideTableEntry::ListOverrideTableEntry ()
    : fListID (0)
{
}

/*
 ********************************************************************************
 ******************************** RTFIO::ListTables *****************************
 ********************************************************************************
 */
RTFIO::ListTables::ListTables ()
    : fEntries ()
    , fOverrideEntries ()
{
}

RTFIO::ListTables::ListTables (const vector<ListTableEntry>& listTableEntries, const vector<ListOverrideTableEntry>& listOverrideTableEntries)
    : fEntries (listTableEntries)
    , fOverrideEntries (listOverrideTableEntries)
{
}

/*
 ********************************************************************************
 ********************************** RTFInfo *************************************
 ********************************************************************************
 */
const Led_PrivateEmbeddingTag RTFIO::kRTFBodyGroupFragmentEmbeddingTag = "RTFBFrag";
#if qPlatform_Windows
const Led_ClipFormat RTFIO::kRTFBodyGroupFragmentClipFormat = static_cast<Led_ClipFormat> (::RegisterClipboardFormat (_T("RTFF")));
#else
const Led_ClipFormat RTFIO::kRTFBodyGroupFragmentClipFormat = 'RTFF';
#endif

inline RTFIO::ControlWordNameMap RTFIO::mkDefaultControlWordNameMap ()
{
    RTFIO::ControlWordNameMap table;
#if qUseMapForControlWordMap
#define TAB_INS_METHOD(name)                              \
    Assert (table.size () == RTFIO::eControlAtom_##name); \
    table.insert (ControlWordNameMap::value_type (#name, RTFIO::eControlAtom_##name))
#else
#define TAB_INS_METHOD(name)                              \
    Assert (table.size () == RTFIO::eControlAtom_##name); \
    table.push_back (ControlWordNameMap::value_type (#name, RTFIO::eControlAtom_##name))
#endif

    TAB_INS_METHOD (tab);
    TAB_INS_METHOD (bullet);
    TAB_INS_METHOD (endash);
    TAB_INS_METHOD (emdash);
    TAB_INS_METHOD (lquote);
    TAB_INS_METHOD (rquote);
    TAB_INS_METHOD (ldblquote);
    TAB_INS_METHOD (rdblquote);

    TAB_INS_METHOD (ansi);
    TAB_INS_METHOD (author);
    TAB_INS_METHOD (b);
    TAB_INS_METHOD (blue);
    TAB_INS_METHOD (brdrs);
    TAB_INS_METHOD (brdrth);
    TAB_INS_METHOD (brdrsh);
    TAB_INS_METHOD (brdrdb);
    TAB_INS_METHOD (brdrdot);
    TAB_INS_METHOD (brdrdash);
    TAB_INS_METHOD (brdrhair);
    TAB_INS_METHOD (brdrdashsm);
    TAB_INS_METHOD (brdrdashd);
    TAB_INS_METHOD (brdrdashdd);
    TAB_INS_METHOD (brdrtriple);
    TAB_INS_METHOD (brdrtnthsg);
    TAB_INS_METHOD (brdrthtnsg);
    TAB_INS_METHOD (brdrtnthtnsg);
    TAB_INS_METHOD (brdrtnthmg);
    TAB_INS_METHOD (brdrthtnmg);
    TAB_INS_METHOD (brdrtnthtnmg);
    TAB_INS_METHOD (brdrtnthlg);
    TAB_INS_METHOD (brdrthtnlg);
    TAB_INS_METHOD (brdrtnthtnlg);
    TAB_INS_METHOD (brdrwavy);
    TAB_INS_METHOD (brdrwavydb);
    TAB_INS_METHOD (brdrdashdotstr);
    TAB_INS_METHOD (brdremboss);
    TAB_INS_METHOD (brdrengrave);
    TAB_INS_METHOD (brdrw);
    TAB_INS_METHOD (brdrcf);
    TAB_INS_METHOD (cchs);
    TAB_INS_METHOD (cell);
    TAB_INS_METHOD (cellx);
    TAB_INS_METHOD (cf);
    TAB_INS_METHOD (clcbpat);
    TAB_INS_METHOD (cpg);
    TAB_INS_METHOD (colortbl);
    TAB_INS_METHOD (deff);
    TAB_INS_METHOD (deflang);
    TAB_INS_METHOD (deftab);
    TAB_INS_METHOD (deleted);
    TAB_INS_METHOD (dibitmap);
    TAB_INS_METHOD (dn);
    TAB_INS_METHOD (emfblip);
    TAB_INS_METHOD (f);
    TAB_INS_METHOD (fbidi);
    TAB_INS_METHOD (fcharset);
    TAB_INS_METHOD (fdecor);
    TAB_INS_METHOD (fi);
    TAB_INS_METHOD (fmodern);
    TAB_INS_METHOD (footer);
    TAB_INS_METHOD (fnil);
    TAB_INS_METHOD (fonttbl);
    TAB_INS_METHOD (fprq);
    TAB_INS_METHOD (froman);
    TAB_INS_METHOD (fs);
    TAB_INS_METHOD (fswiss);
    TAB_INS_METHOD (fscript);
    TAB_INS_METHOD (ftech);
    TAB_INS_METHOD (green);
    TAB_INS_METHOD (header);
    TAB_INS_METHOD (i);
    TAB_INS_METHOD (ilvl);
    TAB_INS_METHOD (info);
    TAB_INS_METHOD (intbl);
    TAB_INS_METHOD (jpegblip);
    TAB_INS_METHOD (li);
    TAB_INS_METHOD (line);
    TAB_INS_METHOD (listtext);
    TAB_INS_METHOD (ledprivateobjectembeddingformat);
    TAB_INS_METHOD (ls);
    TAB_INS_METHOD (mac);
    TAB_INS_METHOD (macpict);
    TAB_INS_METHOD (margb);
    TAB_INS_METHOD (margl);
    TAB_INS_METHOD (margr);
    TAB_INS_METHOD (margt);
    TAB_INS_METHOD (objdata);
    TAB_INS_METHOD (object);
    TAB_INS_METHOD (objemb);
    TAB_INS_METHOD (objh);
    TAB_INS_METHOD (objscalex);
    TAB_INS_METHOD (objscaley);
    TAB_INS_METHOD (objw);
    TAB_INS_METHOD (outl);
    TAB_INS_METHOD (paperh);
    TAB_INS_METHOD (paperw);
    TAB_INS_METHOD (par);
    TAB_INS_METHOD (pard);
    TAB_INS_METHOD (pc);
    TAB_INS_METHOD (pca);
    TAB_INS_METHOD (pich);
    TAB_INS_METHOD (pichgoal);
    TAB_INS_METHOD (picscalex);
    TAB_INS_METHOD (picscaley);
    TAB_INS_METHOD (pict);
    TAB_INS_METHOD (picw);
    TAB_INS_METHOD (picwgoal);
    TAB_INS_METHOD (plain);
    TAB_INS_METHOD (pmmetafile);
    TAB_INS_METHOD (pn);
    TAB_INS_METHOD (pngblip);
    TAB_INS_METHOD (pntext);
    TAB_INS_METHOD (qc);
    TAB_INS_METHOD (qj);
    TAB_INS_METHOD (ql);
    TAB_INS_METHOD (qr);
    TAB_INS_METHOD (red);
    TAB_INS_METHOD (result);
    TAB_INS_METHOD (ri);
    TAB_INS_METHOD (row);
    TAB_INS_METHOD (rtf);
    TAB_INS_METHOD (sa);
    TAB_INS_METHOD (sb);
    TAB_INS_METHOD (shad);
    TAB_INS_METHOD (sl);
    TAB_INS_METHOD (slmult);
    TAB_INS_METHOD (sub);
    TAB_INS_METHOD (super);
    TAB_INS_METHOD (strike);
    TAB_INS_METHOD (stylesheet);
    TAB_INS_METHOD (trleft);
    TAB_INS_METHOD (trgaph);
    TAB_INS_METHOD (trowd);
    TAB_INS_METHOD (trpaddb);
    TAB_INS_METHOD (trpaddl);
    TAB_INS_METHOD (trpaddr);
    TAB_INS_METHOD (trpaddt);
    TAB_INS_METHOD (trspdb);
    TAB_INS_METHOD (trspdl);
    TAB_INS_METHOD (trspdr);
    TAB_INS_METHOD (trspdt);
    TAB_INS_METHOD (tx);
    TAB_INS_METHOD (u);
    TAB_INS_METHOD (uc);
    TAB_INS_METHOD (ul);
    TAB_INS_METHOD (ulnone);
    TAB_INS_METHOD (up);
    TAB_INS_METHOD (v);
    TAB_INS_METHOD (wbitmap);
    TAB_INS_METHOD (wmetafile);
#undef TAB_INS_METHOD
    Assert (table.size () == RTFIO::eControlAtomDynamicRangeStart);
#if !qUseMapForControlWordMap
    sort (table.begin (), table.end ());
#endif
    return table;
}
RTFIO::ControlWordNameMap RTFIO::sControlWordNameMap = RTFIO::mkDefaultControlWordNameMap ();

string RTFIO::GetAtomName (ControlWordAtom atom)
{
    using ITER = RTFIO::ControlWordNameMap::const_iterator;
    ITER start = sControlWordNameMap.begin ();
    ITER end   = sControlWordNameMap.end ();
    for (ITER it = start; it != end; ++it) {
        if ((*it).second == atom) {
            return string ((*it).first);
        }
    }
    Assert (false);
    return "";
}

#if !qUseMapForControlWordMap
struct RTFIO::StringNControlWordAtom_Comparator : binary_function<RTFIO::StringNControlWordAtom, const char*, bool> {
    bool operator() (const RTFIO::StringNControlWordAtom& x, const char* y) const
    {
        return x.first < y;
    }
};
#endif
RTFIO::ControlWordAtom RTFIO::EnterControlWord (
#if qUseMapForControlWordMap
    const ControlWordAtomName& controlWord
#else
    const char* controlWord
#endif
)
{
#if !qUseMapForControlWordMap
    RequireNotNull (controlWord);
#endif

    using ITER = ControlWordNameMap::iterator;
#if qUseMapForControlWordMap
    ITER i = sControlWordNameMap.find (controlWord);
    if (i == sControlWordNameMap.end ()) {
        ControlWordAtom newVal = ControlWordAtom (eControlAtomDynamicRangeStart + sControlWordNameMap.size ());
        i                      = sControlWordNameMap.insert (ControlWordNameMap::value_type (controlWord, newVal)).first;
    }
    return i->second;
#else
    ITER start = sControlWordNameMap.begin ();
    ITER end = sControlWordNameMap.end ();
    ITER i = lower_bound (start, end, controlWord, StringNControlWordAtom_Comparator ());
    if (i != end and controlWord == (*i).first) {
        return (*i).second;
    }
    ControlWordAtom newVal = ControlWordAtom (eControlAtomDynamicRangeStart + sControlWordNameMap.size ());
    sControlWordNameMap.insert (i, ControlWordNameMap::value_type (controlWord, newVal));
    return newVal;
#endif
}

/*
 ********************************************************************************
 ************************ RTFIO::RTFOLEEmbedding ********************************
 ********************************************************************************
 */
const Led_PrivateEmbeddingTag RTFIO::RTFOLEEmbedding::kEmbeddingTag = "OLE2RTFEm";

/*
 ********************************************************************************
 **************** StyledTextIOReader_RTF::ReaderContext *************************
 ********************************************************************************
 */
using ReaderContext = StyledTextIOReader_RTF::ReaderContext;

/*
 ********************************************************************************
 ********** StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination ********
 ********************************************************************************
 */
/*
 *  SPR#0952,0968.
 *  EndParagraph/SetContext/AboutToChange () delayed handling code.
 *      The REASON behind all of this is that RTF will sometimes end a paragraph (with a \par) and our internal handling
 *  will set certain styles (say cuz an RTF scope closes - so values reset) - but we don't want this to force the
 *  emitting of a NEWLINE (cuz RTF docs will sometimes end in a \par when there should be no final NEWLINE in the doc).
 *
 *      So - we need this complex machinery to delay handling of the \par ("\n") and final returning "SetContext" calls. which
 *  preserves their ordering, but allows them to sometimes be cut-off and not executed.
 *
 *      Whenever an EndParagraph () happens - we force any delayed handling of SetContext() calls. However - if we have a pending
 *  EndParagraph (someone called EndParagraph but hasn't yet done anything else 'intersting to force it to be flushed), then
 *  we still allow a SetContext call to be 'cashed' and not yet handled.
 *
 *      So - for example - the RTF "\par }" will cause TWO things to be delayed, whereas the RTF "}\par" will result in
 *  only the delayed para handling.
 *
 *      Note - if we get "\par}}" its not entirely clear what todo. The safest thing is that two successive SetContext() calls with nothing else
 *  in between would cause a flushing - but then we MIGHT end up where stuff was nested by some crazy RTF writer an extra level - and
 *  we'd get the extra garbage char (newline) emitted. Not tragic - but not great. Or we could treat this as just ignoring the previous
 *  SetContext call. This should happen rarely - and I'm not sure what is best. For now - I think I'll just ignore the prev call and allow
 *  multiple SetContext () calls to happen and just ignore earlier ones - LGP 2001-08-23.
 */
using SinkStreamDestination = ReaderContext::SinkStreamDestination;
StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination::SinkStreamDestination (StyledTextIOReader_RTF& reader)
    : Destination_ ()
    , fSinkStream (reader.GetSinkStream ())
    , fRTFInfo (reader.GetRTFInfo ())
    , fReader (reader)
    , fCurrentContext ()
    , fNewContext ()
    , fNewContextPending (false)
    ,
    //fSmallBuffer (),
    fTCharsInSmallBuffer (0)
    , fParaEndedFlag (false)
    , fParaEndBeforeNewContext (false)
    , fInTable (false)
    , fTableOpen (false)
    , fTableNextRowNum (0)
    , fTableInRow (false)
    , fTableNextCellNum (0)
    , fTableInCell (false)
{
}

SinkStreamDestination::~SinkStreamDestination ()
{
    // must FLUSH THIS GUY BEFORE DTOR - cannot flush here cuz that could raise exception, and
    // (at least with MWERKS CW9) that appears to cause unexpected() to be called - illegal in C++
    // to throw from DTOR? - LGP 960921
    Require (fTCharsInSmallBuffer == 0);
}

void SinkStreamDestination::AppendText (const Led_tChar* text, size_t nTChars)
{
    AboutToChange ();
    AppendText_ (text, nTChars);
}

void SinkStreamDestination::AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding)
{
    //  NB: we PROBABLY should apply the current font to this region as well, but so far - at least - it
    //  has no significance. So ignore for now ... LGP 961003

    // Cannot easily see how to cache inserting these. They should be rare enuf not worth
    // worrying about.
    AboutToChange ();
    Flush ();
    fSinkStream.AppendEmbedding (embedding);
}

void SinkStreamDestination::AppendSoftLineBreak ()
{
    AboutToChange ();
    Flush ();
    fSinkStream.AppendSoftLineBreak ();
}

void SinkStreamDestination::EndParagraph ()
{
    AboutToChange ();
    fParaEndedFlag = true;
}

void SinkStreamDestination::UseFont (const Led_IncrementalFontSpecification& fontSpec)
{
    AboutToChange ();
    if (fCurrentContext.fFontSpec != fontSpec) {
        Flush ();
        fCurrentContext.fFontSpec = fontSpec;
    }
}

void SinkStreamDestination::SetJustification (Led_Justification justification)
{
    AboutToChange ();
    if (fCurrentContext.fJustification != justification) {
        Flush ();
        fSinkStream.SetJustification (fCurrentContext.fJustification = justification);
    }
}

void SinkStreamDestination::SetTabStops (const TextImager::StandardTabStopList& tabStops)
{
    AboutToChange ();
    if (fCurrentContext.fTabStops != tabStops) {
        Flush ();
        fSinkStream.SetStandardTabStopList (fCurrentContext.fTabStops = tabStops);
    }
}

void SinkStreamDestination::SetFirstIndent (Led_TWIPS tx)
{
    AboutToChange ();
    if (fCurrentContext.fFirstIndent != tx) {
        Flush ();
        fSinkStream.SetFirstIndent (fCurrentContext.fFirstIndent = tx);
    }
}

void SinkStreamDestination::SetLeftMargin (Led_TWIPS lhs)
{
    AboutToChange ();
    if (fCurrentContext.fLeftMargin != lhs) {
        Flush ();
        fSinkStream.SetLeftMargin (fCurrentContext.fLeftMargin = lhs);
    }
}

void SinkStreamDestination::SetRightMargin (Led_TWIPS rhs)
{
    AboutToChange ();
    Flush ();

    Led_TWIPS effectivePaperSize = fRTFInfo.GetEffectiveDrawingWidth ();
    Led_TWIPS realMargin         = Led_TWIPS (effectivePaperSize - rhs);
    if (realMargin < 0) {
        realMargin = Led_TWIPS (1);
    }
    fSinkStream.SetRightMargin (fCurrentContext.fRightMargin = realMargin);
}

void SinkStreamDestination::SetSpaceBefore (Led_TWIPS tx)
{
    AboutToChange ();
    if (fCurrentContext.fSpaceBefore != tx) {
        Flush ();
        fSinkStream.SetSpaceBefore (fCurrentContext.fSpaceBefore = tx);
    }
}

void SinkStreamDestination::SetSpaceAfter (Led_TWIPS tx)
{
    AboutToChange ();
    if (fCurrentContext.fSpaceAfter != tx) {
        Flush ();
        fSinkStream.SetSpaceAfter (fCurrentContext.fSpaceAfter = tx);
    }
}

void SinkStreamDestination::SetSpaceBetweenLines (Led_Coordinate sl)
{
    AboutToChange ();
    if (fCurrentContext.fSpaceBetweenLines != sl) {
        Flush ();
        fSinkStream.SetLineSpacing (mkLineSpacing_From_RTFValues (fCurrentContext.fSpaceBetweenLines = sl, fCurrentContext.fSpaceBetweenLinesMult));
    }
}

void SinkStreamDestination::SetSpaceBetweenLinesMult (bool multipleLineSpacing)
{
    AboutToChange ();
    if (fCurrentContext.fSpaceBetweenLinesMult != multipleLineSpacing) {
        Flush ();
        fSinkStream.SetLineSpacing (mkLineSpacing_From_RTFValues (fCurrentContext.fSpaceBetweenLines, fCurrentContext.fSpaceBetweenLinesMult = multipleLineSpacing));
    }
}

void SinkStreamDestination::SetTextHidden (bool hidden)
{
    AboutToChange ();
    if (fCurrentContext.fTextHidden != hidden) {
        Flush ();
        fSinkStream.SetTextHidden (fCurrentContext.fTextHidden = hidden);
    }
}

void SinkStreamDestination::SetInTable (bool inTable)
{
    AboutToChange ();
    if (fInTable != inTable) {
        Flush ();
        fInTable = inTable;
    }
}

void SinkStreamDestination::EndRow (bool forceEmit)
{
    if (forceEmit and not fTableInRow) {
        AssureTableOpen ();
        DoStartRow ();
    }
    if (fTableInRow) {
        Flush ();
        if (fTableInCell) {
            EndCell ();
        }

        fSinkStream.EndTableRow ();
        fTableInRow = false;
    }
    Ensure (not fTableInRow);
    Ensure (not fTableInCell);
    SetInTable (false); // not SURE this is right - but I THINK \intbl needs to be repeated after each row???
}

void SinkStreamDestination::EndCell (bool forceEmit)
{
    if (forceEmit and not fTableInCell) {
        AssureTableOpen ();
        if (not fTableInRow) {
            DoStartRow ();
        }
        DoStartCell ();
    }
    if (fTableInCell) {
        Flush ();

        size_t thisCellNum = fTableNextCellNum - 1;
        if (thisCellNum < fThisRow.fCellInfosForThisRow.size ()) {
            fSinkStream.SetCellBackColor (fThisRow.fCellInfosForThisRow[thisCellNum].f_clcbpat);
        }
        fSinkStream.EndTableCell ();
        fTableInCell = false;
    }
}

void SinkStreamDestination::SetListStyle (ListStyle listStyle)
{
    AboutToChange ();
    if (fCurrentContext.fListStyle != listStyle) {
        Flush ();
        fSinkStream.SetListStyle (fCurrentContext.fListStyle = listStyle);
    }
}

void SinkStreamDestination::SetListIndentLevel (unsigned char indentLevel)
{
    AboutToChange ();
    if (fCurrentContext.fListIndentLevel != indentLevel) {
        Flush ();
        fSinkStream.SetListIndentLevel (fCurrentContext.fListIndentLevel = indentLevel);
    }
}

void SinkStreamDestination::SetTableBorderColor (Led_Color c)
{
    AboutToChange ();
    Flush ();
    fSinkStream.SetTableBorderColor (c);
}

void SinkStreamDestination::SetCellX (Led_TWIPS cellx)
{
    AboutToChange ();
    /*
     *  Ends a <celldef> according to the RTF 1.5 spec.
     */
    {
        /*
         *  The cellx value specifies the ENDPOINT of the cell, and we prefer to keep track of the WIDTH of each cell.
         */
        Led_TWIPS sub = Led_TWIPS (0);
        for (auto i = fThisRow.fCellInfosForThisRow.begin (); i != fThisRow.fCellInfosForThisRow.end (); ++i) {
            sub += (*i).f_cellx;
        }
        fNextCellInfo.f_cellx = cellx - sub;
    }

    fThisRow.fCellInfosForThisRow.push_back (fNextCellInfo);

    fNextCellInfo = CellInfo (); // clear out to default values
}

void SinkStreamDestination::SetCellBackColor (const Led_Color& c)
{
    AboutToChange ();
    fNextCellInfo.f_clcbpat = c;
}

void SinkStreamDestination::Call_trowd ()
{
    fNextCellInfo = CellInfo (); // clear out to default values
    fThisRow      = RowInfo ();  // ditto
}

void SinkStreamDestination::Set_trleft (Led_TWIPS t)
{
    fThisRow.f_trleft = t;
}

void SinkStreamDestination::SetDefaultCellMarginsForRow_top (Led_TWIPS t)
{
    fThisRow.fDefaultCellMargins.top = t;
}

void SinkStreamDestination::SetDefaultCellMarginsForRow_left (Led_TWIPS t)
{
    fThisRow.fDefaultCellMargins.left = t;
}

void SinkStreamDestination::SetDefaultCellMarginsForRow_bottom (Led_TWIPS t)
{
    fThisRow.fDefaultCellMargins.bottom = t;
}

void SinkStreamDestination::SetDefaultCellMarginsForRow_right (Led_TWIPS t)
{
    fThisRow.fDefaultCellMargins.right = t;
}

void SinkStreamDestination::SetDefaultCellSpacingForRow_top (Led_TWIPS t)
{
    // Because value written is only half real spacing - See RTF 1.7 spec
    fThisRow.fDefaultCellSpacing.top = Led_TWIPS (t * 2);
}

void SinkStreamDestination::SetDefaultCellSpacingForRow_left (Led_TWIPS t)
{
    // Because value written is only half real spacing - See RTF 1.7 spec
    fThisRow.fDefaultCellSpacing.left = Led_TWIPS (t * 2);
}

void SinkStreamDestination::SetDefaultCellSpacingForRow_bottom (Led_TWIPS t)
{
    // Because value written is only half real spacing - See RTF 1.7 spec
    fThisRow.fDefaultCellSpacing.bottom = Led_TWIPS (t * 2);
}

void SinkStreamDestination::SetDefaultCellSpacingForRow_right (Led_TWIPS t)
{
    // Because value written is only half real spacing - See RTF 1.7 spec
    fThisRow.fDefaultCellSpacing.right = Led_TWIPS (t * 2);
}

void SinkStreamDestination::AssureTableOpen ()
{
    if (not fTableInRow and fTableNextRowNum == 0) {
        fSinkStream.StartTable ();
        fTableOpen = true;
    }
}

void SinkStreamDestination::DoStartRow ()
{
    fSinkStream.StartTableRow ();
    {
        vector<Led_TWIPS> cellWidths;
        for (auto i = fThisRow.fCellInfosForThisRow.begin (); i != fThisRow.fCellInfosForThisRow.end (); ++i) {
            const Led_TWIPS kMinWidth = Led_TWIPS (0);
            Led_TWIPS       thisCellW = (*i).f_cellx;
            if (i == fThisRow.fCellInfosForThisRow.begin ()) {
                thisCellW -= fThisRow.f_trleft;
            }

            // This value of 3/2 * LHS is somewhat empirically derived from the output of MS Word XP. Its really quite
            // hadly documented - the relationship between cell spacing and cellx values.
            //      LGP 2003-05-01 - SPR#1396 (now corresponding change in writer)
            thisCellW -= Led_TWIPS (3 * fThisRow.fDefaultCellSpacing.left / 2);

            if (thisCellW < kMinWidth) {
                thisCellW = kMinWidth;
            }
            cellWidths.push_back (thisCellW);
        }
        fSinkStream.SetCellWidths (cellWidths);
        fSinkStream.SetDefaultCellMarginsForCurrentRow (fThisRow.fDefaultCellMargins.top,
                                                        fThisRow.fDefaultCellMargins.left,
                                                        fThisRow.fDefaultCellMargins.bottom,
                                                        fThisRow.fDefaultCellMargins.right);
        fSinkStream.SetDefaultCellSpacingForCurrentRow (fThisRow.fDefaultCellSpacing.top,
                                                        fThisRow.fDefaultCellSpacing.left,
                                                        fThisRow.fDefaultCellSpacing.bottom,
                                                        fThisRow.fDefaultCellSpacing.right);
    }
    fTableInRow       = true;
    fTableNextCellNum = 0;
    fTableNextRowNum++;
}

void SinkStreamDestination::DoStartCell ()
{
    fSinkStream.StartTableCell (fThisRow.fCellInfosForThisRow[fTableNextCellNum].fColSpan);
    fTableInCell = true;
    fTableNextCellNum++;
}

void SinkStreamDestination::DoEndTable ()
{
    if (fTableInRow) {
        EndRow ();
    }
    if (fTableOpen) {
        fSinkStream.EndTable ();
        fTableOpen        = false;
        fTableNextCellNum = 0;
        fTableNextRowNum  = 0;
    }
    Ensure (not fTableInRow);
    Ensure (not fTableOpen);
}

void SinkStreamDestination::Flush ()
{
    FlushParaEndings ();
    if (fTCharsInSmallBuffer != 0) {
        Led_FontSpecification fsp = fSinkStream.GetDefaultFontSpec ();
        fsp.MergeIn (fCurrentContext.fFontSpec);
        fSinkStream.AppendText (fSmallBuffer, fTCharsInSmallBuffer, &fsp);
        fTCharsInSmallBuffer = 0;
    }
}

void SinkStreamDestination::Done ()
{
    fParaEndedFlag = false; // EXPLAIN -= SPR#0952
    DoEndTable ();
    fSinkStream.EndOfBuffer ();
    Flush ();
}

SinkStreamDestination::Context SinkStreamDestination::GetContext () const
{
    return fCurrentContext;
}

void SinkStreamDestination::SetContext (const Context& c)
{
    fParaEndBeforeNewContext = fParaEndedFlag;
    fNewContext              = c;
    fNewContextPending       = true;
}

void SinkStreamDestination::ApplyContext (const Context& c)
{
    /*
     *  See elaborate comment about SPR#0952,0968 above.
     */
    UseFont (c.fFontSpec);
    SetTabStops (c.fTabStops);
    SetJustification (c.fJustification);
    SetSpaceBefore (c.fSpaceBefore);
    SetSpaceAfter (c.fSpaceAfter);
    SetSpaceBetweenLines (c.fSpaceBetweenLines);
    SetSpaceBetweenLinesMult (c.fSpaceBetweenLinesMult);
    SetListStyle (c.fListStyle);
    SetListIndentLevel (c.fListIndentLevel);
    SetFirstIndent (c.fFirstIndent);
    SetLeftMargin (c.fLeftMargin);
    SetRightMargin (c.fRightMargin);
    SetTextHidden (c.fTextHidden);
}

size_t SinkStreamDestination::current_offset () const
{
    AboutToChange ();
    return fSinkStream.current_offset () + fTCharsInSmallBuffer;
}

void SinkStreamDestination::InsertMarker (Marker* m, size_t at, size_t length, MarkerOwner* markerOwner)
{
    Require (at <= current_offset ());
    AboutToChange ();
    // Flush () before adding markers if any part of the marker must wrap text inserted text. Must let
    // the textstore know about the extra text before we can safely add the marker.
    if (at + length >= fSinkStream.current_offset ()) {
        Flush ();
    }
    fSinkStream.InsertMarker (m, at, length, markerOwner);
}

void SinkStreamDestination::AppendText_ (const Led_tChar* text, size_t nTChars)
{
    if (nTChars > 0) {
        if (fInTable) {
#if 1
            AssureTableOpen ();
#else
            if (not fTableInRow and fTableNextRowNum == 0) {
                fSinkStream.StartTable ();
                fTableOpen = true;
            }
#endif
            if (not fTableInRow) {
                DoStartRow ();
            }
            if (not fTableInCell) {
                DoStartCell ();
            }
        }
        else {
            DoEndTable ();
        }
    }

    if (fTCharsInSmallBuffer + nTChars < (NEltsOf (fSmallBuffer))) {
        (void)::memcpy (&fSmallBuffer[fTCharsInSmallBuffer], text, nTChars * sizeof (Led_tChar));
        fTCharsInSmallBuffer += nTChars;
    }
    else {
        Flush ();
        Assert (fTCharsInSmallBuffer == 0);

        if (nTChars < (NEltsOf (fSmallBuffer))) {
            (void)::memcpy (&fSmallBuffer[0], text, nTChars * sizeof (Led_tChar));
            fTCharsInSmallBuffer = nTChars;
        }
        else {
            // doesn't fit in our buffer, so write it directly...
            Led_FontSpecification fsp = fSinkStream.GetDefaultFontSpec ();
            fsp.MergeIn (fCurrentContext.fFontSpec);
            fSinkStream.AppendText (text, nTChars, &fsp);
        }
    }
}

void SinkStreamDestination::AboutToChange () const
{
    /*
     *  See elaborate comment about SPR#0952,0968 above.
     */
    FlushParaEndings ();
    FlushSetContextCalls ();
}

void SinkStreamDestination::FlushSetContextCalls () const
{
    /*
     *  See elaborate comment about SPR#0952,0968 above.
     */
    if (fNewContextPending) {
        fNewContextPending = false;
        const_cast<SinkStreamDestination*> (this)->ApplyContext (fNewContext);
    }
}

void SinkStreamDestination::FlushParaEndings () const
{
    /*
     *  See elaborate comment about SPR#0952,0968 above.
     */
    if (fParaEndedFlag) {
        fParaEndedFlag = false;
        Led_tChar c    = '\n';
        const_cast<SinkStreamDestination*> (this)->AppendText_ (&c, 1);
    }
}

/*
 ********************************************************************************
 **** StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination::CellInfo ****
 ********************************************************************************
 */
SinkStreamDestination::CellInfo::CellInfo ()
    : f_cellx (Led_TWIPS (0))
    , f_clcbpat (Led_Color::kWhite)
    , fColSpan (1)
{
}

/*
 ********************************************************************************
 **** StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination::RowInfo ****
 ********************************************************************************
 */
SinkStreamDestination::RowInfo::RowInfo ()
    : f_trrh (Led_TWIPS (0))
    , f_trleft (Led_TWIPS (0))
    , fDefaultCellMargins (Led_TWIPS (0), Led_TWIPS (0), Led_TWIPS (0), Led_TWIPS (0))
    , fDefaultCellSpacing (Led_TWIPS (0), Led_TWIPS (0), Led_TWIPS (0), Led_TWIPS (0))
    , fCellInfosForThisRow ()
{
}

/*
 ********************************************************************************
 ******************** StyledTextIOReader_RTF::ReaderContext *********************
 ********************************************************************************
 */
StyledTextIOReader_RTF::ReaderContext::ReaderContext (StyledTextIOReader_RTF& reader)
    : fReader (reader)
    , fDocumentCharacterSet (Characters::kCodePage_ANSI)
    , // ANSI default, according to RTF spec
    fCurrentInputCharSetEncoding (Characters::kCodePage_ANSI)
    ,
#if qWideCharacters
//fMultiByteInputCharBuf (),
#else
#if qPlatform_MacOS
    fCurrentOutputCharSetEncoding (kCodePage_MAC)
    ,
#elif qPlatform_Windows || qXWindows
    fCurrentOutputCharSetEncoding (kCodePage_ANSI)
    , // not sure???
#endif
    fCharsetMappingTable (fCurrentInputCharSetEncoding, fCurrentOutputCharSetEncoding)
    , // note: important these two members DECLARED before this one... else not INITED at this point!
#endif
#if qWideCharacters
    fUnicodeUCValue (1)
    , fSkipNextNChars_UC (0)
    ,
#endif
    fHiddenTextStart (static_cast<size_t> (-1))
    , fDefaultFontNumber (-1)
    , fStartedBodyYet (false)
    , fCurrentDestination (nullptr)
    , fDefaultDestination ()
    , fCurrentGroup (nullptr)
    , fFontTable (nullptr)
    , fColorTable (nullptr)
{
#if qWideCharacters
    memset (fMultiByteInputCharBuf, 0, sizeof (fMultiByteInputCharBuf));
#endif
#if qCannotAssignRValueAutoPtrToExistingOneInOneStepBug || qTroubleOverloadingXofXRefCTORWithTemplatedMemberCTOR
    unique_ptr<Destination_> x = unique_ptr<Destination_> (new SinkStreamDestination (reader));
    fDefaultDestination        = x;
#else
    fDefaultDestination = unique_ptr<Destination_> (new SinkStreamDestination (reader));
#endif
    SetDestination (fDefaultDestination.get ());
}

StyledTextIOReader_RTF::ReaderContext::~ReaderContext ()
{
    Require (fCurrentGroup == nullptr); // all our current groups must be deleted before this whole
    // reader context (else they would have pointers back to us
    // after we've been destroyed)
    delete fFontTable;
    delete fColorTable;
}

void StyledTextIOReader_RTF::ReaderContext::UseInputCharSetEncoding (CodePage codePage)
{
#if qWideCharacters
    fCurrentInputCharSetEncoding = codePage;
#else
    if (fCurrentInputCharSetEncoding != codePage) {
        fCurrentInputCharSetEncoding = codePage;
        fCharsetMappingTable = RTFIO::SingleByteCharsetToCharsetMappingTable (fCurrentInputCharSetEncoding, fCurrentOutputCharSetEncoding);
    }
#endif
}

#if !qWideCharacters
void StyledTextIOReader_RTF::ReaderContext::UseOutputCharSetEncoding (CodePage codePage)
{
    if (fCurrentOutputCharSetEncoding != codePage) {
        fCurrentOutputCharSetEncoding = codePage;
        fCharsetMappingTable          = RTFIO::SingleByteCharsetToCharsetMappingTable (fCurrentInputCharSetEncoding, fCurrentOutputCharSetEncoding);
    }
}
#endif

void StyledTextIOReader_RTF::ReaderContext::PutRawCharToDestination (char c)
{
    RequireNotNull (GetCurrentGroupContext ());

#if qWideCharacters
    if (fSkipNextNChars_UC > 0) {
        fSkipNextNChars_UC--;
        return;
    }
#endif

    CodePage codePage = GetCurrentInputCharSetEncoding ();

    // Weird rules for handling cchs - from MS RTF 1.3 spec:
    //\cchsN    Indicates any characters not belonging to the default
    //          document character set and tells which character set they do belong to.
    //          Macintosh character sets are represented by values greater than 255.
    //          The values for N correspond to the values for the \fcharset control word.
    //
    // But this seems to neglect the concept of \cpg, or \fN&\fcharsets which could have set
    // the current character set.
    //
    // So my best guess to interpret this is that it only applies when the current characterset
    // IS the same as the document character set (AS OF LED 2.3).
    //
    //  As of Led 3.0d6 (2000/04/29) - I've changed this slightly - so if a cchs is specified - it simply overrides
    //  the code current codepage specification. My reason for this change was mostly cuz of what I saw in exising practice in
    //  the few files I had that used \cchs, and because new new CodePage support didn't make it easy to tell if a character
    //  existed in an existing code page (and I think my old tabular mechanism was highly questionable, and incomplete).
    //

#if qWideCharacters
    if ((GetCurrentGroupContext ()->fCCHSCodePage != 0) and
        fDocumentCharacterSet == codePage) {
        codePage = GetCurrentGroupContext ()->fCCHSCodePage;
    }

    if (fMultiByteInputCharBuf[0] == '\0') {
        fMultiByteInputCharBuf[0] = c;
        fMultiByteInputCharBuf[1] = '\0';
    }
    else {
        /*
         *  If we have a PENDING first-byte - then append this to our buffer, and pretend
         *  the we read these two bytes at once.
         */
        fMultiByteInputCharBuf[1] = c; // set it even if its bad so OnBadUserInput can peek()
    }

    wchar_t outChar;
    size_t  nOutChars = 1;
    CodePageConverter (codePage).MapToUNICODE (fMultiByteInputCharBuf, fMultiByteInputCharBuf[1] == 0 ? 1 : 2, &outChar, &nOutChars);
    Assert (nOutChars == 0 or nOutChars == 1);
    if (nOutChars == 1) {
        GetDestination ().AppendText (&outChar, 1);
        fMultiByteInputCharBuf[0] = '\0';
    }
    else {
        if (fMultiByteInputCharBuf[1] == '\0') {
            // Assume was the first byte of a multi-byte sequenece
            // and we should use it next time around...
        }
        else {
            // just assume garbage character - and use our default 'garbage char'
            Led_tChar u = GetReader ().GetDefaultUnsupportedCharacterChar ();
            GetDestination ().AppendText (&u, 1);
            fMultiByteInputCharBuf[0] = '\0';
        }
    }
#else

    if (GetCurrentGroupContext ()->fCCHSCodePage != 0) {
        RTFIO::SingleByteCharsetToCharsetMappingTable mapTable (GetCurrentGroupContext ()->fCCHSCodePage, GetCurrentOutputCharSetEncoding ());
        c = mapTable.Map (c);
    }
    else {
        c = fCharsetMappingTable.Map (c);
    }
    GetDestination ().AppendText (&c, 1);
#endif
}

/*
 ********************************************************************************
 *********** StyledTextIOReader_RTF:ReaderContext::GroupContext *****************
 ********************************************************************************
 */
StyledTextIOReader_RTF::ReaderContext::GroupContext::GroupContext (ReaderContext& readerContext)
    : fReaderContext (readerContext)
    , fParentGroup (readerContext.fCurrentGroup)
    , fCurrentGroupStartIdx (readerContext.GetReader ().GetSrcStream ().current_offset ())
    , fCurrentCodePage (kCodePage_ANSI)
    , fCCHSCodePage (0)
    , fDestinationContext () /// LGP 2001-08-22- ------ FOR NOW - BOGUS INITIAL VALUE- BUT SHOULD GET FROM readerContext::Destination::GetContext() rather than from parent group!!!
{
    if (fParentGroup != nullptr) {
        fDestinationContext = fParentGroup->fDestinationContext; // SEE ABOVE - DONT GET FROM PARENT GROUP BUT FROM CUR DESTINATION CONTEXT!!!
        fCurrentCodePage    = fParentGroup->fCurrentCodePage;
        fCCHSCodePage       = fParentGroup->fCCHSCodePage;
    }
    readerContext.fCurrentGroup = this;
}

StyledTextIOReader_RTF::ReaderContext::GroupContext::~GroupContext ()
{
    Assert (fReaderContext.fCurrentGroup == this); // cuz these always (even with exceptions) must be unwound in
    // reverse order of creation.
    fReaderContext.fCurrentGroup = fParentGroup;
    if (fReaderContext.fCurrentGroup != nullptr) {
        fReaderContext.UseInputCharSetEncoding (fReaderContext.fCurrentGroup->fCurrentCodePage);
        fReaderContext.GetDestination ().SetContext (fReaderContext.fCurrentGroup->fDestinationContext);
    }
}

RTFIO::ControlWordAtomName::ControlWordAtomName (const char* c)
//:fName ()
{
    Characters::CString::Copy (fName, eMaxControlAtomNameLen, c);
}

/*
 ********************************************************************************
 ******************************** StyledTextIOReader_RTF ************************
 ********************************************************************************
 */
const StyledTextIOReader_RTF::SpecialCharMappings StyledTextIOReader_RTF::kMappings[8] = {
    {RTFIO::eControlAtom_tab, 0x0009},
    {RTFIO::eControlAtom_bullet, 0x2022},
    {RTFIO::eControlAtom_endash, 0x2014},
    {RTFIO::eControlAtom_emdash, 0x2013},
    {RTFIO::eControlAtom_lquote, 0x2018},
    {RTFIO::eControlAtom_rquote, 0x2019},
    {RTFIO::eControlAtom_ldblquote, 0x201c},
    {RTFIO::eControlAtom_rdblquote, 0x201d},
};

StyledTextIOReader_RTF::StyledTextIOReader_RTF (SrcStream* srcStream, SinkStream* sinkStream, RTFInfo* rtfInfo)
    : StyledTextIOReader (srcStream, sinkStream)
    , fPlainFont ()
    , fDefaultUnsupportedCharacterChar (LED_TCHAR_OF ('?'))
    , fRTFInfo (rtfInfo == nullptr ? new RTFInfo () : rtfInfo)
    , fOwnRTFInfo (rtfInfo == nullptr)
#if qPlatform_Windows
    , fCachedFontSize (0)
    , fCachedFontSizeTMHeight (0)
#endif
{
    /*
     *  Set default value for 'plain font' - what is used when we encounter \plain
     *  wild freakin guess??? - Seems to make some files read better? Docs really unclear about this...
     *  After looking at some sample code, it looks like we SHOULD reset this to some predefined default.
     *  Unclear what that default should be???
     */
    fPlainFont.SetPointSize (12);
#if qPlatform_Windows
    fCachedFontSize         = 12;
    fCachedFontSizeTMHeight = fPlainFont.PeekAtTMHeight ();
#endif
}

StyledTextIOReader_RTF::~StyledTextIOReader_RTF ()
{
    if (fOwnRTFInfo) {
        delete fRTFInfo;
    }
}

void StyledTextIOReader_RTF::Read ()
{
    ReaderContext readerContext (*this);
    try {
        ReadGroup (readerContext);
    }
    catch (ReadEOFException& /*eof*/) {
        // Signifies un unimportant error - cruft past the end of file. Don't treat this as an
        // error - at least for now. Just eat it (I wonder if this is a mistake? - LGP 960827)
        // If it IS a mistake, we should replace this IGNORE with a 'Led_ThrowBadFormatDataException ();'
    }
    //readerContext.fDefaultDestination->Flush ();
    readerContext.fDefaultDestination->Done ();
}

bool StyledTextIOReader_RTF::QuickLookAppearsToBeRightFormat ()
{
    SrcStreamSeekSaver savePos (GetSrcStream ());

    const char kStandardRTFOpenString[] = "{\\rtf";
    char       buf[sizeof (kStandardRTFOpenString) - 1];
    size_t     bytesRead = GetSrcStream ().read (buf, sizeof (buf));
    return ((bytesRead == sizeof (kStandardRTFOpenString) - 1) and ::memcmp (kStandardRTFOpenString, buf, sizeof (kStandardRTFOpenString) - 1) == 0);
}

void StyledTextIOReader_RTF::ReadGroup (ReaderContext& readerContext)
{
    ReaderContext::GroupContext thisGroupContext (readerContext);

    if (GetNextChar () != RTFIO::kRTFOpenGroupChar) {
        HandleBadlyFormattedInput (true);
    }

    while (true) {
        char c = GetNextChar ();
        switch (c) {
            case RTFIO::kRTFOpenGroupChar: {
                // put open character back, and recursively read a group
                PutBackLastChar ();
                ReadGroup (readerContext);
            } break;

            case RTFIO::kRTFCloseGroupChar: {
                return; // end of group
            } break;

            case RTFIO::kRTFStartTagChar: {
                // put tag-start character back, and re-read the whole tag
                char nextChar = PeekNextChar ();

                switch (nextChar) {
                    // Though the RTF specs (1.4 - 1.7) clearly indicate that
                    // only lower case letters can begin a control word, spec version 1.7
                    // notes a dozen or so exceptions to this rule. WE should never WRITE any such
                    // control words, but for backward compatabilities sake - we must still
                    // be able to read them.
                    // SPR#
                    case 'A':
                    case 'B':
                    case 'C':
                    case 'D':
                    case 'E':
                    case 'F':
                    case 'G':
                    case 'H':
                    case 'I':
                    case 'J':
                    case 'K':
                    case 'L':
                    case 'M':
                    case 'N':
                    case 'O':
                    case 'P':
                    case 'Q':
                    case 'R':
                    case 'S':
                    case 'T':
                    case 'U':
                    case 'V':
                    case 'W':
                    case 'X':
                    case 'Y':
                    case 'Z':
                    case 'a':
                    case 'b':
                    case 'c':
                    case 'd':
                    case 'e':
                    case 'f':
                    case 'g':
                    case 'h':
                    case 'i':
                    case 'j':
                    case 'k':
                    case 'l':
                    case 'm':
                    case 'n':
                    case 'o':
                    case 'p':
                    case 'q':
                    case 'r':
                    case 's':
                    case 't':
                    case 'u':
                    case 'v':
                    case 'w':
                    case 'x':
                    case 'y':
                    case 'z': {
                        PutBackLastChar ();
                        RTFIO::ControlWord cw = ReadControlWord ();
                        if (HandleControlWord (readerContext, cw)) {
                            return; // end of group
                        }
                    } break;

                    case '|': {
                        ConsumeNextChar ();
                        const wchar_t kFormula = 0x0006;
#if qWideCharacters
                        Led_tChar cc = kFormula;
#else
                        CodePageConverter cvt (readerContext.GetCurrentOutputCharSetEncoding ());
                        Led_tChar cc = 0;
                        size_t nBytes = 1;
                        cvt.MapFromUNICODE (&kFormula, 1, &cc, &nBytes);
                        if (nBytes != 1) {
                            cc = fDefaultUnsupportedCharacterChar;
                        }
#endif
                        CheckIfAboutToStartBody (readerContext);
                        readerContext.GetDestination ().AppendText (&cc, 1);
                    } break;

                    case '~': {
                        ConsumeNextChar ();
#if qWideCharacters
                        Led_tChar cc = kNonBreakingSpace;
#else
                        CodePageConverter cvt (readerContext.GetCurrentOutputCharSetEncoding ());
                        Led_tChar cc = 0;
                        size_t nBytes = 1;
                        cvt.MapFromUNICODE (&kNonBreakingSpace, 1, &cc, &nBytes);
                        if (nBytes != 1) {
                            cc = fDefaultUnsupportedCharacterChar;
                        }
#endif
                        CheckIfAboutToStartBody (readerContext);
                        readerContext.GetDestination ().AppendText (&cc, 1);
                    } break;

                    case '-': {
                        ConsumeNextChar ();

                        const wchar_t kOptionalHyphen = 0x00AD; //  RTF 1.5 spec says "Optional Hyphen". This is the character from the UNICODE spec labeled "Soft Hyphen"
                                                                //  that was the closest match I could find
#if qWideCharacters
                        Led_tChar cc = kOptionalHyphen;
#else
                        CodePageConverter cvt (readerContext.GetCurrentOutputCharSetEncoding ());
                        Led_tChar cc = 0;
                        size_t nBytes = 1;
                        cvt.MapFromUNICODE (&kOptionalHyphen, 1, &cc, &nBytes);
                        if (nBytes != 1) {
                            cc = fDefaultUnsupportedCharacterChar;
                        }
#endif
                        CheckIfAboutToStartBody (readerContext);
                        readerContext.GetDestination ().AppendText (&cc, 1);
                    } break;

                    case '_': {
                        ConsumeNextChar ();
                        const wchar_t kNonBreakingHyphen = 0x2011;
#if qWideCharacters
                        Led_tChar cc = kNonBreakingHyphen;
#else
                        CodePageConverter cvt (readerContext.GetCurrentOutputCharSetEncoding ());
                        Led_tChar cc = 0;
                        size_t nBytes = 1;
                        cvt.MapFromUNICODE (&kNonBreakingHyphen, 1, &cc, &nBytes);
                        if (nBytes != 1) {
                            cc = fDefaultUnsupportedCharacterChar;
                        }
#endif
                        CheckIfAboutToStartBody (readerContext);
                        readerContext.GetDestination ().AppendText (&cc, 1);
                    } break;

                    case ':': {
                        //???? subentry in an index???- not sure what todo but ignore??? No char to insert?
                    } break;

                    case '*': {
                        ReadCommentGroup (readerContext); // consumes end of group
                        return;                           // end of group
                    } break;

                    case '\'': {
                        ConsumeNextChar ();

                        int number = ConvertReadSingleHexDigit (GetNextChar ());
                        number *= 16;
                        number += ConvertReadSingleHexDigit (GetNextChar ());
                        c = number;
                        goto ReadNormalChar;
                    } break;

                    default: {
                        // Then this is a quoted quote-char, and we should consume the first, and read
                        // as a normal character the second
                        ConsumeNextChar ();
                        c = nextChar;
                        goto ReadNormalChar;
                    } break;
                }
            };
                break;

            case '\n':
            case '\r': {
                // ignored??
            } break;

            default: {
            ReadNormalChar:
                CheckIfAboutToStartBody (readerContext);
                // Other characters simply get inserted into the current RTF destination
                readerContext.PutRawCharToDestination (c);
            } break;
        }
    }
}

bool StyledTextIOReader_RTF::HandleControlWord (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    switch (controlWord.fWord) {
        case RTFIO::eControlAtom_ansi:
            return (HandleControlWord_ansi (readerContext, controlWord));
        case RTFIO::eControlAtom_author:
            return (HandleControlWord_author (readerContext, controlWord));
        case RTFIO::eControlAtom_b:
            return (HandleControlWord_b (readerContext, controlWord));
        case RTFIO::eControlAtom_brdrs:
        case RTFIO::eControlAtom_brdrth:
        case RTFIO::eControlAtom_brdrsh:
        case RTFIO::eControlAtom_brdrdb:
        case RTFIO::eControlAtom_brdrdot:
        case RTFIO::eControlAtom_brdrdash:
        case RTFIO::eControlAtom_brdrhair:
        case RTFIO::eControlAtom_brdrdashsm:
        case RTFIO::eControlAtom_brdrdashd:
        case RTFIO::eControlAtom_brdrdashdd:
        case RTFIO::eControlAtom_brdrtriple:
        case RTFIO::eControlAtom_brdrtnthsg:
        case RTFIO::eControlAtom_brdrthtnsg:
        case RTFIO::eControlAtom_brdrtnthtnsg:
        case RTFIO::eControlAtom_brdrtnthmg:
        case RTFIO::eControlAtom_brdrthtnmg:
        case RTFIO::eControlAtom_brdrtnthtnmg:
        case RTFIO::eControlAtom_brdrtnthlg:
        case RTFIO::eControlAtom_brdrthtnlg:
        case RTFIO::eControlAtom_brdrtnthtnlg:
        case RTFIO::eControlAtom_brdrwavy:
        case RTFIO::eControlAtom_brdrwavydb:
        case RTFIO::eControlAtom_brdrdashdotstr:
        case RTFIO::eControlAtom_brdremboss:
        case RTFIO::eControlAtom_brdrengrave:
        case RTFIO::eControlAtom_brdrw:
        case RTFIO::eControlAtom_brdrcf:
            return (HandleControlWord_brdrXXX (readerContext, controlWord));
        case RTFIO::eControlAtom_cchs:
            return (HandleControlWord_cchs (readerContext, controlWord));
        case RTFIO::eControlAtom_cell:
            return (HandleControlWord_cell (readerContext, controlWord));
        case RTFIO::eControlAtom_cellx:
            return (HandleControlWord_cellx (readerContext, controlWord));
        case RTFIO::eControlAtom_cf:
            return (HandleControlWord_cf (readerContext, controlWord));
        case RTFIO::eControlAtom_clcbpat:
            return (HandleControlWord_clcbpat (readerContext, controlWord));
        case RTFIO::eControlAtom_cpg:
            return (HandleControlWord_cpg (readerContext, controlWord));
        case RTFIO::eControlAtom_colortbl:
            return (HandleControlWord_colortbl (readerContext, controlWord));
        case RTFIO::eControlAtom_deff:
            return (HandleControlWord_deff (readerContext, controlWord));
        case RTFIO::eControlAtom_deftab:
            return (HandleControlWord_deftab (readerContext, controlWord));
        case RTFIO::eControlAtom_deleted:
            return (HandleControlWord_deleted (readerContext, controlWord));
        case RTFIO::eControlAtom_dn:
            return (HandleControlWord_dn (readerContext, controlWord));
        case RTFIO::eControlAtom_f:
            return (HandleControlWord_f (readerContext, controlWord));
        case RTFIO::eControlAtom_fi:
            return (HandleControlWord_fi (readerContext, controlWord));
        case RTFIO::eControlAtom_footer:
            return (HandleControlWord_footer (readerContext, controlWord));
        case RTFIO::eControlAtom_fonttbl:
            return (HandleControlWord_fonttbl (readerContext, controlWord));
        case RTFIO::eControlAtom_fs:
            return (HandleControlWord_fs (readerContext, controlWord));
        case RTFIO::eControlAtom_header:
            return (HandleControlWord_header (readerContext, controlWord));
        case RTFIO::eControlAtom_i:
            return (HandleControlWord_i (readerContext, controlWord));
        case RTFIO::eControlAtom_ilvl:
            return (HandleControlWord_ilvl (readerContext, controlWord));
        case RTFIO::eControlAtom_info:
            return (HandleControlWord_info (readerContext, controlWord));
        case RTFIO::eControlAtom_intbl:
            return (HandleControlWord_intbl (readerContext, controlWord));
        case RTFIO::eControlAtom_li:
            return (HandleControlWord_li (readerContext, controlWord));
        case RTFIO::eControlAtom_line:
            return (HandleControlWord_line (readerContext, controlWord));
        case RTFIO::eControlAtom_listtext:
            return (HandleControlWord_listtext (readerContext, controlWord));
        case RTFIO::eControlAtom_ls:
            return (HandleControlWord_ls (readerContext, controlWord));
        case RTFIO::eControlAtom_mac:
            return (HandleControlWord_mac (readerContext, controlWord));
        case RTFIO::eControlAtom_margb:
        case RTFIO::eControlAtom_margl:
        case RTFIO::eControlAtom_margr:
        case RTFIO::eControlAtom_margt:
            return (HandleControlWord_margX (readerContext, controlWord));
        case RTFIO::eControlAtom_object:
            return (HandleControlWord_object (readerContext, controlWord));
        case RTFIO::eControlAtom_outl:
            return (HandleControlWord_outl (readerContext, controlWord));
        case RTFIO::eControlAtom_paperh:
            return (HandleControlWord_paperX (readerContext, controlWord));
        case RTFIO::eControlAtom_paperw:
            return (HandleControlWord_paperX (readerContext, controlWord));
        case RTFIO::eControlAtom_par:
            return (HandleControlWord_par (readerContext, controlWord));
        case RTFIO::eControlAtom_pard:
            return (HandleControlWord_pard (readerContext, controlWord));
        case RTFIO::eControlAtom_pc:
            return (HandleControlWord_pc (readerContext, controlWord));
        case RTFIO::eControlAtom_pca:
            return (HandleControlWord_pca (readerContext, controlWord));
        case RTFIO::eControlAtom_pict:
            return (HandleControlWord_pict (readerContext, controlWord));
        case RTFIO::eControlAtom_plain:
            return (HandleControlWord_plain (readerContext, controlWord));
        case RTFIO::eControlAtom_pntext:
            return (HandleControlWord_pntext (readerContext, controlWord));
        case RTFIO::eControlAtom_qc:
            return (HandleControlWord_qc (readerContext, controlWord));
        case RTFIO::eControlAtom_qj:
            return (HandleControlWord_qj (readerContext, controlWord));
        case RTFIO::eControlAtom_ql:
            return (HandleControlWord_ql (readerContext, controlWord));
        case RTFIO::eControlAtom_qr:
            return (HandleControlWord_qr (readerContext, controlWord));
        case RTFIO::eControlAtom_ri:
            return (HandleControlWord_ri (readerContext, controlWord));
        case RTFIO::eControlAtom_row:
            return (HandleControlWord_row (readerContext, controlWord));
        case RTFIO::eControlAtom_rtf:
            return (HandleControlWord_rtf (readerContext, controlWord));
        case RTFIO::eControlAtom_sa:
            return (HandleControlWord_sa (readerContext, controlWord));
        case RTFIO::eControlAtom_sb:
            return (HandleControlWord_sb (readerContext, controlWord));
        case RTFIO::eControlAtom_shad:
            return (HandleControlWord_shad (readerContext, controlWord));
        case RTFIO::eControlAtom_sl:
            return (HandleControlWord_sl (readerContext, controlWord));
        case RTFIO::eControlAtom_slmult:
            return (HandleControlWord_slmult (readerContext, controlWord));
        case RTFIO::eControlAtom_sub:
            return (HandleControlWord_sub (readerContext, controlWord));
        case RTFIO::eControlAtom_super:
            return (HandleControlWord_super (readerContext, controlWord));
        case RTFIO::eControlAtom_strike:
            return (HandleControlWord_strike (readerContext, controlWord));
        case RTFIO::eControlAtom_stylesheet:
            return (HandleControlWord_stylesheet (readerContext, controlWord));
        case RTFIO::eControlAtom_trgaph:
            return (HandleControlWord_trgaph (readerContext, controlWord));
        case RTFIO::eControlAtom_trleft:
            return (HandleControlWord_trleft (readerContext, controlWord));
        case RTFIO::eControlAtom_trowd:
            return (HandleControlWord_trowd (readerContext, controlWord));
        case RTFIO::eControlAtom_trpaddb:
        case RTFIO::eControlAtom_trpaddl:
        case RTFIO::eControlAtom_trpaddr:
        case RTFIO::eControlAtom_trpaddt:
            return (HandleControlWord_trpaddX (readerContext, controlWord));
        case RTFIO::eControlAtom_trspdb:
        case RTFIO::eControlAtom_trspdl:
        case RTFIO::eControlAtom_trspdr:
        case RTFIO::eControlAtom_trspdt:
            return (HandleControlWord_trspdX (readerContext, controlWord));
        case RTFIO::eControlAtom_tx:
            return (HandleControlWord_tx (readerContext, controlWord));
        case RTFIO::eControlAtom_u:
            return (HandleControlWord_u (readerContext, controlWord));
        case RTFIO::eControlAtom_uc:
            return (HandleControlWord_uc (readerContext, controlWord));
        case RTFIO::eControlAtom_ul:
            return (HandleControlWord_ul (readerContext, controlWord));
        case RTFIO::eControlAtom_ulnone:
            return (HandleControlWord_ulnone (readerContext, controlWord));
        case RTFIO::eControlAtom_up:
            return (HandleControlWord_up (readerContext, controlWord));
        case RTFIO::eControlAtom_v:
            return (HandleControlWord_v (readerContext, controlWord));

        default:
            return (HandleControlWord_UnknownControlWord (readerContext, controlWord));
    }
}

bool StyledTextIOReader_RTF::HandleControlWord_ansi (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    readerContext.fDocumentCharacterSet = kCodePage_ANSI;
    readerContext.UseInputCharSetEncoding (readerContext.fDocumentCharacterSet);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_author (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& /*controlWord*/)
{
    SkipToEndOfCurrentGroup (); // we ignore unknown groups
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_b (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_brdrXXX (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    switch (controlWord.fWord) {
        case RTFIO::eControlAtom_brdrcf: {
            if (not controlWord.fHasArg) {
                HandleBadlyFormattedInput (true); // must have a numeric color-number argument
            }
            readerContext.GetDestination ().SetTableBorderColor (LookupColor (readerContext, static_cast<size_t> (controlWord.fValue)));
        } break;
    }
    // CHECK brdrw and brdrcf
    //CHECK FOR VARIOUS TABLE BORDER ATTRIBUTES
    //\brdrs
    //\brdrth
    //brdrtriple

    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_cchs (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    CheckIfAboutToStartBody (readerContext);
    if (readerContext.GetCurrentGroupContext () == nullptr) {
        HandleBadlyFormattedInput (true); // cannot charset
    }
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (true); // must have a numeric font-size argument
    }

    switch (controlWord.fValue) {
        case 0:
            readerContext.GetCurrentGroupContext ()->fCCHSCodePage = kCodePage_ANSI;
            break;
        case 254:
            readerContext.GetCurrentGroupContext ()->fCCHSCodePage = kCodePage_PC;
            break;
        case 255:
            readerContext.GetCurrentGroupContext ()->fCCHSCodePage = kCodePage_PCA;
            break;
        case 256:
            readerContext.GetCurrentGroupContext ()->fCCHSCodePage = kCodePage_MAC;
            break;
        default: {
            // unsure what todo here. We could throw an error, but I suspect its probably best to
            // simply IGNORE the attempted code page setting.
            // ???? LGP 960903
        } break;
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_cell (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    CheckIfAboutToStartBody (readerContext);
    readerContext.GetDestination ().EndCell (true);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_cellx (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    CheckIfAboutToStartBody (readerContext);
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (true); // must have a numeric code page argument
    }
    readerContext.GetDestination ().SetCellX (Led_TWIPS (controlWord.fValue));
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_cf (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_clcbpat (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    CheckIfAboutToStartBody (readerContext);
    readerContext.GetDestination ().SetCellBackColor (LookupColor (readerContext, static_cast<size_t> (controlWord.fValue)));
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_cpg (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    CheckIfAboutToStartBody (readerContext);
    if (readerContext.GetCurrentGroupContext () == nullptr) {
        HandleBadlyFormattedInput (true); // cannot charset
    }
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (true); // must have a numeric code page argument
    }
    readerContext.GetCurrentGroupContext ()->fCurrentCodePage = controlWord.fValue;
    readerContext.UseInputCharSetEncoding (readerContext.GetCurrentGroupContext ()->fCurrentCodePage);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_colortbl (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    if (readerContext.fColorTable != nullptr) {
        HandleBadlyFormattedInput (); // cannot have two color tables...
    }

    /*
     *  Now try to really read color table.
     *
     *  Start just inside the group, and look for each sub-group. And assume each subgroup is
     *  a font specification, and read it as such.
     */
    if (readerContext.GetCurrentGroupContext () == nullptr) {
        HandleBadlyFormattedInput (true);
    }
    GetSrcStream ().seek_to (readerContext.GetCurrentGroupContext ()->fCurrentGroupStartIdx);
    if (GetNextChar () != RTFIO::kRTFOpenGroupChar) {
        HandleBadlyFormattedInput (true);
    }

    {
        RTFIO::ControlWord cword = ReadControlWord ();
        if (cword.fWord != RTFIO::eControlAtom_colortbl or cword.fHasArg) {
            HandleBadlyFormattedInput (true);
        }
    }

    vector<Led_Color> colorTable;
    while (true) {
        Led_Color curColor = Led_Color::kBlack;

    // Read \\redN
    ReadRed:
        switch (PeekNextChar ()) {
            case RTFIO::kRTFCloseGroupChar: {
                ConsumeNextChar ();
                goto ColorsComplete;
            } break;
            case ';': {
                ConsumeNextChar ();
                goto ColorComplete;
            } break;
            case RTFIO::kRTFStartTagChar: {
                RTFIO::ControlWord cword = ReadControlWord ();
                if (cword.fWord == RTFIO::eControlAtom_red) {
                    if (not cword.fHasArg) {
                        HandleBadlyFormattedInput ();
                    }
                    else {
                        curColor = Led_Color (static_cast<Led_Color::ColorValue> (unsigned(cword.fValue) << 8), 0, 0);
                    }
                }
                else {
                    HandleBadlyFormattedInput ();
                }
            } break;
            case '\r':
            case '\n':
            case ' ': {
                ConsumeNextChar ();
                goto ReadRed;
            } break;
            default: {
                HandleBadlyFormattedInput ();
            }
        }

    // Read \\greenN
    ReadGreen:
        switch (PeekNextChar ()) {
            case ';': {
                ConsumeNextChar ();
                goto ColorComplete;
            } break;
            case RTFIO::kRTFStartTagChar: {
                RTFIO::ControlWord cword = ReadControlWord ();
                if (cword.fWord == RTFIO::eControlAtom_green) {
                    if (not cword.fHasArg) {
                        HandleBadlyFormattedInput ();
                    }
                    else {
                        curColor = Led_Color (curColor.GetRed (), static_cast<Led_Color::ColorValue> (unsigned(cword.fValue) << 8), 0);
                    }
                }
                else {
                    HandleBadlyFormattedInput ();
                }
            } break;
            case '\r':
            case '\n':
            case ' ': {
                ConsumeNextChar ();
                goto ReadGreen;
            } break;
            default: {
                HandleBadlyFormattedInput ();
            }
        }

    // Read \\blueN
    ReadBlue:
        switch (PeekNextChar ()) {
            case ';': {
                ConsumeNextChar ();
                goto ColorComplete;
            } break;
            case RTFIO::kRTFStartTagChar: {
                RTFIO::ControlWord cword = ReadControlWord ();
                if (cword.fWord == RTFIO::eControlAtom_blue) {
                    if (not cword.fHasArg) {
                        HandleBadlyFormattedInput ();
                    }
                    else {
                        curColor = Led_Color (curColor.GetRed (), curColor.GetGreen (), static_cast<Led_Color::ColorValue> (unsigned(cword.fValue) << 8));
                    }
                }
                else {
                    HandleBadlyFormattedInput ();
                }
            } break;
            case '\r':
            case '\n':
            case ' ': {
                ConsumeNextChar ();
                goto ReadBlue;
            } break;
            default: {
                HandleBadlyFormattedInput ();
            }
        }

    // Read ';'
    ReadSemiColon:
        switch (PeekNextChar ()) {
            case ';': {
                ConsumeNextChar ();
                goto ColorComplete;
            } break;
            case ' ': {
                ConsumeNextChar ();
                goto ReadSemiColon;
            } break;
            default: {
                HandleBadlyFormattedInput ();
                ConsumeNextChar (); // in case the above doesn't throw - don't get caught in an infinite loop...
            }
        }

    ColorComplete:
        colorTable.push_back (curColor);
    }

ColorsComplete:
    Assert (readerContext.fColorTable == nullptr);
    readerContext.fColorTable = new RTFIO::ColorTable (colorTable);

    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_deff (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput ();
    }
    else {
        readerContext.fDefaultFontNumber = controlWord.fValue;
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_deftab (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput ();
        return false;
    }
    if (controlWord.fValue < 0) {
        HandleBadlyFormattedInput ();
        return false;
    }
    Led_TWIPS tsl = Led_TWIPS (controlWord.fValue);
    if (tsl == 0) {
        HandleBadlyFormattedInput ();
        return false;
    }
    if (fRTFInfo != nullptr) {
        fRTFInfo->fDefaultTabStop = tsl;
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_deleted (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& /*controlWord*/)
{
    /*
     *  Groups marked as deleted, we should not read in any of the text. Eventually, we
     *  might want to keep track of this stuff if we ever supported changebars etc.
     */
    SkipToEndOfCurrentGroup ();
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_dn (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    /*
     *  Kludge/workaround for SPR#1620.
     */
    if (controlWord.fHasArg and controlWord.fValue > 0) {
        RTFIO::ControlWord newCW;
        newCW.fHasArg = false;
        newCW.fWord   = RTFIO::eControlAtom_sub;
        return HandleControlWord_sub (readerContext, newCW);
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_f (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_fi (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
    }
    else {
        readerContext.GetDestination ().SetFirstIndent (readerContext.GetCurrentGroupContext ()->fDestinationContext.fFirstIndent = Led_TWIPS (controlWord.fValue));
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_footer (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& /*controlWord*/)
{
    /*
     *  We ignore these for now. In Led 2.3 - we would treat them as plain text - part of the document. At least then you would SEE
     *  them. But they came out in the wrong place and looked terrible. On balance - this is probably better.
     */
    SkipToEndOfCurrentGroup ();
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_fonttbl (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    if (readerContext.fFontTable != nullptr) {
        HandleBadlyFormattedInput (); // cannot have two font tables...
    }

    /*
     *  Now try to really read font table.
     *
     *  Start just inside the group, and look for each sub-group. And assume each subgroup is
     *  a font specification, and read it as such.
     */
    if (readerContext.GetCurrentGroupContext () == nullptr) {
        HandleBadlyFormattedInput (true);
    }
    GetSrcStream ().seek_to (readerContext.GetCurrentGroupContext ()->fCurrentGroupStartIdx);
    if (GetNextChar () != RTFIO::kRTFOpenGroupChar) {
        HandleBadlyFormattedInput (true);
    }

    vector<FontTableEntry> fontTable;

    while (true) {
        ScanForwardFor ("{}");
        switch (PeekNextChar ()) {
            case RTFIO::kRTFOpenGroupChar: {
                fontTable.push_back (ReadInFontTablesEntry ());
            } break;
            case RTFIO::kRTFCloseGroupChar: {
                ConsumeNextChar ();
                if (readerContext.fFontTable == nullptr) {
                    // ignore second font table - if there are multiples and we didn't throw out of here...
                    readerContext.fFontTable = new FontTable (fontTable);
                }
                return true; // ALL DONE
            } break;
            default: {
                Assert (false);
            } break;
        }
    }
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_fs (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_header (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& /*controlWord*/)
{
    /*
     *  We ignore these for now. In Led 2.3 - we would treat them as plain text - part of the document. At least then you would SEE
     *  them. But they came out in the wrong place and looked terrible. On balance - this is probably better.
     */
    SkipToEndOfCurrentGroup ();
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_i (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_ilvl (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
    }
    else {
        unsigned char listLevel = static_cast<unsigned char> (controlWord.fValue);
        if (listLevel > 8) {
            listLevel = 0;
        }
        readerContext.GetDestination ().SetListIndentLevel (readerContext.GetCurrentGroupContext ()->fDestinationContext.fListIndentLevel = listLevel);
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_info (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& /*controlWord*/)
{
    SkipToEndOfCurrentGroup (); // we ignore unknown groups
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_intbl (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    CheckIfAboutToStartBody (readerContext);
    if (readerContext.GetCurrentGroupContext () == nullptr) {
        HandleBadlyFormattedInput (true); // cannot set INTABLE without a current group!
    }
    readerContext.GetDestination ().SetInTable (true);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_li (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
    }
    else {
        readerContext.GetDestination ().SetLeftMargin (readerContext.GetCurrentGroupContext ()->fDestinationContext.fLeftMargin = Led_TWIPS (controlWord.fValue));
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_line (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    readerContext.GetDestination ().AppendSoftLineBreak ();
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_listtext (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& /*controlWord*/)
{
    SkipToEndOfCurrentGroup (); // we ignore this because we properly handle the lists already
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_ls (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
    }
    else {
        /*
         * TmpHack - really should read this style from the listtable based on given ID
         */
        ListStyle listStyle = eListStyle_Bullet;
        readerContext.GetDestination ().SetListStyle (readerContext.GetCurrentGroupContext ()->fDestinationContext.fListStyle = listStyle);
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_mac (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    readerContext.fDocumentCharacterSet = kCodePage_MAC;
    readerContext.UseInputCharSetEncoding (readerContext.fDocumentCharacterSet);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_margX (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
        return false;
    }
    AssertNotNull (fRTFInfo);
    switch (controlWord.fWord) {
        case RTFIO::eControlAtom_margt: {
            fRTFInfo->fDefaultMarginTop = Led_TWIPS (controlWord.fValue);
        } break;
        case RTFIO::eControlAtom_margb: {
            fRTFInfo->fDefaultMarginBottom = Led_TWIPS (controlWord.fValue);
        } break;
        case RTFIO::eControlAtom_margl: {
            fRTFInfo->fDefaultMarginLeft = Led_TWIPS (controlWord.fValue);
        } break;
        case RTFIO::eControlAtom_margr: {
            fRTFInfo->fDefaultMarginRight = Led_TWIPS (controlWord.fValue);
        } break;
        default: {
            Assert (false);
        }
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_outl (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_paperX (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
        return false;
    }
    AssertNotNull (fRTFInfo);
    switch (controlWord.fWord) {
        case RTFIO::eControlAtom_paperh: {
            fRTFInfo->fDefaultPaperSize.v = Led_TWIPS (controlWord.fValue);
        } break;
        case RTFIO::eControlAtom_paperw: {
            fRTFInfo->fDefaultPaperSize.h = Led_TWIPS (controlWord.fValue);
        } break;
        default: {
            Assert (false);
        }
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_object (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    CheckIfAboutToStartBody (readerContext);
    using UnknownRTFEmbedding = RTFIO::UnknownRTFEmbedding;
    using ControlWord         = RTFIO::ControlWord;
    /*
     *  Now try to really read on object in. Could be a Led-private-format object. Could be an OLE object. Or one of many
     *  types we don't support.
     *
     *  For ones we don't support, we SHOULD use the \result tag - if any - to display the thing reasonably. But for now,
     *  we don't support that. We simply ignore \result tags (see RTF spec for details).
     */

    /*
     *  After the \object tag comes the <objtype>. If this is \objemb, then we have an OLE ombedding, and we'll try to read
     *  that in. If its \ledprivateobjectembeddingformat, then we will try to read that. Anything else, we don't understand, and
     *  we will try to read it is an 'UnknownRTFEmbedding'.
     */
    bool            isOLEEmbedding        = false;
    bool            isPrivateLedEmbedding = false;
    Led_TWIPS_Point shownSize             = Led_TWIPS_Point (Led_TWIPS (0), Led_TWIPS (0));
    vector<char>    objData;
    float           scaleX        = 1.0f;
    float           scaleY        = 1.0f;
    size_t          resultFoundAt = size_t (-1);

    while (true) {
        // Look for either control words, or groups
        if (PeekNextChar () != RTFIO::kRTFStartTagChar) {
            ScanForwardFor ("{}");
        }
        switch (PeekNextChar ()) {
            case RTFIO::kRTFStartTagChar: {
                ControlWord cw = ReadControlWord ();
                switch (cw.fWord) {
                    case RTFIO::eControlAtom_ledprivateobjectembeddingformat:
                        isPrivateLedEmbedding = true;
                        break;
                    case RTFIO::eControlAtom_objemb:
                        isOLEEmbedding = true;
                        break;
                    case RTFIO::eControlAtom_objh:
                        if (not cw.fHasArg) {
                            HandleBadlyFormattedInput ();
                            break;
                        };
                        shownSize.v = Led_TWIPS (cw.fValue);
                        break;
                    case RTFIO::eControlAtom_objw:
                        if (not cw.fHasArg) {
                            HandleBadlyFormattedInput ();
                            break;
                        };
                        shownSize.h = Led_TWIPS (cw.fValue);
                        break;
                    case RTFIO::eControlAtom_objscalex:
                        if (not cw.fHasArg) {
                            HandleBadlyFormattedInput ();
                            break;
                        };
                        scaleX = cw.fValue / 100.0f;
                        break;
                    case RTFIO::eControlAtom_objscaley:
                        if (not cw.fHasArg) {
                            HandleBadlyFormattedInput ();
                            break;
                        };
                        scaleY = cw.fValue / 100.0f;
                        break;
                    case RTFIO::eControlAtom_result:
                        resultFoundAt = readerContext.GetReader ().GetSrcStream ().current_offset ();
                        break;
                }
            } break;
            case RTFIO::kRTFOpenGroupChar: {
                ReadInObjectSubGroupEntry (readerContext, &objData, &resultFoundAt);
            } break;
            case RTFIO::kRTFCloseGroupChar: {
                /*
                     * Sanity check shown size.
                     */
                shownSize.h *= scaleX;
                shownSize.v *= scaleY;
                if (shownSize.v > 20000 or shownSize.h > 20000 or shownSize.h < 100 or shownSize.v < 100) {
                    shownSize = UnknownRTFEmbedding::CalcStaticDefaultShownSize ();
                }

                /*
                     *  First, try to construct the specific kind of object using the info we've extracted.
                     */
                if (isOLEEmbedding and isPrivateLedEmbedding) {
                    HandleBadlyFormattedInput ();
                }
                if (isOLEEmbedding) {
                    try {
                        ConstructOLEEmebddingFromRTFInfo (readerContext, shownSize, objData.size (), &objData.front ());
                    }
                    catch (...) {
                        isOLEEmbedding = false;
                    }
                }
                if (isPrivateLedEmbedding) {
                    try {
                        ConstructLedEmebddingFromRTFInfo (readerContext, objData.size (), &objData.front ());
                    }
                    catch (...) {
                        isPrivateLedEmbedding = false;
                    }
                }

                /*
                     *  If we succeded, cleanup, and we're done.
                     */
                if (isOLEEmbedding or isPrivateLedEmbedding) {
                    ConsumeNextChar ();
                }
                else {
                    // make a fake-embedding object for what we couldn't read in.
                    GetSrcStream ().seek_to (readerContext.GetCurrentGroupContext ()->fCurrentGroupStartIdx);
                    string s = ReadInGroupAndSave ();

                    SimpleEmbeddedObjectStyleMarker* embedding = nullptr;
                    /*
                         *  If there is a result tag and a PICT we can read in it - great. Use that as an arg to the UnknownRTFEmbedding ().
                         *  Otherwise - create one without the DIB/PICT.
                         */
                    if (resultFoundAt != size_t (-1)) {
                        size_t RETURN_TO = readerContext.GetReader ().GetSrcStream ().current_offset ();
                        GetSrcStream ().seek_to (resultFoundAt);
                        if (SearchForwardFor ("\\pict", s.length ())) {
                            ControlWord cw = ReadControlWord ();
                            if (cw.fWord == RTFIO::eControlAtom_pict) {
                                Led_TWIPS_Point shownSize = Led_TWIPS_Point (Led_TWIPS (0), Led_TWIPS (0));
                                Led_TWIPS_Point bmSize    = Led_TWIPS_Point (Led_TWIPS (0), Led_TWIPS (0));
                                vector<char>    objData;
                                ImageFormat     imageFormat = eDefaultImageFormat;
                                ReadTopLevelPictData (&shownSize, &imageFormat, &bmSize, &objData);
                                /*
                                 *  For now - simply convert whatever sort of data it is to DIB data, and then create a DIB embedding.
                                 *  In the future - we may want to keep the original Data - in some cases - like for enhanced-meta-files etc. Then here - read the data another way,
                                 *  and convert another sort of embedding object. The reason we might want to make that change is that the conversion from
                                 *  meta-file data to DIB data can be lossy, and can make the data size much larger - both bad things.
                                 *      --LGP 2000-07-08
                                 */
                                unique_ptr<Led_DIB> dib = unique_ptr<Led_DIB> (ConstructDIBFromData (shownSize, imageFormat, bmSize, objData.size (), &objData.front ()));
                                if (dib.get () != nullptr) {
                                    RTFIO::UnknownRTFEmbedding* e = new RTFIO::UnknownRTFEmbedding (RTFIO::kRTFBodyGroupFragmentClipFormat, RTFIO::kRTFBodyGroupFragmentEmbeddingTag, s.c_str (), s.length (), dib.get ());
                                    e->SetShownSize (shownSize);
                                    embedding = e;
                                }
                            }
                        }
                        GetSrcStream ().seek_to (RETURN_TO);
                    }

                    if (embedding == nullptr) {
                        RTFIO::UnknownRTFEmbedding* e = new RTFIO::UnknownRTFEmbedding (RTFIO::kRTFBodyGroupFragmentClipFormat, RTFIO::kRTFBodyGroupFragmentEmbeddingTag, s.c_str (), s.length ());
                        e->SetShownSize (shownSize);
                        embedding = e;
                    }
                    try {
                        readerContext.GetDestination ().AppendEmbedding (embedding);
                    }
                    catch (...) {
                        delete embedding;
                        throw;
                    }
                }
                return true; // ALL DONE
            } break;
            default: {
                Assert (false);
            } break;
        }
    }
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_par (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    /*
     *  According to RTF spec, \par ENDS a paragraph. This very nearly is accomplished by just emitting a newline in Led. But -
     *  for the special case of the last paragraph in a document - that doesn't work. In that case - Led would produce an extra
     *  empty paragraph at the end of the document (SinkStream output). So - we must be more careful.
     */
    CheckIfAboutToStartBody (readerContext);
    readerContext.GetDestination ().EndParagraph ();
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_pard (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    // Reset ALL defaults here
    // Assign to current context AND destination at the same time...
    readerContext.GetDestination ().SetTabStops (readerContext.GetCurrentGroupContext ()->fDestinationContext.fTabStops = TextImager::StandardTabStopList (GetRTFInfo ().GetDefaultTabStop ()));
    readerContext.GetDestination ().SetJustification (readerContext.GetCurrentGroupContext ()->fDestinationContext.fJustification = eLeftJustify);
    readerContext.GetDestination ().SetFirstIndent (readerContext.GetCurrentGroupContext ()->fDestinationContext.fFirstIndent = Led_TWIPS (0));
    readerContext.GetDestination ().SetLeftMargin (readerContext.GetCurrentGroupContext ()->fDestinationContext.fLeftMargin = Led_TWIPS (0));
    readerContext.GetDestination ().SetRightMargin (readerContext.GetCurrentGroupContext ()->fDestinationContext.fRightMargin = Led_TWIPS (0));
    readerContext.GetDestination ().SetSpaceBefore (readerContext.GetCurrentGroupContext ()->fDestinationContext.fSpaceBefore = Led_TWIPS (0));
    readerContext.GetDestination ().SetSpaceAfter (readerContext.GetCurrentGroupContext ()->fDestinationContext.fSpaceAfter = Led_TWIPS (0));
    readerContext.GetDestination ().SetSpaceBetweenLines (readerContext.GetCurrentGroupContext ()->fDestinationContext.fSpaceBetweenLines = 1000);
    readerContext.GetDestination ().SetSpaceBetweenLinesMult (readerContext.GetCurrentGroupContext ()->fDestinationContext.fSpaceBetweenLinesMult = true);
    readerContext.GetDestination ().SetListStyle (readerContext.GetCurrentGroupContext ()->fDestinationContext.fListStyle = eListStyle_None);
    readerContext.GetDestination ().SetListIndentLevel (readerContext.GetCurrentGroupContext ()->fDestinationContext.fListIndentLevel = 0);

    // as part of debugging SPR#1406- I saw that \pard should be interpretted as resetting this flag to off
    // test carefully that doesn't break anything...
    readerContext.GetDestination ().SetInTable (false);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_pc (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    readerContext.fDocumentCharacterSet = kCodePage_PC;
    readerContext.UseInputCharSetEncoding (readerContext.fDocumentCharacterSet);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_pca (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    readerContext.fDocumentCharacterSet = kCodePage_PCA;
    readerContext.UseInputCharSetEncoding (readerContext.fDocumentCharacterSet);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_pict (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    CheckIfAboutToStartBody (readerContext);

    Led_TWIPS_Point shownSize = Led_TWIPS_Point (Led_TWIPS (0), Led_TWIPS (0));
    Led_TWIPS_Point bmSize    = Led_TWIPS_Point (Led_TWIPS (0), Led_TWIPS (0));
    vector<char>    objData;
    ImageFormat     imageFormat = eDefaultImageFormat;
    ReadTopLevelPictData (&shownSize, &imageFormat, &bmSize, &objData);

    /*
     *  For now - simply convert whatever sort of data it is to DIB data, and then create a DIB embedding.
     *  In the future - we may want to keep the original Data - in some cases - like for enhanced-meta-files etc. Then here - read the data another way,
     *  and convert another sort of embedding object. The reason we might want to make that change is that the conversion from
     *  meta-file data to DIB data can be lossy, and can make the data size much larger - both bad things.
     *      --LGP 2000-07-08
     */
    unique_ptr<Led_DIB>              dib             = unique_ptr<Led_DIB> (ConstructDIBFromData (shownSize, imageFormat, bmSize, objData.size (), &objData.front ()));
    bool                             createSucceeded = dib.get () != nullptr;
    SimpleEmbeddedObjectStyleMarker* embedding       = nullptr;
    if (createSucceeded) {
        embedding = new StandardDIBStyleMarker (dib.get ());
    }
    else {
        // make a fake-embedding object for what we couldn't read in.
        GetSrcStream ().seek_to (readerContext.GetCurrentGroupContext ()->fCurrentGroupStartIdx);
        string s  = ReadInGroupAndSave ();
        embedding = new RTFIO::UnknownRTFEmbedding (RTFIO::kRTFBodyGroupFragmentClipFormat, RTFIO::kRTFBodyGroupFragmentEmbeddingTag, s.c_str (), s.length ());
    }

    if (embedding != nullptr) {
        try {
            readerContext.GetDestination ().AppendEmbedding (embedding);
        }
        catch (...) {
            delete embedding;
            throw;
        }
    }
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_plain (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_pntext (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& /*controlWord*/)
{
    SkipToEndOfCurrentGroup (); // we ignore pntext groups cuz we get the \lsN stuff instead... (spr#968)
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_qc (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    readerContext.GetDestination ().SetJustification (readerContext.GetCurrentGroupContext ()->fDestinationContext.fJustification = eCenterJustify);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_qj (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    readerContext.GetDestination ().SetJustification (readerContext.GetCurrentGroupContext ()->fDestinationContext.fJustification = eFullyJustify);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_ql (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    readerContext.GetDestination ().SetJustification (readerContext.GetCurrentGroupContext ()->fDestinationContext.fJustification = eLeftJustify);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_qr (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    readerContext.GetDestination ().SetJustification (readerContext.GetCurrentGroupContext ()->fDestinationContext.fJustification = eRightJustify);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_ri (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
    }
    else {
        readerContext.GetDestination ().SetRightMargin (readerContext.GetCurrentGroupContext ()->fDestinationContext.fRightMargin = Led_TWIPS (controlWord.fValue));
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_row (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    CheckIfAboutToStartBody (readerContext);
    readerContext.GetDestination ().EndRow (true);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_rtf (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& /*controlWord*/)
{
    // maybe should check version# or something - check only occurs once? No point...
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_sa (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
    }
    else {
        readerContext.GetDestination ().SetSpaceAfter (readerContext.GetCurrentGroupContext ()->fDestinationContext.fSpaceAfter = Led_TWIPS (controlWord.fValue));
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_sb (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
    }
    else {
        readerContext.GetDestination ().SetSpaceBefore (readerContext.GetCurrentGroupContext ()->fDestinationContext.fSpaceBefore = Led_TWIPS (controlWord.fValue));
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_shad (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_sl (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
    }
    else {
        readerContext.GetDestination ().SetSpaceBetweenLines (readerContext.GetCurrentGroupContext ()->fDestinationContext.fSpaceBetweenLines = controlWord.fValue);
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_slmult (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric argument
    }
    else {
        readerContext.GetDestination ().SetSpaceBetweenLinesMult (readerContext.GetCurrentGroupContext ()->fDestinationContext.fSpaceBetweenLinesMult = static_cast<bool> (controlWord.fValue));
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_sub (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_super (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_strike (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_stylesheet (ReaderContext& /*readerContext*/, const RTFIO::ControlWord& /*controlWord*/)
{
    SkipToEndOfCurrentGroup (); // we ignore unknown groups
    return true;
}

bool StyledTextIOReader_RTF::HandleControlWord_trleft (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    CheckIfAboutToStartBody (readerContext);
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric color-number argument
    }
    readerContext.GetDestination ().Set_trleft (Led_TWIPS (controlWord.fValue));
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_trgaph (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    CheckIfAboutToStartBody (readerContext);
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric color-number argument
    }
    Led_TWIPS margins = Led_TWIPS (controlWord.fValue);
    readerContext.GetDestination ().SetDefaultCellMarginsForRow_top (margins);
    readerContext.GetDestination ().SetDefaultCellMarginsForRow_left (margins);
    readerContext.GetDestination ().SetDefaultCellMarginsForRow_bottom (margins);
    readerContext.GetDestination ().SetDefaultCellMarginsForRow_right (margins);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_trowd (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
    CheckIfAboutToStartBody (readerContext);
    readerContext.GetDestination ().Call_trowd ();
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_trpaddX (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    CheckIfAboutToStartBody (readerContext);
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric color-number argument
    }
    Led_TWIPS margin = Led_TWIPS (controlWord.fValue);
    switch (controlWord.fWord) {
        case RTFIO::eControlAtom_trpaddb:
            readerContext.GetDestination ().SetDefaultCellMarginsForRow_bottom (margin);
            break;
        case RTFIO::eControlAtom_trpaddl:
            readerContext.GetDestination ().SetDefaultCellMarginsForRow_left (margin);
            break;
        case RTFIO::eControlAtom_trpaddr:
            readerContext.GetDestination ().SetDefaultCellMarginsForRow_right (margin);
            break;
        case RTFIO::eControlAtom_trpaddt:
            readerContext.GetDestination ().SetDefaultCellMarginsForRow_top (margin);
            break;
        default:
            Assert (false); // NOT REACHED
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_trspdX (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    CheckIfAboutToStartBody (readerContext);
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric color-number argument
    }
    Led_TWIPS margin = Led_TWIPS (controlWord.fValue);
    switch (controlWord.fWord) {
        case RTFIO::eControlAtom_trspdb:
            readerContext.GetDestination ().SetDefaultCellSpacingForRow_bottom (margin);
            break;
        case RTFIO::eControlAtom_trspdl:
            readerContext.GetDestination ().SetDefaultCellSpacingForRow_left (margin);
            break;
        case RTFIO::eControlAtom_trspdr:
            readerContext.GetDestination ().SetDefaultCellSpacingForRow_right (margin);
            break;
        case RTFIO::eControlAtom_trspdt:
            readerContext.GetDestination ().SetDefaultCellSpacingForRow_top (margin);
            break;
        default:
            Assert (false); // NOT REACHED
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_tx (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    if (not controlWord.fHasArg) {
        HandleBadlyFormattedInput (); // must have a numeric color-number argument
    }
    else {
        TextImager::StandardTabStopList* curTabs  = &readerContext.GetCurrentGroupContext ()->fDestinationContext.fTabStops;
        Led_Coordinate                   lastStop = 0;
        for (auto i = curTabs->fTabStops.begin (); i != curTabs->fTabStops.end (); ++i) {
            lastStop += *i;
        }
        Led_TWIPS newStop = Led_TWIPS (controlWord.fValue);
        if (newStop <= lastStop) {
            HandleBadlyFormattedInput ();
            return false; // Allow this to be a recoverable error by ignoring it... LGP 2000-09-20
        }
        Assert (newStop > lastStop);
        curTabs->fTabStops.push_back (Led_TWIPS (newStop - lastStop));
        readerContext.GetDestination ().SetTabStops (*curTabs);
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_u (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
// Unclear how I should treat this for the NON-UNICODE Led case. I COULD read the UNICODE chars - and then map them to narrow. But probably just
// as good to just read whatever narrow characters were already there.
// LGP 2000/04/29
#if qWideCharacters
    if (controlWord.fHasArg) {
        readerContext.fSkipNextNChars_UC = readerContext.fUnicodeUCValue;
        wchar_t u                        = static_cast<wchar_t> (controlWord.fValue);
        readerContext.GetDestination ().AppendText (&u, 1);
    }
    else {
        HandleBadlyFormattedInput ();
    }
#else
    Led_Arg_Unused (readerContext);
    Led_Arg_Unused (controlWord);
#endif
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_uc (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
#if qWideCharacters
    if (not controlWord.fHasArg) {
        readerContext.fUnicodeUCValue = 1;
    }
    else {
        readerContext.fUnicodeUCValue = max (0L, controlWord.fValue);
    }
#else
    Led_Arg_Unused (readerContext);
    Led_Arg_Unused (controlWord);
#endif
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_ul (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_ulnone (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    ApplyFontSpec (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_up (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    /*
     *  Kludge/workaround for SPR#1620.
     */
    if (controlWord.fHasArg and controlWord.fValue > 0) {
        RTFIO::ControlWord newCW;
        newCW.fHasArg = false;
        newCW.fWord   = RTFIO::eControlAtom_super;
        return HandleControlWord_super (readerContext, newCW);
    }
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_v (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    CheckIfAboutToStartBody (readerContext);
    if (readerContext.GetCurrentGroupContext () == nullptr) {
        HandleBadlyFormattedInput (true); // cannot set font name without a current group!
    }
    bool startHidden = controlWord.fHasArg ? controlWord.fValue : true;
    readerContext.GetDestination ().SetTextHidden (readerContext.GetCurrentGroupContext ()->fDestinationContext.fTextHidden = startHidden);
    return false;
}

bool StyledTextIOReader_RTF::HandleControlWord_UnknownControlWord (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    (void)HandlePossibleSpecialCharacterControlWord (readerContext, controlWord);
    return false;
}

bool StyledTextIOReader_RTF::HandlePossibleSpecialCharacterControlWord (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
    // Lookup. If good, then insert special character, and return true. Else return false to handle normally.
    for (size_t i = 0; i < NEltsOf (kMappings); i++) {
        if (controlWord.fWord == kMappings[i].fControlWordName) {
            CheckIfAboutToStartBody (readerContext);
#if qWideCharacters
            readerContext.GetDestination ().AppendText (&kMappings[i].fUNICODECharacter, 1);
#else
            char mbCharBuf[2];
            size_t mbCharBufSize = 2;
            CodePageConverter cvtr (readerContext.GetCurrentOutputCharSetEncoding ());
            cvtr.MapFromUNICODE (&kMappings[i].fUNICODECharacter, 1, mbCharBuf, &mbCharBufSize);
            readerContext.GetDestination ().AppendText (mbCharBuf, mbCharBufSize);
#endif
            return true;
        }
    }
    return false;
}

void StyledTextIOReader_RTF::ReadCommentGroup (ReaderContext& readerContext)
{
    /*
     *  Comment groups are often used for newly introduced groups/features, so they can
     *  be ignored by old readers, and still convey info to NEW readers. So we should
     *  by default - just inore them. But this routine is virutal, and its easy for
     *  subclassers to hook this routine, read the next control word, and see if it is
     *  one they know about, and else fall through to inherrited (THIS) version, to
     *  discard the rest of the group.
     */
    ConsumeNextChar (); // Called looking at the '*' characcter
    if (PeekNextChar () == RTFIO::kRTFStartTagChar) {
        RTFIO::ControlWord cword = ReadControlWord ();
        if (cword.fWord == RTFIO::eControlAtom_pn) {
            ReadIn_pn_Group (readerContext);
            return;
        }
    }

    SkipToEndOfCurrentGroup (); // we ignore unknown groups
}

void StyledTextIOReader_RTF::ReadIn_pn_Group (ReaderContext& readerContext)
{
    /*
     *  Exceedingly primitive support for reading in 'Word 6.0 and Word 95 RTF' style
     *  bullet and paragraph info data. See examples from SPR#1550, and the RTF 1.7 docs
     *  under the heading "Bullets and Numbering / Word 6.0 and Word 95 RTF" for more
     *  info on how to improve this reading code.
     */
    ListStyle listStyle = eListStyle_Bullet;
    if (readerContext.GetParentGroupContext () != nullptr) {
        readerContext.GetDestination ().SetListStyle (readerContext.GetParentGroupContext ()->fDestinationContext.fListStyle = listStyle);
    }
    SkipToEndOfCurrentGroup ();
}

#define qTryQuickISXXX 1
inline bool quickIsAlpha (char c)
{
    return (c >= 'a' and c <= 'z') or
           (c >= 'A' and c <= 'Z');
}
inline bool quickIsDigit (char c)
{
    return (c >= '0' and c <= '9');
}

RTFIO::ControlWord StyledTextIOReader_RTF::ReadControlWord ()
{
    /*
     *  WARNING: THIS ROUTINE ASSUMES 'a'..'z' and '0'..'9' are contiguous.
     *  This is true for ASCII.
     */
    RTFIO::ControlWord controlWord;

    if (GetNextChar () != RTFIO::kRTFStartTagChar) {
        Require (false); // should only be called when we have one...
        HandleBadlyFormattedInput (true);
        Assert (false); // NOTREACHED
    }

    /*
     *  All control-words consist of lower-case alpha characters (see RTF Syntax section of
     *  MS rtf spec), then followed by an optional number. Note funny space char delimiter
     *  behavior is part of spec.
     */
    char c = GetNextChar ();

    /*
     *  See spr#0619.
     *
     *  Can an RTF control word be broken up over multiple lines? Who knows!
     *
     *  The RTF 1.5 spec says pretty clearly what a control word is:
     *
     *          \LetterSequence<Delimiter>
     *              Note that a backslash begins each control word.
     *              The LetterSequence is made up of lowercase alphabetic characters between
     *              "a" and "z" inclusive. RTF is case sensitive, and all RTF control words must be lowercase.
     *
     *  That means that you CANNOT have a newline after the \ and still have a valid control-word? Right?
     *
     *  But - I've been sent sample RTF files that DO have a CRLF after the leading \ (in the \colortbl).
     *  And the RTF 1.5 spec itself - in its first example - shows this RTF text:
     *
     *          {\rtf\ansi\deff0{\fonttbl{\f0\froman Tms Rmn;}{\f1\fdecor
     *          Symbol;}{\f2\fswiss Helv;}}{\colortbl;\red0\green0\blue0;
     *          \red0\green0\blue255;\red0\green255\blue255;\red0\green255\
     *          blue0;\red255\green0\blue255;\red255\green0\blue0;\red255\
     *          green255\blue0;\red255\green255\blue255;}{\stylesheet{\fs20 \snext0Normal;}}{\info{\author John Doe}
     *          {\creatim\yr1990\mo7\dy30\hr10\min48}{\version1}{\edmins0}
     *          {\nofpages1}{\nofwords0}{\nofchars0}{\vern8351}}\widoctrl\ftnbj \sectd\linex0\endnhere \pard\plain \fs20 This is plain text.\par}
     *
     *
     *      Also note that the RTF 1.5 spec goes on to say:
     *
     *          A carriage return (character value 13) or linefeed (character value 10) will be
     *          treated as a \par control if the character is preceded by a backslash. You must
     *          include the backslash; otherwise, RTF ignores the control word.
     *
     *      Pretty clearly - the MSWord RTF readers ignore this dictate - at least under some circumstances.
     *
     *      To ignore this in MY code - you would have to sprinkle
     *          #if 0
     *              while (c == '\n' or c == '\r') {
     *                  c = GetNextChar ();     // Skip CR/LFs - see above See spr#0619 comment.
     *              }
     *          #endif
     *      in several places below.
     *
     *      But I thought that too risky. Way too likely to cause OTHER bugs. So instead - I've simply made the code
     *  less brittle when it finds errors in input RTF. It simply fails to parse (fully) a given colortable entry, and then
     *  moves on.
     */

    {
        RTFIO::ControlWordAtomName controlWordBuf;
        size_t                     controlWordLen = 0;
        for (; quickIsAlpha (c); c = GetNextChar ()) { // NB: used to check (c >= 'a' and c <= 'z') as the RTF 1.5 spec calls for.
            // It clearly says these controlwords must be lower case. However - their sample
            // reader code merely checks for isalpha(). And - I've found MSWord2000 seems to sometimes
            // generate undocumented control words that have mixed case. Sigh. Guess they hadn't read
            // the spec :-).--- SPR#0764

            // LGP 2003-03-17 slight speed tweek - having max buf size and throwing away controlword chars more than max buf size
            // (really should NEVER happen as controlwords shouldn't ever get that long).
            if (controlWordLen < RTFIO::eMaxControlAtomNameLen) {
                controlWordBuf[controlWordLen] = c;
                controlWordLen++;
            }
#if qDebug
            else {
                Assert (false); // just a notification that I should probably up the eMaxControlAtomNameLen define...
            }
#endif
        }
        controlWordBuf[controlWordLen] = '\0';
        controlWord.fWord              = RTFIO::EnterControlWord (controlWordBuf);
    }
    // note at this point, 'c' contains our DELIMITER.

    // According to our RTF Spec, if the 'c' is a SPACE, it is part of the control word (and eaten)
    if (c == ' ') {
        // EAT ME
    }
    // or it is a NUMBER, and we must read/consume the #
    else if (c == '-' or quickIsDigit (c)) {
        bool negative = bool(c == '-');
        if (negative) {
            c = GetNextChar (); // so we start loop looking at right character
        }
        unsigned long number = 0; // shift N add to read/build #
        for (; quickIsDigit (c); c = GetNextChar ()) {
            number *= 10ul;
            number += (unsigned long)(c - '0');
        }
        controlWord.fHasArg = true;
        // Note that this can OVERFLOW/UNDERFLOW for very large numbers? Maybe?
        // Most of spec only provides for 2-byte ints anyhow, so that should rarely, if ever, be a problem.
        controlWord.fValue = negative ? -long(number) : number;
        if (c == ' ') {
            // EAT ME
        }
        // or it is something ELSE, and we must put it BACK
        else {
            PutBackLastChar ();
        }
    }
    // or it is something ELSE, and we must put it BACK
    else {
        PutBackLastChar ();
    }

    return controlWord;
}

void StyledTextIOReader_RTF::AboutToStartBody (ReaderContext& readerContext)
{
    Require (not readerContext.fStartedBodyYet);
    readerContext.fStartedBodyYet = true;

    if (readerContext.fDefaultFontNumber != size_t (-1)) {
        // Set the initial font of our destination to this font spec.
        RequireNotNull (readerContext.GetCurrentGroupContext ());
        Led_IncrementalFontSpecification fontSpec = readerContext.GetCurrentGroupContext ()->fDestinationContext.fFontSpec;

        // We probably SHOULD do a Led_ThrowBadFormatDataException () here, but on pastes from MS Word 5.1, this sometimes happens
        // with no font-table. Go figure!
        if (readerContext.fFontTable != nullptr) {
            Led_IncrementalFontSpecification a = readerContext.fFontTable->GetFontSpec (readerContext.fDefaultFontNumber);
            if (a.GetFontNameSpecifier_Valid ()) {
                fontSpec.SetFontNameSpecifier (a.GetFontNameSpecifier ());

                Led_FontSpecification newPlain = GetPlainFont ();
                newPlain.SetFontNameSpecifier (a.GetFontNameSpecifier ());
                SetPlainFont (newPlain);
            }
        }
        readerContext.GetDestination ().UseFont (readerContext.GetCurrentGroupContext ()->fDestinationContext.fFontSpec = fontSpec);
    }
}

FontTableEntry StyledTextIOReader_RTF::ReadInFontTablesEntry ()
{
    /*
     *  We must be looking at the open brace. Read in the entire group (consuming the close brace),
     *  And return this summary info entry record.
     */
    FontTableEntry entry;

    if (GetNextChar () != RTFIO::kRTFOpenGroupChar) {
        Require (false); // should only be called when looking at one
        //Led_ThrowBadFormatDataException ();
    }
ReadRest:
    while (PeekNextChar () == RTFIO::kRTFStartTagChar) {
        RTFIO::ControlWord cword = ReadControlWord ();
        switch (cword.fWord) {
            case RTFIO::eControlAtom_f: {
                if (not cword.fHasArg) {
                    HandleBadlyFormattedInput (); // font number declarations need an argument (the font #)
                }
                else {
                    entry.fFNum = cword.fValue;
                }
            } break;

            case RTFIO::eControlAtom_fnil:
                entry.fFamily = FontTableEntry::eNil;
                break;
            case RTFIO::eControlAtom_froman:
                entry.fFamily = FontTableEntry::eRoman;
                break;
            case RTFIO::eControlAtom_fswiss:
                entry.fFamily = FontTableEntry::eSwiss;
                break;
            case RTFIO::eControlAtom_fmodern:
                entry.fFamily = FontTableEntry::eModern;
                break;
            case RTFIO::eControlAtom_fscript:
                entry.fFamily = FontTableEntry::eScript;
                break;
            case RTFIO::eControlAtom_fdecor:
                entry.fFamily = FontTableEntry::eDecor;
                break;
            case RTFIO::eControlAtom_ftech:
                entry.fFamily = FontTableEntry::eTech;
                break;
            case RTFIO::eControlAtom_fbidi:
                entry.fFamily = FontTableEntry::eBidi;
                break;

            case RTFIO::eControlAtom_fcharset: {
                if (not cword.fHasArg) {
                    HandleBadlyFormattedInput ();
                }
                else {
                    entry.fCharSet = cword.fValue;
                }
            } break;
            case RTFIO::eControlAtom_fprq: {
                if (not cword.fHasArg) {
                    HandleBadlyFormattedInput ();
                }
                else {
                    entry.fPitch = cword.fValue;
                }
            } break;
        }
    }

    // spr#506 - As of Word97 - it sometimes sticks comment groups (eg {\*\panose})
    // in middle of fonttable entry. Ignore any subgroups in a font-table-entry, as we don't
    // know how to handle them.
    if (PeekNextChar () == RTFIO::kRTFOpenGroupChar) {
        ConsumeNextChar ();
        SkipToEndOfCurrentGroup ();
        goto ReadRest;
    }

    size_t startOfName = GetSrcStream ().current_offset ();
    ScanForwardFor ("{;");
    entry.fFontName = Led_ANSI2SDKString (GrabString (startOfName));

    // SEE SPR#0749 - can also get these funky groups after the font name - skip them til I better understand them...
    if (PeekNextChar () == RTFIO::kRTFOpenGroupChar) {
        ConsumeNextChar ();
        SkipToEndOfCurrentGroup ();
    }

    ScanForwardFor ("}");
    if (GetNextChar () != RTFIO::kRTFCloseGroupChar) {
        HandleBadlyFormattedInput (true); // EOF reading font-entry
        Assert (false);                   // NOTREACHED
    }

    return entry;
}

void StyledTextIOReader_RTF::ReadInObjectSubGroupEntry (ReaderContext& readerContext, vector<char>* data, size_t* resultFoundAt)
{
    RequireNotNull (data);
    if (GetNextChar () != RTFIO::kRTFOpenGroupChar) {
        HandleBadlyFormattedInput (true);
        Assert (false); // NOTREACHED
    }
    if (PeekNextChar () == RTFIO::kRTFStartTagChar) {

        {
            // See if the group is really a \result group - and if so - mark it - and skip out - not an \objdata group!
            size_t             xxxxxxx = readerContext.GetReader ().GetSrcStream ().current_offset ();
            RTFIO::ControlWord xxx     = ReadControlWord ();
            if (xxx.fWord == RTFIO::eControlAtom_result) {
                *resultFoundAt = readerContext.GetReader ().GetSrcStream ().current_offset ();
                SkipToEndOfCurrentGroup ();
                return;
            }
            else {
                GetSrcStream ().seek_to (xxxxxxx);
            }
        }

        // See if we are looking at an \*\objdata, and if so, consume the data and store it away.
        // otherwise, ignore this group.
        ConsumeNextChar ();
        if (PeekNextChar () == '*') {
            ConsumeNextChar ();
        }
        if (PeekNextChar () == RTFIO::kRTFStartTagChar) {
            RTFIO::ControlWord cword = ReadControlWord ();
            if (cword.fWord == RTFIO::eControlAtom_objdata) {
                ReadObjData (data);
                return;
            }
        }
    }
    // if it wasn't an objdata and we somehow didnt read anything, simply skip to the end of the group.
    SkipToEndOfCurrentGroup ();
}

void StyledTextIOReader_RTF::ReadObjData (vector<char>* data)
{
    RequireNotNull (data);

    // Keep reading hex characters, and concatenate them onto 'data'. When we get a close-brace, we are done.
    for (char c = PeekNextChar (); c != RTFIO::kRTFCloseGroupChar; c = PeekNextChar ()) {
        if (c == '\n' or c == '\r') {
            ConsumeNextChar (); // Skip CR/LFs
            continue;
        }
        data->push_back (GetNextRTFHexByte ());
    }
    Assert (PeekNextChar () == RTFIO::kRTFCloseGroupChar);
    ConsumeNextChar (); // Eat terminating brace
}

void StyledTextIOReader_RTF::ConstructOLEEmebddingFromRTFInfo (ReaderContext& readerContext, Led_TWIPS_Point size, size_t nBytes, const void* data)
{
#if qPlatform_Windows
    using RTFOLEEmbedding                                                   = RTFIO::RTFOLEEmbedding;
    const Led_ClipFormat                                kOLEEmbedClipFormat = static_cast<Led_ClipFormat> (::RegisterClipboardFormat (_T ("Object Descriptor")));
    const vector<EmbeddedObjectCreatorRegistry::Assoc>& types               = EmbeddedObjectCreatorRegistry::Get ().GetAssocList ();
    for (size_t i = 0; i < types.size (); i++) {
        EmbeddedObjectCreatorRegistry::Assoc assoc = types[i];
        if (memcmp (assoc.fEmbeddingTag, RTFIO::RTFOLEEmbedding::kEmbeddingTag, sizeof (RTFIO::RTFOLEEmbedding::kEmbeddingTag)) == 0) {
            AssertNotNull (assoc.fReadFromMemory);
            SimpleEmbeddedObjectStyleMarker* embedding = (assoc.fReadFromMemory) (RTFIO::RTFOLEEmbedding::kEmbeddingTag, data, nBytes);
            RTFOLEEmbedding*                 rtfe      = dynamic_cast<RTFOLEEmbedding*> (embedding);
            if (rtfe != nullptr) {
                rtfe->PostCreateSpecifyExtraInfo (size);
            }
            try {
                readerContext.GetDestination ().AppendEmbedding (embedding);
                return;
            }
            catch (...) {
                delete embedding;
                throw;
            }
        }
    }
#else
    Led_Arg_Unused (readerContext);
    Led_Arg_Unused (size);
    Led_Arg_Unused (nBytes);
    Led_Arg_Unused (data);
#endif
    Led_ThrowBadFormatDataException (); // Will be caught by caller, and use "unknown embedding object"
}

void StyledTextIOReader_RTF::ConstructLedEmebddingFromRTFInfo (ReaderContext& readerContext, size_t nBytes, const void* data)
{
    // The first sizeof (Led_PrivateEmbeddingTag) bytes are the type tag, and the rest is standard
    // internalize/externalize data.
    if (nBytes < sizeof (Led_PrivateEmbeddingTag)) {
        Led_ThrowBadFormatDataException (); // Will be caught by caller, and use "unknown embedding object"
    }
    const char*                                         tag           = (const char*)data;
    const char*                                         theData       = tag + sizeof (Led_PrivateEmbeddingTag);
    size_t                                              theDataNBytes = nBytes - sizeof (Led_PrivateEmbeddingTag);
    const vector<EmbeddedObjectCreatorRegistry::Assoc>& types         = EmbeddedObjectCreatorRegistry::Get ().GetAssocList ();
    for (size_t i = 0; i < types.size (); i++) {
        EmbeddedObjectCreatorRegistry::Assoc assoc = types[i];
        if (memcmp (assoc.fEmbeddingTag, tag, sizeof (assoc.fEmbeddingTag)) == 0) {
            AssertNotNull (assoc.fReadFromMemory);
            SimpleEmbeddedObjectStyleMarker* embedding = (assoc.fReadFromMemory) (tag, theData, theDataNBytes);
            try {
                readerContext.GetDestination ().AppendEmbedding (embedding);
                return;
            }
            catch (...) {
                delete embedding;
                throw;
            }
        }
    }
    Led_ThrowBadFormatDataException (); // Will be caught by caller, and use "unknown embedding object"
}

void StyledTextIOReader_RTF::ReadPictData (vector<char>* data)
{
    RequireNotNull (data);

    // Keep reading hex characters, and concatenate them onto 'data'. When we get a close-brace or start of another tag, we are done.
    // This routine can be called many times to append more and more to 'data'.
    for (char c = PeekNextChar (); c != RTFIO::kRTFCloseGroupChar and c != RTFIO::kRTFStartTagChar; c = PeekNextChar ()) {
        if (c == '\n' or c == '\r') {
            ConsumeNextChar (); // Skip CR/LFs
            continue;
        }
        PUSH_BACK (*data, GetNextRTFHexByte ());
    }
    Assert (PeekNextChar () == RTFIO::kRTFCloseGroupChar or PeekNextChar () == RTFIO::kRTFStartTagChar);
}

/*
@METHOD:        StyledTextIOReader_RTF::ReadTopLevelPictData
@DESCRIPTION:   <p>Read in the data from a \pict group.</p>
*/
void StyledTextIOReader_RTF::ReadTopLevelPictData (Led_TWIPS_Point* shownSize, ImageFormat* imageFormat, Led_TWIPS_Point* bmSize, vector<char>* objData)
{
    using UnknownRTFEmbedding = RTFIO::UnknownRTFEmbedding;
    using ControlWord         = RTFIO::ControlWord;

    *imageFormat     = eDefaultImageFormat;
    *shownSize       = Led_TWIPS_Point (Led_TWIPS (0), Led_TWIPS (0));
    *bmSize          = Led_TWIPS_Point (Led_TWIPS (0), Led_TWIPS (0));
    *objData         = vector<char> ();
    float     scaleX = 1.0f;
    float     scaleY = 1.0f;
    Led_TWIPS picH   = Led_TWIPS (0);
    Led_TWIPS picV   = Led_TWIPS (0);

    while (true) {
        switch (PeekNextChar ()) {
            case RTFIO::kRTFStartTagChar: {
                ControlWord cw = ReadControlWord ();
                switch (cw.fWord) {
                    case RTFIO::eControlAtom_emfblip:
                        *imageFormat = eEMF;
                        break;
                    case RTFIO::eControlAtom_pngblip:
                        *imageFormat = ePNG;
                        break;
                    case RTFIO::eControlAtom_jpegblip:
                        *imageFormat = eJPEG;
                        break;
                    case RTFIO::eControlAtom_macpict:
                        *imageFormat = eMacPICT;
                        break;
                    case RTFIO::eControlAtom_pmmetafile:
                        *imageFormat = ePMMetaFile;
                        break;
                    case RTFIO::eControlAtom_wmetafile:
                        *imageFormat = eWMF;
                        break;
                    case RTFIO::eControlAtom_dibitmap:
                        *imageFormat = eDIB;
                        break;
                    case RTFIO::eControlAtom_wbitmap:
                        *imageFormat = eBITMAP;
                        break;
                    case RTFIO::eControlAtom_picw:
                        if (not cw.fHasArg) {
                            HandleBadlyFormattedInput ();
                            break;
                        };
                        bmSize->h = Led_TWIPS (cw.fValue);
                        break;
                    case RTFIO::eControlAtom_pich:
                        if (not cw.fHasArg) {
                            HandleBadlyFormattedInput ();
                            break;
                        };
                        bmSize->v = Led_TWIPS (cw.fValue);
                        break;
                    case RTFIO::eControlAtom_picwgoal:
                        if (not cw.fHasArg) {
                            HandleBadlyFormattedInput ();
                            break;
                        };
                        shownSize->h = Led_TWIPS (cw.fValue);
                        break;
                    case RTFIO::eControlAtom_pichgoal:
                        if (not cw.fHasArg) {
                            HandleBadlyFormattedInput ();
                            break;
                        };
                        shownSize->v = Led_TWIPS (cw.fValue);
                        break;
                    case RTFIO::eControlAtom_picscalex:
                        if (not cw.fHasArg) {
                            HandleBadlyFormattedInput ();
                            break;
                        };
                        scaleX = cw.fValue / 100.0f;
                        break;
                    case RTFIO::eControlAtom_picscaley:
                        if (not cw.fHasArg) {
                            HandleBadlyFormattedInput ();
                            break;
                        };
                        scaleY = cw.fValue / 100.0f;
                        break;
                }
            } break;

            case RTFIO::kRTFOpenGroupChar: {
                // Right now - we don't understand any subgroups - so skip them.
                ConsumeNextChar ();
                SkipToEndOfCurrentGroup ();
            } break;

            case RTFIO::kRTFCloseGroupChar: {
                /*
                     * Sanity check shown size.
                     */
                if (shownSize->h == 0) {
                    shownSize->h = bmSize->h;
                }
                if (shownSize->v == 0) {
                    shownSize->v = bmSize->v;
                }

                shownSize->h *= scaleX;
                shownSize->v *= scaleY;
                if (shownSize->v > 20000 or shownSize->h > 20000 or shownSize->h < 10 or shownSize->v < 10) {
                    *shownSize = UnknownRTFEmbedding::CalcStaticDefaultShownSize ();
                }

                ConsumeNextChar ();
                return; // ALL DONE
            } break;
            default: {
                ReadPictData (objData);
            } break;
        }
    }
}

/*
@METHOD:        StyledTextIOReader_RTF::ConstructDIBFromData
@DESCRIPTION:   <p>Take the given size and data parameters, and consturct a new Led_DIB (which must be freed by caller using delete()). Returns nullptr
    if unable to convert the given format.</p>
*/
Led_DIB* StyledTextIOReader_RTF::ConstructDIBFromData (Led_TWIPS_Point shownSize, ImageFormat imageFormat, Led_TWIPS_Point bmSize, size_t nBytes, const void* data)
{
    if (data == nullptr) {
        HandleBadlyFormattedInput ();
        return nullptr; // allow bad input to be treated as unknown format - just return nullptr...
    }
    switch (imageFormat) {
        case eDIB: {
            const Led_DIB* dib = reinterpret_cast<const Led_DIB*> (data);
            if (Led_GetDIBImageByteCount (dib) != nBytes) {
                HandleBadlyFormattedInput ();
                return nullptr; // allow bad input to be treated as unknown format - just return nullptr...
            }
            return Led_CloneDIB (dib);
        } break;
#if qPlatform_Windows
        case eEMF: {
            Led_DIB*     result = nullptr;
            HENHMETAFILE hMF    = ::SetEnhMetaFileBits (nBytes, reinterpret_cast<const unsigned char*> (data));
            try {
                result = ConstructDIBFromEMFHelper (shownSize, bmSize, hMF);
            }
            catch (...) {
                Verify (::DeleteEnhMetaFile (hMF));
                throw;
            }
            Verify (::DeleteEnhMetaFile (hMF));
            return result;
        } break;
        case eWMF: {
            Led_DIB*     result = nullptr;
            HENHMETAFILE hMF    = ::SetWinMetaFileBits (nBytes, reinterpret_cast<const unsigned char*> (data), nullptr, nullptr);
            try {
                result = ConstructDIBFromEMFHelper (shownSize, bmSize, hMF);
            }
            catch (...) {
                Verify (::DeleteEnhMetaFile (hMF));
                throw;
            }
            Verify (::DeleteEnhMetaFile (hMF));
            return result;
        } break;
#else
            Led_Arg_Unused (shownSize);
            Led_Arg_Unused (bmSize);
#endif
    }
    return nullptr;
}

#if qPlatform_Windows
/*
@METHOD:        StyledTextIOReader_RTF::ConstructDIBFromEMFHelper
@DESCRIPTION:   <p>Construct a Led_DIB given HENHMETAFILE, the desired 'shownSize' and 'bmSize' can OVERRIDE the
            size specified in the metafile itself.</p>
                <p>This routine is only available if @'qPlatform_Windows'.</p>
*/
Led_DIB* StyledTextIOReader_RTF::ConstructDIBFromEMFHelper (Led_TWIPS_Point shownSize, [[maybe_unused]] Led_TWIPS_Point bmSize, const HENHMETAFILE hMF)
{
    RequireNotNull (hMF);

    ENHMETAHEADER header{};
    Verify (::GetEnhMetaFileHeader (hMF, sizeof (header), &header) == sizeof (header));

    // Don't know best way to get a DIB from a metafile - but this way I HOPE will at least WORK!
    Led_Tablet_ screenDC = (::GetWindowDC (nullptr)); // not sure what DC to use to convert MetaFile to DIB - but this seems like a decent guess

    Led_Distance hSize     = screenDC.CvtFromTWIPSH (shownSize.h);
    Led_Distance vSize     = screenDC.CvtFromTWIPSV (shownSize.v);
    Led_Rect     imageRect = Led_Rect (0, 0, vSize, hSize);

    Led_Tablet_ memDC;
    Led_Bitmap  memoryBitmap;

    {
        Led_ThrowIfFalseGetLastError (memDC.CreateCompatibleDC (&screenDC));
        Led_ThrowIfFalseGetLastError (memoryBitmap.CreateCompatibleBitmap (screenDC, hSize, vSize));
        HBITMAP oldBitmapInDC = memDC.SelectObject (memoryBitmap);

        // Erase the background of the image
        {
            Led_Rect             eraser     = imageRect;
            Led_Color            eraseColor = Led_GetTextBackgroundColor ();
            Led_Brush            backgroundBrush (eraseColor.GetOSRep ());
            Led_Win_Obj_Selector pen (&memDC, ::GetStockObject (NULL_PEN));
            Led_Win_Obj_Selector brush (&memDC, backgroundBrush);
            eraser.right++; // lovely - windows doesn't count last pixel... See Docs for Rectangle() and rephrase!!!
            eraser.bottom++;
            memDC.Rectangle (AsRECT (eraser));
        }

        // Create a Palette to be used in the offscreen bitmap - if there was one in the MetaFile
        HPALETTE usePalette = nullptr;
        {
            UINT nPalEntries = ::GetEnhMetaFilePaletteEntries (hMF, 0, nullptr);
            Assert (nPalEntries != GDI_ERROR);
            if (nPalEntries != 0) {
                LOGPALETTE* paletteData    = reinterpret_cast<LOGPALETTE*> (new char[sizeof (LOGPALETTE) + nPalEntries * sizeof (PALETTEENTRY)]);
                paletteData->palVersion    = 0;
                paletteData->palNumEntries = nPalEntries;
                Verify (::GetEnhMetaFilePaletteEntries (hMF, nPalEntries, paletteData->palPalEntry) == nPalEntries);
                usePalette = ::CreatePalette (paletteData);
                delete[](char*) paletteData;
            }
        }
        HPALETTE oldPalette = nullptr;
        if (usePalette != nullptr) {
            oldPalette = ::SelectPalette (memDC, usePalette, true);
            ::RealizePalette (memDC);
        }

        // Play the acual MetaFile into the offscreen bitmap
        bool failed = false;
        {
            RECT rect = AsRECT (imageRect);
            if (PlayEnhMetaFile (memDC.m_hDC, hMF, &rect) == 0) {
                failed = true;
            }
        }

        if (oldPalette != nullptr) {
            ::SelectPalette (memDC, oldPalette, true);
        }
        if (usePalette != nullptr) {
            ::DeleteObject (usePalette);
        }
        if (oldBitmapInDC != nullptr) {
            (void)memDC.SelectObject (oldBitmapInDC);
        }
        if (failed) {
            HandleBadlyFormattedInput ();
            return nullptr; // allow bad input to be treated as unknown format - just return nullptr...
        }
    }

    return Led_DIBFromHBITMAP (memDC.m_hDC, memoryBitmap);
}
#endif

void StyledTextIOReader_RTF::ApplyFontSpec (ReaderContext& readerContext, const RTFIO::ControlWord& cw)
{
    CheckIfAboutToStartBody (readerContext);
    if (readerContext.GetCurrentGroupContext () == nullptr) {
        HandleBadlyFormattedInput (true); // cannot set font name without a current group!
    }
    Led_IncrementalFontSpecification fontSpec = readerContext.GetCurrentGroupContext ()->fDestinationContext.fFontSpec;

    switch (cw.fWord) {
        case RTFIO::eControlAtom_plain: {
            fontSpec = GetPlainFont ();
            // after looking at sample RTF docs (like RTF 1.5 spec) - it seems PLAIN should be interpreted as resetting the hidden flag...
            readerContext.GetDestination ().SetTextHidden (readerContext.GetCurrentGroupContext ()->fDestinationContext.fTextHidden = false);
        } break;

        case RTFIO::eControlAtom_cf: {
            if (not cw.fHasArg) {
                HandleBadlyFormattedInput (); // must have a numeric color-number argument
                return;                       // just ignore the cf record...
            }
            fontSpec.SetTextColor (LookupColor (readerContext, static_cast<size_t> (cw.fValue)));
        } break;

        case RTFIO::eControlAtom_f: {
            if (readerContext.fFontTable == nullptr) {
                // We probably SHOULD do a Led_ThrowBadFormatDataException () here, but on pastes from MS Word 5.1, this sometimes happens
                // with no font-table. Go figure!
                return;
            }
            if (not cw.fHasArg) {
                HandleBadlyFormattedInput (true); // must have a numeric font-number argument
            }
            Led_IncrementalFontSpecification a = readerContext.fFontTable->GetFontSpec (cw.fValue);
            if (a.GetFontNameSpecifier_Valid ()) {
                fontSpec.SetFontNameSpecifier (a.GetFontNameSpecifier ());
            }

            AssertNotNull (readerContext.fFontTable);
            const FontTableEntry* fte = readerContext.fFontTable->LookupEntryByNumber (cw.fValue);
            if (fte == nullptr) {
                // We probably SHOULD do a Led_ThrowBadFormatDataException () here,
                // see spr#0696
                Assert (false); // was getting this cuz of bad fonttable generation with hidden text - Just test if we still get - but not really a bug to get here - since input RTF could really be bad!
                // LGP 2000/04/26
                return;
            }
            if (fte->fCharSet != -1) {
                // Not sure what I should do if Win32CharSetToCodePage returns zero? -- LGP 2002-12-08
                CodePage cp = Platform::Windows::Win32CharSetToCodePage (fte->fCharSet);
                if (cp != 0) {
                    readerContext.GetCurrentGroupContext ()->fCurrentCodePage = cp;
                }
                readerContext.UseInputCharSetEncoding (readerContext.GetCurrentGroupContext ()->fCurrentCodePage);
            }
        } break;

        case RTFIO::eControlAtom_fs: {
            if (not cw.fHasArg) {
                HandleBadlyFormattedInput (true); // must have a numeric font-size argument
            }
            int newSize = cw.fValue / 2;
            if (newSize < 4) {
                newSize = 4;
            }
            if (newSize > 128) {
                newSize = 128;
            }
#if qPlatform_Windows
            if (newSize == fCachedFontSize) {
                fontSpec.PokeAtTMHeight (fCachedFontSizeTMHeight);
                break;
            }
#endif
            fontSpec.SetPointSize (static_cast<uint8_t> (newSize)); //pinned above 4..128
#if qPlatform_Windows
            fCachedFontSize         = newSize;
            fCachedFontSizeTMHeight = fontSpec.PeekAtTMHeight ();
#endif
        } break;

        case RTFIO::eControlAtom_b: {
            bool turnStyleOn = true; // no arg means ON
            if (cw.fHasArg) {
                turnStyleOn = cw.fValue;
            }
            fontSpec.SetStyle_Bold (turnStyleOn);
        } break;

        case RTFIO::eControlAtom_i: {
            bool turnStyleOn = true; // no arg means ON
            if (cw.fHasArg) {
                turnStyleOn = cw.fValue;
            }
            fontSpec.SetStyle_Italic (turnStyleOn);
        } break;

#if qPlatform_MacOS
        case RTFIO::eControlAtom_outl: {
            bool turnStyleOn = true; // no arg means ON
            if (cw.fHasArg) {
                turnStyleOn = cw.fValue;
            }
            fontSpec.SetStyle_Outline (turnStyleOn);
        } break;

        case RTFIO::eControlAtom_shad: {
            bool turnStyleOn = true; // no arg means ON
            if (cw.fHasArg) {
                turnStyleOn = cw.fValue;
            }
            fontSpec.SetStyle_Shadow (turnStyleOn);
        } break;
#endif
        case RTFIO::eControlAtom_sub: {
            fontSpec.SetStyle_SubOrSuperScript (Led_FontSpecification::eSubscript);
        } break;
        case RTFIO::eControlAtom_super: {
            fontSpec.SetStyle_SubOrSuperScript (Led_FontSpecification::eSuperscript);
        } break;
        case RTFIO::eControlAtom_strike: {
            bool turnStyleOn = true; // no arg means ON
            if (cw.fHasArg) {
                turnStyleOn = cw.fValue;
            }
#if qPlatform_Windows
            fontSpec.SetStyle_Strikeout (turnStyleOn);
#endif
        } break;
        case RTFIO::eControlAtom_ul: {
            bool turnStyleOn = true; // no arg means ON
            if (cw.fHasArg) {
                turnStyleOn = cw.fValue;
            }
            fontSpec.SetStyle_Underline (turnStyleOn);
        } break;

        case RTFIO::eControlAtom_ulnone: {
            fontSpec.SetStyle_Underline (false);
        } break;
    }
    readerContext.GetDestination ().UseFont (readerContext.GetCurrentGroupContext ()->fDestinationContext.fFontSpec = fontSpec);
}

unsigned char StyledTextIOReader_RTF::GetNextRTFHexByte () const
{
    char c = GetNextChar ();

    // First nibble
    while (c == '\n' or c == '\r') {
        c = GetNextChar (); // Skip CR/LFs
    }
    unsigned char value = '\0';
    if (c >= '0' and c <= '9') {
        value = c - '0';
    }
    else if (c >= 'a' and c <= 'f') {
        value = c - 'a' + 10;
    }
    else if (c >= 'A' and c <= 'F') {
        value = c - 'A' + 10;
    }
    else {
        // MAYBE we could consider making this RECOVERABLE - but I've never seen this before, and it seems likely to be a syptom of a badly corrupted file... LGP 2000-09-20
        HandleBadlyFormattedInput (true);
    }

    // Second nibble
    c = GetNextChar ();
    while (c == '\n' or c == '\r') {
        c = GetNextChar (); // Skip CR/LFs
    }
    unsigned char value2 = '\0';
    if (c >= '0' and c <= '9') {
        value2 = c - '0';
    }
    else if (c >= 'a' and c <= 'f') {
        value2 = c - 'a' + 10;
    }
    else if (c >= 'A' and c <= 'F') {
        value2 = c - 'A' + 10;
    }
    else {
        // SPR#1489
        HandleBadlyFormattedInput (true);
    }
    value <<= 4;
    value += value2;
    return value;
}

string StyledTextIOReader_RTF::ReadInGroupAndSave ()
{
    size_t startOfGroup = GetSrcStream ().current_offset ();
    if (GetNextChar () != RTFIO::kRTFOpenGroupChar) {
        HandleBadlyFormattedInput (true);
    }
    SkipToEndOfCurrentGroup ();
    return GrabString (startOfGroup);
}

void StyledTextIOReader_RTF::SkipToEndOfCurrentGroup ()
{
    int depth = 1; // assume we start INSIDE a group
    for (; depth > 0;) {
        ScanForwardFor ("{}");
        switch (PeekNextChar ()) {
            case RTFIO::kRTFOpenGroupChar: {
                ConsumeNextChar ();
                depth++;
            } break;
            case RTFIO::kRTFCloseGroupChar: {
                ConsumeNextChar ();
                depth--;
            } break;
            default: {
                Assert (false);
            } break;
        }
    }
}

void StyledTextIOReader_RTF::ScanForwardFor (const char* setOfChars)
{
    RequireNotNull (setOfChars);

#if qUseCompiledSetHack
    bitset<256> compiledSet;
    for (auto p = setOfChars; *p != '\0'; p++) {
        compiledSet[(unsigned char)*p] = true;
    }
#endif

    try {
        for (char c = GetNextChar ();; c = GetNextChar ()) {
            if (c == RTFIO::kRTFQuoteNextCharChar) { // avoid getting confused by embedded '{' etc characters in the actual text.
                continue;
            }
#if qUseCompiledSetHack
            if (compiledSet[(unsigned char)c]) {
                PutBackLastChar ();
                return;
            }
#else
            for (const char* p = setOfChars; *p != '\0'; p++) {
                if (c == *p) {
                    PutBackLastChar ();
                    return;
                }
            }
#endif
        }
    }
    catch (ReadEOFException&) {
        // ignore these exceptions - just return if we reach EOF
        return;
    }
#undef qUseCompiledSetHack
}

/*
@METHOD:        StyledTextIOReader_RTF::SearchForwardFor
@DESCRIPTION:   <p><b>Internal/Private</b></p>
                <p>Search for the given string (up to a max of maxCharsToExamine characters). Return true iff the string is found
            and leave the stream positioned where found. If fail to find - leave stream pointer unchanged. Don't THROW cuz of
            end of file - but just treat that as not finding. Other exceptions also return stream to original position.</p>
*/
bool StyledTextIOReader_RTF::SearchForwardFor (const char* searchFor, size_t maxCharsToExamine)
{
    RequireNotNull (searchFor);
    SrcStream& srcStream   = GetSrcStream ();
    size_t     origOffset  = srcStream.current_offset ();
    size_t     matchStrLen = ::strlen (searchFor);
    Require (matchStrLen >= 1);
    try {
    RetryNextMatch:
        char c = '\0';
        for (; (c = GetNextChar ()) != '\0';) {
            if (c == *searchFor) {
                // Compare further...
                size_t foundAt = srcStream.current_offset () - 1;
                for (c = GetNextChar (); matchStrLen > (srcStream.current_offset () - foundAt); c = GetNextChar ()) {
                    size_t idx = srcStream.current_offset () - foundAt - 1;
                    if (searchFor[idx] == c) {
                        continue; // keep matching
                    }
                    else {
                        // bad match...
                        srcStream.seek_to (foundAt + 1);
                        goto RetryNextMatch;
                    }
                }
                // If we fall through - must have been a good match!
                Assert (matchStrLen == srcStream.current_offset () - foundAt);
                srcStream.seek_to (foundAt);
                return true;
            }
            if (srcStream.current_offset () > origOffset + maxCharsToExamine) {
                srcStream.seek_to (origOffset);
                return false;
            }
        }
    }
    catch (ReadEOFException) {
        srcStream.seek_to (origOffset);
        return false;
    }
    catch (...) {
        srcStream.seek_to (origOffset);
        throw;
    }
    return false;
}

RTFInfo& StyledTextIOReader_RTF::GetRTFInfo () const
{
    AssertNotNull (fRTFInfo);
    return *fRTFInfo;
}

/*
 ********************************************************************************
 ************************ StyledTextIOWriter_RTF::WriterContext *****************
 ********************************************************************************
 */
size_t StyledTextIOWriter_RTF::WriterContext::GetCurSrcOffset () const
{
    return GetSrcStream ().current_offset ();
}

SimpleEmbeddedObjectStyleMarker* StyledTextIOWriter_RTF::WriterContext::GetCurSimpleEmbeddedObjectStyleMarker () const
{
    size_t                                   offset         = GetCurSrcOffset ();
    vector<SimpleEmbeddedObjectStyleMarker*> embeddingsList = GetSrcStream ().CollectAllEmbeddingMarkersInRange (offset - 1, offset);
    Assert (embeddingsList.size () <= 1); // cuz we gave a range of one, and can only have a single
    // embedding in one place. Allow for there to be NONE - if the user
    // wants to allow having NUL characters in his text for other reasons.
    if (embeddingsList.empty ()) {
        return nullptr;
    }
    else {
        return embeddingsList[0];
    }
}

StyledTextIOWriter_RTF::Table* StyledTextIOWriter_RTF::WriterContext::GetCurRTFTable () const
{
    return GetSrcStream ().GetTableAt (GetCurSrcOffset () - 1);
}

/*
 ********************************************************************************
 ***************************** StyledTextIOWriter_RTF ***************************
 ********************************************************************************
 */
StyledTextIOWriter_RTF::StyledTextIOWriter_RTF (SrcStream* srcStream, SinkStream* sinkStream, RTFInfo* rtfInfo)
    : StyledTextIOWriter (srcStream, sinkStream)
    , fCurrentOutputCharSetEncoding (kCodePage_ANSI)
    ,
#if !qWideCharacters
#if qPlatform_MacOS
    fCurrentInputCharSetEncoding (kCodePage_MAC)
    ,
#elif qPlatform_Windows || qXWindows
    fCurrentInputCharSetEncoding (kCodePage_ANSI)
    , // not sure???
#endif
    fCharsetMappingTable (fCurrentInputCharSetEncoding, fCurrentOutputCharSetEncoding)
    , // note: important these two members DECLARED before this one... else not INITED at this point!
#endif
    fCharactersSavedByName ()
    , fCharactersSavedByName_Name2Char ()
    , fCharactersSavedByName_Char2Name ()
    , fRTFInfo (rtfInfo)
    , fFontTable (nullptr)
    , fColorTable (nullptr)
    , fListTable (nullptr)
    , fStyleRunSummary ()
    , fDocumentCharacterSet (kCodePage_ANSI)
    , fSoftLineBreakChar (srcStream->GetSoftLineBreakCharacter ())
    , fHidableTextRuns (srcStream->GetHidableTextRuns ())
{
    static const pair<string, wchar_t> kCharsWrittenByName[] = {
        pair<string, wchar_t> ("tab", L'\x0009'),
        pair<string, wchar_t> ("emdash", L'\x2013'),
        pair<string, wchar_t> ("endash", L'\x2014'),
        pair<string, wchar_t> ("lquote", L'\x2018'),
        pair<string, wchar_t> ("rquote", L'\x2019'),
        pair<string, wchar_t> ("ldblquote", L'\x201c'),
        pair<string, wchar_t> ("rdblquote", L'\x201d'),
    };
    SetCharactersSavedByName (vector<pair<string, wchar_t>> (&kCharsWrittenByName[0], &kCharsWrittenByName[NEltsOf (kCharsWrittenByName)]));
}

StyledTextIOWriter_RTF::~StyledTextIOWriter_RTF ()
{
    delete fFontTable;
    delete fColorTable;
    delete fListTable;
}

void StyledTextIOWriter_RTF::UseOutputCharSetEncoding (CodePage codePage)
{
#if qWideCharacters
    fCurrentOutputCharSetEncoding = codePage;
#else
    if (fCurrentOutputCharSetEncoding != codePage) {
        fCurrentOutputCharSetEncoding = codePage;
        fCharsetMappingTable = RTFIO::SingleByteCharsetToCharsetMappingTable (fCurrentInputCharSetEncoding, fCurrentOutputCharSetEncoding);
    }
#endif
}

#if !qWideCharacters
void StyledTextIOWriter_RTF::UseInputCharSetEncoding (CodePage codePage)
{
    if (fCurrentInputCharSetEncoding != codePage) {
        fCurrentInputCharSetEncoding = codePage;
        fCharsetMappingTable         = RTFIO::SingleByteCharsetToCharsetMappingTable (fCurrentInputCharSetEncoding, fCurrentOutputCharSetEncoding);
    }
}
#endif

void StyledTextIOWriter_RTF::SetCharactersSavedByName (const vector<pair<string, wchar_t>>& charactersSavedByName)
{
    fCharactersSavedByName = charactersSavedByName;
    for (vector<pair<string, wchar_t>>::const_iterator i = fCharactersSavedByName.begin (); i != fCharactersSavedByName.end (); ++i) {
        fCharactersSavedByName_Name2Char.insert (map<string, wchar_t>::value_type (i->first, i->second));
        fCharactersSavedByName_Char2Name.insert (map<wchar_t, string>::value_type (i->second, i->first));
    }
}

void StyledTextIOWriter_RTF::Write ()
{
    WriterContext writerContext (*this);

    write (RTFIO::kRTFOpenGroupChar);
    WriteHeader (writerContext);
    WriteBody (writerContext);
    write (RTFIO::kRTFCloseGroupChar);
}

void StyledTextIOWriter_RTF::WriteHeader (WriterContext& writerContext)
{
    WriteTag ("rtf1");
    WriteDocCharset ();
    WriteFontTable (writerContext);
    WriteColorTable (writerContext);
    WriteListTable ();
    if (fRTFInfo != nullptr) {
        WriteTagNValue ("deftab", fRTFInfo->fDefaultTabStop);
    }
    WriteGenerator ();
}

void StyledTextIOWriter_RTF::WriteBody (WriterContext& writerContext)
{
    /*
     *  Walk through the characters, and output them one at a time. Walk
     *  SIMULTANEOUSLY through the style run information, and output new controlling
     *  tags on the fly.
     */
    AssureStyleRunSummaryBuilt (writerContext);

    WriteStartParagraph (writerContext);
    writerContext.fLastEmittedISR    = StandardStyledTextImager::InfoSummaryRecord (Led_IncrementalFontSpecification (), 0);
    writerContext.fNextStyleChangeAt = 0;
    writerContext.fIthStyleRun       = 0;
    if (not fHidableTextRuns.empty ()) {
        writerContext.fNextHidableTextChangeAt = fHidableTextRuns[0].fOffsetFromPrev;
    }
    Led_tChar c = '\0';
    while (writerContext.GetSrcStream ().readNTChars (&c, 1) != 0) {
        WriteBodyCharacter (writerContext, c);
    }
    if (writerContext.fHidableTextRegionOpen) {
        write ("}");
        writerContext.fNextHidableTextChangeAt = size_t (-1);
    }
}

void StyledTextIOWriter_RTF::WriteBodyCharacter (WriterContext& writerContext, Led_tChar c)
{
    /*
     *  Note - WriteBodyCharacter () gets called AFTER we've read the next character, so the GetCurSrcOffset () is pointing one past the
     *  character we are processing. Thats why here we say "writerContext.GetCurSrcOffset ()-1"
     */

    // HidableText state changes...
    if (writerContext.GetCurSrcOffset () - 1 == writerContext.fNextHidableTextChangeAt) {
        if (writerContext.fHidableTextRegionOpen) {
            write ("}");
            writerContext.fIthHidableTextRun++;
            if (writerContext.fIthHidableTextRun < fHidableTextRuns.size ()) {
                writerContext.fNextHidableTextChangeAt += fHidableTextRuns[writerContext.fIthHidableTextRun].fOffsetFromPrev;
            }
            else {
                writerContext.fNextHidableTextChangeAt = size_t (-1);
            }
        }
        else {
            write ("{");
            WriteTag ("v");
            writerContext.fNextHidableTextChangeAt += fHidableTextRuns[writerContext.fIthHidableTextRun].fElementLength;
        }
        writerContext.fHidableTextRegionOpen = not writerContext.fHidableTextRegionOpen;
    }

    // Style changes
    if (writerContext.GetCurSrcOffset () - 1 == writerContext.fNextStyleChangeAt) {
        const StandardStyledTextImager::InfoSummaryRecord& nextStyleRun = fStyleRunSummary[writerContext.fIthStyleRun];
        if (writerContext.GetCurSrcOffset () <= 1) { // 1+ cuz we've read one character
            EmitBodyFontInfoChange (writerContext, nextStyleRun);
        }
        else {
            EmitBodyFontInfoChange (writerContext, nextStyleRun, writerContext.fLastEmittedISR);
        }
        writerContext.fLastEmittedISR = nextStyleRun;
        writerContext.fNextStyleChangeAt += writerContext.fLastEmittedISR.fLength;
        writerContext.fIthStyleRun++;
    }

    if (writerContext.fCharsToSkip > 0) {
        --writerContext.fCharsToSkip;
        return;
    }

    switch (c) {
        case '\n': {
            WriteTag ("par");
            WriteStartParagraph (writerContext);
        } break;

        case '{': {
            write ("\\{");
        } break;

        case '}': {
            write ("\\}");
        } break;

        case '\\': {
            write ("\\\\");
        } break;

        case kEmbeddingSentinalChar: {
            unique_ptr<StyledTextIOWriter_RTF::Table> table (writerContext.GetCurRTFTable ());
            if (table.get () != nullptr) {
                Assert (writerContext.fCharsToSkip == 0); // must preserve / restore for nested tables?
                WriteTable (writerContext, table.get ());
                size_t x = table->GetOffsetEnd ();
                Assert (x >= 1);
                writerContext.fCharsToSkip = x - 1;
                break;
            }

            SimpleEmbeddedObjectStyleMarker* embedding = writerContext.GetCurSimpleEmbeddedObjectStyleMarker ();
            if (embedding == nullptr) {
                WriteHexCharHelper (kEmbeddingSentinalChar);
            }
            else {
                if (PossiblyWriteUnknownRTFEmbedding (writerContext, embedding)) {
                    break;
                }
                else if (PossiblyWriteOLERTFEmbedding (writerContext, embedding)) {
                    break;
                }
                else if (PossiblyWritePICTEmbedding (writerContext, embedding)) {
                    break;
                }
                WritePrivatLedEmbedding (writerContext, embedding);
            }
        } break;

        default: {
            if (c == fSoftLineBreakChar) {
                WriteTag ("line");
                break;
            }
#if qWideCharacters
            map<wchar_t, string>::const_iterator i  = fCharactersSavedByName_Char2Name.find (c);
            wchar_t                              uc = c;
#else
            CodePageConverter cvt (GetCurrentInputCharSetEncoding ());
            wchar_t uc = '\0';
            size_t ucCharCount = 1;
            cvt.MapToUNICODE (&c, 1, &uc, &ucCharCount);
            map<wchar_t, string>::const_iterator i = fCharactersSavedByName_Char2Name.find (uc);
#endif
            if (i == fCharactersSavedByName_Char2Name.end ()) {
                WritePlainUnicodeCharCharacterHelper (uc);
            }
            else {
                WriteTag ((*i).second.c_str ());
            }
        } break;
    }

    if ((writerContext.GetCurSrcOffset ()) % 80 == 0) {
        write ("\r\n"); // should write a newline every once in a while...
        // according to RTF spec
    }
}

void StyledTextIOWriter_RTF::WritePlainUnicodeCharCharacterHelper (wchar_t c)
{
    char   mbCharBuf[2];
    size_t mbCharCount = 2;
    CodePageConverter (fCurrentOutputCharSetEncoding).MapFromUNICODE (&c, 1, mbCharBuf, &mbCharCount);
    Assert (mbCharCount == 1 or mbCharCount == 2);

    bool needToWriteUNICODE = c >= 0x80; //  write UNICODE if non-ascii
    if (needToWriteUNICODE) {
        // write \uc\u stuff
        WriteTagNValue ("uc", mbCharCount); // keep track of prev value of this guy - so we don't need to write so often - in a context object???
        WriteTagNValue ("u", c);
    }

    unsigned char uc = mbCharBuf[0];
    if (uc < 0x80) {
        write (uc);
    }
    else {
        WriteHexCharHelper (uc);
    }
    if (mbCharCount == 2) {
        uc = mbCharBuf[1];
        if (uc < 0x80) {
            write (uc);
        }
        else {
            WriteHexCharHelper (uc);
        }
    }
}

void StyledTextIOWriter_RTF::WriteHexCharHelper (unsigned char c)
{
    // write hex char
    char buf[5];
    buf[0] = '\\';
    buf[1] = '\'';
    buf[2] = ConvertWriteSingleHexDigit (c / 16);
    buf[3] = ConvertWriteSingleHexDigit (c % 16);
    buf[4] = '\0';
    write (buf);
}

void StyledTextIOWriter_RTF::WriteStartParagraph (WriterContext& writerContext)
{
    WriteTag ("pard");
    if (writerContext.fInTable) {
        WriteTag ("intbl");
    }
    switch (writerContext.GetSrcStream ().GetJustification ()) {
        case eCenterJustify:
            WriteTag ("qc");
            break;
        case eRightJustify:
            WriteTag ("qr");
            break;
        case eFullyJustify:
            WriteTag ("qj");
            break;
        default:
            break; // ignore .... \\pard will capture this...
    }
    TextImager::StandardTabStopList tabStops = writerContext.GetSrcStream ().GetStandardTabStopList ();
    // assume we only save the specified tabstops, and that the default is already saved per-doc
    // since RTF1.4 doesn't seem to have a per-para 'deftabstop' value
    Led_TWIPS tabSoFar = Led_TWIPS (0);
    for (size_t i = 0; i < tabStops.fTabStops.size (); ++i) {
        tabSoFar += tabStops.fTabStops[i];
        WriteTagNValue ("tx", tabSoFar);
    }

    {
        Led_TWIPS fi = writerContext.GetSrcStream ().GetFirstIndent ();
        if (fi != 0) { // don't bother writing if default value
            WriteTagNValue ("fi", fi);
        }
    }

    {
        Led_TWIPS lhs = Led_TWIPS (0);
        Led_TWIPS rhs = Led_TWIPS (0);
        writerContext.GetSrcStream ().GetMargins (&lhs, &rhs);
        if (lhs != 0) { // don't bother writing if default value
            WriteTagNValue ("li", lhs);
        }

        Led_TWIPS effectiveDrawingWidth = fRTFInfo == nullptr ? RTFInfo ().GetEffectiveDrawingWidth () : fRTFInfo->GetEffectiveDrawingWidth ();
        Led_TWIPS rhsRTFMarginInTWIPS   = Led_TWIPS (effectiveDrawingWidth - rhs);
        if (rhsRTFMarginInTWIPS != 0) { // don't bother writing if default value
            WriteTagNValue ("ri", rhsRTFMarginInTWIPS);
        }
    }

    {
        Led_TWIPS sb = writerContext.GetSrcStream ().GetSpaceBefore ();
        if (sb != 0) {
            WriteTagNValue ("sb", sb);
        }
    }
    {
        Led_TWIPS sa = writerContext.GetSrcStream ().GetSpaceAfter ();
        if (sa != 0) {
            WriteTagNValue ("sa", sa);
        }
    }
    {
        Led_LineSpacing sl = writerContext.GetSrcStream ().GetLineSpacing ();
        if (sl.fRule != Led_LineSpacing::eSingleSpace) {
            Led_Coordinate rtfsl = 1000;
            bool           multi = true;
            mkRTFValues_From_LineSpacing (sl, &rtfsl, &multi);
            if (rtfsl != 1000) {
                WriteTagNValue ("sl", rtfsl);
                WriteTagNValue ("slmult", multi);
            }
        }
    }
    {
        ListStyle     listStyle   = eListStyle_None;
        unsigned char indentLevel = 0;
        writerContext.GetSrcStream ().GetListStyleInfo (&listStyle, &indentLevel);
        if (listStyle != eListStyle_None) {
            /// quickie - wrong list level and style - but tmp hack... SPR#0955
            {
                write ("{");
                WriteTag ("listtext");
                write ("\\'b7");
                WriteTag ("tab");
                write ("}");
            }

            //tmphack - REALLY must grab from REAL listtable records!!!
            int listTableID = 1;
            WriteTagNValue ("ls", listTableID);
            if (indentLevel != 0) {
                WriteTagNValue ("ilvl", indentLevel);
            }
        }
    }
}

void StyledTextIOWriter_RTF::WriteTable (WriterContext& writerContext, Table* table)
{
    using CellInfo = StyledTextIOWriter_RTF::Table::CellInfo;
    RequireNotNull (table);
    write ("\r\n"); // should write a newline every once in a while...
    // according to RTF spec
    size_t rows = table->GetRows ();
    for (size_t r = 0; r < rows; ++r) {
        WriteTag ("trowd");

        Led_TWIPS_Rect cellMargins = table->GetDefaultCellMarginsForRow (r);
        Led_TWIPS_Rect cellSpacing = table->GetDefaultCellSpacingForRow (r);
        {
            WriteTagNValue ("trgaph", (cellMargins.top + cellMargins.left + cellMargins.bottom + cellMargins.right) / 4);
        }
        {
            if (cellSpacing.top != 0) {
                WriteTagNValue ("trspdt", cellSpacing.top / 2);
                WriteTagNValue ("trspdft", 3);
            }
            if (cellSpacing.left != 0) {
                WriteTagNValue ("trspdl", cellSpacing.left / 2);
                WriteTagNValue ("trspdfl", 3);
            }
            if (cellSpacing.bottom != 0) {
                WriteTagNValue ("trspdb", cellSpacing.bottom / 2);
                WriteTagNValue ("trspdfb", 3);
            }
            if (cellSpacing.right != 0) {
                WriteTagNValue ("trspdr", cellSpacing.right / 2);
                WriteTagNValue ("trspdfr", 3);
            }
        }

        {
            if (cellMargins.top != 0) {
                WriteTagNValue ("trpaddt", cellMargins.top);
                WriteTagNValue ("trpaddft", 3);
            }
            if (cellMargins.left != 0) {
                WriteTagNValue ("trpaddl", cellMargins.left);
                WriteTagNValue ("trpaddfl", 3);
            }
            if (cellMargins.bottom != 0) {
                WriteTagNValue ("trpaddb", cellMargins.bottom);
                WriteTagNValue ("trpaddfb", 3);
            }
            if (cellMargins.right != 0) {
                WriteTagNValue ("trpaddr", cellMargins.right);
                WriteTagNValue ("trpaddfr", 3);
            }
        }

        vector<CellInfo> cellInfos;
        table->GetRowInfo (r, &cellInfos);
        size_t columns = cellInfos.size ();
        //<celldef>s
        {
            Led_TWIPS cellxSoFar = Led_TWIPS (0);
            for (vector<CellInfo>::const_iterator i = cellInfos.begin (); i != cellInfos.end (); ++i) {
                WriteTagNValue ("clcbpat", fColorTable->LookupColor (i->f_clcbpat));

                // This value of 3/2 * LHS is somewhat empirically derived from the output of MS Word XP. Its really quite
                // hadly documented - the relationship between cell spacing and cellx values.
                //      LGP 2003-05-01 - SPR#1396 (now corresponding change in reader)
                Led_TWIPS cellWidthIncludingSpacing = (*i).f_cellx + Led_TWIPS (3 * cellSpacing.left / 2);

                cellxSoFar += cellWidthIncludingSpacing;
                WriteTagNValue ("cellx", cellxSoFar); // NB: cellx marks the END of a <celldef> in the RTF 1.7 spec
            }
        }
// Actual CELLs
#if qDebug
        size_t nCellsWritten = 0;
#endif
        for (size_t c = 0; c < columns; c++) {
            unique_ptr<StyledTextIOWriter::SrcStream> srcStream = unique_ptr<StyledTextIOWriter::SrcStream> (table->MakeCellSubSrcStream (r, c));
            if (srcStream.get () != nullptr) {
                WriterContext                                       wc (writerContext, *srcStream.get ());
                vector<StandardStyledTextImager::InfoSummaryRecord> x = fStyleRunSummary;
                fStyleRunSummary.clear ();
                AssureStyleRunSummaryBuilt (wc);
                WriteBody (wc);
                fStyleRunSummary = x;
                WriteTag ("cell");
#if qDebug
                nCellsWritten++;
#endif
            }
        }
        Assert (nCellsWritten == cellInfos.size ()); //  must write same number of cells as celldefs
        WriteTag ("row");
        write ("\r\n"); // should write a newline every once in a while...
        // according to RTF spec

        // SPR#1406 must treat \row as same as \par - in that both trigger the start of a new paragraph
        // (pard generation)
        WriteStartParagraph (writerContext);
    }
}

bool StyledTextIOWriter_RTF::PossiblyWriteUnknownRTFEmbedding (WriterContext& /*writerContext*/, SimpleEmbeddedObjectStyleMarker* embedding)
{
// Now see if it is an RTF embedding, and if so, write it out.
#if qNoDeclaringVarInsideCondition
    RTFIO::UnknownRTFEmbedding* ee;
    if (ee = dynamic_cast<RTFIO::UnknownRTFEmbedding*> (embedding))
#else
    if (RTFIO::UnknownRTFEmbedding* ee = dynamic_cast<RTFIO::UnknownRTFEmbedding*> (embedding))
#endif
    {
        // Since UnknownRTFEmbedding is a simple typedef, it will map to any undefiend object embedding.
        // Which is fine, its what we want. But we only want to insert the ones which we created from RTF.
        // This is important cuz others probably don't actually contain valid rtf. And important to tkae
        // any unknown object which has this type cuz could have gotten here via cut/paste, undo, etc,
        // so the object would have lost its dynamic type anyhow (if we counted on an exact
        // match to a subtype RTFIO::UnknownRTFEmbedding).
        if (memcmp (ee->GetTag (), RTFIO::kRTFBodyGroupFragmentEmbeddingTag, sizeof (RTFIO::kRTFBodyGroupFragmentEmbeddingTag)) == 0) {
            write (ee->GetData (), ee->GetDataLength ());
            return true;
        }
    }
    return false;
}

bool StyledTextIOWriter_RTF::PossiblyWriteOLERTFEmbedding (WriterContext& /*writerContext*/, SimpleEmbeddedObjectStyleMarker* embedding)
{
// Now see if it is an OLE RTF embedding, and if so, write it out.
#if qNoDeclaringVarInsideCondition
    RTFIO::RTFOLEEmbedding* anRTFOLEEmbedding;
    if (anRTFOLEEmbedding = dynamic_cast<RTFIO::RTFOLEEmbedding*> (embedding))
#else
    if (RTFIO::RTFOLEEmbedding* anRTFOLEEmbedding = dynamic_cast<RTFIO::RTFOLEEmbedding*> (embedding))
#endif
    {
        // Get encoded format of the object in OLE1 OLESTREAM format
        size_t nBytes;
        Byte*  theDataBytes = nullptr;
        anRTFOLEEmbedding->DoWriteToOLE1Stream (&nBytes, &theDataBytes);

        write ("\r\n");
        write ("{");
        WriteTag ("object");
        WriteTag ("objemb");
        Led_Size size = anRTFOLEEmbedding->GetSize ();
        WriteTagNValue ("objh", Led_CvtScreenPixelsToTWIPSV (size.v));
        WriteTagNValue ("objw", Led_CvtScreenPixelsToTWIPSH (size.h));

        //      WriteTagNValue ("objh", 100);   // get real values here!
        //      WriteTagNValue ("objw", 100);   // get real values here!
        //&&&FIX&&&objw ETC

        string className = Led_SDKString2ANSI (anRTFOLEEmbedding->GetObjClassName ());
        if (not className.empty ()) {
            write ("{\\*\\objclass ");
            // probably SHOULD check className doesn't have any bad characters, like a "{" - or some such...
            // But I think Win32 may protect us against such bad names in ProgIDs?
            write (className.c_str ());
            write ("}");
        }

        //Write the ObjData block
        write ("{");
        write ("\\*\\objdata ");
        WriteHexCharDataBlock (nBytes, theDataBytes);
        delete[] theDataBytes;
        write ("}");

        // Someday add support for \result - but not TODAY

        // DONE
        write ("}");

        return true;
    }
    return false;
}

// WOULD LIKE TO WRITE BOTH dibitmap and wmetafile - but then I can only read dibitmap on MAC (maybe COULD read wmetafile on Mac if I studied wmetafile format better!
// Also - for some reason - I cannot get MSWord to read consistently when I write either DIB or WMF. I THINK there is a bug when I write WMF - cuz I don't read it
// back myself consistently. BUt then - neither does MSWord! I can dump a picture into MSWord 2000 - and save, and reopen - using no app but MSWord 2000 - and it
// loses the image (sometimes only transiently).
// Anyhow - Writing as DIB seems like the best option for the time being...
#define qWriteAsDIB 1
bool StyledTextIOWriter_RTF::PossiblyWritePICTEmbedding (WriterContext& /*writerContext*/, SimpleEmbeddedObjectStyleMarker* embedding)
{
// Now see if it is an OLE RTF embedding, and if so, write it out.
#if qNoDeclaringVarInsideCondition
    StandardDIBStyleMarker* aPictEmbedding;
    if (aPictEmbedding = dynamic_cast<StandardDIBStyleMarker*> (embedding))
#else
    if (StandardDIBStyleMarker* aPictEmbedding = dynamic_cast<StandardDIBStyleMarker*> (embedding))
#endif
    {
        write ("\r\n");
        write ("{");
        WriteTag ("pict");
        const Led_DIB* dib  = aPictEmbedding->GetDIBData ();
        Led_Size       size = Led_GetDIBImageSize (dib);
#if !qWriteAsDIB
        int vEnhSize = size.v * (1400 / 10) * 2.54;
        int hEnhSize = size.h * (1400 / 10) * 2.54;
        WriteTagNValue ("pich", size.v * 20);
        WriteTagNValue ("picw", size.h * 20);
#endif
        WriteTagNValue ("pichgoal", Led_CvtScreenPixelsToTWIPSV (size.v));
        WriteTagNValue ("picwgoal", Led_CvtScreenPixelsToTWIPSH (size.h));

#if qWriteAsDIB
        WriteTag ("dibitmap");
        const void* theDataBytes = dib;
        size_t      nBytes       = Led_GetDIBImageByteCount (dib);

        // See SPR#0811 for details
        WriteTagNValue ("pich", Led_CvtScreenPixelsToTWIPSV (size.v));
        WriteTagNValue ("picw", Led_CvtScreenPixelsToTWIPSH (size.h));
        WriteTagNValue ("wbmbitspixel", dib->bmiHeader.biBitCount == 0 ? 24 : dib->bmiHeader.biBitCount);
        WriteTagNValue ("wbmplanes", dib->bmiHeader.biPlanes == 0 ? 1 : dib->bmiHeader.biPlanes);
        WriteTagNValue ("wbmwidthbytes", Led_GetDIBImageRowByteCount (dib));
#else
        WriteTagNValue ("wmetafile", 8); // not sure what 8 means - but thats what MSWord 2000 seems to write - LGP 2000-07-08

        void* theDataBytes = nullptr;
        size_t nBytes = 0;
        unique_ptr<BYTE> theDataBytes_;
        {
            Led_Tablet_ screenDC = (::GetWindowDC (nullptr)); // not sure what DC to use to convert MetaFile to DIB - but this seems like a decent guess
            UINT mapMode = MM_TEXT;
            //UINT          mapMode =   MM_TWIPS;
            HENHMETAFILE hMF = nullptr;
            {
                RECT rect = AsRECT (Led_Rect (0, 0, vEnhSize, hEnhSize));
                HDC hMFDC = ::CreateEnhMetaFile (nullptr, nullptr, &rect, nullptr);
                ::SetMapMode (hMFDC, mapMode);
                const char* lpBits = reinterpret_cast<const char*> (dib) + Led_GetDIBPalletByteCount (dib) + sizeof (BITMAPINFOHEADER);
                Assert (mapMode == MM_TWIPS or mapMode == MM_TEXT);
                if (mapMode == MM_TWIPS) {
                    ::StretchDIBits (hMFDC, 0, 0, Led_CvtScreenPixelsToTWIPSH (size.h), Led_CvtScreenPixelsToTWIPSV (size.v), 0, 0, size.h, size.v, lpBits, dib, DIB_RGB_COLORS, SRCCOPY);
                }
                else if (mapMode == MM_TEXT) {
                    ::StretchDIBits (hMFDC, 0, 0, size.h, size.v, 0, 0, size.h, size.v, lpBits, dib, DIB_RGB_COLORS, SRCCOPY);
                }
                hMF = ::CloseEnhMetaFile (hMFDC);
            }
            nBytes = ::GetWinMetaFileBits (hMF, 0, nullptr, mapMode, screenDC);
            if (nBytes == 0) {
                Assert (false); //??
                return false;   //??? ERROR
            }
            BYTE* bytes = new BYTE[nBytes];
            Verify (::GetWinMetaFileBits (hMF, nBytes, bytes, mapMode, screenDC) == nBytes);

            theDataBytes = bytes;
            theDataBytes_ = unique_ptr<BYTE> (bytes);

            ::DeleteEnhMetaFile (hMF);
        }
#endif

        //Write the ObjData block
        write ("\r\n");
        WriteHexCharDataBlock (nBytes, theDataBytes);

        // DONE
        write ("}");

        return true;
    }
    return false;
}

#if qGCC_OptBugWithLocalClassesScopedInFunction
struct VectorSinkStream : SimpleEmbeddedObjectStyleMarker::SinkStream {
public:
    virtual void write (const void* buffer, size_t bytes) override
    {
        using ci = const char*;
        fData.insert (fData.end (), ci (buffer), ci (buffer) + bytes);
    }
    vector<char> fData;
};
#endif
void StyledTextIOWriter_RTF::WritePrivatLedEmbedding (WriterContext& /*writerContext*/, SimpleEmbeddedObjectStyleMarker* embedding)
{
#if qBorlandNameInLocalFunctDeclarationSpaceCompilerBug
    using namespace Led;
#endif
#if !qGCC_OptBugWithLocalClassesScopedInFunction
    struct VectorSinkStream : SimpleEmbeddedObjectStyleMarker::SinkStream {
    public:
        virtual void write (const void* buffer, size_t bytes) override
        {
            using ci = const char*;
            fData.insert (fData.end (), ci (buffer), ci (buffer) + bytes);
        }
        vector<char> fData;
    };
#endif
    VectorSinkStream embeddingData;
    embedding->Write (embeddingData);

    write ("\r\n");
    write ("{");
    WriteTag ("object");
    WriteTag ("ledprivateobjectembeddingformat");
    //      WriteTagNValue ("objh", 100);   // get real values here!
    //      WriteTagNValue ("objw", 100);   // get real values here!

    //Write the ObjData block
    write ("{");
    write ("\\*\\objdata ");
    WriteHexCharDataBlock (sizeof (Led_PrivateEmbeddingTag), embedding->GetTag ());
    WriteHexCharDataBlock (embeddingData.fData.size (), &embeddingData.fData.front ());
    write ("}");

    // Someday add support for \result - but not TODAY

    // DONE
    write ("}");
}

void StyledTextIOWriter_RTF::WriteTag (const char* tagStr)
{
    RequireNotNull (tagStr);
    Require (tagStr[0] != '\\');           // we write that ourselves
    Require (::strchr (tagStr, ' ') == 0); // we write trailing space, and better note be others!
    write ('\\');
    write (tagStr);
    write (' ');
}

void StyledTextIOWriter_RTF::WriteTagNValue (const char* tagStr, int value)
{
    RequireNotNull (tagStr);
    Require (tagStr[0] != '\\');           // we write that ourselves
    Require (::strchr (tagStr, ' ') == 0); // we write trailing space, and better note be others!
    write ('\\');
    write (tagStr);
    char buf[1024];
    (void)std::snprintf (buf, NEltsOf (buf), "%d", value);
    write (buf);
    write (' ');
}

void StyledTextIOWriter_RTF::WriteHexCharDataBlock (size_t nBytes, const void* resultData)
{
    const unsigned char* start = (const unsigned char*)resultData;
    const unsigned char* end   = start + nBytes;
    for (const unsigned char* cur = start; cur != end; ++cur) {
        WriteRTFHexByte (*cur);
        if ((cur - start) % 50 == 0) {
            write ("\r\n"); // should write a newline every once in a while...
            // according to RTF spec
        }
    }
}

void StyledTextIOWriter_RTF::WriteRTFHexByte (unsigned char theByte)
{
    unsigned char hiNibble  = (theByte) >> 4;
    unsigned char lowNibble = (theByte)&0xf;
    Require (hiNibble <= 0xf);
    Require (lowNibble <= 0xf);
    write (ConvertWriteSingleHexDigit (hiNibble));
    write (ConvertWriteSingleHexDigit (lowNibble));
}

void StyledTextIOWriter_RTF::WriteDocCharset ()
{
    switch (fDocumentCharacterSet) {
        case kCodePage_ANSI:
            WriteTag ("ansi");
            break;
        case kCodePage_MAC:
            WriteTag ("mac");
            break;
        case kCodePage_PC:
            WriteTag ("pc");
            break;
        case kCodePage_PCA:
            WriteTag ("pca");
            break;
        default: {
            // error?
        }
    }
}

void StyledTextIOWriter_RTF::WriteFontTable (WriterContext& writerContext)
{
    write ('{');
    WriteTag ("fonttbl");

    AssureFontTableBuilt (writerContext);
    AssertNotNull (fFontTable);

    size_t entryCount = fFontTable->fEntries.size ();
    for (size_t i = 0; i < entryCount; i++) {
        WriteFontTablesEntry (fFontTable->fEntries[i]);
    }

    write ('}');
}

void StyledTextIOWriter_RTF::WriteFontTablesEntry (const FontTableEntry& entry)
{
    write ('{');

    WriteTagNValue ("f", entry.fFNum);

    switch (entry.fFamily) {
        case FontTableEntry::eNil:
            WriteTag ("fnil");
            break;
        case FontTableEntry::eRoman:
            WriteTag ("froman");
            break;
        case FontTableEntry::eSwiss:
            WriteTag ("fswiss");
            break;
        case FontTableEntry::eModern:
            WriteTag ("fmodern");
            break;
        case FontTableEntry::eScript:
            WriteTag ("fscript");
            break;
        case FontTableEntry::eDecor:
            WriteTag ("fdecor");
            break;
        case FontTableEntry::eTech:
            WriteTag ("ftech");
            break;
        case FontTableEntry::eBidi:
            WriteTag ("fbidi");
            break;
    }

    if (entry.fCharSet != static_cast<Byte> (-1)) {
        WriteTagNValue ("fcharset", entry.fCharSet);
    }

    if (entry.fPitch != 0) {
        WriteTagNValue ("fprq", entry.fPitch);
    }

    write (Led_SDKString2ANSI (entry.fFontName));

    write (';');

    write ('}');
}

void StyledTextIOWriter_RTF::WriteColorTable (WriterContext& writerContext)
{
    AssureColorTableBuilt (writerContext);
    AssertNotNull (fColorTable);

    write ('{');
    WriteTag ("colortbl");

    size_t entryCount = fColorTable->fEntries.size ();
    for (size_t i = 0; i < entryCount; i++) {
        Led_Color c = fColorTable->LookupColor (i);
        char      buf[1024];
        (void)std::snprintf (buf, NEltsOf (buf), "\\red%d\\green%d\\blue%d;", c.GetRed () >> 8, c.GetGreen () >> 8, c.GetBlue () >> 8);
        write (buf);
    }

    write ('}');
}

void StyledTextIOWriter_RTF::WriteListTable ()
{
    //tmphack...
    RTFIO::ListTableEntry lte;
    lte.fListID         = rand ();
    lte.fListTemplateID = rand ();
    lte.fListStyle      = eListStyle_Bullet;
    lte.fFontID         = 0; // \fN should map to Symbol font????? Find font with bullet char in it....
    RTFIO::ListOverrideTableEntry lote;
    lote.fListID = lte.fListID;

    write ("\r\n{\\*\\listtable");
    WriteListTablesEntry (lte);
    write ("}\r\n");

    write ("{\\*\\listoverridetable");
    WriteListOverrideTablesEntry (lote);
    write ("}\r\n");
}

void StyledTextIOWriter_RTF::WriteListTablesEntry (const RTFIO::ListTableEntry& entry)
{
    write ("{");
    WriteTag ("list");
    WriteTagNValue ("listtemplateid", entry.fListTemplateID);
    write ("{");
    WriteTag ("listlevel");
    WriteTagNValue ("levelnfc", entry.fListStyle);
    WriteTagNValue ("leveljc", 0);
    WriteTagNValue ("levelfollow", 0);
    WriteTagNValue ("levelstartat", 1);
    WriteTagNValue ("levelindent", 0);
    write ("{");
    int levelTemplateID = rand ();
    WriteTag ("leveltext");
    WriteTagNValue ("levelnfc", entry.fListStyle);
    WriteTagNValue ("leveltemplateid", levelTemplateID);
    /*
     * Originally cloned/copied MSWord2k output was:
     *      write ("\\'01\\u-3913 ?;");
     *
     *  Instead now - we write out the 0x2022 bullet character (MSWord assumed Symbol font).
     */
    write ("\\'01");
    WriteTagNValue ("u", 0x2022); // bullet character
    write (" ?;");
    write ("}");
    WriteTagNValue ("f", entry.fFontID);
    WriteTagNValue ("fi", -360);
    WriteTagNValue ("li", 720);
    WriteTag ("jclisttab");
    WriteTagNValue ("tx", 720);
    write ("}");
    WriteTagNValue ("listid", entry.fListID);
    write ("}");
}

void StyledTextIOWriter_RTF::WriteListOverrideTablesEntry (const RTFIO::ListOverrideTableEntry& oEntry)
{
    write ("{");
    WriteTag ("listoverride");
    WriteTagNValue ("listid", oEntry.fListID);
    WriteTagNValue ("listoverridecount", 0);
    WriteTagNValue ("ls", 1);
    write ("}");
}

void StyledTextIOWriter_RTF::WriteGenerator ()
{
    write ("{\\*");
    WriteTag ("generator");
    write ("Sophist Solutions, Inc. Led RTF IO Engine - " qLed_ShortVersionString);
    write (";");
    write ("}");
}

void StyledTextIOWriter_RTF::EmitBodyFontInfoChange (WriterContext& writerContext, const Led_FontSpecification& newOne)
{
    RequireNotNull (fFontTable);

    WriteTag ("plain");

    if (writerContext.fHidableTextRegionOpen) {
        WriteTag ("v"); // cuz plain resets the \v flag
    }

    {
        RequireNotNull (fFontTable);
        const FontTableEntry* fe = fFontTable->LookupEntryByName (newOne.GetFontName ());
        RequireNotNull (fe); // this routine cannot be called with an invalid font table setup. It would imply
        // a bug in the AssureFontTableBuilt () code, most probably...
        WriteTagNValue ("f", fe->fFNum);
    }

    // font size
    WriteTagNValue ("fs", newOne.GetPointSize () * 2);

    // font styles
    if (newOne.GetStyle_Bold ()) {
        WriteTag ("b");
    }
    if (newOne.GetStyle_Italic ()) {
        WriteTag ("i");
    }
    if (newOne.GetStyle_Underline ()) {
        WriteTag ("ul");
    }
    switch (newOne.GetStyle_SubOrSuperScript ()) {
        case Led_FontSpecification::eSubscript:
            WriteTag ("sub");
            break;
        case Led_FontSpecification::eSuperscript:
            WriteTag ("super");
            break;
    }
#if qPlatform_Windows
    if (newOne.GetStyle_Strikeout ()) {
        WriteTag ("strike");
    }
#endif

    WriteTagNValue ("cf", fColorTable->LookupColor (newOne.GetTextColor ()));
}

void StyledTextIOWriter_RTF::EmitBodyFontInfoChange (WriterContext& writerContext, const Led_FontSpecification& newOne, const Led_FontSpecification& /*oldOne*/)
{
    // lets be simplistic to start with...
    EmitBodyFontInfoChange (writerContext, newOne);
}

void StyledTextIOWriter_RTF::AssureColorTableBuilt (WriterContext& writerContext)
{
    if (fColorTable == nullptr) {
        fColorTable = new RTFIO::ColorTable ();
        set<Led_Color> colorsUsed;
        writerContext.GetSrcStream ().SummarizeFontAndColorTable (nullptr, &colorsUsed);
        for (set<Led_Color>::const_iterator i = colorsUsed.begin (); i != colorsUsed.end (); i++) {
            (void)fColorTable->EnterColor (*i);
        }
    }
}

#if qPlatform_Windows
namespace {
    BOOL FAR PASCAL Save_Charset_EnumFontFamiliesProc (ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int /*fontType*/, LPVOID pCharset)
    {
        BYTE* charSet = reinterpret_cast<BYTE*> (pCharset);
        *charSet      = pelf->elfLogFont.lfCharSet;
        return 1;
    }
}
#endif

void StyledTextIOWriter_RTF::AssureFontTableBuilt (WriterContext& writerContext)
{
    if (fFontTable == nullptr) {
        fFontTable = new FontTable (); // no need to try/catch cuz its stored in instance var, and will get destroyed
        // on StyledTextIOWriter_RTF::DTOR
        set<Led_SDK_String> fontNames;
        writerContext.GetSrcStream ().SummarizeFontAndColorTable (&fontNames, nullptr);
#if qPlatform_Windows
        Led_WindowDC screenDC (nullptr);
#endif
        for (set<Led_SDK_String>::const_iterator i = fontNames.begin (); i != fontNames.end (); i++) {
            const Led_SDK_String& name = *i;
            if (fFontTable->LookupEntryByName (name) == nullptr) {
                FontTableEntry fte;
                fte.fFontName = name;
//
// OLD COMMENTS:
//      Led 3.0 code had this. SHOULD do something similar - but I'm not sure its worth it. Maybe do differently using
//      design suggested by some Led customer with my own builtin table of font mapping info.
//
//      Must fill in LOTS more data here - LGP 2000/04/28
//      In particular - fill in the fCharset field - so we know what charset to use
//      when writing multibyte text!
//
//  NEW COMMENTS:
//      Very minimal support to get this working as well as it did for Led 3.0. SPR#1577.
//      Got basically working enough to fix this bug.
//
#if qPlatform_MacOS
                if (name == Led_SDK_TCHAROF ("New York")) {
                    fte.fFamily = FontTableEntry::eSwiss;
                }
                else if (name == Led_SDK_TCHAROF ("Geneva")) {
                    fte.fFamily = FontTableEntry::eRoman;
                }
                else if (name == Led_SDK_TCHAROF ("Monaco")) {
                    fte.fFamily = FontTableEntry::eModern;
                }
                else if (name == Led_SDK_TCHAROF ("Helvetica")) {
                    fte.fFamily = FontTableEntry::eSwiss;
                }
                else if (name == Led_SDK_TCHAROF ("Symbol")) {
                    fte.fFamily = FontTableEntry::eTech;
                }
                else if (name == Led_SDK_TCHAROF ("Times")) {
                    fte.fFamily = FontTableEntry::eRoman;
                }
#elif qPlatform_Windows
                LOGFONT lf;
                (void)::memset (&lf, 0, sizeof (lf));
                Characters::CString::Copy (lf.lfFaceName, NEltsOf (lf.lfFaceName), name.c_str ());
                lf.lfCharSet = DEFAULT_CHARSET;
                BYTE useCharset = DEFAULT_CHARSET;
                ::EnumFontFamiliesEx (screenDC.m_hDC, &lf, (FONTENUMPROC)Save_Charset_EnumFontFamiliesProc, reinterpret_cast<LPARAM> (&useCharset), 0);
                if (useCharset != DEFAULT_CHARSET) {
                    fte.fCharSet = useCharset;
                }
#endif

                fFontTable->Add (fte);
            }
        }
    }
}

void StyledTextIOWriter_RTF::AssureStyleRunSummaryBuilt (WriterContext& writerContext)
{
    if (fStyleRunSummary.empty ()) {
        size_t totalTextLength = writerContext.GetSrcStream ().GetTotalTextLength ();
        if (totalTextLength != 0) {
            fStyleRunSummary = writerContext.GetSrcStream ().GetStyleInfo (0, totalTextLength);
        }
    }
}

void StyledTextIOWriter_RTF::AssureListTableBuilt (WriterContext& /*writerContext*/)
{
    if (fListTable == nullptr) {
        fListTable = new RTFIO::ListTables ();
    }
}
