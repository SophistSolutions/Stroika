/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__CodeGenUtils__
#define	__CodeGenUtils__

/*
 * $Header: /fuji/lewis/RCS/CodeGenUtils.hh,v 1.1 1992/06/20 17:33:49 lewis Exp $
 *
 * Description:
 *		Utility routines to do C++ code generation of Stroika constructs, from interal
 *	Stroika types. For example, if you have a PixelMap in memory, and wish to write it out
 *	in a form that can be compiled, utilities like that go here.
 *
 *		These things are mainly used by the Stroika DialogEditor, but it was deeemed
 *	plausible that they might be useful elsewhere - eg a little PixelMapEditor.
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: CodeGenUtils.hh,v $
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.2  1992/04/27  18:44:05  lewis
 *		Added WriteAsCString (ostream& out, const String& s) and WriteAsPixelMapFromData (ostream& out, const PixelMap& pm, const String& varName).
 *
 *		Revision 1.1  92/04/16  12:36:53  12:36:53  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *
 *
 *
 */

#include	"Config-Foundation.hh"


class	ostream;

/*
 * Write the given string out as a C string so that it can be parsed by a compiler. Take care of
 * any funny embedded characters, and if the line is too long, break it up a bit. Make it look pretty
 * normal for the most likely case that the string has nothing unusual about it.
 */
class	String;
extern	ostream&	WriteAsCString (ostream& out, const String& s);


/*
 * Write out the given pixel map in such a way that it can be compiled by Stroika later. This means write an
 * Array for the CLUT - iff need be, and build that clut. And similarly for the data for the PixelMap. Generate
 * a variable for the PixelMap with the given name. Any other names needed by this process should be manged variants
 * of varName.
 */
class	PixelMap;
extern	ostream&	WriteAsPixelMapFromData (ostream& out, const PixelMap& pm, const String& varName);







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


#endif	/*__CodeGenUtils__*/
