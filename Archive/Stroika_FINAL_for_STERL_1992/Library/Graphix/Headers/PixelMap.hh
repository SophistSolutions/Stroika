/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PixelMap__
#define	__PixelMap__

/*
 * $Header: /fuji/lewis/RCS/PixelMap.hh,v 1.8 1992/09/05 16:14:25 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *		-	Rethink PixelStyle stuff.
 *
 *		-	Use PixelPoint/PixelRect instead of Point/Rect.
 *
 *		-	Implement shared/reference counting.
 *
 *		-	For X, add getreadonly pixmap and make snag work properly...
 *
 *
 * Notes:
 *
 * A PixelMap is a two dimentional array of Pixels. These pixels can be either direct RGB
 * representations of colors, or indexes into a ColorLookupTable, or can be on/off bits for
 * a bitmap.
 *
 * Some of the various issues and features our design must deal with include:
 * inheritance, and perhaps not!!!
 *
 *		(1)		Direct, or Indirect data mapping. IE are pixels colors, or clut indexes?
 *		(2)		bitDepth (1 special case)
 *		(3)		boundsRect
 *		(4)		Must support situation where data is shared (ie wrapper on os struct), or our own.
 *		(5)		Must support "by value" semantics.
 *		(6)		Should support shared reference couting for efficient copying (not critical to do imediatly,
 *				since we do it in Tile).
 *		(7)		Must support subwindowing, and would be desirable with debug turned on to detect unsafe
 *				changes to base PixelMap when windows exist!!!
 *
 *	Parameters governing a PixelMap.
 *
 * 	(1)		Bit Depth, and other info telling how to map from Pixel to Color, like
 *			CLUT, direct mode, etc....
 *	(2)		A Base Address where pixel values can be found.
 *	(3)		A BoundsRect used to offset from base address.
 *
 *
 *
 * <Mac Implementation issues:>
 *
 *	mac pixmap has hRes/vRes in pixels per inch - do we want such a beast here?
 *
 *	NB:		Currently just support Chunky pixels...
 *
 *	Put resource stuff to create by resource into Framework level Icon class.
 *
 *
 *
 * <X Implementation issues:>
 *
 *		An X windows implementation of PixelMap poses several difficulties. First,
 * the two types in X analogous to a PixelMap are Pixmap, and XImage. Pixmap and
 * XImage under X differ primarily in that the Pixmap resides on the server, and
 * the XImage resides on the client. This has the obvious performance implications,
 * plus a not so obvious implication that only Pixmaps are considered drawables by X.
 * Thus, they are used as Tiles.
 *
 *		Another X feature is that you need a connection to a display, and a drawable
 * (presubably only to choose a ~visual") at the time of construction. These choices
 * are immutable for the lifetime of the Pixmap/XImage. Since in Stroika, we construct
 * many Pixelmaps as static objects before we've made our connection, it is clear that
 * we must keep some alternate representation of PixelMaps beyond the two X versions.
 *
 *		Equally obvious is that we must support the X "Pixmap" representation because of
 * its performance characteristics, because it is the only way to do fancy (non-stiple)
 * tiles, and perhaps a few other things (I'm still sketchy on this stuff so pardon
 * any flubs).
 *
 *		Not so obvious, but reasonably we might want to support the XImage representation
 * for things like offscreen bitmaps (in case the server doesnt have the memory!!!),
 * and for performance reasons while updating the Pixels thru client code (eg drawing
 * operations not supported by X).
 *
 *		So, my choice is to support only the above two X representations, and to use
 * a slight variant of the XImage representation for PixelMaps before we've made an
 * X connection or decided how we want to use this PixelMap.
 *
 *		The trick is to allocate our own XImage structure, and use its fields to keep
 * track of relevent information, and then keep a boolean on the side to indicate the
 * validity of the structure. Then, if someone asks for an XImage representation, we can
 * use our XImage structure to build a valid XImage structure for them.
 *
 *		We use a similar trick to build X Pixmaps. We have a GetOSPixMap () method that
 * checks if their is an un-to-date Pixmap made for this Stroika PixelMap, and builds one
 * if need be. This would seem to have all the flexabily you have come to expect from
 * Stroika, and still work under X.
 *
 *		One note about these GetOSPixMap () and GetOSImage () methods is that they
 * must take valid Display/Drawable/Visual (not sure which yet) parameters so we can
 * properly create the appropriate X structures. Also, this guarentees that we build
 * these things on a lazy (as needed) basis, and are always guarenteed to have the right
 * information around when we need it.
 *
 *		The other difficulty not thoroghly addressed is BitBlitting. X's support for this
 * seems woafully inadequate. We will have to provide at least some primitve support for
 * translating across CLUTs, and depths. For now, this support may have to be slow, and
 * clunky. Alas, and alak..
 *
 *		Another philosophical difference to keep in mind between the XGDI and mac is that
 * the mac color system works in RGB space, and X's in pixel space. Most of the logical
 * operators work on a pixel basis with no reference to the color values, and X provides
 * no arithmetic modes at all. We may get stuck implementing this stuff for compatability.
 * Ughh.
 *
 *
 * Changes:
 *	$Log: PixelMap.hh,v $
 *		Revision 1.8  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.7  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  05:45:04  lewis
 *		Made SnagOSImage (for X) const member.
 *
 *		Revision 1.5  1992/07/16  05:17:02  lewis
 *		Added SetOrigin/GetOrigin and SetSize/GetSize methods - defined in
 *		terms of how Get/SetBounds () works - no new functionality- just
 *		convienence.
 *
 *		Revision 1.4  1992/07/01  03:57:21  lewis
 *		Got rid of unneeded String.hh include.
 *
 *		Revision 1.3  1992/07/01  01:40:36  lewis
 *		Got rid of qNestedTypesAvailable flag - assume always true.
 *
 *		Revision 1.2  1992/06/25  16:21:32  lewis
 *		Fix problem with sloppy nested type usage - not caught/allowed by CFront21 so needs bug workaround flag.
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.26  1992/06/03  16:19:39  lewis
 *		Add overloaded CTORs for Point size instead of Rect boundsRect in two places (the ones with PixelStyle
 *		as the first arg).
 *		Also, get rid of XPixelMapFromData () -- no longer needed - our codegen fully portable - I hope.
 *
 *		Revision 1.25  92/05/18  16:45:30  16:45:30  lewis (Lewis Pringle)
 *		Cleanup/Add comments.
 *		Add no arg, and PixelMap (size, baseaddr, rowBytes) ctors.
 *		
 *		Revision 1.24  92/05/01  01:32:14  01:32:14  lewis (Lewis Pringle)
 *		Added XPixelMapFromData () routines as temp hack so we could get it working acceptably for X - soon
 *		we'll do portable new version.
 *		
 *		Revision 1.23  92/04/30  14:59:42  14:59:42  lewis (Lewis Pringle)
 *		Moved ReadWrite/RealOnly ExpandedScanlines out of scope of Pixelmap to work around HP
 *		CFront 2.1 compiler bug - couldnt say friend....
 *		
 *		Revision 1.22  92/04/29  12:16:45  12:16:45  lewis (Lewis Pringle)
 *		Made ComputePixMapAddr (const Point& p) const; private again, and added two new classes:
 *		ReadOnlyExpandedScanLine andReadWriteExpandedScanLine.
 *		These are intended to be only used in the bitblitter, or in places where people are VERY careful,
 *		but allow for VERY big optimiaztions when doing lots of bit operations - since then we dont need to
 *		redundently compute things.
 *		
 *		Revision 1.21  92/04/27  18:36:19  18:36:19  lewis (Lewis Pringle)
 *		Made Depth a nested typedef, and discovered cfront 2.1 basically ignores this. Oh well.
 *		Made ComputPixMapAddr() public - but never used it any place yet - not sure good idea - think more.
 *		Fixup some ambigous comments.
 *		
 *		Revision 1.19  92/04/07  10:58:37  10:58:37  lewis (Lewis Pringle)
 *		port to pc.
 *		
 *		Revision 1.18  92/04/02  11:11:38  11:11:38  lewis (Lewis Pringle)
 *		Get rid of Turnerize since never used, and never implemented properly, and probably a side
 *		effect of implementing some other functionality better. Also, got rid of BitBlit
 *		calls since they are not done by constructing a BitBlitter object.
 *		
 *		Revision 1.17  92/03/28  03:27:35  03:27:35  lewis (Lewis Pringle)
 *		Add SnagOSImage routine (only for X).
 *
 *		Revision 1.15  1992/03/17  03:04:11  lewis
 *		Chagned data arg to const UInt8*.
 *
 *		Revision 1.14  1992/01/28  10:38:59  lewis
 *		Add new PixelMapFromData () routines.
 *
 *
 *
 */

