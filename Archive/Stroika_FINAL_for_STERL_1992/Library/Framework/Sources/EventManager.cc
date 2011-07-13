/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/EventManager.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *		-	On the mac, calculate fSleepTicks from members of idle task priorities...
 *
 *
 * Changes:
 *	$Log: EventManager.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/14  19:53:20  lewis
 *		Change Time usage to reflect changes made in the interface.
 *
 *		Revision 1.2  1992/07/02  04:51:45  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.37  92/05/18  15:28:05  15:28:05  lewis (Lewis Pringle)
 *		Fix return type of SetKeyComposeState (). And, fix minor compiler warnings.
 *		
 *		Revision 1.35  92/04/24  08:59:02  08:59:02  lewis (Lewis Pringle)
 *		Added mac disk insertion event code - not tested - and X code to look at each event type, and decide on each
 *		one wther or not to call XtDispatchEvent. Should document each descision.
 *		
 *		Revision 1.34  92/04/20  14:30:47  14:30:47  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		
 *		Revision 1.33  92/04/07  10:17:09  10:17:09  lewis (Lewis Pringle)
 *		Temporarily set sleepticks back to zero while doing performance testing. Once we have Stroika tuned again, see if upping it
 *		causes any problems (mac).
 *		
 *		Revision 1.32  92/03/22  21:26:19  21:26:19  lewis (Lewis Pringle)
 *		Tried to cleanup the fSleepCount code for the mac, but it needs more research, and is low priority...
 *		
 *		Revision 1.31  1992/03/19  16:45:25  lewis
 *		Added Boolean control variable to suspend/resume idle task processing.
 *
 *		Revision 1.29  1992/03/10  13:03:53  lewis
 *		Changed interface to ProcessKey () to support new keyboard stuff. Still preliminary, and
 *		ad-hoc.
 *
 *		Revision 1.27  1992/02/20  14:01:33  lewis
 *		Fixed mac compile bug.
 *
 *		Revision 1.26  1992/02/19  22:26:37  lewis
 *		Code cleanups, and added Added default event handler for Xt so by default at end of chain we just call
 *		XtDispatchEvent. Hacked Activeate/ReorderActive... to keep other guys out
 *		of last position.
 *
 *		Revision 1.25  1992/02/12  06:56:32  lewis
 *		Switch from OSConfig to GDIConfig
 *
 *		Revision 1.24  1992/01/27  23:36:16  lewis
 *		Add ProcessInterapplicationCommunications () method to EventHandler, and dispatch to it from EventManager::ProcessEvent (under X).
 *
 *		Revision 1.23  1992/01/20  05:41:13  lewis
 *		Delete ExposeCallback, and change IdleTimeCallback from WorkProc to TimeOutProc
 *		Also, moved fAppContext initialization from Application here.
 *
 *		Revision 1.21  1992/01/08  22:25:13  lewis
 *		Ported to mac.
 *
 *		Revision 1.20  1992/01/08  05:43:51  lewis
 *		Redesigned EventManager - broke into seperate classes EventManager and
 *		EventHandler, and implemented new activation scheme. Got all working
 *		under UNIX - still to test/debug on Mac.
 *
 *		Revision 1.18  1992/01/03  18:09:50  lewis
 *		More portable click/doubleclick support (support motif).
 *
 *
 */



#include	<stdlib.h>				// for exit

#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Desk.h>
#include	<Dialogs.h>
#include	<DiskInit.h>
#include	<Events.h>
#include	<OSUtils.h>
#include	<QuickDraw.h>
#include	<stdlib.h>
#include	<SysEqu.h>
#include	<ToolUtils.h>			// just for HiWord/LoWord
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#define String osString
#define Boolean osBoolean
#define KeyCode osKeyCode
#include	<X11/keysym.h>
#endif	/*GDI*/

#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"GDIConfiguration.hh"
#include	"KeyBoard.hh"

#include	"DeskTop.hh"

#include	"EventManager.hh"




#if		qMacOS
/*
 * I thought at one time this might be needed when doing stuff with GDevices - not safe
 * to leave it set to out gdevice?/gworlds?
 */
#define	qResetPortAndGDeviceBeforeSwitchout	0
#endif	/*qMacOS*/







#if		qXGDI
extern	osDisplay*      gDisplay;
#endif


static	KeyBoard	GetAdjustedKeyBoard (const osEventRecord& eventRecord);
static	Boolean		IsDblClick (const Point& oldWhere, const Point& newWhere, const Time& timeDiff);




#if		!qRealTemplatesAvailable
	Implement (Iterator, EventHandlerPtr);
	Implement (Collection, EventHandlerPtr);
	Implement (AbSequence, EventHandlerPtr);
	Implement (Array, EventHandlerPtr);
	Implement (Sequence_Array, EventHandlerPtr);
	Implement (Sequence, EventHandlerPtr);
#endif



#if		qXtToolkit
// Put this event handler at the end of the chain, and call it last - by default it just sends stuff to Xt
class	DefaultEventHandler : public EventHandler {
	public:
		DefaultEventHandler ();
		~DefaultEventHandler ();

