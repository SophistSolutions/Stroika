/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__GDIInit__
#define	__GDIInit__

/*
 * $Header: /fuji/lewis/RCS/GDIInit.hh,v 1.2 1992/09/01 15:34:49 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: GDIInit.hh,v $
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/01/22  04:15:47  lewis
 *		Initial revision
 *
 *
 */

#include	"OSInit.hh"
#include	"Config-Graphix.hh"


#if		qMacGDI
const	Boolean	kNotRequireColorQD		=	False;
const	Boolean	kRequireColorQD			=	True;
#endif	/*qMacGDI*/

class	GDIInit : public OSInit {
	public:
#if		qMacGDI
		GDIInit (
				size_t minHeap = kMinHeap,
				size_t minStack = kMinStack,
#if		mc68020
				Boolean require68020Plus = kRequire68020,
#else	/*mc68020*/
				Boolean require68020Plus = kNotRequire68020,
#endif	/*mc68020*/
				Boolean requireFPU = kNotRequireFPU,
				Boolean requireColorQD = kNotRequireColorQD);
#elif	qXGDI
		GDIInit (size_t minHeap = kMinHeap,
				size_t minStack = kMinStack);
#endif	/*qMacOS*/
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__GDIInit__*/