#include	"Exception.hh"

#include	"ColorLookupTable.hh"
#include	"Rect.hh"
#include	"Region.hh"




// add dither copy mode - not always available - but need enum for 32bit color qd.
// if used when not available, just use eCopyTMode.
enum	TransferMode	{
	eCopyTMode,
	eOrTMode,
	eXorTMode,
	eBicTMode,
	eNotCopyTMode,
	eNotOrTMode,
	eNotXorTMode,
	eNotBicTMode,

#if		qMacGDI
// APPLES COLOR MODES - NOT QUITE SURE HOW TO INTERPRET THESE ELSEWHERE - HACKEM IN FOR NOW
/* Arithmetic transfer modes */
	eGrayisTextOrTMode	=	49,			// only with system 7+??? (Color QD 2.0?)

	eBlendTMode			=	32,
	eAddPinTMode		=	33,
	eAddOverTMode		=	34,
	eSubPinTMode		=	35,
	eAddMaxTMode		=	37,
	eSubOverTMode		=	38,
	eAddMinTMode		=	39,
/* Transparent mode constant */
	eTransparentTMode	=	36,

	eHilightTMode		=	50,			//	needs some more investigation - how do it work???

	eDitherCopyTMode	=	64,
#endif	/*qMacGDI*/
};





#if		qMacGDI
struct osBitMap;
struct osPixMap;
#elif	qXGDI
struct	osDisplay;
struct	osImage;
#endif	/*GDI*/






