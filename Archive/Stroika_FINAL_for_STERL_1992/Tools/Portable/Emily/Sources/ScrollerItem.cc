/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScrollerItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScrollerItem.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 */





#include	"StreamUtils.hh"

#include 	"CheckBox.hh"
#include	"Dialog.hh"
#include	"Scroller.hh"

#include	"CommandNumbers.hh"
#include	"ScrollerItem.hh"
#include	"ScrollerInfo.hh"
#include	"ViewItemInfo.hh"



class	SetScrollerInfoCommand : public Command {
	public:
		SetScrollerInfoCommand (ScrollerItem& item, ScrollerInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		ScrollerItem&	fItem;
		Command*		fItemInfoCommand;
		
		Point	fNewScrollSize;
		Point	fOldScrollSize;
		Boolean	fOldVSBar;
		Boolean	fNewVSBar;
		Boolean	fOldHSBar;
		Boolean	fNewHSBar;
};


/*
 ********************************************************************************
 ****************************** ScrollerItemType ********************************
 ********************************************************************************
 */
ScrollerItemType::ScrollerItemType () :
	ItemType (eBuildGroup, "Scroller", (ItemBuilderProc)&ScrollerItemBuilder)
{
	Require (sThis == Nil);
	sThis = this;
}		
		
ScrollerItemType&	ScrollerItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	ScrollerItemType::ScrollerItemBuilder ()
{
	return (new ScrollerItem ());
}

ScrollerItemType*	ScrollerItemType::sThis = Nil;




/*
 ********************************************************************************
 ****************************** ScrollerItem ************************************
 ********************************************************************************
 */
ScrollerItem::ScrollerItem () :
	GroupItem (ScrollerItemType::Get (), True),
	fHasVerticalSBar (True),
	fHasHorizontalSBar (True)
{
	SetMaxVersion (2);
}

ScrollerItem::ScrollerItem (ItemType& type) :
	GroupItem (type, True),
	fHasVerticalSBar (True),
	fHasHorizontalSBar (True)
{
	SetMaxVersion (2);
}

Boolean	ScrollerItem::GetHasVerticalSBar () const
{
	return (fHasVerticalSBar);
}

void	ScrollerItem::SetHasVerticalSBar (Boolean hasSBar)
{
	if (hasSBar != fHasVerticalSBar) {
		fHasVerticalSBar = hasSBar;
		GetGroupItemView ().SetVerticalScrollBar (hasSBar ? Scroller::kBuildDefaultSlider : Scroller::kBuildNoSlider);
	}
	Ensure (hasSBar == fHasVerticalSBar);
}

Boolean	ScrollerItem::GetHasHorizontalSBar () const
{
	return (fHasHorizontalSBar);
}

void	ScrollerItem::SetHasHorizontalSBar (Boolean hasSBar)
{
	if (hasSBar != fHasHorizontalSBar) {
		fHasHorizontalSBar = hasSBar;
		GetGroupItemView ().SetHorizontalScrollBar (hasSBar ? Scroller::kBuildDefaultSlider : Scroller::kBuildNoSlider);
	}
	Ensure (hasSBar == fHasHorizontalSBar);
}

String	ScrollerItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "Scroller.hh";
	return (kHeaderFileName);
}

void	ScrollerItem::DoRead_ (class istream& from)
{
	GroupItem::DoRead_ (from);
	
	if (GetVersion () > 1) {
		Boolean	h, v;
		from >> v >> h;
		SetHasVerticalSBar (v);
		SetHasHorizontalSBar (h);
	}
}

void	ScrollerItem::DoWrite_ (class ostream& to, int tabCount) const
{
	GroupItem::DoWrite_ (to, tabCount);
	to << tab (tabCount) << GetHasVerticalSBar () << GetHasHorizontalSBar () << newline;
}

void	ScrollerItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	GroupItem::WriteParameters (to, tabCount, language, gui);
}

void	ScrollerItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << "(";
	if (GetHasVerticalSBar ()) {
		to << "Scroller::kBuildDefaultSlider, ";
	}
	else {
		to << "Scroller::kBuildNoSlider, ";
	}
	if (GetHasHorizontalSBar ()) {
		to << "Scroller::kBuildDefaultSlider";
	}
	else {
		to << "Scroller::kBuildNoSlider";
	}
	
	to << ")," << newline;
}

SetScrollerInfoCommand::SetScrollerInfoCommand (ScrollerItem& item, ScrollerInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewScrollSize (info.GetScrollBounds ()),
	fOldScrollSize (item.GetScrollSize ()),
	fOldVSBar (item.GetHasVerticalSBar ()),
	fNewVSBar (info.GetVSBarField ().GetOn ()),
	fOldHSBar (item.GetHasHorizontalSBar ()),
	fNewHSBar (info.GetHSBarField ().GetOn ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}			

void	SetScrollerInfoCommand::DoIt ()
{
	fItemInfoCommand->DoIt ();

	fItem.SetScrollSize (fNewScrollSize);
	fItem.SetHasVerticalSBar (fNewVSBar);
	fItem.SetHasHorizontalSBar (fNewHSBar);
	
	Command::DoIt ();
}

void	SetScrollerInfoCommand::UnDoIt ()
{
	fItemInfoCommand->UnDoIt ();
	fItem.SetScrollSize (fOldScrollSize);
	fItem.SetHasVerticalSBar (fOldVSBar);
	fItem.SetHasHorizontalSBar (fOldHSBar);
	
	Command::UnDoIt ();
}

void	ScrollerItem::SetItemInfo ()
{
	ScrollerInfo info = ScrollerInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetScrollerInfoCommand (*this, info));
		DirtyDocument ();
	}
}

