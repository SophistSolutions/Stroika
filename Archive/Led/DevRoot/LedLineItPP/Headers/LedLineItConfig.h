/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItConfig__
#define	__LedLineItConfig__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItPP/Headers/LedLineItConfig.h,v 2.10 2003/12/13 02:00:05 lewis Exp $
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
 *	Revision 2.10  2003/12/13 02:00:05  lewis
 *	small changes so uses new spellcheck code (and stores engine in applicaiton object)
 *	
 *	Revision 2.9  2002/05/06 21:31:26  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.8  2001/11/27 00:28:26  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.7  2001/08/30 01:01:07  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2001/07/19 21:06:20  lewis
 *	SPR#0960- CW6Pro support
 *	
 *	Revision 2.5  2000/04/15 14:43:05  lewis
 *	qLedUsesNamespaces support, and FlavorPackage.cpp added
 *	
 *	Revision 2.4  1999/12/24 23:23:53  lewis
 *	add PreLim (qSupportSyntaxColoring) support.
 *	
 *	Revision 2.3  1997/12/24 04:52:10  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  1997/07/27  16:03:25  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/03/04  20:27:02  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/01/10  03:51:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */

#include	"LedConfig.h"

#define	kApplicationSignature					'LLAP'

#define kTEXTFileType							'TEXT'
#define kTEXTStationeryFileType					'sEXT'



#if		defined (__cplusplus)
	#if		qLedUsesNamespaces
	using	namespace	Led;
	#endif
	// right now, undo levels waste LOTS of memory, and can slow things down. Leave this number small til
	// we tweek the implementation a bit more - LGP 960119
	const	int	kMaxNumUndoLevels	=	3;
#endif


// Eventually make it a runtime choice based on a user-config dialog.
#ifndef	qSupportAutoIndent
	#define	qSupportAutoIndent	1
#endif


// Eventually make it a runtime choice based on a user-config dialog.
#ifndef	qSupportSmartCutNPaste
	#define	qSupportSmartCutNPaste	1
#endif

#define	qIncludeBasicSpellcheckEngine	1


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



#endif	/*__LedLineItConfig__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
