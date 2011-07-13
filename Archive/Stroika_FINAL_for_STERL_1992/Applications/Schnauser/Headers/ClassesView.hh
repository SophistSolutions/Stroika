/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ClassesView__
#define	__ClassesView__

/*
 * $Header: /fuji/lewis/RCS/ClassesView.hh,v 1.5 1992/09/08 17:08:42 lewis Exp $
 *
 * Description:
 *		This is a browser view, of a list of classes in a project.
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: ClassesView.hh,v $
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

#include	"BrowserClass.hh"
#include	"BrowserView.hh"



class	ClassesBrowserView : public BrowserView  {
	protected:
		ClassesBrowserView ();
	public:
		virtual	void								AddFile (BrowserClass* browserClass)				=	Nil;
		virtual	void								RemoveFile (BrowserClass* browserClass)				=	Nil;
		virtual	SequenceIterator(BrowserClassPtr)*	MakeClassesIterator (SequenceDirection
														d = eSequenceForward) const	=	Nil;
};


#if		!qRealTemplatesAvailable
#if 	qMPW_MacroOverflowProblem
#define	ClassesBrowserViewPtr	clasBRViewP
#endif
typedef	class	ClassesBrowserView*	ClassesBrowserViewPtr;
Declare (Collection, ClassesBrowserViewPtr);
Declare (Sequence, ClassesBrowserViewPtr);
Declare (DoubleLinkList, ClassesBrowserViewPtr);
Declare (Sequence_DoubleLinkList, ClassesBrowserViewPtr);
#endif	/*!qRealTemplatesAvailable*/




/*
 * Maintain a list of all of the ClassesBrowserView s.
 */
class	ClassesBrowserViews  : public BrowserViews {
	public:
		ClassesBrowserViews ();
		virtual ~ClassesBrowserViews ();

		override	SequenceIterator(BrowserViewPtr)*
			MakeBrowserViewIterator (SequenceDirection d
									= eSequenceForward) const;

		nonvirtual	void	AddClassesBrowserView (ClassesBrowserView* classesBrowserView);
		nonvirtual	void	RemoveClassesBrowserView (ClassesBrowserView* classesBrowserView);
		nonvirtual	SequenceIterator(ClassesBrowserViewPtr)*
			MakeClassesBrowserViewIterator (SequenceDirection d
											= eSequenceForward) const;

	private:
		Sequence_DoubleLinkList (ClassesBrowserViewPtr)	fClassesBrowserViews;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__ClassesView__*/

