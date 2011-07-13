/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Adornment.cc,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Adornment.cc,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/04  14:14:41  lewis
 *		Take advantage of new shape letter/envelope support- pass by value, instead of reference.
 *
 *		Revision 1.3  1992/07/03  00:15:13  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.10  1992/05/23  00:59:39  lewis
 *		Get rid of EmptyAdornment since phasing out adornments anyhow.
 *
 *		Revision 1.9  92/02/21  18:05:15  18:05:15  lewis (Lewis Pringle)
 *		Got rid of qGPlus_ClassOpNewDelBroke workaround.
 *
 *
 */



#include	"Debug.hh"

#include	"Shape.hh"

#include	"Adornment.hh"







/*
 ********************************************************************************
 ********************************** Adornment ***********************************
 ********************************************************************************
 */
Adornment::Adornment ():
	fOwner (Nil),
	fAdornRegion (kZeroRect),
	fRegionValid (False)
{
}

Adornment::~Adornment () 
{
}

void	Adornment::Refresh (Panel::UpdateMode update)
{
	if ((update != View::eNoUpdate) and (fOwner != Nil) and (GetOwner ().GetParentView () != Nil)) {
		Refresh (GetAdornRegion (), update);
	}
}

void	Adornment::Refresh (const Region& region, Panel::UpdateMode update)
{
	if ((update != View::eNoUpdate) and (fOwner != Nil) and (GetOwner ().GetParentView () != Nil)) {
		GetOwner ().GetParentView ()->Refresh (region, update);
	}
}

void	Adornment::SetAdornRegion (const Region& region, Panel::UpdateMode update)
{
	if ((not fRegionValid) or (fAdornRegion != region)) {
		fRegionValid = False;
		
		Region	oldRegion = fAdornRegion;
		fAdornRegion = region;
		fRegionValid = True;
		Refresh (fAdornRegion + oldRegion, update);
	}
}

Region	Adornment::GetAdornRegion () const	
{	
	if (not fRegionValid) {
		// Should this really be const method, or should we cast around the const problem???
		((Adornment*)this)->SetAdornRegion (CalcAdornRegion (GetOwner ().GetRegion ()), View::eNoUpdate);
	}
	
	Ensure (fRegionValid);
	return (fAdornRegion); 
}

void	Adornment::Invalidate (Panel::UpdateMode update)
{
	if (fRegionValid) {
		fRegionValid = False;
		if ((update != View::eNoUpdate) and (fOwner != Nil) and (GetOwner ().GetParentView () != Nil)) {
			SetAdornRegion (CalcAdornRegion (GetOwner ().GetRegion ()), update);
		}
	}
}






/*
 ********************************************************************************
 ******************************** CompositeAdornment ****************************
 ********************************************************************************
 */
CompositeAdornment::CompositeAdornment (Adornment* adornment, CompositeAdornment* next) :
	fAdornment (adornment),
	fNextAdornment (next)
{
	RequireNotNil (fAdornment);
}

CompositeAdornment::~CompositeAdornment ()
{
	delete fNextAdornment;
	delete fAdornment;
}
		
void	CompositeAdornment::Draw ()
{
	if (fNextAdornment != Nil) {
		fNextAdornment->Draw ();
	}
	fAdornment->Draw ();
}

Region	CompositeAdornment::CalcAdornRegion (const Region& bounds) const
{
	if (fNextAdornment != Nil) {
		Region newBounds = fNextAdornment->CalcAdornRegion (bounds);
		return (newBounds + fAdornment->CalcAdornRegion (newBounds));
	}
	return (fAdornment->CalcAdornRegion (bounds));
}

void	CompositeAdornment::Invalidate (Panel::UpdateMode update)
{
	if (fNextAdornment != Nil) {
		fNextAdornment->Invalidate (update);
	}
	AssertNotNil (fAdornment);
	fAdornment->Invalidate (update);
	Adornment::Invalidate (update);
}

CompositeAdornment*	CompositeAdornment::RemoveAdornment (Adornment* adornment)
{
	AssertNotNil (adornment);
	if (fAdornment == adornment) {
		CompositeAdornment*	nextAdornment = fNextAdornment;
		if (nextAdornment != Nil) {
			nextAdornment->Invalidate (Panel::eNoUpdate);
		}
		
		fNextAdornment = Nil;
		delete this;
		return (nextAdornment);
	}
	else if (fNextAdornment != Nil) {
		fNextAdornment = fNextAdornment->RemoveAdornment (adornment);
		Invalidate (Panel::eNoUpdate);
	}
	else {
		AssertNotReached ();	// not our adornment!!!
	}
	return (this);
}





