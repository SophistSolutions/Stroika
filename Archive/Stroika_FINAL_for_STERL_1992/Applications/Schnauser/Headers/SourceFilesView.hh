/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SourceFilesView__
#define	__SourceFilesView__

/*
 * $Header: /fuji/lewis/RCS/SourceFilesView.hh,v 1.4 1992/09/01 17:40:39 sterling Exp $
 *
 * Description:
 *		This is a browser view, of a list of files in a project.
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: SourceFilesView.hh,v $
 *		Revision 1.4  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *		Revision 1.3  1992/07/22  01:15:45  lewis
 *		Use Sequence_DoubleLinkList instead of obsolete Sequence_DoubleLinkListPtr.
 *
 *
 *
 *
 */

#include	"Sequence_DoubleLinkList.hh"

#include	"Window.hh"

#include	"FilesView.hh"


// really instead of having our own list here, we need a pointer to the project structure!!!

#if		!qRealTemplatesAvailable
Declare (DoubleLinkList, BrowserFilePtr);
Declare (Sequence_DoubleLinkList, BrowserFilePtr);
#endif	/*!qRealTemplatesAvailable*/

class	SourceFilesView : public FilesBrowserView {
	public:
		SourceFilesView ();
		~SourceFilesView ();

	public:
		override	void	AddFile (BrowserFile* browserFile);
		override	void	RemoveFile (BrowserFile* browserFile);
		override	SequenceIterator(BrowserFilePtr)*	MakeFilesIterator (SequenceDirection d = eSequenceForward) const;

		override	void	Open ();

	protected:
		override	PixelMap	GetIcon_ () const;

	private:
		Sequence_DoubleLinkList(BrowserFilePtr)	fBrowserFiles;
		class	MyFilesView;
		MyFilesView*	fFilesView;
		Window			fWindow;
};




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__SourceFilesView__*/

