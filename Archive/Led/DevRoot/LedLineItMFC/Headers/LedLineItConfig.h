/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItConfig__
#define	__LedLineItConfig__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/LedLineItConfig.h,v 2.13 2004/02/26 03:43:05 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedLineItConfig.h,v $
 *	Revision 2.13  2004/02/26 03:43:05  lewis
 *	added qSupportGenRandomCombosCommand hack my dad can more easily generate a bunch of potential namess
 *	
 *	Revision 2.12  2003/12/11 03:09:12  lewis
 *	SPR#1585: move spellcheckengine object to application. Use qIncludeBasicSpellcheckEngine define to control its inclusion. Define default locaiton for LedLineIt spell dict.
 *	
 *	Revision 2.11  2003/11/26 23:19:52  lewis
 *	set maxundolevels to 512 - based on SPR#1565
 *	
 *	Revision 2.10  2002/12/04 16:50:16  lewis
 *	accidentally checked in with qSupportSyntaxColoring off. Set back to on
 *	
 *	Revision 2.9  2002/12/02 14:22:23  lewis
 *	use struct subclass instead of typedef for non supportsyntaxcoloring case cuz of wierd
 *	compile error (not worth debugging)
 *	
 *	Revision 2.8  2002/05/06 21:31:13  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.7  2001/11/27 00:28:19  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.6  2001/08/30 01:01:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  2001/07/31 15:51:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  2000/04/14 22:41:02  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.3  1999/12/24 23:13:19  lewis
 *	Add (conditioanl on qSupportSyntaxColoring) new Sytnax coloring support. Based on
 *	old tutorial code - but greatly cleaned up and integrated into Led
 *	
 *	Revision 2.2  1997/12/24 04:50:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.1  1997/07/27  16:02:50  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.0  1997/01/10  03:35:47  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 *
 */

#include	"LedConfig.h"



#if		defined (__cplusplus)
	// Test a bit, but I think undo information no longer consumes lots of memory, so we can keep a larger
	// (effictively infinite) undo buffer (SPR#1565) - LGP 2003-11-26
	const	unsigned int	kMaxNumUndoLevels	=	1024;
#endif

#if		qLedUsesNamespaces
using	namespace	Led;
#endif


#define	qSupportGenRandomCombosCommand	0


#define	qSupportSyntaxColoring	1


#if		qSupportSyntaxColoring
	/*
	 *	qSupportOnlyMarkersWhichOverlapVisibleRegion attempts to only keep track of markers
	 *	which will overlap the region displayed
	 *	currently in the window. The premise is that we would rather save the memory for all
	 *	the undisplayed areas, and we wish to save the time it takes to analyze all that text
	 *	(in the case of huge documents), and yet we assume we can always analyze a page of text
	 *	quickly enuf for scrolling purposes. If any of this isn't true, you can try always
	 *	computing the whole thing. Really thats easier.
	 *
	 *	NB: With qSupportOnlyMarkersWhichOverlapVisibleRegion TRUE,
	 *		(a)	when we've scrolled,  we must call SyntaxColoringMarkerOwner::RecheckScrolling ()
	 *		(b)	when font metrics changed, we must call SyntaxColoringMarkerOwner::RecheckAll ()
	 */
	#ifndef	qSupportOnlyMarkersWhichOverlapVisibleRegion
		#define	qSupportOnlyMarkersWhichOverlapVisibleRegion	1
	#endif
#endif


#define	qIncludeBasicSpellcheckEngine		1


#endif	/*__LedLineItConfig__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
