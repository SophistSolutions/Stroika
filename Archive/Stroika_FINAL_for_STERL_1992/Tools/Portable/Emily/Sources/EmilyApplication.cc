/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/EmilyApplication.cc,v 1.7 1992/09/08 16:40:43 lewis Exp $
 *
 * To Do:
 *
 * Changes:
 *	$Log: EmilyApplication.cc,v $
 *		Revision 1.7  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.22  1992/02/15  08:01:44  lewis
 *		Update for menu changes - AbstractMenu now Menu.
 *
 *		Revision 1.20  1992/02/15  07:57:31  lewis
 *		Fixed up includes.
 *
 *		Revision 1.16  1992/02/04  07:25:13  lewis
 *		Add StringMenuItem include.
 *
 *		Revision 1.15  1992/02/03  18:22:49  sterling
 *		worked around RCS problem
 *
 *		Revision 1.12  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *		Revision 1.10  1992/01/27  15:59:16  sterling
 *		set appropriate menuitems to extended
 *
 *
 *
 */





#include	<fstream.h>

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Alert.hh"
#include	"CheckBox.hh"
#include	"Dialog.hh"
#include	"MenuBar.hh"
#include	"PickList.hh"
#include	"StringMenuItem.hh"

#include	"CSymbolText.hh"
#include	"NumberText.hh"
#include	"SimpleAboutBox.hh"

#include	"CommandNumbers.hh"
#include	"EmilyDocument.hh"
#include	"EmilyWindow.hh"
#include	"EmilyPreferences.hh"
#include	"ItemPallet.hh"

#include	"EmilyApplication.hh"






/*
 ********************************************************************************
 ******************************* EmilyApplication *******************************
 ********************************************************************************
 */
EmilyApplication*	EmilyApplication::sThe	=	Nil;
String	EmilyApplication::kDefaultPrepend = 
	"/*\n * $"
	"Header: $\n *\n * Description:\n"
	" *\n * To Do:\n *\n * $"
	"Log: $\n *\n */\n\n\n";
		
Exception	sPreferencesOutOfDate;

EmilyApplication::EmilyApplication (int argc, const char* argv []):
	Application (argc, argv),
	Saveable (3),
	fDocuments (),
	fGrid (Point (10, 10)),
	fGridVisible (False),
	fConstrainTrackers (False),
	fHeaderSuffix (".hh"),
	fSourceSuffix (".cc"),
	fDataSuffix (".view"),
	fDefaultPrepend (kDefaultPrepend),
	fCompileOnce (True)
{
	Assert (sThe == Nil);
	sThe = this;

#if qMacToolkit	
	fMenuBar.AddMenu (&fSystemMenu, fSystemMenu.DefaultName ());
#endif

	fMenuBar.AddMenu (&fFileMenu, fFileMenu.DefaultName ());
	fMenuBar.AddMenu (&fEditMenu, fEditMenu.DefaultName ());
	fMenuBar.AddMenu (&fFormatMenu, fFormatMenu.DefaultName ());
	fMenuBar.AddMenu (&fCustomizeMenu, fCustomizeMenu.DefaultName ());
	fMenuBar.AddMenu (new ItemPallet (), "Tools");
	
#if  qDebug
	fMenuBar.AddMenu (&fDebugMenu, fDebugMenu.DefaultName ());
#endif

	SetMenuBar (&fMenuBar);

	PullSelfToFrontOnStartup ();
}

void	EmilyApplication::Startup ()
{
	Application::Startup ();
	Try {
		int fd = FileSystem::Get ().Open (FileSystem::Get ().GetPreferencesDirectory () + GetPreferencesFileName (), O_RDONLY);
		ifstream	inFile = int (fd);
		Assert (inFile);
		DoRead (inFile);
	}
	Catch1 (sPreferencesOutOfDate) {
		Alert ("Preferences file out of date, please set new parameters.", "OK").Pose ();
		DoCommand (CommandSelection (eSetPreferences));
	}
	Catch () {
		Alert ("Couldn't open preferences file: creating a new one in your preferences folder.", "OK").Pose ();

		fSpacings.Append ("0");
		fSpacings.Append ("1");
		fSpacings.Append ("2");
		fSpacings.Append ("5");
		fSpacings.Append ("10");
		BuildSpacingMenus ();
		DoCommand (CommandSelection (eSetPreferences));
	}
	DoCommand (CommandSelection (eNew));
}

