/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuBarItem.cc,v 1.7 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: MenuBarItem.cc,v $
 *		Revision 1.7  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *
 *
 */





#include	"Format.hh"
#include	"StreamUtils.hh"

#include	"MenuBar.hh"
#include	"PullDownMenu.hh"
#include	"CheckBoxMenuItem.hh"
#include	"SeparatorMenuItem.hh"
#include	"StringMenuItem.hh"

#include	"EmilyWindow.hh"
#include	"ItemPallet.hh"
#include	"MenuBarItem.hh"
#include	"MenuBarInfo.hh"
#include	"ViewItemInfo.hh"
#include	"CommandNumbers.hh"
#include	"Dialog.hh"
#include	"PickList.hh"

#if		!qRealTemplatesAvailable
	Implement (Iterator, MIEPtr);
	Implement (Collection, MIEPtr);
	Implement (AbSequence, MIEPtr);
	Implement (Array, MIEPtr);
	Implement (Sequence_Array, MIEPtr);
	Implement (Sequence, MIEPtr);
#endif

#if		!qRealTemplatesAvailable
	Implement (Iterator, MTEPtr);
	Implement (Collection, MTEPtr);
	Implement (AbSequence, MTEPtr);
	Implement (Array, MTEPtr);
	Implement (Sequence_Array, MTEPtr);
	Implement (Sequence, MTEPtr);
#endif

/*
 ********************************************************************************
 ********************************* MenuBarItemType ******************************
 ********************************************************************************
 */
MenuBarItemType::MenuBarItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildMenu, "MenuBar", (ItemBuilderProc)&MenuBarItemBuilder)
#else
	ItemType (eBuildMenu, "MenuBar", &MenuBarItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
MenuBarItemType&	MenuBarItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	MenuBarItemType::MenuBarItemBuilder ()
{
	return (new MenuBarItem (Get ()));
}

MenuBarItemType*	MenuBarItemType::sThis = Nil;



/*
 ********************************************************************************
 ********************************* MyMenuBar ************************************
 ********************************************************************************
 */
class	MyMenuBar : public MenuBar {
	public:
		MyMenuBar ();
		
		nonvirtual	void	DeleteAllMenus ();
};

MyMenuBar::MyMenuBar () :
	MenuBar ()
{
}

void	MyMenuBar::DeleteAllMenus ()
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		MenuTitle* current = it.Current ();
		DeleteMenuTitle (current);
	}
}


/*
 ********************************************************************************
 ******************************** MenuBarItem ***********************************
 ********************************************************************************
 */
MenuBarItem::MenuBarItem (ItemType& type) :
	ViewItem (type)
{
	SetOwnedView (fMenuBar = new MyMenuBar ());
}

MenuBarItem::~MenuBarItem ()
{
	fMenuBar->DeleteAllMenus ();
}

String	MenuBarItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "MenuBar.hh";
	return (kHeaderFileName);
}

Boolean	MenuBarItem::ItemCanBeEnabled ()
{
	return (False);
}

Boolean	MenuBarItem::IsButton ()
{
	return (False);
}

Boolean	MenuBarItem::IsSlider ()
{
	return (False);
}

Boolean	MenuBarItem::IsText ()
{
	return (False);
}

Boolean	MenuBarItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (False);
}

String	MenuBarItem::CalcMenuName (const String& menuTitle)
{
	return (GetFieldName () + menuTitle + "Menu");
}

void	MenuBarItem::WriteDeclaration (class ostream& to, int tabCount)
{
	ViewItem::WriteDeclaration (to, tabCount);
	ForEach (MenuTitleEntryPtr, it, fMenus) {
		DeclareField (to, tabCount, "Menu", CalcMenuName (it.Current ()->GetTitle ()));
	}
}

void	MenuBarItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	MenuBarItem::WriteDestructor (class ostream& to, int tabCount, CommandNumber gui)
{
	ViewItem::WriteDestructor (to, tabCount, gui);
	ForEach (MenuTitleEntryPtr, it, fMenus) {
		MenuTitleEntry* current = it.Current ();
		const	String	menuName = CalcMenuName (current->GetTitle ());
		to << tab (tabCount) << GetFieldName () << ".RemoveMenu (&" << menuName << ", \"" << current->GetTitle () << "\");" << newline;
	}
}

void	MenuBarItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
	CollectionSize length;
	from >> length;
MyMenuBar* newMenuBar = new MyMenuBar ();
	for (CollectionSize count = 1; count <= length; count++) {
		MenuTitleEntry* entry = new MenuTitleEntry ();
		entry->DoRead (from);
		fMenus.Append (entry);
		newMenuBar->AddMenu (entry->BuildMenu (), entry->GetTitle ());
	}
