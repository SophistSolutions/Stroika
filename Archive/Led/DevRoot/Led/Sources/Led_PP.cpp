/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/Led_PP.cpp,v 2.80 2004/02/10 03:06:01 lewis Exp $
 *
 * Changes:
 *	$Log: Led_PP.cpp,v $
 *	Revision 2.80  2004/02/10 03:06:01  lewis
 *	SPR#1634: Added EnterIdler class to get notified on OnEnterIdle messages. Also - added IdleManager::NonIdleContext class to notify idle-manager of non-idle (eg executing a command) contexts (lots of these usages).
 *	
 *	Revision 2.79  2003/05/21 12:53:12  lewis
 *	SPR#1495: rename UndoableContextHelper::GetSelStart/End to UndoableContextHelper::GetUndoRegionStart/End
 *	
 *	Revision 2.78  2003/05/20 22:41:58  lewis
 *	SPR#1493- react to no more UndoableContextHelper::SetBeforeUndoSelection and use new UndoableContextHelper::CTORs. Use TempMarker fOrigSelection isntead of stack-based originalSelectionMarker. Just mimicked changes I had made to D&D for Led_MFC (still untested)
 *	
 *	Revision 2.77  2003/05/15 12:50:30  lewis
 *	SPR#1487: Add a 'GetEnabled' method to TextInteractor::CommandUpdater and subclasses (to help with SPR)
 *	
 *	Revision 2.76  2003/04/16 14:04:14  lewis
 *	SPR#1437: cosmetic cleanups - nest code and scope in in braces inside of UndoContext instances, and use new TextInteractor::BreakInGroupedCommands () wrapper (and other small cleanups)
 *	
 *	Revision 2.75  2003/04/04 14:41:44  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.74  2003/04/03 16:41:27  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated
 *	command classes, just builtin to TextInteractor/WordProcessor (and instead of template params
 *	use new TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.73  2003/03/28 12:58:37  lewis
 *	SPR#1388: destruction order of IdleManager and OSIdleManagerImpls is arbitrary - since done
 *	by static file scope DTORs. Could work around this other ways (with .h file with ref count
 *	based DTOR - as old days of iostream hack) - but easier to just make IdleManager::SetIdleManagerOSImpl
 *	static method and careful not to recreate IdleManager if setting impl property to NULL
 *	
 *	Revision 2.72  2003/03/20 15:54:32  lewis
 *	SPR#1360 - added IdleManagerOSImpl_Gtk GTK support for new Idler classes (still untested)
 *	
 *	Revision 2.71  2003/03/11 01:39:16  lewis
 *	fix Mac specific code for SPR#1286 - smart cut and paste
 *	
 *	Revision 2.70  2003/02/24 16:57:38  lewis
 *	SPR#1306- support qWideCharacters for MacOS
 *	
 *	Revision 2.69  2002/11/01 15:49:03  lewis
 *	get compiling on mac
 *	
 *	Revision 2.68  2002/05/06 21:33:46  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.67  2001/11/27 00:29:53  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.66  2001/09/26 15:41:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.65  2001/09/12 14:53:31  lewis
 *	SPR#0987- Added conditional UseSecondaryHilight support (macos&win32 only so far). Redo Led_Region
 *	support for MacOS so now more like with PC (no operator conversion to macregion and auto constructs
 *	OSRegion except when called with region arg - in whcih case we dont OWN region and dont delete it on DTOR
 *	
 *	Revision 2.64  2001/09/11 22:29:15  lewis
 *	SPR#1018- preliminary CWPro7 support
 *	
 *	Revision 2.63  2001/09/07 12:45:28  lewis
 *	fix typo in last fix
 *	
 *	Revision 2.62  2001/09/05 16:25:21  lewis
 *	SPR#1016- cleanup semantics of ReaderFlavorPackage::GetFlavorSize/ReaderFlavorPackage::ReadFlavorData
 *	
 *	Revision 2.61  2001/09/05 00:07:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.60  2001/09/05 00:04:35  lewis
 *	SPR#1013- A few small changes to TextInteractor::UndoableContextHelper to enable its use in Led_MFC_
 *	and Led_PP D&D code. And added UndoableContextHelper::SetBeforeUndoSelection () to fix a small trouble
 *	with that code.
 *	
 *	Revision 2.59  2001/08/28 18:43:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.58  2001/08/27 22:24:26  lewis
 *	SPR#0979- fix mac printing (new powerplant)
 *	
 *	Revision 2.57  2001/07/19 02:21:46  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.56  2001/05/01 16:07:43  lewis
 *	fixed bad names - Get/SetCurClickClickCount to just one CLICK
 *	
 *	Revision 2.55  2001/04/17 23:05:30  lewis
 *	Cleanups related to SPR#0871
 *	
 *	Revision 2.54  2000/10/06 13:09:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.53  2000/10/05 17:26:47  lewis
 *	Changed API for SetHasScrollBar to take enum rather than bool - and supported auto-hide/show scrollbars
 *	
 *	Revision 2.52  2000/10/03 21:50:19  lewis
 *	Lots more work relating to SPR#0839- moved stuff into WordProcessorCommonCommandHelper<> template,
 *	and used that better throughout Led_PP code etc. Broke out WP-specific stuff from Led_PP to
 *	separate file.Lots of cleanups of PP wrapper code
 *	
 *	Revision 2.51  2000/09/29 00:15:48  lewis
 *	use GetWindowRect() instead of calcing it from the frame/sbars in computing drop region
 *	
 *	Revision 2.50  2000/09/26 13:55:36  lewis
 *	cleanups, and AdjustCursorSelf now calls Led_MacOS routine
 *	
 *	Revision 2.49  2000/09/25 21:58:16  lewis
 *	Moved most of the draw/layout/scrollbar and (except D&D) mouse handling code from Led_PP to Led_MacOS
 *	
 *	Revision 2.48  2000/09/25 02:14:00  lewis
 *	kill ifdefed code (and fix bad return value
 *	
 *	Revision 2.47  2000/09/25 00:46:05  lewis
 *	Move alot of code from Led_PPView to Led_MacOS_Helper<> template. Soon will move a bunch more.
 *	
 *	Revision 2.46  2000/08/31 20:50:21  lewis
 *	use Led_Pen::kBlackPattern
 *	
 *	Revision 2.45  2000/08/31 15:37:27  lewis
 *	SPR#0828- fixup calls to Pre/PostInteractiveUndoHelper
 *	
 *	Revision 2.44  2000/06/12 16:19:23  lewis
 *	support SPR#0780- Led_Tablet_ support
 *	
 *	Revision 2.43  2000/04/30 14:53:47  lewis
 *	minor tweeks to get compiling on mac
 *	
 *	Revision 2.42  2000/04/16 14:39:37  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.41  2000/04/16 13:58:54  lewis
 *	small tweeks to get recent changes compiling on Mac
 *	
 *	Revision 2.40  2000/04/15 14:32:37  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.39  2000/04/14 22:40:25  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.38  2000/04/04 20:49:57  lewis
 *	SPR# 0732- added  Led_Set_OutOfMemoryException_Handler/Led_Set_BadFormatDataException_Handler/
 *	Led_Set_ThrowOSErrException_Handler. And qGenerateStandardPowerPlantExceptions and qGenerateStandardMFCExceptions
 *	defines. The idea is to make it easier to override these exception throw formats, and get better default behavior
 *	for cases other than MFC and PowerPlant.
 *	
 *	Revision 2.37  1999/12/21 20:55:27  lewis
 *	Use new Led_ThrowIfNull instead of manual check
 *	
 *	Revision 2.36  1999/12/19 16:30:10  lewis
 *	React to SPR#0668- and add extra args to TextInteractor::OnTypedNormalCharacter() call
 *	
 *	Revision 2.35  1999/11/29 20:47:29  lewis
 *	Use Led_GetCurrentGDIPort () instead of peeking at 'qd' global (and kBlack constnat). SPR#0634
 *	
 *	Revision 2.34  1999/11/13 16:32:21  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.33  1999/07/13 22:39:18  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds that
 *	were only needed for these old compilers
 *	
 *	Revision 2.32  1999/06/28 14:59:23  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.31  1999/05/03 22:05:10  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.30  1999/02/21 23:08:43  lewis
 *	spr#0551
 *	
 *	Revision 2.29  1999/02/21 21:49:45  lewis
 *	Always keep fAcquireCount, and check if NULL GetMacPort(), and if so, throw NoTabletAvialable,
 *	rather than silently using bad tablet
 *	
 *	Revision 2.28  1997/12/24 04:40:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.27  1997/09/29  15:05:03  lewis
 *	Lose qLedFirstIndex support, and add (void)FocusDraw in two places I had forgotten it (spr#0480).
 *
 *	Revision 2.26  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.25  1997/07/27  15:10:44  lewis
 *	Fix qLedFirstIndex crashing scrolling bug - spr# 0472.
 *
 *	Revision 2.24  1997/07/14  01:17:04  lewis
 *	Renamed Led_PPView_ to Led_PPView, and TextImager_ to TextImager, etc...
 *	more?
 *
 *	Revision 2.23  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.22  1997/06/18  02:54:51  lewis
 *	WhileSimpleMouseTracking ().
 *	qIncludePrefixFile
 *
 *	Revision 2.21  1997/03/23  00:48:00  lewis
 *	Some fixes to smart cut and paste.
 *	But mostly redid the mouse-based-selection to use new WhileTrackingConstrainSelection ()
 *	support. MUCH clearer now!
 *
 *	Revision 2.20  1997/03/04  20:10:11  lewis
 *	Add support for SmartCutAndPasteMode with DragNDrop.
 *
 *	Revision 2.19  1997/01/20  05:25:28  lewis
 *	Mostly support for new eDefaultUpdate UpdateMode.
 *
 *	Revision 2.18  1997/01/10  03:18:26  lewis
 *	Lots of changes. Mostly first cut at scrolling support. Also redid mixin code. Now templated.
 *
 *	Revision 2.17  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.16  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1996/10/31  00:26:48  lewis
 *	Support forward delete character.
 *
 *	Revision 2.14  1996/10/04  16:36:59  lewis
 *	Work around qExplicitCastToBoolOfExpressionWithBitwiseAndFails.
 *	Optimize update region handling in DrawSelf().
 *	Fixed display bug(occasional) with DoVertScroll(). Must call Update() to get it drawn.
 *	A few other minor tweeks.
 *
 *	Revision 2.13  1996/09/30  14:24:46  lewis
 *	Lots of changes for new scrollbar code/adjust bounds gone. etc.
 *	Added support for (related) qSupportEnterIdleCallback.
 *
 *	Revision 2.12  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.11  1996/08/05  06:08:06  lewis
 *	SetCursor code should use Arrow cursor when over text we can drag.
 *	And new qDragShouldCopyByDefaultBetweenWindows. See SPRs for more info.
 *
 *	Revision 2.10  1996/07/19  16:36:38  lewis
 *	Handle case of D&D from one window to another within the same app
 *	as the same as CROSS-APP. (basicly fixed log buf with global varaible).
 *
 *	Revision 2.9  1996/07/03  01:09:32  lewis
 *	Whole bunch of minor D&D fixups, removing hilight cruft, don't accept
 *	when dropping same test on place of origin, and a few more minor things.
 *	Also fixed bug with erasing margins.
 *
 *	Revision 2.8  1996/06/01  02:16:08  lewis
 *	Lots of changes.
 *	Lose no-RTTI compat code.
 *	Cleanup drag region stuff.
 *	Implement autoscroll while dragging.
 *	Cleanup word selection code, and undo boundary code.
 *	Fix throw arg type to be (ExceptionCode) - for PowerPlant.
 *	more...
 *
 *	Revision 2.7  1996/05/23  19:36:06  lewis
 *	override Led_PPView_::HiliteDropArea () to do better job with dropRgn hilgiht
 *	(take into account the sbar).
 *	Catch throw and dont rethrow on RecieuveDropItem - apple/MWERKS
 *	code barfs badly if we throw across D&D call.
 *	Wrap draw code in try/catch so we don't propage error. Not a good
 *	soluition, but better than putting up alert which will cause redraw.
 *	No more UTextTraits stuff.
 *
 *	Revision 2.6  1996/05/14  20:26:43  lewis
 *	ScrollByIfRoom now takes eImmediateUpdate param in places where we scroll
 *	 (so we invoke new scrollbits code).
 *
 *	Revision 2.5  1996/05/04  21:05:51  lewis
 *	cleanup exact types of throw args in throw-routines.
 *	Add Refresh () method to resolve static scope ambiguity (ie not vtables).
 *
 *	Revision 2.4  1996/04/18  15:28:18  lewis
 *	Lots of cleanups and revisions of the Drag and drop/ flavor code
 *	to make more portable and usable for MFC etc.
 *
 *	Revision 2.3  1996/03/16  18:55:45  lewis
 *	Misc cleanups.
 *	Fixed space overwrites scrollbar bug.
 *	Cleaned up keyprocessing - and now support pageup/down/home/end keys.
 *	No more need for H()/V() accessors.
 *
 *	Revision 2.2  1996/03/05  18:30:36  lewis
 *	Broken Led_PP into  Led_PPView_ and StandardStyledWordWrappedLed_PPView
 *	and SimpleLed_PPView ().
 *	Support drag/drop and cleanued up clickself support. Other cleanups.
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<Scrap.h>
#include	<Script.h>
#include	<ToolUtils.h>

#include	<LDragTask.h>
#include	<LStream.h>
#include	<UDrawingState.h>
#include	<UMemoryMgr.h>
#include	<UKeyFilters.h>
#include	<PP_KeyCodes.h>
#include	<PP_Messages.h>

#include	"CodePage.h"
#include	"IdleManager.h"

#include	"Led_PP.h"







#if		qLedUsesNamespaces
namespace	Led {
#endif







static	bool	sOldCaretShownFlag	=	false;


// Not very intuitive, in my humble opinion. And not what is done on
// Windows (Windows is much more consistent, and does what we used todo).
// But after trying a number of sample mac apps (eg SimpleText) this appears
// to be the consensus approach. Also, DougStein (dougst@aw.com) believes
// its the right thing todo (at least for the mac) - See SPR#
#ifndef	qDragShouldCopyByDefaultBetweenWindows
	#define	qDragShouldCopyByDefaultBetweenWindows	1
#endif

struct	LedStartDragAndDropContext {
	public:
		LedStartDragAndDropContext (Led_PPView* ppView);

	public:
		bool		fOurDrag;
		size_t		fOurDragStart;
		size_t		fOurDragEnd;
#if		qDragShouldCopyByDefaultBetweenWindows
		bool		fForceIsCopy;
		bool		fForceIsMove;
#else
		bool		fOurDragIsMove;
#endif
		bool		fWeRecievedDrop;
		bool		fWeEverDimmedForInnerRejectAfterDraggingOut;
		Led_PPView*	fPPView;
		TempMarker	fOrigSelection;
};
	inline	LedStartDragAndDropContext::LedStartDragAndDropContext (Led_PPView* ppView):
		fOurDrag (false),
		fOurDragStart (0),
		fOurDragEnd (0),
#if		qDragShouldCopyByDefaultBetweenWindows
		fForceIsCopy (false),
		fForceIsMove (false),
#else
		fOurDragIsMove (false),
#endif
		fWeRecievedDrop (false),
		fWeEverDimmedForInnerRejectAfterDraggingOut (false),
		fPPView (ppView),
		fOrigSelection (ppView->GetTextStore (), ppView->GetSelectionStart (), ppView->GetSelectionEnd ())
		{
		}

LedStartDragAndDropContext*	sCurrentDragInfo	=	NULL;










/*
 ********************************************************************************
 *********************** LedSupport class lib support ***************************
 ********************************************************************************
 */
#if		qGenerateStandardPowerPlantExceptions
	static	struct	POWERPLANT_MODULE_INIT {
		static	void	Do_OutOfMemoryException ()
			{
				// With PowerPlant, throw type ExceptionCode - so Catch_() calls in PP code catch
				// the exception...
				throw LException (memFullErr);
				Led_Assert (false);	// better never reach this point!
			}
		static	void	Do_BadFormatDataException ()
			{
				// With PowerPlant, throw type ExceptionCode - so Catch_() calls in PP code catch
				// the exception...
				throw LException (memFullErr);	// NOT SURE WHAT TODO AT THIS POINT? Any better exception to throw???
				Led_Assert (false);	// better never reach this point!
			}
		static	void	Do_ThrowOSErrException (OSErr err)
			{
				// With PowerPlant, throw type ExceptionCode - so Catch_() calls in PP code catch
				// the exception...
				throw LException (err);
				Led_Assert (false);	// better never reach this point!
			}
		POWERPLANT_MODULE_INIT ()
		{
			Led_Set_OutOfMemoryException_Handler (&Do_OutOfMemoryException);
			Led_Set_BadFormatDataException_Handler (&Do_BadFormatDataException);
			Led_Set_ThrowOSErrException_Handler (&Do_ThrowOSErrException);
		}
	}	sPOWERPLANT_MODULE_INIT;
#endif






/*
@CLASS:			IdleManagerOSImpl_PP
@BASES:			@'IdleManager::IdleManagerOSImpl'
@ACCESS:		public
@DESCRIPTION:	<p>Implemenation detail of the idle-task management system. This can generally be ignored by Led users.
			</p>
*/
class	IdleManagerOSImpl_PP : public IdleManager::IdleManagerOSImpl, private LPeriodical {
	public:
		IdleManagerOSImpl_PP ();
		~IdleManagerOSImpl_PP ();

	public:
		override	void	StartSpendTimeCalls ();
		override	void	TerminateSpendTimeCalls ();
		override	float	GetSuggestedFrequency () const;
		override	void	SetSuggestedFrequency (float suggestedFrequency);


		override	void	SpendTime (const EventRecord& /*inMacEvent*/)
			{
				CallSpendTime ();
			}


	private:
		float	fSuggestedFrequency;
};



namespace	{
	/*
	 *	Code to automatically install and remove our idle manager.
	 */
	struct	IdleMangerSetter {
		IdleMangerSetter ()
			{
				IdleManager::SetIdleManagerOSImpl (&fIdleManagerOSImpl);
			}
		~IdleMangerSetter ()
			{
				IdleManager::SetIdleManagerOSImpl (NULL);
			}
		IdleManagerOSImpl_PP	fIdleManagerOSImpl;
	};
	struct	IdleMangerSetter	sIdleMangerSetter;
}






/*
 ********************************************************************************
 **************************** IdleManagerOSImpl_PP ******************************
 ********************************************************************************
 */
IdleManagerOSImpl_PP::IdleManagerOSImpl_PP ():
	fSuggestedFrequency (0)
{
}

IdleManagerOSImpl_PP::~IdleManagerOSImpl_PP ()
{
}

void	IdleManagerOSImpl_PP::StartSpendTimeCalls ()
{
	StartIdling ();
}

void	IdleManagerOSImpl_PP::TerminateSpendTimeCalls ()
{
	StopIdling ();
}

float	IdleManagerOSImpl_PP::GetSuggestedFrequency () const
{
	return fSuggestedFrequency;
}

void	IdleManagerOSImpl_PP::SetSuggestedFrequency (float suggestedFrequency)
{
	// Unfortunately - for now - it appears we must ignore the suggestedFrequency...
	fSuggestedFrequency = suggestedFrequency;
}






/*
 ********************************************************************************
 ******************************* Led_PP_TmpCmdUpdater ***************************
 ********************************************************************************
 */

Led_PP_TmpCmdUpdater::CommandNumber	Led_PP_TmpCmdUpdater::GetCmdID () const
{
	return fCommand;
}

bool	Led_PP_TmpCmdUpdater::GetEnabled () const
{
	return fEnabled;
}

void	Led_PP_TmpCmdUpdater::SetEnabled (bool enabled)
{
	fEnabled = enabled;
}

void	Led_PP_TmpCmdUpdater::SetChecked (bool checked)
{
	fUsesMark = true;
	fMark = checked? checkMark: 0;
}

void	Led_PP_TmpCmdUpdater::SetText (const Led_SDK_Char* text)
{
	fName[0] = ::strlen (text);
	(void)::memcpy (&fName[1], text, fName[0]);
}







/*
 ********************************************************************************
 ************************************ Led_PPView ********************************
 ********************************************************************************
 */
string	Led_PPView::sDragCommandName		=	"Drag";
string	Led_PPView::sDropCommandName		=	"Drop";
string	Led_PPView::sDragNDropCommandName	=	"Drag and Drop";

Led_PPView::Led_PPView ():
	LView (),
	LCommander (),
	LPeriodical (),
#if		TARGET_CARBON
	LDragAndDrop (::GetWindowFromPort (UQDGlobals::GetCurrentPort ()), this),
#else
	LDragAndDrop (UQDGlobals::GetCurrentPort (), this),
#endif
	Led_MacOS_Helper<> ()
{
	mTextAttributes = 0;
	InitTextEdit(0);			// Initialize member variables
}

Led_PPView::Led_PPView (const SPaneInfo	&inPaneInfo, const SViewInfo& inViewInfo, UInt16 inTextAttributes, ResIDT inTextTraitsID):
	LView (inPaneInfo, inViewInfo),
	LCommander (),
	LPeriodical (),
#if		TARGET_CARBON
	LDragAndDrop (::GetWindowFromPort (UQDGlobals::GetCurrentPort ()), this),
#else
	LDragAndDrop (UQDGlobals::GetCurrentPort (), this),
#endif
	Led_MacOS_Helper<> ()
{
	mTextAttributes = inTextAttributes;
	InitTextEdit(inTextTraitsID);
}

Led_PPView::Led_PPView (LStream* inStream):
	LView (inStream),
	LCommander (),
	LPeriodical (),
#if		TARGET_CARBON
	LDragAndDrop (::GetWindowFromPort (UQDGlobals::GetCurrentPort ()), this),
#else
	LDragAndDrop (UQDGlobals::GetCurrentPort (), this),
#endif
	Led_MacOS_Helper<> ()
{
	inStream->ReadData (&mTextAttributes, sizeof(UInt16));

	ResIDT	textTraitsID;
	inStream->ReadData(&textTraitsID, sizeof(ResIDT));
	
	ResIDT	initialTextID;
	inStream->ReadData(&initialTextID, sizeof(ResIDT));
	
	InitTextEdit (textTraitsID);
	
	Handle	initialTextH = ::GetResource ('TEXT', initialTextID);
	if (initialTextH != nil) {
		StHandleLocker	lock (initialTextH);
		size_t			inTextLen	=	::GetHandleSize (initialTextH);
		Led_SmallStackBuffer<Led_tChar>	buf (inTextLen);
		#if		qWideCharacters
			CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (*initialTextH, inTextLen, buf, &inTextLen);
			Led_NativeToNL (buf, inTextLen, buf, inTextLen);
		#else
			Led_NativeToNL (*initialTextH, inTextLen, buf, inTextLen);
		#endif
		Replace (0, 0, buf, inTextLen);
		::ReleaseResource (initialTextH);
	}
}

Led_PPView::~Led_PPView()
{
}

void	Led_PPView::SetupCurrentGrafPort () const
{
	(void)const_cast<Led_PPView*> (this)->FocusDraw ();
}


const string&	Led_PPView::GetDragCommandName ()
{
	return sDragCommandName;
}

void	Led_PPView::SetDragCommandName (const string& cmdName)
{
	sDragCommandName = cmdName;
}

const string&	Led_PPView::GetDropCommandName ()
{
	return sDropCommandName;
}

void	Led_PPView::SetDropCommandName (const string& cmdName)
{
	sDropCommandName = cmdName;
}

const string&	Led_PPView::GetDragNDropCommandName ()
{
	return sDragNDropCommandName;
}

void	Led_PPView::SetDragNDropCommandName (const string& cmdName)
{
	sDragNDropCommandName = cmdName;
}

void	Led_PPView::FinishCreate ()
{
	LView::FinishCreate ();

	SetUseGrafPort (GetMacPort ());

	Rect	textFrame;
	if (CalcLocalFrameRect (textFrame)) {
		SetWindowFrameRect (AsLedRect (textFrame));
	}


	/*
	 *		For V&H scrollbars, they may not have been able to be created til now - so re-call SetHasScrollbar() to get them created.
	 */
	SetScrollBarType (v, GetScrollBarType (v));
	SetScrollBarType (h, GetScrollBarType (h));

	TabletChangedMetrics ();	// must get called at least once when we setup...
}

void	Led_PPView::UpdateWindowRect_ (const Led_Rect& windowRectArea) const
{
	if (not windowRectArea.IsEmpty ()) {
		// TMP HACK
		//
		// Actually, this is probably a good enuf hack for a while to come. It is
		// really only a problem if you have lots of other widgets in the same window,
		// many of which get invaled at the same time, but you only want to update
		// this one. That is QUITE RARE - and difficult to setup.
		//
		// Anyhow - for the right fix to this code, see what I did to Led_TCL. Then it
		// must be suitably hacked up to integrate with the PowerPlant class lib.
		// A few hours work - in all likelihood, and just won't buy is much.
		// LGP 960315
		const_cast<Led_PPView*>(this)->UpdatePort ();
	}
}

void	Led_PPView::Refresh ()
{
	LView::Refresh ();
}

void	Led_PPView::InitTextEdit (ResIDT inTextTraitsID)
{
	mTextTraitsID = inTextTraitsID;
}

#if		qMimicLTextEditAPI
void	Led_PPView::SetTextHandle (Handle inTextH)
{
	Led_RequireNotNil (inTextH);
	StHandleLocker	lock (inTextH);
	SetTextPtr (*inTextH, ::GetHandleSize(inTextH));
}
#endif

#if		qMimicLTextEditAPI
void	Led_PPView::SetTextPtr (Ptr inTextP, SInt32	inTextLen)
{
	Led_Require ((inTextP != NULL) or (inTextLen == 0));

	Led_SmallStackBuffer<Led_tChar>	buf (inTextLen);
	#if		qWideCharacters
		{
			size_t	useLen	=	inTextLen;
			CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (inTextP, inTextLen, buf, &useLen);
			Led_NativeToNL (buf, useLen, buf, useLen);
			inTextLen = useLen;
		}
	#else
		Led_NativeToNL (inTextP, inTextLen, buf, inTextLen);
	#endif

	/*
	 *	Note that by doing a Delete followed by an Insert() instead of just doing
	 *	a replace - we are in-effect interpretting this as destroying all
	 *	markers. And - also - this SetTextPtr () operation could fail in the middle
	 *	leaving an empty text buffer as the result.
	 */
	Replace (0, GetLength () + 0, LED_TCHAR_OF (""), 0);
	Replace (0, 0, buf, inTextLen);
}
#endif

#if		qMimicLTextEditAPI
void	Led_PPView::SetTextTraitsID (ResIDT inTextTraitsID)
{
	mTextTraitsID = inTextTraitsID;
	SPoint32	scrollUnit;
	scrollUnit.h = 4;
	Led_Rect	rr	=	GetCharLocation (GetStartOfRow (GetTopRowInWindow ()));
	mScrollUnit.v = rr.GetHeight ();	// no good answer for styled text - so hack and pick height of first line
	SetScrollUnit (scrollUnit);
}
#endif

#if		qMimicLTextEditAPI
Boolean	Led_PPView::HasAttribute (UInt16 inAttribute)
{
	return ((mTextAttributes & inAttribute) != 0);
}
#endif

#if		qMimicLTextEditAPI
void	Led_PPView::AddAttributes (UInt16 attributesToAdd)
{
	mTextAttributes |= attributesToAdd;
}
void	Led_PPView::SubtractAttributes (UInt16 attributesToSubtract)
{
	mTextAttributes &= ~attributesToSubtract;
}
#endif

#if		qMimicLTextEditAPI
void	Led_PPView::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	Led_MacOS_Helper<>::DidUpdateText (updateInfo);
	UserChangedText ();
}

void	Led_PPView::UserChangedText ()
{
	// No point to this, but to Mimic LTextEdit API - for PP user convenience...
}
#endif

void	Led_PPView::DrawSelf ()
{
	RgnHandle	updateRgn = GetLocalUpdateRgn ();
	Led_ThrowIfNull (updateRgn);
	Handle_Draw (updateRgn);
	::DisposeRgn (updateRgn);
}

void	Led_PPView::HideSelf ()
{
	if (IsOnDuty ()) {				// Shouldn't be on duty when invisible
		SwitchTarget (GetSuperCommander ());
	}
}

#if		__PowerPlant__ >= 0x02208000
void	Led_PPView::AdjustMouseSelf (Point /*inPortPt*/, const EventRecord& inMacEvent, RgnHandle /*outMouseRgn*/)
{
	HandleAdjustCursorEvent (inMacEvent);
}
#else
void	Led_PPView::AdjustCursorSelf (Point /*inPortPt*/, const EventRecord& inMacEvent)
{
	HandleAdjustCursorEvent (inMacEvent);
}
#endif

Boolean	Led_PPView::ObeyCommand (CommandT inCommand, void* ioParam)
{
	IdleManager::NonIdleContext	nonIdleContext;

	if (OnPerformCommand (PP_CommandNumberMapping::Get ().Lookup (inCommand))) {
		return true;
	}
	else {
		Boolean	cmdHandled = true;
		switch (inCommand) {
			case msg_TabSelect:
				if (!IsEnabled()) {
					cmdHandled = false;
					break;
				} // else FALL THRU to SelectAll()

			case cmd_SelectAll:
				SelectAll ();
				break;

			default:
				cmdHandled = LCommander::ObeyCommand (inCommand, ioParam);
				break;
		}
		return cmdHandled;
	}
}

void	Led_PPView::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, Str255 outName)
{
	Led_PP_TmpCmdUpdater enabler (PP_CommandNumberMapping::Get ().Lookup (inCommand), outEnabled, outUsesMark, outMark, outName);				
	if (not OnUpdateCommand (&enabler)) {
		outUsesMark = false;
		switch (inCommand) {
			case	cmd_Cut:
			case	cmd_Clear: {
				// Copy enabled if something is selected
				outEnabled = (GetSelectionStart () != GetSelectionEnd ()) and HasAttribute (textAttr_Editable);
			}
			break;

			case	cmd_Paste: {
				// Copy enabled if something is selected
				outEnabled = HasAttribute (textAttr_Editable) and ShouldEnablePasteCommand ();
			}
			break;

			case	cmd_SelectAll: {
				// Copy enabled if something is selected
				outEnabled = HasAttribute (textAttr_Selectable) and (GetLength () != 0);
			}
			break;

			default: {
				LCommander::FindCommandStatus(inCommand, outEnabled, outUsesMark, outMark, outName);
			}
			break;
		}
	}
}

Boolean	Led_PPView::HandleKeyPress (const EventRecord& inKeyEvent)
{
	if (HasAttribute (textAttr_Editable)) {	// Disallow editing
		bool	handled		=	 HandleKeyEvent (inKeyEvent);
		if (handled) {
			return true;
		}
		else {
			return LCommander::HandleKeyPress (inKeyEvent);
		}
	}
	else {
		return false;
	}
}

void	Led_PPView::SelectAll ()
{
	if (HasAttribute (textAttr_Selectable)) {
		SetSelection (0, GetLength ());
	}
}

void	Led_PPView::ResizeFrameBy (SInt16 inWidthDelta, SInt16 inHeightDelta, Boolean inRefresh)
{
	// Resize Pane
	LView::ResizeFrameBy (inWidthDelta, inHeightDelta, inRefresh);
	Rect	textFrame;
	if (CalcLocalFrameRect (textFrame)) {
		SetWindowFrameRect (AsLedRect (textFrame));
	}
}

void	Led_PPView::MoveBy (SInt32 inHorizDelta, SInt32 inVertDelta, Boolean inRefresh)
{
	LView::MoveBy (inHorizDelta, inVertDelta, inRefresh);
	Rect	textFrame;
	if (CalcLocalFrameRect (textFrame)) {
		SetWindowFrameRect (AsLedRect (textFrame));
	}
}

void	Led_PPView::ScrollImageBy (SInt32 /*inLeftDelta*/, SInt32 /*inTopDelta*/, Boolean /*inRefresh*/)
{
	// for now ignore, since we aren't using scrollers, and PP blithely scrolls us anyhow
	// screwing up our coordinates (only in printing as far as I know, but this is enuf!)
	
	// maybe I should simply pin the scrolling to within my mImageSize - that might be good
	// enuf to workaround this bug, and still allow Led_PPView to be used with LScroller?
}

void	Led_PPView::BeTarget ()
{
	HandleActivateEvent ();
	SetCaretShown (HasAttribute (textAttr_Selectable));
	StartIdling ();					// Idle time used to flash the cursor
}

void	Led_PPView::DontBeTarget ()
{
	HandleDeactivateEvent ();
	StopIdling ();					// Stop flashing the cursor
}

void	Led_PPView::SpendTime (const EventRecord& /*inMacEvent*/)
{
	HandleIdleEvent ();
}

void	Led_PPView::SavePlace (LStream* outPlace)
{
	LView::SavePlace (outPlace);
}

void	Led_PPView::RestorePlace (LStream* inPlace)
{
	LView::RestorePlace (inPlace);
}

void	Led_PPView::CountPanels (UInt32& outHorizPanels, UInt32& outVertPanels)
{
	outHorizPanels = 1;
	SDimension16	frameSize;
	GetFrameSize (frameSize);
	Led_Distance	totalHeight	=	GetCharLocation (GetLength ()).bottom;
	Led_Assert (totalHeight >= 1);
	outVertPanels = ((totalHeight - 1) / frameSize.height) + 1;
	Led_Assert (outVertPanels >= 1);
}

void	Led_PPView::PrintPanelSelf (const PanelSpec& /*inPanel*/)
{
	/*
	 *	Perhaps because of changes in latest PowerPlant - you now must call SetUseGrafPort () later than we used to
	 *	(in Led_PPView::FinishCreate ()). Port not created at that point now.
	 */
	GrafPtr	oldMacPort	=	GetUseGrafPort ();
	SetUseGrafPort (GetMacPort ());

	try {
		TextImager*	imager	=	this;
		imager->Draw (GetWindowRect (), true);

		// advance the scrolling cursor while printing...
		ScrollByIfRoom (GetTotalRowsInWindow ());
		SetUseGrafPort (oldMacPort);
	}
	catch (...) {
		SetUseGrafPort (oldMacPort);
		throw;
	}
}

void	Led_PPView::ClickSelf (const SMouseDownEvent& inMouseDown)
{
	/*
	 *	Cannot call Led_MacOS_Helper<BASE_INTERACTOR>::HandleMouseDownEvent directly because we need to intersperse
	 *	little checks / changes throughout the implementation - partly for 'selectable' support, and partly for D&D
	 *	support, and partly for SwitchTarget () support.
	 */
	if (HandleIfScrollBarClick (inMouseDown.macEvent)) {
		return;
	}

	if (not HasAttribute (textAttr_Selectable)) {
		return;
	}

	if (not IsTarget()) {				// If not the Target, clicking in an TextEdit makes it the Target.
		SwitchTarget (this);
	}

	if (IsTarget()) {
		BreakInGroupedCommands ();

		Led_MacOS_Helper<>::UpdateClickCount (inMouseDown.macEvent.when / 60.0f, AsLedPoint (inMouseDown.macEvent.where));

		if (IsADragSelect (inMouseDown)) {
			HandleDragSelect (inMouseDown);
		}
		else {
			HandleTrackDragSelectInText (inMouseDown.macEvent);
		}

		BreakInGroupedCommands ();
	}
}

bool	Led_PPView::IsADragSelect (const SMouseDownEvent& inMouseDown) const
{
	if (GetCurClickCount () == 1) {
		(void)const_cast<Led_PPView*> (this)->FocusDraw ();

		/*
		 *	This seems to be what most people do. But I think this algorithm should
		 *	be embellished, so we detect that the mouse is down for a while (a half second?)
		 *	and then melt the drag into a regular selection. Also, pay attention to keys
		 *	pressed etc as a hint of intent. But immediately return yes if we are moving
		 *	what we've selected.
		 *
		 *	For now though, just do it the simple way...
		 */
		Led_Region	r;
		GetSelectionWindowRegion (&r, GetSelectionStart (), GetSelectionEnd ());

		Led_Point	origClickAt		=	AsLedPoint (inMouseDown.whereLocal);
		bool		result	=	::PtInRgn (AsQDPoint (origClickAt), r.GetOSRep ());
	
		if (result) {
			(void)const_cast<Led_PPView*> (this)->FocusDraw ();
			while (::StillDown ()) {
				Point	macNewPt;
				::GetMouse (&macNewPt);
				Led_Point	newPt	=	AsLedPoint (macNewPt);
				const	Led_Coordinate	kHysteresis	=	3;
				if ((Led_Abs (newPt.h-origClickAt.h) > kHysteresis) or (Led_Abs (newPt.v-origClickAt.v) > kHysteresis)) {
					return true;
				}
			}
			return false;	// we click in a selection, but don't drag outside, we're probably
							// just trying to get an insertion point
		}
		else {
			return result;
		}
	}
	else {
		// never start drag on multi-clicks
		return false;
	}
}

void	Led_PPView::HandleDragSelect (const SMouseDownEvent& inMouseDown)
{
	class	AppleDAndDFlavorPackage : public WriterFlavorPackage {
		public:
			AppleDAndDFlavorPackage (DragReference inDragRef):
				fDragReference (inDragRef)
				{
				}
			override	void	AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf)
				{
					ItemReference	ir	=	0;	// as far as I can tell, we can pass anything we want here...
					::AddDragItemFlavor (fDragReference, ir, clipFormat, (void*)buf, bufSize, 0);
				}
		private:
			DragReference	fDragReference;
	};
	class	MyDragTask : public LDragTask {
		public:
			MyDragTask (const EventRecord& inEventRecord):
				LDragTask (inEventRecord),
				fPutIntoDragRgn (NULL)
				{
				}
			~MyDragTask ()
				{
				}
			OSErr	DoTheDrag ()
				{
					return ::TrackDrag (mDragRef, &mEventRecord, mDragRegion);
				}
			RgnHandle	fPutIntoDragRgn;
	};

	(void)FocusDraw ();

	Led_Assert (sCurrentDragInfo == NULL);
	sCurrentDragInfo = new LedStartDragAndDropContext (this);

	sCurrentDragInfo->fOurDragStart = GetSelectionStart ();
	sCurrentDragInfo->fOurDragEnd = GetSelectionEnd ();

	try {
		#if		qDragShouldCopyByDefaultBetweenWindows
			// optionKey forces copy in SimpleText 1.3.1, and OpenDoc 1.0 (DR4)
			// controlKey forces MOVE in OpenDoc 1.0 (DR4) (no such feature for SimpleText)
			// Default for BOTH is COPY across windows (apps) and MOVE within a window.
			sCurrentDragInfo->fForceIsCopy = bool (inMouseDown.macEvent.modifiers & optionKey);
			sCurrentDragInfo->fForceIsMove = not sCurrentDragInfo->fForceIsCopy and (inMouseDown.macEvent.modifiers & controlKey);
		#else
			sCurrentDragInfo->fOurDragIsMove = not (inMouseDown.macEvent.modifiers & optionKey);
		#endif
		(void)FocusDraw ();
		MyDragTask				dragTask (inMouseDown.macEvent);
		AppleDAndDFlavorPackage	flavorPackage (dragTask.GetDragReference ());
		ExternalizeFlavors (flavorPackage);
	
		{
			Led_Region	selectionRegion;
			GetSelectionWindowRegion (&selectionRegion, GetSelectionStart (), GetSelectionEnd ());
			// Move selection region into global coordinates
			{
				Point pt = {0,0};
				LocalToGlobal(&pt);
				OffsetRgn (selectionRegion.GetOSRep (), pt.h, pt.v);
			}
			RgnHandle	dragRgn	=	dragTask.GetDragRegion ();
			if (dragRgn != NULL) {
				::CopyRgn (selectionRegion.GetOSRep (), dragRgn);
				::InsetRgn (dragRgn, -1, -1);
				::DiffRgn (dragRgn, selectionRegion.GetOSRep (), dragRgn);
			}
		}


		if (dragTask.DoTheDrag () == noErr) {
			#if		qDragShouldCopyByDefaultBetweenWindows
				bool	isMove	=	sCurrentDragInfo->fForceIsMove or
									(sCurrentDragInfo->fWeRecievedDrop and not sCurrentDragInfo->fForceIsCopy)
									;
			#else
				bool	isMove	=	sCurrentDragInfo->fOurDragIsMove;
			#endif

			if (isMove) {
				/*
				 *	delete the original text on a MOVE (except if it is a move from ourselves to ourselves,
				 *	and re didn't accept the drag).
				 */
				UndoableContextHelper	undoContext (*this,
													sCurrentDragInfo->fWeRecievedDrop? Led_SDK_String (): GetDragCommandName (),
													sCurrentDragInfo->fOrigSelection.GetStart (),
													sCurrentDragInfo->fOrigSelection.GetEnd (),
													true
												);
					{
						TextInteractor::InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0, false, false, eDefaultUpdate);
					}
				undoContext.CommandComplete (sCurrentDragInfo->fOrigSelection.GetEnd ());
			}
		}
		// must do here, even if NOT a MOVE - cuz we skipped this in the RecieveDragItem code...
		BreakInGroupedCommands ();
	}
	catch (...) {
		delete sCurrentDragInfo; sCurrentDragInfo = NULL;
		throw;
	}
	delete sCurrentDragInfo; sCurrentDragInfo = NULL;
}

Boolean	Led_PPView::ItemIsAcceptable (DragReference inDragRef, ItemReference inItemRef)
{
	if (!(mTextAttributes & textAttr_Editable)){
		return false;
	}

	unsigned short	numFlavors	=	0;
	Led_ThrowOSErr (::CountDragItemFlavors (inDragRef, inItemRef, &numFlavors));

	for (size_t i = 0; i < numFlavors; i++) {
		FlavorType	flavorType	=	0;
		Led_ThrowOSErr (::GetFlavorType (inDragRef, inItemRef, i+1, &flavorType));
		if (CanAcceptFlavor (flavorType)) {
			return true;
		}
	}
	return false;
}

void	Led_PPView::ReceiveDragItem (DragReference inDragRef, DragAttributes /*inDragAttrs*/, ItemReference inItemRef, Rect& /*inItemBounds*/)
{
	class	AppleDAndDFlavorPackage : public ReaderFlavorPackage {
		public:
			AppleDAndDFlavorPackage (DragReference inDragRef, ItemReference inItemRef):
				ReaderFlavorPackage (),
				fDragReference (inDragRef),
				fItemReference (inItemRef)
				{
				}
			override	bool	GetFlavorAvailable (Led_ClipFormat clipFormat) const
				{
					FlavorFlags	flavorFlags	=	0;
					OSErr	err	=	GetFlavorFlags (fDragReference, fItemReference, clipFormat, &flavorFlags);
					if (err == badDragFlavorErr) {
						return false;
					}
					Led_ThrowOSErr (err);
					return true;
				}
			override	size_t	GetFlavorSize (Led_ClipFormat clipFormat) const
				{
					Size	flavorSize	=	0;
					Led_ThrowOSErr (::GetFlavorDataSize (fDragReference, fItemReference, clipFormat, &flavorSize));
					return flavorSize;
				}
			override	size_t	ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const
				{
					Size	flavorSizeRetrieved	=	bufSize;
					Led_ThrowOSErr (::GetFlavorData (fDragReference, fItemReference, clipFormat, buf, &flavorSizeRetrieved, 0));
					Led_Ensure (flavorSizeRetrieved <= bufSize);
					return flavorSizeRetrieved;
				}
		private:
			DragReference	fDragReference;
			ItemReference	fItemReference;
	};


	size_t		selStart	=	GetSelectionStart ();

	if (sCurrentDragInfo != NULL and sCurrentDragInfo->fPPView == this) {
		if (selStart >= sCurrentDragInfo->fOurDragStart and selStart <= sCurrentDragInfo->fOurDragEnd) {
			Led_ThrowOSErr (dragNotAcceptedErr);
		}

		sCurrentDragInfo->fWeRecievedDrop = true;
	}

	AppleDAndDFlavorPackage	flavors (inDragRef, inItemRef);

	bool			doSmartCNP	=	GetSmartCutAndPasteMode () and flavors.GetFlavorAvailable (kTEXTClipFormat);
	SmartCNPInfo	smartCNPInfo;
	if (doSmartCNP) {
		size_t	length		=	flavors.GetFlavorSize (kTEXTClipFormat);
		Led_SmallStackBuffer<Led_tChar> buf (length);
		length	=	flavors.ReadFlavorData (kTEXTClipFormat, length, buf);
		if (doSmartCNP) {
			doSmartCNP = LooksLikeSmartPastableText (buf, length, &smartCNPInfo);
		}
	}

	BreakInGroupedCommands ();

	size_t					ucSelStart	=	(sCurrentDragInfo != NULL and sCurrentDragInfo->fPPView == this)?
												sCurrentDragInfo->fOrigSelection.GetStart (): GetSelectionStart ()
											;
	size_t					ucSelEnd	=	(sCurrentDragInfo != NULL and sCurrentDragInfo->fPPView == this)?
												sCurrentDragInfo->fOrigSelection.GetEnd (): GetSelectionEnd ()
											;
	UndoableContextHelper	undoContext (
											*this, 
											(sCurrentDragInfo != NULL and sCurrentDragInfo->fPPView == this)? GetDragNDropCommandName (): GetDropCommandName (),
											GetSelectionStart (), GetSelectionEnd (),
											ucSelStart, ucSelEnd,
											false
										);
		{
			InternalizeBestFlavor (flavors);
			if (doSmartCNP) {
				OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (undoContext.GetUndoRegionStart (), smartCNPInfo);
			}
		}
	undoContext.CommandComplete ();


	// Don't do break here, do it in HandleDragSelect () so it can incorporate a DELETE TEXT on MOVE...
	if (sCurrentDragInfo == NULL or sCurrentDragInfo->fPPView != this) {
		BreakInGroupedCommands ();
	}
}

void	Led_PPView::EnterDropArea (DragReference inDragRef, Boolean inDragHasLeftSender)
{
	sOldCaretShownFlag = GetCaretShown ();
	SetCaretShown (true);
	LDropArea::EnterDropArea (inDragRef, inDragHasLeftSender);
}

void	Led_PPView::LeaveDropArea (DragReference inDragRef)
{
	LDropArea::LeaveDropArea (inDragRef);
	SetCaretShown (sOldCaretShownFlag);
}

void	Led_PPView::InsideDropArea (DragReference inDragRef)
{
	try {
		(void)FocusDraw ();
		Point	qdWhere;
		::GetMouse (&qdWhere);
		Led_Point	where	=	AsLedPoint (qdWhere);
		size_t		newPos	=	GetCharAtClickLocation (where);

		/*
		 *	Handle autoscrolling, if necessary.
		 *
		 *		This autoscrolling code looks deceptively simple. Well -  I guess what its
		 *	doing IS simple. But lots of other very plausible SIMPLE approaches didn't work.
		 *	And if you try SimpleText, or CW9, and do drag-of-text with scrolling, you'll
		 *	see that these applications screw up the drag-hilight when scrolling.
		 *
		 *		The first attempt I made was to simply call UpdateDragHilight() from
		 *	Led_PPView::DrawSelf (). You'd think this would have redrawn the hilght. But
		 *	it didn't (at least not correctly). I futzed with disabling scrollbits stuff,
		 *	and a bunch of other hacks, but nothing seemed to help.
		 *
		 *		Anyhow, this seems to work well - LGP 960530.
		 */
		bool	rehilite	=	false;
		if (newPos < GetMarkerPositionOfStartOfWindow ()) {
			if (mIsHilited) {
				mIsHilited = false;
				UnhiliteDropArea (inDragRef);
				rehilite = true;
			}
			ScrollByIfRoom (-1, eImmediateUpdate);
			newPos = GetCharAtClickLocation (where);
		}
		else if (newPos > GetMarkerPositionOfEndOfWindow ()) {
			if (mIsHilited) {
				mIsHilited = false;
				UnhiliteDropArea (inDragRef);
				rehilite = true;
			}
			ScrollByIfRoom (1, eImmediateUpdate);
			newPos = GetCharAtClickLocation (where);
		}
		if (rehilite) {
			mIsHilited = true;
			Update ();
			OutOfFocus (NULL);
			(void)FocusDraw ();
			HiliteDropArea (inDragRef);
		}

		if (sCurrentDragInfo != NULL and sCurrentDragInfo->fPPView == this and (newPos >= sCurrentDragInfo->fOurDragStart and newPos <= sCurrentDragInfo->fOurDragEnd)) {
			if (mIsHilited) {
				mIsHilited = false;
				UnhiliteDropArea (inDragRef);
				sCurrentDragInfo->fWeEverDimmedForInnerRejectAfterDraggingOut = true;
			}
			SetSelection (sCurrentDragInfo->fOurDragStart, sCurrentDragInfo->fOurDragEnd);
			fLastDrawnOn = true;
			Update ();
			return;
		}

		if (newPos >= GetMarkerPositionOfStartOfWindow () and newPos <= GetMarkerPositionOfEndOfWindow ()) {
			bool	rehilight	=	false;
			if (mIsHilited and GetSelectionStart () != GetSelectionEnd ()) {
				mIsHilited = false;
				UnhiliteDropArea (inDragRef);
				rehilight = true;
			}

			SetSelection (newPos, newPos);
			fLastDrawnOn = true;
			Update ();

			if (not mIsHilited and rehilight or (sCurrentDragInfo != NULL and sCurrentDragInfo->fPPView == this and sCurrentDragInfo->fWeEverDimmedForInnerRejectAfterDraggingOut)) {
				mIsHilited = true;
				HiliteDropArea (inDragRef);
			}
		}
	}
	catch (...) {
		// PP wrapers and Mac toolbox just crash if we throw out, so don't bother - just eat
		// any exceptions here...
	}
}

void	Led_PPView::HiliteDropArea (DragReference inDragRef)
{
	// Similar to LDragAndDrop base class version, but we take into account scrollbar.
	Rect	dropRect		=	AsQDRect (GetWindowRect ());
	(void)FocusDraw ();
	RgnHandle	dropRgn = ::NewRgn ();
	if (dropRgn != NULL) {
		::RectRgn (dropRgn, &dropRect);
		::ShowDragHilite (inDragRef, dropRgn, true);
		::DisposeRgn (dropRgn);
	}
}


#if		qLedUsesNamespaces
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
