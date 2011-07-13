/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Tile.cc,v 1.8 1992/11/25 22:44:47 lewis Exp $
 *
 * TODO:
 *		-	Investigate sharing storage between our PixelMap we keep and the internal
 *			rep of the Tile. May not be possible, or only somtimes?
 *
 *		-	Be more careful about the patType field - and expecialy careful documented
 *			and properly implementing what happens when someone calls get old/new QD
 *			patterns under old/new qd. We should support both - but document limitations.
 *
 *		-	Take into account limitations of Mac QD PixPats - size multiple of two - Not sure
 *			what happens if we don't honor that - no asserts here even - ????
 *
 *
 * Changes:
 *	$Log: Tile.cc,v $
 *		Revision 1.8  1992/11/25  22:44:47  lewis
 *		Got rid of obsolete Implement macro.
 *
 *		Revision 1.7  1992/09/11  18:41:55  sterling
 *		new Shape stuff, got rid of String Peek references
 *
 *		Revision 1.5  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  05:35:19  lewis
 *		Wrap mac-only GetOldQDOSPattern in #ifqMacGDI. And add workaround for
 *		qCFront_NestedTypesHalfAssed to get to compile under GCC.
 *
 *		Revision 1.3  1992/07/16  05:29:38  lewis
 *		Lots of cleanups. Got rid of resource constructors (probably will
 *		be handled totaly from within Resource Mgr Code - talk to sterl
 *		about this).
 *		Big change to dual reps for color/noncolor qd that I've been thinking
 *		about for a long time. No totally type safe - yeah! Got rid of IsColorPixMap ()
 *		method. GetOld/NewPixPat () gauranteed to work under all circumstatnces
 *		whether you have color qd or not (Not sure totally implemented right, but
 *		I think interface is right).
 *		Also, accessors now return const osPatterns, and osPixPat**. If you want to
 *		make changes, make a new one.
 *
 *		Revision 1.2  1992/07/15  21:57:53  lewis
 *		Hack code in CTOR for tile to do special copy/hack for making tiles from
 *		Pixelmaps of size 8x8. Special case rowBytes to be 1. Rewrite this mess
 *		soon properly taking into account limitations of colorQD. This is just
 *		half-assed.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.23  1992/06/09  12:38:20  lewis
 *		Renamed SmartPointer back to Shared.
 *
 *		Revision 1.22  92/06/09  14:22:34  14:22:34  sterling (Sterling Wight)
 *		use template for reference counting
 *		
 *		Revision 1.21  1992/05/18  17:18:29  lewis
 *		Implement comparison and IOStram operators.
 *
 *		Revision 1.20  92/05/13  00:21:05  00:21:05  lewis (Lewis Pringle)
 *		On Mac, Keep PixelMap around for Tiles.
 *		
 *		Revision 1.19  92/05/10  00:35:35  00:35:35  lewis (Lewis Pringle)
 *		Compile on PC.
 *		
 *		Revision 1.18  92/04/30  13:34:09  13:34:09  lewis (Lewis Pringle)
 *		Clenaup TileSharedPart::ExpandTileTo (const PixelMap& referencePMap)
 *		and make it non-const. And add some debug messages - doesn't seem to quite work right/?? Trouble may not be
 *		here though - maybe in what CLUT we get back from UNIX.
 *		
 *		Revision 1.17  92/04/30  03:35:17  03:35:17  lewis (Lewis Pringle)
 *		Added fExpanded field to TileSharedPart, and use that as a cache in ExpandTileTo() a new
 *		function for XGDI. This expands the tile to the target pixelmaps (ie drawables) depth,
 *		CLUT, etc...
 *		Get rid of IsStipple  () since never used - maybe add back later??
 *		
 *		Revision 1.16  92/04/20  14:15:42  14:15:42  lewis (Lewis Pringle)
 *		added typedef char* caddr_t conditional on qSnake && _POSIX_SOURCE, before include of Xutil.h, since that
 *		file references caddr_t which doesn't appear to be part of POSIX.
 *		
 *		Revision 1.15  92/04/15  14:39:01  14:39:01  lewis (Lewis Pringle)
 *		Added hack definitions of iostream and comparison operators - do real ones later -who needs 'em?
 *		
 *		Revision 1.14  92/04/07  11:26:39  11:26:39  lewis (Lewis Pringle)
 *		Compile on PC.
 *		
 *		Revision 1.13  92/04/02  11:23:11  11:23:11  lewis (Lewis Pringle)
 *		Get rid of fReferenceCount since now TileSharedPart subclasses from Shared.
 *		
 *		Revision 1.11  1992/03/22  10:22:27  lewis
 *		Got rid of arg to GetOSPixmap - use global gDisplay instead.
 *
 *		Revision 1.10  1992/02/14  02:51:10  lewis
 *		Ported to mac.
 *
 *		Revision 1.9  1992/02/12  06:16:03  lewis
 *		Use GDIConfig instead of OSConfig, and support new apple definition of osPattern.
 *
 *		Revision 1.8  1992/02/11  22:00:38  lewis
 *		Support keeping actual colors assoicated wtih pixelmap in tile.
 *		Note in the docs that we only support stipples for now, since otherwise
 *		we would need general depth conversion routines.
 *		Allow access to (readonly) underlying PixelMap (not yet supported on mac).
 *		Get rid of SetHi/Lo level rep routines for xgdi version of TileSharedPart.
 *
 *
 */

