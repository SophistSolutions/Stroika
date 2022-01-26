/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScrollBar.cc,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 *		-	Deal properly with scaling of conversion factor in both mac, and motif implementations.
 *		-   change setstepsize to setincrements (merge setstep and setpage)
 *
 *
 * Changes:
 *	$Log: ScrollBar.cc,v $
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.5  1992/07/16  16:56:42  sterling
 *		changed GUI to UI, added Mac_UI and Motif_UI versions
 *
 *		Revision 1.3  1992/07/03  02:13:57  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:34:04  sterling
 *		Lots of changes for base class Slider, and implemented portable mac/motif scrollbars.
 *
 *		Revision 1.45  92/04/30  13:25:53  13:25:53  sterling (Sterling Wight)
 *		use AbstractBordered
 *		
 *		Revision 1.44  92/04/20  14:30:55  14:30:55  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		And used Min (GetBorderHeight (), GetBorderWidth ()) instead of CalcShadowThickness () since that routine went away
 *		from Bordered.cc
 *		
 *		Revision 1.43  92/04/14  19:35:39  19:35:39  lewis (Lewis Pringle)
 *		 Added virtual dtor for ScrollBar because of qSoleInlineVirtualsNotStripped.
 *		And, gave Widget a name for motif scrollbar to make debugging easier - should have VERY little performance
 *		impact (maybe do ifdef qdebug?).
 *		
 *		Revision 1.42  92/04/02  13:07:33  13:07:33  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.41  92/03/26  09:58:24  09:58:24  lewis (Lewis Pringle)
 *		Get rid of oldLive arg to EffectiveLiveChanged.
 *		
 *		Revision 1.38  1992/03/10  00:07:20  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.34  1992/03/05  21:03:58  sterling
 *		support for borders
 *
 *		Revision 1.33  1992/02/21  20:16:29  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.31  1992/02/15  05:35:38  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.29  1992/02/04  04:58:34  sterling
 *		motif tweaks
 *
 *		Revision 1.28  1992/02/03  21:50:41  sterling
 *		massive (but Trivial) interface changes inherited from SLider
 *
 *		Revision 1.27  1992/02/03  17:24:07  lewis
 *		Use class for ScrollBar rather than #define or typedef.
 *
 *		Revision 1.26  1992/01/29  04:47:56  sterling
 *		elimiated typedef inherited for Mac due to compiler bug
 *
 *		Revision 1.25  1992/01/27  07:54:57  lewis
 *		changed MacUI to MacUI.
 *		Added motif calccdefaultsize.
 *		Changed motif stuff to only be sensitive if min!=max, so we wouldnt get callback adjusting our size past a min/max.
 *
 *		Revision 1.24  1992/01/24  23:40:39  lewis
 *		Remove ensures on setsensative calls since we may be unrealized. Also, reset vertical/horizontal attribute
 *		on realization. (MOTIF).
 *
 *		Revision 1.23  1992/01/24  17:53:27  lewis
 *		Override Realize/Unrealize in motif scrollbar. Move sbar actionproc to local scope.
 *
 *		Revision 1.22  1992/01/23  08:12:25  lewis
 *		Call SetBoolResource instead of SetSensative, since motif os control, no longer is responsible for
 *		values when it has not yet been realized.
 *
 *		Revision 1.19  1992/01/20  10:22:39  lewis
 *		Changed some initial values for sbar as temp hack, and be more careful about setting
 *		values when were not yet realized.
 *
 *		Revision 1.18  1992/01/20  04:25:30  lewis
 *		Only set orientation if we have a widget yet, and use oscontrols wrapper.
 *
 *		Revision 1.14  1992/01/14  06:09:40  lewis
 *		Fixed mac code typo.
 *
 *		Revision 1.12  1992/01/10  08:57:12  lewis
 *		Added SetEnabled override to both mac and motif implemenations. Also, fixed SetActive_ implemention ofr
 *		motif to call oscontrol SetSensative.
 *
 *		Revision 1.11  1992/01/08  06:31:26  lewis
 *		Sterl - added ScrollBar_MacUI::CalcDefaultSize ().
 *
 *		Revision 1.10  1992/01/07  04:06:38  lewis
 *		Got rid of bogus portable implementation.
 *
 *		Revision 1.8  1991/12/27  17:02:49  lewis
 *		Fixed TrackPress bug with motif.
 *
 *
 */


#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Controls.h>
#include	<Windows.h>
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/ScrollBar.h>
#endif	/*GDI*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"OSControls.hh"

#include "Time.hh"
#include "Pallet.hh"
#include "EventManager.hh"
#include "Desktop.hh"
#include "Tracker.hh"
#include "ScrollBar.hh"




class	ThumbDragger : public ShapeDragger {
	public:
		ThumbDragger (const Shape& shape, const Rect& shapeExtent, AbstractScrollBar& enclosure);
	
	protected:
		override	Boolean	TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);

	private:
		AbstractScrollBar&	fScrollBar;
};



/*
 ********************************************************************************
 ************************************ AbstractScrollBar *************************
 ********************************************************************************
 */

AbstractScrollBar::AbstractScrollBar (SliderController* sliderController):
	Slider (sliderController)
{
}

Real	AbstractScrollBar::GetStepSize () const
{
	Invariant ();
	return (GetStepSize_ ());
}

void	AbstractScrollBar::SetSteps (Real newStepSize, Real newPageSize, Panel::UpdateMode updateMode)
{
	Invariant ();
	if ((newStepSize != GetStepSize ()) or (newPageSize != GetPageSize ())) {
		SetSteps_ (newStepSize, newPageSize, updateMode);
		Invariant ();
	}
	Ensure (newStepSize == GetStepSize ());
	Ensure (newPageSize == GetPageSize ());
}

void	AbstractScrollBar::SetSteps (Panel::UpdateMode update)
{
	Real range = GetMax () - GetMin ();
	SetSteps (range / 25.0, range / 4.0, update);
}

Real	AbstractScrollBar::GetPageSize () const
{
	Invariant ();
	return (GetPageSize_ ());
}

Real	AbstractScrollBar::GetDisplayed () const
{
	Invariant ();
	return (GetDisplayed_ ());
}

void	AbstractScrollBar::SetDisplayed (Real amountDisplayed, Panel::UpdateMode updateMode)
{
	Invariant ();
	if (amountDisplayed != GetDisplayed ()) {
		SetDisplayed_ (amountDisplayed, updateMode);
	}
	Ensure (amountDisplayed == GetDisplayed ());
}

AbstractScrollBar::Orientation	AbstractScrollBar::GetOrientation () const
{
	Invariant ();
	return (GetOrientation_ ());
}

void	AbstractScrollBar::SetOrientation (AbstractScrollBar::Orientation orientation, Panel::UpdateMode updateMode)
{
	Invariant ();
	if (orientation != GetOrientation ()) {
		SetOrientation_ (orientation, updateMode);
	}
	Ensure (orientation == GetOrientation ());
}

#if		qDebug
void	AbstractScrollBar::Invariant_ () const
{
	Slider::Invariant_ ();
	Require (Abs (GetPageSize_ ()) >= Abs (GetStepSize_ ()));
}
#endif




/*
 ********************************************************************************
 **************************** AbstractScrollBar_MacUI ***************************
 ********************************************************************************
 */

AbstractScrollBar_MacUI::AbstractScrollBar_MacUI (SliderController* sliderController):
	AbstractScrollBar (sliderController)
{
	SetBorder (1, 1, eNoUpdate);
}

Point	AbstractScrollBar_MacUI::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point	newSize = defaultSize;
	if (newSize.GetV () == 0) {
		newSize.SetV (100);
	}
	if (newSize.GetH () == 0) {
		newSize.SetH (100);
	}
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		newSize.SetH (14);	// not 16 cuz of border
	}
	else {
		newSize.SetV (14);	// not 16 cuz of border
	}
	return (newSize + GetBorder ()*2 + GetMargin ()*2);
}


/*
 ********************************************************************************
 **************************** AbstractScrollBar_WinUI ***************************
 ********************************************************************************
 */

AbstractScrollBar_WinUI::AbstractScrollBar_WinUI (SliderController* sliderController):
	AbstractScrollBar (sliderController)
{
	SetBorder (1, 1, eNoUpdate);
}

Point	AbstractScrollBar_WinUI::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point	newSize = defaultSize;
	if (newSize.GetV () == 0) {
		newSize.SetV (100);
	}
	if (newSize.GetH () == 0) {
		newSize.SetH (100);
	}
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		newSize.SetH (14);	// not 16 cuz of border
	}
	else {
		newSize.SetV (14);	// not 16 cuz of border
	}
	return (newSize + GetBorder ()*2 + GetMargin ()*2);
}




/*
 ********************************************************************************
 ***************************** ScrollBar_MacUI_Portable ************************
 ********************************************************************************
 */
ScrollBar_MacUI_Portable::ScrollBar_MacUI_Portable (AbstractScrollBar::Orientation orientation, SliderController* sliderController) :
	AbstractScrollBar_MacUI (sliderController),
	fMin (0),
	fMax (0),
	fValue (0),
	fStepSize (0),
	fPageSize (0),
	fDisplayed (0),
	fUpArrow (Nil),
	fDownArrow (Nil),
	fThumb (Nil),
	fOrientation (orientation)
{
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		fUpArrow = new ArrowButton_MacUI_Portable (AbstractArrowButton::eUp, this);
		fDownArrow = new ArrowButton_MacUI_Portable (AbstractArrowButton::eDown, this);
	}
	else {
		fUpArrow = new ArrowButton_MacUI_Portable (AbstractArrowButton::eLeft, this);
		fDownArrow = new ArrowButton_MacUI_Portable (AbstractArrowButton::eRight, this);
	}
	
	fThumb = new Thumb_MacUI_Portable ();
	
	AddSubView (fUpArrow);
	AddSubView (fDownArrow);
	AddSubView (fThumb);
}

