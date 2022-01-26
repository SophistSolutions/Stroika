/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextEditItem.cc,v 1.8 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextEditItem.cc,v $
 *		Revision 1.8  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.11  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *		Revision 1.10  1992/01/29  05:46:41  sterling
 *		fixed adornment code generation
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"CheckBox.hh"
#include	"Dialog.hh"
#include	"TextEdit.hh"

#include	"CommandNumbers.hh"
#include	"TextEditItem.hh"
#include	"GroupItem.hh"
#include	"ItemPallet.hh"
#include	"NumberText.hh"
#include	"ViewItemInfo.hh"
#include	"TextEditInfo.hh"
#include	"TextViewItem.hh"
#include	"EmilyWindow.hh"



#if		!qRealTemplatesAvailable
	Implement (Iterator, TEParmPtr);
	Implement (Collection, TEParmPtr);
	Implement (AbSequence, TEParmPtr);
	Implement (Array, TEParmPtr);
	Implement (Sequence_Array, TEParmPtr);
	Implement (Sequence, TEParmPtr);
#endif

TextEditParam::TextEditParam (CommandNumber language, CommandNumber gui, AbstractTextEdit& te) :
	Saveable (1),
	fLanguage (language),
	fGUI (gui)
{
	fDefaultWordWrap 	  = te.GetWordWrap ();
	fWordWrap 			  = fDefaultWordWrap;
	fDefaultJustification = te.GetJustification ();
	fJustification		  = fDefaultJustification;
	fDefaultRequireText   = te.GetTextRequired ();
	fRequireText 		  = fDefaultRequireText;
	fDefaultMaxLength 	  = te.GetMaxLength ();
	fMaxLength			  = fDefaultMaxLength;
	fDefaultMultiLine 	  = te.GetMultiLine ();
	fMultiLine			  = fDefaultMultiLine;
	fDefaultText		  = te.GetText ();
	fText				  = fDefaultText;
}

void	TextEditParam::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	from >> fLanguage >> fGUI;
	fLanguage += eFirstLanguage;
	fGUI += eFirstGUI;
	from >> fDefaultWordWrap >> fWordWrap >> fDefaultJustification >> fJustification;
	from >> fDefaultRequireText >> fRequireText >> fDefaultMaxLength >> fMaxLength;
	from >> fDefaultMultiLine >> fMultiLine;
	ReadString (from, fDefaultText);
	ReadString (from, fText);
}

void	TextEditParam::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);
	to << newline;
	to << tab (tabCount) << (fLanguage - eFirstLanguage) << ' ' << (fGUI - eFirstGUI) << newline;
	to << tab (tabCount);
	to << fDefaultWordWrap << fWordWrap << fDefaultJustification << fJustification;
	to << fDefaultRequireText << fRequireText << fDefaultMaxLength << ' ' << fMaxLength << ' ';
	to << fDefaultMultiLine << fMultiLine;
	WriteString (to, fDefaultText);
	WriteString (to, fText);
	to << newline;
}





/*
 ********************************************************************************
 ******************************** TextEditItemType ******************************
 ********************************************************************************
 */
