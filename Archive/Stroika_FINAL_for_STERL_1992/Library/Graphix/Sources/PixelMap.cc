/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMap.cc,v 1.8 1992/09/05 16:14:25 lewis Exp $
 *
 *
 * TODO:
 *		-	Upon profiling on the mac, in becomes apparent that we spend alot of time converting back and forth beteen
 *			Mac and stroika reps for rects, and depth of pixelmap, etc... and checking the format - colorQD avail, and
 *			so on. If we made ALWAYS kept both represenations around, we could do much of this MUCH more quickly. Given
 *			that the data in the pixelmap is almost always much larger than the pixelmap itself, this is a pretty safe
 *			optimization. In a similar vein, keeping cached Stroika reps of the various fields, values like PixMapAddr,
 *			from ComputePixMapAddr would make Get/SetPixel MUCH faster. The only worry is that for pixelmaps handed to
 *			us from the outside world, it might not be safe to cache this? A good bet though, is that if we alloced the
 *			memory, then we can cache pointers into it????
 *
 *		-	Finish testing/debugging stream inserter/extractors for all bitdepths - tested and seems to work for depth=1,2,8
 *			which covers most important cases, but see what depth=3 works funny???
 *
 *		-	Internally we should use the type Depth rather than UInt8 all over the place...
 *
 * Notes:
 *		One worrysome point is that we must be careful if we've alloced the pixels or XLib did since
 *		in one case we call delete() to destroythem and in the other, XDestroyImage, or XFree()???
 *
 * Changes:
 *	$Log: PixelMap.cc,v $
 *		Revision 1.8  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.7  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  05:36:42  lewis
 *		Added a bunch of workarounds for qCFront_NestedTypesHalfAssed to get to compile under
 *		gcc. And, in op>>, got rid of ifdefed out code. Made X SnagOSImage const method - not
 *		sure if that was right, but convienient - and probably right.
 *
 *		Revision 1.5  1992/07/16  05:17:55  lewis
 *		Added SetOrigin/GetOrigin and SetSize/GetSize methods - defined in
 *		 terms of how Get/SetBounds () works - no new functionality- just
 *		 convienence.
 *
 *		Revision 1.4  1992/07/08  02:06:21  lewis
 *		Use AssertNotReached () instead of SwitchFallThru ();.
 *
 *		Revision 1.3  1992/07/01  01:41:04  lewis
 *		Got rid of qNestedTypesAvailable flag - assume always true.
 *
 *		Revision 1.2  1992/06/25  16:22:22  lewis
 *		Fix problem with sloppy nested type usage - not caught/allowed by CFront21 so needs bug workaround flag qCFront_NestedTypesHalfAssed.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.47  1992/06/03  16:21:39  lewis
 *		Change inserter/extracter to write out depth as an int, not a char. Did backward
 *		compatability - delete after a few releases.
 *		Also, changed the routines to use new format, which pays no attention to rowbytes, or
 *		byte/bit ordering - sb fully portable across different endian machines (test more), includign
 *		mac and snake).
 *		Added overloaded CTORs taking Point size as arg isntead of Rect boundsRect since its much more
 *		often the convenient thing to use.
 *		Also, similar to above format, rewrite PixelMapFromData () - two overloaded versions, and change
 *		code in User:Sources:CodeGenUtils.cc:WritePixelMapAsPixelMapFromData (roughly that name).
 *		Only tested briefly under UNIX but at least a few cases work, and should run fine on mac - I hope.
 *
 *		Revision 1.45  92/05/18  17:12:45  17:12:45  lewis (Lewis Pringle)
 *		Added no-arg pixelmap CTOR and one taking a size (isntead of a rect).
 *		Also, added lots of casts to make compiler warnings go away about int type conversions in Get/Set Pixel.
 *		Get a little closer to setting fOSImage->bits_per_pixel correctly for X - its really finiky --
 *		though its limitations are documented!.
 *
 *		Revision 1.44  92/05/13  00:35:16  00:35:16  lewis (Lewis Pringle)
 *		Lots more work on Get/Set Pixel - hopefully getting closer now...
 *
 *		Revision 1.43  92/05/10  00:30:26  00:30:26  lewis (Lewis Pringle)
 *		Ported to qWinGDI - just made it compile.
 *		Also, lots of X work, and work on Get/SetPixel - support more depths, and
 *		LSB/MSB - and properly initialize X Image structure MSB/LSB fields according
 *		to new flags in Config.hh, and set chunksize somewhat more appropriately, but
 *		still wrong - needs more work....
 *
 *		Revision 1.42  92/05/02  22:57:47  22:57:47  lewis (Lewis Pringle)
 *		Compile on mac.
 *
 *		Revision 1.41  92/05/01  01:32:58  01:32:58  lewis (Lewis Pringle)
 *		Added XPixelMapFromData () routines as temp hack so we could get it working acceptably for X - soon
 *		we'll do portable new version.
 *
 *		Revision 1.38  92/04/30  13:41:12  13:41:12  lewis (Lewis Pringle)
 *		In readonlyExpandedScanline class snag os image before starting expansion and assert still none
 *		at end.
 *
 *		Revision 1.37  92/04/30  03:39:18  03:39:18  lewis (Lewis Pringle)
 *		Got rid of old commented out code.
 *		Fixed?? depth 2 case for get/set pixel - at least a little closer I think - see what
 *		sterl came up with???
 *		Call SetDepth in op=/X(X&) ctor for Pixelmaps -sterl reported bug - missing - oops.
 *		In XGDI SetDepth, must reset format too.
 *		No longer assert depth is one of a small set - allow any and hope for best. We will
 *		debug this over the next week or so to make it work.
 *		Make PixelMapFromData () a little more lenient, and try to allow it to work with depth cases
 *		under XGDI. Still needs much work.
 *		XGDI.
 *
 *		Revision 1.36  92/04/29  12:28:31  12:28:31  lewis (Lewis Pringle)
 *		In operator= and DTOR dispose of the fOSPixmap from the server if it existed - I think that
 *		was the source of the memory leak that Carl Rosin at Marcam reported.
 *		Try using some of the portable bit accessors in Get/Set Pixel instead of just XSetPixel and XGetPixel.
 *		Add new class ReadWriteExpandedScanLine to make bitblitting more efficient. Partly implemented.
 *		Didnt start ReadOnlyExpandedScanLine, but it should be quite similar.
 *		Commented out assertion and added simple debugmessage in PixelMapFromData() since our code to write out
 *		PixelMaps in User triggers it. This code needs lots of work - still have not decided what I want to do.
 *		Must come to some desciion really soon since PixelMapEditor must write out stuff that actaully works by
 *		Friday!!.
 *
 *		Revision 1.35  92/04/20  14:15:39  14:15:39  lewis (Lewis Pringle)
 *		added typedef char* caddr_t conditional on qSnake && _POSIX_SOURCE, before include of Xutil.h, since that
 *		file references caddr_t which doesn't appear to be part of POSIX.
 *
 *		Revision 1.34  92/04/17  19:55:29  19:55:29  lewis (Lewis Pringle)
 *		Hack SnagOSImage to not really retrieve image -seems to crash sometimes, and not really needed
 *		for now, til we implemetned offscreen pixelmaps.
 *
 *		Revision 1.32  92/04/07  10:09:08  10:09:08  lewis (Lewis Pringle)
 *		Comment out failing line in invariant for mac - invarants need more work on
 *		all GDIs.
 *
 *		Revision 1.31  92/04/07  01:05:02  01:05:02  lewis (Lewis Pringle)
 *		Got rid of sterls hack.
 *		Worked on PixelMapFromData () to try to make it work with depth. Discovered Xlib does not support
 *		depth=2. Fixed XGDI bug where I set bits_per_pixel to 1<<depth - sb depth!!!!
 *
 *		Revision 1.30  92/04/06  15:07:37  15:07:37  sterling (Sterling Wight)
 *		hack to fix invariant
 *
 *		Revision 1.28  92/04/02  11:17:30  11:17:30  lewis (Lewis Pringle)
 *		Get rid of usage of BitBlit () proc - and defintiions. Use new BitBlitter class instead. Get rid
 *		of Colorize () find some more general mechanism. It was never well thgouth out.
 *		Cleanups.
 *		Really raise exceptions in GetColorLooupTable/Set if wrong type pixmap.
 *		Fixup SnagOSIamge (), and more importantly when its called - were very subtle bugs with this (X).
 *
 *		Revision 1.27  92/03/28  03:28:06  03:28:06  lewis (Lewis Pringle)
 *		Add X routine SnagOSImage to retrieve data from server. Stop ever deleting
 *		base address in GetOSPixmap so we dont need to recreate it in SnagOSImage, or
 *		GetOSImage. Make GetOSImage simple accessor, and use SnagOSImage before returning
 *		base address in GetBaseAddress - not GetBaseAddress_ still is just an accessor. This should allow
 *		for rouines like GetPixel () to work properly since they go thru GetBaseAddress.
 *
 *		Revision 1.26  1992/03/26  18:37:46  lewis
 *		Added a few invariant calls and under motif temporarily got rid of invariant that baseAddr != Nil.
 *
 *		Revision 1.25  1992/03/22  16:55:05  lewis
 *		Added notes on mac profiling to 'TODO' and fixed compile bug on mac.
 *
 *		Revision 1.23  1992/03/22  10:22:27  lewis
 *		Got rid of arg to GetOSPixmap - use global gDisplay instead. Also, get rid of correspoding
 *		field in PixelMap. Worked on XGetImage/XPutImage code - though it was working, but apparently
 *		never exercised enuf - make printouts and examine when we delete what more carefully.
 *
 *		Revision 1.22  1992/03/17  06:29:18  lewis
 *		Change PixelMapFromData () to assume rounding up to 1 byte - not 2 byte boundaries. Still needs some work
 *		deciding on a format...
 *
 *		Revision 1.21  1992/03/17  03:00:18  lewis
 *		Changed arg for PixelMapFromData () to const UInt8*.
 *
 *		Revision 1.20  1992/02/12  03:45:25  lewis
 *		Use GDIConfig instead of OSConfig, and get rid of some unused crap.
 *
 *		Revision 1.19  1992/02/11  00:27:40  lewis
 *		PixelMapFromData() not does quicker/better hack just for mac.
 *
 *		Revision 1.18  1992/01/28  10:40:19  lewis
 *		Added new PixelMapFromData.
 *		Lots of cleanups from while I was trying to fix bug with creating PixelMap in Emily (turned out to be
 *		reversed parameters to ::XPutPixel()), and of course, fixed that bug.
 *
 */

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
extern	"C"	void	_XInitImageFuncPtrs (struct _XImage* image);		// actually documented in O'reilly (page?)
#endif	/*GDI*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"Format.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"

