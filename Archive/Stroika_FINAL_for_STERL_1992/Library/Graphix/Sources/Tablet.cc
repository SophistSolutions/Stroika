/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Tablet.cc,v 1.7 1992/09/05 16:14:25 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Tablet.cc,v $
 *		Revision 1.7  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.6  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  19:46:10  sterling
 *		const osPixPat for some low-level quickdraw calls
 *
 *		Revision 1.4  1992/07/16  18:02:27  lewis
 *		Added new method for mac - Tablet::SetOSBackground (const Tile& backGround).
 *
 *		Revision 1.3  1992/07/16  05:19:29  lewis
 *		React to changes in Mac Tile code - no longer call IsOldQDPattern.
 *
 *		Revision 1.2  1992/06/25  04:23:50  sterling
 *		Change definition of DrawText () to move down by the ascent of the font where we draw.
 *		(Lewis doing checkin - ask sterl about this!!!)
 *		He also changed it for TextWidth () which I believe is pointless so I didnt merge that change.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.61  92/04/30  03:36:50  03:36:50  lewis (Lewis Pringle)
 *		Use new expanded Tile stuff instead of Stipple. Didnt quite work, so left in some
 *		anachronistic stipple usage for XOR mode. Not sure why???? XGDI.
 *		
 *		Revision 1.60  92/04/29  12:21:30  12:21:30  lewis (Lewis Pringle)
 *		In Tablet::BitBlit () if we notice that the sizes of the rects are wrong, then also
 *		do intermediate bitblit under X since it does not support stretching. Really this
 *		code needs lots of work - lots of cases still not handled properly - totally ad-hoc.
 *		
 *		Revision 1.59  92/04/27  18:40:05  18:40:05  lewis (Lewis Pringle)
 *		Apply clip when doing bit blits - oops I forgot on X!!! Also, need them for scrollbits.
 *		Documented bug with scrollbits where we must do erase of area left behind, but havent fixed it
 *		yet.
 *		
 *		Revision 1.58  92/04/20  14:15:40  14:15:40  lewis (Lewis Pringle)
 *		added typedef char* caddr_t conditional on qSnake && _POSIX_SOURCE, before include of Xutil.h, since that
 *		file references caddr_t which doesn't appear to be part of POSIX.
 *		
 *		Revision 1.56  92/04/14  12:53:38  12:53:38  lewis (Lewis Pringle)
 *		Put in asserts that origing for quickdraw setoring always zero, since we fixed code in MacOSControl, and Mac TE to not use
 *		that hack anymore. Later we will use quickdraw setorigin to implement proper
 *		tile alignments.
 *		
 *		Revision 1.55  92/04/08  15:56:06  15:56:06  lewis (Lewis Pringle)
 *		Fixed clipping under X, though I dont totally understand why it didnt work before. The only thing that seems to have been
 *		wrong is that we never set the visible region so by default it was empoty. This
 *		should have caused all graphix to be clipped out, but what confused me was that opposiute happened.
 *		Clipping was ignored.
 *		Anyway, now that I set the visible are right, and have re-enabled claling XSetRegion, it seems to work fine now.
 *		
 *		Revision 1.54  92/04/07  01:07:00  01:07:00  lewis (Lewis Pringle)
 *		Add support for XGDI keeping CLUT and visual info for target drawable. Then use that in
 *		BitBlit to do a depth/color table conversion/strechblit when necessary.
 *		
 *		Revision 1.53  92/04/02  11:32:58  11:32:58  lewis (Lewis Pringle)
 *		Made BitBlit a method of Tablet and got rid of Global BitBlit procesudes. Also, on mac version,
 *		got rid of saving / restoring of port - hope not really needed.
 *		Got rid of fOSDisplay, and sOSDisplay fields/ static members of tablet - use gDisplay instead.
 *		Got rid of GetClip/SetClip from TabletOwner - usually this will be done thru a seperate mechansim, and
 *		would encourage misuse if tabletowner made it too easy to set. If you must, you can still say
 *		GetTablet ()->SetClip (LocalToTablet (XXX)));
 *		Added more overloaded BitBlit routines and now they indirect to Tabeet instead of global functions.
 *		Was more carefuly about making things like Paint/DrawText etc virtual so in View we can override them
 *		and set the clip first.
 *		Added Tablet::GetDrawable accessor for XGDI.
 *		
 *		Revision 1.52  92/03/26  09:22:17  09:22:17  lewis (Lewis Pringle)
 *		Made TextWidht a const method of both Tablet, and TabletOwner.
 *		
 *		Revision 1.50  1992/03/22  10:22:27  lewis
 *		Got rid of arg to GetOSPixmap - use global gDisplay instead.
 *
 *		Revision 1.49  1992/03/16  19:27:18  lewis
 *		Put in implementation of Tablet::GetBounds().
 *
 *		Revision 1.48  1992/03/16  17:19:17  lewis
 *		Temporarily disable REAL grabs for X - since can cause system hangs.
 *
 *		Revision 1.45  1992/02/14  02:36:32  lewis
 *		Turned XSetRegion on again in ApplyClip. Still does not appear to work. Debug soon.
 *
 *		Revision 1.44  1992/02/12  06:10:01  lewis
 *		Code cleanups, convert from OSConfig to GDIConfig, use new mac compiler that changed defn of osPattern.
 *
 *		Revision 1.43  1992/02/11  22:02:04  lewis
 *		Start using fixes to Tile stuff, genral cleanups of X code, and more accurate
 *		conversion of bitblit modes. Document what to do about UseBrush () code for setting
 *		foreground/background pixel properly, but I must research where to find the appropate
 *		color info.
 *
 *		Revision 1.42  1992/02/11  01:50:47  lewis
 *		Add server grab code (not sure good idea to use), and fixed bug with
 *		Carls machine - trouble was ambiguity in X docs about FillStipped versus
 *		FillOpaqueStipped in DrawLines call (gc). Docs seem self contradictory, and
 *		seem to work differently between X Terminal from HP, and their HP server
 *		software that comes with the 730's.???? Must take a close look at all that
 *		graphix code. X overspecified - as near as I can tell - and thats the source
 *		of possible confusion.
 *
 *		Revision 1.41  1992/02/11  00:24:45  lewis
 *		Cleanups for mac - got rid of gworld stuff, and param to InitFromPixelMap.
 *		Got rid of ifdefs debug messages.
 *
 *		Revision 1.40  1992/01/31  05:36:07  lewis
 *		Disable clipping support temporilty til we get enuf other stuff debugged in dealing with framework draw
 *		mechanism so we can properly debug the clipping.
 *
 *		Revision 1.39  1992/01/24  23:42:05  lewis
 *		Remove invalid assertions in X scrollbits.
 *
 *		Revision 1.38  1992/01/22  08:04:06  lewis
 *		Added Sync () method (mostly for X, but can be used on mac).
 *
 *		Revision 1.37  1992/01/22  04:16:20  lewis
 *		Add static member to access the display
 *
 *		Revision 1.35  1992/01/20  12:23:13  lewis
 *		Did (untested) implementation of ScrollBits for X (half assed - too).
 *
 *		Revision 1.33  1992/01/19  22:11:35  lewis
 *		Subtract 1 from width and hight on drawrect to be compat with mac graphix model we inherit.
 *
 *		Revision 1.32  1992/01/18  09:04:54  lewis
 *		Dont assertOut in Scrollbits, and fix bug with DoneWithBrush to DoneWithFont () and call XTextWidth () with
 *		font struct from font.
 *
 *		Revision 1.28  1992/01/16  00:48:47  lewis
 *		Got to compile on mac.
 *
 *		Revision 1.26  1992/01/14  18:41:26  lewis
 *		Added clipping, visible area, and font support for X.
 *
 *		Revision 1.25  1992/01/14  06:00:35  lewis
 *		Added Font support in DrawText - need to do TextWidth () still.  Also, added
 *		class TabletOwner.
 *
 *		Revision 1.24  1992/01/10  08:55:40  lewis
 *		Changed hack for eNotBicTMode in UseBrush () to xor, so we see something reasonable 'til we
 *		really get that stuff working.
 *
 *		Revision 1.23  1992/01/10  03:33:33  lewis
 *		Added support to turn on Sync mode in X, and played with the logical graphix mode translation.
 *
 *		Revision 1.22  1992/01/09  00:01:14  lewis
 *		Changed Outline and PaintRegion to share more code with UNIX/Mac implementaions, and to work
 *		around lack of X region support better with frameregion - instead just call outlineRect().
 *		Also, moved what was a correct implementation of PaintRegion from outlineregion (where it was really
 *		wrong).
 *
 *		Revision 1.21  1992/01/07  01:48:04  lewis
 *		Move ClipRect call to after setport - whole call should probably be deleted but
 *		I haven't the time to look things up...
 *
 *		Revision 1.20  1992/01/03  21:35:44  lewis
 *		Make Tablet::OutlineRegion () more tolerent of failure to exactly set the bounds of a region.
 *		under XGDI.
 *
 *
 */

#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<QDOffscreen.h>
#include	<QuickDraw.h>
#include	<Memory.h>
#include	<OSUtils.h>				// for trap caching
#include	<SysEqu.h>
#include	<Traps.h>				// for trap caching
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#endif	/*GDI*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"BitBlitter.hh"
#include	"GDIConfiguration.hh"
#include	"Picture.hh"
#include	"Region.hh"
#include	"Shape.hh"

