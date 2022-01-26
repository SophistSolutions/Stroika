/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Debug__
#define	__Debug__

/*
 * $Header: /fuji/lewis/RCS/Debug.hh,v 1.5 1992/10/09 17:47:20 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Debug.hh,v $
 *		Revision 1.5  1992/10/09  17:47:20  lewis
 *		*** empty log message ***
 *
 *		Revision 1.4  1992/09/05  04:19:59  lewis
 *		Renamed NULL->Nil again.
 *
 *		Revision 1.2  1992/07/07  22:41:16  lewis
 *		Got rid of EnsureNotReached - made no sense - ensures supposed to be at
 *		end of function saying stuff about return values and return state.
 *		Get rid of SwitchFallThru - Like idea of being forced to say if its an
 *		Assert/Require/Ensure failre - use AssertNotReached () or
 *		RequireNotReached() instead.
 *		Got rid of PerformanceMeasurer class - Mac dependent, and unused, and
 *		not well done anyhow.
 *
 *		Revision 1.8  1992/05/23  00:11:10  lewis
 *		Use qHPCPlus_BarfsIfAssertReturnsVoid instead of qHPCPlus.
 *
 *		Revision 1.6  1992/01/28  22:46:58  lewis
 *		Allow overriding of the assertion proc.
 *
 *
 *
 */

#include	"Config-Foundation.hh"



/*
 * Printf style debug message. Where this is output is conditioned on the global variable
 * gDebugOutputFormat.
 */
extern	void	DebugMessage (const char* format, ...);


extern	enum DebugOutputFormat {
	eDebugOutputToDebugger,
	eDebugOutputToSTDERR,
	eDebugOutputToFile,			// NYI
}	gDebugOutputFormat;





#if		qDebug


/*
 * An iostream interface to debug messages.
 */
class	ostream;
extern	ostream&	gDebugStream;





/*
 * Assertions support
 */
/*
 * Get/Set the default assertion handling procedure. Nil means use default (as opposed to NONE).
 */
extern	void	(*GetAssertHandler ()) (const char* assertType, const char* expression, const char* file, int lineNumber);
extern	void	SetAssertHandler (void (*newHandler) (const char* assertType, const char* expression, const char* file, int lineNumber));


extern
#if		qHPCPlus_BarfsIfAssertReturnsVoid
int
#else
void
#endif
_Assert (const char* assertType, const char* expression, const char* file, int lineNumber);

#if		qHPCPlus_BarfsIfAssertReturnsVoid
#define	Assert(X)			((X) ? 0 : _Assert ("Assertion", #X, __FILE__, __LINE__))
#define	Require(X)			((X) ? 0 : _Assert ("Requirement", #X, __FILE__, __LINE__))
#define	Ensure(X)			((X) ? 0 : _Assert ("Ensure", #X, __FILE__, __LINE__))
#else
#define	Assert(X)			((X) ? (void) 0 : _Assert ("Assertion", #X, __FILE__, __LINE__))
#define	Require(X)			((X) ? (void) 0 : _Assert ("Requirement", #X, __FILE__, __LINE__))
#define	Ensure(X)			((X) ? (void) 0 : _Assert ("Ensure", #X, __FILE__, __LINE__))
#endif

#define	AssertNotNil(X)		Assert((X)!=Nil)
#define	RequireNotNil(X)		Require((X)!=Nil)
#define	EnsureNotNil(X)		Ensure((X)!=Nil)

#define	AssertNotReached()		_Assert ("Assertion", "Not Reached", __FILE__, __LINE__)
#define	AssertNotImplemented()	_Assert ("Assertion", "Not Implemented", __FILE__, __LINE__)
#define	RequireNotReached()		_Assert ("Requirement", "Not Reached", __FILE__, __LINE__)

#else	/*qDebug*/

#define	Assert(X)				0
#define	Require(X)				0
#define	Ensure(X)				0

#define	AssertNotNil(X)		0
#define	RequireNotNil(X)		0
#define	EnsureNotNil(X)		0

#define	AssertNotReached()		0
#define	AssertNotImplemented()	0
#define	RequireNotReached()		0

#endif	/*qDebug*/






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Debug__*/

