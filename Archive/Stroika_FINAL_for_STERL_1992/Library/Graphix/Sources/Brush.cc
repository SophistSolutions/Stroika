/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Brush.cc,v 1.2 1992/09/01 15:36:53 sterling Exp $
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: Brush.cc,v $
 *		Revision 1.2  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/05/19  09:47:02  sterling
 *		inserter and extractor
 *
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Brush.hh"







/*
 ********************************************************************************
 ************************************** Brush ***********************************
 ********************************************************************************
 */

Brush::Brush (const Tile& tile):
	fTile (tile),
	fTransferMode (eCopyTMode)
{
}

Brush::Brush (const Tile& tile, TransferMode tMode):
	fTile (tile),
	fTransferMode (tMode)
{
}

void	Brush::SetTile (const Tile& newTile)
{
	fTile = newTile;
}

void	Brush::SetTransferMode (TransferMode newTransferMode)
{
	fTransferMode = newTransferMode;
}


class ostream&	operator<< (class ostream& out, const Brush& brush)
{
	out << brush.GetTile () << ' ' << brush.GetTransferMode () << ' ';
	return (out);
}

class istream&	operator>> (class istream& in, Brush& brush)
{
	Tile t = kBlackTile;
//	TransferMode mode = eCopyTMode;
	int mode = eCopyTMode;	// weird mac stream library bug
	in >> t >> mode;
	brush.SetTile (t);
	brush.SetTransferMode ((TransferMode)mode);
	return (in);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