class	PixelMap {
	public:
		typedef	UInt32	Pixel;		// should always fit in one of these...
		typedef	UInt8	Depth;
		enum PixelStyle { eChunky, ePlanar, eDirect };

		static	Exception	sNoColorLookupTable;

	public:
		/*
		 * Generate as small a pixelmap as possible (must see if 0x0 works - it should but not positive?).
		 * Such a pixelmap probably contains no bits, so it would be illegal to call getpixel. Useful primarily
		 * when we need to just declare the pixelmap, and we will asign to it later.
		 */
		PixelMap ();

		/*
		 * This Copy CTOR actaully copies all the bytes in the PixMap.  If the src PixelMap
		 * has a non-zero origin for its bounds, a PixMap with the same size, but zero-based bounds
		 * is created. This is because there is practically no value to a non-zero origin for
		 * a pixelmap unless your are subwindowing, and there is a huge memory cost, due to the
		 * layout of scanlines and the interprettation of a bounds.
		 *
		 * CLUTs are copied by value- not reference.
		 */
		PixelMap (const PixelMap& pixelMap);

#if		qMacGDI
		/*
		 * used for low level things like particular monitors, and the like.  Be very careful
		 * with these - be sure our cached values dont become out of sync.
		 * See GetOSRepresention () for description of type of this pointer...
		 */
		PixelMap (osBitMap* anOSRepresentation);
		PixelMap (osPixMap** anOSRepresentation);
#endif	/*qMacGDI*/

		/*
		 * This CTOR DOES NOT copy the data pointed to in Base Address.  If it is NonNil, the
		 * base address is actaully used.  If it is Nil, we allocate our own space.  Similarly
		 * for the SetBaseAddress method.  If we allocate the space, we free it on destruction.
		 * If the addr is handed to us, the we do not.
		 * Note, by default we calc rowbytes, but you can pass in a different one for
		 * a source image.
		 *
		 * This constructor is mainly for bitmaps - it is black and white by default...
		 */
		PixelMap (const Point& size, void* baseAddress = Nil, size_t rowBytes = size_t (-1));

