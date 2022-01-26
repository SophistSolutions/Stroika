/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
/*
 * $Header: /fuji/lewis/RCS/Histogram.cc,v 1.2 1992/09/01 15:58:05 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Histogram.cc,v $
 *		Revision 1.2  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/04/10  00:33:12  lewis
 *		Initial revision
 *
 *
 */

#include	"Debug.hh"

#include	"Shape.hh"

#include	"Histogram.hh"





/*
 ********************************************************************************
 ************************************* Histogram ********************************
 ********************************************************************************
 */

Histogram::Histogram ():
	View (),
	fBars (1)
{
}

CollectionSize	Histogram::GetBars () const
{
	return (fBars);
}

void	Histogram::SetBars (CollectionSize bars, UpdateMode updateMode)
{
	if (fBars != bars) {
		fBars = bars;
		Refresh (updateMode);
	}
}

Tile	Histogram::GetBarTile (CollectionSize bar) const
{
	// keep sequence_array of these...
}

void	Histogram::SetBarTile (CollectionSize bar, const Tile& tile, UpdateMode updateMode)
{
	// keep sequence_array of these...
}

Real	Histogram::GetBarValue (CollectionSize bar) const
{
	// keep sequence_array of these...
}

void	Histogram::SetBarValue (CollectionSize bar, Real value)
{
	// keep sequence_array of these...
}

Real	Histogram::GetTotalValue () const
{
	// cache total value, and update it on sets...
}

Real	Histogram::GetPercentage (CollectionSize bar) const
{
	// return value/totalValue
}

Point	Histogram::CalcDefaultSize (const Point& defaultSize) const
{
	return (Point (100, 100));
}

void	Histogram::Draw (const Region& update)
{
	Fill (Rectangle (), Rect (GetLocalExtent ().GetTopLeft (), Point (GetSize ().GetV (), 10)), kGrayTile);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

