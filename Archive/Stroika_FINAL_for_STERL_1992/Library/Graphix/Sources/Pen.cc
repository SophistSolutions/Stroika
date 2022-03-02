/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Pen.cc,v 1.2 1992/09/01 15:36:53 sterling Exp $
 *
 *
 * TODO:
 *
 * Changes:
 *	$Log: Pen.cc,v $
 *		Revision 1.2  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/05/19  09:49:45  sterling
 *		inserter and extractor
 *
 *		
 *
 */

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Pen.hh"







/*
 ********************************************************************************
 *************************************** Pen ************************************
 ********************************************************************************
 */

Pen::Pen (const Tile& tile):
	Brush (tile),
	fPenSize (Point (1, 1))
{
}

Pen::Pen (const Tile& tile, TransferMode tMode):
	Brush (tile, tMode),
	fPenSize (Point (1, 1))
{
}

Pen::Pen (const Tile& tile, const Point& penSize):
	Brush (tile),
	fPenSize (penSize)
{
}

Pen::Pen (const Tile& tile, TransferMode tMode, const Point& penSize):
	Brush (tile, tMode),
	fPenSize (penSize)
{
}


class ostream&	operator<< (class ostream& out, const Pen& pen)
{
	operator<< (out, (const Brush&)pen);
	out << pen.GetPenSize ();
	return (out);
}

class istream&	operator>> (class istream& in, Pen& pen)
{
	operator>> (in, (Brush&)pen);
	Point penSize;
	in >> penSize;
	pen.SetPenSize (penSize);
	return (in);
}

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


