/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditor.cc,v 1.8 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapEditor.cc,v $
 *		Revision 1.8  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/14  20:01:25  lewis
 *		Use 0 instead of kZeroSeconds.
 *
 *		Revision 1.4  1992/07/03  04:08:18  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.3  1992/06/26  00:51:05  lewis
 *		Fix mismerge.
 *
 *		Revision 1.18  1992/06/09  14:29:45  lewis
 *		Comment out building of tablet for UNIX - broken - only do for mac. Fix soon.
 *
 *		Revision 1.17  92/06/09  16:09:35  16:09:35  sterling (Sterling Wight)
 *		speed tweaks, modified color map entry size change algorithm
 *		
 *		Revision 1.16  92/05/23  02:10:20  02:10:20  lewis (Lewis Pringle)
 *		Include BlockAllocated and fix for using templates.
 *		
 *		Revision 1.15  92/05/20  00:26:55  00:26:55  sterling (Sterling Wight)
 *		uses ColorPicker
 *		
 *		Revision 1.14  92/05/13  00:38:34  00:38:34  lewis (Lewis Pringle)
 *		Lots of cleanups, including changing algorithm for rebuilding CLUT to iterator over the pixelmap, and find all colors that are USED in
 *		the pixelmap, and try to be sure they are used. Drop the requirement that
 *		black and white are in the CLUT, and that the occupy particular positions - if that is required by the MacGDI, we will hack it to be so at
 *		a low level.
 *		
 *		Revision 1.13  92/05/11  21:34:19  21:34:19  lewis (Lewis Pringle)
 *		Change Depth to Colors popup, and allow (if only briefly) all the differnt depths
 *		from 1-8 - we should support them all eventually - disable broken ones before shipping - or
 *		better yet, fix em!!!
 *		
 *		Revision 1.12  92/05/01  01:34:01  01:34:01  lewis (Lewis Pringle)
 *		Set Depth of optionmenu depending on dpeth of PixelMap - in CTOR and PixelMapXXXEditor??::SetPixelMap().
 *		Get rid of old ::Draw code for pixelmaps.
 *		Under X, hide fAutoScale button since uses stuff NYI in Tablet for XGDI.
 *		Adjust positioning of buttons to look a little better - really we need the calcdefaultsizes for
 *		motif widgets to do a better job.
 *		
 *		Revision 1.11  92/04/30  17:13:11  17:13:11  lewis (Lewis Pringle)
 *		Revert fix to PixelItemView::Draw () where we realy need is to call bitblit - not look at
 *		issue of fScale ... Sterl accidentally blew it away when he did isnhis update.
 *		
 *		Revision 1.6  92/04/17  19:51:47  19:51:47  lewis (Lewis Pringle)
 *		Hack to use fixed scale.
 *		
 *		Revision 1.5  92/04/17  17:08:48  17:08:48  lewis (Lewis Pringle)
 *		Futz with layout to look somewhat better under motif, and disable half implemented features.
 *		Hardwire scale to 4 - faster. Fix to whatever you want later sterl.
 *		
 *		Revision 1.3  92/04/09  23:09:43  23:09:43  sterling (Sterling Wight)
 *		CalcDefaultSize and slightly modified Layout
 *		
 *		Revision 1.2  92/04/02  11:44:57  11:44:57  lewis (Lewis Pringle)
 *		Significant improvements - including a new tracker to allow drawing while mouse down, and when done, refresh other
 *		pixelmap view.
 *		
 *
 *
 */

#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"Format.hh"
#include	"StreamUtils.hh"

#include	"BitBlitter.hh"
#include	"Shape.hh"

#include	"CheckBox.hh"
#include	"ColorLookupTable.hh"
#include	"Command.hh"
#include	"Menu.hh"
#include	"Pallet.hh"
#include	"OptionMenu.hh"
#include	"PushButton.hh"
#include	"Tracker.hh"

#include	"NumberText.hh"
#include	"PixelMapEditor.hh"



static	UInt32				IPow (UInt32 n, UInt8 power);
static	Int16				FindBestCubeRoot (Int16 target);
static	ColorLookupTable	RebuildColorTable (UInt8 newDepth, const PixelMap& oldPixelMap);
static	void				BuildStandardColorTable ();				

static	ColorLookupTable*	sStandardColorTable = Nil;




/*
 * Update pixelmap as we go along. Only real trouble with this, is that we are not undoable then.
 */
class	PixelMapTracker : public Tracker {
	public:
		PixelMapTracker (PixEditView& pev, Color color);

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