/*
 ********************************************************************************
 ********************************** ShapeAdornment ******************************
 ********************************************************************************
 */

ShapeAdornment::ShapeAdornment (const Shape& shape, const Pen& pen, const Point& inset):
	fShape (shape),
	fPen (pen),
	fInset (inset)
{
}

ShapeAdornment::ShapeAdornment (const Shape& shape):
	fShape (shape),
	fPen (kDefaultPen),
	fInset (kDefaultInset)
{
}

ShapeAdornment::~ShapeAdornment ()
{
}

void	ShapeAdornment::Draw ()
{
	Rect	r = GetAdornRegion ().GetBounds ();

	AssertNotNil (GetOwner ().GetParentView ());
	GetOwner ().GetParentView ()->OutLine (GetShape (), r, GetPen ());
}

Region	ShapeAdornment::CalcAdornRegion (const Region& region) const
{
	Rect	bounds = region.GetBounds ();
	bounds.InsetBy (fInset);
	return (GetShape ().ToRegion (bounds));
}

void	ShapeAdornment::SetShape (const Shape& newShape, Panel::UpdateMode update)
{
#if 1
// we may want to support equality test...???At least pointer equality?
	fShape = newShape;
	Invalidate (update);

#else
	RequireNotNil (newShape);
	if (fShape != newShape) {
		AssertNotNil (fShape);
		delete fShape;
		fShape = newShape;
		Invalidate (update);
	}
#endif
}

void	ShapeAdornment::SetPen (const Pen& pen, Panel::UpdateMode update)
{
	fPen = pen;
	Refresh (update);
}

Point	ShapeAdornment::GetInset () const
{
	return (fInset);
}

void	ShapeAdornment::SetInset (const Point& inset, Panel::UpdateMode update)
{
	if (fInset != inset) {
		fInset = inset;
		Invalidate (update);
	}
}





/*
 ********************************************************************************
 ******************************* TitledAdornment ********************************
 ********************************************************************************
 */

TitledAdornment::TitledAdornment (const String& title, const Shape& shape, Boolean outlineShape):
	ShapeAdornment (shape),
	fTitle (title),
	fAdornFont (kSystemFont),
	fOutlineShape (outlineShape)
{
}

TitledAdornment::TitledAdornment (const String& title, const Shape& shape, const Pen& pen, const Point& inset):
	ShapeAdornment (shape, pen, inset),
	fTitle (title),
	fAdornFont (kSystemFont),
	fOutlineShape (True)
{
}

Font	TitledAdornment::GetAdornFont () const
{
	return (fAdornFont);
}

void	TitledAdornment::SetAdornFont (const Font& f, Panel::UpdateMode update)
{
	fAdornFont = f;
	Invalidate (update);
}

String	TitledAdornment::GetTitle () const
{
	return (fTitle);
}

void	TitledAdornment::SetTitle (const String& title, Panel::UpdateMode update)
{
	if (fTitle != title) {
		fTitle = title;
		Invalidate (update);
	}
}

Rect	TitledAdornment::GetTitleRect () const
{
	Coordinate	textHeight = GetAdornFont ().GetFontHeight ();
	Coordinate	textWidth  = GetOwner ().TextWidth (GetTitle (), GetAdornFont ());
	return (Rect (kZeroPoint, Point (textHeight, textWidth)));
}

void	TitledAdornment::Draw ()
{
	if (fOutlineShape) {
		/* temp hack - really should be a composite adornment */
		Rect	r = ShapeAdornment::CalcAdornRegion (GetOwner ().GetRegion ()).GetBounds ();
	
		AssertNotNil (GetOwner ().GetParentView ());
		GetOwner ().GetParentView ()->OutLine (GetShape (), r, GetPen ());
	}
	if (GetTitle () != kEmptyString) {
		Rect	r = GetAdornRegion ().GetBounds ();
		Rect	titleRect = GetTitleRect ();

		titleRect.SetOrigin (r.GetOrigin () + Point (0, 5));

		AssertNotNil (GetOwner ().GetParentView ());
		titleRect = titleRect.InsetBy (Point (0, -2));
		GetOwner ().GetParentView ()->Erase (Rectangle (), titleRect);
		titleRect = titleRect.InsetBy (Point (0, 2));
		GetOwner ().GetParentView ()->DrawText (GetTitle (), GetAdornFont (), titleRect.GetOrigin () + Point (-1, 0));
	}
}

