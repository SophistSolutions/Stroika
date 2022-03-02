/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__QueueTests__
#define	__QueueTests__

/*
 * $Header: /fuji/lewis/RCS/QueueTests.hh,v 1.7 1992/11/26 02:53:08 lewis Exp $
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
 *	$Log: QueueTests.hh,v $
 *		Revision 1.7  1992/11/26  02:53:08  lewis
 *		*** empty log message ***
 *
 *		Revision 1.6  1992/10/08  01:23:42  sterling
 *		Changes for using GenClass instead of declare macros. Also use SimpleClass intead
 *		of String.
 *
 *		Revision 1.5  1992/09/29  15:45:43  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.3  1992/09/15  17:30:22  lewis
 *		Got rid of AbXXX classes - now just XXX.
 *
 *		
 *
 */

#include	"Queue.hh"

#include	"SimpleClass.hh"


#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	QueueOfUInt32_hh
	#include	QueueOfSimpleClass_hh
#endif

#if		qRealTemplatesAvailable
	extern	void	SimpleQueueTests (Queue<UInt32>& s);
	extern	void	SimpleQueueTests (Queue<SimpleClass>& s);
#else
	extern	void	SimpleQueueTests (Queue(UInt32)& s);
	extern	void	SimpleQueueTests (Queue(SimpleClass)& s);
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__QueueTests__*/