TextEditItemType::TextEditItemType ():
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildTextEdit, "TextEdit", (ItemBuilderProc)&TextEditItemBuilder)
#else
	ItemType (eBuildTextEdit, "TextEdit", &TextEditItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
TextEditItemType&	TextEditItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

static	AbstractTextEdit*	BuildTextEdit (CommandNumber gui)
{
	if (gui == eMacUI) {
		return (new TextEdit_MacUI (Nil));
	}
	else if (gui == eMotifUI) {
		return (new TextEdit_MotifUI (Nil));
	}
	else if (gui == eWindowsGUI) {
		return (new TextEdit_WinUI (Nil));
	}
	AssertNotReached ();	return (Nil);
}

ViewItem*	TextEditItemType::TextEditItemBuilder ()
{
	TextEditItem* item = new TextEditItem (Get ());
	item->SetTextEdit (BuildTextEdit (EmilyWindow::GetGUI ()));
	item->SetWordWrap (False);
	
	return (item);
}

TextEditItemType*	TextEditItemType::sThis = Nil;




/*
 ********************************************************************************
 ******************************** TextEditItem **********************************
 ********************************************************************************
 */
// to allow our subclasses to use versioning, we do it by hand, instead
// of relying on Saveable's implementation

const	SaveableVersion	kVersion = 5;

TextEditItem::TextEditItem (ItemType& type):
	ViewItem (type),
	fTextEdit (Nil),
	fTextScroller (Nil),
	fHasVerticalSBar (False),
	fHasHorizontalSBar (False)
{
}

TextEditItem::~TextEditItem ()
{
	ForEach (TextEditParamPtr, it, fTEParams) {
		TextEditParam* param = it.Current ();
		delete param;
	}
}

void	TextEditItem::GUIChanged (CommandNumber /*oldGUI*/, CommandNumber newGUI)
{
	SetTextEdit (BuildTextEdit (newGUI));
	ApplyCurrentParams ();
}

Boolean	TextEditItem::GetHasVerticalSBar () const
{
	return (fHasVerticalSBar);
}

void	TextEditItem::SetHasVerticalSBar (Boolean hasSBar)
{
	if (hasSBar != fHasVerticalSBar) {
		fHasVerticalSBar = hasSBar;
		if (fTextScroller != Nil) {
			fTextScroller->SetVerticalScrollBar (hasSBar ? Scroller::kBuildDefaultSlider : Scroller::kBuildNoSlider);
		}
		else if (fHasVerticalSBar) {
			SetOwnedView (Nil);
			fTextScroller = new TextScroller (Scroller::kBuildDefaultSlider, Scroller::kBuildNoSlider);
			SetOwnedView (fTextScroller);
			fTextScroller->SetTextEdit (fTextEdit);	
			fTextEdit->SetBorder (kZeroPoint);
			fTextEdit->SetMargin (kZeroPoint);
		}
	}
	Ensure (hasSBar == fHasVerticalSBar);
}

Boolean	TextEditItem::GetHasHorizontalSBar () const
{
	return (fHasHorizontalSBar);
}

void	TextEditItem::SetHasHorizontalSBar (Boolean hasSBar)
{
	if (hasSBar != fHasHorizontalSBar) {
		fHasHorizontalSBar = hasSBar;
		if (fTextScroller != Nil) {
			fTextScroller->SetHorizontalScrollBar (hasSBar ? Scroller::kBuildDefaultSlider : Scroller::kBuildNoSlider);
		}
		else if (fHasHorizontalSBar) {
			SetOwnedView (Nil);
			fTextScroller = new TextScroller (Scroller::kBuildNoSlider, Scroller::kBuildDefaultSlider);
			SetOwnedView (fTextScroller);
			fTextScroller->SetTextEdit (fTextEdit);	
			fTextEdit->SetBorder (kZeroPoint);
			fTextEdit->SetMargin (kZeroPoint);
		}
	}
	Ensure (hasSBar == fHasHorizontalSBar);
}

String	TextEditItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "TextEdit.hh";
	static	const	String kScrollerHeaderFileName = "ScrollableText.hh";
	
	if ((GetHasVerticalSBar ()) or (GetHasHorizontalSBar ())) {
		return (kScrollerHeaderFileName);
	}
	return (kHeaderFileName);
}

void	TextEditItem::SetTextEdit (AbstractTextEdit* text)
{
	if (fTextEdit != Nil) {
		RemoveFocus (fTextEdit);
		if (fTextScroller == Nil) {
			SetOwnedView (Nil);
		}
		else {
			fTextScroller->SetTextEdit (Nil);
		}
		delete fTextEdit;
	}
	
	fTextEdit = text;
	
	if (fTextScroller == Nil) {
		SetOwnedView (fTextEdit);
	}
	else {
		fTextScroller->SetTextEdit (fTextEdit);
	}
	
	if (fTextEdit != Nil) {
		AddFocus (fTextEdit);
		if (ItemPallet::GetEditMode ()) {
			GetTextEdit ().SetEditMode (AbstractTextEdit::eDisplayOnly);
		}
		else {
			GetTextEdit ().SetEditMode (AbstractTextEdit::eEditable);
		}
		
		/*
			We set our owned view to be the TextScroller, but for code generation we really want it to be the TextEdit itself.
			So we need to manually set up the proper defaults
		 */
		SetDefaultsFromView (GetTextEdit ());
		
		if (fTextScroller != Nil) {
			fTextEdit->SetBorder (kZeroPoint);
			fTextEdit->SetMargin (kZeroPoint);
		}
	}
}