ScrollBar_MacUI_Portable::~ScrollBar_MacUI_Portable ()
{
	RemoveSubView (fUpArrow);
	RemoveSubView (fDownArrow);
	RemoveSubView (fThumb);
	delete fUpArrow;
	delete fDownArrow;
	delete fThumb;
}

Boolean	ScrollBar_MacUI_Portable::TrackPress (const MousePressInfo& mouseInfo)
{
	if (not AbstractScrollBar_MacUI::TrackPress (mouseInfo)) {		
		if (GetThumb ().Contains (mouseInfo.fPressAt)) {
			Rect shapeExtent = GetThumb ().GetExtent ();
			Rect r = GetTrough ();
			if (GetOrientation () == AbstractScrollBar::eVertical) {
				shapeExtent.InsetBy (0, 1);
				Coordinate delta = GetThumb ().GetExtent ().GetBottom () - mouseInfo.fPressAt.GetV ();
				r.SetSize (r.GetSize () - Point (delta, 0));
			}
			else {
				shapeExtent.InsetBy (1, 0);
				Coordinate delta = GetThumb ().GetExtent ().GetRight () - mouseInfo.fPressAt.GetH ();
				r.SetSize (r.GetSize () - Point (0, delta));
			}
			
			ThumbDragger s = ThumbDragger (Rectangle (), shapeExtent, *this);
			s.SetBounds (r);
			s.TrackPress (mouseInfo);
	
			Assert (GetMax () > GetMin ());	
			Real vDelta = 0;
			if (GetOrientation () == AbstractScrollBar::eVertical) {
				Coordinate delta = s.GetNewShapeExtent ().GetTop () - s.GetOldShapeExtent ().GetTop ();
		 		vDelta = ((GetMax () - GetMin ()) * delta) / (GetTrough ().GetHeight () - GetThumb ().GetLocalExtent ().GetHeight ());
			}
			else {
				Coordinate delta = s.GetNewShapeExtent ().GetLeft () - s.GetOldShapeExtent ().GetLeft ();
		 		vDelta = ((GetMax () - GetMin ()) * delta) / (GetTrough ().GetWidth () - GetThumb ().GetLocalExtent ().GetWidth ());
			}
			ChangeValue (GetValue () + vDelta);
		}
		else {
			Boolean movingUp = False;
			
			if (GetOrientation () == AbstractScrollBar::eVertical) {
				movingUp = Boolean (mouseInfo.fPressAt.GetV () <= GetThumb ().GetExtent ().GetTop ());
			}
			else {
				movingUp = Boolean (mouseInfo.fPressAt.GetH () <= GetThumb ().GetExtent ().GetLeft ());
			}
			
			while (EventManager::Get ().MouseStillDown ()) {
				Point loc = AncestorToLocal (EventManager::Get ().GetMouseLocation (), &DeskTop::Get ());
				Boolean up;
				
				if (GetOrientation () == AbstractScrollBar::eVertical) {
					up = Boolean (loc.GetV () <= GetThumb ().GetExtent ().GetTop ());
				}
				else {
					up = Boolean (loc.GetH () <= GetThumb ().GetExtent ().GetLeft ());
				}
				
				if ((up == movingUp) and (not GetThumb ().GetExtent ().Contains (loc))) {
					if (up) {
						ChangeValue (Max (GetValue () - GetPageSize (), GetMin ()));
					}
					else {
						ChangeValue (Min (GetValue () + GetPageSize (), GetMax ()));
					}
				}
			}
		}
	}
	return (True);
}

void	ScrollBar_MacUI_Portable::Draw (const Region& /*update*/)
{
	DrawMacBorder (GetLocalExtent (), GetBorder ());
	
	Color baseColor = Color (kMaxComponent, kMaxComponent, kMaxComponent) * .65;
	Tile t = PalletManager::Get ().MakeTileFromColor (baseColor);
	Fill (Rectangle (), GetTrough (), t);
}

void	ScrollBar_MacUI_Portable::PlaceThumb (Real value, Panel::UpdateMode updateMode)
{
	Rect trough = GetTrough ();
	Coordinate delta = 0;
	if (GetMax () == GetMin ()) {
		Assert (value == GetMin ());
	}
	else {
		Real percent = (value - GetMin ()) / (GetMax () - GetMin ());
		if (GetOrientation () == AbstractScrollBar::eVertical) {
			delta = (trough.GetHeight () - GetThumb ().GetLocalExtent ().GetHeight ()) * percent;
		}
		else {
			delta = (trough.GetWidth () - GetThumb ().GetLocalExtent ().GetWidth ()) * percent;
		}
	}
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		GetThumb ().SetOrigin (trough.GetOrigin () + Point (delta, -1), updateMode);
	}
	else {
		GetThumb ().SetOrigin (trough.GetOrigin () + Point (-1, delta), updateMode);
	}
}

void	ScrollBar_MacUI_Portable::Layout ()
{
	GetUpArrow ().SetSize (GetUpArrow ().CalcDefaultSize ());
	GetDownArrow ().SetSize (GetDownArrow ().CalcDefaultSize ());
	GetThumb ().SetSize (GetThumb ().CalcDefaultSize ());
	PlaceThumb (GetValue (), eDelayedUpdate);
	
	GetUpArrow ().SetOrigin (kZeroPoint);
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		GetDownArrow ().SetOrigin (GetLocalExtent ().GetBotLeft () - Point (GetDownArrow ().GetLocalExtent ().GetHeight (), 0));
	}
	else {
		GetDownArrow ().SetOrigin (GetLocalExtent ().GetTopRight () - Point (0, GetDownArrow ().GetLocalExtent ().GetWidth ()));
	}
	AbstractScrollBar_MacUI::Layout ();
}

void	ScrollBar_MacUI_Portable::ButtonPressed (AbstractButton* /*button*/)
{
}

AbstractThumb&	ScrollBar_MacUI_Portable::GetThumb () const
{
	RequireNotNil (fThumb);
	return (*fThumb);
}

AbstractArrowButton&	ScrollBar_MacUI_Portable::GetUpArrow () const
{
	RequireNotNil (fUpArrow);
	return (*fUpArrow);
}

AbstractArrowButton&	ScrollBar_MacUI_Portable::GetDownArrow () const
{
	RequireNotNil (fDownArrow);
	return (*fDownArrow);
}

void	ScrollBar_MacUI_Portable::ChangeValue (Real value)
{
	Real newValue = Min (Max (floor (value * GetStepSize ())/GetStepSize (), GetMin ()), GetMax ());

	Time time = GetCurrentTime ();
	Rect r;
	Real oldValue = GetValue ();
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		r = GetThumb ().GetExtent ().InsetBy (0, 1);
	}
	else {
		r = GetThumb ().GetExtent ().InsetBy (1, 0);
	}
	SetValue (newValue, eNoUpdate);
	
	if (oldValue != GetValue ()) {
		Color baseColor = Color (kMaxComponent, kMaxComponent, kMaxComponent) * .65;
		Tile t = PalletManager::Get ().MakeTileFromColor (baseColor);
		Fill (Rectangle (), r, t);
		if (GetOrientation () == AbstractScrollBar::eVertical) {
			r = GetThumb ().GetLocalExtent ().InsetBy (0, 1);
		}
		else {
			r = GetThumb ().GetLocalExtent ().InsetBy (1, 0);
		}
		GetThumb ().Refresh (r, eImmediateUpdate);
		time = GetCurrentTime () - time;
		EventManager::Get ().WaitFor (EventManager::Get ().GetDoubleClickTime ()/2 - time);
	}
}

Boolean	ScrollBar_MacUI_Portable::ButtonTracking (AbstractButton* button, Tracker::TrackPhase phase, Boolean mouseInButton)
{
	if ((mouseInButton) and (phase == Tracker::eTrackMove)) {
		if (button == fUpArrow) {
			ChangeValue (GetValue () - GetStepSize ());
		}
		else if (button == fDownArrow) {
			ChangeValue (GetValue () + GetStepSize ());
		}
	}
	return (False);
}

Rect	ScrollBar_MacUI_Portable::GetTrough () const
{
	Rect trough = GetLocalExtent ();
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		trough.InsetBy (Point (0, GetBorder ().GetH () + GetMargin ().GetH ()));
		Coordinate delta = GetUpArrow ().GetExtent ().GetHeight ();
		trough.SetOrigin (trough.GetOrigin () + Point (delta, 0));
		trough.SetSize (trough.GetSize () - Point (delta, 0));
		trough.SetSize (trough.GetSize () - Point (GetDownArrow ().GetExtent ().GetHeight (), 0));
	}
	else {
		trough.InsetBy (Point (GetBorder ().GetV () + GetMargin ().GetV (), 0));
		Coordinate delta = GetUpArrow ().GetExtent ().GetWidth ();
		trough.SetOrigin (trough.GetOrigin () + Point (0, delta));
		trough.SetSize (trough.GetSize () - Point (0, delta));
		trough.SetSize (trough.GetSize () - Point (0, GetDownArrow ().GetExtent ().GetWidth ()));
	}	
	
	return (trough);
}

