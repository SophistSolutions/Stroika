/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ProjectWindow.cc,v 1.5 1992/09/08 17:08:42 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ProjectWindow.cc,v $
 *		Revision 1.5  1992/09/08  17:08:42  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
 *
 *
 *
 */




#include	"Debug.hh"
#include	"Set_BitString.hh"
#include	"StreamUtils.hh"

#include	"DeskTop.hh"
#include	"Grid.hh"
#include	"MenuOwner.hh"
#include	"PixelMapButton.hh"
#include	"Shell.hh"

#include 	"ClassesView.hh"
#include 	"CommandNumbers.hh"
#include 	"FilesView.hh"
#include 	"FunctionsView.hh"
#include 	"SchnauserDocument.hh"
#include 	"SourceFilesView.hh"

#include	"ProjectWindow.hh"



// Should we have one hard-wired source view that never goes away... I think it will be
// less confusing to keep it...
// Specail - always keep at least this around...
// Not required by src code, but cuz I think better
// User Interface - could be talked out of it...
#define		qHaveMainSourceView	1





// Include Icons
#include	"ByIcon.pmc"
#include	"ByName.pmc"
#include	"Classes.pmc"
#include	"Files.pmc"
#include	"Functions.pmc"



//&&&&&&&&&&&&&&&&&&&&&&&&&&&
Declare (Collection, CollectionSize);
Declare (Set, CollectionSize);
Declare (Set_BitString, CollectionSize);

Implement (Collection, CollectionSize);
Implement (Set, CollectionSize);
Implement (Set_BitString, CollectionSize);

// My simple cut at a picklist... without assuming subviews...
// Assumes uniform size of items... Assume 1 column, const line height...