EmilyApplication::~EmilyApplication ()
{
	Require (fDocuments.GetLength () == 0);

	SetMenuBar (Nil);
#if		qMacToolkit	
	fMenuBar.RemoveMenu (&fSystemMenu);
#endif
	fMenuBar.RemoveMenu (&fFileMenu);
	fMenuBar.RemoveMenu (&fEditMenu);
	fMenuBar.RemoveMenu (&fFormatMenu);
	fMenuBar.RemoveMenu (&fCustomizeMenu);
	ItemPallet& itemPallet = ItemPallet::Get ();
	fMenuBar.RemoveMenu (&itemPallet);
#if  qDebug
	fMenuBar.RemoveMenu (&fDebugMenu);
#endif
}

void	EmilyApplication::BuildSpacingMenus ()
{	
	fFormatMenu.fHorizontalSpacingMenu.DeleteAllMenuItems ();
	fFormatMenu.fVerticalSpacingMenu.DeleteAllMenuItems ();
	
	ForEach (String, it, fSpacings) {
		String	current = it.Current ();
		fFormatMenu.fHorizontalSpacingMenu.AddStringMenuItem (eSpaceHorizontally, current);
		fFormatMenu.fVerticalSpacingMenu.AddStringMenuItem (eSpaceVertically, current);
	}

	fFormatMenu.fHorizontalSpacingMenu.AddStringMenuItem (eUniformSpacesHorizontally);
	(fFormatMenu.fHorizontalSpacingMenu.AddStringMenuItem (eUserSpecifyHorizontalSpacing, "Other"))->SetExtended (True);

	fFormatMenu.fVerticalSpacingMenu.AddStringMenuItem (eUniformSpacesVertically);
	(fFormatMenu.fVerticalSpacingMenu.AddStringMenuItem (eUserSpecifyVerticalSpacing, "Other"))->SetExtended (True);
}

void	EmilyApplication::DoSetupMenus ()
{
	Application::DoSetupMenus ();

	EnableCommand (eAboutApplication);
	EnableCommand (eNew);
	EnableCommand (eOpen);
	EnableCommand (eSetPreferences);
	EnableCommand (eMacUI);
	EnableCommand (eMotifUI);
	EnableCommand (eWindowsGUI);
	EnableCommand (eEnglish);
	EnableCommand (eFrench);
	EnableCommand (eGerman);
	EnableCommand (eItalian);
	EnableCommand (eSpanish);
	EnableCommand (eJapanese);
	
#if qMotifUI
DisableCommand (eToggleClipboardShown);
#endif
#if !qUseCustomMenu
ItemPallet::SetPalletSelection (ItemPallet::GetPalletSelection (), Panel::eNoUpdate);
#endif
}

void	EmilyApplication::AddDocument (EmilyDocument* doc)
{
	RequireNotNil (doc);
	Require (not fDocuments.Contains (doc));
	fDocuments.Append (doc);
}

void	EmilyApplication::RemoveDocument (EmilyDocument* doc)
{
	RequireNotNil (doc);
	Require (fDocuments.Contains (doc));
	fDocuments.Remove (doc);
}

Boolean		EmilyApplication::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eAboutApplication:
			DisplaySimpleAboutBox ();
			return (True);

		case eNew:
			CreateDocument ();
			return (True);

		case eOpen: 
			OpenDocument ();
			return (True);

		case eArrow:
#if !qUseCustomMenu	
ItemPallet::SetPalletSelection (eArrow, Panel::eNoUpdate);
#endif
			EditModeChanged (True);
			return (True);
		
		case eThumb:
