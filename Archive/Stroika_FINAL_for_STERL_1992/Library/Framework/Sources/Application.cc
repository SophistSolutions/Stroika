/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Application.cc,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *			-	Go thru in a more through manner and be sure things that are deleted are things that we malloced - otherwise
 *				callers responsability (new policy!!!) LGP Feb 28, 1992
 *
 *			-	Try to limit mac includes...
 *
 * Changes:
 *	$Log: Application.cc,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/10  22:35:09  lewis
 *		Scoped Time constants.
 *
 *		Revision 1.78  92/05/14  15:10:35  15:10:35  lewis (Lewis Pringle)
 *		On mac do an UpdateOSScrap () call on the clipboard object since it will not get notified to do so otherwise.
 *		On X, disable show clipboard since currently broken there.
 *
 *		Revision 1.77  92/04/24  08:54:37  08:54:37  lewis (Lewis Pringle)
 *		Comment out EXIT_FAILURE #define.
 *		Set desired size hints for main view window to take into account new calcdefaultsize for menubarmainview.
 *		Got rid of commented out code.
 *
 *		Revision 1.76  92/04/20  14:21:45  14:21:45  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *
 *		Revision 1.75  92/04/02  13:06:14  13:06:14  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *
 *		Revision 1.74  92/04/02  11:40:22  11:40:22  lewis (Lewis Pringle)
 *		gDisplay has been moved to Stroika-Graphix-Globals.hh - it is still initialized here however.
 *
 *		Revision 1.73  92/03/19  16:41:24  16:41:24  lewis (Lewis Pringle)
 *		Break out of RunOnce () the function ProcessPendingCommands (), and add Booleans to
 *		Suspend/Resume processing.
 *
 *		Revision 1.72  1992/03/18  16:53:54  lewis
 *		When shutting down the application under Xt, hide the main app window, if it isn't already,
 *		before starting to dispose of widgets - the display can be unsightly.
 *
 *		Revision 1.71  1992/03/13  16:32:37  lewis
 *		Use MenuBarMainView in MainApplicationShell isntead of XmMainWindow.
 *		Change menu calls in applcication to make calls in MainAppWindow so it can update its menu field.
 *		(all the above motif only).
 *		Add new method TryToShutDown () and use that instead of ShutDown (). Return ReadOnly strings in
 *		a MacOSErrorToString (or whatever).
 *
 *		Revision 1.70  1992/03/10  01:21:51  lewis
 *		Only call BeREALLYInstalledInSystemMBar () if menuBar not nil in App::SetMenu ().
 *
 *		Revision 1.69  1992/03/10  00:01:37  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.68  1992/03/05  22:57:29  lewis
 *		Delete unused code, and make app shell (under motif) only visible if we have a menu bar.
 *
 *		Revision 1.67  1992/03/05  20:24:05  lewis
 *		Port to motif, and allow (really require) SetMenuBar (Nil).
 *
 *		Revision 1.66  1992/03/05  18:16:14  lewis
 *		No longer build a menubar, but allow one to be set (current one support it being
 *		called once).
 *		No longer assume there is a menubar in the application (later on motif dont put up
 *		app window if there is no menubar?).
 *		No longer really support systemenu, and debugmenu - at least dont keep fields, nor
 *		autobuild them - up to subclasses. Do call static members in those classes to
 *		hook them into the "target chain" istead of old method of calling methods
 *		of the actual menus themselves.
 *
 *		Revision 1.62  1992/02/28  22:18:07  lewis
 *		No longer assume we can freely delete menus that are handed to us. We delete only ones we alloc with new. Its
 *		the caller of SetDebugMenu () or SetSystemMenu ()s responsability to destroy
 *		any menus it sets. Also, remove all menus before destruction and before deletion of the menu bar. Ensure unrealize called.
 *
 *		Revision 1.60  1992/02/17  23:16:35  lewis
 *		Moved version number stuff to a class in Foundation, and a global variable at the top level of stroika (above
 *		the layers).
 *
 *		Revision 1.59  1992/02/15  05:18:41  lewis
 *		Use MenuBar instead of StandardMenuBar.
 *
 *		Revision 1.58  1992/02/15  05:17:29  sterling
 *		split up stream call to work around mac compiler bug (???)
 *
 *		Revision 1.55  1992/02/03  16:47:51  sterling
 *		added include of memory.h for macintosh
 *
 *		Revision 1.54  1992/02/01  19:19:45  lewis
 *		On close of app shell, simulate as quit, rather than alert.
 *
 *		Revision 1.53  1992/01/29  16:45:28  lewis
 *		Added message for new access denied exception.
 *
 *		Revision 1.52  1992/01/29  15:13:57  lewis
 *		Got rid of some anachronistic global variable hacks.
 *		Also, put if (GetMenuBar()==Nil) return into SetupTheMenus since our idle task seems to get called
 *		while it is nil. Thats not necessarily a problem - really I should get rid of any dependency on it
 *		not being nil when this gets called, and we should probably move menubar stuff out of application anyhow!!!
 *
 *		Revision 1.51  1992/01/29  07:00:22  lewis
 *		Added setupthemenus () idle task.
 *
 *		Revision 1.50  1992/01/29  05:03:27  sterling
 *		changed window hints of mainview guy
 *
 *		Revision 1.49  1992/01/29  00:00:17  lewis
 *		Do a flushpreviouscommand in Application::ShutDown () so that any side effects are done
 *		before we quit the application. Its not clear anyone should be counting on such side effects,
 *		but for now, I've found 1 case in Emily.
 *
 *		Revision 1.48  1992/01/28  22:15:10  lewis
 *		No longer inherit from KeyHandler.
 *		No longer build a desktop object - leave that to code in Desktop.hh/cc.
 *		Moved mouse/help region stuff and ProcessMousePress/MouseMove stuff all to desktop.
 *		Should be no mousehandling stuff left in Application.
 *
 *		Revision 1.47  1992/01/28  20:55:29  lewis
 *		Moved clipboard support to Clipboard.hh/cc. Other genral code cleanups.
 *
 *		Revision 1.46  1992/01/28  19:03:19  lewis
 *		Moved exittoshell patch from here to OSInit since  its really osrelated, and has nothing to do with application.
 *
 *		Revision 1.45  1992/01/27  21:38:30  lewis
 *		Be sure we print error message about failure to open display even with debug turned off (used to call
 *		XtWarning - but that does nothing with debug turned off!!). Now just fprintf (stderr).
 *
 *		Revision 1.44  1992/01/23  08:07:44  lewis
 *		No longer call setsensitive for main window shell, since not needed, and no longer supported by oscontrols.
 *
 *		Revision 1.43  1992/01/20  05:42:14  lewis
 *		Move code to put appcontext into eventmanager field to eventmanager, and deleted timerproc suppport,
 *		moved it to EventManager, and changed it there.
 *
 *		Revision 1.42  1992/01/18  09:07:44  lewis
 *		Move much of the mainviewwindow stuff into a motifoscontrol class, and use a real window in a window list,
 *		for the motifapplicationshell. General cleanups and attempts to throw away unused vars / include files.
 *
 *		Revision 1.41  1992/01/17  17:51:14  lewis
 *		Cleaned up includes.
 *		for motif.
 *
 *		Revision 1.40  1992/01/08  05:40:01  lewis
 *		Changed to support new EventHandler design - now inherit from both EventHandler and EventManager. Changed params
 *		to a bunch of methods because of EventHandler changes. Moved IdleTask
 *		support to EventManager. Moved Window update support to Window (and a bit more to EventManager) now
 *		that other EventManagers can work together better now.
 *
 *		Revision 1.39  1992/01/06  10:05:33  lewis
 *		Added Idletime support for Xt.
 *
 *		Revision 1.36  1991/12/27  19:04:24  lewis
 *		sterl added Startup () protected virtual method of opejning docus etc...
 *
 *		Revision 1.35  1991/12/27  16:54:58  lewis
 *		Moved some X initialization code to OSInit.
 *		Use MainApplicationShell for motif.
 *
 *		Revision 1.34  1991/12/18  09:05:59  lewis
 *		Worked on adding WindowSHell support.
 *
 *
 */



