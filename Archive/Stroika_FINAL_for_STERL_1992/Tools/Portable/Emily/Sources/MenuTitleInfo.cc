/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuTitleInfo.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: MenuTitleInfo.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.8  92/04/08  17:23:00  17:23:00  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "MenuTitleInfo.hh"


MenuTitleInfoX::MenuTitleInfoX () :
	fTitle (),
	fName (),
	fList (),
	fEntryGroup (),
	fAccelerator (),
	fMneumonic (),
	fCommand (),
	fLabel (),
	fCheckBox (),
	fExtended (),
	fCommandLabel (),
	fAcceleratorLabel (),
	fMneumonicLabel (),
	fLabelLabel (),
	fField1 (),
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

MenuTitleInfoX::~MenuTitleInfoX ()
{
	RemoveFocus (&fName);
	RemoveFocus (&fEntryGroupTabLoop);

	RemoveSubView (&fTitle);
	RemoveSubView (&fName);
	RemoveSubView (&fList);
	RemoveSubView (&fEntryGroup);
		fEntryGroupTabLoop.RemoveFocus (&fLabel);
		fEntryGroupTabLoop.RemoveFocus (&fCommand);
		fEntryGroupTabLoop.RemoveFocus (&fAccelerator);
		fEntryGroupTabLoop.RemoveFocus (&fMneumonic);

		fEntryGroup.RemoveSubView (&fAccelerator);
		fEntryGroup.RemoveSubView (&fMneumonic);
		fEntryGroup.RemoveSubView (&fCommand);
		fEntryGroup.RemoveSubView (&fLabel);
		fEntryGroup.RemoveSubView (&fCheckBox);
		fEntryGroup.RemoveSubView (&fExtended);
		fEntryGroup.RemoveSubView (&fCommandLabel);
		fEntryGroup.RemoveSubView (&fAcceleratorLabel);
		fEntryGroup.RemoveSubView (&fMneumonicLabel);
		fEntryGroup.RemoveSubView (&fLabelLabel);
	RemoveSubView (&fField1);
	RemoveSubView (&fDelete);
	RemoveSubView (&fInsert);
}

#if   qMacUI

