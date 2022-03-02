/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Debug.cc,v 1.7 1992/09/15 17:15:54 lewis Exp $
 *
 * TODO:
 *			-- Rewrite DebugMessage () to use Borland "DebugBreak (void)";
 *             And OutputDebugString (LPSTR) to output the string.
 *				Also, call ValidateCodeSegments() and ValidateFreeSpaces ()
 *				from our Validate call....(not too sure abuot these - need a debug monotor???).
 *
 *			--	Also, in DebugMessage, use va_args stuff in mac os version too. ANd
 *				to UNIX one more portable/safely - look up how its defined to work.
 *
 * Changes:
 *	$Log: Debug.cc,v $
 *		Revision 1.7  1992/09/15  17:15:54  lewis
 *		Save #if  qGCC && (qSnake || qSparc) rather than just qGCC before
 *		calling _builtin_saveregs_ in DebugMessage(). Not quite sure why
 *		as I have not yet checked the Docs, but it crashes on mac. Probably
 *		only needed on RISC machines? Still, it shouldn't crash! Investigate
 *		later.
 *
 *		Revision 1.6  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.4  1992/07/08  17:37:38  lewis
 *		Had commented out include of Types.h cuz I wasnt sure what it was for. Now I
 *		am - it is for debugstr () so commented back in with explanation as to what
 *		it is for.
 *
 *		Revision 1.3  1992/07/08  01:17:33  lewis
 *		Get rid of PerfomanceMeasurer class.
 *		Minor cleanups including new style for switch statements.
 *
 *		Revision 1.2  1992/07/01  07:40:49  lewis
 *		Add TODO comments about fixing DebugMessage90.
 *
 *		Revision 1.1  1992/06/19  22:29:57  lewis
 *		Initial revision
 *
 *		Revision 1.16  1992/05/23  00:12:33  lewis
 *		Use qHPCPlus_BarfsIfAssertReturnsVoid instead of qHPCPlus.
 *
 *		Revision 1.15  92/05/09  01:34:15  01:34:15  lewis (Lewis Pringle)
 *		Make notes for debugging things to FIX for PC (ported to PC).
 *		
 *
 */




#include	<stdio.h>
#include	<stdarg.h>		// sunCFront 2.1 requires this file to come after stdio.h!!
#include	<stdlib.h>
#include	<string.h>


#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Types.h>			// for debugstr ()
#endif	/*qMacOS*/
#include	"OSRenamePost.hh"

#include	"OSConfiguration.hh"

#include	"Debug.hh"



DebugOutputFormat	gDebugOutputFormat	=	eDebugOutputToDebugger;




/*
 ********************************************************************************
 ******************************** AssertHandlers ********************************
 ********************************************************************************
 */
static	void	(*sAssertHandler) (const char* assertType, const char* expression, const char* file, int lineNumber)	 = Nil;

void	(*GetAssertHandler ()) (const char* assertType, const char* expression, const char* file, int lineNumber)
{
	return (sAssertHandler);
}

void	SetAssertHandler (void (*newHandler) (const char* assertType, const char* expression, const char* file, int lineNumber))
{
	sAssertHandler = newHandler;
}










/*
 ********************************************************************************
 ************************************* _Assert **********************************
 ********************************************************************************
 */

#if		qDebug
#if	qHPCPlus_BarfsIfAssertReturnsVoid
int
#else
void
#endif
_Assert (const char* assertType, const char* expression, const char* file, int lineNumber)
{
	if (sAssertHandler == Nil) {
		switch (gDebugOutputFormat) {
			case eDebugOutputToDebugger:
#if		qMacOS
				DebugMessage ("File '%s'# Line %d \n## %s failed: %s\n", file, lineNumber, assertType, expression);
				break;
#else
				// fall thru
#endif
			case eDebugOutputToSTDERR:
				DebugMessage ("File '%s'; Line %d ## %s failed: %s\n", file, lineNumber, assertType, expression);
				abort ();
				break;
		}
	}
	else {
		(*sAssertHandler) (assertType, expression, file, lineNumber);
	}
#if		qHPCPlus_BarfsIfAssertReturnsVoid
	return (0);
#endif
}
#endif	/*qDebug*/










/*
 ********************************************************************************
 ******************************** DebugMessage **********************************
 ********************************************************************************
 */
void	DebugMessage (const char* format, ...)
{
#if		qGCC && (qSnake || qSparc)
	__builtin_saveregs ();
#endif	/*qGCC*/
	switch (gDebugOutputFormat) {
		case	eDebugOutputToDebugger:
#if		qMacOS
			if (OSConfiguration().SystemDebuggerInstalled ()) {
				char	bigBuf [1024];
				vsprintf (bigBuf, format, (char*)&format+4);
				if (strlen (bigBuf) > 255) {
					debugstr ("message too long (truncating...)");
					bigBuf[255] = 0;
				}
				debugstr (bigBuf);
				break;
			}
			else {
				static	FILE*	outFile	=	fopen ("*debugfile*", "w");
				if (outFile != Nil) {
					vfprintf (outFile, format, (char*)&format+4);
					(void)fprintf (outFile, "\n");
					fflush (outFile);
				}
			}
			break;
#else	/*qMacOS*/
			// fall thru
#endif	/*qMacOS*/
		case	eDebugOutputToSTDERR:
			{
	// works on hp, sun...
	// I think this code here is right, and should work anywhere - test...
	// ALSO, I thought I did this right once before for BorlandC++ - what happend??
				va_list	ap;
				va_start (ap, format);
	// I THINK ITS INHERITNELY AMBIGUOUS IF VALIST IS CRRUENLY AT RIGHT PLACE FOR REST
	// OF STUFF, OR NOT???
	//			(void)va_arg (ap, char*);		// skip format
				vfprintf (stderr, format, ap);
				va_end (ap);
				(void)fprintf (stderr, "\n");
			}
			break;
	}
}










/*
 ********************************************************************************
 *************************** __pure_virtual_called ******************************
 ********************************************************************************
 */
extern	"C"	void	__pure_virtual_called ()
{
	AssertNotReached ();		// MPW version just to exit () - which doesn't tell us much
}









// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

