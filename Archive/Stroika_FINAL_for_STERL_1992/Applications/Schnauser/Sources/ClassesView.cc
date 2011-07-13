/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ClassesView.cc,v 1.5 1992/09/08 17:08:42 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ClassesView.cc,v $
 *		Revision 1.5  1992/09/08  17:08:42  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
 *
 *		Revision 1.3  1992/07/22  01:15:45  lewis
 *		Use Sequence_DoubleLinkList instead of obsolete Sequence_DoubleLinkListPtr.
 *
 *
 *
 */




#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"DeskTop.hh"

#include	"ClassesView.hh"



// THIS SB IN BROWSERClass.cc
		class	BrowserClass;
		
		#if		!qRealTemplatesAvailable
		typedef	BrowserClass*	BrowserClassPtr;
		Implement (Collection, BrowserClassPtr);
		Implement (Sequence, BrowserClassPtr);
		#endif	/*!qRealTemplatesAvailable*/







/*
 ********************************************************************************
 ******************************** ClassesBrowserView ****************************
 ********************************************************************************
 */

ClassesBrowserView::ClassesBrowserView ():
	BrowserView ()
{
}




/*
 ********************************************************************************
 ******************************* ClassesBrowserViews ******************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
Implement (Collection, ClassesBrowserViewPtr);
Implement (Sequence, ClassesBrowserViewPtr);
Implement (DoubleLinkList, ClassesBrowserViewPtr);
Implement (Sequence_DoubleLinkList, ClassesBrowserViewPtr);
#endif	/*!qRealTemplatesAvailable*/

ClassesBrowserViews::ClassesBrowserViews ():
	BrowserViews (),
	fClassesBrowserViews ()
{
}

ClassesBrowserViews::~ClassesBrowserViews ()
{
	Require (fClassesBrowserViews.IsEmpty ());
}

SequenceIterator(BrowserViewPtr)*	ClassesBrowserViews::MakeBrowserViewIterator (SequenceDirection d) const
{
	// Tricky to get right cuz of contravariance - pull sleezy trick for now...
	return ((SequenceIterator(BrowserViewPtr)*) MakeClassesBrowserViewIterator (d));
}

void	ClassesBrowserViews::AddClassesBrowserView (ClassesBrowserView* classesBrowserView)
{
	RequireNotNil (classesBrowserView);
	fClassesBrowserViews.Add (classesBrowserView);
}

void	ClassesBrowserViews::RemoveClassesBrowserView (ClassesBrowserView* classesBrowserView)
{
	RequireNotNil (classesBrowserView);
	fClassesBrowserViews.Remove (classesBrowserView);
}

SequenceIterator(ClassesBrowserViewPtr)*	ClassesBrowserViews::MakeClassesBrowserViewIterator (SequenceDirection d) const
{
	return (fClassesBrowserViews.MakeSequenceIterator (d));
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

