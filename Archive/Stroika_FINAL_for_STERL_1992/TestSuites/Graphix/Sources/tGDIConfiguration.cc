/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/tGDIConfiguration.cc,v 1.1 1992/09/04 02:09:44 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: tGDIConfiguration.cc,v $
 *		Revision 1.1  1992/09/04  02:09:44  lewis
 *		Initial revision
 *
 *		Revision 1.2  1992/09/01  17:52:15  sterling
 *		*** empty log message ***
 *
 *		Revision 1.1  1992/06/20  18:25:55  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/05/01  23:17:59  lewis
 *		Initial revision
 *
 *		Revision 1.7  92/03/24  01:52:35  01:52:35  lewis (Lewis Pringle)
 *		*** empty log message ***
 *		
 *		Revision 1.5  1992/01/22  04:11:30  lewis
 *		 Always call OSInit, and cleanups.
 *
 *
 */

/*
 * Has to come before other includes, so that we know this constructor called VERY early in
 * the game.
 */
#include	"GDIInit.hh"
#pragma	push
#pragma	force_active	on
static	GDIInit	sGDIInit	=	GDIInit ();
#pragma	pop

#include	"Stroika-Graphix-Globals.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"

#include	"TestSuite.hh"




int	main (int /*argc*/, char* /*argv*/[])
{
#if		qMacGDI && qDebug
	/*
	 * Since builtin cout/cerr wont work - at least not well.
	 */
	cout = gDebugStream;
	cerr = gDebugStream;
#endif

	GDIConfiguration	gdiConfig;

	/*
	 * Some consistency checks.
	 */
// NYI	TestCondition (gdiConfig.TotalStackSpace () > gdiConfig.StackAvailable ());
#if		qMacOS
	TestCondition (OSConfiguration ().Has68020Plus () or not (OSConfiguration ().HasFPU ()));
#endif



	/*
	 * Some tests for my current machine so I know things are working.
	 */
// NYI	DebugMessage ("gdiConfig.TotalStackSpace () = %lx", gdiConfig.TotalStackSpace ());
	DebugMessage ("gdiConfig.StackAvailable () = %lx", gdiConfig.StackAvailable ());
// NYI	DebugMessage ("gdiConfig.TotalHeapSpace () = %lx", gdiConfig.TotalHeapSpace ());
// NYI	DebugMessage ("gdiConfig.HeapAvailable () = %lx", gdiConfig.HeapAvailable ());

#if		qMacGDI
	cerr << "gdiConfig.ColorQDAvailable () = " << gdiConfig.ColorQDAvailable () << newline;
	cerr << "gdiConfig.ColorQD32Available () = " << gdiConfig.ColorQD32Available () << newline;
	cerr << "gdiConfig.TimeManagerAvailable () = " << gdiConfig.TimeManagerAvailable () << newline;
	cerr << "gdiConfig.Has68020 () = " << gdiConfig.Has68020 () << newline;
	cerr << "gdiConfig.Has68020Plus () = " << gdiConfig.Has68020Plus () << newline;
	cerr << "gdiConfig.HasFPU () = " << gdiConfig.HasFPU () << newline;
#endif
	return (0);
};




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

