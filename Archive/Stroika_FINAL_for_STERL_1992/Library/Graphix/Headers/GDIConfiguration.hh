/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__GDIConfiguration__
#define	__GDIConfiguration__

/*
 * $Header: /fuji/lewis/RCS/GDIConfiguration.hh,v 1.2 1992/09/01 15:34:49 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *		(1)		Add support to check for various windowmanagers, and maybe also X x-tensions..???
 *
 * Notes:
 *
 * Changes:
 *	$Log: GDIConfiguration.hh,v $
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/02/08  04:50:13  lewis
 *		Moved some stuff here from OSConfiguration.
 *
 *		Revision 1.1  1992/01/22  05:40:51  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"OSConfiguration.hh"

#include	"Config-Graphix.hh"




class	GDIConfiguration : public OSConfiguration {
	public:
		GDIConfiguration ();

#if		qMacGDI
		nonvirtual	Boolean	ColorQDAvailable () const;
		nonvirtual	Boolean	ColorQD32Available () const;

		nonvirtual	Boolean	HasOutlineFonts () const;
#endif

};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__GDIConfiguration__*/

