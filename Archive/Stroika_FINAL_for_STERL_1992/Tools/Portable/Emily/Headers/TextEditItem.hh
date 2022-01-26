/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__TextEditItem__
#define	__TextEditItem__

/*
 * $Header: /fuji/lewis/RCS/TextEditItem.hh,v 1.6 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextEditItem.hh,v $
 *		Revision 1.6  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.8  1992/03/06  21:53:47  sterling
 *		motif
 *
 *
 *
 *
 */


#include	"ScrollableText.hh"
#include	"TextEdit.hh"

#include	"ViewItem.hh"


#if		!qRealTemplatesAvailable
	typedef	class	TextEditParam*	TEParmPtr;
	#define	TextEditParamPtr	TEParmPtr

	Declare (Iterator, TEParmPtr);
	Declare (Collection, TEParmPtr);
	Declare (AbSequence, TEParmPtr);
	Declare (Array, TEParmPtr);
	Declare (Sequence_Array, TEParmPtr);
	Declare (Sequence, TEParmPtr);
#endif

class	TextEditParam : public Saveable {
	public:
		TextEditParam (CommandNumber language, CommandNumber gui, AbstractTextEdit& te);
		
		CommandNumber	fLanguage;
		CommandNumber	fGUI;
		
		Boolean			fDefaultWordWrap;
		AbstractTextView::Justification	fDefaultJustification;
		Boolean			fDefaultRequireText;
		CollectionSize	fDefaultMaxLength;
		Boolean			fDefaultMultiLine;
		String			fDefaultText;
		
		Boolean			fWordWrap;
		AbstractTextView::Justification	fJustification;
		Boolean			fRequireText;
		CollectionSize	fMaxLength;
		Boolean			fMultiLine;
		String			fText;

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
};





class	TextEditItem : public ViewItem {
	public:
		TextEditItem (ItemType& type);
		~TextEditItem ();

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

		override	void	EditModeChanged (Boolean newEditMode);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
				
		nonvirtual	Boolean	GetHasVerticalSBar () const;
		nonvirtual	void	SetHasVerticalSBar (Boolean hasSBar);
		nonvirtual	Boolean	GetHasHorizontalSBar () const;
		nonvirtual	void	SetHasHorizontalSBar (Boolean hasSBar);

		override	String	GetHeaderFileName ();
		
		nonvirtual	CollectionSize	GetMaxLength () const;
		nonvirtual	void			SetMaxLength (CollectionSize maxLength);
		
		nonvirtual	Boolean			GetMultiLine () const;
		nonvirtual	void			SetMultiLine (Boolean multiLine);
		
		nonvirtual	Boolean			GetTextRequired () const;
		nonvirtual	void			SetTextRequired (Boolean required);
		
		nonvirtual	Boolean			GetWordWrap () const;
		nonvirtual	void			SetWordWrap (Boolean wordWrap);
		
		nonvirtual	String	GetText () const;
		nonvirtual	void	SetText (const String& text);

		nonvirtual	AbstractTextView::Justification	GetJustification () const;
		nonvirtual	void	SetJustification (AbstractTextView::Justification just);

		override	String	GetSubviewFieldName () const;

		virtual		void	SetTextEdit (AbstractTextEdit* text);

	protected:
		override	void	Layout ();
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	SetOwnedEnabled (Boolean enabled, UpdateMode updateMode);
		override	void	WriteDeclaration (class ostream& to, int tabCount);
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteDestructor (class ostream& to, int tabCount, CommandNumber gui);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	SetItemInfo ();
		
		override	void	AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI);
		override	void	RemoveParam (CommandNumber language, CommandNumber gui);
		override	void	ApplyParams (CommandNumber language, CommandNumber gui);
		override	void	GUIChanged (CommandNumber oldGUI, CommandNumber newGUI);

		nonvirtual	TextEditParam*	FindTEParams (CommandNumber language, CommandNumber gui) const;
		nonvirtual	TextEditParam&	GetCurrentTEParams () const;

		nonvirtual	String	GetTextScrollerName () const;
	
		nonvirtual	AbstractTextEdit&	GetTextEdit () const;

	private:
		AbstractTextEdit*		fTextEdit;
		TextScroller*	fTextScroller;
		Boolean			fHasVerticalSBar;
		Boolean			fHasHorizontalSBar;
		Sequence(TextEditParamPtr)	fTEParams;
	
	friend	class TextEditItemType;
};

class	TextEditItemType : public ItemType {
	public:
		TextEditItemType ();
		
		static	TextEditItemType&	Get ();
		
	private:		
		static	ViewItem*	TextEditItemBuilder ();
		static	TextEditItemType*	sThis;
};

class	SetTextEditInfoCommand : public Command {
	public:
		SetTextEditInfoCommand (TextEditItem& item, class TextEditInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		TextEditItem&	fItem;
		class Command*	fItemInfoCommand;
		
		String			fNewText;
		String			fOldText;
		Boolean			fNewWordWrap;
		Boolean			fOldWordWrap;
		Boolean			fNewRequireText;
		Boolean			fOldRequireText;
		CollectionSize	fNewMaxLength;
		CollectionSize	fOldMaxLength;
		Boolean			fNewMultiLine;
		Boolean			fOldMultiLine;
		Boolean			fNewHasVerticalSBar;
		Boolean			fOldHasVerticalSBar;
		Boolean			fNewHasHorizontalSBar;
		Boolean			fOldHasHorizontalSBar;
};

#endif	/* __TextEditItem__ */
