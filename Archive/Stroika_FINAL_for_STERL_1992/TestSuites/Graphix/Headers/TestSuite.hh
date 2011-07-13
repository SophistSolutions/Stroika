/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef		__TestSuite__
#define		__TestSuite__

/*
 * $Header: /fuji/lewis/RCS/TestSuite.hh,v 1.1 1992/09/04 02:09:24 lewis Exp $
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
 *	$Log: TestSuite.hh,v $
 *		Revision 1.1  1992/09/04  02:09:24  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/06/20  18:25:34  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/05/01  23:17:41  lewis
 *		Initial revision
 *
 *		Revision 1.6  92/03/11  23:11:46  23:11:46  lewis (Lewis Pringle)
 *		Use cout/cerr instead of gDebugStream since thats not available when debug turned off anyhow, and using
 *		two different streams allows us to differentiate info from actual errors.
 *		
 *		Revision 1.5  1992/01/22  04:13:06  lewis
 *		*** empty log message ***
 *
 *
 *
 */

#include	"Debug.hh"
#include	"StreamUtils.hh"




/*
 * Dont use assert in case debug turned off.  Want to be able to test this
 * stuff with/without debug!
 */



#define	TestCondition(e)\
	if (!(e)) cerr << "File '" << __FILE__ << "'; Line "  << __LINE__ << " ## TEST FAILED: " #e << newline;




#endif		/*__TestSuite__*/


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


