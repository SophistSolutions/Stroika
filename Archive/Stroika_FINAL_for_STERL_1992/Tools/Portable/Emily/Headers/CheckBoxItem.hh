/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__CheckBoxItem__
#define	__CheckBoxItem__

/*
 * $Header: /fuji/lewis/RCS/CheckBoxItem.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: CheckBoxItem.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.13  92/05/13  18:38:14  18:38:14  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.9  1992/03/06  21:53:47  sterling
 *		motif
 *
 *
 *
 *
 */

#include	"ButtonItem.hh"


#if		!qRealTemplatesAvailable
	typedef	class	CheckBoxParam*	CheckBoxParamPtr;
	#define	CBParmPtr	CheckBoxParamPtr
	
	Declare (Iterator, CBParmPtr);
	Declare (Collection, CBParmPtr);
	Declare (AbSequence, CBParmPtr);
	Declare (Array, CBParmPtr);
	Declare (Sequence_Array, CBParmPtr);
	Declare (Sequence, CBParmPtr);
#endif

class	CheckBoxParam : public Saveable {
	public:
		CheckBoxParam (CommandNumber language, CommandNumber gui);
		
		CommandNumber	fLanguage;
		CommandNumber	fGUI;
		
		String		fLabel;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
};

class	CheckBox;
class	CheckBoxItem : public ButtonItem {
	public:
		CheckBoxItem (ItemType& type);
		~CheckBoxItem ();
		
		nonvirtual	AbstractCheckBox&	GetCheckBox () const;
		override	String	GetHeaderFileName ();
	
		nonvirtual	String	GetLabel () const;
		nonvirtual	void	SetLabel (const String& label);

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
	
		override	void	AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI);
		override	void	RemoveParam (CommandNumber language, CommandNumber gui);
		override	void	ApplyParams (CommandNumber language, CommandNumber gui);

		override	void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);

override	void		OldParamReadHack (class istream& from);

		nonvirtual	CheckBoxParam*	FindLabel (CommandNumber language, CommandNumber gui) const;
		nonvirtual	CheckBoxParam&	GetCurrentLabel () const;

	private:
		AbstractCheckBox*			fCheckBox;
		Sequence(CheckBoxParamPtr)	fLabels;
};

class	CheckBoxItemType : public ItemType {
	public:
		CheckBoxItemType ();
		
		static	CheckBoxItemType&	Get ();
		
	private:		
		static	ViewItem*	CheckBoxItemBuilder ();
		static	CheckBoxItemType*	sThis;
};

class	SetCheckBoxInfoCommand : public Command {
	public:
		SetCheckBoxInfoCommand (CheckBoxItem& item, class CheckBoxInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		CheckBoxItem&	fItem;
		class Command*	fItemInfoCommand;
	
		Boolean	fNewOn;
		Boolean	fOldOn;
		String	fNewLabel;
		String	fOldLabel;
};

#endif	/* __CheckBoxItem__ */
