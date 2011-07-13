/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SeparatorMenuItem__
#define	__SeparatorMenuItem__

/*
 * $Header: /fuji/lewis/RCS/SeparatorMenuItem.hh,v 1.4 1992/07/21 19:59:01 sterling Exp $
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
 *	$Log: SeparatorMenuItem.hh,v $
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/14  19:50:55  lewis
 *		Renamed Seperator->Separator.
 *
 *		Revision 1.2  1992/06/25  06:44:34  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.8  1992/03/09  23:58:34  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.6  1992/02/21  19:57:11  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.5  1992/02/06  20:53:24  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.4  1992/02/04  17:02:54  lewis
 *		override IsNativeItem () for SeperatorMenuItem to say that we are a native item.
 *
 *		Revision 1.3  1992/02/04  05:03:46  lewis
 *		Added destructor for ~SeperatorMenuItem_MotifUI_Native ().
 *
 *		Revision 1.2  1992/02/03  23:32:54  lewis
 *		Did abstract base classes, renamed real ones to append Native, and did final class instead of #define or
 *		typedef of SeperatorMenuItem.
 *
 *		Revision 1.1  1992/02/03  22:29:38  lewis
 *		Initial revision
 *
 *
 */

#include	"MenuItem.hh"






class	AbstractSeparatorMenuItem : public MenuItem {
	public:
		AbstractSeparatorMenuItem ();

	protected:
		override	void	SetEnabled_ (Boolean enabled, UpdateMode updateMode);
};



class	SeparatorMenuItem_MacUI : public AbstractSeparatorMenuItem {
	protected:
		SeparatorMenuItem_MacUI ();
};



class	SeparatorMenuItem_MotifUI : public AbstractSeparatorMenuItem {
	protected:
		SeparatorMenuItem_MotifUI ();
};





#if		qMacToolkit
class	SeparatorMenuItem_MacUI_Native : public SeparatorMenuItem_MacUI {
	public:
		SeparatorMenuItem_MacUI_Native ();

		override	void	Draw (const Region& update);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	UpdateOSRep_ ();

	private:
		override	Boolean	IsNativeItem () const;
};
#elif	qXmToolkit
class	SeparatorMenuItem_MotifUI_Native : public SeparatorMenuItem_MotifUI {
	public:
		SeparatorMenuItem_MotifUI_Native ();
		~SeparatorMenuItem_MotifUI_Native ();

	protected:
		override 	void		Realize (osWidget* parent);
		override	void		UnRealize ();
		override	osWidget*	GetWidget () const;
	private:
		osWidget*	fWidget;
};
#endif	/*Toolkit*/



class	SeparatorMenuItem : public
#if		qMacToolkit
	SeparatorMenuItem_MacUI_Native
#elif	qXmToolkit
	SeparatorMenuItem_MotifUI_Native
#endif
	{
		public:
			SeparatorMenuItem ();
};





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__SeparatorMenuItem__*/
