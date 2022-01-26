/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ViewItem.cc,v 1.9 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ViewItem.cc,v $
 *		Revision 1.9  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.8  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.7  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.4  1992/07/04  14:36:35  lewis
 *		Changed SelectorShape to be a SelectorShapeRep - so it would work with new
 *		shape letter/envelope stuff.
 *
 *		Revision 1.30  92/05/01  01:30:28  01:30:28  lewis (Lewis Pringle)
 *		Added X specific bug workaround in TrackPress () - we were accidentally - because of a
 *		incomplete implementation of shape::toregion().
 *		
 *		Revision 1.17  1992/02/18  03:51:37  lewis
 *		Had to change type name of Version to SaveableVersion to avoid name conflict.
 *
 *		Revision 1.13  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *
 */





#include	"MenuOwner.hh"
#include	"StreamUtils.hh"

#include	"GroupItem.hh"
#include	"ItemPallet.hh"
#include	"Shape.hh"
#include	"Dialog.hh"
#include	"PickList.hh"

#include	"DeleteCustomizations.hh"
#include	"ItemType.hh"
#include	"ViewItem.hh"
#include	"ViewItemInfo.hh"
#include	"ViewInfo.hh"
#include	"EmilyWindow.hh"


#if		!qRealTemplatesAvailable
	Implement (Iterator, ViewItemPtr);
	Implement (Collection, ViewItemPtr);
	Implement (AbSequence, ViewItemPtr);
	Implement (Array, ViewItemPtr);
	Implement (Sequence_Array, ViewItemPtr);
	Implement (Sequence, ViewItemPtr);
#endif

#if		!qRealTemplatesAvailable
	Implement (Iterator, ItemParamsPtr);
	Implement (Collection, ItemParamsPtr);
	Implement (AbSequence, ItemParamsPtr);
	Implement (Array, ItemParamsPtr);
	Implement (Sequence_Array, ItemParamsPtr);
	Implement (Sequence, ItemParamsPtr);
#endif


class	ItemSelector : public View, public CommandHandler {
	public:
		ItemSelector (ViewItem& item);		

		override	void	Draw (const Region& update);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		
	private:
		nonvirtual	ViewItem&	GetViewItem () const;

		ViewItem&					fItem;
		class	SelectorShapeRep*	fHandles;
};




// to allow our subclasses to use versioning, we do it by hand, instead
// of relying on Saveable's implementation

static	const	SaveableVersion	kItemParamsVersion = 4;
const	Point	kBadBorder = Point (-100, -100);
const	Point	kBadMargin = Point (-100, -100);

// can't safely use eReadOnly for static on Mac, as Strings embedded in code, 
// and code can move
const	String	ViewItem::kEmptyInitializer = String ("(),"/*, String::eReadOnly*/);
const	String	ViewItem::kFieldAccessor = String ("."/*, String::eReadOnly*/);

ItemParams::ItemParams (CommandNumber language, CommandNumber gui) :
	Saveable (1),
	fLanguage (language),
	fGUI (gui),
	fOrigin (kZeroPoint),
	fSize (kZeroPoint),
	fBorder (kBadBorder),
	fMargin (kBadMargin),
	fEnabled (True),
	fDefaultBorder (kBadBorder),
	fDefaultMargin (kBadMargin),
	fVisible (True),
	fFont (Nil),
	fHelp (kEmptyString)
{
}

ItemParams::~ItemParams ()
{
	delete fFont;
}

void	ItemParams::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	SaveableVersion	version  = Saveable::ReadVersion (kItemParamsVersion, from);

	from >> fLanguage >> fGUI >> fOrigin >> fSize >> fEnabled >> fVisible;
	
	if (version > 1) {
		from >> fBorder >> fMargin;
	}
	if (version > 2) {
		from >> fDefaultBorder >> fDefaultMargin;
	}
	if (version <= 3) {
		fLanguage += (eFirstLanguage - 50056);
		fGUI += (eFirstGUI - 50053);
	}
	else {
		fLanguage += eFirstLanguage;
		fGUI += eFirstGUI;
	}
	
	ReadString (from, fHelp);
	Boolean	hasFont;
	from >> hasFont;
	if (hasFont) {
		Font f (kSystemFont);
		from >> f;
		fFont = new Font (f);
	}
}

void	ItemParams::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);
	Saveable::WriteVersion (kItemParamsVersion, to, tabCount);

	to << newline;
	to << tab (tabCount) << (fLanguage - eFirstLanguage) << ' ' << (fGUI - eFirstGUI) << newline;
	to << tab (tabCount) << fOrigin << fSize << ' ' << fEnabled << fVisible;
	to << fBorder << fMargin << fDefaultBorder << fDefaultMargin << newline;
	to << tab (tabCount);
	WriteString (to, fHelp);
	to << newline;
	Boolean	hasFont = Boolean (fFont != Nil);
	to << tab (tabCount) << hasFont << ' ';
	if (hasFont) {
		to << *fFont;
	}
	to << newline;
}


// to allow our subclasses to use versioning, we do it by hand, instead
// of relying on Saveable's implementation

static const	SaveableVersion	kVersion = 6;

ViewItem::ViewItem (const ItemType& itemType) :
	Saveable (2),
	fOwnedView (Nil),
	fItemType (itemType),
	fGroup (Nil),
	fSelector (Nil),
	fSelected (False),
	fFieldName (kEmptyString),
	fFieldClass (itemType.GetItemClass ()),
	fDefaultFont (Nil),
	fFieldNameTemporary (True),
	fConstrainLeft (False),
	fConstrainTop (False),
	fConstrainHeight (False),
	fConstrainWidth (False),
	fBaseLanguage (EmilyWindow::GetLanguage ()),
	fBaseGUI (EmilyWindow::GetGUI ()),
	fCurrentParams (Nil)
{
	fSelector = new ItemSelector (*this);
	GetSelector ().SetVisible (GetSelected ());
	AddSubView (fSelector);
}
	
ViewItem::~ViewItem ()
{
	RemoveSubView (fSelector);
	delete fSelector;
	
	Require (GetGroup () == Nil);
	if (fOwnedView != Nil) {
		RemoveSubView (fOwnedView);
		delete (fOwnedView);
	}
	delete fDefaultFont;
	ForEach (ItemParamsPtr, it, fParams) {
		ItemParams*	current = it.Current ();
		delete current;
	}
}

