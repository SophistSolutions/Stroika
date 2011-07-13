/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PushButtonInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PushButtonInfo.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!

#include "FocusItem.hh"
#include "TextEdit.hh"
#include "View.hh"

#include "TextView.hh"
#include "TextEdit.hh"
#include "ViewItemInfo.hh"


class PushButtonInfoX : public View, public FocusOwner, public TextController {
	public:
		PushButtonInfoX ();
		~PushButtonInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		TextEdit			fLabel;
		ViewItemInfo		fViewInfo;
		TextView			fField1;

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
class	PushButtonItem;

class PushButtonInfo : public PushButtonInfoX {
	public:
		PushButtonInfo (PushButtonItem& view);

		nonvirtual	TextEdit&	GetLabelField ()
		{
			return (fLabel);
		}
		
		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
};

