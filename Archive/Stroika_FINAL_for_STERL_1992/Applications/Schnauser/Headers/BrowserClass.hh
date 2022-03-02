/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BrowserClass__
#define	__BrowserClass__

/*
 * $Header: /fuji/lewis/RCS/BrowserClass.hh,v 1.3 1992/09/01 17:40:39 sterling Exp $
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
 *	$Log: BrowserClass.hh,v $
 *		Revision 1.3  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *		Revision 1.2  1992/07/03  04:43:04  lewis
 *		Include Sequence.hh isntead of Sequence_DoublyLLOfPointers.hh.
 *
 *
 *
 *
 */

#include	"Sequence.hh"

#include	"BrowserScope.hh"
#include	"BrowserType.hh"




class	BrowserClass : public BrowserType, public BrowserScope {
	public:
		BrowserClass (const String& name, BrowserScope& scope);

	private:
};


#if		!qRealTemplatesAvailable
	typedef	BrowserClass*	BrowserClassPtr;
	Declare (Collection, BrowserClassPtr);
	Declare (Iterator, BrowserClassPtr);
	Declare (AbSequence, BrowserClassPtr);
	Declare (Array, BrowserClassPtr);
	Declare (Sequence_Array, BrowserClassPtr);
	Declare (Sequence, BrowserClassPtr);
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__BrowserClass__*/