		override	void	TrackConstrain (TrackPhase phase, const Point& anchor, const Point& previous, Point& next);
		override	void	TrackFeedback (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);
		override	Boolean	TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);

	private:
		PixEditView&	fPEV;
		Color			fNewPixelValue;
		Tile			fColorTile;
};



class	PixelMapSizeInfo : public View, public ButtonController, public TextController, public FocusOwner {
	public:
		PixelMapSizeInfo (PixelMapEditor& editor);
		~PixelMapSizeInfo ();

		nonvirtual	Point	GetPixelMapSize () const;
		nonvirtual	void	SetPixelMapSize (const Point& newSize, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	Boolean	GetAutoScale () const;
		nonvirtual	void	SetAutoScale (Boolean autoScale, UpdateMode update = eDelayedUpdate);
	
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize = kZeroPoint) const;
		override	void	ButtonPressed (AbstractButton* button);
		override	void	TextChanged (TextEditBase* item);
		override	void	Layout ();

	private:
		NumberText		fHeight;
		NumberText		fWidth;
		TextView		fHeightLabel;
		TextView		fWidthLabel;
		CheckBox		fAutoScale;
		PushButton		fApply;
		PixelMapEditor&	fEditor;
};


class PixelColorInfo;
class	ColorButton : public View {
	public:
		ColorButton (PixelColorInfo& owner, Color color);
		
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		
		nonvirtual	Color	GetColor () const;
		nonvirtual	void	SetColor (Color color, UpdateMode update = eDelayedUpdate);
	
		nonvirtual	void*	operator new (size_t n);
		nonvirtual	void	operator delete (void* p);

	protected:
		override	void	Draw (const Region& update);
	
	private:
		PixelColorInfo&	fOwner;
		Color			fColor;
		Pen				fPen;
};

class	PixelColorInfo : public View, public ButtonController {
	public:
		PixelColorInfo (PixelMapEditor& editor);
		~PixelColorInfo ();
		
		nonvirtual	UInt8	GetDepth () const;
		nonvirtual	void	SetDepth (UInt8 depth);
		
		nonvirtual	Color	GetSelectedColor () const;
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize = kZeroPoint) const;
		override	void	ButtonPressed (AbstractButton* button);

	private:
		nonvirtual	UInt8			SelectionToDepth (CollectionSize selection) const;
		nonvirtual	CollectionSize	DepthToSelection (UInt8 depth) const;
		nonvirtual	void			ResetDepth ();
		
		OptionMenuButton	fDepth;
		ColorButton			fSelectedColor;
		PixelMapEditor&		fEditor;
		
		friend	class	ColorButton;
};







/*
 ********************************************************************************
 ********************************* PixelMapEditor *******************************
 ********************************************************************************
 */

PixelMapEditor::PixelMapEditor ():
	View (),
	fSmallPixelMapView (),
	fBigPixelMapView (),
	fSizeInfo (Nil),
	fColorInfo (Nil)
{
	AddSubView (&fSmallPixelMapView);
	AddSubView (&fBigPixelMapView);
	
	Point	pmSize = fSmallPixelMapView.GetPixelMap ().GetBounds ().GetSize ();

	AddSubView (fSizeInfo = new PixelMapSizeInfo (*this));
	fSizeInfo->SetPixelMapSize (pmSize, eNoUpdate);
	AddFocus (fSizeInfo);
	
	AddSubView (fColorInfo = new PixelColorInfo (*this));
	fColorInfo->SetDepth (1);
fBigPixelMapView.SetScale (8);
}

PixelMapEditor::~PixelMapEditor ()
{
	RemoveFocus (fSizeInfo);
	
	RemoveSubView (&fSmallPixelMapView);
	RemoveSubView (&fBigPixelMapView);
	RemoveSubView (fSizeInfo);
	RemoveSubView (fColorInfo);
	
	delete fSizeInfo;
	delete fColorInfo;
}

const	PixelMap&	PixelMapEditor::GetPixelMap () const
{
	// either one fine - both should be the same...
	return (fSmallPixelMapView.GetPixelMap ());
}

void	PixelMapEditor::SetPixelMap (const PixelMap& pixelMap)
{
	fSmallPixelMapView.SetPixelMap (pixelMap);
	fBigPixelMapView.SetPixelMap (pixelMap);

	Point	pmSize = pixelMap.GetBounds ().GetSize ();
	/*fBigPixelMapView.SetScale (64 / pmSize.GetH ());*/
	fSizeInfo->SetPixelMapSize (pmSize);
	fColorInfo->SetDepth (pixelMap.GetDepth ());
}

