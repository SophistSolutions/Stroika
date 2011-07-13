/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SourceFilesView.cc,v 1.6 1992/09/08 17:08:42 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SourceFilesView.cc,v $
 *		Revision 1.6  1992/09/08  17:08:42  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
 *
 *		Revision 1.4  1992/07/22  01:15:45  lewis
 *		Use Sequence_DoubleLinkList instead of obsolete Sequence_DoubleLinkListPtr.
 *
 *
 *
 */




#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"DeskTop.hh"
#include	"PickList.hh"

#include	"SourceFilesView.hh"



#include	"SourceFiles.pmc"


#if		!qRealTemplatesAvailable
Implement (DoubleLinkList, BrowserFilePtr);
Implement (Sequence_DoubleLinkList, BrowserFilePtr);
#endif	/*!qRealTemplatesAvailable*/


class	MyFilesView : public View, public ButtonController {
	public:
		MyFilesView ();
		~MyFilesView ();

	protected:
		override	void	Layout ();

	private:
		StringPickList	fItems;
};






/*
 ********************************************************************************
 ********************************* SourceFilesView ******************************
 ********************************************************************************
 */

SourceFilesView::SourceFilesView ():
	FilesBrowserView (),
	fBrowserFiles (),
	fFilesView (Nil),
	fWindow ()
{
	fFilesView = new MyFilesView ();
	fWindow.SetMainView (fFilesView);
}

SourceFilesView::~SourceFilesView ()
{
	fWindow.SetMainView (Nil);
}

void	SourceFilesView::AddFile (BrowserFile* browserFile)
{
	RequireNotNil (browserFile);
	fBrowserFiles.Add (browserFile);
}

void	SourceFilesView::RemoveFile (BrowserFile* browserFile)
{
	RequireNotNil (browserFile);
	fBrowserFiles.Remove (browserFile);
}

SequenceIterator(BrowserFilePtr)*	SourceFilesView::MakeFilesIterator (SequenceDirection d) const
{
	return (fBrowserFiles.MakeSequenceIterator (d));
}

void	SourceFilesView::Open ()
{
//	Alert ("Cannot yet open up SourceFilesView", "Sorry").Pose ();
	fWindow.Select ();
}

PixelMap	SourceFilesView::GetIcon_ () const
{
	return (GetSourceFiles ());
}



/*
 ********************************************************************************
 *********************************** MyFilesView ********************************
 ********************************************************************************
 */



MyFilesView::MyFilesView ():
	View (),
	ButtonController (),
	fItems ()
{
	SetHelp ("List of Views onto files, classes, or methods."
			"To open: double click."
			"To create new ones: see Views menu"
			"To delete views: drag them out of this window."
			"Change View to by Icon/Name"
		);

	fItems.SetItemsFloat ();
	AddSubView (&fItems);

fItems.AddItem ("Main.cc");
fItems.AddItem ("Windows.cc");
fItems.AddItem ("IconUtils.cc");
}

MyFilesView::~MyFilesView ()
{
	RemoveSubView (&fItems);
}

void	MyFilesView::Layout ()
{
	fItems.SetSize (GetSize ());
	View::Layout ();
}








// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

