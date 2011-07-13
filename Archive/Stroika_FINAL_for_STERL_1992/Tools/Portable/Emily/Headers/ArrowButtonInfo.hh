/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ArrowButtonInfo.hh,v 1.2 1992/07/21 18:33:08 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ArrowButtonInfo.hh,v $
 *		Revision 1.2  1992/07/21  18:33:08  sterling
 *		added
 *
# Revision 1.1  1992/07/16  15:27:55  sterling
# hi
#
 *		
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!

#include "Button.hh"
#include "FocusItem.hh"
#include "View.hh"

#include "TextView.hh"
#include "GroupView.hh"
#include "RadioButton.hh"
#include "ViewItemInfo.hh"


class ArrowButtonInfoX : public View, public ButtonController, public FocusOwner {
	public:
		ArrowButtonInfoX ();
		~ArrowButtonInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		LabeledGroup		fDirection;
		RadioBank			fDirectionRadioBank;
		FocusOwner			fDirectionTabLoop;
		RadioButton			fRight;
		RadioButton			fDown;
		RadioButton			fLeft;
		RadioButton			fUp;
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
class	ArrowButtonItem;

class ArrowButtonInfo : public ArrowButtonInfoX {
	public:
		ArrowButtonInfo (ArrowButtonItem& view);

		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
		
		nonvirtual	AbstractArrowButton::ArrowDirection	GetDirection () const;
};

