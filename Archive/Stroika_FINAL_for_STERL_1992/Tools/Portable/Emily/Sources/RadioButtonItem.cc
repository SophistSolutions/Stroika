/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/RadioButtonItem.cc,v 1.8 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: RadioButtonItem.cc,v $
 *		Revision 1.8  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.7  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *
 *
 */





#include	"StreamUtils.hh"
#include	"Dialog.hh"
#include	"RadioButton.hh"
#include	"CheckBox.hh"

#include	"CommandNumbers.hh"
#include	"EmilyWindow.hh"
#include	"GroupItem.hh"
#include	"RadioButtonItem.hh"
#include	"ViewItemInfo.hh"

#include	"RadioButtonInfo.hh"



#if		!qRealTemplatesAvailable
	Implement (Iterator, RBParmPtr);
	Implement (Collection, RBParmPtr);
	Implement (AbSequence, RBParmPtr);
	Implement (Array, RBParmPtr);
	Implement (Sequence_Array, RBParmPtr);
	Implement (Sequence, RBParmPtr);
#endif

RadioButtonParam::RadioButtonParam (CommandNumber language, CommandNumber gui) :
	Saveable (1),
	fLanguage (language),
	fGUI (gui),
	fLabel (kEmptyString)
{
}

void	RadioButtonParam::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	from >> fLanguage >> fGUI;
	fLanguage += eFirstLanguage;
	fGUI += eFirstGUI;
	ReadString (from, fLabel);
}

void	RadioButtonParam::DoWrite_ (class ostream& to, int tabCount) const
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
 ****************************** RadioButtonItemType *****************************
 ********************************************************************************
 */
RadioButtonItemType::RadioButtonItemType ():
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildButton, "RadioButton",(ItemBuilderProc) &RadioButtonItemBuilder)
#else
	ItemType (eBuildButton, "RadioButton", &RadioButtonItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
RadioButtonItemType&	RadioButtonItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	RadioButtonItemType::RadioButtonItemBuilder ()
{
	return (new RadioButtonItem (Get ()));
}

RadioButtonItemType*	RadioButtonItemType::sThis = Nil;


static	AbstractRadioButton*	BuildRadioButton (CommandNumber gui, const String& label)
{
	if (gui == eMacUI) {
		return (new RadioButton_MacUI (label));
	}
	else if (gui == eMotifUI) {
		return (new RadioButton_MotifUI (label));
	}
	else if (gui == eWindowsGUI) {
		return (new RadioButton_WinUI (label));
	}
	AssertNotReached ();	return (Nil);
}


/*
 ********************************************************************************
 ****************************** RadioButtonItem *********************************
 ********************************************************************************
 */
RadioButtonItem::RadioButtonItem (ItemType& type):
	ButtonItem (type),
	fRadioButton (Nil),
	fLabels ()
{
	SetButton (fRadioButton = BuildRadioButton (EmilyWindow::GetGUI (), "Radio Button"));
	SetLabel (fRadioButton->GetLabel ());
	SetMaxVersion (5);
}

RadioButtonItem::~RadioButtonItem ()
{
	ForEach (RadioButtonParamPtr, it, fLabels) {
		RadioButtonParam* param = it.Current ();
		delete param;
	}
}

void	RadioButtonItem::GUIChanged (CommandNumber /*oldGUI*/, CommandNumber newGUI)
{
	SetButton (fRadioButton = BuildRadioButton (newGUI, fRadioButton->GetLabel ()));
	ApplyCurrentParams ();
}

String	RadioButtonItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "RadioButton.hh";
	return (kHeaderFileName);
}

AbstractRadioButton&	RadioButtonItem::GetRadioButton () const
{
	RequireNotNil (fRadioButton);
	return (*fRadioButton);
}

void	RadioButtonItem::SetGroup (GroupItem* parent)
{
	if (parent != GetGroup ()) {
		if (GetGroup () != Nil) {
			GetGroup ()->RemoveRadioButton (fRadioButton);
		}
		ButtonItem::SetGroup (parent);
		if (parent != Nil) {
			parent->AddRadioButton (fRadioButton);
		}
	}
}

void	RadioButtonItem::OldParamReadHack (class istream& from)
{
	String foo;
	ReadString (from, foo);
	SetLabel (foo);
}

