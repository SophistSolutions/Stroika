/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Scale.cc,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Scale.cc,v $
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.5  1992/07/16  16:55:33  sterling
 *		changed GUI to UI
 *
 *		Revision 1.3  1992/07/03  02:13:14  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:33:00  sterling
 *		Lots of changes.
 *
 *
 *
 */

#include "Format.hh"
#include "Pallet.hh"
#include "Scale.hh"
#include "Tracker.hh"




class	ScaleThumb;
class	ScaleThumbDragger : public Tracker {
	public:
		ScaleThumbDragger (AbstractScale& abstractScale);

	protected:
		override	void	TrackConstrain (TrackPhase phase, const Point& anchor, const Point& previous, Point& next);
		override	void	TrackFeedback (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);
		override	Boolean	TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);

	private:
		AbstractScale&		fScale;
		Real				fOldValue;
		Real				fNewValue;
		Real				fScalingFactor;
};

class	AbstractScaleThumb : public View {
	public:
		AbstractScaleThumb (AbstractScale& abstractScale);
				
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	private:
		AbstractScale&			fScale;
};


class	ScaleThumb_MacUI : public AbstractScaleThumb {
	public:
		ScaleThumb_MacUI (AbstractScale& abstractScale);
		
		override	void	Draw (const Region& update);
};

class	ScaleThumb_MotifUI : public AbstractScaleThumb {
	public:
		ScaleThumb_MotifUI (AbstractScale& abstractScale);

		override	void	Draw (const Region& update);
};


class	AbstractScaleIndicator : public View {
	public:
		AbstractScaleIndicator (AbstractScale& abstractScale);
		~AbstractScaleIndicator ();

		virtual	Rect	GetThumbExtent () const;

	protected:
		override	void	Layout ();

		override	void	SetBorder_ (const Point& border, UpdateMode updateMode);
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode);
		override	void	SetPlane_ (Bordered::Plane plane, UpdateMode update);

		nonvirtual	AbstractScaleThumb&	GetThumb () const;
		nonvirtual	void				SetThumb (AbstractScaleThumb* thumb);
		
	private:
		AbstractScale&			fScale;
		AbstractScaleThumb*		fThumb;
};

class	ScaleIndicator_MacUI : public AbstractScaleIndicator {
	public:
		ScaleIndicator_MacUI (AbstractScale& abstractScale);
		~ScaleIndicator_MacUI ();
	
	private:
		ScaleThumb_MacUI	fThumb_Mac;
};

class	ScaleIndicator_MotifUI : public AbstractScaleIndicator {
	public:
		ScaleIndicator_MotifUI (AbstractScale& abstractScale);
		~ScaleIndicator_MotifUI ();

	private:
		ScaleThumb_MotifUI	fThumb_Motif;
};

/*
 ********************************************************************************
 ********************************** AbstractScale *******************************
 ********************************************************************************
 */
AbstractScale::AbstractScale (SliderController* sliderController) :
	Slider (sliderController),
	fMin (1),
	fMax (100),
	fValue (1),
	fSensitive (True),
	fThumbLength (24),
	fIndicator (Nil)
{
}

Real	AbstractScale::GetMin_ () const
{
	return (fMin);
}

Real	AbstractScale::GetMax_ () const
{
	return (fMax);
}

void	AbstractScale::SetBounds_ (Real newMin, Real newMax, Real newValue, Panel::UpdateMode update)
{
	fMin = newMin;
	fMax = newMax;
	fValue = newValue;
	InvalidateLayout ();
	Refresh (update);
}

Real	AbstractScale::GetValue_ () const
{
	return (fValue);
}

void	AbstractScale::SetValue_ (Real newValue, Panel::UpdateMode update)
{
	if (fValue != newValue) {
		if (update != eNoUpdate) {
			Refresh (GetTextRect (), update);
		}
		fValue = newValue;
		InvalidateLayout ();
		GetIndicator ().InvalidateLayout ();
		if (update != eNoUpdate) {
			ProcessLayout ();
			Refresh (GetTextRect (), update);
		}
	}
	Ensure (fValue == newValue);
}
	
Point::Direction	AbstractScale::GetOrientation () const
{
	if (GetSize ().GetV () > GetSize ().GetH ()) {
		return (Point::eVertical);
	}
	return (Point::eHorizontal);
}
		
