/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ArrowButton__
#define	__ArrowButton__

/*
 * $Header: /fuji/lewis/RCS/ArrowButton.hh,v 1.4 1992/07/21 19:59:01 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: ArrowButton.hh,v $
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/16  16:25:55  sterling
 *		renamed GUI to UI, made Mac_UI and Motif_UI variants
 *
 *	Revision 1.2  1992/07/04  14:04:07  lewis
 *	Declare fArrowShape as a shape rather than a Polygon (after I changed
 *	shape support to have Letter/Envelope) so I could assign some random shape
 *	initially - rather than the poly which I can only build later.
 *	
 *	Revision 1.1  1992/06/25  04:38:52  sterling
 *	Initial revision
 *	
 *
 *
 *
 */

#include	"Button.hh"
#include	"FocusItem.hh"
#include	"Shape.hh"




class	AbstractArrowButton : public Button {
	public:
		enum ArrowDirection {
			eLeft,
			eUp,
			eRight,
			eDown,
		};
		
	protected:
		AbstractArrowButton (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);

	public:
		nonvirtual	AbstractArrowButton::ArrowDirection	GetDirection () const;
		nonvirtual	void	SetDirection (AbstractArrowButton::ArrowDirection direction, UpdateMode updateMode = eDelayedUpdate);

	protected:
		virtual	void	SetDirection_ (AbstractArrowButton::ArrowDirection direction, UpdateMode updateMode);
	
	private:
		ArrowDirection	fDirection;
};

class	AbstractArrowButton_MacUI : public AbstractArrowButton {
	protected:
		AbstractArrowButton_MacUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};

class	ArrowButton_MacUI_Portable : public AbstractArrowButton_MacUI {
	public:
		ArrowButton_MacUI_Portable (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);
		
	protected:
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	void	Draw (const Region& update);

		override	void	SetDirection_ (AbstractArrowButton::ArrowDirection direction, UpdateMode updateMode);

		virtual		Rect	GetArrowBounds () const;
		
	private:
		Shape	fArrowShape;
};

class	AbstractArrowButton_MotifUI : public AbstractArrowButton {
	protected:
		AbstractArrowButton_MotifUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};

class	ArrowButton_MotifUI_Portable : public AbstractArrowButton_MotifUI {
	public:
		ArrowButton_MotifUI_Portable (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);
		
	protected:
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	void	Draw (const Region& update);

		override	void	SetDirection_ (AbstractArrowButton::ArrowDirection direction, UpdateMode updateMode);
		
		virtual		void	DrawArrow (Boolean pressed);
		nonvirtual	void	DrawMitredLine (Line& line, Coordinate thickness, const Point& fromOffset, const Point& toOffset, const Pen& pen);
		
	private:
		Shape	fArrowShape;
};



class	AbstractArrowButton_WinUI : public AbstractArrowButton {
	protected:
		AbstractArrowButton_WinUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};

class	ArrowButton_WinUI_Portable : public AbstractArrowButton_WinUI {
	public:
		ArrowButton_WinUI_Portable (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);
		
	protected:
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	void	Draw (const Region& update);

		override	void	SetDirection_ (AbstractArrowButton::ArrowDirection direction, UpdateMode updateMode);

		virtual		Rect	GetArrowBounds () const;
		
	private:
		Shape	fArrowShape;
};



class	ArrowButton_MacUI : public
	ArrowButton_MacUI_Portable
	{
		public:
			ArrowButton_MacUI (AbstractArrowButton::ArrowDirection direction);
			ArrowButton_MacUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~ArrowButton_MacUI ();
#endif
};




class	ArrowButton_MotifUI : public
	ArrowButton_MotifUI_Portable
	{
		public:
			ArrowButton_MotifUI (AbstractArrowButton::ArrowDirection direction);
			ArrowButton_MotifUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~ArrowButton_MotifUI ();
#endif
};



class	ArrowButton_WinUI : public
	ArrowButton_WinUI_Portable
	{
		public:
			ArrowButton_WinUI (AbstractArrowButton::ArrowDirection direction);
			ArrowButton_WinUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~ArrowButton_WinUI ();
#endif
};



class	ArrowButton : public
#if		qMacUI
	ArrowButton_MacUI
#elif	qMotifUI
	ArrowButton_MotifUI
#elif	qWinUI
	ArrowButton_WinUI
#endif
	{
		public:
			ArrowButton (AbstractArrowButton::ArrowDirection direction);
			ArrowButton (AbstractArrowButton::ArrowDirection direction, ButtonController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~ArrowButton ();
#endif
};


#endif /* __ArrowButton__ */
