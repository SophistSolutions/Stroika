/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef		__StdAfx__h__
#define 	__StdAfx__h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedTestWin32/Headers/StdAfx.h,v 1.6 2002/05/06 21:36:36 lewis Exp $
 *
 * Description:
 *
 *
 * Changes:
 *	$Log: StdAfx.h,v $
 *	Revision 1.6  2002/05/06 21:36:36  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.5  2001/11/27 00:33:29  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.4  2001/08/30 01:09:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.3  1999/12/09 17:35:54  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 1.2  1999/11/29 22:37:26  lewis
 *	remove some stuff - so compiles on MWERKS compiler
 *	
 *	Revision 1.1  1999/11/15 22:16:59  lewis
 *	Initial revision
 *	
 *
 *
 */


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#ifdef _UNICODE
	#ifndef UNICODE
	#define UNICODE         // UNICODE is used by Windows headers
	#endif
#endif

#ifdef UNICODE
	#ifndef _UNICODE
	#define _UNICODE        // _UNICODE is used by C-runtime/MFC headers
	#endif
#endif



#include	<windows.h>

#if 0
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#endif


#endif

