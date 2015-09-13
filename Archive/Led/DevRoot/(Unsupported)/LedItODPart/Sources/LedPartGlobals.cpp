/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1995.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/LedPartGlobals.cpp,v 2.4 1996/12/13 18:10:13 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedPartGlobals.cpp,v $
 *	Revision 2.4  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.3  1996/12/05  21:12:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.1  1995/12/06  02:03:11  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
 *
 *
 *
 *
 */

#include	"LedPartGlobals.h"



ODUShort		gGlobalsUsageCount	= 0;
LedPartGlobals*	gGlobals			= NULL;