		override	void	ProcessUpdate (const osEventRecord& eventRecord, Window& window, const Region& updateRegion);
		override	void	ProcessInterapplicationCommunications (const osEventRecord& eventRecord);
};
#endif





/*
 ********************************************************************************
 *********************************** EventHandler *******************************
 ********************************************************************************
 */

Sequence(EventHandlerPtr)		EventHandler::sHandlers;
Sequence(EventHandlerPtr)		EventHandler::sActiveHandlers;

EventHandler::EventHandler ()
{
	sHandlers.Append (this);
}

EventHandler::~EventHandler ()
{
	Require (not Activated ());		// de-activate before we are destroyed
									// since we make this check we dont need to explicily examine the sActiveHandlers list
	sHandlers.Remove (this);
}

void	EventHandler::Activate ()
{
#if		qXtToolkit
	if (sActiveHandlers.GetLength () == 0) {
		sActiveHandlers.Append (this);
	}
	else {
		sActiveHandlers.InsertAt (this, sActiveHandlers.GetLength ());		// Keep DefaultEventHandler last...
	}
#else
	sActiveHandlers.Append (this);											// last in event chain by default
#endif
	Ensure (Activated ());
}

void	EventHandler::Deactivate ()
{
	sActiveHandlers.Remove (this);		// last in event chain by default
}

Boolean	EventHandler::Activated ()
{
	return (sActiveHandlers.Contains (this));
}

void	EventHandler::ReorderActiveList (CollectionSize index)
{
	sActiveHandlers.Remove (this);
	sActiveHandlers.InsertAt (this, index);
	Ensure (sActiveHandlers.IndexOf (this) == index);
}

void	EventHandler::ReorderActiveList (EventHandler* neighbor, AddMode addMode)
{
	RequireNotNil (neighbor);
	Require (this != neighbor);

	CollectionSize	index = sActiveHandlers.IndexOf (neighbor);
	CollectionSize	oldIndex = sActiveHandlers.IndexOf (this);

	Require (index != kBadSequenceIndex);
	Require (oldIndex != kBadSequenceIndex);

	if (oldIndex < index) {
		index--;
	}
	ReorderActiveList ((addMode == eAppend)? index + 1 : index);
}

CollectionSize	EventHandler::GetActiveListLength () const
{
	return (sActiveHandlers.GetLength ());
}

void	EventHandler::ProcessMousePress (const osEventRecord& eventRecord, const Point& where, Boolean isDown, const KeyBoard& keyBoardState, UInt8 clickCount)
{
	EventHandler*	next	=	Next ();
	if (next != Nil) {
		next->ProcessMousePress (eventRecord, where, isDown, keyBoardState, clickCount);
	}
}

void	EventHandler::ProcessMouseMoved (const osEventRecord& eventRecord, const Point& newLocation,
										const KeyBoard& keyBoardState, Region& newMouseRegion)
{
	EventHandler*	next	=	Next ();
	if (next != Nil) {
		next->ProcessMouseMoved (eventRecord, newLocation, keyBoardState, newMouseRegion);
	}
}

void	EventHandler::ProcessKey (const osEventRecord& eventRecord, KeyBoard::KeyCode keyCode,
									Boolean isUp, const KeyBoard& keyBoardState, KeyComposeState& composeState)
{
	EventHandler*	next	=	Next ();
	if (next != Nil) {
		next->ProcessKey (eventRecord, keyCode, isUp, keyBoardState, composeState);
	}
}

void	EventHandler::ProcessUpdate (const osEventRecord& eventRecord, Window& window, const Region& updateRegion)
{
	EventHandler*	next	=	Next ();
	if (next != Nil) {
		next->ProcessUpdate (eventRecord, window, updateRegion);
	}
}

void	EventHandler::ProcessSuspend (const osEventRecord& eventRecord)
{
	EventHandler*	next	=	Next ();
	if (next != Nil) {
		next->ProcessSuspend (eventRecord);
	}
}

void	EventHandler::ProcessResume (const osEventRecord& eventRecord)
{
	EventHandler*	next	=	Next ();
	if (next != Nil) {
		next->ProcessResume (eventRecord);
	}
}

void	EventHandler::ProcessImportClipboard (const osEventRecord& eventRecord)
{
	EventHandler*	next	=	Next ();
	if (next != Nil) {
		next->ProcessImportClipboard (eventRecord);
	}
}

void	EventHandler::ProcessExportClipboard (const osEventRecord& eventRecord)
{
	EventHandler*	next	=	Next ();
	if (next != Nil) {
		next->ProcessExportClipboard (eventRecord);
	}
}

void	EventHandler::ProcessInterapplicationCommunications (const osEventRecord& eventRecord)
{
	EventHandler*	next	=	Next ();
	if (next != Nil) {
		next->ProcessInterapplicationCommunications (eventRecord);
	}
}

void	EventHandler::HandleSystemError (UInt16 errorCode)
{
	EventHandler*	next	=	Next ();
	if (next != Nil) {
		next->HandleSystemError (errorCode);
	}
}