Real	ScrollBar_MacUI_Portable::GetMin_ () const
{
	return (fMin);
}

Real	ScrollBar_MacUI_Portable::GetMax_ () const
{
	return (fMax);
}

void	ScrollBar_MacUI_Portable::SetBounds_ (Real newMin, Real newMax, Real newValue, Panel::UpdateMode update)
{
	fMin = newMin;
	fMax = newMax;
	SetValue_ (newValue, update);
}

Real	ScrollBar_MacUI_Portable::GetValue_ () const
{
	return (fValue);
}

void	ScrollBar_MacUI_Portable::SetValue_ (Real newValue, Panel::UpdateMode updateMode)
{
	fValue = newValue;
	PlaceThumb (fValue, updateMode);
}

Real	ScrollBar_MacUI_Portable::GetStepSize_ () const
{
	return (fStepSize);
}

void	ScrollBar_MacUI_Portable::SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode /*updateMode*/)
{
	fStepSize = newStepSize;
	fPageSize = newPageSize;
}

Real	ScrollBar_MacUI_Portable::GetPageSize_ () const
{
	return (fPageSize);
}

Real	ScrollBar_MacUI_Portable::GetDisplayed_ () const
{
	return (fDisplayed);
}

AbstractScrollBar::Orientation	ScrollBar_MacUI_Portable::GetOrientation_ () const
{
	return (fOrientation);
}

void	ScrollBar_MacUI_Portable::SetOrientation_ (AbstractScrollBar::Orientation orientation, Panel::UpdateMode updateMode)
{
	fOrientation = orientation;
	InvalidateLayout ();
	if (GetParentView () != Nil) {
		GetParentView ()->InvalidateLayout ();
		Refresh (updateMode);
	}
}

void	ScrollBar_MacUI_Portable::SetDisplayed_ (Real amountDisplayed, Panel::UpdateMode /*updateMode*/)
{
	fDisplayed = amountDisplayed;
}

void	ScrollBar_MacUI_Portable::SetBorder_ (const Point& border, Panel::UpdateMode updateMode) 
{
	InvalidateLayout ();
	AbstractScrollBar_MacUI::SetBorder_ (border, updateMode);
}

void	ScrollBar_MacUI_Portable::SetMargin_ (const Point& margin, Panel::UpdateMode updateMode) 
{
	InvalidateLayout ();
	AbstractScrollBar_MacUI::SetMargin_ (margin, updateMode);
}




/*
 ********************************************************************************
 ***************************** ScrollBar_WinUI_Portable ************************
 ********************************************************************************
 */
ScrollBar_WinUI_Portable::ScrollBar_WinUI_Portable (AbstractScrollBar::Orientation orientation, SliderController* sliderController) :
	AbstractScrollBar_WinUI (sliderController),
	fMin (0),
	fMax (0),
	fValue (0),
	fStepSize (0),
	fPageSize (0),
	fDisplayed (0),
	fUpArrow (Nil),
	fDownArrow (Nil),
	fThumb (Nil),
	fOrientation (orientation)
{
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		fUpArrow = new ArrowButton_MacUI_Portable (AbstractArrowButton::eUp, this);
		fDownArrow = new ArrowButton_MacUI_Portable (AbstractArrowButton::eDown, this);
	}
	else {
		fUpArrow = new ArrowButton_MacUI_Portable (AbstractArrowButton::eLeft, this);
		fDownArrow = new ArrowButton_MacUI_Portable (AbstractArrowButton::eRight, this);
	}
	
	fThumb = new Thumb_WinUI_Portable ();
	
	AddSubView (fUpArrow);
	AddSubView (fDownArrow);
	AddSubView (fThumb);
}

ScrollBar_WinUI_Portable::~ScrollBar_WinUI_Portable ()
{
	RemoveSubView (fUpArrow);
	RemoveSubView (fDownArrow);
	RemoveSubView (fThumb);
	delete fUpArrow;
	delete fDownArrow;
	delete fThumb;
}

Boolean	ScrollBar_WinUI_Portable::TrackPress (const MousePressInfo& mouseInfo)
{
	if (not AbstractScrollBar_WinUI::TrackPress (mouseInfo)) {		
		if (GetThumb ().Contains (mouseInfo.fPressAt)) {
			Rect shapeExtent = GetThumb ().GetExtent ();
			Rect r = GetTrough ();
			if (GetOrientation () == AbstractScrollBar::eVertical) {
				shapeExtent.InsetBy (0, 1);
				Coordinate delta = GetThumb ().GetExtent ().GetBottom () - mouseInfo.fPressAt.GetV ();
				r.SetSize (r.GetSize () - Point (delta, 0));
			}
			else {
				shapeExtent.InsetBy (1, 0);
				Coordinate delta = GetThumb ().GetExtent ().GetRight () - mouseInfo.fPressAt.GetH ();
				r.SetSize (r.GetSize () - Point (0, delta));
			}
			
			ThumbDragger s = ThumbDragger (Rectangle (), shapeExtent, *this);
			s.SetBounds (r);
			s.TrackPress (mouseInfo);
	
			Assert (GetMax () > GetMin ());	
			Real vDelta = 0;
			if (GetOrientation () == AbstractScrollBar::eVertical) {
				Coordinate delta = s.GetNewShapeExtent ().GetTop () - s.GetOldShapeExtent ().GetTop ();
		 		vDelta = ((GetMax () - GetMin ()) * delta) / (GetTrough ().GetHeight () - GetThumb ().GetLocalExtent ().GetHeight ());
			}
			else {
				Coordinate delta = s.GetNewShapeExtent ().GetLeft () - s.GetOldShapeExtent ().GetLeft ();
		 		vDelta = ((GetMax () - GetMin ()) * delta) / (GetTrough ().GetWidth () - GetThumb ().GetLocalExtent ().GetWidth ());
			}
			ChangeValue (GetValue () + vDelta);
		}
		else {
			Boolean movingUp = False;
			
			if (GetOrientation () == AbstractScrollBar::eVertical) {
				movingUp = Boolean (mouseInfo.fPressAt.GetV () <= GetThumb ().GetExtent ().GetTop ());
			}
			else {
				movingUp = Boolean (mouseInfo.fPressAt.GetH () <= GetThumb ().GetExtent ().GetLeft ());
			}
			
			while (EventManager::Get ().MouseStillDown ()) {
				Point loc = AncestorToLocal (EventManager::Get ().GetMouseLocation (), &DeskTop::Get ());
				Boolean up;
				
				if (GetOrientation () == AbstractScrollBar::eVertical) {
					up = Boolean (loc.GetV () <= GetThumb ().GetExtent ().GetTop ());
				}
				else {
					up = Boolean (loc.GetH () <= GetThumb ().GetExtent ().GetLeft ());
				}
				
				if ((up == movingUp) and (not GetThumb ().GetExtent ().Contains (loc))) {
					if (up) {
						ChangeValue (Max (GetValue () - GetPageSize (), GetMin ()));
					}
					else {
						ChangeValue (Min (GetValue () + GetPageSize (), GetMax ()));
					}
				}
			}
		}
	}
	return (True);
}

void	ScrollBar_WinUI_Portable::Draw (const Region& /*update*/)
{
	DrawMacBorder (GetLocalExtent (), GetBorder ());
	
	Color baseColor = Color (kMaxComponent, kMaxComponent, kMaxComponent) * .65;
	Tile t = PalletManager::Get ().MakeTileFromColor (baseColor);
	Fill (Rectangle (), GetTrough (), t);
}

void	ScrollBar_WinUI_Portable::PlaceThumb (Real value, Panel::UpdateMode updateMode)
{
	Rect trough = GetTrough ();
	Coordinate delta = 0;
	if (GetMax () == GetMin ()) {
		Assert (value == GetMin ());
	}
	else {
		Real percent = (value - GetMin ()) / (GetMax () - GetMin ());
		if (GetOrientation () == AbstractScrollBar::eVertical) {
			delta = (trough.GetHeight () - GetThumb ().GetLocalExtent ().GetHeight ()) * percent;
		}
		else {
			delta = (trough.GetWidth () - GetThumb ().GetLocalExtent ().GetWidth ()) * percent;
		}
	}
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		GetThumb ().SetOrigin (trough.GetOrigin () + Point (delta, -1), updateMode);
	}
	else {
		GetThumb ().SetOrigin (trough.GetOrigin () + Point (-1, delta), updateMode);
	}
}

void	ScrollBar_WinUI_Portable::Layout ()
{
	GetUpArrow ().SetSize (GetUpArrow ().CalcDefaultSize ());
	GetDownArrow ().SetSize (GetDownArrow ().CalcDefaultSize ());
	GetThumb ().SetSize (GetThumb ().CalcDefaultSize ());
	PlaceThumb (GetValue (), eDelayedUpdate);
	
	GetUpArrow ().SetOrigin (kZeroPoint);
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		GetDownArrow ().SetOrigin (GetLocalExtent ().GetBotLeft () - Point (GetDownArrow ().GetLocalExtent ().GetHeight (), 0));
	}
	else {
		GetDownArrow ().SetOrigin (GetLocalExtent ().GetTopRight () - Point (0, GetDownArrow ().GetLocalExtent ().GetWidth ()));
	}
	AbstractScrollBar_WinUI::Layout ();
}

