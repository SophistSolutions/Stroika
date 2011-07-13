/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Menu__
#define	__Menu__

/*
 * $Header: /fuji/lewis/RCS/Menu.hh,v 1.9 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		Menu 				-- Sequence of MenuItems
 *
 * Notes:
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: Menu.hh,v $
 *		Revision 1.9  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.8  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.7  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.6  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.5  1992/07/14  19:47:29  lewis
 *		Renamed AddSeperatorMenuItem->AddSeparatorMenuItem.
 *
 *		Revision 1.4  1992/07/02  04:27:09  lewis
 *		Renamed Sequence_DoublyLLOfPointers -> SequencePtr.
 *
 *		Revision 1.3  1992/07/01  04:05:14  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.2  1992/06/25  05:42:44  sterling
 *		ButtonMenuItems own Toggle instead of Button.
 *		Renamed CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.44  92/05/18  15:51:26  15:51:26  lewis (Lewis Pringle)
 *		On mac, use osMenu instead of osMenuInfo.
 *		
 *		Revision 1.43  92/04/15  13:54:57  13:54:57  lewis (Lewis Pringle)
 *		Added overloaded AddCheckBoxMenuItem () to take string as second param (like StringMenuItem case).
 *		
 *		Revision 1.42  92/04/07  10:13:14  10:13:14  lewis (Lewis Pringle)
 *		Made makeMenuItemIterator inline.
 *		Added hashtable to map command numbers to menu items rapidly.
 *		
 *		Revision 1.41  92/03/26  09:36:15  09:36:15  lewis (Lewis Pringle)
 *		Simplified args to EffectiveLive/Visibility/Font changed - no old value, just the new value passed.
 *		
 *		Revision 1.40  1992/03/24  02:28:50  lewis
 *		Made CalcDefaultSize () const.
 *
 *		Revision 1.39  1992/03/22  16:58:28  lewis
 *		Made motif GetOSRepresenation () const, and made both mac and unix ones inline.
 *
 *		Revision 1.38  1992/03/19  16:47:34  lewis
 *		Get rid of seperate BuildWidget () function for Menu. Expand inline in Realize.
 *
 *		Revision 1.37  1992/03/06  02:30:42  lewis
 *		Had to add DeleteMenuItem(), DeleteAllMenuItems () in reaction to sterls change to View - not
 *		deleting subviews - probably for the best, but impact of change needs more thought with respect
 *		to menus (and related classes).
 *
 *		Revision 1.36  1992/03/02  22:47:48  lewis
 *		Moved SystemMenu to StandardMenus.hh
 *
 *		Revision 1.35  1992/02/28  22:22:35  lewis
 *		Get rid of override of GetParentPanel ().
 *
 *		Revision 1.34  1992/02/27  21:13:31  lewis
 *		Get rid of virtual method for unknown message on the mac since cannot
 *		reliably access its parameters (left as uninitialized pointers!). So
 *		just pass on directly in MDEF to text mdef.
 *		Got rid of PrepareMenuViewToDraw and related data structures - do more
 *		minimal hacks directly in mdef.
 *		Changed args to ChooseItem.
 *		Added EffectiveVisibilityChanged and hook methods Armed and Disamred ()
 *		called from that changed proc. (not really right yet - but a start).
 *		Override GetEffetiveLive () since dosnt do right thing by default.
 *		(above work on mac - now to test under unix).
 *
 *		Revision 1.33  1992/02/15  06:44:27  sterling
 *		eliminated fOldActive
 *
 *		Revision 1.32  1992/02/14  02:55:14  lewis
 *		Gradual further cleanups, including getting rid of methods that are now
 *		redundent with the view interface (now that we are a view), and made
 *		stuff that was mac-only (callbacks from MDEF) done sufficiently portably that
 *		we should be able to use that interface on Xt (motif).
 *
 *		Revision 1.31  1992/02/12  06:35:03  lewis
 *		Got everything down to one class, and renamed it Menu. Get rid of various
 *		#defines as aliases for Menu. Got working on mac, about as well (no known troubles but a memory leak) as it ever did.
 *		Must cleanup code and make more portbale, and make PalletMenu be virtual subclass, and same with TearOfMenu so we can mix them together.
 *
 *		Revision 1.30  1992/02/11  00:55:01  lewis
 *		Moved toward just 1 menu class - not false distinction between UI's.
 *
 *		Revision 1.29  1992/02/06  20:50:22  lewis
 *		Ported to mac.
 *
 *		Revision 1.28  1992/02/05  07:43:05  lewis
 *		Got rid of Menu_MotifUI_Custom, and _Standard, and moved AddButtonItem to AbstractMenu (soon to be
 *		renamed Menu). Also, got rid of the popup field in Menu_MotifUI - instead peek at type of
 *		parent widget. Soon can get rid of class entirely, and have just 1 menu class for motif - a bit more
 *		work to do that for the mac.
 *
 *		Revision 1.27  1992/02/04  22:45:20  lewis
 *		Cleaning up Menu realization/unrealization stuff for motif, to be more View compatable.
 *
 *		Revision 1.26  1992/02/04  16:57:07  lewis
 *		Get rid of Menu_MacUI_Standard since trying to pair this file down, and it did nothing. Added
 *		CanUseNativeMDEF() procedure to AbstractMenu for MacToolkit so we can tell later if we can use the
 *		native mdef or not.
 *
 *		Revision 1.25  1992/02/04  06:00:38  lewis
 *		Made abstractMenu be a view.
 *
 *		Revision 1.24  1992/02/03  23:32:24  lewis
 *		Have AddXXXMenuItem calls return abstract class pointers.
 *
 *		Revision 1.23  1992/02/03  22:24:31  lewis
 *		General cleanups.
 *
 *		Revision 1.21  1992/01/23  20:03:03  sterling
 *		support for PopUp menus under Motif
 *
 *		Revision 1.17  1991/12/27  18:50:53  lewis
 *		sterl merge.
 *
 *
 *
 */

