/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TextEdit__
#define	__TextEdit__

/*
 * $Header: /fuji/lewis/RCS/TextEdit.hh,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *		-	Implement a new kind of string buffer to wrap the native implementation buffers. Will make GetText considerably more efficient.
 *
 * Changes:
 *	$Log: TextEdit.hh,v $
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.5  1992/07/16  16:25:55  sterling
 *		renamed GUI to UI, made Mac_UI and Motif_UI variants
 *
 *		Revision 1.4  1992/07/02  04:42:54  lewis
 *		Just include Sequence.hh instead of Sequence*.
 *
 *		Revision 1.3  1992/07/02  04:40:28  lewis
 *		Renamed Sequence_DoublyLinkedList->Sequence.
 *
 *		Revision 1.2  1992/06/25  07:11:51  sterling
 *		(LGP checked in for sterl) At least changed CalcDefaultSize to CalcDefaultSize_ and reacted to
 *		fact that View now is Bordered - not sure what else.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.36  1992/05/13  12:43:21  lewis
 *		Override SetText() to take char* arg.
 *
 *		Revision 1.35  92/04/17  01:19:25  01:19:25  lewis (Lewis Pringle)
 *		Changed fOSControl to fTextEditControl in MotifOSControl, and added fWrapperControl.
 *		
 *		Revision 1.34  92/04/14  19:36:37  19:36:37  lewis (Lewis Pringle)
 *		Added DTOR for TextEdit in #if qSoleInlineVirtualsNotStripped.
 *		
 *		Revision 1.33  92/04/14  11:59:52  11:59:52  lewis (Lewis Pringle)
 *		Minor cleanups - rename mac clickloopproc.
 *		
 *		Revision 1.32  92/04/09  02:25:22  02:25:22  lewis (Lewis Pringle)
 *		Got rid of EffectiveFontChanged override for motif textedit since taken care of by motif os control.s
 *		
 *		Revision 1.31  92/04/08  15:34:29  15:34:29  lewis (Lewis Pringle)
 *		OVerride TabbingFocus () on mac native TE to translate delayed update to immediate update so we can use TEActivae/TEDeactivate
 *		and get less flicker.
 *		
 *		Revision 1.30  92/03/26  15:17:28  15:17:28  lewis (Lewis Pringle)
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of
 *		oldFocused first argument.
 *		
 *		Revision 1.29  1992/03/26  09:38:44  lewis
 *		Got rid of extra arg 'oldLive' to EffectiveLiveChanged, and newFont and updateMode args to EffectiveFontChanged.
 *
 *		Revision 1.27  1992/03/22  17:09:04  lewis
 *		Got rid of serveral inline functions cuz they were too big (wouldnt compile oon mac with -z15, -z17).
 *
 *		Revision 1.26  1992/03/10  13:06:39  lewis
 *		Add override of DispatchKeyEvent to motif native textedit.
 *
 *		Revision 1.25  1992/03/09  23:59:19  lewis
 *		Quite a bit of reworking to support new keystroke/keyboard interface.
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.24  1992/03/06  22:23:13  lewis
 *		Got rid of a few empty dtors, and made a few ctors/dtors inline.
 *		Implemented TextEdit as a class, rather than a #define.
 *		Made AbstractTextEdit inherit directly from FocusItem, and EnableItem instead of
 *		FocusView, since I believe its VERY bad style to pervert the inheritance hierarchy to save a
 *		tiny bit of implemenation code, and as it turns out (not surprisingly) it ended up saving
 *		several overrides - saving code!
 *
 *		Revision 1.22  1992/03/05  20:07:26  sterling
 *		used Boreded, support single line text
 *		supported max text length
 *
 *		Revision 1.21  1992/02/21  20:04:01  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround, and moved TextEdit #defines together - later use class.
 *		Got rid of unneded destructor.
 *
 *		Revision 1.20  1992/02/15  04:50:37  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.17  1992/01/31  16:25:01  sterling
 *		overrode EffectiveFontChanged for Mac
 *
 *		Revision 1.15  1992/01/24  17:54:35  lewis
 *		Override Realize/Unrealize to update values.
 *
 *		Revision 1.12  1992/01/22  15:09:02  sterling
 *		merged in EditText functionalir many other changes
 *
 *		Revision 1.11  1992/01/20  12:24:03  lewis
 *		Added justification field.
 *
 *
 *
 */

#include	"Sequence.hh"

