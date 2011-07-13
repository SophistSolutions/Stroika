/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BagTests__
#define	__BagTests__

/*
 * $Header: /fuji/lewis/RCS/BagTests.hh,v 1.7 1992/12/04 18:31:16 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: BagTests.hh,v $
 *		Revision 1.7  1992/12/04  18:31:16  lewis
 *		Put back #if qRealTemplatesAvailable stuff - trick with #defines
 *		failed with BCC - not sure why - investigate later...
 *
 *		Revision 1.6  1992/12/03  19:23:09  lewis
 *		Using new macro for Bag(T) expands to Bag<T> as a trial. See if
 *		it works on all targets. If so, we get get rid of a bunch of
 *		ifdefs.
 *
 *		Revision 1.5  1992/10/08  01:23:42  sterling
 *		Changes for using GenClass instead of declare macros. Also use SimpleClass intead
 *		of String.
 *
 *		Revision 1.4  1992/09/29  15:45:43  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.2  1992/09/15  17:30:22  lewis
 *		Got rid of AbXXX classes - now just XXX.
 *
 *		
 *
 */
 
 

#include	"Bag.hh"

#include	"SimpleClass.hh"

#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	BagOfUInt32_hh
	#include	BagOfSimpleClass_hh
#endif

#if		qRealTemplatesAvailable
	extern	void    BagTests (Bag<UInt32>& s);
	extern	void    BagTests (Bag<SimpleClass>& s);
#else
	extern	void	BagTests (Bag(UInt32)& s);
	extern	void	BagTests (Bag(SimpleClass)& s);
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__BagTests__*/


