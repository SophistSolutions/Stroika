/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/EmilyWindow.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: EmilyWindow.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.13  1992/02/19  17:31:05  sterling
 *		enabled setclasinfo when in surtomize mode
 *
 *		Revision 1.11  1992/02/18  01:11:37  lewis
 *		Use new version support.
 *
 *		Revision 1.9  1992/02/04  22:55:26  lewis
 *		Use GetShell().GetExtent (), not just GetExtent () for wiundows.
 *		(maybe should be referencing mainview??).
 *
 *		Revision 1.6  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 */





#include	<fstream.h>

#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Version.hh"

#include	"CommandNumbers.hh"
#include	"Dialog.hh"
#include	"NumberText.hh"
#include	"MenuOwner.hh"
#include	"Shell.hh"
#include	"CheckBox.hh"
#include	"DeskTop.hh"

#include	"Language.hh"

#include	"EmilyApplication.hh"
#include	"ItemPallet.hh"
#include	"EmilyWindow.hh"
#include	"MainGroupItem.hh"
#include	"ClassInfo.hh"

#if qMacUI
CommandNumber	EmilyWindow::sCurrentGUI = eMacUI;
#elif 	qMotifUI
CommandNumber	EmilyWindow::sCurrentGUI = eMotifUI;
#elif	qWindowsGUI
CommandNumber	EmilyWindow::sCurrentGUI = eWindowsGUI;
#endif

CommandNumber	EmilyWindow::sCurrentLanguage = eEnglish;
Boolean			EmilyWindow::sCustomizeOnly = False;

/*
 ********************************************************************************
 *********************************** EmilyWindow ********************************
 ********************************************************************************
 */
EmilyWindow::EmilyWindow (EmilyDocument& document):
	Window (),
	Saveable (1),
	fDocument (document),
	fClassName ("foo"),
	fBaseClass ("View"),
	fHeaderPrepend (kEmptyString),
	fHeaderAppend (kEmptyString),
	fSourcePrepend (kEmptyString),
	fSourceAppend (kEmptyString),
	fDataPrepend (kEmptyString),
	fStringCount (1),	
	fGrid (EmilyApplication::Get ().GetGrid ()),
	fGridVisible (EmilyApplication::Get ().GetGridVisible ()),
#if qMacUI
	fGUI (eMacUI),
#elif 	qMotifUI
	fGUI (eMotifUI),
#elif	qWindowsGUI
	fGUI (eWindowsGUI),
#endif
	fLanguage (eEnglish)
{
	fMainGroup = new MainGroupItem (*this);
	SetMainViewAndTargets (fMainGroup, fMainGroup, fMainGroup);
	SetWindowController (&fDocument);
	fMainGroup->ResetCustomizeOnly ();
}

EmilyWindow::~EmilyWindow ()
{
	SetMainViewAndTargets (Nil, Nil, Nil);
}

EmilyDocument&	EmilyWindow::GetDocument ()
{
	return (fDocument);
}

void	EmilyWindow::PrintPage (PageNumber /*pageNumber*/, class Printer& printer)
{
	RequireNotNil (GetMainView ());
	printer.DrawView (*GetMainView (), kZeroPoint);
}

void	EmilyWindow::CalcPages (PageNumber& userStart, PageNumber& userEnd, const Rect& /*pageRect*/)
{
	userStart = 1;
	userEnd = 1;
}

PageNumber	EmilyWindow::CalcAllPages (const Rect& /*pageRect*/)
{
	return (1);
}

void	EmilyWindow::DoSetupMenus ()
{
	Window::DoSetupMenus ();
	
	SetOn (GetGUI (), True);
	SetOn (GetLanguage (), True);
	SetOn (eCustomizeOnly, sCustomizeOnly);
	
	EnableCommand (eSetClassInfo);
	EnableCommand (eCustomizeOnly);

	EnableCommand (eArrow);
	EnableCommand (eThumb);
	if (ItemPallet::GetEditMode () and (not sCustomizeOnly)) {
		for (CommandNumber cmd = eFirstBuildItem; cmd <= eLastBuildItem; cmd++) {
			EnableCommand (cmd, ItemPallet::ShouldEnable (cmd));
		}
	}
#if !qUseCustomMenu			
if (ItemPallet::GetSelectedItem () != Nil) {
ItemPallet::GetSelectedItem ()->SetOn (Toggle::kOn, Panel::eNoUpdate);
}
#endif		
}


