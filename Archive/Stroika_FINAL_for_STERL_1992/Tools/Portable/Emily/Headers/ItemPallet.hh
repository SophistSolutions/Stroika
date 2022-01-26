/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ItemPallet__
#define	__ItemPallet__

/*
 * $Header: /fuji/lewis/RCS/ItemPallet.hh,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ItemPallet.hh,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.14  1992/03/06  21:53:47  sterling
 *		motif
 *
 *		Revision 1.10  1992/02/15  07:53:18  lewis
 *		Changed StandardMenu to Menu (to reflect Stroika class changes).
 *
 *		Revision 1.7  1992/01/31  18:24:50  sterling
 *		Bootstrapped
 *
 *
 */

#include	"Sequence_Array.hh"

#include	"CommandNumbers.hh"
#include	"Menu.hh"

#define		qUseCustomMenu		1

#if 	qUseCustomMenu
#include	"TearOffMenu.hh"
#endif



#if		!qRealTemplatesAvailable
	Declare (Iterator, CommandNumber);
	Declare (Collection, CommandNumber);
	Declare (AbSequence, CommandNumber);
	Declare (Array, CommandNumber);
	Declare (Sequence_Array, CommandNumber);
	Declare (Sequence, CommandNumber);
#endif


#if qUseCustomMenu	
	#if		!qRealTemplatesAvailable
		typedef	class ItemPalletButton*	IPBPtr;
		#define	ItemPalletButtonPtr	IPBPtr
		
		Declare (Iterator, IPBPtr);
		Declare (Collection, IPBPtr);
		Declare (AbSequence, IPBPtr);
		Declare (Array, IPBPtr);
		Declare (Sequence_Array, IPBPtr);
		Declare (Sequence, IPBPtr);
	#endif
#endif /* qUseCustomMenu */		



#if 	qUseCustomMenu
class ItemPallet : public TearOffMenu {
#else
class ItemPallet : public Menu {
#endif

	public:		
		ItemPallet (Window* w = Nil);
		~ItemPallet ();
		
		static	CommandNumber	GetPalletSelection ();
		static	void			SetPalletSelection (CommandNumber selection = eArrow, UpdateMode update = View::eDelayedUpdate);

		static	Boolean	GetEditMode ();
		static	void	SetEditMode (Boolean newEditMode);
		
		static	void	AddPalletItem (CommandNumber command, const String& itemName);
	
		static	Boolean	ShouldEnable (CommandNumber command);
		
		static	String	GetSelectedString ();
		
		static	MenuItem*	GetSelectedItem () const;
		static	void		SetSelectedItem (MenuItem* view, UpdateMode updateMode = eDelayedUpdate);

		static	ItemPallet& Get ();

		override	Boolean	EndUpdate ();
		
	protected:
		override	void	SelectItem (MenuItem* item, Panel::UpdateMode updateMode);

#if qUseCustomMenu
		override	void	Layout ();
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	Menu*	BuildTornOffView ();
#else
		nonvirtual	void	BuildPalletItem (CommandNumber command, const String& name);
#endif
		nonvirtual	void	BuildPalletItems ();
		nonvirtual	void	AddPalletSubItem (CommandNumber command, const String& itemName);

	private:
		static	ItemPallet*						sThis;
		static	Sequence_Array(String)			sStrings;
		static	Sequence_Array(CommandNumber)	sCommands;
		static	MenuItem*						sSelectedItem;

#if qUseCustomMenu		
		Sequence(ItemPalletButtonPtr)	fItems;
#endif
};

#endif	/* __ItemPallet__ */