void	ScrollBar_WinUI_Portable::ButtonPressed (AbstractButton* /*button*/)
{
}

AbstractThumb&	ScrollBar_WinUI_Portable::GetThumb () const
{
	RequireNotNil (fThumb);
	return (*fThumb);
}

AbstractArrowButton&	ScrollBar_WinUI_Portable::GetUpArrow () const
{
	RequireNotNil (fUpArrow);
	return (*fUpArrow);
}

AbstractArrowButton&	ScrollBar_WinUI_Portable::GetDownArrow () const
{
	RequireNotNil (fDownArrow);
	return (*fDownArrow);
}

void	ScrollBar_WinUI_Portable::ChangeValue (Real value)
{
	Real newValue = Min (Max (floor (value * GetStepSize ())/GetStepSize (), GetMin ()), GetMax ());

	Time time = GetCurrentTime ();
	Rect r;
	Real oldValue = GetValue ();
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		r = GetThumb ().GetExtent ().InsetBy (0, 1);
	}
	else {
		r = GetThumb ().GetExtent ().InsetBy (1, 0);
	}
	SetValue (newValue, eNoUpdate);
	
	if (oldValue != GetValue ()) {
		Color baseColor = Color (kMaxComponent, kMaxComponent, kMaxComponent) * .65;
		Tile t = PalletManager::Get ().MakeTileFromColor (baseColor);
		Fill (Rectangle (), r, t);
		if (GetOrientation () == AbstractScrollBar::eVertical) {
			r = GetThumb ().GetLocalExtent ().InsetBy (0, 1);
		}
		else {
			r = GetThumb ().GetLocalExtent ().InsetBy (1, 0);
		}
		GetThumb ().Refresh (r, eImmediateUpdate);
		time = GetCurrentTime () - time;
		EventManager::Get ().WaitFor (EventManager::Get ().GetDoubleClickTime ()/2 - time);
	}
}

Boolean	ScrollBar_WinUI_Portable::ButtonTracking (AbstractButton* button, Tracker::TrackPhase phase, Boolean mouseInButton)
{
	if ((mouseInButton) and (phase == Tracker::eTrackMove)) {
		if (button == fUpArrow) {
			ChangeValue (GetValue () - GetStepSize ());
		}
		else if (button == fDownArrow) {
			ChangeValue (GetValue () + GetStepSize ());
		}
	}
	return (False);
}

Rect	ScrollBar_WinUI_Portable::GetTrough () const
{
	Rect trough = GetLocalExtent ();
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		trough.InsetBy (Point (0, GetBorder ().GetH () + GetMargin ().GetH ()));
		Coordinate delta = GetUpArrow ().GetExtent ().GetHeight ();
		trough.SetOrigin (trough.GetOrigin () + Point (delta, 0));
		trough.SetSize (trough.GetSize () - Point (delta, 0));
		trough.SetSize (trough.GetSize () - Point (GetDownArrow ().GetExtent ().GetHeight (), 0));
	}
	else {
		trough.InsetBy (Point (GetBorder ().GetV () + GetMargin ().GetV (), 0));
		Coordinate delta = GetUpArrow ().GetExtent ().GetWidth ();
		trough.SetOrigin (trough.GetOrigin () + Point (0, delta));
		trough.SetSize (trough.GetSize () - Point (0, delta));
		trough.SetSize (trough.GetSize () - Point (0, GetDownArrow ().GetExtent ().GetWidth ()));
	}	
	
	return (trough);
}

Real	ScrollBar_WinUI_Portable::GetMin_ () const
{
	return (fMin);
}

Real	ScrollBar_WinUI_Portable::GetMax_ () const
{
	return (fMax);
}

void	ScrollBar_WinUI_Portable::SetBounds_ (Real newMin, Real newMax, Real newValue, Panel::UpdateMode update)
{
	fMin = newMin;
	fMax = newMax;
	SetValue_ (newValue, update);
}

Real	ScrollBar_WinUI_Portable::GetValue_ () const
{
	return (fValue);
}

void	ScrollBar_WinUI_Portable::SetValue_ (Real newValue, Panel::UpdateMode updateMode)
{
	fValue = newValue;
	PlaceThumb (fValue, updateMode);
}

Real	ScrollBar_WinUI_Portable::GetStepSize_ () const
{
	return (fStepSize);
}

void	ScrollBar_WinUI_Portable::SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode /*updateMode*/)
{
	fStepSize = newStepSize;
	fPageSize = newPageSize;
}

Real	ScrollBar_WinUI_Portable::GetPageSize_ () const
{
	return (fPageSize);
}

Real	ScrollBar_WinUI_Portable::GetDisplayed_ () const
{
	return (fDisplayed);
}

AbstractScrollBar::Orientation	ScrollBar_WinUI_Portable::GetOrientation_ () const
{
	return (fOrientation);
}

void	ScrollBar_WinUI_Portable::SetOrientation_ (AbstractScrollBar::Orientation orientation, Panel::UpdateMode updateMode)
{
	fOrientation = orientation;
	InvalidateLayout ();
	if (GetParentView () != Nil) {
		GetParentView ()->InvalidateLayout ();
		Refresh (updateMode);
	}
}

void	ScrollBar_WinUI_Portable::SetDisplayed_ (Real amountDisplayed, Panel::UpdateMode /*updateMode*/)
{
	fDisplayed = amountDisplayed;
}

void	ScrollBar_WinUI_Portable::SetBorder_ (const Point& border, Panel::UpdateMode updateMode) 
{
	InvalidateLayout ();
	AbstractScrollBar_WinUI::SetBorder_ (border, updateMode);
}

void	ScrollBar_WinUI_Portable::SetMargin_ (const Point& margin, Panel::UpdateMode updateMode) 
{
	InvalidateLayout ();
	AbstractScrollBar_WinUI::SetMargin_ (margin, updateMode);
}


#if		qMacToolkit
/*
 ********************************************************************************
 ******************************* ScrollBar_MacUI_Native ************************
 ********************************************************************************
 */


// q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
class	MacOSScrollBar : public MacOSControl {
	public:
		MacOSScrollBar (ScrollBar_MacUI_Native& theSBar):
			MacOSControl (scrollBarProc),
			fSBar (theSBar),
			fGotClickInThumb (False)
		{
		}

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo)
		{
			fGotClickInThumb = False;
			Boolean	result	=	MacOSControl::TrackPress (mouseInfo);
			return (result);
		}
		
		override	void	ActionProc (short partCode)
		{
			if (partCode == inThumb) {
				fGotClickInThumb = True;
			}
			fSBar.ActionProc (partCode);
		}
		
	public:
		inline	Boolean	GetThumbClick () const 
		{ 
			return (fGotClickInThumb); 
		}

	private:
		ScrollBar_MacUI_Native& 	fSBar;
		Boolean						fGotClickInThumb;
};


ScrollBar_MacUI_Native::ScrollBar_MacUI_Native (AbstractScrollBar::Orientation orientation, SliderController* sliderController):
	AbstractScrollBar_MacUI (sliderController),
	fMacSBar (Nil),
	fMin (0),
	fMax (0),
	fValue (0),
	fConversionFactor (0),
	fStepSize (0),
	fPageSize (0),
	fDisplayed (0),
	fOrientation (orientation)
{
	fMacSBar = new MacOSScrollBar (*this);
	AddSubView (fMacSBar);
	fMacSBar->SetHiliteState ((GetEffectiveLive ()? 0: 255), eNoUpdate);
}

ScrollBar_MacUI_Native::~ScrollBar_MacUI_Native ()
{
	RemoveSubView (fMacSBar);
	delete fMacSBar;
}

Boolean	ScrollBar_MacUI_Native::TrackPress (const MousePressInfo& mouseInfo)
{
	Boolean	result	=	AbstractScrollBar_MacUI::TrackPress (mouseInfo);

	/*
	 * If there is a thumb drag, the oscontrol value is updated, and we only want to update
	 * the real value at this point.
	 */
	if (fMacSBar->GetThumbClick ()) {
		Real	newVal;
		newVal = fMin + fMacSBar->GetValue ()/fConversionFactor;
		if (newVal < GetMin ()) {
			newVal = GetMin ();
		}
		if (newVal > GetMax ()) {
			newVal = GetMax ();
		}
		SetValue (newVal, eNoUpdate);
	}
	return (result);
}

void	ScrollBar_MacUI_Native::Draw (const Region& /*update*/)
{
	if (GetBorder () >= Point (1, 1)) {
		DrawBorder_ (GetLocalExtent (), GetBorder () - Point (1, 1));
	}
}

void	ScrollBar_MacUI_Native::SetBorder_ (const Point& border, Panel::UpdateMode update)
{	
	AbstractScrollBar_MacUI::SetBorder_ (border, update);
	InvalidateLayout ();
}

void	ScrollBar_MacUI_Native::SetMargin_ (const Point& margin, Panel::UpdateMode update)
{
	AbstractScrollBar_MacUI::SetMargin_ (margin, update);
	InvalidateLayout ();
}

void	ScrollBar_MacUI_Native::Layout ()
{
	AssertNotNil (fMacSBar);
	Point	offset = GetBorder ();
	if (offset >= Point (1, 1)) {
		offset -= Point (1, 1);
	}
	fMacSBar->SetOrigin (offset);
	fMacSBar->SetSize (GetSize () - offset*2);
	AbstractScrollBar_MacUI::Layout ();
}

