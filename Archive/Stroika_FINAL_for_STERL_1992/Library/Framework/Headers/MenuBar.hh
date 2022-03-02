/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__MenuBar__
#define	__MenuBar__

/*
 * $Header: /fuji/lewis/RCS/MenuBar.hh,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		AbstractMenuBar is a MenuOwner, and a View, that managers a Sequence of MenuTitles,
 * which themselves own Menus, which in turn own MenuItems. AbstractMenuBar is an abstract
 * class, and is the type that should be used to hold references to MenuBars if possible
 * since that will help to ensure portability.
 *
 *		MenuBar_MacUI_Native is the Macintosh implementation of an AbstractMenuBar. It
 * should rarely be referenced at all.
 *
 *		MenuBar_MotifUI is the Motif implementation of an AbstractMenuBar. It
 * should rarely be referenced at all.
 *
 *		MenuBar is the concrete class for menubars that should ususally be instantiated.
 * It manages to build (thru delagation, or inheritance) the appropriate kind of menubar
 * for the current GUI/Toolkit, etc.
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: MenuBar.hh,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/02  04:31:10  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/06/25  05:45:53  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.16  92/04/24  08:52:55  08:52:55  lewis (Lewis Pringle)
 *		Override trackpress, and add help menu support for motif (tried but didnt seem to work - investigate).
 *		
 *		Revision 1.15  92/04/14  07:11:32  07:11:32  lewis (Lewis Pringle)
 *		Override GetWidget for Motif MenuBar to return widget of owned oscontrol.
 *		
 *		Revision 1.14  92/03/24  02:28:50  02:28:50  lewis (Lewis Pringle)
 *		Made CalcDefaultSize () const.
 *		
 *		Revision 1.13  1992/03/19  16:50:49  lewis
 *		Got rid of MenuBar_MacUI_Native::Draw (const Region& update) - just a hack and not needed - instead do
 *		draws in MenuTitles.
 *
 *		Revision 1.12  1992/03/16  17:13:06  lewis
 *		Added dtor for MacUI menubar, and removed GetWidget () override for motif menubar.
 *
 *		Revision 1.11  1992/03/13  16:30:30  lewis
 *		For MenuBar_MotifUI get rid of fWidget, and use fOSControl instead. Still need
 *		GetWidget () method - but thats a bug - must think about more....
 *
 *		Revision 1.10  1992/03/09  23:52:33  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.9  1992/03/06  02:32:13  lewis
 *		Had to add DeleteMenuTitle () in reaction to sterls change to View - not deleting subviews -
 *		probably for the best, but impact of change needs more thought with respect to menubars (and related classes).
 *
 *		Revision 1.8  1992/03/05  22:56:42  lewis
 *		Added CalcDefaultSize () overrides for mac and motif mbars.
 *
 *		Revision 1.7  1992/03/05  17:29:55  lewis
 *		Add layout, draw, and a notion of being REALLY installed in the menubar (just a prototype
 *		design - needs work). Killed MenuBar_MacUI_Portable - it didnt work, and needed a lot
 *		more thought.
 *
 *		Commented the purpose of the various classes in the Description part of the headers.
 *
 *		Revision 1.6  1992/02/29  15:52:37  lewis
 *		Had to get rid of override of dtor and call to Assert (GetLength () == 0) since that indirectly
 *		called GetMenus () - a pure virtual function.
 *
 *		Revision 1.5  1992/02/28  22:13:57  lewis
 *		Add dtor for AbstractMenuBar () when debug on to assert all menus removed.
 *		start making some things const.
 *
 *		Revision 1.3  1992/02/27  21:20:18  lewis
 *		AbstractMenuBar now a view so dont mix in later in various subclasses.
 *		Override GetParentPanel () const in MenuBar_MacUI_Native to say its in the desktop.
 *		Use menutitle instead of MenuTitle_Mac_NativePtr since thats now gone.
 *		Fix sequence usage - const method should return const sequence, and should return Sequence() not Seqeunce_DLL...()
 *
 *		Revision 1.2  1992/02/15  06:14:08  lewis
 *		Do class MenuBar instead of #defines, and other cleanups.
 *
 *		Revision 1.1  1992/02/15  00:41:19  lewis
 *		Initial revision
 *
 *
 */

#include	"MenuOwner.hh"
#include	"MenuTitle.hh"
#include	"PullDownMenu.hh"
#include	"View.hh"






class	AbstractMenuBar : public MenuOwner, public View {
	protected:
		AbstractMenuBar ();