void	ViewItem::DoSetupMenus ()
{
	if (ItemPallet::GetEditMode () and GetSelected ()) {
		EnableCommand (eSetItemInfo);
		EnableCommand (eSizeToFit, CanChangeExtent ());
		EnableCommand (eAutoCustomize, 
			Boolean ((EmilyWindow::GetLanguage () != GetBaseLanguage ()) or 
					 (EmilyWindow::GetGUI () != GetBaseGUI ())));
		EnableCommand (eEnabled, ItemCanBeEnabled ());
	}
}

	// should be scoped cept for q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
	class	SizeToFitCommand : public Command {
		public:
			SizeToFitCommand (ViewItem& item) :
				Command (eSizeToFit, kUndoable),
				fItem (item),
				fOldSize (item.GetItemSize ()),
				fNewSize (item.CalcDefaultSize (item.GetItemSize ()))
			{	
				if (fNewSize == kZeroPoint) {
					fNewSize = fOldSize;
				}
				DoIt ();
			}
			
			override	void	DoIt () 
			{
				fItem.SetItemSize (fNewSize);
				fItem.ApplyCurrentParams ();
				Command::DoIt ();
			}
			
			override	void	UnDoIt () 
			{
				fItem.SetItemSize (fOldSize);
				fItem.ApplyCurrentParams ();
				Command::UnDoIt ();
			}
			
		private:
			ViewItem&	fItem;
			Point		fNewSize;
			Point		fOldSize;
	};
	
	// should be scoped cept for q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
	class	EnableItemCommand : public Command {
		public:
			EnableItemCommand (ViewItem& item) :
				Command (eEnabled, kUndoable),
				fItem (item)
			{
			}
			
			override	void	DoIt ()
			{
				fItem.SetItemEnabled (not fItem.GetItemEnabled ());
				fItem.ApplyCurrentParams ();
				Command::DoIt ();
			}
			
			override	void	UnDoIt ()
			{
				fItem.SetItemEnabled (not fItem.GetItemEnabled ());
				fItem.ApplyCurrentParams ();
				Command::UnDoIt ();
			}

		private:
			ViewItem&	fItem;
	};

Boolean	ViewItem::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eSizeToFit:
			PostCommand (new SizeToFitCommand (*this));
			DirtyDocument ();
			return (True);
		
		case eAutoCustomize:
			AutoCustomize (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
			DirtyDocument ();
			return (True);
			
		case eSetItemInfo:
			SetItemInfo ();
			return (True);

		case eEnabled:
			if (ItemCanBeEnabled ()) {
				PostCommand (new EnableItemCommand (*this));
				DirtyDocument ();
			}
			return (True);

		default:
			return (False);
	}
	AssertNotReached (); return (False);
}

void	ViewItem::Layout ()
{
	View::Layout ();
	GetSelector ().SetExtent (GetLocalExtent ());
	GetOwnedView ().SetSize (GetSize ());
}

Point	ViewItem::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point	newSize = GetOwnedView ().CalcDefaultSize (defaultSize);
	newSize = Max (Min (newSize, GetMaxSize ()), GetMinSize ());
	Point	sizeOffset   = GetOwnedView ().GetDrawnRegion ().GetBounds ().GetSize () - GetOwnedView ().GetSize ();
	newSize += sizeOffset;
	
	return (newSize);
}

Boolean	ViewItem::TrackPress (const MousePressInfo& mouseInfo)
{
	if (ItemPallet::GetPalletSelection () == eArrow) {
		RequireNotNil (GetGroup ());
		if (GetSelected ()) {
			if (mouseInfo.IsOpenSelection ()) {
				(void) DoCommand (CommandSelection (eSetItemInfo));
			}
// Temporary X bug workaround - now I see why we get clicks getting to the widgets in live mode - the
// trouble is that Shape::ToRegions () just return the bounding rect for now - not the real regio, so
// the below test returns TRUE even if the click is in the widget, but not the selector handles - so as
// a workaround - call on selector directly rather than counting of inherited View::TrackPress () to dispatch...
// REally that should be OK in iether case, but ....
// LGP April 30, 1992
//
#if	qXGDI
			else if (GetSelector ().Contains (mouseInfo.fPressAt)) {
				GetSelector ().TrackPress (MousePressInfo (mouseInfo,
					GetSelector ().EnclosureToLocal (mouseInfo.fPressAt)));
			}
#else
			else if (GetSelector ().Contains (mouseInfo.fPressAt)) {
				View::TrackPress (mouseInfo);
			}
#endif
			else if (mouseInfo.IsSimpleSelection () and CanChangeExtent ()) {
				GetGroup ()->DragSelected (MousePressInfo (mouseInfo, GetGroup ()->TabletToLocal (LocalToTablet (mouseInfo.fPressAt))));
			}
			else if (mouseInfo.IsExtendedSelection ()) {
				SetSelected (not GetSelected ());
			}
		}
		else {
			if (mouseInfo.IsExtendedSelection ()) {
				SetSelected (True);
			}
			else {
				GetGroup ()->GetMainGroup ().SelectAllItems (False);
				SetSelected (True);
				GetGroup ()->GetMainGroup ().Update ();
				if (CanChangeExtent ()) {
					GetGroup ()->DragSelected (MousePressInfo (mouseInfo, GetGroup ()->TabletToLocal (LocalToTablet (mouseInfo.fPressAt))));
				}
				
				GetGroup ()->ReorderSubItem (this);
			}
		}
		return (True);
	}
	else if (ItemPallet::GetEditMode ()) {
		AssertNotReached ();
		return (False);
	}

	return (View::TrackPress (mouseInfo));
}

Point	ViewItem::GetMinSize () const
{
	return (Point (10, 16));
}

Point	ViewItem::GetMaxSize () const
{
	return (Point (5000, 5000));
}

GroupItem*	ViewItem::GetGroup () const
{
	return (fGroup);
}

void	ViewItem::SetGroup (GroupItem* parent)
{
	Assert ((parent == Nil) or (GetGroup () == Nil));
	
	fGroup = parent;
}

View&	ViewItem::GetOwnedView () const
{
	RequireNotNil (fOwnedView);
	return (*fOwnedView);
}

void	ViewItem::SetOwnedView (View* ownedView)
{
	if (fOwnedView != ownedView) {
		SetOwnedView_ (ownedView);	
	}
	Ensure (fOwnedView == ownedView);
}

void	ViewItem::SetOwnedView_ (View* ownedView)
{
	Require (fOwnedView != ownedView);
	View* oldView = fOwnedView;

	if (fOwnedView != Nil) {
		RemoveSubView (fOwnedView);
		if (ownedView != Nil) {
			// hack, allow choice of not deleting owned view by calling twice
			// SetOwnedView (Nil); SetOwnedView (newView);
			delete (fOwnedView);
		}
	}
	fOwnedView = ownedView;
	if (fOwnedView != Nil) {
		if (oldView == Nil) {
			SetItemBorder (fOwnedView->GetBorder ());
			SetItemMargin (fOwnedView->GetMargin ());
			SetDefaultBorder (fOwnedView->GetBorder ());
			SetDefaultMargin (fOwnedView->GetMargin ());
		}

		SetDefaultsFromView (*fOwnedView);
		AddSubView (fOwnedView, eAppend);
	}
}