#if !qUseCustomMenu	
ItemPallet::SetPalletSelection (eThumb, Panel::eNoUpdate);
#endif
			EditModeChanged (False);
			return (True);

		case eSetPreferences:
			SetPreferences ();
			return (True);

#if !qUseCustomMenu			
case eBuildSlider:
case eBuildButton:
case eBuildTextEdit:
case eBuildTextView:
case eBuildMenu:
case eBuildPickList:
case eBuildGroup:
case eBuildLine:
case eBuildUser1:
case eBuildUser2:
case eBuildUser3:
case eBuildUser4:
{
extern String	sSelectedString;
extern	CommandSelection	sPalletSelection;
ItemPallet::SetPalletSelection (selection.GetCommandNumber (), Panel::eNoUpdate);
Assert (ItemPallet::GetPalletSelection () == selection.GetCommandNumber ());
sSelectedString = selection.GetName ();
sPalletSelection = selection;

Ensure (ItemPallet::GetSelectedString () == selection.GetName ());
}
return (True);
#endif /* !qUseCustomMenu */

		default:	
			return (Application::DoCommand (selection));
	}
	return (False);
}

void	EmilyApplication::ShowMessageForException (const Exception& exception)
{
	Application::ShowMessageForException (exception);
}

EmilyApplication&	EmilyApplication::Get ()
{
	RequireNotNil (sThe);
	return (*sThe);
}

void	EmilyApplication::DoRead_ (class istream& from)
{
	Saveable::DoRead_ (from);
	
	ReadString (from, fHeaderSuffix);
	ReadString (from, fSourceSuffix);
	ReadString (from, fDataSuffix);
	
	if (GetVersion () > 1) {
		ReadString (from, fDefaultPrepend);
	}
	if (GetVersion () > 2) {
		from >> fCompileOnce;
	}

	fSpacings.RemoveAll ();
	CollectionSize length = 0;
	from >> length;
	String	foo = kEmptyString;
	for (int i = 1; i <= length; i++) {
		ReadString (from, foo);
		fSpacings.Append (foo);
	}
	BuildSpacingMenus ();
	from >> fGrid >> fGridVisible >> fConstrainTrackers;
	
	if (GetVersion () < GetMaxVersion ()) {
		sPreferencesOutOfDate.Raise ();
	}
}

void	EmilyApplication::DoWrite_ (class ostream& to, int tabCount) const
{
	Saveable::DoWrite_ (to, tabCount);

	to << newline;
	WriteString (to, fHeaderSuffix) << newline;
	WriteString (to, fSourceSuffix) << newline;
	WriteString (to, fDataSuffix) << newline;
	WriteString (to, fDefaultPrepend) << newline;
	to << fCompileOnce << newline;
	
	CollectionSize length = fSpacings.GetLength ();
	to << ' ' << length << newline;
	ForEach (String, it, fSpacings) {
		WriteString (to, it.Current ()) << newline;
	}
	to << GetGrid () << ' ' << GetGridVisible () << GetConstrainTrackers () << newline;
}

Point	EmilyApplication::GetGrid () const
{
	return (fGrid);
}

Boolean	EmilyApplication::GetGridVisible () const
{
	return (fGridVisible);
}

Boolean	EmilyApplication::GetConstrainTrackers () const
{
	return (fConstrainTrackers);
}

String	EmilyApplication::GetHeaderSuffix () const
{
	return (fHeaderSuffix);
}

String	EmilyApplication::GetSourceSuffix () const
{
	return (fSourceSuffix);
}

String	EmilyApplication::GetDataSuffix () const
{
	return (fDataSuffix);
}

String	EmilyApplication::GetDefaultPrepend () const
{
	return (fDefaultPrepend);
}

Boolean	EmilyApplication::GetCompileOnce () const
{
	return (fCompileOnce);
}

String	EmilyApplication::GetPreferencesFileName () const
{
	return ("Emily Prefs");
}