#include	"Color.hh"
#include	"Font.hh"

#include	"Changeable.hh"
#include	"FocusItem.hh"
#include	"KeyHandler.hh"
#include	"MenuCommandHandler.hh"
#include	"TextView.hh"








class	TextController;
class	TextEditBase {
	protected:
		TextEditBase (TextController* controller);

	public:
		virtual	~TextEditBase ();

	public:
		nonvirtual	TextController*	GetController () const;
		nonvirtual	void			SetController (TextController* controller);

	private:
		TextController*	fController;		
};



class	TextController : public TextEditBase {
	public:
		TextController (TextController* controller = Nil);
		
		virtual	void	TextChanged (TextEditBase* text);
		virtual	void	TextTracking (const Point& mouseAt);	// mouseAt in TextEdit's local coordinates
};



// should be scoped within AbstractTextEdit
/*
 *	TextSelection specifies a range within a TextEdit. The possible values range from 1 through the length
 *	of text plus 1. A givin value specifies a point just prior to a string index. For example, a TexSelection
 *  of 1,1 represents a blinking carat just prior to the first character. A TextSelection of 1,2 represents
 *  having the first characater highlighted (and thus copyable, etc.).
 */
class	TextSelection {
	public:
		TextSelection (CollectionSize from, CollectionSize to);
		TextSelection (const TextSelection& from);
		
		nonvirtual	const	TextSelection&	operator= (const TextSelection& rhs);

		nonvirtual	CollectionSize	GetFrom () const;
		nonvirtual	void			SetFrom (CollectionSize from);
		nonvirtual	CollectionSize	GetTo () const;
		nonvirtual	void			SetTo (CollectionSize to);
		
		nonvirtual	Boolean		TextIsSelected () const;
		
	private:
		CollectionSize	fFrom;
		CollectionSize	fTo;
};
extern	Boolean	operator== (const TextSelection& lhs, const TextSelection& rhs);
extern	Boolean	operator!= (const TextSelection& lhs, const TextSelection& rhs);



// should be scoped within AbstractTextEdit
class	TextStyle {
	public:
		TextStyle (const Font& font, const Color& color);
		TextStyle (const TextStyle& from);
		
		nonvirtual	const	TextStyle&	operator= (const TextStyle& rhs);

		nonvirtual	Font	GetFont () const;
		nonvirtual	void	SetFont (const Font& font);
		nonvirtual	Color	GetColor () const;
		nonvirtual	void	SetColor (const Color& color);
		
	private:
		Font	fFont;
		Color	fColor;
};
extern	Boolean	operator== (const TextStyle& lhs, const TextStyle& rhs);
extern	Boolean	operator!= (const TextStyle& lhs, const TextStyle& rhs);



// should be scoped within AbstractTextEdit
class	TextRun	{
	public:
		TextRun (const TextStyle& textStyle, const TextSelection& textSelection);
		
		nonvirtual	const	TextRun&	operator= (const TextRun& rhs);

		nonvirtual	TextStyle	GetStyle () const;
		nonvirtual	void		SetStyle (const TextStyle& textStyle);
		
		nonvirtual	TextSelection	GetSelection () const;
		nonvirtual	void			SetSelection (const TextSelection& textSelection);
		
	private:
		TextStyle		fStyle;
		TextSelection	fSelection;
};
extern	Boolean	operator== (const TextRun& lhs, const TextRun& rhs);
extern	Boolean	operator!= (const TextRun& lhs, const TextRun& rhs);



class	AbstractTextEdit;
#if		!qRealTemplatesAvailable
	Declare (Iterator, TextRun);
#endif

class	TextRunIterator : public Iterator(TextRun) {
	public:
		TextRunIterator (AbstractTextEdit&	textEdit, TextSelection selection);

		override	Boolean	Done () const;
		override	void	Next ();
		override	TextRun	Current () const;

	private:
		TextSelection		fSelection;
		AbstractTextEdit&	fTextEdit;
		TextRun				fCurrentRun;
		Boolean				fDone;
};




