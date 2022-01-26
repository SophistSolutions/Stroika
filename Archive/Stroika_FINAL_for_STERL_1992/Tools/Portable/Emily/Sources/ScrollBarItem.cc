/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScrollBarItem.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScrollBarItem.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"ScrollBar.hh"

#include	"ScrollBarItem.hh"
#include	"CommandNumbers.hh"




/*
 ********************************************************************************
 ****************************** ScrollBarItemType *******************************
 ********************************************************************************
 */
ScrollBarItemType::ScrollBarItemType () :
#if		qHP_BadTypedefForStaticProcs
	ItemType (eBuildSlider, "ScrollBar", (ItemBuilderProc)&ScrollBarItemBuilder)
#else
	ItemType (eBuildSlider, "ScrollBar", &ScrollBarItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
ScrollBarItemType&	ScrollBarItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	ScrollBarItemType::ScrollBarItemBuilder ()
{
	return (new ScrollBarItem (Get ()));
}

ScrollBarItemType*	ScrollBarItemType::sThis = Nil;


static	AbstractScrollBar*	BuildScrollBar (CommandNumber gui, AbstractScrollBar::Orientation orientation)
{
	if (gui == eMacUI) {
		return (new ScrollBar_MacUI (orientation, Nil));
	}
	else if (gui == eMotifUI) {
		return (new ScrollBar_MotifUI (orientation, Nil));
	}
	else if (gui == eWindowsGUI) {
		return (new ScrollBar_WinUI (orientation, Nil));
	}
	AssertNotReached ();	return (Nil);
}



/*
 ********************************************************************************
 ****************************** ScrollBarItem ***********************************
 ********************************************************************************
 */
ScrollBarItem::ScrollBarItem (ItemType& type) :
	SliderItem (type),
	fScrollBar (Nil),
	fDefaultStepSize (0),
	fDefaultPageSize (0)
{
	SetScrollBar (BuildScrollBar (EmilyWindow::GetGUI (), AbstractScrollBar::eVertical));
	
	GetScrollBar ().SetBounds (0, 100, 0, eNoUpdate);
	GetScrollBar ().SetSteps (1, 10, eNoUpdate);
	GetScrollBar ().SetDisplayed (5, eNoUpdate);
}

AbstractScrollBar&	ScrollBarItem::GetScrollBar () const
{
	AssertNotNil (fScrollBar);
	return (*fScrollBar);
}

void	ScrollBarItem::SetScrollBar (AbstractScrollBar* scrollBar)
{
	SetSlider (scrollBar);
	fScrollBar = scrollBar;
	if (fScrollBar != Nil) {
		fDefaultStepSize = GetScrollBar ().GetStepSize ();
		fDefaultPageSize = GetScrollBar ().GetPageSize ();
	}
}

void	ScrollBarItem::GUIChanged (CommandNumber /*oldGUI*/, CommandNumber newGUI)
{
	SetScrollBar (BuildScrollBar (newGUI, GetScrollBar ().GetOrientation ()));
	ApplyCurrentParams ();
}


void	ScrollBarItem::Layout ()
{
	SliderItem::Layout ();
	Rect r = GetScrollBar ().GetExtent ();
	if (r.GetHeight () > r.GetWidth ()) {
		GetScrollBar ().SetOrientation (AbstractScrollBar::eVertical);
		GetScrollBar ().SetDisplayed (Min (Max (2000.0 / r.GetHeight (), 10.0), 100.0), eNoUpdate);
	}
	else {
		GetScrollBar ().SetOrientation (AbstractScrollBar::eHorizontal);
		GetScrollBar ().SetDisplayed (Min (Max (2000.0 / r.GetWidth (), 10.0), 100.0), eNoUpdate);
	}
}

String	ScrollBarItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "ScrollBar.hh";
	return (kHeaderFileName);
}

void	ScrollBarItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	ScrollBarItem::DoRead_ (class istream& from)
{
	SliderItem::DoRead_ (from);
	Real	stepSize;
	Real	pageSize;
	from >> ReadReal (stepSize) >> ReadReal (pageSize);
	GetScrollBar ().SetSteps (stepSize, pageSize, eNoUpdate);
}

void	ScrollBarItem::DoWrite_ (class ostream& to, int tabCount) const
{
	SliderItem::DoWrite_ (to, tabCount);
	to << tab (tabCount) << WriteReal (GetScrollBar ().GetStepSize ()) << ' ' << WriteReal (GetScrollBar ().GetPageSize ());
	to << newline;
}

void	ScrollBarItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	SliderItem::WriteParameters (to, tabCount, language, gui);
	if ((fDefaultStepSize != GetScrollBar ().GetStepSize ()) or (fDefaultPageSize != GetScrollBar ().GetPageSize ())) {
		to << tab (tabCount) << GetFieldName () << ".SetSteps (" << 
			GetScrollBar ().GetStepSize () << ", " << GetScrollBar ().GetPageSize () << ", eNoUpdate);" << newline;
	}
}

#include "ScrollBarInfo.hh"

void	ScrollBarItem::SetItemInfo ()
{
	ScrollBarInfo info = ScrollBarInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetScrollBarInfoCommand (*this, info));
		DirtyDocument ();
	}
}

SetScrollBarInfoCommand::SetScrollBarInfoCommand (ScrollBarItem& item, class ScrollBarInfo& info) :
	SetSliderInfoCommand (
		item, 
		info.GetMinValueField ().GetValue (),
		info.GetMaxValueField ().GetValue (),
		info.GetValueField ().GetValue (),
		info.GetViewItemInfo ()),
	fScrollBar (item),
	fNewStepSize (info.GetStepSizeField ().GetValue ()),
	fOldStepSize (item.GetScrollBar ().GetStepSize ()),
	fNewPageSize (info.GetPageSizeField ().GetValue ()),
	fOldPageSize (item.GetScrollBar ().GetPageSize ()),
	fOldOrientation (item.GetScrollBar ().GetOrientation ()),
	fNewOrientation (info.GetOrientation ())
{
}
		
void	SetScrollBarInfoCommand::DoIt ()
{
	AbstractScrollBar& sb = fScrollBar.GetScrollBar ();
	sb.SetSteps (fNewStepSize, fNewPageSize);
	SetSliderInfoCommand::DoIt ();
	if (sb.GetOrientation () != fNewOrientation) {
		sb.SetOrientation (fNewOrientation);
		fScrollBar.SetItemSize (Point (fScrollBar.GetItemSize ().GetH (), fScrollBar.GetItemSize ().GetV ()));
		fScrollBar.ApplyCurrentParams ();
	}
}

void	SetScrollBarInfoCommand::UnDoIt ()
{
	AbstractScrollBar& sb = fScrollBar.GetScrollBar ();
	sb.SetSteps (fOldStepSize, fOldPageSize);
	sb.SetOrientation (fOldOrientation);
	SetSliderInfoCommand::UnDoIt ();
}
