/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ReorderFocusItems.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: ReorderFocusItems.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.7  92/04/08  17:23:12  17:23:12  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *		Revision 1.2  1992/03/06  21:52:58  sterling
 *		motif
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "ReorderFocusItems.hh"


ReorderFocusItemX::ReorderFocusItemX () :
	fItems (),
	fTitle ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

ReorderFocusItemX::~ReorderFocusItemX ()
{
	RemoveSubView (&fItems);
	RemoveSubView (&fTitle);
}

#if   qMacUI

void	ReorderFocusItemX::BuildForMacUI ()
{
	SetSize (Point (200, 200), eNoUpdate);

	fItems.SetExtent (52, 3, 143, 192, eNoUpdate);
	fItems.SetItemsFloat (True);
	fItems.SetController (this);
	AddSubView (&fItems);

	fTitle.SetExtent (0, 0, 50, 197, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fTitle.SetBorder (0, 0, eNoUpdate);
	fTitle.SetMargin (0, 0, eNoUpdate);
	fTitle.SetText ("Rearrange the list of focus items into the desired tabbing order.");
	AddSubView (&fTitle);

}

#elif qMotifUI

void	ReorderFocusItemX::BuildForMotifUI ()
{
	SetSize (Point (200, 200), eNoUpdate);

	fItems.SetExtent (52, 3, 141, 192, eNoUpdate);
	fItems.SetItemsFloat (True);
	fItems.SetController (this);
	AddSubView (&fItems);

	fTitle.SetExtent (0, 0, 50, 197, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fTitle.SetBorder (0, 0, eNoUpdate);
	fTitle.SetMargin (0, 0, eNoUpdate);
	fTitle.SetText ("Rearrange the list of focus items into the desired tabbing order.");
	AddSubView (&fTitle);

	AddFocus (&fItems);
}

#else

void	ReorderFocusItemX::BuildForUnknownGUI ();
{
	SetSize (Point (200, 200), eNoUpdate);

	fItems.SetExtent (52, 3, 143, 192, eNoUpdate);
	fItems.SetItemsFloat (True);
	fItems.SetController (this);
	AddSubView (&fItems);

	fTitle.SetExtent (0, 0, 50, 197, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fTitle.SetBorder (0, 0, eNoUpdate);
	fTitle.SetMargin (0, 0, eNoUpdate);
	fTitle.SetText ("Rearrange the list of focus items into the desired tabbing order.");
	AddSubView (&fTitle);

}

#endif /* GUI */

Point	ReorderFocusItemX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (200, 200));
#elif   qMotifUI
	return (Point (200, 200));
#else
	return (Point (200, 200));
#endif /* GUI */
}

void	ReorderFocusItemX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfItemsSize = fItems.GetSize ();
		fItems.SetSize (kOriginalfItemsSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!

#include "EmilyWindow.hh"

ReorderFocusItem::ReorderFocusItem (GroupItem& group) :
	fGroup (group),
	fFocusItems ()
{
	fItems.SetOwner (this);
	
	CommandNumber gui = EmilyWindow::GetGUI ();
	Font f = fItems.GetEffectiveFont ();
	Set_BitString(FontStyle)	style = kPlainFontStyle;
	style.Add (Font::eItalicFontStyle);
	f.SetStyle (style);
	
	ForEach (ViewItemPtr, it, fGroup.MakeFocusItemIterator ()) {
		fFocusItems.Append (it.Current ());
		String s = it.Current ()->GetFieldName ();
		if (s == kEmptyString) {
			s = "Anonymous " + it.Current ()->GetFieldClass ();
		}

		StringPickListItem& item = *fItems.AddItem (s);
		if (not it.Current ()->IsFocusItem (gui)) {
			item.SetFont (&f);
		}
	}
}
		
SequenceIterator(ViewItemPtr)*	ReorderFocusItem::MakeFocusItemIterator (SequenceDirection d) const
{
	return (fFocusItems.MakeSequenceIterator (d));
}

void	ReorderFocusItem::ListOrderChanged (CollectionSize oldIndex, CollectionSize newIndex)
{
	Require (oldIndex != kBadSequenceIndex);
	Require (newIndex != kBadSequenceIndex);
	if (oldIndex != newIndex) {
		ViewItem*	item = fFocusItems[oldIndex];
		fFocusItems.Remove (item);
		fFocusItems.InsertAt (item, newIndex);
	}
}





ReorderFIPickList::ReorderFIPickList (ButtonController* controller) :
	StringPickList (controller),
	fOwner (Nil)
{
}
		
void	ReorderFIPickList::ReorderItem (AbstractPickListItem* item, CollectionSize index)
{
	CollectionSize oldIndex = GetItemIndex (item);
	StringPickList::ReorderItem (item, index);
	AssertNotNil (fOwner);
	fOwner->ListOrderChanged (oldIndex, index);
}

void	ReorderFIPickList::ReorderItem (AbstractPickListItem* item, AbstractPickListItem* neighbor, AddMode append)
{
	StringPickList::ReorderItem (item, neighbor, append);
	// automatically calls other flavor of ReorderItem
}

void	ReorderFIPickList::SetOwner (ReorderFocusItem* owner)
{
	Require (fOwner == Nil);
	fOwner = owner;
	Ensure (fOwner == owner);
}
