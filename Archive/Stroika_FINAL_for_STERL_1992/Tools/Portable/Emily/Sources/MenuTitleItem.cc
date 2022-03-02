/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuTitleItem.cc,v 1.7 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: MenuTitleItem.cc,v $
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





#include	"StreamUtils.hh"

#include	"Menu.hh"
#include	"PullDownMenu.hh"
#include	"Dialog.hh"
#include	"CheckBoxMenuItem.hh"
#include	"StringMenuItem.hh"

#include	"MenuTitleItem.hh"
#include	"CommandNumbers.hh"
#include	"MenuTitleInfo.hh"




#if		!qRealTemplatesAvailable
	Implement (Iterator, MIEPtr);
	Implement (Collection, MIEPtr);
	Implement (AbSequence, MIEPtr);
	Implement (Array, MIEPtr);
	Implement (Sequence_Array, MIEPtr);
	Implement (Sequence, MIEPtr);
#endif



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
	to << tab (tabCount);
	WriteString (to, fLabel);
	WriteString (to, fMneumonic);
	to << fExtendedName << fCheckBox;
	WriteString (to, fAccelerator);
	WriteString (to, fCommandNumber);
	to << newline;
}






/*
 ********************************************************************************
 ****************************** MenuTitleItemType *******************************
 ********************************************************************************
 */
MenuTitleItemType::MenuTitleItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildMenu, "MenuTitle", (ItemBuilderProc)&MenuTitleItemBuilder)
#else
	ItemType (eBuildMenu, "MenuTitle", &MenuTitleItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
MenuTitleItemType&	MenuTitleItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	MenuTitleItemType::MenuTitleItemBuilder ()
{
	return (new MenuTitleItem (Get ()));
}

MenuTitleItemType*	MenuTitleItemType::sThis = Nil;






/*
 ********************************************************************************
 ********************************* MenuTitleItem ********************************
 ********************************************************************************
 */
	//q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
	class	MyMenuBar : public MenuBar {
		public:
			MyMenuBar ():
				MenuBar ()
			  {
			  }
		nonvirtual	void	AddMenuTitle (MenuTitle* menuTitle, CollectionSize index)
	  {
		  // make accesible...
		MenuBar::AddMenuTitle (menuTitle, index);
	  }
	};
MenuTitleItem::MenuTitleItem (ItemType& type):
	ViewItem (type),
	fMenuBar (Nil),
	fMenuTitle (Nil),
	fTitle (kEmptyString)
{
	MyMenuBar*	mBar = new MyMenuBar ();
	fMenuBar = mBar;
	fMenu = new Menu ();
	fMenuTitle = new PullDownMenuTitle ("temp", fMenu);
	SetOwnedView (fMenuBar);
	mBar->AddMenuTitle (fMenuTitle, eAppend);
	SetTitle ("Menu");
}

String	MenuTitleItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "PullDownMenu.hh";
	return (kHeaderFileName);
}

Point	MenuTitleItem::CalcDefaultSize (const Point& defaultSize) const
{
	return (ViewItem::CalcDefaultSize_ (defaultSize));
}

Boolean	MenuTitleItem::ItemCanBeEnabled ()
{
	return (False);
}

Boolean	MenuTitleItem::IsButton ()
{
	return (True);
}

Boolean	MenuTitleItem::IsSlider ()
{
	return (False);
}

Boolean	MenuTitleItem::IsText ()
{
	return (False);
}

Boolean	MenuTitleItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (False);
}

String	MenuTitleItem::GetMenuFieldName () const
{
	return (GetFieldName () + "Menu");
}

void	MenuTitleItem::WriteDeclaration (class ostream& to, int tabCount)
{
	ViewItem::WriteDeclaration (to, tabCount);
	to << tab (tabCount) << "class Menu*" << tab (4) << GetMenuFieldName () << ";" << newline;
}

void	MenuTitleItem::WriteBuilder (class ostream& to, int tabCount, CommandNumber gui)
{
	ViewItem::WriteBuilder (to, tabCount, gui);
	to << "(" << quote << fMenuTitle->GetTitle () << quote << ", this);" << newline;
	to << tab (tabCount) << GetMenuFieldName () << "= new Menu ();" << newline;
}

void	MenuTitleItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
	
	String foo;
	ReadString (from, foo);
	SetTitle (foo);

	ForEach (MenuItemEntryPtr, it, fEntries) {
		MenuItemEntry* param = it.Current ();
		delete param;
	}
	fEntries.RemoveAll ();

	CollectionSize entryCount;
	from >> entryCount;
	for (int count = 1; count <= entryCount; count++) {
		MenuItemEntry*	newEntry = new MenuItemEntry ();
		char c;
		from >> c;
		Require (c == '{');
		newEntry->DoRead (from);
		from >> c;
		Require (c == '}');
		fEntries.Append (newEntry);	
	}
}