AbstractTextEdit&	TextEditItem::GetTextEdit () const
{
	RequireNotNil (fTextEdit);
	return (*fTextEdit);
}

Point	TextEditItem::CalcDefaultSize_ (const Point& defaultSize) const
{
GetTextEdit ().SetBorder (GetItemBorder(), eNoUpdate);
GetTextEdit ().SetMargin (GetItemMargin(), eNoUpdate);
	Point	oldSize = GetTextEdit ().GetSize ();
	Point newSize = GetTextEdit ().CalcDefaultSize (defaultSize);
	newSize = Max (Min (newSize, GetMaxSize ()), GetMinSize ());
	if ((oldSize.GetH () != 0) and (not GetMultiLine())) {
		newSize.SetH (oldSize.GetH ());
	}
GetTextEdit ().SetBorder (kZeroPoint, eNoUpdate);
GetTextEdit ().SetMargin (kZeroPoint, eNoUpdate);
	return (newSize);
}

void	TextEditItem::Layout ()
{
	ViewItem::Layout ();
	if (fTextScroller != Nil) {
		GetTextEdit ().SetSize (fTextScroller->GetSize ());
	}
}

void	TextEditItem::DoSetupMenus ()
{
	if (ItemPallet::GetEditMode () and GetSelected ()) {
		EnableCommand (eJustifyLeft);
		EnableCommand (eJustifyCenter);
		EnableCommand (eJustifyRight);
//		EnableCommand (eJustifyFull);
	}
	ViewItem::DoSetupMenus ();
}

	// should be scoped cept for q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
	class	JustifyTextCommand : public Command {
		public:
			JustifyTextCommand (CommandNumber commandNumber, TextEditItem& item);
			
			override	void	DoIt ()
			{
				fItem.SetJustification (fJustification);
				fItem.ApplyCurrentParams ();
				Command::DoIt ();
			}
			
			override	void	UnDoIt ()
			{
				fItem.SetJustification (fOldJustification);
				fItem.ApplyCurrentParams ();
				Command::UnDoIt ();
			}

		private:
			TextEditItem&	fItem;
			AbstractTextView::Justification 	fJustification;
			AbstractTextView::Justification 	fOldJustification;
	};
	JustifyTextCommand::JustifyTextCommand (CommandNumber commandNumber, TextEditItem& item) :
		Command (commandNumber, kUndoable),
		fItem (item),
		fJustification (item.GetJustification ()),
		fOldJustification (item.GetJustification ())
	{
		switch (commandNumber) {
			case eJustifyLeft:
				fJustification = AbstractTextView::eJustLeft;
				break;
				
			case eJustifyCenter:
				fJustification = AbstractTextView::eJustCenter;
				break;
				
			case eJustifyRight:
				fJustification = AbstractTextView::eJustRight;
				break;
				
			case eJustifyFull:
				fJustification = AbstractTextView::eJustFull;
				break;
				
			default:
				RequireNotReached ();
		}
	}

Boolean	TextEditItem::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eJustifyLeft:
		case eJustifyCenter:
		case eJustifyRight:
		case eJustifyFull:
			PostCommand (new JustifyTextCommand (selection.GetCommandNumber (), *this));
			DirtyDocument ();
			return (True);

		default:
			return (ViewItem::DoCommand (selection));
	}
	AssertNotReached (); return (False);
}

void	TextEditItem::EditModeChanged (Boolean newEditMode)
{
	if (ItemPallet::GetEditMode ()) {
		GetTextEdit ().SetEditMode (AbstractTextEdit::eDisplayOnly);
	}
	else {
		GetTextEdit ().SetEditMode (AbstractTextEdit::eEditable);
	}
	ViewItem::EditModeChanged (newEditMode);
}

