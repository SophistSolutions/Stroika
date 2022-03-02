/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__MenuBarMainView__
#define	__MenuBarMainView__

/*
 * $Header: /fuji/lewis/RCS/MenuBarMainView.hh,v 1.2 1992/06/25 05:49:38 sterling Exp $
 *
 * Description:
 *
 * Notes:
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: MenuBarMainView.hh,v $
 *		Revision 1.2  1992/06/25  05:49:38  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.3  1992/04/24  08:53:59  lewis
 *		Added CalcDefaultSize () method.
 *
 *		Revision 1.2  92/03/05  22:57:07  22:57:07  lewis (Lewis Pringle)
 *		Did first cut implementation of the class.
 *		
 *		Revision 1.1  1992/03/02  22:51:44  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"View.hh"



// may have to do tracking, targets, etc...
class	AbstractMenuBar;
class	MenuBarMainView : public View {
	public:
		MenuBarMainView ();

		nonvirtual	AbstractMenuBar*	GetMenuBar () const;
		nonvirtual	void				SetMenuBar (AbstractMenuBar* menuBar);

		nonvirtual	View*				GetMainView () const;
		nonvirtual	void				SetMainView (View* mainView);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();

	private:
		AbstractMenuBar*	fMenuBar;
		View*				fMainView;
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


#endif	/*__MenuBarMainView__*/