#include	"PixelMap.hh"





#if 	qMacGDI
/*
 * Redo this code so we can always make either a new or old representation and
 * keep others in sync. If we cvate tcall a gettern, that triggers building that rep.
 *
 * I think that is the best way to get stuff to work most transparently
 * for modules outside of this (ie most generic assumptions interface can overide to outside
 * modules.
 *
 * other code like bitblit and tile can continue to use only one rep or the other depending on
 * if color QD avail, til we fix them later....
 */

inline	void	ClobberCTable (osColorTable** osCTab)
	{
		RequireNotNil (osCTab);
		if (GDIConfiguration ().ColorQD32Available ()) {
			::DisposeCTable (osCTab);
			Assert (MemError () == 0);
			Assert (QDError () == 0);
		}
		else {
			::DisposHandle (osHandle (osCTab));
			Assert (MemError () == noErr);
		}
	}
#endif	/*qMacGDI*/



#if		qXGDI
extern	"C"	int	_XGetBitsPerPixel (osDisplay* dpy, int depth);		// not documented, but we have the source in X11R4, and
																	// its simple - if it disapears, just copy source here...
#endif






#if		qMacGDI
// I probably should be calling CTabChanged instead of xxx = ::GetCTSeed ()!!!!
// But must get latest docs, and re-read...

/*
 * The reference for the magic in this code from Mac Technote 120, about Offscreen PixMaps, etc...
 *
 *	Stream Of Concience Design Document:
 *
 *		First, I think a mistake I've made in the past is confusing the ::SetOrigin() stuff
 *		with the pixmap bounds, and the magic QD does on a ::SetOrigin() to the portRect, and
 *		the portPixMap.bounds.  Now I think I've got it.
 *
 *		Our bounds NEED NOT be zero-zero based, but cannot be NEGATIVE!!!
 *
 *		We window into a region of memory with PixMaps.  The most common case, is that we
 *		window in at 0, 0, but can otherwise window into subsets, and so need PixMap ctors
 *		that let us subwindow (terminology from Cognex - thanx).
 */
#endif








/*
 ********************************************************************************
 *********************************** PixelMap ***********************************
 ********************************************************************************
 */

Exception	PixelMap::sNoColorLookupTable;

PixelMap::PixelMap ():
	fShouldDisposeOfBaseAddr (False),
	fCLUT (kBlackAndWhiteCLUT)
#if		qMacGDI
	,fOSBitMapRepresentation (Nil),
	fOSPixMapRepresentation (Nil)
#elif	qXGDI
	,fOrigin (kZeroPoint),
	fOSImage (Nil),
	fOSPixmap (Nil)
#endif	/*qMacGDI*/
#if		qDebug
	,
	fParentPixelMap (Nil),	// Fields to be sure subwindows are destroyed before we are.
	fNextPixelMap (Nil),
	fSubPixelMaps (Nil)
#endif	/*qDebug*/
{
	Try {
		MakeInitialOSPixMap (eChunky, 1);
		SetBounds (kZeroRect);
		SetRowBytes (0);
		ReAllocatePixelMemory ();		// or should it be Nil???
	}
	Catch () {
#if		qMacGDI
		if (fOSBitMapRepresentation != Nil) {
			// delete bit/pixmap structire
		}
#endif	/*qMacGDI*/
		_this_catch_->Raise ();
	}
	AssertNotNil (GetBaseAddress ());

	SetColorLookupTable (kBlackAndWhiteCLUT);

	Ensure (GetDepth_ () == 1);
	Invariant ();
}

PixelMap::PixelMap (const PixelMap& pixelMap):
	fShouldDisposeOfBaseAddr (False),
	fCLUT (kBlackAndWhiteCLUT)
#if		qMacGDI
	,fOSBitMapRepresentation (Nil),
	fOSPixMapRepresentation (Nil)
#elif	qXGDI
	,fOrigin (kZeroPoint),
	fOSImage (Nil),
	fOSPixmap (Nil)
#endif	/*qMacGDI*/
#if		qDebug
	,
	fParentPixelMap (Nil),	// Fields to be sure subwindows are destroyed before we are.
	fNextPixelMap (Nil),
	fSubPixelMaps (Nil)
#endif	/*qDebug*/
{
	Try {
		MakeInitialOSPixMap (eChunky, 1);		// override choice later

		/*
		 * Interesting choice here when copying pixelmaps to set our new bounds to be the old, rather
		 * than just the SIZE of the old (which would be more space efficient). This may wast some memory, but
		 * but is more like to do hwat the user gueggested in copying the pixelmap. He can
		 * easily make a new pixelmap (HOW!???) of the approparete size, and use BitBlit (NOT QUITE)
		 * to copy just the part he wants.
		 */
		SetBounds (pixelMap.GetBounds ());
		SetDepth (pixelMap.GetDepth ());
		ReCalcRowBytes ();
		ReAllocatePixelMemory ();
	}
	Catch () {
		ReleasePixelMapStorage ();
		_this_catch_->Raise ();
	}

	SetColorLookupTable (pixelMap.fCLUT);

	Assert (pixelMap.GetBounds ().GetOrigin () == kZeroPoint);		// otherwise my temp hack wont work...
	Assert (pixelMap.GetDepth () == GetDepth ());
	memcpy (GetBaseAddress (), pixelMap.GetBaseAddress (), GetImageSize ());

	Invariant ();
}


#if		qMacGDI
PixelMap::PixelMap (osBitMap* anOSRepresentation):
	fShouldDisposeOfBaseAddr (False),
	fCLUT (kBlackAndWhiteCLUT),
	fOSBitMapRepresentation (anOSRepresentation)
#if		qDebug
	,
	fParentPixelMap (Nil),	// Fields to be sure subwindows are destroyed before we are.
	fNextPixelMap (Nil),
	fSubPixelMaps (Nil)
#endif	/*qDebug*/
{
	Invariant ();
}

PixelMap::PixelMap (osPixMap** anOSRepresentation):
	fShouldDisposeOfBaseAddr (False),
	fCLUT (kBlackAndWhiteCLUT),
	fOSBitMapRepresentation (Nil),
	fOSPixMapRepresentation (anOSRepresentation)
#if		qDebug
	,
	fParentPixelMap (Nil),	// Fields to be sure subwindows are destroyed before we are.
	fNextPixelMap (Nil),
	fSubPixelMaps (Nil)
#endif	/*qDebug*/
{
	// The only thing wrong here, I believe, is that we dont properly set the
	// fClut field to reflect the given pixmaps clut
	AssertNotImplemented ();
	Invariant ();
}
#endif	/*qMacGDI*/

PixelMap::PixelMap (const Point& size, void* baseAddress, size_t rowBytes):
	fShouldDisposeOfBaseAddr (False),
	fCLUT (kBlackAndWhiteCLUT)
#if		qMacGDI
	,fOSBitMapRepresentation (Nil),
	fOSPixMapRepresentation (Nil)
#elif	qXGDI
	,fOrigin (kZeroPoint),
	fOSImage (Nil),
	fOSPixmap (Nil)
#endif	/*qMacGDI*/
#if		qDebug
	,
	fParentPixelMap (Nil),	// Fields to be sure subwindows are destroyed before we are.
	fNextPixelMap (Nil),
	fSubPixelMaps (Nil)
#endif	/*qDebug*/
{
	Try {
		MakeInitialOSPixMap (eChunky, 1);
		SetBounds (Rect (kZeroPoint, size));
		if (rowBytes == UInt32 (-1)) {
			ReCalcRowBytes ();
		}
		else {
			SetRowBytes (rowBytes);
		}
		if (baseAddress == Nil) {
			ReAllocatePixelMemory ();
		}
		else {
			SetBaseAddress (baseAddress);
		}
	}
	Catch () {
#if		qMacGDI
		if (fOSBitMapRepresentation != Nil) {
			// delete bit/pixmap structire
		}
#endif	/*qMacGDI*/
		_this_catch_->Raise ();
	}
	AssertNotNil (GetBaseAddress ());

	SetColorLookupTable (kBlackAndWhiteCLUT);

	Ensure (GetDepth_ () == 1);
	Invariant ();
}

PixelMap::PixelMap (const Rect& boundsRect, void* baseAddress, size_t rowBytes):
	fShouldDisposeOfBaseAddr (False),
	fCLUT (kBlackAndWhiteCLUT)
#if		qMacGDI
	,fOSBitMapRepresentation (Nil),
	fOSPixMapRepresentation (Nil)
#elif	qXGDI
	,fOrigin (kZeroPoint),
	fOSImage (Nil),
	fOSPixmap (Nil)
#endif	/*qMacGDI*/
#if		qDebug
	,
	fParentPixelMap (Nil),	// Fields to be sure subwindows are destroyed before we are.
	fNextPixelMap (Nil),
	fSubPixelMaps (Nil)
#endif	/*qDebug*/
{
	Try {
		MakeInitialOSPixMap (eChunky, 1);
		SetBounds (boundsRect);
		if (rowBytes == UInt32 (-1)) {
			ReCalcRowBytes ();
		}
		else {
			SetRowBytes (rowBytes);
		}
		if (baseAddress == Nil) {
			ReAllocatePixelMemory ();
		}
		else {
			SetBaseAddress (baseAddress);
		}
	}
	Catch () {
#if		qMacGDI
		if (fOSBitMapRepresentation != Nil) {
			// delete bit/pixmap structire
		}
#endif	/*qMacGDI*/
		_this_catch_->Raise ();
	}
	AssertNotNil (GetBaseAddress ());

	SetColorLookupTable (kBlackAndWhiteCLUT);

	Ensure (GetDepth_ () == 1);
	Invariant ();
}

PixelMap::PixelMap (PixelStyle pixelStyle, UInt8 depth, const Point& size, void* baseAddress, size_t rowBytes):
	fShouldDisposeOfBaseAddr (False),
	fCLUT (kBlackAndWhiteCLUT)
#if		qMacGDI
	,fOSBitMapRepresentation (Nil),
	fOSPixMapRepresentation (Nil)
#elif	qXGDI
	,fOrigin (kZeroPoint),
	fOSImage (Nil),
	fOSPixmap (Nil)
