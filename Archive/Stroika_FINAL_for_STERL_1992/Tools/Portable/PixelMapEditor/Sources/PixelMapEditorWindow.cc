/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditorWindow.cc,v 1.4 1992/09/08 16:52:13 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapEditorWindow.cc,v $
 *		Revision 1.4  1992/09/08  16:52:13  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:37:42  sterling
 *		*** empty log message ***
 *
 *		Revision 1.6  1992/06/03  16:32:18  lewis
 *		Got rid of default pixelmap for pixelmapeditor. It was only a hack to test out bootstrapping.
 *		Now we use TicTacToes pixelmap as a regression test.
 *
 *		Revision 1.5  92/05/20  00:29:22  00:29:22  sterling (Sterling Wight)
 *		commented out default pixelmap
 *		
 *		Revision 1.4  92/05/02  23:03:22  23:03:22  lewis (Lewis Pringle)
 *		On qMacGDI disable setting of pixelmap from default since format not yet
 *		portable - restore to old state when we write portable format...
 *		
 *		Revision 1.3  92/04/29  12:38:12  12:38:12  lewis (Lewis Pringle)
 *		Fixup file IO and use a file we generated as a default pixelmap.
 *
 *		Revision 1.1  92/04/20  11:06:03  11:06:03  lewis (Lewis Pringle)
 *		Initial revision
 *
 *
 *
 */




#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"MenuOwner.hh"
#include	"Shell.hh"

#include	"PixelMapEditor.hh"

#include 	"PixelMapEditorDocument.hh"

#include	"PixelMapEditorWindow.hh"







class	PixelMapEditorView : public View, public FocusOwner {
	public:
		PixelMapEditorView ();
		~PixelMapEditorView ();

		override	void	Layout ();

	PixelMapEditor	fPixelMapEditor;
};






/*
 ********************************************************************************
 ****************************** PixelMapEditorWindow ****************************
 ********************************************************************************
 */

PixelMapEditorWindow::PixelMapEditorWindow (PixelMapEditorDocument& myDocument):
	Window (),
	fView (Nil),
	fDocument (myDocument)
{
	SetWindowController (&myDocument);
	fView = new PixelMapEditorView ();
	SetMainViewAndTargets (fView, fView, fView);
}

PixelMapEditorWindow::~PixelMapEditorWindow ()
{
	SetMainViewAndTargets (Nil, Nil, Nil);
	delete fView;
}

Boolean	PixelMapEditorWindow::TrackPress (const MousePressInfo& mouseInfo)
{
	if (Window::TrackPress (mouseInfo)) {
// hack til we do commands for changes to make them undoable...
		fDocument.PlusChangeCount ();
		MenuOwner::SetMenusOutOfDate ();
		return (True);
	}
	else {
		return (False);
	}
}

PixelMap	PixelMapEditorWindow::GetPixelMap () const
{
	AssertNotNil (fView);
	return (fView->fPixelMapEditor.GetPixelMap ());
}

void	PixelMapEditorWindow::SetPixelMap (const PixelMap& pm)
{
	AssertNotNil (fView);
	fView->fPixelMapEditor.SetPixelMap (pm);
}







/*
 ********************************************************************************
 ******************************** PixelMapEditorView ****************************
 ********************************************************************************
 */
PixelMapEditorView::PixelMapEditorView ():
	View ()
{
	AddSubView (&fPixelMapEditor);
	AddFocus (&fPixelMapEditor);
}

PixelMapEditorView::~PixelMapEditorView ()
{
	RemoveSubView (&fPixelMapEditor);
	RemoveFocus (&fPixelMapEditor);
}

void	PixelMapEditorView::Layout ()
{
	fPixelMapEditor.SetSize (GetSize ());
	View::Layout ();
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