#include	<stdlib.h>				// for exit
#include	<string.h>


#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Desk.h>
#include	<Errors.h>
#include	<Events.h>
#include	<Files.h>
#include	<Memory.h>
#include	<Menus.h>
#include	<SysEqu.h>
#include	<Resources.h>
#include	<ToolUtils.h>
#include	<Windows.h>
#elif	qXToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<stdio.h>
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/Xm.h>
// not strictly needed - just to hack in menu stuff...
//#include <Xm/MainW.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"File.hh"
#include	"Memory.hh"
#include	"OSConfiguration.hh"
#include	"Sound.hh"
#include	"StreamUtils.hh"

#include	"Cursor.hh"
#include	"Region.hh"

#include	"ClipBoard.hh"

#if 	qSupportClipBoardWindow
#include	"ClipBoardWindow.hh"
#endif	/*qSupportClipBoardWindow*/

#include	"Alert.hh"
#include	"DebugMenu.hh"
#include	"DeskTop.hh"
#include	"Dialog.hh"
#include	"Document.hh"
#include	"MenuBar.hh"
#include	"MenuBarMainView.hh"
#include	"Shell.hh"
#include	"StandardMenus.hh"
#include	"Window.hh"

#include	"Application.hh"





class	MemAllocException : public Exception {
	public:
		MemAllocException ():
			Exception ()
			{
			}

		override	void	Raise ()
			{
				Raise (0);
			}

		nonvirtual	void	Raise (size_t amountAlloced)
			{
				fAmountAlloced = amountAlloced;
				Exception::Raise ();
			}

		nonvirtual	size_t	GetAmountAlloced () const
			{
				return (fAmountAlloced);
			}

	private:
		size_t	fAmountAlloced;
};
static	MemAllocException	gMemAllocException;