// support those base classes later...
class	PL : public View /*, public AbstractButton, public EnableItem, public FocusItem*/,
	// hack - should use other class...
	private SliderController
{
	protected:
		PL (AbstractScrollBar* sBar = Nil);
	public:
		~PL ();

	public:
		nonvirtual	CollectionSize	GetLength () const;
		nonvirtual	void			Remove (CollectionSize i, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void			RemoveAll (UpdateMode updateMode = eDelayedUpdate);


// Discuss meaing of Select/SelectAll when GetAllowMultipleSelections() is FALSE!!!
//
		nonvirtual	Boolean			ItemSelected (CollectionSize i) const;
		nonvirtual	void			Select (CollectionSize i, Boolean selected = True, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void			SelectAll (Boolean selected = True, UpdateMode updateMode = eDelayedUpdate);
#if 0
		nonvirtual	Boolean			GetAllowMultipleSelections () const;
		nonvirtual	void			SetAllowMultipleSelections (Boolean allowMultipleSelections, UpdateMode updateMode = eDelayedUpdate);
#endif


	/*
	 * Stuff to keep track of content area - which items are visible, and which are
	 * hidden.
	 */
	public:
	 	nonvirtual	CollectionSize	GetFirstItemShown () const;
	 	nonvirtual	void			SetFirstItemShown (CollectionSize index, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	Boolean			ItemShown (CollectionSize i) const;
		nonvirtual	CollectionSize	GetLinesShown () const;
		nonvirtual	Rect			GetLinesShownExtent () const;



// These three NYI - but easy -
		nonvirtual	CollectionSize	GetSelectedItem (CollectionSize startFrom = 1);	// nil if past end
		nonvirtual	void			ScrollToSelected ();							// to first, if many...
		nonvirtual	CollectionSize	CountSelected () const;

		nonvirtual	Rect			GetItemExtent (CollectionSize i) const;
		nonvirtual	CollectionSize	PointInWhichItem (const Point& where) const;

	protected:
		override	void	Draw (const Region& update);
		override	void	Layout ();
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);


	/*
	 * This is the interface subclasses are intended to override to be a picklist.
	 */
	protected:
		/*
		 * You must override DrawItem () to specify the drawing behaviour of each of your items.
		 */
		virtual	void	DrawItem_ (CollectionSize item, const Rect& into)						=	Nil;

		/*
		 * You may override ItemClick () to specify the tracking/selection behaviour of each
		 * of your items. By default, this will <<LONG EXPLANATION- INCLUDING SHIFT CLICKS ETC>>
		 */
		virtual	void	ItemClick_ (CollectionSize item, const MousePressInfo& mouseInfo);

		virtual	void	Remove_ (CollectionSize i, UpdateMode updateMode)						=	Nil;
		virtual	void	RemoveAll_ (UpdateMode updateMode)										=	Nil;

	/*
	 * This is the interface subclasses are intended to use, but things that probably should not
	 * be made public without a bit more thought.
	 */
	protected:
		/*
		 * SetLength () to specify how many items in your picklist. Since this is nonvirtual, it
		 * should be kept protected. The reason it is nonvirtual, is that particular subclasses will
		 * probably need more arguments (namely a fill value) to make this sensible). It is only
		 * needed by this class to keep the scrollbar uptodate.
		 */
		nonvirtual	void	SetLength (CollectionSize newLength, UpdateMode updateMode = eDelayedUpdate);

		/*
		 * SetItemHeight () to specify ...
		 */
		nonvirtual	void	SetItemHeight (Coordinate newHeight, UpdateMode updateMode = eDelayedUpdate);

	private:
		AbstractScrollBar*				fScrollBar;
		Boolean							fMyScrollBar;
		Coordinate						fLineHeight;
		Coordinate						fLineWidth;				// ourWidth - scrollbar
		CollectionSize					fLineCount;
		CollectionSize					fTopLine;				// top line shown in scroll area
		CollectionSize					fLinesShown;			// count of lines currently visible
		Set_BitString (CollectionSize)	fSelected;				// which elements are selected...


	private:
		nonvirtual	void	AdjustLinesShown (UpdateMode updateMode = eDelayedUpdate);
};

PL::PL (AbstractScrollBar* sBar):
	View (),
	SliderController (),
	fScrollBar (Nil),
	fMyScrollBar (False),
	fLineHeight (1),
	fLineWidth (0),
	fLineCount (0),
	fTopLine (0),
	fLinesShown (0),
	fSelected ()
{
	if (sBar == Nil) {
		fScrollBar = new ScrollBar (AbstractScrollBar::eVertical);
		fMyScrollBar = True;
	}
	else {
		fScrollBar = sBar;
	}
	AssertNotNil (fScrollBar);
	AddSubView (fScrollBar);
	fScrollBar->SetSliderController ((SliderController*)this);
}

PL::~PL ()
{
	RemoveSubView (fScrollBar);
	if (fMyScrollBar) {
		delete (fScrollBar);
	}
}

CollectionSize	PL::GetLength () const
{
	return (fLineCount);
}

void	PL::Remove (CollectionSize i, UpdateMode updateMode)
{
	// Must update Selected set - need accessor to Butstring to call shift...
	Remove_ (i, updateMode);
}

void	PL::RemoveAll (UpdateMode updateMode)
{
	fSelected.RemoveAll ();
	RemoveAll_ (updateMode);
}

Boolean	PL::ItemSelected (CollectionSize i) const
{
	return (fSelected.Includes (i));
}

void	PL::Select (CollectionSize i, Boolean selected, UpdateMode updateMode)
{
	Require (i >= 1);
	Require (i <= GetLength ());
	if (selected) {
		fSelected.Add (i);
	}
	else {
		fSelected.Remove (i);
	}
	if (updateMode != eNoUpdate and ItemShown (i)) {
		Refresh (GetItemExtent (i), updateMode);
	}
}

void	PL::SelectAll (Boolean selected, UpdateMode updateMode)
{
	/*
	 * First update all visible items efficeintly, and then, make sure the rest of
	 * the selected set is setup properly.
	 */

	if (updateMode != eNoUpdate) {
		for (CollectionSize i = 1; i <= GetLinesShown (); i++) {
gDebugStream << "doing select in select all with valuie" << i + GetFirstItemShown () - 1 << newline;
			Select (i + GetFirstItemShown () - 1, selected, updateMode);
		}
	}

	if (selected) {
		fSelected.Add (1);			// need to do loop to add all...
	}
	else {
		fSelected.RemoveAll ();
	}
}

CollectionSize	PL::GetFirstItemShown () const
{
	return (fTopLine);
}

CollectionSize	PL::GetLinesShown () const
{
	return (fLinesShown);
}

Rect	PL::GetLinesShownExtent () const
{
	Rect	e	=	kZeroRect;
	if (GetFirstItemShown () != 0) {
		e = GetItemExtent (GetFirstItemShown ());
	}
	e.SetSize (e.GetSize () * GetLinesShown ());
	return (e);
}

void	PL::SetFirstItemShown (CollectionSize index, UpdateMode updateMode)
{
	Require (fTopLine >= 0);									// could be no items...
	Require (fTopLine >= 1 or (GetLength () == 0));
	Require (fTopLine <= GetLength () - GetLinesShown () + 1);
	fTopLine = index;
	Refresh (updateMode);			// must be smarter about scrolling...
}

Boolean		PL::ItemShown (CollectionSize i) const
{
	return Boolean ((i >= GetFirstItemShown ()) and (i <= GetFirstItemShown () + GetLinesShown () - 1));
}

Rect	PL::GetItemExtent (CollectionSize i) const
{
	Point	origin	=	Point ((i - GetFirstItemShown ())*fLineHeight, 0);
	Point	size	=	Point (fLineHeight, fLineWidth);
	return (Rect (origin, size));
}

CollectionSize	PL::PointInWhichItem (const Point& where) const
{
	Coordinate	x	=	where.GetH ();
	Coordinate	y	=	where.GetV ();
	if ((x < 0) or (x >= fLineWidth)) {
		return (kBadCollectionSize);
	}
	if (y < 0) {
		return (kBadCollectionSize);
	}
	y /= fLineHeight;
	y++;
	if (y <= GetLength ()) {
		return (y);
	}
	else {
		return (kBadCollectionSize);
	}
}

void	PL::Draw (const Region& update)
{
	for (CollectionSize i = 1; i <= GetLinesShown (); i++) {
		DrawItem_ (i + GetFirstItemShown () - 1, GetItemExtent (i + GetFirstItemShown () - 1));
	}
}

void	PL::Layout ()
{
	const	SBARSIZE		=	16;
	fScrollBar->SetExtent (Rect (GetLocalExtent ().GetTopRight () + Point (-1, -SBARSIZE-1),
		Point (GetSize ().GetV () + 2, SBARSIZE))
	);

	fLineWidth = GetSize ().GetH () - SBARSIZE -1;

	AdjustLinesShown ();
	View::Layout ();
}

Boolean	PL::TrackPress (const MousePressInfo& mouseInfo)
{
	AssertNotNil (fScrollBar);
	if (fScrollBar->PointInside (mouseInfo.fPressAt)) {
		fScrollBar->TrackPress (MousePressInfo (mouseInfo, fScrollBar->EnclosureToLocal (mouseInfo.fPressAt)));
	}
	else {
		// call ItemClick_ - even if point outside any item - may want to deselect all!!!
		ItemClick_ (PointInWhichItem (mouseInfo.fPressAt), mouseInfo);
	}
	return (True);
}

void	PL::ItemClick_ (CollectionSize item, const MousePressInfo& mouseInfo)
{
	if (mouseInfo.IsSimpleSelection ()) {
		if (not ItemSelected (item)) {
			SelectAll (False, eImmediateUpdate);
			if (item != kBadCollectionSize) {
				Select (item, True, eImmediateUpdate);
			}
		}
	}
	else if (mouseInfo.IsExtendedSelection ()) {
		if (item != kBadCollectionSize) {
			Select (item, not ItemSelected (item), eImmediateUpdate);
		}
	}
	else if (mouseInfo.IsOpenSelection ()) {
		// we treat this as a simple selection. Subclasses may wish to do something different,
		// but be careful- we could be told this was a double click, and have a different item
		// if the user clicked twice quickly near the boundary between two items!!!
		if (not ItemSelected (item)) {
			SelectAll (False, eImmediateUpdate);
			if (item != kBadCollectionSize) {
				Select (item, True, eImmediateUpdate);
			}
		}
	}
	else {
		// Is this possible??
		AssertNotReached ();
	}
}

void	PL::SetLength (CollectionSize newLength, UpdateMode updateMode)
{
	fLineCount = newLength;
	// Adjust Set of selected guys???
	AdjustLinesShown (updateMode);
}

void	PL::SetItemHeight (Coordinate newHeight, UpdateMode updateMode)
{
	Require (newHeight > 0);
	fLineHeight = newHeight;
	AdjustLinesShown (updateMode);
}

void	PL::AdjustLinesShown (UpdateMode updateMode)
{
	// figure out how many lines could be shown, and then from that, how many are - adjust
	// top line if necesary...
	Require (fLineHeight > 0);

	Boolean	needFullRefresh	=	False;

	// take care of special case where we go from no length to some or back - in this case
	// we set firstLineShown to a funny(0) value.
	if (Boolean (GetFirstItemShown () == 0) != Boolean (GetLength () == 0)) {
		SetFirstItemShown ((GetLength () == 0)? 0: 1, eNoUpdate);
		needFullRefresh = True;
	}

	fLinesShown = GetSize ().GetV () / fLineHeight;		// could be zero!!! - dont show partial lines...
	if (fLinesShown > GetLength ()) {
		fLinesShown = GetLength ();
gDebugStream << "reset fLinesShown to GetLength (): " << GetLength () << newline;
	}
	if (fLinesShown + GetFirstItemShown ()  > GetLength () + 1) {
		// then we must scroll
gDebugStream << "fLinesShown + GetFirstItemShown () - 1 > GetLength ()" << newline;
gDebugStream << "fLinesShown=" << fLinesShown << newline;
gDebugStream << "GetFirstItemShown ()=" << GetFirstItemShown () << newline;
		SetFirstItemShown (GetLength () - fLinesShown + 1, needFullRefresh? eNoUpdate: updateMode);
	}

	if (needFullRefresh and updateMode != eNoUpdate) {
		Refresh (GetLinesShownExtent (), updateMode);
	}

	Ensure (fLinesShown + GetFirstItemShown () <= GetLength () + 1 );
}







class	SPL : public PL {
	public:
		SPL ();

	protected:
		override	void	DrawItem_ (CollectionSize item, const Rect& into);
		override	void	Remove_ (CollectionSize i, UpdateMode updateMode);
		override	void	RemoveAll_ (UpdateMode updateMode);
};

SPL::SPL ()
{
	SetLength (4);

// DO IN LAYOUT!!!
SetItemHeight (14);
}

void	SPL::DrawItem_ (CollectionSize item, const Rect& into)
{
int fLeftMargin = 2;
int	vSpacing	=	1;
	const Font&	f = GetEffectiveFont ();	
	DrawText ("item 1", into.GetOrigin () + Point (vSpacing, fLeftMargin));
//	if (not GetEffectiveLive ()) {
//		Gray (Rectangle (), into);
//	}
	if (ItemSelected (item) /*and GetPickList ().GetLive ()*/) {
		Hilight (Rectangle (), into);
	}
}

void	SPL::Remove_ (CollectionSize i, UpdateMode updateMode)
{
}

void	SPL::RemoveAll_ (UpdateMode updateMode)
{
}


//&&&&&&&&&&&&&&&&&&&&&&&&&&&












enum	ViewKind {
	eFilesView,
	eClassesView,
	eMethodsView,

	eFirstViewKind		=	eFilesView,	
	eLastViewKind		=	eMethodsView,	
	eTotalViewKinds		=	eLastViewKind-eFirstViewKind+1,	
};




class	ControlPanel : public AbstractFixedGrid_Portable, public AbstractButton {
	public:
		ControlPanel ();
		~ControlPanel ();

	public:
		nonvirtual	ViewKind	GetSelected () const;
		nonvirtual	void		SetSelected (ViewKind viewKind, UpdateMode updateMode = eDelayedUpdate);

	public:
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	Boolean	TrackHelp (const Point& cursorAt, Region& helpRegion);

	protected:
		override	Point	CalcDefaultSize_ (const Point& hint) const;
		override	void	Draw (const Region& update);
		override	void	Layout ();

	private:

		nonvirtual	Rect				GetGridCell (ViewKind viewKind) const;
		nonvirtual	Boolean				PointToViewKind (const Point& p, ViewKind& viewKind) const;

		ViewKind		fSelected;
		PixelMapButton	fFiles;
		PixelMapButton	fClasses;
		PixelMapButton	fFunction;
};



class	ViewListView : public View, public ButtonController {
	public:
		ViewListView ();
		~ViewListView ();

	public:

		nonvirtual	void			OpenSelected ();

		nonvirtual	BrowserViews*	GetBrowserViews () const;
		nonvirtual	void			SetBrowserViews (BrowserViews* browserViews, UpdateMode updateMode = eDelayedUpdate);

// Called by?? So we can update our display minimally...
		nonvirtual	void			AddedBrowserView (BrowserView* browserView, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void			RemovedBrowserView (BrowserView* browserView, UpdateMode updateMode = eDelayedUpdate);

	protected:
		override	void	Layout ();

	private:
		BrowserViews*	fBrowserViews;
//		StringPickList	fItems;
SPL fItems;
};



class	StatusBarView : public View, public ButtonController {
	public:
		StatusBarView ();
		~StatusBarView ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& hint) const;
		override	void	Layout ();

	private:
		TextView		fStatusLabel;
		TextView		fStatus;
		TextView		fViewByLabel;
		PixelMapButton	fByName;
		PixelMapButton	fByIcon;
};





class	ProjectMainView : public View, public KeyHandler, public MenuCommandHandler,
							public ButtonController {
	public:
		ProjectMainView ();
		~ProjectMainView ();

		nonvirtual	ViewKind	GetViewKind () const;
		nonvirtual	void		SetViewKind (ViewKind viewKind, UpdateMode updateMode = eDelayedUpdate);

	protected:
		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

		override	void	Layout ();

		override	void	ButtonPressed (AbstractButton* button);

	private:
#if		qHaveMainSourceView
		SourceFilesView		fMainSourceFilesView;
#endif
		FilesBrowserViews		fFilesView;
		FunctionsBrowserViews	fFunctionsView;
		ClassesBrowserViews		fClassesView;	
		ViewKind				fViewKind;
		ControlPanel			fControlPanel;			// Panel to control which sort of views were looking at
		ViewListView			fViewListView;			// List of the currently selected type of views
		StatusBarView			fStatusBar;
};




/*
 ********************************************************************************
 ************************************* ProjectWindow ****************************
 ********************************************************************************
 */

ProjectWindow::ProjectWindow (SchnauserDocument& myDocument):
	Window (),
	fDocument (myDocument),
	fProjectMainView (Nil)
{
	SetWindowController (&myDocument);
	fProjectMainView = new ProjectMainView ();
	SetMainViewAndTargets (fProjectMainView, fProjectMainView, fProjectMainView);
}

ProjectWindow::~ProjectWindow ()
{
	SetMainViewAndTargets (Nil, Nil, Nil);
	delete fProjectMainView;
}








/*
 ********************************************************************************
 *********************************** ProjectMainView ****************************
 ********************************************************************************
 */
ProjectMainView::ProjectMainView ():
	View (),
	KeyHandler (),
	MenuCommandHandler (),
	ButtonController (),
	fFilesView (),
	fFunctionsView (),
	fClassesView (),
	fViewKind (eFilesView),
	fControlPanel (),
	fViewListView (),
	fStatusBar ()
{
	AddSubView (&fControlPanel);
	AddSubView (&fViewListView);
	AddSubView (&fStatusBar);

	fControlPanel.SetController (this);		// So we get notified when user clicks on different buttons

#if		qHaveMainSourceView
// For now - important that we add early since no notification mechnasm in place for later
// changes - that of course must be fixed soon - LGP 6/29/92
	fMainSourceFilesView.SetName ("¥Main Source View¥");
	fFilesView.AddFilesBrowserView (&fMainSourceFilesView);
#endif	/*qHaveMainSourceView*/

	fViewListView.SetBrowserViews (&fFilesView);		// Must corespond to initial value for fViewKind
}

ProjectMainView::~ProjectMainView ()
{
#if		qHaveMainSourceView
	fFilesView.RemoveFilesBrowserView (&fMainSourceFilesView);
#endif	/*qHaveMainSourceView*/

	RemoveSubView (&fControlPanel);
	RemoveSubView (&fViewListView);
	RemoveSubView (&fStatusBar);
}

void	ProjectMainView::DoSetupMenus ()
{
	SetOn (eViewViewsByName, True);
	EnableCommand (eViewViewsByName);
	EnableCommand (eViewViewsByIcon);

	EnableCommand (eNewFilesView);
	EnableCommand (eNewClassesView);
	EnableCommand (eNewFunctionsView);

// should check if any selected first...
	EnableCommand (eOpenView);
	switch (fViewKind) {
		case	eFilesView:
			SetCommandName (eOpenView, "Open selected Files view");
			break;
		case	eClassesView:
			SetCommandName (eOpenView, "Open selected Classes view");
			break;
		case	eMethodsView:
			SetCommandName (eOpenView, "Open selected Functions view");
			break;
		default:
			AssertNotReached ();
	}
}

Boolean	ProjectMainView::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case	eViewViewsByName: {
			Alert ("eViewViewsByName", "OK").Pose ();
			return (True);
		}
		case	eViewViewsByIcon: {
			Alert ("eViewViewsByIcon", "OK").Pose ();
			return (True);
		}
		case	eNewFilesView: {
			// Should put up alert asking name ..
			String name;
			static int i;
			switch (i++%4) {
				case	0:	name = "Obsolete Files"; break;
				case	1:	name = "Backup Files"; break;
				case	2:	name = "Review these"; break;
				case	3:	name = "Project Backups"; break;
			}
			SourceFilesView*	x = new SourceFilesView ();
			x->SetName (name);
			fFilesView.AddFilesBrowserView (x);
			if (eFilesView == GetViewKind ()) {
				fViewListView.AddedBrowserView (x);
			}
			else {
				SetViewKind (eFilesView);
			}
			return (True);
		}

		case	eNewFunctionsView: {
			// Should put up alert asking name ..
			String name;
			static int i;
			switch (i++%4) {
				case	0:	name = "Window::Open (char*)"; break;
				case	1:	name = "Window::Close ()"; break;
				case	2:	name = "Application::~Application ()"; break;
				case	3:	name = "main (int, char*[])"; break;
			}
			SourceFilesView*	x = new SourceFilesView ();
			x->SetName (name);
			fFunctionsView.AddFunctionsBrowserView ((FunctionsBrowserView *)x);			// hack - should work temporarily...
			if (eMethodsView == GetViewKind ()) {
				fViewListView.AddedBrowserView (x);
			}
			else {
				SetViewKind (eMethodsView);
			}
			return (True);
		}

		case	eNewClassesView: {
			// Should put up alert asking name ..
			String name;
			static int i;
			switch (i++%4) {
				case	0:	name = "Application"; break;
				case	1:	name = "Window"; break;
				case	2:	name = "Sequence<T>"; break;
				case	3:	name = "String"; break;
			}
			SourceFilesView*	x = new SourceFilesView ();
			x->SetName (name);
			fClassesView.AddClassesBrowserView ((ClassesBrowserView *)x);
			if (eClassesView == GetViewKind ()) {
				fViewListView.AddedBrowserView (x);
			}
			else {
				SetViewKind (eClassesView);
			}
			return (True);
		}

		case	eOpenView: {
// hack - assume first one for now...
//fMainSourceFilesView.Open ();
			fViewListView.OpenSelected ();
			return (True);
		}

		default:
			return (False);
	}
}