void	AbstractScale::Layout ()
{
	const	Coordinate	kHorizontalMargin = 5;
	const	Coordinate	kVerticalMargin = 10;
	
	Point	origin = kZeroPoint;
	if (GetOrientation () == Point::eVertical) {
		origin = Point (kVerticalMargin,  kHorizontalMargin + TextWidth ("99"));
	}
	else {
		Font f = GetEffectiveFont ();
		origin = Point (f.GetFontHeight () + kVerticalMargin, kHorizontalMargin);
	}
	Point size = GetSize () - origin - Point (kVerticalMargin, kHorizontalMargin);
	GetIndicator ().SetExtent (Rect (origin, size));

	Slider::Layout ();
}
		
Coordinate	AbstractScale::GetThumbLength () const
{
	return (fThumbLength);
}

void	AbstractScale::SetThumbLength (Coordinate length, Panel::UpdateMode update)
{
	if (fThumbLength != length) {
		fThumbLength = length;
		InvalidateLayout ();
		GetIndicator ().InvalidateLayout ();
		Refresh (update);
	}
	Ensure (fThumbLength == length);
}
	
Boolean	AbstractScale::GetSensitive () const
{
	return (fSensitive);
}

void	AbstractScale::SetSensitive (Boolean sensitive)
{
	fSensitive = sensitive;
}

Boolean	AbstractScale::GetLive () const
{
	return (Boolean (GetSensitive () and Slider::GetLive ()));
}
		
Point	AbstractScale::CalcDefaultSize_ (const Point& hint) const
{
	const	Coordinate	kHorizontalMargin = 5;
	const	Coordinate	kVerticalMargin = 10;
	const	Coordinate	kDefaultScaleLength = 200;
	const	Coordinate	kDefaultScaleHeight = 22;
	
	if (GetOrientation () == Point::eVertical) {
		Coordinate	hPadding = kHorizontalMargin*2 + TextWidth ("99");
		Coordinate	vPadding = kVerticalMargin*2;
		return (Point (kDefaultScaleLength, kDefaultScaleHeight) + Point (vPadding, hPadding));
	}
	else {
		Font f = GetEffectiveFont ();

		Coordinate	hPadding = kHorizontalMargin*2;
		Coordinate	vPadding = kVerticalMargin*2 + f.GetFontHeight ();
		return (Point (kDefaultScaleHeight, kDefaultScaleLength) + Point (vPadding, hPadding));
	}
}

Rect	AbstractScale::GetTextRect () const
{
	String value = ltostring (long (GetValue ()));
	Font f = GetEffectiveFont ();
	
	Rect	textRect = Rect (kZeroPoint, Point (f.GetFontHeight (), TextWidth (value)));

	if (GetOrientation () == Point::eVertical) {
		Point	centerPoint = GetIndicator ().GetOrigin () + CalcRectCenter (GetIndicator ().GetThumbExtent ()) - Point (0, 20);
		return (CenterRectAroundPoint (textRect, centerPoint));
	}
	else {
		Point	centerPoint = GetIndicator ().GetOrigin () + CalcRectCenter (GetIndicator ().GetThumbExtent ()) - Point (18, 0);
		return (CenterRectAroundPoint (textRect, centerPoint));
	}
}

Real	AbstractScale::CalcScalingFactor () const
{
	Point::Direction	d = GetOrientation ();
	Real	range = GetMax () - GetMin ();
	Coordinate	length = GetIndicator ().GetExtent ().InsetBy (GetBorder () + GetMargin ()).GetSize ()[d] - GetThumbLength ();
	return (range / Real (length));
}


void	AbstractScale::Draw (const Region& /*update*/)
{
	DrawText (ltostring (long (GetValue ())), GetTextRect ().GetOrigin ());
}

void	AbstractScale::SetBorder_ (const Point& border, Panel::UpdateMode updateMode)
{
	View::SetBorder_ (border, eNoUpdate);
	GetIndicator ().SetBorder (border, updateMode);
}

void	AbstractScale::SetMargin_ (const Point& margin, Panel::UpdateMode updateMode)
{
	View::SetMargin_ (margin, eNoUpdate);
	GetIndicator ().SetMargin (margin, updateMode);
}

void	AbstractScale::SetPlane_ (Bordered::Plane plane, Panel::UpdateMode update)
{
	View::SetPlane_ (plane, eNoUpdate);
	GetIndicator ().SetPlane (plane, update);
}

AbstractScaleIndicator&	AbstractScale::GetIndicator () const
{
	EnsureNotNil (fIndicator);
	return (*fIndicator);
}

void	AbstractScale::SetIndicator (AbstractScaleIndicator* indicator)
{
	fIndicator = indicator;
}


Scale_MacUI_Portable::Scale_MacUI_Portable (SliderController* sliderController) :
	AbstractScale (sliderController)
{
	AbstractScaleIndicator* indicator = new ScaleIndicator_MacUI (*this);
	SetIndicator (indicator);
	AddSubView (indicator);

	SetBorder (1, 1, eNoUpdate);
	SetMargin (0, 0, eNoUpdate);
}
		
