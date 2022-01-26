/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PopUpInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PopUpInfo.hh,v $
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
#include "PickList.hh"
#include "TextView.hh"
#include "ViewItemInfo.hh"
#include "PushButton.hh"


class PopUpInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		PopUpInfoX ();
		~PopUpInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextEdit			fLabel;
		StringPickList		fList;
		TextView			fTitle;
		TextEdit			fEntry;
		TextView			fEntryLabel;
		TextView			fLabelLabel;
		ViewItemInfo		fViewInfo;
		PushButton			fDelete;
		PushButton			fInsert;

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
class	OptionMenuItem;

class PopUpInfo : public PopUpInfoX {
	public:
		PopUpInfo (OptionMenuItem& view);

		nonvirtual	StringPickList&	GetListField ()
		{
			return (fList);
		}
		
		nonvirtual	TextEdit&	GetLabelField ()
		{
			return (fLabel);
		}
		
		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
		
		nonvirtual	PushButton&		GetInsertButton ()
		{
			return (fInsert);
		}

	protected:
		override	void	ButtonPressed (AbstractButton* button);
		override	void	TextChanged (TextEditBase* item);
};

