/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PopUpInfo.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PopUpInfo.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.23  1992/05/19  11:36:03  sterling
 *		hi
 *
 *		Revision 1.22  92/05/13  18:47:20  18:47:20  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.20  92/04/08  17:23:08  17:23:08  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 *
 *
 */


#include "Menu.hh"
#include "MenuOwner.hh"
#include "ViewItemInfo.hh"

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "PopUpInfo.hh"


PopUpInfoX::PopUpInfoX () :
	fLabel (),
	fList (),
	fTitle (),
	fEntry (),
	fEntryLabel (),
	fLabelLabel (),
	fViewInfo (),
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

PopUpInfoX::~PopUpInfoX ()
{
	RemoveFocus (&fEntry);
	RemoveFocus (&fLabel);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fLabel);
	RemoveSubView (&fList);
	RemoveSubView (&fTitle);
	RemoveSubView (&fEntry);
	RemoveSubView (&fEntryLabel);
	RemoveSubView (&fLabelLabel);
	RemoveSubView (&fViewInfo);
	RemoveSubView (&fDelete);
	RemoveSubView (&fInsert);
}

#if   qMacUI

void	PopUpInfoX::BuildForMacUI ()
{
	SetSize (Point (330, 350), eNoUpdate);

	fLabel.SetExtent (119, 44, 18, 180, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fList.SetExtent (28, 8, 87, 177, eNoUpdate);
	fList.SetItemsFloat (True);
	fList.SetController (this);
	AddSubView (&fList);

	fTitle.SetExtent (5, 5, 21, 338, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set PopUp Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fEntry.SetExtent (72, 234, 26, 106, eNoUpdate);
	fEntry.SetFont (&kApplicationFont);
	fEntry.SetController (this);
	fEntry.SetWordWrap (False);
	fEntry.SetMultiLine (False);
	AddSubView (&fEntry);

	fEntryLabel.SetExtent (74, 189, 21, 43, eNoUpdate);
	fEntryLabel.SetFont (&kSystemFont);
	fEntryLabel.SetText ("Entry");
	AddSubView (&fEntryLabel);

	fLabelLabel.SetExtent (119, 3, 21, 46, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label:");
	AddSubView (&fLabelLabel);

	fViewInfo.SetExtent (140, 4, 187, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	fDelete.SetExtent (35, 273, 24, 70, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	fInsert.SetExtent (35, 199, 24, 70, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	AddFocus (&fEntry);
	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	PopUpInfoX::BuildForMotifUI ()
{
	SetSize (Point (358, 351), eNoUpdate);

	fLabel.SetExtent (118, 49, 26, 143, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	fLabel.SetMultiLine (False);
	AddSubView (&fLabel);

	fList.SetExtent (28, 8, 87, 177, eNoUpdate);
	fList.SetItemsFloat (True);
	fList.SetController (this);
	AddSubView (&fList);

	fTitle.SetExtent (5, 5, 21, 338, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set PopUp Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fEntry.SetExtent (72, 234, 26, 106, eNoUpdate);
	fEntry.SetFont (&kApplicationFont);
	fEntry.SetController (this);
	fEntry.SetWordWrap (False);
	fEntry.SetMultiLine (False);
	AddSubView (&fEntry);

	fEntryLabel.SetExtent (74, 189, 21, 43, eNoUpdate);
	fEntryLabel.SetFont (&kSystemFont);
	fEntryLabel.SetText ("Entry");
	AddSubView (&fEntryLabel);

	fLabelLabel.SetExtent (119, 3, 21, 46, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label:");
	AddSubView (&fLabelLabel);

	fViewInfo.SetExtent (148, 4, 208, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	fDelete.SetExtent (35, 273, 24, 70, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	fInsert.SetExtent (35, 199, 24, 70, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	AddFocus (&fEntry);
	AddFocus (&fList);
	AddFocus (&fInsert);
	AddFocus (&fDelete);
	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#else

void	PopUpInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (330, 350), eNoUpdate);

	fLabel.SetExtent (119, 44, 18, 180, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fList.SetExtent (28, 8, 87, 177, eNoUpdate);
	fList.SetItemsFloat (True);
	fList.SetController (this);
	AddSubView (&fList);

	fTitle.SetExtent (5, 5, 21, 338, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set PopUp Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fEntry.SetExtent (72, 234, 26, 106, eNoUpdate);
	fEntry.SetFont (&kApplicationFont);
	fEntry.SetController (this);
	fEntry.SetWordWrap (False);
	fEntry.SetMultiLine (False);
	AddSubView (&fEntry);

	fEntryLabel.SetExtent (74, 189, 21, 43, eNoUpdate);
	fEntryLabel.SetFont (&kSystemFont);
	fEntryLabel.SetText ("Entry");
	AddSubView (&fEntryLabel);

	fLabelLabel.SetExtent (119, 3, 21, 46, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label:");
	AddSubView (&fLabelLabel);

	fViewInfo.SetExtent (140, 4, 187, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	fDelete.SetExtent (35, 273, 24, 70, eNoUpdate);
	fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fDelete.SetLabel ("Delete", eNoUpdate);
	fDelete.SetController (this);
	AddSubView (&fDelete);

	fInsert.SetExtent (35, 199, 24, 70, eNoUpdate);
	fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fInsert.SetLabel ("Insert", eNoUpdate);
	fInsert.SetController (this);
	AddSubView (&fInsert);

	AddFocus (&fEntry);
	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	PopUpInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (330, 350));
#elif   qMotifUI
	return (Point (358, 351));
#else
	return (Point (330, 350));
#endif /* GUI */
}

void	PopUpInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfLabelSize = fLabel.GetSize ();
		fLabel.SetSize (kOriginalfLabelSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfListSize = fList.GetSize ();
		fList.SetSize (kOriginalfListSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfEntryOrigin = fEntry.GetOrigin ();
		fEntry.SetOrigin (kOriginalfEntryOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfEntryLabelOrigin = fEntryLabel.GetOrigin ();
		fEntryLabel.SetOrigin (kOriginalfEntryLabelOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfDeleteOrigin = fDelete.GetOrigin ();
		fDelete.SetOrigin (kOriginalfDeleteOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfInsertOrigin = fInsert.GetOrigin ();
		fInsert.SetOrigin (kOriginalfInsertOrigin - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include "PopUpItem.hh"

#include "Menu.hh"
#include "MenuOwner.hh"
#include "OptionMenu.hh"

PopUpInfo::PopUpInfo (OptionMenuItem& view) :
	PopUpInfoX ()
{
	fViewInfo.SetUpFromView (view);
	
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	
	GetLabelField ().SetText (view.GetOptionMenu ().GetLabel (), eNoUpdate);

#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (String, it, view.GetOptionMenu ().GetItems ().operator Iterator(String)* ()) {
#else
	ForEach (String, it, view.GetOptionMenu ()GetItems ()) {
#endif
		GetListField ().AddItem (it.Current ());
	}
}

void	PopUpInfo::ButtonPressed (AbstractButton* button)
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
		PopUpInfoX::ButtonPressed (button);
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

void	PopUpInfo::TextChanged (TextEditBase* item)
{
	TextController::TextChanged (item);
	fInsert.SetEnabled (Boolean (fEntry.GetLength () > 0));
}


