/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/FilesView.cc,v 1.6 1992/09/08 17:08:42 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: FilesView.cc,v $
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

#include	"FilesView.hh"






/*
 ********************************************************************************
 ******************************** FilesBrowserView ******************************
 ********************************************************************************
 */

FilesBrowserView::FilesBrowserView ():
	BrowserView ()
{
}






/*
 ********************************************************************************
 ******************************* FilesBrowserViews ******************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
Implement (Collection, FilesBrowserViewPtr);
Implement (Sequence, FilesBrowserViewPtr);
Implement (DoubleLinkList, FilesBrowserViewPtr);
Implement (Sequence_DoubleLinkList, FilesBrowserViewPtr);
#endif	/*!qRealTemplatesAvailable*/

FilesBrowserViews::FilesBrowserViews ():
	BrowserViews (),
	fFilesBrowserViews ()
{
}

FilesBrowserViews::~FilesBrowserViews ()
{
	Require (fFilesBrowserViews.IsEmpty ());
}

	class	MyIt : public SequenceIterator(BrowserViewPtr) {
		public:
			MyIt (SequenceDirection d, const Sequence (FilesBrowserViewPtr)& seq):
				SequenceIterator(BrowserViewPtr) (d),
				fIt (Nil)
				{
					fIt = seq.MakeSequenceIterator (d);
				}
		override	Boolean			Done () 		{ return fIt->Done (); }
		override	void			Next () 		{ fIt->Next (); }
		override	BrowserViewPtr	Current () 		{ return fIt->Current (); }
		override	CollectionSize	CurrentIndex () { return fIt->CurrentIndex (); }
			~MyIt ()
				{
					delete fIt;
				}
		private:
			SequenceIterator(FilesBrowserViewPtr)*	fIt;
	};
SequenceIterator(BrowserViewPtr)*	FilesBrowserViews::MakeBrowserViewIterator (SequenceDirection d) const
{
#if 1
	// make this into a reusable templated guy... Often usefull -
	return (new MyIt (d, fFilesBrowserViews));
#else
	// Tricky to get right cuz of contravariance - pull sleezy trick for now...
	return ((SequenceIterator(BrowserViewPtr)*) MakeFilesBrowserViewIterator (d));
#endif
}

void	FilesBrowserViews::AddFilesBrowserView (FilesBrowserView* filesBrowserView)
{
	RequireNotNil (filesBrowserView);
	fFilesBrowserViews.Add (filesBrowserView);
}

void	FilesBrowserViews::RemoveFilesBrowserView (FilesBrowserView* filesBrowserView)
{
	RequireNotNil (filesBrowserView);
	fFilesBrowserViews.Remove (filesBrowserView);
}

SequenceIterator(FilesBrowserViewPtr)*	FilesBrowserViews::MakeFilesBrowserViewIterator (SequenceDirection d) const
{
	return (fFilesBrowserViews.MakeSequenceIterator (d));
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

