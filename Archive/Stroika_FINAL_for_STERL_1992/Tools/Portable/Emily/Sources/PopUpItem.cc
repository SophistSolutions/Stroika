/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PopUpItem.cc,v 1.5 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *		Revision 1.9  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *		Revision 1.8  1992/01/23  20:00:55  sterling
 *		replace constructor for popup so same between Mac and Motif
 *
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"Menu.hh"
#include	"PickList.hh"
#include	"OptionMenu.hh"

#include	"CommandNumbers.hh"
#include	"PopUpInfo.hh"
#include	"ViewItemInfo.hh"

#include	"PopUpItem.hh"




/*
 ********************************************************************************
 ****************************** OptionMenuItemType **************************
 ********************************************************************************
 */
OptionMenuItemType::OptionMenuItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildMenu, "OptionMenuButton", (ItemBuilderProc)&OptionMenuItemBuilder)
#else
	ItemType (eBuildMenu, "OptionMenuButton", &OptionMenuItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
OptionMenuItemType&	OptionMenuItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	OptionMenuItemType::OptionMenuItemBuilder ()
{
	return (new OptionMenuItem (Get ()));
}

OptionMenuItemType*	OptionMenuItemType::sThis = Nil;






/*
 ********************************************************************************
 ****************************** OptionMenuItem ******************************
 ********************************************************************************
 */
OptionMenuItem::OptionMenuItem (ItemType& type) :
	ViewItem (type),
	fOptionMenu (Nil)
{
	fOptionMenu = new OptionMenuButton ("Label:");
	SetOwnedView (fOptionMenu);
}

String	OptionMenuItem::GetHeaderFileName ()
{
	return (String (String::eReadOnly, "OptionMenu.hh"));
}

OptionMenuButton&	OptionMenuItem::GetOptionMenu () const
{
	RequireNotNil (fOptionMenu);
	return (*fOptionMenu);
}

Boolean	OptionMenuItem::ItemCanBeEnabled ()
{
	return (True);
}

void	OptionMenuItem::SetOwnedEnabled (Boolean enabled, UpdateMode updateMode)
{
	GetOptionMenu ().EnableItem::SetEnabled (enabled, updateMode);
}

void	OptionMenuItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
	
	String	label = kEmptyString;
	ReadString (from, label);
	GetOptionMenu ().SetLabel (label);
	CollectionSize total = 0;
	from >> total;
	
	GetOptionMenu ().SetItemCount (0);
	for (CollectionSize i = 1; i <= total; i++) {
		String	foo = kEmptyString;
		ReadString (from, foo);
		GetOptionMenu ().AppendItem (foo);
	}
}

void	OptionMenuItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);

	WriteString (to, GetOptionMenu ().GetLabel ());
	CollectionSize	last = GetOptionMenu ().GetItemCount ();
	to << last << ' ';
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (String, it, GetOptionMenu ().GetItems ().operator Iterator(String)* ()) {
#else
	ForEach (String, it, GetOptionMenu ().GetItems ()) {
#endif
		to << newline << tab (tabCount+1);
		WriteString (to, it.Current ());
	}
}

void	OptionMenuItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	OptionMenuItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (String, it, GetOptionMenu ().GetItems ().operator Iterator(String)* ()) {
#else
	ForEach (String, it, GetOptionMenu ().GetItems ()) {
#endif
		to << tab (tabCount) << GetFieldName () << ".AppendItem (" << quote << it.Current () << quote << ");" << newline;
	}
	if (GetOptionMenu ().GetItems ().GetLength () > 0) {
		to << tab (tabCount) << GetFieldName () << ".SetCurrentItem (1);" << newline;
	}
	to << tab (tabCount) << GetFieldName () << ".SetLabel (" << quote << GetOptionMenu ().GetLabel () << quote << ");" << newline;
	to << tab (tabCount) << GetFieldName () << ".SetController (this);" << newline;
}

Boolean	OptionMenuItem::IsButton ()
{
	return (True);
}

Boolean	OptionMenuItem::IsSlider ()
{
	return (False);
}

Boolean	OptionMenuItem::IsText ()
{
	return (False);
}

Boolean	OptionMenuItem::IsFocusItem (CommandNumber gui)
{
	if (gui == eMacUI) {
		return (False);
	}
	return (True);
}

void	OptionMenuItem::SetItemInfo ()
{
	PopUpInfo info = PopUpInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (&info.GetInsertButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetOptionMenuInfoCommand (*this, info));
		DirtyDocument ();
	}
}

SetOptionMenuInfoCommand::SetOptionMenuInfoCommand (OptionMenuItem& item, class PopUpInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewLabel (info.GetLabelField ().GetText ()),
	fOldLabel (item.GetOptionMenu ().GetLabel ()),
	fNewEntries (),
	fOldEntries ()
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());

#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (String, it, fItem.GetOptionMenu ().GetItems ().operator Iterator(String)* ()) {
#else
	ForEach (String, it, fItem.GetOptionMenu ().GetItems ()) {
#endif
		fOldEntries.Append (it.Current ());
	}
	CollectionSize last = info.GetListField ().GetItemCount ();
	for (CollectionSize i = 1; i <= last; i++) {
		fNewEntries.Append (info.GetListField ().GetStringByIndex (i));
	}
}
		
void	SetOptionMenuInfoCommand::DoIt ()
{
	fItemInfoCommand->DoIt ();

	fItem.GetOptionMenu ().SetLabel (fNewLabel);
	fItem.GetOptionMenu ().SetItemCount (0);
	ForEach (String, it, fNewEntries) {
		fItem.GetOptionMenu ().AppendItem (it.Current ());
	}
	
	Command::DoIt ();
}

void	SetOptionMenuInfoCommand::UnDoIt ()
{
	fItemInfoCommand->UnDoIt ();

	fItem.GetOptionMenu ().SetLabel (fOldLabel);
	fItem.GetOptionMenu ().SetItemCount (0);
	ForEach (String, it, fOldEntries) {
		fItem.GetOptionMenu ().AppendItem (it.Current ());
	}
	
	Command::UnDoIt ();
}
