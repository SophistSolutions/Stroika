/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__MenuOwner__
#define	__MenuOwner__

/*
 * $Header: /fuji/lewis/RCS/MenuOwner.hh,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		MenuOwners are objects which "own" MenuItems (not Menus). They don't necessarily directly own them, but
 * have some interface to get at that list (we probably should have a makeMenuItemIterator() method then!!).
 * 
 *
 * TODO:
 *			-		Spell qSupportMneumonics pro
 *			-		Cleanup OS vs GDI vs Toolkit stuff...
 *
 * Notes:
 *
 * Changes:
 *	$Log: MenuOwner.hh,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.2  1992/07/02  04:34:41  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.31  92/05/14  15:06:19  15:06:19  lewis (Lewis Pringle)
 *		Use qMPW_APPLE_CPP_BROKEN_FOR_DECLARE2 flag for macro bug workaround.
 *		
 *		Revision 1.30  92/05/11  20:03:54  20:03:54  lewis (Lewis Pringle)
 *		Made GetMenu/GetMenu_ const methods, and made some nontual 1liner wrapper functions inline.
 *		
 *		Revision 1.29  92/04/24  08:47:08  08:47:08  lewis (Lewis Pringle)
 *		Conditionally added template, support, and support for Get/SetMenuItemSelected - keep trakc of static
 *		member variable for communications between menu item widgets and menu.
 *		
 *		Revision 1.28  92/02/29  15:52:37  15:52:37  lewis (Lewis Pringle)
 *		Had to get rid of override of dtor and call to Assert (GetLength () == 0) since that indirectly
 *		called GetMenus () - a pure virtual function.
 *		
 *		Revision 1.27  1992/02/28  22:16:23  lewis
 *		Make const a bunch of methods that sb but weren't.
 *		Add MenuOwner dtor with debug on to assert all submenus destroyed.
 *
 *		Revision 1.25  1992/02/14  23:28:00  lewis
 *		Moved everything but MenuOwner out to seperate files, like MenuBar, PopUpMenu, PullDownMenu, MenuTitle, Accelerator...
 *
 *		Revision 1.23  1992/02/12  07:47:06  lewis
 *		Changed from AbtractMenu to Menu.
 *
 *		Revision 1.21  1992/02/11  00:57:49  lewis
 *		Move toward 1 kind of menu.
 *
 *		Revision 1.20  1992/02/03  22:26:05  lewis
 *		General cleanups.
 *
 *		Revision 1.18  1992/01/23  20:03:38  sterling
 *		support for Motif PopUp menus
 *
 *		Revision 1.17  1992/01/23  04:29:29  sterling
 *		made popup use MotifOSControl
 *
 *
 *
 */

#include	"Mapping_HashTable.hh"
#include	"Sequence.hh"

#include	"Point.hh"

#include	"Accelerator.hh"
#include	"Command.hh"



class	Menu;
class	MenuItem;


#if		!qRealTemplatesAvailable
	class MenuOwner;
	typedef	MenuOwner*		MenuOwnerPtr;
	Declare (Iterator, MenuOwnerPtr);
	Declare (Collection, MenuOwnerPtr);
	Declare (AbSequence, MenuOwnerPtr);
	Declare (Array, MenuOwnerPtr);
	Declare (Sequence_Array, MenuOwnerPtr);
	Declare (Sequence, MenuOwnerPtr);

	
	Declare (Iterator, Accelerator);
	Declare (Collection, Accelerator);
	AbMappingDeclare (CommandNumber, Accelerator);
	Mapping_HTDeclare(CommandNumber, Accelerator);
#endif




class	MenuOwner {
	protected:
		MenuOwner ();

	/*
	 * Access to global table of accelerators.
	 */
	public:
		static	const Accelerator	kNoAccelerator;
		static	AbMapping(CommandNumber, Accelerator)&	GetAccelerators ();

	/*
	 * Access owned menus.
	 */
	public:
		nonvirtual	CollectionSize	GetLength () const;
		nonvirtual	CollectionSize	GetMenuIndex (Menu* menuItem);
		nonvirtual	Menu*			GetMenu (CollectionSize index) const;

		/*
		 * Convenience wrappers on Menu calls. In principle we could always insist that you call GetMenu and
		 * then make a method call on the resulting Menu, but this would be inconvenient and inefficient.
		 */
		nonvirtual	MenuItem*	GetMenuItem (CommandNumber commandNumber) const;

