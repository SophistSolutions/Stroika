/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScrollerInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScrollerInfo.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 *
 */


// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "CheckBox.hh"
#include "TextView.hh"
#include "GroupView.hh"
#include "NumberText.hh"
#include "ViewItemInfo.hh"


class ScrollerInfoX : public GroupView, public ButtonController, public FocusOwner, public TextController {
	public:
		ScrollerInfoX ();
		~ScrollerInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		CheckBox			fVerticalSBar;
		CheckBox			fHorizontalSBar;
		TextView			fTitle;
		LabeledGroup		fScrollBoundsGroup;
		FocusOwner			fScrollBoundsGroupTabLoop;
		NumberText			fScrollH;
		NumberText			fScrollV;
		TextView			fScrollHLabel;
		TextView			fScrollVLabel;
		ViewItemInfo		fViewInfo;

	private:
#if   qMacUI
		nonvirtual void	BuildForMacUI ();
#elif qMotifUI
		nonvirtual void	BuildForMotifUI ();
#else
		nonvirtual void	BuildForUnknownGUI ();
#endif /* GUI */

};



// text past here will be retained: Do not remove or modify this line!!!
class	ScrollerItem;
class ScrollerInfo : public ScrollerInfoX {
	public:
		ScrollerInfo (ScrollerItem& view);
		
		nonvirtual	Point	GetScrollBounds () const;

		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
		
		nonvirtual	CheckBox&	GetVSBarField ()
		{
			return (fVerticalSBar);
		}
		
		nonvirtual	CheckBox&	GetHSBarField ()
		{
			return (fHorizontalSBar);
		}
};