EventHandler*	EventHandler::Next ()
{
	Iterator(EventHandlerPtr)&	it	=	*EventManager::Get ().fCurrentDispatchIterator;
	AssertNotNil (&it);
	Assert (it.Current () == this);

	if (it.Done ()) {
		return (Nil);
	}
	else {
		it.Next ();

// not sure why needed but crashes otherwise...
if (it.Done ()) {
return (Nil);
}
		return (it.Current ());
	}
}






/*
 ********************************************************************************
 ************************************* EventManager *****************************
 ********************************************************************************
 */

EventManager*			EventManager::sThe = Nil;

#if		qDebug
Boolean					EventManager::sPrintIncommingEventsFlag = False;
#endif

#if		qXtToolkit
struct _XtAppStruct*	EventManager::fAppContext = Nil;
#endif



EventManager::EventManager ():
	fIdleTaskSuspendsCount (0),
	fPeriodicTaskQueue (),
	fKeyComposeState (),
	fMouseRgn (kEmptyRegion),
	fCurrentEventRecord (Nil),
	fClickCount (0),
#if		qMacToolkit
	fSleepTicks (6),							//	pick a number, any number (really should
												//	choose more inteligently, based on items in idle task Q)
#endif	/*qMacToolkit*/
	fMouseRgn (kEmptyRegion)
#if		qXtToolkit
	,fDefaultEventHandler (Nil)
#endif
{
	Assert (sThe == Nil);
	sThe = this;
	fCurrentEventRecord = new osEventRecord;
#if		qMacOS
	(*(ResumeProcPtr*)ResumeProc) = &ResumeProcedure;
#endif

#if		qXtToolkit
	extern	struct _XtAppStruct*	gAppContext;
	fAppContext = gAppContext;
	::XtAppAddTimeOut (fAppContext, 0, (XtTimerCallbackProc)IdleTimeProc, (XtPointer)this);
#endif	/*qXtToolkit*/

#if		qXtToolkit
	fDefaultEventHandler = new DefaultEventHandler ();
#endif

#if		qMacToolkit
	if (OSConfiguration ().RunningSystemSix ()) {
		fSleepTicks = 0;							// MUST BE ZERO FOR SOME SYS-6 MFs - ???
	}
// LGP March 20, 1992??????
fSleepTicks = 0;// hack to see if runs faster...menu fixups???
fSleepTicks = 1;// it did make things much faster - try another number - then maybe investigate??? Maybe we need
				// to call eventavail before calling WaitNextEvent, and if none available, process idle/menus, and then do this?
fSleepTicks = 0;
#endif	/*qMacToolkit*/
}

EventManager::~EventManager ()
{
	Require (sThe == this);
	sThe = Nil;

	Require (fIdleTaskSuspendsCount == 0);		// be sure suspends matched with resumes...
#if		qXtToolkit
	delete (fDefaultEventHandler);
#endif
}

EventManager&	EventManager::Get ()
{
	EnsureNotNil (sThe);
	return (*sThe);
}

void	EventManager::RunOnce ()
{
	if (not ProcessNextEvent ()) {
		RunIdleTasks ();
	}

#if		qMacToolkit
	/*
	 * This is a poor workaround for a problem that I don't totally understand with
	 * mouseMoved, and Nil events in system 6 (MF and plain finder).
	 * See also, EventManager::ProcessNextEvent () for related kludge.
	 */
	if (OSConfiguration ().RunningSystemSix ()) {
		// otherwise count on getting the mouse moved events...
		Iterator(EventHandlerPtr)*	savedIterator	=	fCurrentDispatchIterator;
		fCurrentDispatchIterator = EventHandler::sActiveHandlers;
		AssertNotNil (fCurrentDispatchIterator);
		Assert (not fCurrentDispatchIterator->Done ());			// at least it would be pretty wield....
			osEventRecord eventRecord;
			memset (&eventRecord, 0, sizeof (eventRecord));
			fCurrentDispatchIterator->Current ()->ProcessMouseMoved (eventRecord, GetMouseLocation (), GetKeyBoard (), fMouseRgn);
		AssertNotNil (fCurrentDispatchIterator);
		delete (fCurrentDispatchIterator);
		fCurrentDispatchIterator = savedIterator;
	}
#endif	/*qMacToolkit*/
}

