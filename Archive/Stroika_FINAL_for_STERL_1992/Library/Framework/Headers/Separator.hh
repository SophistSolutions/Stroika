/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Separator__
#define	__Separator__

/*
 * $Header: /fuji/lewis/RCS/Separator.hh,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *		Simple class to draw a vertical or horizontal line, to seperate views. As simple
 * as this may sound, different GUIs do this differently, so to capture that in Dialogs,
 * and to allow the placement of these sepertors from the Stroika Dialog Editor, we define
 * a class to accomplish these. Again, these views do nothing, except display a line, and slightly
 * differntly among the GUIs.
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Separator.hh,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.2  1992/06/25  06:39:02  sterling
 *		Not sure what sterl changed - changed CalcDefaultSize to CalcDefaultSize_ + ?.
 *
 *		Revision 1.1  92/04/16  23:43:08  23:43:08  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *
 */


#include	"View.hh"





class	AbstractSeparator : public View {
	protected:
		AbstractSeparator ();
	public:
		~AbstractSeparator ();

		nonvirtual	Point::Direction	GetOrientation () const;
		nonvirtual	void				SetOrientation (Point::Direction direction, UpdateMode updateMode = eDelayedUpdate);

	protected:
		virtual	Point::Direction	GetOrientation_ () const												=	Nil;
		virtual	void				SetOrientation_ (Point::Direction direction, UpdateMode updateMode)	=	Nil;
};



class	AbstractSeparator_MacUI : public AbstractSeparator {
	protected:
		AbstractSeparator_MacUI ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};



class	Separator_MacUI_Portable : public AbstractSeparator_MacUI {
	public:
		Separator_MacUI_Portable (Point::Direction orientation);
		Separator_MacUI_Portable ();

	protected:
		override	void	Draw (const Region& update);

		override	Point::Direction	GetOrientation_ () const;
		override	void				SetOrientation_ (Point::Direction direction, UpdateMode updateMode);

	private:
		Point::Direction	fOrientation;
};



class	AbstractSeparator_MotifUI : public AbstractSeparator {
	public:
		enum Etching { eEtchedIn, eEtchedOut, };
		
	protected:
		AbstractSeparator_MotifUI ();

	public:
		nonvirtual	AbstractSeparator_MotifUI::Etching	GetEtching () const;
		nonvirtual	void								SetEtching (Etching etching, UpdateMode updateMode = eDelayedUpdate);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

		virtual	AbstractSeparator_MotifUI::Etching	GetEtching_ () const									= Nil;
		virtual	void								SetEtching_ (Etching etching, UpdateMode updateMode)	= Nil;
};



class	Separator_MotifUI_Portable : public AbstractSeparator_MotifUI {
	public:
		Separator_MotifUI_Portable (Point::Direction orientation);
		Separator_MotifUI_Portable ();

	protected:
		override	void	Draw (const Region& update);

		override	Point::Direction	GetOrientation_ () const;
		override	void				SetOrientation_ (Point::Direction direction, UpdateMode updateMode);

		override	AbstractSeparator_MotifUI::Etching	GetEtching_ () const;
		override	void	SetEtching_ (Etching etching, UpdateMode update);
	
	private:
		Point::Direction	fOrientation;
		Etching				fEtching;
};




class	AbstractSeparator_WinUI : public AbstractSeparator {
	protected:
		AbstractSeparator_WinUI ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};



class	Separator_WinUI_Portable : public AbstractSeparator_WinUI {
	public:
		Separator_WinUI_Portable (Point::Direction orientation);
		Separator_WinUI_Portable ();

	protected:
		override	void	Draw (const Region& update);

		override	Point::Direction	GetOrientation_ () const;
		override	void				SetOrientation_ (Point::Direction direction, UpdateMode updateMode);

	private:
		Point::Direction	fOrientation;
};



class	Separator_MacUI : public Separator_MacUI_Portable
{
	public:
		Separator_MacUI ();
		Separator_MacUI (Point::Direction orientation);

#if		qSoleInlineVirtualsNotStripped
		virtual ~Separator_MacUI ();
#endif
};




class	Separator_MotifUI : public Separator_MotifUI_Portable
{
	public:
		Separator_MotifUI ();
		Separator_MotifUI (Point::Direction orientation);

#if		qSoleInlineVirtualsNotStripped
		virtual ~Separator_MotifUI ();
#endif
};




class	Separator_WinUI : public Separator_WinUI_Portable
{
	public:
		Separator_WinUI ();
		Separator_WinUI (Point::Direction orientation);

#if		qSoleInlineVirtualsNotStripped
		virtual ~Separator_WinUI ();
#endif
};





/*
 * This is the GUI independent choice for how to build a Seperator view. This is what you should
 * use, most of the time to create a Seperator view.
 */
class	Separator : public
#if		qMacUI
	Separator_MacUI
#elif	qMotifUI
	Separator_MotifUI
#elif	qMotifUI
	Separator_WinUI
#endif	/*GUI*/
{
	public:
		Separator (Point::Direction orientation);
		Separator ();

#if		qSoleInlineVirtualsNotStripped
		virtual ~Separator ();
#endif
};







// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Separator__*/