#include	"Tablet.hh"


#if		qMacGDI
#if		qCacheTraps

pascal	void	(*kDrawTextPtr) (osPtr,short,short) =
			(pascal void (*) (osPtr,short,short))::NGetTrapAddress (_DrawText, ToolTrap);
pascal	short	(*kTextWidthPtr) (osPtr,short,short) =
			(pascal short (*) (osPtr,short,short))::NGetTrapAddress (_TextWidth, ToolTrap);
pascal	void	(*kMoveToPtr) (short, short) =
			(pascal void (*) (short, short))::NGetTrapAddress (_MoveTo, ToolTrap);
pascal	void	(*kLineToPtr) (short, short) =
			(pascal void (*) (short, short))::NGetTrapAddress (_LineTo, ToolTrap);
pascal	void	(*kPenModePtr) (short) =
			(pascal void (*) (short))::NGetTrapAddress (_PenMode, ToolTrap);
pascal	void	(*kSetClipPtr) (osRegion**) =
			(pascal void (*) (osRegion**))::NGetTrapAddress (_SetClip, ToolTrap);
pascal	void	(*kSetPortPtr) (osGrafPort*) =
			(pascal void (*) (osGrafPort*))::NGetTrapAddress (_SetPort, ToolTrap);
pascal	void	(*kSetOriginPtr) (short,short) =
			(pascal void (*) (short,short))::NGetTrapAddress (_SetOrigin, ToolTrap);
pascal	void	(*kFrameRgnPtr) (osRegion**) =
			(pascal void (*) (osRegion**))::NGetTrapAddress (_FrameRgn, ToolTrap);
pascal	void	(*kPaintRgnPtr) (osRegion**) =
			(pascal void (*) (osRegion**))::NGetTrapAddress (_PaintRgn, ToolTrap);

#endif	/*qCacheTraps*/





struct osGrafPort*	Tablet::GetCurrentPort ()
{
#if		qDebug
	osGrafPort*	fred;	
	::GetPort (&fred);
	Assert (fred == qd.thePort);
#endif
	return (qd.thePort);
}

void	Tablet::xDoSetPort (osGrafPort* newPort)
{
	RequireNotNil (newPort);
	if (qd.thePort != newPort) {
#if		qCacheTraps
		RequireNotNil (kSetPortPtr);
		(*kSetPortPtr) (newPort);
#else	/*qCacheTraps*/
		::SetPort (newPort);
#endif	/*qCacheTraps*/
	}
}

inline	void	DoSetOrigin (const Point& origin)
{
Assert (origin == kZeroPoint);		// later we will lift this when we implemnt tile alignment, but for now this
									// should be true..

	RequireNotNil (qd.thePort);
	osPoint	osp;
	os_cvt (-origin, osp);
	if ((qd.thePort->portRect.top != osp.v) or (qd.thePort->portRect.left != osp.h)) {
#if		qCacheTraps
		RequireNotNil (kSetOriginPtr);
		(*kSetOriginPtr) (osp.h, osp.v);
#else	/*qCacheTraps*/
		::SetOrigin (osp.h, osp.v);
#endif	/*qCacheTraps*/
	}
}

inline	void	DoFrameRgn (const osRegion** r)
{
	RequireNotNil (r);
#if		qCacheTraps
	(*kFrameRgnPtr) ((osRegion**)r);
#else	/*qCacheTraps*/
	::FrameRgn ((osRegion**)r);
#endif	/*qCacheTraps*/
}

inline	void	DoPaintRgn (const osRegion** r)
{
	RequireNotNil (r);
#if		qCacheTraps
	(*kPaintRgnPtr) ((osRegion**)r);
#else	/*qCacheTraps*/
	::PaintRgn ((osRegion**)r);
#endif	/*qCacheTraps*/
}

inline	void	DoSetClip (const osRegion** r)
{
	RequireNotNil (r);
#if		qCacheTraps
	(*kSetClipPtr) ((osRegion**)r);
#else	/*qCacheTraps*/
	::SetClip ((osRegion**)r);
#endif	/*qCacheTraps*/
}

#endif	/*qMacGDI*/










#if		qXGDI
static	void	FillInVisualAndCLUT (long drawable, osVisual* visual, ColorLookupTable* clut);
#endif




/*
 ********************************************************************************
 ************************************* Tablet ***********************************
 ********************************************************************************
 */

Tablet::Tablet (PixelMap* pixelMap):
#if		qXGDI
	fClipRegion (kEmptyRegion),
	fVisibleArea (kEmptyRegion),
	fDrawable (Nil),
	fVisual (Nil),
	fColorLookupTable (),
#endif
	fEffectiveClipRegion (kEmptyRegion),
	fEffectiveClipValid (False),
	fTileAlignment (kZeroPoint)
#if		qMacGDI
	,fOSGrafPtr (Nil),
	fWeDestroy (False),
	fPixelMap (pixelMap),
	fDestroyPixelMap (False)
#endif	/*qMacGDI*/
{
	InitFromPixelMap ();
}

Tablet::Tablet (const Point& size, UInt8 depth):
#if		qXGDI
	fClipRegion (kEmptyRegion),
	fVisibleArea (kEmptyRegion),
	fDrawable (Nil),
	fVisual (Nil),
	fColorLookupTable (),
#endif
	fEffectiveClipRegion (kEmptyRegion),
	fEffectiveClipValid (False),
	fTileAlignment (kZeroPoint)
#if		qMacGDI
	,fOSGrafPtr (Nil),
	fWeDestroy (False),
	fPixelMap (Nil),
	fDestroyPixelMap (False)
#endif	/*qMacGDI*/
{
#if		qMacGDI
	fPixelMap = new PixelMap (PixelMap::eChunky, depth, Rect (kZeroPoint, size));
	fDestroyPixelMap = True;
	InitFromPixelMap ();
#endif	/*qMacGDI*/
}

#if		qMacGDI
Tablet::Tablet (struct osGrafPort* osGrafPtr):
	fEffectiveClipRegion (kEmptyRegion),
	fEffectiveClipValid (False),
	fOSGrafPtr (osGrafPtr),
	fTileAlignment (kZeroPoint),
	fWeDestroy (False),
	fPixelMap (Nil),
	fDestroyPixelMap (False)
{
	RequireNotNil (osGrafPtr);
}
#elif	qXGDI
Tablet::Tablet (unsigned long drawable):
	fClipRegion (kEmptyRegion),
	fVisibleArea (kEmptyRegion),
	fDrawable (Nil),
	fVisual (Nil),
	fColorLookupTable (),
	fEffectiveClipRegion (kEmptyRegion),
	fEffectiveClipValid (False),
	fTileAlignment (kZeroPoint)
{
	RequireNotNil (drawable);
	fDrawable = drawable;
	fVisual = new osVisual ();
	FillInVisualAndCLUT (drawable, (osVisual*)fVisual, &fColorLookupTable);
}
#endif	/*qMacGDI*/

Tablet::~Tablet ()
{
#if		qMacGDI
	if (fWeDestroy and (fOSGrafPtr != Nil)) {
		if (GDIConfiguration ().ColorQDAvailable ()) {
				// CloseCPort disposes of the pixmap, and so we must put in a bogus one, for it do destroy
			::SetPort (fOSGrafPtr);
			::SetPortPix (::NewPixMap ());
			::CloseCPort ((osCGrafPort*)fOSGrafPtr);
		}
		else {
			if (fOSGrafPtr!= Nil) {
				::ClosePort (fOSGrafPtr);
			}
		}
		::SetPort (*(osGrafPort**)WMgrPort);
		Assert (qd.thePort != fOSGrafPtr);
		delete (fOSGrafPtr);		// depends on how we create it.
									// Do Close(C)Port first!!!
	}
	else {
		::SetPort (*(osGrafPort**)WMgrPort);
		Assert (qd.thePort != fOSGrafPtr);
	}
	if (fDestroyPixelMap) {
		AssertNotNil (fPixelMap);
		delete fPixelMap;
	}
	Ensure (qd.thePort != fOSGrafPtr);
#endif	/*qMacGDI*/
}

void	Tablet::InitFromPixelMap ()
{
	RequireNotNil (fPixelMap);
#if		qMacGDI
	Assert (not fWeDestroy);
	fWeDestroy = True;
	if (GDIConfiguration ().ColorQDAvailable ()) {
		fOSGrafPtr	=	(osGrafPort*) new osCGrafPort;
		::OpenCPort ((osCGrafPort*)fOSGrafPtr);
		Assert (fOSGrafPtr == qd.thePort);
		// OpenCPort creates a new pixmap, and since we are replacing it, we must dispose of the
		// existing one..
		osPixMap**	oldPixMap	=	((osCGrafPort*)fOSGrafPtr)->portPixMap;
		::SetPortPix (fPixelMap->GetNewOSRepresentation ());
		(*oldPixMap)->pmTable = Nil;	// to disable deletion??? Not really sure how to do this
										// we blew away whatever newpixmap created!
		::DisposePixMap (oldPixMap);
		Assert (((osCGrafPort*)qd.thePort)->portVersion & 0xC000);
		if (GDIConfiguration ().ColorQD32Available ()) {
			::PortChanged (fOSGrafPtr);
		}
	}
	else {
		fOSGrafPtr	=	new osGrafPort;
		::OpenPort (fOSGrafPtr);
		Assert (fOSGrafPtr == qd.thePort);
		::SetPortBits (fPixelMap->GetOldOSRepresentation ());
	}

	/*
	 * Looked at all fields of a grafport/osCGrafPort, and it seemed that was all we needed
	 * to update?
	 */
	os_cvt (fPixelMap->GetBounds (), fOSGrafPtr->portRect);
	::RectRgn (fOSGrafPtr->visRgn, &fOSGrafPtr->portRect);
	Assert (fOSGrafPtr == qd.thePort);
	::ClipRect (&fOSGrafPtr->portRect);
#endif	/*qMacGDI*/
}

