/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/GroupItem.cc,v 1.8 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: GroupItem.cc,v $
 *		Revision 1.8  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.44  1992/06/09  16:34:58  sterling
 *		tweaked SelectAllItems\
 *
 *		Revision 1.39  92/04/14  19:33:26  19:33:26  lewis (Lewis Pringle)
 *		Commented on expensive (and non-obvious) Add/Remove of item in GroupItem::TrackPress().
 *		
 *		Revision 1.27  1992/02/19  17:31:25  sterling
 *		fixed Trackpress bug
 *
 *		Revision 1.21  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *		Revision 1.20  1992/01/29  05:44:39  sterling
 *		fixed code generation for adornment
 *
 *		Revision 1.17  1992/01/24  23:45:44  lewis
 *		Use strtol instead of NumberText::StringToNumber.
 *
 *
 *
 */





#include	"Format.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"Alert.hh"
#include	"Dialog.hh"
#include	"MenuOwner.hh"

#include	"CommandNumbers.hh"
#include	"EmilyApplication.hh"
#include	"Scroller.hh"
#include	"CheckBox.hh"
#include	"PickList.hh"

#include	"GroupItem.hh"
#include	"ItemPallet.hh"
#include	"ItemScrap.hh"
#include	"EmilyWindow.hh"



#include	"ViewItemInfo.hh"
#include	"FontSizeQuery.hh"
#include	"SpacingQuery.hh"
#include	"GroupInfo.hh"
#include	"ReorderFocusItems.hh"



#if		!qRealTemplatesAvailable
	// copied from Stroika-Foundation-Globals.hh - see comment there for explanation...
	#include	"Sequence.hh"
	#include	"Set.hh"
	
	#ifndef		_ContainersOfStringDeclared_
		#define		_ContainersOfStringDeclared_
		Declare (Iterator, String);
		Declare (Collection, String);
		Declare (AbSequence, String);
		Declare (AbSet, String);
		Declare (Array, String);
		Declare (Sequence_Array, String);
		Declare (Sequence, String);
		Declare (Set_Array, String);
		Declare (Set, String);
	#endif		/*_ContainersOfStringDeclared_*/
#endif		/*!qRealTemplatesAvailable*/





#if		!qRealTemplatesAvailable
	Implement (Iterator, GParmPtr);
	Implement (Collection, GParmPtr);
	Implement (AbSequence, GParmPtr);
	Implement (Array, GParmPtr);
	Implement (Sequence_Array, GParmPtr);
	Implement (Sequence, GParmPtr);
#endif

GroupParam::GroupParam (CommandNumber language, CommandNumber gui) :
	Saveable (1),
	fLanguage (language),
	fGUI (gui),
	fScrollSize (kZeroPoint)
{
}

void	GroupParam::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	from >> fLanguage >> fGUI;
	fLanguage += eFirstLanguage;
	fGUI += eFirstGUI;
	from >> fScrollSize;
}

void	GroupParam::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);
	to << newline;
	to << tab (tabCount) << (fLanguage - eFirstLanguage) << ' ' << (fGUI - eFirstGUI) << newline;
	to << tab (tabCount);
	to << fScrollSize << newline;
}

/*
 ********************************************************************************
 ******************************** MagicGroupView ********************************
 ********************************************************************************
 */
MagicGroupView::MagicGroupView ()
{
}
		
void	MagicGroupView::AddView (View* subView, CollectionSize index, Panel::UpdateMode updateMode)	
{
	View::AddSubView (subView, index, updateMode);	
}

void	MagicGroupView::AddView (View* subView, View* neighborView, AddMode addMode, Panel::UpdateMode updateMode)
{
	View::AddSubView (subView, neighborView, addMode, updateMode);
}

void	MagicGroupView::RemoveView (View* subView, Panel::UpdateMode updateMode)
{
	View::RemoveSubView (subView, updateMode);
}

void	MagicGroupView::ReorderView (View* v, CollectionSize index, Panel::UpdateMode updateMode)
{
	View::ReorderSubView (v, index, updateMode);
}

void	MagicGroupView::ReorderView (View* subView, View* neighborView, AddMode addMode, Panel::UpdateMode updateMode)
{
	View::ReorderSubView (subView, neighborView, addMode, updateMode);
}

void	MagicGroupView::Draw (const Region& /*update*/)
{
	CommandNumber gui = EmilyWindow::GetGUI ();
	Point	border = GetBorder ();
	Rect	box = GetLocalExtent ();
	if (gui == eMacUI) {
		DrawMacBorder (box, border);
	}
	else if (gui == eMotifUI) {
		Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
		DrawMotifBorder (box, border, c, GetPlane ());
	}
	else if (gui == eWindowsGUI) {
		Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
		DrawMotifBorder (box, border, c, GetPlane ());
	}
}

/*
 ********************************************************************************
 ********************************* GroupItemType ********************************
 ********************************************************************************
 */
GroupItemType::GroupItemType () :
#if		qHP_BadTypedefForStaticProcs
	ItemType (eBuildGroup, "GroupView", (ItemBuilderProc)&GroupItemBuilder)
#else
	ItemType (eBuildGroup, "GroupView", &GroupItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
	
	Ensure (this == sThis);
}

GroupItemType::~GroupItemType ()
{
}

GroupItemType&	GroupItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}
				
ViewItem*	GroupItemType::GroupItemBuilder ()
{
	return (new GroupItem (Get (), False));
}

GroupItemType*	GroupItemType::sThis = Nil;



/*
 ********************************************************************************
 ********************************* GroupItemView ********************************
 ********************************************************************************
 */

GroupItemView::GroupItemView (AbstractScrollBar* verticalSlider, AbstractScrollBar* horizontalSlider, MagicGroupView* mainView):
	Scroller (Nil, Nil, Nil, verticalSlider, horizontalSlider),
	fMainView (Nil)
{
	fMainView = mainView;	
	SetScrolledView (fMainView);
}

void	GroupItemView::Layout ()
{
	if ((fMainView->GetSize () == kZeroPoint) or (not HasScrollBars ())) {
		fMainView->SetSize (GetSize ());
	}
	Scroller::Layout ();
}
		
MagicGroupView&	GroupItemView::GetMainView () const
{
	RequireNotNil (fMainView);
	Assert (fMainView == GetScrolledView ());
	return (*fMainView);
}

Boolean		GroupItemView::HasScrollBars () const
{
	return (Boolean ((GetVerticalScrollBar () != Nil) or (GetHorizontalScrollBar () != Nil)));
}

Point	GroupItemView::CalcDefaultSize_ (const Point& defaultSize) const
{
	if (defaultSize == kZeroPoint) {
		return (Point (100, 100));
	}
	return (GetMainView ().CalcDefaultSize (defaultSize));
}


/*
 ********************************************************************************
 ********************************* Commands *************************************
 ********************************************************************************
 */


class	AddItemCommand : public Command {
	public:		
		AddItemCommand (GroupItem& group, ViewItem& item);		
		override	void	DoIt ();
		override	void	UnDoIt ();

	private:
		GroupItem&	fGroup;
		ViewItem&	fItem;
};

class	PasteItemsCommand : public Command {
	public:
		PasteItemsCommand (GroupItem& group);		
		~PasteItemsCommand ();
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
		
	private:
		GroupItem&								fGroup;
		Sequence(ViewItemPtr)	fPasteItems;
};

class	ClearSelectedItemsCommand : public Command {
	public:		
		ClearSelectedItemsCommand (GroupItem& group);
		~ClearSelectedItemsCommand ();

		override	void	DoIt ();
		override	void	UnDoIt ();

	private:
		GroupItem&	fGroup;
		Sequence(ViewItemPtr)	fItems;
};

class	GroupItemsCommand : public Command {
	public:		
		GroupItemsCommand (GroupItem& parent);		
		override	void	DoIt ();
		override	void	UnDoIt ();
		
		static	GroupItem*	GroupItems (GroupItem& parent);

	private:
		GroupItem&	fParent;
		GroupItem*	fNewGroup;
};

class	UngroupItemsCommand : public Command {
	public:		
		UngroupItemsCommand (GroupItem* group);		
		override	void	DoIt ();
		override	void	UnDoIt ();
		
		static	void	UngroupItems (GroupItem* group);
	private:
		GroupItem&	fParent;
		GroupItem*	fGroup;
};

class	CompoundCommand : public Command {
	public:
		CompoundCommand (CommandNumber commandNumber);
		~CompoundCommand ();

		override	void	DoIt ();
		override	void	UnDoIt ();
	
	protected:
		CompositeCommand*	fCommands;
};

class	AlignItemsCommand : public CompoundCommand {
	public:		
		AlignItemsCommand (GroupItem& parent, CommandNumber commandNumber);		
		
	private:
		GroupItem&	fParent;
};

class	SpaceItemsCommand : public CompoundCommand {
	public:		
		SpaceItemsCommand (GroupItem& parent, CommandNumber commandNumber, Coordinate gap);		
		
	private:
		nonvirtual	void	BuildSelectedSequence (Point::Direction d);
		
		Sequence(ViewItemPtr)	fSelectedItems;
		GroupItem&	fParent;
};

class	ConstrainItemsCommand : public CompoundCommand {
	public:		
		ConstrainItemsCommand (GroupItem& parent, CommandNumber commandNumber);		
		
	private:
		GroupItem&	fParent;
};

class	SetItemFontCommand : public Command {
	public:
		SetItemFontCommand (ViewItem& item);
		~SetItemFontCommand ();
		
		override	void	DoIt ();
		override	void	UnDoIt ();
	
		Font	fNewFont;
	
	private:
		Font*		fOldFont;
		ViewItem&	fItem;
};

class	FontSizesCommand : public CompoundCommand {
	public:		
		FontSizesCommand (GroupItem& parent, FontSize fontSize);		
		
	private:
		GroupItem&	fParent;
};

class	SetFontsCommand : public CompoundCommand {
	public:		
		SetFontsCommand (GroupItem& parent, const String& fontName);		
		
	private:
		GroupItem&	fParent;
};

class	SetFontStylesCommand : public CompoundCommand {
	public:		
		SetFontStylesCommand (GroupItem& parent, CommandNumber commandNumber);		
		
	private:
		GroupItem&	fParent;
};

class	InheritFontsCommand : public CompoundCommand {
	public:		
		InheritFontsCommand (GroupItem& parent);		
		
	private:
		GroupItem&	fParent;
};

/*
 ********************************************************************************
 ********************************* GroupItem ************************************
 ********************************************************************************
 */
// to allow our subclasses to use versioning, we do it by hand, instead
// of relying on Saveable's implementation

