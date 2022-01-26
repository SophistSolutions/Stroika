/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TallyTests__
#define	__TallyTests__

/*
 * $Header: /fuji/lewis/RCS/TallyTests.hh,v 1.6 1992/11/26 02:53:33 lewis Exp $
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
 *	$Log: TallyTests.hh,v $
 *		Revision 1.6  1992/11/26  02:53:33  lewis
 *		*** empty log message ***
 *
 *		Revision 1.5  1992/10/08  01:23:42  sterling
 *		Changes for using GenClass instead of declare macros. Also use SimpleClass intead
 *		of String.
 *
 *		Revision 1.4  1992/09/29  15:45:43  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *		
 *
 */
 
 

#include	"Tally.hh"

#include	"SimpleClass.hh"

#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	TallyOfUInt32_hh
	#include	TallyOfSimpleClass_hh
#endif

#if		qRealTemplatesAvailable
	extern	void	SimpleTallyTests (Tally<UInt32>& s);
	extern	void	SimpleTallyTests (Tally<SimpleClass>& s);
#else
	extern	void	SimpleTallyTests (Tally(UInt32)& s);
	extern	void	SimpleTallyTests (Tally(SimpleClass)& s);
#endif





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__TallyTests__*/