Scale_MacUI_Portable::~Scale_MacUI_Portable ()
{
	AbstractScaleIndicator* indicator = &GetIndicator ();
	AssertNotNil (indicator);
	RemoveSubView (indicator);
	SetIndicator (Nil);
}




Scale_MotifUI_Portable::Scale_MotifUI_Portable (SliderController* sliderController) :
	AbstractScale (sliderController)
{
	AbstractScaleIndicator* indicator = new ScaleIndicator_MotifUI (*this);
	SetIndicator (indicator);
	AddSubView (indicator);

	SetBorder (2, 2, eNoUpdate);
	SetMargin (0, 0, eNoUpdate);
	SetPlane (Bordered::eIn, eNoUpdate);
}
		
Scale_MotifUI_Portable::~Scale_MotifUI_Portable ()
{
	AbstractScaleIndicator* indicator = &GetIndicator ();
	AssertNotNil (indicator);
	RemoveSubView (indicator);
	SetIndicator (Nil);
}




Scale_WinUI_Portable::Scale_WinUI_Portable (SliderController* sliderController) :
	AbstractScale (sliderController)
{
	AbstractScaleIndicator* indicator = new ScaleIndicator_MotifUI (*this);
	SetIndicator (indicator);
	AddSubView (indicator);

	SetBorder (2, 2, eNoUpdate);
	SetMargin (0, 0, eNoUpdate);
	SetPlane (Bordered::eIn, eNoUpdate);
}
		
Scale_WinUI_Portable::~Scale_WinUI_Portable ()
{
	AbstractScaleIndicator* indicator = &GetIndicator ();
	AssertNotNil (indicator);
	RemoveSubView (indicator);
	SetIndicator (Nil);
}




Scale::Scale (SliderController* sliderController) :
#if		qMacUI
	Scale_MacUI_Portable (sliderController)
#elif	qMotifUI
	Scale_MotifUI_Portable (sliderController)
#elif	qWinUI
	Scale_WinUI_Portable (sliderController)
#endif
{
}


Scale::Scale () :
#if		qMacUI
	Scale_MacUI_Portable (Nil)
#elif		qMotifUI
	Scale_MotifUI_Portable (Nil)
#elif	qWinUI
	Scale_WinUI_Portable (Nil)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
Scale::~Scale ()
{
}
#endif


/*
 ********************************************************************************
 **************************** AbstractScaleIndicator ****************************
 ********************************************************************************
 */
AbstractScaleIndicator::AbstractScaleIndicator (AbstractScale& abstractScale) :
	View (),
	fScale (abstractScale),
	fThumb (Nil)
{
	Tile t = PalletManager::Get ().MakeTileFromColor (kDarkGrayColor);
	SetBackground (&t, eNoUpdate);
}

AbstractScaleIndicator::~AbstractScaleIndicator ()
{
}

AbstractScaleThumb&	AbstractScaleIndicator::GetThumb () const
{
	EnsureNotNil (fThumb);
	return (*fThumb);
}

void	AbstractScaleIndicator::SetThumb (AbstractScaleThumb* thumb)
{
	fThumb = thumb;
}


Rect	AbstractScaleIndicator::GetThumbExtent () const
{
	return (GetThumb ().GetExtent ());
}

void	AbstractScaleIndicator::Layout ()
{
	Point::Direction	d = fScale.GetOrientation ();
	Point::Direction	otherD = (d == Point::eVertical) ? Point::eHorizontal : Point::eVertical;

	Rect	extent = InsetBy (GetLocalExtent (), GetBorder () + GetMargin ());
	
	Point	thumbSize;
	thumbSize.SetVH (d, fScale.GetThumbLength ());
	thumbSize.SetVH (otherD, extent.GetSize ()[otherD]);
	GetThumb ().SetSize (thumbSize);

	Real	length = extent.GetSize ()[d];
	Real	range = fScale.GetMax () - fScale.GetMin ();
	
	Point	thumbOrigin;
	thumbOrigin.SetVH (otherD, extent.GetOrigin ()[otherD]);
	
	thumbOrigin.SetVH (d, extent.GetOrigin ()[d] + (length - thumbSize[d]) * (fScale.GetValue () - fScale.GetMin ()) / range);
	GetThumb ().SetOrigin (thumbOrigin);
	
	View::Layout ();
}

void	AbstractScaleIndicator::SetBorder_ (const Point& border, Panel::UpdateMode updateMode)
{
	View::SetBorder_ (border, updateMode);
	GetThumb ().SetBorder (border, updateMode);
}

