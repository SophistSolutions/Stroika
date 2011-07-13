/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__GraphixResource__
#define	__GraphixResource__

/*
 * $Header: /fuji/lewis/RCS/GraphixResources.hh,v 1.1 1992/06/19 22:33:01 lewis Exp $
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
 *	$Log: GraphixResources.hh,v $
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.2  1992/06/10  02:21:22  lewis
 *		Moved Declare of String Resource back to Foundation:Resource.hh - no reason to not keep
 *		Foundation resources declared there.
 *
Revision 1.1  92/06/09  23:37:34  23:37:34  lewis (Lewis Pringle)
Initial revision

 *
 * << Moved here from Foundation Resource.hh >>
 */

#include	"Resource.hh"

#include	"Color.hh"		// included cuz predeclared resource type
#include	"Font.hh"		// included cuz predeclared resource type
#include	"PixelMap.hh"	// included cuz predeclared resource type
#include	"Point.hh"		// included cuz predeclared resource type
#include	"Rect.hh"		// included cuz predeclared resource type
#include	"Tile.hh"		// included cuz predeclared resource type



Declare (Resource, Color);
Declare (Resource, Font);
Declare (Resource, PixelMap);
Declare (Resource, Point);
Declare (Resource, Rect);
Declare (Resource, Tile);



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif /* __GraphixResources__ */


