/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__PickListEditItem__
#define	__PickListEditItem__

/*
 * $Header: /fuji/lewis/RCS/PickListItem.hh,v 1.4 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PickListItem.hh,v $
 *		Revision 1.4  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.9  1992/03/06  21:53:47  sterling
 *		motif
 *
 *		Revision 1.7  1992/01/31  18:24:50  sterling
 *		Bootstrapped
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





class	StringPickList;
class	PickListEditItem : public ViewItem {
	public:
		PickListEditItem (ItemType& type);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		
		nonvirtual	StringPickList&	GetPickList () const;
		override	String	GetHeaderFileName ();
	
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	SetOwnedEnabled (Boolean enabled, UpdateMode updateMode);
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();

	private:
		StringPickList*	fPickList;
};

class	PickListEditItemType : public ItemType {
	public:
		PickListEditItemType ();
		
		static	PickListEditItemType&	Get ();
		
	private:		
		static	ViewItem*	PickListEditItemBuilder ();
		static	PickListEditItemType*	sThis;
};

class	SetPickListInfoCommand : public Command {
	public:
		SetPickListInfoCommand (PickListEditItem& item, class StringPickListInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		PickListEditItem&	fItem;
		class Command*		fItemInfoCommand;
	
		Boolean	fNewFloatingSelection;
		Boolean	fOldFloatingSelection;
		Boolean	fNewMultipleSelections;
		Boolean	fOldMultipleSelections;
		Sequence(String)	fNewEntries;
		Sequence(String)	fOldEntries;
};

#endif	/* __PickListEditItem__ */