class	SetClassInfoCommand : public Command {
	public:
		SetClassInfoCommand (EmilyWindow& window, class ClassInfo& info);
		~SetClassInfoCommand ();
		
		override	void	DoIt ();
		override	void	UnDoIt ();
	
	private:
		EmilyWindow&	fWindow;
		
		String	fNewClassName;
		String	fOldClassName;
		String	fNewBaseClassName;
		String	fOldBaseClassName;
		Point	fNewSize;
		Point	fOldSize;
		String	fNewHelp;
		String	fOldHelp;
		const Font*	fNewFont;
		const Font*	fOldFont;
		Boolean	fOldAutoSize;
		Boolean	fNewAutoSize;
};

SetClassInfoCommand::SetClassInfoCommand (EmilyWindow& window, class ClassInfo& info) :
	Command (eSetClassInfo, kUndoable),
	fWindow (window),
	fNewClassName (info.GetClassNameField ().GetText ()),
	fOldClassName (window.fClassName),
	fNewBaseClassName (info.GetBaseClassNameField ().GetText ()),
	fOldBaseClassName (window.fBaseClass),
	fNewSize (Point (info.GetSizeVField ().GetValue (), info.GetSizeHField ().GetValue ())),
	fOldSize (window.GetMainGroup ().GetScrollSize ()),
	fNewHelp (info.GetHelpField ().GetText ()),
	fOldHelp (window.GetMainGroup ().GetHelp ()),
	fNewFont (info.fFont),
	fOldFont (window.GetMainGroup ().GetFont ()),
	fOldAutoSize (window.GetMainGroup ().GetAutoSize ()),
	fNewAutoSize (info.GetAutoSizeField ().GetOn ())
{
	if (fNewFont != Nil) {
		fNewFont = new Font (*fNewFont);
	}
	if (fOldFont != Nil) {
		fOldFont = new Font (*fOldFont);
	}
}			
		
SetClassInfoCommand::~SetClassInfoCommand ()
{
	delete fNewFont;
	delete fOldFont;
}
		
void	SetClassInfoCommand::DoIt ()
{
	fWindow.fClassName = fNewClassName;
	fWindow.fBaseClass = fNewBaseClassName;
	fWindow.GetMainGroup ().SetHelp (fNewHelp);
	if (fNewSize != fOldSize) {
		if (not fNewAutoSize) {
			fWindow.GetMainGroup ().SetScrollSize (fNewSize);
		}
	}
	if (fNewFont != fOldFont) {
		fWindow.GetMainGroup ().SetFont (fNewFont);
		fWindow.GetMainGroup ().Refresh ();
	}
	fWindow.GetMainGroup ().SetAutoSize (fNewAutoSize);
	fWindow.GetMainGroup ().ApplyCurrentParams ();
	
	Command::DoIt ();
}
		
void	SetClassInfoCommand::UnDoIt ()
{
	fWindow.fClassName = fOldClassName;
	fWindow.fBaseClass = fOldBaseClassName;
	fWindow.GetMainGroup ().SetHelp (fOldHelp);
	fWindow.GetMainGroup ().SetAutoSize (fOldAutoSize);
	if ((fNewSize != fOldSize) or (fNewAutoSize != fOldAutoSize)) {
		if (not fOldAutoSize) {
			fWindow.GetMainGroup ().SetScrollSize (fOldSize);
		}
	}
	if (fNewFont != fOldFont) {
		fWindow.GetMainGroup ().SetFont (fOldFont);
		fWindow.GetMainGroup ().Refresh ();
	}
	fWindow.GetMainGroup ().ApplyCurrentParams ();
	
	Command::UnDoIt ();
}


Boolean		EmilyWindow::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eSetClassInfo: 
			{
				ClassInfo info = ClassInfo (GetMainGroup ());
				info.GetClassNameField ().SetText (fClassName);
				info.GetBaseClassNameField ().SetText (fBaseClass);
				info.GetSizeVField ().SetValue (GetMainGroup ().GetScrollSize ().GetV ());
				info.GetSizeHField ().SetValue (GetMainGroup ().GetScrollSize ().GetH ());
				info.GetHelpField ().SetText (GetMainGroup ().GetHelp ());
				info.GetAutoSizeField ().SetOn (GetMainGroup ().GetAutoSize ());
				
				Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
				d.SetDefaultButton (d.GetOKButton ());
				
				if (d.Pose ()) {
					PostCommand (new SetClassInfoCommand (*this, info));
					PostCommand (new DocumentDirtier (GetDocument ()));
				}
			}
			return (True);

		case eMacUI:
		case eMotifUI:
		case eWindowsGUI:
			SetGUI (selection.GetCommandNumber ());
			return (True);
		
		case eEnglish:
		case eFrench:
		case eGerman:
		case eItalian:
		case eSpanish:
		case eJapanese:
			SetLanguage (selection.GetCommandNumber ());
			return (True);
		
		case eCustomizeOnly:
			if (sCustomizeOnly) {
				SetLanguage (fLanguage);
				SetGUI (fGUI);
			}
			SetCustomizeOnly (not sCustomizeOnly);
			return (True);

		default:
			return (Window::DoCommand (selection));
	}
}