static	const	SaveableVersion	kGroupVersion = 5;

GroupItem::GroupItem (ItemType& type, Boolean useScrollBars) :
	ViewItem (type),
	fRadioBank (),
	fRadioButtonCount (0),
	fGroupItemView (Nil),
	fOldSize (kZeroPoint),
	fWriteSelectedOnly (False)
{
	SetGroupView (useScrollBars, new MagicGroupView ());
	SetItemBorder (Point (1, 1));
	SetItemMargin (Point (1, 1));
	SetMaxVersion (2);
	AddFocus (&fFocusOwner);
}

GroupItem::~GroupItem ()
{
	ForEach (ViewItemPtr, it, fItems) {
		RemoveSubItem (it.Current ());
	}
	ForEach (ViewItemPtr, it1, fFocusItems) {
		fFocusOwner.RemoveFocus (it1.Current ());
	}
	RemoveFocus (&fFocusOwner);

	ForEach (GroupParamPtr, it2, fGroupParams) {
		GroupParam* param = it2.Current ();
		delete param;
	}
}

String	GroupItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "GroupView.hh";
	return (kHeaderFileName);
}

void	GroupItem::SetGroupView (Boolean useScrollBars, MagicGroupView* groupView)
{
	if (useScrollBars) {
		fGroupItemView = new GroupItemView (Scroller::kBuildDefaultSlider, Scroller::kBuildDefaultSlider, groupView);
	}
	else {
		fGroupItemView = new GroupItemView (Scroller::kBuildNoSlider, Scroller::kBuildNoSlider, groupView);
	}

GetGroupItemView ().SetBorder (GetGroupItemView ().GetMainView ().GetBorder (), eNoUpdate);
GetGroupItemView ().SetMargin (GetGroupItemView ().GetMainView ().GetMargin (), eNoUpdate);

	SetOwnedView (fGroupItemView);

GetGroupItemView ().SetBorder (kZeroPoint);
GetGroupItemView ().SetMargin (kZeroPoint);
}

void	GroupItem::AddRadioButton (AbstractRadioButton* button)
{
	fRadioBank.AddRadioButton (button);
	fRadioButtonCount++;
}

void	GroupItem::RemoveRadioButton (AbstractRadioButton* button)
{
	fRadioBank.RemoveRadioButton (button);
	fRadioButtonCount--;
}

String	GroupItem::GetRadioBankName ()
{
	if (GetGroup () == Nil) {
		return ("fRadioBank");
	}
	return (GetFieldName () + "RadioBank");
}

FocusOwner&	GroupItem::GetTabLoop ()
{
	return (fFocusOwner);
}

String	GroupItem::GetTabLoopName () const
{
	if (GetGroup () == Nil) {
		return (kEmptyString);
	}
	return (GetFieldName () + "TabLoop");
}

Sequence(ViewItemPtr)&	GroupItem::GetFocusItems () 
{
	return (fFocusItems);
}

String	GroupItem::GetFocusItemFieldName () const
{
	return (GetTabLoopName ());
}

Boolean	GroupItem::DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
				     KeyComposeState& composeState)
{
    if (HandleKeyEvent (code, isUp, keyBoardState, composeState)) {
	return (True);
    }
    return (GetTabLoop ().DispatchKeyEvent (code, isUp, keyBoardState, composeState));
}

Boolean	GroupItem::HandleKeyStroke (const KeyStroke& keyStroke)
{
	if (ItemPallet::GetEditMode () and (not EmilyWindow::GetCustomizeOnly ()) and
	    ((keyStroke == KeyStroke::kClear) or
	     (keyStroke == KeyStroke::kDelete) or
	     (keyStroke == KeyStroke::kBackspace))) {
		return (DoCommand (CommandSelection (eClear)));
	}
	return (False);
}

void	GroupItem::SetSelected (Boolean selected, Panel::UpdateMode updateMode)
{
	ViewItem::SetSelected (selected, updateMode);
	if (not selected) {
		SelectAllItems (False, updateMode);
	}
}

void	GroupItem::SelectAllItems (Boolean select, Panel::UpdateMode update)
{
	Panel::UpdateMode tempUpdate = update;
	if (update == eImmediateUpdate) {
		tempUpdate = eDelayedUpdate;
	}
	ForEach (ViewItemPtr, it, fItems) {
		it.Current ()->SetSelected (select, tempUpdate);
	}
	if (update == eImmediateUpdate) {
		Update ();
	}
}

Region	GroupItem::CalcSelectedRegion () const
{
	Region	region = kEmptyRegion;
	ForEach (ViewItemPtr, it, fItems) {
		ViewItem&	current = *it.Current ();
		if (current.GetSelected ()) {
			AssertNotNil (current.GetParentView ());
			region = region + current.GetParentView ()->LocalToTablet (current.GetDrawnRegion ());
		}
	}
	return (region);
}

void	GroupItem::DragSelected (const MousePressInfo& mouseInfo)
{
	Region	region = CalcSelectedRegion ();
	MagicGroupView&	mainView = GetGroupItemView ().GetMainView ();
	Rect	bounds = mainView.TabletToLocal (region.GetBounds ());
	ShapeDragger shapeDragger = ShapeDragger (RegionShape (region), bounds, mainView);
	shapeDragger.SetHysteresis (Point (1, 1));	
	if (EmilyApplication::Get ().GetConstrainTrackers ()) {
		shapeDragger.SetGrid (EmilyApplication::Get ().GetGrid ());
	}

	shapeDragger.SetScroller (fGroupItemView);
	(void) shapeDragger.TrackPress (MousePressInfo (mouseInfo, mainView.TabletToLocal (LocalToTablet (mouseInfo.fPressAt))));
	Point	delta = shapeDragger.GetNewShapeExtent ().GetOrigin () - shapeDragger.GetOldShapeExtent ().GetOrigin ();
	
	if (delta != kZeroPoint) {
		ForEach (ViewItemPtr, it, fItems) {
			AssertNotNil (it.Current ());
			if (it.Current ()->GetSelected ()) {
				PostCommand (new SizeItemCommand (*it.Current (), "Move", 
							 it.Current ()->GetOrigin () + delta, it.Current ()->GetSize ()));
				DirtyDocument ();
			}
		}
	}
}

void	GroupItem::EditModeChanged (Boolean newEditMode)
{
	GetTabLoop ().SetCurrentFocus ((FocusItem*)Nil, eDelayedUpdate, not FocusItem::kValidate);
	ViewItem::EditModeChanged (newEditMode);
	EnsureFramed ();
	ForEach (ViewItemPtr, it, fItems) {
		it.Current ()->EditModeChanged (newEditMode);
	}
}

void	GroupItem::GUIChanged (CommandNumber oldGUI, CommandNumber newGUI)
{
	if (1) {
		// hack to avoid unnecessary constraints
		GetGroupItemView ().GetMainView ().SetSize (kZeroPoint, eNoUpdate);
	}

	ViewItem::GUIChanged (oldGUI, newGUI);
	ForEach (ViewItemPtr, it, fItems) {
		it.Current ()->GUIChanged (oldGUI, newGUI);
	}
	
}

void	GroupItem::LanguageChanged (CommandNumber oldLanguage, CommandNumber newLanguage)
{
	if (1) {
		// hack to avoid unnecessary constraints
		GetGroupItemView ().GetMainView ().SetSize (kZeroPoint, eNoUpdate);
	}

	ViewItem::LanguageChanged (oldLanguage, newLanguage);
	ForEach (ViewItemPtr, it, fItems) {
		it.Current ()->LanguageChanged (oldLanguage, newLanguage);
	}
}

const	Point	kDefaultHystersis = Point (3, 3);
static	Boolean	PointOnRect (const Point& p, const Rect& r, const Point& hysteresis = kDefaultHystersis);
static	Boolean	PointOnRect (const Point& p, const Rect& r, const Point& hysteresis)
{
	if (r.Contains (p)) {
		return (Boolean (
			(Abs (p.GetV () - r.GetTop ()) <= hysteresis.GetV ()) or
			(Abs (p.GetV () - r.GetBottom ()) <= hysteresis.GetV ()) or
			(Abs (p.GetH () - r.GetLeft ()) <= hysteresis.GetH ()) or
			(Abs (p.GetH () - r.GetRight ()) <= hysteresis.GetH ())
		));
	}
	return (False);
}

