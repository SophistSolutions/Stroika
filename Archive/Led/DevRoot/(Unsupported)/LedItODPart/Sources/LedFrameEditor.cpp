/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1996.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/LedFrameEditor.cpp,v 2.17 1996/12/13 18:10:13 lewis Exp $
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
 *	$Log: LedFrameEditor.cpp,v $
 *	Revision 2.17  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.16  1996/12/05  21:12:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1996/09/30  14:45:58  lewis
 *	Add support for qSupportEnterIdleCallback and for new UpdateScrollBar (no AdjustBounds) code.
 *
 *	Revision 2.14  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.13  1996/06/01  02:29:35  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1996/05/23  20:35:50  lewis
 *	*** empty log message ***
 *
 *	Revision 2.11  1996/03/16  19:36:02  lewis
 *	No longer need for H()/V() accessors
 *
 *	Revision 2.10  1996/03/04  08:14:14  lewis
 *	Provide Led_ThrowOSErr() implementation
 *
 *	Revision 2.9  1996/02/26  23:13:28  lewis
 *	TextInteracter --> TextInteractor.
 *	Min/Max --> Led_Min/Led_Max.
 *	Fixed new multi-sbar multi-facet code to not create sbars for printer
 *	facets (crashed).
 *	And don't do erases when printing (now printing works better, but still
 *	not 100% right).
 *
 *	Revision 2.8  1996/02/05  05:09:13  lewis
 *	Lots of changes - mainly merged Led_OpenDOc code in here, and merged into Led editor subclass.
 *
 *	Revision 2.7  1996/01/22  05:54:05  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1996/01/04  00:29:46  lewis
 *	Code cleanups - mostly don't pass around Environment* stuff anymore.
 *
 *	Revision 2.5  1995/12/16  05:04:21  lewis
 *	Hack around problem where we drag, and drop within the same selection
 *	causes havoc. Use a global variable to avoid this case. Later clean this
 *	code up.
 *
 *	Revision 2.4  1995/12/15  04:12:38  lewis
 *	Massive diffs. Support for dragWithin, and StartDrag.
 *	Also, added a TypeingCommand, though it is just a quickie, primitive
 *	version. But it does allow SOME undo of typing.
 *
 *	Revision 2.3  1995/12/13  06:19:14  lewis
 *	Lots of changes, cleanups. Mainy added FrameEditor::LedWithHooks::FixupScrolledOffEmbeddings ()
 *	to fix scrolling problems with OD parts embedded (still a proto fix - needs
 *	more work).
 *	Lots of other cleanups. Added small margin at top left so easier to click
 *	before embedded parts at start of line.
 *
 *	Revision 2.2  1995/12/08  07:22:19  lewis
 *	Mainly fixed a number of bugs with the scrollbar, incluiding
 *	adding an ActivateScrollbars method, and doing the HilightCOntrol
 *	there, and not in AdjustScrollbar.
 *	Also, save/restore port/origin in scrolling callback. These caused
 *	a number of quiurky bugs in scrolling.
 *	And had wrong number arg to HiliteControl!
 *
 *	Revision 2.1  1995/12/06  02:03:11  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
 *
 *
 *
 *	<<< OLD material from Led_OpenDoc.cpp - merged in here 960202 >>>
 *	Revision 1.13  1996/01/22  05:56:08  lewis
 *	Moved DidUpdateText() support for notcing updates throught the
 *	textstore and refreshing all views into Led itself, so other class libs
 *	like MFC get same functionality.
 *
 *	Revision 1.12  1996/01/11  08:53:00  lewis
 *	New bugfixed double click word selection code. Fixes old bug with
 *	dblclick getting dropped sometimes on slow machines. And added new
 *	qDoubleClickSelectsSpaceAfterWord support (though off by default for mac).
 *
 *	Revision 1.11  1995/12/15  04:16:43  lewis
 *	Use xor to display caret, and use new functional style
 *	AsQDRect() etc, instead of member function.
 *
 *	Revision 1.10  1995/12/13  06:25:17  lewis
 *	Add deletes of some iterators that were forgotten (OOPS should have used
 *	TempIterator). And moved new caret stuff here from TextInteractor, so
 *	works better with facets.
 *
 *	Revision 1.9  1995/12/09  05:52:35  lewis
 *	Lose class SimpleLed_OpenDoc.
 *	Use PeekAtTextStore_ instead of PeekAtTextStore().
 *
 *	Revision 1.8  1995/12/08  07:27:20  lewis
 *	A number of further fixes to the UpdateWindowRect etc code, based on
 *	email/nettraffic with that susman guy at Apple. I simplified things a bit
 *	and fixed another bug which was still drawing too much (never did Validate,
 *	after Update). Also, though I don't know why, added in biasCanvas ptr.
 *
 *	Revision 1.7  1995/12/06  01:33:43  lewis
 *	Add workaround for Update () bug - qOpenDocBuggyUpdateWorkAround -
 *	caused too much redisplay.
 *	catch/reset (sDoingUpdateModeReplaceOn flag on excpetions.
 *	Now use eventavial - just returned false before - for check if any key
 *	events pending call.
 *
 *	Revision 1.6  1995/11/25  00:25:22  lewis
 *	Put in (preliminary) support for various keys, like arrow keys,
 *	and page up / down etc. Only partly there. But enuf for beta.
 *
 *	Revision 1.5  1995/11/04  23:12:56  lewis
 *	Lose class WholeTextMarker and support for it like SpecifyTextSotre
 *	override, etc. Not needed. We can just override DidUpdateText()
 *	in MarkerOwner, and get notified by the TextStore directly now.
 *
 *	Revision 1.4  1995/11/02  22:25:47  lewis
 *	Don't compute default font and set in CTOR - now taken care of
 *	by TextImager::Get??DefaultFont()
 *	StandardStyledWordWrappedLed_OpenDoc now takes styleDatabase arg.
 *	Also, careful not toall SpecifyTextStore(NULL) in CTOR.
 *
 *	Revision 1.3  1995/10/19  22:22:03  lewis
 *	Add new StandardStyledWordWrappedLed_OpenDoc.
 *
 *	Revision 1.2  1995/10/09  22:30:02  lewis
 *	Lots of changes. Mainly code cleanups. Fixed repfresh/UpdateWindowRect
 *	code to iterate over facets and frames, and do the right thing for
 *	clipping (at least much better). And now keep work grafport for
 *	AcquireTablet when no facet handy.
 *	Use chunked array text store (accidentally had left default to simple)
 *	Move Replace override to Led_OpenDoc - from SimpleLedOpenDoc - cuz not
 *	needed for old reason, only for updatemode hack and refreshing other
 *	frames.
 *	that now works well, by the way.
 *
 *
 *
 */

#include	<limits.h>


// definitely needed
#include	<BArray.h>
#include	<ISOStr.h>
#include	<StdTypIO.h>
#include	<UseRsrcM.h>

#include	<Arbitrat.xh>
#include	<Disptch.xh>
#include	<DgItmIt.xh>
#include	<Shape.xh>
#include	<StdDefs.xh>
#include	<StdProps.xh>
#include	<StdTypes.xh>
#include	<StdExts.xh>

// OpenDoc Utilities
#include	<FlipEnd.h>


// needed?
#include	<Clipbd.xh>
#include	<DragDrp.xh>
#include	<Link.xh>
#include	<LinkB.xh>
#include	<LinkSrc.xh>
#include	<LinkSrcB.xh>
#include	<LinkSpec.xh>
#include	<NamRslvr.xh>
#include	<ODAplEvt.xh>
#include	<ODDesUtl.h>
#include	<ODUtils.h>
#include	<ODOSLTkn.xh>
#include	<ODUtils.h>
#include	<Trnsform.xh>
#include <WinUtils.h>
#include <CmdDefs.xh>
#include <StorageU.xh>
#include <FocusSet.xh>
#include <Foci.xh>
#include <MenuBar.xh>
#include <Window.xh>
#include <WinStat.xh>
#include <ODSessn.xh>
#include <PlfmDef.h>
#include <FocusLib.h>
#include <StorUtil.h>
#include "ODMemory.h"
#include <BndNSUtl.h>
#include <ObjectNS.xh>
#include <ValueNS.xh>
#include <NmSpcMg.xh>
#include <Except.h>
#include <InfoUtil.h>
#include <Draft.xh>
#include <Canvas.xh>
#include <Part.xh>
#include <Facet.xh>
#include <Frame.xh>
#include <FrFaItr.xh>
#include <Info.xh>
#include <PstObj.xh>
#include "TempObj.h"
#include "TempIter.h"

#include "LedPartDef.h"


#if		qMacOS
	#include	<AppleEvents.h>
	#include	<AEObjects.h>
	#include	<AERegistry.h>
	#include	<Dialogs.h>
	#include	<Drag.h>
	#include	<Errors.h>
	#include	<Icons.h>
	#include	<Resources.h>
	#include	<ToolUtils.h>
	#include	<GXMath.h>
#endif

#include	"Embedding.h"
#include	"LedContent.h"
#include	"LedPart.h"
#include	"LedPartCommands.h"

#include	"LedFrameEditor.h"



const	Led_Coordinate	kLeftMargin	=	4;
const	Led_Coordinate	kTopMargin	=	2;



const	SBARSIZE1					=	15;







#if		qMacOS
	// From THINK Reference "Keyboard Compatability"
	const	kLeftArrowKeyCode	=	0x7B;
	const	kRightArrowKeyCode	=	0x7C;
	const	kUpArrowKeyCode		=	0x7E;
	const	kDownArrowKeyCode	=	0x7D;

	const	kKeyHomeKeyCode		=	0x73;
	const	kKeyEndKeyCode		=	0x77;
	const	kKeyPageUpKeyCode	=	0x74;
	const	kKeyPageDownKeyCode	=	0x79;

	const	kKeyClearKeyCode	=	0x47;
	const	kKeyFwdDeleteKeyCode=	0x75;
#endif



/*
 *	Since we sometimes need to use a tablet (aka hdc/canvas/grafport) when there hasn't
 *	been a facet associated with us yet, use use this temporary one. This should not
 *	be drawn into, but simply used for calculating text metrics.
 */
inline	Led_Tablet	GetTmpWorkTablet ()
	{
		#if		qMacOS
			static	CGrafPort	workPort	=	{ NULL };	// zero'd out
			if (workPort.portPixMap == NULL) {
				// initialize first time though only
				::OpenCPort (&workPort);
			}
			return (Led_Tablet (&workPort));
		#else
			#error	"Not yet implemented"
		#endif
	}













inline	Environment*	SafeGetEnvironment ()
	{
		Environment*	ev	=	::somGetGlobalEnvironment ();
		Led_AssertNotNil (ev);
		return ev;
	}






class	TypingCommand : public CCommand	{
	public:
	// -- Init --
	TypingCommand(LedPart* theEditor, FrameEditor& frameEditor):
		CCommand(theEditor,  kODTrue, kODTrue, kUndoTypingIndex, kRedoTypingIndex),
		fFrameEditor (frameEditor),
		fOldText (NULL)
		{
		}

		virtual ~TypingCommand()
			{
				delete[] fOldText;
			}
		
		// -- Inherited API --
		virtual void DoCommand()
			{	
				// Resolve the drop before we set the action history
				CCommand::DoCommand();
			}
		virtual void UndoCommand()
			{
				CCommand::UndoCommand();
				TextInteractor_&	ed			=	*fFrameEditor.GetEditor ();
				size_t	selStart	=	ed.GetSelectionStart ();
				size_t	selEnd		=	ed.GetSelectionEnd ();
				if ((selStart == selEnd) and selStart == fOldSelStart+1) {
					// simple typing case
					ed.Replace (fOldSelStart, fOldSelStart+1, fOldText, fOldTextLen);
					// since this code is still flaky, be sure selection still in range!!!
					if (fOldSelStart <= ed.GetLength () and fOldSelEnd <= ed.GetLength ()) {
						ed.SetSelection (fOldSelStart, fOldSelEnd);
					}
				}
				else {
					// ingore - we don't know how yet...
					SysBeep (1);
				}
			}
		virtual void RedoCommand()
			{
				CCommand::RedoCommand();
				// MUST DO WORK HERE!!!
				SysBeep (1);	// nyi
			}
		
		virtual void CaptureCommandState()
			{
				Led_Assert (fOldText == NULL);
				TextImager_&	ed			=	*fFrameEditor.GetEditor ();
				size_t	selStart	=	ed.GetSelectionStart ();
				size_t	selEnd		=	ed.GetSelectionEnd ();

				fOldTextLen	 = selEnd-selStart;
				fOldText = new Led_tChar [fOldTextLen];
				ed.GetTextStore ().CopyOut (selStart, fOldTextLen, fOldText);
				fOldSelStart = selStart;
				fOldSelEnd = selEnd;
			}
		virtual void Commit(ODDoneState state)
			{
			}
	
	
	protected:
		FrameEditor&	fFrameEditor;
		char*			fOldText;
		size_t			fOldTextLen;
		size_t			fOldSelStart;
		size_t			fOldSelEnd;
};




class	PerFacetInfo {
	public:
		PerFacetInfo ()
			#if		qMacOS
			:fVerticalScrollBar (NULL)
			#endif
			{
			}
	#if		qMacOS
		ControlRef	fVerticalScrollBar;
	#endif
};





/*
 ********************************************************************************
 *********************** LedSupport class lib support ***************************
 ********************************************************************************
 */

void	Led_ThrowOutOfMemoryException ()
{
	THROW (kODErrOutOfMemory);
	Led_Assert (false);	// better never reach this point!
}

void	Led_ThrowBadFormatDataException ()
{
	THROW (kODErrInvalidPersistentFormat);	// just a wild guess as to what to throw...
	Led_Assert (false);	// better never reach this point!
}

#if		qMacOS
void	Led_ThrowOSErr (OSErr err)
{
	THROW_IF_ERROR (err);
}
#endif






/*
 ********************************************************************************
 *********************************** FrameEditor ********************************
 ********************************************************************************
 */
FrameEditor::FrameEditor (LedPart* partInfo, StandardStyledTextImager::SharedStyleDatabase* styleDatabase, TextStore_* useTextStore, ODFrame* useODFrame):
	fODFrame (useODFrame),
	fHasVerticalScrollBar (useODFrame->IsRoot (::somGetGlobalEnvironment())),
	fPartInfo (partInfo),
	fPartWindow (NULL),
	fSourceFrame (NULL),
	fAttachedFrames (),
	fEditor (this, styleDatabase, useODFrame),
	fScrollBarScale (1),
	fFrameActive (false),
	fFrameReactivate (false),
	fShouldDisposeWindow (false),
	fActiveFacet (false),
	fAttachedFrame (false)
{
	Led_RequireNotNil (partInfo);
	Led_RequireNotNil (styleDatabase);
	Led_RequireNotNil (useTextStore);
	Led_RequireNotNil (useODFrame);

	fEditor.SpecifyTextStore (useTextStore);

	// should we acquire frame here?
}

FrameEditor::~FrameEditor ()
{
	ODSafeReleaseObject(fAttachedFrame);
	ODSafeReleaseObject(fSourceFrame);

	fEditor.SpecifyTextStore (NULL);

	// should we release frame here?
}

	// eventually either replace this with a stackbased object which does all this setup,
	// or find a wholly better way that takes care of all the facets and frames that
	// must be notified...
	#define	TmpHackDoneAlot()\
		Led_AssertNotNil (fPartInfo);\
		ODFacet*	activeFacet	=	fPartInfo->GetActiveFacetForFrame (fODFrame);\
		Led_AssertNotNil (activeFacet);\
		Environment*	ev = ::somGetGlobalEnvironment ();\
		Led_AssertNotNil (ev);\
		CFocus tmpTabHack (ev, activeFacet);\
		FrameEditor::LedODInteractor::TmpUseFacetObj useFacetObj (&fEditor, activeFacet);

void	FrameEditor::OnSelectAllCommand ()
{
	TmpHackDoneAlot();

	fEditor.SetSelection (qLedFirstIndex, fEditor.GetLength ()+qLedFirstIndex);
}

void	FrameEditor::FrameShapeChanged ()
{
	Environment*	ev	=	::SafeGetEnvironment ();
	Led_AssertNotNil (ev);


// NOW ADJUST WINDOWRECT, and LAYOUT WIDTH
	Led_AssertNotNil (fODFrame);
	{
		Rect		frameRect;
		{
			ODShape*	frameShape	= fODFrame->AcquireFrameShape (ev, NULL);
			Led_AssertNotNil (frameShape);
			ODRect		bbox;
			frameShape->GetBoundingBox (ev, &bbox);
			frameShape->Release (ev);
			frameRect = AsQDRect (bbox);
		}
	
	
		if (GetHasScrollBar ()) {
			frameRect.right -= SBARSIZE1;
		}
	
		Led_Assert (frameRect.right > frameRect.left);		// really shouldnt assert, but check and deal with this!!!


		// TMP HACK - see how this works??? LGP 951211
		// a little bit of margin on top/left
		frameRect.top += kTopMargin;
		frameRect.left += kLeftMargin;
	
	
	// kindof a hack - sometimes we have an active facet, and others we don't.
	// use it when we have it, and hope for the best when we don't
	// LGP 951008
		ODFacet*	activeFacet	=	fPartInfo->GetActiveFacetForFrame (fODFrame);
		if (activeFacet == NULL) {
			fEditor.SetLayoutWidth (frameRect.right-frameRect.left);
			fEditor.SetWindowRect (AsLedRect (frameRect));
		}
		else {
			CFocus tmpTabHack (ev, activeFacet);
			FrameEditor::LedODInteractor::TmpUseFacetObj useFacetObj (&fEditor, activeFacet);
			fEditor.SetLayoutWidth (frameRect.right-frameRect.left);
			fEditor.SetWindowRect (AsLedRect (frameRect));
		}
	}
	Led_AssertNotNil (fODFrame);
	fODFrame->Invalidate (ev, NULL, NULL);
	fODFrame->InvalidateActiveBorder (ev);

	RepositionScrollBar (NULL);
}

void 	FrameEditor::FacetAdded (ODFacet* facet)
{
	Environment*	ev	=	::SafeGetEnvironment ();

	PerFacetInfo*	itsFacetInfo	=	 new PerFacetInfo ();
	facet->SetPartInfo (ev, (ODInfoType)itsFacetInfo);

	Led_AssertNotNil (fODFrame);
	Led_Require (fODFrame == facet->GetFrame (ev));

	if (GetHasScrollBar () and facet->GetCanvas (ev)->IsDynamic (ev)) {
		fODFrame->Invalidate(ev, NULL, NULL);

		Rect	bogus_scrollBarRect = {0, 0, 0, 0};
		itsFacetInfo->fVerticalScrollBar = ::NewControl (facet->GetCanvas(ev)->GetQDPort(ev),
											&bogus_scrollBarRect, "\p", true, 
											0/*val*/, 0/*min*/, 0/*max*/, scrollBarProc, 0);

		RepositionScrollBar (facet);

	}
	fEditor.FixupScrolledOffEmbeddings ();
	fEditor.TabletChangedMetrics ();
}

void	FrameEditor::SetHasScrollBar (bool hasScrollBar)
{
	if (GetHasScrollBar () != hasScrollBar) {
		Environment*	ev	=	::SafeGetEnvironment ();
		TempODFrameFacetIterator iter (ev, fODFrame);
		for (ODFacet* facet = iter.First (); facet != kODNULL; facet = iter.Next ()) {
			PerFacetInfo*	itsFacetInfo	=	(PerFacetInfo*)facet->GetPartInfo (ev);
			if (hasScrollBar and facet->GetCanvas (ev)->IsDynamic (ev)) {
				Rect	bogus_scrollBarRect = {0, 0, 0, 0};
				Led_Assert (itsFacetInfo->fVerticalScrollBar == NULL);
				itsFacetInfo->fVerticalScrollBar = ::NewControl (facet->GetCanvas(ev)->GetQDPort(ev),
													&bogus_scrollBarRect, "\p", true, 
													0/*val*/, 0/*min*/, 0/*max*/, scrollBarProc, 0);
			}
			else {
				if (itsFacetInfo->fVerticalScrollBar != NULL) {
					::DisposeControl (itsFacetInfo->fVerticalScrollBar);
					itsFacetInfo->fVerticalScrollBar = NULL;
				}
			}
		}
		fHasVerticalScrollBar = hasScrollBar;
		FrameShapeChanged ();
	}
}

void 	FrameEditor::FacetRemoved (ODFacet* facet)
{
	Environment*	ev	=	::SafeGetEnvironment ();

	PerFacetInfo*	itsFacetInfo	=	(PerFacetInfo*)facet->GetPartInfo (ev);

	if (itsFacetInfo != NULL) {
		if (itsFacetInfo->fVerticalScrollBar != NULL) {
			::DisposeControl (itsFacetInfo->fVerticalScrollBar);
		}
		facet->SetPartInfo (ev, NULL);
		delete itsFacetInfo;
	}

#if 0
// MUST FIX TO SUPPORT MULTIPLE FACETS!!! - either separate sbar per facet, or soemthing like that...
	if (fVerticalScrollBar != NULL) {
		::DisposeControl (fVerticalScrollBar);
		fVerticalScrollBar = NULL;
	}
#endif
}

void	FrameEditor::DrawFacet (ODFacet* facet, ODShape* invalidShape)
{
	#if		qSupportEnterIdleCallback
		fEditor.CallEnterIdleCallback ();
	#endif

	Led_RequireNotNil (facet);

	Environment*	ev	=	::SafeGetEnvironment ();

	bool	printing	=	not facet->GetCanvas (ev)->IsDynamic (ev);	// not QUITE right answer, but should be close enuf???

	PerFacetInfo*	itsFacetInfo	=	(PerFacetInfo*)facet->GetPartInfo (ev);

	FrameEditor::LedODInteractor::TmpUseFacetObj useFacetObj (&fEditor, facet);

	if (not printing and itsFacetInfo->fVerticalScrollBar != NULL) {
		::Draw1Control (itsFacetInfo->fVerticalScrollBar);
	}

	// Warning - Led draws outside its window rect - it assumes externally clipped (perhaps it shouldn't?
	// LGP 950822
	{
		// SHOULD ALSO ERASE LEFT/TOP MARGINS???
		if (not printing) {
			Rect	wndRct;
			// top
			wndRct.top = fEditor.GetWindowRect ().top-kTopMargin;
			wndRct.left = fEditor.GetWindowRect ().left-kLeftMargin;
			wndRct.bottom = fEditor.GetWindowRect ().top;
			wndRct.right = fEditor.GetWindowRect ().right;
			::EraseRect (&wndRct);
		
			wndRct.top = fEditor.GetWindowRect ().top-kTopMargin;
			wndRct.left = fEditor.GetWindowRect ().left-kLeftMargin;
			wndRct.bottom = fEditor.GetWindowRect ().bottom;
			wndRct.right = fEditor.GetWindowRect ().left;
			::EraseRect (&wndRct);
		}

		Rect	wndRct;
		wndRct.top = fEditor.GetWindowRect ().top;
		wndRct.left = fEditor.GetWindowRect ().left;
		wndRct.bottom = fEditor.GetWindowRect ().bottom;
		wndRct.right = fEditor.GetWindowRect ().right;
	
		RgnHandle	oldClip = NULL;
		RgnHandle	newClip;
		::GetClip(oldClip = ODNewRgn ());
		::ClipRect(&wndRct);
		::GetClip(newClip = ::ODNewRgn ());
		::SectRgn(oldClip, newClip, newClip);
		::SetClip(newClip);

		ODRect	invalRect;
		invalidShape->GetBoundingBox (ev, &invalRect);
		Led_Rect	qdInvalRect	=	AsLedRect (invalRect);

		fEditor.FixupScrolledOffEmbeddings ();
		fEditor.Draw (qdInvalRect, printing);
		if (not printing and fEditor.GetCaretShown ()) {
			fEditor.DrawCaret ();
		}

		::SetClip(oldClip);
		::DisposeRgn(oldClip);
		::DisposeRgn(newClip);
	}
}

void	FrameEditor::BlinkCaretIfNeeded ()
{
	fEditor.BlinkCaretIfNeeded ();
}

void	FrameEditor::TypeChar (Led_tChar theChar, unsigned char keyCode, short theModifers)
{
// must update other facets and other frames????
	TypingCommand* command = new TypingCommand(fPartInfo, *this);
	fPartInfo->ExecuteCommand (command);


	Environment*	ev	=	::SafeGetEnvironment ();

	ODFacet*	activeFacet	=	fPartInfo->GetActiveFacetForFrame (fODFrame);

	Led_AssertNotNil (activeFacet);
	CFocus tmpTabHack (ev, activeFacet);
	FrameEditor::LedODInteractor::TmpUseFacetObj useFacetObj (&fEditor, activeFacet);
	fEditor.TypeChar (theChar, keyCode, theModifers);
}

void	FrameEditor::RepositionScrollBar (ODFacet* facet)
{
	Led_AssertNotNil (fODFrame);

	Environment*	ev	=	::SafeGetEnvironment ();
	if (facet == NULL) {
		// iterate and reposistion each facets sbars...
		TempODFrameFacetIterator iter (ev, fODFrame);
		for (ODFacet* subFacet = iter.First (); subFacet != kODNULL; subFacet = iter.Next ()) {
			RepositionScrollBar (subFacet);
		}
	}
	else {
		PerFacetInfo*	itsFacetInfo	=	(PerFacetInfo*)facet->GetPartInfo (ev);
		// Note: we must check here for itsFacetInfo!=NULL, and cannot assert cuz
		// we get back facets in our facet iterator even though a FacetAdded call has not
		// yet happened (during frameadded) - LGP 960129 - OpenDoc 1.0.
		if (itsFacetInfo != NULL and itsFacetInfo->fVerticalScrollBar != NULL) {
			Rect		frameRect;
			{
				ODShape*	frameShape	= fODFrame->AcquireFrameShape (ev, NULL);
				Led_AssertNotNil (frameShape);
				ODRect		bbox;
				frameShape->GetBoundingBox (ev, &bbox);
				frameShape->Release (ev);
				frameRect = AsQDRect (bbox);
			}
		
			Rect	scrollBarRect = frameRect;
			scrollBarRect.left = scrollBarRect.right - 15;
			scrollBarRect.right += 1;
			scrollBarRect.top -= 1;
			if (fODFrame->IsRoot (::somGetGlobalEnvironment())) {
				scrollBarRect.bottom -= 14;
			}
	
			//  a bit of trickery to avoid this move redrawing the scrollbars before I'm ready...
			UInt8 oldContrlVis = (*itsFacetInfo->fVerticalScrollBar)->contrlVis;
			(*itsFacetInfo->fVerticalScrollBar)->contrlVis = 0;
				::MoveControl (itsFacetInfo->fVerticalScrollBar, scrollBarRect.left, scrollBarRect.top);
				::SizeControl (itsFacetInfo->fVerticalScrollBar, scrollBarRect.right - scrollBarRect.left, scrollBarRect.bottom - scrollBarRect.top);
			(*itsFacetInfo->fVerticalScrollBar)->contrlVis = oldContrlVis;
		}
	}
}

void	FrameEditor::UpdateScrollBars (ODFacet* facet)
{
	Environment*	ev	=	::SafeGetEnvironment ();
	if (facet == NULL) {
		// iterate and reposistion each facets sbars...
		TempODFrameFacetIterator iter (ev, fODFrame);
		for (ODFacet* subFacet = iter.First (); subFacet != kODNULL; subFacet = iter.Next ()) {
			UpdateScrollBars (subFacet);
		}
	}
	else {
		PerFacetInfo*	itsFacetInfo	=	(PerFacetInfo*)facet->GetPartInfo (ev);
		// Note: we must check here for itsFacetInfo!=NULL, and cannot assert cuz
		// we get back facets in our facet iterator even though a FacetAdded call has not
		// yet happened (during frameadded) - LGP 960129 - OpenDoc 1.0.
		if (itsFacetInfo != NULL and itsFacetInfo->fVerticalScrollBar != NULL) {
			size_t	startOfWindow		=	fEditor.GetMarkerPositionOfStartOfWindow ();
			size_t	endOfWindow			=	fEditor.GetMarkerPositionOfEndOfWindow ();
			size_t	verticalWindowSpan	=	endOfWindow - startOfWindow;
			size_t	available			=	Led_Max (int (fEditor.GetLength ()) - int (verticalWindowSpan), 0);

			// Because mac&windows only support values for sbar up to 32K - we must scale the values...
			const	kMaxSBarValue	=	32000;	// roughly... its really a bit more...
			fScrollBarScale = available/kMaxSBarValue + 1;	// always at least factor of ONE
			Led_Assert (fScrollBarScale >= 1);
	
			available /= fScrollBarScale;
			Led_Assert (available < kMaxSBarValue);
	
			#if		qMacOS
				Led_AssertNotNil (itsFacetInfo->fVerticalScrollBar);
				::SetControlMinimum (itsFacetInfo->fVerticalScrollBar, 0);
				::SetControlMaximum (itsFacetInfo->fVerticalScrollBar, available);
			#elif	qWindows
				SetScrollRange (SB_VERT, 0, available);	
				EnableScrollBar (SB_VERT, (available!=0)? ESB_ENABLE_BOTH: ESB_DISABLE_BOTH);
			#endif
	
			Led_Assert (startOfWindow >= 1);	// 1 based Led
			startOfWindow--;				// sbar range starts at zero.
	
			startOfWindow /= fScrollBarScale;
			Led_Assert (startOfWindow < kMaxSBarValue);
			Led_Assert (startOfWindow <= available);
	
			#if		qMacOS
				::SetControlValue (itsFacetInfo->fVerticalScrollBar, startOfWindow);
			#elif	qWindows
				SetScrollPos (SB_VERT, startOfWindow);
			#endif
		}
	}
}

static	FrameEditor*	sCurrentlyTrackingThisFrame	=	NULL;
pascal	void	FrameEditor::MyScrollProc (ControlHandle /*scrollBar*/, short ctlPart)
{
	GrafPort*	savedPort	=	qd.thePort;
	short oldLeft = savedPort->portRect.left;
	short oldTop = savedPort->portRect.top;

	Led_AssertNotNil (sCurrentlyTrackingThisFrame);
	sCurrentlyTrackingThisFrame->DoVertScroll (ctlPart);
	
	::SetPort (savedPort);
	::SetOrigin (oldLeft, oldTop);
}

void	FrameEditor::AdjustCursor (Led_Point where)
{
	if (where.h > fEditor.GetWindowRect ().right) {
		// in scrollbar
		SetCursor (&ODQDGlobals.arrow);
	}
	else {
		Cursor**		iBeam		=	::GetCursor (iBeamCursor);
		unsigned char	oldState	=	::HGetState (Handle (iBeam));
		SetCursor (*iBeam);
		::HSetState (Handle (iBeam), oldState);
	}
}

void	FrameEditor::SetSelectionAndCaretShown (bool shown)
{
	fEditor.SetCaretShown (shown);
	fEditor.SetSelectionShown (shown);
}

void	FrameEditor::HandleMouseDownInFrame (ODEventData* event, ODFrame* frame, ODFacet* facet)
{
	Environment*	ev	=	::SafeGetEnvironment ();

	Led_Assert (frame == fODFrame);
	if (IsADragSelect (event, facet)) {
		HandleDragSelect (event, facet);
	}
	else {
		Point	macPoint = AsQDPoint (GlobalToLocal (facet, AsLedPoint (event->where)));

		ControlHandle	scrollBar	=	NULL;
		WindowPtr		window = facet->GetCanvas(ev)->GetQDPort(ev);
		
		ODSShort	partCode = ::FindControl (macPoint, window, &scrollBar);
		if (partCode == kControlNoPart) {
			// then not in scrollBar, so process as normal click in text

			// tmp hack - add focus object here since our acquireTablet () code doesnt work!
			// LGP 950823
			CFocus tmpTabHack (ev, facet);
			FrameEditor::LedODInteractor::TmpUseFacetObj useFacetObj (&fEditor, facet);
			// You now have the click point in fact coordinate space, so do something.
			fEditor.DoClick (Led_Point (macPoint.v, macPoint.h), event->modifiers, event->when);
		}
		else {
CFocus tmpTabHack (SafeGetEnvironment (), facet);
			HandleScrollBarClick (window, scrollBar, partCode, AsLedPoint (macPoint));
		}
	}
}

extern	bool	sOurDrag		=	false;
extern	size_t	sOurDragStart	=	0;
extern	size_t	sOurDragEnd		=	0;

bool	FrameEditor::HandleDragWithin (ODFacet* facet, Led_Point where)
{
	CFocus tmpTabHack (SafeGetEnvironment (), facet);
	FrameEditor::LedODInteractor::TmpUseFacetObj useFacetObj (&fEditor, facet);
	size_t	newPos		=	fEditor.GetCharAtClickLocation (where);

	if (sOurDrag and (newPos >= sOurDragStart and newPos <= sOurDragEnd)) {
		fEditor.SetSelection (sOurDragStart, sOurDragEnd);
		fEditor.ForceCaretShownOn ();
		fEditor.Update ();
		return true;
	}

	if (newPos >= fEditor.GetMarkerPositionOfStartOfWindow () and
		newPos <= fEditor.GetMarkerPositionOfEndOfWindow ()
		) {
		fEditor.SetSelection (newPos, newPos);
		fEditor.ForceCaretShownOn ();
		fEditor.Update ();
	}
	return true;
}

bool	FrameEditor::IsADragSelect (ODEventData* event, ODFacet* facet)
{
	/*
	 *	This seems to be what most people do. But I think this algorithm should
	 *	be embellished, so we detect that the mouse is down for a while (a half second?)
	 *	and then melt the drag into a regular selection. Also, pay attention to keys
	 *	pressed etc as a hint of intent. But immediately return yes if we are moving
	 *	what we've selected.
	 *
	 *	For now though, just do it the simple way...
	 */
	Led_Region	r	=	NULL;
	fEditor.GetSelectionWindowRegion (&r, GetSelectionStart (), GetSelectionEnd ());

	Led_Point	origClickAt		=	GlobalToLocal (facet, AsLedPoint (event->where));
	bool	result	=	::PtInRgn (AsQDPoint (origClickAt), r);
	::DisposeRgn (r); 

	if (result) {
		while (::StillDown ()) {
			Point	macNewPt;
			::GetMouse (&macNewPt);
			::LocalToGlobal (&macNewPt);
			Led_Point	newPt	=	GlobalToLocal (facet, Led_Point (macNewPt.v, macNewPt.h));
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

void	FrameEditor::HandleDragSelect (ODEventData* event, ODFacet* facet)
{
	Environment*	ev	=	SafeGetEnvironment ();

	Marker	originalSelectionMarker;
	{
		size_t	originalSelStart	=	GetSelectionStart ();
		size_t	originalSelEnd		=	GetSelectionEnd ();

		Led_Assert (not sOurDrag);
		Led_Assert (sOurDragStart == 0);
		Led_Assert (sOurDragEnd == 0);
		sOurDrag		=	true;
		sOurDragStart	=	originalSelStart;
		sOurDragEnd		=	originalSelEnd;

		// Use a marker to keep track of the original selection cuz as part of the drop, it might
		// get adjusted (like if we drop in the same frame)
		fEditor.GetTextStore ().AddMarker (&originalSelectionMarker, originalSelStart, originalSelEnd-originalSelStart, &fEditor);
	}

	SOM_TRY
		ODDropResult	result = kODDropFail;
		TempODPart		part = kODNULL;
		ODFrame*		frame = facet->GetFrame(ev);
		
		ODDragAndDrop* dragdrop = fPartInfo->GetSession ()->GetDragAndDrop(ev);
		dragdrop->Clear(ev);		// Remove existing data from the drag container.
		
		ODStorageUnit* dragdropSU = dragdrop->GetContentStorageUnit(ev);
		
		// Add the properties we need to successfully externalize
		// ourselves into the destination storage unit.
		fPartInfo->CheckAndAddProperties (dragdropSU);
		
		// Write out the part's state information.
		ODDraft* toDraft = ODGetDraft(ev, dragdropSU);
		ODDraft* fromDraft = fPartInfo->GetDraft ();
		ODDraftKey key = kODNULLKey;
	
		TRY
			// The clone type (cut or copy) should depend on whether the document
			// is read-only. (i.e. a read-only document must use kODCloneCopy -- a 
			// r/w document must use kODCloneCut if it wants to support kODDropMove).
			ODCloneKind method = (fPartInfo->IsReadOnly () ? kODCloneCopy : kODCloneCut);

			// Build clone information
			CloneInfo cloneInfo(0, fPartInfo->GetDraft (), facet->GetFrame(ev), kODCloneCut);
			cloneInfo.fFrom = fPartInfo->LookupFrameEditor (facet->GetFrame (ev)).GetSelectionStart ();
			cloneInfo.fTo =  fPartInfo->LookupFrameEditor (facet->GetFrame (ev)).GetSelectionEnd ();
			fPartInfo->fPartContent->Externalize (dragdropSU, &cloneInfo);
			
		CATCH_ALL
			// TextEditor: Clone to drag-and-drop container failed
			if (key != kODNULLKey)
				fromDraft->AbortClone(ev, key);
			RERAISE;
		ENDTRY


		Led_Region	selectionRegion	=	NULL;
		fEditor.GetSelectionWindowRegion (&selectionRegion, GetSelectionStart (), GetSelectionEnd ());

		// Move selection region into global coordinates
		{
			Point pt = {0,0};
			CFocus initiateDrawing(ev, facet);
			LocalToGlobal(&pt);
			OffsetRgn (selectionRegion, pt.h, pt.v);
		}

		RgnHandle dragRgn = ODNewRgn();
		TRY
			CopyRgn((RgnHandle) selectionRegion, dragRgn);
			InsetRgn(dragRgn, -1, -1);
			DiffRgn(dragRgn, (RgnHandle) selectionRegion, dragRgn);

			// Create Byte Arrays for the 2 dynamic parameters
			{
				TempODByteArrayStruct dragRgnBA = CreateByteArrayStruct(&dragRgn, sizeof(RgnHandle));
				TempODByteArrayStruct eventBA = CreateByteArrayStruct(&event, sizeof(ODEventData*));
				
				ODPart* returnPart;
				result = dragdrop->StartDrag(ev, frame, kODDragImageRegionHandle, dragRgnBA, &returnPart, eventBA);
				part = returnPart;		// Put it into our TempODPart
			}

			// Handle drop result.
			if ( result == kODDropMove ) {
				// If we're dropping into our own part, we'll handle the clearing
				// of the old selection in our Drop code, so don't do it here (because
				// the selection will now be the dropped data).
//				if ( !ODObjectsAreEqual(ev, fPartInfo->GetODPart (), part) ) {
					// Remove the dragged content.
					fEditor.Replace (originalSelectionMarker.GetStart (), originalSelectionMarker.GetEnd (), "", 0);
//				}
			}
			else if ( result == kODDropFail ) {
				// Restore the "old" selection if the drop failed
				SetSelection (originalSelectionMarker.GetStart (), originalSelectionMarker.GetEnd ());
			}
		CATCH_ALL
			DisposeRgn(dragRgn);
			DisposeRgn(selectionRegion);
			RERAISE;
		ENDTRY

		DisposeRgn(dragRgn);
		DisposeRgn(selectionRegion);

	SOM_CATCH_ALL
		// assuming this falls thru, then we get the RemoveMarker done below...
	SOM_ENDTRY
	fEditor.GetTextStore ().RemoveMarker (&originalSelectionMarker);
	Led_Assert (sOurDrag);
	Led_Assert (sOurDragStart != 0);
	Led_Assert (sOurDragEnd != 0);
	sOurDrag		=	false;
	sOurDragStart	=	0;
	sOurDragEnd		=	0;
}

void	FrameEditor::HandleScrollBarClick (ODPlatformWindow window, ControlHandle scrollBar, short partCode, Led_Point where)
{
	// clicked in sbar, so handle scrolling
	switch (partCode) {
		case	kControlIndicatorPart: {	// thumb
			::SetPort (window);
			if (::TrackControl (scrollBar, AsQDPoint (where), NULL) == kControlIndicatorPart) {
				// User dragged thumb, so adjust text
				size_t	newPos	=	::GetControlValue (scrollBar) * fScrollBarScale;
				newPos = Led_Max (newPos, 0);
				newPos = Led_Min (newPos, fEditor.GetLength ());
	
				/*
				 *	Be careful of special case where vUnit > 1 and so we get truncation of values,
				 *	and may not be able to get a value of the end of buffer. So we check for that
				 *	special case and correct.
				 */
				int	aMax	=	::GetControlMaximum (scrollBar);
				Led_Assert (aMax >= 0);
				if (::GetControlValue (scrollBar)  == size_t (aMax)) {
					newPos = fEditor.GetLength ();
				}
				fEditor.SetTopRowInWindowByMarkerPosition (newPos + 1, TextInteractor_::eDelayedUpdate);
			}
		}
		break;

		case	kControlUpButtonPart:
		case	kControlDownButtonPart:
		case	kControlPageUpPart:
		case	kControlPageDownPart: {
			static	ControlActionUPP scrollProcUPP = NewControlActionProc (MyScrollProc);
			Led_Assert (sCurrentlyTrackingThisFrame == NULL);
			sCurrentlyTrackingThisFrame = this;	
			(void)::TrackControl (scrollBar, AsQDPoint (where), scrollProcUPP);
			Led_Assert (sCurrentlyTrackingThisFrame == this);
			sCurrentlyTrackingThisFrame = NULL;
		}
		break;

		default: {
			// what part!
			Led_Assert (false);	// not reached
		}
		break;
	}
}

void	FrameEditor::ActivateScrollBars (bool activate)
{
	Environment*	ev	=	SafeGetEnvironment ();
	TempODFrameFacetIterator iter (ev, fODFrame);
	for (ODFacet* facet = iter.First (); facet != kODNULL; facet = iter.Next ()) {
		PerFacetInfo*	pfi	=	(PerFacetInfo*)facet->GetPartInfo (ev);
		Led_AssertNotNil (pfi);	// all should be filled in by now...
		if (pfi->fVerticalScrollBar != NULL) {
			UpdateScrollBars (facet);
			::HiliteControl (pfi->fVerticalScrollBar, activate? 0: 255);
		}
	}
}

void	FrameEditor::DoVertScroll (short whichPart)
{
	if (whichPart == kControlUpButtonPart or whichPart == kControlDownButtonPart or
		whichPart == kControlPageUpPart or whichPart == kControlPageDownPart) {
		if (not fEditor.DelaySomeForScrollBarClick ()) {
			return;
		}
	}

	switch (whichPart) {
		case	kControlNoPart:			break;
		case	kControlUpButtonPart:	fEditor.ScrollByIfRoom (-1, TextInteractor_::eImmediateUpdate);	break;
		case	kControlDownButtonPart:	fEditor.ScrollByIfRoom (1, TextInteractor_::eImmediateUpdate);	break;
		case	kControlPageUpPart:		fEditor.ScrollByIfRoom (- long (fEditor.GetTotalRowsInWindow ()), TextInteractor_::eImmediateUpdate);	break;
		case	kControlPageDownPart:	fEditor.ScrollByIfRoom (fEditor.GetTotalRowsInWindow (), TextInteractor_::eImmediateUpdate);	break;
		default:						Led_Assert (false);	// cannot think of any other parts we should support
	}
}

void	FrameEditor::ClearSelection ()
{
	fEditor.Replace (GetSelectionStart (), GetSelectionEnd (), "", 0);
}

void	FrameEditor::Externalize (ODStorageUnitView* storageUnitView)
{
	// This method assumes that OpenDoc has passed us a storageUnitView
	// that is focused to a property, but no particular value.
	
	ODStorageUnit* storageUnit = storageUnitView->GetStorageUnit (SafeGetEnvironment ());
	CleanseFrameInfoProperty(storageUnit);
	ExternalizeFrameInfo(storageUnit, 0, kODNULL);
}

void FrameEditor::CleanseFrameInfoProperty (ODStorageUnit* storageUnit)
{
	ODULong numValues;
	ODULong index;
		
	Environment*	ev	=	::SafeGetEnvironment ();

	numValues = storageUnit->CountValues(ev);
	
	for (index = numValues; index >= 1; index--)
	{
		// Index, from 1 to n, through the values.
		storageUnit->Focus(ev, kODNULL, kODPosSame, 
								kODNULL, index, kODPosUndefined);
								
		// Get the ISO type name for the value. The temp object
		// will automatically delete the returned value when this
		// scope is exited.
		TempODValueType value = storageUnit->GetType(ev);
		
		// If the value type is not one we support, remove it.
		if ( ODISOStrCompare(value, kLedPartInfo) != 0 )
			storageUnit->Remove(ev);
	}
}

void	FrameEditor::ExternalizeFrameInfo (ODStorageUnit* storageUnit, ODDraftKey key, ODFrame* scopeFrame)
{
	// This method behaves much the same way as the SamplePart::ExternalizeStateInfo
	// method.
	
	Environment*	ev	=	::SafeGetEnvironment ();

	if ( storageUnit->Exists(ev, kODNULL, kLedPartInfo, 0) )
	{
		// Persistent object references are stored in a side table, rather than
		// in the property/value stream. Thus, deleting the contents of a value
		// will not "delete" the references previously written to that value. To
		// completely "delete" all references written to the value, we must
		// remove the value and add it back.

		storageUnit->Focus(ev, kODNULL, kODPosSame, kLedPartInfo, 0 , kODPosUndefined);
		storageUnit->Remove(ev);
	}
	
	if ( fSourceFrame )
	{
		ODStorageUnitRef	weakRef;
		ODID				frameID = fSourceFrame->GetID(ev);
		ODID				scopeFrameID = ( scopeFrame ? scopeFrame->GetID(ev) : kODNULLID );
		ODDraft*			fromDraft = fSourceFrame->GetStorageUnit(ev)->GetDraft(ev);

		storageUnit->AddValue(ev, kLedPartInfo);
		
		// If a draft key exists, then we are being cloned to another draft.
		// We must "weak" clone our display frame and reference the cloned
		// frame. The part re-uses the frameID variable so there aren't two
		// different GetWeakStorageUnitRef calls.
		if ( key )
			frameID = fromDraft->WeakClone(ev, key, frameID, kODNULLID, scopeFrameID);
		
		// Write out weak references to each of the part's display frames.
		storageUnit->GetWeakStorageUnitRef(ev, frameID, weakRef);
		TRY
			StorageUnitSetValue(storageUnit, ev, sizeof(ODStorageUnitRef), (ODPtr)&weakRef);
		CATCH_ALL
		ENDTRY
	}
}

void	FrameEditor::CloneInto (ODDraftKey key, ODStorageUnitView* storageUnitView, ODFrame* scopeFrame)
{
	// This method assumes that OpenDoc has passed us a storageUnitView
	// that is focused to a property, but no particular value.
	
	ODStorageUnit* storageUnit = storageUnitView->GetStorageUnit(SafeGetEnvironment ());

	if ( storageUnit->Exists(SafeGetEnvironment (), kODNULL, kLedPartInfo, 0) == kODFalse ) {
		ExternalizeFrameInfo(storageUnit, key, scopeFrame);
	}
}

void	FrameEditor::InitFromStorage (ODStorageUnitView* storageUnitView)
{
	// This method assumes that OpenDoc has passed us a storageUnitView
	// that is focused to a property, but no particular value.
	
	Environment*	ev	=	::SafeGetEnvironment ();
	ODStorageUnit* storageUnit = storageUnitView->GetStorageUnit(ev);

	if ( storageUnit->Exists(ev, kODNULL, kLedPartInfo, 0) )
	{
		TRY
			storageUnit->Focus(ev, kODNULL, kODPosSame,
										kLedPartInfo, 0 , kODPosUndefined);
	
			ODULong size = storageUnit->GetSize(ev);
		
			// If the frame does not have a source frame, make sure the value
			// is empty.	
			if ( size > 0 )
			{
				ODStorageUnitRef weakRef;
				
				StorageUnitGetValue(storageUnit, ev, sizeof(ODStorageUnitRef),
									(ODPtr)&weakRef);
				
				if ( storageUnit->IsValidStorageUnitRef(ev, weakRef) )
				{
					ODID sourceID = storageUnit->GetIDFromStorageUnitRef(ev, weakRef);
					ODFrame* frame = storageUnit->GetDraft(ev)->AcquireFrame(ev, sourceID);
					
					fSourceFrame = frame;
				}
				else
				{
					fSourceFrame = kODNULL;
				}
			}
			
		CATCH_ALL
			fSourceFrame = kODNULL;
		ENDTRY
	}
}

void	FrameEditor::SetSourceFrame(ODFrame* sourceFrame)
{
	if ( sourceFrame != kODNULL )
	{
		Environment*	ev	=	::SafeGetEnvironment ();
		ODAcquireObject(ev, sourceFrame);
		ODReleaseObject(ev, fSourceFrame);
		fSourceFrame = sourceFrame;
	}
}

void	FrameEditor::ReleaseSourceFrame()
{
	Environment*	ev	=	::SafeGetEnvironment ();
	ODReleaseObject(ev, fSourceFrame);
}

void FrameEditor::AttachFrame (ODFrame* frame)
{
	if ( frame != kODNULL )
	{
		Environment*	ev	=	::SafeGetEnvironment ();
		ODAcquireObject(ev, frame);
		ODReleaseObject(ev, fAttachedFrame);
		fAttachedFrame = frame;
	}
}

void	FrameEditor::DetachFrame()
{
	Environment*	ev	=	::SafeGetEnvironment ();
	ODReleaseObject(ev, fAttachedFrame);
}

size_t	FrameEditor::GetSelectionStart () const
{
	return (fEditor.GetSelectionStart ());
}

size_t	FrameEditor::GetSelectionEnd () const
{
	return (fEditor.GetSelectionEnd ());
}

void	FrameEditor::SetSelection (size_t from, size_t to)
{
	fEditor.SetSelection (from, to);
}

Led_Array<Embedding*>	FrameEditor::GetEmbeddings () const
{
	return (GetEmbeddings (qLedFirstIndex, fEditor.GetLength () + qLedFirstIndex));
}

Led_Array<Embedding*>	FrameEditor::GetEmbeddings (size_t from, size_t to) const
{
	Led_Array<Embedding*>	embeddings;
	Led_Array<SimpleEmbeddedObjectStyleMarker*>	possibleEmbeddings	=	fEditor.CollectAllEmbeddingMarkersInRange (from, to);
	for (size_t i = 0; i < possibleEmbeddings.GetLength (); i++) {
		if (Embedding* e = dynamic_cast<Embedding*>(possibleEmbeddings[i+qLedFirstIndex])) {
			embeddings.Append (e);
		}
	}
	return embeddings;
}

Embedding*	FrameEditor::IsOneEmbeddedShape () const
{
	size_t	selStart	=	GetSelectionStart ();
	size_t	selEnd		=	GetSelectionEnd ();
	Led_Require (selStart <= selEnd);
	if (selEnd - selStart == 1) {
		return (IsOneEmbeddedShape (selStart, selEnd));
	}
	else {
		return NULL;
	}
}

Embedding*	FrameEditor::IsOneEmbeddedShape (size_t from, size_t to) const
{
	Led_Require (from <= to);
	if (to - from == 1) {
		Led_Array<SimpleEmbeddedObjectStyleMarker*>	embeddings	=	fEditor.CollectAllEmbeddingMarkersInRange (from, to);
		if (embeddings.GetLength () == 1) {
			return (dynamic_cast<Embedding*>(embeddings[qLedFirstIndex]));
		}
		else {
			return NULL;
		}
	}
	else {
		return NULL;
	}
}

Led_Point	FrameEditor::GlobalToLocal (ODFacet* facet, Led_Point p)
{
	Environment*	ev	=	::SafeGetEnvironment ();

	GrafPtr savedPort;
	::GetPort (&savedPort);

	ODPlatformWindow facetPort =	NULL;
	{
		TempODWindow odwnd = fODFrame->AcquireWindow (ev);
		facetPort = odwnd->GetPlatformWindow (ev);
		Led_AssertNotNil (facetPort);
	}
	::SetPort (facetPort);

	short oldLeft = savedPort->portRect.left;
	short oldTop = savedPort->portRect.top;
	::SetOrigin (0, 0);

	ODCanvas*	canvas = facet->GetCanvas (ev);

	// Get the mouse in window-local and facet-local coordinatesÉ
	Point	globalPoint	=	AsQDPoint (p);
	::GlobalToLocal (&globalPoint);

	ODPoint windowPoint	=	AsODPoint (globalPoint);
	ODPoint facetPoint = windowPoint;
	{
		TempODTransform transform = facet->AcquireWindowContentTransform (ev, canvas);
		transform->InvertPoint(ev, &facetPoint);
	}

	::SetOrigin (oldLeft, oldTop);
	::SetPort (savedPort);

	return AsLedPoint (facetPoint);
}







/*
 ********************************************************************************
 ************************* FrameEditor::LedODInteractor ****************************
 ********************************************************************************
 */
FrameEditor::LedODInteractor::LedODInteractor (FrameEditor* frameEditor, SharedStyleDatabase* styleDatabase, ODFrame* frame):
	StandardStyledWordWrappedTextInteractor (styleDatabase),
	fCurrentlyUpdatingFacet (NULL),
	fAcquireCount (0),
	fDragAnchor (0),
#if		qMacOS
	fLastDrawnOn (false),
	fTickCountAtLastBlink (0),
#endif
	fFrame (frame),
	fAcquiredTablet (NULL),
	fFrameEditor (frameEditor)
{
	Led_AssertNotNil (frame);
}

FrameEditor::LedODInteractor:: ~LedODInteractor ()
{
	Led_Assert (fAcquireCount == 0);
}

void	FrameEditor::LedODInteractor::DrawCaret ()
{
	DrawCaret_ ();
}

void	FrameEditor::LedODInteractor::InvalidateScrollBarParameters ()
{
	inherited::InvalidateScrollBarParameters ();
	UpdateScrollBars ();	// TMP HACK - avoid port-setting bug with new optimized adjustscrollbar code.
							// not worth debugging this old code base...
							// LGP 960916
	FixupScrolledOffEmbeddings ();
}

void	FrameEditor::LedODInteractor::UpdateScrollBars ()
{
	TextInteractor_::UpdateScrollBars_ ();

	Led_AssertNotNil (fFrameEditor);
	fFrameEditor->UpdateScrollBars ();
}

void	FrameEditor::LedODInteractor::FixupScrolledOffEmbeddings ()
{
	const Led_Array<Embedding*>&	embeddings	=	fFrameEditor->fPartInfo->GetOutstandingDrawnEmbeddings ();
	for (size_t i = 0; i < embeddings.GetLength (); i++) {
		Embedding* e = embeddings[i+qLedFirstIndex];
		if ((e->GetStart () < GetMarkerPositionOfStartOfWindow ()) or 
			(e->GetEnd () > GetMarkerPositionOfEndOfWindow ())
			) {
			e->Removed (false);		// ???? GUESS???
			fFrameEditor->fPartInfo->NoteThatEmbeddingHasNOTBeenDrawn (e);
		}
	}
}

void	FrameEditor::LedODInteractor::ForceCaretShownOn ()
{
	fLastDrawnOn = true;
}

void	FrameEditor::LedODInteractor::Refresh_ (UpdateMode updateMode) const
{
	if (updateMode != eNoUpdate) {
		RefreshWindowRect_ (GetWindowRect (), updateMode);
	}
}

void	FrameEditor::LedODInteractor::Refresh_ (const Led_Rect& area, UpdateMode updateMode) const
{
	// Use of this is not encouraged since it requires word-wrapping (at least in some cases)
	Refresh_ (updateMode);	// tmp hack
}

void	FrameEditor::LedODInteractor::RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const
{
	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_RequireNotNil (ev);

	switch (updateMode) {
		case	eDelayedUpdate: {
			if (not windowRectArea.IsEmpty ()) {
				Led_AssertNotNil (fFrame);
				ODFrameFacetIterator*	iter = fFrame->CreateFacetIterator (ev);
				Led_AssertNotNil (iter);
				for (ODFacet* facet = iter->First (ev); facet != kODNULL; facet = iter->Next (ev)) {
					ODShape*	updateShape = facet->CreateShape (ev);
					ODRect	odr	= AsODRect (windowRectArea);
					updateShape->SetRectangle (ev, &odr);
					facet->Invalidate (ev, updateShape, facet->GetCanvas (ev));
					updateShape->Release (ev);
				}
				delete iter;
			}
		}
		break;
		case	eImmediateUpdate: {
			/*
			 *	Not necessarily the best way todo this, but a very plausible and simple
			 *	to implement one.
			 */
			if (not windowRectArea.IsEmpty ()) {
				FrameEditor::LedODInteractor::RefreshWindowRect_ (windowRectArea, eDelayedUpdate);
				FrameEditor::LedODInteractor::UpdateWindowRect_ (windowRectArea);
			}
		}
		break;
	}
}

void	FrameEditor::LedODInteractor::Update_ () const
{
	UpdateWindowRect_ (GetWindowRect ());
}

void	FrameEditor::LedODInteractor::Update_ (const Led_Rect& /*area*/) const
{
	// Use of this is not encouraged since it requires (at least in some cases)
	// wordwrapping text...
	Led_Assert (false);	// NYI must convert to window coords!!!
}

void	FrameEditor::LedODInteractor::UpdateWindowRect_ (const Led_Rect& windowRectArea) const
{
	if (not windowRectArea.IsEmpty ()) {
		Environment*	ev	=	::somGetGlobalEnvironment ();
		Led_RequireNotNil (ev);
	
		Led_AssertNotNil (fFrame);
		ODFrameFacetIterator*	iter = fFrame->CreateFacetIterator (ev);
		Led_AssertNotNil (iter);
		for (ODFacet* facet = iter->First (ev); facet != kODNULL; facet = iter->Next (ev)) {
			TempODShape	updateShape = facet->CreateShape (ev);
			ODRect	odr	= AsODRect (windowRectArea);
			updateShape->SetRectangle (ev, &odr);
			/*
			 *	Unfortunately, there doesn't appear to be any EASY way to ask for
			 *	the update region of a window, or a facet. And we want to clip
			 *	all our drawing to that update region. So we have a bit of
			 *	work todo.
			 */
			{
				TempODShape	adjustedUpdateShape		=	updateShape->Copy (ev);

				ODWindow* window =	facet->GetWindow(ev);
				TempODShape tShape = ODCopyAndRelease (ev, facet->AcquireAggregateClipShape (ev, kODNULL));
				tShape->Intersect (ev, updateShape);
				ODCanvas* myCanvas = facet->GetCanvas(ev);
				if (!myCanvas->IsOffscreen(ev) ) {	// who cares about too many draws in offscreen? Well - its only a bug workaround!
					TempODTransform xform = facet->AcquireWindowFrameTransform (ev, kODNULL);
					tShape->Transform (ev, xform);
					GrafPtr savedPort;
					::GetPort (&savedPort);
					::SetPort (window->GetPlatformWindow(ev));
					GrafPtr curPort;
					::GetPort (&curPort);
					short oldLeft = curPort->portRect.left;
					short oldTop = curPort->portRect.top;
					SetOrigin(0, 0);

					// Make a copy of the update region, and adjust it into local coordinates
					Led_AssertNotNil (WindowPeek (curPort)->updateRgn);
					RgnHandle	updateRgn	=	ODCopyRgn (WindowPeek (curPort)->updateRgn);
					Led_AssertNotNil (updateRgn);
					{
						Point	offset;
						offset.h = 0;
						offset.v = 0;
						::GlobalToLocal (&offset);
						::OffsetRgn (updateRgn, offset.h, offset.v);
					}

					// now intersect adjustedUpdateShape with real update region. Update inverse back overwriting updateshape
					TempODShape	updateRgnShape = facet->CreateShape (ev);
					updateRgnShape->SetQDRegion (ev, updateRgn);
					updateRgnShape->InverseTransform (ev, xform);
					adjustedUpdateShape->Intersect (ev, updateRgnShape);

					SetOrigin(oldLeft, oldTop);
					SetPort(savedPort);
				}
				if (not adjustedUpdateShape->IsEmpty (ev)) {
					facet->Update (ev, adjustedUpdateShape, facet->GetCanvas (ev));
					facet->Validate (ev, adjustedUpdateShape, facet->GetCanvas (ev));
				}
			}
		}
		delete iter;
	}
}

ODFacet*	FrameEditor::LedODInteractor::TemporarilyUseFacet (ODFacet* facet)
{
	Led_Require (fAcquireCount == 0);	// cannot change facet while tablet acquired!!!
	ODFacet*	old	=	fCurrentlyUpdatingFacet;
	fCurrentlyUpdatingFacet = facet;
	return old;
}

Led_Tablet	FrameEditor::LedODInteractor::AcquireTablet () const
{
	const_cast<FrameEditor::LedODInteractor*>(this)->fAcquireCount++;
	Led_Assert (fAcquireCount < 100);	// not really a requirement - but hard to see how this could happen in LEGIT usage...
										// almost certainly a bug...

	if (fCurrentlyUpdatingFacet == NULL) {
		return (GetTmpWorkTablet ());
	}
	else {
		Environment*	ev	=	::somGetGlobalEnvironment ();
		Led_RequireNotNil (ev);
	
		Led_RequireNotNil (fCurrentlyUpdatingFacet);// we require this always be set via a
													// TemporarilyUseFacet () call before
													// an aquireTablet call happens
		Led_RequireNotNil (fCurrentlyUpdatingFacet->GetCanvas (ev));
		Led_RequireNotNil (fCurrentlyUpdatingFacet->GetCanvas (ev)->GetQDPort (ev));
	
		return (fCurrentlyUpdatingFacet->GetCanvas (ev)->GetQDPort (ev));
	}
}

void	FrameEditor::LedODInteractor::ReleaseTablet (Led_Tablet /*tablet*/) const
{
	Led_Assert (fAcquireCount > 0);
	const_cast<FrameEditor::LedODInteractor*>(this)->fAcquireCount--;
}

bool	FrameEditor::LedODInteractor::QueryInputKeyStrokesPending () const
{
	#if		qMacOS
		EventRecord	eventRecord;
		return (::OSEventAvail (keyDownMask, &eventRecord));
	#endif
}

void	FrameEditor::LedODInteractor::TypeChar (Led_tChar theChar, unsigned char keyCode, short theModifers)
{
	#if		qMacOS
		// Bob Swerdlow says you are supposed todo this - seems silly to me... LGP 950212...
		::ObscureCursor ();
	#endif

	Led_Assert (GetSelectionEnd () <= GetLength () + qLedFirstIndex);

	/*
	 *	Behavior here as listed in Mac. Human Interface Guidelines - See Led SPR#0122
	 */
	bool	optionPressed	=	!!(theModifers&optionKey);
	bool	shiftPressed	=	!!(theModifers&shiftKey);
	bool	commandPressed	=	!!(theModifers&cmdKey);


	switch (keyCode) {
		case kLeftArrowKeyCode: {
			CursorMovementDirection	dir		=	commandPressed? eCursorToStart: eCursorBack;
			CursorMovementUnit		unit	=	optionPressed? eCursorByWord: eCursorByChar;
			if (commandPressed) {
				unit = eCursorByRow;	// NB: ambiguity in case both optionPressed AND commandPressed
			}
			CursorMovementAction	action	=	shiftPressed? eCursorExtendingSelection: eCursorMoving;

			DoSingleCharCursorEdit (dir, unit, action, qPeekForMoreCharsOnUserTyping? eDelayedUpdate: eImmediateUpdate);
			#if		qPeekForMoreCharsOnUserTyping
				UpdateIfNoKeysPending ();
			#endif
		}
		break;

		case kRightArrowKeyCode: {
			CursorMovementDirection	dir		=	commandPressed? eCursorToEnd: eCursorForward;
			CursorMovementUnit		unit	=	optionPressed? eCursorByWord: eCursorByChar;
			if (commandPressed) {
				unit = eCursorByRow;	// NB: ambiguity in case both optionPressed AND commandPressed
			}
			CursorMovementAction	action	=	shiftPressed? eCursorExtendingSelection: eCursorMoving;

			DoSingleCharCursorEdit (dir, unit, action, qPeekForMoreCharsOnUserTyping? eDelayedUpdate: eImmediateUpdate);
			#if		qPeekForMoreCharsOnUserTyping
				UpdateIfNoKeysPending ();
			#endif
		}
		break;

		case kUpArrowKeyCode: {
			CursorMovementDirection	dir		=	(commandPressed or optionPressed)? eCursorToStart: eCursorBack;
			CursorMovementUnit		unit	=	optionPressed? eCursorByLine: eCursorByRow;
			if (commandPressed) {
				unit = eCursorByWindow;	// NB: ambiguity in case both optionPressed AND commandPressed
			}
			CursorMovementAction	action	=	shiftPressed? eCursorExtendingSelection: eCursorMoving;

			DoSingleCharCursorEdit (dir, unit, action, qPeekForMoreCharsOnUserTyping? eDelayedUpdate: eImmediateUpdate);
			#if		qPeekForMoreCharsOnUserTyping
				UpdateIfNoKeysPending ();
			#endif
		}
		break;

		case kDownArrowKeyCode: {
			CursorMovementDirection	dir		=	(commandPressed or optionPressed)? eCursorToEnd: eCursorForward;
			CursorMovementUnit		unit	=	optionPressed? eCursorByLine: eCursorByRow;
			if (commandPressed) {
				unit = eCursorByWindow;	// NB: ambiguity in case both optionPressed AND commandPressed
			}
			CursorMovementAction	action	=	shiftPressed? eCursorExtendingSelection: eCursorMoving;

			DoSingleCharCursorEdit (dir, unit, action, qPeekForMoreCharsOnUserTyping? eDelayedUpdate: eImmediateUpdate);
			#if		qPeekForMoreCharsOnUserTyping
				UpdateIfNoKeysPending ();
			#endif
		}
		break;

		case kKeyHomeKeyCode: {
			DoSingleCharCursorEdit (eCursorToStart, commandPressed? eCursorByBuffer: eCursorByRow, shiftPressed? eCursorExtendingSelection: eCursorMoving,
					qPeekForMoreCharsOnUserTyping? eDelayedUpdate: eImmediateUpdate
				);
		}
		break;

		case kKeyEndKeyCode: {
			DoSingleCharCursorEdit (eCursorToEnd, commandPressed? eCursorByBuffer: eCursorByRow, shiftPressed? eCursorExtendingSelection: eCursorMoving,
					qPeekForMoreCharsOnUserTyping? eDelayedUpdate: eImmediateUpdate
				);
		}
		break;

		case kKeyPageUpKeyCode: {
			ScrollByIfRoom (-(int)GetTotalRowsInWindow ());
		}
		break;

		case kKeyPageDownKeyCode: {
			ScrollByIfRoom (GetTotalRowsInWindow ());
		}
		break;

		case kKeyClearKeyCode: {
			Led_Assert (false);	// nyi
		}
		break;

		case kKeyFwdDeleteKeyCode: {
			Led_Assert (false);	// nyi
		}
		break;

		default: {
			if (theChar == '\r') {
				theChar = '\n';
			}
			OnTypedNormalCharacter (theChar);
			return;
		}
		break;
	}
}
static	int	gClicks	=	1;	// tmp hack!!!
void	FrameEditor::LedODInteractor::DoClick (Led_Point hitPt, short modifierKeys, long when)
{
// quick hack at dbl click detection...
static	long	sLastClickAt	=	0;

if (when-sLastClickAt <= GetDblTime ()) {
	gClicks++;
}
else {
	gClicks = 1;
}
sLastClickAt = when;

	switch (gClicks) {
		case	1: {
			bool	extendSelection	=	bool (modifierKeys & shiftKey);
			size_t	newPos			=	GetCharAtClickLocation (hitPt);
			size_t	newSelStart		=	newPos;
			size_t	newSelEnd		=	newPos;
			if (extendSelection) {
				newSelStart = Led_Min (newSelStart, GetSelectionStart ());
				newSelEnd = Led_Max (newSelEnd, GetSelectionEnd ());
			}

			// Set flag for how to display caret based on where we clicked
			if (not extendSelection) {
				SetCaretShownAfterPos (GetCharWindowLocation (newPos).top <= hitPt.v);
			}


			SetSelection (newSelStart, newSelEnd);

			/*
			 *	Note that we do this just AFTER setting the first selection. This is to avoid
			 *	(if we had done it earlier) displaying the OLD selection and then quickly
			 *	erasing it (flicker). We do an Update () rather than simply use eImmediateUpdate
			 *	mode on the SetSelection () because at this point we want to redisplay the entire
			 *	window if part of it needed it beyond just the part within the selection.
			 */
			Update ();

			fDragAnchor = (newPos > newSelStart)? newSelStart: newSelEnd;
//			DrawCaret (true);

			while (::StillDown ()) {
				Point	macNewPt;
				::GetMouse (&macNewPt);
				Led_Point	newPt	=	Led_Point (macNewPt.v, macNewPt.h);
				size_t	rhsPos	=	GetCharAtClickLocation (newPt);

				/*
 				 *	Handle autoscrolling, if necessary.
				 */
				if (rhsPos < GetMarkerPositionOfStartOfWindow ()) {
					ScrollByIfRoom (-1);
					rhsPos = GetMarkerPositionOfStartOfWindow ();
				}
				else if (rhsPos > GetMarkerPositionOfEndOfWindow ()) {
					ScrollByIfRoom (1);
					rhsPos = FindNextCharacter (GetMarkerPositionOfEndOfWindow ());		// So we select past to end of window
				}

				/*
				 *	Update the selection.
				 */
				if (rhsPos > fDragAnchor) {
					SetSelection (fDragAnchor, rhsPos, eImmediateUpdate);
				}
				else {
					SetSelection (rhsPos, fDragAnchor, eImmediateUpdate);
				}
			}
		}
		break;

		case	2: {
			size_t	wordStart	=	0;
			size_t	wordEnd		=	0;
			bool	wordReal	=	false;
			GetTextStore ().FindWordBreaks (fDragAnchor, &wordStart, &wordEnd, &wordReal);
			fDragAnchor = Led_Min (fDragAnchor, wordStart);
			#if		qDoubleClickSelectsSpaceAfterWord
				if (wordReal) {
					// select the space forward...
					size_t	xWordStart	=	0;
					size_t	xWordEnd	=	0;
					bool	xWordReal	=	false;
					GetTextStore ().FindWordBreaks (wordEnd, &xWordStart, &xWordEnd, &xWordReal);
					if (not xWordReal) {
						wordEnd = xWordEnd;
					}
				}
				else {
					// select the WORD behind...
					wordStart = GetTextStore ().FindFirstWordStartStrictlyBeforePosition (wordStart);
				}
			#endif
			SetSelection (wordStart, wordEnd, eImmediateUpdate);
			while (::StillDown ()) {
				Point	macNewPt;
				::GetMouse (&macNewPt);
				Led_Point	newPt	=	Led_Point (macNewPt.v, macNewPt.h);
				size_t	rhsPos	=	GetCharAtClickLocation (newPt);
		
				/*
 				 *	Handle autoscrolling, if necessary.
				 */
				if (rhsPos < GetMarkerPositionOfStartOfWindow ()) {
					ScrollByIfRoom (-1);
					rhsPos = GetMarkerPositionOfStartOfWindow ();
				}
				else if (rhsPos > GetMarkerPositionOfEndOfWindow ()) {
					ScrollByIfRoom (1);
					rhsPos = FindNextCharacter (GetMarkerPositionOfEndOfWindow ());		// So we select past to end of window
				}

				/*
				 *	Update the selection.
				 */
				if (rhsPos > fDragAnchor) {
					#if		qDoubleClickSelectsSpaceAfterWord	
						SetSelection (wordStart, GetTextStore ().FindFirstWordStartAfterPosition (rhsPos), eImmediateUpdate);
					#else
						SetSelection (fDragAnchor, GetTextStore ().FindFirstWordEndAfterPosition (rhsPos, false), eImmediateUpdate);
					#endif
				}
				else {
					SetSelection (GetTextStore ().FindFirstWordStartStrictlyBeforePosition (rhsPos, qDoubleClickSelectsSpaceAfterWord), wordEnd, eImmediateUpdate);
				}
			}
		}
		break;

		case	3: {
			SelectWholeLineAfter (GetSelectionStart (), false, eImmediateUpdate);
		}
		break;
	}




#if 0
	// Borrowed more or less verbatim from the end of TCL CEditText::DoClick()
	{
		// CSwitchboard will never see the mouse up that ended
		// the drag, so we stuff gLastMouseUp here to allow
		// multi-click counting to work.
		gLastMouseUp.what = mouseUp;
		gLastMouseUp.when = ::TickCount ();
		gLastMouseUp.where = hitPt;
		::LocalToGlobal(&gLastMouseUp.where);
		gLastMouseUp.modifiers = modifierKeys;

		SelectionChanged ();

		//
		// For non-editable but selectable text, we want to display
		// a selection range so the user can copy, but we don't
		// want to display a caret. In that case, we resign the
		// gopher to our supervisor, which will deactivate this text
		if (!editable && (gGopher == this)) {
			long selStart, selEnd;
			GetSelection(&selStart, &selEnd);
			if (selStart == selEnd) {
				itsSupervisor->BecomeGopher (TRUE);
			}
		}
	}
#endif
}

#if		qMacOS
void	FrameEditor::LedODInteractor::BlinkCaretIfNeeded ()
{
	if (GetCaretShown ()) {
		long	now	=	::TickCount ();
		if (now > fTickCountAtLastBlink + GetTickCountBetweenBlinks ()) {
//			DrawCaret (not fLastDrawnOn);

Led_Rect	cr	=	CalculateCaretRect ();
RefreshWindowRect (cr);

fLastDrawnOn = not fLastDrawnOn;
			fTickCountAtLastBlink = now;
		}
	}
}
#endif

#if		qMacOS
void	FrameEditor::LedODInteractor::DrawCaret_ ()
{
	DrawCaret_ (fLastDrawnOn);
}
#endif

#if		qMacOS
void	FrameEditor::LedODInteractor::DrawCaret_ (bool on)
{
	Tablet_Acquirer	tablet (this);
	Led_AssertNotNil (tablet);
	Led_Assert (tablet == qd.thePort);
	// Note: the reason we don't just use the GetCaretShown () flag here, and
	// instead we check that the selection is empty is because we want to reserve that
	// flag for users (client programmers) use. If we used it internally (like in SetSelection()
	// then the users values would get overwritten...
	Led_Assert (GetCaretShown ());
	if (GetSelectionStart () == GetSelectionEnd ()) {
		PenNormal ();
		PenPat(&ODQDGlobals.black);
		PenMode(patXor);
		Led_Rect	caretRect	=	CalculateCaretRect ();
		fLastDrawnOn = on;
		Rect	qdCaretRect	=	AsQDRect (caretRect);
		if (on) {
			::FillRect (&qdCaretRect, &qd.black);
		}
		else {
			::EraseRect (&qdCaretRect);
		}
		PenNormal ();
	}
}
#endif