		nonvirtual	void	SetEnabled (CommandNumber commandNumber, Boolean enabled);
		
#if		qSupportMneumonics		
		nonvirtual	void	SetMneumonic (CommandNumber commandNumber, const String& mneumonic);
#endif		
		
		nonvirtual	void	SetName (CommandNumber commandNumber, const String& name);
		nonvirtual	void	SetOn (CommandNumber commandNumber, Boolean checked);

		static	void	BeginUpdate ();			// resets all menus
		static	void	EndUpdate ();			// redraws menus if necessary.
		
		/*
		 * Call this to let us know something has happened that may have invalidated the
		 * enabling, or text, or some other part of some menu.  It is called automatically
		 * for most things like commands being done(???), clicks, (and more????).
		 */
		static	Boolean		GetMenusOutOfDate ();
		static	void		SetMenusOutOfDate ();
		
		nonvirtual	void	InstallMenu ();
		nonvirtual	void	DeinstallMenu ();
		
#if		qRealTemplatesAvailable
		static	SequenceIterator<MenuOwner*>*	InstalledMenusIterator (SequenceDirection d = eSequenceForward);
#else
		static	SequenceIterator(MenuOwnerPtr)*	InstalledMenusIterator (SequenceDirection d = eSequenceForward);
#endif


	/*
	 * Support invoking tracking, and selecting of some menu item owned (indirectly) by this menu owner.
	 * Static member version goes thru all installed MenuOwners too see if they respond the the accelerator key.
	 */
	public:
		nonvirtual	Boolean	DoMenuSelection (const Point& startPt, MenuItem*& selected);
		static		Boolean	DoMenuSelection (Character keyPressed, MenuItem*& selected);
#if		qXmToolkit
	private:
		static	MenuItem*	sSelectedMenuItem;							// set from activate callbacks and used internally by DoMenuSelection
	protected:
		static	MenuItem*	GetMenuItemSelected ();
	public:
		static	void		SetMenuItemSelected (MenuItem* menuItem);	// only called by motif menu item activate callbacks...
#endif


	protected:
		virtual	CollectionSize	GetLength_ () const 					= Nil;
		virtual	CollectionSize	GetMenuIndex_ (Menu* menuItem)			= Nil;
		virtual	Menu*			GetMenu_ (CollectionSize index) const	= Nil;

		virtual	MenuItem*	GetMenuItem_ (CommandNumber commandNumber) const		=	Nil;

		virtual	void	SetEnabled_ (CommandNumber commandNumber, Boolean enabled)	=	Nil;

#if		qSupportMneumonics		
		virtual	void	SetMneumonic_ (CommandNumber commandNumber, const String& mneumonic) = Nil;
#endif		

		virtual	void	SetName_ (CommandNumber commandNumber, const String& name)		= Nil;
		virtual	void	SetOn_ (CommandNumber commandNumber, Boolean checked)			= Nil;

		virtual	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected)	= Nil;
		virtual	Boolean	DoMenuSelection_ (Character keyPressed, MenuItem*& selected)	= Nil;
		virtual	void	HandleMenuSelection (const MenuItem& item);

		virtual	void	BeginUpdate_ ()				=	Nil;
		virtual	Boolean	EndUpdate_ ()				=	Nil;

	private:
		static	Boolean	sMenusOutOfDate;
#if		qRealTemplatesAvailable
		static	Sequence<MenuOwner*>		sInstalledMenus;
#else
		static	Sequence(MenuOwnerPtr)	sInstalledMenus;
#endif
};





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	CollectionSize	MenuOwner::GetLength () const								{	return (GetLength_ ());					}
inline	Menu*			MenuOwner::GetMenu (CollectionSize index) const				{	Require (index >= 1); Require (index <= GetLength ()); return (GetMenu_ (index));	}
inline	void			MenuOwner::SetMenusOutOfDate ()								{	sMenusOutOfDate = True;					}
inline	Boolean			MenuOwner::GetMenusOutOfDate ()								{	return (sMenusOutOfDate);				}
inline	MenuItem*		MenuOwner::GetMenuItem (CommandNumber commandNumber) const	{	return (GetMenuItem_ (commandNumber));	}
inline	void			MenuOwner::SetEnabled (CommandNumber commandNumber, Boolean enabled)
	{
		SetEnabled_ (commandNumber, enabled);
	}
		



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__MenuOwner__*/

