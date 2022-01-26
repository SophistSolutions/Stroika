/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/BitBlitter.cc,v 1.7 1992/11/25 22:38:27 lewis Exp $
 *
 *
 * TODO:
 *		Stroika should have some LRU cache ATD. When we do, use it here!!!
 *
 *
 *
 * Changes:
 *	$Log: BitBlitter.cc,v $
 *		Revision 1.7  1992/11/25  22:38:27  lewis
 *		Real->double - real obsolete.
 *
 *		Revision 1.6  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.5  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  05:43:37  lewis
 *		Added workarounds for !qCFront_NestedTypesHalfAssed. One of them - for
 *		BitBlitter::BitBlitInfo is highly questionable - see the comments by it - I
 *		suspect gcc 2.2.1 is wrong in this case. Investigate after vacation.
 *
 *		Revision 1.3  1992/07/08  02:02:18  lewis
 *		Use AssertNotReached () instead of SwitchFallThru ().
 *
 *		Revision 1.2  1992/07/02  03:40:42  lewis
 *		Added include <string.h> and got rid of unneeded StreamUtils.hh and Tablet.hh.
 *
 *		Revision 1.15  1992/05/13  00:33:06  lewis
 *		Got rid of old debug mnessages, and added some asserts in building InverseTable
 *		to assure that it is properly constructed.
 *
 *		Revision 1.13  92/04/30  03:31:20  03:31:20  lewis (Lewis Pringle)
 *		In DoPortableApply () re-arange special cases so more sane in choosing, and move
 *		general case out into a new seperate function.
 *		Minor speed tweeks (like register declarations and moving stuff out of loops).
 *		Fixed bug in LRU reording in GetInverseTable.
 *		
 *		Revision 1.12  92/04/29  12:18:36  12:18:36  lewis (Lewis Pringle)
 *		Added proper LRU inverse table caching scheme, and put in seperate function.
 *		Added new speclial case alogorithm using new ReadWriteExpandedScanLine object from PixelMap.
 *		Broke out most specailcase bitblit algorithms into seperate functions to make more readable,
 *		and maintainable.
 *		
 *		Revision 1.11  92/04/20  16:45:41  16:45:41  lewis (Lewis Pringle)
 *		Minor hack to work around MPW compiler bug - function result cast to void sometimes gen bad C code.
 *		
 *		Revision 1.10  92/04/20  14:14:41  14:14:41  lewis (Lewis Pringle)
 *		added typedef char* caddr_t conditional on qSnake && _POSIX_SOURCE, before include of Xutil.h, since that
 *		file references caddr_t which doesn't appear to be part of POSIX.
 *		
 *		Revision 1.8  92/04/17  17:11:44  17:11:44  lewis (Lewis Pringle)
 *		Use new Inverse table support as optimization for copies. Still needs lots of work. Bad hack to
 *		cache one CLUT. Should do better.
 *		
 *		Revision 1.7  92/04/17  12:37:54  12:37:54  sterling (Sterling Wight)
 *		Try optimizing bitblits.
 *		
 *		Revision 1.6  92/04/07  10:02:44  10:02:44  lewis (Lewis Pringle)
 *		Ported to mac - and worked around mac compiler bug in defining BitBlitter::kCopy.
 *		
 *		Revision 1.5  92/04/07  01:03:42  01:03:42  lewis (Lewis Pringle)
 *		Misc work on bitblitter. hack into copy function to use CLUT.Lookup (). Really should be cautions
 *		and only do when necessary, and do now for other functions too.
 *		
 *		Revision 1.4  92/04/02  11:08:22  11:08:22  lewis (Lewis Pringle)
 *		Added fSrc/Dest Pixel/Color to BitBlitInfo, and got rid of Pixel Params to Transfer Funciton.
 *		Define the portable implentations of transfer functions to be pointers to real functions we
 *		implement below (partially).
 *		Rework Apply code so that we never bother using the native X version since it handles so few cases
 *		and in those, it probably is slower that whatever we can do. Lots of cleanups and debuging
 *		of portable mapping algorithm.
 *		
 *		Revision 1.1  1992/03/19  20:37:17  lewis
 *		Initial revision
 *
 *
 */


#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<Memory.h>
#include	<OSUtils.h>
#include	<QDOffscreen.h>
#include	<QuickDraw.h>
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#endif	/*GDI*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"Memory.hh"

#include	"GDIConfiguration.hh"

#include	"BitBlitter.hh"






static	Boolean	HasCLUT (const PixelMap& pm);




/*
 ********************************************************************************
 *********************************** BitBlitInfo ********************************
 ********************************************************************************
 */
#if		!qCFront_NestedTypesHalfAssed
BitBlitter::
#endif
BitBlitInfo::BitBlitInfo (const BitBlitter& bitBlitter, const PixelMap& from, const Rect& fromRect,
						 PixelMap& to, const Rect& toRect, const Region* mask):
	fBitBlitter (bitBlitter),
	fSrcPixelMap (from),
	fDstPixelMap (to),
	fSrcRect (fromRect),
	fDstRect (toRect),
	fMask (mask),
	fSrcAt (fromRect.GetOrigin ()),
	fDstAt (toRect.GetOrigin ()),
	fSrcPixel (0),
	fDstPixel (0),
	fSrcColor (kBlackColor),
	fDstColor (kBlackColor)
{
}





