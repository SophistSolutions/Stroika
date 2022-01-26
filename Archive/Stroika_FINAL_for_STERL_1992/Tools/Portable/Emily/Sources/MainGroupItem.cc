/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MainGroupItem.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: MainGroupItem.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Pallet.hh"
#include	"Shape.hh"

#include	"Dialog.hh"
#include	"PickList.hh"
#include	"Scroller.hh"
#include	"Tracker.hh"

#include	"CommandNumbers.hh"
#include	"EmilyApplication.hh"
#include	"EmilyDocument.hh"
#include	"EmilyWindow.hh"
#include	"MainGroupItem.hh"



/*
 ********************************************************************************
 *********************************** MainGroupItem ******************************
 ********************************************************************************
 */
MainGroupItem::MainGroupItem (EmilyWindow& window):
	GroupItem (GroupItemType::Get (), False),
	fWindow (window),
	fPastePoint (kZeroPoint),
	fFieldCounter (1),
	fOldGrid (kZeroPoint),
	fOldSize (kZeroPoint),
	fCachedPixelMap (Nil),
	fAutoSize (True)
{
	SetScrollSize (Point (250, 250));
	SetMaxVersion (3);
}

MainGroupItem::~MainGroupItem ()
{
	delete fCachedPixelMap;
}

void	MainGroupItem::DoRead_ (class istream& from)
{
	GroupItem::DoRead_ (from);
	
	if (GetVersion () > 2) {
		Boolean autoSize;
		from >> autoSize;
		SetAutoSize (autoSize);
	}

	CommandNumber oldLanguage = EmilyWindow::GetLanguage ();
	CommandNumber oldGUI = EmilyWindow::GetGUI ();
	
	fWindow.SetLanguage (GetBaseLanguage ());
	fWindow.SetGUI (GetBaseGUI ());
	fWindow.SetLanguage (oldLanguage);
	fWindow.SetGUI (oldGUI);
}

void	MainGroupItem::DoWrite_ (class ostream& to, int tabCount) const
{
	GroupItem::DoWrite_ (to, tabCount);
	to << GetAutoSize ();
}

Boolean	MainGroupItem::GetAutoSize () const
{
	return (fAutoSize);
}

void	MainGroupItem::SetAutoSize (Boolean autoSize)
{
	if (GetAutoSize () != autoSize) {
		fAutoSize = autoSize;
		if (fAutoSize) {
			GetGroupItemView ().SetVerticalScrollBar (Scroller::kBuildNoSlider);
			GetGroupItemView ().SetHorizontalScrollBar (Scroller::kBuildNoSlider);
			SetScrollSize (GetSize ());
		}
		else {
			GetGroupItemView ().SetVerticalScrollBar (Scroller::kBuildDefaultSlider);
			GetGroupItemView ().SetHorizontalScrollBar (Scroller::kBuildDefaultSlider);
		}
		InvalidateLayout ();
	}
	Ensure (GetAutoSize () == autoSize);
	ApplyCurrentParams ();
}

String	MainGroupItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = kEmptyString;
	return (kHeaderFileName);
}

void	MainGroupItem::SetSelected_ (Boolean /*selected*/, UpdateMode /*update*/)
{
	AssertNotReached ();	
}

void	MainGroupItem::DoSetupMenus ()
{
	if (this != GetMostSelectedGroup ()) {
		GetMostSelectedGroup ()->DoSetupMenus ();
	}
	GroupItem::DoSetupMenus ();
	EnableCommand (eDeleteCustomization);
	EnableCommand (eReorderFocusItems, Boolean ((not EmilyWindow::GetCustomizeOnly ()) and (GetFocusItems ().GetLength () > 0)));
}

Boolean	MainGroupItem::DoCommand (const CommandSelection& selection)
{
	if (this != GetMostSelectedGroup ()) {
		if (GetMostSelectedGroup ()->DoCommand (selection)) {
			return (True);
		}
	}

	if (selection.GetCommandNumber () == ePaste) {
		/*
		 * Pastes when something is already selected work fine by default. We nuke the old item, 
		 * and paste where it was.
		 * However, when no items are selected we want to paste in the last spot the user clicked. 
		 * If he clicked inside of a subgroup it should paste
		 * within the subgroup, even though that group will not be the "MostSelectedGroup"
		 */
		
		GroupItem* item =  GetGroupContaining (fPastePoint);
		if (item != this) {
			AssertNotNil (item);
			return (item->DoCommand (CommandSelection (ePaste)));
		}
		else {
			return (GroupItem::DoCommand (ePaste));
		}
	}
	else if (GroupItem::DoCommand (selection)) {
		return (True);
	}
	else if (selection.GetCommandNumber () == eDeleteCustomization) {
		DeleteCustomizations ();
		return (True);
	}	
	else if (selection.GetCommandNumber () == eReorderFocusItems) {
		ReorderFocusItems ();
		return (True);
	}
	
	return (False);
}

Boolean		MainGroupItem::TrackPress (const MousePressInfo& mouseInfo)
{
	GroupItem*	groupContaining = GetGroupContaining (LocalToTablet (mouseInfo.fPressAt));
	if (this != groupContaining) {
		return (groupContaining->TrackPress (MousePressInfo (mouseInfo, groupContaining->TabletToLocal (LocalToTablet (mouseInfo.fPressAt)))));
	}

	Point delta = Abs (GetScrollSize () - mouseInfo.fPressAt);
	if ((delta.GetV () <= 1) or (delta.GetH () <= 1)) {
		ShapeSizer sizer = 
			ShapeSizer (Rectangle (), Rect (kZeroPoint, GetScrollSize ()), GetGroupItemView (), GetMinSize (), GetMaxSize ());
		sizer.SetTimeOut (0);

		if ((delta.GetV () <= 1) and (delta.GetH () <= 1)) {
			sizer.SetSizeDirection (ShapeSizer::eBotRight);
		}
		else if (delta.GetV () <= 1) {
			sizer.SetSizeDirection (ShapeSizer::eBottom);
		}
		else {
			sizer.SetSizeDirection (ShapeSizer::eRight);
		}
		
		(void)sizer.TrackPress (mouseInfo);
		SetScrollSize (sizer.GetNewShapeExtent ().GetSize ());
		ApplyCurrentParams ();
		DirtyDocument ();
		
		return (True);
	}
	
	return (GroupItem::TrackPress (mouseInfo));
}