Region	Tablet::GetClip () const
{
#if		qMacGDI
	return (Region (GetOSGrafPtr ()->clipRgn));
#elif	qXGDI
	return (fClipRegion);
#endif	/*GDI*/
}

void	Tablet::SetClip (const Region& newClip)
{
#if		qMacGDI
	SetupOSRepFromClip (newClip, kZeroPoint);
	fEffectiveClipValid = False;
#elif	qXGDI
	// equality test is much cheaper than intersection we must do later, and often we will find
	// simple equality, and can therefore avoid this recomputation...
	if (fClipRegion != newClip) {
		fClipRegion = newClip;
		fEffectiveClipValid = False;
	}
#endif	/*GDI*/
}

Region	Tablet::GetVisibleArea () const
{
#if		qMacGDI
	return (GetOSGrafPtr ()->visRgn);
#elif	qXGDI
	return (fVisibleArea);
#endif	/*qMacGDI*/
}

void	Tablet::SetVisibleArea (const Region& newVisibleArea)
{
#if		qMacGDI
	AssertNotImplemented ();	// not too sure this is a good idea...
#elif	qXGDI
	fVisibleArea = newVisibleArea;
#endif
	fEffectiveClipValid = False;
}

Region	Tablet::GetEffectiveClipRegion () const
{
	if (not fEffectiveClipValid) {
		((Tablet*)this)->fEffectiveClipRegion = GetVisibleArea () * GetClip ();
		((Tablet*)this)->fEffectiveClipValid = True;		// override const cuz were caching values...
	}
	return (fEffectiveClipRegion);
}

void	Tablet::OutLine (const Shape& s, const Rect& shapeBounds, const Pen& pen)
{
	s.OutLine (*this, shapeBounds, pen);
}

void	Tablet::Paint (const Shape& s, const Rect& shapeBounds, const Brush& brush)
{
	s.Paint (*this, shapeBounds, brush);
}

void	Tablet::DrawText (const String& text, const Font& font, const Point& at)
{
	short	ascent = (short)font.GetAscent ();
	
#if		qMacGDI
	SetupOSRepFromFont (eOrTMode, font, kZeroPoint);
#if		qCacheTraps
	(*kMoveToPtr) ((short)at.GetH (), (short)at.GetV () + ascent);
#if qMPW_COMPLEX_ARGS_TO_DIRECT_FUN_BUG
	short	length = (short)text.GetLength ();
	(*kDrawTextPtr) (osPtr (text.Peek ()), 0, length);
#else
	(*kDrawTextPtr) (osPtr (text.Peek ()), 0, (short)text.GetLength ());
#endif
#else	/*qCacheTraps*/
	::MoveTo ((short)at.GetH (), (short)at.GetV () + ascent);
#if qMPW_COMPLEX_ARGS_TO_DIRECT_FUN_BUG
	short	length = (short)text.GetLength ();
	::DrawText (osPtr (text.Peek ()), 0, length);
#else
	::DrawText (osPtr (text.Peek ()), 0, (short)text.GetLength ());
#endif
#endif	/*qCacheTraps*/
#elif	qXGDI
	extern	osDisplay*	gDisplay;
	GC gc = UseFont (font);
	::XDrawString (gDisplay, fDrawable, gc, (int)at.GetH (), (int)at.GetV () + ascent,
				   (const char*)text.Peek (), text.GetLength ());
	DoneWithFont (gc);
#endif	/*GDI*/
}

Coordinate	Tablet::TextWidth (const String& text, const Font& font, const Point& at) const
{
#if		qMacGDI
	SetupOSRepFromFont (eOrTMode, font, kZeroPoint);
	// in principle (and maybe in fact) width depends on where we are in the tablet, since that
	// decides which graphix device we are writing on...
#if		qCacheTraps
	(*kMoveToPtr) ((short)at.GetH (), (short)at.GetV ());
#if qMPW_COMPLEX_ARGS_TO_DIRECT_FUN_BUG
	short	length = (short)text.GetLength ();
	return ((*kTextWidthPtr) (osPtr (text.Peek ()), 0, length));
#else
	return ((*kTextWidthPtr) (osPtr (text.Peek ()), 0, (short)text.GetLength ()));
#endif
#else	/*qCacheTraps*/
	::MoveTo ((short)at.GetH (), (short)at.GetV ());
#if qMPW_COMPLEX_ARGS_TO_DIRECT_FUN_BUG
	short	length = (short)text.GetLength ();
	return (::TextWidth (osPtr (text.Peek ()), 0, length));
#else
	return (::TextWidth (osPtr (text.Peek ()), 0, Int16 (text.GetLength ())));
#endif

#endif	/*qCacheTraps*/
#elif	qXGDI

	return (::XTextWidth ((XFontStruct*)font.GetXFontStruct (), (const char*)text.Peek (), text.GetLength ()));
#endif	/*GDI*/
}

void	Tablet::SetTileAlignment (const Point& tileAlignment)
{
	fTileAlignment = tileAlignment;
}

void*	Tablet::MakeGraphixDeviceIterator ()
{
	AssertNotReached ();		// NYI
	return (Nil);
}

UInt8	Tablet::GetMaxDeviceDepth ()
{
	AssertNotReached ();		// NYI
	return (1);
}

Boolean	Tablet::DeviceSupportsColor ()
{
	AssertNotReached ();		// NYI
	return (True);
}

void	Tablet::OutLineRect (const Rect& r, const Pen& pen)
{
#if		qMacGDI
	osRect	osr;
	os_cvt (r, osr);
	SetupOSRepFromPen (pen, kZeroPoint);
	::FrameRect (&osr);
#elif	qXGDI
	/*
	 * Subtract 1 from the width and height since our graphix model mimics the mac where points refer to
	 * the lines between the pixels, and under X, it refers to the pixels themselves.
	 *
	 * Then, be sure we dont make a zero go huge positive after subtract, and at the same time prevent large
	 * rects that X server might not like.
	 */
	unsigned int	width	=	r.GetWidth () - 1;
	unsigned int	height	=	r.GetHeight () - 1;
	if ((width < 32000) and (height < 32000)) {
		extern	osDisplay*	gDisplay;
		GC gc = UsePen (pen);
		::XDrawRectangle (gDisplay, fDrawable, gc, (int)r.GetLeft (), (int)r.GetTop (), width, height);
		DoneWithPen (gc);
	}
#endif	/*GDI*/
}

void	Tablet::PaintRect (const Rect& r, const Brush& brush)
{
#if		qMacGDI
	osRect	osr;
	os_cvt (r, osr);
	SetupOSRepFromBrush (brush, kZeroPoint);
	::PaintRect (&osr);
#elif	qXGDI
	extern	osDisplay*	gDisplay;
	GC gc = UseBrush (brush);
	::XFillRectangle (gDisplay, fDrawable, gc, (int)r.GetLeft (), (int)r.GetTop (),
					  (unsigned int)r.GetWidth (), (unsigned int)r.GetHeight ());
	DoneWithBrush (gc);
#endif	/*GDI*/
}

void	Tablet::OutLineRoundedRect (const Rect& r, const Point& rounding, const Pen& pen)
{
#if		qMacGDI
	Require (rounding.GetH () == (short)rounding.GetH ());
	Require (rounding.GetV () == (short)rounding.GetV ());

	osRect	osr;
	os_cvt (r, osr);
	SetupOSRepFromPen (pen, kZeroPoint);
	::FrameRoundRect (&osr, (short)rounding.GetH (), (short)rounding.GetV ());
#elif	qXGDI
	// temp hack....
  	OutLineRect (r, pen);
#endif	/*qMacGDI*/
}

void	Tablet::PaintRoundedRect (const Rect& r, const Point& rounding, const Brush& brush)
{
#if		qMacGDI
	Require (rounding.GetH () == (short)rounding.GetH ());
	Require (rounding.GetV () == (short)rounding.GetV ());

	osRect	osr;
	os_cvt (r, osr);
	SetupOSRepFromBrush (brush, kZeroPoint);
	::PaintRoundRect (&osr, (short)rounding.GetH (), (short)rounding.GetV ());
#elif	qXGDI
	// temp hack
  	PaintRect (r, brush);
#endif	/*qMacGDI*/
}