void	AbstractScaleIndicator::SetMargin_ (const Point& margin, Panel::UpdateMode updateMode)
{
	View::SetMargin_ (margin, updateMode);
	GetThumb ().SetMargin (margin, updateMode);
}

void	AbstractScaleIndicator::SetPlane_ (Bordered::Plane plane, Panel::UpdateMode updateMode)
{
	View::SetPlane_ (plane, updateMode);
	Bordered::Plane otherPlane = (plane == Bordered::eIn) ? Bordered::eOut : Bordered::eIn;
	GetThumb ().SetPlane (otherPlane, updateMode);
}



ScaleIndicator_MacUI::ScaleIndicator_MacUI (AbstractScale& abstractScale) :
	AbstractScaleIndicator (abstractScale),
	fThumb_Mac (abstractScale)
{
	SetThumb (&fThumb_Mac);
	AddSubView (&fThumb_Mac);
}

ScaleIndicator_MacUI::~ScaleIndicator_MacUI ()
{
	SetThumb (Nil);
	RemoveSubView (&fThumb_Mac);
}


ScaleIndicator_MotifUI::ScaleIndicator_MotifUI (AbstractScale& abstractScale) :
	AbstractScaleIndicator (abstractScale),
	fThumb_Motif (abstractScale)
{
	SetThumb (&fThumb_Motif);
	AddSubView (&fThumb_Motif);
}

ScaleIndicator_MotifUI::~ScaleIndicator_MotifUI ()
{
	SetThumb (Nil);
	RemoveSubView (&fThumb_Motif);
}


/*
 ********************************************************************************
 ********************************** ScaleThumb **********************************
 ********************************************************************************
 */
AbstractScaleThumb::AbstractScaleThumb (AbstractScale& abstractScale) :
	View (),
	fScale (abstractScale)
{
	Tile t = PalletManager::Get ().MakeTileFromColor (kGrayColor);
	SetBackground (&t, eNoUpdate);
}
		
Boolean	AbstractScaleThumb::TrackPress (const MousePressInfo& mouseInfo)
{
	ScaleThumbDragger dragger = ScaleThumbDragger (fScale);
	dragger.TrackPress (MousePressInfo (mouseInfo, LocalToAncestor (mouseInfo.fPressAt, &fScale)));
	return (True);
}




ScaleThumb_MacUI::ScaleThumb_MacUI (AbstractScale& abstractScale) :
	AbstractScaleThumb (abstractScale)
{
}

void	ScaleThumb_MacUI::Draw (const Region& /*update*/)
{
	DrawMacBorder (GetLocalExtent (), GetBorder ());
}



ScaleThumb_MotifUI::ScaleThumb_MotifUI (AbstractScale& abstractScale) :
	AbstractScaleThumb (abstractScale)
{
}

void	ScaleThumb_MotifUI::Draw (const Region& /*update*/)
{
	Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawMotifBorder (GetLocalExtent (), GetBorder (), c, GetPlane ());
}




/*
 ********************************************************************************
 ******************************* ScaleThumbDragger ******************************
 ********************************************************************************
 */
ScaleThumbDragger::ScaleThumbDragger (AbstractScale& abstractScale) :
	Tracker (abstractScale, *abstractScale.GetTablet (), abstractScale.LocalToTablet (kZeroPoint)),
	fScale (abstractScale),
	fOldValue (abstractScale.GetValue ()),
	fNewValue (abstractScale.GetValue ()),
	fScalingFactor (fScale.CalcScalingFactor ())
{
	SetHysteresis (kZeroPoint);
	SetTimeOut (0);
}

void	ScaleThumbDragger::TrackConstrain (Tracker::TrackPhase /*phase*/, const Point& /*anchor*/, const Point& /*previous*/, Point& /*next*/)
{
}

void	ScaleThumbDragger::TrackFeedback (Tracker::TrackPhase phase, const Point& anchor, const Point& /*previous*/, const Point& next)
{
	if (phase == eTrackMove) {
		Point	delta = next - anchor;
		fNewValue = fOldValue + delta[fScale.GetOrientation ()] * fScalingFactor;
		fNewValue = Max ((long double)fNewValue, (long double)fScale.GetMin ());
		fNewValue = Min ((long double)fNewValue, (long double)fScale.GetMax ());
		
		fScale.SetValue (fNewValue);
		fScale.Update ();
	}
}

Boolean	ScaleThumbDragger::TrackResult (Tracker::TrackPhase /*phase*/, const Point& /*anchor*/, const Point& /*previous*/, const Point& /*next*/)
{
	return (False);
}