void	ProjectMainView::Layout ()
{
	fControlPanel.SetSize (fControlPanel.CalcDefaultSize (GetSize ()));
	fStatusBar.SetSize (fStatusBar.CalcDefaultSize (GetSize ()));

	Coordinate	lvHeight	=	GetSize ().GetV () - fControlPanel.GetSize ().GetV () - fStatusBar.GetSize ().GetV ();

#if 1
	// for some reason this looks better further to the left, and two pixels wider?? Ask Sterl?
	// LGP 6/26/92
	fViewListView.SetOrigin (Point (fControlPanel.GetSize ().GetV (), -1));
	fViewListView.SetSize (Point (lvHeight, GetSize ().GetH ()+2));
#else
	fViewListView.SetOrigin (Point (fControlPanel.GetSize ().GetV (), 0));
	fViewListView.SetSize (Point (lvHeight, GetSize ().GetH ()));
#endif

	fStatusBar.SetOrigin (Point (fViewListView.GetExtent ().GetBottom (), 0));

	View::Layout ();
}

void	ProjectMainView::ButtonPressed (AbstractButton* button)
{
	if (button == &fControlPanel) {
		SetViewKind (fControlPanel.GetSelected ());
	}
}

ViewKind	ProjectMainView::GetViewKind () const
{
	return (fViewKind);
}

