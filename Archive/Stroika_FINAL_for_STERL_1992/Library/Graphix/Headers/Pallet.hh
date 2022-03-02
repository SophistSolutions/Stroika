/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Pallet__
#define	__Pallet__

/*
 * $Header: /fuji/lewis/RCS/Pallet.hh,v 1.4 1992/09/05 16:14:25 lewis Exp $
 *
 * Description:
 *
 * See the Mac Palette Mgr chapter for overall design of this guy.  For now, just forget it.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Pallet.hh,v $
 *		Revision 1.4  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.3  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/06/25  04:07:08  sterling
 *		Added MakeColorFromTile method.
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/04/27  18:35:56  lewis
 *		Work on MakeTileFromColor.
 *
 *		Revision 1.5  1992/02/10  16:23:28  lewis
 *		Renamed from Palette to Pallet (as per sterls suggestion), and made palletmanager class.
 *		Starting to think about making this stuff real.
 *
 *
 *
 */

#include	"Color.hh"
#include	"Tile.hh"







class	Pallet {
	public:
		Pallet ();
		Pallet (const Pallet& pallet);
		~Pallet ();

		const Pallet&	operator= (const Pallet& pallet);
};



/*
 *		This class manages the interface between pallets, and the system color tables. It is mainly
 * used by users to build tiles approximating a given color (either using a direct color represenation,
 * a dither, or a pattern of black and white- if thats all thats available).
 *
 *		There is one of these created per-application, (se
 */
class	PalletManager {
	public:
		PalletManager ();
		virtual ~PalletManager ();

		/*
		 * Generate a tile from the given color. This color might possibly be a solid color of the
		 * given one specified, or it might be a dither of available colors approximating the color
		 * that you requested. In order to accomplish this dithering, we must know something about what
		 * colors will be available when you try to use the given Tile. These assumtions are system dependent,
		 * but hopefully we'll make a good choice.
		 */
		virtual	Tile	MakeTileFromColor (const Color& c);


		/*
		 * What is the color closest to the given tile. Ideally MakeColorFromTile (MakeTileFromColor (c)) .==. c,
		 * but this is no gauranteed - it is quite likely however.
		 */
		virtual	Color	MakeColorFromTile (const Tile& t);


		/*
		 * Usually you just call get, and the first call will automatically build the PalletManager object,
		 * but if you want to override behavior of the PalletManager, you can call set (Nil) to delete the PalletManager,
		 * and then build your own clipboard (eg say PalletManager::Set (new MyClipBoard ()); does the whole job).
		 * PalletManager::Set() deletes the existing one, regardless of who created it, and replaces it with the given
		 * one which better be a new clipboard object.
		 *
		 * NB: Funny name SetThe to avoid conflict with MACRO Set() in foundation!!!
		 */
		static	PalletManager&	Get ();
		static	void			SetThe (PalletManager* newPalletManager);

	private:
		static	PalletManager*	sThe;
};






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Pallet__*/