void	ScrollBar_MacUI_Native::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractScrollBar_MacUI::EffectiveLiveChanged (newLive, eNoUpdate);
	AssertNotNil (fMacSBar);
	fMacSBar->SetHiliteState ((newLive ? 0 : 255), updateMode);
}

Real	ScrollBar_MacUI_Native::GetMin_ () const
{
	return (fMin);
}

Real	ScrollBar_MacUI_Native::GetMax_ () const
{
	return (fMax);
}

Real	ScrollBar_MacUI_Native::GetValue_ () const
{
	return (fValue);
}

void	ScrollBar_MacUI_Native::SetValue_ (Real newValue, Panel::UpdateMode updateMode)
{
	fValue = newValue;
	fMacSBar->SetValue (short ((fValue-fMin)*fConversionFactor), updateMode);
}

void	ScrollBar_MacUI_Native::SetBounds_ (Real newMin, Real newMax, Real newValue, Panel::UpdateMode updateMode)
{
	fMin = newMin;
	fMax = newMax;
	fValue = newValue;
	fConversionFactor = 1;		// calc to adjust for different dynamic ranges...
								// fake it for now

	if (updateMode == eImmediateUpdate) {
		/*
		 * Must update both on screen at once
		 */
		fMacSBar->SetMin (short ((fMin-fMin)*fConversionFactor), eNoUpdate);
		fMacSBar->SetMax (short ((fMax-fMin)*fConversionFactor), eNoUpdate);
		Refresh (eImmediateUpdate);
	}
	else {
		fMacSBar->SetMin (short ((fMin-fMin)*fConversionFactor), updateMode);
		fMacSBar->SetMax (short ((fMax-fMin)*fConversionFactor), updateMode);
	}
	SetValue_ (newValue, updateMode);
#if 0
	// do we really want to do this automagically???
	if ((GetPageSize () == 0) or (GetStepSize () == 0)) {
		SetSteps ();
	}
#endif
}

Real	ScrollBar_MacUI_Native::GetStepSize_ () const
{
	return (fStepSize);
}

Real	ScrollBar_MacUI_Native::GetPageSize_ () const
{
	return (fPageSize);
}

void	ScrollBar_MacUI_Native::SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode /*updateMode*/)
{
	fStepSize = newStepSize;
	fPageSize = newPageSize;
}

Real	ScrollBar_MacUI_Native::GetDisplayed_ () const
{
	return (fDisplayed);
}

void	ScrollBar_MacUI_Native::SetDisplayed_ (Real amountDisplayed, UpdateMode /*updateMode*/)
{
	fDisplayed = amountDisplayed;
}

AbstractScrollBar::Orientation	ScrollBar_MacUI_Native::GetOrientation_ () const
{
	return (fOrientation);
}

void	ScrollBar_MacUI_Native::SetOrientation_ (AbstractScrollBar::Orientation orientation, UpdateMode /*updateMode*/)
{
	fOrientation = orientation;
}

void	ScrollBar_MacUI_Native::ActionProc (short partCode)
{
	Real	newVal	=	GetValue ();
	switch (partCode) {
		case inUpButton:
			newVal -= GetStepSize ();
			break;
			
		case inDownButton:
			newVal += GetStepSize ();
			break;
			
		case inPageUp:
			newVal -= GetPageSize ();
			break;
			
		case inPageDown:
			newVal += GetPageSize ();
			break;
			
		case	inThumb:
			// really no default behavior.  Under Mac UI, dont do anything, other than perhaps
			// constrain on track.  override this proc, to provide your own behavior here.
			// By default, just set Value after ::TrackControl
			return;
			
		default:
			return;
	}
	if (newVal < GetMin ()) {
		newVal = GetMin ();
	}
	if (newVal > GetMax ()) {
		newVal = GetMax ();
	}
	if (GetValue () != newVal) {
		if (GetSliderController () != Nil) {
			GetSliderController ()->SliderChanging (*this, Tracker::eTrackMove, GetValue (), newVal);
		}
	}
	SetValue (newVal, eImmediateUpdate);
}

#endif	/*qMacToolkit*/







/*
 ********************************************************************************
 ******************************* AbstractScrollBar_MotifUI *****************************
 ********************************************************************************
 */

AbstractScrollBar_MotifUI::AbstractScrollBar_MotifUI (SliderController* sliderController):
	   AbstractScrollBar (sliderController)
{
	SetBorder (2, 2, eNoUpdate);
	SetMargin (0, 0, eNoUpdate);	// recheck Kabaru, seems like Motif ignores the margin anyways
	SetPlane (Bordered::eIn, eNoUpdate);
}

Point	AbstractScrollBar_MotifUI::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point	newSize = defaultSize;
	if (newSize.GetV () == 0) {
		newSize.SetV (100);
	}
	if (newSize.GetH () == 0) {
		newSize.SetH (100);
	}
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		newSize.SetH (11);
	}
	else {
		newSize.SetV (11);
	}
	return (newSize + GetBorder ()*2 + GetMargin ()*2);
}






/*
 ********************************************************************************
 *************************** ScrollBar_MotifUI_Portable ************************
 ********************************************************************************
 */
ScrollBar_MotifUI_Portable::ScrollBar_MotifUI_Portable (AbstractScrollBar::Orientation orientation, SliderController* sliderController) :
	AbstractScrollBar_MotifUI (sliderController),
	fMin (0),
	fMax (0),
	fValue (0),
	fStepSize (0),
	fPageSize (0),
	fDisplayed (0),
	fUpArrow (Nil),
	fDownArrow (Nil),
	fThumb (Nil),
	fOrientation (orientation)
{
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		fUpArrow = new ArrowButton_MotifUI_Portable (AbstractArrowButton::eUp, this);
		fDownArrow = new ArrowButton_MotifUI_Portable (AbstractArrowButton::eDown, this);
	}
	else {
		fUpArrow = new ArrowButton_MotifUI_Portable (AbstractArrowButton::eLeft, this);
		fDownArrow = new ArrowButton_MotifUI_Portable (AbstractArrowButton::eRight, this);
	}
	
	fThumb = new Thumb_MotifUI_Portable ();
	
	GetUpArrow ().SetBorder (kZeroPoint, eNoUpdate);
	GetUpArrow ().SetMargin (kZeroPoint, eNoUpdate);
	GetDownArrow ().SetBorder (kZeroPoint, eNoUpdate);
	GetDownArrow ().SetMargin (kZeroPoint, eNoUpdate);
	GetThumb ().SetBorder (GetBorder (), eNoUpdate);
	GetThumb ().SetMargin (kZeroPoint, eNoUpdate);
	
	AddSubView (fUpArrow);
	AddSubView (fDownArrow);
	AddSubView (fThumb);
}

ScrollBar_MotifUI_Portable::~ScrollBar_MotifUI_Portable ()
{
	RemoveSubView (fUpArrow);
	RemoveSubView (fDownArrow);
	RemoveSubView (fThumb);
	delete fUpArrow;
	delete fDownArrow;
	delete fThumb;
}

Boolean	ScrollBar_MotifUI_Portable::TrackPress (const MousePressInfo& mouseInfo)
{
	if (not AbstractScrollBar_MotifUI::TrackPress (mouseInfo)) {		
		if (GetThumb ().Contains (mouseInfo.fPressAt)) {
			Rect shapeExtent = GetThumb ().GetExtent ();
			Rect r = GetTrough ();
			if (GetOrientation () == AbstractScrollBar::eVertical) {
				shapeExtent.InsetBy (0, 1);
				Coordinate delta = GetThumb ().GetExtent ().GetBottom () - mouseInfo.fPressAt.GetV ();
				r.SetSize (r.GetSize () - Point (delta, 0));
			}
			else {
				shapeExtent.InsetBy (1, 0);
				Coordinate delta = GetThumb ().GetExtent ().GetRight () - mouseInfo.fPressAt.GetH ();
				r.SetSize (r.GetSize () - Point (0, delta));
			}
			
			ThumbDragger s = ThumbDragger (Rectangle (), shapeExtent, *this);
			s.SetBounds (r);
			s.TrackPress (mouseInfo);
	
			Assert (GetMax () > GetMin ());	
			Real vDelta = 0;
			if (GetOrientation () == AbstractScrollBar::eVertical) {
				Coordinate delta = s.GetNewShapeExtent ().GetTop () - s.GetOldShapeExtent ().GetTop ();
		 		vDelta = ((GetMax () - GetMin ()) * delta) / (GetTrough ().GetHeight () - GetThumb ().GetLocalExtent ().GetHeight ());
			}
			else {
				Coordinate delta = s.GetNewShapeExtent ().GetLeft () - s.GetOldShapeExtent ().GetLeft ();
		 		vDelta = ((GetMax () - GetMin ()) * delta) / (GetTrough ().GetWidth () - GetThumb ().GetLocalExtent ().GetWidth ());
			}
			ChangeValue (GetValue () + vDelta);
		}
		else {
			Boolean movingUp = False;
			
			if (GetOrientation () == AbstractScrollBar::eVertical) {
				movingUp = Boolean (mouseInfo.fPressAt.GetV () <= GetThumb ().GetExtent ().GetTop ());
			}
			else {
				movingUp = Boolean (mouseInfo.fPressAt.GetH () <= GetThumb ().GetExtent ().GetLeft ());
			}
			
			while (EventManager::Get ().MouseStillDown ()) {
				Point loc = AncestorToLocal (EventManager::Get ().GetMouseLocation (), &DeskTop::Get ());
				Boolean up;
				
				if (GetOrientation () == AbstractScrollBar::eVertical) {
					up = Boolean (loc.GetV () <= GetThumb ().GetExtent ().GetTop ());
				}
				else {
					up = Boolean (loc.GetH () <= GetThumb ().GetExtent ().GetLeft ());
				}
				
				if ((up == movingUp) and (not GetThumb ().GetExtent ().Contains (loc))) {
					if (up) {
						ChangeValue (Max (GetValue () - GetPageSize (), GetMin ()));
					}
					else {
						ChangeValue (Min (GetValue () + GetPageSize (), GetMax ()));
					}
				}
			}
		}
	}
	return (True);
}

