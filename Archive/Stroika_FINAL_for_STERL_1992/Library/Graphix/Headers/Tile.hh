/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Tile__
#define	__Tile__

/*
 * $Header: /fuji/lewis/RCS/Tile.hh,v 1.7 1992/11/25 22:43:47 lewis Exp $
 *
 * Description:
 *
 *		A Tile is used for graphical fill operations.  Then can be made from
 * colors, bitmaps & colors, and from PixelMaps.  Tiles are shared-reference
 * counted objects for improved efficiency.  They are immutable, for similar
 * reasons.  If you want to change one, just change the defining structures, and
 * create a new one.
 *
 * TODO:
 *
 *		On mac, cleanup INVALID reference to qMPW_CFRONT_2_1_NESTED_TYPEDEFS_BREAK_LOADDUMP_BUG bug.
 *
 *		Dont have IsOld/NewFormat () method - whichever one called, generate appropate value on the
 *		fly. Keep both fields. Work with both regardless of whehter or not we have color qd. This is a strict
 *		improvemnt on current funtionality - cases that used to break will now work - not hte other way around.
 *
 *		Consider getting rid of these pre-defined tiles - probably a bad idea to use them -
 *		Use PalleteManager instead so it can sometimes come up with dithers, or real colors, or
 *		whatever...
 *
 *		not sure that this is a good place to put resoruce support. Maybe put it elsewhere. We do
 *		or should allow constuction of os reps, so this is good enuf - somewhereelse do the resource shit.
 *
 *		Redo mac pixmap code so we always keep a PixelMap rep around. Also, a bool saying if we should delete
 *		memory, so we can support pixmaps/ os tiles haned in ctors.
 *
 *		X NOTES:
 *			Right now, we only support stipples, since regular tiles would be useless until we have
 *		general ability to convert depths. We must also be able to transform from using one clut to
 *		another (the one associated with the src pixmap/tile and to the destination drawable).
 *
 * Notes:
 *
 * Changes:
 *	$Log: Tile.hh,v $
 *		Revision 1.7  1992/11/25  22:43:47  lewis
 *		Convert to new genclass based template support.
 *		And use (~const) casts where needed - revisit and see if appropriate???
 *
 *		Revision 1.6  1992/09/11  18:36:49  sterling
 *		use new Shared implementation
 *
 *		Revision 1.4  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/16  05:25:19  lewis
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
 *		Revision 1.18  1992/06/09  12:37:45  lewis
 *		Renamed SmartPointer back to Shared.
 *
 *		Revision 1.17  92/06/09  14:46:50  14:46:50  sterling (Sterling Wight)
 *		used templates for refence counting
 *		
 *		Revision 1.16  1992/05/13  00:21:25  lewis
 *		On Mac, Keep PixelMap around for Tiles.
 *		And, comment on TileSharedPart::DTOR - being nonvirtual.
 *
 *		Revision 1.15  92/04/30  13:38:49  13:38:49  sterling (Sterling Wight)
 *		fixed bug in operator=
 *		
 *		Revision 1.14  92/04/30  13:33:02  13:33:02  lewis (Lewis Pringle)
 *		Made TileSharedPart::ExpandTileTo() non-const - old was accident.
 *		
 *		Revision 1.13  92/04/30  03:26:51  03:26:51  lewis (Lewis Pringle)
 *		Get rid of old cruft.
 *		Get rid of IsStipple () method - currently unused.
 *		Add unsigned long   Tile::ExpandTileTo (const PixelMap& referencePMap)
 *		const method,a nd new fExpanded field to support it.
 *		(XGDI).
 *		
 *		Revision 1.12  92/04/15  14:38:06  14:38:06  lewis (Lewis Pringle)
 *		Declare stream/comparison operators.
 *		
 *		Revision 1.11  92/04/02  11:22:07  11:22:07  lewis (Lewis Pringle)
 *		Comment out qMPW_CFRONT_2_1_NESTED_TYPEDEFS_BREAK_LOADDUMP_BUG to help discover
 *		if it still exists and to document it better.
 *		Also, switched to having TileSharedPart subclass from Shared to get
 *		its implemention.
 *		
 *		Revision 1.9  1992/02/12  03:11:04  lewis
 *		Got working with new apple compiler (new header defn of osPattern).
 *
 *		Revision 1.8  1992/02/11  21:59:18  lewis
 *		Support keeping actual colors assoicated wtih pixelmap in tile.
 *		Note in the docs that we only support stipples for now, since otherwise
 *		we would need general depth conversion routines.
 *
 *
 *
 */