/*
 ********************************************************************************
 *********************************** BitBlitter *********************************
 ********************************************************************************
 */

const	BitBlitter::TransferFunction		BitBlitter::kCopy_Portable		=	BitBlitter::sCopyFunction;
const	BitBlitter::TransferFunction		BitBlitter::kOr_Portable		=	BitBlitter::sOrFunction;
const	BitBlitter::TransferFunction		BitBlitter::kXor_Portable		=	BitBlitter::sXOrFunction;
const	BitBlitter::TransferFunction		BitBlitter::kBic_Portable		=	BitBlitter::sBicFunction;
const	BitBlitter::TransferFunction		BitBlitter::kNotCopy_Portable	=	BitBlitter::sNotCopyFunction;
const	BitBlitter::TransferFunction		BitBlitter::kNotOr_Portable		=	BitBlitter::sNotOrFunction;
const	BitBlitter::TransferFunction		BitBlitter::kNotXor_Portable	=	BitBlitter::sNotXOrFunction;
const	BitBlitter::TransferFunction		BitBlitter::kNotBic_Portable	=	BitBlitter::sNotBicFunction;

#if		qMacGDI
const	BitBlitter::TransferFunction		BitBlitter::kCopy_MacGDI		=	(BitBlitter::TransferFunction)1;
const	BitBlitter::TransferFunction		BitBlitter::kOr_MacGDI			=	(BitBlitter::TransferFunction)2;
const	BitBlitter::TransferFunction		BitBlitter::kXor_MacGDI			=	(BitBlitter::TransferFunction)3;
const	BitBlitter::TransferFunction		BitBlitter::kBic_MacGDI			=	(BitBlitter::TransferFunction)4;
const	BitBlitter::TransferFunction		BitBlitter::kNotCopy_MacGDI		=	(BitBlitter::TransferFunction)5;
const	BitBlitter::TransferFunction		BitBlitter::kNotOr_MacGDI		=	(BitBlitter::TransferFunction)6;
const	BitBlitter::TransferFunction		BitBlitter::kNotXor_MacGDI		=	(BitBlitter::TransferFunction)7;
const	BitBlitter::TransferFunction		BitBlitter::kNotBic_MacGDI		=	(BitBlitter::TransferFunction)8;
#endif	/*qMacGDI*/

#if		qXGDI
const	BitBlitter::TransferFunction		BitBlitter::kCopy_XGDI			=	(BitBlitter::TransferFunction)1;
const	BitBlitter::TransferFunction		BitBlitter::kOr_XGDI			=	(BitBlitter::TransferFunction)2;
const	BitBlitter::TransferFunction		BitBlitter::kXor_XGDI			=	(BitBlitter::TransferFunction)3;
const	BitBlitter::TransferFunction		BitBlitter::kBic_XGDI			=	(BitBlitter::TransferFunction)4;
const	BitBlitter::TransferFunction		BitBlitter::kNotCopy_XGDI		=	(BitBlitter::TransferFunction)5;
const	BitBlitter::TransferFunction		BitBlitter::kNotOr_XGDI			=	(BitBlitter::TransferFunction)6;
const	BitBlitter::TransferFunction		BitBlitter::kNotXor_XGDI		=	(BitBlitter::TransferFunction)7;
const	BitBlitter::TransferFunction		BitBlitter::kNotBic_XGDI		=	(BitBlitter::TransferFunction)8;
#endif	/*qXGDI*/


#if		qMacGDI

#if		qMPW_STATIC_CONST_ENUM_MEMBER_NOT_DEFINED_WHEN_DEFINED_BUG
const	BitBlitter::TransferFunction		BitBlitter::kCopy				=	(BitBlitter::TransferFunction)1;
const	BitBlitter::TransferFunction		BitBlitter::kOr					=	(BitBlitter::TransferFunction)2;
const	BitBlitter::TransferFunction		BitBlitter::kXor				=	(BitBlitter::TransferFunction)3;
const	BitBlitter::TransferFunction		BitBlitter::kBic				=	(BitBlitter::TransferFunction)4;
const	BitBlitter::TransferFunction		BitBlitter::kNotCopy			=	(BitBlitter::TransferFunction)5;
const	BitBlitter::TransferFunction		BitBlitter::kNotOr				=	(BitBlitter::TransferFunction)6;
const	BitBlitter::TransferFunction		BitBlitter::kNotXor				=	(BitBlitter::TransferFunction)7;
const	BitBlitter::TransferFunction		BitBlitter::kNotBic				=	(BitBlitter::TransferFunction)8;
#else
const	BitBlitter::TransferFunction		BitBlitter::kCopy				=	BitBlitter::kCopy_MacGDI;
const	BitBlitter::TransferFunction		BitBlitter::kOr					=	BitBlitter::kOr_MacGDI;
const	BitBlitter::TransferFunction		BitBlitter::kXor				=	BitBlitter::kXor_MacGDI;
const	BitBlitter::TransferFunction		BitBlitter::kBic				=	BitBlitter::kBic_MacGDI;
const	BitBlitter::TransferFunction		BitBlitter::kNotCopy			=	BitBlitter::kNotCopy_MacGDI;
const	BitBlitter::TransferFunction		BitBlitter::kNotOr				=	BitBlitter::kNotOr_MacGDI;
const	BitBlitter::TransferFunction		BitBlitter::kNotXor				=	BitBlitter::kNotXor_MacGDI;
const	BitBlitter::TransferFunction		BitBlitter::kNotBic				=	BitBlitter::kNotBic_MacGDI;
#endif

