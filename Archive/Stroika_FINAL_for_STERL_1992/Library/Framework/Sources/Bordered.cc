/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Bordered.cc,v 1.3 1992/07/03 00:18:15 lewis Exp $
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Bordered.cc,v $
 *		Revision 1.3  1992/07/03  00:18:15  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  07:39:03  sterling
 *		Lots of changes - got rid of AbstractBordered, and BorderedView classes. Made View
 *		subclass from Bordered, and moved some drawing utils there (LGP checking in comments not
 *		quite sure why).
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.6  1992/05/18  16:01:39  lewis
 *		Minor cleanups.
 *
 *		Revision 1.3  92/04/30  13:24:14  13:24:14  sterling (Sterling Wight)
 *		BorderedView stuff
 *
 *		Revision 1.2  92/04/20  14:20:27  14:20:27  lewis (Lewis Pringle)
 *		Get rid of CalcShadowThickness for Bordered - it was a virtual method, conditionally compiled into this
 *		class. This had the unfortunate side effect of making VERY difficult portable motif GUI controls - since they
 *		naturaly wanted to use this function - at least as much as any native versions did. Since this feature was
 *		clearly GUI dependent, I had two choices - a GUI splitting of classes, or blow it away - and since it was VERY
 *		rarely used, and very simple, I just inlined it.
 *		If you find you counted on it, its implenetnation was just Max (GetMarginWidth(),GetMarginHeight()) - I generally
 *		replaced it with Min rather than Max, since that seemed more likely correct, thgough in no case I could
 *		find did the distinction matter.
 *
 *		Revision 1.1  92/03/05  20:13:39  20:13:39  sterling (Sterling Wight)
 *		Initial revision
 *		
 *
 *
 */

#include 	"Format.hh"

#include 	"Color.hh"
#include 	"Shape.hh"
#include	"Pallet.hh"

#include	"Bordered.hh"






/*
 ********************************************************************************
 ************************************* Bordered *********************************
 ********************************************************************************
 */
Bordered::Bordered ():
	fBorder (kZeroPoint),
	fMargin (kZeroPoint),
	fPlane (eOut)
{
}

Bordered::~Bordered ()
{
}
		
void	Bordered::SetBorder (const Point& border, Panel::UpdateMode updateMode)
{
	if (border != GetBorder ()) {
		SetBorder_ (border, updateMode);
	}
	Ensure (border == GetBorder ());
}

void	Bordered::SetBorder (Coordinate height, Coordinate width, Panel::UpdateMode updateMode)
{
	SetBorder (Point (height, width), updateMode);
}

void	Bordered::SetBorder_ (const Point& border, Panel::UpdateMode /*updateMode*/) 
{
	fBorder = border;
}

void	Bordered::SetMargin (const Point& margin, Panel::UpdateMode updateMode)
{
	if (margin != GetMargin ()) {
		SetMargin_ (margin, updateMode);
	}
	Ensure (margin == GetMargin ());
}

void	Bordered::SetMargin (Coordinate height, Coordinate width, Panel::UpdateMode updateMode)
{
	SetMargin (Point (height, width), updateMode);
}

void	Bordered::SetMargin_ (const Point& margin, Panel::UpdateMode /*updateMode*/)
{
	fMargin = margin;
}

void	Bordered::SetPlane (Bordered::Plane plane, Panel::UpdateMode updateMode)
{
	if (GetPlane () != plane) {
		SetPlane_ (plane, updateMode);
	}
	Ensure (GetPlane () == plane);
}

void	Bordered::SetPlane_ (Bordered::Plane plane, Panel::UpdateMode /*updateMode*/)
{
	fPlane = plane;
}









// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