#if		qXGDI
extern	osDisplay*	gDisplay;
osWidget*			gShell;		// hopefully no-one needs this???
  								// actaully still referbenced in PathNameSelectors - but this usage is probably wrong
								// use CreateAppShell () instead???
#endif




#if		qXmToolkit
// should use this on the mac too, and always have window in back??? Then use this for the menubar so we get our menubar clicks???
class	MainAppWindow : public Window {
	public:
		MainAppWindow (const char* appName, osDisplay* display):
			Window (new MainApplicationShell (appName, appName, display))
		{
			class	MainAppWindowController : public WindowController {
				public:
					MainAppWindowController () :
						WindowController ()
					{
					}
					
					override	void	HandleClose (Window& ofWindow)
					{
						Application::Get ().TryToShutDown ();
					}
			};

			SetMainViewAndTargets (&fMenuBarMainView, Nil, Nil);
			SetWindowController (new MainAppWindowController ());
		}
		~MainAppWindow ()
		{
			SetMainViewAndTargets (Nil, Nil, Nil);
		}

		void	SetMenuBar (AbstractMenuBar* menuBar)
	  		{
				fMenuBarMainView.SetMenuBar (menuBar);
			}

		override	WindowShellHints	ApplyWindowShellHints (const WindowShellHints& originalHints)
			{
				WindowShellHints	hints	=	originalHints;
//				hints.SetDesiredSize (Point (100, 400));
				hints.SetDesiredSize (fMenuBarMainView.CalcDefaultSize (Point (100, 400)));
				return (hints);
			}
		MenuBarMainView	fMenuBarMainView;
};
#endif




class	SetupMenusChecker : public PeriodicTask {
	public:
		SetupMenusChecker ()
			{
#if		qSunCFront_OverloadingConfusionBug
				SetPeriodicity ((BigReal)0.1);			// check for change about every 1/2 second
#else
				SetPeriodicity (0.1);					// check for change about every 1/2 second
#endif
			}
		override	void	RunABit ()
			{
				Application::Get ().SetupTheMenus ();
			}
};













/*
 ********************************************************************************
 ************************************* Application ******************************
 ********************************************************************************
 */

Application*	Application::sThe	=	Nil;



// TRY TO GET RID OF THIS FIELD/CONCEPT HERE - AT LEAST CHANGE IT A BIT.. Mar 4, 1992, LGP...
AbstractMenuBar*	Application::GetMenuBar ()	const					{	return (fMenuBar);						}
void	Application::SetMenuBar (AbstractMenuBar* menuBar)
{
#if		qXGDI
	fAppShellWindow->SetMenuBar (menuBar);
#endif

// This is really the wrong place to do this - I think - in swichin /switchout method???
	menuBar->InstallMenu ();

#if		qMacToolkit
// hack for now..
	if (menuBar != Nil) {
		((MenuBar*)menuBar)->BeREALLYInstalledInSystemMBar ();
	}
#endif
	fMenuBar = menuBar;

#if		qXGDI
	AssertNotNil (fAppShellWindow);
	fAppShellWindow->SetVisible (Boolean (fMenuBar != Nil));
#endif
}



Application::Application (int argc, const char* argv []):
	EventManager (),
	MenuCommandHandler (),
	fCommandProcessingSuspendCount (0),
	fThroToEventLoop (),
	fLastLoMemoryChideAt (Time (0)),
	fCheeseBuf (Nil),
	fCheeseBufSize (50*1024),			// 50K default size seems reasonable - enuf for a segment or 2
#if		qSupportClipBoardWindow
	fClipBoardWindow (Nil),
#endif	/*qSupportClipBoardWindow*/
	fMenuBar (Nil),
	fCurrentCommand (Nil),
	fPreviousCommand (Nil),
	fInForeground (True),				// not sure about initial state....
#if		qXGDI
	fAppShellWindow (Nil),