#elif	qXGDI
const	BitBlitter::TransferFunction		BitBlitter::kCopy				=	BitBlitter::kCopy_XGDI;
const	BitBlitter::TransferFunction		BitBlitter::kOr					=	BitBlitter::kOr_XGDI;
const	BitBlitter::TransferFunction		BitBlitter::kXor				=	BitBlitter::kXor_XGDI;
const	BitBlitter::TransferFunction		BitBlitter::kBic				=	BitBlitter::kBic_XGDI;
const	BitBlitter::TransferFunction		BitBlitter::kNotCopy			=	BitBlitter::kNotCopy_XGDI;
const	BitBlitter::TransferFunction		BitBlitter::kNotOr				=	BitBlitter::kNotOr_XGDI;
const	BitBlitter::TransferFunction		BitBlitter::kNotXor				=	BitBlitter::kNotXor_XGDI;
const	BitBlitter::TransferFunction		BitBlitter::kNotBic				=	BitBlitter::kNotBic_XGDI;
#endif	/*GDI*/




BitBlitter::BitBlitter ()
{
}

void	BitBlitter::Apply (const PixelMap& from, PixelMap& to, TransferFunction tFunction)
{
// not quite right?? do we scale or not??
	RequireNotNil (tFunction);
	Rect	newFromRect	=	from.GetBounds ();
	Rect	newToRect	=	to.GetBounds ();
	double	scaleV		=	double (newFromRect.GetHeight ()) / double (newToRect.GetHeight ());
	double	scaleH		=	double (newFromRect.GetWidth ()) / double (newToRect.GetWidth ());
	DoApply (from, newFromRect, to, newToRect, tFunction, scaleV, scaleH, Nil);
}

void	BitBlitter::Apply (const PixelMap& from, PixelMap& to, TransferFunction tFunction, const Region& mask)
{
// not quite right?? do we scale or not??
	RequireNotNil (tFunction);
	Rect	newFromRect	=	from.GetBounds ();
	Rect	newToRect	=	to.GetBounds () * mask.GetBounds ();
	DoApply (from, newFromRect, to, newToRect, tFunction, 1, 1, &mask);
}

void	BitBlitter::Apply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Point& toPoint, TransferFunction tFunction)
{
	RequireNotNil (tFunction);
	Rect	newFromRect	=	fromRect * from.GetBounds ();
	Rect	newToRect	=	Rect (toPoint, fromRect.GetSize ()) * to.GetBounds ();
	DoApply (from, newFromRect ,to, newToRect, tFunction, 1, 1, Nil);
}

void	BitBlitter::Apply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Point& toPoint,
							TransferFunction tFunction, const Region& mask)
{
	RequireNotNil (tFunction);
	Rect	newFromRect	=	fromRect * from.GetBounds ();
	Rect	newToRect	=	Rect (toPoint, fromRect.GetSize ()) * to.GetBounds () * mask.GetBounds ();
	DoApply (from, newFromRect ,to, newToRect, tFunction, 1, 1, &mask);
}

void	BitBlitter::Apply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferFunction tFunction)
{
	RequireNotNil (tFunction);
	double	scaleV		=	double (fromRect.GetHeight ()) / double (toRect.GetHeight ());
	double	scaleH		=	double (fromRect.GetWidth ()) / double (toRect.GetWidth ());
	Rect	newFromRect	=	fromRect * from.GetBounds ();
	Rect	newToRect	=	toRect * to.GetBounds ();
	DoApply (from, newFromRect ,to, newToRect, tFunction, scaleV, scaleH, Nil);
}

void	BitBlitter::Apply (const PixelMap& from, const Rect& fromRect, PixelMap& to,  const Rect& toRect,
							TransferFunction tFunction, const Region& mask)
{
	RequireNotNil (tFunction);
	double	scaleV		=	double (fromRect.GetHeight ()) / double (toRect.GetHeight ());
	double	scaleH		=	double (fromRect.GetWidth ()) / double (toRect.GetWidth ());
	Rect	newFromRect	=	fromRect * from.GetBounds ();
	Rect	newToRect	=	toRect * to.GetBounds () * mask.GetBounds ();
	DoApply (from, newFromRect ,to, newToRect, tFunction, scaleV, scaleH, &mask);
}