Boolean	PixelMapEditor::TrackPress (const MousePressInfo& mouseInfo)
{
	/*
	 * If we get a click in one PixEditView or the other, update the other and return true.
	 * That is because the click probably resulted in some change.
	 *
	 */
	if (fSmallPixelMapView.Contains (mouseInfo.fPressAt)) {
		PixelMapTracker	pmTracker (fSmallPixelMapView, fColorInfo->GetSelectedColor ());
		pmTracker.TrackPress (MousePressInfo (mouseInfo,
				fSmallPixelMapView.EnclosureToLocal (mouseInfo.fPressAt)));
				
		fBigPixelMapView.SetPixelMap (fSmallPixelMapView.GetPixelMap ());
		return (True);
	}
	if (fBigPixelMapView.Contains (mouseInfo.fPressAt)) {
		PixelMapTracker	pmTracker (fBigPixelMapView, fColorInfo->GetSelectedColor ());
		pmTracker.TrackPress (MousePressInfo (mouseInfo,
				fBigPixelMapView.EnclosureToLocal (mouseInfo.fPressAt)));
				
		fSmallPixelMapView.SetPixelMap (fBigPixelMapView.GetPixelMap ());
		return (True);
	}
	return (View::TrackPress (mouseInfo));
}

Point	PixelMapEditor::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	smallViewSize = fSmallPixelMapView.CalcDefaultSize ();
	Point	bigViewSize = fBigPixelMapView.CalcDefaultSize ();
	Point	infoSize = fSizeInfo->CalcDefaultSize ();
	Point	cInfoSize = fColorInfo->CalcDefaultSize ();
	const	Coordinate	kGap = 10;
	
	Coordinate	newWidth = Max (infoSize.GetH (), cInfoSize.GetH ()) + kGap + bigViewSize.GetH ();
	Coordinate	newHeight = Max (
		bigViewSize.GetV (), 
		smallViewSize.GetV () + kGap + infoSize.GetV () + kGap + cInfoSize.GetV ());

	return (Point (newHeight + 10, newWidth));
}

void	PixelMapEditor::Layout ()
{
	/*fBigPixelMapView.SetScale (64 / fSmallPixelMapView.GetPixelMap ().GetBounds ().GetSize ().GetH ());*/

	fSmallPixelMapView.SetOrigin (Point (1, 1));
	fSmallPixelMapView.SetSize (fSmallPixelMapView.CalcDefaultSize ());
	fBigPixelMapView.SetSize (fBigPixelMapView.CalcDefaultSize ());

	AssertNotNil (fSizeInfo);
	fSizeInfo->SetSize (fSizeInfo->CalcDefaultSize ());
	fSizeInfo->SetOrigin (Point (fSmallPixelMapView.GetExtent ().GetBottom () + 10, 1));

	AssertNotNil (fColorInfo);
	fColorInfo->SetSize (fColorInfo->CalcDefaultSize ());
	fColorInfo->SetOrigin (Point (fSizeInfo->GetExtent ().GetBottom () + 10, 1));

// assumed sterl adjusted scale... hack for fixed scale - LGP april 17, 1992
//	fBigPixelMapView.SetOrigin (Point (1, GetSize ().GetH () - fBigPixelMapView.GetSize ().GetH () -1));
	fBigPixelMapView.SetOrigin (Point (10, fColorInfo->GetExtent ().GetRight () + 10));
	
	View::Layout ();
}

void	PixelMapEditor::ResizePixelMaps ()
{
	UInt8	depth = fColorInfo->GetDepth ();

	Rect		pmBounds (kZeroPoint, fSizeInfo->GetPixelMapSize ());
	PixelMap	pm (PixelMap::eChunky, depth, pmBounds);
	PixelMap	oldPM = fSmallPixelMapView.GetPixelMap ();
	Rect		destBounds (kZeroRect);
	pm.SetColorLookupTable (oldPM.GetColorLookupTable ());
	
	if (fSizeInfo->GetAutoScale ()) {
		destBounds = pmBounds;
	}
	else {
		Tablet t (&pm);
		t.Paint (Rectangle (), pmBounds, Brush (kWhiteTile));
		destBounds = Rect (kZeroPoint, oldPM.GetBounds ().GetSize ());
	}

	BitBlitter ().Apply (oldPM, oldPM.GetBounds (), pm, destBounds, BitBlitter::kCopy);

	fSmallPixelMapView.SetPixelMap (pm);
	fBigPixelMapView.SetPixelMap (pm);
	
	InvalidateLayout ();
}