MainGroupItem&	EmilyWindow::GetMainGroup () const
{
	RequireNotNil (fMainGroup);
	return (*fMainGroup);
}

static	const	String	kMachineCodeStartString  = "// text before here will be retained: Do not remove or modify this line!!!";
static	const	String	kMachineCodeEndString    = "// text past here will be retained: Do not remove or modify this line!!!";

void	EmilyWindow::DoRead_ (class istream& from)
{
	char	bigBuf [1000];
	fDataPrepend = String (String::eBuffered, "");
	while (from) {
		if (from.getline (bigBuf, sizeof (bigBuf))) {
			String	s = String (String::eReadOnly, bigBuf, from.gcount ()-1);
			if (s == kMachineCodeStartString) {
				break;
			}

			fDataPrepend += s;
			fDataPrepend += "\n";
		}
	}

	char c;	
	from >> c;
	from.putback (c);
	if (c == 'D') {
		// read off data file creation tag
		from.getline (bigBuf, sizeof (bigBuf));
		from.getline (bigBuf, sizeof (bigBuf));
	}

	Saveable::DoRead_ (from);
	Rect extent = kZeroRect;
	from >> extent;
	WindowShellHints hints = GetShell ().GetWindowShellHints ();
#if 0
	if (extent.GetOrigin () < DeskTop::Get ().GetBounds ().GetBounds ().GetOrigin ()) {
		hints.SetDesiredOrigin (extent.GetOrigin ());
	}
#endif
	hints.SetDesiredSize (extent.GetSize ());
	GetShell ().SetWindowShellHints (hints);
	ReadString (from, fClassName);
	ReadString (from, fBaseClass);
	
sCustomizeOnly = True;
	GetMainGroup ().DoRead (from);
sCustomizeOnly = False;
GetMainGroup ().ResetCustomizeOnly ();
}

void	EmilyWindow::DoWrite_ (class ostream& to, int tabCount) const
{
	if (fDataPrepend != kEmptyString) {
		to << fDataPrepend;
	}
	else {
		to << EmilyApplication::Get ().GetDefaultPrepend ();
	}
	
	to << kMachineCodeStartString << newline << newline;

	String	appLongVersion	=	kApplicationVersion.GetLongVersionString ();
	Assert (appLongVersion != kEmptyString);
	to << "Data file written by " << appLongVersion << "." << newline;
	to << newline << newline;

	Saveable::DoWrite_ (to, tabCount);
	to << newline;
	to << GetShell ().GetExtent () << newline << newline;
	WriteString (to, fClassName);
	WriteString (to, fBaseClass);
	
	GetMainGroup ().ResetFieldCounter ();
	GetMainGroup ().DoWrite (to, tabCount);
	to << newline;
}

void	EmilyWindow::ReadHeaderFile (class istream& from)
{
	char	bigBuf [1000];
	fHeaderPrepend = String (String::eBuffered, "");
	while (from) {
		if (from.getline (bigBuf, sizeof (bigBuf))) {
			String	s = String (String::eReadOnly, bigBuf, from.gcount ()-1);
			if (s == kMachineCodeStartString) {
				break;
			}

			fHeaderPrepend += s;
			fHeaderPrepend += "\n";
		}
	}

	while (from) {
		if (from.getline (bigBuf, sizeof (bigBuf))) {
			String	s = String (String::eReadOnly, bigBuf, from.gcount ()-1);
			if (s == kMachineCodeEndString) {
				break;
			}
		}
	}

	fHeaderAppend = String (String::eBuffered, "");
	while (from) {
		if (from.getline (bigBuf, sizeof (bigBuf), EOF)) {
			fHeaderAppend += String (String::eReadOnly, bigBuf, from.gcount ());
		}
	}
}

