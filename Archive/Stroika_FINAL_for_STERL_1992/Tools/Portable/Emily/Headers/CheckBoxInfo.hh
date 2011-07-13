/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CheckBoxInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: CheckBoxInfo.hh,v $
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

#include "TextEdit.hh"
#include "CheckBox.hh"
#include "ViewItemInfo.hh"
#include "TextView.hh"


class CheckBoxInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		CheckBoxInfoX ();
		~CheckBoxInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextEdit			fLabel;
		CheckBox			fOn;
		ViewItemInfo		fViewInfo;
		TextView			fLabelLabel;
		TextView			fTitle;

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
class CheckBoxItem;
class CheckBoxInfo : public CheckBoxInfoX {
	public:
		CheckBoxInfo (CheckBoxItem& view);

		nonvirtual	CheckBox&		GetOnField ()
		{
			return (fOn);
		}
		
		nonvirtual	TextEdit&		GetLabelField ()
		{
			return (fLabel);
		}

		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
};