void	ProjectMainView::SetViewKind (ViewKind viewKind, UpdateMode updateMode)
{
	if (fViewKind != viewKind) {
		fViewKind = viewKind;
		fControlPanel.SetSelected (viewKind, updateMode);
		switch (fViewKind) {
			case	eFilesView:
				fViewListView.SetBrowserViews (&fFilesView);
				break;
			case	eClassesView:
				fViewListView.SetBrowserViews (&fClassesView);
				break;
			case	eMethodsView:
				fViewListView.SetBrowserViews (&fFunctionsView);
				break;
			default:
				AssertNotReached ();
		}
	}
}






/*
 ********************************************************************************
 *********************************** ControlPanel *******************************
 ********************************************************************************
 */


ControlPanel::ControlPanel ():
	AbstractFixedGrid_Portable (),
	AbstractButton (),
	fSelected (eFilesView),
	fFiles (GetFiles ()),
	fClasses (GetClasses ()),
	fFunction (GetFunctions ())
{
// LGP 6/29/92 - MENTION TO STERL...
// One problem with this grid stuff is that we sometimes get extra line drawn at end of
// grid since things dont divide evenly...


	SetGridLinesDrawn (True);
	SetColumnCount (eTotalViewKinds);
	SetRowCount (1);

	AddSubView (&fFiles);
	AddSubView (&fClasses);
	AddSubView (&fFunction);
}

