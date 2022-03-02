/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__OptionMenuItem__
#define	__OptionMenuItem__

/*
 * $Header: /fuji/lewis/RCS/PopUpItem.hh,v 1.4 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PopUpItem.hh,v $
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.13  92/04/30  14:14:55  14:14:55  sterling (Sterling Wight)
 *		use OptionMenu
 *		
 *		Revision 1.8  1992/03/06  21:53:47  sterling
 *		motif
 *
 *
 *
 *
 */

#include	"ViewItem.hh"



#if		!qRealTemplatesAvailable
// copied from Stroika-Foundation-Globals.hh - see comment there for explanation...
	#include	"Collection.hh"
	#include	"Sequence.hh"
	#include	"Set.hh"
	
	#ifndef		_ContainersOfStringDeclared_
		#define		_ContainersOfStringDeclared_
		Declare (Iterator, String);
		Declare (Collection, String);
		Declare (AbSequence, String);
		Declare (AbSet, String);
		Declare (Array, String);
		Declare (Sequence_Array, String);
		Declare (Sequence, String);
		Declare (Set_Array, String);
		Declare (Set, String);
	#endif		/*_ContainersOfStringDeclared_*/
#endif		/*!qRealTemplatesAvailable*/





class	OptionMenuButton;
class	OptionMenuItem : public ViewItem {
	public:
		OptionMenuItem (ItemType& type);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		
		nonvirtual	OptionMenuButton&	GetOptionMenu () const;
	
		override	String	GetHeaderFileName ();

	protected:
		override	void	SetOwnedEnabled (Boolean enabled, UpdateMode updateMode);
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
	
	private:
		OptionMenuButton*	fOptionMenu;
};

class	OptionMenuItemType : public ItemType {
	public:
		OptionMenuItemType ();
		
		static	OptionMenuItemType&	Get ();
		
	private:		
		static	ViewItem*	OptionMenuItemBuilder ();
		static	OptionMenuItemType*	sThis;
};

class	SetOptionMenuInfoCommand : public Command {
	public:
		SetOptionMenuInfoCommand (OptionMenuItem& item, class PopUpInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		OptionMenuItem&	fItem;
		class Command*	fItemInfoCommand;
	
		String	fNewLabel;
		String	fOldLabel;
		Sequence(String)	fNewEntries;
		Sequence(String)	fOldEntries;
};

#endif	/* __OptionMenuItem__ */
