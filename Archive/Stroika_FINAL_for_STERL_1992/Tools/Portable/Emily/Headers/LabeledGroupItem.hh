/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__LabeledGroupItem__
#define	__LabeledGroupItem__

/*
 * $Header: /fuji/lewis/RCS/LabeledGroupItem.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: LabeledGroupItem.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.2  1992/03/06  21:53:47  sterling
 *		motif
 *
 *
 *
 */

#include	"Bag.hh"
#include	"GroupItem.hh"



#if		!qRealTemplatesAvailable
	typedef	class	LabeledGroupParam*	LGParmPtr;
	#define	LabeledGroupParamPtr	LGParmPtr
	
	Declare (Iterator, LGParmPtr);
	Declare (Collection, LGParmPtr);
	Declare (AbBag, LGParmPtr);
	Declare (Array, LGParmPtr);
	Declare (Bag_Array, LGParmPtr);
	Declare (Bag, LGParmPtr);
#endif

class LabeledMagicGroup;
class	LabeledGroupParam : public Saveable {
	public:
		LabeledGroupParam (CommandNumber language, CommandNumber gui);
		
		CommandNumber	fLanguage;
		CommandNumber	fGUI;
		
		String		fLabel;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
};


class	LabeledGroupItem : public GroupItem {
	public:
		LabeledGroupItem ();
		~LabeledGroupItem ();

		override	String	GetHeaderFileName ();

		nonvirtual	String	GetLabel () const;
		nonvirtual	void	SetLabel (const String& title);

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
	
		override	void	AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI);
		override	void	RemoveParam (CommandNumber language, CommandNumber gui);
		override	void	ApplyParams (CommandNumber language, CommandNumber gui);

		nonvirtual	LabeledGroupParam*	FindLabel (CommandNumber language, CommandNumber gui) const;
		nonvirtual	LabeledGroupParam&	GetCurrentLabel () const;

	private:
		Bag(LabeledGroupParamPtr)	fLabels;
		LabeledMagicGroup*			fLabeledGroup;
};

class	LabeledGroupItemType : public ItemType {
	public:
		LabeledGroupItemType ();
		
		static	LabeledGroupItemType&	Get ();
		
	private:		
		static	ViewItem*	LabeledGroupItemBuilder ();
		static	LabeledGroupItemType*	sThis;
};

class	LabeledMagicGroup : public MagicGroupView {
	public:
		LabeledMagicGroup (const String& label);
		
		nonvirtual	String	GetLabel () const;
		nonvirtual	void	SetLabel (const String& label, UpdateMode updateMode = eDelayedUpdate);
	
		override	void	Draw (const Region& update);

	protected:
		virtual		void	SetLabel_ (const String& label, UpdateMode updateMode);
		override	void	DrawBorder_ (const Rect& box, const Point& border);
	
	private:
		String	fLabel;
};

#endif	/* __LabeledGroupItem__ */
