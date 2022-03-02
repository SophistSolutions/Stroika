/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PickList.cc,v 1.8 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PickList.cc,v $
 *		Revision 1.8  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  21:36:22  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.4  1992/07/03  04:06:55  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.3  1992/07/02  05:12:48  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.18  1992/06/09  16:13:05  sterling
 *		fixed step and page sizes
 *
 *		Revision 1.17  1992/05/20  00:26:30  sterling
 *		fixed scrolling/border bug
 *		
 *		Revision 1.15  92/04/15  13:26:21  13:26:21  sterling (Sterling Wight)
 *		resized picklistitems when picklist changes size
 *		
 *		Revision 1.14  92/03/26  09:24:26  09:24:26  lewis (Lewis Pringle)
 *		Got rid of oldLive first argument to EffectiveLiveChanged () method.
 *		
 *		Revision 1.13  1992/03/19  20:47:38  sterling
 *		made focusitem under mouseless interface
 *
 *		Revision 1.12  1992/03/11  23:10:19  lewis
 *		Got  rid of random anonymous g++ bug workaround.
 *
 *		Revision 1.11  1992/03/05  21:53:59  sterling
 *		support for borders
 *
 *		Revision 1.10  1992/02/15  06:29:16  sterling
 *		use EffectiveLiveChanged
 *
 *		Revision 1.8  1992/01/14  06:10:30  lewis
 *		Update for Font Style Scoping. and change in ctor for ShapeDragger.
 *
 *
 */

#include	"Debug.hh"	

#include	"Shape.hh"

#include	"Application.hh"
#include	"DeskTop.hh"
#include	"ScrollBar.hh"

#include	"PickList.hh"	
		
		
#if		!qRealTemplatesAvailable
	Implement (Iterator, PLstItmPtr);
	Implement (Collection, PLstItmPtr);
	Implement (AbSequence, PLstItmPtr);
	Implement (Array, PLstItmPtr);
	Implement (Sequence_Array, PLstItmPtr);
	Implement (Sequence, PLstItmPtr);
#endif






/*
 ********************************************************************************
 ******************************** AbstractPickList ******************************
 ********************************************************************************
 */
 
const	Boolean	AbstractPickList::kSelected = True;

AbstractPickList::AbstractPickList (ButtonController* controller):
	fScroller (Nil),
	fScrolledView (Nil),
	fAllowMultipleSelections (False),
	fItemsFloat (False),
	fSelected (0)
{
	fScroller = new PickListScroller (*this);
	fScrolledView = new PickListItemContainer (*this);
	AddSubView (fScroller);
	fScroller->SetScrolledViewAndTargets (fScrolledView, Nil, Nil, eNoUpdate);
	SetBorder (1, 1, eNoUpdate);
	SetController (controller);
}

AbstractPickList::~AbstractPickList ()
{
// HACK: want to inval visible area cache as an optimization, but is currently private: how to resolve?
SetVisible (not GetVisible (), eNoUpdate);
	SetController (Nil);
	DeleteAll ();
	fScroller->SetScrolledViewAndTargets (Nil, Nil, Nil, eNoUpdate);
	RemoveSubView (fScroller);
	delete fScrolledView;
}

void	AbstractPickList::Draw (const Region& update)
{
	DrawBorder ();
	EnableView::Draw (update);
}

void	AbstractPickList::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	EnableView::EffectiveLiveChanged (newLive, updateMode);
}

void	AbstractPickList::ItemSelected (AbstractPickListItem* item)
{
	RequireNotNil (item);
	if (item->GetSelected ()) {
		fSelected++;
	}
	else {
		fSelected--;
	}
	ButtonPressed (this);
	Ensure (fSelected >= 0);
	Ensure ((GetAllowMultipleSelections ()) or (fSelected <= 1));
}

void	AbstractPickList::Layout ()
{
	View::Layout ();
	GetScroller ().SetExtent (GetLocalExtent ().InsetBy (GetMargin() + GetBorder ()));
	if (GetScroller ().GetHorizontalScrollBar () == Nil) {
		Coordinate width = GetSize ().GetH ();
		if (GetScroller ().GetVerticalScrollBar () != Nil) {
			width -= GetScroller ().GetVerticalScrollBar ()->GetSize ().GetH ();
		}
		
		fScrolledView->SetSize (Point (fScrolledView->GetSize ().GetV (), width));
	}
}

