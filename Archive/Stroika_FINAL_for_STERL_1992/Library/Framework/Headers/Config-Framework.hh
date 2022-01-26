/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ConfigFramework__
#define	__ConfigFramework__

/*
 * $Header: /fuji/lewis/RCS/Config-Framework.hh,v 1.4 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Config-Framework.hh,v $
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.11  1992/03/09  23:45:14  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *		And cleanups.
 *
 *		Revision 1.10  1992/02/12  06:24:00  lewis
 *		Moved definition of qSupportMneumonics here from MenuItem.hh, or wherever.
 *
 *		Revision 1.8  1992/01/05  06:15:38  lewis
 *		Get rid of backward compatability on control variables - use the new ones
 *		now.
 *
 *		Revision 1.7  1991/12/27  16:49:15  lewis
 *		Use qMac/qX/qXt/qMotif Toolkits rather than qNativeAvailable....
 *
 *
 *
 */


#include	"Config-Graphix.hh"





/*
 * UI defines given here so we can say #if qMacUI, rather than #ifdef, or #if defined ()
 *	Supported gui's include:
 *			qMacUI		-- Apple Macintosh UI
 *			qMotifUI	-- OSF Motif UI
 *			qWinUI		-- Microsoft Windows UI
 */

#if		!qMacUI && !qMotifUI && !qWinUI
#error	"Need to compile for some UI"
#endif

#if		(qMacUI && qWinUI) || (qMotifUI && qWinUI) || (qMacUI && qMotifUI)
#error	"Need to compile for exactly one UI"
#endif


#define	qMouselessInterface		(qMotifUI || qWindowsUI)

#define	qSupportMneumonics		(qMotifUI || qWindowsUI)






/*
 * This decides if we use native or portable controls, windows and do-dads by default.
 */
#ifndef	qNative
#define	qNative		1
#endif



// Sanity check
#if		!qMacToolkit && !qXmToolkit
#error	"need some toolkit"
#endif	/*Toolkit*/

/*
 * Some sanity checks on toolkit flags...
 */
#if		qMacToolkit && !qMacGDI
#error	"Mac toolkit requires mac gdi"
#endif	/*qMacToolkit && !qMacGDI*/

#if		qXToolkit && !qXGDI
#error	"X toolkit requires X gdi"
#endif	/*qXToolkit && !qXGDI*/

#if		qXtToolkit && !qXToolkit
#error	"Xt toolkit requires X toolkit"
#endif	/*qXtToolkit && !qXToolkit*/

#if		qXmToolkit && !qXtToolkit
#error	"Motif toolkit requires Xt"
#endif	/*qXmToolkit && !qXtToolkit*/









/*
 * Application configuration defines (this used to be in Application since they are only
 * used there, but if we treat this file as what we use for configuration info, then maybe
 * they belong here?
 */

#if		qMacOS
// save space in Stroika by defining this to be zero
#ifndef	qIncludeErrorStrings
#define	qIncludeErrorStrings	1
#endif
#endif	/*qMacOS*/


// save space in Stroika by defining this to be zero
#ifndef	qSupportClipBoardWindow
#define	qSupportClipBoardWindow	1
#endif


// save space in Stroika by defining this to be zero
#ifndef	qSupportAutoBusyCursor
#define	qSupportAutoBusyCursor	1
#endif








/*
 * This is the list of backward compatability #defines.
 */

#ifndef	qScrollerWindowCTOR_BackwardCompatability
#define	qScrollerWindowCTOR_BackwardCompatability	0
/*
 * <<Describe here how to avoid dependence on this change...>>>
 */
#endif






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__ConfigFramework__*/
