/* Copyright (c) Sophist Solutions Inc. 1990.  All rights reserved */
#ifndef		__Stroika_Globals__
#define		__Stroika_Globals__

/*
 * $Header: /fuji/lewis/RCS/Stroika-Globals.hh,v 1.1 1992/06/19 22:26:56 lewis Exp $
 *
 * Description:
 *
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Stroika-Globals.hh,v $
 *		Revision 1.1  1992/06/19  22:26:56  lewis
 *		Initial revision
 *
 *		Revision 1.5  1992/02/19  05:06:58  lewis
 *		*** empty log message ***
 *
 *		Revision 1.4  1992/02/17  22:53:16  lewis
 *		Added version number stuff for UNIX.
 *
 *
 *
 */

#include	"Stroika-User-Globals.hh"

#include	"Version.hh"
#if	qMacToolkit
extern	const	Version	kStroikaVersion	=	Version (128);
#else
#include	"StroikaVersion"
#define	development	Version::eDevelopment
#define	alpha		Version::eAlpha
#define	beta		Version::eBeta
#define	release		Version::eRelease
extern	const	Version	kStroikaVersion	=	Version (kStroikaMajorVersion, kStroikaMinorVersion, kStroikaBugFixVersion,
							 kStroikaStage, kStroikaStageVersion,
							 kStroikaShortVersionString, kStroikaLongVersionString);
#endif


#endif		/*__Stroika_Globals__*/


