/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__StackTests__
#define	__StackTests__

/*
 * $Header: /fuji/lewis/RCS/StackTests.hh,v 1.10 1992/11/26 02:53:33 lewis Exp $
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
 *	$Log: StackTests.hh,v $
 *		Revision 1.10  1992/11/26  02:53:33  lewis
 *		*** empty log message ***
 *
 *		Revision 1.9  1992/10/08  01:23:42  sterling
 *		Changes for using GenClass instead of declare macros. Also use SimpleClass intead
 *		of String.
 *
 *		Revision 1.8  1992/09/29  15:45:43  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.2  1992/07/03  04:14:55  lewis
 *		Rename Strings.hh -> SimpleClass.hh.
 *
 *		Revision 1.2  1992/05/10  00:48:59  lewis
 *		Modified for templates / BC++.
 *
 */

#include	"Stack.hh"

#include	"SimpleClass.hh"

#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	StackOfUInt32_hh
	#include	StackOfSimpleClass_hh
#endif


#if		qRealTemplatesAvailable
	extern	void	SimpleTests (Stack<UInt32>& s);
	extern	void	SimpleTests (Stack<SimpleClass>& s);
#else
	extern	void	SimpleTests (Stack(UInt32)& s);
	extern	void	SimpleTests (Stack(SimpleClass)& s);
#endif





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__StackTests__*/


