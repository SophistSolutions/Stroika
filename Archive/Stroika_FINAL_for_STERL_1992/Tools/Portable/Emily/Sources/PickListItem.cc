/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PickListItem.cc,v 1.5 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PickListItem.cc,v $
 *		Revision 1.5  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.7  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *
 */

#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"CheckBox.hh"

#include	"PickList.hh"

#include	"PickListItem.hh"
#include	"CommandNumbers.hh"
#include	"ViewItemInfo.hh"
#include	"PickListInfo.hh"





/*
 ********************************************************************************
 ****************************** PickListEditItemType ****************************
 ********************************************************************************
 */
PickListEditItemType::PickListEditItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildPickList, "StringPickList", (ItemBuilderProc)&PickListEditItemBuilder)
#else
	ItemType (eBuildPickList, "StringPickList", &PickListEditItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
PickListEditItemType&	PickListEditItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	PickListEditItemType::PickListEditItemBuilder ()
{
	return (new PickListEditItem (Get ()));
}

PickListEditItemType*	PickListEditItemType::sThis = Nil;





/*
 ********************************************************************************
 ****************************** PickListEditItem ********************************
 ********************************************************************************
 */
PickListEditItem::PickListEditItem (ItemType& type) :
	ViewItem (type),
	fPickList (Nil)
{
	fPickList = new StringPickList ();
	SetOwnedView (fPickList);
}

String	PickListEditItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "PickList.hh";
	return (kHeaderFileName);
}

StringPickList&	PickListEditItem::GetPickList () const
{
	RequireNotNil (fPickList);
	return (*fPickList);
}

Point	PickListEditItem::CalcDefaultSize_ (const Point& defaultSize) const
{
	if (defaultSize == kZeroPoint) {
		return (ViewItem::CalcDefaultSize_ (Point (60, 100)));
	}
	return (ViewItem::CalcDefaultSize_ (defaultSize));
}

Boolean	PickListEditItem::ItemCanBeEnabled ()
{
	return (True);
}

void	PickListEditItem::SetOwnedEnabled (Boolean enabled, Panel::UpdateMode updateMode)
{
	GetPickList ().SetEnabled (enabled, updateMode);
}

void	PickListEditItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
	
	Boolean	allowMultipleSelection, itemsFloat;
	from >> allowMultipleSelection >> itemsFloat;
	GetPickList ().SetAllowMultipleSelections (allowMultipleSelection);
	GetPickList ().SetItemsFloat (itemsFloat);
	CollectionSize total = 0;
	from >> total;
	for (CollectionSize i = 1; i <= total; i++) {
		String	foo = kEmptyString;
		ReadString (from, foo);
		GetPickList ().AddItem (foo);
	}
}

void	PickListEditItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
	to << tab (tabCount) << GetPickList ().GetAllowMultipleSelections () << ' ' << GetPickList ().GetItemsFloat ();
	CollectionSize	last = GetPickList ().GetItemCount ();
	to << last << ' ';
	for (CollectionSize i = 1; i <= last; i++) {
		to << newline << tab (tabCount+1);
		WriteString (to, GetPickList ().GetStringByIndex (i));
	}
	to << newline;
}

void	PickListEditItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	PickListEditItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
	if (GetPickList ().GetAllowMultipleSelections ()) {
		to << tab (tabCount) << GetFieldName () << ".SetAllowMultipleSelections (True);" << newline;
	}
	if (GetPickList ().GetItemsFloat ()) {
		to << tab (tabCount) << GetFieldName () << ".SetItemsFloat (True);" << newline;
	}
	CollectionSize	last = GetPickList ().GetItemCount ();
	for (CollectionSize i = 1; i <= last; i++) {
		to << tab (tabCount) << GetFieldName () << ".AddItem (" << quote << GetPickList ().GetStringByIndex (i) << quote << ");" << newline;
	}
	to << tab (tabCount) << GetFieldName () << ".SetController (this);" << newline;
}

Boolean	PickListEditItem::IsButton ()
{
	return (True);
}

Boolean	PickListEditItem::IsSlider ()
{
	return (False);
}

Boolean	PickListEditItem::IsText ()
{
	return (False);
}

Boolean	PickListEditItem::IsFocusItem (CommandNumber gui)
{
	if (gui == eMacUI) {
		return (False);
	}
	return (True);
}


void	PickListEditItem::SetItemInfo ()
{
	StringPickListInfo info = StringPickListInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (&info.GetInsertButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetPickListInfoCommand (*this, info));
		DirtyDocument ();
	}
}

SetPickListInfoCommand::SetPickListInfoCommand (PickListEditItem& item, class StringPickListInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewFloatingSelection (info.GetFloatingSelectionField ().GetOn ()),
	fOldFloatingSelection (item.GetPickList ().GetItemsFloat ()),
	fNewMultipleSelections (info.GetMultipleSelectionsField ().GetOn ()),
	fOldMultipleSelections (item.GetPickList ().GetAllowMultipleSelections ()),
	fNewEntries (),
	fOldEntries ()
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());

	CollectionSize last = item.GetPickList ().GetItemCount ();
	for (CollectionSize i = 1; i <= last; i++) {
		fOldEntries.Append (item.GetPickList ().GetStringByIndex (i));
	}
	
	last = info.GetListField ().GetItemCount ();
	for (i = 1; i <= last; i++) {
		fNewEntries.Append (info.GetListField ().GetStringByIndex (i));
	}
}
		
void	SetPickListInfoCommand::DoIt ()
{
	fItemInfoCommand->DoIt ();

	StringPickList&	pickList = fItem.GetPickList ();
	pickList.SetItemsFloat (fNewFloatingSelection);
	pickList.SetAllowMultipleSelections (fNewMultipleSelections);
	pickList.DeleteAll ();
	ForEach (String, it, fNewEntries) {
		pickList.AddItem (it.Current ());
	}
	
	Command::DoIt ();
}

void	SetPickListInfoCommand::UnDoIt ()
{
	fItemInfoCommand->UnDoIt ();

	StringPickList&	pickList = fItem.GetPickList ();
	pickList.SetItemsFloat (fOldFloatingSelection);
	pickList.SetAllowMultipleSelections (fOldMultipleSelections);
	pickList.DeleteAll ();
	ForEach (String, it, fOldEntries) {
		pickList.AddItem (it.Current ());
	}
	
	Command::UnDoIt ();
}
