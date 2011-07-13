/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__FunctionsView__
#define	__FunctionsView__

/*
 * $Header: /fuji/lewis/RCS/FunctionsView.hh,v 1.5 1992/09/08 17:08:42 lewis Exp $
 *
 * Description:
 *		This is a browser view, of a list of Functions in a project.
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: FunctionsView.hh,v $
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

#include	"BrowserView.hh"

// THIS SB IN BROWSERFILE.HH
		class	BrowserFunction;
		
		#if		!qRealTemplatesAvailable
		typedef	BrowserFunction*	BrowserFunctionPtr;
		Declare (Collection, BrowserFunctionPtr);
		Declare (Sequence, BrowserFunctionPtr);
		#endif	/*!qRealTemplatesAvailable*/



class	FunctionsBrowserView : public BrowserView  {
	protected:
		FunctionsBrowserView ();
	public:
		virtual	void	AddFile (BrowserFunction* browserFile)			=	Nil;
		virtual	void	RemoveFile (BrowserFunction* browserFile)		=	Nil;
		virtual	SequenceIterator(BrowserFunctionPtr)*	MakeFunctionsIterator (SequenceDirection d = eSequenceForward) const	=	Nil;
};


#if		!qRealTemplatesAvailable
#if 	qMPW_MacroOverflowProblem
#define	FunctionsBrowserViewPtr	funBRVwP
#endif
typedef	class	BrowserView*	FunctionsBrowserViewPtr;
Declare (Collection, FunctionsBrowserViewPtr);
Declare (Sequence, FunctionsBrowserViewPtr);
Declare (DoubleLinkList, FunctionsBrowserViewPtr);
Declare (Sequence_DoubleLinkList, FunctionsBrowserViewPtr);
#endif	/*!qRealTemplatesAvailable*/

/*
 * Maintain a list of all of the FunctionsBrowserView s.
 */
class	FunctionsBrowserViews  : public BrowserViews {
	public:
		FunctionsBrowserViews ();
		virtual ~FunctionsBrowserViews ();

		override	SequenceIterator(BrowserViewPtr)*	MakeBrowserViewIterator (SequenceDirection d = eSequenceForward) const;

		nonvirtual	void	AddFunctionsBrowserView (FunctionsBrowserView* functionsBrowserView);
		nonvirtual	void	RemoveFunctionsBrowserView (FunctionsBrowserView* functionsBrowserView);
		nonvirtual	SequenceIterator(FunctionsBrowserViewPtr)*	MakeFunctionsBrowserViewIterator (SequenceDirection d = eSequenceForward) const;

	private:
		Sequence_DoubleLinkList (FunctionsBrowserViewPtr)	fFunctionsBrowserViews;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__FunctionsView__*/

