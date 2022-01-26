/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__StandardMenus__
#define	__StandardMenus__

/*
 * $Header: /fuji/lewis/RCS/StandardMenus.hh,v 1.1 1992/06/20 17:27:41 lewis Exp $
 *
 * Description:
 *		Some commonly used menus to just use, or perhaps to subclass from if your menu
 * is to be closely related to a standard one. Most of these menus are simple subclasses
 * of Menu that add the standard menu items for that kind of menu.
 *
 *		SystemMenu is specail in that its meaning is VERY target WindowSystem/GUI/Toolkit
 * dependent. Care should be used in doing anything out of the ordinary with this class (at least
 * right now). For now, best results if you always create it, and on mac add it as first item
 * to Application MenuBar, and do nothing with it under Motif. Subject to change...
 *
 *		DefaultFileMenu is a convience class that allows you to quickly "get up" a file menu
 * in the right format/language/gui etc currently in use. It can be subclassed, and the particular
 * items changed, or its items can be changed externally (say from your subclass of Application that
 * ows this). That allows you to specify just incremental changes in YOUR FILE MENU from the
 * NATURAL (or default for GUI) FILE Menu.
 *
 *		DefaultEditMenu is a convience class that allows you to quickly "get up" an edit menu
 * in the right format/language/gui etc currently in use. It can be subclassed, and the particular
 * items changed, or its items can be changed externally (say from your subclass of Application that
 * ows this). That allows you to specify just incremental changes in YOUR EDIT MENU from the
 * NATURAL (or default for GUI) EDIT Menu.
 *
 *
 * Notes:
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: StandardMenus.hh,v $
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.6  1992/03/26  17:16:39  sterling
 *		added FrameworkCommandNameBuilder
 *
 *		Revision 1.5  1992/03/06  02:35:15  lewis
 *		Added dtor to call DeleteMenuItems () - not really a very safe change, but all thats practical
 *		given sterls new change to View - not deleting subviews, and assuming removed from parent before
 *		thier deletion. I think this change is good, but must study its impact better.
 *
 *		Revision 1.4  1992/03/05  18:02:28  lewis
 *		Added Description.
 *
 *		Revision 1.3  1992/03/05  17:57:10  lewis
 *		Added DefaultName () static member to each class.
 *		Made SystemMenu buildable on all systems - only interesting for now on mac,
 *		but should move code to support system menu for motif here. Only diff is that
 *		the apps are responsible to NOT put this into the MenuBar on motif, and to doso
 *		for the mac (if desired).
 *		Made SystemMenu building multiple times and added static members so Application can
 *		dispatch to us, and we can deal with iterating over all the menus as neccesary, and we
 *		can deal with the eventuality that there might be no system menu HERE.
 *
 *		Revision 1.2  1992/03/03  03:52:59  lewis
 *		Added SystemMenu (forgotton accidentally before) and DefaultFile/Edit Menus.
 *
 *		Revision 1.1  1992/03/02  22:51:56  lewis
 *		Initial revision
 *
 *
 */

#include	"Command.hh"
#include	"Menu.hh"



class FrameworkCommandNamesBuilder {
	public:
		FrameworkCommandNamesBuilder ();
};


/*
 * Although you can Create a SystemMenu object for any system, and as many as you wish, they
 * should only be installed in very system dependent ways, or they will not function properly.
 * 	<< THESE SHOULD BE ENUMERATED MORE CAREFULLY, BUT FOR NOW, LOOK AT SAMPLE PROGRAMS>>
 */
class	SystemMenu : public Menu {
	public:
		SystemMenu ();
		virtual ~SystemMenu ();

	public:
		static	String	DefaultName ();

#if		qMacToolkit
		// typically called by application to enable the desk accessory items in app SystemMenus - usually only
		// one....
		static		void	EnableDeskAccessories (Boolean enable);

		override	void	BeginUpdate ();

		override	Boolean	GetEnabled () const;
	private:
		nonvirtual	void	EnableDAs (Boolean enable);
#endif	/*Toolkit*/
};




class	DefaultFileMenu : public Menu {
	public:
		DefaultFileMenu ();
		virtual ~DefaultFileMenu ();

		static	String	DefaultName ();
};



class	DefaultEditMenu : public Menu {
	public:
		DefaultEditMenu ();
		virtual ~DefaultEditMenu ();

		static	String	DefaultName ();
};





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__StandardMenus__*/