class	ItemDragger	: public ShapeDragger {
	public:
		ItemDragger (AbstractPickListItem& item) :
			ShapeDragger (RegionShape (item.GetRegion ()), item.GetExtent (), *item.GetParentView (), ShapeDragger::eVertical),
			fItem (item)
		{
			SetHysteresis (Point (1, 3));
			SetGrid (Point (item.GetSize ().GetV (), 1));
		}

		override	Boolean	TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next)
		{
			Boolean	result = ShapeDragger::TrackResult (phase, anchor, previous, next);
			if (phase == eTrackRelease) {
				Point	delta = GetNewShapeExtent ().GetOrigin () - GetOldShapeExtent ().GetOrigin ();
				if (delta != kZeroPoint) {
					AbstractPickList&	list = fItem.GetPickList ();
					AbstractPickListItem*	anItem = list.PointInWhichItem (anchor + delta);
					if (anItem != &fItem) {
						if (anItem != Nil) {
							list.ReorderItem (&fItem, anItem);
						}
						else {
							list.ReorderItem (&fItem, list.GetItemCount ());
						}
						list.GetScrolledView ().InvalidateLayout ();
						list.GetScrolledView ().ProcessLayout ();
					}
				}
			}
			return (result);
		}
	private:
		AbstractPickListItem& fItem;
};

Boolean	AbstractPickList::TrackPress (const MousePressInfo& mouseInfo)
{
#if		qMouselessInterface
	if ((not GetEffectiveFocused ()) and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
	}
#endif
	
	if (not (View::TrackPress (mouseInfo))) {
		// do my own tracking here.
		AbstractPickListItem*	item;
		if (((item = PointInWhichItem (mouseInfo.fPressAt)) != Nil) and (item->GetLive ())) {
			if (item->GetSelected ()) {
				if (mouseInfo.IsExtendedSelection ()) {
					item->SetSelected (False, eImmediateUpdate);
				}
				else if (GetItemsFloat () and (CountSelected () == 1)) {
					ItemDragger shapeDragger = ItemDragger (*item);
					(void) shapeDragger.TrackPress (mouseInfo);
				}
			}
			else {
				if (not (GetAllowMultipleSelections () and mouseInfo.IsExtendedSelection ())) {
					// we only want to tell our controller that we have changed once,
					// and that will be done below by the SetSelected call
					// so we temporarily cut the controller chain
					ButtonController*	oldController = GetController ();
					SetController (Nil);
					SelectAll (not kSelected, eImmediateUpdate);
					SetController (oldController);
				}
				item->SetSelected (kSelected, eImmediateUpdate);
				if (GetItemsFloat () and (CountSelected () == 1)) {
					ItemDragger shapeDragger = ItemDragger (*item);
					(void) shapeDragger.TrackPress (mouseInfo);
				}
			}
		}
		else {
			SelectAll (not kSelected, eImmediateUpdate);
		}
	}
	return (True);
}

void	AbstractPickList::LayoutItems ()
{
	Point		itemOrigin = GetScroller ().GetContentArea ().GetBounds ().GetOrigin ();
	Coordinate	itemWidth  = GetScroller ().GetContentArea ().GetBounds ().GetWidth ();
	Coordinate	newHeight = 0;
	ForEach (PLstItmPtr, it, MakeItemIterator ()) {
		AbstractPickListItem*	item	=	it.Current ();
		AssertNotNil (item);
		Assert (&item->GetPickList () == this);
		if (item->GetVisible ()) {
			item->SetOrigin (itemOrigin);
			item->SetSize (Point (item->GetLineHeight (), itemWidth));
			newHeight += item->GetSize ().GetV ();
			itemOrigin.SetV (itemOrigin.GetV () + item->GetSize ().GetV ());
		}
	}
	Point	newSize	=	Point (newHeight, itemWidth);
	if (newSize != GetScrolledView ().GetSize ()) {
		GetScrolledView ().SetSize (newSize);
		GetScroller ().ScrolledViewChangedSize ();		// if we had vbase stuff would be auto from above setsize...
	}
}

