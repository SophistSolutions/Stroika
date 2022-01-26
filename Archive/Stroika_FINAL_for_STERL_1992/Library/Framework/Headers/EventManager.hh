/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__EventManager__
#define	__EventManager__

/*
 * $Header: /fuji/lewis/RCS/EventManager.hh,v 1.4 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 *		These classes are designed to provide a mostly portable interface to the event dispatching
 *		mechanism.  It is hoped that using this will rarely require peeking at OS specific event
 *		information.  However, we do provide some access and configurabilty, in case this is necessary.
 *
 *		Another important goal was the de-centralization of event processing. In early desings, Appliction would
 *		capture MouseClicks and dispatch them to the Desktop, and it would then pass them along to the appropriate window.
 *		Now the Desktop owns an EventManager privately, as an implementation detail, which captures MouseClicks, and
 *		does the routing itself. Application no-longer has ANY knowledge about this. Similarly for UpdateEvent processing -
 *		it is now handled totally within window.cc.
 *
 *		There is one EventManager - and it manages a list of active EventHandlers arranged in a list. (It was
 *		considered maintaining several lists for different events, and only putting EventHandlers on
 *		lists they were interested in, but the particular lists would be system dependent, and 
 *		it is deemed simpler to just keep one list, and to "declare interest" by just activating itself, and
 *		overriding the appropriate virtual method).
 *
 *		When the EventManager processes an event, it sends it to the first element of the EventHandler list.
 *		The default implemention of EventManager is to pass it along to the next member of this list. Obvously
 *		it is easy to override this behavior (and common too) if a particular EventManager has 'handled' the event.
 *		This mechanism is not as sophisticated as it could be, in that it doesn't allow for different classes of
 *		"grabs" where someone might want to be notified no-matter what, or only under certain circumstances, but
 *		its simplicity is very attractive and time will tell if its functionality is sufficient.
 *
 *
 *		Here is a Trial design that was dismissed, and an example of how it would have worked:
 *	&&&&&&&&& BELOW NOT COORECT - JUST FOR DOC PURPOSES &&&&&&&&&&
 *	ONE DESIGN FOR HOW TO MANAGE THE LISTS: BUT NOT GOOD IDEA CUZ SAVE RESTORE TOO HARD TO DO GOOD
 *	JOB!!!! EASY TODO BAD JOB OF THIS BUT SOMETHING SIMPLER IS IN ODER
 *		Any example of usage would be to grab all mouse events, you would tell the event manager to save its
 *		state, and then do a bringtofront (THIS) - (analogous to an X grab). This might plausibly be done by
 *		a tracker. Then when you are done, you might restore things. The save and restore is optional, but
 *		probably more polite. Save and restore is gaurenteed "safe" in that if anything is destroyed, it is
 *		removed from the "save" lists, and any new EventManagers will not be lost - the are added to "save list too."
 *	&&&&&&&&& ABOVE NOT COORECT - JUST FOR DOC PURPOSES &&&&&&&&&&
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *		Most of the methods of EventManager are virtual so that they CAN be overridden in the users subclass of
 *		Application. This does NOT mean that they should be, and they are VERY rarely, and with largely un-gaurenteed
 *		results. It is allowed only as a hook so that the user CAN - if he must - subvert the Stroika event
 *		dispatch to his needs.
 *
 *
 * Changes:
 *	$Log: EventManager.hh,v $
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.2  1992/07/02  03:50:43  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.18  1992/05/18  15:25:13  lewis
 *		Fix return type of SetKeyComposeState ().
 *
 *		Revision 1.17  92/03/19  16:45:25  16:45:25  lewis (Lewis Pringle)
 *		Added Boolean control variable to suspend/resume idle task processing.
 *		
 *		Revision 1.16  1992/03/10  13:02:56  lewis
 *		Changed interface to ProcessKey (), and added new attribute with getters/setters -
 *		fKeyComposeState. Use that to keep track of state accrosss calls to ProcessKey ().
 *
 *		Revision 1.15  1992/02/19  22:22:32  lewis
 *		Added default event handler for Xt so by default at end of chain we just call XtDispatchEvent.
 *
 *		Revision 1.14  1992/01/27  23:35:30  lewis
 *		Add ProcessInterapplicationCommunications method to EventHandler (right now just used for X's ICCM).
 *
 *		Revision 1.13  1992/01/20  05:40:39  lewis
 *		Delete ExposeCallback, and change IdleTimeCallback from WorkProc to TimeOutProc.
 *
 *		Revision 1.12  1992/01/08  05:35:27  lewis
 *		Redesined eventmanager support. Broke things up into two classes, EventManager, and EventHandler,
 *		and moved some stuff from application to EventManager. Purpose of re-organization was to allow de-centralized EventManagment (eg have window take care of
 *		all code for handling window related events). Also, moved IdleTask support here.
 *
 *		Revision 1.11  1992/01/03  07:15:38  lewis
 *		Added GetDoubleClickTime () and GetDoubleClickHysteresis () accessor functions (snag values from user prefs).
 *
 *
 */

