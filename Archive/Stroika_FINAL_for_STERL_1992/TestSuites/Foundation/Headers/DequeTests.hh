/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__DequeTests__
#define	__DequeTests__

/*
 * $Header: /fuji/lewis/RCS/DequeTests.hh,v 1.8 1992/11/26 02:52:44 lewis Exp $
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
 *	$Log: DequeTests.hh,v $
 *		Revision 1.8  1992/11/26  02:52:44  lewis
 *		*** empty log message ***
 *
 *		Revision 1.7  1992/10/08  01:23:42  sterling
 *		Changes for using GenClass instead of declare macros. Also use SimpleClass intead
 *		of String.
 *
 *		Revision 1.6  1992/09/29  15:45:43  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.4  1992/09/15  17:30:22  lewis
 *		Got rid of AbXXX classes - now just XXX.
 *		
 *
 */
 

#include	"Deque.hh"

#include	"SimpleClass.hh"

#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	DequeOfUInt32_hh
	#include	DequeOfSimpleClass_hh
#endif

#if		qRealTemplatesAvailable
	extern	void	SimpleDequeTests (Deque<UInt32>& s);
	extern	void	SimpleDequeTests (Deque<SimpleClass>& s);
#else
	extern	void	SimpleDequeTests (Deque(UInt32)& s);
	extern	void	SimpleDequeTests (Deque(SimpleClass)& s);
#endif





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__DequeTests__*/


