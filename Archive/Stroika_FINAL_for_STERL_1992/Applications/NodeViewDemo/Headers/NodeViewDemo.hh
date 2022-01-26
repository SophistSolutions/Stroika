#ifndef	__NodeViewDemo__
#define	__NodeViewDemo__

/*
 * $Header: /fuji/lewis/RCS/NodeViewDemo.hh,v 1.2 1992/09/01 17:30:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: NodeViewDemo.hh,v $
 *		Revision 1.2  1992/09/01  17:30:27  sterling
 *		*** empty log message ***
 *
 *		Revision 1.6  1992/03/07  13:37:56  lewis
 *		Fixed more new menu support, and got reading working again - fixed to be compat with
 *		sterls name changes in FileBasedDocuement for read/write routines.
 *
 *
 *
 */

#include	"Application.hh"
#include	"DebugMenu.hh"
#include	"Menu.hh"
#include	"MenuBar.hh"
#include	"StandardMenus.hh"
#include	"UserMenus.hh"

class NodeViewCommandNamesBuilder : public UserCommandNamesBuilder {
	public:
		NodeViewCommandNamesBuilder ();
};

class	NodeViewDemo : public Application {
	public:
		NodeViewDemo (int argc, const char* argv []);
		~NodeViewDemo ();

		override	void		DoSetupMenus ();
		override	Boolean		DoCommand (const CommandSelection& selection);

		static	NodeViewDemo&	Get ();

	private:
		NodeViewCommandNamesBuilder	fNamesBuilder;
		MenuBar						fMenuBar;
		SystemMenu					fSystemMenu;
		DefaultFileMenu				fFileMenu;
		DefaultEditMenu				fEditMenu;
#if		qDebug
		DebugMenu					fDebugMenu;
#endif

		static	NodeViewDemo*	sThe;
};




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/* __NodeViewDemo__ */