#endif	/*qMacGDI*/
#if		qDebug
	,
	fParentPixelMap (Nil),	// Fields to be sure subwindows are destroyed before we are.
	fNextPixelMap (Nil),
	fSubPixelMaps (Nil)
#endif	/*qDebug*/
{
	Try {
		MakeInitialOSPixMap (eChunky, depth);
		SetBounds (Rect (kZeroPoint, size));
		if (rowBytes == UInt32 (-1)) {
			ReCalcRowBytes ();
		}
		else {
			SetRowBytes (rowBytes);
		}
		if (baseAddress == Nil) {
			ReAllocatePixelMemory ();
		}
		else {
			SetBaseAddress (baseAddress);
		}
	}
	Catch () {
#if		qMacGDI
		if (fOSBitMapRepresentation != Nil) {
			// delete bit/pixmap structire
		}
#endif	/*qMacGDI*/
		_this_catch_->Raise ();
	}
	AssertNotNil (GetBaseAddress ());

	SetColorLookupTable (kBlackAndWhiteCLUT);

	Invariant ();
}

PixelMap::PixelMap (PixelStyle pixelStyle, UInt8 depth, const Rect& boundsRect, void* baseAddress, size_t rowBytes):
	fShouldDisposeOfBaseAddr (False),
	fCLUT (kBlackAndWhiteCLUT)
#if		qMacGDI
	,fOSBitMapRepresentation (Nil),
	fOSPixMapRepresentation (Nil)
#elif	qXGDI
	,fOrigin (kZeroPoint),
	fOSImage (Nil),
	fOSPixmap (Nil)
#endif	/*qMacGDI*/
#if		qDebug
	,
	fParentPixelMap (Nil),	// Fields to be sure subwindows are destroyed before we are.
	fNextPixelMap (Nil),
	fSubPixelMaps (Nil)
#endif	/*qDebug*/
{
	Try {
		MakeInitialOSPixMap (eChunky, depth);
		SetBounds (boundsRect);
		if (rowBytes == UInt32 (-1)) {
			ReCalcRowBytes ();
		}
		else {
			SetRowBytes (rowBytes);
		}
		if (baseAddress == Nil) {
			ReAllocatePixelMemory ();
		}
		else {
			SetBaseAddress (baseAddress);
		}
	}
	Catch () {
#if		qMacGDI
		if (fOSBitMapRepresentation != Nil) {
			// delete bit/pixmap structire
		}
#endif	/*qMacGDI*/
		_this_catch_->Raise ();
	}
	AssertNotNil (GetBaseAddress ());

	SetColorLookupTable (kBlackAndWhiteCLUT);

	Invariant ();
}

PixelMap::PixelMap (PixelStyle pixelStyle, UInt8 depth, const Point& size, void* baseAddress,
			const ColorLookupTable& clut, size_t rowBytes):
	fShouldDisposeOfBaseAddr (False),
	fCLUT (clut)
#if		qMacGDI
	,fOSBitMapRepresentation (Nil),
	fOSPixMapRepresentation (Nil)
#elif	qXGDI
	,fOrigin (kZeroPoint),
	fOSImage (Nil),
	fOSPixmap (Nil)
#endif	/*qMacGDI*/
#if		qDebug
	,
	fParentPixelMap (Nil),	// Fields to be sure subwindows are destroyed before we are.
	fNextPixelMap (Nil),
	fSubPixelMaps (Nil)
#endif	/*qDebug*/
{
	Try {
		MakeInitialOSPixMap (eChunky, depth);
		SetBounds (Rect (kZeroPoint, size));
		if (rowBytes == UInt32 (-1)) {
			ReCalcRowBytes ();
		}
		else {
			SetRowBytes (rowBytes);
		}
		if (baseAddress == Nil) {
			ReAllocatePixelMemory ();
		}
		else {
			SetBaseAddress (baseAddress);
		}
	}
	Catch () {
#if		qMacGDI
		if (fOSBitMapRepresentation != Nil) {
			// delete bit/pixmap structire
		}
#endif	/*qMacGDI*/
		_this_catch_->Raise ();
	}
	AssertNotNil (GetBaseAddress ());

	SetColorLookupTable (clut);

	Invariant ();
}

PixelMap::PixelMap (PixelStyle pixelStyle, UInt8 depth, const Rect& boundsRect, void* baseAddress,
			const ColorLookupTable& clut, size_t rowBytes):
	fShouldDisposeOfBaseAddr (False),
	fCLUT (clut)
#if		qMacGDI
	,fOSBitMapRepresentation (Nil),
	fOSPixMapRepresentation (Nil)
#elif	qXGDI
	,fOrigin (kZeroPoint),
	fOSImage (Nil),
	fOSPixmap (Nil)
#endif	/*qMacGDI*/
#if		qDebug
	,
	fParentPixelMap (Nil),	// Fields to be sure subwindows are destroyed before we are.
	fNextPixelMap (Nil),
	fSubPixelMaps (Nil)
#endif	/*qDebug*/
{
	Try {
		MakeInitialOSPixMap (eChunky, depth);
		SetBounds (boundsRect);
		if (rowBytes == UInt32 (-1)) {
			ReCalcRowBytes ();
		}
		else {
			SetRowBytes (rowBytes);
		}
		if (baseAddress == Nil) {
			ReAllocatePixelMemory ();
		}
		else {
			SetBaseAddress (baseAddress);
		}
	}
	Catch () {
#if		qMacGDI
		if (fOSBitMapRepresentation != Nil) {
			// delete bit/pixmap structire
		}
#endif	/*qMacGDI*/
		_this_catch_->Raise ();
	}
	AssertNotNil (GetBaseAddress ());

	SetColorLookupTable (clut);

	Invariant ();
}

PixelMap::~PixelMap ()
{
#if		qDebug
	/*
	 * Be sure all subwindows already destroy and update parent list so it knows we're gone.
	 */
	Require (fSubPixelMaps == Nil);
	if (fParentPixelMap != Nil) {
		/*
		 * remove me from parents list
		 */
		if (fParentPixelMap->fSubPixelMaps == this) {
			fParentPixelMap->fSubPixelMaps = fNextPixelMap;
		}
		else {
			for (PixelMap* cur = fParentPixelMap->fSubPixelMaps; cur != Nil; cur = cur->fNextPixelMap) {
				if (cur->fNextPixelMap == this) {
					cur->fNextPixelMap = fNextPixelMap;
					break;
				}
			}
			Assert (cur != Nil);		// otherwise we were not found in the list
		}
	}

#endif	/*qDebug*/

#if		qXGDI
	if (fOSPixmap != Nil) {
		extern	osDisplay*	gDisplay;
		AssertNotNil (gDisplay);
		::XFreePixmap (gDisplay, fOSPixmap);
		fOSPixmap = Nil;
	}
#endif

	ReleasePixelMapStorage ();

#if		qMacGDI
	if (fOSBitMapRepresentation != Nil) {
		delete (fOSBitMapRepresentation);
	}
	if (fOSPixMapRepresentation != Nil) {
		(*GetNewOSRepresentation ())->pmTable = Nil;	// to disable deletion??? Not really sure how to do this
													// we blew away whatever newpixmap created!
		::DisposePixMap (fOSPixMapRepresentation);
		Assert (MemError () == 0);
		Assert (QDError () == 0);
	}
#elif	qXGDI
	delete (fOSImage);		// dont use XCreateImage/XFreeImage since they dont do what
	  						// we want??
#endif	/*qMacGDI*/
}

const	PixelMap&	PixelMap::operator= (const PixelMap& rhs)
{
	Require (rhs.GetBounds ().GetOrigin () == kZeroPoint);	// subimages not yet supported...
	if (&rhs == this) {
		return (*this);
	}
#if		qXGDI
	if (fOSPixmap != Nil) {
		extern	osDisplay*	gDisplay;
		AssertNotNil (gDisplay);
		::XFreePixmap (gDisplay, fOSPixmap);
		fOSPixmap = Nil;
	}
#endif
	SetColorLookupTable (rhs.fCLUT);
	ReleasePixelMapStorage ();
	SetBounds (rhs.GetBounds ());
	SetRowBytes (rhs.GetRowBytes ());
	SetDepth (rhs.GetDepth ());
	ReAllocatePixelMemory ();

	memcpy (GetBaseAddress (), rhs.GetBaseAddress (), GetImageSize ());

	Invariant ();
	Ensure (fShouldDisposeOfBaseAddr);
	return (*this);
}

PixelMap::PixelStyle	PixelMap::GetPixelStyle () const
{
	return (eChunky);
}

void	PixelMap::SetPixelStyle (PixelStyle pixelStyle)
{
	AssertNotImplemented ();
}

UInt8	PixelMap::GetDepth () const
{
	Invariant ();
	return (GetDepth_ ());
}

void	PixelMap::SetDepth (UInt8 depth)
{
#if		qMacGDI
// not too sure about mac - check docs and do requires here...
#elif	qXGDI
	// these are all the XLib / X11R4 supports. Not documented anywhere, but true from reading the sources...
// maybe it is OK???
//	Require ((depth == 1) or (depth == 4) or (depth == 8) or (depth == 16) or (depth == 24) or (depth == 32));
#endif
	Invariant ();
	SetDepth_ (depth);
	Invariant ();
	Ensure (GetDepth_ () == depth);
}

Point	PixelMap::GetOrigin () const
{
	Invariant ();
#if		qMacGDI
	static	Boolean	kColorQD	=	GDIConfiguration ().ColorQDAvailable ();
	if (kColorQD) {
		return (os_cvt ((*GetNewOSRepresentation ())->bounds).GetOrigin ());
	}
	else {
		return (os_cvt ((GetOldOSRepresentation ())->bounds).GetOrigin ());
	}
#elif	qXGDI
	return (fOrigin);
#endif	/*qMacGDI*/
}

void	PixelMap::SetOrigin (const Point& origin)
{
	Invariant ();
#if		qMacGDI
	static	Boolean	kColorQD	=	GDIConfiguration ().ColorQDAvailable ();
	Require (origin >= kZeroPoint);	// dont think negative makes sense
Require (origin == kZeroPoint);	// subimages not yet supported...

	if (kColorQD) {
		os_cvt (Rect (origin, GetSize ()), (*GetNewOSRepresentation ())->bounds);
	}
	else {
		os_cvt (Rect (origin, GetSize ()), (GetOldOSRepresentation ())->bounds);
	}
#elif	qXGDI
	AssertNotNil (fOSImage);
	fOrigin = origin;
#endif	/*qMacGDI*/
	Invariant ();
}