SetOwnedView (fMenuBar = newMenuBar);
}

void	MenuBarItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
	CollectionSize length = fMenus.GetLength ();
	to << tab (tabCount) << length << newline;
	ForEach (MenuTitleEntryPtr, it, fMenus) {
		it.Current ()->DoWrite (to, tabCount);
	}
}

void	MenuBarItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
	tabCount++;
	ForEach (MenuTitleEntryPtr, it, fMenus) {
		MenuTitleEntry* current = it.Current ();
		const	String	menuName = CalcMenuName (current->GetTitle ());
		current->WriteCode (to, tabCount+1, language, gui, menuName);
		to << tab (tabCount) << GetFieldName () << ".AddMenu (&" << menuName << ", \"" << current->GetTitle () << "\");" << newline;
	}
	tabCount--;
}

void	MenuBarItem::EditModeChanged (Boolean newEditMode)
{
	ViewItem::EditModeChanged (newEditMode);
}

void	MenuBarItem::SetItemInfo ()
{
	MenuBarInfo info = MenuBarInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (&info.GetInsertButton ());
	
	if (d.Pose ()) {
MyMenuBar* newMenuBar = new MyMenuBar ();
		ForEach (MenuTitleEntryPtr, it, fMenus) {
			MenuTitleEntry* current = it.Current ();
			delete current;
		}
		fMenus.RemoveAll ();
	       		
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitleEntryPtr, it1,info. GetMenus ().operator Iterator(MenuTitleEntryPtr)* ()) {
#else
	ForEach (MenuTitleEntryPtr, it1, info.GetMenus ()) {
#endif
			fMenus.Append (new MenuTitleEntry (*it1.Current ()));
			newMenuBar->AddMenu (it1.Current ()->BuildMenu (), it1.Current ()->GetTitle ());
		}
fMenuBar->DeleteAllMenus ();
SetOwnedView (fMenuBar = newMenuBar);
		
		Ensure (fMenus.GetLength () == info.GetMenus ().GetLength ());

		Refresh ();
		PostCommand (new SetItemInfoCommand (*this, info.GetViewItemInfo ()));
		
		DirtyDocument ();
	}
}


/*
 ********************************************************************************
 ********************************* MenuItemEntry ********************************
 ********************************************************************************
 */

MenuTitleEntry::MenuTitleEntry () :
	Saveable (1),
	fTitle (kEmptyString)
{
}

MenuTitleEntry::MenuTitleEntry (const String title) :
	Saveable (1),
	fTitle (title)
{
}

MenuTitleEntry::MenuTitleEntry (const MenuTitleEntry& entry) :
	Saveable (1),
	fTitle (entry.GetTitle ())
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuItemEntryPtr, it, entry.GetItems ().operator Iterator(MenuItemEntryPtr)* ()) {
#else
	ForEach (MenuItemEntryPtr, it, entry.GetItems ()) {
#endif
		fItems.Append (new MenuItemEntry (*it.Current ()));
	}
}

MenuTitleEntry::~MenuTitleEntry ()
{
	ForEach (MenuItemEntryPtr, it, fItems) {
		MenuItemEntry* current = it.Current ();
		delete current;
	}
}

String		MenuTitleEntry::GetTitle () const
{
	return (fTitle);
}

const AbSequence(MenuItemEntryPtr)&	MenuTitleEntry::GetItems () const
{
	return (fItems);
}

void		MenuTitleEntry::SetTitle (const String& title)
{
	fTitle = title;
}

void	MenuTitleEntry::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	ReadString (from, fTitle);
	CollectionSize index;
	from >> index;
	for (CollectionSize count = 1; count <= index; count++) {
		MenuItemEntry* entry = new MenuItemEntry ();
		entry->DoRead (from);
		fItems.Append (entry);
	}
}

void	MenuTitleEntry::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);
	to << tab (tabCount);
	WriteString (to, GetTitle ());
	CollectionSize index = fItems.GetLength ();
	to << index << newline;
	ForEach (MenuItemEntryPtr, it, fItems) {
		it.Current ()->DoWrite (to, tabCount+1);
	}
}