class	AbstractTextEdit :	public View, public EnableItem, public FocusItem,
							public AbstractTextView, public TextEditBase, public Changeable {
	public:
		enum EditMode {
			eEditable,		// full editing capabilities
			eSelectOnly,	// user can select (and copy) text, but not alter text
			eDisplayOnly,	// not reponsive to mouse clicks, user cannot alter text
		};
		static	CollectionSize	kAnyLength;
		static	Boolean			kMultiLine;

	protected:
		AbstractTextEdit (TextController* controller);

	public:
		~AbstractTextEdit ();
		
		override	Boolean	GetLive () const;
		

// View and FocusItem and EnableItem if they are subclasses of LiveItem sb virtual subclass so we
// dont have to hack GetEffectiveLive, etcc..
override	Boolean	GetEffectiveLive () const { return (View::GetEffectiveLive ()); }



		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);

		nonvirtual	String	GetText ()	const;													// shadow AbstractTextView version so we can overload function				
		nonvirtual	String	GetText (const TextSelection& selection) const;

		nonvirtual	void	SetText (const String& s, UpdateMode updateMode = eDelayedUpdate);	// shadow AbstractTextView version so we can overload function		
		nonvirtual	void	SetText (const String& s, const TextSelection& selection, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void	SetText (const char* s, UpdateMode updateMode = eDelayedUpdate);	// shadow AbstractTextView version so we can overload function		
		nonvirtual	void	SetText (const char* s, const TextSelection& selection, UpdateMode updateMode = eDelayedUpdate);

		virtual	CollectionSize	GetLength () const = Nil;
		
		nonvirtual	AbstractTextEdit::EditMode	GetEditMode () const;
		nonvirtual	void						SetEditMode (EditMode mode);		

		nonvirtual		Boolean	GetAutoSize (Point::Direction d) const;
		nonvirtual		void	SetAutoSize (Point::Direction d, Boolean autoSize, UpdateMode updateMode = eDelayedUpdate);
	
		// if text is required the textedit will refuse to leave focus with an empty text string
		nonvirtual	Boolean	GetTextRequired () const;
		nonvirtual	void	SetTextRequired (Boolean required);

		override	void	Validate ();
		
		virtual	TextSelection	GetSelection () const																	=	Nil;
		virtual	void			SetSelection (UpdateMode updateMode = eDelayedUpdate)									= 	Nil;
		virtual	void			SetSelection (const TextSelection& selection, UpdateMode updateMode = eDelayedUpdate)	= 	Nil;
		
		virtual	TextStyle	GetStyle (CollectionSize i) const																			= Nil;
		virtual	void		SetStyle (const TextStyle& style, UpdateMode updateMode = eDelayedUpdate)									= Nil;
		virtual	void		SetStyle (const TextStyle& style, const TextSelection& selection, UpdateMode updateMode = eDelayedUpdate)	= Nil;

		virtual	TextRun		GetTextRun (CollectionSize i) const = Nil;

		virtual		const TextRunIterator*	MakeRunIterator ();
		virtual		const TextRunIterator*	MakeRunIterator (const TextSelection& selection);
		
		virtual		TextSelection	Find (const String& pattern, CollectionSize i = 1)							= Nil;
		virtual		TextSelection	Replace (const String& pattern, const String& newText, CollectionSize i = 1) = Nil;
		virtual		void			ReplaceAll (const String& pattern, const String& newText) 					= Nil;

		/*
		 * Get the location in pixels of the given offset into the text, and do the inverse.
		 */
		virtual	Point			GetPosition (CollectionSize i) const		=	Nil;
		virtual	CollectionSize	GetOffset (const Point& p) const		=	Nil;

		/*
		 * Get the word nearest a given location or index.
		 */
		virtual	String	GetNearestWord (const Point& p) const			=	Nil;
		virtual	String	GetNearestWord (CollectionSize i) const			=	Nil;

		virtual	CollectionSize	GetLineCount () const						=	Nil;
		virtual	String			GetLine (CollectionSize i) const				=	Nil;
		virtual	TextSelection	GetLineLocation (CollectionSize line) const	=	Nil;

		virtual	void	DoCut (UpdateMode updateMode)	=	Nil;
		virtual	void	DoCopy (UpdateMode updateMode)	=	Nil;
		virtual	void	DoPaste (UpdateMode updateMode)	=	Nil;
		virtual	void	DoClear (UpdateMode updateMode)	=	Nil;
		virtual	void	DoKeyPress (Character c, UpdateMode updateMode)	=	Nil;

		nonvirtual	CollectionSize	GetMaxLength () const;
		nonvirtual	void			SetMaxLength (CollectionSize maxLength);
		
		nonvirtual	Boolean			GetMultiLine () const;
		nonvirtual	void			SetMultiLine (Boolean multiLine);
		
	protected:
		override	void		EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);
		override	void		EffectiveFocusChanged (Boolean newFocused, UpdateMode updateMode);
		override	void		SetChangeCount_ (ChangeCount newCount);		

		override	String	GetText_ ()	const = Nil;										// shadow AbstractTextView version so we can overload function				
		virtual		String	GetText_ (const TextSelection& selection) const = Nil;

		override	void	SetText_ (const String& s, UpdateMode updateMode) = Nil;		// shadow AbstractTextView version so we can overload function		
		virtual		void	SetText_ (const String& s, const TextSelection& selection, UpdateMode updateMode) = Nil;

		virtual		void	SetEditMode_ (EditMode mode);
		virtual		void	SetTextRequired_ (Boolean required);
		virtual		void	SetAutoSize_ (Point::Direction d, Boolean autoSize, UpdateMode updateMode);
		
		virtual		Boolean	ValidateKey (const KeyStroke& keyStroke);

		/*
		 * Called whenever the size of the text may have changed.  Routine must check if it has, and
		 * call setsize, for the changes in each dimention.  This routine must respect the
		 * values of fVerticalAutoSize and fHorizontalAutoSize, and check them itself.
		 */
		virtual		void	AdjustSize (UpdateMode updateMode) = Nil;
		
		/*
		 * If all conditions for editable text are satisfied (Live, eEditable, and Focused), then make text active (blinking cursor, etc.)
		 */
		virtual		void	SetTextActive (Boolean active, UpdateMode updateMode)	=	Nil;

		virtual		void	TrackingDrag (const Point& mouseAt);	// called repeatedly while drag selecting
																	// override to provide scrolling behavior
		
		virtual	void	SetMaxLength_ (CollectionSize maxLength);
		virtual	void	SetMultiLine_ (Boolean multiLine);

	protected:
		override	Boolean	GetEnabled_ () const;
		override	void	SetEnabled_ (Boolean enabled, UpdateMode updateMode);

	private:															
		Boolean			fEnabled;
		EditMode		fEditMode;
		Boolean			fTextRequired;
		Boolean			fVerticalAutoSize;
		Boolean			fHorizontalAutoSize;
		Boolean			fMultiLine;
		CollectionSize	fMaxLength;
};