Point	PixelMap::GetSize () const
{
	Invariant ();
#if		qMacGDI
	static	Boolean	kColorQD	=	GDIConfiguration ().ColorQDAvailable ();
	if (kColorQD) {
		return (os_cvt ((*GetNewOSRepresentation ())->bounds).GetSize ());
	}
	else {
		return (os_cvt ((GetOldOSRepresentation ())->bounds).GetSize ());
	}
#elif	qXGDI
	AssertNotNil (fOSImage);
	// we set as the image size the whole rect from zero-zero to botright of our rectangle
	// since X itself doesn't support images with non-zero origin.
	return (Point (fOSImage->height, fOSImage->width) - fOrigin);
#endif	/*qMacGDI*/
}

void	PixelMap::SetSize (const Point& size)
{
	Invariant ();
#if		qMacGDI
	static	Boolean	kColorQD	=	GDIConfiguration ().ColorQDAvailable ();
	Require (size >= kZeroPoint);	// dont think negative makes sense

	if (kColorQD) {
		os_cvt (Rect (GetOrigin (), size), (*GetNewOSRepresentation ())->bounds);
	}
	else {
		os_cvt (Rect (GetOrigin (), size), (GetOldOSRepresentation ())->bounds);
	}
#elif	qXGDI
	AssertNotNil (fOSImage);
	// we set as the image size the whole rect from zero-zero to botright of our rectangle
	// since X itself doesn't support images with non-zero origin.
	fOSImage->height = Rect (fOrigin, size).GetBottom ();
	fOSImage->width = Rect (fOrigin, size).GetRight ();
#endif	/*qMacGDI*/
	Invariant ();
}

Rect	PixelMap::GetBounds () const
{
	return (Rect (GetOrigin (), GetSize ()));
}

void	PixelMap::SetBounds (const Rect& boundsRect)
{
	Invariant ();
	SetOrigin (boundsRect.GetOrigin ());
	SetSize (boundsRect.GetSize ());
	Invariant ();
}

Boolean		PixelMap::Contains (const Point& p) const
{
	const	Point	kOneOne	=	Point (1, 1);
	return (Rect (GetBounds ().GetOrigin (), GetBounds ().GetSize()-kOneOne).Contains (p));
}

size_t	PixelMap::GetRowBytes () const
{
	Invariant ();
	return (GetRowBytes_ ());
}

void	PixelMap::SetRowBytes (size_t rowBytes)
{
	SetRowBytes_ (rowBytes);
#if		qMacGDI
	// on a mac, qd generally requires rowbytes to be multiple of two.  Use caution
	// in avoiding this rule, but sometimes we need to, like patterns.
	// MUST COME UP WITH BETTER WAY TO HANDLE THIS PROBLEM!!!
#endif	/*qMacGDI*/
	Invariant ();
	Ensure (GetRowBytes_ () == rowBytes);
}

void	PixelMap::ReCalcRowBytes ()
{
	Invariant ();
	SetRowBytes_ (CalcRowBytes (GetBounds (), GetDepth_ ()));
	Invariant ();
}

void*	PixelMap::GetBaseAddress () const
{
	Invariant ();
#if		qXGDI
	SnagOSImage ();
#endif
	void* baseAddress	=	GetBaseAddress_ ();
	EnsureNotNil (baseAddress);
	Invariant ();
	return (baseAddress);
}

void	PixelMap::SetBaseAddress (void* baseAddress)
{
	Invariant ();
#if		qXGDI
// Dont really want to do this - we need a routine called InvalXPixmap() - thats what we want to call here!!!
	SnagOSImage ();	//???? Maybe no point, but at least we must inval os pixmap, and this does that...
#endif
	void*	oldAddr = GetBaseAddress_ ();
	if (oldAddr != baseAddress) {
		ReleasePixelMapStorage ();
		fShouldDisposeOfBaseAddr = False;
		SetBaseAddress_ (baseAddress);
	}
	Invariant ();
	Ensure (GetBaseAddress () == baseAddress);
}

void	PixelMap::ReAllocatePixelMemory ()
{
	Invariant ();
	SetBaseAddress_ (new char [GetImageSize ()]);
	fShouldDisposeOfBaseAddr = True;
	Invariant ();
}

const ColorLookupTable&	PixelMap::GetColorLookupTable () const
{
	Invariant ();

	if (GetPixelStyle () != eChunky) {		// not sure only case???
		sNoColorLookupTable.Raise ();
	}

	return (fCLUT);
}

void	PixelMap::SetColorLookupTable (const ColorLookupTable& colorLookupTable)
{
	Invariant ();

	if (GetPixelStyle () != eChunky) {		// not sure only case???
		sNoColorLookupTable.Raise ();
	}

	fCLUT = colorLookupTable;
#if		qMacGDI
	if (GDIConfiguration ().ColorQDAvailable ()) {
		(*GetNewOSRepresentation ())->pmTable = (osColorTable**)fCLUT.PeekOSRepresention ();
		AssertNotNil ((*GetNewOSRepresentation ())->pmTable);
	}
#endif	/*qMacGDI*/
	Invariant ();
}

#if		!qCFront_NestedTypesHalfAssed
PixelMap::Pixel	PixelMap::GetPixel (const Point& p) const
#else
Pixel			PixelMap::GetPixel (const Point& p) const
#endif
{
	Require (Contains (p));
	Invariant ();
#if		qXGDI
	SnagOSImage ();
#endif
#if		qMacGDI || qXGDI
	void*	colAddr		=	ComputePixMapAddr (p);
	Pixel	result		=	0;
	switch (GetDepth ()) {
		case	1: {
			Coordinate	relCol		=	p.GetH () % 8;
			UInt8		pixValue	=	*((UInt8*)colAddr);
#if		qLeastSignicicantBitFirst
			UInt8	mask		=	(UInt8)	(1 << relCol);
#else	/*qLeastSignicicantBitFirst*/
			UInt8	mask		=	(UInt8)	(1 << (7 - relCol));
#endif	/*qLeastSignicicantBitFirst*/
			result = !!(pixValue & mask);		// say !! instead of shifting so we get right bit - hopefully thats cheaper?
#if		qXGDI
			Ensure (XGetPixel (GetOSImage (), (int)p.GetH (), (int)p.GetV ()) == result);
#endif
		}
		break;
		case	2: {
			Coordinate	relCol		=	p.GetH () % 4;
			UInt8		pixValue	=	*((UInt8*)colAddr);
#if		qLeastSignicicantBitFirst
			result = (pixValue >> (2*relCol)) & 0x03;
#else	/*qLeastSignicicantBitFirst*/
			result = (pixValue >> (6 - 2*relCol)) & 0x03;
#endif	/*qLeastSignicicantBitFirst*/
		}
		break;
		case	3: {
			Coordinate	relCol		=	p.GetH () % 2;
			UInt8		pixValue	=	*((UInt8*)colAddr);
			if (relCol == 0) {
#if		qLeastSignicicantBitFirst
				result = pixValue & 0x7;
#else	/*qLeastSignicicantBitFirst*/
				result = (pixValue>>4) & 0x7;
#endif	/*qLeastSignicicantBitFirst*/
			}
			else {
#if		qLeastSignicicantBitFirst
				result = (pixValue>>4) & 0x7;
#else	/*qLeastSignicicantBitFirst*/
				result = (pixValue & 0x7);
#endif	/*qLeastSignicicantBitFirst*/
			}
		}
		break;
		case	4: {
			Coordinate	relCol		=	p.GetH () % 2;
			UInt8		pixValue	=	*((UInt8*)colAddr);
			if (relCol == 0) {
#if		qLeastSignicicantBitFirst
				result = pixValue & 0xf;
#else	/*qLeastSignicicantBitFirst*/
				result = (pixValue>>4) & 0xf;
#endif	/*qLeastSignicicantBitFirst*/
			}
			else {
#if		qLeastSignicicantBitFirst
				result = (pixValue>>4) & 0xf;
#else	/*qLeastSignicicantBitFirst*/
				result = (pixValue & 0xf);
#endif	/*qLeastSignicicantBitFirst*/
			}
#if		qXGDI
			Ensure (XGetPixel (GetOSImage (), (int)p.GetH (), (int)p.GetV ()) == result);
#endif
		}
		break;
		case	5: {
			Coordinate	relCol		=	p.GetH ();
			UInt8		pixValue	=	*((UInt8*)colAddr);
#if		qLeastSignicicantBitFirst
			result = pixValue & 0x1f;
#else
			result = (pixValue >> 3);
#endif
			Ensure ((result & 0x1f) == result);
		}
		break;
		case	6: {
			Coordinate	relCol		=	p.GetH ();
			UInt8		pixValue	=	*((UInt8*)colAddr);
#if		qLeastSignicicantBitFirst
			result = pixValue & 0x3f;
#else
			result = (pixValue >> 2);
#endif
			Ensure ((result & 0x3f) == result);
		}
		break;
		case	7: {
			Coordinate	relCol		=	p.GetH ();
			UInt8		pixValue	=	*((UInt8*)colAddr);
#if		qLeastSignicicantBitFirst
			result = pixValue & 0x7f;
#else
			result = (pixValue >> 1);
#endif
			Ensure ((result & 0x7f) == result);
		}
		break;
		case	8: {
			result = (*((UInt8*)colAddr));
#if		qXGDI
			Ensure (XGetPixel (GetOSImage (), (int)p.GetH (), (int)p.GetV ()) == result);
#endif
		}
		break;
		default: {
			AssertNotImplemented ();
		}
		break;
	}
	return (result);
#endif	/*GDI*/
}

