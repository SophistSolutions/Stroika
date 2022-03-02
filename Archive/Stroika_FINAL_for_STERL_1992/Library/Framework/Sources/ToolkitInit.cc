/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ToolkitInit.cc,v 1.2 1992/09/01 15:46:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ToolkitInit.cc,v $
 *		Revision 1.2  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.3  1992/04/20  14:36:08  lewis
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		Other cleanuyps - like adding header for rcs log.
 *
 *		Revision 1.2  92/01/27  04:08:54  04:08:54  sterling (Sterling Wight)
 *		made compile on mac
 *		
 *		Revision 1.1  1992/01/22  17:31:32  lewis
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
#include	<X11/Intrinsic.h>
// they undef it - thanx...
#define String osString
#define Boolean osBoolean
#endif	/*OS*/
#include	"OSRenamePost.hh"


#include	"OSConfiguration.hh"

#include	"ToolkitInit.hh"






/*
 ********************************************************************************
 ********************************** ToolkitInit *********************************
 ********************************************************************************
 */

#if		qMacToolkit
ToolkitInit::ToolkitInit (size_t minHeap, size_t minStack, Boolean require68020Plus, Boolean requireFPU, Boolean requireColorQD):
	GDIInit (minHeap, minStack, require68020Plus, requireFPU, requireColorQD)
{
}
#elif	qXtToolkit

ToolkitInit::ToolkitInit (size_t minHeap, size_t minStack):
	GDIInit (minHeap, minStack)
{
}
#endif	/*Toolkit*/



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