AbstractPickListItem*	AbstractPickList::PointInWhichItem (const Point& where)
{
	Point	globalWhere = LocalToAncestor (where, &DeskTop::Get ());
	ForEach (PLstItmPtr, it, MakeItemIterator ()) {
		AbstractPickListItem*	item	=	it.Current ();
		AssertNotNil (item);
		if ((item->GetVisible ()) and (item->Contains (item->LocalToEnclosure (item->AncestorToLocal (globalWhere, &DeskTop::Get ()))))) {
			return (item);	
		}
	}
	return (Nil);	
}

AbstractPickListItem*	AbstractPickList::AddItem (AbstractPickListItem* item, CollectionSize index)
{
	RequireNotNil (item);
	Require (item->fPickList == Nil);
	if (index == kBadSequenceIndex) {
		index = GetItemCount () + 1;
	}

	item->fPickList = this;
	fPickListItems.InsertAt (item, index);
	GetScrolledView ().AddSubView (item, index);
	return (item);
}

AbstractPickListItem*	AbstractPickList::AddItem (AbstractPickListItem* item, AbstractPickListItem* neighbor, AddMode addMode)
{
	RequireNotNil (neighbor);
	CollectionSize	index = fPickListItems.IndexOf (neighbor);
	Require (index != kBadSequenceIndex);
	AddItem (item, (addMode == eAppend) ? index + 1 : index);
	return (item);
}

void	AbstractPickList::RemoveItem (AbstractPickListItem* item)
{
	RequireNotNil (item);
	Require (item->fPickList == this);
	fPickListItems.Remove (item);
	item->fPickList = Nil;
	GetScrolledView ().RemoveSubView (item);
	GetScrolledView ().InvalidateLayout ();
}

void	AbstractPickList::ReorderItem (AbstractPickListItem* item, CollectionSize index)
{
	CollectionSize	oldIndex = fPickListItems.IndexOf (item);
	Require (oldIndex != kBadSequenceIndex);
	if (index != oldIndex) {
		fPickListItems.Remove (item);
		fPickListItems.InsertAt (item, index);
		GetScrolledView ().ReorderSubView (item, index);		// keep views in same order...
	}
	Ensure (fPickListItems.IndexOf (item) == index);
}

void	AbstractPickList::ReorderItem (AbstractPickListItem* item, AbstractPickListItem* neighbor, AddMode addMode)
{
	RequireNotNil (neighbor);
	Require (item != neighbor);
	
	CollectionSize	index = fPickListItems.IndexOf (neighbor);
	CollectionSize	oldIndex = fPickListItems.IndexOf (item);

	Require (index != kBadSequenceIndex);
	Require (oldIndex != kBadSequenceIndex);
	
	if (oldIndex < index) {
		index--;
	}
	ReorderItem (item, (addMode == eAppend) ? index + 1 : index);
}

CollectionSize	AbstractPickList::GetItemCount () const
{
	return (fPickListItems.GetLength ());
}

CollectionSize	AbstractPickList::GetItemIndex (AbstractPickListItem* item)
{
	RequireNotNil (item);
	Assert (fPickListItems.IndexOf (item) == GetScrolledView ().GetSubViewIndex (item));		// dont count on it, but try to preserve anyhow?
	return (fPickListItems.IndexOf (item));
}

AbstractPickListItem*	AbstractPickList::GetItemByIndex (CollectionSize index)
{
	Require (index >= 1);
	Require (index <= fPickListItems.GetLength ());
	return (fPickListItems [index]);
}

void	AbstractPickList::Delete (AbstractPickListItem* item)
{
	RequireNotNil (item);
	item->SetSelected (False, eNoUpdate);
	RemoveItem (item);
	delete (item);
}

void	AbstractPickList::DeleteAll ()
{
	ForEach (PLstItmPtr, it, MakeItemIterator ()) {
		AbstractPickListItem*	item =	it.Current ();
		AssertNotNil (item);
		Delete (item);
	}
	Ensure (fPickListItems.GetLength () == 0);
}

SequenceIterator(PLstItmPtr)*	AbstractPickList::MakeItemIterator (SequenceDirection d)
{
	return (fPickListItems.MakeSequenceIterator (d));
}