void	PixelMapEditor::ResetPixelMapDepth ()
{
	AssertNotNil (fColorInfo);
	
	UInt8	depth = fColorInfo->GetDepth ();

	Rect		pmBounds (kZeroPoint, fSizeInfo->GetPixelMapSize ());
	PixelMap	pm (PixelMap::eChunky, depth, pmBounds);
	PixelMap	oldPM = fSmallPixelMapView.GetPixelMap ();

	pm.SetColorLookupTable (RebuildColorTable (depth, oldPM));
	BitBlitter ().Apply (oldPM, oldPM.GetBounds (), pm, pmBounds, BitBlitter::kCopy);


	fSmallPixelMapView.SetPixelMap (pm);
	fBigPixelMapView.SetPixelMap (pm);

	Assert (pm.GetDepth () == depth);
	Assert (fSmallPixelMapView.GetPixelMap ().GetDepth () == depth);
	Assert (fBigPixelMapView.GetPixelMap ().GetDepth () == depth);

	InvalidateLayout ();
}











/*
 ********************************************************************************
 *********************************** PixEditView ********************************
 ********************************************************************************
 */

PixEditView::PixEditView ():
	View (),
	fPixelMap (Rect (kZeroPoint, Point (8, 8))),
	fScale (1)
{
#if		qMacGDI
// currently broken under UNIX - sorry... LGP June 9, 1992
	Tablet t (&fPixelMap);
	t.Paint (Rectangle (), Rect (kZeroPoint, Point (8, 8)), Brush (kWhiteTile));
#endif
}

const	PixelMap&	PixEditView::GetPixelMap () const
{
	return (fPixelMap);
}

void	PixEditView::SetPixelMap (const PixelMap& pixelMap, Panel::UpdateMode updateMode)
{
	fPixelMap = pixelMap;
	// not sure right - does this preseve bits???
	fPixelMap.SetBounds (Rect (kZeroPoint, pixelMap.GetBounds ().GetSize ())); // normalize
	Refresh (updateMode);
}

UInt8	PixEditView::GetScale () const
{
	return (fScale);
}

void	PixEditView::SetScale (UInt8 scale, Panel::UpdateMode updateMode)
{
	if (fScale != scale) {
		fScale = scale;
		Refresh (updateMode);
	}
}

Rect	PixEditView::GetPixelRect (const Point& whichPixel) const
{
	Require (GetPixelMap ().Contains (whichPixel));
	return (Rect (whichPixel * fScale, Point (fScale, fScale)));
}

Boolean	PixEditView::TrackPress (const MousePressInfo& mouseInfo)
{
	PixelMapTracker	pmTracker (*this, kBlackColor);
	return (pmTracker.TrackPress (mouseInfo));
}

Point	PixEditView::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	return (GetPixelMap ().GetBounds ().GetSize () * GetScale ());
}

void	PixEditView::Draw (const Region& update)
{
	const	PixelMap&	pm			=	GetPixelMap ();
	Rect				pmBounds	=	pm.GetBounds ();
	BitBlit (pm,
			 Rect (kZeroPoint, pmBounds.GetSize ()),
			 Rect (kZeroPoint, pmBounds.GetSize () * fScale),
			 eCopyTMode);
	View::Draw (update);
}










/*
 ********************************************************************************
 *********************************** PixelMapTracker ****************************
 ********************************************************************************
 */

PixelMapTracker::PixelMapTracker (PixEditView& pev, Color color):
	Tracker (pev, *pev.GetTablet (), pev.LocalToTablet (kZeroPoint)),
	fPEV (pev),
	fNewPixelValue (color),
	fColorTile (PalletManager::Get ().MakeTileFromColor (color))
{
	SetTimeOut (0);
	SetHysteresis (kZeroPoint);
}

Boolean	PixelMapTracker::TrackPress (const MousePressInfo& mouseInfo)
{
	Point	pixelPoint	=	mouseInfo.fPressAt / fPEV.fScale;
	if (fPEV.fPixelMap.Contains (pixelPoint)) {
		return (Tracker::TrackPress (mouseInfo));
	}
	else {
		return (False);
	}
}

void	PixelMapTracker::TrackConstrain (Tracker::TrackPhase /*phase*/, const Point& /*anchor*/, const Point& /*previous*/,
										 Point& /*next*/)
{
	return;		// Done in TrackResult...
}

