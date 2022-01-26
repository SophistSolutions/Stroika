/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BrowserFile__
#define	__BrowserFile__

/*
 * $Header: /fuji/lewis/RCS/BrowserFile.hh,v 1.3 1992/09/01 17:40:39 sterling Exp $
 *
 * Description:
 *		XXX
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: BrowserFile.hh,v $
 *		Revision 1.3  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */

#include	"File.hh"
#include	"Sequence.hh"
#include	"String.hh"

#include	"SchnauserConfig.hh"




class	BrowserFile {
	public:
		BrowserFile (const String& name);

	private:
		String	fName;
};


#if		!qRealTemplatesAvailable
typedef	BrowserFile*	BrowserFilePtr;
Declare (Collection, BrowserFilePtr);
Declare (Sequence, BrowserFilePtr);
#endif	/*!qRealTemplatesAvailable*/




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__BrowserFile__*/