void	ViewItem::SetDefaultsFromView (View& v)
{
	ItemParams& params = GetCurrentParams ();
	
	params.fBorder = v.GetBorder ();
	params.fMargin = v.GetMargin ();
	params.fDefaultBorder = params.fBorder;
	params.fDefaultMargin = params.fMargin;

	if (fDefaultFont != Nil) {
		delete fDefaultFont;
		fDefaultFont = Nil;
	}
	fDefaultFont = v.GetFont ();
	if (fDefaultFont != Nil) {
		fDefaultFont = new Font (*fDefaultFont);
	}
	SetItemFont (fDefaultFont);
	v.SetFont (Nil);
	v.SetBackground (Nil);
}

void	ViewItem::EditModeChanged (Boolean newEditMode)
{
	GetSelector ().SetVisible (Boolean (newEditMode and GetSelected ()));
}

void	ViewItem::AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI)
{
	if (not FindParams (language, gui)) {
		ItemParams*	params = new ItemParams (language, gui);
		ItemParams* oldParams = FindParams (oldLanguage, oldGUI);
		if (oldParams != Nil) {
			CopyParams (oldParams, params);
		}
		fParams.Append (params);
	}
}

void	ViewItem::GUIChanged (CommandNumber /*oldGUI*/, CommandNumber newGUI)
{
	CommandNumber	language = EmilyWindow::GetLanguage ();
	if (FindParams (language, newGUI) == Nil) {
		if (FindParams (language, fBaseGUI) == Nil) {
			AddParam (language, fBaseGUI, fBaseLanguage, fBaseGUI);
		}
		if (newGUI != fBaseGUI) {
			AddParam (language, newGUI, language, fBaseGUI);
		}
	}
	ItemParams* params = FindParams (language, newGUI);
	AssertNotNil (params);
	ApplyParams (language, newGUI);
}

void	ViewItem::LanguageChanged (CommandNumber /*oldLanguage*/, CommandNumber newLanguage)
{
	CommandNumber	gui = EmilyWindow::GetGUI ();
	if (FindParams (newLanguage, gui) == Nil) {
		ItemParams*	baseParams = FindParams (newLanguage, eNoCommand);
		if (baseParams == Nil) {
			baseParams = FindParams (fBaseLanguage, gui);
		}
		if (baseParams == Nil) {
			AddParam (fBaseLanguage, gui, fBaseLanguage, fBaseGUI);
			baseParams = FindParams (fBaseLanguage, gui);
		}
		if ((newLanguage != baseParams->fLanguage) or (gui != baseParams->fGUI)) {
			AddParam (newLanguage, gui, baseParams->fLanguage, baseParams->fGUI);
		}
	}
	ItemParams* params = FindParams (newLanguage, gui);
	AssertNotNil (params);
	ApplyParams (newLanguage, gui);
}
		
String	ViewItem::GetFieldName () const
{
	return (fFieldName);
}

void	ViewItem::SetFieldName (const String& fieldName)
{
	if (fFieldName != fieldName) {
		fFieldName = fieldName;
		fFieldNameTemporary = Boolean (fieldName == kEmptyString);
	}
}

String	ViewItem::CalcFieldName ()
{
	return (GetGroup ()->GetMainGroup ().CalcFieldName ());
}

String	ViewItem::GetBaseClass () const
{
	return (fItemType.GetItemClass ());
}

String	ViewItem::GetFieldClass () const
{
	return (fFieldClass);
}

void	ViewItem::SetFieldClass (const String& fieldName)
{
	fFieldClass = fieldName;
}
		
String	ViewItem::GetHelp () const
{
	return (GetOwnedView ().GetHelp ());
}

void	ViewItem::SetHelp (const String& help)
{
	if (GetCurrentParams ().fHelp != help) {
		GetCurrentParams ().fHelp = help;
		if (EmilyWindow::GetFullEditing ()) {
			ForEach (ItemParamsPtr, it, fParams) {
				it.Current ()->fHelp = help;
			}
		}
	}
}
		
Boolean	ViewItem::GetSelected () const
{
	return (fSelected);
}

void	ViewItem::SetSelected (Boolean selected, Panel::UpdateMode updateMode)
{
	if (fSelected != selected) {
		SetSelected_ (selected, updateMode);
	}
	Ensure (fSelected == selected);
}

void	ViewItem::SetSelected_ (Boolean selected,  Panel::UpdateMode update)
{
	fSelected = selected;
	GetSelector ().SetVisible (selected, update);
	MenuOwner::SetMenusOutOfDate ();
}

Boolean	ViewItem::GetConstrainLeft () const
{
	return (fConstrainLeft);
}

void	ViewItem::SetConstrainLeft (Boolean constrain)
{
	fConstrainLeft = constrain;
}

Boolean	ViewItem::GetConstrainTop () const
{
	return (fConstrainTop);
}

void	ViewItem::SetConstrainTop (Boolean constrain)
{
	fConstrainTop = constrain;
}

Boolean	ViewItem::GetConstrainHeight () const
{
	return (fConstrainHeight);
}

void	ViewItem::SetConstrainHeight (Boolean constrain)
{
	fConstrainHeight = constrain;
}

Boolean	ViewItem::GetConstrainWidth () const
{
	return (fConstrainWidth);
}

void	ViewItem::SetConstrainWidth (Boolean constrain)
{
	fConstrainWidth = constrain;
}

Point	ViewItem::GetItemBorder () const
{
	return (GetCurrentParams ().fBorder);
}

void	ViewItem::SetItemBorder (Point newBorder)
{
	if (GetCurrentParams ().fBorder != newBorder) {
		GetCurrentParams ().fBorder = newBorder;
		if (EmilyWindow::GetFullEditing ()) {
			ForEach (ItemParamsPtr, it, fParams) {
				it.Current ()->fBorder = newBorder;
			}
		}
	}
}

Point	ViewItem::GetItemMargin () const
{
	return (GetCurrentParams ().fMargin);
}

void	ViewItem::SetItemMargin (Point newMargin)
{
	if (GetCurrentParams ().fMargin != newMargin) {
		GetCurrentParams ().fMargin = newMargin;
		if (EmilyWindow::GetFullEditing ()) {
			ForEach (ItemParamsPtr, it, fParams) {
				it.Current ()->fMargin = newMargin;
			}
		}
	}
}

