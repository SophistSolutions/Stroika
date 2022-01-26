/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__MenuTitleItem__
#define	__MenuTitleItem__

/*
 * $Header: /fuji/lewis/RCS/MenuTitleItem.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: MenuTitleItem.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *
 */


#include	"MenuBar.hh"

#include	"ViewItem.hh"
#include	"Accelerator.hh"

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

class	MenuItemEntry : public Saveable {
	public:
		MenuItemEntry ();
	
		nonvirtual	String	GetLabel () const;
		nonvirtual	void	SetLabel (const String& label);
		
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

class	MenuTitleItem : public ViewItem {
	public:
		MenuTitleItem (ItemType& type);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		
		override	String	GetHeaderFileName ();
		
		nonvirtual	String	GetTitle () const;
		nonvirtual	void	SetTitle (const String& title);
		
		nonvirtual	String	GetMenuFieldName () const;

		override	void	EditModeChanged (Boolean newEditMode);

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteDeclaration (class ostream& to, int tabCount);
		override	void	WriteBuilder (class ostream& to, int tabCount, CommandNumber gui);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
	
	private:
		MenuBar*			fMenuBar;
		class MenuTitle*	fMenuTitle;
		class Menu*			fMenu;
		String				fTitle;
		Sequence(MenuItemEntryPtr)	fEntries;
};

class	MenuTitleItemType : public ItemType {
	public:
		MenuTitleItemType ();
		
		static	MenuTitleItemType&	Get ();
		static	ViewItem*	MenuTitleItemBuilder ();
		
	private:		
		static	MenuTitleItemType*	sThis;
};

// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/* __MenuTitleItem__ */
