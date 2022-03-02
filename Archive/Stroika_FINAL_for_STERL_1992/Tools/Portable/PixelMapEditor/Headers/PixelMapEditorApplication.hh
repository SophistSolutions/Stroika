/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PixelMapEditor__
#define	__PixelMapEditor__

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditorApplication.hh,v 1.2 1992/09/01 17:36:41 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: PixelMapEditorApplication.hh,v $
 *		Revision 1.2  1992/09/01  17:36:41  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */

#include	"Application.hh"
#include	"DebugMenu.hh"
#include	"MenuBar.hh"

#include	"PixelMapEditorMenus.hh"



class	PixelMapEditorApplication : public Application {
	public:
		PixelMapEditorApplication (int argc, const char* argv []);
		~PixelMapEditorApplication ();

		static	PixelMapEditorApplication&	Get ();

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

	private:
		PixelMapEditorCommandNamesBuilder	fNamesBuilder;
		MenuBar								fMenuBar;
		SystemMenu							fSystemMenu;
		PixelMapEditorFileMenu				fFileMenu;
		PixelMapEditorEditMenu				fEditMenu;
#if		qDebug
		DebugMenu							fDebugMenu;
#endif

		static	PixelMapEditorApplication*	sThe;
};




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/*__PixelMapEditor__*/