void	PixelMapTracker::TrackFeedback (Tracker::TrackPhase /*phase*/, const Point& /*anchor*/, const Point& /*previous*/,
										const Point& /*next*/)
{
	return;		// Done in TrackResult...
}

Boolean	PixelMapTracker::TrackResult (Tracker::TrackPhase /*phase*/, const Point& /*anchor*/, const Point& /*previous*/,
									  const Point& next)
{
	Point	pixelPoint	=	next / fPEV.fScale;

	if (fPEV.fPixelMap.Contains (pixelPoint) and (fPEV.fPixelMap.GetColor (pixelPoint) != fNewPixelValue)) {
		fPEV.fPixelMap.SetColor (pixelPoint, fNewPixelValue);
		Rect r = fPEV.GetPixelRect (pixelPoint);
		Paint (Rectangle (), r, fColorTile);
	}

	return (False);
}




/*
 ********************************************************************************
 ******************************** PixelMapSizeInfo ******************************
 ********************************************************************************
 */
PixelMapSizeInfo::PixelMapSizeInfo (PixelMapEditor& editor) :
	fHeight (Nil, 4, 4, 64),
	fWidth (Nil, 4, 4, 64),
	fHeightLabel ("V"),
	fWidthLabel ("H"),
	fAutoScale ("Scale", Nil),
	fApply ("Apply", Nil),
	fEditor (editor)
{
	fAutoScale.SetOn (Toggle::kOn, eNoUpdate);
	fApply.SetEnabled (False, eNoUpdate);

#if		qXGDI
// currently buggy under X
	fAutoScale.SetVisible (False, eNoUpdate);
#endif

	AddSubView (&fHeight);
	AddSubView (&fWidth);
	AddSubView (&fHeightLabel);
	AddSubView (&fWidthLabel);
	AddSubView (&fAutoScale);
	AddSubView (&fApply);
	
	AddFocus (&fHeight);
	AddFocus (&fWidth);
	
	fHeight.SetController (this);
	fWidth.SetController (this);
	fAutoScale.SetController (this);
	fApply.SetController (this);
}
					
PixelMapSizeInfo::~PixelMapSizeInfo ()
{
	RemoveFocus (&fHeight);
	RemoveFocus (&fWidth);
	
	RemoveSubView (&fHeight);
	RemoveSubView (&fWidth);
	RemoveSubView (&fHeightLabel);
	RemoveSubView (&fWidthLabel);
	RemoveSubView (&fAutoScale);
	RemoveSubView (&fApply);
}

Point	PixelMapSizeInfo::CalcDefaultSize_ (const Point& /* defaultSize */) const
{
	const Coordinate	marginH = 4;	// should convert to use Bordered
	const Coordinate	marginV = 1;	// should convert to use Bordered

	Point	textSize = fHeight.CalcDefaultSize (kZeroPoint);
	textSize.SetH (40);
	Point	autoScaleSize = fAutoScale.CalcDefaultSize (kZeroPoint);
	Point	applySize = fApply.CalcDefaultSize (kZeroPoint);

	return (Point (
		marginV * 4 + textSize.GetV () + autoScaleSize.GetV () + applySize.GetV () + 10,
		marginH * 3 + textSize.GetH () * 2 + 14));
}

void	PixelMapSizeInfo::Layout ()
{
	const Coordinate	marginH = 4;	// should convert to use Bordered
	const Coordinate	marginV = 4;	// should convert to use Bordered

	const Coordinate	textV = fHeight.CalcDefaultSize (kZeroPoint).GetV ();
	const Coordinate	textH = (GetSize ().GetH () - marginH*3 - 24)/2;

	fHeightLabel.SetOrigin (Point (marginV, marginH));
	fHeightLabel.SetSize (Point (textV, 10));	
	fHeight.SetSize (Point (textV, textH));
	fHeight.SetOrigin (Point (marginV, fHeightLabel.GetExtent ().GetRight () + 2));

	fWidthLabel.SetOrigin (Point (marginV, fHeight.GetExtent ().GetRight () + marginH));
	fWidthLabel.SetSize (Point (textV, 10));
	fWidth.SetSize (fHeight.GetSize ());
	fWidth.SetOrigin (Point (marginV, fWidthLabel.GetExtent ().GetRight () + 2));

	fAutoScale.SetSize (fAutoScale.CalcDefaultSize (kZeroPoint));
	fAutoScale.SetOrigin (Point (
		fHeight.GetExtent ().GetBottom () + marginV, 
		(GetSize ().GetH () - fAutoScale.GetSize ().GetH ())/2));

	fApply.SetSize (fApply.CalcDefaultSize (kZeroPoint));
	fApply.SetOrigin (Point (
		fAutoScale.GetExtent ().GetBottom () + marginV, 
		(GetSize ().GetH () - fApply.GetSize ().GetH ())/2));

	View::Layout ();
}

