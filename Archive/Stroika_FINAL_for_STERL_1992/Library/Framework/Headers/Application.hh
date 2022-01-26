/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Application__
#define	__Application__

/*
 * $Header: /fuji/lewis/RCS/Application.hh,v 1.3 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *
 *
 * TODO:
 *			-	Forground / background stuff here is rundnent with that in EventManger - get rid of it from here???
 *			-	Seriosly consider getting rid of ClipBoardWindow support from here, and requiring
 *				applications to do it themselves. Most of the real work is done in ClipBoardWindow anyhow,
 *				and this would allow for building smaller applicatins, at VERY modest cost.???
 *
 * Changes:
 *	$Log: Application.hh,v $
 *		Revision 1.3  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.26  1992/03/19  16:38:22  lewis
 *		Break out of RunOnce () the function ProcessPendingCommands (), and add Booleans to Suspend/Resume
 *		processing of Commands. Also, make possible calling the process function, whenever you want (from subclasses).
 *
 *		Revision 1.25  1992/03/13  16:03:00  lewis
 *		Comment headers better. Add method TryToShutDown () and refine semantics of ShutDown (). Delete Realize () method.
 *
 *		Revision 1.24  1992/03/05  17:09:03  lewis
 *		Change menubar support. No longer build a menubar, nor do I
 *		manage the DebugMenu/SystemMenu. Thats up to subclasses. I do (since
 *		I'm in target chain) pass control to DebugMenu/SystemMenu static
 *		member functions to act as approparitely. (mac).
 *
 *		Revision 1.23  1992/02/21  19:34:02  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.22  1992/02/17  22:57:09  lewis
 *		Moved version stuff to seperate foundation class.
 *
 *		Revision 1.21  1992/01/29  06:58:51  lewis
 *		Added SetupMenus idle task.
 *
 *		Revision 1.20  1992/01/28  22:12:27  lewis
 *		No longer inherit from KeyHandler - never really was a reason anyhow except thats how macapp 1.1 worked.
 *		Moved Mouse tracking stuff all to Desktop - got rid of help/cursor rgn and eventhander overrides - all moved to Desktop.
 *
 *		Revision 1.19  1992/01/28  20:54:50  lewis
 *		Got rid of ClipBoard support - all moved to ClipBoard.hh/cc.
 *
 *		Revision 1.18  1992/01/18  09:07:12  lewis
 *		Keep MotifApplicationWindow class ptr instead of shell pointer.
 *
 *		Revision 1.16  1992/01/08  05:31:15  lewis
 *		Support new EventManager/EventHandler design. Inherit from both.
 *		Moved periodic task stuff to EventManager, and started moving other things
 *		that don't belong in Application out, such and the Update mechanism for windows.
 *
 *		Revision 1.15  1992/01/06  09:57:22  lewis
 *		Added idle time support for Xt.
 *
 *		Revision 1.13  1991/12/18  09:03:03  lewis
 *		Use ShellWindow stuff instead of manual creation of shell widgets.
 *
 *
 *
 */

#include	"Exception.hh"
#include	"KeyBoard.hh"
#include	"Time.hh"

#include	"Region.hh"

#include	"EventManager.hh"
#include	"MenuCommandHandler.hh"





#if		qXGDI
struct osWidget;
#endif


class	AbstractMenuBar;
class	Window;
#if		qXGDI
class	MainApplicationShell;
#endif
class	Application	:	public EventManager, public EventHandler, public virtual MenuCommandHandler {
	/*
	 * Application is an abstract class, and has a protected constructor to emphasize that fact.
	 */
	protected:
		Application (int argc, const char* argv []);
	public:
		virtual ~Application ();


	/*
	 * Run () is the top level method that you call to begin the main event processing loop. This is typically
	 * called from main () just after your subclass of Application has been built
	 */
	public:
		nonvirtual	void		Run ();


	/*
	 * RunOnce does a single step thru the main event loop (purposefully left ambiguous what that might mean).
	 * Call this from "deep in your non-interactive code" so that some interactive processing can continue. If
	 * you do so, however, BE FOREWARNED, your code must be RE-ENTRANT. Otherwise, dont mess with this call.
	 */
	public:
		override	void		RunOnce ();


	/*
	 * ShutDown () will immediately shutdown the application, shutting each document, and then blowing out
	 * of the main event loop. This can be dangerous to do at times when there are currently active objects
	 * this pointers on the stack. Because of this, it is a protected method - discouraging casual use.
	 *
	 *
	 * TryToShutDown () is very simmilar, but more friendly. It posts a command to be handled later "when
	 * appropriate, and it calls ShutDown ().
	 */
	protected:
		nonvirtual	void		ShutDown ();
	public:
		nonvirtual	void		TryToShutDown ();


