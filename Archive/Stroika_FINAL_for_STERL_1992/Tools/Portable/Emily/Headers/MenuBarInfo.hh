/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuBarInfo.hh,v 1.6 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: MenuBarInfo.hh,v $
 *		Revision 1.6  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
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
#include "ViewItemInfo.hh"
#include "TextView.hh"
#include "PushButton.hh"


class MenuBarInfoX : public View, public ButtonController, public FocusOwner, public TextController {
	public:
		MenuBarInfoX ();
		~MenuBarInfoX ();

		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	protected:
		override	void	Layout ();

		TextEdit			fEntry;
		StringPickList		fList;
		ViewItemInfo		fViewInfo;
		TextView			fTitle;
		TextView			fEntryLabel;
		PushButton			fEditButton;
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
#include	"MenuBarItem.hh"

class MenuBarInfo : public MenuBarInfoX {
	public:
		MenuBarInfo (MenuBarItem& view);
		~MenuBarInfo ();

		nonvirtual	StringPickList&	GetListField ()
		{
			return (fList);
		}
		
		nonvirtual	ViewItemInfo&	GetViewItemInfo ()
		{
			return (fViewInfo);
		}
		
		nonvirtual	PushButton&		GetInsertButton ()
		{
			return (fInsert);
		}
		
		nonvirtual	Sequence(MenuTitleEntryPtr)&	GetMenus ();
		
	protected:
		override	void	ButtonPressed (AbstractButton* button);
		override	void	TextChanged (TextEditBase* item);

	private:	
		MenuBarItem&				fMenuBarItem;
		Sequence(MenuTitleEntryPtr)	fMenus;
};

