/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__MenuTitle__
#define	__MenuTitle__

/*
 * $Header: /fuji/lewis/RCS/MenuTitle.hh,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		A MenuTitle is a Button/MenuOwner whose associated menu is displayed and armed on mousepress.
 *
 *
 * TODO:
 *		Design problem!!! Do we delete the old menu when we say SetMenu, or not. If we do, we must
 *		do the same thing for our destructor. This implies Menus can NEVER be allocated in any way other than
 *		directly with new. (ie not fields or stack variables, or whatever). -lgp feb 14, 1992
 *
 * Notes:
 *
 * Changes:
 *	$Log: MenuTitle.hh,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.2  1992/07/02  04:35:27  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.11  92/05/13  00:00:01  00:00:01  lewis (Lewis Pringle)
 *		Get rid of MenuTitle::Draw () override - if needed in some subclasses, do it there.
 *		
 *		Revision 1.1  92/05/11  20:10:17  20:10:17  lewis (Lewis Pringle)
 *		Initial revision
 *
 *		Revision 1.9  92/03/26  09:36:15  09:36:15  lewis (Lewis Pringle)
 *		Simplified args to EffectiveLive/Visibility/Font changed - no old value, just the new value passed.
 *		
 *		Revision 1.8  1992/03/19  16:52:14  lewis
 *		Added Draw override so for non-native siturations like the mac we at least see something. Needs
 *		work to look right though..
 *
 *		Revision 1.7  1992/03/06  02:33:51  lewis
 *		Added dtor to MenuTitle to call SetMenu (Nil), so it would be removed as a subview - in reaction
 *		to sterls change in View - not deleting subviews. I like the change, but must study its impact
 *		better on MenuTitles, and related classes.
 *
 *		Revision 1.5  1992/02/28  22:17:44  lewis
 *		Made some methods const.
 *
 *		Revision 1.4  1992/02/28  16:25:42  lewis
 *		add include.
 *
 *		Revision 1.3  1992/02/27  21:25:49  lewis
 *		Require menu parameter for MenuTitle (no nil default).
 *		Be an EnableItem, and View (big change). Involves adding boolean, and setenabled/getenabled
 *		/effectivelivechanged overrides,a nd GetLive. Also, disambiguate SetEnabled method in favor
 *		of menuowner version. Moved stuff like MacDoMenuSelection () here from the now defunct MenuTitle_Mac_Native.
 *
 *		Revision 1.1  1992/02/15  00:41:19  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"Sequence.hh"

#include	"EnableItem.hh"
#include	"MenuOwner.hh"
#include	"View.hh"



class	Menu;
class	MenuTitle : public MenuOwner, public EnableItem, public View {
	protected:
		MenuTitle (const String& title, Menu* menu);

	public:
		virtual ~MenuTitle ();

	public:
		nonvirtual	String	GetTitle () const;
		nonvirtual	void	SetTitle (const String& title);

		/*
		 * Get and set the menu that is associated with this menu title. It is the menu that will be (ususally)
		 * poped up, or down, or whatever when a DoMenuSelction is done. The menu parameter can be nil,
		 * but then doing most other operations on the menu is an error.
		 *
		 * Also, NB: the old menu, if it differs from the one being set, will be deleted. Therefore, it is
		 * required that these menus be alloced directly wity new.(NOT SURE THIS IS A GOOD DESIGN - LGP Feb 14, 1992)
		 */
		nonvirtual	Menu*	GetMenu () const;
		nonvirtual	void	SetMenu (Menu* menu);

		override	void	BeginUpdate_ ();
		override	Boolean	EndUpdate_ ();

	protected:
		override	CollectionSize	GetLength_ () const;
		override	CollectionSize	GetMenuIndex_ (Menu* menuItem);
		override	Menu*			GetMenu_ (CollectionSize index) const;

		override	MenuItem*	GetMenuItem_ (CommandNumber commandNumber) const;

		override	void		SetEnabled_ (CommandNumber commandNumber, Boolean enabled);

#if		qSupportMneumonics		
		override	void	SetMneumonic_ (CommandNumber commandNumber, const String& mneumonic);
#endif		

		override	void	SetName_ (CommandNumber commandNumber, const String& name);
		override	void	SetOn_ (CommandNumber commandNumber, Boolean checked);

		virtual		void	SetTitle_ (const String& title);
		virtual		void	SetMenu_ (Menu* menu);

		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected);
		override	Boolean	DoMenuSelection_ (Character keyPressed, MenuItem*& selected);
		override	void	HandleMenuSelection (const MenuItem& item);

#if		qMacToolkit
// taken from MenuTitle_Mac_Native -- not quite sure when / if needed...
		static	Boolean	MacDoMenuSelection (Menu* nativeMenu, const Point& startPt, MenuItem*& selected);
#endif



	/*
	 * Disambiguate call - use MenuOwner version by default (as opposed to EnableItem::SetEnabled()).
	 */
	public:
		nonvirtual	void	SetEnabled (CommandNumber commandNumber, Boolean enabled);



	/*
	 * EnableItem overrides.
	 */
	public:
		override	Boolean	GetLive () const;
	protected:
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);
		override	Boolean	GetEnabled_ () const;
		override	void	SetEnabled_ (Boolean enabled, UpdateMode updateMode);
	private:
		Boolean		fEnabled;


	private:
		Menu*		fMenu;
		String		fTitle;

		friend	class	AbstractMenuBar;	// calls DoMenuSelection_
};



#if		!qRealTemplatesAvailable
	typedef	MenuTitle*		MenuTitlePtr;
	Declare (Iterator, MenuTitlePtr);
	Declare (Collection, MenuTitlePtr);
	Declare (AbSequence, MenuTitlePtr);
	Declare (Array, MenuTitlePtr);
	Declare (Sequence_Array, MenuTitlePtr);
	Declare (Sequence, MenuTitlePtr);
#endif


/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	void	MenuTitle::SetEnabled (CommandNumber commandNumber, Boolean enabled)		{ MenuOwner::SetEnabled (commandNumber, enabled); }

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__MenuTitle__*/

