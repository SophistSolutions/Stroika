/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ClipBoardWindow.cc,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ClipBoardWindow.cc,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/03  00:22:13  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:03:05  sterling
 *		Call SetMainViewAndTargets (Nil, Nil, Nil); in DTOR for window.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.12  1992/03/26  09:48:24  lewis
 *		Got rid of oldVisible args to EffectiveVisibiltyChanged method.
 *
 *		Revision 1.11  1992/02/15  05:31:20  sterling
 *		add/remove idel task when visibility changes
 *
 *		Revision 1.10  1992/01/29  05:06:53  sterling
 *		changed constructor to use new WindowShellHints
 *
 *		Revision 1.9  1992/01/28  20:50:30  lewis
 *		Use EventManager::Get rather than Application::Get ().
 *
 *		Revision 1.8  1992/01/06  04:37:51  lewis
 *		Since we no longer have Window__SetVisible) hook ,we must always
 *		leave check routine running, and check for visibilty init.
 *
 *		Revision 1.7  1991/12/27  17:07:11  lewis
 *		Use new window/shell support.
 *
 *
 *
 */



#include	"StreamUtils.hh"

#include	"ClipBoard.hh"
#include	"EventManager.hh"
#include	"Scroller.hh"
#include	"Shell.hh"

#include	"ClipBoardWindow.hh"




/*
 ********************************************************************************
 ****************************** ClipBoardWindow *********************************
 ********************************************************************************
 */

class	ClipCountChecker : public PeriodicTask {
	public:
		ClipCountChecker (ClipBoardWindow& aClipWindow):
			fClipWindow (aClipWindow)
			{
#if		qSunCFront_OverloadingConfusionBug
				SetPeriodicity ((BigReal)0.5);			// check for change about every 1/2 second
#else
				SetPeriodicity (0.5);					// check for change about every 1/2 second
#endif
			}
		override	void	RunABit ()
			{
				Require (fClipWindow.GetVisible ());
				fClipWindow.CheckClipState ();
			}
	private:
		ClipBoardWindow&	fClipWindow;
};

class	PictView : public View {
	public:
		PictView (const Picture& picture):
			View (),
			fPicture (picture)
			{
			}
		nonvirtual	void	SetPicture (const Picture& picture, UpdateMode updateMode = eDelayedUpdate)
			{
				fPicture = picture;
				SetSize (picture.GetFrame ().GetSize (), updateMode);
				Refresh (updateMode);		// not quite right rect...
			}
		override	void	Draw (const Region& update)
			{
				/*
				 * Only show the picture, not any surrounding whitespace.
				 */
				DrawPicture (fPicture, Rect (kZeroPoint, fPicture.GetFrame ().GetSize ()));
				View::Draw (update);
			}
	private:
		Picture		fPicture;
};



ClipBoardWindow::ClipBoardWindow () :
	Window (),
    fScroller (Nil),
	fClipCountChecker (Nil),
	fPictView (Nil),
	fLastScrapChangeCount (ClipBoard::Get ().GetScrapChangeCount ()-1)
{
	WindowShellHints hints = GetShell ().GetWindowShellHints ();
	hints.SetDesiredSize (Point (100, 400), False);
	hints.SetTitle ("ClipBoard");
	GetShell ().SetWindowShellHints (hints);
	
	fScroller = new Scroller ();
	SetMainViewAndTargets (fScroller, fScroller, fScroller);
	fClipCountChecker = new ClipCountChecker (*this);

	fPictView = new PictView (Picture ());
	fScroller->SetScrolledViewAndTargets (fPictView, Nil, Nil);
}

ClipBoardWindow::~ClipBoardWindow ()
{
	SetMainViewAndTargets (Nil, Nil, Nil);

	if (GetVisible ()) {
		EventManager::Get ().RemoveIdleTask (fClipCountChecker);
	}
	delete (fClipCountChecker);
}

void	ClipBoardWindow::EffectiveVisibilityChanged (Boolean newVisible, Panel::UpdateMode updateMode)
{
	/*
	 * Either add or remove our idle task depending on whether or not were visible. Also, check the
	 * system clip now (before any update events display old clip) for changes. Note that this
	 * code depends on newVisible REALLY being different than "oldVisible".
	 */
	if (newVisible) {
		CheckClipState ();		// be sure contents up-to-date before window made visible
		EventManager::Get ().AddIdleTask (fClipCountChecker);
	}
	else {
		EventManager::Get ().RemoveIdleTask (fClipCountChecker);
	}
	Window::EffectiveVisibilityChanged (newVisible, updateMode);
}

void	ClipBoardWindow::CheckClipState ()
{
	if (ClipBoard::Get ().GetScrapChangeCount () != fLastScrapChangeCount) {
		AssertNotNil (fPictView);
		fPictView->SetPicture (ClipBoard::Get ().RenderAsPicture ());

// This part is a pain - with virtual base classes, it could be made to go away by forceing
// you to put a ScrolledView into a Scroller, and to mix that feature in.
		AssertNotNil (fScroller);
		fScroller->ScrolledViewChangedSize ();

		fLastScrapChangeCount = ClipBoard::Get ().GetScrapChangeCount ();
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

