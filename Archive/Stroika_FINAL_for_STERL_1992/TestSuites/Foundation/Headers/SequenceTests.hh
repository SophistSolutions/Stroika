/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef		__SequenceTests__
#define		__SequenceTests__

/*
 * $Header: /fuji/lewis/RCS/SequenceTests.hh,v 1.11 1992/11/26 02:53:23 lewis Exp $
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
 *	$Log: SequenceTests.hh,v $
 *		Revision 1.11  1992/11/26  02:53:23  lewis
 *		*** empty log message ***
 *
 *		Revision 1.10  1992/10/08  01:23:42  sterling
 *		Changes for using GenClass instead of declare macros. Also use SimpleClass intead
 *		of String.
 *
 *		Revision 1.9  1992/09/29  15:45:43  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.5  1992/09/15  17:30:22  lewis
 *		Got rid of AbXXX classes - now just XXX.
 *
 *		Revision 1.2  1992/07/03  04:14:55  lewis
 *		Conditionally declare sequence/containers of SimpleClass.
 *
 *		Revision 1.3  1992/06/11  00:07:20  lewis
 *		Got rid of Sort.
 *
 *		Revision 1.2  92/05/10  00:46:37  00:46:37  lewis (Lewis Pringle)
 *		Port to BC++/templates.
 *		
 *
 *
 */

#include	"Sequence.hh"

#include	"SimpleClass.hh"



#if		!qRealTemplatesAvailable
	#include 	"TFileMap.hh"
	#include	SequenceOfUInt32_hh
	#include	SequenceOfSimpleClass_hh
#endif

#if		qRealTemplatesAvailable
	extern	void	SimpleTests (Sequence<UInt32>& s);
	extern	void	Test_Deletions_During_Iteration (Sequence<UInt32>& s);
	
	extern	void	SimpleTests (Sequence<SimpleClass>& s);
	extern	void	Test_Deletions_During_Iteration (Sequence<SimpleClass>& s);
#else
	extern	void	SimpleTests (Sequence(UInt32)& s);
	extern	void	Test_Deletions_During_Iteration (Sequence(UInt32)& s);
	
	extern	void	SimpleTests (Sequence(SimpleClass)& s);
	extern	void	Test_Deletions_During_Iteration (Sequence(SimpleClass)& s);
#endif



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__SequenceTests__*/