void	AbstractPickList::SelectAll (Boolean selected, Panel::UpdateMode updateMode)
{
	Require ((not selected) or (GetAllowMultipleSelections ()));
	
	if (((selected) 	and (CountSelected () != GetItemCount ())) 
	 or ((not selected) and (CountSelected () > 0))) {

		// we only want to tell our controller that we have changed once,
		// so we temporarily cut the controller chain
		ButtonController*	oldController = GetController ();
		SetController (Nil);
		CollectionSize	goal = ((selected) ? GetItemCount () : 0);
		ForEach (PLstItmPtr, it, MakeItemIterator ()) {
			if (CountSelected () == goal) {
				break;
			}
			AbstractPickListItem*	item	=	it.Current ();
			AssertNotNil (item);
			item->SetSelected (selected, updateMode);
		}
		SetController (oldController);
		ButtonPressed (this);
	}
}

Boolean		AbstractPickList::GetAllowMultipleSelections () const
{
	return (fAllowMultipleSelections);
}

void	AbstractPickList::SetAllowMultipleSelections (Boolean allowMultipleSelections)
{
	fAllowMultipleSelections = allowMultipleSelections;
}

Boolean	AbstractPickList::GetItemsFloat () const
{
	return (fItemsFloat);
}

void	AbstractPickList::SetItemsFloat (Boolean floating)
{
	fItemsFloat = floating;
}

AbstractPickListItem*	AbstractPickList::GetSelectedItem (UInt32 i)
{
	UInt32	selectCount	=	0;
	ForEach (PLstItmPtr, it, MakeItemIterator ()) {
		AbstractPickListItem*	item	=	it.Current ();
		AssertNotNil (item);
		if (item->GetSelected ()) {
			selectCount++;
		}
		if (i == selectCount) {
			return (item);
		}
	}
	return (Nil);
}

UInt32	AbstractPickList::CountSelected () const
{
	Ensure (fSelected >= 0);
	Ensure ((fSelected <= 1) or (GetAllowMultipleSelections ()));
	return (fSelected);
}


void	AbstractPickList::ScrollToSelected ()
{
	AbstractPickListItem*	selectedItem	=	GetSelectedItem ();
	if (selectedItem != Nil) {
		AssertNotReached ();		// NYI
	}
}

Sequence(PLstItmPtr)&	AbstractPickList::GetPickListItems () const
{
	return (fPickListItems);
}

Scroller&	AbstractPickList::GetScroller () const
{
	EnsureNotNil (fScroller);
	return (*fScroller);
}

PickListItemContainer&	AbstractPickList::GetScrolledView () const
{
	EnsureNotNil (fScrolledView);
	return (*fScrolledView);
}

#if qMouselessInterface
Boolean	AbstractPickList::GetLive () const
{
	return (EnableView::GetLive ());
}
#endif




/*
 ********************************************************************************
 ******************************* PickListItemContainer **************************
 ********************************************************************************
 */

PickListItemContainer::PickListItemContainer (AbstractPickList& pickList):
	GroupView (),
	fPickList (pickList)
{
}

PickListItemContainer::~PickListItemContainer ()
{
}

void	PickListItemContainer::Layout ()
{
	fPickList.LayoutItems ();
	View::Layout ();
}



class	PickListScrollBar : public ScrollBar {
	public:
		PickListScrollBar (AbstractScrollBar::Orientation orientation, AbstractPickList& pickList) :
			ScrollBar (orientation),
			fPickList (pickList)
		{
		}
		
		virtual	void	foo ();	// COMPILER BUG
		override	Real	GetStepSize_ () const
		{
			if (fPickList.GetItemCount () > 0) {
				Coordinate	index = GetValue_ ();
				if (index == 0) {
					index = 1;
				}
				else if (index > fPickList.GetItemCount ()) {
					index = fPickList.GetItemCount ();
				}
				AbstractPickListItem*	item = fPickList.GetItemByIndex (index);
				AssertNotNil (item);
				return (item->GetLineHeight ());
			}
			else {
				return (ScrollBar::GetStepSize_ ());
			}
		}

		override	Real	GetPageSize_ () const
		{
			Coordinate	stepSize = GetStepSize_ ();
			if (stepSize > 0) {
				Coordinate	pageSize = (fPickList.GetSize ().GetV () / stepSize) * stepSize;
				Assert ((pageSize % stepSize) == 0);
				if (pageSize == 0) {
					pageSize = stepSize;
				}
				return (pageSize);
			}
			else {
				return (ScrollBar::GetPageSize_ ());
			}
		}
		