void	EmilyWindow::WriteHeaderFile (class ostream& to)
{
	if (fHeaderPrepend != kEmptyString) {
		to << fHeaderPrepend;
	}
	else {
		to << EmilyApplication::Get ().GetDefaultPrepend ();
	}

	to << kMachineCodeStartString << newline << newline;
	
	String	compilationVariable = "__" + fClassName + "__";
	if (EmilyApplication::Get ().GetCompileOnce ()) {
		to << "#ifndef " << compilationVariable << newline;
		to << "#define " << compilationVariable << newline << newline;
	}
	
	static	const	String	kViewString = "View";
	static	const	String	kGroupViewString = "GroupView";
	static	const	String	kLabelGroupString = "LabeledGroup";
	
	Boolean	generateBaseClasses = Boolean ((fBaseClass == kViewString) or (fBaseClass == kGroupViewString) or (fBaseClass == kLabelGroupString));
	if (generateBaseClasses) {
		if (GetMainGroup ().IsButton ()) {
			to << "#include " << quote << "Button.hh" << quote << newline;
		}
		if (GetMainGroup ().IsFocusItem (eAnyGUI)) {
			to << "#include " << quote << "FocusItem.hh" << quote << newline;
		}
		if (GetMainGroup ().IsSlider ()) {
			to << "#include " << quote << "Slider.hh" << quote << newline;
		}
		if (GetMainGroup ().IsText ()) {
			to << "#include " << quote << "TextEdit.hh" << quote << newline;
		}
		to << "#include " << quote << "View.hh" << quote << newline;
		to << newline;
	}
	GetMainGroup ().WriteIncludes (to, 0);
	to << newline << newline;

	
	to << "class "<< fClassName << " : public " << fBaseClass;
	if (generateBaseClasses) {
		if (GetMainGroup ().IsButton ()) {
			to << ", public ButtonController";
		}
		if (GetMainGroup ().IsSlider ()) {
			to << ", public SliderController";
		}
		if (GetMainGroup ().IsFocusItem (eAnyGUI)) {
			to << ", public FocusOwner";
		}
		if (GetMainGroup ().IsText ()) {
			to << ", public TextController";
		}
	}
	
	to << " {" << newline << tab << "public:" << newline;

	// declare constructor
	to << tab (2) << fClassName << " ();" << newline;

	// declare destructor
	to << tab (2) << "~" << fClassName << " ();" << newline << newline;

	// declare CalcDefaultSize method
	to << tab (2) << "override" << tab << "Point" << tab << "CalcDefaultSize_ (const Point& defaultSize) const;" << newline << newline;

	// declare layout method
	to << tab << "protected:" << newline;
	to << tab (2) << "override" << tab << "void" << tab << "Layout ();" << newline << newline;

	// declare fields
	GetMainGroup ().WriteDeclaration (to, 2);
	to << newline << tab << "private:" << newline;

	Boolean first = True;
	for (CommandNumber gui = eFirstGUI; gui <= eLastGUI; gui++) {
		if (GetMainGroup ().ParamsExist (eAnyLanguage, gui)) {
			String 	directive = GetGUICompilationDirective (gui);
			if (first) {
				to << "#if   q";
				first = False;
			}
			else {
				to << "#elif q";
			}
			to << directive << newline;
			to << tab (2) << "nonvirtual void" << tab << "BuildFor" << directive << " ();" << newline;
		}
	}
	Assert (not first);	// must have at least one gui
	to << "#else" << newline;
//	to << tab (2) << "nonvirtual void" << tab << "BuildFor" << GetGUICompilationDirective (GetMainGroup ().GetBaseGUI ()) << " ();" << newline;
	to << tab (2) << "nonvirtual void" << tab << "BuildForUnknownGUI ();" << newline;
	to << "#endif /* GUI */" << newline << newline;
	
	to << "};" << newline << newline;

	if (EmilyApplication::Get ().GetCompileOnce ()) {
		to << "#endif /* " << compilationVariable << " */" << newline;
	}

	to << newline << newline << kMachineCodeEndString << newline;
	if (fHeaderAppend != kEmptyString) {
		to << fHeaderAppend;
	}
}