void	Tablet::OutLineOval (const Rect& r, const Pen& pen)
{
#if		qMacGDI
	osRect	osr;
	os_cvt (r, osr);
	SetupOSRepFromPen (pen, kZeroPoint);
	::FrameOval (&osr);
#elif	qXGDI
	extern	osDisplay*	gDisplay;
	GC gc = UsePen (pen);
	::XDrawArc (gDisplay, fDrawable, gc, (int)r.GetLeft (), (int)r.GetTop (),
				(unsigned int)r.GetWidth (), (unsigned int)r.GetHeight (), 0, 360*64);
	DoneWithPen (gc);
#endif	/*qMacGDI || qXGDI*/
}

void	Tablet::PaintOval (const Rect& r, const Brush& brush)
{
#if		qMacGDI
	osRect	osr;
	os_cvt (r, osr);
	SetupOSRepFromBrush (brush, kZeroPoint);
	::PaintOval (&osr);
#elif	qXGDI
	extern	osDisplay*	gDisplay;
	GC gc = UseBrush (brush);
	::XFillArc (gDisplay, fDrawable, gc, (int)r.GetLeft (), (int)r.GetTop (),
				(unsigned int)r.GetWidth (), (unsigned int)r.GetHeight (), 0, 360*64);
	DoneWithPen (gc);
#endif	/*qMacGDI || qXGDI*/
}

void	Tablet::OutLineArc (Angle startAngle, Angle arcAngle, const Rect& r, const Pen& pen)
{
#if		qMacGDI
	osRect	osr;
	os_cvt (r, osr);
	SetupOSRepFromPen (pen, kZeroPoint);
	// See IM I-180 and Shape.hh (class Arc)
	::FrameArc (&osr, 90-short (startAngle.AsDegrees ()), -short (arcAngle.AsDegrees ()));
#elif	qXGDI
	extern	osDisplay*	gDisplay;
	GC gc = UsePen (pen);
	::XDrawArc (gDisplay, fDrawable, gc, (int)r.GetLeft (), (int)r.GetTop (),
				(unsigned int)r.GetWidth (), (unsigned int)r.GetHeight (), startAngle.AsDegrees ()*64, arcAngle.AsDegrees ()*64);
	DoneWithPen (gc);
#endif	/*qMacGDI || qXGDI*/
}

void	Tablet::PaintArc (Angle startAngle, Angle arcAngle, const Rect& r, const Brush& brush)
{
#if		qMacGDI
	osRect	osr;
	os_cvt (r, osr);
	SetupOSRepFromBrush (brush, kZeroPoint);
	// See IM I-180 and Shape.hh (class Arc)
	::PaintArc (&osr, 90-short (startAngle.AsDegrees ()), -short (arcAngle.AsDegrees ()));
#elif	qXGDI
	extern	osDisplay*	gDisplay;
	GC gc = UseBrush (brush);
	::XFillArc (gDisplay, fDrawable, gc, (int)r.GetLeft (), (int)r.GetTop (),
				(unsigned int)r.GetWidth (), (unsigned int)r.GetHeight (), startAngle.AsDegrees ()*64, arcAngle.AsDegrees ()*64);
	DoneWithBrush (gc);
#endif	/*qMacGDI*/
}

Region	Tablet::GetBounds () const
{
#if		qMacGDI
	return (GetOSGrafPtr ()->visRgn);			// not sure this is the right answer - what if part of it are obscured...
												// not too sure of easy way to get the asnwer...
#elif	qXGDI
	extern	osDisplay*	gDisplay;
	osWindowAttributes	xwa;
	memset (&xwa, 0, sizeof (xwa));
	Status s = ::XGetWindowAttributes (gDisplay, fDrawable, &xwa);
	Assert (s != 0);		// should never happen (Orielly -II - 255)...
	// Not too sure about the origin - what is this used for anyway????
	return (Rect (kZeroPoint, Point (xwa.height, xwa.width)));
	
#endif	/*qMacGDI*/
}

#if		qXGDI
// Stolen from X11R4/mit/lib/X/region.h Must be some better way to do this, but what???
typedef struct {
    short x1, x2, y1, y2;
} Box, BOX, BoxRec, *BoxPtr;
typedef struct {
    short x, y, width, height;
}RECTANGLE, RectangleRec, *RectanglePtr;

typedef struct osRegion /*_XRegion*/ {
    long size;
    long numRects;
    BOX *rects;
    BOX extents;
} REGION;
#endif	/*qXGDI*/

void	Tablet::OutLineRegion (const Region& region, const Rect& r, const Pen& pen)
{
	if (not r.Empty ()) {
		Rect	bounds 	= region.GetBounds ();
		if (r == bounds) {
#if		qMacGDI
			SetupOSRepFromPen (pen, kZeroPoint);
			DoFrameRgn (region.PeekAtOSRegion ());
#elif	qXGDI
  			OutLineRect (r, pen);
#endif	/*GDI*/
		}
		else {
			Region	tmpRgn = region;
			tmpRgn.SetBounds (r);
  			/*
			 * note that we pass along tmpRgn.GetBounds () instead of r, in case the differ so we
			 * don't recurse forever - they should never differ, but occasionally they do - at least
			 * under X.
			 */
			OutLineRegion (tmpRgn, tmpRgn.GetBounds (), pen);
		}
	}
}

void	Tablet::PaintRegion (const Region& region, const Rect& r, const Brush& brush)
{
	if (not r.Empty ()) {
		Rect	bounds 	= region.GetBounds ();
		if (r == bounds) {
#if		qMacGDI
			SetupOSRepFromBrush (brush, kZeroPoint);
			DoPaintRgn (region.PeekAtOSRegion ());
#elif	qXGDI
			GC gc = UseBrush (brush);
			{
				// code stolen and warmed over from X11R4 region implemention of ::XSetRegion()
				register	osRegion*	theRegion	=	region.PeekAtOSRegion ();
				register int i;
				register osRect *xr, *pr;
				register BOX *pb;
				xr = new osRect [theRegion->numRects];
				for (pr = xr, pb = theRegion->rects, i = theRegion->numRects; --i >= 0; pr++, pb++) {
					pr->x = pb->x1;
					pr->y = pb->y1;
					pr->width = pb->x2 - pb->x1;
					pr->height = pb->y2 - pb->y1;
				}
				extern	osDisplay*	gDisplay;
				::XFillRectangles (gDisplay, fDrawable, gc, xr, theRegion->numRects);
				delete (xr);
			}
			DoneWithBrush (gc);
#endif	/*GDI*/
		}
		else {
			Region	tmpRgn = region;
			tmpRgn.SetBounds (r);
  			/*
			 * note that we pass along tmpRgn.GetBounds () instead of r, in case the differ so we
			 * don't recurse forever - they should never differ, but occasionally they do - at least
			 * under X.
			 */
			PaintRegion (tmpRgn, tmpRgn.GetBounds (), brush);
		}
	}
}

#if		qMacGDI
void	Tablet::OutLinePolygon (struct osPolygon** polygon, const Rect& r, const Pen& pen)
{
	RequireNotNil (polygon);
	Rect	logicalBounds = os_cvt ((*polygon)->polyBBox);

	SetupOSRepFromPen (pen, kZeroPoint);
	if (r != logicalBounds) {
		osRect	osr;
		osPolygon**	tempPolygon = (osPolygon**) ::NewHandle (::GetHandleSize ((osHandle) polygon));
		
		os_cvt (r, osr);

		memcpy (*tempPolygon, *(osHandle)polygon, (size_t)::GetHandleSize ((osHandle) polygon));

		::MapPoly (tempPolygon, &(*polygon)->polyBBox, &osr);
		::FramePoly (tempPolygon);
		::KillPoly (tempPolygon);
	}
	else {
		::FramePoly (polygon);
	}
}

void	Tablet::PaintPolygon (struct osPolygon** polygon, const Rect& r, const Brush& brush)
{
	RequireNotNil (polygon);
	Rect	logicalBounds = os_cvt ((*polygon)->polyBBox);
	SetupOSRepFromBrush (brush, kZeroPoint);
	if (r != logicalBounds) {
		osPolygon**	tempPolygon = (osPolygon**) ::NewHandle (::GetHandleSize ((osHandle) polygon));
		memcpy ((osPtr) *tempPolygon, *(osHandle)polygon, (size_t)::GetHandleSize ((osHandle) polygon));

		osRect	osr;
		os_cvt (r, osr);
		::MapPoly (tempPolygon, &(*polygon)->polyBBox, &osr);
		::PaintPoly (tempPolygon);
		::KillPoly (tempPolygon);
	}
	else {
		::PaintPoly (polygon);
	}
}
#endif	/*qMacGDI*/

void	Tablet::DrawLine (const Point& from, const Point& to, const Pen& pen)
{
	Require (from.GetH () == (short)from.GetH ());
	Require (from.GetV () == (short)from.GetV ());
	Require (to.GetH () == (short)to.GetH ());
	Require (to.GetV () == (short)to.GetV ());
#if		qMacGDI
	SetupOSRepFromPen (pen, kZeroPoint);
#if		qCacheTraps
	(*kMoveToPtr) ((short)from.GetH (), (short)from.GetV ());
	(*kLineToPtr) ((short)to.GetH (), (short)to.GetV ());
#else	/*qCacheTraps*/
	::MoveTo ((short)from.GetH (), (short)from.GetV ());
	::LineTo ((short)to.GetH (), (short)to.GetV ());
#endif	/*qCacheTraps*/
#elif	qXGDI
	extern	osDisplay*	gDisplay;
	GC gc = UsePen (pen);
	::XDrawLine (gDisplay, fDrawable, gc, (short)from.GetH (), (short)from.GetV (), (short)to.GetH (), (short)to.GetV ());
	DoneWithPen (gc);
#endif	/*qMacGDI*/
}

