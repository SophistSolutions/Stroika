/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PushButtonItem.cc,v 1.8 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PushButtonItem.cc,v $
 *		Revision 1.8  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/22  00:34:55  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.7  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"PushButton.hh"

#include	"PushButtonItem.hh"
#include	"CommandNumbers.hh"
#include	"ViewItemInfo.hh"
#include	"PushButtonInfo.hh"
#include	"EmilyWindow.hh"


#if		!qRealTemplatesAvailable
	Implement (Iterator, PBParmPtr);
	Implement (Collection, PBParmPtr);
	Implement (AbSequence, PBParmPtr);
	Implement (Array, PBParmPtr);
	Implement (Sequence_Array, PBParmPtr);
	Implement (Sequence, PBParmPtr);
#endif




PushButtonParam::PushButtonParam (CommandNumber language, CommandNumber gui) :
	Saveable (1),
	fLanguage (language),
	fGUI (gui),
	fLabel (kEmptyString)
{
}

void	PushButtonParam::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	from >> fLanguage >> fGUI;
	fLanguage += eFirstLanguage;
	fGUI += eFirstGUI;
	ReadString (from, fLabel);
}

void	PushButtonParam::DoWrite_ (class ostream& to, int tabCount) const
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
 ****************************** PushButtonItemType ******************************
 ********************************************************************************
 */
PushButtonItemType::PushButtonItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildButton, "PushButton", (ItemBuilderProc)&PushButtonItemBuilder)
#else
	ItemType (eBuildButton, "PushButton", &PushButtonItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
PushButtonItemType&	PushButtonItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	PushButtonItemType::PushButtonItemBuilder ()
{
	return (new PushButtonItem (Get ()));
}

PushButtonItemType*	PushButtonItemType::sThis = Nil;

static	AbstractPushButton*	BuildPushButton (CommandNumber gui, const String& label)
{
	if (gui == eMacUI) {
		return (new PushButton_MacUI (label));
	}
	else if (gui == eMotifUI) {
		return (new PushButton_MotifUI (label));
	}
	else if (gui == eWindowsGUI) {
		return (new PushButton_WinUI (label));
	}
	AssertNotReached ();	return (Nil);
}

/*
 ********************************************************************************
 ****************************** PushButtonItem **********************************
 ********************************************************************************
 */
PushButtonItem::PushButtonItem (ItemType& type) :
	ButtonItem (type),
	fPushButton (Nil),
	fLabels ()
{
	SetButton (fPushButton = BuildPushButton (EmilyWindow::GetGUI (), "Push Button"));
	SetLabel (fPushButton->GetLabel ());
	SetMaxVersion (5);
}

PushButtonItem::~PushButtonItem ()
{
	ForEach (PushButtonParamPtr, it, fLabels) {
		PushButtonParam* param = it.Current ();
		delete param;
	}
}

void	PushButtonItem::GUIChanged (CommandNumber /*oldGUI*/, CommandNumber newGUI)
{
	SetButton (fPushButton = BuildPushButton (EmilyWindow::GetGUI (), GetPushButton ().GetLabel ()));
	if (newGUI == eWindowsGUI) {
		Tile	t = PalletManager::Get ().MakeTileFromColor (kGrayColor);
		GetPushButton ().SetBackground (&t);
	}
	ApplyCurrentParams ();
}

String	PushButtonItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "PushButton.hh";
	return (kHeaderFileName);
}

AbstractPushButton&	PushButtonItem::GetPushButton () const
{
	RequireNotNil (fPushButton);
	return (*fPushButton);
}

void	PushButtonItem::OldParamReadHack (class istream& from)
{
	String foo;
	ReadString (from, foo);
	SetLabel (foo);
}