		/*
		 * This CTOR DOES NOT copy the data pointed to in Base Address.  If it is NonNil, the
		 * base address is actaully used.  If it is Nil, we allocate our own space.  Similarly
		 * for the SetBaseAddress method.  If we allocate the space, we free it on destruction.
		 * If the addr is handed to us, the we do not.
		 * Note, by default we calc rowbytes, but you can pass in a different one for
		 * a source image.
		 *
		 * This constructor is mainly for bitmaps - it is black and white by default...
		 */
		PixelMap (const Rect& boundsRect, void* baseAddress = Nil, size_t rowBytes = size_t (-1));

		/*
		 * This CTOR DOES NOT copy the data pointed to in Base Address.  If it is NonNil, the
		 * base address is actaully used.  If it is Nil, we allocate our own space.  Similarly
		 * for the setBaseAddress methods.  If we allocate the space, we free it on destruction.
		 * If the addr is handed to us, the we do not.
		 * Note, by default we calc rowbytes, but you can pass in a different one for
		 * a source image.
		 */
		PixelMap (PixelStyle pixelStyle, Depth depth, const Point& size, void* baseAddress = Nil,
			size_t rowBytes = size_t (-1));

		/*
		 * This CTOR DOES NOT copy the data pointed to in Base Address.  If it is NonNil, the
		 * base address is actaully used.  If it is Nil, we allocate our own space.  Similarly
		 * for the setBaseAddress methods.  If we allocate the space, we free it on destruction.
		 * If the addr is handed to us, the we do not.
		 * Note, by default we calc rowbytes, but you can pass in a different one for
		 * a source image.
		 */
		PixelMap (PixelStyle pixelStyle, Depth depth, const Rect& boundsRect, void* baseAddress = Nil,
			size_t rowBytes = size_t (-1));

		/*
		 * This CTOR DOES NOT copy the data pointed to in Base Address.  If it is NonNil, the
		 * base address is actaully used.  If it is Nil, we allocate our own space.  Similarly
		 * for the setBaseAddress methods.  If we allocate the space, we free it on destruction.
		 * If the addr is handed to us, the we do not.
		 * Note, by default we calc rowbytes, but you can pass in a different one for
		 * a source image.
		 */
		PixelMap (PixelStyle pixelStyle, Depth depth, const Point& size, void* baseAddress,
			const ColorLookupTable& clut, size_t rowBytes = size_t (-1));

		/*
		 * This CTOR DOES NOT copy the data pointed to in Base Address.  If it is NonNil, the
		 * base address is actaully used.  If it is Nil, we allocate our own space.  Similarly
		 * for the setBaseAddress methods.  If we allocate the space, we free it on destruction.
		 * If the addr is handed to us, the we do not.
		 * Note, by default we calc rowbytes, but you can pass in a different one for
		 * a source image.
		 */
		PixelMap (PixelStyle pixelStyle, Depth depth, const Rect& boundsRect, void* baseAddress,
			const ColorLookupTable& clut, size_t rowBytes = size_t (-1));


		~PixelMap ();
		nonvirtual	const	PixelMap&	operator= (const PixelMap& rhs);


		/*
		 * This whole interface needs more thought - maybe no SetPixelStyle (PixelStyle pixelStyle),
		 * but rather SetChunky (), SetPlaner (), etc... and force params to be given then
		 * that specify needed components.
		 */
		nonvirtual	PixelStyle	GetPixelStyle () const;
		nonvirtual	void		SetPixelStyle (PixelStyle pixelStyle);	// don't use lightly!!!


		/*
		 * Sets the depth field of the PixelMap. Does not cause Pixels to be reallocated, nor
		 * does it cause rowbytes to be recalculated. These can be done manually, or automatically
		 * by calcing ReCalcRowBytes (), and then ReAllocatePixelMemmory ().
		 *
		 * SetDepth () should almost always be followed by ReCalcRowBytes () and ReAllocatePixelMemory ().
		 */
		nonvirtual	Depth		GetDepth () const;
		nonvirtual	void		SetDepth (Depth depth);