Color	Tablet::GetColor (const Point& p) const
{
#if		qMacGDI
	Assert ((short)p.GetH () == p.GetH ());
	Assert ((short)p.GetV () == p.GetV ());
	static	Boolean	kColorQDAvailable	=	GDIConfiguration ().ColorQDAvailable ();
	if (kColorQDAvailable) {
		RGBColor	rgbColor;
		Color		c	=	kBlackColor;
		::GetCPixel ((short)p.GetH (), (short)p.GetV (), &rgbColor);
		c.SetRGB (rgbColor.red, rgbColor.green, rgbColor.blue);
		return (c);
	}
	else {
		return (::GetPixel ((short)p.GetH (), (short)p.GetV ())? kBlackColor: kWhiteColor);
		
	}
#elif	qXGDI
	AssertNotImplemented ();
#endif	/*qMacGDI*/
}

void	Tablet::SetColor (const Point& p, const Color& c)
{
#if		qMacGDI
	Assert ((short)p.GetH () == p.GetH ());
	Assert ((short)p.GetV () == p.GetV ());
	static	Boolean	kColorQDAvailable	=	GDIConfiguration ().ColorQDAvailable ();

	if (kColorQDAvailable) {
		RGBColor	rgbColor;
		c.GetRGB (rgbColor.red, rgbColor.green, rgbColor.blue);
		::SetCPixel ((short)p.GetH (), (short)p.GetV (), &rgbColor);
	}
	else {
		AssertNotReached ();		// QD does not implement this - so we must
									// ourselves - pretty easy since we are
									// gaurenteed to only have 1 pixmap (and a bitmap at that).
	}
#elif	qXGDI
	AssertNotImplemented ();
#endif	/*qMacGDI*/
}

void	Tablet::DrawPicture (const Picture& picture, const Rect& intoRect)
{
#if		qMacGDI
	osRect		osr;
	os_cvt (intoRect, osr);
	DoSetPort ();
	DoSetOrigin (kZeroPoint);					// need to handle this better
	::DrawPicture ((osPicture**)picture.PeekOSRepresentation (), &osr);
#elif	qXGDI
	AssertNotImplemented ();
#endif	/*qMacGDI*/
}

void	Tablet::ScrollBits (const Rect& r, const Point& delta, const Tile& background)
{
#if		qMacGDI
	static	Boolean	kColorQDAvailable	=	GDIConfiguration ().ColorQDAvailable ();

	DoSetPort ();
	DoSetOrigin (kZeroPoint);					// need to handle this better

	if (kColorQDAvailable) {
		// ~const on osPixPat
		::BackPixPat ((osPixPat**) background.GetNewQDPattern ());
	}
	else {
		::BackPat (background.GetOldQDPattern ());
	}

	Region	updateRgn;
	osRect	osr;
	osPoint	osp;
	os_cvt (delta, osp);
	os_cvt (r, osr);
	::ScrollRect (&osr, osp.h, osp.v, updateRgn.GetOSRegion ());

	// Since ScrollRect doesnt seem to be updating the updateregion with the new tile, we
	// fake it here... Is this a bug with colorQD 2.0?
	PaintRegion (updateRgn, updateRgn.GetBounds (), Brush (background, eCopyTMode));

	::BackPat (&qd.white);		// should save old - if I knew how...
								// actually probably easy...
#elif	qXGDI
	extern	osDisplay*	gDisplay;
	GC gc;
	XGCValues		gcv;
	gcv.subwindow_mode = IncludeInferiors;
	if (! (gc = ::XCreateGC (gDisplay, fDrawable, GCSubwindowMode, &gcv))) {
		AssertNotReached (); // must raise exception!!
	}

	ApplyCurrentClip (gc);
// IGNORE USE SPECIFIED SIZES - WE DONT SUPPORET SCALING YET!!!
	::XCopyArea (gDisplay, fDrawable, fDrawable, gc,
				(int)r.GetLeft (), (int)r.GetTop (),
				(unsigned int)r.GetWidth (), (unsigned int)r.GetHeight (),
				(int)r.GetLeft () + (int)delta.GetH (), (int)r.GetTop () + (int)delta.GetV ());


// should probably erase exposed area - like we do with paintregion call above with quickdraw...
//	::XClearArea (gDisplay, fDrawable, fDrawable, gc, 

	::XFreeGC (gDisplay, gc);


#endif	/*qMacGDI*/
}

void	Tablet::BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode)
{
#if		qMacGDI
	osRect	osFromRect;
	osRect	osToRect;
	os_cvt (fromRect, osFromRect);
	os_cvt (toRect, osToRect);

	DoSetPort ();
	DoSetOrigin (kZeroPoint);					// need to handle this better
	if (GDIConfiguration ().ColorQDAvailable ()) {
		osPixMap*	fromPMPtr	=	*from.GetNewOSRepresentation ();
		::CopyBits ((osBitMap*)fromPMPtr, &GetOSGrafPtr ()->portBits, &osFromRect, &osToRect, tMode, Nil);
	}
	else {
// may need to do something similar to XGDI in non-colorqd case for mac - to do depth conversion...
		::CopyBits (from.GetOldOSRepresentation (), &GetOSGrafPtr ()->portBits,	&osFromRect, &osToRect, tMode, Nil);
	}
#elif	qXGDI

	/*
	 * X provides only the skimpiest of BitBlitting support. So our approach here is to decide if X supports our
	 * request, and if so, then use X, and otherwise to use our BitBlitter to format the Pixelmap in a way that
	 * X will be able to support, and make a second call (recursive) to this method.
	 *
	 * Generally, X should be able to support our request iff depth, and color tables match. Otherwise, we must
	 * transform ourselves to something that does match. This is because X only gives us two relevant tools - XCopyArea,
	 * and XCopyPlane. XCopyPlane is the closest to what we want in that it lets us do SOME depth-conversion/color
	 * conversion, but it does it SO badly, we cannot use it. That leaves us the the XCopyArea () entry which
	 * requires depth/color tables to already be pre-matched (actaully X ignores the src color table, but we cannot
	 * since we want the result to look right).
	 */
	AssertNotNil (fVisual);

// not quite right...
	PixelMap::PixelStyle	ourStyle=  PixelMap::eChunky;
	switch (((osVisual*)fVisual)->c_class) {
		case	StaticGray:		ourStyle = PixelMap::eChunky; break;
		case	GrayScale:		ourStyle = PixelMap::eChunky; break;
		case	StaticColor:	ourStyle = PixelMap::eChunky; break;
		case	PseudoColor:	ourStyle = PixelMap::eChunky; break;
		case	TrueColor:		ourStyle = PixelMap::eDirect; break;
		case	DirectColor:	ourStyle = PixelMap::eDirect; break;
		default: SwitchFallThru (); break;
	}

	if ((from.GetDepth () != ((osVisual*)fVisual)->bits_per_rgb) or (ourStyle != from.GetPixelStyle ()) or
// dont bother with this test for now - causes us to run out of memory???? April 6, 1992
// try size test again!!!
		(fromRect.GetSize () != toRect.GetSize ()) or
		((ourStyle == PixelMap::eChunky) and (fColorLookupTable != from.GetColorLookupTable ()))) {
// I hacked this up - not right now - nor before - look more calosely...

		PixelMap	tmp = 	PixelMap (ourStyle, ((osVisual*)fVisual)->bits_per_rgb, Rect (kZeroPoint, toRect.GetSize ()),
									  Nil, fColorLookupTable);

		BitBlitter ().Apply (from, tmp, BitBlitter::kCopy);
		BitBlit (tmp, Rect (kZeroPoint, toRect.GetSize ()), toRect, tMode);
	}
	else {
		extern	osDisplay*	gDisplay;
		GC gc;
		XGCValues		gcv;
		gcv.background = WhitePixel (gDisplay, DefaultScreen (gDisplay));
		gcv.foreground = BlackPixel (gDisplay, DefaultScreen (gDisplay));
		gcv.subwindow_mode = IncludeInferiors;

		if (! (gc = ::XCreateGC (gDisplay, GetDrawable (), GCSubwindowMode | GCBackground | GCForeground, &gcv))) {
			AssertNotReached (); // must raise exception!!
		}

		ApplyCurrentClip (gc);

		// IGNORE USE SPECIFIED SIZES - WE DONT SUPPORET SCALING YET!!!
		Assert (fromRect.GetLeft () == 0);
		Assert (fromRect.GetTop () == 0);
		::XCopyArea (gDisplay, from.GetOSPixmap (), GetDrawable (), gc,
					(int)fromRect.GetLeft (), (int)fromRect.GetTop (),
					(unsigned int)fromRect.GetWidth (), (unsigned int)fromRect.GetHeight (),
					(int)toRect.GetLeft (), (int)toRect.GetTop ());
		::XFreeGC (gDisplay, gc);
	}
#endif	/*GDI*/
}

