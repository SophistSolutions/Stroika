/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SmallWindow.cc,v 1.4 1992/09/08 17:38:33 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SmallWindow.cc,v $
 *		Revision 1.4  1992/09/08  17:38:33  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:44:49  sterling
 *		*** empty log message ***
 *
 *		Revision 1.2  1992/06/25  09:51:43  sterling
 *		Add dtor to set mainview to nil - should delete but dont yet.
 *
 *		Revision 1.11  1992/04/27  09:39:02  lewis
 *		Sterl had temporarily checked in a hacked up version to test his slider-now revert to real
 *		version of small, and put his slider into user.
 *
 *		Revision 1.7  1992/02/02  05:41:05  lewis
 *		Sterl got rid of windowhints window ctor arg. Instead call GetShell().GetWindowHints(), make change,
 *		and call set. Did for good reasons, but sufficiently awkward that I'm not sure it belongs here in a
 *		simple demo.
 *
 *		Revision 1.6  1992/01/22  19:26:42  lewis
 *		Centered text in SmallView.
 *
 *
 */



#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Shell.hh"

#include 	"SmallDocument.hh"
#include	"SmallWindow.hh"




class	SmallView : public View {
	public:
		SmallView ();

		override	void	Draw (const Region& update);
};




/*
 ********************************************************************************
 *********************************** SmallWindow ********************************
 ********************************************************************************
 */

SmallWindow::SmallWindow (SmallDocument& myDocument):
	Window (),
	fView (Nil)
{
	SetWindowController (&myDocument);
	fView = new SmallView ();
	SetMainViewAndTargets (fView, Nil, Nil);
}

SmallWindow::~SmallWindow ()
{
// See what SetMainViewAndTargets does - should not delete args but if it doesnt, we
// must delete fView!!!
	SetMainViewAndTargets (Nil, Nil, Nil);
}



/*
 ********************************************************************************
 ************************************* SmallView ********************************
 ********************************************************************************
 */
SmallView::SmallView ():
	View ()
{
}

void	SmallView::Draw (const Region& /*update*/)
{
	const	String	kMessage	=	"Hello world";
	Font			f			=	GetEffectiveFont ();
	Coordinate		width		=	TextWidth (kMessage);
	Coordinate		height		=	f.GetAscent () + f.GetDescent ();
	Rect			r			=	Rect (kZeroPoint, Point (height, width));
	r = CenterRectAroundRect (r, GetLocalExtent ());
	DrawText (kMessage, r.GetBotLeft () - Point (f.GetDescent (), 0));
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