void	EmilyApplication::EditModeChanged (Boolean newMode)
{
	Application::Get ().FlushPreviousCommand ();
	ForEach (EmilyDocumentPtr, it, fDocuments) {
		AssertNotNil (it.Current ());
		it.Current ()->EditModeChanged (newMode);
	}
}


class	SetPreferencesInfo : public Command {
	public:
		SetPreferencesInfo (EmilyApplication& app, PreferencesInfo& infoDialog);
		
		override	void	DoIt () 
		{
			fApplication.fGrid = Point (fNewGridV, fNewGridH);
			fApplication.fGridVisible = fNewGridVisible;
			fApplication.fConstrainTrackers = fNewConstrainTrackers;
			fApplication.fDataSuffix = fNewDataSuffix;
			fApplication.fHeaderSuffix = fNewHeaderSuffix;
			fApplication.fSourceSuffix = fNewSourceSuffix;
			fApplication.fDefaultPrepend = fNewDefaultPrepend;
			fApplication.fCompileOnce = fNewCompileOnce;
			
			fApplication.fSpacings.RemoveAll ();
			ForEach (String, it, fNewSpacings) {
				fApplication.fSpacings.Append (it.Current ());
			}
			fApplication.BuildSpacingMenus ();
			RefreshIfNeeded ();
			WriteOutFile ();
			
			Command::DoIt ();
		}
			
		override	void	UnDoIt () 
		{
			fApplication.fGrid = Point (fOldGridV, fOldGridH);
			fApplication.fGridVisible = fOldGridVisible;
			fApplication.fConstrainTrackers = fOldConstrainTrackers;
			fApplication.fDataSuffix = fOldDataSuffix;
			fApplication.fHeaderSuffix = fOldHeaderSuffix;
			fApplication.fSourceSuffix = fOldSourceSuffix;
			fApplication.fDefaultPrepend = fOldDefaultPrepend;
			fApplication.fCompileOnce = fOldCompileOnce;
			
			fApplication.fSpacings.RemoveAll ();
			ForEach (String, it, fOldSpacings) {
				fApplication.fSpacings.Append (it.Current ());
			}
			fApplication.BuildSpacingMenus ();
			RefreshIfNeeded ();
			WriteOutFile ();
			
			Command::UnDoIt ();
		}
		
		nonvirtual	void	RefreshIfNeeded ()
		{
			if ((fNewGridV != fOldGridV) or (fNewGridH != fOldGridH) or (fNewGridVisible != fOldGridVisible)) {
				ForEach (EmilyDocumentPtr, it, fApplication.fDocuments) {
					it.Current ()->GetWindow ().GetMainView ()->Refresh ();
				}
			}
		}
		
		nonvirtual	void	WriteOutFile ()
		{
#if qMacOS
			int	fd = FileSystem::Get_MacOS ().Open (FileSystem::Get ().GetPreferencesDirectory () + fApplication.GetPreferencesFileName (), O_WRONLY | O_TRUNC | O_CREAT, 
				fApplication.GetApplicationCreator (), 'TEXT');
#else
			int	fd = FileSystem::Get ().Open (FileSystem::Get ().GetPreferencesDirectory () + fApplication.GetPreferencesFileName (), O_WRONLY | O_TRUNC | O_CREAT);
#endif
			ofstream	to = fd;
			fApplication.DoWrite (to);	
			Assert (to);
			to.close ();
			::close (fd);					// not sure needed???
		}

	private:
		EmilyApplication&	fApplication;
		Coordinate			fNewGridV;
		Coordinate			fOldGridV;
		Coordinate			fNewGridH;
		Coordinate			fOldGridH;
		Boolean				fNewGridVisible;
		Boolean				fOldGridVisible;
		Boolean				fNewConstrainTrackers;
		Boolean				fOldConstrainTrackers;
		String				fNewDataSuffix;
		String				fOldDataSuffix;
		String				fNewHeaderSuffix;
		String				fOldHeaderSuffix;
		String				fNewSourceSuffix;
		String				fOldSourceSuffix;
		String				fNewDefaultPrepend;
		String				fOldDefaultPrepend;
		Boolean				fNewCompileOnce;
		Boolean				fOldCompileOnce;
		Sequence(String)	fNewSpacings;
		Sequence(String)	fOldSpacings;
};