void	ScrollBar_MotifUI_Portable::Draw (const Region& /*update*/)
{
	Color 	backGroundColor = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	Color	selectColor;
	Color	topShadow;
	Color	bottomShadow;
	
	CalcColorSet (backGroundColor, selectColor, topShadow, bottomShadow);

	DrawMotifBorder (GetLocalExtent (), GetBorder (), backGroundColor, GetPlane ());
	Tile t = PalletManager::Get ().MakeTileFromColor (selectColor);
	Fill (Rectangle (), GetLocalExtent ().InsetBy (GetBorder ()), t);
}

void	ScrollBar_MotifUI_Portable::PlaceThumb (Real value, Panel::UpdateMode updateMode)
{
	Rect trough = GetTrough ();
	Coordinate delta = 0;
	if (GetMax () == GetMin ()) {
		Assert (value == GetMin ());
		GetThumb ().SetSize (kZeroPoint);
	}
	else {
		Real percent = (value - GetMin ()) / (GetMax () - GetMin ());
		Real thumbPercent  = (GetDisplayed () - GetMin ()) / (GetMax () - GetMin ());
		Coordinate minHeight = 2;
		if (GetOrientation () == AbstractScrollBar::eVertical) {
			GetThumb ().SetSize (Point (Max (Coordinate (trough.GetHeight () * thumbPercent), minHeight), trough.GetWidth ()));
			delta = (trough.GetHeight () - GetThumb ().GetLocalExtent ().GetHeight ()) * percent;
		}
		else {
			GetThumb ().SetSize (Point (trough.GetHeight (), Max (Coordinate (trough.GetWidth () * thumbPercent), minHeight)));
			delta = (trough.GetWidth () - GetThumb ().GetLocalExtent ().GetWidth ()) * percent;
		}
	}
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		GetThumb ().SetOrigin (trough.GetOrigin () + Point (delta, 0), updateMode);
	}
	else {
		GetThumb ().SetOrigin (trough.GetOrigin () + Point (0, delta), updateMode);
	}
}

void	ScrollBar_MotifUI_Portable::Layout ()
{
	GetUpArrow ().SetSize (GetUpArrow ().CalcDefaultSize ());
	GetDownArrow ().SetSize (GetDownArrow ().CalcDefaultSize ());
	PlaceThumb (GetValue (), eDelayedUpdate);
	
	GetUpArrow ().SetOrigin (GetBorder ());
	Rect r = InsetBy (GetLocalExtent (), GetBorder ());
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		GetDownArrow ().SetOrigin (r.GetBotLeft () - Point (GetDownArrow ().GetLocalExtent ().GetHeight (), 0));
	}
	else {
		GetDownArrow ().SetOrigin (r.GetTopRight () - Point (0, GetDownArrow ().GetLocalExtent ().GetWidth ()));
	}
	AbstractScrollBar_MotifUI::Layout ();
}

void	ScrollBar_MotifUI_Portable::ButtonPressed (AbstractButton* /*button*/)
{
}

AbstractThumb&	ScrollBar_MotifUI_Portable::GetThumb () const
{
	RequireNotNil (fThumb);
	return (*fThumb);
}

AbstractArrowButton&	ScrollBar_MotifUI_Portable::GetUpArrow () const
{
	RequireNotNil (fUpArrow);
	return (*fUpArrow);
}

AbstractArrowButton&	ScrollBar_MotifUI_Portable::GetDownArrow () const
{
	RequireNotNil (fDownArrow);
	return (*fDownArrow);
}

void	ScrollBar_MotifUI_Portable::ChangeValue (Real value)
{
	Real newValue = Min (Max (floor (value * GetStepSize ())/GetStepSize (), GetMin ()), GetMax ());

	Time time = GetCurrentTime ();
	Rect r;
	Real oldValue = GetValue ();
	r = GetThumb ().GetExtent ();
	SetValue (newValue, eNoUpdate);
	
	if (oldValue != GetValue ()) {
		Color 	backGroundColor = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
		Color	selectColor;
		Color	topShadow;
		Color	bottomShadow;
		
		CalcColorSet (backGroundColor, selectColor, topShadow, bottomShadow);
		
		Tile t = PalletManager::Get ().MakeTileFromColor (selectColor);
		Fill (Rectangle (), r, t);
		Refresh (GetThumb ().GetExtent (), eImmediateUpdate);
		time = GetCurrentTime () - time;
		EventManager::Get ().WaitFor (EventManager::Get ().GetDoubleClickTime ()/2 - time);
	}
}

Boolean	ScrollBar_MotifUI_Portable::ButtonTracking (AbstractButton* button, Tracker::TrackPhase phase, Boolean mouseInButton)
{
	if ((mouseInButton) and (phase == Tracker::eTrackMove)) {
		if (button == fUpArrow) {
			ChangeValue (GetValue () - GetStepSize ());
		}
		else if (button == fDownArrow) {
			ChangeValue (GetValue () + GetStepSize ());
		}
	}
	return (False);
}

Rect	ScrollBar_MotifUI_Portable::GetTrough () const
{
	Rect trough = InsetBy (GetLocalExtent (), GetBorder ());
	if (GetOrientation () == AbstractScrollBar::eVertical) {
		Coordinate delta = GetUpArrow ().GetExtent ().GetHeight ();
		trough.SetOrigin (trough.GetOrigin () + Point (delta, 0));
		delta += GetDownArrow ().GetExtent ().GetHeight ();
		trough.SetSize (trough.GetSize () - Point (delta, 0));
	}
	else {
		Coordinate delta = GetUpArrow ().GetExtent ().GetWidth ();
		trough.SetOrigin (trough.GetOrigin () + Point (0, delta));
		delta += GetDownArrow ().GetExtent ().GetWidth ();
		trough.SetSize (trough.GetSize () - Point (0, delta));
	}	
	
	return (trough);
}

Real	ScrollBar_MotifUI_Portable::GetMin_ () const
{
	return (fMin);
}

Real	ScrollBar_MotifUI_Portable::GetMax_ () const
{
	return (fMax);
}

void	ScrollBar_MotifUI_Portable::SetBounds_ (Real newMin, Real newMax, Real newValue, Panel::UpdateMode update)
{
	fMin = newMin;
	fMax = newMax;
	SetValue_ (newValue, update);
}

Real	ScrollBar_MotifUI_Portable::GetValue_ () const
{
	return (fValue);
}

void	ScrollBar_MotifUI_Portable::SetValue_ (Real newValue, Panel::UpdateMode updateMode)
{
	fValue = newValue;
	PlaceThumb (fValue, updateMode);
}

Real	ScrollBar_MotifUI_Portable::GetStepSize_ () const
{
	return (fStepSize);
}

void	ScrollBar_MotifUI_Portable::SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode /*updateMode*/)
{
	fStepSize = newStepSize;
	fPageSize = newPageSize;
}

Real	ScrollBar_MotifUI_Portable::GetPageSize_ () const
{
	return (fPageSize);
}

Real	ScrollBar_MotifUI_Portable::GetDisplayed_ () const
{
	return (fDisplayed);
}

AbstractScrollBar::Orientation	ScrollBar_MotifUI_Portable::GetOrientation_ () const
{
	return (fOrientation);
}

void	ScrollBar_MotifUI_Portable::SetOrientation_ (AbstractScrollBar::Orientation orientation, Panel::UpdateMode updateMode)
{
	fOrientation = orientation;
	InvalidateLayout ();
	if (GetParentView () != Nil) {
		GetParentView ()->InvalidateLayout ();
		Refresh (updateMode);
	}
}

void	ScrollBar_MotifUI_Portable::SetDisplayed_ (Real amountDisplayed, UpdateMode /*updateMode*/)
{
	fDisplayed = amountDisplayed;
}

void	ScrollBar_MotifUI_Portable::SetBorder_ (const Point& border, Panel::UpdateMode updateMode) 
{
	InvalidateLayout ();
	AbstractScrollBar_MotifUI::SetBorder_ (border, updateMode);
}

void	ScrollBar_MotifUI_Portable::SetMargin_ (const Point& margin, Panel::UpdateMode updateMode) 
{
	InvalidateLayout ();
	AbstractScrollBar_MotifUI::SetMargin_ (margin, updateMode);
}



#if		qXmToolkit
/*
 ********************************************************************************
 **************************** ScrollBar_MotifUI_Native *************************
 ********************************************************************************
 */

