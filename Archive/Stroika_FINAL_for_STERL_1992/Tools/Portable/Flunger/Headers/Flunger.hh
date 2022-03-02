/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Flunger__
#define	__Flunger__

/*
 * $Header: /fuji/lewis/RCS/Flunger.hh,v 1.1 1992/09/08 18:24:17 lewis Exp $
 *
 * Description:
 *
 * A Flunger is a 
 *
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Flunger.hh,v $
# Revision 1.1  1992/09/08  18:24:17  lewis
# Initial revision
#
 *
 *
 */

#include	"Config-Foundation.hh"



// Wait til we get latest version from sterl of container stuff - thats what broke...
#define	qPathNameBroke	1

class PathName;
#if		qPathNameBroke
extern	void	FlungeFile (const String& fromFile, const String& toFile, ostream& diagnosticOut, Boolean verbose);
#else
extern	void	FlungeFile (const PathName& fromFile, const PathName& toFile, ostream& diagnosticOut, Boolean verbose);
#endif



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Flunger__*/