#include	"HashTable.hh"
#include	"Sequence.hh"
#include	"String.hh"

#include	"Point.hh"
#include	"Rect.hh"

#include	"Button.hh"
#include	"Command.hh"
#include	"MenuItem.hh"


#if 	qMacToolkit
struct	osMenu;
struct	osRect;
#endif



#if		!qRealTemplatesAvailable
Declare (HashTable, MenuItemPtr);
#endif

class	AbstractStringMenuItem;
class	AbstractCheckBoxMenuItem;
class	AbstractCascadeMenuItem;
class	AbstractButtonMenuItem;
class	Menu : public ButtonController, public View {
	public:
		Menu ();
		virtual	~Menu ();

	/*
	 * Miscelaneous utility routines to make, and add specific kinds of menu items to this menu. Since these
	 * are all essentially trivial one-liners, I'm not too sure it makes sense to even have the accessors, since
	 * they convey a sense that there is really more going on (?).
	 */
	public:
		nonvirtual	AbstractStringMenuItem*		AddStringMenuItem (CommandNumber commandNumber);
		nonvirtual	AbstractStringMenuItem*		AddStringMenuItem (CommandNumber commandNumber, Boolean extendedName);
		nonvirtual	AbstractStringMenuItem*		AddStringMenuItem (CommandNumber commandNumber, const String& title);
		nonvirtual	AbstractCheckBoxMenuItem*	AddCheckBoxMenuItem (CommandNumber commandNumber);
		nonvirtual	AbstractCheckBoxMenuItem*	AddCheckBoxMenuItem (CommandNumber commandNumber, const String& title);
		nonvirtual	AbstractCascadeMenuItem*	AddCascadeMenuItem (Menu* cascadeMenu, const String& title);
		nonvirtual	void						AddSeparatorMenuItem ();
		nonvirtual	AbstractButtonMenuItem*		AddButtonMenuItem (Toggle* button, CommandNumber commandNumber);




	/*
	 * Controlled access to list of menuitems.
	 */
	public:
		nonvirtual	void	AddMenuItem (MenuItem* menuItem, CollectionSize index = kBadSequenceIndex);
		nonvirtual	void	AddMenuItem (MenuItem* menuItem, MenuItem* neighbor, AddMode addMode = eAppend);		

		nonvirtual	void	RemoveMenuItem (MenuItem* menuItem);		
		nonvirtual	void	RemoveAllMenuItems ();		
		nonvirtual	void	ReorderMenuItem (MenuItem* menuItem, CollectionSize index);
		nonvirtual	void	ReorderMenuItem (MenuItem* menuItem, MenuItem* neighbor, AddMode addMode = eAppend);

		nonvirtual	CollectionSize	GetLength () const;
		nonvirtual	CollectionSize	GetMenuItemIndex (MenuItem* menuItem);
		nonvirtual	MenuItem*		GetMenuItemByIndex (CollectionSize index);
		nonvirtual	MenuItem*		GetMenuItemByCommand (CommandNumber commandNumber);

		nonvirtual	SequenceIterator(MenuItemPtr)*	MakeMenuItemIterator (SequenceDirection d = eSequenceForward) const;


// not sure this is good idea, but convient since we no longer have views deleting their
// subviews...
// Probably not a good idea...
	public:
		nonvirtual	void	DeleteMenuItem (MenuItem* menuItem);
		nonvirtual	void	DeleteAllMenuItems ();
		

	/*
	 * Definition: a menu is enabled iff an owned MenuItem is enabled. (Defn? or by default thats what we do?)
	 */
	public:
		virtual	Boolean	GetEnabled () const;



	/*
	 * These are all convienience routines that simply apply the given function/change to all MenuItems in this menu with
	 * the given command number. There can be any number of matches (including zero) and they all are updated.
	 */
	public:
		nonvirtual	void	SetEnabled (CommandNumber commandNumber, Boolean enabled);
		nonvirtual	void	SetName (CommandNumber commandNumber, const String& name);
		nonvirtual	void	SetOn (CommandNumber commandNumber, Boolean checked);
#if		qSupportMneumonics		
		nonvirtual	void	SetMneumonic (CommandNumber commandNumber, const String& mneumonic);
#endif		