void	PixelMap::SetPixel (const Point& p, Pixel pixel)
{
	Require (Contains (p));
	Require (p >= kZeroPoint);
	Require (pixel < (1 << GetDepth ()));		// be sure it will fit...

	Invariant ();
#if		qXGDI
	SnagOSImage ();
#endif
#if		qMacGDI || qXGDI
	register	void*	colAddr		=	ComputePixMapAddr (p);
	switch (GetDepth ()) {
		case	1: {
			Require (pixel == 0 or pixel == 1);
			Coordinate	relCol	=	p.GetH () % 8;
#if		qLeastSignicicantBitFirst
			UInt8	mask		=	(UInt8)	(1 << relCol);
#else
			UInt8	mask		=	(UInt8)	(1 << (7 - relCol));
#endif
			if (pixel) {
				*((UInt8*)colAddr) |= mask;
			}
			else {
				*((UInt8*)colAddr) &= ~mask;
			}
#if		qXGDI
			Ensure (XGetPixel (GetOSImage (), (int)p.GetH (), (int)p.GetV ()) == pixel);
#endif
		}
		break;
		case	2: {
			Require (pixel >= 0 and pixel <= 3);
			Coordinate	relCol	=	p.GetH () % 4;

			UInt8	newByte		=	0x3;		// clear out old 2 pixel range
#if		qLeastSignicicantBitFirst
			newByte <<= (2*relCol);
#else
			newByte <<= (2*(3-relCol));
#endif
			*((UInt8*)colAddr) &= ~newByte;

			if (pixel) {
				UInt8	newByte		=	UInt8 (pixel);		// now or in new value
#if		qLeastSignicicantBitFirst
				newByte <<= (2*relCol);
#else	/*qLeastSignicicantBitFirst*/
				newByte <<= (2*(3-relCol));
#endif	/*qLeastSignicicantBitFirst*/
				*((UInt8*)colAddr) |= newByte;
			}
		}
		break;
		case	3: {
			UInt8		pixValue	=	*((UInt8*)colAddr);
			Coordinate	relCol		=	p.GetH () % 2;
#if		qLeastSignicicantBitFirst
			if (relCol == 0) {
				*((UInt8*)colAddr) &= 0xf0;
				*((UInt8*)colAddr) |= UInt8 (pixel);
			}
			else {
				*((UInt8*)colAddr) &= 0x0f;
				*((UInt8*)colAddr) |= (UInt8 (pixel) << 4);
			}
#else
			if (relCol == 0) {
				*((UInt8*)colAddr) &= 0x0f;
				*((UInt8*)colAddr) |= (UInt8 (pixel) << 4);
			}
			else {
				*((UInt8*)colAddr) &= 0xf0;
				*((UInt8*)colAddr) |= UInt8 (pixel);
			}
#endif
		}
		break;
		case	4: {
			UInt8		pixValue	=	*((UInt8*)colAddr);
			Coordinate	relCol		=	p.GetH () % 2;
#if		qLeastSignicicantBitFirst
			if (relCol == 0) {
				*((UInt8*)colAddr) &= 0xf0;
				*((UInt8*)colAddr) |= UInt8 (pixel);
			}
			else {
				*((UInt8*)colAddr) &= 0x0f;
				*((UInt8*)colAddr) |= (UInt8 (pixel) << 4);
			}
#else
			if (relCol == 0) {
				*((UInt8*)colAddr) &= 0x0f;
				*((UInt8*)colAddr) |= (UInt8 (pixel) << 4);
			}
			else {
				*((UInt8*)colAddr) &= 0xf0;
				*((UInt8*)colAddr) |= UInt8 (pixel);
			}
#endif
#if		qXGDI
			Ensure (XGetPixel (GetOSImage (), (int)p.GetH (), (int)p.GetV ()) == pixel);
#endif
		}
		break;
		case	5: {
			Assert ((pixel & 0x1f) == pixel);
#if		qLeastSignicicantBitFirst
			*((UInt8*)colAddr) = UInt8 (pixel);
#else
			*((UInt8*)colAddr) = UInt8 (pixel << 3);
#endif
		}
		break;
		case	6: {
			Assert ((pixel & 0x3f) == pixel);
#if		qLeastSignicicantBitFirst
			*((UInt8*)colAddr) = UInt8 (pixel);
#else
			*((UInt8*)colAddr) = (UInt8 (pixel) << 2);
#endif
		}
		break;
		case	7: {
			Assert ((pixel & 0x7f) == pixel);
#if		qLeastSignicicantBitFirst
			*((UInt8*)colAddr) = UInt8 (pixel);
#else
			*((UInt8*)colAddr) = (UInt8 (pixel) << 1);
#endif
		}
		break;
		case	8: {
			*((UInt8*)colAddr) = UInt8 (pixel);
#if		qXGDI
			Ensure (XGetPixel (GetOSImage (), (int)p.GetH (), (int)p.GetV ()) == pixel);
#endif
		}
		break;
		default: {
#if		qXGDI
			XPutPixel (GetOSImage (), (int)p.GetH (), (int)p.GetV (), pixel);
#else
			AssertNotReached ();		// other sizes NYI
#endif
		}
		break;
	}
#endif	/*GDI*/
	Ensure (GetPixel (p) == pixel);
}

Color	PixelMap::GetColor (const Point& p) const
{
	Require (Contains (p));
	Invariant ();
Assert (GetPixelStyle () == eChunky);		// otherwise NYI
	Pixel	pixelValue	=	GetPixel (p);

	return (fCLUT[pixelValue+1]);			// pixel values are zero based - clut indexes 1 based
}

void	PixelMap::SetColor (const Point& p, const Color& c)
{
	Require (Contains (p));
	Invariant ();
Assert (GetPixelStyle () == eChunky);		// otherwise NYI
	SetPixel (p, fCLUT[c] - 1);				// pixel values are zero based - clut indexes 1 based
}

size_t	PixelMap::CalcRowBytes (const Rect& bounds, UInt8 depth)
{
#if		qMacGDI
	// Note: TN 120 says bounds.Width () cuz they are computing right size for their
	// offscreen pixmap, and we want to know actaul rowbytes in THIS pixmap.
	return ((((depth * bounds.GetRight ()) + 15) >> 4) << 1);		// TN 120 page ~4
#elif	qXGDI

// hack for hp since it only seems to like rowbytes multiple of 4???
//rowBytes = ((rowBytes + 3) >> 2) << 2;

	// hp seems to like this to be a multiple of 4, so round up...
#if 0
	size_t rowBytes = ((depth * bounds.GetRight () + 3) >> 2) << 2;
#endif
	size_t rowBytes;
	if (depth == 1) {
// cuz were XYPixmap/bitmap
		rowBytes = ((depth * bounds.GetRight () + 3) >> 2) << 2;
	}
	else {
// undocumented but required based on looking at code for X11R4 implementation XImUtil.c
// X requires depth be rounded up to 8. Then we round total up to mult of 4.
#define ROUNDUP(nbytes, pad) ((((nbytes) + ((pad) - 1)) / (pad)) * (pad))
		rowBytes = ((ROUNDUP (depth, 8) * bounds.GetRight () + 3) >> 2) << 2;
	}
	Ensure (rowBytes % 4 == 0);
	Ensure (rowBytes != 0);
	return (rowBytes);
#endif	/*qMacGDI*/
}

size_t	PixelMap::CalcImageSize (const Rect& bounds, UInt8 depth)
{
	return (CalcRowBytes (bounds, depth) * bounds.GetBottom ());
}

size_t	PixelMap::GetImageSize () const
{
	Invariant ();
	return (GetRowBytes_ () * GetBounds ().GetBottom ());
}

void	PixelMap::MakeSubWindow (PixelMap& fillIn, const Rect& subWindow) const
{
	Invariant ();
// call that ctor and insert in subwindow list, if debug on...
	AssertNotImplemented ();

//	fillIn = PixelMap (GetDepth (), Get);
//PixelMap::PixelMap (UInt8 depth, const Rect& boundsRect, void* baseAddress,
//			ColorLookupTable* clut, size_t rowBytes):
}

UInt8	PixelMap::GetDepth_ () const
{
	/*
	 * low level accessor which give us gdi independence without the checking for valid values (and invariants)
	 */
#if		qMacGDI
	static	Boolean	kColorQD	=	GDIConfiguration ().ColorQDAvailable ();
	if (kColorQD) {
		// if we did not alloc base address, then it may have changed out from under us.
		// OH, we need a CTOR that takes a PixMapHandle!!!!!!
		// anyway, we may need to peek at that.
		return ((*GetNewOSRepresentation ())->pixelSize);
	}
	else {
		return (1);
	}
#elif	qXGDI
	AssertNotNil (fOSImage);
	return (fOSImage->depth);
#endif	/*qMacGDI*/
}

void	PixelMap::SetDepth_ (UInt8 depth)
{
	/*
	 * low level accessor which give us gdi independence without the checking for valid values (and invariants)
	 */
#if		qMacGDI
	static	Boolean	kColorQD	=	GDIConfiguration ().ColorQDAvailable ();
	Assert (kColorQD);		// NYI otherwise...
	Require (depth <= 32);
	(*GetNewOSRepresentation ())->pixelSize = depth;

// different if not chunky!!!
	Assert ((*GetNewOSRepresentation ())->pixelType == chunky);
	(*GetNewOSRepresentation ())->cmpSize = depth;
#elif	qXGDI
	AssertNotNil (fOSImage);

	fOSImage->depth = depth;
	switch (depth) {
		case	1:	fOSImage->bits_per_pixel = 1; break;
		case	2:	fOSImage->bits_per_pixel = 2; break;		// X doesn't support 2, SB 4
		case	3:	fOSImage->bits_per_pixel = 4; break;
		case	4:	fOSImage->bits_per_pixel = 4; break;
		case	5:	fOSImage->bits_per_pixel = 8; break;
		case	6:	fOSImage->bits_per_pixel = 8; break;
		case	7:	fOSImage->bits_per_pixel = 8; break;
		case	8:	fOSImage->bits_per_pixel = 8; break;
		default:	AssertNotReached ();
	}

if (depth <= 8) {
fOSImage->bitmap_unit = 8;
}


//	Assert ((depth == 1) or (fOSImage->format == ZPixmap));		// just cuz we dont support otherwise... Should someday...?
	if (depth == 1) {
		// this maybe assumes black and white clut - not sure yet ????
		fOSImage->format = XYBitmap;
	}
	else {
		switch (GetPixelStyle ()) {
			case	eChunky:	fOSImage->format = ZPixmap;  break;
			case	ePlanar:	fOSImage->format = XYPixmap; break;
			case	eDirect:	fOSImage->format = ZPixmap; break;
			default:			AssertNotReached (); break;
		}
	}

	_XInitImageFuncPtrs (fOSImage);			// necessary to reset function pointers
#endif	/*qMacGDI*/
}