#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<OSUtils.h>
#include	<Memory.h>
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
#include	"StreamUtils.hh"

#include	"BitBlitter.hh"
#include	"GDIConfiguration.hh"

#include	"Tile.hh"






/*
 ********************************************************************************
 *************************************** Tile ***********************************
 ********************************************************************************
 */

Tile::Tile (const PixelMap& pixelMap):
	fSharedPart (new TileSharedPart (pixelMap))
{
}

#if		qMacGDI
const	osPattern*	Tile::GetOldQDPattern () const
{
	EnsureNotNil (fSharedPart->fOSPattern);
	return (&(*fSharedPart->fOSPattern)->pat1Data);
}
#endif









/*
 ********************************************************************************
 ********************************* TileSharedPart *******************************
 ********************************************************************************
 */

TileSharedPart::TileSharedPart (const PixelMap& pixelMap):
	fPixelMap (pixelMap),
#if		qMacGDI
	fOSPattern (Nil)
#elif	qXGDI
	fExpanded (pixelMap)
#endif	/*GDI*/
{
#if		qMacGDI
	if (GDIConfiguration ().ColorQDAvailable ()) {
// Hack for QD special case - MUST DO ALL THIS MORE GENERALLY...
		if (pixelMap.GetBounds ().GetSize () == Point (8, 8)) {
			PixelMap	pm (Point (8, 8), Nil, 1);
			BitBlitter ().Apply (pixelMap, pm, BitBlitter::kCopy);
			fPixelMap = pm;
		}
		osPixMap**	theSrcOSPixMap	=	fPixelMap.GetNewOSRepresentation ();
		RequireNotNil (theSrcOSPixMap);
		if ((fOSPattern = ::NewPixPat ()) == Nil) {
			gMemoryException.Raise ();
		}

		char	oldPatHState	=	::HGetState (osHandle (fOSPattern));
		::HLock (osHandle (fOSPattern));

		/*
		 * Seems to work even if rowbytes == 1. Simpler than doing copy ourselves, so
		 * leave it, but if it gives us trouble, we can do manual clone.
		 */
		::CopyPixMap (theSrcOSPixMap, (*fOSPattern)->patMap);
	
		// See IM V-73
		if (((*theSrcOSPixMap)->bounds.left == 0) and
			((*theSrcOSPixMap)->bounds.right == 8)) {
			(*theSrcOSPixMap)->rowBytes = 1 | 0x8000;
		}
		Assert (((*theSrcOSPixMap)->rowBytes & 0x7fff) ==
				((*theSrcOSPixMap)->bounds.right -
				 (*theSrcOSPixMap)->bounds.left + 1
				 )*
				(*theSrcOSPixMap)->pixelSize/8
			);
		Assert (((*theSrcOSPixMap)->rowBytes & 0x7fff) >= 1);

		/*
		 * Clone the actual image.  Don't bother resetting the baseAddr of the osPixMap, since
		 * IM V-56 says its ignored (presumably because real data is in the patData handle,
		 * and so that data addr can float.
		 */
		osHandle	destHandle	=	(*fOSPattern)->patData;
		size_t		imageSize	=	((*theSrcOSPixMap)->rowBytes & 0x7fff) * (*theSrcOSPixMap)->bounds.bottom;
		::SetHandleSize (destHandle, imageSize);
		if (MemError () != noErr) {
			::HSetState (osHandle (fOSPattern), oldPatHState);
			gMemoryException.Raise (MemError ());
		}
		memcpy (*destHandle, (*theSrcOSPixMap)->baseAddr, imageSize);
	
		::HSetState (osHandle (fOSPattern), oldPatHState);
		(*fOSPattern)->patXValid = -1;// unncessary???
	}
	else {
		/*
		 * Now matter what the bitdepth, and what the size, use the upper 8x8 lhs box, and
		 * translate 0 pixels into 0, and nonzero pixels into ones.
		 */
		UInt8	macPatternBytes [sizeof (osPattern)];
		for (UInt8 row = 0; row < sizeof (osPattern); row++) {
			macPatternBytes [row] = 0;
			for (UInt8 col = 0; col < 8; col++) {
				if (pixelMap.GetPixel (pixelMap.GetBounds ().GetOrigin () + Point (row, col))) {
					macPatternBytes [row] |= (1 << col);		// right order?
				}
			}
		}
		fOSPattern = (osPixPat**)::NewHandle (sizeof (osPixPat));
		::HLock (osHandle (fOSPattern));
		memcpy (&(*fOSPattern)->pat1Data, &macPatternBytes, sizeof (osPattern));
	}
#endif	/*qMacGDI*/
}

