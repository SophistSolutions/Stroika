/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ItemPallet.cc,v 1.8 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ItemPallet.cc,v $
 *		Revision 1.8  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.16  1992/02/04  07:25:43  lewis
 *		Added CascadeMenuItem and CheckBoxMenuItem includes.
 *
 *		Revision 1.14  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *
 *
 */





#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"CascadeMenuItem.hh"
#include	"CheckBoxMenuItem.hh"
#include	"DeskTop.hh"
#include	"PixelMapButton.hh"
#include	"MenuOwner.hh"
#include	"PalletMenu.hh"
#include	"PopUpMenu.hh"

#include	"EmilyApplication.hh"
#include	"ItemPallet.hh"


#if		!qRealTemplatesAvailable
	Implement (Iterator, CommandNumber);
	Implement (Collection, CommandNumber);
	Implement (AbSequence, CommandNumber);
	Implement (Array, CommandNumber);
	Implement (Sequence_Array, CommandNumber);
	Implement (Sequence, CommandNumber);
#endif


#if qUseCustomMenu	
	#if		!qRealTemplatesAvailable
		Implement (Iterator, IPBPtr);
		Implement (Collection, IPBPtr);
		Implement (AbSequence, IPBPtr);
		Implement (Array, IPBPtr);
		Implement (Sequence_Array, IPBPtr);
		Implement (Sequence, IPBPtr);
	#endif
#endif /* qUseCustomMenu */		







/*static*/	String	sSelectedString = kEmptyString;




#if qUseCustomMenu

/*
 ********************************************************************************
 ********************************** ItemPalletButton ****************************
 ********************************************************************************
 */
class	ItemPalletButton : public PixelMapButton {
	public:
		ItemPalletButton (const PixelMap& pixelMap, CommandNumber command);
	
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		nonvirtual	void	PickItem (const MousePressInfo& mouseInfo);
		
	public:
		Sequence_Array(String) 	fStrings;
		CommandNumber			fCommand;
};

ItemPalletButton::ItemPalletButton (const PixelMap& pixelMap, CommandNumber command):
	PixelMapButton (pixelMap),
	fCommand (command)
{
	SetBorder (1, 1, eNoUpdate);
	SetMargin (1, 1, eNoUpdate);
}

Boolean	ItemPalletButton::TrackPress (const MousePressInfo& mouseInfo)
{
	if (GetLive ()) {
		PickItem (mouseInfo);
		GetParentView ()->InvalidateLayout ();
		Application::Get ().ProcessCommand (CommandSelection (fCommand));
	}
	return (True);
}

void	ItemPalletButton::PickItem (const MousePressInfo& mouseInfo)
{
	if (fStrings.GetLength () == 0) {
		sSelectedString  = kEmptyString;
	}
	else {
		PopUpMenuTitle	popUp (kEmptyString, Nil);
		ForEach (String, it, fStrings) {
			popUp.GetMenu ()->AddStringMenuItem (CommandHandler::ePopupItem, it.Current ());
		}
		AddSubView (&popUp);
		popUp.SetSize (popUp.CalcDefaultSize (kZeroPoint), eNoUpdate);
		(void)popUp.TrackPress (mouseInfo.fPressAt);
		RemoveSubView (&popUp);
		MenuItem* mItem = popUp.GetCurrentItem ();
		if (mItem != Nil) {
			sSelectedString = mItem->GetName ();
		}
		else {
			sSelectedString = fStrings[1];
		}
	}
}

#endif /* qUseCustomMenu */


/*
 ********************************************************************************
 ************************************* ItemPallet *******************************
 ********************************************************************************
 */
ItemPallet*	ItemPallet::sThis = Nil;
Sequence_Array(String)			ItemPallet::sStrings;
Sequence_Array(CommandNumber)	ItemPallet::sCommands;
MenuItem*						ItemPallet::sSelectedItem = Nil;

#if !qUseCustomMenu		
CommandSelection	sPalletSelection = CommandSelection (0);
#endif

ItemPallet::ItemPallet (Window* w)
#if qUseCustomMenu
	 : TearOffMenu (w)
#endif
{
if (sThis == Nil) {
	Require (sThis == Nil);
	
	sThis = this;
}
	BuildPalletItems ();
	SetPalletSelection (eArrow);

	EnsureNotNil (sThis);
}

ItemPallet::~ItemPallet ()
{
}

Boolean	ItemPallet::EndUpdate ()
{
#if 	qUseCustomMenu
	Boolean result = TearOffMenu::EndUpdate ();
	View* mainView = GetTearOffWindow ().GetMainView ();
	if ((mainView != Nil) and (GetTearOffWindow ().GetVisible ())) {
		mainView->InvalidateLayout ();
	}
#else
	Boolean result = Menu::EndUpdate ();
#endif
	
	return (result);
}

