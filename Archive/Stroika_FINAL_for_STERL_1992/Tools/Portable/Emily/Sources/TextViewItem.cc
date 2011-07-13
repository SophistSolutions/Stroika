/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextViewItem.cc,v 1.8 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextViewItem.cc,v $
 *		Revision 1.8  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.13  1992/03/06  21:52:58  sterling
 *		motif
 *
 *
 *
 */





#include	"StreamUtils.hh"
#include	"Dialog.hh"
#include	"TextView.hh"
#include	"CheckBox.hh"

#include	"CommandNumbers.hh"
#include	"TextViewItem.hh"
#include	"GroupItem.hh"
#include	"ItemPallet.hh"
#include	"ViewItemInfo.hh"
#include	"TextViewInfo.hh"
#include	"EmilyWindow.hh"



#if		!qRealTemplatesAvailable
	Implement (Iterator, TVParmPtr);
	Implement (Collection, TVParmPtr);
	Implement (AbSequence, TVParmPtr);
	Implement (Array, TVParmPtr);
	Implement (Sequence_Array, TVParmPtr);
	Implement (Sequence, TVParmPtr);
#endif

TextViewParam::TextViewParam (CommandNumber language, CommandNumber gui) :
	Saveable (1),
	fLanguage (language),
	fGUI (gui),
	fText (kEmptyString),
	fJustification (AbstractTextView::eJustLeft),
	fWordWrap (False)
{
}

void	TextViewParam::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	from >> fLanguage >> fGUI;
	fLanguage += eFirstLanguage;
	fGUI += eFirstGUI;
	ReadString (from, fText);
	from >> fJustification >> fWordWrap;
}

void	TextViewParam::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);
	to << newline;
	to << tab (tabCount) << (fLanguage - eFirstLanguage) << ' ' << (fGUI - eFirstGUI) << newline;
	to << tab (tabCount);
	WriteString (to, fText);
	to << fJustification << fWordWrap;
	to << newline;
}



/*
 ********************************************************************************
 ****************************** TextViewItemType ******************************
 ********************************************************************************
 */
TextViewItemType::TextViewItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildTextView, "TextView", (ItemBuilderProc)&TextViewItemBuilder)
#else
	ItemType (eBuildTextView, "TextView", &TextViewItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
TextViewItemType&	TextViewItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	TextViewItemType::TextViewItemBuilder ()
{
	return (new TextViewItem (Get ()));
}

TextViewItemType*	TextViewItemType::sThis = Nil;





/*
 ********************************************************************************
 ****************************** TextViewItem **********************************
 ********************************************************************************
 */
TextViewItem::TextViewItem (ItemType& type) :
	ViewItem (type),
	fTextEdit (Nil),
	fTVParams ()
{
	SetTextView (new TextEdit ());
	SetMaxVersion (3);
}

TextViewItem::~TextViewItem ()
{
	ForEach (TextViewParamPtr, it, fTVParams) {
		TextViewParam* param = it.Current ();
		delete param;
	}
}

String	TextViewItem::GetHeaderFileName ()
{
	static	const	String kTextViewFileName = "TextView.hh";
	static	const	String kTextEditFileName = "TextEdit.hh";
	
	if (GetWordWrap ()) {
		return (kTextEditFileName);
	}
	return (kTextViewFileName);
}

void	TextViewItem::SetTextView (TextEdit* staticText)
{
	if (fTextEdit != Nil) {
		SetOwnedView (Nil);
		fTextEdit = Nil;
	}
	fTextEdit = staticText;
	if (fTextEdit != Nil) {
		fTextEdit->SetEditMode (AbstractTextEdit::eDisplayOnly);
		fTextEdit->SetWordWrap (False);
		fTextEdit->SetBorder (kZeroPoint);
		fTextEdit->SetMargin (kZeroPoint);
		fTextEdit->SetFont (Nil);
		
		SetOwnedView (fTextEdit);

		SetJustification (AbstractTextView::eJustLeft);
		SetText ("Text View");
		
		ApplyCurrentParams ();
	}
}

Boolean	TextViewItem::GetWordWrap () const
{
	return (GetCurrentTVParams ().fWordWrap);
}

void	TextViewItem::SetWordWrap (Boolean wordWrap)
{
	if (wordWrap != GetWordWrap ()) {
		SetWordWrap_ (wordWrap);
	}
	Ensure (wordWrap == GetWordWrap ());
}

void	TextViewItem::SetWordWrap_ (Boolean wordWrap)
{
	GetCurrentTVParams ().fWordWrap = wordWrap;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (TextViewParamPtr, it, fTVParams) {
			TextViewParam*	current = it.Current ();
			current->fWordWrap = wordWrap;
		}
	}
}