Menu*	MenuTitleEntry::BuildMenu () const
{
	Menu* menu = new Menu ();
	ForEach (MenuItemEntryPtr, it, fItems) {
		MenuItemEntry* current = it.Current ();
		
		if (current->GetCheckBox ()) {
			AbstractCheckBoxMenuItem* item = menu->AddCheckBoxMenuItem (CommandHandler::eNoCommand);
			item->SetDefaultName (current->GetLabel ());
item->SetName (current->GetLabel ());
#if		qSupportMneumonics		
			item->SetMneumonic (current->GetMneumonic ());
#endif
		}
		else {
			AbstractStringMenuItem* item = menu->AddStringMenuItem (CommandHandler::eNoCommand, current->GetLabel ());
item->SetName (current->GetLabel ());
			item->SetExtended (current->GetExtended ());
#if		qSupportMneumonics		
			item->SetMneumonic (current->GetMneumonic ());
#endif
		}
	}
	return (menu);
}

void	MenuTitleEntry::WriteCode (class ostream& to, int tabCount, CommandNumber /*language*/, CommandNumber /*gui*/, const String& menuName)
{
	if (fItems.GetLength () > 0) {
		String	menuItemName = menuName + "Item";
		to << tab (tabCount) << "MenuItem*" << tab << menuItemName << " = Nil;" << newline;

		ForEach (MenuItemEntryPtr, it, fItems) {
			MenuItemEntry* current = it.Current ();
			if (current->GetCheckBox ()) {
				to << tab (tabCount) << menuItemName << " = " << menuName << ".AddCheckBoxMenuItem (" 
					<< current->GetCommandNumber () << ");" << newline;
			}
			else {
				to << tab (tabCount) << menuItemName << " = " << menuName << ".AddStringMenuItem (" 
					<< current->GetCommandNumber () << ");" << newline;
			}
			tabCount++;
			if (current->GetLabel () != kEmptyString) {
				to << tab (tabCount) << menuItemName << "->SetDefaultName (" << quote << current->GetLabel () << quote << ");" << newline;
			}
			if (current->GetMneumonic () != kEmptyString) {
				to << "#if qSupportMneumonics" << newline;
				to << tab (tabCount) << menuItemName << "->SetMneumonic (" << quote << current->GetMneumonic () << quote << ");" << newline;
				to << "#endif" << newline;
			}
			tabCount--;
		}
	}
}


/*
 ********************************************************************************
 ********************************* MenuItemEntry ********************************
 ********************************************************************************
 */

MenuItemEntry::MenuItemEntry () :
	fLabel (kEmptyString),
	fAccelerator (kEmptyString),
	fMneumonic (kEmptyString),
	fExtendedName (False),
	fCheckBox (False),
	fCommandNumber (kEmptyString)
{
}

MenuItemEntry::MenuItemEntry (const MenuItemEntry& entry) :
	fLabel (entry.GetLabel ()),
	fAccelerator (entry.GetAccelerator ()),
	fMneumonic (entry.GetMneumonic ()),
	fExtendedName (entry.GetExtended ()),
	fCheckBox (entry.GetCheckBox ()),
	fCommandNumber (entry.GetCommandNumber ())
{
}
	
String	MenuItemEntry::GetLabel () const
{
	return (fLabel);
}

void	MenuItemEntry::SetLabel (const String& label)
{
	fLabel = label;
}
				
String	MenuItemEntry::GetAccelerator () const
{
	return (fAccelerator);
}

void	MenuItemEntry::SetAccelerator (const String& accelerator)
{
	fAccelerator = accelerator;
}
		
String		MenuItemEntry::GetMneumonic () const
{
	return (fMneumonic);
}

void	MenuItemEntry::SetMneumonic (const String& mneumonic)
{
	fMneumonic = mneumonic;
}
		
Boolean		MenuItemEntry::GetExtended () const
{
	return (fExtendedName);
}

void	MenuItemEntry::SetExtended (Boolean extendedName)
{
	fExtendedName = extendedName;
}

Boolean		MenuItemEntry::GetCheckBox () const
{
	return (fCheckBox);
}

void	MenuItemEntry::SetCheckBox (Boolean checkBox)
{
	fCheckBox = checkBox;
}

String	MenuItemEntry::GetCommandNumber () const
{
	return (fCommandNumber);
}

void	MenuItemEntry::SetCommandNumber (const String& commandNumber)
{
	fCommandNumber = commandNumber;
}

void	MenuItemEntry::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	ReadString (from, fLabel);
	ReadString (from, fMneumonic);
	from >> fExtendedName >> fCheckBox;
	ReadString (from, fAccelerator);
	ReadString (from, fCommandNumber);
}

void	MenuItemEntry::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);
	to << newline;
	to << tab (tabCount);
	WriteString (to, fLabel);
	WriteString (to, fMneumonic);
	to << fExtendedName << fCheckBox;
	WriteString (to, fAccelerator);
	WriteString (to, fCommandNumber);
	to << newline;
}

