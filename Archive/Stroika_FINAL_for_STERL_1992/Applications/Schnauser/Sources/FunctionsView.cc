/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/FunctionsView.cc,v 1.5 1992/09/08 17:08:42 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: FunctionsView.cc,v $
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
#include	"StreamUtils.hh"

#include	"DeskTop.hh"

#include	"FunctionsView.hh"



// THIS SB IN BROWSERFunction.cc
		class	BrowserFunction;
		
		#if		!qRealTemplatesAvailable
		typedef	BrowserFunction*	BrowserFunctionPtr;
		Implement (Collection, BrowserFunctionPtr);
		Implement (Sequence, BrowserFunctionPtr);
		#endif	/*!qRealTemplatesAvailable*/







/*
 ********************************************************************************
 ******************************** FunctionsBrowserView **************************
 ********************************************************************************
 */

FunctionsBrowserView::FunctionsBrowserView ():
	BrowserView ()
{
}




/*
 ********************************************************************************
 *************************** FunctionsBrowserViews ******************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
Implement (Collection, FunctionsBrowserViewPtr);
Implement (Sequence, FunctionsBrowserViewPtr);
Implement (DoubleLinkList, FunctionsBrowserViewPtr);
Implement (Sequence_DoubleLinkList, FunctionsBrowserViewPtr);
#endif	/*!qRealTemplatesAvailable*/

FunctionsBrowserViews::FunctionsBrowserViews ():
	BrowserViews (),
	fFunctionsBrowserViews ()
{
}

FunctionsBrowserViews::~FunctionsBrowserViews ()
{
	Require (fFunctionsBrowserViews.IsEmpty ());
}

SequenceIterator(BrowserViewPtr)*	FunctionsBrowserViews::MakeBrowserViewIterator (SequenceTraversalDirection d) const
{
	// Tricky to get right cuz of contravariance - pull sleezy trick for now...
	return ((SequenceIterator(BrowserViewPtr)*) MakeFunctionsBrowserViewIterator (d));
}

void	FunctionsBrowserViews::AddFunctionsBrowserView (FunctionsBrowserView* functionsBrowserView)
{
	RequireNotNil (functionsBrowserView);
	fFunctionsBrowserViews.Add (functionsBrowserView);
}

void	FunctionsBrowserViews::RemoveFunctionsBrowserView (FunctionsBrowserView* functionsBrowserView)
{
	RequireNotNil (functionsBrowserView);
	fFunctionsBrowserViews.Remove (functionsBrowserView);
}

SequenceIterator(FunctionsBrowserViewPtr)*	FunctionsBrowserViews::MakeFunctionsBrowserViewIterator (SequenceTraversalDirection d) const
{
	return (fFunctionsBrowserViews.MakeSequenceIterator (d));
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

