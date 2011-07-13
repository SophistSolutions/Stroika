/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__PushButtonItem__
#define	__PushButtonItem__

/*
 * $Header: /fuji/lewis/RCS/PushButtonItem.hh,v 1.6 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PushButtonItem.hh,v $
 *		Revision 1.6  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
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
	typedef	class	PushButtonParam*	PushButtonParamPtr;
	#define	PBParmPtr	PushButtonParamPtr
	
	Declare (Iterator, PBParmPtr);
	Declare (Collection, PBParmPtr);
	Declare (AbSequence, PBParmPtr);
	Declare (Array, PBParmPtr);
	Declare (Sequence_Array, PBParmPtr);
	Declare (Sequence, PBParmPtr);
#endif

class	PushButtonParam : public Saveable {
	public:
		PushButtonParam (CommandNumber language, CommandNumber gui);
		
		CommandNumber	fLanguage;
		CommandNumber	fGUI;
		
		String		fLabel;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
};

class	PushButton;
class	PushButtonItem : public ButtonItem {
	public:
		PushButtonItem (ItemType& type);
		~PushButtonItem ();
	
		nonvirtual	AbstractPushButton&	GetPushButton () const;
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
	
		override	void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);

		override	void	AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI);
		override	void	RemoveParam (CommandNumber language, CommandNumber gui);
		override	void	ApplyParams (CommandNumber language, CommandNumber gui);
		
override	void		OldParamReadHack (class istream& from);
		
		nonvirtual	PushButtonParam*	FindLabel (CommandNumber language, CommandNumber gui) const;
		nonvirtual	PushButtonParam&	GetCurrentLabel () const;

	private:
		AbstractPushButton*				fPushButton;
		Sequence(PushButtonParamPtr)	fLabels;
};

class	PushButtonItemType : public ItemType {
	public:
		PushButtonItemType ();
		
		static	PushButtonItemType&	Get ();
		
	private:		
		static	ViewItem*	PushButtonItemBuilder ();
		static	PushButtonItemType*	sThis;
};

class	SetPushButtonInfoCommand : public Command {
	public:
		SetPushButtonInfoCommand (PushButtonItem& item, class PushButtonInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		PushButtonItem&	fItem;
		class Command*	fItemInfoCommand;
	
		String	fNewLabel;
		String	fOldLabel;
};


#endif	/* __PushButtonItem__ */