ControlPanel::~ControlPanel ()
{
	RemoveSubView (&fFiles);
	RemoveSubView (&fClasses);
	RemoveSubView (&fFunction);
}

ViewKind	ControlPanel::GetSelected () const
{
	return (fSelected);
}

void	ControlPanel::SetSelected (ViewKind viewKind, UpdateMode updateMode)
{
	if (fSelected != viewKind) {
		if (updateMode != eNoUpdate) {
			Refresh (GetGridCell (fSelected));
		}
		fSelected = viewKind;
		if (updateMode != eNoUpdate) {
			Refresh (GetGridCell (fSelected));
		}
		if (updateMode == eImmediateUpdate) {
			Update ();
		}
	}
}

Boolean	ControlPanel::TrackPress (const MousePressInfo& mouseInfo)
{
	ViewKind	viewKind	=	GetSelected ();
	if (PointToViewKind (mouseInfo.fPressAt, viewKind)) {
		SetSelected (viewKind, eImmediateUpdate);
	}
	Pressed ();
	return (True);
}

Boolean	ControlPanel::TrackHelp (const Point& cursorAt, Region& helpRegion)
{
// Kindof kludgy, and doesnt work well - must cleanup how we deal with displaying the help balloons...
	Require (GetEffectiveLive ());

	if (GetHelpEnabled ()) {
		for (int i = eFirstViewKind; i <= eLastViewKind; i++) {
			if (GetGridCell (ViewKind (i)).Contains (cursorAt)) {
				switch (ViewKind (i)) {
					case	eFilesView:
						SetHelp ("Select this icon to see Files views.");
						break;
					case	eClassesView:
						SetHelp ("Select this icon to see Classes views.");
						break;
					case	eMethodsView:
						SetHelp ("Select this icon to see Methods views.");
						break;
				
				}
				ShowHelp (LocalToAncestor (cursorAt, &DeskTop::Get ()), LocalToAncestor (helpRegion, &DeskTop::Get ()));
				helpRegion = GetGridCell (ViewKind (i));
				return (True);
			}
		}
SetHelp ("Select one of these icons to change which views you can select from");
		ShowHelp (LocalToAncestor (cursorAt, &DeskTop::Get ()), LocalToAncestor (helpRegion, &DeskTop::Get ()));
		helpRegion = kEmptyRegion;
		return (True);
	}
	return (False);
}

