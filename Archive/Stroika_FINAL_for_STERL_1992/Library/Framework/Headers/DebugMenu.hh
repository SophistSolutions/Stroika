/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__DebugMenu__
#define	__DebugMenu__

/*
 * $Header: /fuji/lewis/RCS/DebugMenu.hh,v 1.1 1992/06/20 17:27:41 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: DebugMenu.hh,v $
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.11  1992/03/05  17:17:55  lewis
 *		Allow for multiple debug menus, and get rid of base class MenuCommandHandler, and
 *		instead provide static members to provide the menusetup, and execution handling.
 *
 *		Revision 1.10  1992/02/14  02:53:04  lewis
 *		Added printregionstatictics enum.
 *
 *		Revision 1.9  1992/02/12  06:26:48  lewis
 *		Added eToggleValidateHeapStorageOnAllocs, and changed from inheriting
 *		from StandardMenu to Menu (StandardMenu no longer exists).
 *
 *		Revision 1.8  1992/01/10  03:05:03  lewis
 *		Added ToggleSyncMode to DebugMenu under X.
 *
 *
 *
 *
 *
 */

#include	"Debug.hh"

#include	"Command.hh"
#include	"Menu.hh"



#if		qDebug
class	DebugMenu : public Menu  {
	public:
		enum	{
			ePrintMemoryStatistics	=	CommandHandler::eFirstDebugMenuCommand,
			eValidateHeapStorage,
			eToggleValidateHeapStorageOnAllocs,
			eTogglePrintIncomingEvents,
#if		qKeepRegionAllocStatistics
			ePrintRegionStatistics,
#endif
#if		qXGDI 
			eToggleSynchronizedMode,
#endif
		};

		DebugMenu ();
		virtual ~DebugMenu ();

		/*
		 * Handle precisely the commands defined in this menu.
		 */
		static		void		DoSetupsForDebugMenus ();
		static		Boolean		DoCommandDebugMenu (const CommandSelection& selection);


		static	String	DefaultName ();
};
#endif	/*qDebug*/


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__DebugMenu__*/

