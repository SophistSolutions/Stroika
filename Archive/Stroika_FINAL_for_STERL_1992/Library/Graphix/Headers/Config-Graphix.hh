/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__ConfigGraphix__
#define	__ConfigGraphix__

/*
 * $Header: /fuji/lewis/RCS/Config-Graphix.hh,v 1.2 1992/09/01 15:34:49 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Supported GDIs include:
 *		qMacGDI
 *		qXGDI
 *		qWinGDI
 *
 * Changes:
 *	$Log: Config-Graphix.hh,v $
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/02/14  02:33:11  lewis
 *		Added qKeepRegionAllocStatistics define.
 *
 *
 *
 */

#include	"Config-Foundation.hh"



/*
 * Maintain statistics in the region allocation code. This is almost only ever done
 * when debug is turned on, but can be done seperately if need be. It is sufficiently
 * cheap it might as well always be done when debug is turned on.
 */
#define		qKeepRegionAllocStatistics		qDebug





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__ConfigGraphix__*/