Boolean	GroupItem::TrackPress (const MousePressInfo& mouseInfo)
{
	if (ItemPallet::GetPalletSelection () == eThumb) {
		return (View::TrackPress (mouseInfo));
	}
	else if (ItemPallet::GetPalletSelection () == eArrow) {
		if (GetGroup () == Nil) {
			// hack cuz in MainView we want our scrollbars to always be active
			if (View::TrackPress (mouseInfo)) {
				return (True);
			}
		}
		else {
			// we don't call View::TrackPress, and instead do equivalent work ourselves
			// because we only want to propagate clicks in ViewItems, not other owned views.
			// this can be a factor in subclasses of GroupItem that are already composites
			// but that allow people to add still more views to them
		
			ForEach (ViewItemPtr, it, fItems) {
				register View* v	=	it.Current ();
				AssertNotNil (v);
				AssertNotNil (v->GetParentView ());
				Point	mousePress = LocalToLocal (mouseInfo.fPressAt, v->GetParentView ());
				if (v->GetLive () and v->Contains (mousePress)) {
					return (v->TrackPress (MousePressInfo (mouseInfo, v->EnclosureToLocal (mousePress))));
				}
			}
		}

		if ((GetGroup () != Nil) and PointOnRect (mouseInfo.fPressAt, GetGroupItemView ().GetExtent ())) {
			(void) ViewItem::TrackPress (mouseInfo);
			return (True);
		}
#if		qMacUI
		else if ((GetGroup () != Nil) and mouseInfo.IsSimpleSelection () and CanChangeExtent () and (mouseInfo.fKeyBoard.GetKey (KeyBoard::eCommandKey))) {
#else
		else if ((GetGroup () != Nil) and mouseInfo.IsSimpleSelection () and CanChangeExtent () and (mouseInfo.fKeyBoard.GetKey (KeyBoard::eControlKey))) {
#endif
		
			GetMainGroup ().SelectAllItems (False);
			SetSelected (True);
			GetGroup ()->DragSelected (MousePressInfo (mouseInfo, GetGroup ()->TabletToLocal (LocalToTablet (mouseInfo.fPressAt))));
		}
		else {
			GetMainGroup ().SelectAllItems (False);

			MagicGroupView&	mainView = GetGroupItemView ().GetMainView ();
			Selector	selector = Selector (mainView, Rectangle (), mainView.GetLocalExtent (), True);
			selector.SetTimeOut (0);
			selector.SetScroller (fGroupItemView);
			(void) selector.TrackPress (MousePressInfo (mouseInfo, mainView.TabletToLocal (LocalToTablet (mouseInfo.fPressAt))));
			Rect selection = mainView.LocalToLocal (selector.GetSelection (), this);
			ForEach (ViewItemPtr, it, fItems) {
				AssertNotNil (it.Current ());
				Rect extent = it.Current ()->LocalToLocal (it.Current ()->GetLocalExtent (), this);
				if (Intersects (extent, selection)) {
					it.Current ()->SetSelected (True);
				}
			}
			GetMainGroup ().Update ();
			
			if (CountSelected () == 0) {
				SetPastePoint (LocalToTablet (mouseInfo.fPressAt));
			}
		}
		return (True);
	}
	else {
		ItemBuilderProc	proc = ItemType::LookupItemBuilder (ItemPallet::GetSelectedString ());
		if (proc != Nil) {
			GetMainGroup ().SelectAllItems (False);
			ViewItem*	newViewItem = (*proc) ();
			RequireNotNil (newViewItem);
			ShapeSizer sizer (
				newViewItem->GetShape (), 
				Rect (mouseInfo.fPressAt, Point (1, 1)), 
				*this, 
				kZeroPoint, 
				newViewItem->GetMaxSize (),
				ShapeSizer::eBotRight);

			sizer.SetTimeOut (0);
			(void)sizer.TrackPress (mouseInfo);
			Rect	extent = sizer.GetNewShapeExtent ();
			extent.SetOrigin (GetGroupItemView ().GetMainView ().TabletToLocal (LocalToTablet (extent.GetOrigin ())));
			if (EmilyApplication::Get ().GetConstrainTrackers ()) {
				extent.SetOrigin (RoundBy (extent.GetOrigin (), EmilyApplication::Get ().GetGrid ()));
			}
			newViewItem->SetItemOrigin (extent.GetOrigin ());
			newViewItem->SetItemSize (extent.GetSize ());

			/*
			 * If the user did not stretch out a reasonable size (perhaps just clicked), then
			 * make the object of default size.
			 */		
			if ((not (newViewItem->GetItemSize () >= newViewItem->GetMinSize ())) or 
				(not (newViewItem->GetItemSize () <= newViewItem->GetMaxSize ()))) {
							
				/*
				 * Note this is a rather expensive, but largely necessary hack to calculate the default size of the
				 * view. We must first add it as a subitem, so that it inherits all the correct font info, etc.
				 * then when were thru, remove it.
				 *
				 * This routine should probably take an update mode to avoid flicker. I believe it currently does flicker
				 * on X.
				 */
				AddSubItem (newViewItem, True, eNoUpdate);
				Point	newSize = newViewItem->CalcDefaultSize ();
				RemoveSubItem (newViewItem, eNoUpdate);
				
				if (newSize == kZeroPoint) {
					newSize = newViewItem->GetMinSize ();
				}
				newViewItem->SetItemSize (newSize);
			}
			newViewItem->SetSelected (True, eNoUpdate);
			newViewItem->ApplyCurrentParams ();

#if 	qMacUI
			if ((not mouseInfo.fKeyBoard.GetKey (KeyBoard::eOptionKey))) {
#else
			if (True) {
#endif
				ItemPallet::SetPalletSelection (eArrow);
			}
			Application::Get ().PostCommand (new AddItemCommand (*this, *newViewItem));
			DirtyDocument ();
		}
		return (True);
	}
	
	AssertNotReached ();	return (True);
}

void	GroupItem::ConstrainItems (const Point& sizeDelta)
{
	ForEach (ViewItemPtr, it, fItems) {
		AssertNotNil (it.Current ());
		ViewItem&	item = *it.Current ();
		
		Point	newOrigin = item.GetItemOrigin ();
		if (item.GetConstrainLeft ()) {
			newOrigin.SetH (newOrigin.GetH () + sizeDelta.GetH ());
		}
		if (item.GetConstrainTop ()) {
			newOrigin.SetV (newOrigin.GetV () + sizeDelta.GetV ());
		}
		
		Point	newSize = item.GetItemSize ();
		if (item.GetConstrainWidth ()) {
			newSize.SetH (newSize.GetH () + sizeDelta.GetH ());
		}
		if (item.GetConstrainHeight ()) {
			newSize.SetV (newSize.GetV () + sizeDelta.GetV ());
		}
		item.SetItemOrigin (newOrigin);
		item.SetItemSize (newSize);
		item.ApplyCurrentParams ();
	}
}

void	GroupItem::Layout ()
{
	EnsureFramed ();

	if (((fOldSize == kZeroPoint) and (GetScrollSize () == kZeroPoint)) or (not GetGroupItemView ().HasScrollBars ())) {
		if (GetScrollSize () != GetSize ()) {
			SetScrollSize (GetSize ());
if (GetGroup () == Nil) {
	DirtyDocument ();
}
		}
		ApplyCurrentParams ();
	}

	fOldSize = GetSize ();
	ViewItem::Layout ();
}

void	GroupItem::DoSetupMenus ()
{
	Boolean	fullEditing = not EmilyWindow::GetCustomizeOnly ();
		
	if (ItemPallet::GetEditMode ()) {
		if (GetSelected ()) {
			ViewItem::DoSetupMenus ();
			EnableCommand (eUngroupItems, Boolean (fullEditing and (fItems.GetLength () > 0)));
			EnableCommand (eReorderFocusItems, Boolean (fullEditing and (fFocusItems.GetLength () > 0)));
		}
		else {
			if (ClipBoard::Get ().Contains (ItemScrap::eView)) {
				EnableCommand (ePaste, fullEditing);
			}		
		
			CollectionSize	selected = 0;
			ForEach (ViewItemPtr, it, fItems) {
				AssertNotNil (it.Current ());
				if (it.Current ()->GetSelected ()) {
					it.Current ()->DoSetupMenus ();
					selected++;
				}
			}
						
			if (selected > 0) {
				if (selected < fItems.GetLength ()) {
					EnableCommand (eSelectAll);
				}
				
				EnableCommand (eClear, fullEditing);
				EnableCommand (eCopy);
				EnableCommand (eDuplicate, fullEditing);
				EnableCommand (eCut, fullEditing);

				EnableCommand (eGroupItems, fullEditing);
				EnableCommand (eFont);
				EnableCommand (eFontSize);
				EnableCommand (eUserSpecifyFontSize);
				EnableCommand (eInheritFont);

				EnableCommand (ePlainFontCommand);
				EnableCommand (eBoldFontCommand);
				EnableCommand (eItalicFontCommand);
				EnableCommand (eUnderlineFontCommand);
				EnableCommand (eOutlineFontCommand);
				EnableCommand (eShadowFontCommand);
				
				EnableCommand (eConstrainLeft, fullEditing);
				EnableCommand (eConstrainTop, fullEditing);
				EnableCommand (eConstraintWidth, fullEditing);
				EnableCommand (eConstrainHeight, fullEditing);

				if (selected > 1) {
					EnableCommand (eAlignLeft);
					EnableCommand (eAlignCenterH);
					EnableCommand (eAlignRight);
					EnableCommand (eAlignTop);
					EnableCommand (eAlignCenterV);
					EnableCommand (eAlignBottom);
					
					EnableCommand (eSpaceVertically);
					EnableCommand (eSpaceHorizontally);
					EnableCommand (eUserSpecifyVerticalSpacing);
					EnableCommand (eUniformSpacesVertically);
					EnableCommand (eUserSpecifyHorizontalSpacing);
					EnableCommand (eUniformSpacesHorizontally);
				}
			}
			EnableCommand (eSelectAll, Boolean (fItems.GetLength () > 0));
		}
	}
	else {
		GetTabLoop ().DoSetupMenus ();
	}
}

Boolean		GroupItem::DoCommand (const CommandSelection& selection)
{
	if (ItemPallet::GetEditMode ()) {
		CommandNumber commandNumber = selection.GetCommandNumber ();
		switch (commandNumber) {
			case eSelectAll:
				SelectAllItems (True, eImmediateUpdate); 
				return (True);
		
			case ePaste:
				if (GetSelected ()) {
					return (False);
				}
				else {
					if (CountSelected () != 0) {
						Region region = CalcSelectedRegion ();
						SetPastePoint (region.GetBounds ().GetOrigin ());
						(void) DoCommand (CommandSelection (eClear));
					}
					PostCommand (new PasteItemsCommand (*this));
				}
				return (True);
			
			case eClear:
				Application::Get ().PostCommand (new ClearSelectedItemsCommand (*this));
				DirtyDocument ();
				return (True);
			
			case eCopy:
				ClipBoard::Get ().SetPrivateScrap (new ItemScrap (*this));
				MenuOwner::SetMenusOutOfDate ();	
				return (True);
				
			case eDuplicate:
				{
					DoCommand (eCopy);
					Region region = CalcSelectedRegion ();
					SetPastePoint (region.GetBounds ().GetOrigin () + Point (10, 10));
					SelectAllItems (False);
					DoCommand (ePaste);
					Application::Get ().GetCurrentCommand ()->SetName ("Duplicate");
				}
				return (True);
	
			case eCut:
				DoCommand (eCopy);
				DoCommand (eClear);
				Application::Get ().GetCurrentCommand ()->SetName ("Cut");
				return (True);
	
			case eGroupItems:
				Application::Get ().PostCommand (new GroupItemsCommand (*this));
				DirtyDocument ();
				return (True);
			
			case eUngroupItems:
				if (GetSelected ()) {
					Application::Get ().PostCommand (new UngroupItemsCommand (this));
					DirtyDocument ();
					return (True);
				}
				break;
			
			case eReorderFocusItems:
				if (GetSelected ()) {
					ReorderFocusItems ();
					return (True);
				}
				break;

			case eFontSize: 
				{
					FontSize fontSize = FontSize (strtol (selection.GetName ()));
					Application::Get ().PostCommand (new FontSizesCommand (*this, fontSize));
					DirtyDocument ();
				}
				return (True);
				
			case eUserSpecifyFontSize: 
				{
					FontSizeQuery query = FontSizeQuery ();
					Dialog d = Dialog (&query, &query, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
					d.SetDefaultButton (d.GetOKButton ());
					if (d.Pose ()) {
						Application::Get ().PostCommand (new FontSizesCommand (*this, query.GetFontSize ()));
						DirtyDocument ();
						return (True);
					}
					else {
						return (False);
					}
					
				}
				AssertNotReached ();
	
			case eFont:
				Application::Get ().PostCommand (new SetFontsCommand (*this, selection.GetName ()));
				DirtyDocument ();
				return (True);
			
			case eInheritFont:
				Application::Get ().PostCommand (new InheritFontsCommand (*this));
				DirtyDocument ();
				return (True);
			
			case ePlainFontCommand:
			case eBoldFontCommand:
			case eItalicFontCommand:
			case eUnderlineFontCommand:
			case eOutlineFontCommand:
			case eShadowFontCommand:
				Application::Get ().PostCommand (new SetFontStylesCommand (*this, commandNumber));
				DirtyDocument ();
				return (True);
	
			case eAlignLeft:
			case eAlignCenterH:
			case eAlignRight:
			case eAlignTop:
			case eAlignCenterV:
			case eAlignBottom:
				Application::Get ().PostCommand (new AlignItemsCommand (*this, commandNumber));
				DirtyDocument ();
				return (True);
			
			case eUserSpecifyHorizontalSpacing:
			case eUserSpecifyVerticalSpacing: 
				{
					SpacingQuery query = SpacingQuery ();
					Dialog d = Dialog (&query, &query, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
					d.SetDefaultButton (d.GetOKButton ());
					if (d.Pose ()) {
						Application::Get ().PostCommand (new SpaceItemsCommand (*this, 
							((commandNumber == eUserSpecifyVerticalSpacing) ? eSpaceVertically : eSpaceHorizontally), 
							query.GetSpacing ()));
						DirtyDocument ();
						return (True);
					}
					else {
						return (False);
					}
				}
				AssertNotReached ();

			case eSpaceVertically:
			case eSpaceHorizontally: 
				{
					Coordinate gap = (Coordinate) strtol (selection.GetName ());
					Application::Get ().PostCommand (new SpaceItemsCommand (*this, commandNumber, gap));
					DirtyDocument ();
				}
				return (True);
			
			case eUniformSpacesVertically:
			case eUniformSpacesHorizontally:
				{
					Application::Get ().PostCommand (new SpaceItemsCommand (*this, commandNumber, 0));
					DirtyDocument ();
				}
				return (True);
	
			case eConstrainLeft:
			case eConstrainTop:
			case eConstraintWidth:
			case eConstrainHeight:
				Application::Get ().PostCommand (new ConstrainItemsCommand (*this, commandNumber));
				DirtyDocument ();
				return (True);
	
			default:
				break;		
		}
		if (GetSelected ()) {
			return (ViewItem::DoCommand (selection));
		}
		else {
			Boolean	result = False;
			ForEach (ViewItemPtr, it, fItems.MakeSequenceIterator (eSequenceBackward)) {
				AssertNotNil (it.Current ());
				ViewItem&	item = *it.Current ();
				if (item.GetSelected ()) {
					result = Boolean (item.DoCommand (selection) or result);
				}
			}
			return (result);
		}
	}
	else {
		return (GetTabLoop ().DoCommand (selection));
	}
	
	AssertNotReached ();	return (False);
}

Boolean	GroupItem::IsButton ()
{
	ForEach (ViewItemPtr, it, fItems) {
		if (it.Current ()->IsButton ()) {
			return (True);
		}
	}
	
	return (False);
}

Boolean	GroupItem::IsSlider ()
{
	ForEach (ViewItemPtr, it, fItems) {
		if (it.Current ()->IsSlider ()) {
			return (True);
		}
	}
	
	return (False);
}

Boolean	GroupItem::IsText ()
{
	ForEach (ViewItemPtr, it, fItems) {
		if (it.Current ()->IsText ()) {
			return (True);
		}
	}
	
	return (False);
}

Boolean	GroupItem::IsFocusItem (CommandNumber gui)
{
	if (gui == eAnyGUI) {
		return (Boolean (fFocusItems.GetLength () > 0));
	}
	
	ForEach (ViewItemPtr, it, MakeFocusItemIterator ()) {
		if (it.Current ()->IsFocusItem (gui)) {
			return (True);
		}
	}
	return (False);
}

Boolean	GroupItem::ItemCanBeEnabled ()
{
	return (False);
}


GroupItem&	GroupItem::GetMainGroup ()
{
	if (GetGroup () != Nil) {
		return (GetGroup ()->GetMainGroup ());
	}
	return (*this);
}


GroupItemView&	GroupItem::GetGroupItemView () const
{
	RequireNotNil (fGroupItemView);
	return (*fGroupItemView);
}

void	GroupItem::AddSubItem (ViewItem* item, Boolean prepend, Panel::UpdateMode update)
{
	RequireNotNil (item);
	Require (not fItems.Contains (item));

	if (item->IsFocusItem (eAnyGUI)) {
		fFocusItems.Append (item);
		GetTabLoop ().AddFocus (item);
		
		if (GetFocusOwner () == Nil) {
			// we just became a focusitem ourselves, need to be added to parents list
			if (GetGroup () != Nil) {
				GetGroup ()->GetTabLoop ().AddFocus (this);
				GetGroup ()->GetFocusItems ().Append (this);
			}
		}
	}

	GetGroupItemView ().GetMainView ().AddView (item);
	if (prepend) {
		fItems.Prepend (item);
	}
	else {
		fItems.Append (item);
	}

	CommandNumber currentGUI = EmilyWindow::GetGUI ();
	CommandNumber currentLanguage = EmilyWindow::GetLanguage ();
	ForEach (ItemParamsPtr, it, MakeParamsIterator ()) {
		ItemParams*	current = it.Current ();
		
		if (it.Current ()->fGUI != currentGUI) {
			item->GUIChanged (currentGUI, it.Current ()->fGUI);
			currentGUI = it.Current ()->fGUI;
		}
		if (it.Current ()->fLanguage != currentLanguage) {
			item->LanguageChanged (currentLanguage, it.Current ()->fLanguage);
			currentLanguage = it.Current ()->fLanguage;
		}
	}
	item->SetGroup (this);
	if (currentGUI != EmilyWindow::GetGUI ()) {
		item->GUIChanged (currentGUI, EmilyWindow::GetGUI ());
	}
	if (currentLanguage != EmilyWindow::GetLanguage ()) {
		item->LanguageChanged (currentLanguage, EmilyWindow::GetLanguage ());
	}
	item->ApplyCurrentParams ();
	GetGroupItemView ().GetMainView ().Refresh (item->GetDrawnRegion ());
}

void	GroupItem::RemoveSubItem (ViewItem* item, Panel::UpdateMode update)
{
	RequireNotNil (item);
	Require (fItems.Contains (item));

	if (item->IsFocusItem (eAnyGUI)) {
		fFocusItems.Remove (item);
		GetTabLoop ().RemoveFocus (item);

		if (fFocusItems.GetLength () == 0) {
			// we just stopped being a focusitem ourselves, need to be removed from parents list
			if (GetGroup () != Nil) {
				GetGroup ()->GetTabLoop ().RemoveFocus (this);
				GetGroup ()->GetFocusItems ().Remove (this);
			}
		}
	}
	
	GetGroupItemView ().GetMainView ().Refresh (item->GetDrawnRegion ());
	GetGroupItemView ().GetMainView ().RemoveView (item, update);
	fItems.Remove (item);
	item->SetGroup (Nil);
}

void	GroupItem::ReorderSubItem (ViewItem* item, CollectionSize index)
{
	CollectionSize	oldIndex = fItems.IndexOf (item);
	Assert (oldIndex != kBadSequenceIndex);
	fItems.Remove (item);
	fItems.InsertAt (item, ((oldIndex < index) ? index-1 : index));
	GetGroupItemView ().GetMainView ().ReorderView (item, index);

// ensure fails in classes like grid that control their own subviews
//	Ensure (GetGroupItemView ().GetMainView ().GetSubViewIndex (item) == fItems.IndexOf (item));
}

GroupItem*	GroupItem::GetMostSelectedGroup ()
{
	GroupItem*	mostSelected = Nil;
	ForEach (ViewItemPtr, it, fItems) {
		mostSelected = it.Current ()->GetMostSelectedGroup ();
		if (mostSelected != Nil) {
			return (mostSelected);
		}
	}
	return (ViewItem::GetMostSelectedGroup ());
}

GroupItem*	GroupItem::GetGroupContaining (const Point& point)
{
	Point	localPoint = TabletToLocal (point);
	
	if (GetLocalExtent ().Contains (localPoint)) {
		GroupItem*	groupContaining = Nil;
		ForEach (ViewItemPtr, it, fItems) {
			groupContaining = it.Current ()->GetGroupContaining (point);
			if (groupContaining != Nil) {
				return (groupContaining);
			}
		}
		return (this);
	}

	return (Nil);
}

void	GroupItem::SetPastePoint (const Point& point)
{
	GetMainGroup ().SetPastePoint (point);
}

Point	GroupItem::GetPastePoint () const 
{
	GroupItem*	This = (GroupItem*) this;	// work around const-ness
	return (This->GetMainGroup ().GetPastePoint ());
}

Boolean	GroupItem::SubItemsCanChangeExtent () const
{
	return (True);
}

void	GroupItem::SetItemBorder_ (Point newBorder)
{
	GetGroupItemView ().GetMainView ().SetBorder (newBorder);
}

void	GroupItem::SetItemMargin_ (Point newMargin)
{
	GetGroupItemView ().GetMainView ().SetMargin (newMargin);
}

void	GroupItem::EnsureFramed ()
{
	if (ItemPallet::GetEditMode ()) {
		if (GetItemBorder () == kZeroPoint) {
			GetGroupItemView ().GetMainView ().SetBorder (Point (1, 1));
		}
	}
	else {
		GetGroupItemView ().GetMainView ().SetBorder (GetItemBorder ());
	}
}

SequenceIterator(ViewItemPtr)*	GroupItem::MakeItemIterator (SequenceDirection d) const
{
	return (fItems.MakeSequenceIterator (d));
}

SequenceIterator(ViewItemPtr)*	GroupItem::MakeFocusItemIterator (SequenceDirection d) const
{
	return (fFocusItems.MakeSequenceIterator (d));
}

Point	GroupItem::GetScrollSize () const
{
	return (GetCurrentGroupParam ().fScrollSize);
}

Point	GroupItem::GetScrollSize (CommandNumber language, CommandNumber gui) const
{
	GroupParam*	param = FindGroupParam (language, gui);
	
	if (param == Nil) {
		ViewItem*	This = (ViewItem*)this;
		This->AddParam (language, gui, GetBaseLanguage (), GetBaseGUI ());
		param = FindGroupParam (language, gui);
	}
	EnsureNotNil (param);
	return (param->fScrollSize);
}

void	GroupItem::SetScrollSize (const Point& scrollSize)
{
	GetCurrentGroupParam ().fScrollSize = scrollSize;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (GroupParamPtr, it, fGroupParams) {
			GroupParam*	current = it.Current ();
			current->fScrollSize = scrollSize;
		}
	}
}


CollectionSize	GroupItem::CountSelected () const
{
	CollectionSize total = 0;
	ForEach (ViewItemPtr, it, fItems) {
		if (it.Current ()->GetSelected ()) {
			total++;
		}
	}
	return (total);
}

class	SetGroupInfoCommand : public Command {
	public:
		SetGroupInfoCommand (GroupItem& item, GroupInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		GroupItem&	fItem;
		Command*	fItemInfoCommand;
};

SetGroupInfoCommand::SetGroupInfoCommand (GroupItem& item, GroupInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil)
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}			

void	SetGroupInfoCommand::DoIt ()
{
	fItemInfoCommand->DoIt ();
	Command::DoIt ();
}

void	SetGroupInfoCommand::UnDoIt ()
{
	fItemInfoCommand->UnDoIt ();
	Command::UnDoIt ();
}

void	GroupItem::SetItemInfo ()
{
	GroupInfo info = GroupInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetGroupInfoCommand (*this, info));
		DirtyDocument ();
	}
}

void	GroupItem::WriteSelected (class ostream& to)
{
	Require (not fWriteSelectedOnly);	// we are not reentrant
	fWriteSelectedOnly = True;
	
	DoWrite (to, 0);	

	Require (fWriteSelectedOnly);	// no one else should play with this
	fWriteSelectedOnly = False;
}
		
void	GroupItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
	SaveableVersion	version;
	Try {
		version = Saveable::ReadVersion (kGroupVersion, from);
	}
	Catch1 (sBadVersion) {
		version = GetVersion ();
	}

	if (version <= 4) {
		Point	scrollSize;
		from >> scrollSize;
		SetScrollSize (scrollSize);
	}
	
	if (version <= 2) {
		Boolean	framed;
		from >> framed;
		if (framed) {
			SetItemBorder (Point (1, 1));
			SetItemMargin (Point (1, 1));
			SetDefaultBorder (Point (0, 0));
			SetDefaultMargin (Point (0, 0));
		}
		String	foo;
		ReadString (from, foo);
		Font f (kSystemFont);
		from >> f;
	}
	CollectionSize items = kBadSequenceIndex;
	from >> items;
	Require (items != kBadSequenceIndex);
	
	for (CollectionSize count = items; count > 0; count--) {
		String	itemBaseClass;
		ReadString (from, itemBaseClass);
		ItemBuilderProc	proc = ItemType::LookupItemBuilder (itemBaseClass);
		if (proc != Nil) {
			String	itemClass;
			ReadString (from, itemClass);

			ViewItem*	newViewItem = (*proc) ();
			RequireNotNil (newViewItem);
			newViewItem->SetFieldClass (itemClass);

			char c;
			from >> c;
			Require (c == '{');
			newViewItem->DoRead (from);
			from >> c;
			Require (c == '}');

			AddSubItem (newViewItem, False);
		}
		else {
			char c;
			from >> c;
			
			if (c != '{') {
				Saveable::sBadVersion.Raise ();
			}
			
			Boolean	shouldAbort = False;	// use variable to ensure proper cleanup during throw
			{
				String s = "Ignoring unknown view called " + itemBaseClass + ".";
				Alert alert = Alert (s, "Abort", "Okay");
				alert.Pose ();
				shouldAbort = not alert.GetCancelled ();
			}
			if (shouldAbort) {
				static	FileException	sFileReadCancelled;
				sFileReadCancelled.Raise ();
			}
			
			int	bracketCount = 1;
			while (from and (bracketCount > 0)) {
				from >> c;
				if (c == '{') {
					bracketCount++;
				}
				else if (c == '}') {
					bracketCount--;
				}
			}
		}		
	}
	if (version >= 4) {
		fFocusItems.RemoveAll ();

		CollectionSize length;
		from >> length;
		for (int i = 1; i <= length; i++) {
			CollectionSize index;
			from >> index;
			fFocusItems.Append (fItems[index]);
		}
		Ensure (fFocusItems.GetLength () == length);
	}
	if (version >= 5) {
		ForEach (GroupParamPtr, it, fGroupParams) {
			GroupParam* param = it.Current ();
			delete param;
		}
		fGroupParams.RemoveAll ();

		CollectionSize paramCount;
		from >> paramCount;
		for (int count = 1; count <= paramCount; count++) {
			GroupParam*	newParams = new GroupParam (eEnglish, eMacUI);
			char c;
			from >> c;
			Require (c == '{');
			newParams->DoRead (from);
			from >> c;
			Require (c == '}');
			fGroupParams.Append (newParams);	
		}
	}
	ApplyCurrentParams ();
}