Boolean	TextEditItem::ItemCanBeEnabled ()
{
	return (True);
}

void	TextEditItem::SetOwnedEnabled (Boolean enabled, Panel::UpdateMode updateMode)
{
	GetTextEdit ().SetEnabled (enabled, updateMode);
}

void	TextEditItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
	SaveableVersion	version;
	Try {
		version = Saveable::ReadVersion (kVersion, from);
	}
	Catch1 (sBadVersion) {
		version = 1;
	}
	
	Boolean	adorned = False;
	if (version == 1) {
		from >> adorned;
	}
	
	if (version <= 4) {
		Boolean	wordWrap;
		AbstractTextView::Justification	justification;
		from >> wordWrap >> justification;
		SetWordWrap (wordWrap);
		SetJustification (justification);
		
		String foo;
		ReadString (from, foo);
		SetText (foo);

		if (version == 1) {
			String	label;
			Font labelFont (kSystemFont);
			
			ReadString (from, label);
			from >> labelFont;
			SetDefaultBorder (Point (1, 1));
			SetDefaultMargin (Point (0, 0));
			if (adorned) {
				SetItemBorder (Point (1, 1));
				SetItemMargin (Point (0, 0));
				SetExtent (GetExtent ().InsetBy (Point (-1, -1)));
			}
			else {
				SetItemBorder (Point (0, 0));
				SetItemMargin (Point (0, 0));
			}
		}
	
		if (version > 1) {
			CollectionSize maxLength;
			Boolean		  multiLine;
			from >> maxLength >> multiLine;
			if (version == 2) {
				Point	border, margin;
				from >> border >> margin;
				SetItemBorder (border);
				SetItemMargin (margin);
			}
			
			if (maxLength == kMaxCollectionSize) {
				maxLength = AbstractTextEdit::kAnyLength;
			}
	
			SetMaxLength (maxLength);
			SetMultiLine (multiLine);
		}
	}
	
	if (version > 3) {
		Boolean v, h;
		from >> v >> h;
		SetHasVerticalSBar (v);
		SetHasHorizontalSBar (h);
	}
	if (version >= 5) {
		ForEach (TextEditParamPtr, it, fTEParams) {
			TextEditParam* param = it.Current ();
			delete param;
		}
		fTEParams.RemoveAll ();

		CollectionSize paramCount;
		from >> paramCount;
		for (int count = 1; count <= paramCount; count++) {
			TextEditParam*	newParams = new TextEditParam (eEnglish, eMacUI, GetTextEdit ());
			char c;
			from >> c;
			Require (c == '{');
			newParams->DoRead (from);
			from >> c;
			Require (c == '}');
			fTEParams.Append (newParams);	
		}
	}
}

void	TextEditItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
	Saveable::WriteVersion (kVersion, to, tabCount);

	to << newline;
	to << tab (tabCount) << GetHasVerticalSBar () << GetHasHorizontalSBar () << newline;

	CollectionSize	paramCount = fTEParams.GetLength ();
	to << tab (tabCount) << paramCount << newline;
	ForEach (TextEditParamPtr, it, fTEParams) {
		to << tab (tabCount) << '{' << newline;
		it.Current ()->DoWrite (to, tabCount+1);
		to << tab (tabCount) << '}' << newline;
	}
}

String	TextEditItem::GetTextScrollerName () const
{
	return (GetFieldName () + "Scroller");
}

String	TextEditItem::GetSubviewFieldName () const
{
	if (GetHasVerticalSBar () or GetHasHorizontalSBar ()) {
		return (GetTextScrollerName ());
	}
	return (GetFieldName ());
}

void	TextEditItem::WriteDeclaration (class ostream& to, int tabCount)
{
	ViewItem::WriteDeclaration (to, tabCount);
	if (GetHasVerticalSBar () or GetHasHorizontalSBar ()) {
		DeclareField (to, tabCount, "TextScroller", GetTextScrollerName ());
	}
}

