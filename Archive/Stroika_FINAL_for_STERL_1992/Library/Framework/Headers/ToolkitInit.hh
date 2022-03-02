/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ToolkitInit__
#define	__ToolkitInit__

/*
 * $Header: /fuji/lewis/RCS/ToolkitInit.hh,v 1.3 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: ToolkitInit.hh,v $
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/01/22  17:31:13  lewis
 *		Initial revision
 *
 *
 */

#include	"GDIInit.hh"

#include	"Config-Framework.hh"



class	ToolkitInit : public GDIInit {
	public:
#if		qMacGDI
		ToolkitInit (
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
		ToolkitInit (size_t minHeap = kMinHeap,
				size_t minStack = kMinStack);
#endif	/*qMacOS*/
};


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__ToolkitInit__*/