void	BitBlitter::DoApply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Rect& toRect,
							TransferFunction tFunction, double scaleV, double scaleH, const Region* mask)
{
	RequireNotNil (tFunction);
#if		qMacGDI
	/*
	 * This code assumes that the MacGDI functions are numbered sequentially - not very safe, but very
	 * simple, and THAT does engender some safety. Also, note that a switch on kCopy would be nice but
	 * the compilers dont seem to allow it cuz its not quite const enuf (they really should in this case).
	 * Also, an if then else if skip chain would be long and therefore error prone.
	 */
	switch (int (tFunction)) {
		case	1:	DoMacGDIApply (from, fromRect, to, toRect, srcCopy, scaleV, scaleH, mask); Assert (kCopy_MacGDI == tFunction); break;
		case	2:	DoMacGDIApply (from, fromRect, to, toRect, srcOr, scaleV, scaleH, mask); Assert (kOr_MacGDI == tFunction); break;
		case	3:	DoMacGDIApply (from, fromRect, to, toRect, srcXor, scaleV, scaleH, mask); Assert (kXor_MacGDI == tFunction); break;
		case	4:	DoMacGDIApply (from, fromRect, to, toRect, srcBic, scaleV, scaleH, mask); Assert (kBic_MacGDI == tFunction); break;
		case	5:	DoMacGDIApply (from, fromRect, to, toRect, notSrcCopy, scaleV, scaleH, mask); Assert (kNotCopy_MacGDI == tFunction); break;
		case	6:	DoMacGDIApply (from, fromRect, to, toRect, notSrcOr, scaleV, scaleH, mask); Assert (kNotOr_MacGDI == tFunction); break;
		case	7:	DoMacGDIApply (from, fromRect, to, toRect, notSrcXor, scaleV, scaleH, mask); Assert (kNotXor_MacGDI == tFunction); break;
		case	8:	DoMacGDIApply (from, fromRect, to, toRect, notSrcBic, scaleV, scaleH, mask); Assert (kNotBic_MacGDI == tFunction); break;
		default:	DoPortableApply (from, fromRect, to, toRect, tFunction, scaleV, scaleH, mask); break;
	}
#elif	qXGDI

	/*
	 * There are a whole plethora of cases that X does not support. Rather difficult to enumerate them all.
	 * Also, since doing them from X may involve making transfers to and from the server, and having it do
	 * the computations, there may be NO point in having X do these transforms (in those few cases where it
	 * is capable). So for now, just ALWAYS do them locally, with our portable implementation.
	 */
	if (True or (from.GetDepth () != to.GetDepth ()) or (scaleV != 1) or (scaleH != 1)) {
		/*
		 * This code assumes that the XGDI functions are numbered sequentially - not very safe, but very
		 * simple, and THAT does engender some safety. Also, note that a switch on kCopy would be nice but
		 * the compilers dont seem to allow it cuz its not quite const enuf (they really should in this case).
		 * Also, an if then else if skip chain would be long and therefore error prone.
		 */
		switch (int (tFunction)) {
			case	1:	DoPortableApply (from, fromRect, to, toRect, kCopy_Portable, scaleV, scaleH, mask); Assert (kCopy_XGDI == tFunction); break;
			case	2:	DoPortableApply (from, fromRect, to, toRect, kOr_Portable, scaleV, scaleH, mask); Assert (kOr_XGDI == tFunction); break;
			case	3:	DoPortableApply (from, fromRect, to, toRect, kXor_Portable, scaleV, scaleH, mask); Assert (kXor_XGDI == tFunction); break;
			case	4:	DoPortableApply (from, fromRect, to, toRect, kBic_Portable, scaleV, scaleH, mask); Assert (kBic_XGDI == tFunction); break;
			case	5:	DoPortableApply (from, fromRect, to, toRect, kNotCopy_Portable, scaleV, scaleH, mask); Assert (kNotCopy_XGDI == tFunction); break;
			case	6:	DoPortableApply (from, fromRect, to, toRect, kNotOr_Portable, scaleV, scaleH, mask); Assert (kNotOr_XGDI == tFunction); break;
			case	7:	DoPortableApply (from, fromRect, to, toRect, kNotXor_Portable, scaleV, scaleH, mask); Assert (kNotXor_XGDI == tFunction); break;
			case	8:	DoPortableApply (from, fromRect, to, toRect, kNotBic_Portable, scaleV, scaleH, mask); Assert (kNotBic_XGDI == tFunction); break;
			default:	DoPortableApply (from, fromRect, to, toRect, tFunction, scaleV, scaleH, mask); break;
		}
	}
	else {
		/*
		 * This code assumes that the XGDI functions are numbered sequentially - not very safe, but very
		 * simple, and THAT does engender some safety. Also, note that a switch on kCopy would be nice but
		 * the compilers dont seem to allow it cuz its not quite const enuf (they really should in this case).
		 * Also, an if then else if skip chain would be long and therefore error prone.
		 */
		switch (int (tFunction)) {
			case	1:	DoXGDIApply (from, fromRect, to, toRect, GXcopy, scaleV, scaleH, mask); Assert (kCopy_XGDI == tFunction); break;
			case	2:	DoXGDIApply (from, fromRect, to, toRect, GXor, scaleV, scaleH, mask); Assert (kOr_XGDI == tFunction); break;
			case	3:	DoXGDIApply (from, fromRect, to, toRect, GXxor, scaleV, scaleH, mask); Assert (kXor_XGDI == tFunction); break;
			case	4:	DoXGDIApply (from, fromRect, to, toRect, GXxor, scaleV, scaleH, mask); Assert (kBic_XGDI == tFunction); break;
			case	5:	DoXGDIApply (from, fromRect, to, toRect, GXcopyInverted, scaleV, scaleH, mask); Assert (kNotCopy_XGDI == tFunction); break;
			case	6:	DoXGDIApply (from, fromRect, to, toRect, GXcopy, scaleV, scaleH, mask); Assert (kNotOr_XGDI == tFunction); break;
			case	7:	DoXGDIApply (from, fromRect, to, toRect, GXequiv, scaleV, scaleH, mask); Assert (kNotXor_XGDI == tFunction); break;
			case	8:	DoXGDIApply (from, fromRect, to, toRect, GXcopy, scaleV, scaleH, mask); Assert (kNotBic_XGDI == tFunction); break;
			default:	DoPortableApply (from, fromRect, to, toRect, tFunction, scaleV, scaleH, mask); break;
		}
	}
#endif	/*GDI*/
}

