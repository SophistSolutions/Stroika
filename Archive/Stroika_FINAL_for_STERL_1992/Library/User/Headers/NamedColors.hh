/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__NamedColors__
#define	__NamedColors__

/*
 * $Header: /fuji/lewis/RCS/NamedColors.hh,v 1.3 1992/09/01 15:54:46 sterling Exp $
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
 *	$Log: NamedColors.hh,v $
 *		Revision 1.3  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/01  04:08:09  lewis
 *		Remamed Strings.hh to String.hh.
 *
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.1  92/05/19  13:42:32  13:42:32  sterling (Sterling Wight)
 *		Initial revision
 *
 *		
 *		
 *
 */

#include	"Mapping.hh"
#include	"Mapping_HashTable.hh"
#include	"String.hh"

#include	"Color.hh"

#if		!qRealTemplatesAvailable
	Declare (Iterator, Color);
	Declare (Collection, Color);
	AbMappingDeclare (String, Color);
	Mapping_HTDeclare(String, Color);
#endif



extern	const	AbMapping(String, Color)&	GetNamedColors ();

extern	Color	StringToColor (const String& colorName);
extern	String	ColorToXFormatColorName (Color c);




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/* __NamedColors__ */