	/*
	 * Override from panel - when armed - our visible area extends outside of our parent.
	 * (Need to add more explicit notion of arm???)
	 */
	public:
		override	Region	GetVisibleArea () const;


	/*
	 * This decides if we are armed or not (often at least???) Simply calls Armed (), Disarmed ().
	 */
	protected:
		override	void	EffectiveVisibilityChanged (Boolean newVisible, UpdateMode updateMode);

		virtual		void	Armed ();			// hook procedures called on arm/disarm (NYI)
		virtual		void	Disarmed ();



// live stuff??
	protected:
		override	Boolean	GetEffectiveLive () const;



	/*
	 * used in performing update - sets all MenuItems to "clean slate".
	 * in EndUpdate, checks to see if any menuitem characteristics have
	 * changed.
	 */
	public:
		virtual	void	BeginUpdate ();		// resets all items
		virtual	Boolean	EndUpdate ();		// returns True if any MenuItems changed


	/*
	 * Access to toolkit represntation allowed, but discouraged. Your milage may vary :-).
	 */
	public:
#if		qMacToolkit
		nonvirtual	osMenu**	GetOSRepresentation () const;
#elif	qXtToolkit
		nonvirtual	osWidget*	GetOSRepresentation () const;
#endif


	/*
	 * Override these hooks to "notice" changes in the list of menu items.
	 * (Dont forget to call inherited!)
	 */
	protected:
		virtual	void	AddMenuItem_ (MenuItem* menuItem, CollectionSize index);
		virtual	void	RemoveMenuItem_ (MenuItem* menuItem);
		virtual	void	ReorderMenuItem_ (MenuItem* menuItem, CollectionSize index);





	/*
	 * ChooseItem is called when selecting and deselecting items. It is passed a position (in local
	 * coordinates) and a starting selected menu number going in. You should return the new value.
	 *
	 * This can be overriden as a hook, or to change the hilighting behaviour of the menu. Note that
	 * it gets called often during tracking in the menu, and not just when an item is chosen (finally selected)!
	 */
	protected:
		virtual	CollectionSize	ChooseItem (const Point& hitPt, CollectionSize whichItem);


	public:
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:	
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();
		virtual		void	SelectItem (MenuItem* item, UpdateMode updateMode);		// not sure I like this routine

	private:
		MenuItem*	fCurrentlySelected;




#if		qMacToolkit
	/*
	 * On the mac, you can achieve greater compatability with future system realeases, and system patches (aka inits)
	 * by using the native menu definition procs. However, that scheme is not general enuf for Stroika to place
	 * arbitrary buttons into menu items. So we must know if ALL of your items are native. If they are, we will use
	 * a native MDEF (by default). You can override this and force either the native mdef or the Stroika MDEF by
	 * overriding this routine, but do so at your own peril.
	 *
	 * By default, this routine simply returns True iff each subitem is native.
	 */
	protected:
		virtual	Boolean		CanUseNativeMDEF ();
#endif


	private:
#if		qRealTemplatesAvailable
		Sequence<MenuItem*>		fItems;
		HashTable<MenuItem*>					fItemsHashTable;	// for rapid lookup by CommandNumber
#else	/*qRealTemplatesAvailable*/
		Sequence(MenuItemPtr)	fItems;
		HashTable(MenuItemPtr)					fItemsHashTable;	// for rapid lookup by CommandNumber
#endif	/*qRealTemplatesAvailable*/
		Boolean									fOldEnabled;


#if		qMacToolkit
	private:
		static	short	sLastMenuID;
		osMenu**		fOSMenu;
		char**			fSavedDefProc;
		char**			fRealDefProc;		// native one we built originally...
#endif


#if		qXtToolkit
	public:
		override	void		Realize (osWidget* parent);
		override	void		UnRealize ();
		override	osWidget*	GetWidget () const;
	private:
		osWidget*	fWidget;
#endif


// stuff to support mac custom menus...
// redo so we do magic on THIS GUY, IF NEED BE, and not on the menu items themselves...
#if		qMacToolkit
	protected:
		virtual	void	PlacePopup (const Point& hitPt, CollectionSize whichItem, Rect* popupRectangle, CollectionSize* topOfMenu);
		virtual	void	ProcessMessage (short theMessage, osMenu** theOSMenu, osRect* menuRect, long hitPt, short* whichItem);

	private:
		// stuff to support MDEF callback procs...
		struct	_PrivateMDEFRecord**	fMDEFRecord;
		static	pascal	void	MDEFProc (short theMessage, osMenu** theOSMenu, osRect* menuRect, long hitPt, short* whichItem);

		nonvirtual	void	PatchMenuProc ();
#endif
};










/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	SequenceIterator(MenuItemPtr)*	Menu::MakeMenuItemIterator (SequenceDirection d) const
	{
		return (fItems.MakeSequenceIterator (d));
	}
#if		qMacToolkit
inline	osMenu**		Menu::GetOSRepresentation () const	{	return (fOSMenu);	}
#elif	qXtToolkit
inline	osWidget*		Menu::GetOSRepresentation () const	{	RequireNotNil (fWidget); return (fWidget);	}
#endif	/*Toolkit*/



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Menu__*/

