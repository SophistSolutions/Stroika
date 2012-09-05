/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/SpelledIt/ScavangeWordsFromNet/ScavangerConfig.h,v 1.2 2003/06/18 14:20:58 lewis Exp $
 *
 * Changes:
 *	$Log: ScavangerConfig.h,v $
 *	Revision 1.2  2003/06/18 14:20:58  lewis
 *	cleanups - lose alot of code I wasn't using (but maybe useful in the future as code framgments - like code to fetch URLs using ServerXMLHTTPRequest)
 *	
 *	
 *	
 *
 *
 *
 */
#ifndef	__ScavengerConfig_h__
#define	__ScavengerConfig_h__	1

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off ATL's hiding of some common and often safely ignored warning messages
#define _ATL_ALL_WARNINGS


#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>

using namespace ATL;

#endif