class	AbstractTextEdit_MacUI : public AbstractTextEdit {
	public:
		~AbstractTextEdit_MacUI ();

	protected:
		AbstractTextEdit_MacUI (TextController* controller);
};



#if		qMacToolkit

class	TextEdit_MacUI_Native : public AbstractTextEdit_MacUI {
	public:
		TextEdit_MacUI_Native (TextController* controller = Nil);
		~TextEdit_MacUI_Native ();

		override	Boolean	TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);

		override	void	Layout ();
		override	void	Draw (const Region& update);
		override	void	Render (const Region& updateRegion);

		override	CollectionSize	GetLength () const;

		override	TextSelection	GetSelection () const;
		override	void			SetSelection (UpdateMode updateMode = eDelayedUpdate);
		override	void			SetSelection (const TextSelection& selection, UpdateMode updateMode = eDelayedUpdate);

		override	TextStyle	GetStyle (CollectionSize i) const;
		override	void		SetStyle (const TextStyle& style, UpdateMode updateMode = eDelayedUpdate);
		override	void		SetStyle (const TextStyle& style, const TextSelection& selection, UpdateMode updateMode = eDelayedUpdate);

		override	TextRun		GetTextRun (CollectionSize i) const;

		override	TextSelection	Find (const String& pattern, CollectionSize i = 1);
		override	TextSelection	Replace (const String& pattern, const String& newText, CollectionSize i = 1);
		override	void			ReplaceAll (const String& pattern, const String& newText);

		override	Point			GetPosition (CollectionSize i) const;
		override	CollectionSize	GetOffset (const Point& p) const;

		override	String	GetNearestWord (const Point& p) const;
		override	String	GetNearestWord (CollectionSize i) const;

		override	CollectionSize	GetLineCount () const;
		override	String			GetLine (CollectionSize i) const;
		override	TextSelection	GetLineLocation (CollectionSize line) const;

		nonvirtual	struct	TERec**	GetOSRepresentation () const;

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	TabbingFocus (SequenceDirection d, UpdateMode updateMode);
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

		override	String	GetText_ ()	const;		
		override	String	GetText_ (const TextSelection& selection) const;

		override	void	SetText_ (const String& s, UpdateMode updateMode) ;	
		override	void	SetText_ (const String& s, const TextSelection& selection, UpdateMode updateMode);

		override	Justification	GetJustification_ () const;
		override	void			SetJustification_ (Justification justification, UpdateMode updateMode);
		
		override	Boolean	GetWordWrap_ () const;
		override	void	SetWordWrap_ (Boolean wordWrap, UpdateMode updateMode);
		override	void	AdjustSize (UpdateMode updateMode);
		override	void	SetTextActive (Boolean active, UpdateMode updateMode);

		override	void	DoCut (UpdateMode updateMode);
		override	void	DoCopy (UpdateMode updateMode);
		override	void	DoPaste (UpdateMode updateMode);
		override	void	DoClear (UpdateMode updateMode);
		override	void	DoKeyPress (Character c, UpdateMode updateMode);

	private:
		struct	TERec**		fTEHandle;
		class	TEBlinker*	fBlinker;
		TextStyle			fEffectiveStyle;

		nonvirtual	void	PrepareForDraw (UpdateMode updateMode);
		nonvirtual	void	ResetEffectiveStyle ();
		nonvirtual	void	ResetNilRun ();

		static	TextEdit_MacUI_Native*	sCurrentTE;

		static	pascal	unsigned char	sClickLoopProc ();

	friend	class	TEBlinker;
	friend	class	TEClipCommand;
	friend	class	TEKeyCommand;
};
#endif	/*qMacToolkit*/







