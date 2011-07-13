/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/GDIInit.cc,v 1.2 1992/09/01 15:36:53 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: GDIInit.cc,v $
 *		Revision 1.2  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/04/20  14:15:38  lewis
 *		added typedef char* caddr_t conditional on qSnake && _POSIX_SOURCE, before include of Xutil.h, since that
 *		file references caddr_t which doesn't appear to be part of POSIX.
 *
 *		Revision 1.3  1992/02/12  03:28:28  lewis
 *		Use GDIConfig rather than OSConfig, and get rid of some unneeded code (was cloned from OSInit).
 *
 *		Revision 1.2  1992/01/27  04:11:05  sterling
 *		made compile on Mac
 *
 *		Revision 1.1  1992/01/22  04:15:51  lewis
 *		Initial revision
 *
 *
 */

#include	<stdlib.h>

#if			!qMacOS
#include	<stdio.h>
#endif

#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Dialogs.h>
#include	<Events.h>
#include	<Fonts.h>
#include	<Memory.h>
#include	<Menus.h>
#include	<Events.h>
#include	<OSUtils.h>
#include	<QuickDraw.h>
#include	<SegLoad.h>
#include	<TextEdit.h>
#include	<ToolUtils.h>
#include	<Windows.h>
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<stdio.h>
#include	<X11/Intrinsic.h>
// they undef it - thanx...
#define String osString
#define Boolean osBoolean
#endif	/*OS*/
#include	"OSRenamePost.hh"


#include	"GDIConfiguration.hh"

#include	"GDIInit.hh"




#if		qMacOS
// So we can safely abort when built for 020, and running on 68000
#pragma	processor	68000
#endif	/*qMacOS*/






/*
 ********************************************************************************
 *************************************** OSInit *********************************
 ********************************************************************************
 */

#if		qMacGDI
GDIInit::GDIInit (size_t minHeap, size_t minStack, Boolean require68020Plus, Boolean requireFPU, Boolean requireColorQD):
	OSInit (minHeap, minStack, require68020Plus, requireFPU)
{
	if (requireColorQD) {
		AssureThat (GDIConfiguration ().ColorQDAvailable (), "Color Quickdraw required to run this program.");
	}
}
#elif	qXGDI

struct _XtAppStruct*	gAppContext;
#if		qDebug
static	int	myXErrorHandler (osDisplay* display, XErrorEvent* errorEvent)
{
	char	buf [1024];
	::XGetErrorText (display, errorEvent->error_code, buf, sizeof (buf));
	fprintf (stderr, "X Error:\n"
						"\ttype = %d\n"
						"\tresourceid = %d\n"
						"\tserial# = %d\n"
						"\terror_code = %d\n"
						"\trequest_code = %d\n"
						"\tminor_code = %d\n"
						"\t\"%s\"\n",
				errorEvent->type,
				errorEvent->resourceid,
				errorEvent->serial,
				errorEvent->error_code,
				errorEvent->request_code,
				errorEvent->minor_code,
				buf);
	abort ();
	return (0);
}

static	void	myXtErrorHandler (osString message)
{
	fprintf (stderr, "X Toolkit Error: %s\n", message);
	abort ();
}
#endif	/*qDebug*/

static	void	myXtWarningHandler (osString message)
{
#if		qDebug
	fprintf (stderr, "Xt Warning: %s\n", message);
#else
	// ignore warnings - at least for now with debug off
#endif
}

GDIInit::GDIInit (size_t minHeap, size_t minStack):
	OSInit (minHeap, minStack)
{
	::XtToolkitInitialize ();
	gAppContext = ::XtCreateApplicationContext ();
#if		qDebug
	::XSetErrorHandler (myXErrorHandler);					// override default implementation to cause core dump
	::XtSetErrorHandler (myXtErrorHandler);					// override default implementation to cause core dump
#endif
	::XtAppSetWarningHandler (gAppContext, myXtWarningHandler);
}
#endif	/*GDI*/



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


