/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PixelMapFormat__
#define	__PixelMapFormat__

/*
 * $Header: /fuji/lewis/RCS/PixelMapFormats.hh,v 1.2 1992/09/01 15:54:46 sterling Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: PixelMapFormats.hh,v $
 *		Revision 1.2  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  92/05/19  11:55:57  11:55:57  lewis (Lewis Pringle)
 *		Implemented read XPM/XMP2, and write of XPM (write of xpm2 coming soon).
 *		
 *		Revision 1.1  92/05/18  18:28:28  18:28:28  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *		
 *
 */

#include	"Config-Foundation.hh"

class	istream;
class	ostream;
class	PixelMap;

extern	istream&	ReadPixelMapFromXPM (istream& from, PixelMap& into, String& name);
extern	ostream&	WritePixelMapToXPM (ostream& to, const PixelMap& from, const String& name);
extern	istream&	ReadPixelMapFromXPM2 (istream& from, PixelMap& into);
extern	ostream&	WritePixelMapToXPM2 (ostream& to, const PixelMap& from);



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/* __PixelMapFormat__ */