void	EmilyWindow::ReadSourceFile (class istream& from)
{
	char	bigBuf [1000];
	fSourcePrepend = String (String::eBuffered, "");

	while (from) {
		if (from.getline (bigBuf, sizeof (bigBuf))) {
			String	s = String (String::eReadOnly, bigBuf, from.gcount ()-1);
			if (s == kMachineCodeStartString) {
				break;
			}

			fSourcePrepend += s;
			fSourcePrepend += "\n";
		}
	}

	while (from) {
		if (from.getline (bigBuf, sizeof (bigBuf))) {
			String	s = String (String::eReadOnly, bigBuf, from.gcount ()-1);
			if (s == kMachineCodeEndString) {
				break;
			}
		}
	}

	fSourceAppend = String (String::eBuffered, "");
	while (from) {
		if (from.getline (bigBuf, sizeof (bigBuf), EOF)) {
			fSourceAppend += String (String::eReadOnly, bigBuf, from.gcount ());
		}
	}
}

void	EmilyWindow::WriteSourceFile (class ostream& to)
{
	if (fSourcePrepend != kEmptyString) {
		to << fSourcePrepend;
	}
	else {
		to << EmilyApplication::Get ().GetDefaultPrepend ();
	}

	to << kMachineCodeStartString << newline << newline << newline;
	
	to << "#include " << quote << "Language.hh" << quote << newline;
	to << "#include " << quote << "Shape.hh" << quote << newline;
	to << newline;

	to << "#include " << quote << fDocument.GetHeaderFilePathName ().GetFileName () << quote << newline << newline << newline;

	// define constructor
	to << fClassName << "::" <<  fClassName << " ()";

	//  init fields to zero, in case of exception so we do right thing freeing them...
	StringStream	tempTo;		// we use a stringstream to nuke the trailing comma
	tempTo << " :" << newline;
	GetMainGroup ().WriteBuilder (tempTo, 1);
	String	initializers = String (tempTo);

	if (initializers.GetLength () > 2) {
// still have bad stream bug where zero length returns -1
		initializers.SetLength (initializers.GetLength () -2);	// one for comma, one for newline
	}
	else {
		initializers.SetLength (0);
	}

	to << initializers << newline;
	to << "{" << newline;
	
	Boolean first = True;
	for (CommandNumber gui = eFirstGUI; gui <= eLastGUI; gui++) {
		if (GetMainGroup ().ParamsExist (eAnyLanguage, gui)) {
			String 	directive = GetGUICompilationDirective (gui);
			if (first) {
				to << "#if   q";
				first = False;
			}
			else {
				to << "#elif q";
			}
			to << directive << newline;
		
			to << tab << "BuildFor" << directive << " ();" << newline;
		}
	}
	Assert (not first);	// must have at least one gui
	to << "#else" << newline;
//	to << tab << "BuildFor" << GetGUICompilationDirective (GetMainGroup ().GetBaseGUI ()) << " ();" << newline;
	to << tab << "BuildForUnknownGUI ();" << newline;
	to << "#endif /* GUI */" << newline << "}" << newline << newline;

	// define destructor
	to << fClassName << "::~" <<  fClassName << " ()" << newline;
	to << "{" << newline;
	GetMainGroup ().WriteDestructor (to, 0, GetMainGroup ().GetBaseGUI ());
	to << "}" << newline << newline;

	// define GUI builders
	first = True;
	for (gui = eFirstGUI; gui <= eLastGUI; gui++) {
		if (GetMainGroup ().ParamsExist (eAnyLanguage, gui)) {
			String 	directive = GetGUICompilationDirective (gui);
			if (first) {
				to << "#if   q";
				first = False;
			}
			else {
				to << newline;
				to << "#elif q";
			}
			to << directive << newline << newline;
		
			to << "void" << tab << fClassName << "::BuildFor" << directive << " ()" << newline << "{" << newline;
			GetMainGroup ().WriteInitializer (to, 0, gui);
			to << "}" << newline;
		}
	}
	Assert (not first);	// must have at least one gui
	to << newline << "#else" << newline << newline;
	to << "void" << tab << fClassName << "::BuildForUnknownGUI ();" << newline << "{" << newline;
	GetMainGroup ().WriteInitializer (to, 0, GetMainGroup ().GetBaseGUI ());
	to << "}" << newline << newline;
	to << "#endif /* GUI */" << newline << newline;

	// define CalcDefaultSize method
	to << "Point" << tab << fClassName << "::CalcDefaultSize_ (const Point& /*defaultSize*/) const" << newline << "{" << newline;
	first = True;
	for (gui = eFirstGUI; gui <= eLastGUI; gui++) {
		if (GetMainGroup ().ParamsExist (eAnyLanguage, gui)) {
			String 	directive = GetGUICompilationDirective (gui);
			if (first) {
				to << "#if   q" << directive << newline;
				first = False;
			}
			else {
				to << "#elif   q" << directive << newline;
			}
			Point	scrollSize = GetMainGroup ().GetScrollSize (GetMainGroup ().GetBaseLanguage (), gui);
			to << tab << "return (Point (" << scrollSize.GetV () << ", " << scrollSize.GetH () << "));" << newline;
		}
	}
	Assert (not first);
	to << "#else" << newline;
	Point	scrollSize = GetMainGroup ().GetScrollSize (GetMainGroup ().GetBaseLanguage (), GetMainGroup ().GetBaseGUI ());
	to << tab << "return (Point (" << scrollSize.GetV () << ", " << scrollSize.GetH () << "));" << newline;
	to << "#endif /* GUI */" << newline << "}" << newline << newline;
		
	// define layout method
	to << "void" << tab << fClassName << "::Layout ()" << newline << "{" << newline;
	GetMainGroup ().WriteLayout (to, 1);
	to << tab << fBaseClass << "::Layout ();" << newline;
	to << "}" << newline;

	to << newline << newline << kMachineCodeEndString << newline;
	if (fSourceAppend != kEmptyString) {
		to << fSourceAppend;
	}
}

