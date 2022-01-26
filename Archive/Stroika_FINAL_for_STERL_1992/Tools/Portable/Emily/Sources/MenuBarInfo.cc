/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuBarInfo.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: MenuBarInfo.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.9  92/04/08  17:22:59  17:22:59  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "MenuBarInfo.hh"


MenuBarInfoX::MenuBarInfoX () :
	fEntry (),
	fList (),
	fViewInfo (),
	fTitle (),
	fEntryLabel (),
	fEditButton (),
	fDelete (),
	fInsert ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

MenuBarInfoX::~MenuBarInfoX ()
{
	RemoveFocus (&fEntry);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fEntry);
	RemoveSubView (&fList);
	RemoveSubView (&fViewInfo);
	RemoveSubView (&fTitle);
	RemoveSubView (&fEntryLabel);
	RemoveSubView (&fEditButton);
	RemoveSubView (&fDelete);
	RemoveSubView (&fInsert);
}

#if   qMacUI

void	MenuBarInfoX::BuildForMacUI ()
{
	SetSize (Point (315, 350), eNoUpdate);

	fEntry.SetExtent (98, 248, 18, 92, eNoUpdate);
	fEntry.SetFont (&kApplicationFont);
	fEntry.SetController (this);
	fEntry.SetWordWrap (False);
	fEntry.SetMultiLine (False);
	AddSubView (&fEntry);

	fList.SetExtent (29, 6, 87, 191, eNoUpdate);
	fList.SetController (this);
	AddSubView (&fList);

	fViewInfo.SetExtent (119, 3, 191, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	fTitle.SetExtent (5, 5, 20, 332, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set MenuBar Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fEntryLabel.SetExtent (100, 210, 16, 33, eNoUpdate);
	fEntryLabel.SetFont (&kSystemFont);
	fEntryLabel.SetText ("Title:");
	AddSubView (&fEntryLabel);

	fEditButton.SetExtent (63, 244, 19, 60, eNoUpdate);
	fEditButton.SetLabel ("Edit", eNoUpdate);
	fEditButton.SetController (this);
	AddSubView (&fEditButton);

	fDelete.SetExtent (34, 284, 20, 60, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	fInsert.SetExtent (34, 210, 20, 60, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	AddFocus (&fEntry);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	MenuBarInfoX::BuildForMotifUI ()
{
	SetSize (Point (352, 350), eNoUpdate);

	fEntry.SetExtent (91, 240, 26, 104, eNoUpdate);
	fEntry.SetFont (&kApplicationFont);
	fEntry.SetController (this);
	fEntry.SetWordWrap (False);
	fEntry.SetMultiLine (False);
	AddSubView (&fEntry);

	fList.SetExtent (29, 6, 87, 191, eNoUpdate);
	fList.SetController (this);
	AddSubView (&fList);

	fViewInfo.SetExtent (119, 3, 231, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	fTitle.SetExtent (5, 5, 20, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set MenuBar Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fEntryLabel.SetExtent (94, 204, 21, 38, eNoUpdate);
	fEntryLabel.SetFont (&kSystemFont);
	fEntryLabel.SetText ("Title:");
	AddSubView (&fEntryLabel);

	fEditButton.SetExtent (63, 244, 23, 70, eNoUpdate);
	fEditButton.SetLabel ("Edit", eNoUpdate);
	fEditButton.SetController (this);
	AddSubView (&fEditButton);

	fDelete.SetExtent (29, 275, 24, 70, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	fInsert.SetExtent (29, 201, 24, 70, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	AddFocus (&fEntry);
	AddFocus (&fList);
	AddFocus (&fInsert);
	AddFocus (&fDelete);
	AddFocus (&fViewInfo);
	AddFocus (&fEditButton);
}

#else

void	MenuBarInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (315, 350), eNoUpdate);

	fEntry.SetExtent (98, 248, 18, 92, eNoUpdate);
	fEntry.SetFont (&kApplicationFont);
	fEntry.SetController (this);
	fEntry.SetWordWrap (False);
	fEntry.SetMultiLine (False);
	AddSubView (&fEntry);

	fList.SetExtent (29, 6, 87, 191, eNoUpdate);
	fList.SetController (this);
	AddSubView (&fList);

	fViewInfo.SetExtent (119, 3, 191, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	fTitle.SetExtent (5, 5, 20, 332, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set MenuBar Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fEntryLabel.SetExtent (100, 210, 16, 33, eNoUpdate);
	fEntryLabel.SetFont (&kSystemFont);
	fEntryLabel.SetText ("Title:");
	AddSubView (&fEntryLabel);

	fEditButton.SetExtent (63, 244, 19, 60, eNoUpdate);
	fEditButton.SetLabel ("Edit", eNoUpdate);
	fEditButton.SetController (this);
	AddSubView (&fEditButton);

	fDelete.SetExtent (34, 284, 20, 60, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	fInsert.SetExtent (34, 210, 20, 60, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	AddFocus (&fEntry);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	MenuBarInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (315, 350));
#elif   qMotifUI
	return (Point (352, 350));
#else
	return (Point (315, 350));
#endif /* GUI */
}

void	MenuBarInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfEntryOrigin = fEntry.GetOrigin ();
		fEntry.SetOrigin (kOriginalfEntryOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfListSize = fList.GetSize ();
		fList.SetSize (kOriginalfListSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfEntryLabelOrigin = fEntryLabel.GetOrigin ();
		fEntryLabel.SetOrigin (kOriginalfEntryLabelOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfEditButtonOrigin = fEditButton.GetOrigin ();
		fEditButton.SetOrigin (kOriginalfEditButtonOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfDeleteOrigin = fDelete.GetOrigin ();
		fDelete.SetOrigin (kOriginalfDeleteOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfInsertOrigin = fInsert.GetOrigin ();
		fInsert.SetOrigin (kOriginalfInsertOrigin - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!

#include	"Dialog.hh"

#include	"EmilyWindow.hh"
#include	"MenuTitleInfo.hh"

MenuBarInfo::MenuBarInfo (MenuBarItem& view) :
	MenuBarInfoX (),
	fMenuBarItem (view),
	fMenus ()
{
	fViewInfo.SetUpFromView (fMenuBarItem);

	ForEach (MenuTitleEntryPtr, it, view.fMenus) {
		fMenus.Append (new MenuTitleEntry (*it.Current ()));
		GetListField ().AddItem (it.Current ()->GetTitle ());
	}
	fEditButton.SetEnabled (False);
}

MenuBarInfo::~MenuBarInfo ()
{
	ForEach (MenuTitleEntryPtr, it, fMenus) {
		MenuTitleEntry* current = it.Current ();
		delete current;
	}
}

Sequence(MenuTitleEntryPtr)&	MenuBarInfo::GetMenus ()
{
	return (fMenus);
}

void	MenuBarInfo::ButtonPressed (AbstractButton* button)
{
	if (button == &fList) {
		fEntry.SetText (GetListField ().GetSelectedString ());
		fEntry.SetSelection ();
	}
	else if (button == &fEditButton) {
		CollectionSize index = GetListField ().GetItemIndex (GetListField ().GetSelectedItem ());

		MenuTitleEntry* entry = fMenus[index];
		AssertNotNil (entry);
		MenuTitleInfo info = MenuTitleInfo (*entry);
		Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
		d.SetDefaultButton (&info.GetInsertButton ());
		
		if (d.Pose ()) {
			delete entry;
			fMenus.SetAt (new MenuTitleEntry (info.GetMenu ()), index);
		}
	}
	else if (button == &fInsert) {
		String	foo = fEntry.GetText ();
		Assert (foo != kEmptyString);
		AbstractPickListItem*	item = GetListField ().GetSelectedItem ();
		if (item == Nil) {
			GetListField ().AddItem (foo);
			MenuTitleEntry* entry = new MenuTitleEntry ();
			entry->SetTitle (foo);
			fMenus.Append (entry);
		}
		else {
			CollectionSize index = GetListField ().GetItemIndex (item);
			fMenus[index]->SetTitle (foo);
			((StringPickListItem*)item)->SetText (foo);
		}
		fEntry.SetText (kEmptyString);
		GetListField ().SelectAll (not PickList::kSelected);
	}
	else if (button == &fDelete) {
		AbstractPickListItem*	item = GetListField ().GetSelectedItem ();
		CollectionSize index = GetListField ().GetItemIndex (item);
		GetListField ().Delete (item);
		fMenus.Remove (fMenus[index]);
		fEntry.SetText (kEmptyString);
	}
	else {
		MenuBarInfoX::ButtonPressed (button);
	}
	
	if (GetListField ().GetSelectedItem () != Nil) {
		fInsert.SetLabel ("Change");
		fDelete.SetEnabled (True);
		fEditButton.SetEnabled (True);
	}
	else {
		fInsert.SetLabel ("Insert");
		fDelete.SetEnabled (False);
		fEditButton.SetEnabled (False);
	}
	fInsert.SetEnabled (Boolean (fEntry.GetLength () > 0));
}

void	MenuBarInfo::TextChanged (TextEditBase* item)
{
	TextController::TextChanged (item);
	fInsert.SetEnabled (Boolean (fEntry.GetLength () > 0));
}