void	RadioButtonItem::DoRead_ (class istream& from)
{
	ButtonItem::DoRead_ (from);

	if ((GetVersion () <= 2) or (GetVersion () == 4)) {
		String foo;
		ReadString (from, foo);
		SetLabel (foo);
	}
	if (GetVersion () >= 5) {
		ForEach (RadioButtonParamPtr, it, fLabels) {
			RadioButtonParam* param = it.Current ();
			delete param;
		}
		fLabels.RemoveAll ();

		CollectionSize paramCount;
		from >> paramCount;
		for (int count = 1; count <= paramCount; count++) {
			RadioButtonParam*	newParams = new RadioButtonParam (eEnglish, eMacUI);
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

void	RadioButtonItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ButtonItem::DoWrite_ (to, tabCount);
	CollectionSize	paramCount = fLabels.GetLength ();
	to << tab (tabCount) << paramCount << newline;
	ForEach (RadioButtonParamPtr, it, fLabels) {
		to << tab (tabCount) << '{' << newline;
		it.Current ()->DoWrite (to, tabCount+1);
		to << tab (tabCount) << '}' << newline;
	}
}

void	RadioButtonItem::WriteBuilder (class ostream& to, int tabCount)
{
	ButtonItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	RadioButtonItem::WriteDestructor (class ostream& to, int tabCount, CommandNumber gui)
{
	ViewItem::WriteDestructor (to, tabCount, gui);
	to << tab (tabCount) << GetGroup ()->GetRadioBankName () << ".RemoveRadioButton (&" << GetFieldName () << ");" << newline;
}

void	RadioButtonItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	RadioButtonParam*	currentParams = FindLabel (GetBaseLanguage (), gui);
	AssertNotNil (currentParams);
	to << tab (tabCount) << GetFieldName () << ".SetLabel (" << quote << currentParams->fLabel << quote << ", eNoUpdate);" << newline;
	to << tab (tabCount) << GetGroup ()->GetRadioBankName () << ".AddRadioButton (&" << GetFieldName () << ");" << newline;
	ButtonItem::WriteParameters (to, tabCount, language, gui);
}

void	RadioButtonItem::WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteCustomization (to, tabCount, language, gui);
	if ((language != GetBaseLanguage ()) or (gui != GetBaseGUI ())) {
		RadioButtonParam*	baseParams 	  = FindLabel (GetBaseLanguage (), GetBaseGUI ());
		RadioButtonParam*	currentParams = FindLabel (language, gui);
		
		AssertNotNil (baseParams);
		if (currentParams != Nil) {
			if (currentParams->fLabel != baseParams->fLabel) {
				to << tab (tabCount) << GetFieldName () << kFieldAccessor;
				to << "SetLabel (" << quote << currentParams->fLabel << quote << ");" << newline;
			}
		}
	}
}

String	RadioButtonItem::GetLabel () const
{
	return (GetCurrentLabel ().fLabel);
}

void	RadioButtonItem::SetLabel (const String& label)
{
	GetCurrentLabel ().fLabel = label;
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (RadioButtonParamPtr, it, fLabels) {
			RadioButtonParam*	current = it.Current ();
			current->fLabel = label;
		}
	}
}

RadioButtonParam*	RadioButtonItem::FindLabel (CommandNumber language, CommandNumber gui) const
{
	ForEach (RadioButtonParamPtr, it, fLabels) {
		RadioButtonParam*	current = it.Current ();
		
		if (((current->fLanguage == language) or (language == CommandHandler::eNoCommand)) and 
			((current->fGUI == gui) or (gui == CommandHandler::eNoCommand))) {
			
			return (current);
		}
	}
	return (Nil);
}

RadioButtonParam&	RadioButtonItem::GetCurrentLabel () const
{
	RadioButtonParam*	param = FindLabel (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	
	if (param == Nil) {
		ViewItem*	This = (ViewItem*)this;
		This->AddParam (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI (), GetBaseLanguage (), GetBaseGUI ());
		param = FindLabel (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	}
	EnsureNotNil (param);
	return (*param);
}

void	RadioButtonItem::AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI)
{
	ViewItem::AddParam (language, gui, oldLanguage, oldGUI);
	RadioButtonParam*	param = new RadioButtonParam (language, gui);
	RadioButtonParam* oldParams = FindLabel (oldLanguage, oldGUI);
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

void	RadioButtonItem::RemoveParam (CommandNumber language, CommandNumber gui)
{
	ViewItem::RemoveParam (language, gui);
	RadioButtonParam* param = FindLabel (language, gui);
	if (param != Nil) {
		fLabels.Remove (param);
	}
}

void	RadioButtonItem::ApplyParams (CommandNumber language, CommandNumber gui)
{
	ViewItem::ApplyParams (language, gui);
	RadioButtonParam* param = FindLabel (language, gui);
	if (param != Nil) {
		GetRadioButton ().SetLabel (param->fLabel);
	}
}

void	RadioButtonItem::SetItemInfo ()
{
	RadioButtonInfo info = RadioButtonInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetRadioButtonInfoCommand (*this, info));
		DirtyDocument ();
	}
}

SetRadioButtonInfoCommand::SetRadioButtonInfoCommand (RadioButtonItem& item, class RadioButtonInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewLabel (info.GetLabelField ().GetText ()),
	fOldLabel (item.GetLabel ()),
	fNewOn (info.GetOnField ().GetOn ()),
	fOldOn (item.GetRadioButton ().GetOn ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}
		
void	SetRadioButtonInfoCommand::DoIt ()
{
	fItem.SetLabel (fNewLabel);
	fItem.GetRadioButton ().SetOn (fNewOn);
	fItemInfoCommand->DoIt ();
	
	Command::DoIt ();
}

void	SetRadioButtonInfoCommand::UnDoIt ()
{
	fItem.SetLabel (fOldLabel);
	fItem.GetRadioButton ().SetOn (fOldOn);
	fItemInfoCommand->UnDoIt ();
	
	Command::UnDoIt ();
}