void	GroupItem::DoWrite_ (class ostream& to, int tabCount) const
{	
	ViewItem::DoWrite_ (to, tabCount);
	Saveable::WriteVersion (kGroupVersion, to, tabCount);
	
	to << newline;
	CollectionSize items = (fWriteSelectedOnly) ? CountSelected () : fItems.GetLength ();
	to << tab (tabCount) << items << newline;
	ForEach (ViewItemPtr, it, fItems) {
		if ((not fWriteSelectedOnly) or (it.Current ()->GetSelected ())) {
			WriteItem (*it.Current (), to, tabCount+1);
		}
	}
	
	if (fWriteSelectedOnly) {
		Sequence(ViewItemPtr)	itemsTemp;
		Sequence(ViewItemPtr)	focusTemp;
		ForEach (ViewItemPtr, it, fItems) {
			if (it.Current ()->GetSelected ()) {
				itemsTemp.Append (it.Current ());
			}
		}
		ForEach (ViewItemPtr, it1, MakeFocusItemIterator ()) {
			if (it1.Current ()->GetSelected ()) {
				focusTemp.Append (it1.Current ());
			}
		}
		{
			CollectionSize length = focusTemp.GetLength ();
			to << tab (tabCount) << length << newline;
			if (length > 0) {
				to << tab (tabCount+1);
				ForEach (ViewItemPtr, it, focusTemp) {
					CollectionSize index = itemsTemp.IndexOf (it.Current ());
					to << index << ' ';
				}
				to << newline;
			}
		}
	}
	else {
		CollectionSize length = fFocusItems.GetLength ();
		to << tab (tabCount) << length << newline;
		if (length > 0) {
			to << tab (tabCount+1);
			ForEach (ViewItemPtr, it, MakeFocusItemIterator ()) {
				CollectionSize index = fItems.IndexOf (it.Current ());
				to << index << ' ';
			}
			to << newline;
		}
	}

	CollectionSize	paramCount = fGroupParams.GetLength ();
	to << tab (tabCount) << paramCount << newline;
	ForEach (GroupParamPtr, it1, fGroupParams) {
		to << tab (tabCount) << '{' << newline;
		it1.Current ()->DoWrite (to, tabCount+1);
		to << tab (tabCount) << '}' << newline;
	}
}