AbstractTextView::Justification	TextViewItem::GetJustification () const
{
	return (GetCurrentTVParams ().fJustification);
}

void	TextViewItem::SetJustification (AbstractTextView::Justification just)
{
	if (just != GetJustification ()) {
		SetJustification_ (just);
	}
	Ensure (just == GetJustification ());
}

void	TextViewItem::SetJustification_ (AbstractTextView::Justification just)
{
	GetCurrentTVParams ().fJustification = just;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (TextViewParamPtr, it, fTVParams) {
			TextViewParam*	current = it.Current ();
			current->fJustification = just;
		}
	}
}

String	TextViewItem::GetText () const
{
	return (GetCurrentTVParams ().fText);
}

void	TextViewItem::SetText (const String& text)
{
	if (text != GetText ()) {
		SetText_ (text);
	}
	Ensure (text == GetText ());
}

void	TextViewItem::SetText_ (const String& text)
{
	GetCurrentTVParams ().fText = text;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (TextViewParamPtr, it, fTVParams) {
			TextViewParam*	current = it.Current ();
			current->fText = text;
		}
	}
}

AbstractTextEdit&	TextViewItem::GetTextView () const
{
	RequireNotNil (fTextEdit);
	return (*fTextEdit);
}

void	TextViewItem::DoSetupMenus ()
{
	if (ItemPallet::GetEditMode () and GetSelected ()) {
		EnableCommand (eJustifyLeft);
		EnableCommand (eJustifyCenter);
		EnableCommand (eJustifyRight);
//		EnableCommand (eJustifyFull);
	}
	ViewItem::DoSetupMenus ();
}

class	JustifyTextViewCommand : public Command {
	public:
		JustifyTextViewCommand (CommandNumber commandNumber, TextViewItem& item);

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
		TextViewItem&						fItem;
		AbstractTextView::Justification 	fJustification;
		AbstractTextView::Justification 	fOldJustification;
};
		JustifyTextViewCommand::JustifyTextViewCommand (CommandNumber commandNumber, TextViewItem& item) :
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


Boolean	TextViewItem::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eJustifyLeft:
		case eJustifyCenter:
		case eJustifyRight:
		case eJustifyFull:
			PostCommand (new JustifyTextViewCommand (selection.GetCommandNumber (), *this));
			DirtyDocument ();
			return (True);

		default:
			return (ViewItem::DoCommand (selection));
	}
	AssertNotReached (); return (False);
}

void	TextViewItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);

	if (GetVersion () <= 2) {
		Boolean	wordWrap;
		from >> wordWrap;
		SetWordWrap (wordWrap);

		AbstractTextView::Justification	justification;
		from >> justification;
		SetJustification (justification);
		String foo;
		ReadString (from, foo);
		SetText (foo);
	}
	if (GetVersion () >= 3) {
		ForEach (TextViewParamPtr, it, fTVParams) {
			TextViewParam* param = it.Current ();
			delete param;
		}
		fTVParams.RemoveAll ();

		CollectionSize paramCount;
		from >> paramCount;
		for (int count = 1; count <= paramCount; count++) {
			TextViewParam*	newParams = new TextViewParam (eEnglish, eMacUI);
			char c;
			from >> c;
			Require (c == '{');
			newParams->DoRead (from);
			from >> c;
			Require (c == '}');
			fTVParams.Append (newParams);	
		}
	}
}

void	TextViewItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
	CollectionSize	paramCount = fTVParams.GetLength ();
	to << tab (tabCount) << paramCount << newline;
	ForEach (TextViewParamPtr, it, fTVParams) {
		to << tab (tabCount) << '{' << newline;
		it.Current ()->DoWrite (to, tabCount+1);
		to << tab (tabCount) << '}' << newline;
	}
}

void	TextViewItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	TextViewItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
	if (GetWordWrap ()) {
		to << tab (tabCount) << GetFieldName () << ".SetEditMode (AbstractTextEdit::eDisplayOnly);" << newline;
		if (GetItemBorder () == kZeroPoint) {
			to << tab (tabCount) << GetFieldName () << ".SetBorder (0, 0, eNoUpdate);" << newline;
		}
		if (GetItemMargin () == kZeroPoint) {
			to << tab (tabCount) << GetFieldName () << ".SetMargin (0, 0, eNoUpdate);" << newline;
		}
	}

	TextViewParam* params = FindTVParams (language, gui);
	if (params != Nil) {
		if (params->fText != kEmptyString) {
			to << tab (tabCount) << GetFieldName () << ".SetText (" << quote << params->fText << quote << ");" << newline;
		}
		if (params->fJustification != AbstractTextView::eJustLeft) {
			to << tab (tabCount) << GetFieldName () << ".SetJustification (";
			to << GetJustificationAsText (params->fJustification) << ");" << newline;
		}
	}
}