MenuItem*	ItemPallet::GetSelectedItem () const
{
	AssertNotNil (sSelectedItem);
	return (sSelectedItem);
}

void	ItemPallet::SetSelectedItem (MenuItem* b, Panel::UpdateMode updateMode)
{
	AssertNotNil (sThis);
	sThis->SelectItem (b, updateMode);	
}

void	ItemPallet::SelectItem (MenuItem* item, Panel::UpdateMode updateMode)
{
	if (item == Nil) {
		// pallets always have some item selected
		item = GetSelectedItem ();
	}
#if		qUseCustomMenu
	else {
		ItemPalletButton* current = Nil;
		ForEach (ItemPalletButtonPtr, it, fItems) {
			if ((it.Current ()->fCommand == item->GetCommandNumber ()) and (it.Current ()->GetLive ())) {
				current = it.Current ();
				break;
			}
		}
		
		if (current != Nil) {	// could be Nil if disabled
			if (GetTearOffWindow ().GetMainView () == this) {
				Point	where = current->AncestorToLocal (Application::Get ().GetMouseLocation (), &DeskTop::Get ());
				current->TrackPress (where);
			}
			else {
				if (current->fStrings.GetLength () > 0) {
					sSelectedString = current->fStrings[1];
				}
				else {
					sSelectedString = kEmptyString;
				}
			}
		}
		else {
			item = GetSelectedItem ();
		}
	}
#endif /* qUseCustomMenu */


	sSelectedItem = item;
#if		qUseCustomMenu
	View* mainView = GetTearOffWindow ().GetMainView ();
	if ((mainView != Nil) and (GetTearOffWindow ().GetVisible ())) {
		mainView->InvalidateLayout ();
		mainView->Update ();
	}
#endif /* qUseCustomMenu */
	Menu::SelectItem (item, updateMode);
}

void	ItemPallet::SetPalletSelection (CommandNumber selection, Panel::UpdateMode update)
{
	AssertNotNil (sThis);
#if !qUseCustomMenu		
	sPalletSelection = selection;
#endif
	sThis->SetSelectedItem (sThis->GetMenuItemByCommand (selection), update);
}

CommandNumber	ItemPallet::GetPalletSelection ()
{
	AssertNotNil (sThis);
#if !qUseCustomMenu		
	return (sPalletSelection.GetCommandNumber ());
#endif
	return (sThis->GetSelectedItem ()->GetCommandNumber ());
}

String	ItemPallet::GetSelectedString ()
{
	return (sSelectedString);
}

Boolean		ItemPallet::GetEditMode ()
{
	return (Boolean (GetPalletSelection () != eThumb));
}

void	ItemPallet::SetEditMode (Boolean newEditMode)
{
	if (newEditMode != GetEditMode ()) {
		if (newEditMode) {
			SetPalletSelection (eArrow);
		}
		else {
			SetPalletSelection (eThumb);
		}
	}
	Ensure (newEditMode == GetEditMode ());
}

void	ItemPallet::AddPalletItem (CommandNumber command, const String& itemName)
{
	if (sThis != Nil) {
		sThis->AddPalletSubItem (command, itemName);
	}
	else {
		// we allow this function to be called at startup (by ItemTypes) and we do not
		// wish to guarantee construction order, since this would be awkward for people
		// adding new item types. Instead keep lists of everything added, and stick them
		// in the pallet when we finally get around to building it.
		sStrings.Append (itemName);
		sCommands.Append (command);
	}
}

Boolean	ItemPallet::ShouldEnable (CommandNumber command)
{
	if ((not GetEditMode ()) or EmilyWindow::GetCustomizeOnly ()) {
		return (False);
	}
	
#if qUseCustomMenu
	ForEach (ItemPalletButtonPtr, it, ItemPallet::Get ().fItems) {
		if (it.Current ()->fCommand == command) {
			return (Boolean (it.Current ()->fStrings.GetLength () > 0));
		}
	}
#else
	return (True);
#endif /* qUseCustomMenu */
	AssertNotReached (); return (False);
}

ItemPallet& ItemPallet::Get ()
{
	return (*sThis);
}

