/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/NetLedIt/Sources/Stubs.c,v 1.1 2004/01/01 04:20:41 lewis Exp $
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Stubs.c,v $
 *	Revision 1.1  2004/01/01 04:20:41  lewis
 *	moved NetLedIt to (Unsupported)
 *	
 *	Revision 1.5  2002/05/06 21:35:08  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.4  2001/11/27 00:33:05  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.3  2001/08/30 00:43:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.2  2000/09/28 19:50:37  lewis
 *	Finished stubs for (ActiveLedIt! based) Java API for NetLedIt!.
 *	
 *
 *
 *
 *
 */


/*
** Ok, so we don't usually include .c files (only .h files) but we're
** doing it here to avoid some fancy make rules. First pull in the common
** glue code:
*/
#ifdef XP_UNIX
#include "../../../common/npunix.c"
#endif

/*
** Next, define IMPLEMENT_NetLedIt in order to pull the stub code in when
** compiling the Simple.c file. Without doing this, we'd get a
** UnsatisfiedLinkError whenever we tried to call one of the native
** methods:
*/
#define IMPLEMENT_NetLedIt
/*
** Finally, include the native stubs, initialization routines and
** debugging code. You should be building with DEBUG defined in order to
** take advantage of the diagnostic code that javah creates for
** you. Otherwise your life will be hell.
*/

#if		_MSC_VER
	#pragma	warning (disable : 4244)
	#pragma	warning (disable : 4761)
#endif

#if 1
	#include "jri.h"
	#define virtual
	#include "NetLedIt.c"
	#include "netscape_plugin_Plugin.c"
	#undef virtual
#else
#include "NetLedIt.c"
#include "netscape_plugin_Plugin.c"
#endif
