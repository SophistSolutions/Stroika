/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ScrollBar__
#define	__ScrollBar__

/*
 * $Header: /fuji/lewis/RCS/ScrollBar.hh,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: ScrollBar.hh,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/16  16:25:55  sterling
 *		renamed GUI to UI, made Mac_UI and Motif_UI variants
 *
 *		Revision 1.2  1992/06/25  06:31:44  sterling
 *		Lots of changes including new portable implementations of Mac/Motif ScrollBars.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.27  92/04/30  13:09:27  13:09:27  sterling (Sterling Wight)
 *		use AbstractBordered
 *		
 *		Revision 1.26  92/04/14  19:35:06  19:35:06  lewis (Lewis Pringle)
 *		Added virtual dtor for ScrollBar because of qSoleInlineVirtualsNotStripped.
 *		
 *		Revision 1.25  92/03/26  09:38:44  09:38:44  lewis (Lewis Pringle)
 *		Got rid of oldLive arg to EffectiveLiveChanged.
 *		
 *		Revision 1.23  1992/03/09  23:57:37  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.22  1992/03/05  20:07:26  sterling
 *		used Bordered
 *
 *		Revision 1.21  1992/02/21  19:55:12  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.20  1992/02/15  04:50:37  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.19  1992/02/03  21:49:55  sterling
 *		massive (but trivial) interface changes inherited from Slider
 *
 *		Revision 1.18  1992/02/03  17:23:29  lewis
 *		Use class for ScrollBar rather than #define or typedef.
 *
 *		Revision 1.17  1992/01/29  04:37:29  sterling
 *		eliminated typedef inherited from Mac due to compiler bug
 *
 *		Revision 1.16  1992/01/27  07:52:15  lewis
 *		renamed from MacUI to MacUI.
 *		Also, added calcdefaultsize for motif gui.
 *		Deleted backwards compat stuff for 2.0 and g++.
 *
 *		Revision 1.15  1992/01/24  17:52:34  lewis
 *		Get rid of old g++ and 2.0 compat (pure virtuals).
 *		Added Realize/Unrealize overrides to motif scrollbar.
 *
 *		Revision 1.12  1992/01/10  08:56:35  lewis
 *		Added SetEnabled override to mac and motif implementations.
 *
 *		Revision 1.11  1992/01/08  06:07:46  lewis
 *		Sterl - added CalcDefaultSize to Strollbar_MacUI.
 *
 *		Revision 1.10  1992/01/07  04:01:53  lewis
 *		Got rid of portable implementations from headers since never actually implemented
 *		and there is nothing non-obvious to the declarations.
 *
 *
 *
 */

#include	"ArrowButton.hh"
#include	"Slider.hh"



class	AbstractScrollBar : public Slider {
	public:
		enum Orientation { eVertical, eHorizontal};

	protected:
		AbstractScrollBar (SliderController* sliderController);
	
	public:
		nonvirtual	Real	GetStepSize () const;
		nonvirtual	Real	GetPageSize () const;
		nonvirtual	void	SetSteps (Real newStepSize, Real newPageSize, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	SetSteps (UpdateMode update = eDelayedUpdate);
		nonvirtual	Real	GetDisplayed () const;
		nonvirtual	void	SetDisplayed (Real amountDisplayed, UpdateMode updateMode = eDelayedUpdate);
		
		nonvirtual	AbstractScrollBar::Orientation	GetOrientation () const;
		nonvirtual	void	SetOrientation (AbstractScrollBar::Orientation orientation, UpdateMode updateMode = eDelayedUpdate);
		
	protected:
		virtual	Real	GetStepSize_ () const												= Nil;
		virtual	Real	GetPageSize_ () const												= Nil;
		virtual	void	SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode update)	= Nil;
		virtual	Real	GetDisplayed_ () const										= Nil;
		virtual	void	SetDisplayed_ (Real amountDisplayed, UpdateMode updateMode)	= Nil;
		virtual	AbstractScrollBar::Orientation	GetOrientation_ () const = Nil;
		virtual	void	SetOrientation_ (AbstractScrollBar::Orientation orientation, UpdateMode updateMode) = Nil;
		
#if		qDebug
		override	void	Invariant_ () const;
#endif
};


class	AbstractScrollBar_MacUI : public AbstractScrollBar {
	protected:
		AbstractScrollBar_MacUI (SliderController* sliderController);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};