#if		qMacGDI
void	BitBlitter::DoMacGDIApply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Rect& toRect,
									int tFunction, double scaleV, double scaleH, const Region* mask)
{
	Require (tFunction >= 0);
	Require (tFunction <= 100);		// didn't accidentally pass procptr???

	// Got to think about colorizing problem and lack for ports.  Also, make uses inverse table
	// of one of these guys forget which which is slightly non-portable>?? 
	// basically, simple copybiuts call here though...

	osRect	osFromRect;
	osRect	osToRect;
	os_cvt (fromRect, osFromRect);
	os_cvt (toRect, osToRect);
	osRegion**	theOSMask	=	(mask==Nil)? Nil: mask->PeekAtOSRegion ();
	if (GDIConfiguration ().ColorQDAvailable ()) {
		osPixMap*	fromPMPtr	=	*from.GetNewOSRepresentation ();
		osPixMap*	toPMPtr		=	*to.GetNewOSRepresentation ();
		::CopyBits ((osBitMap*)fromPMPtr, (osBitMap*)toPMPtr, &osFromRect, &osToRect, tFunction, theOSMask);
	}
	else {
		::CopyBits (from.GetOldOSRepresentation (), to.GetOldOSRepresentation (), &osFromRect, &osToRect, tFunction, theOSMask);
	}
}
#endif	/*qMacGDI*/

#if		qXGDI
void	BitBlitter::DoXGDIApply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Rect& toRect,
								int tFunction, double scaleV, double scaleH, const Region* mask)
{
	AssertNotReached ();		// For now dont bother using this, since it does so badly...
								// If we do want to use this, it needs LOTS of work - LGP April 1, 1992

	Require (scaleV == 1);		// All that X supports for now (X11R4)
	Require (scaleH == 1);		// All that X supports for now (X11R4)

	extern	osDisplay*	gDisplay;
// SEE PAGE 100 of X VOLUME II	::XCopyPlane ();
	GC gc;
	XGCValues		gcv;
	if (from.GetColorLookupTable () == to.GetColorLookupTable ()) {
		gcv.background = 0;
		gcv.foreground = 1;
	}
	else {
		gcv.background = WhitePixel (gDisplay, DefaultScreen (gDisplay));
		gcv.foreground = BlackPixel (gDisplay, DefaultScreen (gDisplay));
	}
	gcv.subwindow_mode = IncludeInferiors;
	if (! (gc = ::XCreateGC (gDisplay, to.GetOSPixmap (), GCSubwindowMode | GCBackground | GCForeground, &gcv))) {
		AssertNotReached (); // must raise exception!!
	}

	// IGNORE USE SPECIFIED SIZES - WE DONT SUPPORET SCALING YET!!!
	Assert (fromRect.GetLeft () == 0);
	Assert (fromRect.GetTop () == 0);
	::XCopyPlane (gDisplay, from.GetOSPixmap (), to.GetOSPixmap (), gc,
				(int)fromRect.GetLeft (), (int)fromRect.GetTop (),
				(unsigned int)fromRect.GetWidth (), (unsigned int)fromRect.GetHeight (),
				(int)toRect.GetLeft (), (int)toRect.GetTop (),
				1);
	::XFreeGC (gDisplay, gc);
}
#endif	/*qXGDI*/