size_t	PixelMap::GetRowBytes_ () const
{
	/*
	 * low level accessor which give us gdi independence without the checking for valid values (and invariants)
	 */
#if		qMacGDI
	static	Boolean	kColorQD	=	GDIConfiguration ().ColorQDAvailable ();
	if (kColorQD) {
		Ensure ((*GetNewOSRepresentation ())->rowBytes & UInt16 (0x8000));	// be sure bits set => color pixmap
		return ((*GetNewOSRepresentation ())->rowBytes & UInt16 (~0x8000));	// and off flag bits
	}
	else {
		// should ensure small enuf.... to be reall...
		return ((GetOldOSRepresentation ())->rowBytes);
	}
#elif	qXGDI
	AssertNotNil (fOSImage);
	return (fOSImage->bytes_per_line);
#endif	/*qMacGDI*/
}

void	PixelMap::SetRowBytes_ (size_t rowBytes)
{
	/*
	 * low level accessor which give us gdi independence without the checking for valid values (and invariants)
	 */
#if		qMacGDI
	static	Boolean	kColorQD	=	GDIConfiguration ().ColorQDAvailable ();
	// on a mac, qd generally requires rowbytes to be multiple of two.  Use caution
	// in avoiding this rule, but sometimes we need to, like patterns.
	// MUST COME UP WITH BETTER WAY TO HANDLE THIS PROBLEM!!!

	if (kColorQD) {
		(*GetNewOSRepresentation ())->rowBytes = rowBytes | 0x8000;	// or in flag bits
	}
	else {
		GetOldOSRepresentation ()->rowBytes = rowBytes;
	}
#elif	qXGDI
	AssertNotNil (fOSImage);
	fOSImage->bytes_per_line = rowBytes;
	_XInitImageFuncPtrs (fOSImage);			// necessary to reset function pointers
#endif	/*GDI*/
}

void*	PixelMap::GetBaseAddress_ () const
{
	/*
	 * low level accessor which give us gdi independence without the checking for valid values (and invariants)
	 */
#if		qMacGDI
	static	Boolean	kColorQD	=	GDIConfiguration ().ColorQDAvailable ();
	if (kColorQD) {
		return ((*GetNewOSRepresentation ())->baseAddr);
	}
	else {
		return (GetOldOSRepresentation ()->baseAddr);
	}
#elif	qXGDI
	AssertNotNil (fOSImage);
	return (fOSImage->data);
#endif	/*qMacGDI*/
}

void	PixelMap::SetBaseAddress_ (void* baseAddress)
{
	/*
	 * low level accessor which give us gdi independence without the checking for valid values (and invariants)
	 */
#if		qMacGDI
	static	const	Boolean	kHasColorQD	=	GDIConfiguration ().ColorQDAvailable ();

	// dont call GetBaseAddress () cuz could be Nil at this point...
	void*	oldAddr	=	kHasColorQD? (*GetNewOSRepresentation ())->baseAddr: GetOldOSRepresentation ()->baseAddr;

	if (kHasColorQD) {
		(*GetNewOSRepresentation ())->baseAddr = osPtr (baseAddress);
	}
	else {
		GetOldOSRepresentation ()->baseAddr = osPtr (baseAddress);
	}
#elif	qXGDI
	AssertNotNil (fOSImage);
	fOSImage->data = (char*)baseAddress;
#endif	/*qMacGDI*/
}

#if		qXGDI
void	PixelMap::SnagOSImage () const
{
	extern	osDisplay*	gDisplay;
	RequireNotNil (fOSImage);
	if (fOSPixmap != Nil) {
		/*
		 * This method is conceptually const, but since it keeps cached values, we must
		 * get around its constness.
		 */
		PixelMap*	non_const_this	=	this;

		AssertNotNil (gDisplay);	// cuz it was needed to make the os pixmap to begin with...

// Since this appears to be buggy, disable for now --- Not sure we ever take advantage of it anyhow!!!
// Only when we do offscreen pixelmaps...
#if 0
		// copy values back
		// adjust format if bitmap, cuz api for this routine says to pass ONLY XYPixmap or ZPixmap -- Orielly II-225
		int	format = (fOSImage->format==XYBitmap)? XYPixmap: ZPixmap;
		osImage* im = ::XGetImage (gDisplay, fOSPixmap, 0, 0, fOSImage->width, fOSImage->height, AllPlanes, format);
		AssertNotNil (im);
		// copy values and free one we alloced


		XImage newImage = *im;
		newImage.data = fOSImage->data;				// keep our old data pointer
		newImage.obdata = fOSImage->obdata;			// not too sure about this???
		newImage.format = fOSImage->format;			// keep our old format - sb same except in XYBitmap case above

		*fOSImage = newImage;


		// now we must copy bytes of image, before destroying it..
		memcpy (fOSImage->data, im->data, GetImageSize ());

		XDestroyImage (im);	// macro so cannon use scope operator...
#endif

		::XFreePixmap (gDisplay, fOSPixmap);
		non_const_this->fOSPixmap = Nil;
	}
	EnsureNotNil (fOSImage);
	Ensure (fOSPixmap == Nil);
}
#endif	/*GDI*/

void	PixelMap::ReleasePixelMapStorage ()
{
#if		qMacGDI
	static	const	Boolean	kHasColorQD	=	GDIConfiguration ().ColorQDAvailable ();

	// dont call GetBaseAddress () cuz could be Nil at this point...
	void*	oldAddr	=	kHasColorQD? (*GetNewOSRepresentation ())->baseAddr: GetOldOSRepresentation ()->baseAddr;
	if (fShouldDisposeOfBaseAddr and (oldAddr != Nil)) {
		delete (oldAddr);
		if (kHasColorQD) {
			(*GetNewOSRepresentation ())->baseAddr = Nil;
		}
		else {
			GetOldOSRepresentation ()->baseAddr = Nil;
		}
	}
#elif	qXGDI
	AssertNotNil (fOSImage);
	if (fShouldDisposeOfBaseAddr and (fOSImage->data != Nil)) {
		delete (fOSImage->data);
		fShouldDisposeOfBaseAddr = False;
		fOSImage->data = Nil;
	}
#endif	/*qMacGDI*/
}

void*	PixelMap::ComputePixMapAddr (const Point& p) const
{
Require (Rect (GetBounds ().GetOrigin (), GetBounds ().GetSize()-Point(1,1)).Contains (p));
	Invariant ();
// not quite right...
	Require (GetBounds ().Contains (p));
	RequireNotNil (GetBaseAddress ());

// not terribly confident in this - LGP Sept 20, 1990

// I think the rowAddr calc is right, but colAddr needs work - LGP May 9, 1992
	size_t	rowAddr		=	(size_t)GetBaseAddress () + (size_t)p.GetV () * GetRowBytes ();
	size_t	colAddr		=	rowAddr + (size_t)((p.GetH () * GetDepth ()) >> 3);	// only right since depth power of 2???

	return ((void*)colAddr);
}

#if		qMacGDI
osBitMap*	PixelMap::GetOldOSRepresentation () const
{
	Require (not GDIConfiguration ().ColorQDAvailable ());		// lift restriction!
	EnsureNotNil (fOSBitMapRepresentation);
	return (fOSBitMapRepresentation);
}

osPixMap**	PixelMap::GetNewOSRepresentation () const
{
	Require (GDIConfiguration ().ColorQDAvailable ());			// lift restriction!
	EnsureNotNil (fOSPixMapRepresentation);
	return (fOSPixMapRepresentation);
}
#elif	qXGDI
osImage*	PixelMap::GetOSImage () const
{
	EnsureNotNil (fOSImage);
	return (fOSImage);
}

unsigned long	PixelMap::GetOSPixmap () const
{
	extern	osDisplay*	gDisplay;
	RequireNotNil (gDisplay);
	if (fOSPixmap == Nil) {
		/*
		 * This method is conceptually const, but since it keeps cached values, we must
		 * get around its constness.
		 */
		PixelMap*	non_const_this	=	this;

		GC			gc;
		XGCValues	gcv;

		non_const_this->fOSPixmap = ::XCreatePixmap (gDisplay, DefaultRootWindow (gDisplay),
									 fOSImage->width, fOSImage->height, fOSImage->depth);
//		gcv.foreground = BlackPixel (gDisplay, DefaultScreen (gDisplay));
//		gcv.background = WhitePixel (gDisplay, DefaultScreen (gDisplay));
// hack for bitmaps - not sure what is right here...
		gcv.foreground = 1;
		gcv.background = 0;
		if (! (gc = ::XCreateGC (gDisplay, fOSPixmap, GCForeground | GCBackground, &gcv))) {
			AssertNotReached (); // must raise exception!!
		}
// put these into invariabt???
		Assert (fOSImage->xoffset == 0);		// would anything else make sense???
		Assert (fOSImage->depth!=1 or fOSImage->format == XYBitmap);
		::XPutImage (gDisplay, fOSPixmap, gc, fOSImage, 0, 0, 0, 0, fOSImage->width, fOSImage->height);
		::XFreeGC (gDisplay, gc);
	}
	return (fOSPixmap);
}
#endif	/*qMacGDI*/