Point	PixelMapSizeInfo::GetPixelMapSize () const
{
	Require (fHeight.GetValue () > 0);
	Require (fWidth.GetValue () > 0);
	Require (fHeight.GetValue () <= 100);	// we maxed out somewhere around there - sanity check..
	Require (fWidth.GetValue () <= 100);	// we maxed out somewhere around there - sanity check..
	return (Point (Coordinate (fHeight.GetValue ()), Coordinate (fWidth.GetValue ())));
}

void	PixelMapSizeInfo::SetPixelMapSize (const Point& newSize, Panel::UpdateMode update)
{
	if (GetPixelMapSize () != newSize) {
		fHeight.SetValue (newSize.GetV (), update);
		fWidth.SetValue (newSize.GetH (), update);
//		fApply.SetEnabled (True);
	}
	Ensure (GetPixelMapSize () == newSize);
}
		
Boolean	PixelMapSizeInfo::GetAutoScale () const
{
	return (fAutoScale.GetOn ());
}

void	PixelMapSizeInfo::SetAutoScale (Boolean autoScale, Panel::UpdateMode update)
{
	if (GetAutoScale () != autoScale) {
		fAutoScale.SetOn (autoScale, update);
	}
	Ensure (GetAutoScale () == autoScale);
}
	
void	PixelMapSizeInfo::ButtonPressed (AbstractButton* button)
{
	if (button == &fApply) {
		fEditor.ResizePixelMaps ();
		fApply.SetEnabled (False);
	}
}

void	PixelMapSizeInfo::TextChanged (TextEditBase* /*item*/)
{
	fApply.SetEnabled (True);
}




/*
 ********************************************************************************
 ********************************* PixelColorInfo *******************************
 ********************************************************************************
 */

PixelColorInfo::PixelColorInfo (PixelMapEditor& editor) :
	fDepth ("Colors:"),
	fSelectedColor (*this, kBlackColor),
	fEditor (editor)
{
	fDepth.AppendItem ("2");
	fDepth.AppendItem ("4");
	fDepth.AppendItem ("8");
	fDepth.AppendItem ("16");
	fDepth.AppendItem ("32");
	fDepth.AppendItem ("64");
	fDepth.AppendItem ("128");
	fDepth.AppendItem ("256");
// for now, call setcurrentitem instead of setdepth since setdepth calls GetDepth() and currentitem is bogus !!!
	fDepth.SetCurrentItem (1);
//	SetDepth (1);
	fDepth.SetController (this);	
	AddSubView (&fDepth);
	AddSubView (&fSelectedColor);

	fDepth.SetSize (Point (30, 150), eNoUpdate);
	fSelectedColor.SetOrigin (Point (35, 0), eNoUpdate);
	fSelectedColor.SetSize (Point (20, 160), eNoUpdate);

	ResetDepth ();
}

PixelColorInfo::~PixelColorInfo ()
{
	RemoveSubView (&fDepth);
	RemoveSubView (&fSelectedColor);

	ForEach (ViewPtr, it, MakeSubViewIterator ()) {
		View* current = it.Current ();
		RemoveSubView (current);
		delete current;
	}
}

UInt8	PixelColorInfo::GetDepth () const
{
	return (SelectionToDepth (fDepth.GetCurrentItem ()));
}

void	PixelColorInfo::SetDepth (UInt8 depth)
{
	if (depth != GetDepth ()) {
		fDepth.SetCurrentItem (DepthToSelection (depth));
		ResetDepth ();
	}
}

Color	PixelColorInfo::GetSelectedColor () const
{
	return (fSelectedColor.GetColor ());
}

UInt8	PixelColorInfo::SelectionToDepth (CollectionSize selection) const
{
	switch (strtoul (fDepth.GetItem (selection))) {
		case	2:		return (1);
		case	4:		return (2);
		case	8:		return (3);
		case	16:		return (4);
		case	32:		return (5);
		case	64:		return (6);
		case	128:	return (7);
		case	256:	return (8);
		default:		AssertNotReached ();
	}
}

CollectionSize	PixelColorInfo::DepthToSelection (UInt8 depth) const
{
	String	sToSelect = ultostring (1 << depth);

#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEachT (SequenceIterator, String, it, fDepth.GetItems ().operator SequenceIterator(String)* ()) {
#else
	ForEachT (SequenceIterator, String, it, fDepth.GetItems ()) {
#endif
		if (it.Current () == sToSelect) {
			return (it.CurrentIndex ());
		}
	}
	AssertNotReached ();	// unsupported depth!!!
}
		
