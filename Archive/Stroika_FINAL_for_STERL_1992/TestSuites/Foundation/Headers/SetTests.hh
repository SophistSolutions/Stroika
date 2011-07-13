/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SetTests__
#define	__SetTests__

/*
 * $Header: /fuji/lewis/RCS/SetTests.hh,v 1.8 1992/11/26 02:53:33 lewis Exp $
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
 *	$Log: SetTests.hh,v $
 *		Revision 1.8  1992/11/26  02:53:33  lewis
 *		*** empty log message ***
 *
 *		Revision 1.7  1992/10/08  01:23:42  sterling
 *		Changes for using GenClass instead of declare macros. Also use SimpleClass intead
 *		of String.
 *
 *		Revision 1.6  1992/09/29  15:45:43  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 */

#include	"Set.hh"
#include	"SimpleClass.hh"

#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	SetOfUInt32_hh
	#include	SetOfSimpleClass_hh
#endif



#if		qRealTemplatesAvailable
	extern	void	SimpleTests (Set<UInt32>& s);
	extern	void	SimpleTests (Set<SimpleClass>& s);
#else
	extern	void	SimpleTests (Set(UInt32)& s);
	extern	void	SimpleTests (Set(SimpleClass)& s);
#endif





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__SetTests__*/


