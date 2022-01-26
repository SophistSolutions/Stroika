/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Brush__
#define	__Brush__

/*
 * $Header: /fuji/lewis/RCS/Brush.hh,v 1.1 1992/06/19 22:33:01 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Brush.hh,v $
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.6  1992/05/19  09:37:50  sterling
 *		inserter and extractor
 *
 *
 */

#include	"Tile.hh"



class	Brush {
	public:
#if		qDoesntAutoGenXOfXRefRight
		Brush (const Brush& b): fTile (b.fTile), fTransferMode (b.fTransferMode) {}
#endif
		Brush (const Tile& tile);
		Brush (const Tile& tile, TransferMode tMode);

		nonvirtual	const	Tile&	GetTile () const;
		nonvirtual	void			SetTile (const Tile& newTile);

		nonvirtual	TransferMode	GetTransferMode () const;
		nonvirtual	void			SetTransferMode (TransferMode newTransferMode);

	private:
		Tile			fTile;
		TransferMode	fTransferMode;
};

class ostream&	operator<< (class ostream& out, const Brush& brush);
class istream&	operator>> (class istream& in, Brush& brush);



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline		const	Tile&	Brush::GetTile () const			{ return (fTile); }
inline		TransferMode	Brush::GetTransferMode () const	{ return (fTransferMode); }


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Brush__*/
