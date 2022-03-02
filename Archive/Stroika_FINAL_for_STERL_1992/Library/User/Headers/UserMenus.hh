/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__UserMenus__
#define	__UserMenus__

/*
 * $Header: /fuji/lewis/RCS/UserMenus.hh,v 1.1 1992/06/20 17:33:49 lewis Exp $
 *
 * Description:
 *		Some commonly used menus to just use, or perhaps to subclass from if your menu
 * is to be closely related to a standard one. Most of these menus are simple subclasses
 * of Menu that add the standard menu items for that kind of menu.
 *
 *
 *
 * Notes:
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: UserMenus.hh,v $
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.2  1992/04/02  17:43:59  lewis
 *		Make Menus in FormatMenu protected.
 *
# Revision 1.1  92/03/26  16:47:31  16:47:31  sterling (Sterling Wight)
# Initial revision
# 
 *
 *
 */

#include	"Menu.hh"
#include	"StandardMenus.hh"

#include	"UserCommand.hh"



class UserCommandNamesBuilder : public FrameworkCommandNamesBuilder {
	public:
		UserCommandNamesBuilder ();
};



class DefaultFontMenu : public Menu {
	public:
		DefaultFontMenu ();
		virtual ~DefaultFontMenu ();

		static	String	DefaultName ();
};



class DefaultFontSizeMenu : public Menu {
	public:
		DefaultFontSizeMenu ();
		virtual ~DefaultFontSizeMenu ();

		static	String	DefaultName ();
};



class DefaultFontStyleMenu : public Menu {
	public:
		DefaultFontStyleMenu ();
		virtual ~DefaultFontStyleMenu ();

		static	String	DefaultName ();
};



class DefaultFontJustificationMenu : public Menu {
	public:
		DefaultFontJustificationMenu ();
		virtual ~DefaultFontJustificationMenu ();

		static	String	DefaultName ();
};



class DefaultFormatMenu : public Menu {
	public:
		DefaultFormatMenu ();
		virtual ~DefaultFormatMenu ();

		static	String	DefaultName ();
	
	protected:
		DefaultFontMenu					fFontMenu;
		DefaultFontSizeMenu				fFontSizeMenu;
		DefaultFontStyleMenu			fFontStyleMenu;
		DefaultFontJustificationMenu	fFontJustificationMenu;
};





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


#endif	/*__UserMenus__*/

