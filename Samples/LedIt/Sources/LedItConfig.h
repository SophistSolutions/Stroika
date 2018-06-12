/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#ifndef __LedItConfig__
#define __LedItConfig__ 1

#include "Stroika/Foundation/StroikaPreComp.h"
#include "Stroika/Frameworks/Led/Config.h"

#if defined(__cplusplus)
#include "Stroika/Frameworks/Led/GDI.h"
#endif

#if qPlatform_MacOS
#define kApplicationSignature 'LDAP'

#define kTEXTFileType 'TEXT'
#define kTEXTStationeryFileType 'sEXT'
#define kLedPrivateDocumentFileType 'LDDC'           // LedIt! Portable (mac/pc) rich text document
#define kLedPrivateDocumentStationeryFileType 'sDDC' // LedIt! Portable (mac/pc) rich text stationary
#endif

// Didn't make it into 2.0 - sigh...LGP 960520
#define qSupportPasteSpecial 0

#ifndef qSupportStyleSheets
#define qSupportStyleSheets 0
#endif

#define qURLStyleMarkerNewDisplayMode 1

#ifndef qIncludeBasicSpellcheckEngine
#define qIncludeBasicSpellcheckEngine 1
#endif

#if defined(__cplusplus)

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

static_assert (qHasFeature_ATLMFC, "Error: LedIt requires the ATLMFC feature to be set true when building Stroika");

enum FileFormat {
    eTextFormat,
    eLedPrivateFormat,
    eRTFFormat,
    eHTMLFormat,
    eUnknownFormat,
    eDefaultFormat = eRTFFormat
};
#endif

#if defined(__cplusplus)
// Due to work on SPR#1564 and 1565 - up this undo count from 3 to something effectively infinite.
const unsigned int kMaxNumUndoLevels = 512;
#endif

#if defined(__cplusplus)
const Led_TWIPS kLedItViewTopMargin    = Led_TWIPS (120);
const Led_TWIPS kLedItViewBottomMargin = Led_TWIPS (0);
const Led_TWIPS kLedItViewLHSMargin    = Led_TWIPS (150);
const Led_TWIPS kLedItViewRHSMargin    = Led_TWIPS (0);
#endif

// Eventually make it a runtime choice based on a user-config dialog (on mac - already chooseable for PC).
//#ifndef   qSupportSmartCutNPaste
//  #define qSupportSmartCutNPaste  1
//#endif

#endif /*__LedItConfig__*/