ScrollBar_MotifUI_Native::ScrollBar_MotifUI_Native (Orientation orientation, SliderController* sliderController):
	AbstractScrollBar_MotifUI (sliderController),
	fOSControl (Nil),
	fMin (1),
	fMax (100),
	fValue (1),
	fConversionFactor (1),
	fStepSize (1),
	fPageSize (10),
	fDisplayed (1),
	fOrientation (orientation)
{
	class	MyOSScrollBar : public MotifOSControl {
		public:
			MyOSScrollBar (ScrollBar_MotifUI_Native& scrollBar):
				MotifOSControl (),
				fScrollBar (scrollBar)
			{
			}
			
			override	osWidget*	DoBuild_ (osWidget* parent)
			{
				RequireNotNil (parent);
				osWidget* w = ::XmCreateScrollBar (parent, "ScrollBar", Nil, 0);

				int range = (int) (fScrollBar.GetMax() - fScrollBar.GetMin ());
				int sliderSize = ((range <= 100) ? (Min (range, 10)) : (range / 10));

				Arg	args [6];
				XtSetArg (args[0], XmNincrement, (int) fScrollBar.GetStepSize ());
				XtSetArg (args[1], XmNpageIncrement, (int) fScrollBar.GetPageSize ());
				XtSetArg (args[2], XmNmaximum, (int) fScrollBar.GetMax ());
				XtSetArg (args[3], XmNminimum, (int) fScrollBar.GetMin ());
				XtSetArg (args[4], XmNvalue, (int) fScrollBar.GetValue ());
				XtSetArg (args[5], XmNsliderSize, sliderSize);
				::XtSetValues (w, args, 6);

				::XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc)sCallBackProc, (XtPointer)&fScrollBar);

				return (w);
			}
			
			static void sCallBackProc (osWidget* w, caddr_t client_data, caddr_t call_data)
				{
					int theMax, theMin, theValue;
					Arg	args [3];
					XtSetArg (args[0], XmNmaximum, &theMax);
					XtSetArg (args[1], XmNminimum, &theMin);
					XtSetArg (args[2], XmNvalue, &theValue);
					::XtGetValues (w, args, 3);

					ScrollBar_MotifUI_Native* fred = (ScrollBar_MotifUI_Native*)client_data;
					AssertNotNil (fred);
					Real	range = theMax - theMin;
					fred->SetValue (theMax * Real (theValue) / range);
				}

		private:
			ScrollBar_MotifUI_Native&	fScrollBar;
	};
	
	AddSubView (fOSControl = new MyOSScrollBar (*this));

	SetBorder_ (GetBorder (), eNoUpdate);
	SetMargin_ (GetMargin (), eNoUpdate);
}

ScrollBar_MotifUI_Native::~ScrollBar_MotifUI_Native ()
{
	RemoveSubView (fOSControl);
	delete fOSControl;
}

void	ScrollBar_MotifUI_Native::SetSize_ (const Point& newSize, UpdateMode update)
{
	inherited::SetSize_ (newSize, update);
	  
	AssertNotNil (fOSControl);
	fOSControl->SetUnsignedCharResourceValue (XmNorientation, (newSize.GetV () >= newSize.GetH ())? XmVERTICAL: XmHORIZONTAL);
}

void	ScrollBar_MotifUI_Native::Layout ()
{
	AssertNotNil (fOSControl);
	Assert (fOSControl->GetOrigin () == kZeroPoint);
	fOSControl->SetSize (GetSize ());
	inherited::Layout ();
}

void	ScrollBar_MotifUI_Native::EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode)
{
	inherited::EffectiveLiveChanged (newLive, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetBooleanResourceValue (XmNsensitive, Boolean (newLive and (GetMin () != GetMax ())));
}

Real	ScrollBar_MotifUI_Native::GetMin_ () const
{
	return (fMin);
}

Real	ScrollBar_MotifUI_Native::GetMax_ () const
{
	return (fMax);
}

Real	ScrollBar_MotifUI_Native::GetValue_ () const
{
	return (fValue);
}

void	ScrollBar_MotifUI_Native::SetValue_ (Real newValue, UpdateMode updateMode)
{
	fValue = newValue;

	if (fOSControl->GetOSRepresentation () != Nil) {
		Arg	args [1];
		XtSetArg (args[0], XmNvalue, int ((fValue-fMin)*fConversionFactor));
		::XtSetValues (fOSControl->GetOSRepresentation (), args, 1);
	}
}

void	ScrollBar_MotifUI_Native::SetBounds_ (Real newMin, Real newMax, Real newValue, UpdateMode updateMode)
{
	fMin = newMin;
	fMax = newMax;
	fValue = newValue;
	fConversionFactor = 1;		// calc to adjust for different dynamic ranges...
								// fake it for now
//gDebugStream << "in setminmax, newMin = " << newMin << ",  newMax = " << newMax << newline;
	if (fOSControl->GetOSRepresentation () != Nil) {
		int	newValue = GetValue_ ();
	
		// Motif value specification is pretty brain-dead as it makes the user take into
		// account the slidersize, which can vary over time! seems really pretty hard to
		// simply disable the scrollbar
		if (newMin == newMax) {
			newMin = 0;
			newMax = 1;
			newValue = 0;
		}
// need to add GetOrientation!!!
Real	displayed;
if (GetSize ().GetV () > GetSize ().GetH ()) {
	displayed = GetSize ().GetV ();
}
else {
	displayed = GetSize ().GetH ();
}
newMax += displayed;
		int range = (int) (newMax - newMin);
		// this is what O'Reilly documents as the algorithm, but it seems really brain dead, so I hope it is wrong
		//int sliderSize = ((range <= 100) ? (Min (range, 10)) : (range / 10));
//		int sliderSize = displayed*displayed/range + .5;
		int sliderSize = displayed;
		sliderSize = Min (sliderSize, newMax);
//gDebugStream << "setting values, min = " << newMin << " max = " << newMax << " sliderSize = " << sliderSize << " displayed = " << displayed << newline;
		Arg	args [4];
		XtSetArg (args[0], XmNminimum, (int) newMin);
		XtSetArg (args[1], XmNmaximum, (int) newMax);
		XtSetArg (args[2], XmNvalue, newValue);
		XtSetArg (args[3], XmNsliderSize, sliderSize);
		::XtSetValues (fOSControl->GetOSRepresentation (), args, 4);
	}
#if 0
	if ((GetPageSize () == 1) or (GetStepSize () == 0)) {
		SetSteps ();
	}
#endif

	fOSControl->SetBooleanResourceValue (XmNsensitive, Boolean (GetEffectiveLive () and (GetMin () != GetMax ())));
}

Real	ScrollBar_MotifUI_Native::GetStepSize_ () const
{
	return (fStepSize);
}

void	ScrollBar_MotifUI_Native::SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode /*updateMode*/)
{
	fStepSize = newStepSize;
	fPageSize = newPageSize;
}

Real	ScrollBar_MotifUI_Native::GetPageSize_ () const
{
	return (fPageSize);
}

Real	ScrollBar_MotifUI_Native::GetDisplayed_ () const
{
	return (fDisplayed);
}

void	ScrollBar_MotifUI_Native::SetDisplayed_ (Real amountDisplayed, UpdateMode /*updateMode*/)
{
	fDisplayed = amountDisplayed;
}

AbstractScrollBar::Orientation	ScrollBar_MotifUI_Native::GetOrientation_ () const
{
	return (fOrientation);
}

void	ScrollBar_MotifUI_Native::SetOrientation_ (AbstractScrollBar::Orientation orientation, UpdateMode /*updateMode*/)
{
	fOrientation = orientation;
}

void	ScrollBar_MotifUI_Native::SetBorder_ (const Point& border, UpdateMode update) 
{
	View::SetBorder_ (border, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorderHeight (), GetBorderWidth ()));
	Refresh (update);
}

void	ScrollBar_MotifUI_Native::SetMargin_ (const Point& margin, UpdateMode update)
{
	View::SetMargin_ (margin, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetDimensionResourceValue (XmNmarginHeight, GetMarginHeight ());
    fOSControl->SetDimensionResourceValue (XmNmarginWidth, GetMarginWidth ());
	Refresh (update);
}

void	ScrollBar_MotifUI_Native::SetPlane_ (AbstractBordered::Plane plane, UpdateMode update)
{
	View::SetPlane_ (plane, eNoUpdate);
	AssertNotNil (fOSControl);
//	fOSControl->SetDimensionResourceValue (XmNmarginHeight, GetMarginHeight ());
	Refresh (update);
}


void	ScrollBar_MotifUI_Native::Realize (osWidget* parent)
{
	// Dont call inherited version since we call our childs realize directly...
	RequireNotNil (parent);
	AssertNotNil (fOSControl);
	fOSControl->Realize (parent);
	AssertNotNil (fOSControl->GetOSRepresentation ());
	SetBounds (GetMin (), GetMax (), GetValue (), eNoUpdate);
	fOSControl->SetUnsignedCharResourceValue (XmNorientation, (GetSize ().GetV () >= GetSize ().GetH ())? XmVERTICAL: XmHORIZONTAL);
	fOSControl->SetBooleanResourceValue (XmNsensitive, Boolean (GetEffectiveLive () and (GetMin () != GetMax ())));
	fOSControl->SetDimensionResourceValue (XmNmarginHeight, GetMarginHeight ());
    fOSControl->SetDimensionResourceValue (XmNmarginWidth, GetMarginWidth ());
	fOSControl->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorderHeight (), GetBorderWidth ()));
}

void	ScrollBar_MotifUI_Native::UnRealize ()
{
	AssertNotNil (fOSControl);
	fOSControl->UnRealize ();
	Assert (fOSControl->GetOSRepresentation () == Nil);
}