void	Tablet::BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode, const Region& mask)
{
#if		qMacGDI
	osRect	osFromRect;
	osRect	osToRect;
	os_cvt (fromRect, osFromRect);
	os_cvt (toRect, osToRect);

	DoSetPort ();
	DoSetOrigin (kZeroPoint);					// need to handle this better
	if (GDIConfiguration ().ColorQDAvailable ()) {
		osPixMap*	fromPMPtr	=	*from.GetNewOSRepresentation ();
		::CopyBits ((osBitMap*)fromPMPtr, &GetOSGrafPtr ()->portBits, &osFromRect, &osToRect, tMode, mask.PeekAtOSRegion ());
	}
	else {
		::CopyBits (from.GetOldOSRepresentation (), &GetOSGrafPtr ()->portBits,	&osFromRect, &osToRect, tMode, mask.PeekAtOSRegion ());
	}
#elif	qXGDI
	// SEE PAGE 100 of X VOLUME II	::XCopyPlane ();

	extern	osDisplay*	gDisplay;
	GC gc;
	XGCValues		gcv;
	gcv.background = WhitePixel (gDisplay, DefaultScreen (gDisplay));
	gcv.foreground = BlackPixel (gDisplay, DefaultScreen (gDisplay));
	gcv.subwindow_mode = IncludeInferiors;
	if (! (gc = ::XCreateGC (gDisplay, GetDrawable (), GCSubwindowMode | GCBackground | GCForeground, &gcv))) {
		AssertNotReached (); // must raise exception!!
	}

	// IGNORE USE SPECIFIED SIZES - WE DONT SUPPORET SCALING YET!!!
	Assert (fromRect.GetLeft () == 0);
	Assert (fromRect.GetTop () == 0);
	XCopyPlane (gDisplay, from.GetOSPixmap (), GetDrawable (), gc,
				(int)fromRect.GetLeft (), (int)fromRect.GetTop (),
				(unsigned int)fromRect.GetWidth (), (unsigned int)fromRect.GetHeight (),
				(int)toRect.GetLeft (), (int)toRect.GetTop (),
				1);
	::XFreeGC (gDisplay, gc);
#endif	/*GDI*/
}

void	Tablet::BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode)
{
#if		qMacGDI
	osRect	osFromRect;
	osRect	osToRect;
	os_cvt (fromRect, osFromRect);
	os_cvt (toRect, osToRect);

	DoSetPort ();
	DoSetOrigin (kZeroPoint);					// need to handle this better
	if (GDIConfiguration ().ColorQDAvailable ()) {
		osPixMap*	toPMPtr	=	*to.GetNewOSRepresentation ();
		::CopyBits (&GetOSGrafPtr ()->portBits, (osBitMap*)toPMPtr, &osFromRect, &osToRect, tMode, Nil);
	}
	else {
		::CopyBits (&GetOSGrafPtr ()->portBits,	to.GetOldOSRepresentation (), &osFromRect, &osToRect, tMode, Nil);
	}
#elif	qXGDI
	// SEE PAGE 100 of X VOLUME II	::XCopyPlane ();

	extern	osDisplay*	gDisplay;
	GC gc;
	XGCValues		gcv;
	gcv.background = WhitePixel (gDisplay, DefaultScreen (gDisplay));
	gcv.foreground = BlackPixel (gDisplay, DefaultScreen (gDisplay));
	gcv.subwindow_mode = IncludeInferiors;
	if (! (gc = ::XCreateGC (gDisplay, GetDrawable (), GCSubwindowMode | GCBackground | GCForeground, &gcv))) {
		AssertNotReached (); // must raise exception!!
	}

	// IGNORE USE SPECIFIED SIZES - WE DONT SUPPORET SCALING YET!!!
	Assert (fromRect.GetLeft () == 0);
	Assert (fromRect.GetTop () == 0);
	XCopyPlane (gDisplay, GetDrawable (), to.GetOSPixmap (), gc,
				(int)fromRect.GetLeft (), (int)fromRect.GetTop (),
				(unsigned int)fromRect.GetWidth (), (unsigned int)fromRect.GetHeight (),
				(int)toRect.GetLeft (), (int)toRect.GetTop (),
				1);
	::XFreeGC (gDisplay, gc);
#endif	/*qMacGDI*/
}

void	Tablet::BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode, const Region& mask)
{
#if		qMacGDI
	osRect	osFromRect;
	osRect	osToRect;
	os_cvt (fromRect, osFromRect);
	os_cvt (toRect, osToRect);

	DoSetPort ();
	DoSetOrigin (kZeroPoint);					// need to handle this better
	if (GDIConfiguration ().ColorQDAvailable ()) {
		osPixMap*	toPMPtr	=	*to.GetNewOSRepresentation ();
		::CopyBits (&GetOSGrafPtr ()->portBits, (osBitMap*)toPMPtr, &osFromRect, &osToRect, tMode, mask.PeekAtOSRegion ());
	}
	else {
		::CopyBits (&GetOSGrafPtr ()->portBits,	to.GetOldOSRepresentation (), &osFromRect, &osToRect, tMode, mask.PeekAtOSRegion ());
	}
#elif	qXGDI
	// SEE PAGE 100 of X VOLUME II	::XCopyPlane ();

	extern	osDisplay*	gDisplay;
	GC gc;
	XGCValues		gcv;
	gcv.background = WhitePixel (gDisplay, DefaultScreen (gDisplay));
	gcv.foreground = BlackPixel (gDisplay, DefaultScreen (gDisplay));
	gcv.subwindow_mode = IncludeInferiors;
	if (! (gc = ::XCreateGC (gDisplay, GetDrawable (), GCSubwindowMode | GCBackground | GCForeground, &gcv))) {
		AssertNotReached (); // must raise exception!!
	}

	// IGNORE USE SPECIFIED SIZES - WE DONT SUPPORET SCALING YET!!!
	Assert (fromRect.GetLeft () == 0);
	Assert (fromRect.GetTop () == 0);
	XCopyPlane (gDisplay, GetDrawable (), to.GetOSPixmap (), gc,
				(int)fromRect.GetLeft (), (int)fromRect.GetTop (),
				(unsigned int)fromRect.GetWidth (), (unsigned int)fromRect.GetHeight (),
				(int)toRect.GetLeft (), (int)toRect.GetTop (),
				1);
	::XFreeGC (gDisplay, gc);
#endif	/*GDI*/
}

#if		qMacGDI
struct	ContextRecord {
	osGrafPort*	fSavePort;
	osGrafPort*	fNewWorkPort;
	osPoint		fOrigin;
	Region		fClip;
	short		fFontID;
	short		fFontSize;
	short		fFontStyle;
	PenState	fPenState;
//	osPixPat**	fPenPixPat;
};


inline	Boolean	IsColorGrafPort (osGrafPort* osg)
{
	RequireNotNil (osg);
	// check high two bits of version
	// I think this corresponds to rowbytes which guarentteed always smaller?
	return (Boolean (!!(((osCGrafPort*)osg)->portVersion & 0xC000)));
}

static	ContextRecord	sContextRecordStack [10];
const	kContextRecordSize  = sizeof (sContextRecordStack)/sizeof (sContextRecordStack[0]);
static	UInt16			sContextRecordStackTop	=	0;

void	Tablet::PrepareOSRep (osGrafPort* newWorkPort)
{
	Require (sContextRecordStackTop < kContextRecordSize);	// no overflow please

	// Save current info on context stack.
	ContextRecord&	theCR	=	sContextRecordStack [sContextRecordStackTop];

	theCR.fSavePort = qd.thePort;
	theCR.fNewWorkPort = newWorkPort;

	::SetPort (theCR.fNewWorkPort);			// so we save params of port we'll munge

	// See IM - Vol 1 - Page 166 - This is actually right way to 'GetOrigin ()' for port!!!
	theCR.fOrigin = *(osPoint*)&qd.thePort->portRect;
	::GetClip (theCR.fClip.GetOSRegion ());
	theCR.fFontID = qd.thePort->txFont;
	theCR.fFontSize = qd.thePort->txSize;
	theCR.fFontStyle = qd.thePort->txFace;
	::GetPenState (&theCR.fPenState);
	sContextRecordStackTop++;		// push new record on stack

	::SetOrigin (0, 0);	// Setup new port.
}

void	Tablet::RestoreOSRep ()
{
	static	Boolean	kColorQDAvailable	=	GDIConfiguration ().ColorQDAvailable ();
	/*
	 * This code in restore might not be necessary, since we always assure we set things up
	 * properly before calling any of our own code.  This is only in case we are using some
	 * system provided port, we must restore it to its previous state.  We used to (WMgrPort).
	 * I dont think we do any more.  In which case, this code can go.  But also make sure
	 * were calling setorigin (will soon for PixPat offset fix but NYI).
	 */

	sContextRecordStackTop--;
	Require (sContextRecordStackTop < kContextRecordSize);	// no underflow please (since unsigned - really right!)

	ContextRecord&	theCR	=	sContextRecordStack [sContextRecordStackTop];

	::SetPort (theCR.fNewWorkPort);
	::SetOrigin (theCR.fOrigin.h, theCR.fOrigin.v);
	DoSetClip (theCR.fClip.PeekAtOSRegion ());
	::TextFont (theCR.fFontID);
	::TextSize (theCR.fFontSize);
	::TextFace (theCR.fFontStyle);
#if		0
// cuz I DID SOMETHING BAD WITH CONTROLS???
	// for some bizarre reason GetPenState () seems to crash sometimes
	::MoveTo (theCR.fPenState.pnLoc.h, theCR.fPenState.pnLoc.h);
	::PenSize (theCR.fPenState.pnSize.h, theCR.fPenState.pnSize.v);
	::PenMode (theCR.fPenState.pnMode);
	::PenPat (theCR.fPenState.pnPat);
#else
	::SetPenState (&theCR.fPenState);
#endif
#if 0
	if (theCR.fPenPixPat != Nil) {
		Assert (GDIConfiguration ().ColorQDAvailable ());		// how else?
// need OSRENAME FOR THESE GUYS (fPenPixPat)
		// See IM V-74-75 to see why this is safe/right
		::PenPixPat (theCR.fPenPixPat);
	}
#endif
	::SetPort (theCR.fSavePort);	// restore original - maybe not necesary, but...
}