class	AbstractTextEdit_MotifUI : public AbstractTextEdit {
	public:
		~AbstractTextEdit_MotifUI ();
		
	protected:
		AbstractTextEdit_MotifUI (TextController* controller);
};



#if		qXmToolkit

class	TextEdit_MotifUI_Native : public AbstractTextEdit_MotifUI {
	public:
		TextEdit_MotifUI_Native (TextController* controller = Nil);
		virtual ~TextEdit_MotifUI_Native ();

		override	Boolean	TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);
		override	Boolean	DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
							KeyComposeState& composeState);
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);

		override	void	Layout ();
		override	void	Draw (const Region& update);

		override	CollectionSize	GetLength () const;

		override	TextSelection	GetSelection () const;
		override	void			SetSelection (UpdateMode updateMode = eDelayedUpdate);
		override	void			SetSelection (const TextSelection& selection, UpdateMode updateMode = eDelayedUpdate);
		
		override	TextStyle	GetStyle (CollectionSize i) const;
		override	void		SetStyle (const TextStyle& style, UpdateMode updateMode = eDelayedUpdate);
		override	void		SetStyle (const TextStyle& style, const TextSelection& selection, UpdateMode updateMode = eDelayedUpdate);

		override	TextRun		GetTextRun (CollectionSize i) const;

		override	TextSelection	Find (const String& pattern, CollectionSize i = 1);
		override	TextSelection	Replace (const String& pattern, const String& newText, CollectionSize i = 1);
		override	void			ReplaceAll (const String& pattern, const String& newText);

		override	Point			GetPosition (CollectionSize i) const;
		override	CollectionSize	GetOffset (const Point& p) const;

		override	String	GetNearestWord (const Point& p) const;
		override	String	GetNearestWord (CollectionSize i) const;

		override	CollectionSize	GetLineCount () const;
		override	String			GetLine (CollectionSize i) const;
		override	TextSelection	GetLineLocation (CollectionSize line) const;

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

		override	String	GetText_ ()	const;		
		override	String	GetText_ (const TextSelection& selection) const;

		override	void	SetText_ (const String& s, UpdateMode updateMode) ;	
		override	void	SetText_ (const String& s, const TextSelection& selection, UpdateMode updateMode);

		override	Justification	GetJustification_ () const;
		override	void			SetJustification_ (Justification justification, UpdateMode updateMode);
		
		override	Boolean	GetWordWrap_ () const;
		override	void	SetWordWrap_ (Boolean wordWrap, UpdateMode updateMode);

		override	void	AdjustSize (UpdateMode updateMode);
		override	void	SetTextActive (Boolean active, UpdateMode updateMode);

		override	void	DoCut (UpdateMode updateMode);
		override	void	DoCopy (UpdateMode updateMode);
		override	void	DoPaste (UpdateMode updateMode);
		override	void	DoClear (UpdateMode updateMode);
		override	void	DoKeyPress (Character c, UpdateMode updateMode);

		override	void	SetBorder_ (const Point& border, UpdateMode updateMode); 
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode); 

		override	void	SetMaxLength_ (CollectionSize maxLength);
		override	void	SetMultiLine_ (Boolean multiLine);