void	PixelMap::MakeInitialOSPixMap (PixelStyle pixelStyle, UInt8 depth)
{
#if		qMacGDI
	if (GDIConfiguration ().ColorQDAvailable ()) {
		if ((fOSPixMapRepresentation = ::NewPixMap ()) == Nil) {
			gMemoryException.Raise ();
		}

		(*GetNewOSRepresentation ())->packType = 0;
		(*GetNewOSRepresentation ())->packSize = 0;

UInt8 cmpCount = 1;		// we must do better here!!!

// used to just always say chunky here - not sure I got names right since on UNIX box now... LGP
		switch (pixelStyle) {
			case	eChunky:	(*GetNewOSRepresentation ())->pixelType = chunky; break;
			case	ePlanar:	(*GetNewOSRepresentation ())->pixelType = planar; break;
			case	eDirect:	(*GetNewOSRepresentation ())->pixelType = chunky; break;			// is that right???
			default:			AssertNotReached (); break;
		}
		(*GetNewOSRepresentation ())->cmpCount = cmpCount;
		(*GetNewOSRepresentation ())->pixelSize = depth;
		(*GetNewOSRepresentation ())->cmpSize = depth/cmpCount;

		(*GetNewOSRepresentation ())->planeBytes = 0;
		(*GetNewOSRepresentation ())->pmReserved = 0;		// see IM-V-54
		(*GetNewOSRepresentation ())->baseAddr = Nil;

		//blow away the osPM clut, since we'll alloc our own
		// here be careful to do it after the CopyPixMap() since that expects a valid CLUT to copy into...
		AssertNotNil ((*fOSPixMapRepresentation)->pmTable);
		ClobberCTable ((*fOSPixMapRepresentation)->pmTable);
		(*fOSPixMapRepresentation)->pmTable = Nil;
	}
	else {
		fOSBitMapRepresentation = new osBitMap;
		(GetOldOSRepresentation ())->baseAddr = Nil;
	}
#elif	qXGDI

	// See SetDepth()
// I am no longer sure this is true ????
//	Require ((depth == 1) or (depth == 4) or (depth == 8) or (depth == 16) or (depth == 24) or (depth == 32));

	fOSImage = new osImage ();
	fOSImage->width = 0;						// size of image
	fOSImage->height = 0;
	fOSImage->xoffset = 0;						//  number of pixels offset in X direction

	//	XYBitmap, XYPixmap, ZPixmap
	if (depth == 1) {
		// this maybe assumes black and white clut - not sure yet ????
		fOSImage->format = XYBitmap;
	}
	else {
		switch (pixelStyle) {
			case	eChunky:	fOSImage->format = ZPixmap;  break;
			case	ePlanar:	fOSImage->format = XYPixmap; break;
			case	eDirect:	fOSImage->format = ZPixmap; break;
			default:			AssertNotReached (); break;
		}
	}
	fOSImage->data = Nil;						//	pointer to image data

	// Assume that all our images are in native bit/byte ordering...

#if		qLeastSignicicantByteFirst
	fOSImage->byte_order = LSBFirst;			//	data byte order - least significant byte first(see X-1-173)
#else
	fOSImage->byte_order = MSBFirst;			//	data byte order - most significant byte first(see X-1-173)
#endif

	fOSImage->bitmap_unit = 32;					//	quant. of scanline 8, 16, 32
												//  We use 32 since ??? we always will pass in longs, at least 32bits each as
												//	pixel values, even for single bits. Not too sure about meaning of this???
if (depth <= 8) {
	fOSImage->bitmap_unit = 8;
}


#if		qLeastSignicicantBitFirst
	fOSImage->bitmap_bit_order =	LSBFirst;
#else
	fOSImage->bitmap_bit_order =	MSBFirst;
#endif

	fOSImage->bitmap_pad = 32;					//	8, 16, 32 either XY or ZPixmap
												//  I believe this value is ignored, except in autocalculating the bytes_per_line
												//  for us, and since we calc that, it should not matter
	fOSImage->depth = depth;					//	depth of image
	fOSImage->bytes_per_line = 0;				//	accelarator to next line
//	fOSImage->bits_per_pixel = depth;			//	bits per pixel (ZPixmap)
	switch (depth) {
		case	1:	fOSImage->bits_per_pixel = 1; break;
		case	2:	fOSImage->bits_per_pixel = 2; break;		// X doesn't support 2, SB 4
		case	3:	fOSImage->bits_per_pixel = 4; break;
		case	4:	fOSImage->bits_per_pixel = 4; break;
		case	5:	fOSImage->bits_per_pixel = 8; break;
		case	6:	fOSImage->bits_per_pixel = 8; break;
		case	7:	fOSImage->bits_per_pixel = 8; break;
		case	8:	fOSImage->bits_per_pixel = 8; break;
		default:	AssertNotReached ();
	}
	fOSImage->red_mask = 0;						//	bits in z arrangment
	fOSImage->green_mask = 0;
	fOSImage->blue_mask = 0;
	fOSImage->obdata = Nil;						//	hook for the object routines to hang on


	_XInitImageFuncPtrs (fOSImage);
#endif	/*qXGDI*/
}


#if		qDebug
void	PixelMap::Invariant_ () const
{
#if		qMacGDI
#elif	qXGDI
#endif	/*qMacGDI || qXGDI*/
	Ensure (GetDepth_ () >= 1);
#if		0 && qMacGDI
// not quite right for x right now... or mac - should be though - work on it...
	EnsureNotNil (GetBaseAddress_ ());
#endif

// not right, but we currently should always be nil
Require (fSubPixelMaps == Nil);
Require (fParentPixelMap == Nil);
Require (fNextPixelMap == Nil);
}
#endif	/*qDebug*/







/*
 ********************************************************************************
 **************************** ReadWriteExpandedScanLine *************************
 ********************************************************************************
 */

