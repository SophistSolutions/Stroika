/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TearOffMenu__
#define	__TearOffMenu__

/*
 * $Header: /fuji/lewis/RCS/TearOffMenu.hh,v 1.4 1992/09/08 16:00:29 lewis Exp $
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
 *
 * Changes:
 *	$Log: TearOffMenu.hh,v $
 *		Revision 1.4  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/16  15:54:53  sterling
 *		modified to support new menu scheme
 *
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.12  1992/02/28  15:56:40  lewis
 *		Renamed TearOffCustomMenu to TearOffMenu.
 *
 *		Revision 1.11  1992/02/28  15:37:45  lewis
 *		Update signature of ChooseItem method to reflect change in base class Menu.
 *
 *		Revision 1.10  1992/02/14  03:37:50  lewis
 *		Change args to ChooseItem to reflect change in class Menu, and make get rid
 *		of MenuRect in InTearRegion for the same reason (that param removed from ChosoeItem). Now use GetLocalExtent. Also,
 *		changed semantics of both those routines to use local coordinates rather than
 *		global.
 *		Also, only conditionally be a virutal subclass of Menu depending on qMPW_VIRTUAL_BASE_CLASS_BUG.
 *
 *		Revision 1.9  1992/02/12  07:24:27  lewis
 *		Make TearOffCustomMenu a virtual Menu instead of a CustomMenu (btw, should be called TearOffMenu instead).
 *
 *
 */

#include	"Menu.hh"


class	Window;
class	TearOffMenu : public Menu {
	public:
		TearOffMenu (Window* w = Nil);
		~TearOffMenu ();

		nonvirtual	Window&	GetTearOffWindow () const;
		
	protected:
		override	CollectionSize	ChooseItem (const Point& hitPt, CollectionSize whichItem);

		nonvirtual	Boolean	InTearRegion (const Point& p) const;
		
		virtual		Menu*	BuildTornOffView () = Nil;

	private:
		Window* 	fTearOffWindow;
		Boolean		fBuiltWindow;

	friend	class	TearOffDragger;
	friend	class	TearOffCommand;
};



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/* __TearOffMenu__ */
