/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuBarMainView.cc,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: MenuBarMainView.cc,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/04/24  09:02:10  lewis
 *		Install MenuBar on construction, and iimplement CalcDefaultSize().
 *
 *		Revision 1.2  1992/03/05  22:58:32  lewis
 *		Did first cut implementation.
 *
 *		Revision 1.1  1992/03/02  22:52:10  lewis
 *		Initial revision
 *
 *
 */



#include	"StreamUtils.hh"

#include	"MenuBar.hh"

#include	"MenuBarMainView.hh"






/*
 ********************************************************************************
 ******************************** MenuBarMainView *******************************
 ********************************************************************************
 */
MenuBarMainView::MenuBarMainView ():
	View (),
	fMenuBar (Nil),
	fMainView (Nil)
{
}

AbstractMenuBar*	MenuBarMainView::GetMenuBar () const
{
	return (fMenuBar);
}

void	MenuBarMainView::SetMenuBar (AbstractMenuBar* menuBar)
{
	if (fMenuBar != Nil) {
		RemoveSubView (fMenuBar);
		fMenuBar->DeinstallMenu ();
		fMenuBar = Nil;
	}
	fMenuBar = menuBar;
	if (fMenuBar != Nil) {
		AddSubView (fMenuBar);
		fMenuBar->InstallMenu ();
	}
}

View*	MenuBarMainView::GetMainView () const
{
	return (fMainView);
}

void	MenuBarMainView::SetMainView (View* mainView)
{
	if (fMainView != Nil) {
		RemoveSubView (fMainView);
		fMainView = Nil;
	}
	fMainView = mainView;
	if (fMainView != Nil) {
		AddSubView (fMainView);
	}
}

Point	MenuBarMainView::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point	size			=	kZeroPoint;
	Point	menuBarSize		=	kZeroPoint;
	Point	mainViewSize	=	kZeroPoint;
	if (fMenuBar != Nil) {
		menuBarSize = fMenuBar->CalcDefaultSize ();
	}
	if (fMainView != Nil) {
		mainViewSize = fMainView->CalcDefaultSize ();
	}

	/*
	 * Start with the size of the main view. Then add in vertical hight of menubar,
	 * and make sure our size at least as wide as MBAR. If any dimension turns out to be
	 * zero, use the default size.
	 *
	 * Still, if any dimension turns out to be zero (AT LEAST UNDER X to avoid crashes) be sure
	 * result size != 0 in any dimension.
	 */
	size	=	mainViewSize;
	size.SetV (size.GetV () + menuBarSize.GetV ());
	size.SetH (Max (size.GetH (), menuBarSize.GetH ()));

	if (size.GetH () == 0) {
		size.SetH (defaultSize.GetH ());
	}
	if (size.GetV () == 0) {
		size.SetV (defaultSize.GetV ());
	}

#if		qXGDI
	// otherwise crash and burn...
	if (size.GetH () == 0) {
		size.SetH (100);
	}
	if (size.GetV () == 0) {
		size.SetV (100);
	}
#endif
	return (size);
}

void	MenuBarMainView::Layout ()
{
	Point	mBarSize	=	kZeroPoint;
	Point	ourSize		=	GetSize ();

	if (fMenuBar != Nil) {
		mBarSize = fMenuBar->CalcDefaultSize ();
		mBarSize.SetH (ourSize.GetH ());
		fMenuBar->SetOrigin (kZeroPoint);
		fMenuBar->SetSize (mBarSize);
	}
	if (fMainView != Nil) {
		fMainView->SetOrigin (Point (mBarSize.GetV (), 0));
		Coordinate	height	=	ourSize.GetV ();
		height -= mBarSize.GetV ();
		if (height <= 0) {
			height = 0;
		}
		fMainView->SetSize (Point (height, ourSize.GetH ()));
	}
	View::Layout ();
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