		/*
		 * Adjusts the bounds field of the PixelMap. Does not cause Pixels to be reallocated, nor
		 * does it cause rowbytes to be recalculated. These can be done manually, or automatically
		 * by calcing ReCalcRowBytes (), and then ReAllocatePixelMemmory ().
		 *
		 * SetOrigin () should almost always be followed by ReCalcRowBytes () and ReAllocatePixelMemory ().
		 */
		nonvirtual	Point		GetOrigin () const;
		nonvirtual	void		SetOrigin (const Point& origin);


		/*
		 * Adjusts the bounds field of the PixelMap. Does not cause Pixels to be reallocated, nor
		 * does it cause rowbytes to be recalculated. These can be done manually, or automatically
		 * by calcing ReCalcRowBytes (), and then ReAllocatePixelMemmory ().
		 *
		 * SetSize () should almost always be followed by ReCalcRowBytes () and ReAllocatePixelMemory ().
		 */
		nonvirtual	Point		GetSize () const;
		nonvirtual	void		SetSize (const Point& size);


		/*
		 * Sets the bounds field of the PixelMap. Does not cause Pixels to be reallocated, nor
		 * does it cause rowbytes to be recalculated. These can be done manually, or automatically
		 * by calcing ReCalcRowBytes (), and then ReAllocatePixelMemmory ().
		 *
		 * SetBounds () should almost always be followed by ReCalcRowBytes () and ReAllocatePixelMemory ().
		 */
		nonvirtual	Rect		GetBounds () const;
		nonvirtual	void		SetBounds (const Rect& boundsRect);


		/*
		 * Returns True iff the point p is within the bounds of the pixelmap. This returns True
		 * iff its OK to call Get/SetPixel of the particular point.
		 */
		nonvirtual	Boolean		Contains (const Point& p) const;


		/*
		 * Sets the rowBytes field of the PixelMap. It does not check for consistency with the bounds,
		 * nor the depth. It does nothing to the allocated pixel storage. To reallocate pixel storage,
		 * call ReAllocatePixelMemory (). SetRowBytes () should RARELY be called.
		 */
		nonvirtual	size_t		GetRowBytes () const;
		nonvirtual	void		SetRowBytes (size_t rowBytes);
		nonvirtual	void		ReCalcRowBytes ();


		/*
		 * SetBaseAddress () sets the base address of a pixelmap.  If caller gives us base address,
		 * he is reponsible for disposing of it.  Otherwise if ReAllocatePixelMemory () is used,
		 * we are.
		 */
		nonvirtual	void*		GetBaseAddress () const;
		nonvirtual	void		SetBaseAddress (void* baseAddress);
		nonvirtual	void		ReAllocatePixelMemory ();


		/*
		 * SetColorLookupTable () copies by value the given ColorLookupTable for use in this
		 * PixelMap for interpretting the color values of our pixels. It will raise an exception
		 * if we are the wrong type of PixelMap (EG direct).
		 */
		nonvirtual	const ColorLookupTable&	GetColorLookupTable () const;
		nonvirtual	void					SetColorLookupTable (const ColorLookupTable& colorLookupTable);


		/*
		 * Retreive/Set the given pixel value at the point p, relative to our bounds.
		 */
		nonvirtual	Pixel	GetPixel (const Point& p) const;
		nonvirtual	void	SetPixel (const Point& p, Pixel pixel);


		/*
		 * Retreive/Set the given color value at the point p, relative to our bounds. The color
		 * is mapped to the approprate pixel value in a manner that depends on the PixelStyle, and
		 * if it is an eIndirect, based on the current ColorLookupTable values.
		 */
		nonvirtual	Color	GetColor (const Point& p) const;
		nonvirtual	void	SetColor (const Point& p, const Color& c);


