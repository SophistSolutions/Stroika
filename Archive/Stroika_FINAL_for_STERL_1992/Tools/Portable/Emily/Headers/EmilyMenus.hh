/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__EmilyMenus__
#define	__EmilyMenus__

/*
 * $Header: /fuji/lewis/RCS/EmilyMenus.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: EmilyMenus.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 *
 */

#include	"Menu.hh"
#include	"StandardMenus.hh"
#include	"UserMenus.hh"


class CommandNamesBuilder : public UserCommandNamesBuilder {
	public:
		CommandNamesBuilder ();
};

class EmilyFileMenu : public DefaultFileMenu {
	public:
		EmilyFileMenu ();
};

class EmilyEditMenu : public DefaultEditMenu {
	public:
		EmilyEditMenu ();
};

class EmilyFormatMenu : public DefaultFormatMenu {
	public:
		EmilyFormatMenu ();
		~EmilyFormatMenu ();
	
		Menu	fAlignmentMenu;
		Menu	fHorizontalSpacingMenu;
		Menu	fVerticalSpacingMenu;
		Menu	fAttachmentMenu;
};

class EmilyCustomizeMenu : public Menu {
	public:
		EmilyCustomizeMenu ();
		virtual ~EmilyCustomizeMenu ();

		static	String	DefaultName ();
		
		Menu	fLanguagesMenu;
		Menu	fGUIMenu;
};


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/* __EmilyMenus__ */