#endif
	fSetupMenusChecker (Nil)
{
	Activate ();

#if		qMacOS
	/*
	 * Be sure we've cleaned up memory, at this stage...
	 */
	const size_t	bigSize	=	0x8fffffff;
	::PurgeMem (bigSize); 
	(void)::CompactMem (bigSize); 
#endif	/*qMacOS*/

	Require (sThe == Nil);
	sThe = this;

#if		qSunCFront_StaticFuncsSBPlainTypeBug
	SetOutOfMemoryProc ((Boolean (*)(size_t))&UrgentMemoryNeed_);
#else
	SetOutOfMemoryProc (&UrgentMemoryNeed_);
#endif


	/*
	 * Initialize X Toolkit - probably should go into OSINIT!!!
	 * Then open display.
	 */
#if		qXGDI
	struct ApplicationData {
#if		qSunCFrontDontLikeZeroSizedStructBug
		char xxx;
#endif
	};
	ApplicationData appData;	// snag command line args in X format...

	/*
	 *	Goal is to not automatically specify the application shell type, which is done for you
	 *	in the XtAppInitialize call. Code to call the low-level stuff swiped from OSF/Motif Programmers
	 *	Guide, 3-10 and 3-37. Trouble is, the actual code for XtAppInitialize does some other stuff too,
	 *	and I`m not sure whether it is important or not. Better study up on the X stuff, and figure out
	 *	exactly what we need to do here.
	 */
	Cardinal c_argc = argc;
	Display*        display = ::XtOpenDisplay (fAppContext, Nil, argv[0], "StroikaDemo", Nil, 0, &c_argc, (char**)argv);
	argc = c_argc;
	if (display == Nil) {
		fprintf (stderr, "Failed to open display, exiting...\n");
		::exit (0);
	}
	gDisplay = display;
#endif	/*qXGDI*/


	/*
	 * Create App Shell.
	 */
#if		qXGDI
	fAppShellWindow = new MainAppWindow (argv[0], display);
	gShell = fAppShellWindow->GetShell ().GetOSRepresentation ();

	::XtGetApplicationResources (gShell, (XtPointer)&appData, Nil, 0, Nil, 0);
#endif	/*qXGDI*/

#if		qXGDI
	Cursor::sTheDisplay = display;
	Cursor::sTheInputWindow = ::XtWindow (fAppShellWindow->GetOSRepresentation ());

//	kWatchCursor.Install (); //works, but not good til we have idle tasks working right to reset back to
// normal - need other code to cursor sets working...
#endif	/*!XGDI*/


#if		qSupportClipBoardWindow
	SetClipBoardWindow (new ClipBoardWindow ());
#endif	/*qSupportClipBoardWindow*/


	/*
	 * Subtle... Make sure we build the idle task AFTER we've set things up, like the clipboard window,
	 * since who knows when the idletask will get called?
	 */
	fSetupMenusChecker = new SetupMenusChecker ();
	AddIdleTask (fSetupMenusChecker);

}

Application::~Application ()
{
	if (fSetupMenusChecker != Nil) {
		AddIdleTask (fSetupMenusChecker);
		delete (fSetupMenusChecker); fSetupMenusChecker = Nil;
	}
#if		qSupportClipBoardWindow
	if (fClipBoardWindow != Nil) {
		Assert (not (ClipBoardShown ()));
		delete (fClipBoardWindow); fClipBoardWindow = Nil;
	}
#endif	/*qSupportClipBoardWindow*/
	Deactivate ();
}

void	Application::Startup ()
{
}

void	Application::Run ()
{
	Startup ();

	while (True) {
		Try {
			RunOnce ();
		}
		Catch () {
			break;
		}
	}

	/*
	 * In case an animated cursor is currently installed, we must install a benign one. If this fails,
	 * oh well.
	 */
	Try {
		kArrowCursor.Install ();
	}
	Catch () {
		// ignore...
	}
}

void	Application::RunOnce ()
{
	Try {
#if		qMacOS
			::HiliteMenu (0);
#endif	/*qMacOS*/
		EventManager::RunOnce ();

		if (CommandProcessingEnabled ()) {
			ProcessPendingCommands ();
		}

		/*
		 * Check and see we've plenty of memory.
		 */
		if (CriticallyLowOnMemory ()) {
			if (GetCurrentTime () > fLastLoMemoryChideAt + 30) {
				fLastLoMemoryChideAt = GetCurrentTime ();	// measure from when chide done...
				ChideUserAboutLowMemory ();
			}
		}
	}
	Catch1 (fThroToEventLoop) {
		_this_catch_->Raise ();
	}
	Catch () {
		Try {
			ShowMessageForException (*_this_catch_);
		}
		Catch () {
			/*
			 * We are clearly in deep shit at this point.  Call exit, rather than exittoshell
			 * in the hopes we can get something cleaned up. Dont call shutdown, since that
			 * would probably just get us in deeper.  Remember, we cannot display any alerts to
			 * the user, or anything at this point if things are so bad that we cannot even
			 * call ShowMessageForException!!!
			 */
#if		qDebug
			DebugMessage ("exception while ShowMessageForException! - aborting..."); 
#endif	/*qDebug*/
			Beep (); Beep (); Beep ();

// Need ansi spec to see if this is required , and if so, document as bug when missing!!!
//#ifndef	EXIT_FAILURE
//#define	EXIT_FAILURE	1
//#endif
			::exit (EXIT_FAILURE);
		}
	}
}