Boolean	EventManager::GetNextEvent (osEventRecord& eventRecord)
{
#if		qMacOS
	static	Boolean		hasWNE	=	True;		// really must check osconfig

#if		qResetPortAndGDeviceBeforeSwitchout
	::SetPort (*(osGrafPort**)WMgrPort);
	if (GDIConfiguration ().ColorQDAvailable ()) {
		::SetGDevice (::GetMainDevice ());
	}
#endif

	if (hasWNE) {
		if (::WaitNextEvent (everyEvent, &eventRecord, fSleepTicks, fMouseRgn.GetOSRegion ())) {
			*fCurrentEventRecord = eventRecord;
			return (True);
		}
	}
	else {
		::SystemTask ();
		if (::GetNextEvent (everyEvent, &eventRecord)) {
			*fCurrentEventRecord = eventRecord;
			return (True);
		}
	}
	return (False);
#elif	qXGDI
	::XtAppNextEvent (fAppContext, &eventRecord);
	*fCurrentEventRecord = eventRecord;
#if		qDebug
	if (GetPrintIncommingEventsFlag ()) {
		switch (eventRecord.type) {
			case	KeyPress:			gDebugStream << "Got <KeyPress> event" << newline; break;
			case	KeyRelease:			gDebugStream << "Got <KeyRelease> event" << newline; break;
			case	ButtonPress:		gDebugStream << "Got <ButtonPress> event" << newline; break;
			case	ButtonRelease:		gDebugStream << "Got <ButtonRelease> event" << newline; break;
			case	MotionNotify:		gDebugStream << "Got <MotionNotify> event" << newline; break;
			case	EnterNotify:		gDebugStream << "Got <EnterNotify> event" << newline; break;
			case	LeaveNotify:		gDebugStream << "Got <LeaveNotify> event" << newline; break;
			case	FocusIn:			gDebugStream << "Got <FocusIn> event"<< newline; break;
			case	FocusOut:			gDebugStream << "Got <FocusOut> event" << newline; break;
			case	KeymapNotify:		gDebugStream << "Got <KeymapNotify> event" << newline; break;
			case	Expose:				gDebugStream << "Got <Expose> event" << newline; break;
			case	GraphicsExpose:		gDebugStream << "Got <GraphicsExpose> event" << newline; break;
			case	NoExpose:			gDebugStream << "Got <NoExpose> event" << newline; break;
			case	VisibilityNotify:	gDebugStream << "Got <VisibilityNotify> event" << newline; break;
			case	CreateNotify:		gDebugStream << "Got <CreateNotify> event" << newline; break;
			case	DestroyNotify:		gDebugStream << "Got <DestroyNotify> event" << newline; break;
			case	UnmapNotify:		gDebugStream << "Got <UnmapNotify> event" << newline; break;
			case	MapNotify:			gDebugStream << "Got <MapNotify> event" << newline; break;
			case	MapRequest:			gDebugStream << "Got <MapRequest> event" << newline; break;
			case	ReparentNotify:		gDebugStream << "Got <ReparentNotify> event" << newline; break;
			case	ConfigureNotify:	gDebugStream << "Got <ConfigureNotify> event" << newline; break;
			case	ConfigureRequest:	gDebugStream << "Got <ConfigureRequest> event" << newline; break;
			case	GravityNotify:		gDebugStream << "Got <GravityNotify> event" << newline; break;
			case	ResizeRequest:		gDebugStream << "Got <ResizeRequest> event" << newline; break;
			case	CirculateNotify:	gDebugStream << "Got <CirculateNotify> event" << newline; break;
			case	CirculateRequest:	gDebugStream << "Got <CirculateRequest> event" << newline; break;
			case	PropertyNotify:		gDebugStream << "Got <PropertyNotify> event" << newline; break;
			case	SelectionClear:		gDebugStream << "Got <SelectionClear> event" << newline; break;
			case	SelectionRequest:	gDebugStream << "Got <SelectionRequest> event" << newline; break;
			case	SelectionNotify:	gDebugStream << "Got <SelectionNotify> event" << newline; break;
			case	ColormapNotify:		gDebugStream << "Got <ColormapNotify> event" << newline; break;
			case	ClientMessage:		gDebugStream << "Got <ClientMessage> event" << newline; break;
			case	MappingNotify:		gDebugStream << "Got <MappingNotify> event" << newline; break;
			default:					gDebugStream << "Got <Unknown type: " << eventRecord.type << "> event" << newline; break;
		}
	}
#endif	/*qDebug*/
	return (True);
#endif	/*Toolkit*/
}

Boolean	EventManager::EventAvailable ()	const
{
#if		qMacToolkit
#if		qResetPortAndGDeviceBeforeSwitchout
	::SetPort (*(osGrafPort**)WMgrPort);
	if (GDIConfiguration ().ColorQDAvailable ()) {
		::SetGDevice (::GetMainDevice ());
	}
#endif	/*qResetPortAndGDeviceBeforeSwitchout*/
	osEventRecord	eventRecord;
	return Boolean (::EventAvail (everyEvent, &eventRecord));
#else	/*Toolkit*/
	AssertNotReached (); return (True);
#endif	/*Toolkit*/
}

Boolean	EventManager::ProcessNextEvent ()
{
	/*
	 * According to IM I-279, activate events are the hightest priority events.  Therefore, since
	 * we ignore mac activation messages, we call our own routine to check if activations need
	 * to be done before checking for any other sort of event.
	 */
	DeskTop::Get ().HandleActivation ();

	osEventRecord	eventRecord;
	if (GetNextEvent (eventRecord)) {
		ProcessEvent (eventRecord);
#if		qMacToolkit
		/*
		 * Work around bug/funny behavior in system 6 multifinder. Seems that we constantly
		 * get mousemoved events, and never get Nil events. Maybe particular multifinder that
		 * is the trouble, but this change will work fine regardless. LGP 1/31/91.
		 */
		if (OSConfiguration ().RunningSystemSix ()) {
			if ((eventRecord.what == osEvt) and ((UInt32 (eventRecord.message) >> 24) == mouseMovedMessage)) {
				return (False);	// so we do idle time stuff
			}
		}
#endif	/*qMacToolkit*/
		return (True);
	}
	return (False);
}

