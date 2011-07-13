/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PickListInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PickListInfo.hh,v $
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

#include "TextView.hh"
#include "TextEdit.hh"
#include "PushButton.hh"
#include "CheckBox.hh"
#include "ViewItemInfo.hh"
#include "PickList.hh"


class StringPickListInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		StringPickListInfoX ();
		~StringPickListInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fEntryLabel;
		TextEdit			fEntry;
		TextView			fTitle;
		PushButton			fInsert;
		CheckBox			fMultipleSelections;
		CheckBox			fFloatingSelection;
		ViewItemInfo		fViewInfo;
		StringPickList		fList;
		PushButton			fDelete;

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
class	PickListEditItem;

class StringPickListInfo : public StringPickListInfoX {
	public:
		StringPickListInfo (PickListEditItem& view);

		nonvirtual	StringPickList&	GetListField ()
		{
			return (fList);
		}
		
		nonvirtual	CheckBox&	GetMultipleSelectionsField ()
		{
			return (fMultipleSelections);
		}
		
		nonvirtual	CheckBox&	GetFloatingSelectionField ()
		{
			return (fFloatingSelection);
		}
		
		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
		
		nonvirtual	PushButton&	GetInsertButton ()
		{
			return (fInsert);
		}
				
	protected:
		override	void	ButtonPressed (AbstractButton* button);
		override	void	TextChanged (TextEditBase* item);
};