void	MenuTitleInfoX::BuildForMacUI ()
{
	SetSize (Point (166, 354), eNoUpdate);

	fTitle.SetExtent (5, 3, 16, 346, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set MenuTitle Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fName.SetExtent (28, 42, 16, 127, eNoUpdate);
	fName.SetFont (&kApplicationFont);
	fName.SetController (this);
	fName.SetWordWrap (False);
	AddSubView (&fName);

	fList.SetExtent (52, 6, 70, 146, eNoUpdate);
	fList.SetItemsFloat (True);
	fList.SetController (this);
	AddSubView (&fList);

	fEntryGroup.SetExtent (52, 155, 109, 172, eNoUpdate);
	fEntryGroup.SetBorder (1, 1, eNoUpdate);
	fEntryGroup.SetMargin (1, 1, eNoUpdate);
		fAccelerator.SetExtent (48, 90, 16, 43, eNoUpdate);
		fAccelerator.SetFont (&kApplicationFont);
		fAccelerator.SetController (this);
		fAccelerator.SetWordWrap (False);
		fAccelerator.SetMultiLine (False);
		fEntryGroup.AddSubView (&fAccelerator);

		fMneumonic.SetExtent (66, 90, 16, 43, eNoUpdate);
		fMneumonic.SetFont (&kApplicationFont);
		fMneumonic.SetController (this);
		fMneumonic.SetWordWrap (False);
		fMneumonic.SetMultiLine (False);
		fEntryGroup.AddSubView (&fMneumonic);

		fCommand.SetExtent (25, 73, 17, 95, eNoUpdate);
		fCommand.SetFont (&kApplicationFont);
		fCommand.SetController (this);
		fCommand.SetWordWrap (False);
		fCommand.SetMultiLine (False);
		fEntryGroup.AddSubView (&fCommand);

		fLabel.SetExtent (3, 43, 16, 125, eNoUpdate);
		fLabel.SetFont (&kApplicationFont);
		fLabel.SetController (this);
		fLabel.SetWordWrap (False);
		fLabel.SetMultiLine (False);
		fEntryGroup.AddSubView (&fLabel);

		fCheckBox.SetLabel ("CheckBox", eNoUpdate);
		fCheckBox.SetExtent (91, 83, 15, 80, eNoUpdate);
		fCheckBox.SetController (this);
		fEntryGroup.AddSubView (&fCheckBox);

		fExtended.SetLabel ("Extended", eNoUpdate);
		fExtended.SetExtent (91, 3, 15, 78, eNoUpdate);
		fExtended.SetController (this);
		fEntryGroup.AddSubView (&fExtended);

		fCommandLabel.SetExtent (26, 2, 16, 69, eNoUpdate);
		fCommandLabel.SetFont (&kSystemFont);
		fCommandLabel.SetText ("Command:");
		fEntryGroup.AddSubView (&fCommandLabel);

		fAcceleratorLabel.SetExtent (48, 6, 16, 81, eNoUpdate);
		fAcceleratorLabel.SetFont (&kSystemFont);
		fAcceleratorLabel.SetText ("Accelerator:");
		fEntryGroup.AddSubView (&fAcceleratorLabel);

		fMneumonicLabel.SetExtent (65, 7, 16, 80, eNoUpdate);
		fMneumonicLabel.SetFont (&kSystemFont);
		fMneumonicLabel.SetText ("Mneumonic:");
		fEntryGroup.AddSubView (&fMneumonicLabel);

		fLabelLabel.SetExtent (3, 3, 16, 40, eNoUpdate);
		fLabelLabel.SetFont (&kSystemFont);
		fLabelLabel.SetText ("Label:");
		fEntryGroup.AddSubView (&fLabelLabel);

		fEntryGroupTabLoop.AddFocus (&fLabel);
		fEntryGroupTabLoop.AddFocus (&fCommand);
		fEntryGroupTabLoop.AddFocus (&fAccelerator);
		fEntryGroupTabLoop.AddFocus (&fMneumonic);
	AddSubView (&fEntryGroup);

	fField1.SetExtent (28, 8, 16, 33, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Title:");
	AddSubView (&fField1);

	fDelete.SetExtent (28, 260, 20, 60, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	fInsert.SetExtent (28, 175, 20, 60, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	AddFocus (&fName);
	AddFocus (&fEntryGroupTabLoop);
}

#elif qMotifUI

void	MenuTitleInfoX::BuildForMotifUI ()
{
	SetSize (Point (208, 391), eNoUpdate);

	fTitle.SetExtent (5, 3, 22, 384, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Add Menu Items");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fName.SetExtent (24, 43, 26, 161, eNoUpdate);
	fName.SetFont (&kApplicationFont);
	fName.SetController (this);
	fName.SetWordWrap (False);
	AddSubView (&fName);

	fList.SetExtent (60, 3, 144, 192, eNoUpdate);
	fList.SetItemsFloat (True);
	fList.SetController (this);
	AddSubView (&fList);

	fEntryGroup.SetExtent (61, 197, 140, 189, eNoUpdate);
	fEntryGroup.SetBorder (1, 1, eNoUpdate);
	fEntryGroup.SetMargin (1, 1, eNoUpdate);
		fAccelerator.SetExtent (53, 89, 26, 43, eNoUpdate);
		fAccelerator.SetFont (&kApplicationFont);
		fAccelerator.SetController (this);
		fAccelerator.SetWordWrap (False);
		fAccelerator.SetMultiLine (False);
		fEntryGroup.AddSubView (&fAccelerator);

		fMneumonic.SetExtent (80, 89, 26, 43, eNoUpdate);
		fMneumonic.SetFont (&kApplicationFont);
		fMneumonic.SetController (this);
		fMneumonic.SetWordWrap (False);
		fMneumonic.SetMultiLine (False);
		fEntryGroup.AddSubView (&fMneumonic);

		fCommand.SetExtent (27, 73, 26, 105, eNoUpdate);
		fCommand.SetFont (&kApplicationFont);
		fCommand.SetController (this);
		fCommand.SetWordWrap (False);
		fCommand.SetMultiLine (False);
		fEntryGroup.AddSubView (&fCommand);

		fLabel.SetExtent (2, 46, 26, 133, eNoUpdate);
		fLabel.SetFont (&kApplicationFont);
		fLabel.SetController (this);
		fLabel.SetWordWrap (False);
		fLabel.SetMultiLine (False);
		fEntryGroup.AddSubView (&fLabel);

		fCheckBox.SetLabel ("CheckBox", eNoUpdate);
		fCheckBox.SetExtent (110, 91, 25, 94, eNoUpdate);
		fCheckBox.SetController (this);
		fEntryGroup.AddSubView (&fCheckBox);

		fExtended.SetLabel ("Extended", eNoUpdate);
		fExtended.SetExtent (111, 3, 25, 90, eNoUpdate);
		fExtended.SetController (this);
		fEntryGroup.AddSubView (&fExtended);

		fCommandLabel.SetExtent (28, 2, 21, 71, eNoUpdate);
		fCommandLabel.SetFont (&kSystemFont);
		fCommandLabel.SetText ("Command:");
		fEntryGroup.AddSubView (&fCommandLabel);

		fAcceleratorLabel.SetExtent (56, 5, 21, 82, eNoUpdate);
		fAcceleratorLabel.SetFont (&kSystemFont);
		fAcceleratorLabel.SetText ("Accelerator:");
		fEntryGroup.AddSubView (&fAcceleratorLabel);

		fMneumonicLabel.SetExtent (83, 6, 21, 79, eNoUpdate);
		fMneumonicLabel.SetFont (&kSystemFont);
		fMneumonicLabel.SetText ("Mneumonic:");
		fEntryGroup.AddSubView (&fMneumonicLabel);

		fLabelLabel.SetExtent (3, 3, 21, 43, eNoUpdate);
		fLabelLabel.SetFont (&kSystemFont);
		fLabelLabel.SetText ("Label:");
		fEntryGroup.AddSubView (&fLabelLabel);

		fEntryGroupTabLoop.AddFocus (&fExtended);
		fEntryGroupTabLoop.AddFocus (&fCheckBox);
		fEntryGroupTabLoop.AddFocus (&fLabel);
		fEntryGroupTabLoop.AddFocus (&fCommand);
		fEntryGroupTabLoop.AddFocus (&fAccelerator);
		fEntryGroupTabLoop.AddFocus (&fMneumonic);
	AddSubView (&fEntryGroup);

	fField1.SetExtent (26, 2, 21, 38, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Title:");
	AddSubView (&fField1);

	fDelete.SetExtent (28, 300, 24, 70, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	fInsert.SetExtent (28, 215, 24, 70, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	AddFocus (&fName);
	AddFocus (&fList);
	AddFocus (&fInsert);
	AddFocus (&fDelete);
	AddFocus (&fEntryGroupTabLoop);
}

#else

void	MenuTitleInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (166, 354), eNoUpdate);

	fTitle.SetExtent (5, 3, 16, 346, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set MenuTitle Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fName.SetExtent (28, 42, 16, 127, eNoUpdate);
	fName.SetFont (&kApplicationFont);
	fName.SetController (this);
	fName.SetWordWrap (False);
	AddSubView (&fName);

	fList.SetExtent (52, 6, 70, 146, eNoUpdate);
	fList.SetItemsFloat (True);
	fList.SetController (this);
	AddSubView (&fList);

	fEntryGroup.SetExtent (52, 155, 109, 172, eNoUpdate);
	fEntryGroup.SetBorder (1, 1, eNoUpdate);
	fEntryGroup.SetMargin (1, 1, eNoUpdate);
		fAccelerator.SetExtent (48, 90, 16, 43, eNoUpdate);
		fAccelerator.SetFont (&kApplicationFont);
		fAccelerator.SetController (this);
		fAccelerator.SetWordWrap (False);
		fAccelerator.SetMultiLine (False);
		fEntryGroup.AddSubView (&fAccelerator);

		fMneumonic.SetExtent (66, 90, 16, 43, eNoUpdate);
		fMneumonic.SetFont (&kApplicationFont);
		fMneumonic.SetController (this);
		fMneumonic.SetWordWrap (False);
		fMneumonic.SetMultiLine (False);
		fEntryGroup.AddSubView (&fMneumonic);

		fCommand.SetExtent (25, 73, 17, 95, eNoUpdate);
		fCommand.SetFont (&kApplicationFont);
		fCommand.SetController (this);
		fCommand.SetWordWrap (False);
		fCommand.SetMultiLine (False);
		fEntryGroup.AddSubView (&fCommand);

		fLabel.SetExtent (3, 43, 16, 125, eNoUpdate);
		fLabel.SetFont (&kApplicationFont);
		fLabel.SetController (this);
		fLabel.SetWordWrap (False);
		fLabel.SetMultiLine (False);
		fEntryGroup.AddSubView (&fLabel);

		fCheckBox.SetLabel ("CheckBox", eNoUpdate);
		fCheckBox.SetExtent (91, 83, 15, 80, eNoUpdate);
		fCheckBox.SetController (this);
		fEntryGroup.AddSubView (&fCheckBox);

		fExtended.SetLabel ("Extended", eNoUpdate);
		fExtended.SetExtent (91, 3, 15, 78, eNoUpdate);
		fExtended.SetController (this);
		fEntryGroup.AddSubView (&fExtended);

		fCommandLabel.SetExtent (26, 2, 16, 69, eNoUpdate);
		fCommandLabel.SetFont (&kSystemFont);
		fCommandLabel.SetText ("Command:");
		fEntryGroup.AddSubView (&fCommandLabel);

		fAcceleratorLabel.SetExtent (48, 6, 16, 81, eNoUpdate);
		fAcceleratorLabel.SetFont (&kSystemFont);
		fAcceleratorLabel.SetText ("Accelerator:");
		fEntryGroup.AddSubView (&fAcceleratorLabel);

		fMneumonicLabel.SetExtent (65, 7, 16, 80, eNoUpdate);
		fMneumonicLabel.SetFont (&kSystemFont);
		fMneumonicLabel.SetText ("Mneumonic:");
		fEntryGroup.AddSubView (&fMneumonicLabel);

		fLabelLabel.SetExtent (3, 3, 16, 40, eNoUpdate);
		fLabelLabel.SetFont (&kSystemFont);
		fLabelLabel.SetText ("Label:");
		fEntryGroup.AddSubView (&fLabelLabel);

		fEntryGroupTabLoop.AddFocus (&fLabel);
		fEntryGroupTabLoop.AddFocus (&fCommand);
		fEntryGroupTabLoop.AddFocus (&fAccelerator);
		fEntryGroupTabLoop.AddFocus (&fMneumonic);
	AddSubView (&fEntryGroup);

	fField1.SetExtent (28, 8, 16, 33, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Title:");
	AddSubView (&fField1);

	fDelete.SetExtent (28, 260, 20, 60, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	fInsert.SetExtent (28, 175, 20, 60, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	AddFocus (&fName);
	AddFocus (&fEntryGroupTabLoop);
}

#endif /* GUI */

Point	MenuTitleInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (166, 354));
#elif   qMotifUI
	return (Point (208, 391));
#else
	return (Point (166, 354));
#endif /* GUI */
}

void	MenuTitleInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfNameSize = fName.GetSize ();
		fName.SetSize (kOriginalfNameSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfListSize = fList.GetSize ();
		fList.SetSize (kOriginalfListSize - Point (kSizeDelta.GetV (), 0));
		static const Point	kOriginalfEntryGroupOrigin = fEntryGroup.GetOrigin ();
		fEntryGroup.SetOrigin (kOriginalfEntryGroupOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfDeleteOrigin = fDelete.GetOrigin ();
		fDelete.SetOrigin (kOriginalfDeleteOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfInsertOrigin = fInsert.GetOrigin ();
		fInsert.SetOrigin (kOriginalfInsertOrigin - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"EmilyWindow.hh"

MenuTitleInfo::MenuTitleInfo (const MenuTitleEntry& entry) :
	MenuTitleInfoX (),
	fMenu (entry)
{
	GetNameField ().SetText (fMenu.GetTitle ());
	
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuItemEntryPtr, it, fMenu.GetItems ().operator Iterator(MenuItemEntryPtr)* ()) {
#else
	ForEach (MenuItemEntryPtr, it, fMenu.GetItems ()) {
#endif
		fList.AddItem (it.Current ()->GetLabel ());
	}
}

void	MenuTitleInfo::ClearFields ()
{
	fLabel.SetText (kEmptyString);
	fCommand.SetText (kEmptyString);
	fAccelerator.SetText (kEmptyString);
	fMneumonic.SetText (kEmptyString);
	fExtended.SetOn (False);
	fCheckBox.SetOn (False);
}

void	MenuTitleInfo::ButtonPressed (AbstractButton* button)
{
	if (button == &fList) {
		AbstractPickListItem*	item = fList.GetSelectedItem ();
		if (item == Nil) {
			ClearFields ();
		}
		else {
			CollectionSize index = fList.GetItemIndex (item);
			MenuItemEntry* entry = fMenu.GetItems ()[index];
			
			fLabel.SetText (entry->GetLabel ());
			fLabel.SetSelection ();
			fCommand.SetText (entry->GetCommandNumber ());
			fCommand.SetSelection ();
			fAccelerator.SetText (entry->GetAccelerator ());
			fMneumonic.SetText (entry->GetMneumonic ());
			fCheckBox.SetOn (entry->GetCheckBox ());
			fExtended.SetOn (entry->GetExtended ());
		}
	}
	else if (button == &fInsert) {
		AbstractPickListItem*	item = fList.GetSelectedItem ();
		CollectionSize index = kBadSequenceIndex;
		if (item != Nil) {
			index = fList.GetItemIndex (item);
		}

		MenuItemEntry* entry;
		if (index != kBadSequenceIndex) {
			entry = fMenu.GetItems ()[index];
		}
		else {
			entry = new MenuItemEntry ();
		}

		entry->SetLabel (fLabel.GetText ());
		entry->SetCommandNumber (fCommand.GetText ());
		entry->SetAccelerator (fAccelerator.GetText ());
		entry->SetMneumonic (fMneumonic.GetText ());
		entry->SetExtended (fExtended.GetOn ());
		entry->SetCheckBox (fCheckBox.GetOn ());
		
		String	foo = entry->GetLabel ();
		Assert (foo != kEmptyString);
		if (item == Nil) {
			fList.AddItem (foo);
			fMenu.GetItems ().Append (entry);
		}
		else {
			((StringPickListItem*)item)->SetText (foo);
		}
		ClearFields ();
		
		fEntryGroupTabLoop.SetCurrentFocus (&fLabel);
		fList.SelectAll (not PickList::kSelected);
	}
	else if (button == &fDelete) {
		CollectionSize index = fList.GetItemIndex (fList.GetSelectedItem ());
		
		Assert (index != kBadSequenceIndex);
		
		fList.Delete (fList.GetSelectedItem ());
		MenuItemEntry* entry = fMenu.GetItems ()[index];
		fMenu.GetItems ().RemoveAt (index);
		delete entry;

		ClearFields ();
	}
	else {
		MenuTitleInfoX::ButtonPressed (button);
	}
	
	if (fList.GetSelectedItem () != Nil) {
		fInsert.SetLabel ("Change");
		fDelete.SetEnabled (True);
	}
	else {
		fInsert.SetLabel ("Insert");
		fDelete.SetEnabled (False);
	}
	fInsert.SetEnabled (Boolean ((fLabel.GetLength () > 0) and (fCommand.GetLength () > 0)));
}

void	MenuTitleInfo::TextChanged (TextEditBase* item)
{
	TextController::TextChanged (item);
	fInsert.SetEnabled (Boolean ((fLabel.GetLength () > 0) or (fCommand.GetLength () > 0)));
}

const MenuTitleEntry&	MenuTitleInfo::GetMenu () const
{
	return (fMenu);
}

