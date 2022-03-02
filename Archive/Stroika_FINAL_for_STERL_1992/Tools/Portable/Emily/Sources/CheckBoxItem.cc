/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CheckBoxItem.cc,v 1.8 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: CheckBoxItem.cc,v $
 *		Revision 1.8  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.11  1992/02/15  07:47:51  lewis
 *		Fix missing include.
 *
 *		Revision 1.8  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"CheckBox.hh"

#include	"CommandNumbers.hh"
#include	"CheckBoxInfo.hh"
#include	"EmilyWindow.hh"
#include	"ViewItemInfo.hh"

#include	"CheckBoxItem.hh"


#if		!qRealTemplatesAvailable
	Implement (Iterator, CBParmPtr);
	Implement (Collection, CBParmPtr);
	Implement (AbSequence, CBParmPtr);
	Implement (Array, CBParmPtr);
	Implement (Sequence_Array, CBParmPtr);
	Implement (Sequence, CBParmPtr);
#endif



CheckBoxParam::CheckBoxParam (CommandNumber language, CommandNumber gui) :
	Saveable (1),
	fLanguage (language),
	fGUI (gui),
	fLabel (kEmptyString)
{
}

void	CheckBoxParam::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	from >> fLanguage >> fGUI;
	fLanguage += eFirstLanguage;
	fGUI += eFirstGUI;
	ReadString (from, fLabel);
}

void	CheckBoxParam::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);
	to << newline;
	to << tab (tabCount) << (fLanguage - eFirstLanguage) << ' ' << (fGUI - eFirstGUI) << newline;
	to << tab (tabCount);
	WriteString (to, fLabel);
	to << newline;
}


/*
 ********************************************************************************
 ****************************** CheckBoxItemType ********************************
 ********************************************************************************
 */
CheckBoxItemType::CheckBoxItemType () :
#if		qHP_BadTypedefForStaticProcs
	ItemType (eBuildButton, "CheckBox", (ItemBuilderProc) &CheckBoxItemBuilder)
#else
	ItemType (eBuildButton, "CheckBox", &CheckBoxItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
CheckBoxItemType&	CheckBoxItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	CheckBoxItemType::CheckBoxItemBuilder ()
{
	RequireNotNil (sThis);
	return (new CheckBoxItem (Get ()));
}

CheckBoxItemType*	CheckBoxItemType::sThis = Nil;


static	AbstractCheckBox*	BuildCheckBox (CommandNumber gui, const String& label)
{
	if (gui == eMacUI) {
		return (new CheckBox_MacUI (label));
	}
	else if (gui == eMotifUI) {
		return (new CheckBox_MotifUI (label));
	}
	else if (gui == eWindowsGUI) {
		return (new CheckBox_WinUI (label));
	}
	AssertNotReached ();	return (Nil);
}

/*
 ********************************************************************************
 ****************************** CheckBoxItem **********************************
 ********************************************************************************
 */
CheckBoxItem::CheckBoxItem (ItemType& type) :
	ButtonItem (type),
	fCheckBox (Nil),
	fLabels ()
{
	SetButton (fCheckBox = BuildCheckBox (EmilyWindow::GetGUI (), "Check Box"));
	SetLabel (fCheckBox->GetLabel ());
	SetMaxVersion (5);
}

CheckBoxItem::~CheckBoxItem ()
{
	ForEach (CheckBoxParamPtr, it, fLabels) {
		CheckBoxParam* param = it.Current ();
		delete param;
	}
}

void	CheckBoxItem::GUIChanged (CommandNumber /*oldGUI*/, CommandNumber newGUI)
{
	SetButton (fCheckBox = BuildCheckBox (newGUI, fCheckBox->GetLabel ()));
	ApplyCurrentParams ();
}

String	CheckBoxItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "CheckBox.hh";
	return (kHeaderFileName);
}

AbstractCheckBox&	CheckBoxItem::GetCheckBox () const
{
	RequireNotNil (fCheckBox);
	return (*fCheckBox);
}

void	CheckBoxItem::OldParamReadHack (class istream& from)
{
	String foo;
	ReadString (from, foo);
	SetLabel (foo);
}

void	CheckBoxItem::DoRead_ (class istream& from)
{
	ButtonItem::DoRead_ (from);

	if ((GetVersion () <= 2) or (GetVersion () == 4)) {
		String foo;
		ReadString (from, foo);
		SetLabel (foo);
	}
	if (GetVersion () >= 5) {
		ForEach (CheckBoxParamPtr, it, fLabels) {
			CheckBoxParam* param = it.Current ();
			delete param;
		}
		fLabels.RemoveAll ();

		CollectionSize paramCount;
		from >> paramCount;
		for (int count = 1; count <= paramCount; count++) {
			CheckBoxParam*	newParams = new CheckBoxParam (eEnglish, eMacUI);
			char c;
			from >> c;
			Require (c == '{');
			newParams->DoRead (from);
			from >> c;
			Require (c == '}');
			fLabels.Append (newParams);	
		}
	}
}