void	TextEditItem::WriteDestructor (class ostream& to, int tabCount, CommandNumber gui)
{
	ViewItem::WriteDestructor (to, tabCount, gui);
	if (GetHasVerticalSBar () or GetHasHorizontalSBar ()) {
		to << tab (tabCount+1) << GetTextScrollerName () << ".SetTextEdit (Nil);" << newline;
	}
}

void	TextEditItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
	
	if (GetHasVerticalSBar () or GetHasHorizontalSBar ()) {
		to << tab (tabCount) << GetTextScrollerName () << "(";
		if (GetHasVerticalSBar ()) {
			to << "Scroller::kBuildDefaultSlider, ";
		}
		else {
			to << "Scroller::kBuildNoSlider, ";
		}
		if (GetHasHorizontalSBar ()) {
			to << "Scroller::kBuildDefaultSlider";
		}
		else {
			to << "Scroller::kBuildNoSlider";
		}
		
		to << ");" << newline;
	}
}

void	TextEditItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
	to << tab (tabCount) << GetFieldName () << ".SetController (this);" << newline;
	
	TextEditParam* params = FindTEParams (language, gui);
	if (params != Nil) {
		if (params->fText != params->fDefaultText) {
			to << tab (tabCount) << GetFieldName () << ".SetText (" << quote << params->fText << quote << ");" << newline;
		}
		if (params->fWordWrap != params->fDefaultWordWrap) {
			to << tab (tabCount) << GetFieldName () << ".SetWordWrap (" << GetBooleanAsText (params->fWordWrap) << ");" << newline;
		}
		if (params->fJustification != params->fDefaultJustification) {
			to << tab (tabCount) << GetFieldName () << ".SetJustification (" << GetJustificationAsText (params->fJustification) << ");" << newline;
		}
		if (params->fRequireText != params->fDefaultRequireText) {
			to << tab (tabCount) << GetFieldName () << ".SetTextRequired (" << GetBooleanAsText (params->fRequireText) << ");" << newline;
		}
		if (params->fMaxLength != params->fDefaultMaxLength) {
			to << tab (tabCount) << GetFieldName () << ".SetMaxLength (" << params->fMaxLength << ");" << newline;
		}
		if (params->fMultiLine != params->fDefaultMultiLine) {
			to << tab (tabCount) << GetFieldName () << ".SetMultiLine (" << GetBooleanAsText (params->fMultiLine) << ");" << newline;
		}
	}
	
	if (GetHasVerticalSBar () or GetHasHorizontalSBar ()) {
		to << tab (tabCount) << GetFieldName () << ".SetBorder (kZeroPoint, eNoUpdate);" << newline;
		to << tab (tabCount) << GetFieldName () << ".SetMargin (kZeroPoint, eNoUpdate);" << newline;
		to << tab (tabCount+1) << GetTextScrollerName () << ".SetTextEdit (&" << GetFieldName () << ");" << newline;
	}
}

void	TextEditItem::WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteCustomization (to, tabCount, language, gui);
	if ((language != GetBaseLanguage ()) or (gui != GetBaseGUI ())) {
		TextEditParam*	baseParams 	  = FindTEParams (GetBaseLanguage (), GetBaseGUI ());
		TextEditParam*	currentParams = FindTEParams (language, gui);
		
		AssertNotNil (baseParams);
		if (currentParams != Nil) {
			if (currentParams->fText != baseParams->fText) {
				to << tab (tabCount) << GetFieldName () << kFieldAccessor;
				to << "SetText (" << quote << currentParams->fText << quote << ");" << newline;
			}
			if (currentParams->fJustification != baseParams->fJustification) {
				to << tab (tabCount) << GetFieldName () << ".SetJustification (" << GetJustificationAsText (currentParams->fJustification) << ");" << newline;
			}
			if (currentParams->fWordWrap != baseParams->fWordWrap) {
				to << tab (tabCount) << GetFieldName () << ".SetWordWrap (" << GetBooleanAsText (currentParams->fWordWrap) << ");" << newline;
			}
			if (currentParams->fRequireText != baseParams->fRequireText) {
				to << tab (tabCount) << GetFieldName () << ".SetTextRequired (" << GetBooleanAsText (currentParams->fRequireText) << ");" << newline;
			}
			if (currentParams->fMaxLength != baseParams->fMaxLength) {
				to << tab (tabCount) << GetFieldName () << ".SetMaxLength (" << currentParams->fMaxLength << ");" << newline;
			}
			if (currentParams->fWordWrap != baseParams->fWordWrap) {
				to << tab (tabCount) << GetFieldName () << ".SetWordWrap (" << GetBooleanAsText (currentParams->fWordWrap) << ");" << newline;
			}
		}
	}
}