	private:
		AbstractPickList&	fPickList;
};
void	PickListScrollBar::foo ()	// COMPILER BUG
{
}

/*
 ********************************************************************************
 ************************************ PickListScroller **************************
 ********************************************************************************
 */

PickListScroller::PickListScroller (AbstractPickList& pickList):
	Scroller (Nil, Nil, Nil, new PickListScrollBar (AbstractScrollBar::eVertical, pickList), kBuildNoSlider),
	fPickList (pickList)
{
}

void	PickListScroller::AdjustStepAndPageSizes ()
{
	// we do nothing, cuz our special scroll bar handles this for us
	// (which allows for variable sized scrolling)
}




/*
 ********************************************************************************
 *****************************AbstractPickListItem ******************************
 ********************************************************************************
 */
const	Coordinate	AbstractPickListItem::kUseViewDefaultHeight	= 0;

AbstractPickListItem::AbstractPickListItem ():
	fPickList (Nil),
	fSelected (False)
{
}

AbstractPickListItem::~AbstractPickListItem ()
{
	Assert (fPickList == Nil);
}

void	AbstractPickListItem::Draw (const Region& /*update*/)
{
	if (GetSelected () and GetPickList ().GetLive ()) {
		Hilight ();
	}
}

AbstractPickList&	AbstractPickListItem::GetPickList () const
{
	EnsureNotNil (fPickList);
	return (*fPickList);
}

Boolean	AbstractPickListItem::GetSelected () const
{
	return (fSelected);
}

void	AbstractPickListItem::SetSelected (Boolean selected, Panel::UpdateMode updateMode)
{
	if (fSelected != selected) {
		if ((selected) and (not GetPickList ().GetAllowMultipleSelections ())) {
			GetPickList ().SelectAll (not PickList::kSelected, updateMode);
		}
		fSelected = selected;
		if (updateMode == eImmediateUpdate) {
			Hilight ();				// not guaranteed to work, but dont use this mode unless already drawn...
		}
		else {
			Refresh (updateMode);
		}
		GetPickList ().ItemSelected (this);
	}
}




/*
 ********************************************************************************
 ****************************** PickList ****************************************
 ********************************************************************************
 */
PickList::PickList (ButtonController* controller) :
	AbstractPickList (controller)
{
}

AbstractPickListItem*	PickList::GetItemFromView (View* view)
{
	ForEach (PLstItmPtr, it, MakeItemIterator ()) {
		PickListItem*	item	=	(PickListItem*) it.Current ();
		AssertNotNil (item);
		if (item->fItsView == view) {
			return (item);
		}
	}
	return (Nil);
}

AbstractPickListItem*	PickList::AddItem (View* item, CollectionSize index)
{
	return (AbstractPickList::AddItem (new PickListItem (item), index));
}

AbstractPickListItem*	PickList::AddItem (View* item, AbstractPickListItem* neighbor, AddMode append)
{
	return (AbstractPickList::AddItem (new PickListItem (item), neighbor, append));
}
				
void	PickList::RemoveItem (View* item)
{
	AbstractPickList::RemoveItem (GetItemFromView (item));
}

void	PickList::ReorderItem (View* item, CollectionSize index)
{
	AbstractPickList::ReorderItem (GetItemFromView (item), index);
}

void	PickList::ReorderItem (View* item, AbstractPickListItem* neighbor, AddMode append)
{
	AbstractPickList::ReorderItem (GetItemFromView (item), neighbor, append);
}

View*	PickList::GetItemByIndex (CollectionSize index)
{
	PickListItem*	item = (PickListItem*) AbstractPickList::GetItemByIndex (index);
	return ((item == Nil) ? Nil : item->fItsView);
}
/*
 ********************************************************************************
 ****************************** PickListItem ******************************
 ********************************************************************************
 */

	// sb scoped inside ctor but for bug in MPW CFRONT 1.0 final...
	class	MyMagicView : public View {
		public:
			MyMagicView (PickListItem& customPickListItem):
				fCustomPickListItem (customPickListItem)
				{
				}
			virtual	void	ptbl_hack ();
			override	void	Draw (const Region& /*update*/)
				{
// if/when we support being grayedout, also do that here...
					if (fCustomPickListItem.GetSelected ()) {
						Hilight ();
					}
				}
		private:
			PickListItem&	fCustomPickListItem;
	};
	void	MyMagicView::ptbl_hack () {}