#include	"Debug.hh"
#include	"Shared.hh"

#include	"PixelMap.hh"



#if		qMacGDI
struct	osPattern;
struct	osPixPat;
#endif

class	TileSharedPart;

#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include SharedOfTileSharedPart_hh
#endif


class	Tile {
	public:
		Tile (const Tile& tile);
		Tile (const PixelMap& pixelMap);
		~Tile ();

		nonvirtual	Tile&	operator= (const Tile& tile);

	public:
		nonvirtual	const PixelMap&	PeekAtPixmap () const;

#if		qMacGDI
		nonvirtual	const	osPattern*	GetOldQDPattern () const;
		nonvirtual	const	osPixPat**	GetNewQDPattern () const;
#elif	qXGDI
		nonvirtual	unsigned long	GetOSPixmap () const;
		nonvirtual	unsigned long	ExpandTileTo (const PixelMap& referencePMap) const;
#endif	/*qMacGDI*/

	private:
#if		qRealTemplatesAvaialble
		Shared<TileSharedPart>	fSharedPart;
#else
		Shared(TileSharedPart)	fSharedPart;
#endif
};



// This class should be scoped inside Tile but for compiler bugs...
// qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
	struct	TileSharedPart {
		PixelMap		fPixelMap;
#if		qMacGDI
		osPixPat**		fOSPattern;
#elif	qXGDI
		PixelMap		fExpanded;
#endif	/*qMacGDI*/
	
		TileSharedPart (const PixelMap& pixelMap);

		/*
		 * Note that we are not using virtual DTORs here, and so its important that we be careful not to
		 * keep pointers to this guys base class so DTOR called properly.
		 */
		nonvirtual ~TileSharedPart ();

#if		qXGDI
		nonvirtual	unsigned long	GetOSPixmap ();
		nonvirtual	unsigned long	ExpandTileTo (const PixelMap& referencePMap);
#endif	/*GDI*/
		nonvirtual	const	PixelMap&	PeekAtPixmap ();

		friend	class	Tile;
	};






/*
 * Stream inserters and extractors.
 */
class ostream;
class istream;
extern	ostream&	operator<< (ostream& out, const Tile& tile);
extern	istream&	operator>> (istream& in, Tile& tile);




/*
 * Comparison operators.
 */
extern	Boolean	operator== (const Tile& lhs, const Tile& rhs);
extern	Boolean	operator!= (const Tile& lhs, const Tile& rhs);






extern	const	Tile	kWhiteTile;
extern	const	Tile	kLightGrayTile;
extern	const	Tile	kGrayTile;
extern	const	Tile	kDarkGrayTile;
extern	const	Tile	kBlackTile;






/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
	inline	Tile::Tile (const Tile& tile):
		fSharedPart (tile.fSharedPart)
	{
	}
	inline	Tile::~Tile ()
	{
	}
	inline	Tile&	Tile::operator= (const Tile& tile)
	{
		fSharedPart = tile.fSharedPart;
		return (*this);
	}
#if		qMacGDI
	inline	const	osPixPat**	Tile::GetNewQDPattern () const
	{
		EnsureNotNil (fSharedPart->fOSPattern);
		return (fSharedPart->fOSPattern);
	}
#elif	qXGDI
	inline	unsigned long	Tile::GetOSPixmap () const
	{
		// (~const)
		// conceptually this is const but because of caching, we must break
		// const...
		return (((Tile*)this)->fSharedPart->GetOSPixmap ());
	}
		
	inline	unsigned long	Tile::ExpandTileTo (const PixelMap& referencePMap) const
	{
		// (~const)
		// conceptually this is const but because of caching, we must break
		// const...
		return (((Tile*)this)->ExpandTileTo (referencePMap));
	}
#endif /*GDI*/
	inline	const	PixelMap&	Tile::PeekAtPixmap () const
	{
		// (~const)
		// conceptually this is const but because of caching, we must break
		// const...
		return (((Tile*)this)->fSharedPart->PeekAtPixmap ());
	}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Tile__*/