	public:
		nonvirtual	AbstractMenuBar*	GetMenuBar ()	const;
		nonvirtual	void				SetMenuBar (AbstractMenuBar* menuBar);

		nonvirtual	void		ProcessCommand (const CommandSelection& selection);

		nonvirtual	void		PostCommand (Command* postedCommand);
		nonvirtual	Command*	GetCurrentCommand () const;
		nonvirtual	void		FlushPreviousCommand ();	// when can no longer support "Undo"

		/*
		 * These routines are used for unusual command manipulations, such as building
		 * larger compound commands from already executed commands (like TextEdit does 
		 * during key processing. Use with caution!
		 */
		nonvirtual	Command*	GetPreviousCommand () const;
		nonvirtual	void		SetPreviousCommand (Command* command);


	/*
	 * Process any pending commands. This is typically called from RunOnce (), which is, in turn, typically
	 * called from the main event loop "Application::Run".
	 *
	 * Command processing can be temporarily disabled in a stack-based fasion by calling SuspendCommandProcessing (),
	 * and resumed by calling ResumeCommandProcssing(). In the event of nested calls, no command processing will
	 * take place until all Suspend's have been matched with resumes. It is a error to not match them properly
	 * (either forgetting to call Resume or calling it too many times).
	 *
	 * When CommandProcessing is disabled, you can still call ProcessPendingCommands () manually - though note
	 * is is a protected member. This is to discourage its casual use, but make it not too difficult to use.
	 */
	protected:
		nonvirtual	void	ProcessPendingCommands ();

	public:
		nonvirtual	void	SuspendCommandProcessing ();
		nonvirtual	void	ResumeCommandProcessing ();

	protected:
		nonvirtual	Boolean	CommandProcessingEnabled () const;

	private:
		UInt16	fCommandProcessingSuspendCount;



	/*
	 * Menu update and handling support.
	 */
	public:
		override	void		DoSetupMenus ();
		override	Boolean		DoCommand (const CommandSelection& selection);

		/*
		 * Not necessary to call this, called automatically.  Might want to call anyhow
		 * to be sure menus display up-to-date.  NB: it is a no-op if menus uptodate, so
		 * call be called freely.
		 */
		nonvirtual	void		SetupTheMenus ();

	protected:
		/*
		 * called at startup, should be modified to allow opening of files from
		 * the Finder, or printing, but how to choose arguments portably needs 
		 * discussion.
		 */
		virtual		void	Startup ();

		virtual		void	SetupUndoMenu ();
		virtual		void	HandleUndoMenu ();



	/*
	 * Exception Handling.
	 */
	public:
		virtual	void	ShowMessageForException (const Exception& exception);

#if		qMacOS
		nonvirtual	String	OSErrorToString (short macOSError);
#endif

#if		qSupportAutoBusyCursor
	public:
		/*
		 * Code for supporting the busycursor, and Application auto-detecting that the
		 * application is busy, and showing the busy cursor to let the user know, without
		 * elaborate user-program intervention. Another important feature of this mechanism
		 * is that it works properly on hardware of differing speed - you dont get the busy
		 * cursor for 1/10th second on a real fast machine just cuz you would have gotten one
		 * for longer on a slow machine.
		 */
		nonvirtual	void	BusyCursor () const;		// turn on busy cursor
		nonvirtual	Boolean	BusyCursorOn () const;		// Check if on
		nonvirtual	void	NotBusy () const;			// Even though we have not been to the
														// event loop in a while, take this
														// as being equivilent (actually our getnextevent
														// override just calls this)
		// you may need to disable this when you are relinquishing control to some NONStroika piece
		// of code, but under user feedback, and so you dont want to change the cursor. When you
		// set this false, it is up to you (even in the presense of exceptions) to set it back.
		nonvirtual	Boolean	GetAutoBusyCursorEnabled ();
		nonvirtual	void	SetAutoBusyCursorEnabled (Boolean enabled);

		nonvirtual	Time	GetAutoBusyCursorThreshold ();
		nonvirtual	void	SetAutoBusyCursorThreshold (const Time& threshold);

	private:
		class				AutoBusyCurorTask;
		Boolean				fCursorBusy;
		Boolean				fAutoBusyCursorEnabled;		// do autobusy support - often temporarily shutoff
		Time				fAutoBusyCursorThreshold;	// Times longer than this between NotBusy() calls (when were enabled)
														// generate a busycursor call
		Time				fLastTimeNotBusy;			// basically this is the last time of a GetNextEvent () call
		AutoBusyCurorTask*	fAutoBusyCurorTask;
#endif	/*qSupportAutoBusyCursor*/



	/*
	 * Check if the running application is running in the foreground or background. Running
	 * in the foreground means that you are the currently frontmost application that the user
	 * is focusing on.
	 */
	public:
		nonvirtual	Boolean	InBackground () const;
		nonvirtual	Boolean	InForeground () const;


