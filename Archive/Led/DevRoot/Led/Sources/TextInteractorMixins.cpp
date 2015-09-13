/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/TextInteractorMixins.cpp,v 2.32 2002/05/06 21:33:59 lewis Exp $
 *
 * Changes:
 *	$Log: TextInteractorMixins.cpp,v $
 *	Revision 2.32  2002/05/06 21:33:59  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.31  2001/11/27 00:30:04  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.30  2001/08/29 23:36:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.29  2001/08/28 18:43:44  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.28  1999/11/13 16:32:24  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.27  1999/05/03 22:05:22  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.26  1997/12/24 04:40:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.25  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.24  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.23  1997/06/18  03:14:08  lewis
 *	Moved StandardStyledWordWrappedTextInteractor, SimpleTextInteractor, and
 *	WordWrappedTextInteractor
 *	to their own files.
 *	qIncludePrefixFile
 *
 *	Revision 2.22  1997/01/20  05:29:51  lewis
 *	Scrolling cleanups, and support for new eDelayedUpdate (actually that support was to enable
 *	some of the scrolling cleanups).
 *
 *	Revision 2.21  1997/01/10  03:10:34  lewis
 *	HUGE changes
 *	Moved Scrollbits stuff (ScrollByLine) to TextInteractor_::SetTopRowIn...().
 *	(part of major - but incomplete - scrolling re-org).
 *	See header for changes - now templated support for combining imagers/interactors for
 *	mixins.
 *
 *	Revision 2.20  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.19  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1996/10/31  00:11:34  lewis
 *	Cleanup/fix scrollbits based scrolling - for pc use ScrollWindow instead of ScrollDC.
 *	Tweek scrolling on (mostly mac) up by doing xtra Update() call so logical clipping
 *	optimization works better.
 *
 *	Revision 2.17  1996/09/30  14:26:47  lewis
 *	Added support for qSupportWholeWindowInvalidOptimization
 *
 *	Revision 2.16  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.15  1996/07/19  16:44:25  lewis
 *	something of a hack to work aound style- (fEmptyStyle) bug - don't call
 *	StandardStyledTextIntercot::SetSelection_() if no style change in
 *	mixin method. Reason is to avoid resting the empty style record.
 *	Funny interaction and called several times in the way we implemented
 *	<BACKSPACE> key. Should probably find some cleaner way...
 *
 *	Revision 2.14  1996/07/03  01:16:25  lewis
 *	Fixed a remaining windows display glitch with ScrollRect() code.
 *	I still don't trust the code cuz of the various kludgy offsets I don't
 *	understand, but it appears to work on WinNT and Win95 - so I guess we'll
 *	live with it til someone reports a bug.
 *
 *	Revision 2.13  1996/06/01  02:11:54  lewis
 *	Fix scrollRect() bugs (in case window obscured by some other window - deal
 *	with bad bits).
 *
 *	Revision 2.12  1996/05/23  19:44:19  lewis
 *	SetDefaultFont() no longer does a GetDefaultFont () != defaultFont - wasn't really right
 *	since we were comparing real font with incrmental one.
 *	Added comment there about REAL fix to this problem and how to better address teh
 *	UpdateMode stuff.
 *
 *	Revision 2.11  1996/05/14  20:27:07  lewis
 *	CLeanup mixins HookLosingTextStore/HookGainedNewTextStore.
 *	Added conditioanlly new qDoScrollbitsHack code.
 *
 *	Revision 2.10  1996/02/26  22:02:41  lewis
 *	SetTopRowInWindowByMarkerPosition now overloaded to take updatemode.
 *	Renamed TextInteracter --> TextInteractor.
 *
 *	Revision 2.9  1996/02/05  05:23:18  lewis
 *	Moved alot of this stuff to new file/class StyledTextInteractor.
 *	Also - redid the Specify Text STore code/API
 *
 *	Revision 2.8  1996/01/22  05:28:27  lewis
 *	Mixin WordWrappedTextInteractor::DidUpdateText().
 *
 *	Revision 2.7  1995/12/09  05:44:30  lewis
 *	Partly for speed, but also more for working around MSVC 4.0 bugs with
 *	vtables during object creation, we use inline PeekAtTextStore_ instead
 *	of virtual PeekAtTextStore().
 *
 *	Revision 2.6  1995/11/25  00:30:36  lewis
 *	Added StandardStyledWordWrappedTextInteractor support for adding
 *	embeddings, like pictures etc. Support still primitive, and not well
 *	done.
 *
 *	Revision 2.5  1995/11/04  23:22:21  lewis
 *	No longer need overrides for StandardStyledWordWrappedTextInteractor::MeasureSegmentHeight ().
 *
 *	Revision 2.4  1995/11/02  22:41:18  lewis
 *	StandardStyledWordWrappedTextInteractor::DidUpdateText override
 *	for mixin.
 *	(styleDatabase) support.
 *
 *	Revision 2.3  1995/10/19  22:26:35  lewis
 *	New StandardStyledWordWrappedTextInteractor.
 *
 *	Revision 2.2  1995/10/09  22:47:29  lewis
 *	Fixed (I HOPE - otehrwise introduced) a bug in ScrollSoShowing
 *	wrapper function - negleected adnTryToShowMarkerPos parameter.
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.5  1995/06/08  05:16:40  lewis
 *	Code cleanups
 *
 *	Revision 1.4  1995/06/05  21:29:08  lewis
 *	Lose override of SelectWholeLineAfter - See SPR#0320.
 *
 *	Revision 1.3  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.2  1995/05/21  17:50:31  lewis
 *	God rid of qLotsOfSubclassesFromMixinPointYeildTemporarilyBadVTable
 *	 workaround.
 *	Must be done in subclasses which subclass from concrete TextStore.
 *
 *	Revision 1.1  1995/05/20  05:04:55  lewis
 *	Initial revision
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"TextInteractorMixins.h"







// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