#endif	/*qXmToolkit*/






/*
 ********************************************************************************
 *********************************** ThumbDragger *******************************
 ********************************************************************************
 */
ThumbDragger::ThumbDragger (const Shape& shape, const Rect& shapeExtent, AbstractScrollBar& scrollbar) :
	ShapeDragger (shape, shapeExtent, scrollbar),
	fScrollBar (scrollbar)
{
	if (fScrollBar.GetOrientation () == AbstractScrollBar::eVertical) {
		SetHysteresis (Point (1, 0));
		SetDragDirection (ShapeDragger::eVertical);
	}
	else {
		SetHysteresis (Point (0, 1));
		SetDragDirection (ShapeDragger::eHorizontal);
	}
}

Boolean	ThumbDragger::TrackResult (Tracker::TrackPhase phase, const Point& anchor, const Point& previous, const Point& next)
{
	Real vDelta = 0;
	if (fScrollBar.GetOrientation () == AbstractScrollBar::eVertical) {
		Coordinate	delta = next.GetV () - anchor.GetV ();
		vDelta = ((fScrollBar.GetMax () - fScrollBar.GetMin ()) * delta) / 
			(GetBounds ().GetBounds ().GetHeight () + GetOldShapeExtent ().GetTop () - anchor.GetV ());
	}
	else {
		Coordinate	delta = next.GetH () - anchor.GetH ();
		vDelta = ((fScrollBar.GetMax () - fScrollBar.GetMin ()) * delta) / 
			(GetBounds ().GetBounds ().GetWidth () + GetOldShapeExtent ().GetLeft () - anchor.GetH ());
	}
	
	Real newValue = floor ((fScrollBar.GetValue () + vDelta) * fScrollBar.GetStepSize ())/fScrollBar.GetStepSize ();
	newValue = Min (Max (newValue, fScrollBar.GetMin ()), fScrollBar.GetMax ());
	Boolean result = fScrollBar.Changing (phase, fScrollBar.GetValue (), newValue);
	
	return (Boolean (ShapeDragger::TrackResult (phase, anchor, previous, next) or result));
}


/*
 ********************************************************************************
 ********************************** AbstractThumb *******************************
 ********************************************************************************
 */
AbstractThumb::AbstractThumb () :
	View ()
{
}

/*
 ********************************************************************************
 *************************** AbstractThumb_MacUI *******************************
 ********************************************************************************
 */
AbstractThumb_MacUI::AbstractThumb_MacUI () :
	AbstractThumb ()
{
	SetBorder (1, 1, eNoUpdate);
}

Point	AbstractThumb_MacUI::CalcDefaultSize_ (const Point& defaultSize) const
{
	return (Point (14, 14) + GetBorder ()*2 + GetMargin ()*2);
}

/*
 ********************************************************************************
 ************************** Thumb_MacUI_Portable *******************************
 ********************************************************************************
 */
Thumb_MacUI_Portable::Thumb_MacUI_Portable () :
	AbstractThumb_MacUI ()
{
}

void	Thumb_MacUI_Portable::Draw (const Region& /*update*/)
{
	DrawMacBorder (GetLocalExtent (), GetBorder ());

	Rect r = GetLocalExtent ().InsetBy (GetBorder ());
	Color baseColor = Color (kMaxComponent, kMaxComponent, kMaxComponent) * .65;
	Tile t = PalletManager::Get ().MakeTileFromColor (baseColor * 1.4);
	Fill (Rectangle (), r, t);
	
	Tile t1 = PalletManager::Get ().MakeTileFromColor (baseColor);
	r.SetOrigin (r.GetOrigin () + Point (1, 1));
	r.SetSize (r.GetSize () - Point (1, 1));
	Fill (Rectangle (), r, t1);
	
	r.InsetBy (Point (2, 3));
	Fill (Rectangle (), r, t);
	Tile t2 = PalletManager::Get ().MakeTileFromColor (kDarkGrayColor);
	Rect r1 = r;
	r1.SetSize (Point (1, r1.GetWidth ()));
	r1.SetOrigin (r1.GetOrigin () + Point (1, 0));
	while (r1.GetBotRight () < r.GetBotRight ()) {
		Fill (Rectangle (), r1, t2);
		r1.SetOrigin (r1.GetOrigin () + Point (2, 0));
	}
}




/*
 ********************************************************************************
 *************************** AbstractThumb_WinUI *******************************
 ********************************************************************************
 */
AbstractThumb_WinUI::AbstractThumb_WinUI () :
	AbstractThumb ()
{
	SetBorder (1, 1, eNoUpdate);
}

Point	AbstractThumb_WinUI::CalcDefaultSize_ (const Point& defaultSize) const
{
	return (Point (14, 14) + GetBorder ()*2 + GetMargin ()*2);
}

/*
 ********************************************************************************
 ************************** Thumb_WinUI_Portable *******************************
 ********************************************************************************
 */
Thumb_WinUI_Portable::Thumb_WinUI_Portable () :
	AbstractThumb_WinUI ()
{
}

void	Thumb_WinUI_Portable::Draw (const Region& /*update*/)
{
	DrawMacBorder (GetLocalExtent (), GetBorder ());

	Rect r = GetLocalExtent ().InsetBy (GetBorder ());
	Color baseColor = Color (kMaxComponent, kMaxComponent, kMaxComponent) * .65;
	Tile t = PalletManager::Get ().MakeTileFromColor (baseColor * 1.4);
	Fill (Rectangle (), r, t);
	
	Tile t1 = PalletManager::Get ().MakeTileFromColor (baseColor);
	r.SetOrigin (r.GetOrigin () + Point (1, 1));
	r.SetSize (r.GetSize () - Point (1, 1));
	Fill (Rectangle (), r, t1);
	
	r.InsetBy (Point (2, 3));
	Fill (Rectangle (), r, t);
	Tile t2 = PalletManager::Get ().MakeTileFromColor (kDarkGrayColor);
	Rect r1 = r;
	r1.SetSize (Point (1, r1.GetWidth ()));
	r1.SetOrigin (r1.GetOrigin () + Point (1, 0));
	while (r1.GetBotRight () < r.GetBotRight ()) {
		Fill (Rectangle (), r1, t2);
		r1.SetOrigin (r1.GetOrigin () + Point (2, 0));
	}
}


/*
 ********************************************************************************
 ************************* AbstractThumb_MotifUI *******************************
 ********************************************************************************
 */
AbstractThumb_MotifUI::AbstractThumb_MotifUI () :
	AbstractThumb ()
{
	SetBorder (1, 1, eNoUpdate);
}

Point	AbstractThumb_MotifUI::CalcDefaultSize_ (const Point& defaultSize) const
{
	return (Point (7, 7) + GetBorder ()*2 + GetMargin ()*2);
}

/*
 ********************************************************************************
 ************************ Thumb_MotifUI_Portable *******************************
 ********************************************************************************
 */
Thumb_MotifUI_Portable::Thumb_MotifUI_Portable () :
	AbstractThumb_MotifUI ()
{
}

void	Thumb_MotifUI_Portable::Draw (const Region& /*update*/)
{
	Color 	backGroundColor = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawMotifBorder (GetLocalExtent (), GetBorder (), backGroundColor, GetPlane ());
	Tile t = PalletManager::Get ().MakeTileFromColor (backGroundColor);
	Fill (Rectangle (), GetLocalExtent ().InsetBy (GetBorder ()), t);
}








/*
 ********************************************************************************
 ******************************** ScrollBar_MacUI ******************************
 ********************************************************************************
 */
ScrollBar_MacUI::ScrollBar_MacUI (AbstractScrollBar::Orientation orientation, SliderController* sliderController) :
#if		qNative && qMacToolkit
	ScrollBar_MacUI_Native (orientation, sliderController)
#else
	ScrollBar_MacUI_Portable (orientation, sliderController)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
ScrollBar_MacUI::~ScrollBar_MacUI () {}
#endif










/*
 ********************************************************************************
 ******************************* ScrollBar_MotifUI *****************************
 ********************************************************************************
 */
ScrollBar_MotifUI::ScrollBar_MotifUI (AbstractScrollBar::Orientation orientation, SliderController* sliderController):
#if		qNative && qXmToolkit
	ScrollBar_MotifUI_Native (orientation, sliderController)
#else
	ScrollBar_MotifUI_Portable (orientation, sliderController)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
ScrollBar_MotifUI::~ScrollBar_MotifUI () {}
#endif





/*
 ********************************************************************************
 ******************************** ScrollBar_WinUI ******************************
 ********************************************************************************
 */
ScrollBar_WinUI::ScrollBar_WinUI (AbstractScrollBar::Orientation orientation, SliderController* sliderController) :
	ScrollBar_WinUI_Portable (orientation, sliderController)
{
}

#if		qSoleInlineVirtualsNotStripped
ScrollBar_WinUI::~ScrollBar_WinUI () {}
#endif





/*
 ********************************************************************************
 ************************************* ScrollBar *******************************
 ********************************************************************************
 */
ScrollBar::ScrollBar (AbstractScrollBar::Orientation orientation, SliderController* sliderController) :
#if		qMacUI
	ScrollBar_MacUI (orientation, sliderController)
#elif	qMotifUI
	ScrollBar_MotifUI (orientation, sliderController)
#elif	qWinUI
	ScrollBar_WinUI (orientation, sliderController)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
ScrollBar::~ScrollBar () {}
#endif






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


