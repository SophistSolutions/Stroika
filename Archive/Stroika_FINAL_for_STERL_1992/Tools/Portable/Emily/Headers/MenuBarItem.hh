/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__MenuBarItem__
#define	__MenuBarItem__

/*
 * $Header: /fuji/lewis/RCS/MenuBarItem.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: MenuBarItem.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.8  92/05/13  18:38:30  18:38:30  lewis (Lewis Pringle)
 *		STERL.
 *		
 *
 *
 *
 */

#include	"GroupItem.hh"
#include	"Menu.hh"


#if		!qRealTemplatesAvailable
	typedef	class	MenuItemEntry*	MenuItemEntryPtr;
	#define	MIEPtr	MenuItemEntryPtr
	
	Declare (Iterator, MIEPtr);
	Declare (Collection, MIEPtr);
	Declare (AbSequence, MIEPtr);
	Declare (Array, MIEPtr);
	Declare (Sequence_Array, MIEPtr);
	Declare (Sequence, MIEPtr);
#endif

#if		!qRealTemplatesAvailable
	typedef	class	MenuTitleEntry*	MenuTitleEntryPtr;
	#define	MTEPtr	MenuTitleEntryPtr
	
	Declare (Iterator, MTEPtr);
	Declare (Collection, MTEPtr);
	Declare (AbSequence, MTEPtr);
	Declare (Array, MTEPtr);
	Declare (Sequence_Array, MTEPtr);
	Declare (Sequence, MTEPtr);
#endif



class	MenuBarItem : public ViewItem {
	public:
		MenuBarItem (ItemType& type);
		~MenuBarItem ();

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		
		override	String	GetHeaderFileName ();

		override	void	EditModeChanged (Boolean newEditMode);

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteDeclaration (class ostream& to, int tabCount);
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteDestructor (class ostream& to, int tabCount, CommandNumber gui);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
	
	private:
		nonvirtual	String	CalcMenuName (const String& menuTitle);

		class MyMenuBar*							fMenuBar;
		Sequence(MenuTitleEntryPtr)	fMenus;
	
	friend	class	MenuBarInfo;	// peeks at fMenus
};

class	MenuBarItemType : public ItemType {
	public:
		MenuBarItemType ();
		
		static	MenuBarItemType&	Get ();
		
	private:		
		static	ViewItem*	MenuBarItemBuilder ();
		static	MenuBarItemType*	sThis;
};

class MenuTitle;

class	MenuItemEntry : public Saveable {
	public:
		MenuItemEntry ();
		MenuItemEntry (const MenuItemEntry& entry);
	
		nonvirtual	String		GetLabel () const;
		nonvirtual	void		SetLabel (const String& label);
		
		nonvirtual	String		GetAccelerator () const;
		nonvirtual	void		SetAccelerator (const String& accelerator);
		
		nonvirtual	String		GetMneumonic () const;
		nonvirtual	void		SetMneumonic (const String& mneumonic);
		
		nonvirtual	Boolean		GetExtended () const;
		nonvirtual	void		SetExtended (Boolean extendedName);

		nonvirtual	Boolean		GetCheckBox () const;
		nonvirtual	void		SetCheckBox (Boolean checkBox);
		
		nonvirtual	String	GetCommandNumber () const;
		nonvirtual	void	SetCommandNumber (const String& commandNumber);
				
	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;

	private:
		String			fLabel;
		String			fAccelerator;
		String			fMneumonic;
		Boolean			fExtendedName;
		Boolean			fCheckBox;
		String			fCommandNumber;
};

class	MenuTitleEntry : public Saveable {
	public:
		MenuTitleEntry ();
		MenuTitleEntry (const String title);
		MenuTitleEntry (const MenuTitleEntry& entry);
		~MenuTitleEntry ();
	
		nonvirtual	String		GetTitle () const;
		nonvirtual	void		SetTitle (const String& title);

		nonvirtual	const AbSequence(MenuItemEntryPtr)&	GetItems () const;
		
		virtual	Menu*	BuildMenu () const;
		
		virtual	void	WriteCode (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui, const String& menuName);
		
	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;

	private:
		String			fTitle;
		Sequence(MenuItemEntryPtr)	fItems;
};


#endif	/* __MenuBarItem__ */