void	Application::ShutDown ()
{
	FlushPreviousCommand ();		// Somewhat funny place to do this, but some commands are not fully commited until
									// this is done and quitting without doing it could cause problems...

	/*
	 * Try to close each document, and deal with any exceptions that may arise. One trouble with the
	 * way we are doing this is that it has nothing to do with the order of the windows on the
	 * screen. Not quite sure how to fix this (lots of alternatives leap to mind).... Not
	 * so serious we need to worry now.
	 */
	volatile	Exception*	passOn	=	Nil;
#if		qMPW_NO_VOLATILES_BUG
	Exception*& xxxFREDxxx = passOn;		// force not in register...
#endif	/*qMPW_NO_VOLATILES_BUG*/
	{
#if		0
// first try shutting the document associated with each front window. If that leaves any docs
// with no windows, then close those (not sure this can happen?).

// trouble with implementing this is that only Docuemnts have a trytoclose method, and so for
// each window, we must ask each doc if it owns it (do a method that just returns the
// doc) and then use that doc to try to close it. Below code not that great, but good enuf
// for now...
Window*	lastTimesFrontWindow	=	Nil
Window*	frontWindow	=	Nil
while ( (frontWindow = Desktop::Get ().GetFrontWindow ()) != Nil ) {
	Assert (frontWindow->GetVisible ());
}

#endif
		ForEach (DocumentPtr, it, Document::MakeDocumentIterator ()) {
			Document*	d	=	it.Current ();
			AssertNotNil (d);
			Try {
				d->TryToClose ();		// can fail
			}
			Catch () {
				passOn = _this_catch_;
				break;
			}
		}
	}
	if (passOn != Nil) {		// propogate exception after closing scope which forces dtor on iterator...
		passOn->Raise ();
	}

#if		qSupportClipBoardWindow
	ShowClipBoard (False);
#endif	/*qSupportClipBoardWindow*/

#if		qMacToolkit
	ClipBoard::Get ().UpdateOSScrap ();		// Since we will not get a message from the finder now...
#endif	/*qMacToolkit*/

#if		qXtToolkit
	fAppShellWindow->SetVisible (False);
#endif	/*qXtToolkit*/

	/*
	 * All must be closed up by now, so go ahead and quit.
	 */
	fThroToEventLoop.Raise ();
}



	class	QuitCommand : public Command {
		public:
			QuitCommand (void (Application::*shutDown) ());

			override	void	DoIt ()
				{
// Broken into two lines cuz hp compiler barfs otherwise...
					Application* app = &Application::Get ();
					(app->*fShutDown) ();
				}
			override	Boolean	CommitsLastCommand () const
				{
					return (True);
				}
		private:
	   		void	(Application::*fShutDown) ();
	};

QuitCommand::QuitCommand (void (Application::*shutDown) ()):
	  Command (CommandHandler::eQuit),
		  fShutDown (shutDown)
		{
		}

void	Application::TryToShutDown ()
{
	void	(Application::*f) () = &Application::ShutDown;
	PostCommand (new QuitCommand (f));
}

void	Application::ProcessCommand (const CommandSelection& selection)
{
	Assert (selection.GetCommandNumber () != eNoCommand);		// not sure if thats wrong???

	/*
	 * This consists of going thru each window plane, and give each active window, in turn,
	 * a shot at the command.  Those windows (may) in turn pass the DoCommand on to the
	 * WindowController (usually a document), and that document (may) pass that along to the
	 * DocumentController (usually the Application), but to implement some sort of modality, or
	 * because the command has been handled, that might not actually happen.
	 */
	MenuOwner::SetMenusOutOfDate ();
	if (DeskTop::Get ().DoCommand (selection)) {
		return;
	}
	if (not DeskTop::Get ().GetModal ()) {
		if (DoCommand (selection)) {
			return;
		}
	}
#if		qDebug
	if (DebugMenu::DoCommandDebugMenu (selection)) {
		return;
	}
#endif	/*qDebug*/
	if (selection.GetCommandNumber () == eUndo) {
		HandleUndoMenu ();
	}
}

void	Application::PostCommand (Command* postedCommand)
{
	RequireNotNil (postedCommand);
	MenuOwner::SetMenusOutOfDate ();
	if (fCurrentCommand == Nil) {
		fCurrentCommand = postedCommand;
	}
	else {
		fCurrentCommand = new CompositeCommand (postedCommand, fCurrentCommand);
	}
}

void	Application::FlushPreviousCommand ()
{
	if (fPreviousCommand != Nil) {
		delete (fPreviousCommand); fPreviousCommand = Nil;
		MenuOwner::SetMenusOutOfDate ();
	}
}

void	Application::ProcessPendingCommands ()
{
	/*
	 * Process any pending commands.
	 */
	if (fCurrentCommand != Nil) {
		Command*	todo	=	fCurrentCommand;
		fCurrentCommand = Nil;			// in case command invokes recursive run...

		// do here since this can make recursive call do runonce ();
		// note the funny behavior if a rucursive call - eg dialog - does undoable command...
		// could be bad???

// right here should check CriticallyLowOnMemory (), and some other method of 
// command to see if it will use much memory...  Cannot just always punt since close is 
// a command!!  But, if not enuf memoruy, just launch dlog saying cannot do command since
// not enuf memory.
		MenuOwner::SetMenusOutOfDate ();
		if (not todo->GetDone ()) {
			todo->DoIt ();
		}
		if (todo->CommitsLastCommand ()) {
			if (fPreviousCommand != Nil) {
				delete fPreviousCommand;
				fPreviousCommand = Nil;
			}
		}
		if (todo->GetUndoable ()) {
			Assert (fPreviousCommand == Nil);
			fPreviousCommand = todo;
		}
		else {
			delete (todo);
		}
	}
}