Point	PixelColorInfo::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	return (Point (240, 160));
}

void	PixelColorInfo::ResetDepth ()
{
	ForEach (ViewPtr, it, MakeSubViewIterator ()) {
		View* current = it.Current ();
		if ((current != &fDepth) and (current != &fSelectedColor)) {
			RemoveSubView (current);
			delete current;
		}
	}
	Point	buttonSize = Point (160, 160);
	const ColorLookupTable&	colorTable = fEditor.GetPixelMap ().GetColorLookupTable ();
	CollectionSize	buttonsNeeded = colorTable.GetEntryCount ();
	buttonSize /= Min ((long double) (buttonsNeeded), (long double) (16));
	buttonSize.SetV (Min (buttonSize.GetV (), Coordinate (20)));
	buttonSize.SetH (Min (buttonSize.GetH (), Coordinate (80)));
	Point	kButtonOriginStart = fSelectedColor.GetExtent ().GetBotLeft () + Point (5, 0);
	Coordinate	maxRight = fSelectedColor.GetExtent ().GetRight ();
	Point	buttonOrigin = kButtonOriginStart;
	for (CollectionSize i = 1; i <= buttonsNeeded; i++) {
		ColorButton* button = new ColorButton (*this, colorTable[i]);
		button->SetOrigin (buttonOrigin, eNoUpdate);
		button->SetSize (buttonSize, eNoUpdate);
		AddSubView (button);
		buttonOrigin += Point (0, buttonSize.GetH ());
		if (buttonOrigin.GetH () >= maxRight) {
			buttonOrigin = Point (buttonOrigin.GetV () + buttonSize.GetV (), kButtonOriginStart.GetH ());
		}
	}
	fSelectedColor.SetColor (colorTable[1]);
	Refresh ();
}

void	PixelColorInfo::ButtonPressed (AbstractButton* button)
{
	if (button == &fDepth) {
		fEditor.ResetPixelMapDepth ();
		ResetDepth ();
	}
}



/*
 ********************************************************************************
 ********************************* ColorButton **********************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (ColorButton);
BlockAllocatedImplement (ColorButton);
#endif	/*!qRealTemplatesAvailable*/

ColorButton::ColorButton (PixelColorInfo& owner, Color color):
	View (),
	fOwner (owner),
	fColor (color),
	fPen (PalletManager::Get ().MakeTileFromColor (fColor))
{
}

void*	ColorButton::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<ColorButton>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(ColorButton)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	ColorButton::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<ColorButton>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(ColorButton)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}

#include "ColorPicker.hh"
#include "Dialog.hh"