SetPreferencesInfo::SetPreferencesInfo (EmilyApplication& app, PreferencesInfo& infoDialog) :
	Command (eSetPreferences, kUndoable),
	fApplication (app),
	fNewGridV (infoDialog.GetVerticalField ().GetValue ()),
	fOldGridV (app.fGrid.GetV ()),
	fNewGridH (infoDialog.GetHorizontalField ().GetValue ()),
	fOldGridH (app.fGrid.GetH ()),
	fNewGridVisible (infoDialog.GetGridOnField ().GetOn ()),
	fOldGridVisible (app.fGridVisible),
	fNewConstrainTrackers (infoDialog.GetConstrainTrackingField ().GetOn ()),
	fOldConstrainTrackers (app.fConstrainTrackers),
	fNewDataSuffix (infoDialog.GetDataSuffixField ().GetText ()),
	fOldDataSuffix (app.fDataSuffix),
	fNewHeaderSuffix (infoDialog.GetHeaderSuffixField ().GetText ()),
	fOldHeaderSuffix (app.fHeaderSuffix),
	fNewSourceSuffix (infoDialog.GetSourceSuffixField ().GetText ()),
	fOldSourceSuffix (app.fSourceSuffix),
	fNewDefaultPrepend (infoDialog.GetDefaultPrependField ().GetText ()),
	fOldDefaultPrepend (app.fDefaultPrepend),
	fNewCompileOnce (infoDialog.GetCompileOnceField ().GetOn ()),
	fOldCompileOnce (app.fCompileOnce),
	fNewSpacings (),
	fOldSpacings ()
{				
	ForEach (String, it, fApplication.fSpacings) {
		fOldSpacings.Append (it.Current ());
	}
	CollectionSize	length = infoDialog.GetSpacingList ().GetItemCount ();
	for (CollectionSize index = 1; index <= length; index++) {
		String	s = infoDialog.GetSpacingList ().GetStringByIndex (index);
		fNewSpacings.Append (s);
	}
}



void	EmilyApplication::SetPreferences ()
{
	PreferencesInfo info = PreferencesInfo ();
	info.GetVerticalField ().SetValue (fGrid.GetV (), Panel::eNoUpdate);
	info.GetHorizontalField ().SetValue (fGrid.GetH (), Panel::eNoUpdate);
	info.GetGridOnField ().SetOn (fGridVisible, Panel::eNoUpdate);
	info.GetConstrainTrackingField ().SetOn (fConstrainTrackers, Panel::eNoUpdate);
	info.GetDataSuffixField ().SetText (fDataSuffix);
	info.GetHeaderSuffixField ().SetText (fHeaderSuffix);
	info.GetSourceSuffixField ().SetText (fSourceSuffix);
	info.GetDefaultPrependField ().SetText (fDefaultPrepend);
	info.GetCompileOnceField ().SetOn (fCompileOnce);
	ForEach (String, it, fSpacings) {
		info.GetSpacingList ().AddItem (it.Current ());
	}

	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (Nil, View::eNoUpdate);
	
	if (d.Pose ()) {
		PostCommand (new SetPreferencesInfo (*this, info));
	}
}

void	EmilyApplication::CreateDocument ()
{
	EmilyDocument*	theDoc	=	 new EmilyDocument ();
	theDoc->DoNewState ();
}

void	EmilyApplication::OpenDocument ()
{
	EmilyDocument*	theDoc	=	 new EmilyDocument ();
	Try {
		theDoc->Read ();
	}
	Catch () {
		// failed to open document, so delete it.
		theDoc->Close ();		// deletes it.
	}
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