ReadWriteExpandedScanLine::ReadWriteExpandedScanLine (PixelMap& pm, Coordinate row):
	fPixelMap (pm),
	fExpandedData (Nil),
	fWhichRow (row),
	fColumns (pm.GetBounds ().GetRight ()),
	fChunkSize (0)
{
	Assert (fPixelMap.GetBounds ().GetOrigin () == kZeroPoint);	// not implemented properly otherwise...
	Require (row >= 0);
	Require (row < fPixelMap.GetBounds ().GetBottom ());
	fChunkSize = (fPixelMap.GetDepth () + 7)/8;
	if (fChunkSize == 3) {
		fChunkSize = 4;
	}
	if (Peeking ()) {
#if		qXGDI
		fPixelMap.SnagOSImage ();		// be sure no image cached on server!!!
#endif
		fExpandedData = ((char*)fPixelMap.GetBaseAddress () + fPixelMap.GetRowBytes ()*row);
	}
	else {
// for now do this the slow way - later try hacks to make fast - like for bitmaps to bytes we can use 256 elt array
// of 8 byte arrays of answers!!!
		fExpandedData = new char [fColumns*fChunkSize];

		switch (fChunkSize) {
			case	1: {
				register	UInt8*	cursor	=	GetScanLine8 ();
				for (register Coordinate col = 0; col < fColumns; col++) {
					Assert (fPixelMap.GetPixel (Point (row, col)) == UInt8 (fPixelMap.GetPixel (Point (row, col))));	// no loss of precision
					*cursor++ = UInt8 (fPixelMap.GetPixel (Point (row, col)));
				}
			}
			break;
			case	2: {
				register	UInt16*	cursor	=	GetScanLine16 ();
				for (register Coordinate col = 0; col < fColumns; col++) {
					Assert (fPixelMap.GetPixel (Point (row, col)) == UInt16 (fPixelMap.GetPixel (Point (row, col))));	// no loss of precision
					*cursor++ = UInt16 (fPixelMap.GetPixel (Point (row, col)));
				}
			}
			break;
			case	4: {
				register	UInt32*	cursor	=	GetScanLine32 ();
				for (register Coordinate col = 0; col < fColumns; col++) {
					*cursor++ = UInt32 (fPixelMap.GetPixel (Point (row, col)));
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

ReadWriteExpandedScanLine::~ReadWriteExpandedScanLine ()
{
	if (Peeking ()) {
#if		qXGDI
		// were all done - we've been updating the right data structures all along - we win!!!
		Require (fPixelMap.fOSPixmap == Nil);	// assert not built while scanlines expanded!!! At least if it was,
												// we should blow it away - maybe we need some sort of lock flag?
#endif
	}
	else {
		// copy our array back - could be done much more efficiently - but hack for now...

		switch (fChunkSize) {
			case	1: {
				register	UInt8*	cursor	=	GetScanLine8 ();
				for (register Coordinate col = 0; col < fColumns; col++) {
					fPixelMap.SetPixel (Point (fWhichRow, col), *cursor++);
				}
			}
			break;
			case	2: {
				register	UInt16*	cursor	=	GetScanLine16 ();
				for (register Coordinate col = 0; col < fColumns; col++) {
					fPixelMap.SetPixel (Point (fWhichRow, col), *cursor++);
				}
			}
			break;
			case	4: {
				register	UInt32*	cursor	=	GetScanLine32 ();
				for (register Coordinate col = 0; col < fColumns; col++) {
					fPixelMap.SetPixel (Point (fWhichRow, col), *cursor++);
				}
			}
			break;
			default: {
				AssertNotReached ();
			}
			break;
		}

		delete (fExpandedData);
	}
}

UInt8*	ReadWriteExpandedScanLine::GetScanLine8 () const
{
	Require (fPixelMap.GetDepth () <= 8);
	Assert (fChunkSize == 1);
	return ((UInt8*)fExpandedData);
}

UInt16*	ReadWriteExpandedScanLine::GetScanLine16 () const
{
	Require (fPixelMap.GetDepth () >= 9);
	Require (fPixelMap.GetDepth () <= 16);
	Assert (fChunkSize == 2);
	return ((UInt16*)fExpandedData);
}

UInt32*	ReadWriteExpandedScanLine::GetScanLine32 () const
{
	Require (fPixelMap.GetDepth () >= 17);
	Require (fPixelMap.GetDepth () <= 32);
	Assert (fChunkSize == 4);
	return ((UInt32*)fExpandedData);
}

nonvirtual	Boolean	ReadWriteExpandedScanLine::Peeking () const
{
#if		!qCFront_NestedTypesHalfAssed
	PixelMap::Depth	d	=	fPixelMap.GetDepth ();
#else
	Depth			d	=	fPixelMap.GetDepth ();
#endif
	return (Boolean (d == 8 or d == 16 or d == 32));
}








/*
 ********************************************************************************
 **************************** PixelMapFromData **********************************
 ********************************************************************************
 */

PixelMap	PixelMapFromData (const Point& size, const UInt8* data, size_t nBytes)
{
	// construct a bitmap with the clut set implicitly to black and white...
	PixelMap		pm			=	PixelMap (size);
	const UInt8*	curLine		= 	data;
	for (Coordinate row = 0; row < size.GetV (); row++) {
		for (Coordinate col = 0; col < size.GetH (); col++) {
			UInt8	rightByte	=	curLine [col / 8];
			UInt8	mask		=	(UInt8)	(1 << (col % 8));
			pm.SetPixel (Point (row, col), !!(rightByte & mask));
		}
		curLine += (size.GetV () + 7)/8;		// assume tightest packing but round up to byte per scanline...
	}
	return (pm);
}

PixelMap	PixelMapFromData (const Point& size, const UInt8* data, size_t nBytes,
#if		!qCFront_NestedTypesHalfAssed
	PixelMap::Depth depth,
#else
	Depth depth,
#endif
	const Color clut[])
{
	PixelMap	pMap	=	PixelMap (PixelMap::eChunky, depth, Rect (kZeroPoint, size), Nil, ColorLookupTable (clut, 1 << depth));

	unsigned pixelsPerByte = 8/depth;
	if (pixelsPerByte > 1) {
		// Pack into bytes if depth small enuf
		const	UInt8*	curPtr	=	data;
		for (Coordinate row = 0; row < size.GetV (); row++) {
			UInt8		aByte 		=	0;
			unsigned	packedSoFar	=	0;
			for (Coordinate col = 0; col < size.GetH (); col++) {
				// 
				if (packedSoFar >= pixelsPerByte-1 or (col == 0)) {
					aByte	=	*curPtr++;
					pMap.SetPixel (Point (row, col), aByte % (1 << depth));		// only low order bits...
					aByte >>= depth;	// get next higher order bits into low part of byte
					packedSoFar = 0;
				}
				else {
					pMap.SetPixel (Point (row, col), aByte % (1 << depth));		// only low order bits...
					aByte >>= depth;	// get next higher order bits into low part of byte
					packedSoFar ++;
				}
			}
		}
		Require (curPtr == data + nBytes);		// be sure array they gave us right size - try to detect format errors...
	}
	else {
		// read in individual pixels... Stored as scanlines, pixels within a scanline seperated by spaces, and
		// stored as base 16 (without prefixing 0x) unsigned integers.
		const	UInt8*	curPtr	=	data;
Assert (depth == 8); 	// all we support so far...
		for (Coordinate row = 0; row < size.GetV (); row++) {
			for (Coordinate col = 0; col < size.GetH (); col++) {
#if		!qCFront_NestedTypesHalfAssed
				PixelMap::
#endif
				Pixel	pixel	=	*curPtr++;
				pMap.SetPixel (Point (row, col), pixel);
			}
		}
		Require (curPtr == data + nBytes);		// be sure array they gave us right size - try to detect format errors...
	}

	return (pMap);
}









/*
 ********************************************************************************
 **************************** iostream support **********************************
 ********************************************************************************
 */

static	Boolean	HasCLUT (const PixelMap& pm)
{
	Try {
		return (pm.GetColorLookupTable () != kBlackAndWhiteCLUT);
	}
	Catch () {
		return (False);
	}
}

ostream&	operator<< (ostream& out, const PixelMap& pixelMap)
{
	Rect	pmBounds = pixelMap.GetBounds ();
	out << lparen << int (pixelMap.GetDepth ()) << ' ' << pmBounds << ' ';

	if (HasCLUT (pixelMap)) {
		out << "C" << pixelMap.GetColorLookupTable ();
	}
	else {
		out << "N";
	}
	out << ' ' << pixelMap.GetRowBytes () << newline << tab;

	// New format write
	unsigned pixelsPerByte = 8/pixelMap.GetDepth ();
	if (pixelsPerByte > 1) {
		// Pack into bytes if depth small enuf
		for (Coordinate row = pmBounds.GetTop (); row < pmBounds.GetBottom (); row++) {
			out << tab;
			UInt8		aByte 		=	0;
			unsigned	packedSoFar	=	0;
			for (Coordinate col = pmBounds.GetLeft (); col < pmBounds.GetRight (); col++) {
#if		!qCFront_NestedTypesHalfAssed
				PixelMap::
#endif
				Pixel	pixel	=	pixelMap.GetPixel (Point (row, col));

				aByte += (pixel << (packedSoFar*pixelMap.GetDepth ()));
				packedSoFar ++;

				// when we've filled as much as possible into this byte, or were at the end of the
				// scanline, force out the byte.
				if (packedSoFar >= pixelsPerByte or (col == pmBounds.GetRight () - 1)) {
					out << ultostring (aByte, 16);
					if (col < pmBounds.GetRight () - 1) {
						out << " ";
					}
					packedSoFar = 0;
					aByte = 0;
				}

			}
			out << newline;
		}
	}
	else {
		// write out individual pixels...
		for (Coordinate row = pmBounds.GetTop (); row < pmBounds.GetBottom (); row++) {
			out << tab;
			for (Coordinate col = pmBounds.GetLeft (); col < pmBounds.GetRight (); col++) {
#if		!qCFront_NestedTypesHalfAssed
				PixelMap::
#endif
				Pixel	pixel	=	pixelMap.GetPixel (Point (row, col));

				Assert (sizeof (long) >= sizeof (pixel));
				out << ultostring (pixel, 16);
				if (col < pmBounds.GetRight () - 1) {
					out << " ";
				}
			}
			out << newline;
		}
	}

	out << tab << rparen;
	return (out);
}

istream&	operator>> (istream& in, PixelMap& pixelMap)
{
	char	ch;
	in >> ch;
	if (ch != lparen) {					// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	// Backward compat reading - used to be written as an unsigned char. LGP Jun 2, 1992
	Boolean	oldFormat = True;	// keep oldFormat flag based on depth so we know if we do ReadBytes () for
								// all the bytes of the image, or use new format stuff.
	UInt8 depth;
	in >> depth;
	if (depth > 8) {
		oldFormat = False;
		in.putback (depth);
		int	realDepth;
		in >> realDepth;		// this is really what should be done...
		depth = realDepth;
		if (realDepth > 32 or realDepth <= 0) {
			in.clear (ios::badbit | in.rdstate ());
			return (in);
		}
	}

	Rect	boundsRect	=	kZeroRect;
	in >> boundsRect;

	// check on clut - field is optionally, and marked with a C or an N
	char	clutChar;
	in >> clutChar;
	ColorLookupTable*	clut	=	Nil;
	switch (clutChar) {
		case	'C': {
			clut = new ColorLookupTable ();
			in >> *clut;
		}
		break;
		case	'N': {
			// nothing to read
		}
		break;
		default: {
			in.clear (ios::badbit | in.rdstate ());
			return (in);
		}
		break;
	}

	size_t rowBytes;
	in >> rowBytes;

	if (oldFormat) {
		void*	baseAddress		=	Nil;
		size_t	size			=	0;
		if (not (ReadBytes (in, size, baseAddress))) {
			delete (clut);
			in.clear (ios::badbit | in.rdstate ());
			Assert (baseAddress == Nil);
			return (in);
		}
		AssertNotNil (baseAddress);		// if ReadBytes () succeeded, it must have filled this in...

	//	if (size != ?? ) {ETC	- CHECK VALID SIZE FOR OTHER PARAMS...
	//		in.clear (ios::badbit | in.rdstate ());
	//		return (in);
	//	}


		in >> ch;
		if (!in or (ch != rparen)) {				// check for surrounding parens
			in.clear (ios::badbit | in.rdstate ());
			delete (clut);
			delete (baseAddress);
			return (in);
		}

		/*
		 * We win.
		 */
		Assert (in);
		// are we sure we want PixelMap::eChunky? maybe save that too?
		if (clut == Nil) {
			pixelMap = PixelMap (PixelMap::eChunky, depth, boundsRect, baseAddress, rowBytes);
		}
		else {
			pixelMap = PixelMap (PixelMap::eChunky, depth, boundsRect, baseAddress, *clut, rowBytes);
		}
		delete (clut);
		delete (baseAddress);
	}
	else {
		/*
		 * New format.
		 */


		Assert (in);
		// are we sure we want PixelMap::eChunky? maybe save that too?
		PixelMap*	tmpPixMap	=	Nil;
		if (clut == Nil) {
			tmpPixMap = new PixelMap (PixelMap::eChunky, depth, boundsRect, Nil);
		}
		else {
			tmpPixMap = new PixelMap (PixelMap::eChunky, depth, boundsRect, Nil, *clut);
		}
		delete (clut);

		unsigned pixelsPerByte = 8/depth;
		if (pixelsPerByte > 1) {
			// Pack into bytes if depth small enuf
			for (Coordinate row = boundsRect.GetTop (); row < boundsRect.GetBottom (); row++) {
				UInt8		aByte 		=	0;
				unsigned	packedSoFar	=	0;
				for (Coordinate col = boundsRect.GetLeft (); col < boundsRect.GetRight (); col++) {
					// 
					if (packedSoFar >= pixelsPerByte-1 or (col == boundsRect.GetLeft ())) {
						String	pixStr;

						in >> pixStr;
						aByte	=	strtoul (pixStr, 16);

						tmpPixMap->SetPixel (Point (row, col), aByte % (1 << depth));		// only low order bits...
						aByte >>= depth;	// get next higher order bits into low part of byte
						packedSoFar = 0;
					}
					else {
						tmpPixMap->SetPixel (Point (row, col), aByte % (1 << depth));		// only low order bits...
						aByte >>= depth;	// get next higher order bits into low part of byte
						packedSoFar ++;
					}
				}
			}
		}
		else {
			// read in individual pixels... Stored as scanlines, pixels within a scanline seperated by spaces, and
			// stored as base 16 (without prefixing 0x) unsigned integers.
			for (Coordinate row = boundsRect.GetTop (); row < boundsRect.GetBottom (); row++) {
				for (Coordinate col = boundsRect.GetLeft (); col < boundsRect.GetRight (); col++) {
					String	pixStr;
					in >> pixStr;

#if		!qCFront_NestedTypesHalfAssed
					PixelMap::
#endif
					Pixel	pixel	=	strtoul (pixStr, 16);

					tmpPixMap->SetPixel (Point (row, col), pixel);
				}
			}
		}

		in >> ch;
		if (!in or (ch != rparen)) {				// check for surrounding parens
			in.clear (ios::badbit | in.rdstate ());
			delete (tmpPixMap);
			return (in);
		}

		pixelMap = *tmpPixMap;
		delete (tmpPixMap);
	}

	return (in);
}





/*
 ********************************************************************************
 ************************************* operator== *******************************
 ********************************************************************************
 */
Boolean	operator== (const PixelMap& lhs, const PixelMap& rhs)
{
	if (lhs.GetPixelStyle () != rhs.GetPixelStyle ()) {
		return (False);
	}
	if (lhs.GetDepth () != rhs.GetDepth ()) {
		return (False);
	}
	if (lhs.GetBounds () != rhs.GetBounds ()) {
		return (False);
	}
	if (lhs.GetRowBytes () != rhs.GetRowBytes ()) {
		return (False);
	}

AssertNotReached ();
// Must do forloop and check 		nonvirtual	Pixel	GetPixel (const Point& p) const;
// For each pixel in the bounds. Cannot just do memcmp () except in lucky case where bounds
// is zero/zero. Actually, that is so common, we should test before trying the loop!!!

	return (True);
}





/*
 ********************************************************************************
 ************************************* operator!= *******************************
 ********************************************************************************
 */
Boolean	operator!= (const PixelMap& lhs, const PixelMap& rhs)
{
	return (not (lhs == rhs));
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***