#if		qXmToolkit
		override	void	Realize (osWidget* parent);
		override	void	UnRealize ();
#endif

	private:
		class	MotifOSControl;
		MotifOSControl*		fWrapperControl;
		MotifOSControl*		fTextEditControl;
		TextStyle			fEffectiveStyle;
		String				fText_UnRealized;			// value of text when object unrealized...
		Justification		fJustification;
		Boolean				fWordWrap;
		TextSelection		fSelection;
};

#endif	/*qXmToolkit*/




class	TextEdit_MacUI : public
#if		qNative && qMacToolkit
	TextEdit_MacUI_Native
#else
	// should be portable TE engine
	TextEdit_MacUI_Native
#endif
	{
		public:
			TextEdit_MacUI (TextController* controller);
		
#if		qSoleInlineVirtualsNotStripped
			virtual ~TextEdit_MacUI ();
#endif

		protected:
			override	void	Draw (const Region& update);
};

class	TextEdit_MotifUI : public
#if		qNative && qXmToolkit
	TextEdit_MotifUI_Native
#else
	// should be portable TE engine
	TextEdit_MacUI_Native
#endif
	{
		public:
			TextEdit_MotifUI (TextController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~TextEdit_MotifUI ();
#endif

		protected:
			override	void	Draw (const Region& update);
};


class	TextEdit_WinUI : public
	// should be portable TE engine
	TextEdit_MacUI_Native
{
		public:
			TextEdit_WinUI (TextController* controller);

#if		qSoleInlineVirtualsNotStripped
			virtual ~TextEdit_WinUI ();
#endif

		protected:
			override	void	Draw (const Region& update);
};






class	TextEdit : public
#if		qMacUI
	TextEdit_MacUI
#elif	qMotifUI
	TextEdit_MotifUI
#elif	qWinUI
	TextEdit_WinUI
#endif
	{
		public:
			TextEdit (TextController* controller = Nil);

#if		qSoleInlineVirtualsNotStripped
			virtual ~TextEdit ();
#endif
};








/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	TextEditBase::TextEditBase (TextController* controller):	fController (controller)	{}
inline	TextEditBase::~TextEditBase ()							{}
inline	TextController*	TextEditBase::GetController () const	{	return (fController);		}


inline	const	TextSelection&	TextSelection::operator= (const TextSelection& rhs)
{
	fFrom = rhs.fFrom;
	fTo = rhs.fTo;
	return (*this);
}

inline	CollectionSize	TextSelection::GetFrom () const				{	return (fFrom);	}
inline	void			TextSelection::SetFrom (CollectionSize from)	{	fFrom = from;	}
inline	CollectionSize	TextSelection::GetTo () const				{	return (fTo);	}
inline	void			TextSelection::SetTo (CollectionSize to)		{ 	fTo = to;		}
inline	Boolean			TextSelection::TextIsSelected () const		{	return (Boolean (GetTo () > GetFrom ()));	}

inline	Font	TextStyle::GetFont () const					{	return (fFont);		}
inline	void	TextStyle::SetFont (const Font& font)		{	fFont = font;		}
inline	Color	TextStyle::GetColor () const				{	return (fColor);	}
inline	void	TextStyle::SetColor (const Color& color)	{	fColor = color;		}



inline	const	TextRun&	TextRun::operator= (const TextRun& rhs)
{
	fStyle = rhs.fStyle;
	fSelection = rhs.fSelection;
	return (*this);
}
inline	TextStyle		TextRun::GetStyle () const								{ 	return (fStyle);		}
inline	void			TextRun::SetStyle (const TextStyle& style)				{ 	fStyle = style;			}
inline	TextSelection	TextRun::GetSelection () const							{ 	return (fSelection);	}
inline	void			TextRun::SetSelection (const TextSelection& selection)	{	fSelection = selection;	}



#if		qMacToolkit
inline	struct	TERec**	TextEdit_MacUI_Native::GetOSRepresentation () const		{ EnsureNotNil (fTEHandle); return (fTEHandle); }
#endif	/*qMacToolkit*/




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__TextEdit__*/