void	ItemPallet::BuildPalletItems ()
{
#if qUseCustomMenu
	const	Rect	kSICNRect = Rect (kZeroPoint, Point (16, 16));
	
	static	UInt16	kARROW_Data [] = {
		0x0000, 0x0800, 0x0C00, 0x0E00, 0x0F00, 0x0F80, 0x0FC0, 0x0FE0,
		0x0FF0, 0x0F80, 0x0D80, 0x08C0, 0x00C0, 0x0060, 0x0060, 0x0000,
	};
	
	static	UInt16	kTHUMB_Data [] = {
		0x0000, 0x0300, 0x0480, 0x0480, 0x0480, 0x0480, 0x64F8, 0x94AC,
		0x4CAA, 0x240A, 0x2002, 0x1002, 0x0804, 0x0804, 0x0404, 0x0404,
	};
	
	static	UInt16	kPUSHBUTTON_Data [] = {
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFE, 0x8001, 0x84A1,
		0x8AC1, 0x8AC1, 0x84A1, 0x8001, 0x7FFE, 0x0000, 0x0000, 0x0000,
	};
	
	static	UInt16	kEDITTEXT_Data [] = {
		0xFFFF, 0x8001, 0x8181, 0x8181, 0x83C1, 0x82C1, 0x82E1, 0x86E1,
		0x8471, 0x8FF1, 0x8839, 0x9839, 0x901D, 0xB83F, 0x8001, 0xFFFF,
	};
	
	static	UInt16	kSTATICTEXT_Data [] = {
		0x0000, 0x0000, 0x0180, 0x0180, 0x03C0, 0x02C0, 0x02E0, 0x06E0,
		0x0470, 0x0FF0, 0x0838, 0x1838, 0x101C, 0x383E, 0x0000, 0x0000, 
	};
	
	static	UInt16	kSCROLLBAR_Data [] = {
		0x03C0, 0x03C0, 0x03C0, 0x0240, 0x0240, 0x0240, 0x03C0, 0x0240,
		0x03C0, 0x0240, 0x0240, 0x0240, 0x0240, 0x03C0, 0x03C0, 0x03C0,
	};
		
	static	UInt16	kPOPUP_Data [] = {
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFE, 0x8003,
		0x8003, 0x8003, 0xFFFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000,
	};
	
	static	UInt16	kPICKLIST_Data [] = {
		0xFFFF, 0x8009, 0x8009, 0xFFFF, 0x8009, 0x8009, 0xFFF9, 0x8009,
		0x8009, 0xFFF9, 0x8009, 0x8009, 0xFFFF, 0x8009, 0x8009, 0xFFFF,
	};
	
	static	UInt16	kGROUP_Data [] = {
		0x0000, 0x1500, 0xD57F, 0x9501, 0x8001, 0x8001, 0x8001, 0x8001,
		0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0xFFFF, 0x0000, 0x0000,
	};
	
	static	UInt16	kLINEVIEW_Data [] = {
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xAAAB,
		0xD555, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	};

	static	UInt16	kUSER1_Data [] = {
		0x00C0, 0x03C0, 0x0FC0, 0x0FC0, 0x01C0, 0x01C0, 0x01C0, 0x01C0,
		0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x0FF8, 0x0FF8, 0x0000,
	};

	static	UInt16	kUSER2_Data [] = {
		0x0000, 0x07E0, 0x0FF0, 0x0C30, 0x0C30, 0x0030, 0x0070, 0x00E0,
		0x01C0, 0x0380, 0x0700, 0x0E00, 0x0C30, 0x0FF0, 0x0FF0, 0x0000,
	};

	static	UInt16	kUSER3_Data [] = {
		0x0000, 0x07E0, 0x0FF0, 0x0C30, 0x0C60, 0x00C0, 0x0180, 0x03E0,
		0x03F0, 0x0030, 0x0030, 0x0030, 0x0C30, 0x0FF0, 0x07E0, 0x0000,
	};

	static	UInt16	kUSER4_Data [] = {
		0x0000, 0x0060, 0x00E0, 0x01E0, 0x0360, 0x0660, 0x0C60, 0x0FF0,
		0x0FF0, 0x0060, 0x0060, 0x0060, 0x0060, 0x00F0, 0x00F0, 0x0000,
	};

	AbstractButtonMenuItem*	item = Nil;
	ItemPalletButton*	button = Nil;
	
	button = new ItemPalletButton (PixelMap (kSICNRect, kARROW_Data), eArrow);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to use 'SELECT' mode.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kTHUMB_Data), eThumb);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to use 'RUN' mode.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kSCROLLBAR_Data), eBuildSlider);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of sliders by "
				   "clicking and dropping, or dragging an outline in the main window.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kPUSHBUTTON_Data), eBuildButton);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of buttons by "
				   "clicking and dropping, or dragging an outline in the main window.");
	
	button = new ItemPalletButton (PixelMap (kSICNRect, kEDITTEXT_Data), eBuildTextEdit);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of editable text items by "
				   "clicking and dropping, or dragging an outline in the main window.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kSTATICTEXT_Data), eBuildTextView);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of static text items by "
				   "clicking and dropping, or dragging an outline in the main window.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kPOPUP_Data), eBuildMenu);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of menus by "
				   "clicking and dropping, or dragging an outline in the main window.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kPICKLIST_Data), eBuildPickList);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of picklists by "
				   "clicking and dropping, or dragging an outline in the main window.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kGROUP_Data), eBuildGroup);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of groups by "
				   "clicking and dropping, or dragging an outline in the main window.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kLINEVIEW_Data), eBuildLine);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of a seperating line by "
				   "clicking and dropping, or dragging an outline in the main window.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kUSER1_Data), eBuildUser1);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of user defined views by"
				   "clicking and dropping, or dragging an outline in the main window.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kUSER2_Data), eBuildUser2);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of user defined views by"
				   "clicking and dropping, or dragging an outline in the main window.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kUSER3_Data), eBuildUser3);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of user defined views by"
				   "clicking and dropping, or dragging an outline in the main window.");

	button = new ItemPalletButton (PixelMap (kSICNRect, kUSER4_Data), eBuildUser4);
	fItems.Append (button);
	item = AddButtonMenuItem (button, button->fCommand);
	button->SetHelp ("Choose this to allow creation of user defined views by"
				   "clicking and dropping, or dragging an outline in the main window.");

	CollectionSize	total = sStrings.GetLength ();
	Require (total == sCommands.GetLength ());
	for (CollectionSize count = 1; count <= total; count++) {
		AddPalletSubItem (sCommands[count], sStrings[count]);
	}