Boolean	ColorButton::TrackPress (const MousePressInfo& mouseInfo)
{
	if (mouseInfo.IsOpenSelection ()) {
		ColorPicker picker = ColorPicker (fColor);
		picker.SetColor (GetColor (), eNoUpdate);
		Dialog d = Dialog (&picker, &picker, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
		if (d.Pose ()) {
			SetColor (picker.GetColor ());
		}
	}
	
	fOwner.fSelectedColor.SetColor (GetColor ());
	return (True);
}

void	ColorButton::Draw (const Region& /*update*/)
{
	Rectangle r;
	Rect extent = GetLocalExtent ();
	Paint (r, extent, fPen);
	OutLine (r, extent);
}

Color	ColorButton::GetColor () const
{
	return (fColor);
}

void	ColorButton::SetColor (Color color, Panel::UpdateMode update)
{
	if (GetColor () != color) {
		fColor = color;
		fPen = Pen (PalletManager::Get ().MakeTileFromColor (fColor));
		Refresh (GetLocalExtent ().InsetBy (Point (1, 1)), update);
	}
	Ensure (GetColor () == color);
}






/*
 ********************************************************************************
 ************************************** IPow ************************************
 ********************************************************************************
 */

static	UInt32	IPow (UInt32 n, UInt8 power)
{
	// Sloppy implementation - slow and ignores overflow (though overflow ignore maybe OK)?
	UInt32	result = 1;
	for (int i = 1; i <= power; i++) {
		result *= n;
	}
	return (result);
}





/*
 ********************************************************************************
 ********************************* FindBestCubeRoot *****************************
 ********************************************************************************
 */

static	Int16	FindBestCubeRoot (Int16 target)
{
	// finds the largest cube root whose cube is smaller than the target number
	// pretty dumb implementation
	for (int i = 1; i <= 40; i++) {
		Int16	spaceUsed = i * i * i;
		if (spaceUsed > target) {
			return (i-1);
		}
	}
	AssertNotReached ();	return (0);
}




/*
 ********************************************************************************
 ********************************* RebuildColorTable ****************************
 ********************************************************************************
 */
static	ColorLookupTable	RebuildColorTable (UInt8 newDepth, const PixelMap& oldPixelMap)
{
	Require (newDepth <= 8);	// should never build huge tables -- use direct colors instead

	Int16	tableSize = IPow (2, newDepth);
	Assert (tableSize <= 256);	// we demanded a depth <= 8 earlier

	ColorLookupTable	table;
	table.SetEntryCount (tableSize);
	const ColorLookupTable&	oldTable = oldPixelMap.GetColorLookupTable ();

	ColorIndex	colorsUsed	=	0;
	if (tableSize >= oldTable.GetEntryCount ()) {
		int	oldTableSize = oldTable.GetEntryCount ();
		for (colorsUsed = 1; colorsUsed <= oldTableSize; colorsUsed++) {
			table.SetEntry (colorsUsed, oldTable[colorsUsed]);
		}
	}
	else {
		/*
		 * First see what colors were used in the old pixelmap, and use these first if possible...
		 */
	
		for (Coordinate row = oldPixelMap.GetBounds ().GetTop (); row < oldPixelMap.GetBounds ().GetBottom (); row++) {
			for (Coordinate col = oldPixelMap.GetBounds ().GetLeft (); col < oldPixelMap.GetBounds ().GetRight (); col++) {
				Color		c			=	oldPixelMap.GetColor (Point (row, col));
				Boolean	found	=	False;
				for (ColorIndex i = 1; i <= colorsUsed; i++) {
					if (c == table[i]) {
						found = True;
						break;
					}
				}
				if (not found) {
					// ie its not already in there...
					table.SetEntry (++colorsUsed, c);
					if (colorsUsed >= tableSize) {
						return (table);
					}
				}
			}
		}
	}
	
	Real	steps = tableSize - colorsUsed;
	Assert (steps >= 0);
	if (steps > 0) {
		if (sStandardColorTable == Nil) {
			BuildStandardColorTable ();
		}
		
		// sample from the standard CLUT
		Real total = Real (sStandardColorTable->GetEntryCount ());
		for (Real i = 1; i <= steps; i++) {
			table.SetEntry (++colorsUsed, (*sStandardColorTable)[(ColorIndex) (total/steps*i)]);
		}

	}
	Assert (colorsUsed == tableSize);
	
	return (table);
}

static	void	BuildStandardColorTable ()
{
	const	ColorIndex	tableSize = 256;
	
	Assert (sStandardColorTable == Nil);
	sStandardColorTable = new ColorLookupTable ();
	sStandardColorTable->SetEntryCount (tableSize);
	
	ColorIndex	colorsUsed	=	0;

	Int16 steps =  6;	// 6^3 == 216 which is biggest cube root that fits in 256
	Real	stepSize = Real (kMaxComponent - kMinComponent) / (steps -1);
	for (Real red = kMaxComponent; red >= kMinComponent; red -= stepSize) {
		for (Real green = kMaxComponent; green >= kMinComponent; green -= stepSize) {
			for (Real blue = kMaxComponent; blue >= kMinComponent; blue -= stepSize) {
				sStandardColorTable->SetEntry (++colorsUsed, Color (red, green, blue));
			}
		}
	}

	// handle sluff by defining gray values
	Int16	sluff = tableSize - colorsUsed;
	Assert (sluff >= 0);
	if (sluff > 0) {
		// gray values have identical rgb values, with intensity determined by nearness to white
		// i.e. small numbers make for lighter grays. This is a linear walk, simpler than above.
		Real	stepSize = Real (kMaxComponent - kMinComponent + 1) / Real (sluff + 1);

		// As a quirk, we work our way down from black, and fill than table from max entry
		// to last filled spot, always overwriting the last filled spot (which always held black).
		// We do this to ensure that kBlackColor is always the last element of our CLUT (which
		// I think Apple required)
		ColorIndex	where = tableSize+1;
		for (Real intensity = kMinComponent; intensity < kMaxComponent; intensity += stepSize) {
			sStandardColorTable->SetEntry (--where, Color (intensity, intensity, intensity));
		}
		Assert (where == colorsUsed);
	}
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