void	MainGroupItem::Layout ()
{
	ItemParams* params = FindParams (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	AssertNotNil (params);	
	params->fSize = GetSize ();
	params->fOrigin = GetOrigin ();

	GroupItem::Layout ();
}

void	MainGroupItem::Draw (const Region& update)
{
	if (EmilyApplication::Get ().GetGridVisible ()) {
		Point	grid = EmilyApplication::Get ().GetGrid ();
		Point	topLeft = RoundBy (update.GetBounds ().GetTopLeft (), grid);
		Point	botRight = update.GetBounds ().GetBotRight ();
		
		if ((fOldGrid != grid) or (fOldSize != GetSize ()) or (fCachedPixelMap == Nil)) {
			if (fCachedPixelMap != Nil) {
				delete fCachedPixelMap;
				fCachedPixelMap = Nil;
			}
			fCachedPixelMap = new PixelMap (Rect (kZeroPoint, Point (1, GetSize ().GetH ())));
			for (register Coordinate whereH = 0; whereH < GetSize ().GetH (); whereH++) {
				fCachedPixelMap->SetPixel (Point (0, whereH), Boolean ((whereH % grid.GetH ()) == 0));
			}
			fOldGrid = grid;
			fOldSize = GetSize ();
		}
		AssertNotNil (fCachedPixelMap);
	
		Rect r = fCachedPixelMap->GetBounds ();
		for (register Coordinate whereV = topLeft.GetV (); whereV < botRight.GetV (); whereV += grid.GetV ()) {
			r.SetOrigin (Point (whereV, 0));
			BitBlit (*fCachedPixelMap, fCachedPixelMap->GetBounds (), r);
		}
	}
	if (not (GetScrollSize () >= GetSize ())) {
		Point	p = GetScrollSize () - Point (1, 1);
		OutLine (Line (Point (p.GetV (), 0), Point (p.GetV (), p.GetH ())));
		OutLine (Line (Point (0, p.GetH ()), Point (p.GetV (), p.GetH ())));
	}
	GroupItem::Draw (update);
}

Point	MainGroupItem::CalcDefaultSize_ (const Point& defaultSize) const
{
	if (defaultSize == kZeroPoint) {
		return (Point (300, 300));
	}
	return (defaultSize);
}

GroupItem*	MainGroupItem::GetMostSelectedGroup ()
{
	GroupItem*	mostSelected = GroupItem::GetMostSelectedGroup ();
	if (mostSelected != Nil) {
		return (mostSelected);
	}
	return (this);
}

GroupItem*	MainGroupItem::GetGroupContaining (const Point& point)
{
	GroupItem*	groupContaining = GroupItem::GetGroupContaining (point);
	EnsureNotNil (groupContaining);
	return (groupContaining);
}

Boolean	MainGroupItem::CalcFramed () const
{
	return (False);
}

void	MainGroupItem::DirtyDocument ()
{
	PostCommand (new DocumentDirtier (fWindow.GetDocument ()));
}

void	MainGroupItem::SetPastePoint (const Point& point)
{
	fPastePoint = point;
}

Point	MainGroupItem::GetPastePoint () const
{
	return (fPastePoint);
}

void	MainGroupItem::ResetFieldCounter ()
{
	fFieldCounter = 1;
}

String	MainGroupItem::CalcFieldName ()
{
	static	const	String	kFieldName = String (String::eReadOnly, "fField");
	
	OStringStream	buf;
	buf << kFieldName << fFieldCounter++;
	return (String (buf));
}

void	MainGroupItem::ResetCustomizeOnly ()
{
	CollectionSize count = 0;
	ForEach (ItemParamsPtr, it, MakeParamsIterator ()) {
		count++;
	}
	fWindow.SetCustomizeOnly (Boolean (count > 1));
}

void	MainGroupItem::GUIChanged (CommandNumber oldGUI, CommandNumber newGUI)
{
	GroupItem::GUIChanged (oldGUI, newGUI);
	ResetCustomizeOnly ();
}

void	MainGroupItem::LanguageChanged (CommandNumber oldLanguage, CommandNumber newLanguage)
{
InvalidateLayout ();
	GroupItem::LanguageChanged (oldLanguage, newLanguage);
	ResetCustomizeOnly ();
}

void	MainGroupItem::WriteInitializer (class ostream& to, int tabCount, CommandNumber gui)
{
	WriteFont (to, 1, GetFont (), Nil, "SetFont", kEmptyString, True);
	Point p = GetScrollSize (GetBaseLanguage (), gui);
	to << tab << "SetSize (Point (" << p.GetV () << ", " << p.GetH () << "), eNoUpdate);" << newline << newline;

	GroupItem::WriteInitializer (to, tabCount, gui);
}

// given public scope
CommandNumber	MainGroupItem::GetBaseGUI () const
{
	return (GroupItem::GetBaseGUI ());
}

// given public scope
CommandNumber	MainGroupItem::GetBaseLanguage () const
{
	return (GroupItem::GetBaseLanguage ());
}
