/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BrowserView__
#define	__BrowserView__

/*
 * $Header: /fuji/lewis/RCS/BrowserView.hh,v 1.4 1992/09/08 17:08:42 lewis Exp $
 *
 * Description:
 *		Abstract classes which gives use views into our browser database.
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: BrowserView.hh,v $
 *		Revision 1.4  1992/09/08  17:08:42  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *		Revision 1.1  1992/06/30  03:57:05  lewis
 *		Initial revision
 *		
 *
 *
 *
 */

#include	"Sequence.hh"
#include	"String.hh"

#include	"SchnauserConfig.hh"


class	BrowserView  {
	protected:
		BrowserView ();
	public:
		virtual ~BrowserView ();

		nonvirtual	String	GetName () const;
		nonvirtual	void	SetName (const String& name);

		/*
		 * Launch (or bring to front) your associated window/view.
		 */
		virtual		void	Open ()			=		Nil;


		/*
		 * Get the icon associated with this class of views. This should vary on a
		 * per-class, not per-instance - basis.
		 */
		nonvirtual	PixelMap	GetIcon () const;

	protected:
		virtual	PixelMap	GetIcon_ () const		=		Nil;

	private:
		String	fName;
};


#if		!qRealTemplatesAvailable
typedef	class	BrowserView*	BrowserViewPtr;
Declare (Collection, BrowserViewPtr);
Declare (Sequence, BrowserViewPtr);
#endif	/*!qRealTemplatesAvailable*/

class	BrowserViews  {
	protected:
		BrowserViews ();
	public:
		virtual ~BrowserViews ();

		virtual	SequenceIterator(BrowserViewPtr)*	MakeBrowserViewIterator (SequenceDirection d = eSequenceForward) const	=	Nil;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__BrowserView__*/

