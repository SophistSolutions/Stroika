/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ButtonMenuItem__
#define	__ButtonMenuItem__

/*
 * $Header: /fuji/lewis/RCS/ButtonMenuItem.hh,v 1.3 1992/07/21 19:59:01 sterling Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: ButtonMenuItem.hh,v $
 *		Revision 1.3  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.2  1992/06/25  05:12:20  sterling
 *		Use Toggle instead of Button - and some other changes to AbstractButtonMenuItem having
 *		to do with other button changes (LGP checking in and thats all I understood).
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.8  1992/04/20  14:19:59  lewis
 *		Use void* rather than caddr_t, since that is not POSIX.
 *
 *		Revision 1.7  92/03/24  02:28:50  02:28:50  lewis (Lewis Pringle)
 *		Made CalcDefaultSize () const.
 *		
 *		Revision 1.6  1992/03/09  23:42:27  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.5  1992/02/17  05:10:39  lewis
 *		Made callback proc member function for ButtonMenuItem_MotifUI_Native.
 *
 *		Revision 1.4  1992/02/14  23:23:28  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workarounds since apple fixed the bug.
 *		Also, added realize/unrealize etc overrides for motif native ButtenMenuitem.
 *
 *		Revision 1.3  1992/02/06  20:40:49  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.2  1992/02/04  03:44:45  lewis
 *		Add native / intermediate versions of ButtonMenuItem, and class ButtonMenuItem instead
 *		of #define or typedef.
 *
 *		Revision 1.1  1992/02/03  22:29:38  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"MenuItem.hh"








class	AbstractButtonMenuItem : public MenuItem {
	public:
		AbstractButtonMenuItem (Toggle* button, CommandNumber commandNumber);


	protected:
		nonvirtual	Toggle*	GetButton ();
		nonvirtual	void	SetButton (Toggle* newButton);

		override	void	SetEnabled_ (Boolean enabled, UpdateMode update);
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);
		override	void	Layout ();
		override	void	UpdateOSRep_ ();
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

		virtual		void	SetButton_ (Toggle* newButton);

	private:
		Toggle*	fButton;
};



class	ButtonMenuItem_MacUI : public AbstractButtonMenuItem {
	public:
		ButtonMenuItem_MacUI (Toggle* button, CommandNumber commandNumber);
};



class	ButtonMenuItem_MotifUI : public AbstractButtonMenuItem {
	public:
		ButtonMenuItem_MotifUI (Toggle* button, CommandNumber commandNumber);
};



#if		qMacToolkit
class	ButtonMenuItem_MacUI_Native : public ButtonMenuItem_MacUI {
	public:
		ButtonMenuItem_MacUI_Native (Toggle* button, CommandNumber commandNumber);
};
#elif	qXmToolkit
class	ButtonMenuItem_MotifUI_Native : public ButtonMenuItem_MotifUI {
	public:
		ButtonMenuItem_MotifUI_Native (Toggle* button, CommandNumber commandNumber);
		~ButtonMenuItem_MotifUI_Native ();

	protected:
		override 	void		Realize (osWidget* parent);
		override	void		UnRealize ();
		override	osWidget*	GetWidget () const;
	private:
		osWidget*	fWidget;
	protected:
		override	void	UpdateOSRep_ ();

	private:
		static	void 	sCallBackProc (osWidget* w, void* client_data, void* call_data);
};
#endif	/*Toolkit*/







class	ButtonMenuItem : public
#if		qMacToolkit
	ButtonMenuItem_MacUI_Native
#elif	qXmToolkit
	ButtonMenuItem_MotifUI_Native
#endif
	{
		public:
			ButtonMenuItem (Toggle* button, CommandNumber commandNumber);
};






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__ButtonMenuItem__*/