Point	ViewItem::GetDefaultBorder () const
{
	return (GetCurrentParams ().fDefaultBorder);
}

void	ViewItem::SetDefaultBorder (Point newBorder)
{
	if (GetCurrentParams ().fDefaultBorder != newBorder) {
		GetCurrentParams ().fDefaultBorder = newBorder;
		if (EmilyWindow::GetFullEditing ()) {
			ForEach (ItemParamsPtr, it, fParams) {
				it.Current ()->fDefaultBorder = newBorder;
			}
		}
	}
}

Point	ViewItem::GetDefaultMargin () const
{
	return (GetCurrentParams ().fDefaultMargin);
}

void	ViewItem::SetDefaultMargin (Point newMargin)
{
	if (GetCurrentParams ().fDefaultMargin != newMargin) {
		GetCurrentParams ().fDefaultMargin = newMargin;
		if (EmilyWindow::GetFullEditing ()) {
			ForEach (ItemParamsPtr, it, fParams) {
				it.Current ()->fDefaultMargin = newMargin;
			}
		}
	}
}

Boolean	ViewItem::UsesBorders ()
{
	return (Boolean (GetDefaultBorder () != kBadBorder));
}

void	ViewItem::SetItemBorder_ (Point newBorder)
{
	GetOwnedView ().SetBorder (newBorder);
}

void	ViewItem::SetItemMargin_ (Point newMargin)
{
	GetOwnedView ().SetMargin (newMargin);
}


Point	ViewItem::GetItemOrigin () const
{
	return (GetCurrentParams ().fOrigin);
}

void	ViewItem::SetItemOrigin (const Point& origin)
{
	if (GetCurrentParams ().fOrigin != origin) {
		GetCurrentParams ().fOrigin = origin;
		if (EmilyWindow::GetFullEditing ()) {
			ForEach (ItemParamsPtr, it, fParams) {
				it.Current ()->fOrigin = origin;
			}
		}
	}
}

Point	ViewItem::GetItemSize () const
{
	return (GetCurrentParams ().fSize);
}

void	ViewItem::SetItemSize (const Point& size)
{
	if (GetCurrentParams ().fSize != size) {
		GetCurrentParams ().fSize = size;
		if (EmilyWindow::GetFullEditing ()) {
			ForEach (ItemParamsPtr, it, fParams) {
				it.Current ()->fSize = size;
			}
		}
	}
}

Boolean	ViewItem::GetItemEnabled () const
{
	return (GetCurrentParams ().fEnabled);
}

void	ViewItem::SetItemEnabled (Boolean enabled)
{
	if (GetCurrentParams ().fEnabled != enabled) {
		GetCurrentParams ().fEnabled = enabled;
		if (EmilyWindow::GetFullEditing ()) {
			ForEach (ItemParamsPtr, it, fParams) {
				it.Current ()->fEnabled = enabled;
			}
		}
	}
}

Boolean	ViewItem::GetItemVisible () const
{
	return (GetCurrentParams ().fVisible);
}

void	ViewItem::SetItemVisible (Boolean visible)
{
	if (GetCurrentParams ().fVisible != visible) {
		GetCurrentParams ().fVisible = visible;
		if (EmilyWindow::GetFullEditing ()) {
			ForEach (ItemParamsPtr, it, fParams) {
				it.Current ()->fVisible = visible;
			}
		}
	}
}

const Font*	ViewItem::GetItemFont () const
{
	return (GetCurrentParams ().fFont);
}

void	ViewItem::SetItemFont (const Font* font)
{
	if ((GetCurrentParams ().fFont != font) and 
	   ((font == Nil) or (GetCurrentParams ().fFont == Nil) or (*GetCurrentParams ().fFont != *font))) {
	   
		delete GetCurrentParams ().fFont;
		if (font == Nil) {
			GetCurrentParams ().fFont = Nil;
		}
		else {
			GetCurrentParams ().fFont = new Font (*font);
		}
		if (EmilyWindow::GetFullEditing ()) {
			ForEach (ItemParamsPtr, it, fParams) {
				ItemParams*	current = it.Current ();
				delete current->fFont;
				if (font == Nil) {
					current->fFont = Nil;
				}
				else {
					current->fFont = new Font (*font);
				}
			}
		}
	}
}

Boolean	ViewItem::CanChangeExtent () const
{
	return (Boolean ((GetGroup () == Nil) or (GetGroup ()->SubItemsCanChangeExtent ())));
}


ItemSelector&	ViewItem::GetSelector () const
{
	RequireNotNil (fSelector);
	return (*fSelector);
}

GroupItem*	ViewItem::GetMostSelectedGroup ()
{
	if (GetSelected ()) {
		return (GetGroup ());
	}
	return (Nil);
}

GroupItem*	ViewItem::GetGroupContaining (const Point& /*point*/)
{
	return (Nil);
}

#if qUseResourceBased
Boolean	ViewItem::IsResourceBased () const
{
	return (False);
}
#endif	/* qUseResourceBased */


void	ViewItem::DirtyDocument ()
{
	GetGroup ()->GetMainGroup ().DirtyDocument ();
}

void	ViewItem::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	SaveableVersion	version;
	Try {
		version = Saveable::ReadVersion (kVersion, from);
	}
	Catch1 (sBadVersion) {
		version = GetVersion ();
	}

	if (version >= 4) {
		CollectionSize paramCount;
		from >> fBaseLanguage >> fBaseGUI >> paramCount;
		ForEach (ItemParamsPtr, it, fParams) {
			ItemParams*	current = it.Current ();
			delete current;
		}
		fParams.RemoveAll ();
		fCurrentParams = Nil;
		
		for (int count = 1; count <= paramCount; count++) {
			ItemParams*	newParams = new ItemParams (eEnglish, eMacUI);
			char c;
			from >> c;
			Require (c == '{');
			newParams->DoRead (from);
			
			if (version <= 5) {
				char c;
				from >> c;
				Boolean	oldVersion = Boolean (c != '}');
				from.putback (c);
				
				if (oldVersion) {
					OldParamReadHack (from);
				}
			}
			
			from >> c;
			Require (c == '}');
			fParams.Append (newParams);
		}
		from >> fFieldNameTemporary;
	}
	else {
		Point	origin, size;
		from >> origin >> size;
		SetItemOrigin (origin);
		SetItemSize (size);
	
		Boolean	cLeft, cTop, cWidth, cHeight, enabled;
		from >> cLeft >> cTop >> cHeight >> cWidth;
		SetConstrainLeft (cLeft);
		SetConstrainTop (cTop);
		SetConstrainWidth (cWidth);
		SetConstrainHeight (cHeight);
	
		if (version == 2) {
			Boolean	autoSizeV, autoSizeH;
			Int32	fixedPointV, fixedPointH;
			from >> autoSizeV >> autoSizeH >> fixedPointV >> fixedPointH;
		}
	
		from >> enabled >> fFieldNameTemporary;
		SetItemEnabled (enabled);
		
		Boolean	hasFont;
		from >> hasFont;
		if (hasFont) {
			Font f (kSystemFont);
			from >> f;
			SetItemFont (&f);
		}
		else {
			SetItemFont (Nil);
		}
	}
	
	String foo;
	if (not fFieldNameTemporary) {
		ReadString (from, foo);
		SetFieldName (foo);
	}
	if (version < 4) {
		ReadString (from, foo);
		SetHelp (foo);
	}
	else {
		Boolean	cLeft, cTop, cWidth, cHeight;
		from >> cLeft >> cTop >> cHeight >> cWidth;
		SetConstrainLeft (cLeft);
		SetConstrainTop (cTop);
		SetConstrainWidth (cWidth);
		SetConstrainHeight (cHeight);
	}
	if (version <= 4) {
		fBaseLanguage += (eFirstLanguage - 50056);
		fBaseGUI += (eFirstGUI - 50053);
	}
	else {
		fBaseLanguage += eFirstLanguage;
		fBaseGUI += eFirstGUI;
	}
}