	public:
		nonvirtual	void	AddMenu (Menu* menu, const String& title, CollectionSize index = eAppend);
		nonvirtual	void	RemoveMenu (Menu* menu);
		nonvirtual	void	ReorderMenu (Menu* menu, CollectionSize index);

		nonvirtual	void	Redraw ();

	protected:
		nonvirtual	void	AddMenuTitle (MenuTitle* menuTitle, CollectionSize index);
		nonvirtual	void	RemoveMenuTitle (MenuTitle* menuTitle);
		nonvirtual	void	DeleteMenuTitle (MenuTitle* menuTitle);

		virtual		MenuTitle*	BuildMenuTitle (Menu* menu, const String& title) = Nil;

		virtual		void	AddMenuTitle_ (MenuTitle* menuTitle, CollectionSize index);
		virtual		void	RemoveMenuTitle_ (MenuTitle* menuTitle);

		virtual		void		Redraw_ () = Nil;

		override	CollectionSize	GetLength_ () const;
		override	CollectionSize	GetMenuIndex_ (Menu* menuItem);
		override	Menu*			GetMenu_ (CollectionSize index) const;
		
		override	MenuItem*	GetMenuItem_ (CommandNumber commandNumber) const;

		override	void	SetEnabled_ (CommandNumber commandNumber, Boolean enabled);

#if		qSupportMneumonics		
		override	void	SetMneumonic_ (CommandNumber commandNumber, const String& mneumonic);
#endif		

		override	void	SetName_ (CommandNumber commandNumber, const String& name);
		override	void	SetOn_ (CommandNumber commandNumber, Boolean checked);

		override	void	BeginUpdate_ ();
		override	Boolean	EndUpdate_ ();

		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected) = Nil;
		override	Boolean	DoMenuSelection_ (Character keyPressed, MenuItem*& selected);

		virtual	const	Sequence(MenuTitlePtr)&	GetMenus () const = Nil;
};





#if		qMacToolkit
class	MenuBar_MacUI_Native : public AbstractMenuBar {
	public:
		MenuBar_MacUI_Native ();
		~MenuBar_MacUI_Native ();


// Start attempt at supporting multiple menu bars, and cooperation as to the installed one...
void	BeREALLYInstalledInSystemMBar ();
static	MenuBar_MacUI_Native*	GetREALLYInstalledInSystemMBar ();


	public:
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected);
		override	void	HandleMenuSelection (const MenuItem& item);

		override	const	Sequence(MenuTitlePtr)&	GetMenus () const;

		override	void		Redraw_ ();
		override	void		Layout ();

		override	MenuTitle*	BuildMenuTitle (Menu* menu, const String& title);
		override	void		AddMenuTitle_ (MenuTitle* menuTitle, CollectionSize index);
		override	void		RemoveMenuTitle_ (MenuTitle* menuTitle);

		override	Panel*		GetParentPanel () const;

	private:
		char**	fOSMenuBar;		// belive it or not, thats what the mac's represenation is :-)
		Sequence(MenuTitlePtr)	fMenus;

	public:
		static	Sequence(MenuTitlePtr)	sCascadeMenus;
};



#elif   qXmToolkit



class	MenuBar_MotifUI : public AbstractMenuBar {
	public:
		MenuBar_MotifUI ();
		~MenuBar_MotifUI ();

		override 	void		Realize (osWidget* parent);
		override 	void		UnRealize ();
		override	osWidget*	GetWidget () const;

	public:
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);


		// the the help menu to the given menu, wich must already be instelled in us.
		// Set to nil, before removing that menu.
		nonvirtual	void	SetHelpMenu (Menu* whichMenu);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected);

		override	const	Sequence(MenuTitlePtr)&	GetMenus () const;
		override	void							Redraw_ ();

		override	MenuTitle*	BuildMenuTitle (Menu* menu, const String& title);
		override	void		AddMenuTitle_ (MenuTitle* menuTitle, CollectionSize index);
		override	void		RemoveMenuTitle_ (MenuTitle* menuTitle);

	protected:
		override	void		Layout ();

	private:
		Sequence(MenuTitlePtr)	fMenus;
		Menu*									fHelpMenu;

		class	MotifOSControl;
		MotifOSControl*	fOSControl;
};


#endif	/*Toolkit*/





class	MenuBar : public
#if		qMacToolkit
	MenuBar_MacUI_Native
#elif	qXmToolkit
	MenuBar_MotifUI
#endif
{
	public:
		MenuBar ();
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__MenuBar__*/