class	AbstractScrollBar_MotifUI : public AbstractScrollBar {
	protected:
		AbstractScrollBar_MotifUI (SliderController* sliderController);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};


class	AbstractScrollBar_WinUI : public AbstractScrollBar {
	protected:
		AbstractScrollBar_WinUI (SliderController* sliderController);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};


class	AbstractThumb : public View {
	protected:
		AbstractThumb ();
};

class	AbstractThumb_MacUI : public AbstractThumb {
	protected:
		AbstractThumb_MacUI ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};

class	AbstractThumb_MotifUI : public AbstractThumb {
	protected:
		AbstractThumb_MotifUI ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};

class	AbstractThumb_WinUI : public AbstractThumb {
	protected:
		AbstractThumb_WinUI ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};

class	Thumb_MacUI_Portable : public AbstractThumb_MacUI {
	public:
		Thumb_MacUI_Portable ();
				

	protected:
		override	void	Draw (const Region& update);
};

class	Thumb_MotifUI_Portable : public AbstractThumb_MotifUI {
	public:
		Thumb_MotifUI_Portable ();
				

	protected:
		override	void	Draw (const Region& update);
};

class	Thumb_WinUI_Portable : public AbstractThumb_WinUI {
	public:
		Thumb_WinUI_Portable ();
				

	protected:
		override	void	Draw (const Region& update);
};


class	ScrollBar_MacUI_Portable : public AbstractScrollBar_MacUI, public ButtonController {
	public:
		ScrollBar_MacUI_Portable (AbstractScrollBar::Orientation orientation, SliderController* sliderController);
		~ScrollBar_MacUI_Portable ();

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	void	Draw (const Region& update);

		nonvirtual	Rect	GetTrough () const;

	protected:
		override	void	Layout ();

		override	Real	GetMin_ () const;
		override	Real	GetMax_ () const;
		override	void	SetBounds_ (Real newMin, Real newMax, Real newValue, UpdateMode update);
		override	Real	GetValue_ () const;
		override	void	SetValue_ (Real newValue, UpdateMode updateMode);
		override	Real	GetStepSize_ () const;
		override	Real	GetPageSize_ () const;
		override	void	SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode update);
		override	Real	GetDisplayed_ () const;
		override	void	SetDisplayed_ (Real amountDisplayed, UpdateMode updateMode);
		override	AbstractScrollBar::Orientation	GetOrientation_ () const;
		override	void	SetOrientation_ (AbstractScrollBar::Orientation orientation, UpdateMode updateMode);

		override	void	SetBorder_ (const Point& border, UpdateMode updateMode); 
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode); 
		
		override	void	ButtonPressed (AbstractButton* button);
		override	Boolean	ButtonTracking (AbstractButton* button, Tracker::TrackPhase phase, Boolean mouseInButton);

		nonvirtual	void	PlaceThumb (Real value, UpdateMode updateMode);
		virtual		void	ChangeValue (Real newValue);
		
		nonvirtual	AbstractThumb&	GetThumb () const;
		nonvirtual	AbstractArrowButton&	GetUpArrow () const;
		nonvirtual	AbstractArrowButton&	GetDownArrow () const;

	private:
		Real					fMin;
		Real					fMax;
		Real					fValue;
		Real					fStepSize;
		Real					fPageSize;
		Real					fDisplayed;
		AbstractArrowButton*	fUpArrow;
		AbstractArrowButton*	fDownArrow;
		AbstractThumb*			fThumb;
		Orientation				fOrientation;
};

#if		qMacToolkit
class	MacOSScrollBar;
class	ScrollBar_MacUI_Native : public AbstractScrollBar_MacUI {
	public:
		ScrollBar_MacUI_Native (AbstractScrollBar::Orientation orientation, SliderController* sliderController);
		~ScrollBar_MacUI_Native ();

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	void	Draw (const Region& update);