Boolean	TextEditItem::IsButton ()
{
	return (False);
}

Boolean	TextEditItem::IsSlider ()
{
	return (False);
}

Boolean	TextEditItem::IsText ()
{
	return (True);
}

Boolean	TextEditItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (True);
}

CollectionSize	TextEditItem::GetMaxLength () const
{
	return (GetCurrentTEParams ().fMaxLength);
}

void	TextEditItem::SetMaxLength (CollectionSize maxLength)
{
	GetCurrentTEParams ().fMaxLength = maxLength;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (TextEditParamPtr, it, fTEParams) {
			TextEditParam*	current = it.Current ();
			current->fMaxLength = maxLength;
		}
	}
}
		
Boolean	TextEditItem::GetMultiLine () const
{
	return (GetCurrentTEParams ().fMultiLine);
}

void	TextEditItem::SetMultiLine (Boolean multiLine)
{
	GetCurrentTEParams ().fMultiLine = multiLine;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (TextEditParamPtr, it, fTEParams) {
			TextEditParam*	current = it.Current ();
			current->fMultiLine = multiLine;
		}
	}
}

Boolean	TextEditItem::GetTextRequired () const
{
	return (GetCurrentTEParams ().fRequireText);
}

void	TextEditItem::SetTextRequired (Boolean required)
{
	GetCurrentTEParams ().fRequireText = required;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (TextEditParamPtr, it, fTEParams) {
			TextEditParam*	current = it.Current ();
			current->fRequireText = required;
		}
	}
}

Boolean	TextEditItem::GetWordWrap () const
{
	return (GetCurrentTEParams ().fWordWrap);
}

void	TextEditItem::SetWordWrap (Boolean wordWrap)
{
	GetCurrentTEParams ().fWordWrap = wordWrap;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (TextEditParamPtr, it, fTEParams) {
			TextEditParam*	current = it.Current ();
			current->fWordWrap = wordWrap;
		}
	}
}

String	TextEditItem::GetText () const
{
	return (GetCurrentTEParams ().fText);
}

void	TextEditItem::SetText (const String& text)
{
	GetCurrentTEParams ().fText = text;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (TextEditParamPtr, it, fTEParams) {
			TextEditParam*	current = it.Current ();
			current->fText = text;
		}
	}
}

AbstractTextView::Justification	TextEditItem::GetJustification () const
{
	return (GetCurrentTEParams ().fJustification);
}

void	TextEditItem::SetJustification (AbstractTextView::Justification just)
{
	GetCurrentTEParams ().fJustification = just;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (TextEditParamPtr, it, fTEParams) {
			TextEditParam*	current = it.Current ();
			current->fJustification = just;
		}
	}
}

TextEditParam*	TextEditItem::FindTEParams (CommandNumber language, CommandNumber gui) const
{
	ForEach (TextEditParamPtr, it, fTEParams) {
		TextEditParam*	current = it.Current ();
		
		if (((current->fLanguage == language) or (language == CommandHandler::eNoCommand)) and 
			((current->fGUI == gui) or (gui == CommandHandler::eNoCommand))) {
			
			return (current);
		}
	}
	return (Nil);
}