void	EmilyWindow::EditModeChanged (Boolean newEditMode)
{
	MenuOwner::SetMenusOutOfDate ();
	GetMainGroup ().EditModeChanged (newEditMode);
}

CommandNumber	EmilyWindow::GetGUI ()
{
	return (sCurrentGUI);
}

void	EmilyWindow::SetGUI (CommandNumber gui)
{
	if (sCurrentGUI != gui) {
		CommandNumber	oldGUI = sCurrentGUI;
		sCurrentGUI = gui;
		MenuOwner::SetMenusOutOfDate ();

		if (sCurrentGUI == eMacUI) {
			SetBackground (&kWhiteTile);
		}
		else if (sCurrentGUI == eMotifUI) {
			Tile	t = PalletManager::Get ().MakeTileFromColor (kGrayColor);
			SetBackground (&t);
		}
		else if (sCurrentGUI == eWindowsGUI) {
			SetBackground (&kWhiteTile);
		}
		GetMainGroup ().GUIChanged (oldGUI, sCurrentGUI);
GetMainGroup ().Refresh ();
SetMainView (fMainGroup);	// what a ridiculous hack to get it to resize stuff!!!
GetMainGroup ().Refresh ();
Update ();
	}
}

CommandNumber	EmilyWindow::GetLanguage ()
{
	return (sCurrentLanguage);
}

void	EmilyWindow::SetLanguage (CommandNumber language)
{
	if (sCurrentLanguage != language) {
		CommandNumber	oldLanguage = sCurrentLanguage;
		sCurrentLanguage = language;
		MenuOwner::SetMenusOutOfDate ();

		if (sCurrentLanguage != fLanguage) {
			SetCustomizeOnly (True);
		}
		else if (sCurrentGUI == fGUI) {
			SetCustomizeOnly (False);
		}
		GetMainGroup ().LanguageChanged (oldLanguage, sCurrentLanguage);
	}
}

Boolean	EmilyWindow::GetCustomizeOnly ()
{
	return (sCustomizeOnly);
}

Boolean	EmilyWindow::GetFullEditing ()
{
	return (not sCustomizeOnly);
}

void	EmilyWindow::SetCustomizeOnly (Boolean customizeOnly)
{
	if (sCustomizeOnly != customizeOnly) {
		sCustomizeOnly = customizeOnly;
		MenuOwner::SetMenusOutOfDate ();
	}
}

String	GetGUICompilationDirective (CommandNumber gui)
{
	switch (gui) {
		case eMacUI:
			return ("MacUI");
		case eMotifUI:
			return ("MotifUI");
		case eWindowsGUI:
			return ("WindowsGUI");
		default:
			RequireNotReached ();
	}
	AssertNotReached ();	return (kEmptyString);
}

String	GetLanguageCompilationDirective (CommandNumber language)
{
	switch (language) {
		case eEnglish:
			return ("English");
		case eFrench:
			return ("French");
		case eGerman:
			return ("German");
		case eItalian:
			return ("Italian");
		case eSpanish:
			return ("Spanish");
		case eJapanese:
			return ("Japanese");
		default:
			RequireNotReached ();
	}
	AssertNotReached ();	return (kEmptyString);
}