const	Point	Tablet::kDontSetOrigin	= Point (kMaxInt32, kMaxInt32);

void	Tablet::SetupOSRepFromBrush (struct osGrafPort* grafPtr, const Brush& brush, const Point& origin)
{
	static	Boolean	kColorQDAvailable	=	GDIConfiguration ().ColorQDAvailable ();
	xDoSetPort (grafPtr);
	if (origin != kDontSetOrigin) {
Assert (origin == kZeroPoint);		// later we will lift this when we implemnt tile alignment, but for now this
									// should be true..

		DoSetOrigin (origin);
	}
	if (kColorQDAvailable) {
#if		qCacheTraps
		(*kPenModePtr) (brush.GetTransferMode ());
#else	/*qCacheTraps*/
		::PenMode (brush.GetTransferMode ());
#endif	/*qCacheTraps*/
	}
	else {
		// always use pattern - not src modes/???
		// RETHINK????
		// why does apple distinguish - seems
		// penMode always use |8, and copybits
		// NEVER????
		short	thePenMode	=	brush.GetTransferMode ();
		thePenMode &= 0xF;									// Dont use big color modes...
															// this is probably wrong - need
															// to be sure we gen closest approx
															// mode.  Hopefully this works out
															// but MUST CHECK
		thePenMode |= 0x8;
#if		qCacheTraps
		(*kPenModePtr) (thePenMode);
#else	/*qCacheTraps*/
		::PenMode (thePenMode);
#endif	/*qCacheTraps*/
	}
	const Tile&	tile	=	brush.GetTile ();
	if (kColorQDAvailable) {
		// ~const on osPixPat
		::PenPixPat ((osPixPat**) tile.GetNewQDPattern ());
	}
	else {
		::PenPat (tile.GetOldQDPattern ());
	}
}

void	Tablet::SetupOSRepFromPen (struct osGrafPort* grafPtr, const Pen& pen, const Point& origin)
{
	SetupOSRepFromBrush (grafPtr, pen, origin);
	osPoint	osp	=	os_cvt (pen.GetPenSize (), osp);
	::PenSize (osp.h, osp.v);
}

void	Tablet::SetupOSRepFromFont (struct osGrafPort* grafPtr, TransferMode tMode, const Font& font, const Point& origin)
{
	static	Boolean	kColorQDAvailable	=	GDIConfiguration ().ColorQDAvailable ();

	xDoSetPort (grafPtr);
	if (origin != kDontSetOrigin) {
Assert (origin == kZeroPoint);		// later we will lift this when we implemnt tile alignment, but for now this
									// should be true..

		DoSetOrigin (origin);
	}
	if (qd.thePort->txFont != font.GetID ()) {
		::TextFont (font.GetID ());
	}
	if (qd.thePort->txSize != font.GetSize ()) {
		::TextSize (font.GetSize ());
	}
	if (qd.thePort->txFace != font.GetStyleOSRep ()) {
		::TextFace (font.GetStyleOSRep ());
	}

	if (kColorQDAvailable) {
		::TextMode (tMode);
	}
	else {
		// See SetupBrush ()
		// IM 1-171 is even more restrictive, but we'll try this and see what happens... For now.
		::TextMode (tMode & 0x8);
	}
}

void	Tablet::SetupOSRepFromClip (struct osGrafPort* grafPtr, const Region& clip, const Point& origin)
{
	xDoSetPort (grafPtr);
	if (origin != kDontSetOrigin) {
Assert (origin == kZeroPoint);		// later we will lift this when we implemnt tile alignment, but for now this
									// should be true..

		DoSetOrigin (origin);
	}
	DoSetClip (clip.PeekAtOSRegion ());
}

void	Tablet::SetOSBackground (const Tile& backGround)
{
	DoSetPort ();
	if (GDIConfiguration ().ColorQDAvailable ()) {
		const osPixPat** newPattern = backGround.GetNewQDPattern ();
		AssertNotNil (newPattern);
		// ~const on osPixPat
		::BackPixPat ((osPixPat**)newPattern);
	}
	else {
		const osPattern* newPattern = backGround.GetOldQDPattern ();
		AssertNotNil (newPattern);
		::BackPat (newPattern);
	}
}

void	Tablet::SetupOSRepFromBrush (const Brush& brush, const Point& origin)
{
	DoSetPort ();
	SetupOSRepFromBrush (GetOSGrafPtr (), brush, origin);
}

void	Tablet::SetupOSRepFromPen (const Pen& pen, const Point& origin)
{
	DoSetPort ();
	SetupOSRepFromPen (GetOSGrafPtr (), pen, origin);
}

void	Tablet::SetupOSRepFromFont (TransferMode tMode, const Font& font, const Point& origin)
{
	DoSetPort ();
	SetupOSRepFromFont (GetOSGrafPtr (), tMode, font, origin);
}

void	Tablet::SetupOSRepFromClip (const Region& clip, const Point& origin)
{
	DoSetPort ();
	SetupOSRepFromClip (GetOSGrafPtr (), clip, origin);
}

void	Tablet::DoSetPort () const
{
	xDoSetPort (GetOSGrafPtr ());
}

#endif	/*qMacGDI*/

void	Tablet::Sync ()
{
// makes sense, but NYI for mac
#if		qXGDI
	extern	osDisplay*	gDisplay;
	::XFlush (gDisplay);
#endif
}

void	Tablet::ServerUnGrab ()
{
	// temporarily disable grabs since not terribly important, and occasionally causes the server to hang - not
	// sure why...LGP March 14, 1992
#if		qXGDI && 0
	extern	osDisplay*	gDisplay;
	::XUngrabServer (gDisplay);
#endif
}

void	Tablet::ServerGrab ()
{
	// temporarily disable grabs since not terribly important, and occasionally causes the server to hang - not
	// sure why...LGP March 14, 1992
#if		qXGDI && 0
	extern	osDisplay*	gDisplay;
	::XGrabServer (gDisplay);
#endif
}

#if		qXGDI && qDebug
Boolean		Tablet::sSyncMode	=	False;

Boolean	Tablet::GetSyncMode ()
{
	return (sSyncMode);
}

void	Tablet::SetSyncMode (Boolean syncMode)
{
	sSyncMode = syncMode;
	extern	osDisplay*	gDisplay;
	(void)::XSynchronize (gDisplay, syncMode);
}
#endif	/*qXGDI && qDebug*/


#if		qXGDI
struct	_XGC*	Tablet::UseBrush (const Brush& brush)
{
	extern	osDisplay*	gDisplay;
	_XGC*			aGC;
	XGCValues		gcv;

	// Just quick guess at these values
	switch (brush.GetTransferMode ()) {
	  case	eCopyTMode:		gcv.function = GXcopy;	break;
	  case	eOrTMode:		gcv.function = GXor;	break;
	  case	eXorTMode:		gcv.function = GXxor;	break;
	  case	eBicTMode:		gcv.function = GXxor;	break;	// obviously wrong, but looks better than other wrong alternatives..
	  case	eNotCopyTMode:	gcv.function = GXcopyInverted;	break;
	  case	eNotOrTMode:	gcv.function = GXcopy;	break;
	  case	eNotXorTMode:	gcv.function = GXequiv;	break;  //I THINK THIS IS RIGHT BUT PLAIN XOR LOOKS BETTER IN TRACKERS FOR NOW HACK...
	  //case	eNotXorTMode:	gcv.function = GXxor;	break;
	  case	eNotBicTMode:	gcv.function = GXxor;	break;
	  default: AssertNotReached ();
	}


// hack to make XOR look right - should NOT be needed!!! without this works OK for outlines, but not fillrectangles...
if (gcv.function == GXxor) {
	gcv.background = WhitePixel (gDisplay, DefaultScreen (gDisplay));
	gcv.foreground = BlackPixel (gDisplay, DefaultScreen (gDisplay));
	gcv.foreground = gcv.foreground ^ gcv.background;
	gcv.fill_style = FillStippled;
	gcv.stipple = brush.GetTile ().GetOSPixmap ();
	gcv.subwindow_mode = IncludeInferiors;
	if (! (aGC = ::XCreateGC (gDisplay, fDrawable, GCSubwindowMode | GCFunction | GCStipple | GCFillStyle | GCBackground | GCForeground, &gcv))) {
		AssertNotReached (); // must raise exception!!
	}
	ApplyCurrentClip (aGC);
	return (aGC);
}


