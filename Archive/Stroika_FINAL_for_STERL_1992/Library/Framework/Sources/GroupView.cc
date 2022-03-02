/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/GroupView.cc,v 1.5 1992/09/01 15:46:50 sterling Exp $
 *
 * TODO:
 *	
 *
 * Changes:
 *	$Log: GroupView.cc,v $
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/03  00:59:44  lewis
 *		Neaten, shorten lines, and Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:08:57  sterling
 *		Change border drawing.
 *
 *		Revision 1.1  1992/03/26  09:25:17  lewis
 *		Initial revision
 *
 *
 *
 *
 */



#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"GroupView.hh"






/*
 ********************************************************************************
 *************************************** GroupView ******************************
 ********************************************************************************
 */

GroupView::GroupView ():
	View ()
{
}

GroupView::GroupView (const Rect& extent):
	View ()
{
	SetExtent (extent, eNoUpdate);
}

void	GroupView::AddSubView (View* subView, CollectionSize index, Panel::UpdateMode updateMode)	
{	
	View::AddSubView (subView, index, updateMode);	
}

void	GroupView::AddSubView (View* subView, View* neighborView, AddMode addMode, Panel::UpdateMode updateMode)
{
	View::AddSubView (subView, neighborView, addMode, updateMode);
}

void	GroupView::RemoveSubView (View* subView, Panel::UpdateMode updateMode)
{
	View::RemoveSubView (subView, updateMode);
}

void	GroupView::ReorderSubView (View* v, CollectionSize index, Panel::UpdateMode updateMode)
{
	View::ReorderSubView (v, index, updateMode);
}

void	GroupView::ReorderSubView (View* subView, View* neighborView, AddMode addMode, Panel::UpdateMode updateMode)
{
	View::ReorderSubView (subView, neighborView, addMode, updateMode);
}

CollectionSize	GroupView::GetSubViewCount () const
{
	return (View::GetSubViewCount ());
}

CollectionSize	GroupView::GetSubViewIndex (View* v)
{
	return (View::GetSubViewIndex (v));
}

View*	GroupView::GetSubViewByIndex (CollectionSize index)
{
	return (View::GetSubViewByIndex (index));
}

SequenceIterator(ViewPtr)*	GroupView::MakeSubViewIterator (SequenceDirection d) const
{
	return (View::MakeSubViewIterator (d));
}

void	GroupView::Draw (const Region& /*update*/)
{
	DrawBorder ();
}





/*
 ********************************************************************************
 ************************************ LabeledGroup ******************************
 ********************************************************************************
 */

LabeledGroup::LabeledGroup (const String& label):
	fLabel (label)
{
	SetBorder (1, 1, eNoUpdate);
	SetMargin (1, 1, eNoUpdate);
}
		
String	LabeledGroup::GetLabel () const
{
	return (fLabel);
}

void	LabeledGroup::SetLabel (const String& label, Panel::UpdateMode updateMode)
{
	if (label != GetLabel ()) {
		SetLabel_ (label, updateMode);
	}
	Ensure (label == GetLabel ());
}
	
void	LabeledGroup::Draw (const Region& /*update*/)
{
	if (GetLabel () == kEmptyString) {
		DrawBorder ();
	}
	else {
		Font f = GetEffectiveFont ();
		Coordinate	textHeight = f.GetFontHeight ();
		Coordinate	textWidth  = TextWidth (GetLabel (), f);
#if		qMacUI
		Rect	titleRect = Rect (Point (0, 5), Point (textHeight, textWidth));
#else
		Rect	titleRect = Rect (Point (GetBorder ().GetV () + 2, 5), Point (textHeight, textWidth));
#endif

#if		qMacUI
		Rect r = Rect (Point (textHeight/2, 0), GetSize () - Point (textHeight/2, 0));
#else
		Rect r = GetLocalExtent ();
#endif
		DrawBorder_ (r, GetBorder ());

		titleRect = titleRect.InsetBy (Point (0, -2));
		Erase (Rectangle (), titleRect);
		titleRect = titleRect.InsetBy (Point (0, 2));
		DrawText (GetLabel (), f, titleRect.GetOrigin () + Point (-1, 0));
	}
}

void	LabeledGroup::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