		/*
		 * routines to calculate sizes/positions of pixels in PixMap...
		 */
		static		size_t	CalcRowBytes (const Rect& bounds, Depth depth);
		static		size_t	CalcImageSize (const Rect& bounds, Depth depth);
		nonvirtual	size_t	GetImageSize ()  const;


// Make sure we also protect in destructive things like SetRowBytes, SetDepth, etc....????
		/*
		 * Making a subwindow of an existing PixelMap. This is somewhat unsafe, as you must
		 * be sure your subwindows are destroyed before thier parent. Subwindows do not
		 * copy pixels, only point to sub-portion of existing PixelMap. When debug is on, we
		 * protect against (with asserts) accidental deletion of a parent PixelMap with existing
		 * subwindows.
		 */
		nonvirtual	void	MakeSubWindow (PixelMap& fillIn, const Rect& subWindow) const;


// below doc si wrong and must be reqreitten to support new stargety wehre
// we construct both types on demand, and fill in properly. The assumetion that things work as
// below in the rest of the code can be maintained, of course, til the code here is pristince
// and solid, since this is a totalyl compatable change....
//
//
		/*
		 * For mac, OSRep can be osBitMap*, or osPixMap** (PIXMAPHANDLE).  Depending
		 * on whether or not Color QuickDraw is available (IRRESPECTIVE OF HOW PIXMAP WAS
		 * DEFINED).
		 * IE USE COLOR QD STRUCTURES IFF COLOR QD AVAILABLE!!!
		 */
// trying to move towards type safety.  Call routine you want, for now and get assertions
// if you call wrong one.  Someday, do even better, but this is an improvment over always returning
// void*, and casting!!!
#if		qMacGDI
		nonvirtual	osBitMap*	GetOldOSRepresentation () const;
		nonvirtual	osPixMap**	GetNewOSRepresentation () const;
#elif	qXGDI
		nonvirtual	osImage*		GetOSImage () const;				// invalidates pixmap representation (not
																		// sure it should - but only save thing for
																		// now).
		nonvirtual	unsigned long	GetOSPixmap () const;
#endif	/*GDI*/
		

		inline	void	Invariant () const
			{
#if		qDebug
				Invariant_ ();
#endif	/*qDebug*/
			}



	private:
		/*
 		 * low level accessors which give us gdi independence without the checking
		 * for valid values (and invariants)
		 */
		nonvirtual	Depth		GetDepth_ () const;
		nonvirtual	void		SetDepth_ (Depth depth);
		nonvirtual	size_t		GetRowBytes_ () const;
		nonvirtual	void		SetRowBytes_ (size_t rowBytes);
		nonvirtual	void*		GetBaseAddress_ () const;
		nonvirtual	void		SetBaseAddress_ (void* baseAddress);

	private:
		Boolean				fShouldDisposeOfBaseAddr;
		ColorLookupTable	fCLUT;
#if		qMacGDI
		osBitMap*			fOSBitMapRepresentation;
		osPixMap**			fOSPixMapRepresentation;
#elif	qXGDI
		Point				fOrigin;
		osImage*			fOSImage;			// we always fake this - XLib implementation does too!!!
		unsigned long		fOSPixmap;			// Nil means bad
												// True iff the real value of the pixels is only
												// valid on the server. This implies fOSPixmap is valid.
												// Also, our local fOSImage->data is bad (Nil)
#endif	/*GDI*/


#if		qXGDI
		nonvirtual	void	SnagOSImage () const;
#endif	/*GDI*/

#if		qDebug
		/*
		 * Fields to be sure subwindows are destroyed before we are.
		 */
		PixelMap*			fParentPixelMap;
		PixelMap*			fNextPixelMap;
		PixelMap*			fSubPixelMaps;
#endif	/*qDebug*/


		// played around with making this rooutine public - but decided bad idea - instead add new suite of low level
		// methods to expand scanline to uniform format...
		nonvirtual	void*	ComputePixMapAddr (const Point& p) const;