Point	ControlPanel::CalcDefaultSize_ (const Point& hint) const
{
	Point xx = AbstractFixedGrid_Portable::CalcDefaultSize_ (hint);
	return (Point (xx.GetV (), hint.GetH ()));
}

void	ControlPanel::Draw (const Region& update)
{
	AbstractFixedGrid_Portable::Draw (update);
	Hilight (Rectangle (), GetGridCell (GetSelected ()));
}

void	ControlPanel::Layout ()
{
	const	kVSluff	=	4;
SetRowsHeight (fFiles.CalcDefaultSize ().GetV () + kVSluff*2);
SetColumnsWidth (GetSize ().GetH () / eTotalViewKinds);
	fFiles.SetSize (fFiles.CalcDefaultSize ());
	fClasses.SetSize (fClasses.CalcDefaultSize ());
	fFunction.SetSize (fFunction.CalcDefaultSize ());
	AbstractFixedGrid_Portable::Layout ();
}

Rect	ControlPanel::GetGridCell (ViewKind viewKind) const
{
	Require (viewKind >= eFirstViewKind);
	Require (viewKind <= eLastViewKind);
	return (AbstractFixedGrid_Portable::GetGridCell (viewKind-eFirstViewKind+1));
}

Boolean	ControlPanel::PointToViewKind (const Point& p, ViewKind& viewKind) const
{
	for (int i = eFirstViewKind; i <= eLastViewKind; i++) {
		if (GetGridCell (ViewKind (i)).Contains (p)) {
			viewKind = ViewKind (i);
			return (True);
		}
	}
	return (False);
}