TileSharedPart::~TileSharedPart ()
{
#if		qMacGDI
	if (fOSPattern != Nil) {
		if (GDIConfiguration ().ColorQDAvailable ()) {
			::HUnlock (osHandle (fOSPattern));
			::DisposeHandle (osHandle (fOSPattern));
		}
		else {
			::DisposPixPat (fOSPattern);
		}
		fOSPattern = Nil;
	}
#endif	/*qMacGDI*/
}

#if		qXGDI
unsigned	long	TileSharedPart::GetOSPixmap ()
{
	return (fPixelMap.GetOSPixmap ());
}

unsigned long	TileSharedPart::ExpandTileTo (const PixelMap& referencePMap)
{
	/*
	 * Keep a cache of the pixelmap expanded to the desired one, and if need be, then compute it.
	 * Since the reference PMap clut etc should RARELY change, this should almost always happen just once.
	 */
	PixelMap::PixelStyle	style	=	referencePMap.GetPixelStyle ();
#if		!qCFront_NestedTypesHalfAssed
	PixelMap::
#endif
	Depth					depth	=	referencePMap.GetDepth ();
	Rect					bRect	=	Rect (kZeroPoint, fPixelMap.GetBounds ().GetSize ());
	if ((fExpanded.GetPixelStyle () != style) or
		(fExpanded.GetDepth () != depth) or
		((style == PixelMap::eChunky) and
		 (fExpanded.GetColorLookupTable () != referencePMap.GetColorLookupTable ()))) {

		if (style == PixelMap::eChunky) {
			fExpanded = PixelMap (style, depth, bRect, Nil, referencePMap.GetColorLookupTable ());
		}
		else {
			fExpanded = PixelMap (style, depth, bRect);
		}
		BitBlitter ().Apply (fPixelMap, fExpanded, BitBlitter::kCopy);
	}
	return (fExpanded.GetOSPixmap ());
}
#endif	/*GDI*/

const	PixelMap&	TileSharedPart::PeekAtPixmap ()
{
	return (fPixelMap);
}








/*
 ********************************************************************************
 ********************************* iostream operators  **************************
 ********************************************************************************
 */
ostream&	operator<< (ostream& out, const Tile& tile)
{
	out << lparen << tile.PeekAtPixmap () << rparen;
	return (out);
}

istream&	operator>> (istream& in, Tile& tile)
{
	char	ch;
	in >> ch;
	if (ch != lparen) {					// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	PixelMap	pm;
	in >> pm;

	in >> ch;
	if (ch != lparen) {					// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	if (in) {
		tile = Tile (pm);
	}
	return (in);
}





/*
 ********************************************************************************
 ******************************* Comparison operators  **************************
 ********************************************************************************
 */
Boolean	operator== (const Tile& lhs, const Tile& rhs)
{
	return (lhs.PeekAtPixmap () == rhs.PeekAtPixmap ());
}

Boolean	operator!= (const Tile& lhs, const Tile& rhs)
{
	return (lhs.PeekAtPixmap () != rhs.PeekAtPixmap ());
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

