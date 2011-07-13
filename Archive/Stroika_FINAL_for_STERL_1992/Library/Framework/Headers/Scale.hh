/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Scale__
#define	__Scale__

/*
 * $Header: /fuji/lewis/RCS/Scale.hh,v 1.4 1992/07/21 19:59:01 sterling Exp $
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
 *	$Log: Scale.hh,v $
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/16  16:25:55  sterling
 *		renamed GUI to UI, made Mac_UI and Motif_UI variants
 *
 *		Revision 1.2  1992/06/25  06:28:25  sterling
 *		react to slider changes? (LGP checked in).
 *
 *		Revision 1.1  1992/04/30  13:11:05  sterling
 *		Initial revision
 *
 *
 */
 
#include "Slider.hh"

class	AbstractScaleIndicator;
class	AbstractScale : public Slider {
	public:
		AbstractScale (SliderController* sliderController);

		override	Real	GetMin_ () const;
		override	Real	GetMax_ () const;
		override	void	SetBounds_ (Real newMin, Real newMax, Real newValue, UpdateMode update);
		override	Real	GetValue_ () const;
		override	void	SetValue_ (Real newValue, UpdateMode update);
		
		nonvirtual	Point::Direction	GetOrientation () const;
		
		nonvirtual	Coordinate	GetThumbLength () const;
		nonvirtual	void SetThumbLength (Coordinate length, UpdateMode update = eDelayedUpdate);
	
		nonvirtual	Boolean	GetSensitive () const;
		nonvirtual	void	SetSensitive (Boolean sensitive);
		
		override	Boolean	GetLive () const;
				
		nonvirtual	Rect	GetTextRect () const;
		nonvirtual	Real	CalcScalingFactor () const;
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Draw (const Region& update);
		override	void	Layout ();

		override	void	SetBorder_ (const Point& border, UpdateMode updateMode);
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode);
		override	void	SetPlane_ (Bordered::Plane plane, UpdateMode update);

		nonvirtual	AbstractScaleIndicator&	GetIndicator () const;
		nonvirtual	void					SetIndicator (AbstractScaleIndicator* indicator);
		
	private:
		Real				fMin;
		Real				fMax;
		Real				fValue;
		Boolean				fSensitive;
		Coordinate			fThumbLength;
		AbstractScaleIndicator*	fIndicator;
};


class	Scale_MacUI_Portable : public AbstractScale {
	public:
		Scale_MacUI_Portable (SliderController* sliderController);
		
		~Scale_MacUI_Portable ();
};

class	Scale_MotifUI_Portable : public AbstractScale {
	public:
		Scale_MotifUI_Portable (SliderController* sliderController);
		
		~Scale_MotifUI_Portable ();
};

class	Scale_WinUI_Portable : public AbstractScale {
	public:
		Scale_WinUI_Portable (SliderController* sliderController);
		
		~Scale_WinUI_Portable ();
};




class	Scale : public
#if		qMacUI
	Scale_MacUI_Portable
#elif	qMotifUI
	Scale_MotifUI_Portable
#elif	qWinUI
	Scale_WinUI_Portable
#endif
{
	public:
		Scale (SliderController* sliderController);
		Scale ();

#if		qSoleInlineVirtualsNotStripped
		virtual ~Scale ();
#endif
};



#endif	/* __Scale__ */