	protected:
		override	void	Layout ();

		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);

		override	Real	GetMin_ () const;
		override	Real	GetMax_ () const;
		override	void	SetBounds_ (Real newMin, Real newMax, Real newValue, UpdateMode update);
		override	Real	GetValue_ () const;
		override	void	SetValue_ (Real newValue, UpdateMode updateMode);
		override	Real	GetStepSize_ () const;
		override	Real	GetPageSize_ () const;
		override	void	SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode update);
		override	Real	GetDisplayed_ () const;
		override	void	SetDisplayed_ (Real amountDisplayed, UpdateMode updateMode);

		override	AbstractScrollBar::Orientation	GetOrientation_ () const;
		override	void	SetOrientation_ (AbstractScrollBar::Orientation orientation, UpdateMode updateMode);

		override	void	SetBorder_ (const Point& border, UpdateMode updateMode); 
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode); 

		/* override (and usually call inherited) to provide behavior while tracking */
		virtual	void	ActionProc (short partCode);


	private:
		MacOSScrollBar*		fMacSBar;
		Real				fMin;
		Real				fMax;
		Real				fValue;
		Real				fConversionFactor;		// (max-min)/kMaxUInt16 - roughly...
		Real				fStepSize;
		Real				fPageSize;
		Real				fDisplayed;
		Orientation			fOrientation;
	friend	MacOSScrollBar;
};

#endif	/*qMacToolkit*/




class	ScrollBar_MotifUI_Portable : public AbstractScrollBar_MotifUI, public ButtonController {
	public:
		ScrollBar_MotifUI_Portable (AbstractScrollBar::Orientation orientation, SliderController* sliderController);
		~ScrollBar_MotifUI_Portable ();

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	void	Draw (const Region& update);

		nonvirtual	Rect	GetTrough () const;

	protected:
		override	void	Layout ();

		override	Real	GetMin_ () const;
		override	Real	GetMax_ () const;
		override	void	SetBounds_ (Real newMin, Real newMax, Real newValue, UpdateMode update);
		override	Real	GetValue_ () const;
		override	void	SetValue_ (Real newValue, UpdateMode updateMode);
		override	Real	GetStepSize_ () const;
		override	Real	GetPageSize_ () const;
		override	void	SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode update);
		override	Real	GetDisplayed_ () const;
		override	void	SetDisplayed_ (Real amountDisplayed, UpdateMode updateMode);
		override	AbstractScrollBar::Orientation	GetOrientation_ () const;
		override	void	SetOrientation_ (AbstractScrollBar::Orientation orientation, UpdateMode updateMode);

		override	void	SetBorder_ (const Point& border, UpdateMode updateMode); 
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode); 
		
		override	void	ButtonPressed (AbstractButton* button);
		override	Boolean	ButtonTracking (AbstractButton* button, Tracker::TrackPhase phase, Boolean mouseInButton);

		nonvirtual	void	PlaceThumb (Real value, UpdateMode updateMode);
		virtual		void	ChangeValue (Real newValue);
		
		nonvirtual	AbstractThumb&	GetThumb () const;
		nonvirtual	AbstractArrowButton&	GetUpArrow () const;
		nonvirtual	AbstractArrowButton&	GetDownArrow () const;

	private:
		Real					fMin;
		Real					fMax;
		Real					fValue;
		Real					fStepSize;
		Real					fPageSize;
		Real					fDisplayed;
		AbstractArrowButton*	fUpArrow;
		AbstractArrowButton*	fDownArrow;
		AbstractThumb*			fThumb;
		Orientation				fOrientation;
};



#if		qXmToolkit

class	ScrollBar_MotifUI_Native : public AbstractScrollBar_MotifUI {
	public:
		ScrollBar_MotifUI_Native (AbstractScrollBar::Orientation orientation, SliderController* sliderController = Nil);
		~ScrollBar_MotifUI_Native ();

	protected:
		override	void	Layout ();

		override	void	SetSize_ (const Point& newSize, UpdateMode update);
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);

		override	Real	GetMin_ () const;
		override	Real	GetMax_ () const;
		override	void	SetBounds_ (Real newMin, Real newMax, Real newValue, UpdateMode update);
		override	Real	GetValue_ () const;
		override	void	SetValue_ (Real newValue, UpdateMode updateMode);
		override	Real	GetStepSize_ () const;
		override	Real	GetPageSize_ () const;
		override	void	SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode update);
		override	Real	GetDisplayed_ () const;
		override	void	SetDisplayed_ (Real amountDisplayed, UpdateMode updateMode);
		override	AbstractScrollBar::Orientation	GetOrientation_ () const;
		override	void	SetOrientation_ (AbstractScrollBar::Orientation orientation, UpdateMode updateMode);

		override	void	Realize (osWidget* parent);
		override	void	UnRealize ();

		override	void	SetBorder_ (const Point& border, UpdateMode updateMode);
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode);
		override	void	SetPlane_ (AbstractBordered::Plane plane, UpdateMode update);

	private:
		typedef	AbstractScrollBar_MotifUI	inherited;
		class	MotifOSControl;
		MotifOSControl*		fOSControl;
		Real				fMin;
		Real				fMax;
		Real				fValue;
		Real				fConversionFactor;		// (max-min)/kMaxUInt16 - roughly...
		Real				fStepSize;
		Real				fPageSize;
		Real				fDisplayed;
		Orientation			fOrientation;
};