/*
 ********************************************************************************
 *********************************** ViewListView *******************************
 ********************************************************************************
 */

ViewListView::ViewListView ():
	View (),
	ButtonController (),
	fBrowserViews (Nil),
	fItems ()
{
	SetHelp ("List of Views onto files, classes, or methods."
			"To open: double click."
			"To create new ones: see Views menu"
			"To delete views: drag them out of this window."
			"Change View to by Icon/Name"
		);

//	fItems.SetItemsFloat ();
	AddSubView (&fItems);
}

ViewListView::~ViewListView ()
{
	RemoveSubView (&fItems);
}

void	ViewListView::Layout ()
{
	fItems.SetSize (GetSize ());
	View::Layout ();
}

void	ViewListView::OpenSelected ()
{
}

void	ViewListView::AddedBrowserView (BrowserView* browserView, UpdateMode updateMode)
{
	RequireNotNil (browserView);
//	fItems.AddItem (browserView->GetName ());
}

void	ViewListView::RemovedBrowserView (BrowserView* browserView, UpdateMode updateMode)
{
	RequireNotNil (browserView);
// really these should not deepend on names!!! temp hack...
//	fItems.RemoveItem (browserView->GetName ());
}

BrowserViews*	ViewListView::GetBrowserViews () const
{
	return (fBrowserViews);
}

