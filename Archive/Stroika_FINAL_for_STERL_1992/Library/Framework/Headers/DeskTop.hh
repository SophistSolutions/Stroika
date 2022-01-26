/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__DeskTop__
#define	__DeskTop__

/*
 * $Header: /fuji/lewis/RCS/DeskTop.hh,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		The frontmost WindowPlanes are stored first in the list of WindowPlanes.
 *
 * TODO:
 *
 * Changes:
 *	$Log: DeskTop.hh,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.2  1992/07/02  03:48:07  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.11  1992/03/09  23:45:49  lewis
 *		Use new HandleKeyStroke () interface.
 *
 *		Revision 1.10  1992/01/28  22:16:32  lewis
 *		Added Set method to be able to do a user-defined subclass of Desktop. Also, add DesktopEventHandler to Desktop.
 *
 *		Revision 1.9  1992/01/19  21:23:22  lewis
 *		Added AlertPlane.
 *
 *		Revision 1.8  1992/01/18  09:10:46  lewis
 *		Added backpallete - mostly as temp hack for main app shell window.
 *
 *		Revision 1.7  1992/01/15  09:59:20  lewis
 *		Support for new class Enclosure, and reorganization with Panels.
 *
 *
 */

#include	"KeyBoard.hh"
#include	"WindowPlane.hh"


class	DeskTop : public Panel, public MenuCommandHandler {
	public:
		DeskTop ();
		~DeskTop ();


		/*
		 * Usually you just call get, and the first call will automatically build the DeskTop object,
		 * but if you want to override behavior of the DeskTop, you can call set (Nil) to delete the DeskTop,
		 * and then build your own DeskTop (eg say DeskTop::Set (new MyDeskTop ()); does the whole job).
		 * DeskTop::Set() deletes the existing one, regardless of who created it, and replaces it with the given
		 * one which better be a new clipboard object.
		 *
		 * The only catch, is that you must me careful that you do this before there have been any windows created.
		 * (NOT SURE THIS IS EASY - MAKING THIS OVERRIDEBALE MAY REQUIRE MORE WORK!!!)
		 *
		 * NB: Funny name SetThe to avoid conflict with MACRO Set() in foundation!!!
		 */
		static	DeskTop&	Get ();
		static	void		SetThe (DeskTop* newDeskTop);

		/*
		 * Return the bounds of the desktop.  NB. this is not the same as the bounds of
		 * the desktops tablet since this excudes the menubar.
		 */
		nonvirtual	Region	GetBounds () const;

		override	Tablet*	GetTablet () const;
		override	Panel*	GetParentPanel () const;

		override	void		DoSetupMenus ();
		override	Boolean		DoCommand (const CommandSelection& selection);
		override	Boolean		DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
												  KeyComposeState& composeState);


		/*
		 * This routine is called by the event loop when no higher priority events
		 * are available, and the old set of active windows is compared with the set that
		 * should be active, and windows are activated, and de-activated as appropriate.
		 * This is done this way so as to get minimal activate/deactivate paired events, ala
		 * the way the MacToolbox implements it.
		 */
		nonvirtual	void	HandleActivation ();

		virtual		void	AddWindowPlane (WindowPlane* w);
		virtual		void	RemoveWindowPlane (WindowPlane* w);
		virtual		void	ReorderWindowPlane (WindowPlane* w, CollectionSize index = 1);
		nonvirtual	void	ReorderWindowPlane (WindowPlane* w, WindowPlane* behindPlane);
		
		nonvirtual	SequenceIterator(WPlnPtr)*	MakePlaneIterator (SequenceDirection d = eSequenceForward) const;
		
		nonvirtual	WindowPlane*	GetFrontWindowPlane () const;
		nonvirtual	Window*	GetFrontWindow () const;
		virtual		Boolean			GetModal () const;
		
		virtual		Window*	FindWindow (const Point& where)	const;

		virtual		void	SynchronizeOSWindows ();
		
		nonvirtual	WindowPlane*	GetAlertPlane () const;	
		nonvirtual	WindowPlane*	GetDialogPlane () const;	
		nonvirtual	WindowPlane*	GetBackPalettePlane () const;	
		nonvirtual	WindowPlane*	GetPalettePlane () const;	
		nonvirtual	WindowPlane*	GetStandardPlane () const;	


		// hack so we can avoid doing right gdevice support and still
		// get bounds rect for zooming windows and placing dialogs.
		nonvirtual	Rect	GetMainDeviceDeskBounds () const;

	private:
		Tablet*								fDeskTopTablet;
		Boolean								fActivationStatusMayHaveChanged;
		Sequence(WPlnPtr)					fWindowPlanes;
		WindowPlane*						fAlertPlane;
		WindowPlane*						fDialogPlane;
		WindowPlane*						fBackPalettePlane;
		WindowPlane*						fPalettePlane;
		WindowPlane*						fStandardPlane;
		static	DeskTop*					sThe;
		class	DeskTopEventHandler*		fEventHandler;
};



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	WindowPlane*	DeskTop::GetAlertPlane () const			{	return (fAlertPlane);			}
inline	WindowPlane*	DeskTop::GetDialogPlane () const		{	return (fDialogPlane);			}
inline	WindowPlane*	DeskTop::GetBackPalettePlane () const	{	return (fBackPalettePlane);		}
inline	WindowPlane*	DeskTop::GetPalettePlane () const		{	return (fPalettePlane);			}
inline	WindowPlane*	DeskTop::GetStandardPlane () const		{	return (fStandardPlane);		}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***



#endif	/*__DeskTop__*/

