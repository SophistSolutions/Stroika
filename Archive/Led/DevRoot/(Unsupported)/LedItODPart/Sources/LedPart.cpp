/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1996.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/LedPart.cpp,v 2.15 1996/12/13 18:10:13 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedPart.cpp,v $
 *	Revision 2.15  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.14  1996/12/05  21:12:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.13  1996/09/30  14:46:36  lewis
 *	new TextStore::Find() API - SearchParameters arg.
 *
 *	Revision 2.12  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.11  1996/05/23  20:35:50  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1996/03/16  19:36:33  lewis
 *	Added OnEnterFindStringCommand () support.
 *
 *	Revision 2.9  1996/02/26  23:11:36  lewis
 *	Use kBadIndex instead of size_t (-1).
 *	TextInteracter --> TextInteractor.
 *
 *	Revision 2.8  1996/02/05  05:10:19  lewis
 *	Massive changes.
 *	Find dialog, and Prefernces dialog.
 *	Looked at diffs for DrawShapes 1.0 and 1.0.1(DR4 pre and final CDs),
 *	and tried to apply many.
 *	Try to support drop-as.
 *	qUseResFileHackToEnableBaloonHelp.
 *
 *	Revision 2.7  1996/01/22  05:54:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1996/01/04  00:31:11  lewis
 *	Massive changes. Mostly got rid of Environment* arg stuff. And new _od naming covention
 *	for SOM calls from OD to our part.
 *
 *	Revision 2.5  1995/12/16  05:05:19  lewis
 *	Delete printer etc in ReleaseAll() instead of DTOR (as per warnings
 *	in recipies)
 *	Hack drag/drop code to peek at global variable from FrameEditor module
 *	about selection stuff to avoid nasty behavior when we drop and drag
 *	from the same selection. Should cleanup to not use globals!
 *
 *	Revision 2.4  1995/12/15  04:14:28  lewis
 *	Massive diffs.
 *	Including new support for a faster way of keeping track of embeddings
 *	that have been realized, but must (maybe) need to be unrealized cuz
 *	of being scrolled off.
 *	Added primitve printer support.
 *	Many other misc changes, DragDrop, Undo, etc...
 *
 *	Revision 2.3  1995/12/13  06:22:23  lewis
 *	MASSIVE changes.
 *	SafeGetEnvironment instead of passing Environemt* arg all over the place.
 *	Lose publish/subscribe support crap (which didn't really work anyhow).
 *	Use new od_ convention for methods called directly by SOM wrappers (partly
 *	implemented).
 *	got embedding working a little better (not sure any of that was in here).
 *	Use new TempIterator stuff.
 *
 *	Revision 2.2  1995/12/08  07:24:15  lewis
 *	Fix bug with font menu command number ranges.
 *	rename/cleanup.
 *	Call scrolltoselection in some edit cases (cut/paste mainly) that were
 *	forgotten.
 *	Add find/findagain etc menu items (though no real code behind this).
 *	Call new ActivatingScrollbar method, to fix scrollbar hilite bugs.
 *
 *	Revision 2.1  1995/12/06  02:03:11  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
 *
 *	Revision 2.0  1995/11/25  00:41:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1995/11/05  02:13:40  lewis
 *	Revised about box. Use new StyledIOReader for Drag / Drop files.
 *	Added popup menu to picker dialog toochose format.
 *	Added STYL support / resource support for StyleIOReaders.
 *
 *	Revision 2.3  1995/11/02  22:49:04  lewis
 *	Lots of changes
 *	Substantial rewrite or change of the internalize / esternalize code.
 *	Now vectors together much better, pays attention toelection ranges.
 *	Used for drag drop now. Supports styled text now.
 *	Menu updating fixed for style run/DoContinuous support.
 *	New file format, and more consitency dealig with support document formats.
 *
 *	Revision 2.2  1995/10/19  22:56:29  lewis
 *	Hard to tell just what changed since option-8 characters in previous
 *	checkin confused rcsdiff. Now I've gotten rid of them for future checkins.
 *	Should be mostly very minor changes - includign not calling GetDefaultFont
 *	and isntead adjust font by creating FontSPecification object and only
 *	setting one field and then only setting its valid bit.
 *
 *	Revision 2.1  1995/10/09  23:08:17  lewis
 *	Massive changes.
 *	cleaned up, and integerated some changes from reviewing SampePartC++ supported
 *	part example from DR3.
 *	New OpenWindow code from there.
 *	fixed most (understood) clipping and erasing problems.
 *	now handle multiple facets and frames (mostly) correctly (no multiple scrobbars per facet yet and havent tested).
 *	too much to account here...
 *
 *	Revision 2.0  1995/09/06  21:13:57  lewis
 *	*** empty log message ***
 *
 *
 *
 */


// DCS Profiling $$$$$
#ifdef qProfileLedPart
#include <profiler.h>
#endif


// -- OpenDoc Utilities --

#ifndef _EXCEPT_
// Exceptions define several important macros (ie. CHECKENV)
// which are used in the SOM method dispatch glue. If Except.h
// is not included early enough, exceptions may not be thrown
// correctly when returning from a SOM method with "ev" parameter set.
#include <Except.h>
#endif

// -- LedPart Includes --


#include "LedPartDef.h"

#include "LedPart.h"

#include "FrameProxy.h"

#include	"LedPartCommands.h"

#include	"Embedding.h"

// -- OpenDoc Includes --

#ifndef _ODTYPES_
#include <ODTypes.h>
#endif

#ifndef SOM_Module_OpenDoc_Global_Types_defined // For kODPresDefault
#include <ODTypesM.xh>
#endif

#ifndef SOM_ODPart_xh
#include <Part.xh>
#endif

#ifndef SOM_ODTranslation_xh
#include <Translt.xh>
#endif

#ifndef SOM_ODFacetIterator_xh
#include <FacetItr.xh>
#endif

#ifndef SOM_ODFacet_xh
#include <Facet.xh>
#endif

#ifndef SOM_ODClipboard_xh
#include <Clipbd.xh>
#endif

#ifndef SOM_ODDragItemIterator_xh
#include <DgItmIt.xh>
#endif

#ifndef SOM_ODDragAndDrop_xh
#include <DragDrp.xh>
#endif

#ifndef SOM_ODFrame_xh
#include <Frame.xh>
#endif

#ifndef SOM_ODFrameFacetIterator_xh
#include <FrFaItr.xh>
#endif

#ifndef SOM_ODArbitrator_xh
#include <Arbitrat.xh>
#endif

#ifndef SOM_Module_OpenDoc_Foci_defined
#include <Foci.xh>
#endif

#ifndef SOM_ODShape_xh
#include <Shape.xh>
#endif

#ifndef SOM_Module_OpenDoc_StdProps_defined
#include <StdProps.xh>
#endif

#ifndef SOM_Module_OpenDoc_StdTypes_defined
#include <StdTypes.xh>
#endif

#ifndef SOM_Module_OpenDoc_StdDefs_defined
#include <StdDefs.xh>
#endif

#ifndef SOM_Module_OpenDoc_Commands_defined
#include <CmdDefs.xh>
#endif

#ifndef SOM_ODDraft_xh
#include <Draft.xh>
#endif
#include <Disptch.xh>

#ifndef SOM_ODStorageUnit_xh
#include <StorageU.xh>
#endif

#ifndef SOM_ODStorageUnitView_xh
#include <SUView.xh>
#endif

#ifndef SOM_ODLinkSrc_xh
#include <LinkSrc.xh>
#endif

#ifndef SOM_ODLink_xh
#include <Link.xh>
#endif

#ifndef SOM_ODTransform_xh
#include <Trnsform.xh>
#endif

#ifndef SOM_ODFocusSet_xh
#include <FocusSet.xh>
#endif

#ifndef SOM_ODMenuBar_xh
#include <MenuBar.xh>
#endif

#ifndef SOM_ODWindow_xh
#include <Window.xh>
#endif

#ifndef SOM_ODWindowState_xh
#include <WinStat.xh>
#endif

#ifndef SOM_ODCanvas_xh
#include <Canvas.xh>
#endif

#ifndef SOM_ODSession_xh
#include <ODSessn.xh>
#endif

// -- OpenDoc Utilities --

#ifndef _NMSPCUTL_
#include <NmSpcUtl.h>
#endif

#ifndef _BNDNSUTL_
#include <BndNSUtl.h>
#endif

#ifndef _STDTYPIO_
#include <StdTypIO.h>
#endif

#ifndef SOM_ODInfo_xh
#include <Info.xh>
#endif

#ifndef _TEMPOBJ_
#include <TempObj.h>
#endif

#ifndef _TEMPITER_
#include <TempIter.h>
#endif

#ifndef _ODUTILS_
#include <ODUtils.h>
#endif

#ifndef _STDTYPIO_
#include <StdTypIO.h>
#endif

#ifndef _ORDCOLL_
#include "OrdColl.h"
#endif

#ifndef _ODDEBUG_
#include <ODDebug.h>
#endif

#ifndef _ITEXT_
#include <IText.h>
#endif

#ifndef _FOCUSLIB_
#include <FocusLib.h>
#endif

#ifndef _USERSRCM_
#include <UseRsrcM.h>
#endif

#ifndef _ODMEMORY_
#include <ODMemory.h>
#endif

#ifndef _ISOSTR_
#include <ISOStr.h>
#endif

#ifndef _PLFMDEF_
#include <PlfmDef.h>
#endif

#ifndef _PASCLSTR_
//#include <PasclStr.h>
#endif

#ifndef _STORUTIL_
#include <StorUtil.h>
#endif

#ifndef _WINUTILS_
#include <WinUtils.h>
#endif

#ifndef _UTILERRS_
#include <UtilErrs.h>
#endif

// -- Macintosh Includes --

#ifndef __ERRORS__
#include <Errors.h>
#endif

#ifndef __RESOURCES__
#include <Resources.h>
#endif

#ifndef __DIALOGS__
#include <Dialogs.h>
#endif

#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif

#ifndef __ICONS__
#include <Icons.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __LOWMEM__
#include <LowMem.h>
#endif


#include <Printer.h>

#include	<TempIter.h>


#ifndef mathRoutinesIncludes
#include <math routines.h>
#endif

#include <DlogUtil.h>


#include	"LedContent.h"

#include	"LedFrameEditor.h"




/*
 *	I'm not 100% sure this is safe, it but seems to work and allow the ballon help
 *	in the menus to work. So we'll give it a shot. Leave it easily disablable so
 *	we can lose the feature if things look flakey and I think this might be to blame.
 *	See email 951009- "Jens Alfke" <jens_alfke@powertalk.apple.com>
 */
#ifndef	qOLD_UseResFileHackToEnableBaloonHelp
	//#define	qOLD_UseResFileHackToEnableBaloonHelp	1
	// WIth OpenDoc 1.0 - this breaks - at the very least - the dialog box that should
	// come up when doc is dirty and you say close...
	#define	qOLD_UseResFileHackToEnableBaloonHelp	0
#endif

#if		qOLD_UseResFileHackToEnableBaloonHelp
	static	ODSLong	sLibResVal	=	0;
#endif
#if		qUseResFileHackToEnableBaloonHelp
	static	ODSLong	sLibResVal	=	-1;
#endif




const	kALittleNudge				=		4;
const	kMacWindowTitleBarHeight	=		20;




inline	Environment*	SafeGetEnvironment ()
	{
		Environment*	ev	=	::somGetGlobalEnvironment ();
		Led_AssertNotNil (ev);
		return ev;
	}



/*
 *	Command IDs
 */


#define	kBaseFontNameCmdID				20100
#define	kLastFontNameCmdID				20299

#define	kFontSize9CmdID					20300
#define	kFontSize10CmdID				(kFontSize9CmdID + 1)
#define	kFontSize12CmdID				(kFontSize9CmdID + 2)
#define	kFontSize14CmdID				(kFontSize9CmdID + 3)
#define	kFontSize18CmdID				(kFontSize9CmdID + 4)
#define	kFontSize24CmdID				(kFontSize9CmdID + 5)
#define	kFontSize36CmdID				(kFontSize9CmdID + 6)
#define	kFontSize48CmdID				(kFontSize9CmdID + 7)
#define	kFontSize72CmdID				(kFontSize9CmdID + 8)
#define	kFontSizeOtherCmdID				(kFontSize9CmdID + 9)

#define	kNormalFontStyleCmdID			20400
#define	kBoldFontStyleCmdID				(kNormalFontStyleCmdID+1)
#define	kItalicFontStyleCmdID			(kNormalFontStyleCmdID+2)
#define	kUnderlineFontStyleCmdID		(kNormalFontStyleCmdID+3)
#define	kOutlineFontStyleCmdID			(kNormalFontStyleCmdID+4)

#define	kFindCommand					20500
#define	kFindAgainCommand				20501
#define	kEnterFindStringCommand			20502


#if 0

	inline	int	ToggleStyle (int origStyle, int toggle)
		{
			if (origStyle & toggle) {
				origStyle &= ~toggle;
			}
			else {	
				origStyle |= toggle;
			}
			return origStyle;
		}
#endif


static	bool	sTmpHackHasSBar	=	false;




static	void 			HiliteFacet (ODDragAndDrop* dad, ODFacet* facet, ODBoolean show);
static	ODBoolean		IsValidRect( Rect& rect );
static	ODUShort		CountFramesFacets (ODFrame* frame);
static	Rect			TilePartWindow(Rect* facetBounds, Rect* partWindowBounds);
static	void			GetEditorScriptLanguage(ODScriptCode* script, ODLangCode* language);
static	void 			SetMenuItemText( Environment* ev, ODID menuID, ODID menuItem );
static	ODBoolean		RectContainsRect(Rect& rect, Rect& inside);
static	ODBoolean 		ValueOnClipboard(Environment *ev, ODValueType type, ODSession* session);
static	void			FixedToIntRect(ODRect& fixedRect, Rect& intRect);
static	void			IntToFixedRect(Rect& intRect, ODRect& fixedRect);








/*
 ********************************************************************************
 ************************************ LedPart ***********************************
 ********************************************************************************
 */

// Description:	This is the C++ class constructor.
//
// Warnings:	You are not allowed to throw an exception from this
//				method.
LedPart::LedPart():
	fPrinter (NULL),
	fLastSearchText (NULL),
	fWrapSearch (true),
	fWholeWordSearch (false),
	fCaseSensativeSearch (false)
{
	SOM_Trace ("LedPart","somInit");
	fGroupCount			= 0L;
	fEmbeddedFrames		= kODNULL;
	fHilightedDragFacet	= kODNULL;
	fActiveBorderShape	= kODNULL;

	fDisplayFrameProxies= kODNULL;
	fDirty				= kODFalse;
	fSelf				= NULL;
	fReadOnlyStorage	= kODFalse;
	fPartContent = kODNULL;
}

// Description:	This is the SOM equivalent of a C++ class destructor.
//				In this routine, you should clean up any class members
//				which were not cleaned up in the ReleaseAll and
//				Release methods; and finally call you parent class's
//				somUninit (destructor) method.
//
// Warnings:	You are not allowed to throw an exception from this
//				method.
LedPart::~LedPart()
{
    SOM_Trace ("LedPart", "~LedPart");

}

TextStore_&		LedPart::GetTextStore () const
{
	Led_AssertNotNil (fPartContent);
	return (fPartContent->GetTextStore ());
}

StandardStyledTextImager::SharedStyleDatabase&	LedPart::GetStyleDatabase () const
{
	Led_AssertNotNil (fPartContent);
	return (fPartContent->GetStyleDatabase ());
}

ODSession*	LedPart::GetSession () const
{
	Environment*	ev	=	SafeGetEnvironment ();
	ODStorageUnit* tStorage = GetODPart ()->GetStorageUnit(ev);
	ODSession* tSession = tStorage->GetSession(ev);
	
	return tSession;
}

ODDraft* LedPart::GetDraft() const
{
	Environment*	ev	=	SafeGetEnvironment ();
	return GetODPart ()->GetStorageUnit(ev)->GetDraft(ev);
}

COrderedList* LedPart::GetShapeList() const
{
	return fPartContent->GetShapeList ();
}

COrderedList* LedPart::GetEmbeddedFrames() const
{
	return fEmbeddedFrames;
}

COrderedList* LedPart::GetContentDisplayFrames () const
{
	COrderedList* theList = kODNULL;
	
	if ( fDisplayFrameProxies )
	{
		Environment*	ev	=	SafeGetEnvironment ();
		COrdListIterator fiter(fDisplayFrameProxies);
		for ( CDisplayFrameProxy* frameProxy = (CDisplayFrameProxy*)fiter.First();
				fiter.IsNotComplete();
				frameProxy=(CDisplayFrameProxy*)fiter.Next())
		{
			// This will cause the frame to internalize
			ODFrame* frame = frameProxy->GetFrame(ev);
			
			if (( frame->GetPresentation(ev) == gGlobals->fMainPresentation ) &&
				( frame->GetViewType(ev) == gGlobals->fFrameView ))
			{
				// Only create the list if we find any
				if (!theList)
					theList = new COrderedList;
				
				// Add the found frame to the list	
				theList->AddLast(frame);
			}
		}
	}

	return theList;
}

ODFrame* LedPart::GetFirstSourceFrame ()
{		
	if ( fDisplayFrameProxies ) {
		Environment*	ev	=	SafeGetEnvironment ();
		COrdListIterator fiter(fDisplayFrameProxies);
		for ( CDisplayFrameProxy* frameProxy = (CDisplayFrameProxy*)fiter.First();
				fiter.IsNotComplete();
				frameProxy=(CDisplayFrameProxy*)fiter.Next())
		{
			// $$$$$ This will cause the frame to internalize
			ODFrame* frame = frameProxy->GetFrame(ev);
			FrameEditor* info = (FrameEditor*) frame->GetPartInfo(ev);
 
			if (( frame->GetPresentation(ev) == gGlobals->fMainPresentation ) &&
				( frame->GetViewType(ev) == gGlobals->fFrameView )&&!info->HasSourceFrame())
				{
					return frame;
				}
		}
	}
	return kODNULL;
}

// Description:	This method is called when a new instance of this part
//				is being created. The part should write out all the
//				standard properties and values that the part expects
//				to see.
//
// Warning:		It is not appropriate to require user interaction while
//				stationery is being created. Do not present the user
//				with error dialogs or splash screens from this method.
void	LedPart::od_InitPart (ODStorageUnit* storageUnit, ODPart* partWrapper)
{
	SOM_Trace ("LedPart","InitPart");

	TRY {
		// To allow editor swapping (translation) at runtime, OpenDoc requires
		// that we pass in a "reference" to ourselves when interacting with the
		// API (ie. WindowState::RegisterWindow(), Dispatcher::RegisterIdle, etc).
		// The "partWrapper" passed to us here and in InitPartFromStorage is the
		// "reference" OpenDoc is asking us to use.
		fSelf = partWrapper;
			
		// We are being created, either as part of generating stationery or
		// by some editor instantiating the part, so the destination storage
		// must be writeable.
		fReadOnlyStorage = kODFalse;
			
		// Call the common initialization code to get set up.
		Initialize (storageUnit);


		// Since we have just been created, our state/content info has
		// never been written out, so setting our "dirty" flag will
		// give us a chance to do that.
		SetDirty ();
	}
	CATCH_ALL {
		ODSetSOMException (SafeGetEnvironment (), ErrorCode());

		// Clean up will occur in the destructor which will be called
		// shortly after we return the error.
		RERAISE;
	}
	ENDTRY
}

// Description:	This method is called when a document/stationery is
//				being opened or when the part is internalized by its
//				containing part. The part should merely read in the
//				saved state/content and initialize itself. The part
// 				must not alter its storage unit; otherwise, the "Save"
// 				menu item becomes enabled without the user actually
//				having made a change to the document.
void	LedPart::od_InitPartFromStorage (ODStorageUnit* storageUnit, ODPart* partWrapper)
{
	SOM_Trace ("LedPart","InitPartFromStorage");

	TRY {
		// To allow editor swapping (translation) at runtime, OpenDoc requires
		// that we pass in a "reference" to ourselves when interacting with the
		// API (ie. WindowState::RegisterWindow(), Dispatcher::RegisterIdle, etc).
		// The "partWrapper" passed to us here and in InitPart is the
		// "reference" OpenDoc is asking us to use.
		fSelf = partWrapper;

		// Are we being opened from a read-only draft? If so, we cannot
		// write anything back out to our storage unit.
		fReadOnlyStorage = ( GetDraft ()->GetPermissions (SafeGetEnvironment ()) == kODDPReadOnly );

		// Call the common initialization code to get set up.
		Initialize (storageUnit);

		// Read in the state the part was in when it was last Externalized.
		// This allows the part to present the same "environment" the user
		// had the part set up in the last time it was edited.
		InternalizeStateInfo (storageUnit);

		// Read in the contents for your part editor.
		InternalizeContent (storageUnit);
	}
	CATCH_ALL {
		ODSetSOMException (SafeGetEnvironment (), ErrorCode());

		// Clean up will occur in the destructor which will be called
		// shortly after we return the error.
		RERAISE;
	}
	ENDTRY
}

// Description:	This method is called during the internalization of
//				the part from a document. The methods purpose is to 
//				initialize all fields of the part and to convert 
//				ISO types to tokens for faster comparisons throughout
//				the code.
void	LedPart::Initialize (ODStorageUnit* storageUnit)
{
	SOM_Trace ("LedPart","Initialize");

	Environment*	ev	=	SafeGetEnvironment ();

	// DCS Profiling $$$$$
	#ifdef qProfileLedPart
	OSErr err = ::ProfilerInit(collectDetailed, ticksTimeBase, 200, 22);
	::ProfilerSetStatus(kODFalse);
	#endif
	
	// Grab a reference to the Session object. This is merely for
	// convenience.
	ODSession* session = GetODPart ()->GetStorageUnit(ev)->GetSession(ev);
		
	// Create a list to keep track of the facets we are being
	// displayed in. Also used for maintenance (i.e., Purging memory).
	fDisplayFrameProxies = new COrderedList;

	// Create a content object which knows how to internalize and externalize
	fPartContent = new LedContent (this);
	
	// Create a list to keep track of the embedded frames we have. 
	fEmbeddedFrames = new COrderedList;

	fPrinter = CPrinter::New (ev, storageUnit);

	// First check to see if the library's global variables have
	// been initialized (meaning another part instantiation is already
	// running).

	if ( gGlobalsUsageCount == 0 ) {
		// Allocate globals structure
		gGlobals = new LedPartGlobals;
			
		
		// Cache the ISO representation o 'TEXT' so we don't have to call 
		// translation every time
		ODTranslation* translation = GetSession ()->GetTranslation(ev); 
		gGlobals->fCurrentTextPartKind = translation->GetISOTypeFromPlatformType(ev, 
														'TEXT', kODPlatformDataType);
				
#if 0
		// It is required that parts instantiate their menu bars from 
		// the base OpenDoc menu bar. This maintains consistency in the
		// default menu items and their placement.
		// Since the object is a copy, we can add and subtract menus and
		// items without affecting other running parts.
		gGlobals->fMenuBar = session->GetWindowState(ev)->CopyBaseMenuBar(ev);
#endif

		// Load and register our menus
		ReBuildMenuBar ();

		// We will be using the following foci (shared resources) in this
		// part. For convenience, we tokenize the values here and store
		// them for equivalence tests in the activation methods.
		gGlobals->fClipboardFocus = session->Tokenize(ev, kODClipboardFocus);
		gGlobals->fSelectionFocus = session->Tokenize(ev, kODSelectionFocus);
		gGlobals->fMenuFocus      = session->Tokenize(ev, kODMenuFocus);
		gGlobals->fModalFocus   	= session->Tokenize(ev, kODModalFocus);
		gGlobals->fKeyFocus   	= session->Tokenize(ev, kODKeyFocus);
	
		// $$$$$ COMMENT
		gGlobals->fKeyFocus 		= session->Tokenize(ev, kODKeyFocus);
	
		// Also for convenience, we tokenize our part's main presentation
		// and the standard view types.
		gGlobals->fMainPresentation = session->Tokenize(ev, kMainPresentation);
		gGlobals->fUndefinedPresentation = session->Tokenize(ev, kODPresDefault);
		
		gGlobals->fFrameView 		= session->Tokenize(ev, kODViewAsFrame);
		gGlobals->fLargeIconView 	= session->Tokenize(ev, kODViewAsLargeIcon);
		gGlobals->fSmallIconView 	= session->Tokenize(ev, kODViewAsSmallIcon);
		gGlobals->fThumbnailView 	= session->Tokenize(ev, kODViewAsThumbnail);

		// not sure we need all thiese
		gGlobals->fScrapTextValue = session->GetTranslation(ev)->GetISOTypeFromPlatformType(ev, 'TEXT', kODPlatformDataType);
		#if		qSTTXTSupported
		gGlobals->fStyledTextKind = session->GetTranslation(ev)->GetISOTypeFromPlatformType(ev, 'stxt', kODPlatformDataType);
		#endif
		gGlobals->fSTYLKind = session->GetTranslation(ev)->GetISOTypeFromPlatformType(ev, 'styl', kODPlatformDataType);
		gGlobals->fTextFileValue = session->GetTranslation(ev)->GetISOTypeFromPlatformType(ev, 'TEXT', kODPlatformFileType);
		gGlobals->fAppleHFSFlavor = session->GetTranslation(ev)->GetISOTypeFromPlatformType(ev, 'hfs ', kODPlatformDataType);

		// Lastly, we will package the menu and selection focus
		// so that we can request the "set" at activation time.
		gGlobals->fUIFocusSet = session->GetArbitrator(ev)->CreateFocusSet(ev);
		gGlobals->fUIFocusSet->Add(ev, gGlobals->fMenuFocus);
		gGlobals->fUIFocusSet->Add(ev, gGlobals->fSelectionFocus);
		gGlobals->fUIFocusSet->Add(ev, gGlobals->fKeyFocus);
					
		// Determine what Script/Language the part is localized for.
		// This is important/necessary for creating OpenDoc's text objects.
		GetEditorScriptLanguage(&gGlobals->fEditorsScript, &gGlobals->fEditorsLanguage);
		
		// The first client of the global variables is running.
		gGlobalsUsageCount = 1;
	}
	else {
		// If the globals have been initialized, we just bump the refcount of the
		// menu bar. This prevents will prevent it from going away before we are
		// done using it. We also keep a "usage" count so that we can null out the
		// global variables when we are finished using them.
		gGlobalsUsageCount++;
	}
}

bool	LedPart::od_HasExtension (ODType /*extensionName*/)
{
	return false;
}

void			LedPart::od_ReleaseExtension (ODExtension* /*extension*/)
{
}

ODExtension*	LedPart::od_AcquireExtension (ODType /*extensionName*/)
{
	return NULL;
}

// Description:	This method is called the first time  an instance
//				of this part is initalized to create LedPart
//				specific menus and add them to the menubar.
void	LedPart::ReBuildMenuBar ()
{
    SOM_Trace ("LedPart","ReBuildMenuBar");

 	Environment*	ev	=	SafeGetEnvironment ();

	// Delete any old menubar that may, or may not exist...
	if (gGlobals->fMenuBar != NULL) {
		// Remove our menus from the old menubar.
		gGlobals->fMenuBar->RemoveMenu(ev, kFontMenuID);
		gGlobals->fMenuBar->RemoveMenu(ev, kFontSizeMenuID);
		gGlobals->fMenuBar->RemoveMenu(ev, kFontStyleMenuID);
		gGlobals->fMenuBar->RemoveMenu(ev, kFormatMenuID);
		ODReleaseObject(ev, gGlobals->fMenuBar);
	}

	// Grab a reference to the Session object. This is merely for
	// convenience.
	ODSession* session = GetODPart ()->GetStorageUnit(ev)->GetSession(ev);
		
	// It is required that parts instantiate their menu bars from 
	// the base OpenDoc menu bar. This maintains consistency in the
	// default menu items and their placement.
	// Since the object is a copy, we can add and subtract menus and
	// items without affecting other running parts.
	gGlobals->fMenuBar = session->GetWindowState(ev)->CopyBaseMenuBar(ev);

	ODMenuBar*	menuBar	=	gGlobals->fMenuBar;
	Led_AssertNotNil (menuBar);
	

	// Load menu resources and add them to the menubar
	ODSLong ref;
	ref = BeginUsingLibraryResources();

	TRY	{
		menuBar->AddMenuLast(ev, kFormatMenuID, GetMenu(kFormatMenuID), GetODPart ());

		gGlobals->fFontNameMenu				= ::GetMenu (kFontMenuID);
		::AppendResMenu (gGlobals->fFontNameMenu, 'FOND');
		gGlobals->fFontSizeMenu				= ::GetMenu (kFontSizeMenuID);
		gGlobals->fFontStyleMenu			= ::GetMenu (kFontStyleMenuID);

		menuBar->AddSubMenu(ev, kFontMenuID, gGlobals->fFontNameMenu, GetODPart ());
		menuBar->AddSubMenu(ev, kFontSizeMenuID, gGlobals->fFontSizeMenu, GetODPart ());
		menuBar->AddSubMenu(ev, kFontStyleMenuID, gGlobals->fFontStyleMenu, GetODPart ());
	}
	CATCH_ALL {
		Led_Assert (false);
	}
	ENDTRY
	EndUsingLibraryResources(ref);


// must do something like this to bind command numbers to menus!!!
	for (int i=1; i <= ::CountMItems (gGlobals->fFontNameMenu); i++) {
		Str255	fontName;
		::GetMenuItemText (gGlobals->fFontNameMenu, i, fontName);
		ODSShort	fontNum;
		::GetFNum (fontName, &fontNum);
		menuBar->RegisterCommand (ev, kBaseFontNameCmdID + i, kFontMenuID, i);
	}

	menuBar->RegisterCommand (ev, kFontSize9CmdID, kFontSizeMenuID, 1);
	menuBar->RegisterCommand (ev, kFontSize10CmdID, kFontSizeMenuID, 2);
	menuBar->RegisterCommand (ev, kFontSize12CmdID, kFontSizeMenuID, 3);
	menuBar->RegisterCommand (ev, kFontSize14CmdID, kFontSizeMenuID, 4);
	menuBar->RegisterCommand (ev, kFontSize18CmdID, kFontSizeMenuID, 5);
	menuBar->RegisterCommand (ev, kFontSize24CmdID, kFontSizeMenuID, 6);
	menuBar->RegisterCommand (ev, kFontSize36CmdID, kFontSizeMenuID, 7);
	menuBar->RegisterCommand (ev, kFontSize48CmdID, kFontSizeMenuID, 8);
	menuBar->RegisterCommand (ev, kFontSize72CmdID, kFontSizeMenuID, 9);
	menuBar->RegisterCommand (ev, kFontSizeOtherCmdID, kFontSizeMenuID,11);

	menuBar->RegisterCommand (ev, kNormalFontStyleCmdID, kFontStyleMenuID, 1);
	menuBar->RegisterCommand (ev, kBoldFontStyleCmdID, kFontStyleMenuID, 3);
	menuBar->RegisterCommand (ev, kItalicFontStyleCmdID, kFontStyleMenuID, 4);
	menuBar->RegisterCommand (ev, kUnderlineFontStyleCmdID, kFontStyleMenuID, 5);
	menuBar->RegisterCommand (ev, kOutlineFontStyleCmdID, kFontStyleMenuID, 6);


	// Add find/replace menu items
	{
		ODMenuID		editMenuID	=	0;
		ODMenuItemID	ignored		=	0;
		menuBar->GetMenuAndItem (ev, kODCommandCopy, &editMenuID, &ignored);
		MenuInfo**	editMenu	=	menuBar->GetMenu (ev, editMenuID);
		short		nextMenuItem	=	::CountMItems (editMenu);
		
		::InsertMenuItem (editMenu, "\p-", nextMenuItem);
		nextMenuItem++;
		
		::InsertMenuItem (editMenu, "\pFindÉ", nextMenuItem);
		nextMenuItem++;
		::SetItemCmd (editMenu, nextMenuItem, 'F');
		menuBar->RegisterCommand (ev, kFindCommand, editMenuID, nextMenuItem);

		::InsertMenuItem (editMenu, "\pFind Again", nextMenuItem);
		nextMenuItem++;
		::SetItemCmd (editMenu, nextMenuItem, 'G');
		menuBar->RegisterCommand (ev, kFindAgainCommand, editMenuID, nextMenuItem);

		::InsertMenuItem (editMenu, "\pEnter 'Find' String", nextMenuItem);
		nextMenuItem++;
		::SetItemCmd (editMenu, nextMenuItem, 'E');
		menuBar->RegisterCommand (ev, kEnterFindStringCommand, editMenuID, nextMenuItem);
	}
}

// Description:	This method is called when a kODEvtMouseDown event
//				occurs in the menubar and the part owns the "menu"
//				focus.
//
//				The part enables the appropriate items for its current
//				state and updates the menu items to reflect this
//				editor.
void	LedPart::od_AdjustMenus (ODFrame* frame)
{
    SOM_Trace ("LedPart","AdjustMenus");

 	Environment*	ev	=	SafeGetEnvironment ();

	// The menubar object always calls the root part's AdjustMenus method before
	// calling the menu focus owner's. Because of this, we need to validate the
	// menubar in the case where we are the root part.
	if (frame->IsRoot (ev)) {
	    // We are required to re-validate the menubar before displaying it because
		// any part can/could swap the base menubar at any time.
		if (!gGlobals->fMenuBar->IsValid(ev)) {
			ReBuildMenuBar ();
		}
	}


	#if		qUseResFileHackToEnableBaloonHelp
		Led_Assert (sLibResVal == -1);
		sLibResVal = BeginUsingLibraryResources ();
		Led_Assert (sLibResVal != -1);
	#endif


	AdjustAboutBoxCommand ();

	// Document menu
	AdjustOpenSelectionCommand (frame);
	if (frame->IsRoot (ev)) {
 		// PRINTING: Always enable Page Setup and Print when the root part.
 		// Note: We call AcquireCurrentMenuBar here because, if we support
 		// embedding, we have to enable the print menus whenever we are the
 		// root part of the window, whether or not we have the menu focus.
		TempODMenuBar mbar = GetSession ()->GetWindowState(ev)->AcquireCurrentMenuBar (ev);
		mbar->EnableCommand (ev, kODCommandPageSetup, kODTrue);
		mbar->EnableCommand (ev, kODCommandPrint, kODTrue);
	}


	// Edit menu
	AdjustClipboardCommands (frame);
	AdjustClearCommand (frame);
	AdjustSelectAllCommand (frame);
	AdjustGetPartInfoCommand (frame);
	AdjustPreferencesCommand (frame);
	AdjustViewAsWindowCommand (frame);
	AdjustFindEtcCommand (frame);

	// Format menu
	AdjustFontCommands (frame);
}

void	LedPart::AdjustAboutBoxCommand ()
{
	// Change the "About" menu item text for our part.
	::SetMenuItemText (SafeGetEnvironment (), kODCommandAbout, kAboutTextID);
}

void	LedPart::OnAboutBoxCommand (ODFrame* frame)
{
	DoDialogBox (frame, kAboutBoxID, 0, qLed_ShortVersionString, __DATE__);
}

void	LedPart::AdjustOpenSelectionCommand (ODFrame* frame)
{
	gGlobals->fMenuBar->EnableCommand (SafeGetEnvironment (), kODCommandOpen, LookupFrameEditor (frame).IsOneEmbeddedShape () != NULL);
}

void	LedPart::OnOpenSelectionCommand (ODFrame* frame)
{
	if (Embedding* e = LookupFrameEditor (frame).IsOneEmbeddedShape ()) {
		e->Open (SafeGetEnvironment (), frame);
	}
}

void	LedPart::AdjustGetPartInfoCommand (ODFrame* frame)
{
	gGlobals->fMenuBar->EnableCommand (SafeGetEnvironment (), kODCommandGetPartInfo, LookupFrameEditor (frame).IsOneEmbeddedShape () != NULL);
}

void	LedPart::OnGetPartInfoCommand (ODFrame* frame)
{
	Environment*	ev = SafeGetEnvironment ();

	// Selection should be just one embedded shape
	Embedding* tShape = LookupFrameEditor (frame).IsOneEmbeddedShape ();
	THROW_IF_NULL(tShape);
	
	// We should always have at least one frame available
	// Actually, this could fail if the part is scrolled off screen
	// DCS $$$$$
	ODFrame* tFrame = tShape->GetAnyFrame(ev);
	THROW_IF_NULL(tFrame);
	
	// Get any facet to pass to ShowPartFrameInfo
	TempODFrameFacetIterator facets(ev,tFrame);
	ODInfo* info = GetSession ()->GetInfo(ev);
	info->ShowPartFrameInfo( ev,  facets.Current(), !IsReadOnly());
}

void	LedPart::AdjustPreferencesCommand (ODFrame* /*frame*/)
{
	gGlobals->fMenuBar->EnableCommand (SafeGetEnvironment (), kODCommandPreferences, true);
}

void	LedPart::OnPreferencesCommand (ODFrame* frame)
{
	sTmpHackHasSBar = LookupFrameEditor (frame).GetHasScrollBar ();
	if (DoDialogBox (frame, kPrefs_DialogID, 0, "", "") == ok) {
		LookupFrameEditor (frame).SetHasScrollBar (sTmpHackHasSBar);
	}
}

void	LedPart::AdjustViewAsWindowCommand (ODFrame* frame)
{
	// Enable the "View As Window" command always while we are not the root part.
	gGlobals->fMenuBar->EnableCommand (SafeGetEnvironment (), kODCommandViewAsWin, !frame->IsRoot (SafeGetEnvironment ()));
}

void	LedPart::OnViewAsWindowCommand (ODFrame* frame)
{
	od_Open (frame);
}

void	LedPart::AdjustSelectAllCommand (ODFrame* /*frame*/)
{
	gGlobals->fMenuBar->EnableCommand (SafeGetEnvironment (), kODCommandSelectAll, GetTextStore ().GetLength () != 0);
}

void	LedPart::OnSelectAllCommand (ODFrame* frame)
{
	LookupFrameEditor (frame).OnSelectAllCommand ();
}

void	LedPart::AdjustClearCommand (ODFrame* frame)
{
	FrameEditor&	frameEditor	=	LookupFrameEditor (frame);
	gGlobals->fMenuBar->EnableCommand (SafeGetEnvironment (), kODCommandClear, not IsReadOnly () and (frameEditor.GetSelectionStart () != frameEditor.GetSelectionEnd ()));
}

void	LedPart::OnClearCommand (ODFrame* frame)
{
	FrameEditor&	frameEditor	=	LookupFrameEditor (frame);
	frameEditor.GetEditor ()->Replace (frameEditor.GetSelectionStart (), frameEditor.GetSelectionEnd (), "", 0);
	frameEditor.GetEditor ()->ScrollToSelection (TextInteractor_::eDelayedUpdate);
}

void	LedPart::AdjustClipboardCommands (ODFrame* frame)
{
	Led_RequireNotNil (frame);

	Environment*	ev = ::SafeGetEnvironment ();

	FrameEditor&	frameEditor	=	LookupFrameEditor (frame);
	bool	emptySelection	=	(frameEditor.GetSelectionStart () == frameEditor.GetSelectionEnd ());

	ODBoolean canModify = !IsReadOnly();
	ODBoolean canClear = canModify;

	ODArbitrator* tArbitrator = GetSession ()->GetArbitrator(ev);
	ODFrame* clipboardFocusOwner = tArbitrator->AcquireFocusOwner(ev, gGlobals->fClipboardFocus);
	if ( (frame == clipboardFocusOwner) || 
		 (tArbitrator->RequestFocus(ev, gGlobals->fClipboardFocus, frame)) )
	{
		// This will allow the pasting of foreign content.  
		// Pass kODNULL for the kind, to just test for content.
		ODBoolean canPaste = canModify && ValueOnClipboard(ev, kODNULL, GetSession ());

		// enable cut only if we have a selection and the draft is modifiable
		gGlobals->fMenuBar->EnableCommand(ev, kODCommandCut, canClear);
		
		// Can only copy if we have a selection
		gGlobals->fMenuBar->EnableCommand(ev, kODCommandCopy, !emptySelection);
		
		// enable paste only if we support the type available & the drafts modifiable
		gGlobals->fMenuBar->EnableCommand(ev, kODCommandPaste, canPaste);
		gGlobals->fMenuBar->EnableCommand(ev, kODCommandPasteAs, canPaste);
	}
	ODReleaseObject(ev, clipboardFocusOwner);
}

void	LedPart::OnCutCommand (ODFrame* frame)
{
	CCutCopyCommand* command = new CCutCopyCommand (this,  frame, kODCommandCut);
	ExecuteCommand (command);
	LookupFrameEditor (frame).GetEditor ()->ScrollToSelection (TextInteractor_::eDelayedUpdate);
}

void	LedPart::OnCopyCommand (ODFrame* frame)
{
	CCutCopyCommand* command = new CCutCopyCommand (this, frame, kODCommandCopy);
	ExecuteCommand (command);
}

void	LedPart::OnPasteCommand (ODFrame* frame)
{
	CPasteCommand* command = new CPasteCommand(this, frame);
	ExecuteCommand (command);
	LookupFrameEditor (frame).GetEditor ()->ScrollToSelection (TextInteractor_::eDelayedUpdate);
}

void	LedPart::OnPasteAsCommand (ODFrame* frame)
{
	Environment*	ev = SafeGetEnvironment ();

	// Get any facet to pass to ShowPasteAsDlg
	TempODFrameFacetIterator facets(ev,frame);
	ODFacet*	facet	=	facets.Current ();
	// Get the clipboard storage unit and internalize it
	ODClipboard* clip = GetSession ()->GetClipboard(ev);
	
	// Get the clipboard content storage unit
	ODStorageUnit* contentSU = clip->GetContentStorageUnit(ev);
	
	ASSERT_NOT_NULL(contentSU->Exists(ev, kODPropContents, kODNULL, 0));

	ODBoolean result;
	ODBoolean canMerge = contentSU->Exists(ev, kODPropContents, kLedPartKind, 0);
	ODBoolean canEmbedSingleFrame = contentSU->Exists(ev, kODPropContentFrame, kODWeakStorageUnitRef, 0);
	ODPasteAsMergeSetting mergeSetting;
	ODPasteAsResult paResult;
	
	if (canMerge)
	{
		if (canEmbedSingleFrame)
			mergeSetting = kODPasteAsEmbed;
		else
			mergeSetting = kODPasteAsMerge;
	}
	else
		mergeSetting = kODPasteAsEmbedOnly;
			
	// ---- Display the Paste As... dialog ----
	result = clip->ShowPasteAsDialog(ev, kODTrue, mergeSetting, 
									 facet,	// facet from which dialog is triggered
									 gGlobals->fFrameView, // viewType of data
									  &paResult);
		
	if (result) {
		ODBoolean doLink = paResult.pasteLinkSetting == kODTrue;
		try {
			if (doLink) {
				ODBoolean defaultIsMerge = (mergeSetting == kODPasteAsMerge);
				Led_Assert (false);	// NYI
			}
			else {
				// Since this part only supports 1 kind, we can ignore that translation
				// result portion of paResult. The paste as command will do the right
				// thing in the native kind case. We need to worry about the force
				// embed case.
				CPasteAsCommand* command = new CPasteAsCommand(this, frame, !paResult.mergeSetting);
				ExecuteCommand (command);
			}
		}
		catch (...) {
			// Dispose of pasteAsResult fields
			ODDisposePtr(paResult.selectedKind);
			ODDisposePtr(paResult.translateKind);
			ODDisposePtr(paResult.editor);
			throw;
		}
	}
	
	// Dispose of pasteAsResult fields
	ODDisposePtr(paResult.selectedKind);
	ODDisposePtr(paResult.translateKind);
	ODDisposePtr(paResult.editor);


	LookupFrameEditor (frame).GetEditor ()->ScrollToSelection (TextInteractor_::eDelayedUpdate);
}

void	LedPart::AdjustFindEtcCommand (ODFrame* frame)
{
	Environment*	ev = SafeGetEnvironment ();

	ODMenuBar*	menuBar	=	gGlobals->fMenuBar;
	Led_AssertNotNil (menuBar);

	size_t	selStart	=	LookupFrameEditor (frame).GetSelectionStart ();
	size_t	selEnd		=	LookupFrameEditor (frame).GetSelectionEnd ();

	menuBar->EnableCommand (ev, kFindCommand, true);
	menuBar->EnableCommand (ev, kFindAgainCommand, fLastSearchText != NULL);
	menuBar->EnableCommand (ev, kEnterFindStringCommand, selStart != selEnd);
}

void	LedPart::OnFindEtcCommand (ODFrame* frame, ODCommandID theMenuCommand)
{
	if (theMenuCommand == kFindCommand) {
		if (DoDialogBox (frame, kFind_DialogID, 0, "", "") != ok) {
			return;
		}
	}

	// search for last text entered into find dialog (could have been a find again).
	size_t	origSelStart	=	LookupFrameEditor (frame).GetSelectionStart ();
	size_t	origSelEnd		=	LookupFrameEditor (frame).GetSelectionEnd ();
#if 1
	TextStore_::SearchParameters sp (fLastSearchText, fWrapSearch, fWholeWordSearch, fCaseSensativeSearch);
	size_t	whereTo	=	GetTextStore ().Find (sp, origSelEnd);
#else
	size_t	whereTo	=	GetTextStore ().Find (fLastSearchText, origSelEnd, fWrapSearch? TextStore_::eWrapAtEnd: TextStore_::eSearchToEnd, fCaseSensativeSearch, fWholeWordSearch);
#endif
	if ((whereTo == kBadIndex) or (whereTo == origSelStart)) {
		Led_BeepNotify ();
	}
	else {
		LookupFrameEditor (frame).SetSelection (whereTo, whereTo + strlen (fLastSearchText));
		LookupFrameEditor (frame).GetEditor ()->ScrollToSelection ();
	}
}

void	LedPart::OnEnterFindStringCommand (ODFrame* frame)
{
	size_t	selStart	=	LookupFrameEditor (frame).GetSelectionStart ();
	size_t	selEnd		=	LookupFrameEditor (frame).GetSelectionEnd ();
	size_t	selLength	=	selEnd-selStart;

	delete[] fLastSearchText;
	fLastSearchText = NULL;
	
	fLastSearchText = new Led_tChar [selLength + 1];
	GetTextStore ().CopyOut (selStart, selLength, fLastSearchText);
	fLastSearchText[selLength] = '\0';
}


inline	short	FontCmdToFontID (MenuHandle	fontMenu, long cmd)
	{
		static	Led_Array<short>	sFontIDMapCache;	// OK to keep static cuz never changes during run of app

		Led_Require (cmd >= kBaseFontNameCmdID);
		Led_Require (cmd <= kLastFontNameCmdID);
		size_t	idx	=	cmd-kBaseFontNameCmdID;

		// Pre-fill cache - at least to the cmd were looking for...
		for (size_t i = sFontIDMapCache.GetLength (); i <= idx; i++) {
			Str255		pFontName = {0};
			::GetMenuItemText (fontMenu, idx, pFontName);
			short	familyID	=	0;
			::GetFNum (pFontName, &familyID);
			sFontIDMapCache.Append (familyID);
		}
		return sFontIDMapCache[idx + qLedFirstIndex];
	}



void	LedPart::AdjustFontCommands (ODFrame* frame)
{
	Environment*	ev = SafeGetEnvironment ();

	ODMenuBar*	menuBar	=	gGlobals->fMenuBar;
	Led_AssertNotNil (menuBar);

	Led_AssertNotNil (LookupFrameEditor (frame).GetEditor ());

	#if		qUsingStyledText
		FrameEditor&	frameEditor	=	LookupFrameEditor (frame);
		Led_Assert (frameEditor.GetSelectionEnd () >= frameEditor.GetSelectionStart ());
		size_t	selectionLength	=	frameEditor.GetSelectionEnd () - frameEditor.GetSelectionStart ();
		Led_IncrementalFontSpecification	fontSpec = frameEditor.GetEditor ()->GetContinuousStyleInfo (frameEditor.GetSelectionStart (), selectionLength);
	#if 0
		unsigned int	styleFlags = StandardStyledTextImager::eDCSF_doFont | StandardStyledTextImager::eDCSF_doFace | StandardStyledTextImager::eDCSF_doSize;
		FrameEditor&	frameEditor	=	LookupFrameEditor (frame);
		Led_Assert (frameEditor.GetSelectionEnd () >= frameEditor.GetSelectionStart ());
		size_t	selectionLength	=	frameEditor.GetSelectionEnd () - frameEditor.GetSelectionStart ();
		Led_IncrementalFontSpecification	fontSpec	=	frameEditor.GetEditor ()->DoContinuousStyle (frameEditor.GetSelectionStart (), selectionLength, &styleFlags);
	#endif
	#else
		Led_IncrementalFontSpecification	fontSpec	=	LookupFrameEditor (frame).GetEditor ()->GetDefaultFont ();
	#endif

	// Font NAME menu
	ODPlatformMenu	fontNameMenu	=	gGlobals->fFontNameMenu;
	size_t			nMenuItems		=	::CountMItems (fontNameMenu);
	for (int i=1; i <= nMenuItems; i++) {
#if 1
		if (fontSpec.GetFontNameSpecifier_Valid ()) {
			short	fontNum	=	FontCmdToFontID (fontNameMenu, kBaseFontNameCmdID + i);
			menuBar->CheckCommand (ev, kBaseFontNameCmdID + i, fontSpec.GetFontNameSpecifier () == fontNum);
		}
		else {
			menuBar->CheckCommand (ev, kBaseFontNameCmdID + i,  false);
		}
#else
		Str255	fontName;
		::GetMenuItemText (fontNameMenu, i, fontName);
		ODSShort	fontNum;
		::GetFNum (fontName, &fontNum);
		menuBar->CheckCommand (ev, kBaseFontNameCmdID + i, fontSpec.fFontIDValid and (fontNum == fontSpec.fFontID));
#endif
	}


//		menuBar->CheckCommand (ev, kBaseFontNameCmdID + i, fontSpec.GetPointSize_Valid () and fontSpec.GetPointSize () == chosenFontSize);



	// Font SIZE menu
	ODPlatformMenu	fontSizeMenu	=	gGlobals->fFontSizeMenu;
	if (fontSpec.GetFontNameSpecifier_Valid ()) {
		::SetItemStyle (fontSizeMenu, 1, (::RealFont (fontSpec.GetFontNameSpecifier (), 9) ? outline : normal)); 
		::SetItemStyle (fontSizeMenu, 2, (::RealFont (fontSpec.GetFontNameSpecifier (),10) ? outline : normal)); 
		::SetItemStyle (fontSizeMenu, 3, (::RealFont (fontSpec.GetFontNameSpecifier (),12) ? outline : normal)); 
		::SetItemStyle (fontSizeMenu, 4, (::RealFont (fontSpec.GetFontNameSpecifier (),14) ? outline : normal)); 
		::SetItemStyle (fontSizeMenu, 5, (::RealFont (fontSpec.GetFontNameSpecifier (),18) ? outline : normal)); 
		::SetItemStyle (fontSizeMenu, 6, (::RealFont (fontSpec.GetFontNameSpecifier (),24) ? outline : normal)); 
		::SetItemStyle (fontSizeMenu, 7, (::RealFont (fontSpec.GetFontNameSpecifier (),36) ? outline : normal)); 
		::SetItemStyle (fontSizeMenu, 8, (::RealFont (fontSpec.GetFontNameSpecifier (),48) ? outline : normal)); 
		::SetItemStyle (fontSizeMenu, 9, (::RealFont (fontSpec.GetFontNameSpecifier (),72) ? outline : normal));
	}
	else {
		::SetItemStyle (fontSizeMenu, 1, normal); 
		::SetItemStyle (fontSizeMenu, 2, normal); 
		::SetItemStyle (fontSizeMenu, 3, normal); 
		::SetItemStyle (fontSizeMenu, 4, normal); 
		::SetItemStyle (fontSizeMenu, 5, normal); 
		::SetItemStyle (fontSizeMenu, 6, normal); 
		::SetItemStyle (fontSizeMenu, 7, normal); 
		::SetItemStyle (fontSizeMenu, 8, normal); 
		::SetItemStyle (fontSizeMenu, 9, normal);
	}
	menuBar->CheckCommand (ev, kFontSize9CmdID,  fontSpec.GetPointSize_Valid () and (fontSpec.GetPointSize () ==  9));
	menuBar->CheckCommand (ev, kFontSize10CmdID, fontSpec.GetPointSize_Valid () and (fontSpec.GetPointSize () == 10));
	menuBar->CheckCommand (ev, kFontSize12CmdID, fontSpec.GetPointSize_Valid () and (fontSpec.GetPointSize () == 12));
	menuBar->CheckCommand (ev, kFontSize14CmdID, fontSpec.GetPointSize_Valid () and (fontSpec.GetPointSize () == 14));
	menuBar->CheckCommand (ev, kFontSize18CmdID, fontSpec.GetPointSize_Valid () and (fontSpec.GetPointSize () == 18));
	menuBar->CheckCommand (ev, kFontSize24CmdID, fontSpec.GetPointSize_Valid () and (fontSpec.GetPointSize () == 24));
	menuBar->CheckCommand (ev, kFontSize36CmdID, fontSpec.GetPointSize_Valid () and (fontSpec.GetPointSize () == 36));
	menuBar->CheckCommand (ev, kFontSize48CmdID, fontSpec.GetPointSize_Valid () and (fontSpec.GetPointSize () == 48));
	menuBar->CheckCommand (ev, kFontSize72CmdID, fontSpec.GetPointSize_Valid () and (fontSpec.GetPointSize () == 72));

	// Font STYLE menu
	menuBar->CheckCommand (ev, kNormalFontStyleCmdID, 		fontSpec.GetStyle_Plain_Valid () and fontSpec.GetStyle_Plain ());
	menuBar->CheckCommand (ev, kBoldFontStyleCmdID, 		fontSpec.GetStyle_Bold_Valid () and fontSpec.GetStyle_Bold ());
	menuBar->CheckCommand (ev, kItalicFontStyleCmdID, 		fontSpec.GetStyle_Italic_Valid () and fontSpec.GetStyle_Italic ());
	menuBar->CheckCommand (ev, kUnderlineFontStyleCmdID, 	fontSpec.GetStyle_Underline_Valid () and fontSpec.GetStyle_Underline ());
	menuBar->CheckCommand (ev, kOutlineFontStyleCmdID, 	fontSpec.GetStyle_Outline_Valid () and fontSpec.GetStyle_Outline ());
}

void	LedPart::OnFontCommand (ODFrame* frame, ODCommandID theMenuCommand)
{
	Led_AssertNotNil (LookupFrameEditor (frame).GetEditor ());
	Led_IncrementalFontSpecification	fontSpec;

	// Font NAME
	if (theMenuCommand >= kBaseFontNameCmdID and theMenuCommand <= kLastFontNameCmdID) {
		ODPlatformMenu	fontNameMenu	=	gGlobals->fFontNameMenu;
		Str255			fontName;
		::GetMenuItemText (fontNameMenu, theMenuCommand-kBaseFontNameCmdID, fontName);
		ODSShort	fontNum	=	geneva;	// default in case something goes wrong...
		::GetFNum (fontName, &fontNum);
#if 0
		fontSpec.fFontID = fontNum;
		fontSpec.fFontIDValid = true;
#endif
		fontSpec.SetFontNameSpecifier (fontNum);
	}
	else {
		switch (theMenuCommand) {
#if 1
			// Font SIZE
			case	kFontSize9CmdID:		fontSpec.SetPointSize (9);	break;
			case	kFontSize10CmdID:		fontSpec.SetPointSize (10);	break;
			case	kFontSize12CmdID:		fontSpec.SetPointSize (12);	break;
			case	kFontSize14CmdID:		fontSpec.SetPointSize (14);	break;
			case	kFontSize18CmdID:		fontSpec.SetPointSize (18);	break;
			case	kFontSize24CmdID:		fontSpec.SetPointSize (24);	break;
			case	kFontSize36CmdID:		fontSpec.SetPointSize (36);	break;
			case	kFontSize48CmdID:		fontSpec.SetPointSize (48);	break;
			case	kFontSize72CmdID:		fontSpec.SetPointSize (72);	break;
			case	kFontSizeOtherCmdID:	fontSpec.SetPointSize (9);	Led_Assert (false); /*NYI*/ break;

			// Font STYLE
			case	kNormalFontStyleCmdID:		fontSpec.SetStyle_Plain ();	break;
			case	kBoldFontStyleCmdID:
			case	kItalicFontStyleCmdID:
			case	kUnderlineFontStyleCmdID:
			case	kOutlineFontStyleCmdID: {
				FrameEditor&	frameEditor	=	LookupFrameEditor (frame);
				Led_Assert (frameEditor.GetSelectionEnd () >= frameEditor.GetSelectionStart ());
				size_t	selectionLength	=	frameEditor.GetSelectionEnd () - frameEditor.GetSelectionStart ();
				Led_IncrementalFontSpecification	origFontSpec = frameEditor.GetEditor ()->GetContinuousStyleInfo (frameEditor.GetSelectionStart (), selectionLength);
				switch (theMenuCommand) {
					case	kBoldFontStyleCmdID:		fontSpec.SetStyle_Bold (not (origFontSpec.GetStyle_Bold_Valid () and origFontSpec.GetStyle_Bold ())); break;
					case	kItalicFontStyleCmdID:		fontSpec.SetStyle_Italic (not (origFontSpec.GetStyle_Italic_Valid () and origFontSpec.GetStyle_Italic ())); break;
					case	kUnderlineFontStyleCmdID:	fontSpec.SetStyle_Underline (not (origFontSpec.GetStyle_Underline_Valid () and origFontSpec.GetStyle_Underline ())); break;
					case	kOutlineFontStyleCmdID:		fontSpec.SetStyle_Outline (not (origFontSpec.GetStyle_Outline_Valid () and origFontSpec.GetStyle_Outline ())); break;
				}
			}
			break;
#else
			// Font SIZE
			case	kFontSize9CmdID:		fontSpec.fFontSize = 9;		fontSpec.fFontSizeValid = true;	break;
			case	kFontSize10CmdID:		fontSpec.fFontSize = 10;	fontSpec.fFontSizeValid = true;	break;
			case	kFontSize12CmdID:		fontSpec.fFontSize = 12;	fontSpec.fFontSizeValid = true;	break;
			case	kFontSize14CmdID:		fontSpec.fFontSize = 14;	fontSpec.fFontSizeValid = true;	break;
			case	kFontSize18CmdID:		fontSpec.fFontSize = 18;	fontSpec.fFontSizeValid = true;	break;
			case	kFontSize24CmdID:		fontSpec.fFontSize = 24;	fontSpec.fFontSizeValid = true;	break;
			case	kFontSize36CmdID:		fontSpec.fFontSize = 36;	fontSpec.fFontSizeValid = true;	break;
			case	kFontSize48CmdID:		fontSpec.fFontSize = 48;	fontSpec.fFontSizeValid = true;	break;
			case	kFontSize72CmdID:		fontSpec.fFontSize = 72;	fontSpec.fFontSizeValid = true;	break;
			case	kFontSizeOtherCmdID:	fontSpec.fFontSize = 9;	Led_Assert (false); /*NYI*/ break;

			// Font STYLE
			case	kNormalFontStyleCmdID:		fontSpec.fFontStyle = 0;											fontSpec.fFontStyleValid = true;	break;
			case	kBoldFontStyleCmdID:		fontSpec.fFontStyle = ToggleStyle (fontSpec.fFontStyle, bold);		fontSpec.fFontStyleValid = true;	break;
			case	kItalicFontStyleCmdID:		fontSpec.fFontStyle = ToggleStyle (fontSpec.fFontStyle, italic);	fontSpec.fFontStyleValid = true;	break;
			case	kUnderlineFontStyleCmdID:	fontSpec.fFontStyle = ToggleStyle (fontSpec.fFontStyle, underline);	fontSpec.fFontStyleValid = true;	break;
			case	kOutlineFontStyleCmdID:		fontSpec.fFontStyle = ToggleStyle (fontSpec.fFontStyle, outline);	fontSpec.fFontStyleValid = true;	break;
#endif

			default:	Led_Assert (false);	// not reached
		}
	}

	LookupFrameEditor (frame).GetEditor ()->SetDefaultFont (fontSpec);
}

// Description:	This method is called each time an object releases the
//				part. If the refcount falls to 0, the part should
//				release the GetODPart() part reference.
//
// Warning:		If the part releases any other object when the refcount falls to
//				zero, it will need to override the Acquire method so
//				that the object can be referenced again if the parts refcount
//				should be incremented before it is deleted.
void	LedPart::od_Release ()
{
    SOM_Trace ("LedPart","Release");

	Environment*	ev	=	SafeGetEnvironment ();

	if (GetODPart ()->GetRefCount(ev) == 0) {
		GetDraft ()->ReleasePart (ev, GetODPart ());
		#if		qDebug
			fSelf = NULL;
		#endif
	}
}

// Description:	This method is called just prior to the part being
//				deleted by the Draft. The part must release all
//				references to all refcounted objects it has stored
//				internally; not doing so, will cause an "invalid ref
//				count" exception/error.
void	LedPart::od_ReleaseAll ()
{
    SOM_Trace ("LedPart","ReleaseAll");

	Environment*	ev	=	SafeGetEnvironment ();

	// DCS Profiling $$$$$
	#ifdef qProfileLedPart
	char x[255] = "/pLedPartProfile";
	OSErr err = ::ProfilerDump((StringPtr)"\pServo:LedPartProfile");
	::ProfilerTerm();
	#endif
	
	// Release our display frames and delete their proxy
	if (fDisplayFrameProxies)
	{
		COrdListIterator fiter(fDisplayFrameProxies);
		for ( CDisplayFrameProxy* frameProxy = (CDisplayFrameProxy*)fiter.First();
				fiter.IsNotComplete();
				frameProxy=(CDisplayFrameProxy*)fiter.Next())
		{
			if (frameProxy->IsFrameInMemory())
			{
				ODFrame* frame = frameProxy->GetFrame(ev);
				ODReleaseObject(ev,frame);
			}
			delete frameProxy;
		}
	}
	
	// 
	if ( fDisplayFrameProxies )
	{
		ODDeleteObject(fDisplayFrameProxies);
		fDisplayFrameProxies = kODNULL;
	}
	
	// Release the active border shape
	if (fActiveBorderShape)
	{
		ODReleaseObject(ev,fActiveBorderShape);
	}
	
	// If the last part instance using the globals is released,
	// we need to NULL out the globals.
	
	if ( --gGlobalsUsageCount == 0 )
	{
		// Release the menu bar
		ODReleaseObject(ev,gGlobals->fMenuBar);
		
		// the FocusSet is not shared so  delet it
		if ( gGlobals->fUIFocusSet )
		{
			ODDeleteObject(gGlobals->fUIFocusSet);
			gGlobals->fUIFocusSet = kODNULL;
		}
	}
	
    ODDeleteObject(fPrinter);
    ODDeleteObject(fPartContent);
    ODDeleteObject(fEmbeddedFrames);
}

// Description:	This method is called when the OpenDoc requires more
//				memory for allocating objects and just before a part
//				is deleted. The part should free up as much memory as
//				it can.
//
//				The part determines which views are being "used" in
//				its display frames. The resources for the unused view
//				types are then purged.
ODSize	LedPart::od_Purge (ODSize /*size*/)
{
    SOM_Trace ("LedPart","Purge");

	return 0;
}

// Description:	This method could be used to read in settings
//				information for the part. This would be information
//				related to the workings of the part editor, not the
//				content.
//
// Note:		The function StorageUnitGetValue simplifies the use of
//				ODByteArrary, which is required by the StorageUnit
//				interface. Look in StorUtil.h/cpp for an example of
//				using the ODByteArray struct.
//------------------------------------------------------------------------------
void	LedPart::InternalizeStateInfo (ODStorageUnit* /*storageUnit*/)
{
    SOM_Trace ("LedPart","InternalizeStateInfo");
}

// Description:	This method is called during interalization of the
//				part. Document state info, content of part, as well as any link info
// 				is read in. The work is all done by the LedContent object.
//------------------------------------------------------------------------------
void	LedPart::InternalizeContent (ODStorageUnit* storageUnit)
{
	SOM_Trace ("LedPart","InternalizeContent");

	// focus to our content property.
	Environment*	ev	=	SafeGetEnvironment ();
	TRY {
		if (storageUnit->Exists(ev, kODPropContents, kLedPartKind, 0)) {
			storageUnit->Focus(ev, kODPropContents, kODPosUndefined, kLedPartKind, 0, kODPosUndefined);
			fPartContent->InternalizeOurContent (storageUnit, kODNULL, qLedFirstIndex);
		}
	}
	CATCH_ALL {
		ODSetSOMException (ev, ErrorCode());
	}
	ENDTRY
}

// Description:	When a frame is being cloned by the Draft, it will ask the owner
//				(part) to clone its info annotation on the frame.
//
//				The part uses a C++ helper class to encapsulate the information
//				we store with each frame, so we let it clone itself to the
//				storage unit view.
void	LedPart::od_ClonePartInfo (ODDraftKey key, ODInfoType partInfo, ODStorageUnitView* storageUnitView, ODFrame* scopeFrame)
{
    SOM_Trace ("ClonePartInfo","ClonePartInfo");

	// Tell our frame info class to write itself out into the pre-
	// focused storage unit.
	((FrameEditor*) partInfo)->CloneInto (key, storageUnitView, scopeFrame);
}

// Description:	This method is called during cloning, typically
//				inside of Data Interchange operations. The part should
//				write out the current state and contents.
void	LedPart::od_CloneInto (ODDraftKey key, ODStorageUnit* destinationSU, ODFrame* scopeFrame)
{
	SOM_Trace ("LedPart","CloneInto");

	Environment*	ev	=	SafeGetEnvironment ();

	// We must first verify that we've never written to this storage unit.
	// If we have, we should do nothing, otherwise we need to write out
	// the current state of the part content.
	if (destinationSU->Exists (ev, kODPropContents, kLedPartKind, 0) == kODFalse) {
		
		// $$$$$ This is where, we should check for the existence of the kODPropContentFrame property
		// and write a promise if it's there.  When doing so, we will need to read any kODPropCloneKind's kODCloneKind value
		// and use that in our cloneKind record.  Otherwise the promise, when fulfilled may call BeginClone with the wrong
		// cloneKind (ie one different from the containing part which called BeginClone before cloning this as a single
		// embedded frame.
		
		// based
		// Add the properties we need to successfully externalize
		// ourselves into the destination storage unit.
		CheckAndAddProperties (destinationSU);
				
		// Assemble the bits of info necessary for the clone operation.
		// These are the: key from the clone begun by OD, the source draft,
		// the scope frame, and the clone kind.
		CloneInfo info (key, GetDraft (), scopeFrame, kODCloneCopy);
		
		// Write out the part's state information.
		ExternalizeStateInfo (destinationSU);
		
		// Write out the part's content.
		ExternalizeContent (destinationSU, &info);
	}
}

// Description:	Write out our persistent state if changes have
//				occurred and if our storage is writeable. The part
//				must call its parent class behavior because one or
//				more of our parent classes contains implementation.
void	LedPart::od_Externalize ()
{
    SOM_Trace ("LedPart","Externalize");

	if ( fDirty && !IsReadOnly()) {
		Environment*	ev 	=	SafeGetEnvironment ();
		TRY
			// Get our storage unit.
			ODStorageUnit* storageUnit = GetODPart ()->GetStorageUnit(ev);
	
			// Verify that the storage unit has the appropriate properties
			// and values to allow us to run. If not, add them.
			CheckAndAddProperties (storageUnit);
		
			// Verify that there are no "bogus" values in the Content
			// property.
			CleanseContentProperty (storageUnit);
		
			// Write out the part's state information.
			ExternalizeStateInfo (storageUnit);
				
			// Write out the part's content.
			ExternalizeContent (storageUnit, kODNULL);

		CATCH_ALL
			// Alert the user of the problem.
			DoDialogBox (kODNULL, kErrorBoxID, kErrExternalizeFailed);
			// Change the exception value, so the DocShell doesn't display an
			// error dialog.
			SetErrorCode(kODErrAlreadyNotified);
			// Alert the caller.
			RERAISE;
		ENDTRY

		// Flag our part as no longer being dirty.
		fDirty = kODFalse;
	}
}

// Description:	This method is called during externalization pr cloning of the
//				part. The current "state" of the editor should be
//				written out. This "state" information may be lost
//				during Data Interchange operations, so the part needs
//				to recover gracefully if information is missing or
//				incomplete.
void	LedPart::ExternalizeStateInfo (ODStorageUnit* storageUnit)
{
    SOM_Trace ("LedPart","ExternalizeStateInfo");

	Led_AssertNotNil (fPrinter);
	fPrinter->Externalize (SafeGetEnvironment (), storageUnit);


// Externalize the part's display frame list.
// don't do this anymore - not sure - was causing some problems with other embedded frames, unclear if needed???
// must read so-called RECIPIES on this crap!!!
// LGP 951211

	storageUnit->Focus (SafeGetEnvironment (), kODPropDisplayFrames, kODPosUndefined, kODWeakStorageUnitRefs, 0, kODPosUndefined);
}

// Description:	This method is called during exteralization of the
//				part. The content of the part should be written out.
//
// Note: This method adds an additional parameter unlike other parts derived
// from LedPart. The cloneInfo parameter is used to communicate clone info
// passed into the CloneInto method so that the editor may clone embedded its
// embedded frames to the given storage.
void	LedPart::ExternalizeContent (ODStorageUnit* storageUnit,  CloneInfo* cloneInfo)
{
    SOM_Trace ("LedPart","ExternalizeContent");

	TRY
#if 1
		// maybe should pay attention to the prefered kind? From ChangePartKind??? - LGP 951214
		//fPartContent->ExternalizeNativeContent (storageUnit, cloneInfo);
// externalize all formats...
		fPartContent->Externalize (storageUnit, cloneInfo);
#else
		// focus to our content property.
		if ( storageUnit->Exists(ev, kODPropContents, kLedPartKind, 0) )
		{
			storageUnit->Focus(ev, kODPropContents, kODPosUndefined,
									kLedPartKind, 0, kODPosUndefined);
	
			fPartContent->Externalize (storageUnit, cloneInfo);
		}
#endif
	CATCH_ALL
		ODSetSOMException(SafeGetEnvironment (), ErrorCode());
	ENDTRY
}

// Description:	This method is called by OpenDoc when a promise of data ( that 
// we made ) needs to be resolved. The storage unit we get passed should be 
// focused to a proeprty and value containing our original byte array that we
// promised earlier. We need to read that byte array and use it to write the
// actual data that was promisedf into that SAME property and value.
void	LedPart::od_FulfillPromise (ODStorageUnitView* /*promiseSUView*/)
{
	Led_Assert (false);	// we make no promises
}

// Description:	This method is called during exteralization of the
//				part. The part should remove any value in the content property
//				that it cannot "accurately" write to.
void	LedPart::CleanseContentProperty (ODStorageUnit* storageUnit)
{
    SOM_Trace ("LedPart","CleanseContentProperty");

	ODULong numValues;
	ODULong index;

	Environment*	ev	=	SafeGetEnvironment ();

	storageUnit->Focus(ev, kODPropContents, kODPosUndefined, kODNULL, 0, kODPosAll);
	
	numValues = storageUnit->CountValues(ev);
	for (index = numValues; index >= 1; index--) {
		// Index from 1 to n through the values.
		storageUnit->Focus(ev, kODPropContents, kODPosUndefined, 
								kODNULL, index, kODPosUndefined);
								
		// Get the ISO type name for the value. The temp object
		// will automatically delete the returned value when this
		// scope is exited.
		TempODValueType value = storageUnit->GetType(ev);

		// If the value type is not one we support, remove it.
//		if ( ODISOStrCompare(value, kLedPartKind) != 0 )
//			storageUnit->Remove(ev);
#if 1
		bool	remove = ODISOStrCompare (value, kLedPartKind)!=0;

		#if		qSTTXTSupported
		if (ODISOStrEqual(value, gGlobals->fStyledTextKind) )
			remove = kODFalse;
		#endif

		if ( ODISOStrEqual(value, gGlobals->fScrapTextValue) )
			remove = kODFalse;

		if (remove)
			storageUnit->Remove(ev);
#endif
	}
}

// Description:	This method is called during the creation of
//				stationery, to prepare a storage unit, and during
//				externalization, to verify that all the properties we
//				need are present.
//
//				The part adds the default content property, a
//				preferred editor property (to aid in part binding), 
//				and a default name for the part.
void	LedPart::CheckAndAddProperties (ODStorageUnit*	storageUnit )
{
    SOM_Trace ("LedPart","CheckAndAddProperties");

	Environment*	ev	=	SafeGetEnvironment ();

	// Create our content property and preferred content property kind.
	if ( !storageUnit->Exists(ev, kODPropContents, kODNULL, 0) ) {
		storageUnit->AddProperty(ev, kODPropContents);
	}


	// Add values for all the kinds we support, in fidelity order.
	if ( !storageUnit->Exists(ev, kODPropContents, kLedPartKind, 0) )
	{	
		storageUnit->Focus(ev, kODPropContents, kODPosUndefined, kODNULL, 0, kODPosAll);
		storageUnit->AddValue(ev, kLedPartKind);
	}
#if 0
	if ( !storageUnit->Exists(ev, kODPropContents, gGlobals->fStyledTextKind, 0) )
	{
		storageUnit->Focus(ev, kODPropContents, kODPosUndefined, kODNULL, 0, kODPosUndefined);
		storageUnit->AddValue(ev, gGlobals->fStyledTextKind);
	}
#endif
	if ( !storageUnit->Exists(ev, kODPropContents, gGlobals->fScrapTextValue, 0) )
	{
		storageUnit->Focus(ev, kODPropContents, kODPosUndefined, kODNULL, 0, kODPosUndefined);
		storageUnit->AddValue(ev, gGlobals->fScrapTextValue);
	}

	// Since we are setting up the preferred kind property, we just write
	// out our default "kind" for the editor. We can write out the user
	// chosen kind in the ExternalizeStateInfo method.
	if ( !storageUnit->Exists(ev, kODPropPreferredKind, kODISOStr, 0) ) {
		TRY
			ODSetISOStrProp(ev, storageUnit, kODPropPreferredKind, kODISOStr, kLedPartKind);
		CATCH_ALL
			// Remove the property and value if something went wrong.
			ODSURemoveProperty(ev, storageUnit, kODPropPreferredKind);
		ENDTRY
	}
	
	// Add our display frame list.
	
	if ( !storageUnit->Exists(ev, kODPropDisplayFrames, kODNULL, 0) )
		storageUnit->AddProperty(ev, kODPropDisplayFrames);
	if ( !storageUnit->Exists(ev, kODPropDisplayFrames, kODWeakStorageUnitRefs, 0) )
	{
		storageUnit->Focus(ev, kODPropDisplayFrames, kODPosUndefined, kODNULL, 0, kODPosAll);
		storageUnit->AddValue(ev, kODWeakStorageUnitRefs);
	}
}

// Description:	This method is called by the part when the content or
//				state of the part has been modified by the user and
//				the "Save" menu item should be enabled.
void	LedPart::SetDirty ()
{
    SOM_Trace ("LedPart","SetDirty");

	// There is no need to repeatedly tell the draft we have changed;
	// once is sufficient.
	if (!fDirty && !fReadOnlyStorage) {
		fDirty = kODTrue;
		GetDraft ()->SetChangedFromPrev (SafeGetEnvironment ());
	}
}

// Description:	This method is called when the user wants to save the document
//				with multiple representations of the data. This is especially
//				useful for increasing the portability of documents
//				cross-platform.
//
//				A part should verify each kind is valid, that it exists in
//				the content property in the correct order, and write the data.
void	LedPart::od_ExternalizeKinds (ODTypeList* kindset)
{
	if ( !fReadOnlyStorage ) {
		ODBoolean preferredKindWritten = kODFalse;

		Environment*	ev	=	SafeGetEnvironment ();

		// Get our storage unit.
		ODStorageUnit* storageUnit = GetODPart ()->GetStorageUnit(ev);
		
		// Verify that the storage unit has the appropriate properties
		// and values to allow us to run. If not, add them.
		CheckAndAddProperties (storageUnit);
	
		// Verify that there are no "bogus" values in the Content
		// property.
		CleanseContentProperty (storageUnit);
	
		// Iterate over the kindset and write out the content types
		// that we support.
		TempODTypeListIterator tliter(ev, kindset);
		for ( ODType kind = tliter.First(); tliter.IsNotComplete(); kind = tliter.Next() ) {
			// Check to see if this is a kind we support. If so, write it.
			if ( ODISOStrCompare(kind, kLedPartKind) == 0 ) {
				// Write out the part's content.
				ExternalizeContent (storageUnit, kODNULL);
				// This is our preferrend kind so we don't need to write it again.
				// Part editors with more than one kind need to test each kind
				// against the preferred kind to make sure it's been written.
				preferredKindWritten = kODTrue;
			}
		}
	
		// Write out the part's state information.
		ExternalizeStateInfo (storageUnit);

		// Even if the kind set contains no types we support, we must at least
		// write out our current "preferred" kind.
		if ( preferredKindWritten == kODFalse ) {
			// Write out the part's preferred content kind, which, for LedPart,
			// is the only kind.
			ExternalizeContent (storageUnit, kODNULL);
		}
	}
}

// Description:	This method is called when the changes the part's primary kind
//				and/or when the part editor is switched via the Info dialog.
//
//				The editor should record the new "preferred" kind and change the
//				UI, if necessary, to allow editing of that kind. Don't write the
//				properties/values/data until the Externalize is called.
void	LedPart::od_ChangeKind (ODType kind)
{
	// LedPart only has one kind, but we need to make sure the "Preferred Kind"
	// property has the correct value.
	if ( ODISOStrCompare (kind, kLedPartKind) == 0 ) {
		Environment*	ev	=	SafeGetEnvironment ();

		// Get our storage unit.
		ODStorageUnit* storageUnit = GetODPart ()->GetStorageUnit(ev);
		
		TRY
			// Write out the users preferred kind.
			ODSetISOStrProp(ev, storageUnit, kODPropPreferredKind, kODISOStr, kLedPartKind);
			
			// Changing our kind dirties our content.
			SetDirty ();
			
			// Immediately externalize ourselves in the "new" format.
			od_Externalize ();
		CATCH_ALL
			// Remove the property and value if something went wrong.
			ODSURemoveProperty(ev, storageUnit, kODPropPreferredKind);
		ENDTRY
	}
	else {
		// for now maybe this is the best we can do, but we should support other kinds!
		THROW(kODErrInvalidValueType);
	}
}

// Description:	When a frame is being internalized by the Draft, it
//				will ask the owner (part) to read in its info
//				annotation on the frame.
//
//				The part info class knows how to internalize itself
//				from a storage unit view, so we just call
//				FrameEditor::InitFromStorage.
ODInfoType LedPart::od_ReadPartInfo (ODFrame* frame,  ODStorageUnitView* storageUnitView)
{
    SOM_Trace ("LedPart","ReadPartInfo");

	FrameEditor* frameInfo = kODNULL;

	Led_RequireNotNil (fPartContent);
	TRY
		frameInfo = new FrameEditor (this, &GetStyleDatabase (), &GetTextStore (), frame);
		frameInfo->InitFromStorage(storageUnitView);

		// Hook the runtime display frame synchonization mechanism back up.
		if ( frameInfo->HasSourceFrame() ) {
			Environment*	ev	=	SafeGetEnvironment ();
			FrameEditor* sourceFrameInfo =  (FrameEditor*) frameInfo->GetSourceFrame()->GetPartInfo(ev);
			sourceFrameInfo->AttachFrame(frame);
		}
		fFrameEditors.Append (frameInfo);
		
	CATCH_ALL
	ENDTRY
	
	return (ODInfoType)frameInfo;
}

// Description:	When a frame is being externalized by the Draft, it
//				will ask the owner (part) to write out its info
//				annotation on the frame.
//
//				The part info class knows how to externalize itself
//				to a storage unit view, so we just call Externalize.
void	LedPart::od_WritePartInfo (ODInfoType partInfo, ODStorageUnitView*	storageUnitView)
{
    SOM_Trace ("LedPart","WritePartInfo");
	((FrameEditor*) partInfo)->Externalize (storageUnitView);
}

// Description:	This method is called in response to a frame being
//				created for our part.
//
//				The part records the existence of a new display frame
//				in its internal display frame list, as well as,
//				verify that the frame is "set up" correctly (i.e.,
//				valid viewType). The part also creates and stores its
//				"frame info" class in the new frame. 
void	LedPart::od_DisplayFrameAdded (ODFrame* frame)
{
	SOM_Trace ("LedPart","DisplayFrameAdded");
	
	Environment*	ev	=	SafeGetEnvironment ();

	ODBoolean isMainPresentation = frame->GetPresentation(ev)==gGlobals->fMainPresentation;

	// If we are being embedded into another part, the presentation field
	// will be NULL, so we need to set it something meaningful.
	// The view field may also be null, if so, we prefer to be displayed
	// in a frame view.

	// We shouldn't be given a frame with an foreign presentation
	// If we do get one, set it to our main presentation.
	ODTypeToken tPresentation = frame->GetPresentation(ev);
	if ( tPresentation != gGlobals->fMainPresentation) {
		frame->SetPresentation(ev, gGlobals->fMainPresentation);
		isMainPresentation = kODTrue;
	}
	
	
	// Only frame with gGlobals->fMainPresentation are droppable.
	if (isMainPresentation) {
		frame->SetDroppable(ev, kODTrue);
	
		// Create frame proxies for any shapes already added when this
		// frame was added
		COrdListIterator iter(GetShapeList ());
		for ( Embedding* shape = (Embedding*)iter.First();  iter.IsNotComplete(); shape = (Embedding*)iter.Next() ) {
			shape->DisplayFrameAdded(ev, frame);
		}
	}

	
	// If there is no view type then default it to gGlobals->fFrameView
	if ( frame->GetViewType(ev) == kODNullTypeToken ) {
		frame->SetViewType(ev, gGlobals->fFrameView);
	}
			
	// Hang our "state" info off of the new display frame. We use
	// the FrameEditor object for activation, updating, and window
	// maintenance.
	Led_RequireNotNil (fPartContent);
	FrameEditor* frameInfo = new FrameEditor (this, &GetStyleDatabase (), &GetTextStore (), frame);
	frame->SetPartInfo(ev, (ODInfoType)frameInfo);
	fFrameEditors.Append (frameInfo);

	// Create a proxy for this frame and add it to our list	
	CDisplayFrameProxy* tProxy = new CDisplayFrameProxy();
	tProxy->InitializeDisplayFrameProxy(ev, this, kODNULLID, frame);
	fDisplayFrameProxies->AddLast(tProxy);

	// Since we maintain a persistent list of weak references to our
	// persistent display frames, having one added to the part dirties it.
	if (isMainPresentation) {
		SetDirty();
	}


	// shape not exactly changed, but do this to get led window rect, and scroll bar positions etc right
	LookupFrameEditor (frame).FrameShapeChanged ();
}

// Description:	This method is called when one of our display frames,
//				previously written out, is internalized. This method
//				is called instead of DisplayFrameAdded because a "new"
//				frame is not being created; an existing one is being
//				reconstituted.
void	LedPart::od_DisplayFrameConnected (ODFrame* frame)
{
    SOM_Trace ("LedPart","DisplayFrameConnected");

	Environment*	ev	=	SafeGetEnvironment ();

	ODBoolean isMainPresentation = frame->GetPresentation(ev)==gGlobals->fMainPresentation;
	
	// We shouldn't be given a frame with an foreign presentation
	// If we do get one, set it to our main presentation.
	ODTypeToken tPresentation = frame->GetPresentation(ev);
	if ( tPresentation != gGlobals->fMainPresentation)
	{
		frame->SetPresentation(ev, gGlobals->fMainPresentation);
		isMainPresentation = kODTrue;
	}
	
	if ( frame->IsRoot(ev) ) {
		// If the frame being added is a root frame, we know that a window
		// is associated with this frame. Notify ourselves that we need to 
		// clean it up when the frame goes away.
		FrameEditor* frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
		frameInfo->SetShouldDisposeWindow(kODTrue);
		
		// In addition, we need to verify that the frame is in "frame" view.
		// If the user dragged an icon from a document to Finder and then
		// opens the resultant document, the view would be "icon". However
		// displaying a icon is useless, so we need to change the view to
		// frame.
		if ( frame->GetViewType(ev) != gGlobals->fFrameView )
			frame->SetViewType(ev, gGlobals->fFrameView);	
	}
	
	if (isMainPresentation) {
		// Ensure that embedded content is re-attached to saved frames correctly.
		COrdListIterator iter(GetShapeList ());
		for ( Embedding* shape = (Embedding*)iter.First();
			 iter.IsNotComplete();
			 shape = (Embedding*)iter.Next() )
		{
			shape->DisplayFrameConnected(ev, frame);
		}
	}
		
	// Only frame with gGlobals->fMainPresentation are droppable.
	if (frame->GetPresentation(ev)==gGlobals->fMainPresentation) {
		frame->SetDroppable(ev, kODTrue);
	}
		
	// Iterate over our display collection to match the frame with
	// an existing proxy with the correct frame ID. If we find it,
	// replace the ID with the actual frame. If we don't find it,
	// signal an error.
	ODBoolean found = kODFalse;
	COrdListIterator fiter(fDisplayFrameProxies);
	for ( CDisplayFrameProxy* proxy = (CDisplayFrameProxy*) fiter.First();
			fiter.IsNotComplete(); proxy = (CDisplayFrameProxy*) fiter.Next() )
	{
		if ( proxy->GetFrameID() == frame->GetID(ev) )
		{
			// Set the proxy as internalized, if it is not already.
			if (!proxy->IsFrameInMemory())
				proxy->SetFrame(frame);
			
			found = kODTrue;
		}
	}

	// If we didn't find a proxy, then create one.
	if ( !found ) {
		CDisplayFrameProxy* tProxy = new CDisplayFrameProxy();
		tProxy->InitializeDisplayFrameProxy(ev, this, kODNULLID, frame);
		fDisplayFrameProxies->AddLast(tProxy);
	
		// Since we maintain a persistent list of weak references to our
		// persistent display frames, having one added to the part dirties it.
		if (isMainPresentation) {
			SetDirty();
		}
	}
	

	// shape not exactly changed, but do this to get led window rect, and scroll bar positions etc right
	LookupFrameEditor (frame).FrameShapeChanged ();
}

// Description:	This method is called when a frame has been closed or removed.
//				The method cleans up the references and state information stored
//				in the FrameEditor class.
void	LedPart::CleanupDisplayFrame (ODFrame* frame, ODBoolean frameRemoved)
{
	SOM_Trace ("LedPart","CleanupDisplayFrame");

	Environment*	ev	=	SafeGetEnvironment ();

	ODError		error = noErr;
	FrameEditor*	frameInfo = (FrameEditor*) frame->GetPartInfo(ev);

	TRY
		// If we are the root of a child window, we need to notify
		// our source frame that we are going away.
		if ( frameInfo->HasSourceFrame() )
		{
			ODFrame* sourceFrame = frameInfo->GetSourceFrame();
			FrameEditor* sourceFrameInfo = (FrameEditor*) sourceFrame->GetPartInfo(ev);
			
			if ( frameRemoved )
			{
				// Invalidate the source frame. We do this because the
				// source frame may have a unique display when it has
				// been opened into a part window. This forces the
				// frame to redraw "not opened".
				sourceFrame->Invalidate(ev, kODNULL, kODNULL);
			}
			
			// Release our reference to the source frame.
			frameInfo->ReleaseSourceFrame();
			
			// Detach ourself from the source frame.
			sourceFrameInfo->DetachFrame();
	
			// If the frame is the root, it is a part window going away
			// and we need to notify our source frame that it no longer
			// has a part window.
			if ( frame->IsRoot(ev) )
				sourceFrameInfo->SetPartWindow(kODNULL);
		}
	CATCH_ALL
		error = ErrorCode();
	ENDTRY
	
	TRY
		// If the frame was removed from the document, we need to remove
		// any child window displaying that frame.
		if ( frameRemoved )
		{
			// If we have a child window, we need to close it.
			if ( frameInfo->HasPartWindow() )
			{
				ODWindow* window = frameInfo->GetPartWindow();
				frameInfo->SetPartWindow(kODNULL);
				window->CloseAndRemove(ev);
			}
		}
	CATCH_ALL
		error = ErrorCode();
	ENDTRY

	TRY
		// If we have attached frames, we need to notify them that we
		// are going away.
	
		if ( frameInfo->HasAttachedFrame() )
		{
			FrameEditor*	attachedFrameInfo;
			ODFrame*	attachedFrame = frameInfo->GetAttachedFrame();
			
			attachedFrameInfo = (FrameEditor*) attachedFrame->GetPartInfo(ev);
			
			// Tell the attached frame that it's source is going away.
			attachedFrameInfo->ReleaseSourceFrame ();
			
			// Remove the attached frame from our state info.
			frameInfo->DetachFrame ();
		}
	CATCH_ALL
		error = ErrorCode();
	ENDTRY
	
	// If anything went wrong, signal an error.
	THROW_IF_ERROR(error);
}

void	LedPart::od_LinkUpdated (ODLink* /*odLink*/, ODUpdateID /*change*/)
{
	Led_Assert (false);		// NYI
}

void	LedPart::od_RevealLink (ODLinkSource* /*linkSource*/)
{
	Led_Assert (false);		// NYI
}

bool	LedPart::od_EditInLinkAttempted (ODFrame* /*frame*/)
{
	Led_Assert (false);		// NYI
	return false;
}

void	LedPart::od_LinkStatusChanged (ODFrame* /*frame*/)
{
	Led_Assert (false);		// NYI
}

//------------------------------------------------------------------------------
// Method:		DisplayFrameRemoved
// Origin:		ODPart
//
// Description:	This method is called in response to a frame being
//				removed from our part.
//
//				The part removes the frame from its internal display
//				frame list and reliquishes any foci that it still
//				owned. Lastly, if the frame has a source frame
//				(it was the root frame of a part window), we will
//				record the part window bounds so that any subsequent
//				part windows opened on the source frame will appear
//				in the same location.
//------------------------------------------------------------------------------

void LedPart::od_DisplayFrameRemoved (ODFrame* frame)
{
    SOM_Trace ("LedPart","DisplayFrameRemoved");

	Environment*	ev	=	SafeGetEnvironment ();

	ODBoolean isMainPresentation = frame->GetPresentation(ev)==gGlobals->fMainPresentation;
	
	TRY
		FrameEditor* frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
		
		// Make sure the frame going away does not own any foci. Forgetting
		// to do this, will cause a "refcounting" error when the frame
		// is deleted by the draft.
		RelinquishAllFoci (frame);
		
		// Notify any embedded content of frame being removed, if the frame
		// is in the main presentation
		if (frame->GetPresentation(ev)==gGlobals->fMainPresentation)
		{
			COrdListIterator iter(GetShapeList ());
			for ( Embedding* shape = (Embedding*)iter.First();
				 iter.IsNotComplete();
				 shape = (Embedding*)iter.Next() )
			{
				shape->DisplayFrameRemoved(ev, frame);
			}
		}
		
		// Locate the proxy, and remove it
		COrdListIterator fiter(fDisplayFrameProxies);
		for ( CDisplayFrameProxy* frameProxy = (CDisplayFrameProxy*)fiter.First();
				fiter.IsNotComplete();
				frameProxy = (CDisplayFrameProxy*)fiter.Next())
		{
			// shouldn't get called to remove a frame that has
			// not yet been internalized
			if (frameProxy->IsFrameInMemory())
			{
				if ( ODObjectsAreEqual(ev, frame,frameProxy->GetFrame(ev)) )
				{
					fiter.RemoveCurrent();
					delete frameProxy;
				}
			}
		}

		// Clean up the display frame.
		CleanupDisplayFrame (frame, true);
		// Clean up any associated window.
		CleanupWindow (frame);
		// Dispose of the frame's runtime state info.
		frame->SetPartInfo(ev, (ODInfoType) kODNULL);
		fFrameEditors.RemoveAt (IndexOf (fFrameEditors, frameInfo));
		ODDeleteObject(frameInfo);
		

		// Since we maintain a persistent list of weak references to our
		// persistent display frames, having one removed from the part dirties it.
		if (isMainPresentation) {
			SetDirty();
		}
	
	CATCH_ALL
		DoDialogBox (frame, kErrorBoxID, kErrRemoveFrame);
		
		// Alert the caller.
		RERAISE;
	ENDTRY
}

//------------------------------------------------------------------------------
// Method:		DisplayFrameClosed
// Origin:		ODPart
//
// Description:	This method is called in response to a frame being
//				closed (not removed).
//
//				For a part as simple as this, we do not distinguish
//				between a frame being closed or removed. As such, we
//				just call DisplayFrameRemoved. 
//------------------------------------------------------------------------------

void	LedPart::od_DisplayFrameClosed (ODFrame* frame)
{
	SOM_Trace ("LedPart","DisplayFrameClosed");
	
	Environment*	ev	=	SafeGetEnvironment ();

	TRY
		FrameEditor* frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
	
		// Make sure the frame going away does not own any foci. Forgetting
		// to do this, will cause a "refcounting" error when the frame
		// is deleted by the draft.
		RelinquishAllFoci (frame);

		// Notify any embedded content of frame being removed, if the frame
		// is in the main presentation
		if (frame->GetPresentation(ev)==gGlobals->fMainPresentation)
		{
			COrdListIterator iter(GetShapeList ());
			for ( Embedding* shape = (Embedding*)iter.First();
				 iter.IsNotComplete();
				 shape = (Embedding*)iter.Next() )
			{
				shape->DisplayFrameClosed(ev, frame);
			}
		}

		// Locate the proxy, and remove it
		COrdListIterator fiter(fDisplayFrameProxies);
		for ( CDisplayFrameProxy* frameProxy = (CDisplayFrameProxy*)fiter.First();
				fiter.IsNotComplete();
				frameProxy = (CDisplayFrameProxy*)fiter.Next())
		{
			// shouldn't get called to remove a frame that has
			// not yet been internalized
			if (frameProxy->IsFrameInMemory())
			{
				if (frame==frameProxy->GetFrame(ev))
				{
					// Remove current makes the list consistent. This way
					// we can modify the list while we are iterating over
					// it.
					fiter.RemoveCurrent();
					delete frameProxy;
				}
			}
		}

#if		1
// see where moved from above...

		// Clean up the display frame.
		CleanupDisplayFrame (frame, true);
		// Clean up any associated window.
		CleanupWindow (frame);
		// Dispose of the frame's runtime state info.
		fFrameEditors.RemoveAt (IndexOf (fFrameEditors, frameInfo));
		frame->SetPartInfo(ev, (ODInfoType) kODNULL);
		ODDeleteObject (frameInfo);
#endif


	CATCH_ALL
		DoDialogBox(frame, kErrorBoxID, kErrRemoveFrame);
		
		// Alert the caller.
		RERAISE;
	ENDTRY
}

// Description:	If a part which we are contained in is opened into a
//				part window, it is required to iterate over its
//				embedded frames and add new display frames in the part
//				window. After each new embedded frame is created, this
//				method will be called.
//
//				Given all that, and given our lack of interesting
//				content, we just validate the frame. 
void	LedPart::od_AttachSourceFrame (ODFrame* frame, ODFrame* sourceFrame)
{
    SOM_Trace ("LedPart","AttachSourceFrame");

	Environment*	ev	=	SafeGetEnvironment ();

	// Tell the new frame about its source.
	FrameEditor* frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
	frameInfo->SetSourceFrame (sourceFrame);
	
	// And tell the source about its new dependent.
	FrameEditor* sourceFrameInfo = (FrameEditor*) sourceFrame->GetPartInfo(ev);
	sourceFrameInfo->AttachFrame (frame);
	
	// In both cases, refcounting of the frame and sourceFrame is
	// handled by the FrameEditor class.
}

// Description:	This method is Called by an embedded part to indicate it no 
// longer needs the frame to display itself.
void	LedPart::od_RemoveEmbeddedFrame (ODFrame* embeddedFrame )
{
    SOM_Trace ("LedPart","RemoveEmbeddedFrame");

	Embedding* tShape = ShapeForFrame (embeddedFrame);
	THROW_IF_NULL(tShape);
	RemoveShape(tShape);	// Remove from shape list

	// Remove the shape from content, and put the frame in "limbo"
	tShape->SetInLimbo (kODTrue);
	tShape->Removed (true);
}

// Description:	This method is called in response to one of our
//				display frame's viewType field being modified. We call
//				this method on ourselves when new display frames are
//				added, but it call also be called when the user
//				changes the view in the "part info" dialog.
//
//				The part first loads the appropriate view icons if
//				needed and then calculates a new "used" shape based on
//				the bounds of the new view type. If any problems occur
//				while changing the view, the part defaults back to 
//				frame view (the part's default view).
void	LedPart::od_ViewTypeChanged (ODFrame* frame)
{
    SOM_Trace ("LedPart","ViewTypeChanged");

	Environment*	ev	=	SafeGetEnvironment ();

	ODTypeToken	view = frame->GetViewType(ev);

	// Change this frame's used shape to match the new view setting.
	TempODShape newUsedShape = CalcNewUsedShape (frame);	

	frame->Invalidate(ev, kODNULL, kODNULL);
	frame->ChangeUsedShape(ev, newUsedShape, kODNULL);
	frame->Invalidate(ev, kODNULL, kODNULL);
}

void	LedPart::od_PresentationChanged (ODFrame* /*frame*/)
{
}

void	LedPart::od_SequenceChanged (ODFrame* /*frame*/)
{
}

// Description:	Containing parts which have wrapped content to the used shape of 
// an embedded frame will need to adjust the layout of that content for the new used shape.
void	LedPart::od_UsedShapeChanged (ODFrame* /*embeddedFrame*/)
{
// REALLY I SHOULD RELAYOUT THE TEXT BASED ON THIS CHANGE - LGP 951213


	// Currently we do not support wrapping content to the used shape.
	SOM_Trace ("LedPart","UsedShapeChanged");
//LookupFrameEditor (frame).FrameShapeChanged ();
}

// Description:	This method is called in response to a frame's shape being
//				altered, either by the user or the part we are embedded in.
//
//				To keep all dependent frames in sync, we need to propogate the
//				new frame shape the frames dependent on the changed frame. This
//				is done by observing the display frames stored in the frame info
//				and calling RequestFrameShape for each.
void	LedPart::od_FrameShapeChanged (ODFrame* frame)
{
	SOM_Trace ("LedPart","FrameShapeChanged");

	Environment*	ev	=	SafeGetEnvironment ();

#if 1
// from new DrawEditor part - LGP 960129
	// Adjust the "used" shape for the new frame shape.
	TempODShape usedShape = this->CalcNewUsedShape(frame);
	frame->ChangeUsedShape(ev, usedShape, kODNULL);
#else
	// If the frame is the root frame of a part or document window, it should
	// have no effect on its attached frames or source frame.

	if ( !frame->IsRoot(ev) )
	{
		TempODShape	frameShape = frame->AcquireFrameShape(ev, kODNULL);
		FrameEditor&	frameEditor	=	LookupFrameEditor (frame);
		ODFrame*	displayFrame;
		
		if ( frameEditor.HasSourceFrame() )
		{
			displayFrame = frameEditor.GetSourceFrame();
			
			TempODShape frameShapeCopy = frameShape->Copy(ev);
			TempODShape returnShape = displayFrame->RequestFrameShape(ev, frameShapeCopy, kODNULL);
			
			displayFrame->Invalidate(ev, kODNULL, kODNULL);
		}
	
		if ( frameEditor.HasAttachedFrame() )
		{
			displayFrame = frameEditor.GetAttachedFrame();
			
			TempODShape frameShapeCopy = frameShape->Copy(ev);
			TempODShape returnShape = displayFrame->RequestFrameShape(ev, frameShapeCopy, kODNULL);
				
			displayFrame->Invalidate(ev, kODNULL, kODNULL);
		}
	}
#endif

// shape not exactly changed, but do this to get led window rect, and scroll bar positions etc right
LookupFrameEditor (frame).FrameShapeChanged ();
}

// Description:	This method is called by the part when a frame, that has been
//				previously opened, is being opened again.
//
//				The method retrieves the existing window for the frame and
//				returns it.				
ODWindow*	LedPart::AcquireFramesWindow (ODFrame* frame)
{
    SOM_Trace ("SamplePart","GetFramesWindow");
	Environment*	ev	=	SafeGetEnvironment ();
	FrameEditor* frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
	ODWindow* window = frameInfo->GetPartWindow();
	ODAcquireObject(ev, window);

	return window;
}
	
// Description:	This method is called when OpenDoc, a containing part,
//				or the active editor would like to open a frame into
//				a seperate window. If a source frame is passed into
//				this method, the editor is being asked to open a part
//				window.
//
//				The part first checks to see if the display frame has
//				been opened into a window before (this will be true
//				if a source frame is passed in to the method and if
//				the source frame has a valid window ID). If there is
//				already a window displaying the frame, just bring that
//				window to the front. If not, we create a window from
//				scratch and display it.
ODID	LedPart::od_Open (ODFrame* frame)
{
    SOM_Trace ("LedPart","Open");

	ODID windowID;
	ODWindow* window = kODNULL;
	WindowProperties* windowProperties = kODNULL;
	
	Environment*	ev	=	SafeGetEnvironment ();

	TRY
		// Because the frame parameter being passed to us can be one of
		// three things, we must determine what it is; either the root
		// frame of a existing document, the source frame for a part
		// window, or null if we are opening a new document.
		if ( frame == kODNULL ) {
			// Calculate the bounding rectangle for a new window
			Rect windowRect = CalcPartWindowSize (kODNULL);
			// Get the default setting for a document window.
			windowProperties = GetDefaultWindowProperties (kODNULL, &windowRect);
			// Create a Mac Window and register it with OpenDoc.
			window = CreateWindow (kODNULL, kODFrameObject, windowProperties);
		}
		else if ( frame->IsRoot(ev) ) {
			// Get the previously saved settings for the document window.
			windowProperties = GetSavedWindowProperties (frame);
			
			if ( windowProperties == kODNULL )
			{
				// Calculate the bounding rectangle for a new window
				Rect windowRect = CalcPartWindowSize (frame);
				// Get the default setting for a document window.
				windowProperties = GetDefaultWindowProperties (kODNULL, &windowRect);
			}

			// Create a Mac Window and register it with OpenDoc.
			window = CreateWindow (frame, kODFrameObject, windowProperties);
			
			// We release the source frame here because we didn't call
			// EndGetWindowProperties and becuase we are done with it.
			// ODReleaseObject(ev, windowProperties->sourceFrame);
		}
		else // frame is a source frame
		{
			window = AcquireFramesWindow (frame);
	
			if ( window == kODNULL )
			{
				// Calculate the bounding rectangle for a new window
				Rect windowRect = CalcPartWindowSize ( frame);
				// Get the default setting for a document window.
				windowProperties = GetDefaultWindowProperties (frame, &windowRect);
				// Create a Mac Window and register it with OpenDoc.
				window = CreateWindow (kODNULL, kODFrameObject, windowProperties);
				
				// Tell the source frame that it is opened in a part window.
				FrameEditor* frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
				frameInfo->SetPartWindow(window);
			}
		}
	
		// Create the window's root facet.
		window->Open(ev);
		// Make the window visible.
		window->Show(ev);
		// Activate and select the window.
		window->Select(ev);
	
		// Cleanup allocate memory.
		ODDeleteObject(windowProperties);
		
		// Get window id to return.
		windowID = (window ? window->GetID(ev) : kODNULLID);
		ODReleaseObject(ev, window);
	
	CATCH_ALL
		// If we threw early, the source frame's refcount may be too high.
		if ( windowProperties )
			ODReleaseObject(ev, windowProperties->sourceFrame);
		// Cleanup the created items.
		ODDeleteObject(windowProperties);
		ODSafeReleaseObject(window);
		windowID = kODNULLID;
		// Alert the caller.
		RERAISE;
	ENDTRY

	return windowID;
}

// Description:	This method is called by the part when a window needs to be
//				created for a frame being opened.
//
//				The part uses the information passed in windowProperties to 
//				create the appropriate window. The generated window is
//				registered with OpenDoc as a new window (RegisterWindow) or as a
//				window from an existing document (RegisterWindowForFrame).
ODWindow*	LedPart::CreateWindow (ODFrame* frame, ODType frameType, WindowProperties* windowProperties)
{
    SOM_Trace ("LedPart","CreateWindow");

	ODPlatformWindow	platformWindow	= kODNULL;
	ODWindow*			window			= kODNULL;
	
	// Using the name and the calculated rectangle, create a new window.
	// Note that we are allocating the window record in temp mem using
	// the OpenDoc memory mgr. This helps reduce app heap usage.
	// In addition, OpenDoc requires that all new windows be initially hidden
	// so that it can correctly layer windows/palettes.
	platformWindow = ::NewCWindow((Ptr)ODNewPtr(sizeof(WindowRecord)),
								&(windowProperties->boundsRect),
								windowProperties->title,
								kODFalse, /* visible */
								windowProperties->procID,
								(WindowPtr)-1L,
								windowProperties->hasCloseBox,
								windowProperties->refCon);

	if ( platformWindow ) {
		Environment*	ev	=	SafeGetEnvironment ();
		TRY
			ODWindowState* windowState = ODGetSession(ev,GetODPart ())->GetWindowState(ev);
			
			// Shoud the window be saved in the document? Yes if the root frame is
			// persistent.
			ODBoolean saveWindow = (ODISOStrCompare(frameType,kODFrameObject) == 0);
			
			// Tell the window object that we will be disposing the window record
			// when the root frame is closed/removed.
			ODBoolean shouldDispose = kODFalse;
								
			// Determine whether we are creating a new window (frame is null),
			// or opening a previous saved window (frame is valid).
			if ( frame == kODNULL ) {								
				// Tell OpenDoc about it by creating an OpenDoc window object.
				window = windowState->
							RegisterWindow(ev, 
								platformWindow,					// Macintosh WindowPtr
								frameType,						// Frame type (Persistent/Non-persistent)
								windowProperties->isRootWindow,	// Is this a document window?
								windowProperties->isResizable,	// Is this window resizeable?
								windowProperties->isFloating,	// Is this window floating?
								saveWindow,						// Should this window be persistent?
								shouldDispose,					// (see comment above)
								GetODPart (),							// Part reference to us
								gGlobals->fFrameView,			// What view should the window have?
								gGlobals->fMainPresentation,	// What presentation should the window have?
								windowProperties->sourceFrame);	// The display frame being opened, if any
			}
			else {
				// Tell OpenDoc about it by creating an OpenDoc window object.
				window = windowState->
							RegisterWindowForFrame(ev, 
								platformWindow,					// Macintosh WindowPtr
								frame,	 						// Frame type (Persistent/Non-persistent)
								windowProperties->isRootWindow,	// Is this a document window?
								windowProperties->isResizable,	// Is this window resizeable?
								windowProperties->isFloating,	// Is this window floating?
								saveWindow,						// Should this window be persistent?
								shouldDispose,					// (see comment above)
								windowProperties->sourceFrame);	// The display frame being opened, if any
			}
			
		CATCH_ALL
			// Cleanup Macintosh Window.
			CloseWindow(platformWindow);
			ODDisposePtr(platformWindow);
			// Get the right error message for the problem.
			ODSShort errMsgNum = (!frame && windowProperties->sourceFrame)
									? kErrCantOpenPartWindow : kErrCantOpenDocWindow;
			// Alert the user of the problem.
			DoDialogBox (frame, kErrorBoxID, errMsgNum);
			// Change the exception value, so the DocShell doesn't display an
			// error dialog.
			SetErrorCode(kODErrAlreadyNotified);
			// Alert the caller.
			RERAISE;
		ENDTRY
	}

	return window;
}

// Description:	This method is called by the part when a window needs to be
//				cleaned up for a frame being closed/removed.
//
//				The part deallocates the window buffer allocated in the
//				CreateWindow() method.
void	LedPart::CleanupWindow (ODFrame* frame)
{
    SOM_Trace ("LedPart","CleanupWindow");
    
   	TRY
		Environment*	ev	=	SafeGetEnvironment ();
		FrameEditor*	frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
	    if ( frameInfo->ShouldDisposeWindow() )
	    {
	    	TempODWindow window = frame->AcquireWindow(ev);
	    	THROW_IF_NULL(window);
	    	
	    	ODPlatformWindow windowPtr = window->GetPlatformWindow(ev);
	    	CloseWindow(windowPtr);
	    	ODDisposePtr(windowPtr);
	    }
	CATCH_ALL
		DoDialogBox (frame, kErrorBoxID, kErrWindowGone);
		// consume excpetion because it's not fatal.
	ENDTRY
}

// Description:	This method is called by the part when a new window is being
//				created. The method examines the frame which is being opened
//				a generates the default window parameters to pass to the
//				Mac Toolbox.
WindowProperties*	LedPart::GetDefaultWindowProperties (ODFrame* sourceFrame, Rect* windowRect )
{
	SOM_Trace ("LedPart","GetDefaultWindowProperties");
    
	WindowProperties*	windowProperties = new WindowProperties;

	// Calculate the offset for the window based on the sourceFrame.	
	if ( sourceFrame ) {
		CalcPartWindowPosition (sourceFrame, windowRect);
	}
	else {
		OffsetRect(windowRect, kALittleNudge, GetMBarHeight() + kMacWindowTitleBarHeight);
	}

	windowProperties->boundsRect = *windowRect;

	Environment*	ev			=	SafeGetEnvironment ();
	ODIText*		windowName	=	NULL;

	// Get the part name to use for the new window. If we can't
	// get a valid name for the part, we generate one using the user
	// visible category name of the part's category.
	windowName = ODGetITextProp(ev, GetODPart ()->GetStorageUnit(ev), kODPropName, 
									kODMacIText, kODNULL);
									
	if ( (windowName == kODNULL) || (GetITextStringLength(windowName) == 0) )
	{
		ODNameSpaceManager* nsMgr = ODGetSession(ev,GetODPart ())->GetNameSpaceManager(ev);
					
		// Get the category string from the category name space.
		if ( !GetUserCatFromCat(nsMgr, kODCategoryDrawing, &windowName) )
#if ODDebug
			// This should never happen. Check NMAP for errors.
			Led_Assert (false);	//	Category NMAP bad, or Preferences corrupted.
#else
			THROW(kODErrInvalidNSType);
#endif
	}

	// Convert the ODIText into a Pascal string.
	GetITextString(windowName, windowProperties->title);
	// Dispose of the name object.
	DisposeIText(windowName);
	
	// Fill in the other fields of the Window Properties struct.
	
	windowProperties->procID = zoomDocProc;
	windowProperties->hasCloseBox = kODTrue;
	windowProperties->refCon = (long) kODNULL;
	windowProperties->wasVisible = kODFalse;
	windowProperties->isResizable = kODTrue;
	windowProperties->isFloating = kODFalse;
	windowProperties->isRootWindow = sourceFrame ? kODFalse : kODTrue;
	windowProperties->shouldShowLinks = kODFalse;
	windowProperties->sourceFrame = sourceFrame;
	
	return windowProperties;
}

// Description:	This method is called by the part to read in saved information
//				for a window from an existing document.
WindowProperties*	LedPart::GetSavedWindowProperties (ODFrame* frame)
{
    SOM_Trace ("LedPart","GetSavedWindowProperties");

	WindowProperties*	windowProperties = new WindowProperties;
	
	Environment*	ev			=	SafeGetEnvironment ();

	// If we fail to load the window properties from storage, delete
	// the structure so the calling code will behave appropriately.	
	if ( !BeginGetWindowProperties(ev, frame, windowProperties) )
	{
		ODDeleteObject(windowProperties);
		EndGetWindowProperties(ev, windowProperties);
		return kODNULL;
	}
	else
		EndGetWindowProperties(ev, windowProperties);
	
	// Note: We don't call EndGetWindowProperties because it releases the
	// source frame, which we will need after this method returns.
	
	// Verify the window is still visible on a monitor.
	
	RgnHandle windowRgn = ODNewRgn();
	ODBoolean repositionWindow = kODFalse;
	
	// We are only concerned with the window's title bar being
	// visible, so calcuate the titlebar rect from the current
	// window bounds.
	Rect adjustedBounds = windowProperties->boundsRect;
	adjustedBounds.bottom = adjustedBounds.top;
	adjustedBounds.top -= kMacWindowTitleBarHeight;
	
	// Intersect the monitor's region
	RectRgn(windowRgn, &adjustedBounds);
	SectRgn(windowRgn, GetGrayRgn(), windowRgn);
	
	if ( !EmptyRgn(windowRgn) )
	{
		const	 kMinVertVisPortion		=		10;
		const	 kMinHorzVisPortion		=		16;
		// If the visible portion of the window is too small, we need
		// to reposition it.
		Rect intersectedBounds = (**windowRgn).rgnBBox;
		if ( (intersectedBounds.right-intersectedBounds.left < kMinHorzVisPortion) ||
				(intersectedBounds.bottom-intersectedBounds.top < kMinVertVisPortion) )
			repositionWindow = kODTrue;
	}
	else
	{
		// If the window is completely offscreen, we need to reposition it.
		repositionWindow = kODTrue;
	}
	ODDisposeHandle((ODHandle)windowRgn);
	
	// If not, we need to move it so the user can see it.
	if ( repositionWindow )
	{
		Rect windowRect = (windowProperties->boundsRect);
		// Move the window to {0,0} coordinates.
		OffsetRect(&windowRect, -windowRect.left, -windowRect.top);
		// Now move the window to the default window position.
		OffsetRect(&windowRect, kALittleNudge, GetMBarHeight() + kMacWindowTitleBarHeight);
		// Save the new window position in our windowProperties.
		windowProperties->boundsRect = windowRect;
	}
	
	return windowProperties;
}

// Description:	This method is called by the part to determine what
//				size a new window should be.
Rect	LedPart::CalcPartWindowSize (ODFrame* sourceFrame)
{
    SOM_Trace ("LedPart","CalcPartWindowSize");

	const ODSShort kOnePageWidth = 600;
	
	Rect	windowRect;
	ODRect	frameRect;
	
	// If a source frame is given, the part is being asked to open one of
	// its display frames into a part window. Otherwise, we are being opened
	// as the root frame of the current document and should size the window
	// accordingly.

	// Set up the child window's size to be that of the display frame being opened.
	if ( sourceFrame ) {
		Environment*	ev			=	SafeGetEnvironment ();
	
		// Retrieve the fixed point bounding box for the frame.
		ODShape* frameShape = sourceFrame->AcquireFrameShape (ev, kODNULL);
		frameShape->GetBoundingBox(ev, &frameRect);
		
		// Convert that into a Quickdraw rectangle.
		FixedToIntRect(frameRect, windowRect);
		ODReleaseObject (ev,frameShape);
	}
	// Otherwise, just open a large window.
	else
	{
		// (3 * kODLargeIconSize) prevents the window from covering the volume
		// icons on the desktop which is a violation of Macintosh HI Guidelines.
		
		SetRect(&windowRect, 0, 0,
				  ODQDGlobals.screenBits.bounds.right - (3 * kODLargeIconSize),
				  ODQDGlobals.screenBits.bounds.bottom
				  	- GetMBarHeight() - kMacWindowTitleBarHeight - kALittleNudge);		
		
		if ( windowRect.right - windowRect.left > kOnePageWidth )
			windowRect.right = windowRect.left + kOnePageWidth;
	}
	
	return windowRect;
}

// Description:	This method is called by the part to determine where
//				to align the new window (top left corner of the screen
//				or tiled to a frame).
Rect	LedPart::CalcPartWindowPosition (ODFrame* frame,  Rect* partWindowBounds)
{
    SOM_Trace ("LedPart","CalcPartWindowPosition");

	ODFacet*		activeFacet;
	ODShape*		frameShape;
	ODRect			bbox;
	Rect			bounds;
	
	// We need to know which facet of the frame we are opening to position
	// the child window.
	activeFacet = GetActiveFacetForFrame (frame);
	
	// This should never occur, but if it did, it would be fatal.
	// So we will just pass back the same rectangle.
	if ( activeFacet == kODNULL )
		return *partWindowBounds;
	
	Environment*	ev			=	SafeGetEnvironment ();

	// For the purposes of tiling, we need the to know the area of the
	// document the frame occupies. We do this by getting the bouding
	// box and offsetting it by the aggregate external window transform
	// of the facet.
	
	frameShape = activeFacet->GetFrame(ev)->AcquireFrameShape(ev, kODNULL);
	TempODTransform windowFrameTransform = activeFacet->AcquireWindowFrameTransform(ev, kODNULL);
	TempODShape boundsShape = ODCopyAndRelease(ev, frameShape);

	// Translate the bounds rect into window coordinates.
	boundsShape->Transform(ev, windowFrameTransform);
	
	// Get and convert the bounding box into a QuickDraw rectangle.
	boundsShape->GetBoundingBox(ev, &bbox);
	FixedToIntRect(bbox, bounds);
		
	// We then call our method to tile the child window.
	*partWindowBounds = TilePartWindow(&bounds, partWindowBounds);
	
	// Set the port and origin so we can convert the rect to
	// global Window Mgr coordinates.
	SetPort(activeFacet->GetCanvas(ev)->GetQDPort(ev));
	SetOrigin(0,-kMacWindowTitleBarHeight);
	
	// Convert the local coordinates to global Window Mgr coordinates.
	LocalToGlobal((Point*)(&(partWindowBounds->top)));
	LocalToGlobal((Point*)(&(partWindowBounds->bottom)));
	
	return *partWindowBounds;
}

// Description:	This method is called by the part when it needs to 
//				know what the active facet is for the part, if any.
//
//				The part uses this method specifically to find the
//				facet of a source frame when opening a part window.
ODFacet*	LedPart::GetActiveFacetForFrame (ODFrame* frame)
{
    SOM_Trace ("LedPart","GetActiveFacetForFrame");

	Environment*	ev			=	SafeGetEnvironment ();

	ODFacet*		facet = kODNULL;
	FrameEditor*		frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
	
	// If the frame is active, and it should be, get the active facet
	// from the frame state info.
	if ( frameInfo->IsFrameActive() )
	{
		facet = frameInfo->GetActiveFacet();
	}
	if (facet != NULL) {
		Led_Assert (::ODIsBlockAnObject(facet));
		return facet;
	}

	// Otherwise, iterate over the display frames looking for one
	// that has an active facet... there should be at least one.
	{
#if 1
		for (TempODFrameFacetIterator iter (ev, frame); iter; ++iter ) {
			facet = iter.Current ();
			if (facet->IsSelected(ev) ) {
				return facet;
			}
		}
#else
		ODFacet*	selectedFacet;
		ODFrameFacetIterator* fiter = frame->CreateFacetIterator(ev);
		for ( selectedFacet = fiter->First(ev);
			 				  fiter->IsNotComplete(ev);
			  selectedFacet = fiter->Next(ev) )
		{
facet = selectedFacet;	// even if none selected, at least return SOMETHING!
			if ( selectedFacet->IsSelected(ev) )
			{
				break;
			}
		}
		
//		facet = selectedFacet;
			
		ODDeleteObject(fiter);
#endif
	}

	return facet;
}

// Description:	This method is called in response to one of display frame's view
//				being changed. The method calculates the appropriate usedShape
//				for the new view type.
ODShape* LedPart::CalcNewUsedShape (ODFrame* frame)
{
    SOM_Trace ("LedPart","CalcNewUsedShape");

	ODShape* usedShape = kODNULL;
	RgnHandle usedRgn;
	
	Environment*	ev			=	SafeGetEnvironment ();

	// If the view is "frame", we intentionally return a nil shape;
	// doing so, will reset the used shape to equal the frame shape.

	ODTypeToken view = frame->GetViewType(ev);

	if ( view == gGlobals->fLargeIconView ||
			view == gGlobals->fSmallIconView ||
			view == gGlobals->fThumbnailView )
	{
		TRY
			Rect bounds;
			usedRgn = ODNewRgn();

			if ( view == gGlobals->fLargeIconView || view == gGlobals->fSmallIconView )
			{
				CUsingLibraryResources res;

				// Set the bounds rect for the icon size.
				SetRect(&bounds, 0, 0,
						(view == gGlobals->fLargeIconView) ? kODLargeIconSize : kODSmallIconSize,
						(view == gGlobals->fLargeIconView) ? kODLargeIconSize : kODSmallIconSize);

				// Convert the icon mask into a Region.
				THROW_IF_ERROR( IconIDToRgn(usedRgn, &bounds, atAbsoluteCenter, kBaseResourceID) );
			}
			else if ( view == gGlobals->fThumbnailView )
			{	
				//bounds = (**(PicHandle)gGlobals->fThumbnail).picFrame;
				bounds.top = 0;
				bounds.left = 0;
				bounds.bottom = kODThumbnailSize;
				bounds.right = kODThumbnailSize;
				RectRgn(usedRgn,&bounds);
			}
		
			usedShape = frame->CreateShape(ev);
			usedShape->SetQDRegion(ev, usedRgn);
							
		CATCH_ALL
			ODSafeReleaseObject(usedShape);
			ODDisposeHandle((ODHandle)usedRgn);
			usedShape = kODNULL;
		ENDTRY
	}
		
	return usedShape;
}

void	LedPart::UpdateFrame (ODFrame* frame, ODTypeToken	view, ODShape* usedShape)
{
    SOM_Trace ("LedPart","UpdateFrame");

	Environment*	ev			=	SafeGetEnvironment ();

	// Update the frame to have the new view and UsedShape.
	frame->Invalidate(ev, kODNULL, kODNULL);
	frame->SetViewType(ev, view);
	frame->ChangeUsedShape(ev, usedShape, kODNULL);
	frame->Invalidate(ev, kODNULL, kODNULL);
}

// Description:	This method is called when any part adds a facet to
//				one of our display frames.
//
//				The part calls ViewTypeChanged to load the appropriate
//				resource for display in this facet and then activates
//				the frame if we are the root part of an active window.
void LedPart::od_FacetAdded (ODFacet* facet)
{
    SOM_Trace ("LedPart","FacetAdded");

	Environment*	ev			=	SafeGetEnvironment ();

	ODFrame* frame = facet->GetFrame(ev);
	FrameEditor* frameInfo = (FrameEditor*) frame->GetPartInfo(ev);

	ODBoolean isMainPresentation = facet->GetFrame(ev)->
			GetPresentation(ev)==gGlobals->fMainPresentation;
	
	// Notify our embedded content of the new facet, Only  if it is not a palette
	if (isMainPresentation)
	{
		COrdListIterator iter(GetShapeList ());
		for ( Embedding* shape = (Embedding*)iter.First();
			 iter.IsNotComplete();
			 shape = (Embedding*)iter.Next() )
		{
			shape->FacetAdded(ev, facet);
		}
		LookupFrameEditor (facet->GetFrame (ev)).FacetAdded (facet);

		// We call this to prepare our part for being displayed in one of
		// the four standard views: Large Icon, Small Icon, Frame, and
		// Thumbnail.
		od_ViewTypeChanged (frame);
			
		// If this is the root frame/facet of a given window and it is NOT in a palette
		// we should activate ourselves. 
		if (frame->IsRoot(ev))
		{
			if ( ActivateFrame (frame) )
				frameInfo->SetActiveFacet(facet);
		}
	
		// If a frame had all of its facets removed, the frame would have
		// hidden any of its part windows. If the frame becomes visible again,
		// by having a facet added to it, we will "show" the part window for
		// the frame. Except, in the case where are a frame could be scrolled out of view,
		// and purged. When scrolling is implemented this will need to change.
		
		if ( (CountFramesFacets(frame) == 1) )
		{
			if ( frameInfo->HasPartWindow() )
				frameInfo->GetPartWindow()->Show(ev);
		}
		
		// Only clip the facet if it is not a palette
		ClipEmbeddedFacets (facet);
		
	}
	
}

// Description:	This method is called when any part removes a facet
//				from one of our display frames.
//
//				The part just removes the "active" note from the
//				appropriate display frame if necessary since this
//				facet will not be available, nor active, again.
void	LedPart::od_FacetRemoved (ODFacet* facet)
{
    SOM_Trace ("LedPart","FacetRemoved");

	Environment*	ev			=	SafeGetEnvironment ();

	ODFrame*	frame = facet->GetFrame(ev);
	FrameEditor* frameInfo = (FrameEditor*) frame->GetPartInfo(ev);

	ODBoolean isMainPresentation = frame->GetPresentation(ev)==gGlobals->fMainPresentation;
	
	// If the facet was the active facet, it can no longer be.
	if ( frameInfo->GetActiveFacet() == facet )
		frameInfo->SetActiveFacet(kODNULL);

	// If a frame has all of its facets removed, the frame becomes
	// "hidden". If the frame a part window, the part window should also
	// be hidden. This may need to change when scrolling is implemented.
	// The case where a frame is scrolled out of view and purged temprorarily, its
	// part window may remain visible.
	
	
	if (isMainPresentation) {
		if ( (CountFramesFacets (frame) == 0) ) {
			if ( frameInfo->HasPartWindow() )
				frameInfo->GetPartWindow()->Hide(ev);
		}
		LookupFrameEditor (facet->GetFrame (ev)).FacetRemoved (facet);

		// Remove any embedded facets of the given facet
		COrdListIterator iter(GetShapeList ());
		for ( Embedding* shape = (Embedding*)iter.First();
			 iter.IsNotComplete();
			 shape = (Embedding*)iter.Next() )
		{
			shape->FacetRemoved(ev, facet);
		}
	}

}

void	LedPart::od_CanvasChanged (ODFacet* /*facet*/)
{
}

void	LedPart::od_CanvasUpdated (ODCanvas* /*canvas*/)
{
}

// Description:	An embedded part has asked to change the shape of its frame.
ODShape*	LedPart::od_RequestFrameShape (ODFrame* embeddedFrame, ODShape* frameShape)
{
	// Grant any shape, for the time being
	
	Environment*	ev			=	SafeGetEnvironment ();

	ODShape* contentShape = kODNULL;
	TRY
		// First, find the shape representing the given frame and resize it
		Embedding* tShape = ShapeForFrame (embeddedFrame);
		THROW_IF_NULL(tShape);
#if 1
		// First make a copy of the incoming shape
		contentShape = frameShape->Copy(ev);
		
		// Transform the shape before setting the content bounds
		Point offset;
		Rect tRect;
		
		tShape->GetBoundingBox(&tRect);
		offset.h = tRect.left;
		offset.v = tRect.top;
		
		TempODTransform tTransform = embeddedFrame->CreateTransform(ev);
		tTransform->SetQDOffset(ev, &offset);
		contentShape->Transform(ev, tTransform);
		
		// Change the embedded content's bounding rectangle, without
		// causing the actual ODFrame to be resized.
		tShape->SetShapeRectangle(contentShape);

#if 0
		// Redraw the shape with its new bounds
		//this->InvalidateShape(ev, tShape);
		// invalidate the shapes area in all of our content frames
		{
			COrderedList* contentFrames = this->GetContentDisplayFrames();
			// If we have any display frames..
			if (contentFrames != NULL and contentFrames->Count()>0) {
				// Then iterate over our them
				COrdListIterator iter(contentFrames);
				
				// Create a tempory shape to use to invalidate the CShape
				ODShape* tttShape = ((ODFrame*)iter.First())->CreateShape(ev);
				tShape->GetUpdateShape(ev, tttShape);
				
				// invalidate the shapes area in all of our content frames
				for ( ODFrame* frame = (ODFrame*)iter.First();
					 iter.IsNotComplete();
					 frame = (ODFrame*)iter.Next() )
				{
					frame->Invalidate(ev, tttShape, kODNULL);
				}
			
				// Release acquired geometry
				ODReleaseObject(ev,tttShape);
			}
			// Delete temp list
			delete contentFrames;
		}
#endif
#else
		tShape->SetBoundingBox (ev, frameShape);
#endif
	CATCH_ALL
		Led_Assert (false);	//	This frame is NOT embedded in this part!		
		return kODNULL;
	ENDTRY
	
	// The leaf part will release this shape
	ODAcquireObject(ev, frameShape);

	return frameShape;
}

// Description:	An embedded part has asked for an additional frame.
ODFrame*	LedPart::od_RequestEmbeddedFrame (ODFrame* containingFrame, 
											ODFrame* baseFrame,
											ODShape* frameShape,
											ODPart* embedPart,
											ODTypeToken viewType,
											ODTypeToken presentation,
											ODBoolean /*isOverlaid*/)
{
	Environment*	ev			=	SafeGetEnvironment ();

	// Create a new content shape for the new frame
	// Grant any shape, for the time being
	ODRgnHandle tODRegion = frameShape->GetQDRegion(ev);
	Rect shapeBounds = (*tODRegion)->rgnBBox;

	Embedding*	newShape = new Embedding(this);
	newShape->SetBoundingBox(ev, shapeBounds);
	
	// Make a proxy for it
	CEmbeddedFrameProxy* frameProxy = new CEmbeddedFrameProxy();
	frameProxy->InitializeEmbeddedFrameProxy(ev, 
								this, 
								newShape, 
								containingFrame, 
								embedPart,
								viewType,
								presentation);							
				
	
	// Add the proxy to the shape
	newShape->AddFrameProxy(frameProxy);
	
	// Add the shape to the document
	AddShape(newShape);
	
	ODFrame* resultFrame = frameProxy->GetFrame(ev);
	THROW_IF_NULL(resultFrame);
	
	// Assign Frame group and sequence IDs
	
	// First check to see if the base frame is grouped
	ODULong tGroup = baseFrame->GetFrameGroup(ev);
	
	// If the group is zero, then it is not a part of a group
	// Create a unique group for the base frame AND the new frame
	if (tGroup==0L) {
		// Get new group ID
		tGroup = GetNewGroupID();
		
		// Set BOTH frames to this new group
		baseFrame->SetFrameGroup(ev, tGroup);
		resultFrame->SetFrameGroup(ev, tGroup);
		
		// Since we have created a new group, then it's obvious that the 
		// base frame is sequence #1 and resultFrame is 2.
		baseFrame->ChangeSequenceNumber(ev, 1L);
		resultFrame->ChangeSequenceNumber(ev, 2L);
	}
	else
	// Base frame is already grouped
	{
		// Make the newly created frame be in the base frames group
		resultFrame->SetFrameGroup(ev, tGroup);
		
		// Set the sequence number of the frame to the next in the series
		ODULong sequenceCount = frameProxy->FindNextSequence(ev, tGroup);
		resultFrame->ChangeSequenceNumber(ev, sequenceCount+1);
	}
	
	return resultFrame;
}

void LedPart::InvalidateActiveBorder (ODFrame* frame)
{
	if (fActiveBorderShape) {
		frame->Invalidate (SafeGetEnvironment (), fActiveBorderShape, kODNULL);
	}
}

ODShape*	LedPart::od_AdjustBorderShape (ODFacet* embeddedFacet, ODShape* borderShape)
{
	Environment*	ev			=	SafeGetEnvironment ();

	ODFacet* containingFacet = embeddedFacet->GetContainingFacet(ev);
	ODCanvas* tBiasCanvas = containingFacet->GetCanvas(ev);
	
	// The active border is moving to a new frame
	// Get rid of our saved copy and return kODNULL to OpenDoc per recipe.
	if (borderShape == kODNULL) {
		// OpenDoc appears to be calling us twice in a row with a null bordershape
		ODReleaseObject(ev,fActiveBorderShape);
		fActiveBorderShape = kODNULL;
		ClipEmbeddedFacets (containingFacet);
		return kODNULL;
	}
	
	ODTransform* tTransform = kODNULL;
	ODShape* tShape = kODNULL;
	ODShape* adjusted = borderShape->Copy(ev);
	
	// Put our copy of the given shape into frame coordinates
	tTransform = embeddedFacet->AcquireExternalTransform(ev, tBiasCanvas);
	adjusted->Transform(ev, tTransform);
	ODReleaseObject(ev,tTransform);
	
	// Find the intersection of the adjusted shape and the display facet
	ODShape* displayShape = containingFacet->GetFrame(ev)->AcquireUsedShape(ev, tBiasCanvas);
	adjusted->Intersect(ev, displayShape);
	ODReleaseObject(ev,displayShape);
	
	ODFacet* facet = kODNULL;
	ODBoolean above = kODFalse;
	
	COrdListIterator iter(GetShapeList ());
	for (Embedding* shape = (Embedding*)iter.Last();
	 	iter.IsNotComplete(); 
	 	shape = (Embedding*)iter.Previous())
	{
		// Clip embedded content
		{
			facet = ((Embedding*)shape)->GetEmbeddedFacet(containingFacet);
			
			if (above)
			{
				tShape = ODCopyAndRelease(ev, facet->GetFrame(ev)->AcquireUsedShape(ev, tBiasCanvas));
				tTransform = facet->AcquireExternalTransform(ev, tBiasCanvas);
				tShape->Transform(ev, tTransform);
				adjusted->Subtract(ev, tShape);
				ODReleaseObject(ev,tShape);
				ODReleaseObject(ev,tTransform);
			}
			else
				above = (ODObjectsAreEqual(ev, facet, embeddedFacet));
		}
	}
	
	if (fActiveBorderShape == kODNULL)
	{
		fActiveBorderShape = adjusted->Copy(ev);
	}

// DCS Test $$$$$ Do we need this ??
//	else
//		fActiveBorderShape->Union(ev, adjusted);
	
	// Put adjusted shape into frame coordinates
	tTransform = embeddedFacet->AcquireExternalTransform(ev, tBiasCanvas);
	adjusted->InverseTransform(ev, tTransform); 
	ODReleaseObject(ev,tTransform);
	
	ClipEmbeddedFacets (containingFacet);
		
	return adjusted;
}

// Description:	Iterate over & clip all facets for the given frame.
void	LedPart::ClipEmbeddedFacets (ODFrame* containingFrame)
{
	#ifdef qProfileLedPart
	// $$$$$ DCS Profiling
	:: ProfilerSetStatus(kODTrue);
	#endif

	// Clip embedded facets for all facets of this frame
	for (TempODFrameFacetIterator iter (SafeGetEnvironment (), containingFrame); iter; ++iter ) {
		ClipEmbeddedFacets (iter.Current ());
	}
	
	#ifdef qProfileLedPart
		// $$$$$ DCS Profiling
		:: ProfilerSetStatus(kODFalse);
	#endif
}

// Description:	Clip content away from facets embedded within the given
// facet.
void	LedPart::ClipEmbeddedFacets (ODFacet* containingFacet)
{
    SOM_Trace ("LedPart","ClipEmbeddedFacets");

	Environment*	ev			=	SafeGetEnvironment ();
	ODCanvas*		tBiasCanvas = containingFacet->GetCanvas(ev);
	ODGeometryMode	tGeoMode = GetCanvasGeometryMode(ev, tBiasCanvas);
	ODShape*		workingClip = 
				ODCopyAndRelease(ev, containingFacet->AcquireClipShape(ev, tBiasCanvas));
	THROW_IF_NULL(workingClip);
			
	// We can only embed into our main presentation,
	// so return if this facet doesn't belong to it
	ODTypeToken presentation = containingFacet->GetFrame(ev)->GetPresentation(ev);
	if (presentation != gGlobals->fMainPresentation)
		return;
	
	ODFacet*	embeddedFacet = kODNULL;
	ODShape* 	newClipShape = kODNULL;
	ODShape* 	newMaskShape = kODNULL;
	ODShape*	tShape = containingFacet->CreateShape(ev);
	ODRgnHandle	tRegion = kODNULL;
	ODTransform* clipTransform = kODNULL;

	// Compute the clip by iterating over all content, native and embedded
	COrdListIterator iter(GetShapeList ());
	for (Embedding* shape = (Embedding*)iter.First();
	 	iter.IsNotComplete(); 
	 	shape = (Embedding*)iter.Next())
	{
		// Clip embedded content
		{
			embeddedFacet = shape->GetEmbeddedFacet(containingFacet);
			
			// Start with the facets FrameShape
			newClipShape = ODCopyAndRelease(ev, 
					embeddedFacet->GetFrame(ev)->AcquireFrameShape(ev, tBiasCanvas));
			newClipShape->SetGeometryMode(ev, tGeoMode);
			
			// Get used shape to obscure underlying facets
			newMaskShape = ODCopyAndRelease(ev, 
						embeddedFacet->GetFrame(ev)->AcquireUsedShape(ev, tBiasCanvas));
			
			clipTransform = embeddedFacet->AcquireExternalTransform(ev, tBiasCanvas);
			
			// Put into containing frame coordinates
			newClipShape->Transform(ev, clipTransform); 
			newClipShape->Intersect(ev, workingClip);
			
			// Put into embedded frame coordinates
			newClipShape->InverseTransform(ev, clipTransform);
			embeddedFacet->ChangeGeometry(ev, newClipShape, kODNULL, tBiasCanvas);
			
			newMaskShape->Transform(ev, clipTransform);
			workingClip->Subtract(ev, newMaskShape);
			
			// Release acquired geometry
			ODReleaseObject(ev,newClipShape);
			ODReleaseObject(ev,newMaskShape);
			ODReleaseObject(ev,clipTransform);
		}
	}

	// Release acquired geometry
	ODReleaseObject(ev,workingClip);
	ODReleaseObject(ev,tShape);
}

// Description:	This method is called when a facet of a part's display
//				frame intersects the invalidated portion of an OpenDoc
//				window. The invalidShape parameter passed in is the
//				portion of the facet which has been invalidated.
//
//				The part sets up the drawing environment using a
//				C++ helper class (CFocusDrawingEnv) and then calls the
//				appropriate drawing method based on the frame's
//				viewType.
void	LedPart::od_Draw (ODFacet* facet, ODShape* invalidShape)
{
	SOM_Trace ("LedPart","Draw");
	
	Environment*	ev			=	SafeGetEnvironment ();

	// Focus the port and origin for drawing in our facet.
	// Note that this instance of the CFocusDrawingEnv class
	// is being allocated on the stack. When the execution
	// leaves the scope of this method, the destructor (which
	// cleans up the drawing environment) is automatically
	// called.
	CFocus initiateDrawing(ev, facet, invalidShape);
		
	// Check the presentation we are being requested to draw
	ODTypeToken presentation = facet->GetFrame(ev)->GetPresentation(ev);
	
	// If its the main presentation, then check the view type of the
	// frame and draw it.
	if (presentation == gGlobals->fMainPresentation)
	{
		ODTypeToken view = facet->GetFrame(ev)->GetViewType(ev);
		if ( view == gGlobals->fLargeIconView || view == gGlobals->fSmallIconView )
			DrawIconView (facet);
		else if ( view == gGlobals->fThumbnailView )
			DrawThumbnailView (facet);
		else
			DrawFrameView(facet, invalidShape);

		if (fHilightedDragFacet == facet) {
			HiliteFacet (GetSession ()->GetDragAndDrop(ev), facet, true);
		}
	}
}

// Description:	This method is called by the part when the frame being
//				drawn is in "frame" view.
//
//				LedPart really has not intrisic content. However,
//				to provide some visual display, we draw the class name
//				at 80% of the frame's current height using an outlined
//				font.
ODFacet*	gImDoingFacet;	// hack set in LedPart::DrawFrameView
void	LedPart::DrawFrameView (ODFacet* facet, ODShape* invalidShape)
{
    SOM_Trace ("LedPart","DrawFrameView");
	
	Led_RequireNotNil (facet);
	Led_RequireNotNil (invalidShape);

	Environment*	ev	=	SafeGetEnvironment ();

	ODFrame*	frame;
	ODSLong		savedRefNum = 0;
	FrameEditor*	frameInfo;

	// If the facet being draw is attached to a source frame in
	// another window, we need to access the source frame to determine
	// what size to draw the content.
	frameInfo = (FrameEditor*) facet->GetFrame(ev)->GetPartInfo(ev);
	if ( frameInfo->HasSourceFrame() )
		frame = frameInfo->GetSourceFrame();
	else
		frame = facet->GetFrame(ev);

	// Clear the screen
	GrafPtr thePort;
	GetPort(&thePort);

Led_Assert (gImDoingFacet == NULL);	// would fail with nested led objects, so we must find a better way!!!
gImDoingFacet = facet;
	frameInfo->DrawFacet (facet, invalidShape);
Led_Assert (gImDoingFacet == facet);
gImDoingFacet = NULL;
}

// Description:	This method is called by the part when the frame being
//				drawn is in "standard icon" view.
//
//				The part uses the Icon Utilities toolbox manager to
//				aid in drawing icons in active windows. The Guidelines
//				require a different appearance for selected icons in
//				inactive windows, which we do manually.
void	LedPart::DrawIconView (ODFacet* facet)
{
    SOM_Trace ("LedPart","DrawIconView");

	Environment*	ev	=	SafeGetEnvironment ();

	Rect				iconRect;
	IconTransformType 	transformType = ttNone;
	ODFrame*			frame; 
	ODTypeToken			viewType;
	
	frame		= facet->GetFrame(ev);
	viewType	= frame->GetViewType(ev);
	FrameEditor*	frameInfo 	= (FrameEditor*) frame->GetPartInfo(ev);
	
	// Check to see if the facet is selected
	if ( facet->GetHighlight(ev) == kODFullHighlight )
		transformType = ttSelected;
	
	// Check to see if the frame has been opened into a part window.
	if ( frameInfo->HasPartWindow() &&
		 frameInfo->GetPartWindow()->IsShown(ev) )
		transformType |= ttOpen;
	
	// Draw the icon.
	if ( viewType == gGlobals->fLargeIconView )
		SetRect(&iconRect, 0, 0, kODLargeIconSize, kODLargeIconSize);
	else // ( viewType == gGlobals->fSmallIconView )
		SetRect(&iconRect, 0, 0, kODSmallIconSize, kODSmallIconSize);

	CUsingLibraryResources res;
	PlotIconID(&iconRect, atAbsoluteCenter, transformType, kDocumentIcons);
}

// Description:	This method is called by the part when the frame being
//				drawn is in "thumbnail" view.
//
//				The part uses a picture for its thumbnail view because
//				it has no intrinsic content. A picture resource is
//				probably not sufficient for parts with real content.
void	LedPart::DrawThumbnailView (ODFacet* facet)
{
    SOM_Trace ("LedPart","DrawThumbnailView");

	Environment*	ev	=	SafeGetEnvironment ();

	/*
	 *	This code needs alot of work. But basic idea is to draw a copy of the normal page
	 *	into a full sized picture, and then draw the picture into a small area, to get
	 *	our scaling done.
	 */
	ODFrame*	frame     = facet->GetFrame (ev);

	Led_AssertNotNil (LookupFrameEditor (frame).GetEditor ());
	FrameEditor::LedODInteractor&	editor	=	*LookupFrameEditor (frame).GetEditor ();
	FrameEditor::LedODInteractor::TmpUseFacetObj useFacetObj (&editor, facet);

	Rect	rct;
	rct.top = editor.GetWindowRect ().top;
	rct.left = editor.GetWindowRect ().left;
	rct.bottom = editor.GetWindowRect ().bottom;
	rct.right = editor.GetWindowRect ().right;
	/*
	 *	Draw the page into a picture and play it back, so we get scaling
	 */
	PicHandle	thumbnailPicture	= ::OpenPicture (&rct);
	if (thumbnailPicture == NULL) {
		Led_ThrowOutOfMemoryException ();
	}
	editor.Draw (editor.GetWindowRect (), false);

	::ClosePicture();

	::SetRect(&rct, 0, 0, 32000, 32000);
	::EraseRect (&rct);
	::SetRect(&rct, 0, 0, kODThumbnailSize, kODThumbnailSize);
	::DrawPicture(thumbnailPicture, &rct);
	::KillPicture (thumbnailPicture);
}

// Description:	This method is called when the ExternalTransform or
//				ClipShape of a facet on one this part's display frames
//				changes.
void	LedPart::od_GeometryChanged (ODFacet* facet, ODBoolean clipShapeChanged, ODBoolean /*externalTransformChanged*/)
{
    SOM_Trace ("LedPart","GeometryChanged");
	if ( clipShapeChanged ) {
		// Specifying kODNULL means to invalidate the clipShape (which was
		// calculated from the usedShape).
		facet->Invalidate (SafeGetEnvironment (), kODNULL, kODNULL);
	}
// New sample part indicates this not needed? - LGP 960129	ClipEmbeddedFacets (facet);  // extra work, but gives correct display
}

void	LedPart::od_HighlightChanged (ODFacet* facet)
{
	Environment*	ev	=	SafeGetEnvironment ();

	ODFrame* frame = facet->GetFrame(ev);
	
	// The frame view has no "special" drawing characteristics
	// when opened or selected, so we don't need to update our
	// content.
	
	if ( frame->GetViewType(ev) != gGlobals->fFrameView ) {
		frame->Invalidate(ev, kODNULL, kODNULL);
	}

	// But we DO need to notify the embedded facets (if any) of the change
	ODHighlight tHighlight = facet->GetHighlight(ev);
	for (TempODFacetIterator iter (ev, facet, kODChildrenOnly, kODFrontToBack); iter; ++iter ) {
		iter->ChangeHighlight(ev, tHighlight);
	}
}

// Description:	This method is called when another part (or possibly
//				ourself) is requesting a focus for one of its display
//				frames. Returning true means we are willing to give
//				up the requested focus.
//
//				The part willingly gives up any focus unless it is the
//				modal focus which we don't want to give up until we
//				are completely done displaying a modal dialog.
bool	LedPart::od_BeginRelinquishFocus (ODTypeToken focus, ODFrame* /*ownerFrame*/, ODFrame* proposedFrame)
{
    SOM_Trace ("LedPart","BeginRelinquishFocus");

	ODBoolean	willRelinquish = kODTrue;

	if (focus == gGlobals->fSelectionFocus) {
		// If one of our embeddings grabs the focus (usually due to a click) we should update
		// our selection region to have that embedding selected.
		Environment*	ev				=	SafeGetEnvironment ();
		Embedding*		embedding		=	ShapeForFrame (proposedFrame);
		if (embedding != NULL) {
			ODFrame*	containingFrame	=	proposedFrame->AcquireContainingFrame (ev);
			Led_AssertNotNil (containingFrame);
			LookupFrameEditor (containingFrame).SetSelection (embedding->GetStart (), embedding->GetEnd ());
			containingFrame->Release (ev);
		}
	}

	// Another part is trying to put up a Modal dialog while we
	// are currently displaying ours. Deny the request.
	if (focus == gGlobals->fModalFocus) {
		Environment*	ev	=	SafeGetEnvironment ();

		ODPart* proposedPart = proposedFrame->AcquirePart(ev);
		if ( ODObjectsAreEqual(ev, proposedPart, GetODPart ()) == kODFalse ) {
			willRelinquish = kODFalse;
		}
		ODReleaseObject(ev,proposedPart);
	}
	
	return willRelinquish;
}

// Description:	This method is called when it is actually time to give
//				up a focus that had been requested by another part (or
//				possibly ourself).
//
//				The part calls its FocusLost method to handle the 
//				"reliquishing" of the particular focus.
void	LedPart::od_CommitRelinquishFocus (ODTypeToken focus, ODFrame* ownerFrame, ODFrame* /*proposedFrame*/)
{
	SOM_Trace ("LedPart","CommitRelinquishFocus");
	// We agreed to give up our FocusSet and now we are being asked to
	// do so.
	od_FocusLost (focus, ownerFrame);
}

// Description:	This method is called when another part (or possibly
//				ourself) requested a focus for one of its display
//				frames, but we returned kODFalse from
//				BeginRelinqishFocus for one, or all, of the requested
//				focus. At this point, we are being told to resume
//				ownership of the focus.
//
//				The part calls its FocusAcquired method to handle the 
//				re-"acquisition" of the particular focus.
void	LedPart::od_AbortRelinquishFocus (ODTypeToken /*focus*/, ODFrame* /*ownerFrame*/, ODFrame* /*proposedFrame*/)
{
    SOM_Trace ("LedPart","AbortRelinquishFocus");

	// Some parts may have suspended some events in the BeginRelinquishFocus
	// method. If so, they would resume those events here.
}

// Description:	This method is called when the Arbitrator has
//				registered us as the "owner" of the particular focus.
//				This can occur if a focus is requested, or if a focus
//				is transfered to one of the part's display frames.
//
//				The part will display the menu bar when the menu focus
//				is acquired, and mark a frame active if the selection
//				focus is acquired.
void	LedPart::od_FocusAcquired (ODTypeToken focus, ODFrame* ownerFrame)
{
	SOM_Trace ("LedPart","FocusAcquired");
	Environment*	ev = SafeGetEnvironment ();
	ODArbitrator*	arbitrator =  GetSession ()->GetArbitrator(ev);
	if ( focus == gGlobals->fMenuFocus ) {
		if ( GetSession ()->GetArbitrator(ev)->RequestFocusSet(ev, gGlobals->fUIFocusSet, ownerFrame)) {
			PartActivated (ownerFrame);
		}
	}
	else if ( focus == gGlobals->fSelectionFocus ) {
		if ( GetSession ()->GetArbitrator(ev)->RequestFocusSet(ev, gGlobals->fUIFocusSet, ownerFrame)) {
			PartActivated (ownerFrame);
		}
	}
}

// Description:	This method is called when the Arbitrator has
//				unregistered us as the "owner" of the particular
//				focus.
//
//				The part unmarks the active frame if the selection
//				focus is lost.
void	LedPart::od_FocusLost (ODTypeToken focus, ODFrame* ownerFrame)
{
    SOM_Trace ("LedPart","FocusLost");

	if ( focus == gGlobals->fSelectionFocus ) {
		Environment*	ev = SafeGetEnvironment ();
		// Mark the frame as inactive
		FrameEditor* frameInfo = (FrameEditor*) ownerFrame->GetPartInfo(ev);
		frameInfo->SetFrameActive(kODFalse);

		LookupFrameEditor (ownerFrame).SetSelectionAndCaretShown (false);
		GetODPart ()->GetStorageUnit(ev)->GetSession(ev)->GetDispatcher(ev)->UnregisterIdle (ev, GetODPart (), ownerFrame);
	}
	else if (focus == gGlobals->fMenuFocus) {
		#if		qOLD_UseResFileHackToEnableBaloonHelp
			EndUsingLibraryResources (sLibResVal);		// tmp hack to see if fixes balloon help??? - see email 951009- "Jens Alfke" <jens_alfke@powertalk.apple.com>
		#endif
	}
}

// Description:	This method relinquishes all foci for the given frame.
void	LedPart::RelinquishAllFoci (ODFrame* frame)
{
	Environment*	ev = SafeGetEnvironment ();
	ODArbitrator* arbitrator = GetODPart ()->GetStorageUnit(ev)->GetSession(ev)->GetArbitrator(ev);
	
	// Make sure the frame going away does not own any foci. Forgetting
	// to do this, will cause a "refcounting" error when the frame
	// is deleted by the draft.
	
	// AcquireFocusOwner increments the frame ref count
	TRY
	TempODFrame focusFrame = arbitrator->AcquireFocusOwner(ev, gGlobals->fKeyFocus);
	if (focusFrame!=kODNULL)
	{
		if ( ODObjectsAreEqual(ev, focusFrame, frame) )
		{
			arbitrator->RelinquishFocus(ev, gGlobals->fKeyFocus, frame);
			od_FocusLost (gGlobals->fKeyFocus, frame);
		}
	}
	CATCH_ALL
	ENDTRY
	
	TRY
	// AcquireFocusOwner increments the frame ref count
	TempODFrame focusFrame = arbitrator->AcquireFocusOwner(ev, gGlobals->fClipboardFocus);
	if (focusFrame!=kODNULL)
	{
		if ( ODObjectsAreEqual(ev, focusFrame, frame) )
		{
			arbitrator->RelinquishFocus(ev, gGlobals->fClipboardFocus, frame);
			od_FocusLost (gGlobals->fClipboardFocus, frame);
		}
	}
	CATCH_ALL
	ENDTRY

	// AcquireFocusOwner increments the frame ref count
	TRY
	TempODFrame focusFrame = arbitrator->AcquireFocusOwner(ev, gGlobals->fSelectionFocus);
	if (focusFrame!=kODNULL)
	{
		if ( ODObjectsAreEqual(ev, focusFrame, frame) )
		{
			arbitrator->RelinquishFocus(ev, gGlobals->fSelectionFocus, frame);
			od_FocusLost (gGlobals->fSelectionFocus, frame);
		}
	}
	CATCH_ALL
	ENDTRY
	
	// AcquireFocusOwner increments the frame ref count
	TRY
	TempODFrame focusFrame = arbitrator->AcquireFocusOwner(ev, gGlobals->fMenuFocus);
	if (focusFrame!=kODNULL)
	{
		if ( ODObjectsAreEqual(ev, focusFrame, frame) )
		{
			arbitrator->RelinquishFocus(ev, gGlobals->fMenuFocus, frame);
			od_FocusLost (gGlobals->fMenuFocus, frame);
		}
	}
	CATCH_ALL
	ENDTRY
}

// Description:	This method is called when the part has successfully acquired
//				the set of foci which allow it to "run".
void	LedPart::PartActivated (ODFrame* frame)
{
    SOM_Trace ("LedPart","PartActivated");
    
    // We are required to re-validate the menubar before displaying it because
    // any part can/could swap the base menubar at any time.
	Environment*	ev = SafeGetEnvironment ();
	if (!gGlobals->fMenuBar->IsValid (ev)) {
		ReBuildMenuBar ();
	}
		
	// Display our menu bar.
	gGlobals->fMenuBar->Display(ev);

	#if		qOLD_UseResFileHackToEnableBaloonHelp
		sLibResVal = BeginUsingLibraryResources ();		// tmp hack to see if fixes balloon help??? - see email 951009- "Jens Alfke" <jens_alfke@powertalk.apple.com>
	#endif

	// And set our "active" state.
	FrameEditor* frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
	frameInfo->SetFrameActive(kODTrue);

	/*
	 *	Only blink the caret if we have the SELECTION focus
	 */
	ODArbitrator* arbitrator = GetODPart ()->GetStorageUnit(ev)->GetSession(ev)->GetArbitrator(ev);
	TempODFrame focusFrame = arbitrator->AcquireFocusOwner(ev, gGlobals->fKeyFocus);
	if (focusFrame!=kODNULL and ODObjectsAreEqual(ev, focusFrame, frame)) {

		LookupFrameEditor (frame).SetSelectionAndCaretShown (true);

		// divide by four since otherwise blinks too slow??? LGP 950821???
		// seems a little better with OpenDoc 1.0, especially at first. But they
		// seem to slow down unless I divide!!! - LGP 951212
		ODIdleFrequency	ticks = TextInteractor_::GetTickCountBetweenBlinks ()/4;

		Led_AssertNotNil (GetODPart ()->GetStorageUnit(ev));
		Led_AssertNotNil (GetODPart ()->GetStorageUnit(ev)->GetSession(ev));
		Led_AssertNotNil (GetODPart ()->GetStorageUnit(ev)->GetSession(ev)->GetDispatcher(ev));
		GetODPart ()->GetStorageUnit(ev)->GetSession(ev)->GetDispatcher(ev)->RegisterIdle(ev, GetODPart (), frame, ticks);
	}
}

// Description:	This method is called by the part when a kODEvtMouseUp
//				occurs in an inactive frame in an active window, and
//				when an OpenDoc document comes forward.
//
//				The part activates the frame by requesting the
//				UIFocusSet (created in Initialize) and by calling
//				FocusAcquired if we were successful. The method
//				returns true if no problems were encountered as a
//				signal to the caller that the frame is now "active".
ODBoolean	LedPart::ActivateFrame (ODFrame* frame)
{
    SOM_Trace ("LedPart","ActivateFrame");

	Environment*	ev = SafeGetEnvironment ();

	// only activate frames that are in the main presentation
	ODTypeToken presentation = frame->GetPresentation(ev);
	if (presentation == gGlobals->fMainPresentation) {
		// Request the set of foci necessary to become active.
		if ( GetSession ()->GetArbitrator(ev)->
					RequestFocusSet(ev, gGlobals->fUIFocusSet, frame))
		{
			PartActivated (frame);
		}
		else
			// We were unable to acquire the foci we need to be active.
			return kODFalse;
	}
	else
		// Cannot activate frames not in the main presentation.
		return kODFalse;
	
	// We sucessfully acquired the foci we need to be active.
	return kODTrue;
}

// Description:	This method is called by the part when a process
//				switch event (kODEvtOS) occurs.
//
//				The part remembers the frame's active state and restores it when
//				events come in. If the frame is active and the window is being
//				sent into the background, remember to reactivate the part when
//				the window is reactivated.
void	LedPart::WindowActivating(ODFrame* frame, ODBoolean activating)
{
    SOM_Trace ("LedPart","WindowActivating");

	FrameEditor& frameEditor = LookupFrameEditor (frame);
	
	// Check the presentation of the frame
	// Make sure it is the main presentation
	Environment*	ev = ::SafeGetEnvironment ();
	ODTypeToken presentation = frame->GetPresentation (ev);
	if (presentation == gGlobals->fMainPresentation)
	{

	// The window is being activated and we had the selection focus when
	// the window was deactivated, so activate ourself.
		if ( activating && frameEditor.FrameNeedsReactivating() ) {
			ActivateFrame(frame);
			frameEditor.SetFrameReactivate(kODFalse);
		}
		// The window is being deactivated and we have the active part.
		// So give up our foci and remind ourselves that we need to request
		// the focus when the window is reactivated.
		else if ( !activating && frameEditor.IsFrameActive() )
		{
			frameEditor.SetFrameReactivate(kODTrue);
		}

#if 1
		// DO SOMETHING LIKE THIS???
#else
		// Must set hilite state of selected facets here
		fSelection->Activate (ev, activating, frame);
#endif

		if (!activating) {
			RelinquishAllFoci (frame);
		}

		{
		// hack - really must iterate over all facets, todo this right!!!
			ODFacet*	facet	=	GetActiveFacetForFrame (frame);
			if (facet != NULL) {
				CFocus tab (ev, facet);
				frameEditor.ActivateScrollBars (activating);
			}
		}

	}

}

// Description:	This method is called when a command needs to be done for the first time.
//				
//
// Assumptions:	This method assumes that if the command was not undoable that it may
//				be deleted after it is executed. Undo-able commands get registered with
//				OpenDoc and connot be deleted until they are committed.
void	LedPart::ExecuteCommand (CCommand* command)
{
    SOM_Trace ("LedPart","ExecuteCommand");
	try {
		command->DoCommand();			// execute the command
		if (!command->CanUndo()) {	// This command was not registered with OpenDoc.
			command->Commit (kODDone);
			delete command;
		}
	}
	catch (...) {
		command->AbortAction ();
		throw;
	}
}

// Description:	This method is called by OpenDoc when a command needs to be undone.
void	LedPart::od_UndoAction (ODActionData* actionState)
{
    SOM_Trace ("LedPart","UndoAction");
	TRY
		CCommand* undoCommand = *((CCommand**)actionState->_buffer);
		if (undoCommand==kODNULL)
			return;
		undoCommand->UndoCommand();
	CATCH_ALL
		ODSetSOMException(SafeGetEnvironment (), kODErrUndefined);
	ENDTRY
}

// Description:	This method is called by OpenDoc when a command needs to be redone.
void	LedPart::od_RedoAction (ODActionData* actionState)
{
    SOM_Trace ("LedPart","RedoAction");

	TRY
		CCommand* redoCommand = *((CCommand**)actionState->_buffer);
		
		// Better not be null!
		if (redoCommand==kODNULL)
			return;
		
		redoCommand->RedoCommand();
	CATCH_ALL
		ODSetSOMException (SafeGetEnvironment (), kODErrUndefined);
	ENDTRY
}

// Description:	This method is called by OpenDoc when an action has been
// committed.
void	LedPart::od_DisposeActionState (ODActionData* actionState, ODDoneState doneState)
{
    SOM_Trace ("LedPart","DisposeActionState");

	TRY
		CCommand* theCommand = *((CCommand**)actionState->_buffer);
	
		ODBoolean shouldDeleteCommand = kODTrue;
		
		// The command may be null, as in the case of a begin action.
		// See Command.h
		if (theCommand)
		{
			theCommand->Commit(doneState);
			
			if (shouldDeleteCommand)
				delete theCommand;
		}
		
	CATCH_ALL
		ODSetSOMException (SafeGetEnvironment (), kODErrUndefined);
	ENDTRY
}

// Description:	The method is called when an event, which falls into
//				the category of an owned focus, occurs. The two
//				exceptions to this are "mouse movement" events and 
//				embedded frame events, which can occur when a part
//				owns no foci.
//
//				The part returns true if the event was handled.
bool	LedPart::od_HandleEvent (ODEventData* event, ODFrame* frame, ODFacet* facet, ODEventInfo* eventInfo)
{
    SOM_Trace ("LedPart","HandleEvent");

	#if		qUseResFileHackToEnableBaloonHelp
		if (sLibResVal != -1) {
			EndUsingLibraryResources (sLibResVal);
			sLibResVal = -1;
		}
	#endif

	bool	eventHandled = false;
	switch (event->what) {

#if 1
		case kODEvtMouseDown:
		case kODEvtBGMouseDown :		
		case kODEvtMouseUpBorder:
		case kODEvtMouseUp:
		case kODEvtMouseUpEmbedded:
		case kODEvtMouseDownEmbedded:
		case kODEvtBGMouseDownEmbedded :
			eventHandled = HandleMouseEvent (event, facet, eventInfo);
			break;
			
		// Special case
		case kODEvtMouseDownBorder:
				eventHandled = HandleMouseDownInEmbeddedFrameBorder(facet, event, eventInfo );
			break;
			
#else
		case kODEvtMouseDown:
			eventHandled = HandleMouseEvent (event, facet, eventInfo);
			break;
			
		case kODEvtMouseUpBorder:
		case kODEvtMouseUp:
		case kODEvtMouseUpEmbedded:
// UNSURE WHY WE DONT SET eventHandled here!????
			HandleMouseUpActivate (facet);
			break;
		
		case kODEvtMouseDownEmbedded:
				eventHandled = HandleMouseDownInEmbeddedFrame (facet, event, eventInfo );
			break;
			
		case kODEvtMouseDownBorder:
				eventHandled = HandleMouseDownInEmbeddedFrameBorder (facet, event, eventInfo );
			break;
#endif

		case kODEvtMenu:
			eventHandled = HandleMenuEvent (event, frame);
			break;

		case kODEvtActivate:
			// We are being notified that a window we are displayed in has
			// just been de/activated (un/hilighted).
			WindowActivating (frame, (event->modifiers & activeFlag));
			eventHandled = kODTrue;
			break;
	
		case kODEvtMouseEnter: {
			if (frame != NULL) {
				LookupFrameEditor (frame).AdjustCursor (AsLedPoint (eventInfo->where));
				eventHandled = kODTrue;
			}
		}
		break;

		case kODEvtMouseLeave: {
			SetCursor (&ODQDGlobals.arrow);
			eventHandled = kODTrue;
		}
		break;

		case kODEvtMouseWithin: {
			if (frame != NULL) {
				LookupFrameEditor (frame).AdjustCursor (AsLedPoint (eventInfo->where));
				eventHandled = kODTrue;
			}
		}
		break;

		case kODEvtWindow:
			eventHandled = HandleWindowEvent (event, frame);
			break;
		
		// Other events a part might handle
		case kODEvtNull: {
			if (frame != NULL and DisplayingAsFrameMode (frame)) {
				LookupFrameEditor (frame).BlinkCaretIfNeeded ();
			}
		}
		break;

		case kODEvtKeyDown:
		case kODEvtAutoKey: {
			if (not (event->modifiers & cmdKey)) {
				if (DisplayingAsFrameMode (frame)) {
					LookupFrameEditor (frame).TypeChar ((event->message & charCodeMask), (event->message & keyCodeMask)>>8, event->modifiers);
					SetDirty ();
				}
				else {
					Led_BeepNotify ();
				}
				eventHandled = kODTrue;
			}
		}
		break;

		case kODEvtKeyUp: {
		}
		break;

		case kODEvtDisk: {
		}
		break;

		case kODEvtOS: {
			// We are being notified that a window we are display in has
			// just been suspened/resumed.
			WindowActivating (frame, (event->modifiers & resumeFlag));
			eventHandled = kODTrue;
		}
		break;

		default:
			break;
	}

	return eventHandled;
}

// Description:	This method is called by the part when a menu event is
//				received.
//
//				Using the menu bar object, we determine what the menu
//				command is, and call the appropriate method to handle
//				it.
bool	LedPart::HandleMenuEvent (ODEventData* event, ODFrame* frame)
{
	SOM_Trace ("LedPart","HandleMenuEvent");

	ODULong		menuResult	= event->message;
	ODUShort	menu		= HiWord(menuResult);
	ODUShort	item		= LoWord(menuResult);

	Environment*	ev = SafeGetEnvironment ();

	ODCommandID commandID =  gGlobals->fMenuBar->GetCommand(ev, menu, item);

	if (menu == kFontMenuID or menu == kFontSizeMenuID or menu == kFontStyleMenuID) {
		OnFontCommand (frame, commandID);
		return true;
	}

	switch (commandID) {
		case kODCommandAbout:			OnAboutBoxCommand (frame);				return true;

		// Document
		case kODCommandOpen:			OnOpenSelectionCommand (frame);			return true;

		// Edit
		case kODCommandCopy:			OnCopyCommand (frame);					return true;
		case kODCommandCut: 			OnCutCommand (frame);					return true;	
		case kODCommandPaste:			OnPasteCommand (frame);					return true;
		case kODCommandPasteAs:			OnPasteAsCommand (frame);				return true;
		case kODCommandClear:			OnClearCommand (frame);					return true;
		case kODCommandSelectAll: 		OnSelectAllCommand (frame);				return true; 
		case kODCommandGetPartInfo:		OnGetPartInfoCommand (frame);			return true;
		case kODCommandPreferences:		OnPreferencesCommand (frame);			return true;
		case kODCommandViewAsWin:		OnViewAsWindowCommand (frame);			return true;
		case kFindCommand:				OnFindEtcCommand (frame, commandID);	return true;
		case kFindAgainCommand:			OnFindEtcCommand (frame, commandID);	return true;
		case kEnterFindStringCommand:	OnEnterFindStringCommand (frame);		return true;
		case kODCommandPageSetup:
			if( GetPrinter()->PageSetup(SafeGetEnvironment ()) )
				SetDirty();
			break;
			
		case kODCommandPrint:
			Print (frame);
			break;
	
		// Other commands a part might handle
		case kODCommandInsert:
			break;

	}

	return false;
}

// Description:	This method is called by handlers which respond to mouse ups.
// Here we will do the necessary work to activate the frame and window, if they
// are not already active.
//
//
// Remember:	When a frame is inactive, the first mouse up event
//				should activate it; inactive frames do not recieve
//				kODEvtMouseDown events.
bool	LedPart::HandleMouseUpActivate (ODFacet* facet)
{
	Environment* ev		=	SafeGetEnvironment ();

	ODWindow*	window	=	facet->GetWindow(ev);

	TRY
		// Activate inactive windows on the first mouse up event.
		if ( !window->IsActive(ev) )
			window->Select(ev);
		// Activate the frame (if needed) on all subsequent mouse up events.
		else
		{
			ODFrame* frame = facet->GetFrame(ev);
			
			// Get our state information from the PartInfo of the frame.
			FrameEditor* frameInfo = (FrameEditor*) frame->GetPartInfo(ev);
							
			// If this frame is not the active one, activate it by requesting
			// the appropriate foci.
	
			if ( !frameInfo->IsFrameActive() )
			{
				if ( ActivateFrame(frame) )
				{
					// Keep track of which facet was the last active for positioning
					// child windows.
					frameInfo->SetActiveFacet(facet);
				}
				else
					// We were unable to acquire the necessary focus and activate it.
					return kODFalse;
			}
		}
	CATCH_ALL
		return kODFalse;
	ENDTRY
	
	return kODTrue;
}

// Description:	This method is called by the part when a mouse event
//				is recieved.
//
//				A typical part would determine the event type(up/down)
//				and respond appropriately, but we have no "real"
//				content model, so we activate the part as appropriate
//				and call a generic method to handle the event.
//
// Remember:	When a frame is inactive, the first mouse up event
//				should activate it; inactive frames do not recieve
//				kODEvtMouseDown events.
bool	LedPart::HandleMouseEvent (ODEventData*	event, ODFacet* facet, ODEventInfo*	eventInfo )
{
    SOM_Trace ("LedPart","HandleMouseEvent");

	Environment* ev		=	SafeGetEnvironment ();

	if ( facet != kODNULL ) {
#if 1
		switch ( event->what)
		{
		case kODEvtMouseUp :
		case kODEvtMouseUpEmbedded:
		case kODEvtMouseUpBorder:
				this->HandleMouseUpActivate (facet);
				return kODTrue;
				break;
				
		case kODEvtMouseDownEmbedded:
		case kODEvtBGMouseDownEmbedded :	
				return HandleMouseDownInEmbeddedFrame(facet, event, eventInfo );
				break;
				
		case kODEvtMouseDown :		
		case kODEvtBGMouseDown :		
			// Handle the mouse down event.
			this->HandleMouseDown(facet, event, eventInfo);
			return kODTrue;
			break;
			
		case kODEvtMouseDownBorder:
			return HandleMouseDownInEmbeddedFrameBorder(facet, event, eventInfo );
			return kODTrue;
			break;
		
			// Always handles event
			return kODTrue;
		default:
			return kODFalse;
		}
#else
		if ( event->what == kODEvtMouseUp ) {
			HandleMouseUpActivate (facet);
		}
		else if ( event->what == kODEvtMouseDown ) {
			// Handle the mouse down event.
			HandleMouseDown (facet, event, eventInfo);
			
			// Always handles event
			return kODTrue;
		}
#endif
	}
	else
	{
		// User clicked outside the bounds of a Modal window.
		
		SysBeep(1);
		return kODTrue;
	}

	return false;
}

// Description:	This method is called by the part to handle window events.
bool	LedPart::HandleWindowEvent (ODEventData* /*event*/, ODFrame* /*frame*/)
{
	return false;
}

bool	LedPart::od_RevealFrame (ODFrame* /*embeddedFrame*/, ODShape* /*revealShape*/)
{
	return false;
}

void	LedPart::od_EmbeddedFrameSpec (ODFrame* /*embeddedFrame*/, ODObjectSpec* /*spec*/)
{
}

// Description:	This method may be called by OpenDoc when an asynchronous
// drop is completed. I also call this method from my DropShape command to 
// turn off drag hiliting.
void	LedPart::od_DropCompleted (ODPart* /*part*/, ODDropResult /*result*/)
{
	// If drag hiliting is on turn it off
	if (fHilightedDragFacet != NULL) {
		Environment*	ev = ::SafeGetEnvironment ();
		HiliteFacet (GetSession ()->GetDragAndDrop(ev), fHilightedDragFacet, kODFalse);
		fHilightedDragFacet = kODNULL;
	}
}

// Description:	Track the drag. The part should display a drag target border within 
// 				the facet. During drag tracking (DragEnter, DragWithin) the part should 
//				never attempt to read data from any of the storage units supplied
//				by the iterator. The part should only inspect the type of
//				the dragged data.
//
//				If the part can accept a drop, it should return 
//				kODTrue. Otherwise, it should return kODFalse.

static	bool	oldCaretShownFlag	=	false;	// should use an instance variable???

ODDragResult	LedPart::od_DragEnter (ODDragItemIterator* /*dragInfo*/, ODFacet* facet, ODPoint*  /*where*/)
{
	// Do not accept drags if this draft is marked read only
	if (IsReadOnly())
		return kODFalse;
	
	// Do not accept drags if we are in low memory conditions
	TRY
		// Will throw if in low mem
		::ODCheckAppHeapSpace();
	CATCH_ALL
		return kODFalse;
	ENDTRY
	
	Environment*	ev = ::SafeGetEnvironment ();

	// We can accept anything
	HiliteFacet (GetSession ()->GetDragAndDrop(ev), facet, kODTrue);
	fHilightedDragFacet = facet;

	ODFrame*	frame	=	facet->GetFrame (SafeGetEnvironment ());
	oldCaretShownFlag = LookupFrameEditor (frame).GetEditor ()->GetCaretShown ();
	LookupFrameEditor (frame).GetEditor ()->SetCaretShown (true);

	return kODTrue;
}

// Description:	This method is called by OpenDoc when a drag occurs within 
// a facet.
void	LedPart::od_DragLeave (ODFacet* facet, ODPoint*  /*where*/)
{
	Environment*	ev = ::SafeGetEnvironment ();
	if (fHilightedDragFacet != NULL) {
		HiliteFacet (GetSession ()->GetDragAndDrop(ev), facet, kODFalse);
		fHilightedDragFacet = kODNULL;
	}
	ODFrame*	frame	=	facet->GetFrame (ev);
	LookupFrameEditor (frame).GetEditor ()->SetCaretShown (oldCaretShownFlag);
}

// Description:	This method is called by OpenDoc when a drag occurs within 
// a facet.
ODDragResult	LedPart::od_DragWithin (ODDragItemIterator* /*dragInfo*/, ODFacet* facet, ODPoint*  where)
{
	ODFrame*	frame	=	facet->GetFrame (SafeGetEnvironment ());
	return LookupFrameEditor (frame).HandleDragWithin (facet, AsLedPoint (*where));
}

extern	bool	sOurDrag;
extern	size_t	sOurDragStart;
extern	size_t	sOurDragEnd;

// Description:	This method is called by OpenDoc when a drop occurs.
ODDropResult	LedPart::od_Drop (ODDragItemIterator* dragInfo, ODFacet* facet, ODPoint*  where)
{
	Environment*	ev = ::SafeGetEnvironment ();

	if (sOurDrag) {
		size_t	newPos	=	LookupFrameEditor (facet->GetFrame (ev)).GetSelectionStart ();
		if (newPos >= sOurDragStart and newPos <= sOurDragEnd) {
			if (fHilightedDragFacet != NULL) {
				HiliteFacet (GetSession ()->GetDragAndDrop(ev), facet, kODFalse);
				fHilightedDragFacet = kODNULL;
			}
			return kODDropFail;
		}
	}
	
	ODDropResult dropResult = kODDropFail;

	// ----- 'where' is in frame coordinate. Put it in content coordinate
	ODPoint dropPoint = *where;
	ODFrame* tFrame = facet->GetFrame(ev);
	ODTransform* frameTrans = tFrame->AcquireInternalTransform(ev, kODNULL);
	frameTrans->InvertPoint(ev, &dropPoint); // Drop point is modified in place
	ODReleaseObject(ev,frameTrans);
		
	//$$$$$ we need first to check the drag attributes and show a paste as dialog if so indicated
	// It would be nice to factor some common code from here and from DoPasteAs (for the menu choice)
	// but there are several little differences that make it a nuisance to do so.
	
	ODBoolean usePAResult = kODFalse;
	ODStorageUnit* dropSU = dragInfo->First(ev);
	ODBoolean canMerge = dropSU->Exists(ev, kODPropContents, kLedPartKind, 0);
	ODBoolean canEmbedSingleFrame = dropSU->Exists(ev, kODPropContentFrame, kODWeakStorageUnitRef, 0);
	ODPasteAsMergeSetting mergeSetting;
	ODPasteAsResult paResult;
	
	if (canMerge) {
		if (canEmbedSingleFrame)
			mergeSetting = kODPasteAsEmbed;
		else
			mergeSetting = kODPasteAsMerge;
	}
	else {
		mergeSetting = kODPasteAsEmbedOnly;
	}

	ODSession* session = GetSession ();
	ODDragAndDrop* drag = session->GetDragAndDrop(ev);
	ODULong attributes = drag->GetDragAttributes(ev);
		
	if (attributes & kODDropIsPasteAs) {
		usePAResult = drag->ShowPasteAsDialog(ev, kODTrue, mergeSetting, 
									 facet,	// facet from which dialog is triggered
									 session->Tokenize(ev, kODViewAsFrame), // viewType of data
									 dropSU, &paResult);
		if (usePAResult)
		{
			ODBoolean doLink = paResult.pasteLinkSetting == kODTrue;
	
			if (doLink)
			{
				ODBoolean defaultIsMerge = (mergeSetting == kODPasteAsMerge);
				Led_Assert (false);	// NYI
				return kODDropCopy;
			}
		}
		else
			return kODDropFail;  // User cancelled.
									 
	}

#if 1
	CDropShapeCommand* dropCommand = kODNULL;
	if (attributes & kODDropIsPasteAs)
	{
		
		usePAResult = drag->ShowPasteAsDialog(ev, kODTrue, mergeSetting, 
									 facet,	// facet from which dialog is triggered
									 session->Tokenize(ev, kODViewAsFrame), // viewType of data
									 dropSU, &paResult);
		if (usePAResult)
		{
			TRY
				ODBoolean doLink = paResult.pasteLinkSetting == kODTrue;
		
				if (doLink)
				{
					ODBoolean defaultIsMerge = (mergeSetting == kODPasteAsMerge);
					dropResult = kODDropFail;
					Led_Assert(false);	// nyi
				}
				else
				{
					ODBoolean doMerge = canMerge && paResult.mergeSetting;
				
					// determine embed vs merge choice from paResult.
					dropCommand = new CDropAsCommand(this, facet, dragInfo, dropPoint, !doMerge);
					ExecuteCommand (dropCommand);
					
					// Remember the drop result
					dropResult = dropCommand->GetDropResult();
				}
				
			CATCH_ALL
				dropResult = kODDropFail;
			ENDTRY
		}
		else
		{
			// User cancelled.
			
			// Turn off drag highlight
			if (fHilightedDragFacet != NULL) {
				HiliteFacet (GetSession()->GetDragAndDrop(ev), facet, kODFalse);
				fHilightedDragFacet = kODNULL;
			}
			
			// Fail the drop
			dropResult = kODDropFail;  
		}
									 
		// Dispose of pasteAsResult fields
		ODDisposePtr(paResult.selectedKind);
		ODDisposePtr(paResult.translateKind);
		ODDisposePtr(paResult.editor);
	}
	else
	{
		dropCommand = new CDropShapeCommand(this, facet, dragInfo, dropPoint, kODTrue);

		// This should be safe to do since D&D is undoable, the command won't be deleted
		// right away. So, we can query the drop result in the next block.
		ExecuteCommand (dropCommand);
		
		// Remember the drop result
		dropResult = dropCommand->GetDropResult();
	}
					
	return dropResult;
#else

	if (usePAResult)
	{
		//$$$$$(MH) determine embed vs merge choice from paResult here DCS Look HERE
	}
	
	CDropShapeCommand* dropCommand = new CDropShapeCommand(this, facet, dragInfo, dropPoint, kODTrue);

	// This should be safe to do since D&D is undoable, the command won't be deleted
	// right away. So, we can query the drop result in the next block.
	ExecuteCommand (dropCommand);

	// Remember the drop result
	dropResult = dropCommand->GetDropResult();
	
	// Delete the command if it was not undoable ( i.e not registered with OpenDoc )
	if (!dropCommand->CanUndo()) {
		dropCommand->Commit(kODDone);
		delete dropCommand;
	}

	return dropResult;
#endif
}

bool	LedPart::HandleMouseDownInEmbeddedFrame (ODFacet* facet, ODEventData* /*event*/, ODEventInfo* eventInfo)
{
	// possibly I should check if the click is on the edge, and resize? Otheriwse, just do nothing
	// and let embedded part handle the click???
	// LGP 951210

	Led_RequireNotNil (facet);
	Led_RequireNotNil (eventInfo);

	/*
	 *	When we click on an embedding, we should change the selction to refer to that part.
	 *	Note, we also reset the selection on a focus change to one of our embeddings. We do
	 *	the check here ALSO, in case the frame is disabled somehow (like shown as icon).
	 *	We still want to reset the selection.
	 */
	Environment*	ev				=	SafeGetEnvironment ();
	ODFrame*		embeddedFrame	=	eventInfo->embeddedFrame;
	Led_AssertNotNil (embeddedFrame);
	Embedding*		embedding		=	ShapeForFrame (embeddedFrame);
	Led_AssertNotNil (embedding);
	TempODFrame	containingFrame	=	embeddedFrame->AcquireContainingFrame (ev);
	Led_AssertNotNil (containingFrame);
	LookupFrameEditor (containingFrame).SetSelection (embedding->GetStart (), embedding->GetEnd ());

	return true;
}

bool	LedPart::HandleMouseDownInEmbeddedFrameBorder (ODFacet* /*facet*/, ODEventData* /*event*/, ODEventInfo* /*eventInfo*/)
{
	// possibly I should check if the click is on the edge, and resize? Otheriwse, just do nothing
	// and let embedded part handle the click???
	// Possibly I should deactivate the part, and simply select it???? I think this is SO!!!
	// LGP 951210
	return false;
}

// Description:	This method is called by the part to handle mouse
//				events.
void	LedPart::HandleMouseDown (ODFacet* facet, ODEventData*	event, ODEventInfo*	/*eventInfo*/)
{
    SOM_Trace ("LedPart","HandleMouseDown");
    

	Environment* ev		=	SafeGetEnvironment ();
	ODTypeToken presentation = facet->GetFrame(ev)->GetPresentation(ev);
	if (presentation == gGlobals->fMainPresentation) {
		// because our current HandleMouseDown code does a loop and STILLDOWN(), that appears
		// to eat our mouse up event. So we fake/handle that here, so we get our
		// focus changed, etc...
		// Also, I think we really want todo most of that activation stuff on the mouse
		// DOWN anyhow!!!
		//
		// LGP 951205
		HandleMouseUpActivate (facet);

		ODFrame*	frame	=	facet->GetFrame (ev);
		(void)LookupFrameEditor (frame).HandleMouseDownInFrame (event, frame, facet);
	}
}

void	LedPart::od_EmbeddedFrameUpdated (ODFrame* /*frame*/, ODUpdateID /*updateID*/ )
{
	// For now at least, I believe we are not interested in changes to embedded content...
}

ODLinkSource*	LedPart::od_CreateLink (ODByteArray* /*data*/)
{
	THROW (kODErrDoesNotLink);
	return NULL;	// so no compiler warning...
}

CPrinter*	LedPart::GetPrinter ()
{
	return fPrinter;
}

void	LedPart::Print (ODFrame* frame)
{


/// See if this is good place to set/reset scrolltoend flag!!!

	try {
		GetPrinter()->PrintDocument (SafeGetEnvironment (), frame);
	}
	catch (...) {
		throw;
	}
}

// Description:	This method makes OpenDoc calls necessary before a dialog is displayed.
// This method returns false if it can't successfully prepare ( i.e can't get modal focus ).
ODBoolean	LedPart::PrepareForDialog (ODFrame* frame)
{
    SOM_Trace ("LedPart","PrepareForDialog");

	Environment*	ev				=	SafeGetEnvironment ();

	ODSession*	session = GetODPart ()->GetStorageUnit(ev)->GetSession(ev);
	
	// Our About box is modal so we must request the Modal focus to prevent
	// multiple modal dialogs being displayed simultaneously.
	
	if ( session->GetArbitrator(ev)->RequestFocus(ev, gGlobals->fModalFocus, frame) )
	{
		TRY
			// Dim the frontmost document window.
			session->GetWindowState(ev)->DeactivateFrontWindows(ev);
			return kODTrue;
		CATCH_ALL
			session->GetArbitrator(ev)->RelinquishFocus(ev, gGlobals->fModalFocus, frame);
			RERAISE;
		ENDTRY
	}
	
	return kODFalse;
}

// Description:	This method makes OpenDoc calls necessary when a dialog is closed.
void	LedPart::DoneWithDialog (ODFrame* frame)
{
    SOM_Trace ("LedPart","DoneWithDialog");

	Environment*	ev				=	SafeGetEnvironment ();

	ODSession*	session = GetODPart ()->GetStorageUnit(ev)->GetSession(ev);
	
	// Inform the Arbitrator that we no longer require the Modal focus.
	session->GetArbitrator(ev)->RelinquishFocus(ev, gGlobals->fModalFocus, frame);
	
	// Hilite the frontmost document window.
	session->GetWindowState(ev)->ActivateFrontWindows(ev);
}

// Description:	This method is called by the part when a dialog needs to be
//				displayed (ie. the About Box). If a valid error number is passed
//				in, an error dialog will be displayed.
bool	LedPart::DoDialogBox (ODFrame* frame, ODSShort dialogID, ODUShort errorNumber, const char* paramText1, const char* paramText2)
{
    SOM_Trace ("LedPart","DoDialogBox");

	Environment*	ev				=	SafeGetEnvironment ();

	ODFrame* focusFrame = frame;
	ODSession*	session = ODGetSession(ev,GetODPart ());
	
	// If the calling method does not have a frame available to it, we need to
	// locate a frame to use for requesting the modal focus. Find the first valid
	// frame in our display frames list.
	if (focusFrame == kODNULL) {
		COrdListIterator fiter(fDisplayFrameProxies);
		for ( CDisplayFrameProxy* frameProxy = (CDisplayFrameProxy*)fiter.First();
				fiter.IsNotComplete();
				frameProxy = (CDisplayFrameProxy*)fiter.Next())
		{
			// shouldn't get called to remove a frame that has
			// not yet been internalized
			if (frameProxy->IsFrameInMemory())
			{
				if (frameProxy->GetFrame(ev))
				{
					focusFrame = frameProxy->GetFrame(ev); 
				}
			}
		}
	}

	ODSShort	itemHit	=	cancel;

	// Our dialog boxes are modal so we must request the Modal focus to prevent
	// multiple modal dialogs being displayed simultaneously.
	if ( PrepareForDialog(frame) ) {
		DialogPtr	dialog;

		ODSLong rfRef;
		rfRef = BeginUsingLibraryResources();
		{
			//dialog = GetNewDialog(dialogID, kODNULL, (WindowPtr) -1L);
			dialog = ODGetNewDialog(NULL,  dialogID, session, true);
			
			if ( dialog )
			{
				Handle	itemHandle;
				Rect	itemRect;
				short	itemType;
				Str255	textPStr;


				Str255	pParam1;
				pParam1[0] = (paramText1==NULL)? 0: strlen (paramText1);
				::memcpy (&pParam1[1], paramText1, pParam1[0]);
				Str255	pParam2;
				pParam2[0] = (paramText2==NULL)? 0: strlen (paramText2);
				::memcpy (&pParam2[1], paramText2, pParam2[0]);
				::ParamText (pParam1, pParam2, "\p", "\p");
				if ( errorNumber > 0 ) {
					Str255	errStr;
					GetIndString (errStr, kErrorStringResID, errorNumber);
					GetDialogItem (dialog, kErrStrFieldID, &itemType, &itemHandle, &itemRect);
					SetDialogItemText (itemHandle, errStr);
					
					// We don't need the cancel button for an error dialog.
					HideDialogItem(dialog, cancel);

					SetDialogDefaultItem(dialog, ok);
				}
				
				// GROSS HACK - we really should encapsualte this separately, and have subclasses for each dialog!!!
				if (dialogID == kFind_DialogID) {
					GetDialogItem (dialog, 4, &itemType, &itemHandle, &itemRect);
					textPStr[0] = (fLastSearchText==NULL)? 0: strlen (fLastSearchText);
					memcpy (&textPStr[1], fLastSearchText, textPStr[0]);
					SetDialogItemText (itemHandle, textPStr);
					SelectDialogItemText (dialog, 4, 0, 1000);

					GetDialogItem (dialog, 5, &itemType, &itemHandle, &itemRect);
					SetControlMinimum (ControlRef (itemHandle), 0);
					SetControlMaximum (ControlRef (itemHandle), 1);
					SetControlValue (ControlRef (itemHandle), fWrapSearch);

					GetDialogItem (dialog, 6, &itemType, &itemHandle, &itemRect);
					SetControlMinimum (ControlRef (itemHandle), 0);
					SetControlMaximum (ControlRef (itemHandle), 1);
					SetControlValue (ControlRef (itemHandle), fWholeWordSearch);

					GetDialogItem (dialog, 7, &itemType, &itemHandle, &itemRect);
					SetControlMinimum (ControlRef (itemHandle), 0);
					SetControlMaximum (ControlRef (itemHandle), 1);
					SetControlValue (ControlRef (itemHandle), fCaseSensativeSearch);
				}
				if (dialogID == kPrefs_DialogID) {
					GetDialogItem (dialog, 3, &itemType, &itemHandle, &itemRect);
					SetControlMinimum (ControlRef (itemHandle), 0);
					SetControlMaximum (ControlRef (itemHandle), 1);
					SetControlValue (ControlRef (itemHandle), sTmpHackHasSBar);

				}
	
				SetCursor(&ODQDGlobals.arrow);
				ShowWindow(dialog);
				
				do {
					ModalDialog (kODNULL, &itemHit);
					if (dialogID == kFind_DialogID) {
						if (itemHit >= 5 and itemHit <= 7) {
							GetDialogItem (dialog, itemHit, &itemType, &itemHandle, &itemRect);
							SetControlValue (ControlRef (itemHandle), !GetControlValue (ControlRef (itemHandle)));
						}
					}
					if (dialogID == kPrefs_DialogID) {
						if (itemHit == 3) {
							GetDialogItem (dialog, itemHit, &itemType, &itemHandle, &itemRect);
							SetControlValue (ControlRef (itemHandle), !GetControlValue (ControlRef (itemHandle)));
						}
					}
				}
				while ((itemHit != ok) and (itemHit != cancel));

				if (dialogID == kFind_DialogID) {
					GetDialogItem (dialog, 4, &itemType, &itemHandle, &itemRect);
					delete[] fLastSearchText;
					fLastSearchText = NULL;
					GetDialogItemText (itemHandle, textPStr);
					fLastSearchText = new char[textPStr[0] + 1];
					memcpy (fLastSearchText, &textPStr[1], textPStr[0]);
					fLastSearchText[textPStr[0]] = '\0';

					GetDialogItem (dialog, 5, &itemType, &itemHandle, &itemRect);
					fWrapSearch = GetControlValue (ControlRef (itemHandle));

					GetDialogItem (dialog, 6, &itemType, &itemHandle, &itemRect);
					fWholeWordSearch = GetControlValue (ControlRef (itemHandle));

					GetDialogItem (dialog, 7, &itemType, &itemHandle, &itemRect);
					fCaseSensativeSearch = GetControlValue (ControlRef (itemHandle));
				}
				if ((dialogID == kPrefs_DialogID) and (itemHit == ok)) {
					GetDialogItem (dialog, 3, &itemType, &itemHandle, &itemRect);
					sTmpHackHasSBar	= GetControlValue (ControlRef (itemHandle));
				}

				DisposeDialog(dialog);
			}
			else
			{
				// Could not load About box dialog... something is amiss.
				SysBeep(2);
			}
		}
		EndUsingLibraryResources(rfRef);
		
		DoneWithDialog(frame);
	}
	else
		// If we can't get the modal focus, then another modal dialog is
		// already being displayed.
		SysBeep(2);

	return (itemHit == ok);
}


// Description:	This method should return the embedding shape that is representing
// the given embedded frame.
//
// This method should be used primarily for event handlers which get passed 
// that the user is manipulating.
Embedding*	LedPart::ShapeForFrame (ODFrame* embeddedFrame)
{
	Environment*	ev				=	SafeGetEnvironment ();
	COrdListIterator iter(GetShapeList ());
	for (Embedding* shape = (Embedding*)iter.First(); iter.IsNotComplete(); shape = (Embedding*)iter.Next()) {
		ODBoolean itsThere = shape->ContainsProxyForFrame(ev, embeddedFrame);
		if (itsThere)
			return shape;
	}
	
	return kODNULL;
}

// Description:	This method adds the given shape to our list.
void	LedPart::AddShape(Embedding* shape)
{
	Led_RequireNotNil (shape);
	Led_Require (not GetShapeList ()->Contains (shape));
	GetShapeList ()->AddFirst (shape);
	Led_Assert (GetShapeList ()->Contains (shape));
	shape->Added();
	NoteThatEmbeddingHasBeenDrawn (shape);	// even though not drawn yet, make sure it makes it onto our
											// list of shapes to check if they need to be uninternalized...
}

// Description:	This method removes the given shape from our list and temporarily
// removes it from the document ( if it is an embedded shape ).
void	LedPart::RemoveShape (Embedding* shape)
{
	Led_RequireNotNil (shape);
	Led_Require (GetShapeList ()->Contains (shape));
	GetShapeList ()->Remove(shape);
	Led_Assert (not GetShapeList ()->Contains (shape));
	shape->Removed (false);
	NoteThatEmbeddingHasNOTBeenDrawn (shape);
}

bool	LedPart::DisplayingAsFrameMode (ODFrame* frame) const
{
	Led_AssertNotNil (frame);
	return (frame->GetViewType (SafeGetEnvironment ()) == gGlobals->fFrameView);
}

void	LedPart::NoteThatEmbeddingHasBeenDrawn (Embedding* e)
{
	if (IndexOf (fEmbeddingsLoaded, e) == kBadIndex) {
		fEmbeddingsLoaded.Append (e);
	}
}

void	LedPart::NoteThatEmbeddingHasNOTBeenDrawn (Embedding* e)
{
	size_t loadedIdx	=	IndexOf (fEmbeddingsLoaded, e);
	if (loadedIdx != kBadIndex) {
		fEmbeddingsLoaded.RemoveAt (loadedIdx);
	}
}









static	void	HiliteFacet (ODDragAndDrop* dad, ODFacet* facet, ODBoolean show)
{
	Environment*	ev				=	SafeGetEnvironment ();
	CFocus forDrawing(ev, facet);
	
	ODFrame*	displayFrame = facet->GetFrame(ev);
	ODShape*	frameShape = displayFrame->AcquireFrameShape(ev, kODNULL);
	RgnHandle	bRgn = frameShape->GetQDRegion(ev);
	
	// Release acquired geometry
	ODReleaseObject(ev, frameShape);

	if (show)
		ShowDragHilite(dad->GetDragReference(ev), bRgn, true);
	else
		HideDragHilite(dad->GetDragReference(ev));
}


//--------------------------------------------------------------------
// ValidRect
//--------------------------------------------------------------------
static	ODBoolean IsValidRect( Rect& rect )
{
	return (rect.right>=rect.left && rect.bottom>=rect.top);
}



//--------------------------------------------------------------------
// CountFacets
//--------------------------------------------------------------------

static	ODUShort	CountFramesFacets (ODFrame* frame)
{
	ODUShort facetCount = 0;
	ODFacet* facet;
	TempODFrameFacetIterator ffiter (SafeGetEnvironment (), frame);
	facet = ffiter.First();
	while ( ffiter.IsNotComplete() )
	{
		facetCount++;
		facet = ffiter.Next();
	}
	return facetCount;
}


static	Rect TilePartWindow(Rect* facetBounds, Rect* partWindowBounds)
{
	const short	kWindowTilingConst	= 20;
	const short kLeftToRight		= 0;
	const short kRightToLeft		= -1;
	
	short direction;
	
	// Get the direction for the primary script system running on this machine.
	// (Right-to-Left or Left-to-Right)
	direction = GetSysDirection();
	
	// The child window should be tiled from the topLeft corner of the 
	// active facet whose frame is being opened.
	if ( direction == kLeftToRight )
	{
		// Position the window rect at the top/left corner of the facet.
		OffsetRect(partWindowBounds, facetBounds->left, facetBounds->top);
		// Now tile the window rect down and to the right.
		OffsetRect(partWindowBounds, kWindowTilingConst, kWindowTilingConst);
	}
	// The child window should be tiled from the topRight corner of the 
	// active facet whose frame is being opened.
	else if ( direction == kRightToLeft )
	{
		// Position the window rect at the top/right corner of the facet.
		OffsetRect(partWindowBounds, (partWindowBounds->right - facetBounds->right),
					facetBounds->top);
		// Now tile the window rect down and to the left.
		OffsetRect(partWindowBounds, -kWindowTilingConst, kWindowTilingConst);
	}
	
	return *partWindowBounds;
}

//--------------------------------------------------------------------
// GetEditorScriptLanguage
//--------------------------------------------------------------------

static	void GetEditorScriptLanguage(ODScriptCode* script, ODLangCode* language)
{
	typedef struct versResource {
		// Not accurrate template, just used to get at the region code.
		// (see Inside Mac: Toolbox Essentials 7-70)
		long			versionNumberStuff;
		unsigned short	regionCode;
		Str255			versionNumberString;
		Str255			versionMessageString;
	} versResource, **versResourceHdl;
	
	OSErr	error = noErr;
	long	region;
	
	ODSLong tRef;
	tRef = BeginUsingLibraryResources();
	{
		Handle versHdl = GetResource('vers', 1);
		
		// Get the region code of the editor, otherwise use the
		// region code the of the primary system script.
		
		if ( versHdl )
		{
			region = (long)(*(versResourceHdl)versHdl)->regionCode;
			ReleaseResource(versHdl);
		}
		else
		{
			region = GetScriptManagerVariable(smRegionCode);
		}
		
		// Spanish & Japanese are not actually supported by the editor.
		// They are provided as examples of how to add recognition of
		// additional regions (see Script.h for region codes).
		
		switch ((short)region) {
			case verUS:
				*script = smRoman;
				*language = langEnglish;
				break;
			case verSpain:
				*script = smRoman;
				*language = langSpanish;
				break;
			case verJapan:
				*script = smJapanese;
				*language = langJapanese;
				break;
			default:
				*script = smRoman;
				*language = langEnglish;
		}				
	}
	EndUsingLibraryResources(tRef);
}

//--------------------------------------------------------------------
// SetMenuItemText
//--------------------------------------------------------------------
static	void SetMenuItemText( Environment* ev, ODID command, ODID cmdTextID )
{
	Str63			text;
	ODIText*		menuItem = kODNULL;
	
	ODSLong tRef;
	tRef = BeginUsingLibraryResources();
	
	TRY
	
		// Load the command text from the menu strings list resource
		GetIndString(text, kMenuStringResID, cmdTextID);
		
		// Create the itext string
		menuItem = CreateIText(gGlobals->fEditorsScript, gGlobals->fEditorsLanguage, (StringPtr)&text);
		
		// Change the menu item
		if ( menuItem )
			gGlobals->fMenuBar->SetItemString(ev, command, menuItem);
			
		// Dispose the menu text, since the menu bar makes a copy
		DisposeIText(menuItem);
	
	CATCH_ALL
		Led_Assert (false);	//	SetMenuItemText Failed
	ENDTRY;
	
	EndUsingLibraryResources(tRef);

}

//--------------------------------------------------------------------
// RectContainsRect
//--------------------------------------------------------------------
static	ODBoolean RectContainsRect(Rect& rect, Rect& inside)
{
	Rect tRect = rect;
	Rect containsRect = inside;
	
	// Ensure similar co-ods
	OffsetRect(&tRect, -tRect.left, -tRect.top);
	OffsetRect(&containsRect, -containsRect.left, -containsRect.top);
	
	return (tRect.bottom <= containsRect.bottom && tRect.right < containsRect.right);
}



static	ODBoolean ValueOnClipboard(Environment *ev, ODValueType type, ODSession* session)
{
	ODBoolean		result = kODFalse;
	ODClipboard*	clipboard = session->GetClipboard(ev);

	TRY
		ODStorageUnit* su = clipboard->GetContentStorageUnit(ev);
		result = su->Exists(ev, kODPropContents, type, 0);
	CATCH_ALL
	ENDTRY

	return result;
}

//--------------------------------------------------------------------
// FixedToIntRect
//--------------------------------------------------------------------

static	void FixedToIntRect(ODRect& fixedRect, Rect& intRect)
{
	intRect.top		= FixedToInt(fixedRect.top);
	intRect.left	= FixedToInt(fixedRect.left);
	intRect.bottom	= FixedToInt(fixedRect.bottom);
	intRect.right	= FixedToInt(fixedRect.right);
}

//--------------------------------------------------------------------
// IntToFixedRect
//--------------------------------------------------------------------

static	void IntToFixedRect(Rect& intRect, ODRect& fixedRect)
{
	fixedRect.left		= ff(intRect.left);
	fixedRect.top		= ff(intRect.top);
	fixedRect.right		= ff(intRect.right);
	fixedRect.bottom	= ff(intRect.bottom);
}