void	GroupItem::WriteItem (ViewItem& item, class ostream& to, int tabCount)
{
	to << tab (tabCount);
	WriteString (to, item.GetBaseClass ());
	WriteString (to, item.GetFieldClass ());
	to << newline << tab (tabCount) << '{' << newline;
	item.DoWrite (to, tabCount+1);
	to << tab (tabCount) << '}' << newline;
}

void	GroupItem::WriteDeclaration (class ostream& to, int tabCount)
{
	if (GetGroup () != Nil) {
		ViewItem::WriteDeclaration (to, tabCount);
	}

	if (fRadioButtonCount > 0) {
		DeclareField (to, tabCount, "RadioBank", GetRadioBankName ());
	}
	
	CollectionSize	focusItems = 0;
	ForEach (FocusItemPtr, focusIt, GetTabLoop ().MakeFocusIterator ()) {
		focusItems++;
	}

	if ((focusItems > 0) and (GetGroup () != Nil)) {
		DeclareField (to, tabCount, "FocusOwner", GetTabLoopName ());
	}	

	ForEach (ViewItemPtr, it, fItems) {
		AssertNotNil (it.Current ());
		it.Current ()->WriteDeclaration (to, tabCount);
	}
}

void	GroupItem::WriteIncludes (class ostream& to, int tabCount)
{
	static	Sequence(String)	sIncludeFiles;
	
	if (GetGroup () == Nil) {
		sIncludeFiles.RemoveAll ();
	}
	
	ForEach (ViewItemPtr, it, fItems) {
		AssertNotNil (it.Current ());
		String	headerFile = it.Current ()->GetHeaderFileName ();
		if (headerFile != kEmptyString) {
			if (not sIncludeFiles.Contains (headerFile)) {
				sIncludeFiles.Append (headerFile);
				it.Current ()->WriteInclude (to, tabCount);
			}
		}
		it.Current ()->WriteIncludes (to, tabCount);
	}
}


void	GroupItem::WriteInitializer (class ostream& to, int tabCount, CommandNumber gui)
{
	int	tabPlus = 1;
	Boolean	isMainGroup = Boolean (GetGroup () == Nil);
	if (not isMainGroup) {
		ViewItem::WriteInitializer (to, tabCount, gui);
		tabPlus++;
	}
	if (fRadioButtonCount > 0) {
		to << tab (tabCount+tabPlus) << GetRadioBankName () << ".SetController (this);" << newline << newline;
	}

	ForEach (ViewItemPtr, it, fItems) {
		ViewItem*	current = it.Current ();
		AssertNotNil (current);
		
		current->WriteInitializer (to, tabCount + tabPlus - 1, gui);
		to << tab (tabCount+tabPlus);
		
		if (not isMainGroup) {
			to << GetFieldName () << kFieldAccessor;
		}
		to << "AddSubView (&" << current->GetSubviewFieldName () << ");" << newline << newline;
	}
    
	if (IsFocusItem (gui)) {
		ForEach (ViewItemPtr, it, MakeFocusItemIterator ()) {
			ViewItem&	current = *it.Current ();
			if (current.IsFocusItem (gui)) {
				to << tab (tabCount+tabPlus);
				if (not isMainGroup) {
					Assert (GetTabLoopName () != kEmptyString);
					to << GetTabLoopName () << kFieldAccessor;
				}

				to << "AddFocus (&" << current.GetFocusItemFieldName () << ");" << newline;
			}
		}
	}
	
	{
	Boolean first = True;
	ForEach (ItemParamsPtr, it, MakeParamsIterator ()) {
		ItemParams*	current = it.Current ();
		CommandNumber language = current->fLanguage;
		
		if ((language != GetBaseLanguage ()) and (current->fGUI == gui)) {
// can't seem to have more than one OStringStream open at a time (or something like that)
#if 0
			OStringStream	tempTo;

			WriteCustomizations (tempTo, tabCount+tabPlus, language, gui);
			
			String	customization = String (tempTo);
Assert (customization.GetLength () <= 2000);
if (customization.GetLength () > 2000) {
DebugMessage ("bad length = %d", customization.GetLength ());
}

			if (customization != kEmptyString) {
				if (first) {
					to << newline;
					first = False;
				}
				if (GetGroup () == Nil) {
					to << "#if q" << GetLanguageCompilationDirective (language) << newline;
				}
				to << customization;
				if (GetGroup () == Nil) {
					to << "#endif" << newline;
				}
			}
#else
		if (first) {
			to << newline;
			first = False;
		}
		if (GetGroup () == Nil) {
			to << "#if q" << GetLanguageCompilationDirective (language) << newline;
		}
		WriteCustomizations (to, tabCount+tabPlus, language, gui);
		if (GetGroup () == Nil) {
			to << "#endif" << newline;
		}
#endif
		}
	}
	}
}

