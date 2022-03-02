/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__TextViewItem__
#define	__TextViewItem__

/*
 * $Header: /fuji/lewis/RCS/TextViewItem.hh,v 1.5 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextViewItem.hh,v $
 *		Revision 1.5  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/04  14:35:33  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
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
	typedef	class	TextViewParam*	TVParmPtr;
	#define	TextViewParamPtr	TVParmPtr

	Declare (Iterator, TVParmPtr);
	Declare (Collection, TVParmPtr);
	Declare (AbSequence, TVParmPtr);
	Declare (Array, TVParmPtr);
	Declare (Sequence_Array, TVParmPtr);
	Declare (Sequence, TVParmPtr);
#endif

class	TextViewParam : public Saveable {
	public:
		TextViewParam (CommandNumber language, CommandNumber gui);
		
		CommandNumber	fLanguage;
		CommandNumber	fGUI;
		
		AbstractTextView::Justification	fJustification;
		String							fText;
		Boolean							fWordWrap;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
};



class	TextView;
class	TextEdit;
class	TextViewItem : public ViewItem {
	public:
		TextViewItem (ItemType& type);
		~TextViewItem ();

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
	
		override	String	GetHeaderFileName ();
		
		nonvirtual	Boolean	GetWordWrap () const;
		nonvirtual	void	SetWordWrap (Boolean wordWrap);

		nonvirtual	AbstractTextView::Justification	GetJustification () const;
		nonvirtual	void	SetJustification (AbstractTextView::Justification just);

		nonvirtual	String	GetText () const;
		nonvirtual	void	SetText (const String& text);
				
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

		nonvirtual	TextViewParam*	FindTVParams (CommandNumber language, CommandNumber gui) const;
		nonvirtual	TextViewParam&	GetCurrentTVParams () const;

		virtual		void	SetTextView (TextEdit* TextView);

		virtual		void	SetWordWrap_ (Boolean wordWrap);
		virtual		void	SetJustification_ (AbstractTextView::Justification just);
		virtual		void	SetText_ (const String& text);

		nonvirtual	AbstractTextEdit&	GetTextView () const;

	private:
		TextEdit*	fTextEdit;
		Sequence(TextViewParamPtr)	fTVParams;
};

class	TextViewItemType : public ItemType {
	public:
		TextViewItemType ();
		
		static	TextViewItemType&	Get ();
		
	private:		
		static	ViewItem*	TextViewItemBuilder ();
		static	TextViewItemType*	sThis;
};

class	SetTextViewInfoCommand : public Command {
	public:
		SetTextViewInfoCommand (TextViewItem& item, class TextViewInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		TextViewItem&	fItem;
		class Command*	fItemInfoCommand;
		
		String	fNewText;
		String	fOldText;
		Boolean	fNewWordWrap;
		Boolean	fOldWordWrap;
};

#endif	/* __TextViewItem__ */