void	PushButtonItem::DoRead_ (class istream& from)
{	
	ButtonItem::DoRead_ (from);
	
	if ((GetVersion () <= 2) or (GetVersion () == 4)) {
		String foo;
		ReadString (from, foo);
		SetLabel (foo);
	}
	if (GetVersion () >= 5) {
		ForEach (PushButtonParamPtr, it, fLabels) {
			PushButtonParam* param = it.Current ();
			delete param;
		}
		fLabels.RemoveAll ();

		CollectionSize paramCount;
		from >> paramCount;
		for (int count = 1; count <= paramCount; count++) {
			PushButtonParam*	newParams = new PushButtonParam (eEnglish, eMacUI);
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

void	PushButtonItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ButtonItem::DoWrite_ (to, tabCount);
	CollectionSize	paramCount = fLabels.GetLength ();
	to << tab (tabCount) << paramCount << newline;
	ForEach (PushButtonParamPtr, it, fLabels) {
		to << tab (tabCount) << '{' << newline;
		it.Current ()->DoWrite (to, tabCount+1);
		to << tab (tabCount) << '}' << newline;
	}
}

void	PushButtonItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	PushButtonItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ButtonItem::WriteParameters (to, tabCount, language, gui);

	PushButtonParam*	currentParams = FindLabel (GetBaseLanguage (), gui);
	AssertNotNil (currentParams);
	String	label = currentParams->fLabel;
	Boolean	needsQuotes = False;
	if (label == AbstractPushButton::kOKLabel) {
		label = "AbstractPushButton::kOKLabel";
	}
	else if (label == AbstractPushButton::kCancelLabel) {
		label = "AbstractPushButton::kCancelLabel";
	}
	else if (label == AbstractPushButton::kYesLabel) {
		label = "AbstractPushButton::kYesLabel";
	}
	else if (label == AbstractPushButton::kNoLabel) {
		label = "AbstractPushButton::kNoLabel";
	}
	else {
		needsQuotes = True;
	}
	
	to << tab (tabCount) << GetFieldName () << ".SetLabel (";

	if (needsQuotes) {
		to << quote << label << quote;
	}
	else {
		to << label;
	}
	to << ", eNoUpdate);" << newline;
	to << tab (tabCount) << GetFieldName () << ".SetController (this);" << newline;
}

void	PushButtonItem::WriteCustomization (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteCustomization (to, tabCount, language, gui);
	if ((language != GetBaseLanguage ()) or (gui != GetBaseGUI ())) {
		PushButtonParam*	baseParams 	  = FindLabel (GetBaseLanguage (), GetBaseGUI ());
		PushButtonParam*	currentParams = FindLabel (language, gui);
		
		AssertNotNil (baseParams);
		if (currentParams != Nil) {
			if (currentParams->fLabel != baseParams->fLabel) {
				to << tab (tabCount) << GetFieldName () << kFieldAccessor;
				to << "SetLabel (" << quote << currentParams->fLabel << quote << ");" << newline;
			}
		}
	}
}

String	PushButtonItem::GetLabel () const
{
	return (GetCurrentLabel ().fLabel);
}

void	PushButtonItem::SetLabel (const String& label)
{
	GetCurrentLabel ().fLabel = label;
	CommandNumber	gui = EmilyWindow::GetGUI ();
	if (EmilyWindow::GetFullEditing ()) {
		ForEach (PushButtonParamPtr, it, fLabels) {
			PushButtonParam*	current = it.Current ();
			current->fLabel = label;
		}
	}
}

PushButtonParam*	PushButtonItem::FindLabel (CommandNumber language, CommandNumber gui) const
{
	ForEach (PushButtonParamPtr, it, fLabels) {
		PushButtonParam*	current = it.Current ();
		
		if (((current->fLanguage == language) or (language == CommandHandler::eNoCommand)) and 
			((current->fGUI == gui) or (gui == CommandHandler::eNoCommand))) {
			
			return (current);
		}
	}
	return (Nil);
}

PushButtonParam&	PushButtonItem::GetCurrentLabel () const
{
	PushButtonParam*	param = FindLabel (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	
	if (param == Nil) {
		ViewItem*	This = (ViewItem*)this;
		This->AddParam (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI (), GetBaseLanguage (), GetBaseGUI ());
		param = FindLabel (EmilyWindow::GetLanguage (), EmilyWindow::GetGUI ());
	}
	EnsureNotNil (param);
	return (*param);
}

void	PushButtonItem::AddParam (CommandNumber language, CommandNumber gui, CommandNumber oldLanguage, CommandNumber oldGUI)
{
	ViewItem::AddParam (language, gui, oldLanguage, oldGUI);
	PushButtonParam*	param = new PushButtonParam (language, gui);
	PushButtonParam* oldParams = FindLabel (oldLanguage, oldGUI);
	if (oldParams != Nil) {
		param->fLabel = oldParams->fLabel;
	}
	
	fLabels.Append (param);

ItemParams* itemParams = FindParams (language, gui);
AssertNotNil (itemParams);
if (gui == eMacUI) {
itemParams->fDefaultBorder = Point (1, 1);
itemParams->fDefaultMargin = Point (1, 4);
}
else {
itemParams->fDefaultBorder = Point (2, 2);
itemParams->fDefaultMargin = Point (2, 8);
}
}

void	PushButtonItem::RemoveParam (CommandNumber language, CommandNumber gui)
{
	ViewItem::RemoveParam (language, gui);
	PushButtonParam* param = FindLabel (language, gui);
	if (param != Nil) {
		fLabels.Remove (param);
	}
}

void	PushButtonItem::ApplyParams (CommandNumber language, CommandNumber gui)
{
	ViewItem::ApplyParams (language, gui);
	PushButtonParam* param = FindLabel (language, gui);
	if (param != Nil) {
		GetPushButton ().SetLabel (param->fLabel);
	}
}

void	PushButtonItem::SetItemInfo ()
{
	PushButtonInfo info = PushButtonInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetPushButtonInfoCommand (*this, info));
		DirtyDocument ();
	}
}

SetPushButtonInfoCommand::SetPushButtonInfoCommand (PushButtonItem& item, class PushButtonInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewLabel (info.GetLabelField ().GetText ()),
	fOldLabel (item.GetLabel ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
}
		
void	SetPushButtonInfoCommand::DoIt ()
{
	fItem.SetLabel (fNewLabel);
	fItemInfoCommand->DoIt ();
	
	Command::DoIt ();
}

void	SetPushButtonInfoCommand::UnDoIt ()
{
	fItem.SetLabel (fOldLabel);
	fItemInfoCommand->UnDoIt ();
	
	Command::UnDoIt ();
}