void	ViewListView::SetBrowserViews (BrowserViews* browserViews, UpdateMode updateMode)
{
	if (fBrowserViews != browserViews) {
		if (fBrowserViews != Nil) {
//			fItems.DeleteAll ();
		}
		fBrowserViews = browserViews;
		if (fBrowserViews != Nil) {
			ForEach (BrowserViewPtr, it, fBrowserViews->MakeBrowserViewIterator ()) {
//				fItems.AddItem (it.Current ()->GetName ());
			}
		}
	}
}







/*
 ********************************************************************************
 *********************************** StatusBarView ******************************
 ********************************************************************************
 */

StatusBarView::StatusBarView ():
	View (),
	ButtonController (),
	fStatusLabel (),
	fStatus (),
	fViewByLabel (),
	fByName (),
	fByIcon ()
{
	fStatusLabel.SetText ("Status:");
	fStatus.SetText ("good");
	fViewByLabel.SetText ("View By:");
	fByName.SetPixelMap (GetByName ());
	fByIcon.SetPixelMap (GetByIcon ());

	fStatusLabel.SetFont (&kApplicationFont);
	fViewByLabel.SetFont (&kApplicationFont);
	Font	f	=	kApplicationFont;
	f.SetStyle (Set_BitString(FontStyle) (Font::eBoldFontStyle));
	fStatus.SetFont (&f);

	// Help text...
	fStatus.SetHelp ("Some stupid status information - ignore this for your own good");
	fByName.SetHelp ("View the exiting Views by Name");
	fByIcon.SetHelp ("View the exiting Views by Icon");

	AddSubView (&fStatusLabel);
	AddSubView (&fStatus);
	AddSubView (&fViewByLabel);
	AddSubView (&fByName);
	AddSubView (&fByIcon);
}

StatusBarView::~StatusBarView ()
{
	RemoveSubView (&fStatusLabel);
	RemoveSubView (&fStatus);
	RemoveSubView (&fViewByLabel);
	RemoveSubView (&fByName);
	RemoveSubView (&fByIcon);
}

Point	StatusBarView::CalcDefaultSize_ (const Point& hint) const
{
	return (Point (14, hint.GetH ()));
}

void	StatusBarView::Layout ()
{
	fStatusLabel.SetOrigin (Point (1, 5));
	fStatusLabel.SetSize (fStatusLabel.CalcDefaultSize ());

	fStatus.SetOrigin (fStatusLabel.GetExtent ().GetTopRight () + Point (0, 10));
	fStatus.SetSize (fStatus.CalcDefaultSize ());

	fViewByLabel.SetOrigin (fStatus.GetExtent ().GetTopRight () + Point (0, 25));
	fViewByLabel.SetSize (fViewByLabel.CalcDefaultSize ());

	fByName.SetOrigin (fViewByLabel.GetExtent ().GetTopRight () + Point (-1, 4));
	fByName.SetSize (fByName.CalcDefaultSize ());
	fByIcon.SetOrigin (fByName.GetExtent ().GetTopRight () + Point (0, 2));
	fByIcon.SetSize (fByIcon.CalcDefaultSize ());

	View::Layout ();
}











// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

