/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef		__TestSuite__
#define		__TestSuite__

/*
 * $Header: /fuji/lewis/RCS/TestSuite.hh,v 1.3 1992/11/26 02:53:33 lewis Exp $
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
 *		Revision 1.3  1992/11/26  02:53:33  lewis
 *		*** empty log message ***
 *
 *		Revision 1.2  1992/09/11  13:17:18  lewis
 *		Use endl instead of newline and getrid of include of StreamUtils - use iostream.h instead.
 *
 *		Revision 1.6  92/03/11  23:11:46  23:11:46  lewis (Lewis Pringle)
 *		Use cout/cerr instead of gDebugStream since thats not available when debug turned off anyhow, and using
 *		two different streams allows us to differentiate info from actual errors.
 *		
 *
 *
 */

#include	<iostream.h>

#include	"Debug.hh"




/*
 * Dont use assert in case debug turned off.  Want to be able to test this
 * stuff with/without debug!
 */



#define	TestCondition(e)\
	if (!(e)) cout << "File '" << __FILE__ << "'; Line "  << __LINE__ << " ## TEST FAILED: " #e << endl;




#endif		/*__TestSuite__*/


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


