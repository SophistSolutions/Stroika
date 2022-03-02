/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuTitleInfo.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: MenuTitleInfo.hh,v $
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
#include "PickList.hh"
#include "GroupView.hh"
#include "CheckBox.hh"
#include "PushButton.hh"


class MenuTitleInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		MenuTitleInfoX ();
		~MenuTitleInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextView			fTitle;
		TextEdit			fName;
		StringPickList		fList;
		GroupView			fEntryGroup;
		FocusOwner			fEntryGroupTabLoop;
		TextEdit			fAccelerator;
		TextEdit			fMneumonic;
		TextEdit			fCommand;
		TextEdit			fLabel;
		CheckBox			fCheckBox;
		CheckBox			fExtended;
		TextView			fCommandLabel;
		TextView			fAcceleratorLabel;
		TextView			fMneumonicLabel;
		TextView			fLabelLabel;
		TextView			fField1;
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
#include "MenuBarItem.hh"

class MenuTitleInfo : public MenuTitleInfoX {
	public:
		MenuTitleInfo (const MenuTitleEntry& entry);

		nonvirtual	TextEdit&	GetNameField ()
		{
			return (fName);
		}
		
		nonvirtual	PushButton&	GetInsertButton ()
		{
			return (fInsert);
		}
		
		nonvirtual	const MenuTitleEntry&	GetMenu () const;
		
	protected:
		override	void	ButtonPressed (AbstractButton* button);
		override	void	TextChanged (TextEditBase* item);

	private:	
		nonvirtual	void	ClearFields ();

		MenuTitleEntry	fMenu;
};