	/*
	 * Force app to front on startup.  Mostly useful to display spash screens, otherwise
	 * unnessesary. Also, generally, and portably useful to say startup stuff done, and
	 * ready to start GUI.
	 */
	protected:
		nonvirtual	void	PullSelfToFrontOnStartup ();



	/*
	 * EventHandler overrides.
	 */
	protected:
		override	void	ProcessSuspend (const osEventRecord& eventRecord);
		override	void	ProcessResume (const osEventRecord& eventRecord);



	/*
	 * Low memory handling.
	 *
	 *		Default implementation is that UrgentMemoryNeed_ () is hooked into the foundation level
	 *	memory manager, and called when some memory request cannot be satisfied.  Then,
	 *	the virtual UrgentMemoryNeed () is called, implementing our 'CheeseBuf' mechanism.
	 *	The application will optionally keep a buffer of size specifyable by SetCheeseBufSize ().
	 *	Then, when a memory request cannot be satisfied, the virtual UrgentMemoryNeed () is called.
	 *	though this can be overriden to provide different behavior, the default is to free
	 *	the chessebuf.  This allows processing to continue without an exception (also, when
	 *	code we have no control over - eg graphix toolbox, segment load, etc. -
	 *	requests memory, we want those to always succeed).  Then, at the next queicent point,
	 *	we alert the user to this low memory condition at periodic intervals, until the problem
	 *	is alieviated.  The problem is defined to be allieviated when we have successfully
	 *	reallocated the CheeseBuf.
	 *
	 *		ChideUserAboutLowMemory () displays a dialog indicating that we are critically
	 *	low on memory, and that something should be done, like closing windows, etc...
	 *
	 *		A word of thanks, this strategy is borrowed - at least in part - from MacApp 2.0.
	 */
	public:
		virtual		Boolean	CriticallyLowOnMemory ();
		virtual		void	CheckAlloc (size_t amount);		// Raise Exception if not enuf memory to alloc
															// This is just a rough estimate - you could still
															// fail to alloc this amount of memory, but in most
															// cases this will allow you to fail earlier, and
															// therefore reduce the risk of leaving things
															// in a bad state.

	protected:
// for now since there is no idle task support, do in main event loop, but later create task...
		virtual		void	ChideUserAboutLowMemory ();
		Time		fLastLoMemoryChideAt;

		virtual		Boolean	UrgentMemoryNeed (size_t bytesNeeded);

		nonvirtual	size_t	GetCheeseBufSize () const;
		nonvirtual	void	SetCheeseBufSize (size_t bytesNeeded);

	private:
		static		Boolean	UrgentMemoryNeed_ (size_t bytesNeeded);
		nonvirtual	void	AllocCheeseBuf ();

		void*		fCheeseBuf;
		size_t		fCheeseBufSize;

#if		qSupportClipBoardWindow
	/*
	 * ClipBoard management.
	 */
	public:
		nonvirtual	Window*	GetClipBoardWindow () const;
		nonvirtual	void	SetClipBoardWindow (Window* newWindow);

		nonvirtual	Boolean	ClipBoardShown () const;
		nonvirtual	void	ShowClipBoard (Boolean visible = True);
	private:
		Window*		fClipBoardWindow;
#endif	/*qSupportClipBoardWindow*/



	/*
	 * Access to info about this application - generally OS dependent.
	 */
	public:
#if		qMacOS
		nonvirtual	UInt32				GetApplicationCreator () const;
		nonvirtual	PixelMap			GetApplicationICON () const;
#endif	/*qMacOS*/

	public:
		static	Application&	Get ();

	private:
		Exception	fThroToEventLoop;

		AbstractMenuBar*	fMenuBar;
		Command*			fCurrentCommand;
		Command*			fPreviousCommand;
		Boolean				fInForeground;

		static	Application*	sThe;

#if		qXGDI
		class	MainAppWindow;
		MainAppWindow*			fAppShellWindow;
#endif	/*qXGDI*/
		class	SetupMenusChecker;
		SetupMenusChecker*		fSetupMenusChecker;
};



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Command*			Application::GetCurrentCommand () const				{	return (fCurrentCommand);				}
inline	Command*			Application::GetPreviousCommand () const			{	return (fPreviousCommand);				}
inline	void				Application::SetPreviousCommand (Command* command)	{	fPreviousCommand = command;				}
inline	Boolean				Application::InBackground () const					{	return (not fInForeground);				}
inline	Boolean				Application::InForeground () const					{	return (fInForeground);					}
inline	size_t				Application::GetCheeseBufSize () const				{	return (fCheeseBufSize);				}
inline	Application&		Application::Get ()									{	AssertNotNil (sThe);	return (*sThe);	}





// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__Application__*/