void	GroupItem::WriteDestructor (class ostream& to, int tabCount, CommandNumber gui)
{
	if (IsFocusItem (gui)) {
		Boolean	removed = False;
		ForEach (ViewItemPtr, it, MakeFocusItemIterator ()) {
			ViewItem&	current = *it.Current ();
			if (current.IsFocusItem (gui)) {
				to << tab (tabCount+1);
				if (GetGroup () != Nil) {
					Assert (GetTabLoopName () != kEmptyString);
					to << GetTabLoopName () << kFieldAccessor;
				}

				to << "RemoveFocus (&" << current.GetFocusItemFieldName () << ");" << newline;
				removed = True;
			}
		}
		if (removed) {
			to << newline;
		}
	}
	ForEach (ViewItemPtr, it, fItems) {
		ViewItem*	current = it.Current ();
		AssertNotNil (current);
		to << tab (tabCount+1);
		
		if (GetGroup () != Nil) {
			to << GetFieldName () << kFieldAccessor;
		}
		to << "RemoveSubView (&" << current->GetSubviewFieldName () << ");" << newline;
		current->WriteDestructor (to, tabCount+1, gui);
	}
}


void	GroupItem::WriteBuilder (class ostream& to, int tabCount)
{
	if (GetGroup () != Nil) {
		ViewItem::WriteBuilder (to, tabCount);
		to << ViewItem::kEmptyInitializer << newline;
	}
	if (fRadioButtonCount > 0) {
		to << tab (tabCount) << GetRadioBankName () << " ()," << newline;
	}
#if 0
	if (GetGroup () != Nil) {
		if (IsFocusItem (gui) and (fFocusItems.GetLength () > 0)) {
			to << tab (tabCount) << GetTabLoopName () << " ()," << newline << newline;
		}
	}
#endif

	CollectionSize	length = fItems.GetLength ();
	if (length > 0) {		
		ForEach (ViewItemPtr, it, fItems) {
			AssertNotNil (it.Current ());
			it.Current ()->WriteBuilder (to, tabCount);
		}
	}
}

void	GroupItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
}

String	GroupItem::GetFieldSizeConstantName () const
{
	if (GetFieldName () == kEmptyString) {
		return ("kSizeDelta");
	}
	return (GetFieldName () + "SizeDelta");
}


void	GroupItem::WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
  	ViewItem::WriteCustomization (to, tabCount, language, gui);
}

void	GroupItem::WriteCustomizations (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ForEach (ViewItemPtr, it, fItems) {
		ViewItem*	current = it.Current ();
		AssertNotNil (current);
		
		current->WriteCustomizations (to, tabCount, language, gui);
	}
  	ViewItem::WriteCustomizations (to, tabCount, language, gui);
}

void	GroupItem::WriteLayout (class ostream& to, int tabCount)
{
	ViewItem::WriteLayout (to, tabCount);

	Boolean	mustConstrain = False;
	ForEach (ViewItemPtr, it, fItems) {
		ViewItem& current = *it.Current ();
		
		if (current.GetConstrainLeft ()  or current.GetConstrainTop () or
			current.GetConstrainWidth () or current.GetConstrainHeight ()) {
			
			mustConstrain = True;
			break;
		}
	}

	if (mustConstrain) {
		String prependage = "const Point " + GetFieldSizeConstantName () + " = ";
		if (GetGroup () == Nil) {
			to << tab (tabCount) << prependage << "CalcDefaultSize () - GetSize ();" <<  newline;
		}
		else {
			prependage += "Point (";
			
			Boolean first = True;
			for (CommandNumber gui = eFirstGUI; gui <= eLastGUI; gui++) {
				if (ParamsExist (eAnyLanguage, gui)) {
					String 	directive = GetGUICompilationDirective (gui);
					if (first) {
						to << "#if   q" << directive << newline;
						first = False;
					}
					else {
						to << "#elif   q" << directive << newline;
					}
					Point scrollSize = GetScrollSize (GetBaseLanguage (), gui);
					to  << tab (tabCount) << prependage << scrollSize.GetV () << "," << scrollSize.GetH () << ") - ";
					to << GetFieldName () << ".GetSize ();" << newline;
				}
			}
			Assert (not first);
			to << "#else" << newline;
			Point scrollSize = GetScrollSize (GetBaseLanguage (), GetBaseGUI ());
			to  << tab (tabCount) << prependage << scrollSize.GetV () << "," << scrollSize.GetH () << ") - ";
			to << GetFieldName () << ".GetSize ();" << newline;
			to << "#endif /* GUI */" << newline;
		}
	}
	
	ForEach (ViewItemPtr, it1, fItems) {
		it1.Current ()->WriteLayout (to, tabCount+1);
	}
}

GroupParam*	GroupItem::FindGroupParam (CommandNumber language, CommandNumber gui) const
{
	ForEach (GroupParamPtr, it, fGroupParams) {
		GroupParam*	current = it.Current ();
		
		if (((current->fLanguage == language) or (language == CommandHandler::eNoCommand)) and 
			((current->fGUI == gui) or (gui == CommandHandler::eNoCommand))) {
			
			return (current);
		}
	}
	return (Nil);
}