void	ViewItem::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);
	Saveable::WriteVersion (kVersion, to, tabCount);
	to << newline;
	
	CollectionSize	paramCount = fParams.GetLength ();
	to << tab (tabCount) << (fBaseLanguage - eFirstLanguage) << ' ' << (fBaseGUI - eFirstGUI) << ' ' << paramCount << newline;
	ForEach (ItemParamsPtr, it, fParams) {
		to << tab (tabCount) << '{' << newline;
		it.Current ()->DoWrite (to, tabCount+1);
		to << tab (tabCount) << '}' << newline;
	}

	to << tab (tabCount) << fFieldNameTemporary;
	if (not fFieldNameTemporary) {
		to << newline << tab (tabCount);
		WriteString (to, GetFieldName ());
	}
	to << tab (tabCount) << fConstrainLeft << fConstrainTop << fConstrainHeight << fConstrainWidth << newline;
}

String	ViewItem::GetSubviewFieldName () const
{
	return (GetFieldName ());
}

String	ViewItem::GetFocusItemFieldName () const
{
	return (GetFieldName ());
}

/* Code Generation */
void	ViewItem::DeclareField (class ostream& to, int tabCount, const String& className, const String& fieldName)
{
	const	int	kSpacing = 5;
	int	gap = Max (1, kSpacing - int ((className.GetLength ()) / 4));
	to << tab (tabCount) << className << tab (gap) << fieldName << ";" << newline;
}

void	ViewItem::WriteDeclaration (class ostream& to, int tabCount)
{
	if (fFieldNameTemporary) {
		SetFieldName (CalcFieldName ());
		fFieldNameTemporary = True;	// set false by SetFieldName
	}
	DeclareField (to, tabCount, GetFieldClass (), GetFieldName ());
}

void	ViewItem::WriteInclude (class ostream& to, int tabCount)
{
	if (GetHeaderFileName () != kEmptyString) {
		to << tab (tabCount) << "#include " << quote << GetHeaderFileName () << quote << newline;
	}
}

void	ViewItem::WriteIncludes (class ostream& /*to*/, int /*tabCount*/)
{
}

void	ViewItem::WriteInitializer (class ostream& to, int tabCount, CommandNumber gui)
{
	tabCount++;
//	WriteBuilder (to, tabCount);
	ItemParams* params = FindParams (GetBaseLanguage (), gui);
	if (params == Nil) {
		AddParam (fBaseLanguage, gui, fBaseLanguage, fBaseGUI);
		params = FindParams (GetBaseLanguage (), gui);
	}
	AssertNotNil (params);
	WriteParameters (to, tabCount, fBaseLanguage, gui);
}

void	ViewItem::WriteDestructor (class ostream& to, int tabCount, CommandNumber /*gui*/)
{
}
		
void	ViewItem::WriteBuilder (class ostream& to, int tabCount)
{
	RequireNotNil (GetGroup ());
	to << tab (tabCount) << GetFieldName () << space (1);
}
		
void	ViewItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ItemParams* temp = FindParams (language, gui);
	if (temp != Nil) {
		ItemParams& params = *temp;
		String	subViewPrologue = GetSubviewFieldName ();
		if (subViewPrologue != kEmptyString) {
			subViewPrologue += kFieldAccessor;
		}
		String	fieldPrologue = GetFieldName ();
		if (fieldPrologue != kEmptyString) {
			fieldPrologue += kFieldAccessor;
		}
		
		to << tab (tabCount) << subViewPrologue << "SetExtent ("
			<< params.fOrigin.GetV () << ", " << params.fOrigin.GetH () << ", " 
			<< params.fSize.GetV () << ", " << params.fSize.GetH () << ", eNoUpdate);" << newline;
		if (params.fHelp != kEmptyString) {
			to << tab (tabCount) << fieldPrologue << "SetHelp ("<< quote << params.fHelp << quote << ");" << newline;
		}
		if (not params.fEnabled) {
			to << tab (tabCount) << fieldPrologue << "SetEnabled (not EnableItem::kEnabled, eNoUpdate);" << newline;
		}
		if (params.fBorder != params.fDefaultBorder) {
			to << tab (tabCount) << subViewPrologue << "SetBorder (";
			to << params.fBorder.GetV () << ", " << params.fBorder.GetH ();
			to << ", eNoUpdate);" << newline;
		}
		if (params.fMargin != params.fDefaultMargin) {
			to << tab (tabCount) << subViewPrologue << "SetMargin (";
			to << params.fMargin.GetV () << ", " << params.fMargin.GetH ();
			to << ", eNoUpdate);" << newline;
		}
		WriteFont (to, tabCount, GetFont (), fDefaultFont, "SetFont", GetFieldName (), True);
	}
}
		