void	TextViewItem::WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteCustomization (to, tabCount, language, gui);
	if ((language != GetBaseLanguage ()) or (gui != GetBaseGUI ())) {
		TextViewParam*	baseParams 	  = FindTVParams (GetBaseLanguage (), GetBaseGUI ());
		TextViewParam*	currentParams = FindTVParams (language, gui);
		
		AssertNotNil (baseParams);
		if (currentParams != Nil) {
			if (currentParams->fText != baseParams->fText) {
				to << tab (tabCount) << GetFieldName () << kFieldAccessor;
				to << "SetText (" << quote << currentParams->fText << quote << ");" << newline;
			}
			if (currentParams->fJustification != baseParams->fJustification) {
				to << tab (tabCount) << GetFieldName () << kFieldAccessor;
				to << "SetJustification (" << GetJustificationAsText (currentParams->fJustification) << ");" << newline;
			}
			if (currentParams->fWordWrap != baseParams->fWordWrap) {
				to << tab (tabCount) << GetFieldName () << ".SetWordWrap (" << GetBooleanAsText (currentParams->fWordWrap) << ");" << newline;
			}
		}
	}
}

Boolean	TextViewItem::IsButton ()
{
	return (False);
}

Boolean	TextViewItem::IsSlider ()
{
	return (False);
}

Boolean	TextViewItem::IsText ()
{
	return (False);
}

Boolean	TextViewItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (False);
}

Boolean	TextViewItem::ItemCanBeEnabled ()
{
	return (False);
}

TextViewParam*	TextViewItem::FindTVParams (CommandNumber language, CommandNumber gui) const
{
	ForEach (TextViewParamPtr, it, fTVParams) {
		TextViewParam*	current = it.Current ();
		
		if (((current->fLanguage == language) or (language == CommandHandler::eNoCommand)) and 
			((current->fGUI == gui) or (gui == CommandHandler::eNoCommand))) {
			
			return (current);
		}
	}
	return (Nil);
}

TextViewParam&	TextViewItem::GetCurrentTVParams () const
{
	TextViewParam*	param = FindTVParams (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	
	if (param == Nil) {
		ViewItem*	This = (ViewItem*)this;
		This->AddParam (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI (), GetBaseLanguage (), GetBaseGUI ());
		param = FindTVParams (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	}
	EnsureNotNil (param);
	return (*param);
}

void	TextViewItem::AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI)
{
	ViewItem::AddParam (language, gui, oldLanguage, oldGUI);
	TextViewParam*	param = new TextViewParam (language, gui);
	TextViewParam* oldParams = FindTVParams (oldLanguage, oldGUI);
	if (oldParams != Nil) {
		*param = *oldParams;
		param->fLanguage = language;
		param->fGUI = gui;
	}
	
	fTVParams.Append (param);
}

void	TextViewItem::RemoveParam (CommandNumber language, CommandNumber gui)
{
	ViewItem::RemoveParam (language, gui);
	TextViewParam* param = FindTVParams (language, gui);
	if (param != Nil) {
		fTVParams.Remove (param);
	}
}

void	TextViewItem::ApplyParams (CommandNumber language, CommandNumber gui)
{
	ViewItem::ApplyParams (language, gui);
	TextViewParam* param = FindTVParams (language, gui);
	if (param != Nil) {
		GetTextView ().SetText (param->fText);
		GetTextView ().SetJustification (param->fJustification);
		GetTextView ().SetWordWrap (param->fWordWrap);
		if (param->fWordWrap) {
			if (GetFieldClass () == "TextView") {
				SetFieldClass ("TextEdit");
			}
		}
		else {
			if (GetFieldClass () == "TextEdit") {
				SetFieldClass ("TextView");
			}
		}
	}
}

void	TextViewItem::SetItemInfo ()
{
	TextViewInfo info = TextViewInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetTextViewInfoCommand (*this, info));
		DirtyDocument ();
	}
}

SetTextViewInfoCommand::SetTextViewInfoCommand (TextViewItem& item, class TextViewInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewText (info.GetTextField ().GetText ()),
	fOldText (item.GetText ()),
	fNewWordWrap (info.GetWordWrapField ().GetOn ()),
	fOldWordWrap (item.GetWordWrap ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}
		
void	SetTextViewInfoCommand::DoIt ()
{
	fItem.SetText (fNewText);
	fItem.SetWordWrap (fNewWordWrap);
	fItemInfoCommand->DoIt ();
	
	Command::DoIt ();
}

void	SetTextViewInfoCommand::UnDoIt ()
{
	fItem.SetText (fOldText);
	fItem.SetWordWrap (fOldWordWrap);
	fItemInfoCommand->UnDoIt ();
	Command::UnDoIt ();
}
