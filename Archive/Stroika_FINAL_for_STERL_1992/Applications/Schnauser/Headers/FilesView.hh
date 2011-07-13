/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__FilesView__
#define	__FilesView__

/*
 * $Header: /fuji/lewis/RCS/FilesView.hh,v 1.5 1992/09/08 17:08:42 lewis Exp $
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
 *	$Log: FilesView.hh,v $
 *		Revision 1.5  1992/09/08  17:08:42  lewis
 *		Renamed NULL -> Nil.
 *
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

#include	"BrowserFile.hh"
#include	"BrowserView.hh"




class	FilesBrowserView : public BrowserView  {
	protected:
		FilesBrowserView ();
	public:
		virtual	void	AddFile (BrowserFile* browserFile)			=	Nil;
		virtual	void	RemoveFile (BrowserFile* browserFile)		=	Nil;
		virtual	SequenceIterator(BrowserFilePtr)*	MakeFilesIterator (SequenceDirection d = eSequenceForward) const	=	Nil;
};


#if		!qRealTemplatesAvailable
#if 	qMPW_MacroOverflowProblem
#define	FilesBrowserViewPtr	fileBRViewP
#endif
typedef	class	FilesBrowserView*	FilesBrowserViewPtr;
Declare (Collection, FilesBrowserViewPtr);
Declare (Sequence, FilesBrowserViewPtr);
Declare (DoubleLinkList, FilesBrowserViewPtr);
Declare (Sequence_DoubleLinkList, FilesBrowserViewPtr);
#endif	/*!qRealTemplatesAvailable*/

/*
 * Maintain a list of all of the FilesBrowserView s.
 */
class	FilesBrowserViews  : public BrowserViews {
	public:
		FilesBrowserViews ();
		virtual ~FilesBrowserViews ();

		override	SequenceIterator(BrowserViewPtr)*	MakeBrowserViewIterator (SequenceDirection d = eSequenceForward) const;

		nonvirtual	void	AddFilesBrowserView (FilesBrowserView* filesBrowserView);
		nonvirtual	void	RemoveFilesBrowserView (FilesBrowserView* filesBrowserView);
		nonvirtual	SequenceIterator(FilesBrowserViewPtr)*	MakeFilesBrowserViewIterator (SequenceDirection d = eSequenceForward) const;

	private:
		Sequence_DoubleLinkList (FilesBrowserViewPtr)	fFilesBrowserViews;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__FilesView__*/