void	MenuTitleItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
	WriteString (to, GetTitle ());
	CollectionSize	entryCount = fEntries.GetLength ();
	to << tab (tabCount) << entryCount << newline;
	ForEach (MenuItemEntryPtr, it, fEntries) {
		to << tab (tabCount) << '{' << newline;
		it.Current ()->DoWrite (to, tabCount+1);
		to << tab (tabCount) << '}' << newline;
	}
}

void	MenuTitleItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);

	to << tab (tabCount) << GetFieldName () << "->SetTitle (" << quote << GetTitle () << quote << ");" << newline;

	ForEach (MenuItemEntryPtr, it, fEntries) {
		MenuItemEntry& entry = *it.Current ();
		
		if (entry.GetLabel () == "-") {
			to << tab (tabCount) << GetMenuFieldName () << "->AddSeparatorMenuItem ();" << newline;
		}
		else {
			String	commandNumber = entry.GetCommandNumber ();
			if (commandNumber == kEmptyString) {
				commandNumber = "CommandHandler::eNoCommand";
			}
			String params = commandNumber;
			if (entry.GetLabel () != kEmptyString) {
				params += ", \"";
				params += entry.GetLabel ();
				params += "\"";
			}

			
			if (entry.GetCheckBox ()) {
				to << tab (tabCount) << GetMenuFieldName () << "->AddCheckBoxMenuItem (" << params << ");" << newline;
			}
			else {
				to << tab (tabCount) << GetMenuFieldName () << "->AddStringMenuItem (" << params << ");" << newline;
			}
		}
	}
	to << tab (tabCount) << GetFieldName () << "->SetMenu (" << GetMenuFieldName () << ");" << newline;
}

String	MenuTitleItem::GetTitle () const
{
	return (fTitle);
}

void	MenuTitleItem::SetTitle (const String& title)
{
	fTitle = title;
	AssertNotNil (fMenuTitle);
	fMenuTitle->SetTitle (title);
}

void	MenuTitleItem::EditModeChanged (Boolean newEditMode)
{
if (newEditMode) {
	fMenuBar->DeinstallMenu ();
}
else {
	fMenuBar->InstallMenu ();
}

	// when becoming live, built up menu
	
	if (newEditMode) {
		fMenuTitle->SetMenu (Nil);
	}
	else {
		AssertNotNil (fMenu);
		fMenuTitle->SetMenu (Nil);
		delete fMenu;
		fMenu = new Menu ();
		ForEach (MenuItemEntryPtr, it, fEntries) {
			MenuItemEntry& entry = *it.Current ();

			if (entry.GetLabel () == "-") {
				fMenu->AddSeparatorMenuItem ();
			}
			else {
				String	commandNumber = entry.GetCommandNumber ();
				if (commandNumber == kEmptyString) {
					commandNumber = "CommandHandler::eNoCommand";
				}
				String params = commandNumber;
				if (entry.GetLabel () != kEmptyString) {
					params += ", \"";
					params += entry.GetLabel ();
					params += "\"";
				}

				if (entry.GetCheckBox ()) {
					if (entry.GetLabel () == kEmptyString) {
						fMenu->AddCheckBoxMenuItem (/*commandNumber*/CommandHandler::eNoCommand);
					}
					else {
						(fMenu->AddCheckBoxMenuItem (CommandHandler::eNoCommand))->SetDefaultName (entry.GetLabel ());
					}
				}
				else {
					if (entry.GetLabel () == kEmptyString) {
						fMenu->AddStringMenuItem (CommandHandler::eNoCommand);
					}
					else {
						fMenu->AddStringMenuItem (CommandHandler::eNoCommand, entry.GetLabel ());
					}
				}
			}
		}
		fMenuTitle->SetMenu (fMenu);
	}

	ViewItem::EditModeChanged (newEditMode);
}

void	MenuTitleItem::SetItemInfo ()
{
	MenuTitleInfo info = MenuTitleInfo (*this, fEntries);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (info.GetInsertButton ());
	
	if (d.Pose ()) {
		ForEach (MenuItemEntryPtr, it, fEntries) {
			MenuItemEntry* entry = it.Current ();
			delete entry;
		}
		fEntries.RemoveAll ();
		ForEach (MenuItemEntryPtr, it1, info.fEntries) {
			fEntries.Append (new MenuItemEntry (*it1.Current ()));
		}
		SetTitle (info.GetNameField ().GetText ());
		Refresh ();
		DirtyDocument ();
	}
}


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***