void	CheckBoxItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ButtonItem::DoWrite_ (to, tabCount);
	CollectionSize	paramCount = fLabels.GetLength ();
	to << tab (tabCount) << paramCount << newline;
	ForEach (CheckBoxParamPtr, it, fLabels) {
		to << tab (tabCount) << '{' << newline;
		it.Current ()->DoWrite (to, tabCount+1);
		to << tab (tabCount) << '}' << newline;
	}
}

void	CheckBoxItem::WriteBuilder (class ostream& to, int tabCount)
{
	ButtonItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	CheckBoxItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	CheckBoxParam*	currentParams = FindLabel (GetBaseLanguage (), gui);
	AssertNotNil (currentParams);
	to << tab (tabCount) << GetFieldName () << ".SetLabel (" << quote << currentParams->fLabel << quote << ", eNoUpdate);" << newline;
	ButtonItem::WriteParameters (to, tabCount, language, gui);
	to << tab (tabCount) << GetFieldName () << ".SetController (this);" << newline;
}

void	CheckBoxItem::WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteCustomization (to, tabCount, language, gui);
	if ((language != GetBaseLanguage ()) or (gui != GetBaseGUI ())) {
		CheckBoxParam*	baseParams 	  = FindLabel (GetBaseLanguage (), GetBaseGUI ());
		CheckBoxParam*	currentParams = FindLabel (language, gui);
		
		AssertNotNil (baseParams);
		if (currentParams != Nil) {
			if (currentParams->fLabel != baseParams->fLabel) {
				to << tab (tabCount) << GetFieldName () << kFieldAccessor;
				to << "SetLabel (" << quote << currentParams->fLabel << quote << ");" << newline;
			}
		}
	}
}

String	CheckBoxItem::GetLabel () const
{
	return (GetCurrentLabel ().fLabel);
}

void	CheckBoxItem::SetLabel (const String& label)
{
	GetCurrentLabel ().fLabel = label;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (CheckBoxParamPtr, it, fLabels) {
			CheckBoxParam*	current = it.Current ();
			current->fLabel = label;
		}
	}
}

CheckBoxParam*	CheckBoxItem::FindLabel (CommandNumber language, CommandNumber gui) const
{
	ForEach (CheckBoxParamPtr, it, fLabels) {
		CheckBoxParam*	current = it.Current ();
		
		if (((current->fLanguage == language) or (language == CommandHandler::eNoCommand)) and 
			((current->fGUI == gui) or (gui == CommandHandler::eNoCommand))) {
			
			return (current);
		}
	}
	return (Nil);
}

CheckBoxParam&	CheckBoxItem::GetCurrentLabel () const
{
	CheckBoxParam*	param = FindLabel (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	
	if (param == Nil) {
		ViewItem*	This = (ViewItem*)this;
		This->AddParam (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI (), GetBaseLanguage (), GetBaseGUI ());
		param = FindLabel (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	}
	EnsureNotNil (param);
	return (*param);
}

void	CheckBoxItem::AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI)
{
	ViewItem::AddParam (language, gui, oldLanguage, oldGUI);
	CheckBoxParam*	param = new CheckBoxParam (language, gui);
	CheckBoxParam* oldParams = FindLabel (oldLanguage, oldGUI);
	if (oldParams != Nil) {
		param->fLabel = oldParams->fLabel;
	}
	
	fLabels.Append (param);

ItemParams* itemParams = FindParams (language, gui);
AssertNotNil (itemParams);
if (gui == eMacUI) {
itemParams->fDefaultBorder = Point (0, 0);
itemParams->fDefaultMargin = Point (0, 0);
}
else {
itemParams->fDefaultBorder = Point (2, 2);
itemParams->fDefaultMargin = Point (2, 2);
}
}

void	CheckBoxItem::RemoveParam (CommandNumber language, CommandNumber gui)
{
	ViewItem::RemoveParam (language, gui);
	CheckBoxParam* param = FindLabel (language, gui);
	if (param != Nil) {
		fLabels.Remove (param);
	}
}

void	CheckBoxItem::ApplyParams (CommandNumber language, CommandNumber gui)
{
	ViewItem::ApplyParams (language, gui);
	CheckBoxParam* param = FindLabel (language, gui);
	if (param != Nil) {
		GetCheckBox ().SetLabel (param->fLabel);
	}
}

void	CheckBoxItem::SetItemInfo ()
{
	CheckBoxInfo info = CheckBoxInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetCheckBoxInfoCommand (*this, info));
		DirtyDocument ();
	}
}

SetCheckBoxInfoCommand::SetCheckBoxInfoCommand (CheckBoxItem& item, class CheckBoxInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewLabel (info.GetLabelField ().GetText ()),
	fOldLabel (item.GetLabel ()),
	fNewOn (info.GetOnField ().GetOn ()),
	fOldOn (item.GetCheckBox ().GetOn ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}
		
void	SetCheckBoxInfoCommand::DoIt ()
{
	fItem.SetLabel (fNewLabel);
	fItem.GetCheckBox ().SetOn (fNewOn);
	fItemInfoCommand->DoIt ();
	
	Command::DoIt ();
}

void	SetCheckBoxInfoCommand::UnDoIt ()
{
	fItem.SetLabel (fOldLabel);
	fItem.GetCheckBox ().SetOn (fOldOn);
	fItemInfoCommand->UnDoIt ();
	
	Command::UnDoIt ();
}