PickListItem::PickListItem (View* itsView):
	AbstractPickListItem (),
	fItsView (itsView),
	fMagicView (Nil),
	fLineHeight (kUseViewDefaultHeight)
{
	RequireNotNil (itsView);

	/*
	 * NB: The ordering of these views is important.  Since we draw back to front, magicview
	 * must come first.
	 */
	AddSubView (itsView);
	AddSubView (fMagicView = new MyMagicView (*this));
}

PickListItem::~PickListItem ()
{
}

Coordinate	PickListItem::GetLineHeight () const
{
	if (fLineHeight == kUseViewDefaultHeight) {
		return (fItsView->CalcDefaultSize ().GetV ());
	}
	else {
		return (fLineHeight);
	}
}

void	PickListItem::SetLineHeight (Coordinate lineHeight, Panel::UpdateMode updateMode)
{
	if (fLineHeight != lineHeight) {
		fLineHeight = lineHeight;
		InvalidateLayout ();
		Refresh (updateMode);
	}
}

void	PickListItem::Layout ()
{
	AbstractPickListItem::Layout ();
	AssertNotNil (fItsView);
	AssertNotNil (fMagicView);
	fItsView->SetSize (GetSize ());
	fMagicView->SetSize (GetSize ());
}

void	PickListItem::Draw (const Region& update)
{
	EnableView::Draw (update);		// do NOT call AbstractPickListItem::Draw () since it would
								// do Hilighting/graying behviour at the wrong time
								// because of how drawing of subviews works...
								// See View::Render().
}

void	PickListItem::SetSelected (Boolean selected, Panel::UpdateMode updateMode)
{
	if (GetSelected () != selected) {
		AbstractPickListItem::SetSelected (selected, eNoUpdate);
		fMagicView->Refresh (fMagicView->GetLocalExtent (), updateMode);
		// directly calling fMagicView->Hilight when in immediateUpdate failed -- BAD STROIKA BUG
	}
}



/*
 ********************************************************************************
 ************************************ StringPickList ****************************
 ********************************************************************************
 */
StringPickList::StringPickList (ButtonController* controller):
	AbstractPickList (controller)
{
}

StringPickListItem*	StringPickList::StringToItem (const String& s, CollectionSize startIndex)
{
	CollectionSize countIndex = 1;	// this is grotesque, can't we build it into iterators???
	ForEach (PLstItmPtr, it, MakeItemIterator ()) {
		if (countIndex++ >= startIndex) {
			// I hate casting - must find better way...
			// I think templates will do the trick
			StringPickListItem*	item	=	(StringPickListItem*)it.Current ();
			if (item->GetText () == s) {
				return (item);
			}
		}
	}
	return (Nil);
}

String	StringPickList::GetSelectedString (UInt32 i)
{
	AbstractPickListItem*	pli	=	AbstractPickList::GetSelectedItem (i);
	if (pli == Nil) {
		return (kEmptyString);
	}
	else {
		// I hate casting - must find better way...
		return (((StringPickListItem*)pli)->GetText ());
	}
}

StringPickListItem*	StringPickList::AddItem (String s, CollectionSize index)
{
	return ((StringPickListItem*) AbstractPickList::AddItem (new StringPickListItem (s), index));
}

StringPickListItem*	StringPickList::AddItem (String s, AbstractPickListItem* neighbor, AddMode append)
{
	return ((StringPickListItem*) AbstractPickList::AddItem (new StringPickListItem (s), neighbor, append));
}

String	StringPickList::GetStringByIndex (CollectionSize index)
{
	AbstractPickListItem*	pli	=	GetItemByIndex (index);
	if (pli == Nil) {
		return (kEmptyString);
	}
	else {
		// I hate casting - must find better way...
		return (((StringPickListItem*)pli)->GetText ());
	}
}



/*
 ********************************************************************************
 ******************************** StringPickListItem ****************************
 ********************************************************************************
 */
StringPickListItem::StringPickListItem (const String& text):
	fLineHeight (kUseViewDefaultHeight),
	fText (text),
	fLeftMargin (4)
{
}

StringPickListItem::~StringPickListItem ()
{
}

String	StringPickListItem::GetText () const
{
	return (fText);
}

void	StringPickListItem::SetText (const String& text, Panel::UpdateMode updateMode)
{
	fText = text;
	Refresh (updateMode);
}