Region	TitledAdornment::CalcAdornRegion (const Region& bounds) const
{
	Region	region = ShapeAdornment::CalcAdornRegion (bounds);
	
	if (GetTitle () != kEmptyString) {
		Rect	r = region.GetBounds ();
		Rect	titleRect = GetTitleRect ();
		titleRect.SetOrigin (Point (r.GetTop () - titleRect.GetHeight ()/2 + 1, r.GetLeft () + 5));
		titleRect = titleRect.InsetBy (Point (0, -2));
		region += titleRect;
	}
	return (region);
}






/*
 ********************************************************************************
 ******************************* LabelAdornment *********************************
 ********************************************************************************
 */

const	Coordinate	LabelAdornment::kDefaultGap	= 5;


LabelAdornment::LabelAdornment (const String& label, const Shape& shape, Boolean outlineShape):
	ShapeAdornment (shape),
	fLabel (label),
	fAdornFont (kSystemFont),
	fGap (kDefaultGap),
	fOutlineShape (outlineShape)
{
// temp hack
#if qMotifUI
fOutlineShape = False;
#endif
}

LabelAdornment::LabelAdornment (const String& label, const Shape& shape, const Pen& pen, const Point& inset):
	ShapeAdornment (shape, pen, inset),
	fLabel (label),
	fAdornFont (kSystemFont),
	fGap (kDefaultGap),
	fOutlineShape (True)
{
}

Font	LabelAdornment::GetAdornFont () const
{
	return (fAdornFont);
}

void	LabelAdornment::SetAdornFont (const Font& f, Panel::UpdateMode update)
{
	fAdornFont = f;
	Invalidate (update);
}

String	LabelAdornment::GetLabel () const
{
	return (fLabel);
}

Coordinate	LabelAdornment::GetGap () const
{
	return (fGap);
}

void	LabelAdornment::SetGap (Coordinate gap, Panel::UpdateMode update)
{
	if (fGap != gap) {
		fGap = gap;
		Invalidate (update);
	}
}

void	LabelAdornment::SetLabel (const String& label, Panel::UpdateMode update)
{
	if (fLabel != label) {
		fLabel = label;
		Invalidate (update);
	}
}

Rect	LabelAdornment::GetLabelRect () const
{
	Coordinate	textWidth  = GetOwner ().TextWidth (GetLabel (), GetAdornFont ());
	Coordinate	textHeight = GetAdornFont ().GetFontHeight ();
	return (Rect (kZeroPoint, Point (textHeight, textWidth)));
}

void	LabelAdornment::Draw ()
{
	if (fOutlineShape) {
		/* temp hack - really should be a composite adornment */
		Rect	r = ShapeAdornment::CalcAdornRegion (GetOwner ().GetRegion ()).GetBounds ();
	
		AssertNotNil (GetOwner ().GetParentView ());
		GetOwner ().GetParentView ()->OutLine (GetShape (), r, GetPen ());
	}
	if (GetLabel () != kEmptyString) {
		Rect	drawnExtent = GetAdornRegion ().GetBounds ();
		Rect	labelRect 	= GetLabelRect ();

		labelRect.SetOrigin (drawnExtent.GetOrigin () + 
			Point ((drawnExtent.GetHeight () - labelRect.GetHeight ())/2, 0));
			
		AssertNotNil (GetOwner ().GetParentView ());
		GetOwner ().GetParentView ()->DrawText (GetLabel (), GetAdornFont (), labelRect.GetOrigin ());
	}
}

Region	LabelAdornment::CalcAdornRegion (const Region& bounds) const
{
	Region	region = ShapeAdornment::CalcAdornRegion (bounds);
	Assert (GetOwner ().GetTablet () != Nil);
	if (GetOwner ().GetTablet () != Nil) {
		if (GetLabel () != kEmptyString) {
			Rect	drawnExtent = region.GetBounds ();
			Rect	labelRect = GetLabelRect ();
			
			labelRect.SetOrigin (drawnExtent.GetOrigin () + 
				Point ((drawnExtent.GetHeight () - labelRect.GetHeight ())/2, 
				-(labelRect.GetWidth () + fGap)));
			region += labelRect;
		}
	}
	return (region);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

