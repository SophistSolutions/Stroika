/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef __LedLineItConfig__
#define __LedLineItConfig__ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/Led/Config.h"

#if defined(__cplusplus)

static_assert (qHasFeature_ATLMFC, "Error: LedLineIt requires the ATLMFC feature to be set true when building Stroika");

// Test a bit, but I think undo information no longer consumes lots of memory, so we can keep a larger
// (effictively infinite) undo buffer (SPR#1565) - LGP 2003-11-26
const unsigned int kMaxNumUndoLevels = 1024;
#endif

#define qSupportGenRandomCombosCommand 0

#define qSupportSyntaxColoring 1

#if qSupportSyntaxColoring
/*
 *  qSupportOnlyMarkersWhichOverlapVisibleRegion attempts to only keep track of markers
 *  which will overlap the region displayed
 *  currently in the window. The premise is that we would rather save the memory for all
 *  the undisplayed areas, and we wish to save the time it takes to analyze all that text
 *  (in the case of huge documents), and yet we assume we can always analyze a page of text
 *  quickly enuf for scrolling purposes. If any of this isn't true, you can try always
 *  computing the whole thing. Really thats easier.
 *
 *  NB: With qSupportOnlyMarkersWhichOverlapVisibleRegion TRUE,
 *      (a) when we've scrolled,  we must call SyntaxColoringMarkerOwner::RecheckScrolling ()
 *      (b) when font metrics changed, we must call SyntaxColoringMarkerOwner::RecheckAll ()
 */
#ifndef qSupportOnlyMarkersWhichOverlapVisibleRegion
#define qSupportOnlyMarkersWhichOverlapVisibleRegion 1
#endif
#endif

#define qIncludeBasicSpellcheckEngine 1

#endif /*__LedLineItConfig__*/