Coordinate	StringPickListItem::GetLineHeight () const
{
	if (fLineHeight == kUseViewDefaultHeight) {
		const Font&	f = GetEffectiveFont ();
		return (f.GetFontHeight ());
	}
	else {
		return (fLineHeight);
	}
}

void	StringPickListItem::SetLineHeight (Coordinate lineHeight, Panel::UpdateMode updateMode)
{
	if (fLineHeight != lineHeight) {
		fLineHeight = lineHeight;
		InvalidateLayout ();
		Refresh (updateMode);
	}
}

void	StringPickListItem::Draw (const Region& update)
{
	const Font&	f = GetEffectiveFont ();	
	DrawText (GetText (), Point ((GetSize ().GetV () - (f.GetFontHeight ()))/2, fLeftMargin));
	if (not GetEffectiveLive ()) {
		Gray ();
	}
	AbstractPickListItem::Draw (update);
}

Coordinate	StringPickListItem::GetLeftMargin () const
{
	return (fLeftMargin);
}

void		StringPickListItem::SetLeftMargin (Coordinate margin, Panel::UpdateMode updateMode)
{
	if (fLeftMargin != margin) {
		fLeftMargin = margin;
		Refresh (updateMode);
	}
}

StringPickList&	StringPickListItem::GetPickList () const
{
	return ((StringPickList&) AbstractPickListItem::GetPickList ());
}


/*
 ********************************************************************************
 ******************************* PixelMapStringPickListItem *********************
 ********************************************************************************
 */
PixelMapStringPickListItem::PixelMapStringPickListItem (const String& text, const PixelMap* icon) :
	StringPickListItem (text),
	fPixelMap (Nil)
{
	SetPixelMap (icon, eNoUpdate);
}

PixelMapStringPickListItem::~PixelMapStringPickListItem ()
{
	if (fPixelMap != Nil) {
		delete fPixelMap;
	}
}

Coordinate	PixelMapStringPickListItem::GetLineHeight () const
{
	Coordinate	lineHeight = StringPickListItem::GetLineHeight ();
	if (fPixelMap == Nil) {
		return (lineHeight);
	}
	else {
		return (Max (lineHeight, fPixelMap->GetBounds ().GetHeight ()));
	}
}

void	PixelMapStringPickListItem::Draw (const Region& update)
{
	if (fPixelMap != Nil) {
		Point	offset = Point ((GetSize ().GetV () - fPixelMap->GetBounds ().GetHeight ())/2, 1);
		BitBlit (*fPixelMap, fPixelMap->GetBounds (), fPixelMap->GetBounds () + offset);
	}
	StringPickListItem::Draw (update);
}

PixelMap	PixelMapStringPickListItem::GetPixelMap () const
{
	EnsureNotNil (fPixelMap);
	return (*fPixelMap);
}

void	PixelMapStringPickListItem::SetPixelMap (const PixelMap* pixelMap, Panel::UpdateMode updateMode)
{
	if (fPixelMap != Nil) {
		delete fPixelMap;
	}
	if (pixelMap == Nil) {
		fPixelMap = Nil;
		SetLeftMargin (1, updateMode);
	}
	else {
		fPixelMap = new PixelMap (*pixelMap);
		SetLeftMargin (2 + fPixelMap->GetBounds ().GetWidth (), updateMode);
	}
}




/*
 ********************************************************************************
 ***************************** HeirarchicalPickListItem *************************
 ********************************************************************************
 */
HeirarchicalPickListItem::HeirarchicalPickListItem (const String& text) :
	StringPickListItem (text),
	fExpanded (False),
	fParentItem (Nil),
	fSubItemCount (0)
{
}

Boolean	HeirarchicalPickListItem::TrackPress (const MousePressInfo& mouseInfo)
{
	if (mouseInfo.IsOpenSelection () /* WAS: Application::Get ().GetClickCount () == 2 */) {
		SetExpanded (not GetExpanded ());
		return  (True);
	}
	return (StringPickListItem::TrackPress (mouseInfo));	
}