#include	"PeriodicTask.hh"
#include	"Sequence.hh"

#include	"Region.hh"

#include	"Config-Framework.hh"
#include	"KeyBoard.hh"



#if		qMacToolkit
struct	osEventRecord;
#elif	qXtToolkit
union	osEventRecord;
struct	osWidget;
#endif	/*Toolkit*/

class	KeyBoard;
class	Time;
class	Window;



#if		!qRealTemplatesAvailable
	typedef	class EventHandler*	EventHandlerPtr;
	Declare (Iterator, EventHandlerPtr);
	Declare (Collection, EventHandlerPtr);
	Declare (AbSequence, EventHandlerPtr);
	Declare (Array, EventHandlerPtr);
	Declare (Sequence_Array, EventHandlerPtr);
	Declare (Sequence, EventHandlerPtr);
#endif



class	EventHandler {
	protected:
		EventHandler ();

	public:
		virtual ~EventHandler ();

	/*
	 * Being activated, and deactivated means going on and off the EventManagers queue of handlers to
	 * dispatch incoming events. It is a bug to call Activate/DeActivate when you are activated/deactivated.
	 */
	protected:
		nonvirtual	void	Activate ();
		nonvirtual	void	Deactivate ();
		nonvirtual	Boolean	Activated ();

		nonvirtual	void	ReorderActiveList (CollectionSize index = 1);		// by default go to front of list
		nonvirtual	void	ReorderActiveList (EventHandler* neighbor, AddMode addMode = ePrepend);

		nonvirtual	CollectionSize	GetActiveListLength () const;


	/*
	 * These protected virtual methods are called by the EventManager, for the first active EventHandler
	 * on reception of an event, and by default they pass the event along to the next member of the
	 * chain. The first argument is always the os event record that caused the event.
	 */
	protected:
		virtual	void	ProcessMousePress (const osEventRecord& eventRecord, const Point& where, Boolean isDown,
										   const KeyBoard& keyBoardState, UInt8 clickCount);
		virtual	void	ProcessMouseMoved (const osEventRecord& eventRecord, const Point& newLocation,
										   const KeyBoard& keyBoardState, Region& newMouseRegion);
		virtual	void	ProcessKey (const osEventRecord& eventRecord, KeyBoard::KeyCode keyCode, Boolean isUp, const KeyBoard& keyBoardState, KeyComposeState& composeState);
		virtual	void	ProcessUpdate (const osEventRecord& eventRecord, Window& window, const Region& updateRegion);
		virtual	void	ProcessSuspend (const osEventRecord& eventRecord);
		virtual	void	ProcessResume (const osEventRecord& eventRecord);
		virtual	void	ProcessImportClipboard (const osEventRecord& eventRecord);
		virtual	void	ProcessExportClipboard (const osEventRecord& eventRecord);
		virtual	void	ProcessInterapplicationCommunications (const osEventRecord& eventRecord); // for apple events(mac) and ClientMessages(X)

		virtual	void	HandleSystemError (UInt16 errorCode);			// Should be mac only...??>? At least arg must be different to be portable..

	private:
		nonvirtual	EventHandler*	Next ();

		static	Sequence(EventHandlerPtr)	sHandlers;
		static	Sequence(EventHandlerPtr)	sActiveHandlers;		// Frontmost (ie first crack at events) are first in list

	friend	class	EventManager;
};





class	EventManager {
	/*
	 * Application is the only EventManager.
	 */
	protected:
		EventManager ();

	/*
	 * Application is the only eventmanager, and only no-one should delete it thru this interface.
	 * Usually an application (with base class EventManager) is created on the stack in main, and there
	 * deletion is done when it goes out of scope.
	 */
	private:
		virtual ~EventManager ();	


	public:
		static	EventManager&	Get ();


	/*
	 * Perform a quantum of runing, looking for events, and running idle tasks, etc...
	 */
	public:
		virtual	void	RunOnce ();


