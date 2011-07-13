/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PixelMapEditorMenus__
#define	__PixelMapEditorMenus__

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditorMenus.hh,v 1.1 1992/06/20 18:21:59 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: PixelMapEditorMenus.hh,v $
 *		Revision 1.1  1992/06/20  18:21:59  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/04/29  12:39:06  lewis
 *		Initial revision
 *
 *		Revision 1.1  92/04/20  11:05:24  11:05:24  lewis (Lewis Pringle)
 *		Initial revision
 *
 *
 *
 *
 */

#include	"DebugMenu.hh"
#include	"Menu.hh"
#include	"StandardMenus.hh"


class PixelMapEditorCommandNamesBuilder : public FrameworkCommandNamesBuilder {
	public:
		PixelMapEditorCommandNamesBuilder ();
};


class	PixelMapEditorFileMenu	: public DefaultFileMenu {
	public:
		PixelMapEditorFileMenu ();
};


class	PixelMapEditorEditMenu	: public DefaultEditMenu {
	public:
		PixelMapEditorEditMenu ();
};




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__PixelMapEditorMenus__*/

