/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
/*
 * $Header: /fuji/lewis/RCS/SeriesGraph.cc,v 1.5 1992/09/01 15:58:05 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SeriesGraph.cc,v $
 *		Revision 1.5  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  21:36:22  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/03  04:09:32  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/07/02  05:12:48  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/04/15  13:24:57  sterling
 *		Initial revision
 *
 *
 *
 */

#include	"Debug.hh"
#include	"Format.hh"

#include	"Shape.hh"

#include	"SeriesGraph.hh"





/*
 ********************************************************************************
 *********************************** GraphSeries ********************************
 ********************************************************************************
 */
GraphSeries::GraphSeries (const Tile& tile, const String& title) :
	fTile (tile),
	fTitle (title)
{
}

Tile	GraphSeries::GetTile () const
{
	return (fTile);
}

void	GraphSeries::SetTile (const Tile& tile)
{
	fTile = tile;
}

String	GraphSeries::GetTitle () const
{
	return (fTitle);
}

void	GraphSeries::SetTitle (const String& title)
{
	fTitle = title;
}




#if		!qRealTemplatesAvailable
	Implement (Iterator, GrSerPtr);
	Implement (Collection, GrSerPtr);
	Implement (AbSequence, GrSerPtr);
	Implement (Array, GrSerPtr);
	Implement (Sequence_Array, GrSerPtr);
	Implement (Sequence, GrSerPtr);
#endif




/*
 ********************************************************************************
 ************************************ SeriesGraph *******************************
 ********************************************************************************
 */
SeriesGraph::SeriesGraph () :
	View ()
{
}
	
Axis&	SeriesGraph::GetXAxis () const
{
	return (GetXAxis_ ());
}
		
Axis&	SeriesGraph::GetYAxis () const
{
	return (GetYAxis_ ());
}
		
SequenceIterator(GraphSeriesPtr)*	SeriesGraph::MakeSeriesIterator (SequenceDirection d) const
{
	return (MakeSeriesIterator_ (d));
}
		
String	SeriesGraph::GetTitle () const
{
	return (GetTitle_ ());
}

void	SeriesGraph::SetTitle (const String& title, Panel::UpdateMode update)
{
	if (GetTitle () != title) {
		SetTitle_ (title, update);
	}
	Ensure (GetTitle () == title);
}

Boolean	SeriesGraph::GetTitleShown () const
{
	return (GetTitleShown_ ());
}

void	SeriesGraph::SetTitleShown (Boolean shown, Panel::UpdateMode update)
{
	if (GetTitleShown () != shown) {
		SetTitleShown_ (shown, update);
	}
	Ensure (GetTitleShown () == shown);
}
		
Boolean	SeriesGraph::GetLegendShown () const
{
	return (GetLegendShown_ ());
}

void	SeriesGraph::SetLegendShown (Boolean shown, Panel::UpdateMode update)
{
	if (GetLegendShown () != shown) {
		SetLegendShown_ (shown, update);
	}
	Ensure (GetLegendShown () == shown);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