#else
	AddCheckBoxMenuItem (eArrow)->SetDefaultName ("Select Mode");
	AddCheckBoxMenuItem (eThumb)->SetDefaultName ("Live Mode");
	AddSeparatorMenuItem ();

	BuildPalletItem (eBuildSlider, "Slider");
	BuildPalletItem (eBuildButton, "Button");
	BuildPalletItem (eBuildTextEdit, "Text Edit");
	BuildPalletItem (eBuildTextView, "Text View");
	BuildPalletItem (eBuildMenu, "Menu");
	BuildPalletItem (eBuildPickList, "Grid");
	BuildPalletItem (eBuildGroup, "Group");
	BuildPalletItem (eBuildLine, "Graphics");
	BuildPalletItem (eBuildUser1, "User One");
	BuildPalletItem (eBuildUser2, "User Two");
	BuildPalletItem (eBuildUser3, "User Three");
	BuildPalletItem (eBuildUser4, "User Four");

#endif /* qUseCustomMenu */
}

#if !qUseCustomMenu
void	ItemPallet::BuildPalletItem (CommandNumber command, const String& name)
{
	Menu*	menu = Nil;
	CollectionSize	total = sStrings.GetLength ();
	
	MenuItem* item = AddCascadeMenuItem (menu = new Menu (), name);
	for (CollectionSize count = 1; count <= total; count++) {
		if (sCommands[count] == command) {
			menu->AddCheckBoxMenuItem (command)->SetDefaultName (sStrings[count]);
		}
	}
}
#endif

void	ItemPallet::AddPalletSubItem (CommandNumber command, const String& itemName)
{
#if qUseCustomMenu
	ForEach (ItemPalletButtonPtr, it, fItems) {
		if (it.Current ()->fCommand == command) {
			it.Current ()->fStrings.Append (itemName);
			return;
		}
	}
#endif /* qUseCustomMenu */
	AssertNotReached ();
}



#if qUseCustomMenu
Menu*	ItemPallet::BuildTornOffView ()
{
	Window& w = GetTearOffWindow ();
	ItemPallet* pallet = new ItemPallet (&w);
	return (pallet);
}

void	ItemPallet::Layout ()
{
	Point	itemOrigin	=	kZeroPoint;
	Coordinate	maxWidth = 40;
	ForEach (MenuItemPtr, it, MakeMenuItemIterator ()) {
		register MenuItem* button	=	it.Current ();
		AssertNotNil (button);
		button->SetOrigin (itemOrigin, eNoUpdate);
		button->SetSize (button->CalcDefaultSize (button->GetSize ()), eNoUpdate);
		if (button->GetExtent ().GetRight () >= maxWidth) {
			itemOrigin = Point (itemOrigin.GetV () + button->GetSize ().GetV (), 0);
		}
		else {
			itemOrigin += Point (0, button->GetSize ().GetH ());
		}
	}
	Boolean	canEdit = Boolean (GetEditMode () and (not EmilyWindow::GetCustomizeOnly ()));
	ForEach (ItemPalletButtonPtr, it1, fItems) {
		register ItemPalletButton* button	=	it1.Current ();
		button->SetEnabled (Boolean (
			((button->fStrings.GetLength () > 0) and (canEdit)) or  
			(button->fCommand == eArrow) or 
			(button->fCommand == eThumb)));
		button->SetOn (Boolean (GetPalletSelection () == button->fCommand));
	}
	View::Layout ();
}

Point	ItemPallet::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	((ItemPallet*)this)->Layout ();
	Rect extent = kZeroRect;
	ForEach (MenuItemPtr, it, MakeMenuItemIterator ()) {
		extent = BoundingRect (extent, it.Current ()->GetExtent ());
	}
	return (extent.GetSize ());
}
#endif