void	EventManager::ProcessEvent (const osEventRecord& eventRecord)
{
	Iterator(EventHandlerPtr)*	savedIterator	=	fCurrentDispatchIterator;
	fCurrentDispatchIterator = EventHandler::sActiveHandlers;
	AssertNotNil (fCurrentDispatchIterator);

	if (not fCurrentDispatchIterator->Done ()) {
		EventHandler*	first	=	fCurrentDispatchIterator->Current ();
		AssertNotNil (first);

#if		qMacToolkit
		switch (eventRecord.what)	{
			case mouseUp: {
				first->ProcessMousePress (eventRecord, Point (eventRecord.where.v, eventRecord.where.h), False,
					GetAdjustedKeyBoard (eventRecord), fClickCount);
			}
			break;
			case mouseDown: {
				static	osEventRecord	sLastMousePress;	// inited to zero
				Point	newMouseLoc = os_cvt (eventRecord.where);
	
				if ((sLastMousePress.what == mouseDown) and
					IsDblClick (newMouseLoc, os_cvt (sLastMousePress.where), Time (BigReal (eventRecord.when - sLastMousePress.when)/60.0))) {
					fClickCount++;
				}
				else {
					fClickCount = 1;
				}
				sLastMousePress = eventRecord;
	
				first->ProcessMousePress (eventRecord, newMouseLoc, True, GetAdjustedKeyBoard (eventRecord), fClickCount);
			}
			break;
			case keyUp: {
				KeyBoard keys = GetAdjustedKeyBoard (eventRecord);
				keys.SetKey ((KeyBoard::KeyCode) ((eventRecord.message & keyCodeMask) >> 8), True);
				first->ProcessKey (eventRecord, (KeyBoard::KeyCode) ((eventRecord.message & keyCodeMask) >> 8), True, keys, fKeyComposeState);
			}
			break;
			case keyDown:
			case autoKey: {
				KeyBoard keys = GetAdjustedKeyBoard (eventRecord);
				keys.SetKey ((KeyBoard::KeyCode) ((eventRecord.message & keyCodeMask) >> 8), True);
				first->ProcessKey (eventRecord, (KeyBoard::KeyCode) ((eventRecord.message & keyCodeMask) >> 8), False, keys, fKeyComposeState);
			}
			break;
	
			case updateEvt: {
				osWindowPtr theOSWindow	= osWindowPtr (eventRecord.message);
				AssertNotNil (theOSWindow);
				Assert (osWindowPeek (theOSWindow)->windowKind == userKind);
				Window* aWindow = (Window*) ::GetWRefCon (theOSWindow);
				AssertNotNil (aWindow);
				Assert (aWindow->GetOSRepresentation () == theOSWindow);
				first->ProcessUpdate (eventRecord, *aWindow, aWindow->GlobalToLocal (osWindowPeek (theOSWindow)->updateRgn));
			}
			break;
			case diskEvt:			{
				// untested code - snarfed from the net - try sometime... whatever it does, better than ntohing???
				// LGP April 23, 1992
				if (HiWord (eventRecord.message) != 0) {
					osPoint	where;
					where.h	= ((qd.screenBits.bounds.right -  qd.screenBits.bounds.left)/ 2) - (304 / 2);
					where.v = ((qd.screenBits.bounds.bottom - qd.screenBits.bounds.top)/ 3)  - (104 / 2);
					::InitCursor();
					(void)::DIBadMount (where,  eventRecord.message);
				}

			}
			break;
			case activateEvt:		/* ignore - do ourselves? */		break;
			case osEvt:	{
				switch (UInt32 (eventRecord.message) >> 24) {
					case	suspendResumeMessage: {
						Boolean		switchingIn			= Boolean (eventRecord.message & 1);
						Boolean		convertClipboard	= Boolean (!!(eventRecord.message & 2));
						fMouseRgn = kEmptyRegion;	// cuz macapp did???
						if (switchingIn) {
							if (convertClipboard) {
								Iterator(EventHandlerPtr)*	saved2	=	fCurrentDispatchIterator;
								fCurrentDispatchIterator = EventHandler::sActiveHandlers;
								AssertNotNil (fCurrentDispatchIterator);
								Assert (not fCurrentDispatchIterator->Done ());
								
								fCurrentDispatchIterator->Current ()->ProcessImportClipboard (eventRecord);
								
								AssertNotNil (fCurrentDispatchIterator);
								delete (fCurrentDispatchIterator);
								fCurrentDispatchIterator = saved2;
							}
							first->ProcessResume (eventRecord);
						}
						else {
							if (convertClipboard) {
								Iterator(EventHandlerPtr)*	saved2	=	fCurrentDispatchIterator;
								fCurrentDispatchIterator = EventHandler::sActiveHandlers;
								AssertNotNil (fCurrentDispatchIterator);
								Assert (not fCurrentDispatchIterator->Done ());
								
								fCurrentDispatchIterator->Current ()->ProcessExportClipboard (eventRecord);
								
								AssertNotNil (fCurrentDispatchIterator);
								delete (fCurrentDispatchIterator);
								fCurrentDispatchIterator = saved2;
							}
							first->ProcessSuspend (eventRecord);
						}
					}
					break;
	
					case mouseMovedMessage: {
						first->ProcessMouseMoved (eventRecord, Point (eventRecord.where.v, eventRecord.where.h), GetAdjustedKeyBoard (eventRecord), fMouseRgn);
					}
					break;
				}
				break;
			}
			break;
	
			default:	/* Ignore other events */
#if 	qDebug
			gDebugStream <<"unknown event type = " << eventRecord.what << newline; 
#endif	/*qDebug*/
			break;
		}
#elif	qXtToolkit
		switch (eventRecord.type) {
			case	KeyPress: {
				first->ProcessKey (eventRecord, eventRecord.xkey.keycode, False, GetAdjustedKeyBoard (eventRecord), fKeyComposeState);
			}
			break;
			case	KeyRelease: {
				first->ProcessKey (eventRecord, eventRecord.xkey.keycode, True, GetAdjustedKeyBoard (eventRecord), fKeyComposeState);
			}
			break;
			case	ButtonPress: {
				static	osEventRecord	sLastMousePress;	// inited to zero
				Point	newMouseLoc	= Point (eventRecord.xbutton.y_root, eventRecord.xbutton.x_root);
	
				if (IsDblClick (newMouseLoc, Point (sLastMousePress.xbutton.y_root, sLastMousePress.xbutton.x_root),
								Time (Real (eventRecord.xbutton.time - sLastMousePress.xbutton.time)/1000.0))) {
					fClickCount++;
				}
				else {
					fClickCount = 1;
				}

				sLastMousePress = eventRecord;
	
				first->ProcessMousePress (eventRecord, newMouseLoc, True, GetAdjustedKeyBoard (eventRecord), fClickCount);
			}
			break;
			case	ButtonRelease: {
				// Pass this directly to the widget - at least for now -
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	MotionNotify: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	EnterNotify: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	LeaveNotify: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	FocusIn: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	FocusOut: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	KeymapNotify: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case Expose: {
				Window*	w	=	Window::OSLookup (eventRecord.xexpose.window);
				if (w == Nil) {
					/*
					 * Not really a problem - could - and probably is a widget window - in which cases we've patched its 
					 * ExposeProc in MotifOSControl...
					 */
					osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
					::XtDispatchEvent (&er);
				}
				else {
					Point	updateOrigin	=	Point (eventRecord.xexpose.y, eventRecord.xexpose.x);
					Point	updateSize		=	Point (eventRecord.xexpose.height, eventRecord.xexpose.width);
					first->ProcessUpdate (eventRecord, *w, Region (Rect (updateOrigin, updateSize)));
				}
			}
			break;
			case	GraphicsExpose: {
#if		qDebug
				gDebugStream << "GraphicsExpose event?" << newline;
#endif
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	NoExpose: {
#if		qDebug && 0
				gDebugStream << "NoExpose event? What a concept!" << newline;
#endif
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	VisibilityNotify: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	CreateNotify: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	DestroyNotify: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	UnmapNotify: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	MapNotify: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	MapRequest: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	ReparentNotify: {
				// Pass this directly to the widget - at least for now - later when we have such a concept in
				// Stroika, we may wish to intercept it...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case ConfigureNotify: {
				Window* ourWin = Window::OSLookup (eventRecord.xconfigure.window);
				if (ourWin != Nil) {
					ourWin->HandleConfigureNotify (eventRecord);
				}
				else {
					// for now let fall thru...
					osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
					::XtDispatchEvent (&er);
				}
			}
			break;
			case ConfigureRequest: {
				// I haven't the foggiest Idea of what this is for (Window managers?). Do default...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	GravityNotify: {
				// I haven't the foggiest Idea of what this is for. Do default...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	ResizeRequest: {
				// I haven't the foggiest Idea of what this is for. Do default...
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	CirculateNotify: {
				// We should do something VERY diffrernt here - keep our stacking order for top level shells in SYNC
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	CirculateRequest: {
				// Do default - dont know what its for?
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	PropertyNotify: {
				// Do default - dont know what its for?
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	SelectionClear: {
				// Do default - dont know what its for?
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	SelectionNotify: {
				// Do default - dont know what its for?
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case	ColormapNotify: {
				// Do default - dont know what its for?
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			case ClientMessage: {
				first->ProcessInterapplicationCommunications (eventRecord);
			}
			break;
			case	MappingNotify: {
				// Do default - dont know what its for?
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
			default: {
#if		qDebug
				gDebugStream << "GOT A ??? EVNET #" << eventRecord.type << newline;
#endif
				osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
				::XtDispatchEvent (&er);
			}
			break;
		  }
#endif	/*Toolkit*/
	}
	AssertNotNil (fCurrentDispatchIterator);
	delete (fCurrentDispatchIterator);
	fCurrentDispatchIterator = savedIterator;
}

void	EventManager::AddIdleTask (PeriodicTask* task)
{
	RequireNotNil (task);
// SB DONE IN PERIODICTASK QUEUE ISTSLEF! Require (not (fPeriodicTaskQueue.Contains (task)));
	fPeriodicTaskQueue.AddTask (task);
}

void	EventManager::RemoveIdleTask (PeriodicTask* task)
{
	RequireNotNil (task);
	fPeriodicTaskQueue.RemoveTask (task);
}

void	EventManager::SuspendIdleTasks ()
{
	Require (fIdleTaskSuspendsCount < 10);		// really ok- just sanity check...
	fIdleTaskSuspendsCount++;
}

void	EventManager::ResumeIdleTasks ()
{
	Require (fIdleTaskSuspendsCount >= 1);		// underflow -- too many resumes - not enuf suspends...
	fIdleTaskSuspendsCount--;
}

Boolean	EventManager::IdleTasksRunnable () const
{
	return (Boolean (fIdleTaskSuspendsCount == 0));
}

void	EventManager::RunIdleTasks ()
{
	const	Time	kPeriodicTaskChunk	=	0.05;
	fPeriodicTaskQueue.Run (kPeriodicTaskChunk);
}

Point	EventManager::GetMouseLocation ()	const
{
#if		qMacToolkit	
	osPoint	osp;
	::GetMouse (&osp);
	::LocalToGlobal (&osp);
	return (Point (osp.v, osp.h));
#elif	qXToolkit
	osWindow		rootReturn	=	Nil;
	osWindow		winReturn	=	Nil;
	int				rootXReturn	=	0;
	int				rootYReturn	=	0;
	int				winXReturn	=	0;
	int				winYReturn	=	0;
	unsigned int	maskReturn	=	0;
	// Ignore return value - only pay attention to root returns - see Oreilly II-354
	(void)::XQueryPointer (gDisplay, DefaultRootWindow (gDisplay),
						   &rootReturn, &winReturn,
						   &rootXReturn, &rootYReturn,
						   &winXReturn, &winYReturn,
						   &maskReturn);
	return (Point (rootYReturn, rootXReturn));
#endif	/*Toolkit*/
}

Boolean	EventManager::GetMousePressed () const
{
#if		qMacOS	
#if		qResetPortAndGDeviceBeforeSwitchout
	::SetPort (*(osGrafPort**)WMgrPort);
	if (GDIConfiguration ().ColorQDAvailable ()) {
		::SetGDevice (::GetMainDevice ());
	}
#endif	/*qResetPortAndGDeviceBeforeSwitchout*/
	return Boolean (::StillDown ());
#elif	qXGDI
	osWindow		rootReturn	=	Nil;
	osWindow		winReturn	=	Nil;
	int				rootXReturn	=	0;
	int				rootYReturn	=	0;
	int				winXReturn	=	0;
	int				winYReturn	=	0;
	unsigned int	maskReturn	=	0;
	// Ignore return value - only pay attention to root returns - see Oreilly II-354
	(void)::XQueryPointer (gDisplay, DefaultRootWindow (gDisplay),
						   &rootReturn, &winReturn,
						   &rootXReturn, &rootYReturn,
						   &winXReturn, &winYReturn,
						   &maskReturn);
	return Boolean (!! (maskReturn & Button1Mask));
#endif	/*GDI*/
}

Boolean	EventManager::MouseStillDown ()	const
{
#if		qMacOS	
#if		qResetPortAndGDeviceBeforeSwitchout
	::SetPort (*(osGrafPort**)WMgrPort);
	if (GDIConfiguration ().ColorQDAvailable ()) {
		::SetGDevice (::GetMainDevice ());
	}
#endif	/*qResetPortAndGDeviceBeforeSwitchout*/
	return Boolean (::osButton ());
#elif	qXGDI
	// really not quite right...
	return (GetMousePressed ());
#endif	/*GDI*/
}

void	EventManager::WaitFor (const Time& delay)
{
#if		qMacToolkit
	if (delay > 0) {
		long	unused;
		::Delay (delay.GetSeconds () * 60, &unused);
	}
#elif	qXToolkit
	// VERY BAD WAY!!!
	Time end = GetCurrentTime () + delay;
	while (GetCurrentTime () < end)
	  ;
#endif	/*Toolkit*/
}

void	EventManager::GetCurrentEvent (osEventRecord& eventRecord)
{
	eventRecord = *fCurrentEventRecord;
}

Time	EventManager::GetDoubleClickTime ()
{
#if		qMacToolkit
	return (Time (Real (GetDblTime ()) / 60.0));
#elif	qXtToolkit
	AssertNotNil (gDisplay);
	return (Time (0, ::XtGetMultiClickTime (gDisplay) * 1000));
#endif	/*Toolkit*/
}

Coordinate	EventManager::GetDoubleClickHysteresis ()
{
	return (3);
}

#if		qDebug
Boolean	EventManager::GetPrintIncommingEventsFlag ()
{
	return (sPrintIncommingEventsFlag);
}

void	EventManager::SetPrintIncommingEventsFlag (Boolean printIncommingEvents)
{
	sPrintIncommingEventsFlag = printIncommingEvents;
}
#endif	/*qDebug*/

KeyComposeState	EventManager::GetKeyComposeState ()
{
	return (fKeyComposeState);
}

void	EventManager::SetKeyComposeState (KeyComposeState keyComposeState)
{
	fKeyComposeState = keyComposeState;
}

#if		qMacOS
pascal	void	EventManager::ResumeProcedure ()
{
	// not a good idea to drop into the debugger here since I think this can only happen
	// if there is no debugger installed, and for the time being, were having trouble detecting
	// whether or not the debugger is installed.

	// otherwise count on getting the mouse moved events...
	Iterator(EventHandlerPtr)*	savedIterator	=	sThe->fCurrentDispatchIterator;
	sThe->fCurrentDispatchIterator = EventHandler::sActiveHandlers;
	AssertNotNil (sThe->fCurrentDispatchIterator);
	Assert (not sThe->fCurrentDispatchIterator->Done ());			// at least it would be pretty wield....
		osEventRecord eventRecord;
		memset (&eventRecord, 0, sizeof (eventRecord));
		sThe->fCurrentDispatchIterator->Current ()->ProcessMouseMoved (eventRecord, sThe->GetMouseLocation (), GetKeyBoard (), sThe->fMouseRgn);
	AssertNotNil (sThe->fCurrentDispatchIterator);
	delete (sThe->fCurrentDispatchIterator);
	sThe->fCurrentDispatchIterator = savedIterator;
}
#endif	/*qMacOS*/

#if		qXtToolkit
void	EventManager::IdleTimeProc (char* client_data, void* intervalID)
{
	if (Get ().IdleTasksRunnable ()) {
		Get ().RunIdleTasks ();
	}

	int delayInMilliseconds = 50;
	(void)::XtAppAddTimeOut (fAppContext, delayInMilliseconds, (XtTimerCallbackProc)IdleTimeProc, client_data);
}
#endif	/*qXtToolkit*/






#if		qXToolkit
/*
 ********************************************************************************
 ********************************* DefaultEventHandler **************************
 ********************************************************************************
 */
DefaultEventHandler::DefaultEventHandler ():
	EventHandler ()
{
	EventHandler::Activate ();
	ReorderActiveList (GetActiveListLength ());		// be last...
}

DefaultEventHandler::~DefaultEventHandler ()
{
	Deactivate ();
}

void	DefaultEventHandler::ProcessUpdate (const osEventRecord& eventRecord, Window& window, const Region& updateRegion)
{
	Assert (eventRecord.type == Expose);
	osEventRecord	er = eventRecord;	// copy since XtDispatchEvent takes non-const arg
	::XtDispatchEvent (&er);
}

void	DefaultEventHandler::ProcessInterapplicationCommunications (const osEventRecord& eventRecord)
{
	Assert (eventRecord.type == ClientMessage);
	osEventRecord	er	=	eventRecord;
	::XtDispatchEvent (&er);
}
#endif	/*qXToolkit*/







/*
 ********************************************************************************
 ******************************* GetAdjustedKeyBoard ****************************
 ********************************************************************************
 */



static	KeyBoard	GetAdjustedKeyBoard (const osEventRecord& eventRecord)
{
	KeyBoard	theKeyBoard	=	GetKeyBoard ();

#if		qMacOS
	theKeyBoard.SetKey (KeyBoard::eControlKey, Boolean (!!(eventRecord.modifiers & controlKey)));
	theKeyBoard.SetKey (KeyBoard::eOptionKey, Boolean (!!(eventRecord.modifiers & optionKey)));
	theKeyBoard.SetKey (KeyBoard::eCapsLockKey, Boolean (!!(eventRecord.modifiers & alphaLock)));
	theKeyBoard.SetKey (KeyBoard::eShiftKey, Boolean (!!(eventRecord.modifiers & shiftKey)));
	theKeyBoard.SetKey (KeyBoard::eCommandKey, Boolean (!!(eventRecord.modifiers & cmdKey)));
#else

	if (eventRecord.xkey.state & ShiftMask) {
		theKeyBoard.SetKey (KeyBoard::eShiftKey, True);
	}
	if (eventRecord.xkey.state & ControlMask) {
		theKeyBoard.SetKey (KeyBoard::eControlKey, True);
	}
#endif	/*qMacOS*/
	return (theKeyBoard);
}




/*
 ********************************************************************************
 ************************************* IsDblClick *******************************
 ********************************************************************************
 */

static	Boolean	IsDblClick (const Point& oldWhere, const Point& newWhere, const Time& timeDiff)
{
	return Boolean ((Distance (oldWhere, newWhere) < EventManager::Get ().GetDoubleClickHysteresis ()) and
					(timeDiff <= EventManager::Get ().GetDoubleClickTime ()));
}







// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