void	ViewItem::WriteFont (class ostream& to, UInt8 tabCount, const Font* font, const Font* defaultFont,
				   const String& methodName, const String& objectName, Boolean byAddress)
{
	if ((font != defaultFont) and ((font == Nil) or (defaultFont == Nil) or (*font != *defaultFont))) {
		String	fontName;
		if (font == Nil) {
			fontName = "Nil";
		}
		else if (*font == kSystemFont) {
			fontName = "kSystemFont";
		}
		else if (*font == kApplicationFont) {
			fontName = "kApplicationFont";
		}
		else {
			const	String	kFontName = "font";
			
			OStringStream	buf;
			static	int	i	=	0;
			buf << kFontName << i++;
			fontName = String (buf);
			
			to << tab (tabCount) << "Font" << tab << fontName << " = Font ("
			   << quote << font->GetName () << quote << ", " << font->GetSize ()  << ");" << newline;
// do something about font styles!!!
		}
		to << tab (tabCount);
		if (objectName != kEmptyString) {
			to << objectName << kFieldAccessor;
		}
		to << methodName;
		if ((byAddress) and (font != Nil)) {
			to << " (&";
		}
		else {
			to << " (";
		}
		to << fontName << ");" << newline;
	}
}

void	ViewItem::WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	if ((language != GetBaseLanguage ()) or (gui != GetBaseGUI ())) {
		ItemParams*	baseParams = FindParams (GetBaseLanguage (), GetBaseGUI ());
		AssertNotNil (baseParams);
		ItemParams*	currentParams = FindParams (language, gui);
		if (currentParams != Nil) {
			if (currentParams->fOrigin != baseParams->fOrigin) {
				to << tab (tabCount) << GetSubviewFieldName () << kFieldAccessor;
				to << "SetOrigin (Point (" << currentParams->fOrigin.GetV () << "," << currentParams->fOrigin.GetH () << "), eNoUpdate);" << newline;
			}
			if (currentParams->fSize != baseParams->fSize) {
				to << tab (tabCount) << GetSubviewFieldName () << kFieldAccessor;
				to << "SetSize (Point (" << currentParams->fSize.GetV () << "," << currentParams->fSize.GetH () << "), eNoUpdate);" << newline;
			}
			if (currentParams->fBorder != baseParams->fBorder) {
				to << tab (tabCount) << GetSubviewFieldName () << kFieldAccessor;
				to << "SetBorder (Point (" << currentParams->fBorder.GetV () << "," << currentParams->fBorder.GetH () << "), eNoUpdate);" << newline;
			}
			if (currentParams->fMargin != baseParams->fMargin) {
				to << tab (tabCount) << GetSubviewFieldName () << kFieldAccessor;
				to << "SetMargin (Point (" << currentParams->fMargin.GetV () << "," << currentParams->fMargin.GetH () << "), eNoUpdate);" << newline;
			}
			if (currentParams->fEnabled != baseParams->fEnabled) {
				to << tab (tabCount) << GetFieldName () << kFieldAccessor;
				if (currentParams->fEnabled) {
					to << "SetEnabled (EnableItem::kEnabled, eNoUpdate);" << newline;
				}
				else {
					to << "SetEnabled (not EnableItem::kEnabled, eNoUpdate);" << newline;
				}
			}
			if (currentParams->fVisible != baseParams->fVisible) {
				to << tab (tabCount) << GetSubviewFieldName () << kFieldAccessor;
				if (currentParams->fVisible) {
					to << "SetVisible (Panel::kVisible, eNoUpdate);" << newline;
				}
				else {
					to << "SetVisible (not Panel::kVisible, eNoUpdate);" << newline;
				}
			}
			WriteFont (to, tabCount, currentParams->fFont, baseParams->fFont, "SetFont", GetFieldName (), True);
			if (currentParams->fHelp != baseParams->fHelp) {
				to << tab (tabCount) << GetFieldName () << kFieldAccessor;
				to << "SetHelp ("<< quote << currentParams->fHelp << quote << ");" << newline;
			}
       	}
	}
}

void	ViewItem::WriteCustomizations (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	WriteCustomization (to, tabCount, language, gui);
}

void	ViewItem::WriteLayout (class ostream& to, int tabCount)
{
	if (GetGroup () != Nil) {
		String	groupSizeDeltaName = GetGroup ()->GetFieldSizeConstantName ();
		if (GetConstrainLeft () or GetConstrainTop ()) {
			String	fieldOriginName = "kOriginal" + GetFieldName () + "Origin";
			
			to << tab (tabCount) << "static const Point" << tab << fieldOriginName << " = " << GetSubviewFieldName () << ".GetOrigin ();" << newline;
			to << tab (tabCount) << GetSubviewFieldName () << ".SetOrigin (" << fieldOriginName;
			to << " - Point (";
			if (GetConstrainTop ()) {
				to << groupSizeDeltaName << ".GetV (), ";
			}
			else {
				to << "0, ";
			}	
			if (GetConstrainLeft ()) {
				to << groupSizeDeltaName << ".GetH ()";
			}
			else {
				to << "0";
			}	
			
			to << "));" << newline;
		}
		if (GetConstrainWidth () or GetConstrainHeight ()) {
			String	fieldSizeName = "kOriginal" + GetFieldName () + "Size";

			to << tab (tabCount) << "static const Point" << tab << fieldSizeName << " = " << GetSubviewFieldName () << ".GetSize ();" << newline;

			to << tab (tabCount) << GetSubviewFieldName () << ".SetSize ("  << fieldSizeName;
			to << " - Point (";
			if (GetConstrainHeight ()) {
				to << groupSizeDeltaName << ".GetV (), ";
			}
			else {
				to << "0, ";
			}	
			if (GetConstrainWidth ()) {
				to << groupSizeDeltaName << ".GetH ()";
			}
			else {
				to << "0";
			}	
			
			to << "));" << newline;
		}
	}
}

SequenceIterator(ItemParamsPtr)*	ViewItem::MakeParamsIterator (SequenceDirection d) const
{
	return (fParams.MakeSequenceIterator (d));
}

void	ViewItem::RemoveParam (CommandNumber language, CommandNumber gui)
{
	ItemParams* params = FindParams (language, gui);
	if (params != Nil) {
		if (params == fCurrentParams) {
			fCurrentParams = Nil;
		}
		
		fParams.Remove (params);
	}
	Ensure (FindParams (language, gui) == Nil);
}


ItemParams*	ViewItem::FindParams (CommandNumber language, CommandNumber gui) const
{
	if ((fCurrentParams != Nil) and (fCurrentParams->fLanguage == language) and (fCurrentParams->fGUI == gui)) {
		return (fCurrentParams);
	}
	
	ForEach (ItemParamsPtr, it, fParams) {
		ItemParams*	current = it.Current ();
		
		if (((current->fLanguage == language) or (language == CommandHandler::eNoCommand)) and 
			((current->fGUI == gui) or (gui == CommandHandler::eNoCommand))) {
			
			if ((current->fLanguage == EmilyWindow::GetLanguage ()) and (current->fGUI == EmilyWindow::GetGUI ())) {
				ViewItem*	This = (ViewItem*)this;
				This->fCurrentParams = current;
			}
			
			return (current);
		}
	}
	return (Nil);
}