	// define a bogus pixelmap with the right attributes for the current drawable. This will be used
	// as a hint to the Tile stuff in constructing the appropriate expanded tile.
	PixelMap*	bogusPMap	=	Nil;

// if our monitor is indirect device... handle other cases later
	if (True) {
		bogusPMap = new PixelMap (PixelMap::eChunky, ((osVisual*)fVisual)->bits_per_rgb, Rect (kZeroPoint, Point (8, 8)),
								  Nil, fColorLookupTable);
	}
	else {
	}
	AssertNotNil (bogusPMap);

	gcv.tile = brush.GetTile ().ExpandTileTo (*bogusPMap);
	delete (bogusPMap);
	gcv.fill_style = FillTiled;
	gcv.subwindow_mode = IncludeInferiors;
	if (! (aGC = ::XCreateGC (gDisplay, fDrawable, GCSubwindowMode | GCFunction | GCTile | GCFillStyle, &gcv))) {
		AssertNotReached (); // must raise exception!!
	}

	ApplyCurrentClip (aGC);
	return (aGC);


#if		0

	_XGC*			aGC;
	XGCValues		gcv;

	// Just quick guess at these values
	switch (brush.GetTransferMode ()) {
	  case	eCopyTMode:		gcv.function = GXcopy;	break;
	  case	eOrTMode:		gcv.function = GXor;	break;
	  case	eXorTMode:		gcv.function = GXxor;	break;
	  case	eBicTMode:		gcv.function = GXxor;	break;	// obviously wrong, but looks better than other wrong alternatives..
	  case	eNotCopyTMode:	gcv.function = GXcopyInverted;	break;
	  case	eNotOrTMode:	gcv.function = GXcopy;	break;
	  case	eNotXorTMode:	gcv.function = GXequiv;	break;  //I THINK THIS IS RIGHT BUT PLAIN XOR LOOKS BETTER IN TRACKERS FOR NOW HACK...
	  //case	eNotXorTMode:	gcv.function = GXxor;	break;
	  case	eNotBicTMode:	gcv.function = GXxor;	break;
	  default: AssertNotReached ();
	}
	gcv.stipple = brush.GetTile ().GetOSPixmap ();
	if (brush.GetTransferMode () == eCopyTMode) {
		gcv.fill_style = FillOpaqueStippled;
	}
	else {
		gcv.fill_style = FillStippled;
	}


// These 2 assignemts are WRONG - look at the two colors (FOR NOW WE ONYL SUPPORT 2) in the pixelmap for the tile,
// and lookup their pixel values (using aproximations if ncessary), and use those pixel values...
//
	extern	osDisplay*	gDisplay;
	gcv.background = WhitePixel (gDisplay, DefaultScreen (gDisplay));
	gcv.foreground = BlackPixel (gDisplay, DefaultScreen (gDisplay));



	// hack to see if this fixes my color problems???
	// See Orielly volume 1-196
	if (gcv.function == GXxor) {
		gcv.foreground = gcv.foreground ^ gcv.background;
	}
	
	gcv.subwindow_mode = IncludeInferiors;
	if (! (aGC = ::XCreateGC (gDisplay, fDrawable, GCSubwindowMode | GCFunction | GCStipple | GCFillStyle | GCBackground | GCForeground, &gcv))) {
		AssertNotReached (); // must raise exception!!
	}

	ApplyCurrentClip (aGC);
	return (aGC);
#endif
}

void	Tablet::DoneWithBrush (struct _XGC* theGC)
{
	RequireNotNil (theGC);
	extern	osDisplay*	gDisplay;
	::XFreeGC (gDisplay, theGC);
}

struct	_XGC*	Tablet::UsePen (const Pen& pen)
{
	return (UseBrush (pen));
}

void	Tablet::DoneWithPen (struct _XGC* theGC)
{
	DoneWithBrush (theGC);
}

struct	_XGC*	Tablet::UseFont (const Font& font)
{
	_XGC*			aGC;
	XGCValues		gcv;

	gcv.font = font.GetID ();
	gcv.subwindow_mode = IncludeInferiors;
	extern	osDisplay*	gDisplay;
	if (! (aGC = ::XCreateGC (gDisplay, fDrawable, GCSubwindowMode | GCFont, &gcv))) {
		AssertNotReached (); // must raise exception!!
	}
	ApplyCurrentClip (aGC);
	return (aGC);
}

void	Tablet::DoneWithFont (struct _XGC* theGC)
{
	RequireNotNil (theGC);
	extern	osDisplay*	gDisplay;
	::XFreeGC (gDisplay, theGC);
}

void	Tablet::ApplyCurrentClip (struct _XGC* theGC)
{
	extern	osDisplay*	gDisplay;
	::XSetRegion (gDisplay, theGC, GetEffectiveClipRegion ().PeekAtOSRegion ());
}

#endif	/*qXGDI*/














/*
 ********************************************************************************
 ********************************* TabletOwner **********************************
 ********************************************************************************
 */

TabletOwner::TabletOwner ()
{
}

TabletOwner::~TabletOwner ()
{
}

Point	TabletOwner::LocalToTablet (const Point& p) const
{
	return (LocalToTablet_ (p));
}

Point	TabletOwner::TabletToLocal (const Point& p) const
{
	return (TabletToLocal_ (p));
}

Rect	TabletOwner::LocalToTablet (const Rect& r) const
{
	return (Rect (LocalToTablet (r.GetOrigin ()), r.GetSize ()));
}

Rect	TabletOwner::TabletToLocal (const Rect& r) const
{
	return (Rect (TabletToLocal (r.GetOrigin ()), r.GetSize ()));
}

Region	TabletOwner::LocalToTablet (const Region& r) const
{
	Point	origin = r.GetBounds ().GetOrigin ();
	return (r + (LocalToTablet (origin) - origin));
}

Region	TabletOwner::TabletToLocal (const Region& r) const
{
	Point	origin = r.GetBounds ().GetOrigin ();
	return (r + (TabletToLocal (origin) - origin));
}

void	TabletOwner::OutLine (const Shape& s, const Rect& shapeBounds, const Pen& pen)
{
	register	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->OutLine (s, LocalToTablet (shapeBounds), pen);
}

void	TabletOwner::Paint (const Shape& s, const Rect& shapeBounds, const Brush& brush)
{
	register	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->Paint (s, LocalToTablet (shapeBounds), brush);
}

void	TabletOwner::DrawText (const String& text, const Font& font, const Point& at)
{
	register	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->DrawText (text, font, LocalToTablet (at));
}

Coordinate	TabletOwner::TextWidth (const String& text, const Font& font, const Point& at) const
{
	register	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	return (t->TextWidth (text, font, LocalToTablet (at)));
}

void	TabletOwner::DrawPicture (const Picture& picture, const Rect& intoRect)
{
	register	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->DrawPicture (picture, LocalToTablet (intoRect));
}

void	TabletOwner::DrawPicture (const Picture& picture, const Point& at)
{
	DrawPicture (picture, picture.GetFrame () + at);
}

void	TabletOwner::ScrollBits (const Rect& r, const Point& delta, const Tile& background)
{
	register	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->ScrollBits (LocalToTablet (r), delta, background);
}

void	TabletOwner::BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode)
{
	register	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->BitBlit (from, fromRect, LocalToTablet (toRect), tMode);
}

void	TabletOwner::BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode, const Region& mask)
{
	register	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->BitBlit (from, fromRect, LocalToTablet (toRect), tMode, mask);
}

void	TabletOwner::BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode)
{
	register	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->BitBlit (fromRect, to, LocalToTablet (toRect), tMode);
}

void	TabletOwner::BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode, const Region& mask)
{
	register	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->BitBlit (fromRect, to, LocalToTablet (toRect), tMode, mask);
}






#if		qXGDI
/*
 ********************************************************************************
 ********************************* FillInVisualAndCLUT **************************
 ********************************************************************************
 */

static	void	FillInVisualAndCLUT (long drawable, osVisual* visual, ColorLookupTable* clut)
{
	RequireNotNil (drawable);
	RequireNotNil (visual);
	RequireNotNil (clut);

	extern	osDisplay*	gDisplay;
	osWindowAttributes	xwa;
	memset (&xwa, 0, sizeof (xwa));
	Status s = ::XGetWindowAttributes (gDisplay, drawable, &xwa);
	Assert (s != 0);		// should never happen (Orielly -II - 255)...
	AssertNotNil (xwa.visual);
	memcpy (visual, xwa.visual, sizeof (*visual));

	osColormap	cmap		=	xwa.colormap;
	int			mapEntries	=	visual->map_entries;
Require (mapEntries <= 256);		// otherwise we must handle differrently???
	osColor*	colors		=	new osColor [mapEntries];
	memset (colors, 0, sizeof (osColor)*mapEntries);
	for (int i = 0; i < mapEntries; i++) {
		colors [i].pixel = i;
	}

	::XQueryColors (gDisplay, cmap, colors, mapEntries);
	clut->SetEntryCount (mapEntries);
	Assert (clut->GetEntryCount () == mapEntries);
	for (i = 1; i <= mapEntries; i++) {
		clut->SetEntry (i, Color (colors [i-1].red, colors[i-1].green, colors[i-1].blue));
	}
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***