void	Application::SuspendCommandProcessing ()
{
	Require (fCommandProcessingSuspendCount < 10);		// sanity check - really OK in principle
	fCommandProcessingSuspendCount ++;
}

void	Application::ResumeCommandProcessing ()
{
	Require (fCommandProcessingSuspendCount >= 1);
	fCommandProcessingSuspendCount --;
}

Boolean	Application::CommandProcessingEnabled () const
{
	return (Boolean (fCommandProcessingSuspendCount == 0));
}

void	Application::DoSetupMenus ()
{
	EnableCommand (eQuit, True);

	EnableCommand (ePopupItem, True);
	EnableCommand (eCascadeItem, True);

#if		qSupportClipBoardWindow
#if	!qXtToolkit
	// Broken still - no pictures implemented..
	EnableCommand (eToggleClipboardShown, True);
#endif
	SetCommandName (eToggleClipboardShown, ClipBoardShown ()? "Hide ClipBoard": "Show ClipBoard");
#endif	/*qSupportClipBoardWindow*/

#if 	qMacOS
	SystemMenu::EnableDeskAccessories (True);
#endif	/*qMacOS*/
#if		qDebug
	DebugMenu::DoSetupsForDebugMenus ();
#endif	/*qDebug*/
}

Boolean		Application::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case	eQuit: {
			TryToShutDown ();
			return (True);
		}
		break;

#if		qSupportClipBoardWindow
		case	eToggleClipboardShown: {
			ShowClipBoard (not (ClipBoardShown ()));
			return (True);
		}
		break;
#endif	/*qSupportClipBoardWindow*/
	}
	return (False);
}

void	Application::SetupTheMenus ()
{
// tmp hack LGP Jan 29 - VERY SOON fix this some other way...
// HACK NO LONGER NEEDED! LGP March 4... if (GetMenuBar () == Nil) { return; }

	if (MenuOwner::GetMenusOutOfDate ()) {
		MenuOwner::BeginUpdate ();

		DeskTop::Get ().DoSetupMenus ();
		if (not DeskTop::Get ().GetModal ()) {
			DoSetupMenus ();
		}
		SetupUndoMenu ();

		MenuOwner::EndUpdate ();
	}
}

void	Application::SetupUndoMenu ()
{
	if (fPreviousCommand == Nil) {
		SetCommandName (eUndo, "Can't Undo");
	}
	else {
		String	commandName	=	fPreviousCommand->GetName ();
		String	menuText	=	fPreviousCommand->GetDone ()? "Undo": "Redo";
		if (commandName != kEmptyString) {
			menuText += " ";
			menuText += commandName;
		}
		EnableCommand (eUndo, True);
		SetCommandName (eUndo, menuText);
	}
}

void	Application::HandleUndoMenu ()
{
	AssertNotNil (fPreviousCommand);
	if (fPreviousCommand->GetDone ()) {
		fPreviousCommand->UnDoIt ();
	}
	else {
		fPreviousCommand->ReDoIt ();
	}
}