		nonvirtual	void	ReleasePixelMapStorage ();


#if		qMacGDI || qXGDI || qWinGDI
		nonvirtual	void	MakeInitialOSPixMap (PixelStyle pixelStyle, Depth depth);
#endif	/*GDI*/

#if		qDebug
		nonvirtual	void	Invariant_ () const;
#endif	/*qDebug*/


	friend	class	ReadWriteExpandedScanLine;
};



// Should be scoped by CFront 21 barfs when we try to make nested classes friends... LGP April 30, 1992
		/*
		 * Utility classes to allow access to an expanded, and easily, rapidly accessible scanline. The storage returned
		 * by these accessor classes may be an internal pointer inside of the PixelMap, or it may be other storage.
		 * While updating a ReadWriteExpandedScanLine be sure to remember your changes are only gauranteed synced with
		 * the real PixelMap data when you destroy the expandedScanLine. It is considered illegal, though currently unchecked
		 * to use change the pixelmap thru both interfaces, or count on changes in one being reflected in the other.
		 */
		class	ReadOnlyExpandedScanLine {
			public:
				ReadOnlyExpandedScanLine (const PixelMap& pm, Coordinate row);
				~ReadOnlyExpandedScanLine ();

			private:// dont copy these...
				ReadOnlyExpandedScanLine (const ReadOnlyExpandedScanLine& from);
				const ReadOnlyExpandedScanLine& operator= (const ReadOnlyExpandedScanLine& from);

			public:
				nonvirtual	const	UInt8*	GetScanLine8 () const;
				nonvirtual	const	UInt16*	GetScanLine16 () const;
				nonvirtual	const	UInt32*	GetScanLine32 () const;

				nonvirtual	void	Sync ();

			private:
				const	PixelMap&	fPixelMap;
				const	void*		fExpandedData;

				nonvirtual	Boolean	Peeking () const;
		};
		class	ReadWriteExpandedScanLine {
			public:
				ReadWriteExpandedScanLine (PixelMap& pm, Coordinate row);
				~ReadWriteExpandedScanLine ();

			private:// dont copy these...
				ReadWriteExpandedScanLine (const ReadWriteExpandedScanLine& from);
				const ReadWriteExpandedScanLine& operator= (const ReadWriteExpandedScanLine& from);

			public:
				nonvirtual	UInt8*	GetScanLine8 () const;
				nonvirtual	UInt16*	GetScanLine16 () const;
				nonvirtual	UInt32*	GetScanLine32 () const;

				nonvirtual	UInt8	GetChunkSize () const	{ return (fChunkSize); }

			private:
				PixelMap&	fPixelMap;
				void*		fExpandedData;
				Coordinate	fWhichRow;
				Coordinate	fColumns;
				UInt8		fChunkSize;

				nonvirtual	Boolean	Peeking () const;
		};





/*
 * These routines assume as a canonical format for pixelmaps, (mac format- spell out precisely what this is).
 * (for one thing, this means all data sb aligned as TN 120 page ~4 ((depth*width+15) >> 4) << 1 (ie CalcRowBytes ()
 * better work).
 *
 *
 * We could expand the number of different kinds of arguments given to these functions, but they are not
 * strictly necessary, and will probably be rarely used. At any rate, they are expected to be used in
 * restricted circumstances (when you have a pixmap declared as static data).
 */
extern	PixelMap	PixelMapFromData (const Point& size, const UInt8* data, size_t nBytes);		// bitmap data
extern	PixelMap	PixelMapFromData (const Point& size, const UInt8* data, size_t nBytes,
#if		!qCFront_NestedTypesHalfAssed
						PixelMap::Depth depth, 
#else
						Depth depth, 
#endif
						const Color clut[]);										// pixelmap data









/*
 * Stream inserters and extractors.
 */
class ostream;
class istream;
extern	ostream&	operator<< (ostream& out, const PixelMap& pixelMap);
extern	istream&	operator>> (istream& in, PixelMap& pixelMap);



/*
 * Comparison operators.
 */
extern	Boolean	operator== (const PixelMap& lhs, const PixelMap& rhs);
extern	Boolean	operator!= (const PixelMap& lhs, const PixelMap& rhs);









/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__PixelMap__*/