TextEditParam&	TextEditItem::GetCurrentTEParams () const
{
	TextEditParam*	param = FindTEParams (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	
	if (param == Nil) {
		ViewItem*	This = (ViewItem*)this;
		This->AddParam (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI (), GetBaseLanguage (), GetBaseGUI ());
		param = FindTEParams (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	}
	EnsureNotNil (param);
	return (*param);
}

void	TextEditItem::AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI)
{
	ViewItem::AddParam (language, gui, oldLanguage, oldGUI);
	TextEditParam*	param = new TextEditParam (language, gui, GetTextEdit ());
	TextEditParam* oldParams = FindTEParams (oldLanguage, oldGUI);
	if (oldParams != Nil) {
		*param = *oldParams;
		param->fLanguage = language;
		param->fGUI = gui;
	}
	
	fTEParams.Append (param);

ItemParams* itemParams = FindParams (language, gui);
AssertNotNil (itemParams);
if (gui == eMacUI) {
itemParams->fDefaultBorder = Point (1, 1);
itemParams->fDefaultMargin = Point (0, 0);
}
else {
itemParams->fDefaultBorder = Point (2, 2);
itemParams->fDefaultMargin = Point (2, 2);
}

}

void	TextEditItem::RemoveParam (CommandNumber language, CommandNumber gui)
{
	ViewItem::RemoveParam (language, gui);
	TextEditParam* param = FindTEParams (language, gui);
	if (param != Nil) {
		fTEParams.Remove (param);
	}
}

void	TextEditItem::ApplyParams (CommandNumber language, CommandNumber gui)
{
	ViewItem::ApplyParams (language, gui);
	TextEditParam* param = FindTEParams (language, gui);
	if (param != Nil) {
		GetTextEdit ().SetWordWrap (param->fWordWrap);
		GetTextEdit ().SetTextRequired (param->fRequireText);
		GetTextEdit ().SetMaxLength (param->fMaxLength);
		GetTextEdit ().SetText (param->fText);
		GetTextEdit ().SetMultiLine (param->fMultiLine);
		GetTextEdit ().SetJustification (param->fJustification);
	}
}

void	TextEditItem::SetItemInfo ()
{
	TextEditInfo info = TextEditInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetTextEditInfoCommand (*this, info));
		DirtyDocument ();
	}
}


SetTextEditInfoCommand::SetTextEditInfoCommand (TextEditItem& item, class TextEditInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewText (info.GetTextField ().GetText ()),
	fOldText (item.GetText ()),
	fNewWordWrap (info.GetWordWrapField ().GetOn ()),
	fOldWordWrap (item.GetWordWrap ()),
	fNewRequireText (info.GetRequireTextField ().GetOn ()),
	fOldRequireText (item.GetTextRequired ()),
	fNewMaxLength (info.GetMaxLengthField ().GetValue ()),
	fOldMaxLength (item.GetMaxLength ()),
	fNewMultiLine (info.GetMultiLineField ().GetOn ()),
	fOldMultiLine (item.GetMultiLine ()),
	fNewHasVerticalSBar (info.GetVSBarField ().GetOn ()),
	fOldHasVerticalSBar (item.GetHasVerticalSBar ()),
	fNewHasHorizontalSBar (info.GetHSBarField ().GetOn ()),
	fOldHasHorizontalSBar (item.GetHasHorizontalSBar ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}
		
void	SetTextEditInfoCommand::DoIt ()
{
	fItem.SetWordWrap (fNewWordWrap);
	fItem.SetTextRequired (fNewRequireText);
	fItem.SetMultiLine (fNewMultiLine);
	fItem.SetMaxLength (fNewMaxLength);
	fItem.SetText (fNewText);
	fItem.SetHasVerticalSBar (fNewHasVerticalSBar);
	fItem.SetHasHorizontalSBar (fNewHasHorizontalSBar);
	
	fItemInfoCommand->DoIt ();
	Command::DoIt ();
}

void	SetTextEditInfoCommand::UnDoIt ()
{
	fItem.SetWordWrap (fOldWordWrap);
	fItem.SetTextRequired (fOldRequireText);
	fItem.SetMultiLine (fOldMultiLine);
	fItem.SetMaxLength (fOldMaxLength);
	fItem.SetText (fOldText);
	fItem.SetHasVerticalSBar (fOldHasVerticalSBar);
	fItem.SetHasHorizontalSBar (fOldHasHorizontalSBar);
	
	fItemInfoCommand->UnDoIt ();
	Command::UnDoIt ();
}