HeirarchicalPickListItem*	HeirarchicalPickListItem::AddSubItem (HeirarchicalPickListItem* item, CollectionSize index)
{
	if (index == kBadSequenceIndex) {
		index = GetPickList ().GetItemIndex (this) + fSubItemCount + 1;
	}
	
	RequireNotNil (item);

	item->fParentItem = this;
	item->SetLeftMargin (GetLeftMargin () + 8);
	item->SetVisible (GetExpanded ());

	GetPickList ().AbstractPickList::AddItem (item, index);

	if (++fSubItemCount == 1) {
		Font	f = GetEffectiveFont ();	
		f.SetStyle (Set_BitString(FontStyle) (Font::eBoldFontStyle));
		SetFont (&f);
	}

	return (item);
}

HeirarchicalPickListItem*	HeirarchicalPickListItem::AddSubItem (const String& s, CollectionSize index)
{
	if (index == kBadSequenceIndex) {
		index = fSubItemCount + 1;
	}
	return (AddSubItem (new HeirarchicalPickListItem (s), GetPickList ().GetItemIndex (this) + index));
}

HeirarchicalPickListItem*	HeirarchicalPickListItem::AddSubItem (const String& s, HeirarchicalPickListItem* neighbor, AddMode append)
{
	RequireNotNil (neighbor);
	Require (neighbor->fParentItem == this);

	CollectionSize index = GetPickList ().GetItemIndex (neighbor);
	if (append == eAppend) {
		index++;
	}

	return (AddSubItem (new HeirarchicalPickListItem (s), index));
}

void	HeirarchicalPickListItem::RemoveSubItem (const String& s)
{
	StringPickListItem*	item = GetPickList ().StringToItem (s, GetPickList ().GetItemIndex (this)+1);
	RequireNotNil (item);

	GetPickList ().RemoveItem (item);
	if (--fSubItemCount == 0) {
		Font	f = GetEffectiveFont ();	
		f.SetStyle (kPlainFontStyle);
		SetFont (&f);
	}
}

UInt16	HeirarchicalPickListItem::GetSubItemCount () const
{
	return (fSubItemCount);
}

HeirarchicalPickListItem*	HeirarchicalPickListItem::GetParentItem () const
{
	return (fParentItem);
}
		
Boolean	HeirarchicalPickListItem::GetExpanded () const
{
	return (fExpanded);
}

void	HeirarchicalPickListItem::SetExpanded (Boolean expanded, Panel::UpdateMode update)
{
	if (fExpanded != expanded) {
		fExpanded = expanded;
#if 1		
		if (fSubItemCount > 0) {
			CollectionSize startIndex = GetPickList ().GetItemIndex (this) + 1;
			CollectionSize endIndex = startIndex + fSubItemCount - 1;
			CollectionSize index = 0;
			ForEach (PLstItmPtr, it, GetPickList ().MakeItemIterator ()) {
				if (++index > endIndex) {
					break;
				}
				if (index >= startIndex) {
					it.Current ()->SetVisible (fExpanded, update);
				}
			}
			GetPickList ().GetScrolledView ().InvalidateLayout ();
		}
#else
		if (fSubItemCount > 0) {
			ForEach (PLstItmPtr, it, MakeSubItemIterator ()) {
				AssertNotNil (it.Current ());
				it.Current ()->SetVisible (fExpanded, update);
			}
			GetPickList ().GetScrolledView ().InvalidateLayout ();
		}
#endif
	}
}

#if 0
SequenceIterator(PLstItmPtr)*	HeirarchicalPickListItem::MakeSubItemIterator ()
{
	return (new SubItemIterator (GetPickList ().GetPickListItems (), *this));
}

SubItemIterator::SubItemIterator (const Sequence(PLstItmPtr)& iterateOver, HeirarchicalPickListItem& item) :
	SequenceIterator(PLstItmPtr) (iterateOver),
	fItem (item),
	fIndex (1),
	fStartIndex (fItem.GetPickList ().GetItemIndex (&fItem) + 1)
{
	Require (fStartIndex != kBadSequenceIndex);
	for (; NotDone (); Next ()) {
		if (fIndex >= fStartIndex) {
			break;
		}
	}
}

void	SubItemIterator::Next ()
{
	fIndex++;
	SequenceIterator(PLstItmPtr)::Next ();
}

Boolean	SubItemIterator::Done ()
{
	if (fIndex > (fStartIndex + fItem.GetSubItemCount () - 1)) {
		return (True);
	}
	return (SequenceIterator(PLstItmPtr)::Done ());
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