void	BitBlitter::DoPortableApply (const PixelMap& from, const Rect& fromRect, PixelMap& to, const Rect& toRect,
									 TransferFunction tFunction, double scaleV, double scaleH, const Region* mask)
{
	Require (int (tFunction) > 10);	// be sure we've been given a real proc ptr here... Not one of our magic numbers...

	/*
	 * Early concepts: much more thought required here:
	 *
	 * The basic algorithm is to iterate over points in the range, and back compute values from the
	 * domain. Trim our copy to take into account any masking. First check for a few interesting special
	 * cases, and see if we can avoid looping...
	 */

	Rect clippedToRect	=	toRect * to.GetBounds ();
	if (mask != Nil) {
		clippedToRect = clippedToRect * mask->GetBounds ();
	}


// Must be more careful in these special cases about clipping!!!


	/*
	 * Some special cases we can handle more efficiently.
	 */

	if (tFunction == sCopyFunction) {
		Boolean	weScale			=	Boolean (scaleH != 1 or scaleV != 1);
		Boolean	fromHasClut		=	HasCLUT (from);
		Boolean	toHasClut		=	HasCLUT (to);
		Boolean	equalCluts		=	False;		// only makes sense if fromHasClut toHasClut

		if (fromHasClut and toHasClut) {
			equalCluts = Boolean (from.GetColorLookupTable () == to.GetColorLookupTable ());
		}

		Boolean	pixelFormatsSame	=	Boolean ((from.GetDepth () == to.GetDepth ()) and
												 (from.GetPixelStyle () == to.GetPixelStyle ()) and
												 ((fromHasClut == toHasClut) and (not (fromHasClut or equalCluts))));


		if (weScale) {
			if (pixelFormatsSame) {
				SpecialBitBlit_Copy_PixelValuesSameBoundsMayDiffer (from, to, clippedToRect, scaleV, scaleH);
				return;
			}
			if (toHasClut) {
				SpecialBitBlit_Copy_WithCLUT (from, to, clippedToRect, scaleV, scaleH);
				return;
			}
		}
		else {
			if (pixelFormatsSame and
				(fromRect == toRect) and 
				(from.GetBounds () == to.GetBounds ()) and
				(from.GetBounds () == clippedToRect)
				) {

				/*
				 * A fairly common case we can do quite well on. In fact, detecting this case may be more expensive
				 * than the copy!!!
				 */
				SpecialBitBlit_Copy_RectsSame_DepthSame_BoundsSame_ClutSame (from, to);
				return;
			}

			if (toHasClut) {
				SpecialBitBlit_Copy_WithCLUT (from, to, clippedToRect, 1, 1);	// do new version of this with scale hardwired!!!
				return;
			}
		}
	}



	/*
	 * If we get this far, go ahead and do the brute force transformation.
	 */
	GeneralCase_PortableApply (from, fromRect, to, toRect, tFunction, scaleV, scaleH, mask);
}

#if		!qCFront_NestedTypesHalfAssed
	PixelMap::
#endif
Pixel	BitBlitter::sCopyFunction (const BitBlitInfo& bitBlitInfo)
{
// hack.. do this sometimes but not always!!! only if converting depth/cluts
	return (bitBlitInfo.fDstPixelMap.GetColorLookupTable ().Lookup (bitBlitInfo.fSrcColor)-1);
//	return (bitBlitInfo.fSrcPixel);
}

#if		!qCFront_NestedTypesHalfAssed
	PixelMap::
#endif
Pixel	BitBlitter::sOrFunction (const BitBlitInfo& bitBlitInfo)
{
// if scaling, we should take into account neighboring pixels???
	return (bitBlitInfo.fSrcPixel | bitBlitInfo.fDstPixel);
}

#if		!qCFront_NestedTypesHalfAssed
	PixelMap::
#endif
Pixel	BitBlitter::sXOrFunction (const BitBlitInfo& bitBlitInfo)
{
// if scaling, we should take into account neighboring pixels???
	return (bitBlitInfo.fSrcPixel ^ bitBlitInfo.fDstPixel);
}

#if		!qCFront_NestedTypesHalfAssed
	PixelMap::
#endif
Pixel	BitBlitter::sBicFunction (const BitBlitInfo& bitBlitInfo)
{
// if scaling, we should take into account neighboring pixels???
AssertNotReached();// lookup??
//	return (lhs);
}

#if		!qCFront_NestedTypesHalfAssed
	PixelMap::
#endif
Pixel	BitBlitter::sNotCopyFunction (const BitBlitInfo& bitBlitInfo)
{
// if scaling, we should take into account neighboring pixels???
	return (bitBlitInfo.fSrcPixel ^ bitBlitInfo.fDstPixel);
}

#if		!qCFront_NestedTypesHalfAssed
	PixelMap::
#endif
Pixel	BitBlitter::sNotOrFunction (const BitBlitInfo& bitBlitInfo)
{
// if scaling, we should take into account neighboring pixels???
	return (~ (bitBlitInfo.fSrcPixel | bitBlitInfo.fDstPixel) );
}

#if		!qCFront_NestedTypesHalfAssed
	PixelMap::
#endif
Pixel	BitBlitter::sNotXOrFunction (const BitBlitInfo& bitBlitInfo)
{
// if scaling, we should take into account neighboring pixels???
	return (~ (bitBlitInfo.fSrcPixel ^ bitBlitInfo.fDstPixel) );
}

#if		!qCFront_NestedTypesHalfAssed
	PixelMap::