#endif	/*qXmToolkit*/


class	ScrollBar_WinUI_Portable : public AbstractScrollBar_WinUI, public ButtonController {
	public:
		ScrollBar_WinUI_Portable (AbstractScrollBar::Orientation orientation, SliderController* sliderController);
		~ScrollBar_WinUI_Portable ();

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	void	Draw (const Region& update);

		nonvirtual	Rect	GetTrough () const;

	protected:
		override	void	Layout ();

		override	Real	GetMin_ () const;
		override	Real	GetMax_ () const;
		override	void	SetBounds_ (Real newMin, Real newMax, Real newValue, UpdateMode update);
		override	Real	GetValue_ () const;
		override	void	SetValue_ (Real newValue, UpdateMode updateMode);
		override	Real	GetStepSize_ () const;
		override	Real	GetPageSize_ () const;
		override	void	SetSteps_ (Real newStepSize, Real newPageSize, UpdateMode update);
		override	Real	GetDisplayed_ () const;
		override	void	SetDisplayed_ (Real amountDisplayed, UpdateMode updateMode);
		override	AbstractScrollBar::Orientation	GetOrientation_ () const;
		override	void	SetOrientation_ (AbstractScrollBar::Orientation orientation, UpdateMode updateMode);

		override	void	SetBorder_ (const Point& border, UpdateMode updateMode); 
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode); 
		
		override	void	ButtonPressed (AbstractButton* button);
		override	Boolean	ButtonTracking (AbstractButton* button, Tracker::TrackPhase phase, Boolean mouseInButton);

		nonvirtual	void	PlaceThumb (Real value, UpdateMode updateMode);
		virtual		void	ChangeValue (Real newValue);
		
		nonvirtual	AbstractThumb&	GetThumb () const;
		nonvirtual	AbstractArrowButton&	GetUpArrow () const;
		nonvirtual	AbstractArrowButton&	GetDownArrow () const;

	private:
		Real					fMin;
		Real					fMax;
		Real					fValue;
		Real					fStepSize;
		Real					fPageSize;
		Real					fDisplayed;
		AbstractArrowButton*	fUpArrow;
		AbstractArrowButton*	fDownArrow;
		AbstractThumb*			fThumb;
		Orientation				fOrientation;
};



class	ScrollBar_MacUI : public
#if		qNative && qMacToolkit
	ScrollBar_MacUI_Native
#else
	ScrollBar_MacUI_Portable
#endif
	{
		public:
			ScrollBar_MacUI (AbstractScrollBar::Orientation orientation, SliderController* sliderController);

#if		qSoleInlineVirtualsNotStripped
			virtual ~ScrollBar_MacUI ();
#endif
};




class	ScrollBar_MotifUI : public
#if		qNative && qXmToolkit
	ScrollBar_MotifUI_Native
#else
	ScrollBar_MotifUI_Portable
#endif
	{
		public:
			ScrollBar_MotifUI (AbstractScrollBar::Orientation orientation, SliderController* sliderController);

#if		qSoleInlineVirtualsNotStripped
			virtual ~ScrollBar_MotifUI ();
#endif
};



class	ScrollBar_WinUI : public
	ScrollBar_WinUI_Portable
	{
		public:
			ScrollBar_WinUI (AbstractScrollBar::Orientation orientation, SliderController* sliderController);

#if		qSoleInlineVirtualsNotStripped
			virtual ~ScrollBar_WinUI ();
#endif
};




class	ScrollBar : public
#if		qMacUI
	ScrollBar_MacUI
#elif	qMotifUI
	ScrollBar_MotifUI
#elif	qWinUI
	ScrollBar_WinUI
#endif
	{
		public:
			ScrollBar (AbstractScrollBar::Orientation orientation, SliderController* sliderController = Nil);

#if		qSoleInlineVirtualsNotStripped
			virtual ~ScrollBar ();
#endif
};







/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__ScrollBar__*/