GroupParam&	GroupItem::GetCurrentGroupParam () const
{
	GroupParam*	param = FindGroupParam (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	
	if (param == Nil) {
		ViewItem*	This = (ViewItem*)this;
		This->AddParam (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI (), GetBaseLanguage (), GetBaseGUI ());
		param = FindGroupParam (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	}
	EnsureNotNil (param);
	return (*param);
}

void	GroupItem::AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI)
{
	ViewItem::AddParam (language, gui, oldLanguage, oldGUI);
	GroupParam*	param = new GroupParam (language, gui);
	GroupParam* oldParams = FindGroupParam (oldLanguage, oldGUI);
	if (oldParams != Nil) {
		param->fScrollSize = oldParams->fScrollSize;
	}
	
	fGroupParams.Append (param);
}

void	GroupItem::RemoveParam (CommandNumber language, CommandNumber gui)
{
	ViewItem::RemoveParam (language, gui);
	GroupParam* param = FindGroupParam (language, gui);
	if (param != Nil) {
		fGroupParams.Remove (param);
	}
	ForEach (ViewItemPtr, it, fItems) {
		it.Current ()->RemoveParam (language, gui);
	}
}

void	GroupItem::ApplyParams (CommandNumber language, CommandNumber gui)
{
	ViewItem::ApplyParams (language, gui);
	GroupParam* param = FindGroupParam (language, gui);
	if (param != Nil) {
		if (param->fScrollSize != GetGroupItemView ().GetMainView ().GetSize ()) {
			Point	oldSize = GetGroupItemView ().GetMainView ().GetSize ();
	
			GetGroupItemView ().GetMainView ().SetSize (param->fScrollSize);
			GetGroupItemView ().ScrolledViewChangedSize ();
			
			Point	newSize = GetGroupItemView ().GetMainView ().GetSize ();
			
			if ((oldSize != kZeroPoint) and (newSize != oldSize)) {
				ConstrainItems (newSize - oldSize);
			}
		}
	}
}

void	GroupItem::ReorderFocusItems ()
{
	ReorderFocusItem v = ReorderFocusItem (*this);
	
	Dialog d = Dialog (&v, &v, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		fFocusItems.RemoveAll ();
		ForEach (ViewItemPtr, it, v.MakeFocusItemIterator ()) {
			fFocusItems.Append (it.Current ());
		}

		DirtyDocument ();
	}
}

/*
 ********************************************************************************
 ******************************** CompoundCommand *******************************
 ********************************************************************************
*/
CompoundCommand::CompoundCommand (CommandNumber commandNumber) :
	Command (commandNumber, kUndoable),
	fCommands (Nil)
{
}

CompoundCommand::~CompoundCommand ()
{
	delete fCommands;
}

void	CompoundCommand::DoIt ()
{
	RequireNotNil (fCommands);
	fCommands->DoIt ();
	Command::DoIt ();
}

void	CompoundCommand::UnDoIt ()
{
	RequireNotNil (fCommands);
	fCommands->UnDoIt ();
	Command::UnDoIt ();
}

/*
 ********************************************************************************
 ***************************** SetItemFontCommand *******************************
 ********************************************************************************
*/
SetItemFontCommand::SetItemFontCommand (ViewItem& item) :
	Command (eFont, kUndoable),
	fItem (item),
	fOldFont (Nil),
	fNewFont (item.GetEffectiveFont ())
{
	if (item.GetFont () != Nil) {
		fOldFont = new Font (*item.GetFont ());
	}
}

SetItemFontCommand::~SetItemFontCommand ()
{
	delete fOldFont;
}

void	SetItemFontCommand::DoIt ()
{
	fItem.SetItemFont (&fNewFont);
	fItem.ApplyCurrentParams ();
	fItem.Refresh ();
	Command::DoIt ();
}

void	SetItemFontCommand::UnDoIt ()
{
	fItem.SetItemFont (fOldFont);
	fItem.ApplyCurrentParams ();
	fItem.Refresh ();
	Command::UnDoIt ();
}

/*
 ********************************************************************************
 ***************************** SetFontStylesCommand *****************************
 ********************************************************************************
*/
SetFontStylesCommand::SetFontStylesCommand (GroupItem& parent, CommandNumber commandNumber) :
	CompoundCommand (commandNumber),
	fParent (parent)
{
	int fontStyle = -1;
	switch (commandNumber) {
		case ePlainFontCommand:
			break;
			
		case eBoldFontCommand:
			fontStyle = Font::eBoldFontStyle;
			break;
			
		case eItalicFontCommand:
			fontStyle = Font::eItalicFontStyle;
			break;
			
		case eUnderlineFontCommand:
			fontStyle = Font::eUnderlineFontStyle;
			break;
			
		case eOutlineFontCommand:
			fontStyle = Font::eOutlineFontStyle;
			break;
			
		case eShadowFontCommand:
			fontStyle = Font::eShadowFontStyle;
			break;
		
		default:
			RequireNotReached ();
	}
	
	ForEach (ViewItemPtr, it, fParent.MakeItemIterator ()) {
		AssertNotNil (it.Current ());
		ViewItem& current = *it.Current ();
		
		if (current.GetSelected ()) {
			SetItemFontCommand* command = new SetItemFontCommand (current);

			if (fontStyle == -1) {
				command->fNewFont.SetStyle (kPlainFontStyle);
			}
			else {
				FontStyle	correctTypeFontStyle = FontStyle (fontStyle);
				
				Set_BitString(FontStyle)	style = command->fNewFont.GetStyle ();
				if (style.Contains (correctTypeFontStyle)) {
					style.Remove (correctTypeFontStyle);
				}
				else {
					style.Add (correctTypeFontStyle);
				}
				command->fNewFont.SetStyle (style);
			}
			fCommands = new CompositeCommand (command, fCommands);
		}
	}
}

/*
 ********************************************************************************
 ***************************** InheritFontsCommand ******************************
 ********************************************************************************
*/
	// should be scoped cept for q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
	class	InheritFontCommand : public Command {
		public:
			InheritFontCommand (ViewItem& item) :
				Command (eInheritFont, kUndoable),
				fItem (item),
				fOldFont (Nil)
			{
				if (item.GetFont () != Nil) {
					fOldFont = new Font (*item.GetFont ());
				}
			}
			
			~InheritFontCommand ()
			{
				delete fOldFont;
			}
			
			override	void	DoIt ()
			{
				fItem.SetItemFont (Nil);
				fItem.ApplyCurrentParams ();
				fItem.Refresh ();
				Command::DoIt ();
			}
			
			override	void	UnDoIt ()
			{
				fItem.SetItemFont (fOldFont);
				fItem.ApplyCurrentParams ();
				fItem.Refresh ();
				Command::UnDoIt ();
			}
			
		private:
			ViewItem&	fItem;
			Font*		fOldFont;
	};

InheritFontsCommand::InheritFontsCommand (GroupItem& parent) :
	CompoundCommand (eInheritFont),
	fParent (parent)
{
	ForEach (ViewItemPtr, it, fParent.MakeItemIterator ()) {
		AssertNotNil (it.Current ());
		ViewItem& current = *it.Current ();
		
		if (current.GetSelected ()) {
			fCommands = new CompositeCommand (new InheritFontCommand (current), fCommands);
		}
	}
}

/*
 ********************************************************************************
 ******************************** SetFontsCommand *******************************
 ********************************************************************************
*/
SetFontsCommand::SetFontsCommand (GroupItem& parent, const String& fontName) :
	CompoundCommand (eFont),
	fParent (parent)
{
	ForEach (ViewItemPtr, it, fParent.MakeItemIterator ()) {
		AssertNotNil (it.Current ());
		ViewItem& current = *it.Current ();
		
		if (current.GetSelected ()) {
			SetItemFontCommand* command = new SetItemFontCommand (current);
			command->fNewFont.SetName (fontName);
			
			fCommands = new CompositeCommand (command, fCommands);
		}
	}
}


/*
 ********************************************************************************
 ***************************** FontSizeCommand ****************************
 ********************************************************************************
*/
FontSizesCommand::FontSizesCommand (GroupItem& parent, FontSize fontSize) :
	CompoundCommand (eFontSize),
	fParent (parent)
{
	ForEach (ViewItemPtr, it, fParent.MakeItemIterator ()) {
		AssertNotNil (it.Current ());
		ViewItem& current = *it.Current ();
		
		if (current.GetSelected ()) {
			SetItemFontCommand* command = new SetItemFontCommand (current);
			command->fNewFont.SetSize (fontSize);
			fCommands = new CompositeCommand (command, fCommands);
		}
	}
}


/*
 ********************************************************************************
 ******************************* SpaceItemsCommand ******************************
 ********************************************************************************
*/
SpaceItemsCommand::SpaceItemsCommand (GroupItem& parent, CommandNumber commandNumber, Coordinate gap) :
	CompoundCommand (commandNumber),
	fParent (parent)
{
	BuildSelectedSequence (((commandNumber == eSpaceVertically)  or (commandNumber == eUniformSpacesVertically))
		? Point::eVertical : Point::eHorizontal);

	Assert (fSelectedItems.GetLength () > 0);
	Point	where = fSelectedItems[1]->GetOrigin ();
	
	Region	region = kEmptyRegion;
	Point	itemSizes = kZeroPoint;
	CollectionSize	totalSelected = 0;
	{
		ForEach (ViewItemPtr, it, parent.MakeItemIterator ()) {
			AssertNotNil (it.Current ());
			if (it.Current ()->GetSelected ()) {
				region = region + it.Current ()->GetRegion ();
				itemSizes += it.Current ()->GetSize ();
				totalSelected++;
			}
		}
	}
	Rect	bounds = region.GetBounds ();
	Require (totalSelected > 1);

	if (commandNumber == eUniformSpacesVertically) {
		gap = (bounds.GetHeight () - itemSizes.GetV ()) / (totalSelected-1);
	}
	else if (commandNumber == eUniformSpacesHorizontally) {
		gap = (bounds.GetWidth () - itemSizes.GetH ()) / (totalSelected-1);
	}

	ForEach (ViewItemPtr, it, fSelectedItems) {
		AssertNotNil (it.Current ());
		ViewItem& current = *it.Current ();
		
		Assert (it.Current ()->GetSelected ());
		Point	newOrigin = current.GetOrigin ();
		switch (commandNumber) {
			case eSpaceVertically:
			case eUniformSpacesVertically:
				newOrigin = Point (where.GetV (), newOrigin.GetH ());
				where.SetV (where.GetV () + current.GetSize ().GetV () + gap);
				break;
				
			case eSpaceHorizontally:
			case eUniformSpacesHorizontally:
				newOrigin = Point (newOrigin.GetV (), where.GetH ());
				where.SetH (where.GetH () + current.GetSize ().GetH () + gap);
				break;
				
			default:	
				RequireNotReached ();			
		}
		fCommands = new CompositeCommand (new SizeItemCommand (current, "Move", newOrigin, current.GetSize ()), fCommands);
	}
}

void	SpaceItemsCommand::BuildSelectedSequence (Point::Direction d)
{
	ForEach (ViewItemPtr, it, fParent.MakeItemIterator ()) {
		ViewItem*	current = it.Current ();
		AssertNotNil (current);
		
		if (current->GetSelected ()) {
			Boolean inserted = False;
			CollectionSize	index = 0;
			Coordinate	coor1 = current->GetOrigin ()[d];
			ForEach (ViewItemPtr, it1, fSelectedItems) {
				index++;
				Assert (it1.Current () != current);				
				if (coor1 < it1.Current ()->GetOrigin ()[d]) {
					fSelectedItems.InsertAt (current, index);
					inserted = True;
					break;
				}
			}
			if (not inserted) {
				fSelectedItems.Append (current);
			}
		}
	}
}


/*
 ********************************************************************************
 ***************************** AlignItemsCommand ***************************
 ********************************************************************************
*/
AlignItemsCommand::AlignItemsCommand (GroupItem& parent, CommandNumber commandNumber) :
	CompoundCommand (commandNumber),
	fParent (parent)
{
	Region	region = kEmptyRegion;
	{
	ForEach (ViewItemPtr, it, parent.MakeItemIterator ()) {
		AssertNotNil (it.Current ());
		if (it.Current ()->GetSelected ()) {
			region = region + it.Current ()->GetRegion ();
		}
	}
	}
	Rect	bounds = region.GetBounds ();

	ForEach (ViewItemPtr, it, fParent.MakeItemIterator ()) {
		AssertNotNil (it.Current ());
		ViewItem& current = *it.Current ();
		
		if (current.GetSelected ()) {
			Point	newOrigin = current.GetOrigin ();
			switch (commandNumber) {
				case eAlignLeft:
					newOrigin = Point (newOrigin.GetV (), bounds.GetLeft ());
					break;
					
				case eAlignCenterH:
					newOrigin = Point (newOrigin.GetV (), bounds.GetLeft () + (bounds.GetWidth () - current.GetExtent ().GetWidth ())/2);
					break;
					
				case eAlignRight:
					newOrigin = Point (newOrigin.GetV (), bounds.GetRight () - current.GetSize ().GetH ());
					break;
					
				case eAlignTop:
					newOrigin = Point (bounds.GetTop (), newOrigin.GetH ());
					break;

				case eAlignCenterV:
					newOrigin = Point (bounds.GetTop () + (bounds.GetHeight () - current.GetExtent ().GetHeight ())/2, newOrigin.GetH ());
					break;
					
				case eAlignBottom:
					newOrigin = Point (bounds.GetBottom () - current.GetSize ().GetV (), newOrigin.GetH ());
					break;
					
				default:	
					RequireNotReached ();			
			}
			fCommands = new CompositeCommand (new SizeItemCommand (current, "Move", newOrigin, current.GetSize ()), fCommands);
		}
	}
}

/*
 ********************************************************************************
 ***************************** ConstrainItemCommand ****************************
 ********************************************************************************
*/
class	ConstrainItemCommand : public Command {
	public:
		ConstrainItemCommand  (ViewItem& item, CommandNumber commandNumber);
		
		override	void	DoIt ()
		{
			switch (GetCommandNumber ()) {
				case eConstrainLeft:
					fItem.SetConstrainLeft (not fOldConstraint);
					break;
		
				case eConstrainTop:
					fItem.SetConstrainTop (not fOldConstraint);
					break;
		
				case eConstraintWidth:
					fItem.SetConstrainWidth (not fOldConstraint);
					break;
		
				case eConstrainHeight:
					fItem.SetConstrainHeight (not fOldConstraint);
					break;
		
				default:
					AssertNotReached ();
			}
			fItem.Refresh ();
			Command::DoIt ();
		}
		
		override	void	UnDoIt ()
		{
			switch (GetCommandNumber ()) {
				case eConstrainLeft:
					fItem.SetConstrainLeft (fOldConstraint);
					break;
		
				case eConstrainTop:
					fItem.SetConstrainTop (fOldConstraint);
					break;
		
				case eConstraintWidth:
					fItem.SetConstrainWidth (fOldConstraint);
					break;
		
				case eConstrainHeight:
					fItem.SetConstrainHeight (fOldConstraint);
					break;
		
				default:
					AssertNotReached ();
			}
			fItem.Refresh ();
			Command::UnDoIt ();
		}
	private:
		Boolean		fOldConstraint;
		ViewItem&	fItem;
};
		ConstrainItemCommand::ConstrainItemCommand  (ViewItem& item, CommandNumber commandNumber) :
			Command (commandNumber, kUndoable),
			fItem (item),
			fOldConstraint (False)
		{
			switch (GetCommandNumber ()) {
				case eConstrainLeft:
					fOldConstraint = fItem.GetConstrainLeft ();
					break;
		
				case eConstrainTop:
					fOldConstraint = fItem.GetConstrainTop ();
					break;
		
				case eConstraintWidth:
					fOldConstraint = fItem.GetConstrainWidth ();
					break;
		
				case eConstrainHeight:
					fOldConstraint = fItem.GetConstrainHeight ();
					break;
		
				default:
					AssertNotReached ();
			}
		}


/*
 ********************************************************************************
 ***************************** ConstrainItemsCommand ****************************
 ********************************************************************************
*/
ConstrainItemsCommand::ConstrainItemsCommand (GroupItem& parent, CommandNumber commandNumber) :
	CompoundCommand (commandNumber),
	fParent (parent)
{
	ForEach (ViewItemPtr, it, fParent.MakeItemIterator ()) {
		AssertNotNil (it.Current ());
		ViewItem& current = *it.Current ();
		
		if (current.GetSelected ()) {
			fCommands = new CompositeCommand (new ConstrainItemCommand (current, commandNumber), fCommands);
		}
	}
}

/*
 ********************************************************************************
 ****************************** SizeItemCommand *********************************
 ********************************************************************************
 */

SizeItemCommand::SizeItemCommand (ViewItem& view, const String& name, const Point& newOrigin, const Point& newSize):
	Command (CommandHandler::eNoCommand, Command::kUndoable),
	fView (view),
	fOldOrigin (view.GetOrigin ()),
	fNewOrigin (newOrigin),
	fOldSize (view.GetSize ()),
	fNewSize (newSize)
{	
	SetName (name);
}

void	SizeItemCommand::DoIt () 
{
	fView.SetItemOrigin (fNewOrigin);
	fView.SetItemSize (fNewSize);
	fView.ApplyCurrentParams ();
	Command::DoIt ();
}

void	SizeItemCommand::UnDoIt () 
{
	fView.SetItemOrigin (fOldOrigin);
	fView.SetItemSize (fOldSize);
	fView.ApplyCurrentParams ();
	Command::UnDoIt ();
}

/*
 ********************************************************************************
 ********************************* AddItemCommand *******************************
 ********************************************************************************
*/
AddItemCommand::AddItemCommand (GroupItem& group, ViewItem& item) :
	Command (CommandHandler::eNoCommand, kUndoable),
	fGroup (group),
	fItem (item)		
{
	String s = "Build ";
	s += item.GetFieldClass ();
	
	SetName (s);
}
		
void	AddItemCommand::DoIt ()
{
	fGroup.AddSubItem (&fItem);
	Command::DoIt ();
}
		
void	AddItemCommand::UnDoIt ()
{
	fGroup.RemoveSubItem (&fItem);
	Command::UnDoIt ();
}


/*
 ********************************************************************************
 ******************************** GroupItemsCommand *****************************
 ********************************************************************************
*/
GroupItem*	GroupItemsCommand::GroupItems (GroupItem& parent)
{
	Region	region = parent.CalcSelectedRegion ();
	Rect	bounds = region.GetBounds ().InsetBy (Point (-2, -3));
	bounds = parent.TabletToLocal (bounds);
	GroupItem* newGroup = new GroupItem (GroupItemType::Get (), False);
	newGroup->SetItemOrigin (bounds.GetOrigin ());
	newGroup->SetItemSize (bounds.GetSize ());
	newGroup->ApplyCurrentParams ();
	
	ForEach (ViewItemPtr, it1, parent.MakeItemIterator (eSequenceBackward)) {
		ViewItem* current = it1.Current ();
		AssertNotNil (current);
		
		if (current->GetSelected ()) {
			parent.RemoveSubItem (current);
			current->SetItemOrigin (current->GetItemOrigin () - newGroup->GetItemOrigin ());
			current->SetSelected (False);
			newGroup->AddSubItem (current);
		}
	}
	parent.AddSubItem (newGroup);
	newGroup->SetSelected (True);
	return (newGroup);
}
		
GroupItemsCommand::GroupItemsCommand (GroupItem& parent) :
	Command (eGroupItems, kUndoable),
	fParent (parent),
	fNewGroup (Nil)		
{
}
		
void	GroupItemsCommand::DoIt ()
{
	Require (fNewGroup == Nil);
	fNewGroup = GroupItems (fParent);
	Command::DoIt ();
}
		
void	GroupItemsCommand::UnDoIt ()
{
	RequireNotNil (fNewGroup);
	UngroupItemsCommand::UngroupItems (fNewGroup);
	fNewGroup = Nil;
	Command::UnDoIt ();
}


/*
 ********************************************************************************
 ******************************* UngroupItemsCommand ****************************
 ********************************************************************************
*/
UngroupItemsCommand::UngroupItemsCommand (GroupItem* group) :
	Command (eUngroupItems, kUndoable),
	fGroup (group),
	fParent (*group->GetGroup ())
{
}
	
void	UngroupItemsCommand::DoIt ()
{
	UngroupItems (fGroup);
	fGroup = Nil;
	Command::DoIt ();
}

void	UngroupItemsCommand::UnDoIt ()
{
	Require (fGroup == Nil);
	fGroup = GroupItemsCommand::GroupItems (fParent);
	Command::UnDoIt ();
}
		
void	UngroupItemsCommand::UngroupItems (GroupItem* group)
{
	RequireNotNil (group);
	GroupItem* parent = group->GetGroup ();
	RequireNotNil (parent);
	
	{
	// subtle point: we wrap the iterator in a scope to ensure that the iterator is deleted
	// before we delete the group: otherwise the iterator's list would be deleted out from
	// under it with tragic results
	
	ForEach (ViewItemPtr, it, group->MakeItemIterator ()) {
		ViewItem* current = it.Current ();
		AssertNotNil (current);
		
		Point newOrigin = group->LocalToEnclosure (current->GetItemOrigin ());
		group->RemoveSubItem (current);
		current->SetItemOrigin (newOrigin);
		current->SetSelected (True);
		parent->AddSubItem (current);
	}
	}
	parent->Refresh (group->GetDrawnRegion ());	// this should not be needed, must be a bug in Stroika
	parent->RemoveSubItem (group);
	delete group;
}



/*
 ********************************************************************************
 **************************** ClearSelectedItemsCommand *************************
 ********************************************************************************
*/
ClearSelectedItemsCommand::ClearSelectedItemsCommand (GroupItem& group) :
	Command (CommandHandler::eClear, kUndoable),
	fGroup (group)
{
	ForEach (ViewItemPtr, it, fGroup.MakeItemIterator (eSequenceBackward)) {
		ViewItem*	current = it.Current ();
		if (current->GetSelected ()) {
			fItems.Append (current);
		}
	}
}

ClearSelectedItemsCommand::~ClearSelectedItemsCommand ()
{
	if (GetDone ()) {
		ForEach (ViewItemPtr, it, fItems) {
			ViewItem*	current = it.Current ();
			delete current;
		}
	}
}

void	ClearSelectedItemsCommand::DoIt ()
{
	ForEach (ViewItemPtr, it, fItems) {
		ViewItem*	current = it.Current ();
		fGroup.RemoveSubItem (current);
	}
	Command::DoIt ();
}

void	ClearSelectedItemsCommand::UnDoIt ()
{
	ForEach (ViewItemPtr, it, fItems) {
		ViewItem*	current = it.Current ();
		fGroup.AddSubItem (current);
	}
	Command::UnDoIt ();
}

/*
 ********************************************************************************
 ******************************** PasteItemsCommand ******************************
 ********************************************************************************
*/
#include "fstream.h"

PasteItemsCommand::PasteItemsCommand (GroupItem& group) :
	Command (CommandHandler::ePaste, kUndoable),
	fGroup (group),
	fPasteItems ()
{
	GroupItem	tempGroup (GroupItemType::Get (), False);
	
	size_t size = 0;
	void* data = Nil;
	if (ClipBoard::Get ().GetClip (ItemScrap::eView, data, size)) {
		AssertNotNil (data);
		StringStream	stream;
		stream << (char*)data;
		tempGroup.DoRead (stream);
	}

	ForEach (ViewItemPtr, it, tempGroup.MakeItemIterator ()) {
		ViewItem*	current = it.Current ();
		tempGroup.RemoveSubItem (current);
		current->SetFieldName (kEmptyString);
		current->SetSelected (True, Panel::eNoUpdate);
		fPasteItems.Append (current);
	}
}

PasteItemsCommand::~PasteItemsCommand () 
{
	if (not GetDone ()) {
		ForEach (ViewItemPtr, it, fPasteItems) {
			AssertNotNil (it.Current ());
			ViewItem*	item = it.Current ();
			delete item;
		}
	}
}
		
void	PasteItemsCommand::DoIt () 
{
	Point	pasteOrigin = Point (kMaxCoordinate, kMaxCoordinate);
	ForEach (ViewItemPtr, it, fPasteItems) {
		AssertNotNil (it.Current ());
		ViewItem& current = *it.Current ();
		pasteOrigin = Min (pasteOrigin, current.GetOrigin ());
	}

	Point	pasteOffset = fGroup.TabletToLocal (fGroup.GetPastePoint ());
	Assert (fGroup.GetLocalExtent ().Contains (pasteOffset));
	ForEach (ViewItemPtr, it1, fPasteItems) {
		ViewItem&	current = *it1.Current ();
		current.SetItemOrigin (current.GetItemOrigin () + pasteOffset - pasteOrigin);
		current.ApplyCurrentParams ();
		fGroup.AddSubItem (&current);
		fGroup.Refresh (current.GetDrawnRegion ());
	}
	fGroup.Update ();
	Command::DoIt ();
}
		
void	PasteItemsCommand::UnDoIt () 
{
	ForEach (ViewItemPtr, it, fPasteItems) {
		AssertNotNil (it.Current ());
		fGroup.Refresh (it.Current ()->GetDrawnRegion ());
		fGroup.RemoveSubItem (it.Current ());
	}
	fGroup.Update ();
	Command::UnDoIt ();
}
