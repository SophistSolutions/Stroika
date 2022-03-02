/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__StringMenuItem__
#define	__StringMenuItem__

/*
 * $Header: /fuji/lewis/RCS/StringMenuItem.hh,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: StringMenuItem.hh,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/16  16:26:52  sterling
 *		made portable mac code invert properly when tracking
 *
 *		Revision 1.12  1992/06/03  21:16:24  lewis
 *		Made GetDisplayName () const.
 *
 *		Revision 1.10  92/03/28  03:26:31  03:26:31  lewis (Lewis Pringle)
 *		Added EffectiveFontChanged ().
 *		
 *		Revision 1.8  1992/03/09  23:58:59  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.7  1992/02/14  23:29:27  lewis
 *		Got rid of workaround for qMPW_SymbolTableOverflowProblem.
 *
 *		Revision 1.6  1992/02/06  20:57:39  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.5  1992/02/04  17:04:55  lewis
 *		override IsNativeItem () for StringMenuItem_MacUI_Native to say we are a native item (used in
 *		AbstractMenu).
 *
 *		Revision 1.4  1992/02/04  05:05:39  lewis
 *		Added dtor  ~StringMenuItem_MotifUI_Native ();
 *
 *		Revision 1.3  1992/02/04  03:50:38  lewis
 *		Add native / intermediate gui classes, and StringMenuItem derived class to replace old #define/
 *		typedef mechanism.
 *
 *		Revision 1.1  1992/02/03  22:29:38  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"MenuItem.hh"







class	AbstractStringMenuItem : public MenuItem {
	public:
		AbstractStringMenuItem (CommandNumber commandNumber);	
	
		nonvirtual	Boolean	GetExtended () const;
		nonvirtual	void	SetExtended (Boolean extendedName);
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

		virtual		void	SetExtended_ (Boolean extendedName);
		virtual		String	GetDisplayName () const;
	
	private:
		Boolean		fExtended;
};



class	StringMenuItem_MacUI : public AbstractStringMenuItem {
	public:
		enum Mark {
			eNoMark			=	0,		// From <Fonts.h>
			eCommandMark	=	17,		// From <Fonts.h>
			eCheckMark		=	18,		// From <Fonts.h>
			eDiamondMark	=	19,		// From <Fonts.h>
			eAppleMark		=	20,		// From <Fonts.h>
		};

		StringMenuItem_MacUI (CommandNumber commandNumber);	

		nonvirtual	Mark	GetMark () const;
		nonvirtual	void	SetMark (Mark mark);

	protected:	
		virtual		Mark	GetMark_ () const			=	Nil;
		virtual		void	SetMark_ (Mark mark)		=	Nil;
};




class	StringMenuItem_MotifUI : public AbstractStringMenuItem {
	public:
		StringMenuItem_MotifUI (CommandNumber commandNumber);
};






#if		qMacToolkit

class	StringMenuItem_MacUI_Native : public StringMenuItem_MacUI {
	public:
		StringMenuItem_MacUI_Native (CommandNumber commandNumber);	

		override	void	Draw (const Region& update);
		
	protected:	
		override	Boolean	Track (Tracker::TrackPhase phase, Boolean mouseInButton);
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

		override	void	UpdateOSRep_ ();

		override	Mark	GetMark_ () const;
		override	void	SetMark_ (Mark mark);

	private:
		override	Boolean	IsNativeItem () const;

	private:
		Mark		fMark;
};

#elif	qXmToolkit

class	StringMenuItem_MotifUI_Native : public StringMenuItem_MotifUI {
	public:
		StringMenuItem_MotifUI_Native (CommandNumber commandNumber);
		~StringMenuItem_MotifUI_Native ();

	protected:
		override 	void		Realize (osWidget* parent);
		override	void		UnRealize ();
		override	osWidget*	GetWidget () const;
	private:
		osWidget*	fWidget;
	protected:
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);
		override	void	UpdateOSRep_ ();
};

#endif	/*Toolkit*/




class	StringMenuItem : public
#if		qMacToolkit
	StringMenuItem_MacUI_Native
#elif	qXmToolkit
	StringMenuItem_MotifUI_Native
#endif
	{
		public:
			StringMenuItem (CommandNumber commandNumber);
};








// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__StringMenuItem__*/
