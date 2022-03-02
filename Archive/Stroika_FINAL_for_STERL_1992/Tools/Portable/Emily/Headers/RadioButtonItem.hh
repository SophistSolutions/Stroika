/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__RadioButtonItem__
#define	__RadioButtonItem__

/*
 * $Header: /fuji/lewis/RCS/RadioButtonItem.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: RadioButtonItem.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.13  92/05/13  18:38:42  18:38:42  lewis (Lewis Pringle)
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
	typedef	class	RadioButtonParam*	RBParmPtr;
	#define	RadioButtonParamPtr	RBParmPtr
	
	Declare (Iterator, RBParmPtr);
	Declare (Collection, RBParmPtr);
	Declare (AbSequence, RBParmPtr);
	Declare (Array, RBParmPtr);
	Declare (Sequence_Array, RBParmPtr);
	Declare (Sequence, RBParmPtr);
#endif

class	RadioButtonParam : public Saveable {
	public:
		RadioButtonParam (CommandNumber language, CommandNumber gui);
		
		CommandNumber	fLanguage;
		CommandNumber	fGUI;
		
		String		fLabel;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
};


class	RadioButton;
class	RadioButtonItem : public ButtonItem {
	public:
		RadioButtonItem (ItemType& type);
		~RadioButtonItem ();

		override	void	SetGroup (GroupItem* parent);

		nonvirtual	AbstractRadioButton&	GetRadioButton () const;
		override	String	GetHeaderFileName ();
		
		nonvirtual	String	GetLabel () const;
		nonvirtual	void	SetLabel (const String& label);

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteDestructor (class ostream& to, int tabCount, CommandNumber gui);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
	
		override	void	AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI);
		override	void	RemoveParam (CommandNumber language, CommandNumber gui);
		override	void	ApplyParams (CommandNumber language, CommandNumber gui);

override	void		OldParamReadHack (class istream& from);
	
		override	void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);

		nonvirtual	RadioButtonParam*	FindLabel (CommandNumber language, CommandNumber gui) const;
		nonvirtual	RadioButtonParam&	GetCurrentLabel () const;

	private:
		AbstractRadioButton*							fRadioButton;
		Sequence(RadioButtonParamPtr)	fLabels;
};

class	RadioButtonItemType : public ItemType {
	public:
		RadioButtonItemType ();
		
		static	RadioButtonItemType&	Get ();
		
	private:		
		static	ViewItem*	RadioButtonItemBuilder ();
		static	RadioButtonItemType*	sThis;
};

class	SetRadioButtonInfoCommand : public Command {
	public:
		SetRadioButtonInfoCommand (RadioButtonItem& item, class RadioButtonInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		RadioButtonItem&	fItem;
		class Command*		fItemInfoCommand;
	
		Boolean	fNewOn;
		Boolean	fOldOn;
		String	fNewLabel;
		String	fOldLabel;
};

#endif	/* __RadioButtonItem__ */