#endif
Pixel	BitBlitter::sNotBicFunction (const BitBlitInfo& bitBlitInfo)
{
// if scaling, we should take into account neighboring pixels???
AssertNotReached();// lookup??
//	return (lhs);
}

void	BitBlitter::SpecialBitBlit_Copy_RectsSame_DepthSame_BoundsSame_ClutSame (const PixelMap& from, PixelMap& to)
{
	Require (from.GetBounds () == to.GetBounds ());
	Assert (from.GetDepth () == to.GetDepth ());
	Assert (from.GetImageSize () == to.GetImageSize ());
	// use memmove since memory could overlap!!!
	memmove (to.GetBaseAddress (), from.GetBaseAddress (), to.GetImageSize ());
}

void	BitBlitter::SpecialBitBlit_Copy_PixelValuesSameBoundsMayDiffer (const PixelMap& from, PixelMap& to,
										const Rect& copyToRect, double scaleV, double scaleH)
{
	Require (HasCLUT (from));
	Require (HasCLUT (to));
	Require (from.GetColorLookupTable () == to.GetColorLookupTable ());
	Require (from.GetDepth () == to.GetDepth ());

				const Coordinate	rowFirst	=	copyToRect.GetTop ();
				const Coordinate	rowLast		=	copyToRect.GetBottom ();
				const Coordinate	colFirst	=	copyToRect.GetLeft ();
	register	const Coordinate	colLast		=	copyToRect.GetRight ();

	for (Coordinate row = copyToRect.GetTop (); row < rowLast; row++) {
		Point	srcPt	=	Point (row * scaleV, 0);
		ReadWriteExpandedScanLine	dstScanLine (to, row);
		switch (dstScanLine.GetChunkSize ()) {
			case	1: {
				register	UInt8*	cursor	=	dstScanLine.GetScanLine8 () + colFirst;
				for (register Coordinate col = colFirst; col < colLast; col++) {
					srcPt.SetH (col*scaleH);
					*cursor++ = from.GetPixel (srcPt);
				}
			}
			break;
			case	2: {
				register	UInt16*	cursor	=	dstScanLine.GetScanLine16 () + colFirst;;
				for (register Coordinate col = colFirst; col < colLast; col++) {
					srcPt.SetH (col*scaleH);
					*cursor++ = from.GetPixel (srcPt);
				}
			}
			break;
			case	4: {
				register	UInt32*	cursor	=	dstScanLine.GetScanLine32 () + colFirst;
				for (register Coordinate col = colFirst; col < colLast; col++) {
					srcPt.SetH (col*scaleH);
					*cursor++ = from.GetPixel (srcPt);
				}
			}
			break;
			default: {
				AssertNotReached ();
			}
			break;
		}
	}
}

void	BitBlitter::SpecialBitBlit_Copy_WithCLUT (const PixelMap& from, PixelMap& to,
			const Rect& copyToRect, double scaleV, double scaleH)
{
				ColorLookupTable	toClut		=	to.GetColorLookupTable ();
				const InverseTable&	inv			=	GetInverseTable (toClut);
				const Coordinate	rowFirst	=	copyToRect.GetTop ();
				const Coordinate	rowLast		=	copyToRect.GetBottom ();
				const Coordinate	colFirst	=	copyToRect.GetLeft ();
	register	const Coordinate	colLast		=	copyToRect.GetRight ();
#if		!qCFront_NestedTypesHalfAssed
	PixelMap::
#endif
				Depth				dstDepth	=	to.GetDepth ();

	for (Coordinate row = rowFirst; row < rowLast; row++) {
		Point	srcPt	=	Point (row * scaleV, 0);
		ReadWriteExpandedScanLine	dstScanLine (to, row);
		register	UInt8*	cursor	=	dstScanLine.GetScanLine8 () + colFirst;
		for (register Coordinate col = colFirst; col < colLast; col++) {
			srcPt.SetH (col*scaleH);
			Assert (inv.Lookup (from.GetColor (srcPt)) - 1 >= 0);
			Assert (inv.Lookup (from.GetColor (srcPt)) - 1 < (1 << dstDepth));
			*cursor++ = inv.Lookup (from.GetColor (srcPt)) - 1;
		}
	}
}

