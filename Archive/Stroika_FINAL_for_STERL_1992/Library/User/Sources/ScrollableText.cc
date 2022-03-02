/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScrollableText.cc,v 1.5 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScrollableText.cc,v $
 *		Revision 1.5  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/03  04:08:48  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.19  1992/05/19  09:32:20  sterling
 *		added layout to reduce dependency on TextEdit autosize
 *
 *		Revision 1.18  92/05/13  18:23:20  18:23:20  lewis (Lewis Pringle)
 *		STERL - set border for TextEdits to kZeroPoint when in Scroller.
 *		
 *		Revision 1.16  1992/03/16  16:14:13  sterling
 *		commented out crufty debugmessage
 *
 *		Revision 1.15  1992/03/09  15:19:04  lewis
 *		Fixed missing #if qDebug.
 *
 *		Revision 1.13  1992/03/05  21:52:23  sterling
 *		total rewrite
 *
 *		Revision 1.12  1992/02/15  10:02:52  lewis
 *		EffectiveLiveChanged call needs more work.
 *
 */

#include "ScrollableText.hh"








/*
 ********************************************************************************
 ********************************* TextScroller *********************************
 ********************************************************************************
 */

TextScroller::TextScroller (AbstractScrollBar* verticalSlider, AbstractScrollBar* horizontalSlider) :
	Scroller (Nil, Nil, Nil, verticalSlider, horizontalSlider),
	fTextEdit (Nil)
{
}
	
void	TextScroller::AdjustStepAndPageSizes ()
{
	if (GetTextEdit () != Nil) {
		if (GetVerticalScrollBar () != Nil) {
			Coordinate lineHeight = GetTextEdit ()->GetEffectiveFont ().GetLineHeight ();
			Assert (lineHeight > 10);
			Coordinate pageHeight = GetScrollRect ().GetHeight ();
			Coordinate	pageLineCount	=	Max (Coordinate (pageHeight / lineHeight - 1), Coordinate (1));
			GetVerticalScrollBar ()->SetSteps (lineHeight, lineHeight * pageLineCount);
		}
		if (GetHorizontalScrollBar () != Nil) {
			Coordinate fontWidth = GetTextEdit ()->GetEffectiveFont ().GetMaxCharWidth ();
			GetHorizontalScrollBar ()->SetSteps (fontWidth, Max (0.75 * GetScrollRect ().GetWidth (), Real (fontWidth)));
		}
	}
}

void	TextScroller::TextChanged (TextEditBase* text)
{
	RequireNotNil (GetTextEdit ());
	Require (text == GetTextEdit ());

	ScrolledViewChangedSize ();
	ScrollTo (GetTextEdit ()->GetSelection ().GetFrom (), eDelayedUpdate);
}

void	TextScroller::TextTracking (const Point& mouseAt)
{
	RequireNotNil (GetTextEdit ());
	
	ScrollTo (GetTextEdit ()->GetOffset (mouseAt), eImmediateUpdate);
}

#include "StreamUtils.hh"
void	TextScroller::ScrollTo (CollectionSize at, Panel::UpdateMode updateMode)
{
	RequireNotNil (GetTextEdit ());

	Point		where =	GetTextEdit ()->GetPosition (at);
if (where == kZeroPoint) {
return;
}
	Font	f = GetTextEdit ()->GetStyle (at).GetFont ();
	
	Point	cellSize	=	Point (f.GetLineHeight (), f.GetMaxCharWidth ());
#if		qDebug
//gDebugStream << "scrollto " << Rect (where-cellSize, cellSize) << newline;
#endif
	ScrollSoShowing (Rect (where-cellSize, cellSize), False, updateMode);
}
			
void	TextScroller::SetTextEdit (AbstractTextEdit* textEdit)
{
	Require ((fTextEdit == Nil) or (textEdit == Nil));
	SetTextEdit_ (textEdit);
}

AbstractTextEdit*	TextScroller::GetTextEdit () const
{
	return (fTextEdit);
}
		
void	TextScroller::SetTextEdit_ (AbstractTextEdit* textEdit)
{
	SetController (Nil);
	
	fTextEdit = textEdit;
	if (fTextEdit != Nil) {
		fTextEdit->SetAutoSize (Point::eVertical, Boolean (GetVerticalScrollBar () != Nil));
		fTextEdit->SetAutoSize (Point::eHorizontal, Boolean (GetHorizontalScrollBar () != Nil));
		fTextEdit->SetBorder (kZeroPoint, eNoUpdate);
		fTextEdit->SetMargin (kZeroPoint, eNoUpdate);
		fTextEdit->SetController (Nil);
		fTextEdit->SetController (this);
	}
	SetScrolledViewAndTargets (fTextEdit, fTextEdit, fTextEdit);
}

void	TextScroller::Layout ()
{
	// really need to do better
	// Lewis things we should abandon AutoSize in TextEdit entirely, in which
	// case this routine must implement all of AutoSize functionality
	if ((fTextEdit != Nil) and (not fTextEdit->GetAutoSize (Point::eHorizontal))) {
		Coordinate	newWidth = GetScrollRect ().GetSize ().GetH ();
		if (fTextEdit->GetSize ().GetH () != newWidth) {
			fTextEdit->SetSize (Point (fTextEdit->GetSize ().GetV (), newWidth));
//			fTextEdit->AdjustSize ();
fTextEdit->SetDirty ();	// hack cuz couldn't call AdjustSize directly
		}
	}
	
	Scroller::Layout ();
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

