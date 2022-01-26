/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PickListInfo.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PickListInfo.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "PickListInfo.hh"


StringPickListInfoX::StringPickListInfoX () :
	fEntryLabel (),
	fEntry (),
	fTitle (),
	fInsert (),
	fMultipleSelections (),
	fFloatingSelection (),
	fViewInfo (),
	fList (),
	fDelete ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

StringPickListInfoX::~StringPickListInfoX ()
{
	RemoveFocus (&fEntry);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fEntryLabel);
	RemoveSubView (&fEntry);
	RemoveSubView (&fTitle);
	RemoveSubView (&fInsert);
	RemoveSubView (&fMultipleSelections);
	RemoveSubView (&fFloatingSelection);
	RemoveSubView (&fViewInfo);
	RemoveSubView (&fList);
	RemoveSubView (&fDelete);
}

#if   qMacUI

void	StringPickListInfoX::BuildForMacUI ()
{
	SetSize (Point (361, 351), eNoUpdate);

	fEntryLabel.SetExtent (72, 201, 21, 43, eNoUpdate);
	fEntryLabel.SetFont (&kSystemFont);
	fEntryLabel.SetText ("Entry");
	AddSubView (&fEntryLabel);

	fEntry.SetExtent (70, 240, 26, 100, eNoUpdate);
	fEntry.SetFont (&kApplicationFont);
	fEntry.SetController (this);
	fEntry.SetWordWrap (False);
	AddSubView (&fEntry);

	fTitle.SetExtent (5, 5, 19, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set StringPickList Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fInsert.SetExtent (32, 199, 24, 70, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	fMultipleSelections.SetLabel ("Multiple Selections", eNoUpdate);
	fMultipleSelections.SetExtent (121, 196, 25, 146, eNoUpdate);
	fMultipleSelections.SetController (this);
	AddSubView (&fMultipleSelections);

	fFloatingSelection.SetLabel ("Floating Selection", eNoUpdate);
	fFloatingSelection.SetExtent (121, 19, 25, 139, eNoUpdate);
	fFloatingSelection.SetController (this);
	AddSubView (&fFloatingSelection);

	fViewInfo.SetExtent (145, 4, 212, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	fList.SetExtent (29, 6, 87, 191, eNoUpdate);
	fList.SetItemsFloat (True);
	fList.SetController (this);
	AddSubView (&fList);

	fDelete.SetExtent (32, 271, 24, 70, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	AddFocus (&fEntry);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	StringPickListInfoX::BuildForMotifUI ()
{
	SetSize (Point (365, 350), eNoUpdate);

	fEntryLabel.SetExtent (71, 196, 21, 43, eNoUpdate);
	fEntryLabel.SetFont (&kSystemFont);
	fEntryLabel.SetText ("Entry");
	AddSubView (&fEntryLabel);

	fEntry.SetExtent (70, 240, 26, 100, eNoUpdate);
	fEntry.SetFont (&kApplicationFont);
	fEntry.SetController (this);
	fEntry.SetWordWrap (False);
	AddSubView (&fEntry);

	fTitle.SetExtent (5, 5, 19, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set StringPickList Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fInsert.SetExtent (32, 199, 24, 70, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	fMultipleSelections.SetLabel ("Multiple Selections", eNoUpdate);
	fMultipleSelections.SetExtent (121, 196, 25, 146, eNoUpdate);
	fMultipleSelections.SetController (this);
	AddSubView (&fMultipleSelections);

	fFloatingSelection.SetLabel ("Floating Selection", eNoUpdate);
	fFloatingSelection.SetExtent (121, 19, 25, 139, eNoUpdate);
	fFloatingSelection.SetController (this);
	AddSubView (&fFloatingSelection);

	fViewInfo.SetExtent (145, 4, 218, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	fList.SetExtent (29, 6, 87, 191, eNoUpdate);
	fList.SetItemsFloat (True);
	fList.SetController (this);
	AddSubView (&fList);

	fDelete.SetExtent (32, 271, 24, 70, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	AddFocus (&fEntry);
	AddFocus (&fList);
	AddFocus (&fInsert);
	AddFocus (&fDelete);
	AddFocus (&fFloatingSelection);
	AddFocus (&fMultipleSelections);
	AddFocus (&fViewInfo);
}

#else

void	StringPickListInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (361, 351), eNoUpdate);

	fEntryLabel.SetExtent (72, 201, 21, 43, eNoUpdate);
	fEntryLabel.SetFont (&kSystemFont);
	fEntryLabel.SetText ("Entry");
	AddSubView (&fEntryLabel);

	fEntry.SetExtent (70, 240, 26, 100, eNoUpdate);
	fEntry.SetFont (&kApplicationFont);
	fEntry.SetController (this);
	fEntry.SetWordWrap (False);
	AddSubView (&fEntry);

	fTitle.SetExtent (5, 5, 19, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set StringPickList Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fInsert.SetExtent (32, 199, 24, 70, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	fMultipleSelections.SetLabel ("Multiple Selections", eNoUpdate);
	fMultipleSelections.SetExtent (121, 196, 25, 146, eNoUpdate);
	fMultipleSelections.SetController (this);
	AddSubView (&fMultipleSelections);

	fFloatingSelection.SetLabel ("Floating Selection", eNoUpdate);
	fFloatingSelection.SetExtent (121, 19, 25, 139, eNoUpdate);
	fFloatingSelection.SetController (this);
	AddSubView (&fFloatingSelection);

	fViewInfo.SetExtent (145, 4, 212, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	fList.SetExtent (29, 6, 87, 191, eNoUpdate);
	fList.SetItemsFloat (True);
	fList.SetController (this);
	AddSubView (&fList);

	fDelete.SetExtent (32, 271, 24, 70, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	AddFocus (&fEntry);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	StringPickListInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (361, 351));
#elif   qMotifUI
	return (Point (365, 350));
#else
	return (Point (361, 351));
#endif /* GUI */
}

void	StringPickListInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"PickListItem.hh"
#include	"EmilyWindow.hh"

StringPickListInfo::StringPickListInfo (PickListEditItem& view) :
	StringPickListInfoX ()
{
	fViewInfo.SetUpFromView (view);
	
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);

	GetMultipleSelectionsField ().SetOn (view.GetPickList ().GetAllowMultipleSelections (), eNoUpdate);
	GetFloatingSelectionField ().SetOn (view.GetPickList ().GetItemsFloat (), eNoUpdate);

	CollectionSize last = view.GetPickList ().GetItemCount ();
	for (CollectionSize i = 1; i <= last; i++) {
		GetListField ().AddItem (view.GetPickList ().GetStringByIndex (i));
	}

	Boolean	fullEditing = Boolean (not EmilyWindow::GetCustomizeOnly ());
	GetMultipleSelectionsField ().SetEnabled (fullEditing);
	GetFloatingSelectionField ().SetEnabled (fullEditing);
}

void	StringPickListInfo::ButtonPressed (AbstractButton* button)
{
	if (button == &fList) {
		fEntry.SetText (GetListField ().GetSelectedString ());
		fEntry.SetSelection ();
	}
	else if (button == &fInsert) {
		String	foo = fEntry.GetText ();
		Assert (foo != kEmptyString);
		AbstractPickListItem*	item = GetListField ().GetSelectedItem ();
		if (item == Nil) {
			GetListField ().AddItem (foo);
		}
		else {
			((StringPickListItem*)item)->SetText (foo);
		}
		fEntry.SetText (kEmptyString);
		GetListField ().SelectAll (not PickList::kSelected);
	}
	else if (button == &fDelete) {
		GetListField ().Delete (GetListField ().GetSelectedItem ());
		fEntry.SetText (kEmptyString);
	}
	else {
		StringPickListInfoX::ButtonPressed (button);
	}
	
	if (GetListField ().GetSelectedItem () != Nil) {
		fInsert.SetLabel ("Change");
		fDelete.SetEnabled (True);
	}
	else {
		fInsert.SetLabel ("Insert");
		fDelete.SetEnabled (False);
	}
	fInsert.SetEnabled (Boolean (fEntry.GetLength () > 0));
}

void	StringPickListInfo::TextChanged (TextEditBase* item)
{
	TextController::TextChanged (item);
	fInsert.SetEnabled (Boolean (fEntry.GetLength () > 0));
}