void	Application::ShowMessageForException (const Exception& exception)
{
	/*
	 * Since we must make sure to get most specific exceptions then least, we must have some careful
	 * ordering of these cases. I choose to first sort by module where the exception is defined, and
	 * then between modules by alphabetically within a layer, and between layers, by layer.
	 * (SOMEONE translate above instructions into swahili, for clarity sake).
	 */

// Document excpetions
	if (exception.Match (gUserCancelCloseException)) {
		// ignore...
	}

// File manager exceptions
	else if (exception.Match (FileSystem::sPlainFileOperationOnDirectory)) {
		Alert ("Plain file operation on a directory.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (FileSystem::sDirectoryOperationOnNonDirectory)) {
		Alert ("Directory operation on non-directory.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (FileSystem::sFileNameBadCharacter)) {
		Alert ("Bad character in filename.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (FileSystem::sFileNameTooLong)) {
		Alert ("Filename too long.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (FileSystem::sAccessPermissionDenied)) {
		Alert ("File access permission denied.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (FileSystem::sFileExists)) {
		Alert ("File exists.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (FileSystem::sTooManyFilesOpen)) {
		Alert ("Too many files open.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (FileSystem::sFileNotFound)) {
		Alert ("File not found.", AbstractPushButton::kOKLabel).Pose ();
	}
#if	qMacOS
	else if (exception.Match (FileSystem::sFileLocked)) {
		Alert ("Attempted to write to locked file. Please unlock file in finder.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (FileSystem::sFileProjectorLocked)) {
		Alert ("Attempted to write to file protected by projector. Please check out file from projector.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (FileSystem::sResourceNotFound)) {
		Alert ("Resource not found.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (FileSystem::sNoResourceFork)) {
		Alert ("Resource Fork Empty.", AbstractPushButton::kOKLabel).Pose ();
	}
#endif	/*qMacOS*/
	else if (exception.Match (FileSystem::sException)) {
#if		qMacOS
		Alert (OSErrorToString (((const FileException&)exception).GetErrNumber ()) + ".", AbstractPushButton::kOKLabel).Pose ();
#endif	/*qMacOS*/
	}

// Saveable Exceptions
	else if (exception.Match (Saveable::sFileFormatTooNew)) {
		Alert ("File format too new - cannot open with this version of the program.", AbstractPushButton::kOKLabel).Pose ();
	}
	else if (exception.Match (Saveable::sBadVersion)) {
		Alert ("File has a corrupt version identification.", AbstractPushButton::kOKLabel).Pose ();
	}

// Memory manager exceptions
	else if (exception.Match (gMemoryException)) {
#if		qDebug
		DebugMessage ("caught MemoryException: (oserr = %d)", gMemoryException.GetOSErr ());
#endif	/*qDebug*/
#if		qMacOS
		Alert (OSErrorToString (((const MemoryException&)exception).GetOSErr ()) + ".", AbstractPushButton::kOKLabel).Pose ();
#endif	/*qMacOS*/
	}

// Focus exceptions
	else if (exception.Match (FocusItem::sFocusFailed)) {
		if (FocusItem::sFocusFailed.GetMessage () != kEmptyString) {
			static	StagedAlert	focusFailed = StagedAlert (kEmptyString, 3);
			focusFailed.SetMessage (FocusItem::sFocusFailed.GetMessage ());
			focusFailed.Pose ();
		}
	}

// Local Application Exceptions
	else if (exception.Match (gMemAllocException)) {
		OStringStream	buf;
		buf << "Not enough memory to complete the requested action ("
			<< dec << ((const MemAllocException&)exception).GetAmountAlloced ();
		buf	<< " bytes requested). Try closing some documents, or giving the application more memory"
			<< " from the Finder's Get File Info";
		Alert (String (buf), AbstractPushButton::kOKLabel).Pose ();
	}


// Unknown exceptions
	else {
#if		qDebug
		DebugMessage ("caught unknown Exception (%p)", &exception);
#endif	/*qDebug*/
		Alert ("caught unknown exception", AbstractPushButton::kOKLabel).Pose ();
	}
}

#if		qMacOS
String	Application::OSErrorToString (short macOSError)
{
	switch (macOSError) {
#if		qIncludeErrorStrings
		case	dskFulErr:			return (String (String::eReadOnly, "Disk full"));
		case	tmfoErr:			return (String (String::eReadOnly, "Too many files open"));
		case	fnfErr:				return (String (String::eReadOnly, "File not found"));
		case	fLckdErr:			return (String (String::eReadOnly, "File is locked"));
		case	fBsyErr:			return (String (String::eReadOnly, "File is busy (delete)"));
		case	dupFNErr:			return (String (String::eReadOnly, "Duplicate file name"));
		case	memFullErr:			return (String (String::eReadOnly, "Not enough room in heap zone"));
		case	resNotFound:		return (String (String::eReadOnly, "Resource not found"));
		case	resFNotFound:		return (String (String::eReadOnly, "Resource file not found"));
#endif	/*qIncludeErrorStrings*/
		default: {
			StringStream	buf;
			buf << "Unknown OS error #" << macOSError << "";
			return (String (buf));
		}
	}
	AssertNotReached (); return (kEmptyString);
}
#endif	/*qMacOS*/

void	Application::PullSelfToFrontOnStartup ()
{
#if		qMacOS
	/*
	 * Technote #180 - Multifinder Miscelanea recomends a few calls to _EventAvail
	 * to get the application pulled to the front, before displaying any splash screen.
	 * This is only necessary to call if the 'canBackground' bit is set in the SIZE resource.
	 */
	for (int i = 1; i <= 3; i++) {
		(void)EventAvailable ();
	}
#endif	/*qMacOS*/
}

void	Application::ProcessSuspend (const osEventRecord& eventRecord)
{
	Assert (fInForeground);
	fInForeground = False;
	EventHandler::ProcessSuspend (eventRecord);
}

void	Application::ProcessResume (const osEventRecord& eventRecord)
{
	Assert (not fInForeground);
	fInForeground = True;
	MenuOwner::SetMenusOutOfDate ();
	EventHandler::ProcessResume (eventRecord);
}

Boolean	Application::CriticallyLowOnMemory ()
{
	if (fCheeseBuf == Nil) {
		AllocCheeseBuf ();
	}
	return Boolean (fCheeseBuf == Nil);
}

void	Application::CheckAlloc (size_t amount)
{
	/*
	 * Set the outofmemory proc to nil so we dont use the cheesebuf in our estimation if we have
	 * enuf memory. Then alloc memory, and see if we can get it. Be careful to restore
	 * the memory, and outofmemory proc.
	 *
	 *	NB: Not too sure its a good idea to do this save/switch/restore of out of mem proc.
	 *		may want to leave it there, and just rely on CriticallyLowOnMemory () call to
	 *		see if we lost our cheesebuf. The reason is, if someone else has hooked in, and
	 *		has some other stuff they would like to free up to make this work, they then can
	 *		do that in their UrgentMemoryNeed() method override. Advantage of save/restore here
	 *		is that it saves the effort of allocing and freeing the memory, and leaves us vulnerable
	 *		to serious memory shortages for less time (not sure thats an issue). Also, those
	 *		may just be more convienient semantics for this function - if a user really wants to
	 *		do what I said, he can deal with running out of memory when get calls this function
	 *		and it fails....
	 */
	volatile	Boolean (*savedMemProc) (size_t)	=	GetOutOfMemoryProc ();
	SetOutOfMemoryProc (Nil);

	void*	p	=	Nil;
	Try {
		p =	new char [amount];
		if (CriticallyLowOnMemory ()) {
			gMemoryException.Raise ();
		}
	}
	Catch () {
		if (p != Nil) {
			delete (p);
		}
#if 	qMacOS
		SetOutOfMemoryProc (savedMemProc);
#endif	/*qMacOS*/
		gMemAllocException.Raise (amount);
	}
	delete (p);
#if 	qMacOS
	SetOutOfMemoryProc (savedMemProc);
#endif	/*qMacOS*/
}

void	Application::ChideUserAboutLowMemory ()
{
	// try to alert the user- if we cannot - too bad...
	// also use new to make cleaning up the dialog object easier - really BETTER: we should keep tiny
	// mem stash here so this wont fail!!!
	Alert*	a	=	Nil;
	Try {
		Alert*	a	=	new Alert ("Very low on memory, try closing some windows, or use the Finder's"
									" `GetInfo' to give the application more memory.", AbstractPushButton::kOKLabel);
		AssertNotNil (a);
		a->Pose ();
		delete (a);
	}
	Catch () {
		delete (a);
	}
}

Boolean	Application::UrgentMemoryNeed (size_t /*bytesNeeded*/)
{	
#if		qDebug
	DebugMessage ("Application::UrgentMemoryNeed");
#endif	/*qDebug*/
	if (fCheeseBuf == Nil) {
		return (False);					// could free nothing up
	}
	else {
		delete (fCheeseBuf); fCheeseBuf = Nil;
		fLastLoMemoryChideAt = Time (0);
		return (True);					// could free up a little
	}
}

void	Application::SetCheeseBufSize (size_t bytesNeeded)
{
	fLastLoMemoryChideAt = Time (0);
	delete (fCheeseBuf); fCheeseBuf = Nil;
	fCheeseBufSize = bytesNeeded;
	AllocCheeseBuf ();
}

Boolean	Application::UrgentMemoryNeed_ (size_t bytesNeeded)
{
	AssertNotNil (sThe);
	return (sThe->UrgentMemoryNeed (bytesNeeded));
}

void	Application::AllocCheeseBuf ()
{
	// avoid pointless, and possibly confusing call on OutOfMemory proc.
	Boolean (*savedMemProc) (size_t)	=	GetOutOfMemoryProc ();
	SetOutOfMemoryProc (Nil);

	Try {
		if (fCheeseBuf == Nil) {
			fCheeseBuf = new char [fCheeseBufSize];
		}
	}
	Catch () {
		// do nothing
	}

	SetOutOfMemoryProc (savedMemProc);
}

#if		qSupportClipBoardWindow
Window*	Application::GetClipBoardWindow () const
{
	EnsureNotNil (fClipBoardWindow);
	return (fClipBoardWindow);
}

void	Application::SetClipBoardWindow (Window* newWindow)
{
	RequireNotNil (newWindow);
	if (fClipBoardWindow != newWindow) {
		if (fClipBoardWindow != Nil) {
			delete (fClipBoardWindow); fClipBoardWindow = Nil;
		}
		fClipBoardWindow = newWindow;
	}
	EnsureNotNil (fClipBoardWindow);
}

Boolean	Application::ClipBoardShown () const
{
	return (GetClipBoardWindow ()->GetVisible ());
}

void	Application::ShowClipBoard (Boolean visible)
{
	if (visible) {
		GetClipBoardWindow ()->Select ();
	}
	else {
		GetClipBoardWindow ()->SetVisible (not View::kVisible);
	}
}
#endif	/*qSupportClipBoardWindow*/

#if		qMacOS
UInt32		Application::GetApplicationCreator () const
{
	// code derived from:	Apple Macintosh Developer Technical Support
	//						Collection of Utilities for DTS Sample code
	short		savedResFile;
	osHandle	returnHandle;

	savedResFile = ::CurResFile ();
	::UseResFile (*(short*)CurApRefNum);	// could use GetAppParms () instead of global reference
	returnHandle = ::Get1IndResource ('BNDL', 1);
	::UseResFile (savedResFile);
	return ((returnHandle == Nil)? '????': *(OSType *) (*returnHandle));
}

PixelMap	Application::GetApplicationICON () const
{
	AssertNotReached ();		// can be gotton by looking through the bundle resource,
								// getting the icon ID, and getting that resource...
}
#endif	/*qMacOS*/



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

