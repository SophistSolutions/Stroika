/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ToolkitConfiguration__
#define	__ToolkitConfiguration__

/*
 * $Header: /fuji/lewis/RCS/ToolkitConfiguration.hh,v 1.3 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *		(1)		Add support to check for various windowmanagers, and maybe also X x-tensions..???
 *
 * Notes:
 *
 * Changes:
 *	$Log: ToolkitConfiguration.hh,v $
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/01/22  17:31:19  lewis
 *		Initial revision
 *
 *
 */

#include	"GDIConfiguration.hh"

#include	"Config-Framework.hh"


class	ToolkitConfiguration : public GDIConfiguration {
	public:
		ToolkitConfiguration ();
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__ToolkitConfiguration__*/