void	BitBlitter::GeneralCase_PortableApply (const PixelMap& from, const Rect& fromRect, PixelMap& to,
								const Rect& toRect,	TransferFunction tFunction, double scaleV, double scaleH,
								const Region* mask)
{
	Rect clippedToRect	=	toRect * to.GetBounds ();
	if (mask != Nil) {
		clippedToRect = clippedToRect * mask->GetBounds ();
	}

// I probably have these ifdefs wrong - both versions should work in both cases, I believe, but
// the first failed under gcc 2.2.1 - LGP 7/21/92... Investigate later...
#if		qCFront_NestedTypesHalfAssed
	BitBlitInfo	bbi		=	BitBlitInfo (*this, from, fromRect, to, clippedToRect, mask);
#else
	BitBlitter::BitBlitInfo	bbi		=	BitBlitter::BitBlitInfo (*this, from, fromRect, to, clippedToRect, mask);
#endif

	for (Coordinate row = clippedToRect.GetTop (); row < clippedToRect.GetBottom (); row++) {
		// adjust bbi here
		bbi.fDstAt.SetV (row);
		bbi.fSrcAt.SetV (row * scaleV);
		for (Coordinate col = clippedToRect.GetLeft (); col < clippedToRect.GetRight (); col++) {
			bbi.fDstAt.SetH (col);
			bbi.fSrcAt.SetH (col * scaleH);
			Assert (from.Contains (bbi.fSrcAt));

			bbi.fSrcPixel = from.GetPixel (bbi.fSrcAt);
			bbi.fDstPixel = to.GetPixel (bbi.fDstAt);
			bbi.fSrcColor = from.GetColor (bbi.fSrcAt);
			bbi.fDstColor = to.GetColor (bbi.fDstAt);

			/*
			 * Mappings can be done on the basis of colors or Pixels, but the result we get from our function
			 * must be in Pixels.
			 */
			to.SetPixel (bbi.fDstAt, (tFunction) (bbi));
		}
	}
}

const	InverseTable&	BitBlitter::GetInverseTable (const ColorLookupTable& clut)
{
	/*
	 * Simple routine to build the inverse table. Only reason to have this routine is so we can
	 * maintain a simple LRU cache of inverse tables (may want to think about how to make configurable
	 * which itables we cache, and how many???).
	 *
	 * Note that this rouinte returns a reference ONLY because we are doing this caching. That would be
	 * a bad choice otherwise. We feel safe doing this only because this routine is private.
	 *
	 * Notes on the size of the cache - even though we do shared reference counting of cluts, it can still
	 * be pretty expensive to compare them for equality in the case of a miss. Dont want this too big. Also,
	 * inverse tables can be rather sizable.
	 */
	enum { eITableCacheSize = 5 };
	static	ColorLookupTable**		sCachedCluts			=	Nil;
	static	InverseTable**			sCachedInverseTables	=	Nil;

	if (sCachedCluts == Nil) {
		Assert (sCachedInverseTables == Nil);

		// Fill array with eITableCacheSize cluts...
		sCachedCluts = new ColorLookupTable* [eITableCacheSize];
		for (int i = 0; i < eITableCacheSize; i++) {
			sCachedCluts[i] = new ColorLookupTable (kBlackAndWhiteCLUT);
		}

		// Fill array with eITableCacheSize matching inverse tables.
		sCachedInverseTables = new InverseTable* [eITableCacheSize];
		for (i = 0; i < eITableCacheSize; i++) {
			sCachedInverseTables[i] = new InverseTable (kBlackAndWhiteCLUT);
		}
	}
	AssertNotNil (sCachedCluts);
	AssertNotNil (sCachedInverseTables);


	// See if we have a match ---
	for (int i = 0; i < eITableCacheSize; i++) {
		AssertNotNil (sCachedCluts);
		AssertNotNil (sCachedCluts[i]);
		if (*sCachedCluts[i] == clut) {
			/*
			 * Yeah!!! A cache hit - ring that bell!!!
			 * Now bring ourselves to the head of the LRU, and reshuffle.
			 */
			ColorLookupTable*	newTop	=	sCachedCluts[i];
			InverseTable*		newTopI	=	sCachedInverseTables[i];

			for (int j = i; j != 0; j--) {
				sCachedCluts[j] = sCachedCluts[j-1];
				sCachedInverseTables[j] = sCachedInverseTables[j-1];
			}

			sCachedCluts[0] = newTop;
			sCachedInverseTables[0] = newTopI;

			goto Done;
		}
	}

	/*
	 * No cache hit, so blow away end guy, and add this new one. Then bring him to the front of the cache.
	 */
	{
		ColorLookupTable*	tmp	=	sCachedCluts[eITableCacheSize-1];
		InverseTable*		tmpI=	sCachedInverseTables[eITableCacheSize-1];
		for (i = 1; i < eITableCacheSize; i++) {		// yes I do mean start at 1 - blow away the last guy
			sCachedCluts[i] = sCachedCluts[i-1];
			sCachedInverseTables[i] = sCachedInverseTables[i-1];
		}
		sCachedCluts[0] = tmp;
		sCachedInverseTables[0] = tmpI;
		*sCachedCluts[0] = clut;
		*sCachedInverseTables[0] = InverseTable (clut);
	}

Done:
	Ensure (*sCachedCluts[0] == clut);
	EnsureNotNil (sCachedInverseTables[0]);

	return (*sCachedInverseTables[0]);
}










/*
 ********************************************************************************
 ************************************** HasCLUT *********************************
 ********************************************************************************
 */
static	Boolean	HasCLUT (const PixelMap& pm)
{
	Try {
#if		qMPW_CFront
		// MPW CFront does like cast - LGP April 18, 1992
		/*(void)*/pm.GetColorLookupTable ();
#else
		(void)pm.GetColorLookupTable ();
#endif
	}
	Catch () {
		return (False);
	}
	return (True);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***