	/*
	 * Access to events.
	 */
	public:
		virtual	Boolean	GetNextEvent (osEventRecord& eventRecord);
		virtual	Boolean	EventAvailable () const;


	/*
	 * Dispatch the event to the active EventHandlers.
	 */
	protected:
		virtual	Boolean	ProcessNextEvent ();
		virtual	void	ProcessEvent (const osEventRecord& eventRecord);



	/*
	 * Idle task managment.
	 *
	 * Add/Remove idle tasks from the Q. Also, allow for a stack-based suspension of idle tasks.
	 * If you call SuspendIdleTask (), no idle task processing will be called until they are Resume ()'d.
	 * In the event of nested calls, no idle task processing will take place until all Suspend's have been
	 * matched with resumes. It is a error to not match them properly (either forgetting to call Resume or
	 * calling it too many times).
	 *
	 * When idle tasks are disabled, you can call RunIdleTasks () manually - though note is is a protected member.
	 * This is to discourage its casual use, but make it not too difficult to use.
	 */
	public:
		nonvirtual	void	AddIdleTask (PeriodicTask* task);
		nonvirtual	void	RemoveIdleTask (PeriodicTask* task);

		nonvirtual	void	SuspendIdleTasks ();
		nonvirtual	void	ResumeIdleTasks ();

	protected:
		nonvirtual	Boolean	IdleTasksRunnable () const;
		virtual		void	RunIdleTasks ();		// Only called automatically if IdleTasksRunnable ()

	private:
		UInt16				fIdleTaskSuspendsCount;
		PeriodicTaskQueue	fPeriodicTaskQueue;





	/*
	 * Access to mouse location and info. Since some systems have multiple buttons, these are taken to
	 * refer to the main button. If this is insufficient, we may later add a default parameter...
	 */
	public:
		virtual	Point	GetMouseLocation () const;
		virtual	Boolean	GetMousePressed () const;
		virtual	Boolean	MouseStillDown () const;		//	the button is currently down and there are no more mouse events pending (ie same mouse click - not mouse down, up, and down again)


	/*
	 * wait for the specifed amount of time.
	 *		Very unclear what this should do about idle tasks, etc.... What should be allowed to
	 *		run when this happens - must think about threads...
	 */
	public:
		virtual	void	WaitFor (const Time& delay);





nonvirtual	void	GetCurrentEvent (osEventRecord& eventRecord);			// not sure this is necssary anymore...-still is cuz of MotifOSControl - soon we'll fix that
																			// when we generate bogus events...


	/*
	 * GUI User Configuration parameters - probably should go into some other configuration class  - a GUIConfiguration like
	 * OSConfiguration () class in Foundaituon.
	 */
	public:
		virtual	Time		GetDoubleClickTime ();
		virtual	Coordinate	GetDoubleClickHysteresis ();


	/*
	 * Trace events debugging support.
	 */
#if		qDebug
	public:
		static		Boolean	GetPrintIncommingEventsFlag ();
		static		void	SetPrintIncommingEventsFlag (Boolean printIncommingEvents);
	private:
		static		Boolean	sPrintIncommingEventsFlag;
#endif	/*qDebug*/



	public:
		nonvirtual	KeyComposeState	GetKeyComposeState ();
		nonvirtual	void			SetKeyComposeState (KeyComposeState keyComposeState);
	private:
		KeyComposeState	fKeyComposeState;


	/*
	 * Access to these protected members which control how the eventmanager operates is discouraged, but possible.
	 */
	protected:
		Region			fMouseRgn;
		osEventRecord*	fCurrentEventRecord;
		UInt8			fClickCount;
#if		qMacToolkit
		UInt32			fSleepTicks;				/* ticks to give up control on WNE */
#endif	/*qMacToolkit*/


	private:
		static	EventManager*	sThe;

		Iterator(EventHandlerPtr)*	fCurrentDispatchIterator;		// EventHandler is friend so it can access this

#if 	qMacToolkit
		static	pascal	void	ResumeProcedure ();
#endif	/*qMacToolkit*/

#if		qXtToolkit
		static	void	IdleTimeProc (char* client_data, void* intervalID);
#endif	/*qXtToolkit*/


#if		qXtToolkit
		protected:
			static	struct _XtAppStruct*	fAppContext;		// this should probably go elsehwere but not sure yet...
#endif	/*qXtToolkit*/


#if		qXtToolkit
		class	DefaultEventHandler*	fDefaultEventHandler;
#endif

	friend	class	EventHandler;
};







// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__EventManager__*/