ItemParams&	ViewItem::GetCurrentParams () const
{
	ItemParams* params = FindParams (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	
	if (params == Nil) {
		ViewItem*	This = (ViewItem*)this;
		This->AddParam (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI (), fBaseLanguage, fBaseGUI);
		params = FindParams (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	}
	EnsureNotNil (params);
	return (*params);
}

Boolean	ViewItem::ParamsExist (CommandNumber language, CommandNumber gui) const
{
	ItemParams* params = FindParams (language, gui);
	return (Boolean (params != Nil));
}


void	ViewItem::ApplyCurrentParams ()
{
// quickee hack
GetCurrentParams ();
	ApplyParams (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
}

void	ViewItem::ApplyParams (CommandNumber language, CommandNumber gui)
{
	ItemParams* params = FindParams (language, gui);
	AssertNotNil (params);
	
	SetOrigin (params->fOrigin);
	SetSize (params->fSize);
	SetOwnedEnabled (params->fEnabled, eDelayedUpdate);
	SetVisible (params->fVisible);
	if (params->fBorder != kBadBorder) {
		SetItemBorder_ (params->fBorder);
	}
	if (params->fMargin != kBadMargin) {
		SetItemMargin_ (params->fMargin);
	}
	SetFont (params->fFont);
	GetOwnedView ().SetHelp (params->fHelp);
}

void	ViewItem::SetOwnedEnabled (Boolean /*enabled*/, UpdateMode /*updateMode*/)
{
}

void	ViewItem::OldParamReadHack (class istream& /*from*/)
{
	// in pushbuttonitem, radiobuttonitem, and checkboxitem, read only files
	// info
}

void	ViewItem::CopyParams (ItemParams* oldParams, ItemParams* newParams)
{
	RequireNotNil (oldParams);
	RequireNotNil (newParams);
	Require ((oldParams->fLanguage != newParams->fLanguage) or (oldParams->fGUI != newParams->fGUI));

	newParams->fOrigin = oldParams->fOrigin;
	newParams->fSize = oldParams->fSize;
	newParams->fBorder = oldParams->fBorder;
	newParams->fMargin = oldParams->fMargin;
	newParams->fDefaultBorder = oldParams->fDefaultBorder;
	newParams->fDefaultMargin = oldParams->fDefaultMargin;
	newParams->fEnabled = oldParams->fEnabled;
	newParams->fVisible = oldParams->fVisible;
	newParams->fHelp = oldParams->fHelp;
	if (oldParams->fFont == Nil) {
		newParams->fFont = Nil;
	}
	else {
		newParams->fFont = new Font (*oldParams->fFont);
	}
}

void	ViewItem::AutoCustomize (CommandNumber language, CommandNumber gui)
{
	ItemParams*	params = FindParams (language, gui);
	ItemParams* oldParams = FindParams (GetBaseLanguage (), GetBaseGUI ());

	AssertNotNil (params);
	AssertNotNil (oldParams);
	
	if ((oldParams->fBorder == oldParams->fDefaultBorder) and (oldParams->fMargin == oldParams->fDefaultMargin)) {
		params->fBorder = params->fDefaultBorder;
		params->fMargin = params->fDefaultMargin;
		ApplyCurrentParams ();
	}
}

void	ViewItem::DeleteCustomizations ()
{
	DeleteCustomizationsDialog d = DeleteCustomizationsDialog ();
	
	StringPickList&	list = d.GetCustomizationList ();
	ForEach (ItemParamsPtr, it, fParams) {
		AssertNotNil (it.Current ());
		
		ItemParams&	current = *it.Current ();
	
		String guiName;
		String languageName;
				
		CommandNameTable::Get ().Lookup (current.fGUI, &guiName);
		CommandNameTable::Get ().Lookup (current.fLanguage, &languageName);
		
		list.AddItem (guiName + " -- " + languageName);
		
		if ((current.fGUI == GetBaseGUI ()) and (current.fLanguage == GetBaseLanguage ())) {
			list.GetItemByIndex (list.GetItemCount ())->SetEnabled (False);
		}
	}
	
	if (d.Pose ()) {
		CollectionSize index = list.GetItemCount ();
		ForEach (PLstItmPtr, it, list.MakeItemIterator (eSequenceBackward)) {
			if (it.Current ()->GetSelected ()) {
				ItemParams* params = fParams[index];
				AssertNotNil (params);
				RemoveParam (params->fLanguage, params->fGUI);
			}
			index--;
		}
		DirtyDocument ();
	}
}

void	ViewItem::SetItemInfo ()
{
	ViewInfo info = ViewInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetItemInfoCommand (*this, info.GetViewItemInfo ()));
		DirtyDocument ();
	}
}

void	ViewItem::ReorderFocusItems ()
{
}

/*
 ********************************************************************************
 ******************************** SelectorShapeRep *********************************
 ********************************************************************************
 */
class	SelectorShapeRep : public ShapeRepresentation {
	public:
		static	const	Point	kHandleSize;

		SelectorShapeRep ();

		override	Region	ToRegion (const Rect& shapeBounds) const;
		override	Boolean	Contains (const Point& p, const Rect& shapeBounds) const;
		override	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		override	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;

		nonvirtual	Rect	GetHandle (ShapeSizer::SizeDirection c, const Rect& shapeBounds)	const;
		nonvirtual	ShapeSizer::SizeDirection	GetDirection (const Point& p, const Rect& shapeBounds) const;
};

const	Point	SelectorShapeRep::kHandleSize	=	Point (4, 4);

SelectorShapeRep::SelectorShapeRep ()
{
}

Region	SelectorShapeRep::ToRegion (const Rect& shapeBounds) const
{
	Region	r = kEmptyRegion;
	
	for (ShapeSizer::SizeDirection i = ShapeSizer::eBotRight; i <= ShapeSizer::eRight; i++) {
		r += GetHandle (i, shapeBounds);
	}
	return (r);
}

Boolean	SelectorShapeRep::Contains (const Point& p, const Rect& shapeBounds) const
{
	return (Boolean (GetDirection (p, shapeBounds) != ShapeSizer::eDefault));
}

ShapeSizer::SizeDirection	SelectorShapeRep::GetDirection (const Point& p, const Rect& shapeBounds) const
{
	for (ShapeSizer::SizeDirection i = ShapeSizer::eBotRight; i <= ShapeSizer::eRight; i++) {
		if (GetHandle (i, shapeBounds).Contains (p)) {
			return (i);
		}
	}
	return (ShapeSizer::eDefault);
}

void	SelectorShapeRep::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
{
	for (ShapeSizer::SizeDirection i = ShapeSizer::eBotRight; i <= ShapeSizer::eRight; i++) {
		on.OutLineRect (GetHandle (i, shapeBounds), pen);
	}
}

void	SelectorShapeRep::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
{
	for (ShapeSizer::SizeDirection i = ShapeSizer::eBotRight; i <= ShapeSizer::eRight; i++) {
		on.PaintRect (GetHandle (i, shapeBounds), brush);
	}
}

Rect	SelectorShapeRep::GetHandle (ShapeSizer::SizeDirection c, const Rect& shapeBounds)	const
{
	static	const Rect	littleRect	=	Rect (kZeroPoint, kHandleSize);
	Rect	boundsRect	=	shapeBounds;
	Point	delta		=	kZeroPoint;
	switch (c) {
		case ShapeSizer::eTopLeft:	
			delta = boundsRect.GetTopLeft ();	
			break;
			
		case ShapeSizer::eTop:		
			delta = boundsRect.GetTopLeft () + Point (0, (shapeBounds.GetWidth () - kHandleSize.GetH ())/2); 
			break;
			
		case ShapeSizer::eTopRight:	
			delta = boundsRect.GetTopRight () - Point (0, kHandleSize.GetH ());	
			break;
			
		case ShapeSizer::eRight:	
			delta = boundsRect.GetTopRight () + Point ((shapeBounds.GetHeight () - kHandleSize.GetV ())/2, -kHandleSize.GetH ()); 
			break;
			
		case ShapeSizer::eBotRight:	
			delta = boundsRect.GetBotRight () - kHandleSize;	
			break;
			
		case ShapeSizer::eBottom:	
			delta = boundsRect.GetBotLeft () + Point (-kHandleSize.GetV (), (shapeBounds.GetWidth () - kHandleSize.GetH ())/2); 
			break;
			
		case ShapeSizer::eBotLeft:	
			delta = boundsRect.GetBotLeft () - Point (kHandleSize.GetV (), 0);	
			break;
			
		case ShapeSizer::eLeft:		
			delta = boundsRect.GetTopLeft () + Point ((shapeBounds.GetHeight () - kHandleSize.GetV ())/2, 0); 
			break;
			
		default:			
			RequireNotReached ();
	}
	return (littleRect + delta);
}



/*
 ********************************************************************************
 ******************************** ItemSelector **********************************
 ********************************************************************************
 */
ItemSelector::ItemSelector (ViewItem& item) :
	fItem (item)
{
	fHandles = new SelectorShapeRep ();
	SetShape (fHandles);
}

ViewItem&	ItemSelector::GetViewItem () const
{
	return (fItem);
}

Boolean	ItemSelector::TrackPress (const MousePressInfo& mouseInfo)
{
	Require (ItemPallet::GetEditMode () and (GetViewItem ().GetGroup () != Nil));
	ViewItem&	viewItem = GetViewItem ();

	if (viewItem.CanChangeExtent ()) {
		ShapeSizer sizer (
			RegionShape (viewItem.GetDrawnRegion ()), 
			viewItem.GetDrawnRegion ().GetBounds (), 
			*viewItem.GetParentView (), 
			viewItem.GetMinSize (), 
			viewItem.GetMaxSize (),
			fHandles->GetDirection (mouseInfo.fPressAt, GetLocalExtent ()));
		
		sizer.SetTimeOut (0);
		(void) sizer.TrackPress (MousePressInfo (mouseInfo, viewItem.GetParentView ()->TabletToLocal (LocalToTablet (mouseInfo.fPressAt))));		
		SizeItemCommand*	command = new SizeItemCommand (viewItem, "Resize", 
			viewItem.GetOrigin () + sizer.GetNewShapeExtent ().GetOrigin () - sizer.GetOldShapeExtent ().GetOrigin (),
			viewItem.GetSize () + sizer.GetNewShapeExtent ().GetSize () - sizer.GetOldShapeExtent ().GetSize ());
		command->DoIt ();
		PostCommand (command);
		viewItem.DirtyDocument ();
		return (True);
	}
	return (False);
}

void	ItemSelector::Draw (const Region& /*update*/)
{
	Assert (GetViewItem ().GetSelected ());
	Assert (ItemPallet::GetEditMode ());
	if (GetViewItem ().GetGroup () != Nil) {
		Brush	cornerBrush = GetViewItem ().View::GetLive () ? kBlackTile: kGrayTile;
		Brush	constrainBrush = kGrayTile;
		
		Rect	extent = GetLocalExtent ();
		for (ShapeSizer::SizeDirection i = ShapeSizer::eBotRight; i <= ShapeSizer::eRight; i++) {
			switch (i) {
				case ShapeSizer::eTopLeft:	
				case ShapeSizer::eTopRight:	
				case ShapeSizer::eBotRight:	
				case ShapeSizer::eBotLeft:	
					Paint (Rectangle (), fHandles->GetHandle (i, extent), cornerBrush);
					break;
					
				case ShapeSizer::eTop:		
					Paint (Rectangle (), fHandles->GetHandle (i, extent), 
						GetViewItem ().GetConstrainTop () ? constrainBrush : cornerBrush);
					break;
					
				case ShapeSizer::eRight:	
					Paint (Rectangle (), fHandles->GetHandle (i, extent), 
						GetViewItem ().GetConstrainWidth () ? constrainBrush : cornerBrush);
					break;
					
				case ShapeSizer::eBottom:	
					Paint (Rectangle (), fHandles->GetHandle (i, extent), 
						GetViewItem ().GetConstrainHeight () ? constrainBrush : cornerBrush);
					break;
					
				case ShapeSizer::eLeft:		
					Paint (Rectangle (), fHandles->GetHandle (i, extent), 
						GetViewItem ().GetConstrainLeft () ? constrainBrush : cornerBrush);
					break;

				default:
					AssertNotReached ();
			}
		}
	}
}

String	GetBooleanAsText (Boolean b)
{
	static	String	kTrueString  = "True";
	static	String	kFalseString = "False";
	
	if (b) {
		return (kTrueString);
	}
	else {
		return (kFalseString);
	}
}

String	GetJustificationAsText (AbstractTextView::Justification just)
{
	switch (just) {
		case AbstractTextView::eJustLeft:
			return ("AbstractTextView::eJustLeft");

		case AbstractTextView::eJustCenter:
			return ("AbstractTextView::eJustCenter");

		case AbstractTextView::eJustRight:
			return ("AbstractTextView::eJustRight");
			
		case AbstractTextView::eJustFull:
			return ("AbstractTextView::eJustFull");

		default:
			RequireNotReached ();
	}
}

