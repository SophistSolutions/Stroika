/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TearOffMenu.cc,v 1.6 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: TearOffMenu.cc,v $
 *		Revision 1.6  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  20:22:12  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/16  15:56:33  sterling
 *		modified to support new menu scheme
 *
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *		Revision 1.19  92/02/28  15:57:44  15:57:44  lewis (Lewis Pringle)
 *		Renamed TearOffCustomMenu to TearOffMenu.
 *		
 *		Revision 1.18  1992/02/28  15:40:11  lewis
 *		Update signature of ChooseItem method to reflect change in base class Menu.
 *
 *		Revision 1.16  1992/02/14  03:45:46  lewis
 *		React to change in args and meaning of params of ChooseItem ()... No longer
 *		takes menu rect, use GetLocalExtent () instead. Also, params in local coordinates so we must convert ourselves to Desktop
 *		as necessary. Because of this, we get rid of menuRect param to InTearRegion,a
 *		dn the trackers, and make them work more with local coords.
 *
 *		Revision 1.15  1992/02/12  07:29:05  lewis
 *		Subclass from Menu, not CustomMenu.
 *
 *		Revision 1.14  1992/02/11  01:34:30  lewis
 *		Support new Menu changes (1 class) and no more CustomMenuView.
 *
 *		Revision 1.12  1992/02/05  07:42:26  lewis
 *		Working on cleaning up motif menus - for now so still works on mac keep old code for mac only...
 *
 *		Revision 1.10  1992/01/29  05:37:53  sterling
 *		changed to not pass WindowShellHints
 *
 *		Revision 1.8  1992/01/16  01:00:48  lewis
 *		Made compile under unix
 *
 *		Revision 1.6  1991/12/27  17:08:28  lewis
 *		Use new Window shell support.
 *
 *		Revision 1.34  1991/12/18  09:05:59  lewis
 *		Worked on adding WindowShell support.
 *
 *
 */

#include	"Shape.hh"

#include	"DeskTop.hh"
#include	"Shell.hh"
#include	"Window.hh"
#include	"WindowPlane.hh"

#include	"TearOffMenu.hh"	








/*
 ********************************************************************************
 *********************************** TearOffMenu ********************************
 ********************************************************************************
 */

TearOffMenu::TearOffMenu (Window* w):
	Menu (),
	fTearOffWindow (w),
	fBuiltWindow (False)
{
	if (fTearOffWindow == Nil) {
		fTearOffWindow = new Window (new StandardPalletWindowShell ());
		fBuiltWindow = True;
	}
	AssertNotNil (fTearOffWindow);
}

TearOffMenu::~TearOffMenu ()
{
	if (fBuiltWindow) {
		View* tornOffView = fTearOffWindow->GetMainView ();
		if (tornOffView != Nil) {
			fTearOffWindow->SetMainView (Nil);
			delete tornOffView;
		}
		fTearOffWindow->Close ();
		delete (fTearOffWindow);
	}
}

Window&	TearOffMenu::GetTearOffWindow () const
{
	EnsureNotNil (fTearOffWindow);
	return (*fTearOffWindow);
}

	// Moved out of chooseitem cuz of: q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
	// could scope inside if, but for compiler bug in apples cfront 1.0 final (DEC 1990)

	// make command since cannot call wmgr while in a menu...
	class	TearOffCommand : public Command {
		public:
			TearOffCommand (TearOffMenu& oldMenu, Window& w, Point p):
				Command (CommandHandler::eNoCommand, kNotUndoable),
				fWindow (w),
				fWhere (p)
			{
				if (fWindow.GetMainView () == Nil) {
					fWindow.SetMainView (oldMenu.BuildTornOffView ());
				}
			}
			
			override	void	DoIt ()
			{
				if (fWindow.GetVisible ()) {
					fWindow.SetVisible (not View::kVisible);
				}
				WindowShellHints hints = fWindow.GetShell ().GetWindowShellHints ();
				hints.SetDesiredOrigin (fWhere);
				fWindow.GetShell ().SetWindowShellHints (hints);
				
				fWindow.Select ();
				Command::DoIt ();
			}

		private:
			Window&		fWindow;
			Point		fWhere;
	};


	class	TearOffDragger : public ShapeDragger {
		public:
			TearOffDragger (TearOffMenu& tearOff, Window& w, const Point& hitPt, const Point& menuSize):
				ShapeDragger (RegionShape (w.GetLocalExtent ()), Rect (hitPt, menuSize), w.GetPlane (), *w.GetPlane ().GetTablet (), kZeroPoint),
				fTearOff (tearOff),
				fWindow (w)
			{
				SetHysteresis (kZeroPoint);
				SetTimeOut (0);
			}
	
		protected:
			override	Boolean		TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next)
				{
					ShapeDragger::TrackResult (phase, anchor, previous, next);
					if (phase == eTrackRelease) {
						if (fTearOff.InTearRegion (next)) {
							fWindow.PostCommand (new TearOffCommand (fTearOff, fWindow, next));
						}
						return (False);
					}
					else {
						// abort if move out of tear off region...
						return (not (fTearOff.InTearRegion (next)));
					}
				}
	
		private:
			TearOffMenu&	fTearOff;
			Window&			fWindow;
	};

CollectionSize	TearOffMenu::ChooseItem (const Point& hitPt, CollectionSize whichItem)
{
	if (InTearRegion (hitPt)) {
		if (not GetTearOffWindow ().GetVisible ()) {
			GetTearOffWindow ().SetContentSize (GetSize ());
			GetTearOffWindow ().ProcessLayout ();
		}
		/*
		 * Since we are doing the drag in the desktop, we should convert to desktop coords.
		 */
		 Point deskTopHitPt	=	LocalToAncestor (hitPt, &DeskTop::Get ());
		(void)TearOffDragger (*this, GetTearOffWindow (), deskTopHitPt, GetSize ()).TrackPress (MousePressInfo (deskTopHitPt));
		return (0);
	}
	else {
		return (Menu::ChooseItem (hitPt, whichItem));
	}
}

Boolean	TearOffMenu::InTearRegion (const Point& p) const
{
	const	Coordinate	kTearMargin	=	10;
	Rect				r	=	GetLocalExtent ();

#if		qMacToolkit
	/*
	 * Never tear off if we are in the menu bar, even if just within tear margin of this menu!
	 */
	const	Coordinate	kMBarHeight = 	20;	//	sb ::GetMBarHeight();
	if (LocalToAncestor (p, &DeskTop::Get ()).GetV () <= kMBarHeight) {
		return (False);
	}
#endif
	return Boolean (not r.InsetBy (Point (-kTearMargin, -kTearMargin)).Contains (p));
}



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

