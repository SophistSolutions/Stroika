/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextEdit.cc,v 1.10 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 *		-	Consider getting rid of the AutoSize stuff, and replace it with a CalcViewRect () method - then
 *			use that from TextChanged Callback in TextScroller.
 *
 *		-	Whole thing needs major reworking in design, and for mac, in tablet usage....<PARTLY DONE-GOT RID OF SETORIGIN>
 *
 *		-	Views definition of GetVisibleArea() including adorments in somewhat confusing. In fact, I think
 *			the mac code here is confused - I think it is being treated as if it didnt contain the adornemnts.
 *			Talk over with sterl, and try to cleanup.
 *
 *
 *
 * Changes:
 *	$Log: TextEdit.cc,v $
 *		Revision 1.10  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.9  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.8  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.7  1992/07/16  17:55:47  lewis
 *		In prepare for draw, reset os background under mac os.
 *
 *		Revision 1.6  1992/07/16  17:10:28  sterling
 *		changed GUI to UI, added MacUI_Portable and MotifUI_Portabel
 *
 *		Revision 1.3  1992/07/03  02:18:32  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *		Also a few others - Justification, ClipBoard, etc...
 *
 *		Revision 1.2  1992/06/25  08:48:40  sterling
 *		Lots of changes including renamed CalcDefaultSize to CalcDefaultSize_, border changes, starting
 *		trying to fix (with lgp) the erase of white with other backgrounds - really getting te to use
 *		right background - still needs work.
 *
 *		Revision 1.81  1992/05/21  17:32:10  lewis
 *		React to change in definition of SubString.
 *
 *		Revision 1.80  92/05/20  00:25:20  00:25:20  sterling (Sterling Wight)
 *		character validation for Motif
 *		
 *		Revision 1.79  92/05/19  10:39:46  10:39:46  sterling (Sterling Wight)
 *		tweaked mac AdjustSize stuff
 *		
 *		Revision 1.78  92/05/13  12:47:09  12:47:09  lewis (Lewis Pringle)
 *		STERL: Override SetText() to take char* arg. And, futz with TextEdit_MacUI_Native::ResetEffectiveStyle () in case
 *		were using AutoSize () mode???
 *		
 *		Revision 1.77  92/05/12  23:41:38  23:41:38  lewis (Lewis Pringle)
 *		Get rid of View::EffectiveLiveChanged () call in AbstractEditText::EfectiveLiveChanged() since we should have no subviews -
 *		if some subclass does have subviews - up to them to pass along...
 *		
 *		Revision 1.76  92/04/20  14:35:07  14:35:07  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		Also, use )Min (GetBorderHeight (), GetBorderWidth ()) instead of Bordered::CalcShadowThickness () since that
 *		rouitine went away - See rcslog in Bordered.hh.
 *		
 *		Revision 1.74  92/04/17  13:34:46  13:34:46  lewis (Lewis Pringle)
 *		Set a few extra params like marginwidth, hgiht, etc for drawing area widget in hack for motif.
 *		Didnt help a bit.
 *		Also, call EncosureToLocal after call to GetVisibleArea().
 *		
 *		Revision 1.73  92/04/17  01:21:22  01:21:22  lewis (Lewis Pringle)
 *		Fixed mac code to call LocalToTablet/tabletToLocal on TeGetOffset/TEGetPoint calls.
 *		Added to motif stuff a wrapper widget and a qUseWrapper flag local to the
 *		file to turn on / off the facility. Semes to fix some bugs and create others - par for the course.
 *		In motif textedit::SetText (both overloads) must call PlusChangCount so tet comes up autosized. Must cleanup this code- that is aweful.
 *		Get rid of whose autosizing thing - could be done in TextChagned callback of TextScrollers / TextControllers.
 *		
 *		Revision 1.72  92/04/14  19:41:33  19:41:33  lewis (Lewis Pringle)
 *		Add TextEdit::~TextEdit () ifdefed for qSoleInlineVirtualsNotStripped.
 *		
 *		Revision 1.71  92/04/14  12:33:21  12:33:21  lewis (Lewis Pringle)
 *		Reorganize so class declareations go at top of file, and implemtations (even of little helper classes)
 *		go at the bottom.
 *		
 *		Do the SetFont (&kApplicationFont) in each GUI specific TE class - not always true regardless of GUI.
 *		In fact, we probably want to change this for Motif.
 *		
 *		In Motif - give name to widget - primarily useful for debugging.
 *		
 *		Renamed TextEdit_MacUI_Native::ClickLoopProc () to TextEdit_MacUI_Native::sClickLoopProc ().
 *		
 *		Reimplement void	TextEdit_MacUI_Native::PrepareForDraw (UpdateMode updateMode) to not
 *		use quickdraw SetOrigin stuff. Do all of our work in Tablet coordinates. Also, needed to
 *		update TextEdit_MacUI_Native::PTrackPress code to make this work..
 *		
 *		
 *		Revision 1.70  92/04/09  02:25:52  02:25:52  lewis (Lewis Pringle)
 *		Got rid of EffectiveFontChanged override for motif textedit since taken care of by motif os controls.
 *		Also, verified/commented on problem with motif textedit justificiation.
 *		
 *		Revision 1.69  92/04/08  15:39:03  15:39:03  lewis (Lewis Pringle)
 *		On SetActivate on mac TE only inval local region inset by border, so we dont
 *		redraw boundary. Also, add override of TabbingFocus for mac so that we get teactivate/deactivate called rather than invals.
 *		Should make tabbing more flicker free on mac but this is not tested yet.
 *		
 *		Revision 1.68  92/04/06  15:27:43  15:27:43  sterling (Sterling Wight)
 *		minor tweak to Mac CalcDefaultSize
 *		
 *		Revision 1.67  92/04/02  13:07:37  13:07:37  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.66  92/04/02  11:43:09  11:43:09  lewis (Lewis Pringle)
 *		Comment out workaround to bug with Focus stuff - we used to call TextWidth () to force a reset of
 *		the current focused view -= hack city...
 *		
 *		Revision 1.65  92/03/26  15:15:20  15:15:20  lewis (Lewis Pringle)
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of oldFocused first argument.
 *		
 *		Revision 1.64  1992/03/26  09:59:36  lewis
 *		Get rid of oldLive arg to EffectiveLiveChanged(), and Add newFont/UpdateMode args to EffectiveFontChanged,
 *		and then do some of the stuff that used to be done by View - inval layouts, and refresh().
 *
 *		Revision 1.62  1992/03/22  21:52:15  lewis
 *		Documented case of #if		qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG
 *		Also, made out-of-line several inlines that were too big - mosyly op== for TextRun, etc...
 *
 *		Revision 1.61  1992/03/17  17:09:37  sterling
 *		Add some black magic to motif calcdefaultsize to get better result in emily - temp hack.
 *
 *		Revision 1.60  1992/03/17  03:08:46  lewis
 *		Make motif TE commands NOT undoable, since they are currently broken. Did quickie implementation of AutoSize (), and
 *		slightly improved CalcDefaultSize for motif. Implemented GetLineCount () - badly - but acceptably.
 *
 *		Revision 1.58  1992/03/11  08:10:07  lewis
 *		In TextEdit_MotifUI_Native::SetTextActive () call ::XmProcessTraversal to keep motif focus
 *		in sync with ours.
 *
 *		Revision 1.57  1992/03/10  13:04:48  lewis
 *		Add override of TextEdit_MotifUI_DispatchKeyEvent () to get dispatch to go to oscontrol.
 *		Going thru command, and other cruft didnt seem to work right??? Not sure why we ever
 *		did that. Needs more consideration.
 *
 *		Revision 1.56  1992/03/10  00:34:33  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *		Also, change interface to ValidateKey () to take KeyStroke instead of Character, and KeyBoard.
 *		Use new DispatchKeyEvent () interface instead of old HandleKey () interface.
 *
 *		Revision 1.53  1992/03/06  22:27:03  lewis
 *		Got rid of a few empty dtors, and made a few ctors/dtors inline.
 *		Implemented TextEdit as a class, rather than a #define.
 *		Made AbstractTextEdit inherit directly from FocusItem, and EnableItem instead of
 *		FocusView, since I believe its VERY bad style to pervert the inheritance hierarchy to save a
 *		tiny bit of implemenation code, and as it turns out (not surprisingly) it ended up saving
 *		several overrides - saving code!
 *		Add CanBeFocused() to conjunction in TrackPress for mac and motif. Not sure right, but if not, document why????
 *
 *		Revision 1.48  1992/03/05  21:16:01  sterling
 *		support for borders, also maxLength and SingleLine editing
 *
 *		Revision 1.47  1992/02/28  15:22:54  lewis
 *		Added TextWidth () hack to TextEdit_MacUI_Native::PrepareForDraw () to work around bug
 *		reported by Bob Hogan. Trouble is we really must redo View Focus support soon, then
 *		this class of hacks goes away...
 *
 *		Revision 1.45  1992/02/19  22:20:18  sterling
 *		better default size for Motif
 *
 *		Revision 1.44  1992/02/19  17:22:07  sterling
 *		included SeskTop for Macintosh
 *
 *		Revision 1.41  1992/02/15  05:35:59  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.39  1992/02/04  22:07:29  sterling
 *		still working on Motif implementation.
 *
 *		Revision 1.38  1992/02/04  06:53:45  sterling
 *		hacked at Motif code
 *
 *		Revision 1.37  1992/02/03  16:38:48  sterling
 *		made CalcDefaultSize on Mac resold state
 *
 *		Revision 1.36  1992/01/31  16:57:26  sterling
 *		GrabFocus used
 *		added EffectiveFontChanged to Mac dode
 *
 *		Revision 1.35  1992/01/31  05:37:08  lewis
 *		Got rid of unused global.
 *
 *		Revision 1.34  1992/01/29  04:49:52  sterling
 *		fixed Mac problems
 *
 *		Revision 1.31  1992/01/27  04:08:05  sterling
 *		made compile on Mac
 *
 *		Revision 1.30  1992/01/24  23:41:34  lewis
 *		Set sensative in realize method (this is still broken - fix soon).
 *
 *		Revision 1.29  1992/01/24  17:55:12  lewis
 *		Add Realize/UnRealize methdos to update values when they were set while we were unrealized.
 *
 *		Revision 1.28  1992/01/23  08:13:27  lewis
 *		MotifOSControls no longer keeps track of sensative flag while were unrealized.
 *
 *		Revision 1.26  1992/01/22  15:18:29  sterling
 *		major changes, merged with EditText
 *
 *		Revision 1.24  1992/01/20  12:24:18  lewis
 *		Added justification support for motif.
 *
 *		Revision 1.21  1992/01/16  01:01:41  lewis
 *		Made compile on mac.
 *
 *		Revision 1.18  1992/01/10  08:58:22  lewis
 *		Added SetSensative call in TextEdit for motif SetActive native. Also, Fixed Motif native TE to call inherited
 *		on mouse press - was cutting off execution.
 *
 *		Revision 1.17  1992/01/10  03:35:21  lewis
 *		Got rid of Layout override - didn't seem to be helping anyhow.
 *		Revision 1.15  1992/01/08  22:27:10  lewis
 *		Ported to mac.
 *
 *		Revision 1.14  1992/01/08  06:41:14  lewis
 *		Sterl - merged his mac te bug fixes.
 *
 *		Revision 1.13  1992/01/06  10:07:35  lewis
 *		Ignore call to AdjustSize () for now - dont abort. Also, did cut at TEKey () for
 *		Xt, but it didn't seem to work - will need tooinvestigate...
 *
 *		Revision 1.11  1991/12/27  17:01:49  lewis
 *		Return slightly more reasonable value for TextEdit_MotifUI_Native::CalcDefautltSize ().
 *
 *
 */


#include	<string.h>
#include	<ctype.h>

#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Events.h>
#include	<Memory.h>
#include	<QuickDraw.h>
#include	<TextEdit.h>
#elif 	qXmToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/DrawingA.h>
#include	<Xm/Text.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Application.hh"
#include	"ClipBoard.hh"
#include	"DeskTop.hh"
#include	"OSControls.hh"
#include	"MenuOwner.hh"

#include	"TextEdit.hh"









class	TECommand : public Command {
	public:
		TECommand (AbstractTextEdit& te, CommandNumber cmdNumber);
		override	void	DoIt ();
		override	void	UnDoIt ();
		virtual		void	DoTextEdit () = Nil;
		
	protected:
		AbstractTextEdit&	fTextEdit;
		String				fOldSelectedText;
		TextSelection		fSelection;
		TextSelection		fNewTextSelection;
		UInt32				fOldChangeCount;
};


class	TEKeyCommand : public TECommand {
	public:
		TEKeyCommand (AbstractTextEdit& te, Character c);

		override	void	DoTextEdit ();

		override	void	UnDoIt ();

		nonvirtual	void	SetCommitsLast (Boolean commits);
		override	Boolean	CommitsLastCommand () const;

	private:
		Character	fCharacter;
		Boolean		fCommitsLastCommand;
		Boolean		fDidUndo;
};

class	TEClipCommand : public TECommand {
	public:
		TEClipCommand (AbstractTextEdit& te, CommandNumber cmdNumber);

		override	void	DoTextEdit ();
};








#if		qMacGDI
static	osGrafPort*	GetWorkPort ()
{
	static	osGrafPort*	sPort = Nil;
	
	if (sPort == Nil) {
		Tablet*	t = new Tablet (Point (1, 1), 1);
		sPort = t->GetOSGrafPtr ();
	}
	EnsureNotNil (sPort);
	return (sPort);
}
#endif	/*qMacGDI*/









/*
 ********************************************************************************
 ********************************** TextEditBase ********************************
 ********************************************************************************
 */
void	TextEditBase::SetController (TextController* controller)
{
	Require ((controller == Nil) or (fController == Nil));
	
	fController = controller;
	
	Ensure (fController == controller);
}






/*
 ********************************************************************************
 ******************************* TextController *********************************
 ********************************************************************************
 */
TextController::TextController (TextController* controller):
	TextEditBase (controller)
{
}
		
void	TextController::TextChanged (TextEditBase* text)
{
	MenuOwner::SetMenusOutOfDate ();
	if (GetController () != Nil) {
		GetController ()->TextChanged (text);
	}
}

void	TextController::TextTracking (const Point& mouseAt)
{
	if (GetController () != Nil) {
		GetController ()->TextTracking (mouseAt);
	}
}






/*
 ********************************************************************************
 ********************************* TextSelection ********************************
 ********************************************************************************
 */
TextSelection::TextSelection (CollectionSize from, CollectionSize to):
	fFrom (from),
	fTo (to)
{
}

TextSelection::TextSelection (const TextSelection& from):
	fFrom (from.fFrom),
	fTo (from.fTo)
{
}

Boolean	operator== (const TextSelection& lhs, const TextSelection& rhs)
{
	return Boolean ((lhs.GetFrom () == rhs.GetFrom ()) and (lhs.GetTo () == rhs.GetTo ()));
}

Boolean	operator!= (const TextSelection& lhs, const TextSelection& rhs)
{
	return Boolean ((lhs.GetFrom () != rhs.GetFrom ()) or (lhs.GetTo () != rhs.GetTo ()));
}






/*
 ********************************************************************************
 *********************************** TextStyle **********************************
 ********************************************************************************
 */
TextStyle::TextStyle (const Font& font, const Color& color):
	fFont (font),
	fColor (color)
{
}

TextStyle::TextStyle (const TextStyle& from):
	fFont (from.GetFont ()),
	fColor (from.GetColor ())
{
}


Boolean	operator== (const TextStyle& lhs, const TextStyle& rhs)
{
#if		qMPW_TEMP_OF_CLASS_WITH_DTOR_IN_EXPR_BUG
// because of anopyher NYI bug with MPW Cfront, we need to mangle this further...
//File ":Headers:TextEdit.hh"; line 628 # sorry, not implemented: cannot expand inline function operator ==() with  statement after "return"
Boolean result = False;
	if (lhs.GetFont () == rhs.GetFont ()) {
		if (lhs.GetColor () == rhs.GetColor ()) {
			result = True;
		}
	}
	return (result);
#else
	return ((lhs.GetFont () == rhs.GetFont ()) and (lhs.GetColor () == rhs.GetColor ()));
#endif
}
Boolean	operator!= (const TextStyle& lhs, const TextStyle& rhs)
{
#if		qMPW_TEMP_OF_CLASS_WITH_DTOR_IN_EXPR_BUG
// because of anopyher NYI bug with MPW Cfront, we need to mangle this further...
//File ":Headers:TextEdit.hh"; line 628 # sorry, not implemented: cannot expand inline function operator ==() with  statement after "return"
Boolean result = False;
	if (lhs.GetFont () != rhs.GetFont ()) {
		if (lhs.GetColor () != rhs.GetColor ()) {
			result = True;
		}
	}
	return (result);
#else
	return ((lhs.GetFont () != rhs.GetFont ()) or (lhs.GetColor () != rhs.GetColor ()));
#endif
}






/*
 ********************************************************************************
 ************************************ TextRun ***********************************
 ********************************************************************************
 */
TextRun::TextRun (const TextStyle& textStyle, const TextSelection& textSelection):
	fStyle (textStyle),
	fSelection (textSelection)
{
}

const	TextStyle&	TextStyle::operator= (const TextStyle& rhs)
{
	fFont = rhs.fFont;
	fColor = rhs.fColor;
	return (*this);
}

Boolean	operator== (const TextRun& lhs, const TextRun& rhs)
{
#if		qMPW_TEMP_OF_CLASS_WITH_DTOR_IN_EXPR_BUG
// because of anopyher NYI bug with MPW Cfront, we need to mangle this further...
//File ":Headers:TextEdit.hh"; line 628 # sorry, not implemented: cannot expand inline function operator ==() with  statement after "return"
Boolean result = False;
	if (lhs.GetSelection () == rhs.GetSelection ()) {
		if (lhs.GetStyle () == rhs.GetStyle ()) {
			result = True;
		}
	}
	return (result);
#else
		return ((lhs.GetSelection () == rhs.GetSelection ()) and (lhs.GetStyle () == rhs.GetStyle ()));
#endif
	}
Boolean	operator!= (const TextRun& lhs, const TextRun& rhs)
	{
#if		qMPW_TEMP_OF_CLASS_WITH_DTOR_IN_EXPR_BUG
// because of anopyher NYI bug with MPW Cfront, we need to mangle this further...
//File ":Headers:TextEdit.hh"; line 628 # sorry, not implemented: cannot expand inline function operator ==() with  statement after "return"
Boolean result = False;
	if (lhs.GetSelection () != rhs.GetSelection ()) {
		if (lhs.GetStyle () != rhs.GetStyle ()) {
			result = True;
		}
	}
	return (result);
#else
	return ((lhs.GetSelection () != rhs.GetSelection ()) or (lhs.GetStyle () != rhs.GetStyle ()));
#endif
}











/*
 ********************************************************************************
 ******************************* AbstractTextEdit *******************************
 ********************************************************************************
 */
CollectionSize	AbstractTextEdit::kAnyLength = 1000000000;
Boolean			AbstractTextEdit::kMultiLine = True;

AbstractTextEdit::AbstractTextEdit (TextController* controller):
	View (),
	EnableItem (),
	FocusItem (),
	AbstractTextView (), 
	TextEditBase (controller),
	Changeable (),
	fEnabled (True),
	fEditMode (eEditable),
	fTextRequired (False),
	fVerticalAutoSize (False),
	fHorizontalAutoSize (False),
	fMultiLine (kMultiLine),
	fMaxLength (kAnyLength)
{
}

AbstractTextEdit::~AbstractTextEdit ()
{
}

void	AbstractTextEdit::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	// Don't bother calling inherited EffectiveLiveChanged () since we should have no subviews, and thats all
	// the inherited one helps with...
	if (GetEffectiveFocused ()) {
		SetTextActive (newLive, updateMode);
		updateMode = eNoUpdate;
	}
	Refresh (updateMode);
}

void	AbstractTextEdit::EffectiveFocusChanged (Boolean newFocused, Panel::UpdateMode updateMode)
{
	if (GetEffectiveLive ()) {
		SetSelection ();
		SetTextActive (newFocused, updateMode);
		updateMode = eNoUpdate;
	}
	FocusItem::EffectiveFocusChanged (newFocused, updateMode);
}

void	AbstractTextEdit::SetChangeCount_ (ChangeCount newCount)
{
	Changeable::SetChangeCount_ (newCount);
	if (GetController () != Nil) {
		GetController ()->TextChanged (this);
	}
	if (fVerticalAutoSize or fHorizontalAutoSize) {
		AdjustSize (eDelayedUpdate);
	}
}

Boolean	AbstractTextEdit::GetLive () const
{
	return (Boolean (GetEnabled () and View::GetLive () and (GetEditMode () != eDisplayOnly)));
}

void	AbstractTextEdit::DoSetupMenus ()
{
	FocusItem::DoSetupMenus ();
	TextSelection	currentSelection = GetSelection ();
	Boolean	isSelection = currentSelection.TextIsSelected ();
	if (GetEditMode () == eEditable) {
		EnableCommand (eCut, isSelection);
		EnableCommand (ePaste, ClipBoard::Get ().Contains (ClipBoard::eText));
		EnableCommand (eClear, isSelection);
	}
	EnableCommand (eCopy, isSelection);
	EnableCommand (eSelectAll, Boolean ((currentSelection.GetFrom () > 1) or (currentSelection.GetTo () <= GetLength ())));
}

Boolean	AbstractTextEdit::DoCommand (const CommandSelection& selection)
{
	CommandNumber commandNumber = selection.GetCommandNumber ();
	switch (commandNumber) {
		case eCut:
		case eCopy:
		case ePaste:
		case eClear: {
			Assert (GetEditMode () != eDisplayOnly);
			Assert ((commandNumber == eCopy) or (GetEditMode () == eEditable));
			PostCommand (new TEClipCommand (*this, commandNumber));
			return (True);
		}
		break;
		case	eSelectAll: {
			Assert (GetLive ());
			SetSelection ();
		}
		break;
		default: {
			return (FocusItem::DoCommand (selection));
		}
		break;
	}
}

Boolean	AbstractTextEdit::ValidateKey (const KeyStroke& keyStroke)
{
	char	c	=	keyStroke.GetCharacter ();		// tmp hack

	if (((c == '\n') and (not GetMultiLine ())) or ((GetMaxLength () != kAnyLength) and (GetLength () == GetMaxLength ()))) {
		return (False);
	}
	return (True);
}

Boolean	AbstractTextEdit::HandleKeyStroke (const KeyStroke& keyStroke)
{
	Require (GetEffectiveLive ());
	
	if (ValidateKey (keyStroke)) {
		if (GetEditMode () == eEditable) {
			char	c	=	keyStroke.GetCharacter ();		// not safe for multibyute chars....
			TEKeyCommand*	keyCommand = new TEKeyCommand (*this, c);
			keyCommand->DoIt ();
	
			Command*	oldCommand = Application::Get ().GetPreviousCommand ();
			if ((keyStroke != KeyStroke::kDelete) and (oldCommand != Nil) and (oldCommand->GetDone ()) and (oldCommand->GetCommandNumber () == eKeyPress)) {
				// we want to put the old key command back on the queue, so
				// that both keystrokes get consed into a single compound command
				Application::Get ().SetPreviousCommand (Nil);
				PostCommand (oldCommand);
				keyCommand->SetCommitsLast (False);
			}
			PostCommand (keyCommand);
			return (True);
		}
	}
	
	return (FocusItem::HandleKeyStroke (keyStroke));
}

String	AbstractTextEdit::GetText () const
{
	return (AbstractTextView::GetText ());
}

String	AbstractTextEdit::GetText (const TextSelection& selection) const
{
	Invariant ();
	return (GetText_ (selection));
}

void	AbstractTextEdit::SetText (const String& s, Panel::UpdateMode updateMode)
{
	AbstractTextView::SetText (s, updateMode);
}

void	AbstractTextEdit::SetText (const String& s, const TextSelection& selection, Panel::UpdateMode updateMode)
{
	Invariant ();
	SetText_ (s, selection, updateMode);
}

void	AbstractTextEdit::SetText (const char* s, Panel::UpdateMode updateMode)
{
	AbstractTextView::SetText (String (s), updateMode);
}

void	AbstractTextEdit::SetText (const char* s, const TextSelection& selection, Panel::UpdateMode updateMode)
{
	Invariant ();
	SetText_ (String (s), selection, updateMode);
}

void	AbstractTextEdit::Validate ()
{
	if (GetTextRequired () and (GetLength () == 0)) {
		FocusItem::sFocusFailed.Raise (String (String::eReadOnly, "This field must be filled in."));
	}
	FocusItem::Validate ();
}

AbstractTextEdit::EditMode	AbstractTextEdit::GetEditMode () const
{
	Invariant ();
	return (fEditMode);
}

void	AbstractTextEdit::SetEditMode (EditMode mode)
{
	Invariant ();
	if (GetEditMode () != mode) {
		SetEditMode_ (mode);
	}
	Ensure (GetEditMode () == mode);
}

Boolean	AbstractTextEdit::GetAutoSize (Point::Direction d) const
{
	Invariant ();
	switch (d) {
		case	Point::eVertical:		return (fVerticalAutoSize);
		case	Point::eHorizontal:		return (fHorizontalAutoSize);
		default:						RequireNotReached ();
	}
	AssertNotReached (); return (True);
}

void	AbstractTextEdit::SetAutoSize (Point::Direction d, Boolean autoSize, Panel::UpdateMode updateMode)
{
	Invariant ();
	if (GetAutoSize (d) != autoSize) {
		SetAutoSize_ (d, autoSize, updateMode);
		if (autoSize) {
			AdjustSize (updateMode);
		}
	}
	Ensure (GetAutoSize (d) == autoSize);
}
	
CollectionSize	AbstractTextEdit::GetMaxLength () const
{
	Invariant ();
	return (fMaxLength);
}

void	AbstractTextEdit::SetMaxLength (CollectionSize maxLength)
{
	Invariant ();
	if (maxLength != GetMaxLength ()) {
		SetMaxLength_ (maxLength);
	}
	Ensure (maxLength == GetMaxLength ());
}
		
Boolean		AbstractTextEdit::GetMultiLine () const
{
	Invariant ();
	return (fMultiLine);
}

void	AbstractTextEdit::SetMultiLine (Boolean multiLine)
{
	Invariant ();
	if (multiLine != GetMultiLine ()) {
		SetMultiLine_ (multiLine);
	}
	Ensure (multiLine == GetMultiLine ());
}

void	AbstractTextEdit::SetMaxLength_ (CollectionSize maxLength)
{
	fMaxLength = maxLength;
	if ((fMaxLength != kAnyLength) and (fMaxLength > GetLength ())) {
		String	text = GetText ();
		text.SetLength (fMaxLength);
		SetText (text);
	}
}

void	AbstractTextEdit::SetMultiLine_ (Boolean multiLine)
{
	fMultiLine = multiLine;
	if (fMultiLine) {
		String text = GetText ();
		String	newText;
		newText.SetLength (text.GetLength ());
		CollectionSize	newIndex = 1;
		for (CollectionSize oldIndex = 1; oldIndex <= text.GetLength (); oldIndex++) {
			if (text[oldIndex] != '\n') {
				newText.SetCharAt (text[oldIndex], newIndex++);
			}
		}
		newText.SetLength (newIndex-1);
		SetText (newText);
	}
}

Boolean	AbstractTextEdit::GetTextRequired () const
{
	Invariant ();
	return (fTextRequired);
}

void	AbstractTextEdit::SetTextRequired (Boolean required)
{
	Invariant ();
	if (GetTextRequired () != required) {
		SetTextRequired_ (required);
	}
	Ensure (GetTextRequired () == required);
}

const TextRunIterator*	AbstractTextEdit::MakeRunIterator ()
{
	Invariant ();
	return (MakeRunIterator (TextSelection (1, GetLength () + 1)));
}

const TextRunIterator*	AbstractTextEdit::MakeRunIterator (const TextSelection& selection)
{
	Invariant ();
	return (new TextRunIterator (*this, selection));
}

void	AbstractTextEdit::TrackingDrag (const Point& mouseAt)
{
	if (GetController () != Nil) {
		GetController ()->TextTracking (mouseAt);
	}
}

void	AbstractTextEdit::SetEditMode_ (EditMode mode)
{
	fEditMode = mode;
	PlusChangeCount ();
}

void	AbstractTextEdit::SetTextRequired_ (Boolean required)
{
	fTextRequired = required;
}

void	AbstractTextEdit::SetAutoSize_ (Point::Direction d, Boolean autoSize, Panel::UpdateMode /*updateMode*/)
{
	switch (d) {
		case Point::eVertical:
			fVerticalAutoSize = autoSize;
			break;
			
		case Point::eHorizontal:
			fHorizontalAutoSize = autoSize;
			break;
			
		default:						
			RequireNotReached ();
	}
}

Boolean	AbstractTextEdit::GetEnabled_ () const
{
	return (fEnabled);
}

void	AbstractTextEdit::SetEnabled_ (Boolean enabled, Panel::UpdateMode updateMode)
{
	Boolean	oldLive = GetEffectiveLive ();
	fEnabled = enabled;

	if (oldLive != GetEffectiveLive ()) {
		EffectiveLiveChanged (not oldLive, updateMode);
	}
}







/*
 ********************************************************************************
 ******************************* AbstractTextEdit_MacUI ************************
 ********************************************************************************
 */
AbstractTextEdit_MacUI::~AbstractTextEdit_MacUI ()
{
}

AbstractTextEdit_MacUI::AbstractTextEdit_MacUI (TextController* controller):
	   AbstractTextEdit (controller)
{
	SetBorder (1, 1, eNoUpdate);
}








#if		qMacToolkit
/*
 ********************************************************************************
 ******************************* TextEdit_MacUI_Native **************************
 ********************************************************************************
 */


#if		qMPW_CFront
// If apple implements this call as a macro, we must adjust it cuz we changed type type used in it...
#ifdef	TEScrapHandle
#undef	TEScrapHandle
#define TEScrapHandle() (* (osHandle*) 0xAB4)
#endif
#endif	/*qMPW_CFront*/

class	TextEdit_MacUI_Native_Scrap : public PrivateScrap {
	public:
		TextEdit_MacUI_Native_Scrap ()
		{
			sLastChangeCount = ClipBoard::Get ().GetScrapChangeCount ();
		}
#if		qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG
		virtual	void	ptbl_hack ();
#endif
		override	CollectionSize			CountTypes ()					{ return (1); }
		override	ClipBoard::ClipType		GetIthType (CollectionSize i)	{ Require (i == 1); return (ClipBoard::eText); }
		override	size_t					GetIthSize (CollectionSize i)	{ Require (i == 1); return (TEGetScrapLen ()); }

		override	Boolean	GetClip (ClipBoard::ClipType type, void*& data, size_t& size)
		{
			switch (type) {
				case	ClipBoard::eText: {
					size = GetIthSize (1);
					data = new char [size];
					osHandle osh	=	TEScrapHandle ();
					::HLock (osh);
					memcpy (data, *osh, size);
					::HUnlock (osh);
					return (True);
				}
				break;
				default: {
					return (False);
				}
			}
			AssertNotReached (); return (False);
		}

		override	void	ConvertToPublic ()
		{
		}

		static	void	DoCut (TERec** theTERec)
		{
			ClipBoard::Get ().SetPrivateScrap (new TextEdit_MacUI_Native_Scrap ());
			sLastChangeCount = ClipBoard::Get ().GetScrapChangeCount ();
			::TECut (theTERec);
		}
			
		static	void	DoCopy (TERec** theTERec)
		{
			ClipBoard::Get ().SetPrivateScrap (new TextEdit_MacUI_Native_Scrap ());
			sLastChangeCount = ClipBoard::Get ().GetScrapChangeCount ();
			::TECopy (theTERec);
		}
			
		static	void	DoPaste (TERec** theTERec)
		{
			if (ClipBoard::Get ().GetScrapChangeCount () != sLastChangeCount) {
				sLastChangeCount = ClipBoard::Get ().GetScrapChangeCount ();
			}
			::TEStylPaste (theTERec);
		}
		
	static	UInt32	sLastChangeCount;
};
UInt32	TextEdit_MacUI_Native_Scrap::sLastChangeCount	=	0;

#if		qMPW_NO_GEN_VTABLE_KEY_FUNCT_BUG
void	TextEdit_MacUI_Native_Scrap::ptbl_hack () {}
#endif

class	TEBlinker : public PeriodicTask {
	public:
		TEBlinker (TextEdit_MacUI_Native& te):
			fTextEdit (te)
		{
		}
		
		override	void	RunABit ()
		{
			Assert (fTextEdit.GetEffectiveLive ());
			fTextEdit.PrepareForDraw (View::eImmediateUpdate);
			::TEIdle (fTextEdit.GetOSRepresentation ());
		}
		
	private:
		TextEdit_MacUI_Native&	fTextEdit;
};


TextEdit_MacUI_Native*	TextEdit_MacUI_Native::sCurrentTE	=	Nil;


TextEdit_MacUI_Native::TextEdit_MacUI_Native (TextController* controller):
	AbstractTextEdit_MacUI (controller),
	fTEHandle (Nil),
	fBlinker (Nil),
	fEffectiveStyle (kApplicationFont, kBlackColor)
{	
	SetFont (&kApplicationFont);

	osGrafPort*	aPort = qd.thePort;

/*
 * This prepare, etc...  should not be needed but for bug patching Nil style iterator...
 * ask STERL Saturday, September 7, 1991 11:24:39 PM.
 */
	PrepareForDraw (eNoUpdate);

	osRect	kZeroOSRect = {0, 0, 0, 0};
	fTEHandle = ::TEStylNew (&kZeroOSRect, &kZeroOSRect);
	if (fTEHandle == Nil) {
		gMemoryException.Raise ();
	}
	Assert (qd.thePort == GetWorkPort ());
	::SetPort (aPort);

	fBlinker = new TEBlinker (*this);

	::SetClikLoop (&sClickLoopProc, GetOSRepresentation ());

	SetWordWrap_ (True, eNoUpdate);				// called protected version so that real work is always done (no comparison with existing value)
	SetJustification_ (eJustLeft, eNoUpdate);	// called protected version so that real work is always done (no comparison with existing value)

	AdjustSize (eNoUpdate);
}

TextEdit_MacUI_Native::~TextEdit_MacUI_Native ()
{
	Require (not (GetEffectiveLive ()));
	delete (fBlinker);
	if (fTEHandle != Nil) {
		::TEDispose (fTEHandle);
	}
}

Boolean	TextEdit_MacUI_Native::TrackMovement (const Point& /*cursorAt*/, Region& mouseRgn, const KeyBoard& /*keyBoardState*/)
{
	kIBeamCursor.Install ();
	mouseRgn = GetLocalExtent ();
	return (True);
}

Boolean	TextEdit_MacUI_Native::TrackPress (const MousePressInfo& mouseInfo)
{
	if (not GetEffectiveFocused () and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
		return (True);
	}

	if (GetLength () > 0) {	// weird work around for Nil style bug
		osPoint	osp	=	os_cvt (LocalToTablet (mouseInfo.fPressAt), osp);
		PrepareForDraw (eImmediateUpdate);
		Assert (sCurrentTE == Nil);
		sCurrentTE = this;
		::TEClick (osp, mouseInfo.IsExtendedSelection (), GetOSRepresentation ());
		Assert (sCurrentTE == this);
		sCurrentTE = Nil;
	
		MenuOwner::SetMenusOutOfDate ();
	}
	return (True);
}

void	TextEdit_MacUI_Native::TabbingFocus (SequenceDirection d, Panel::UpdateMode updateMode)
{
	// On mac native textedit, doing a teactivate/deactivate much better for tabbing thru textedit. So translate
	// delayed update, which is usually passed in TabbingFocus, to immediateUpdate so we can use the TEActivate.
	AbstractTextEdit_MacUI::TabbingFocus (d, updateMode==eDelayedUpdate? eImmediateUpdate: updateMode);
}

void	TextEdit_MacUI_Native::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	AbstractTextEdit_MacUI::EffectiveFontChanged (newFont, updateMode);

// Some of this may be unncessary...

	InvalidateLayout ();					// cuz our layout may change
	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
	}

	ResetEffectiveStyle ();

	Refresh (updateMode);					// cuz our display may change
}

void	TextEdit_MacUI_Native::DoSetupMenus ()
{
	AbstractTextEdit::DoSetupMenus ();
}

Boolean	TextEdit_MacUI_Native::DoCommand (const CommandSelection& selection)
{
	return (AbstractTextEdit::DoCommand (selection));
}

Boolean	TextEdit_MacUI_Native::HandleKeyStroke (const KeyStroke& keyStroke)
{
	Require (GetEffectiveLive ());
	if (GetEditMode () == eEditable) {
		if (keyStroke == KeyStroke::kClear) {
			DoCommand (CommandSelection (eClear));
			return (True);
		}
	}
	return (AbstractTextEdit::HandleKeyStroke (keyStroke));
}

void	TextEdit_MacUI_Native::Layout ()
{
	osRect	osr;

	Rect	r = GetLocalExtent ().InsetBy (GetBorder () + GetMargin ());
	(*GetOSRepresentation ())->viewRect = 	os_cvt (r, osr);
	(*GetOSRepresentation ())->destRect = 	os_cvt (r, osr);

	PrepareForDraw (eNoUpdate);
	ResetEffectiveStyle ();

	::TECalText (GetOSRepresentation ());
	
	AbstractTextEdit::Layout ();
}

void	TextEdit_MacUI_Native::SetTextActive (Boolean active, Panel::UpdateMode update)
{
#if		0
if (update == eDelayedUpdate) {
	// temp hack to avoid flicker, hard to decide what to inval if we really try
	// to call refresh as is done below
	if (not GetSelection ().TextIsSelected ()) {
		update = eImmediateUpdate;
	}
}
#endif
	PrepareForDraw (update);
	if (active) {
		::TEActivate (GetOSRepresentation ());
		if (GetEditMode () == eEditable) {
			Application::Get ().AddIdleTask (fBlinker);
		}
	}
	else {
		::TEDeactivate (GetOSRepresentation ());
		if (GetEditMode () == eEditable) {
			Application::Get ().RemoveIdleTask (fBlinker);
		}
	}
	if (update == eDelayedUpdate) {
		Refresh (GetLocalRegion ().InsetBy (GetBorder ()), eDelayedUpdate);
	}

	MenuOwner::SetMenusOutOfDate ();
}

void	TextEdit_MacUI_Native::Render (const Region& updateRegion)
{
	// we need to do graying under some circumstances
	// not sure this is right, but good start...

	AbstractTextEdit::Render (updateRegion);
	if (not GetEnabled ()) {
		Gray ();
	}
}

void	TextEdit_MacUI_Native::Draw (const Region& update)
{
	osRect	osr	=	os_cvt (update.GetBounds (), osr);

	PrepareForDraw (eImmediateUpdate);
	Assert ((*GetOSRepresentation ())->inPort == GetTablet ()->GetOSGrafPtr ());
	Assert ((*GetOSRepresentation ())->inPort == qd.thePort);
	::TEUpdate (&osr, GetOSRepresentation ());
}

Point	TextEdit_MacUI_Native::CalcDefaultSize_ (const Point& defaultSize) const
{
	osRect	osr;
	Point	size = defaultSize;

	/*
	 * hack workaround since if size is zero, then tecaltext hangs doing calculation!!!
	 */
	if (size.GetH () == 0) {
		size.SetH (32000);
	}

	osGrafPort*	aPort = qd.thePort;
	PrepareForDraw (eNoUpdate);

	Rect	newExtent = Rect (kZeroPoint, size).InsetBy (GetBorder () + GetMargin ());
	struct	TERec**		osTE	=	GetOSRepresentation ();
	(*osTE)->viewRect = 	os_cvt (newExtent, osr);
	(*osTE)->destRect = 	os_cvt (newExtent, osr);
	
	ResetEffectiveStyle ();
	::TECalText (osTE);
	
	Rect	r = GetLocalExtent ().InsetBy (GetBorder () + GetMargin ());
	(*GetOSRepresentation ())->viewRect = 	os_cvt (r, osr);
	(*GetOSRepresentation ())->destRect = 	os_cvt (r, osr);

	if (qd.thePort == GetWorkPort ()) {
		::SetPort (aPort);
	}
	
	Coordinate	lineCount	=	GetLineCount ();
	if (lineCount == 0) {
		size.SetV (fEffectiveStyle.GetFont ().GetLineHeight () + 1);
	}
	else {
		Coordinate	newV = ::TEGetHeight (lineCount, 0, osTE);
		if (GetLength () != 0) {
			if (GetText (TextSelection (GetLength (), GetLength ()+1)) == "\n") {
				newV += ::TEGetHeight ((*osTE)->nLines, (*osTE)->nLines, osTE);
			}
		}
		size.SetV (newV);
	}

	Coordinate	maxWidth = 0;
	for (lineCount = 1; lineCount <= (*osTE)->nLines; lineCount++) {
		osPoint	osp1	=	::TEGetPoint ((*osTE)->lineStarts[lineCount-1], osTE);
		osPoint	osp2;
		if ((lineCount == (*osTE)->nLines)) {
			osp2 =	::TEGetPoint (GetLength (), osTE);
		}
		else {
			osp2 =	::TEGetPoint ((*osTE)->lineStarts[lineCount]-1, osTE);
		}
		Coordinate	lineWidth = osp2.h - osp1.h;
		maxWidth = Max (maxWidth, lineWidth +1);	// why do I need a +1???
	}
	size.SetH (maxWidth);

	return (size + GetBorder()*2 + GetMargin()*2);
}

CollectionSize	TextEdit_MacUI_Native::GetLength () const
{
	AssertNotNil (GetOSRepresentation ());
	Ensure ((*GetOSRepresentation ())->teLength >= 0);
	return ((*GetOSRepresentation ())->teLength);
}

TextSelection	TextEdit_MacUI_Native::GetSelection () const
{
	AssertNotNil (GetOSRepresentation ());
	return (TextSelection (
		(*GetOSRepresentation ())->selStart + 1, 
		(*GetOSRepresentation ())->selEnd + 1));
}

void	TextEdit_MacUI_Native::SetSelection (Panel::UpdateMode updateMode)
{
	SetSelection (TextSelection (1, GetLength ()+1), updateMode);
}

void	TextEdit_MacUI_Native::SetSelection (const TextSelection& selection, Panel::UpdateMode updateMode)
{
	Require (selection.GetFrom () >= 1);

	TextSelection	oldSelection = GetSelection ();
	if (oldSelection != selection) {
		osGrafPort*	aPort = qd.thePort;
		PrepareForDraw (updateMode);
		::TESetSelect (selection.GetFrom ()-1, selection.GetTo ()-1, GetOSRepresentation ());
		if (qd.thePort == GetWorkPort ()) {
			::SetPort (aPort);
		}
		if (updateMode == eDelayedUpdate) {
			Refresh (eDelayedUpdate);
		}

		if (GetEffectiveLive ()) {
			MenuOwner::SetMenusOutOfDate ();
		}
	}
}

String	TextEdit_MacUI_Native::GetText_ ()	const
{
	return (GetText_ (TextSelection (1, GetLength ()+1)));
}

String	TextEdit_MacUI_Native::GetText_ (const TextSelection& selection) const
{
	struct	TERec**		osTE	=	GetOSRepresentation ();
	AssertNotNil (osTE);

	Require (selection.GetFrom () >= 1);

	if (selection.GetFrom () <= selection.GetTo ()) {
		CollectionSize	len	=	selection.GetTo () - selection.GetFrom ();
		String			s	=	kEmptyString;
		s.SetLength (len);
		osHandle	textHandle		=	(*osTE)->hText;
		char		oldState		=	::HGetState (textHandle);
		::HLock (textHandle);
		char*	cPtr	=	(char*)&(*textHandle)[selection.GetFrom ()-1];
		for (CollectionSize i = 1; i <= len; i++) {
			s.SetCharAt (*cPtr++, i);
		}
		::HSetState (textHandle, oldState);
		return (s);
	}
	else {
		return (kEmptyString);
	}
}

void	TextEdit_MacUI_Native::SetText_ (const String& s, Panel::UpdateMode updateMode)
{
	if (s.GetLength () > GetMaxLength ()) {
		s.SetLength (GetMaxLength ());
	}
	
	SetText_ (s, TextSelection (1, GetLength ()+1), updateMode);
}

void	TextEdit_MacUI_Native::SetText_ (const String& s, const TextSelection& selection, Panel::UpdateMode updateMode)
{
	struct	TERec**		osTE	=	GetOSRepresentation ();
	AssertNotNil (osTE);

	Boolean	hadText = Boolean (GetLength () > 0);
	Boolean	allTextWasSelected = Boolean ((GetLength () == 0) or ((selection.GetFrom () == 1) and (selection.GetTo () > GetLength ())));

	osGrafPort*	aPort = qd.thePort;
	PrepareForDraw (eNoUpdate);
	
	SetSelection (selection, eNoUpdate);
	
	::TEDelete (osTE);
	::TEInsert ((osPtr)s.Peek (), s.GetLength (), osTE);
	if (allTextWasSelected) {
		Font	f = fEffectiveStyle.GetFont ();
		osTextStyle	textStyle;
		textStyle.tsFont = f.GetID ();
		textStyle.tsFace = f.GetStyleOSRep ();
		textStyle.tsSize = f.GetSize ();
		fEffectiveStyle.GetColor ().GetRGB (textStyle.tsColor.red, textStyle.tsColor.green, textStyle.tsColor.blue);	
	
		(*osTE)->selStart = 0;
		(*osTE)->selEnd = GetLength ();
		::TESetStyle (doAll, &textStyle, False, osTE);
		(*osTE)->selStart = GetLength ();
		if (GetLength () == 0) {
			ResetNilRun ();
		}
	}
	if (qd.thePort == GetWorkPort ()) {
		::SetPort (aPort);
	}
	if (allTextWasSelected) {
		SetSelection (eNoUpdate);
	}

	InvalidateLayout ();
	Refresh (updateMode);
	PlusChangeCount ();
}

TextStyle	TextEdit_MacUI_Native::GetStyle (CollectionSize i) const
{
	Require (i >= 1);
	Require (i <= GetLength ()+1);
	AssertNotNil (GetOSRepresentation ());
	Require (short (i-1) == (i-1));						// avoid truncation

	osTextStyle	osStyle;
	Int16	lineHeight;
	Int16	fontAscent;
	
	::TEGetStyle (i-1, &osStyle, &lineHeight, &fontAscent, GetOSRepresentation ());
	return (TextStyle (
		Font (osStyle.tsFont, osStyle.tsSize, osStyle.tsFace), 
		Color (osStyle.tsColor.red, osStyle.tsColor.green, osStyle.tsColor.blue)));
}

void	TextEdit_MacUI_Native::SetStyle (const TextStyle& style, Panel::UpdateMode update)
{
	SetStyle (style, TextSelection (1, GetLength () + 1), update);
}

void	TextEdit_MacUI_Native::SetStyle (const TextStyle& style, const TextSelection& selection, Panel::UpdateMode update)
{
	AssertNotNil (GetOSRepresentation ());

	Require (selection.GetFrom () >= 1);

	TextSelection	oldSelection = GetSelection ();
	SetSelection (selection, eNoUpdate);
	osTextStyle	osStyle;
	Font	f = style.GetFont ();
	osStyle.tsFont = f.GetID ();
	osStyle.tsSize = f.GetSize ();
	osStyle.tsFace = f.GetStyleOSRep ();
	style.GetColor ().GetRGB (osStyle.tsColor.red, osStyle.tsColor.green, osStyle.tsColor.blue);
	::TESetStyle (doAll, &osStyle, False, GetOSRepresentation ());
	if ((selection.GetFrom () == 1) and (selection.GetTo () > GetLength ())) {
		Font	f = style.GetFont ();
		SetFont (&f);
		fEffectiveStyle = style;
		ResetNilRun ();
	}
	SetSelection (oldSelection, eNoUpdate);


	Refresh (update);
}

TextRun		TextEdit_MacUI_Native::GetTextRun (CollectionSize index) const
{
	Require (index >= 1);
	Require (index <= GetLength ()+1);
	AssertNotNil (GetOSRepresentation ());
	Require (short (index-1) == (index-1));						// avoid truncation

	TEStyleHandle	osStyles = ::GetStylHandle (GetOSRepresentation ());
	AssertNotNil (osStyles);
	for (CollectionSize i = 0; i < (*osStyles)->nRuns; i++) {
		if ((*osStyles)->runs[i].startChar >= index) {
			STElement	stElement = (*(*osStyles)->styleTab)[i];
			Font	f = Font (stElement.stFont, stElement.stSize, stElement.stFace);
			Color	c = Color (stElement.stColor.red, stElement.stColor.green, stElement.stColor.blue);
			TextStyle		textStyle = TextStyle (f, c);
			CollectionSize	end = GetLength () + 1;
			if ((i+1) < (*osStyles)->nRuns) {
				end = (*osStyles)->runs[i+1].startChar; 
			}
			TextSelection	textSelection = TextSelection ((*osStyles)->runs[i].startChar, end);
			return (TextRun (textStyle, textSelection));
		}
	}
	AssertNotReached ();
}

TextSelection	TextEdit_MacUI_Native::Find (const String& pattern, CollectionSize i)
{
AssertNotReached ();
return (TextSelection (1, 1));
}

TextSelection	TextEdit_MacUI_Native::Replace (const String& pattern, const String& newText, CollectionSize i)
{
AssertNotReached ();
return (TextSelection (1, 1));
}

void	TextEdit_MacUI_Native::ReplaceAll (const String& pattern, const String& newText)
{
AssertNotReached ();
}


AbstractTextView::Justification	TextEdit_MacUI_Native::GetJustification_ () const
{
	switch ((*GetOSRepresentation ())->just) {
		case		teJustLeft:				return (eJustLeft);
		case		teJustCenter:			return (eJustCenter);
		case		teJustRight:			return (eJustRight);
		case		teForceLeft:			return (eJustLeft);		// ??
		default:	AssertNotReached ();	return (eJustLeft);		// ??
	}
}

void	TextEdit_MacUI_Native::SetJustification_ (AbstractTextView::Justification justification, Panel::UpdateMode update)
{
	switch (justification) {
		case		eJustLeft:		(*GetOSRepresentation ())->just = teJustLeft; break;
		case		eJustCenter:	(*GetOSRepresentation ())->just = teJustCenter; break;
		case		eJustRight:		(*GetOSRepresentation ())->just = teJustRight; break;
		case		eJustFull:		(*GetOSRepresentation ())->just = teJustLeft; break;
		default:	RequireNotReached ();
	}
	Refresh (update);
	PlusChangeCount ();
}
		
Boolean	TextEdit_MacUI_Native::GetWordWrap_ () const
{
	return (Boolean ((*GetOSRepresentation ())->crOnly > -1));
}

void	TextEdit_MacUI_Native::SetWordWrap_ (Boolean wordWrap, Panel::UpdateMode update)
{
	struct	TERec**		osTE	=	GetOSRepresentation ();
	(*osTE)->crOnly = (wordWrap? 1 : -1);
	InvalidateLayout ();
	Refresh (update);
	PlusChangeCount ();
}

Point	TextEdit_MacUI_Native::GetPosition (CollectionSize i) const
{
	struct	TERec**		osTE	=	GetOSRepresentation ();
	Require (i >= 1);
	Require (i <= GetLength ()+1);
	AssertNotNil (osTE);
	Require (short (i-1) == (i-1));						// avoid truncation

	/*
	 * As near as I can tell, TEGetPoint returns the wrong value for when the last line
	 * ends in a newline.  It returns the leftmost location on that last line, as opposed to
	 * the one right after the end.  Until I understand otherwise, we correct for this oversight
	 * here.	(LGP Jan7, 1991)
	 */

#if		qMPW_TEMP_OF_CLASS_WITH_DTOR_IN_EXPR_BUG
	if ((i == GetLength () + 1) and (GetLength () != 0)) {
		if (GetText (TextSelection (GetLength (), GetLength ()+1)) == "\n") {
			Point	p	=	TabletToLocal (os_cvt (::TEGetPoint (short (i-1), osTE)));
			return (p + Point (::TEGetHeight ((*osTE)->nLines, (*osTE)->nLines, osTE), 0));
		}
		else {
			return (TabletToLocal (os_cvt (::TEGetPoint (short (i-1), osTE))));
		}
	}
#else	/*qMPW_TEMP_OF_CLASS_WITH_DTOR_IN_EXPR_BUG*/
	if ((i == GetLength () + 1) and (GetText (TextSelection (GetLength (), GetLength ()+1)) == '\n')) {
		Point	p	=	TabletToLocal (os_cvt (::TEGetPoint (short (i-1), osTE)));
		return (p + Point (::TEGetHeight ((*osTE)->nLines, (*osTE)->nLines, osTE), 0));
	}
#endif	/*qMPW_TEMP_OF_CLASS_WITH_DTOR_IN_EXPR_BUG*/
	else {
		return (TabletToLocal (os_cvt (::TEGetPoint (short (i-1), osTE))));
	}
}

CollectionSize	TextEdit_MacUI_Native::GetOffset (const Point& p) const
{
	AssertNotNil (GetOSRepresentation ());
	osPoint	osp;
	Assert (::TEGetOffset (os_cvt (LocalToTablet (p), osp), GetOSRepresentation ()) >= 0);
	return (::TEGetOffset (os_cvt (LocalToTablet (p), osp), GetOSRepresentation ()) + 1);
}

String	TextEdit_MacUI_Native::GetNearestWord (const Point& p) const
{
	return (GetNearestWord (GetOffset (p)));
}

String	TextEdit_MacUI_Native::GetNearestWord (CollectionSize offset) const
{
	String	s = GetText ();
	CollectionSize	length = s.GetLength ();
	if ((offset > 0) and (offset <= length)) {
		if (isalnum (s[offset].GetAsciiCode ())) {
			for (CollectionSize	startOffset = offset-1; startOffset > 1; startOffset--) {
				if (not isalnum (s[startOffset].GetAsciiCode ())) {
					startOffset++;
					break;
				}
			}
			for (CollectionSize	endOffset = offset; endOffset < length; endOffset++) {
				if (not isalnum (s[endOffset].GetAsciiCode ())) {
					endOffset--;
					break;
				}
			}
			Assert (startOffset <= endOffset);
			return (s.SubString (startOffset, endOffset-startOffset+1));
		}
	}
	return (kEmptyString);
}

CollectionSize	TextEdit_MacUI_Native::GetLineCount () const
{
	return ((*GetOSRepresentation ())->nLines);
}

String		TextEdit_MacUI_Native::GetLine (CollectionSize i) const
{
	Require (i >= 1);
	Require (i <= GetLength ());
	
	for (int	lineCount = 1; lineCount <= (*GetOSRepresentation ())->nLines; lineCount++) {
		if ((*GetOSRepresentation ())->lineStarts[lineCount] > i) {
			return (GetText (GetLineLocation (lineCount)));
		}
	}
	AssertNotReached ();
	return (kEmptyString);
}

TextSelection	TextEdit_MacUI_Native::GetLineLocation (CollectionSize line) const
{
	struct	TERec**	osTE	=	GetOSRepresentation ();
	return (TextSelection ((*osTE)->lineStarts[line-1], (*osTE)->lineStarts[line]));
}

void	TextEdit_MacUI_Native::AdjustSize (Panel::UpdateMode updateMode)
{
	struct	TERec**	osTE	=	GetOSRepresentation ();
	Point			oldSize	=	GetSize ();
	Point			newSize	=	oldSize;

	if (GetAutoSize (Point::eVertical)) {
		/*
		 * Adjust lineCount to reflect that when there are no characters, there are no
		 * lines, but TE fakes one extra line to type into.  Also, when the last character
		 * is a NL, TE does a similar trick.
		 */
		Coordinate	lineCount	=	GetLineCount ();
		if (lineCount == 0) {
			newSize.SetV (fEffectiveStyle.GetFont ().GetLineHeight ());
		}
		else {
			Coordinate	newV = ::TEGetHeight (lineCount, 0, osTE);
			if (GetLength () != 0) {
				String	lastChar = GetText (TextSelection (GetLength (), GetLength ()+1));
				Assert (lastChar.GetLength () == 1);
				if (lastChar[1] == '\n') {
					newV += ::TEGetHeight ((*osTE)->nLines, (*osTE)->nLines, osTE);
				}
			}
			newSize.SetV (newV);
		}
	}
	if (GetAutoSize (Point::eHorizontal)) {
		const	Coordinate	kPageWidth	=	32000;		// really big - to aviod trouble -
														// need better solution, but hard with
														// Apple's native TextEdit
		newSize.SetH (kPageWidth);
	}

	if (oldSize != newSize) {
		if (updateMode != eNoUpdate) {
			Region	oldVisibleArea	=	GetVisibleArea ();
			SetSize (newSize, eNoUpdate);
		
			if (GetTablet () != Nil) {	// hack cuz would crash otw...
				ProcessLayout ();
			}
	
			/* Incase the reset exposed new space, refresh that area.	*/
			Refresh (GetVisibleArea () - oldVisibleArea, updateMode);
		}
		else {
			SetSize (newSize, eNoUpdate);
		}
	}
}

void	TextEdit_MacUI_Native::DoCut (Panel::UpdateMode updateMode)
{
	Assert (GetEffectiveLive ());
	PrepareForDraw (updateMode);
	TextEdit_MacUI_Native_Scrap::DoCut (GetOSRepresentation ());
	if (updateMode == eDelayedUpdate) {
		Refresh (eDelayedUpdate);
	}
	PlusChangeCount ();
}

void	TextEdit_MacUI_Native::DoCopy (Panel::UpdateMode /*updateMode*/)
{
	Assert (GetEffectiveLive ());
	TextEdit_MacUI_Native_Scrap::DoCopy (GetOSRepresentation ());
}

void	TextEdit_MacUI_Native::DoPaste (Panel::UpdateMode updateMode)
{
	Assert (GetEffectiveLive ());
	PrepareForDraw (updateMode);
	TextEdit_MacUI_Native_Scrap::DoPaste (GetOSRepresentation ());

// why this refresh - we did a preparefordraw () with the right update mode - te should have done update for us???
// same for other clip operations - test !!! LGP April 16, 1992
// Really - this is OK, but we want to be less brutal - need to be able to get selected
// Text Region, before and after, and also, region of text past end of selection
// but still visible. This is what we inval...
	if (updateMode == eDelayedUpdate) {
		Refresh (eDelayedUpdate);
	}
	PlusChangeCount ();
}

void	TextEdit_MacUI_Native::DoClear (Panel::UpdateMode updateMode)
{
	Assert (GetEffectiveLive ());
	PrepareForDraw (updateMode);
	::TEDelete (GetOSRepresentation ());
	if (updateMode == eDelayedUpdate) {
		Refresh (eDelayedUpdate);
	}
	PlusChangeCount ();
}

void	TextEdit_MacUI_Native::DoKeyPress (Character c, Panel::UpdateMode updateMode)
{
	PrepareForDraw (updateMode);
	::TEKey (c.GetAsciiCode (), GetOSRepresentation ());
	if (updateMode == eDelayedUpdate) {
		Refresh (eDelayedUpdate);
	}
	PlusChangeCount ();
}


void	TextEdit_MacUI_Native::PrepareForDraw (Panel::UpdateMode updateMode)
{
	if (GetTablet () == Nil) {
		ResetEffectiveStyle ();
		Font	f = fEffectiveStyle.GetFont ();
		GetWorkPort ()->txFont = f.GetID ();
		GetWorkPort ()->txFace = f.GetStyleOSRep ();
		GetWorkPort ()->txSize = f.GetSize ();
		Tablet::SetupOSRepFromClip (GetWorkPort (), kEmptyRegion, kZeroPoint);
	}
	else {
		if (updateMode == eImmediateUpdate) {
			Tablet::SetupOSRepFromClip (GetTablet ()->GetOSGrafPtr (), LocalToTablet (GetVisibleArea ()), kZeroPoint);
		}
		else {
			Tablet::SetupOSRepFromClip (GetTablet ()->GetOSGrafPtr (), kEmptyRegion, kZeroPoint);
		}
		GetTablet ()->SetOSBackground (GetEffectiveBackground ());
	}
	Ensure ((GetTablet () != Nil) or (qd.thePort == GetWorkPort ()));

	if (fTEHandle != Nil) {
		struct	TERec**	osTE	=	GetOSRepresentation ();
		(*osTE)->inPort = qd.thePort;

		/*
		 * Setting the view/dst rect too small cauzes mac TE to hang, so make sure we keep to some minimim...
		 */
		Point	size	=	GetSize () - 2*GetBorder () - 2*GetMargin ();
		if (size.GetV () < 15) {		// sb lineheight...
			size.SetV (15);
		}
		if (size.GetH () < 15) {		// sb charWidth...
			size.SetH (15);
		}
		Rect	tabletExtent	=	LocalToTablet (Rect (GetBorder () + GetMargin (), size));
		osRect	osr;
		os_cvt (tabletExtent, osr);
		(*GetOSRepresentation ())->viewRect = 	osr;
		(*GetOSRepresentation ())->destRect = 	osr;
	}
}

void	TextEdit_MacUI_Native::ResetNilRun ()
{
	struct	TERec**	osTE	=	GetOSRepresentation ();
	Font	f = fEffectiveStyle.GetFont ();
	osTextStyle	textStyle;
	textStyle.tsFont = f.GetID ();
	textStyle.tsFace = f.GetStyleOSRep ();
	textStyle.tsSize = f.GetSize ();
	fEffectiveStyle.GetColor ().GetRGB (textStyle.tsColor.red, textStyle.tsColor.green, textStyle.tsColor.blue);	

	TextSelection	oldSelection = GetSelection ();
	(*osTE)->selStart = 0;
	(*osTE)->selEnd = 0;

	::TESetStyle (doAll, &textStyle, False, osTE);
	TEStyleHandle	osStyles 	= ::GetStylHandle (osTE);
	NilStHandle	NilStyle	= (*osStyles)->NilStyle;
	StScrpHandle	NilScrap	= (*NilStyle)->NilScrap;
	Assert ((*NilScrap)->scrpNStyles == 1);
	ScrpSTElement*	element = (*NilScrap)->scrpStyleTab;
	element->scrpFont = f.GetID ();
	element->scrpFace = f.GetStyleOSRep ();
	element->scrpSize = f.GetSize ();
	element->scrpColor = textStyle.tsColor;

	(*osTE)->selStart = oldSelection.GetFrom ()-1;
	(*osTE)->selEnd = oldSelection.GetTo ()-1;
}

void	TextEdit_MacUI_Native::ResetEffectiveStyle ()
{
	Font	newFont = GetEffectiveFont ();
	Font	f = fEffectiveStyle.GetFont ();

	if (newFont != f) {
		struct	TERec**	osTE	=	GetOSRepresentation ();

		osTextStyle	oldTextStyle;
		oldTextStyle.tsFont = f.GetID ();
		oldTextStyle.tsFace = f.GetStyleOSRep ();
		oldTextStyle.tsSize = f.GetSize ();
		fEffectiveStyle.GetColor ().GetRGB (oldTextStyle.tsColor.red, oldTextStyle.tsColor.green, oldTextStyle.tsColor.blue);	
	
		fEffectiveStyle.SetFont (newFont);
		f = fEffectiveStyle.GetFont ();
		osTextStyle	textStyle;
		textStyle.tsFont = f.GetID ();
		textStyle.tsFace = f.GetStyleOSRep ();
		textStyle.tsSize = f.GetSize ();
		fEffectiveStyle.GetColor ().GetRGB (textStyle.tsColor.red, textStyle.tsColor.green, textStyle.tsColor.blue);	
		
		TextSelection	oldSelection = GetSelection ();
		(*osTE)->selStart = 0;
		(*osTE)->selEnd = 32000;
		::TEReplaceStyle (doAll, &oldTextStyle, &textStyle, False, osTE);
		(*osTE)->selStart = oldSelection.GetFrom ()-1;
		(*osTE)->selEnd = oldSelection.GetTo ()-1;
		
		ResetNilRun ();
		
		if ((GetTablet () != Nil) and (GetSize ().GetH () > 8)) {
			osGrafPort*	aPort = qd.thePort;
			PrepareForDraw (eNoUpdate);
			::TECalText (GetOSRepresentation ());
			if (qd.thePort == GetWorkPort ()) {
				::SetPort (aPort);
			}
		}
		if (GetAutoSize (Point::eVertical) or GetAutoSize (Point::eHorizontal)) {
			AdjustSize (eDelayedUpdate);
		}
	}
}

pascal	unsigned char	TextEdit_MacUI_Native::sClickLoopProc ()
{
	AssertNotNil (sCurrentTE);
	sCurrentTE->TrackingDrag (sCurrentTE->AncestorToLocal (EventManager::Get ().GetMouseLocation (), &DeskTop::Get ()));
	sCurrentTE->PrepareForDraw (eImmediateUpdate);		// reset QuickDraw params
	return (True);
}
#endif	/*qMacToolkit*/










/*
 ********************************************************************************
 *************************** AbstractTextEdit_MotifUI  *************************
 ********************************************************************************
 */
AbstractTextEdit_MotifUI::~AbstractTextEdit_MotifUI ()
{
}

AbstractTextEdit_MotifUI::AbstractTextEdit_MotifUI (TextController* controller):
	   AbstractTextEdit (controller)
{
	SetBorder (2, 2, eNoUpdate);
	SetMargin (2, 2, eNoUpdate);
}










#if		qXmToolkit
/*
 ********************************************************************************
 **************************** TextEdit_MotifUI_Native **************************
 ********************************************************************************
 */


#define	qUseWrapper	0


TextEdit_MotifUI_Native::TextEdit_MotifUI_Native (TextController* controller):
	AbstractTextEdit_MotifUI (controller),
	fEffectiveStyle (kApplicationFont, kBlackColor),
	fWrapperControl (Nil),
	fTextEditControl (Nil),
	fText_UnRealized (kEmptyString),
	fJustification (eJustLeft),
	fWordWrap (True),
	fSelection (1, 1)
{
	SetFont (&kApplicationFont);

	class	MyOSTextEdit : public MotifOSControl {
		public:
			MyOSTextEdit (TextEdit_MotifUI_Native& te):
				MotifOSControl (),
			  	fTextEdit (te)
				{
				}	
			override	osWidget*	DoBuild_ (osWidget* parent)
				{
					RequireNotNil (parent);
					osWidget* w = ::XmCreateText (parent, "TextEdit", Nil, 0);
					char*	tmp = fTextEdit.GetText ().ToCString ();
					AssertNotNil (w);
					::XmTextSetString (w, tmp);
					delete (tmp);
					
					return (w);
				}

			TextEdit_MotifUI_Native&	fTextEdit;
	};


	class	MyWrapper : public MotifOSControl {
		public:
			MyWrapper (TextEdit_MotifUI_Native& te):
				MotifOSControl ()
				{
					AddSubView (fMyOSTextEdit = new MyOSTextEdit (te));
				}	
			override	osWidget*	DoBuild_ (osWidget* parent)
				{
					RequireNotNil (parent);
					struct { Arg args[3];} hack;
					XtSetArg (hack.args[0], XmNmarginWidth, 0);
					XtSetArg (hack.args[1], XmNmarginHeight, 0);
					XtSetArg (hack.args[2], XmNshadowThickness, 0);
					return (::XmCreateDrawingArea (parent, "TextEditWrapper", hack.args, 3));
				}
		  public:
			MyOSTextEdit*	fMyOSTextEdit;
	};

#if		qUseWrapper
	MyWrapper*	wrapper	= new MyWrapper (*this);
	AddSubView (wrapper);
	AssertNotNil (wrapper);
	AssertNotNil (wrapper->fMyOSTextEdit);
	fWrapperControl = wrapper;
	fTextEditControl = wrapper->fMyOSTextEdit;
#else
	AddSubView (fTextEditControl = new MyOSTextEdit (*this));
#endif
	SetBorder_ (GetBorder (), eNoUpdate);
	SetMargin_ (GetMargin (), eNoUpdate);
}

TextEdit_MotifUI_Native::~TextEdit_MotifUI_Native ()
{
	Require (not GetEffectiveLive ());
}

Boolean	TextEdit_MotifUI_Native::TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState)
{
#if 0
	Require (GetEffectiveLive ());
	kIBeamCursor.Install ();
	mouseRgn = GetLocalExtent ();
	return (True);
#else
	return (AbstractTextEdit::TrackMovement (cursorAt, mouseRgn, keyBoardState));
#endif
}

Boolean	TextEdit_MotifUI_Native::TrackPress (const MousePressInfo& mouseInfo)
{
	Require (GetEffectiveLive ());
// Should we add && CanBeFocused () to test before GrabFocus???? I think so - TRY ... LGP - March 5, 1992
	if (not GetEffectiveFocused () and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
// WHY IS THIS COMMENTED OUT??? LGP - March 5, 1992
//		return (True);
	}

	Boolean	result = AbstractTextEdit::TrackPress (mouseInfo);	
	MenuOwner::SetMenusOutOfDate ();
	return (result);
}

void	TextEdit_MotifUI_Native::DoSetupMenus ()
{
	AbstractTextEdit::DoSetupMenus ();
}

Boolean	TextEdit_MotifUI_Native::DoCommand (const CommandSelection& selection)
{
	return (AbstractTextEdit::DoCommand (selection));
}

Boolean	TextEdit_MotifUI_Native::DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
					KeyComposeState& composeState)
{

PlusChangeCount ();// hack...
	AssertNotNil (fTextEditControl);
	
	KeyComposeState tmpKeyComposeState = composeState;
	KeyStroke keyStroke;
	if (ApplyKeyEvent (keyStroke, code, isUp, keyBoardState, tmpKeyComposeState)) {
		if (not ValidateKey (keyStroke)) {
			return (False);
		}
	}
	return (fTextEditControl->DispatchKeyEvent (code, isUp, keyBoardState, composeState));
}

Boolean	TextEdit_MotifUI_Native::HandleKeyStroke (const KeyStroke& keyStroke)
{
	return (AbstractTextEdit::HandleKeyStroke (keyStroke));
}

void	TextEdit_MotifUI_Native::Layout ()
{
	AssertNotNil (fTextEditControl);

#if		qUseWrapper
	fWrapperControl->SetOrigin (kZeroPoint, eNoUpdate);
	fWrapperControl->SetSize (GetSize (), eNoUpdate);

	Rect bounds = EnclosureToLocal (GetVisibleArea ()).GetBounds ()*GetLocalExtent ();
	fTextEditControl->SetOrigin (bounds.GetOrigin (), eNoUpdate);
	fTextEditControl->SetSize (bounds.GetSize (), eNoUpdate);
#else
	fTextEditControl->SetOrigin (kZeroPoint, eNoUpdate);
	fTextEditControl->SetSize (GetSize (), eNoUpdate);
#endif

	AbstractTextEdit::Layout ();
}

void	TextEdit_MotifUI_Native::SetTextActive (Boolean active, UpdateMode updateMode)
{
	AssertNotNil (fTextEditControl);
	fTextEditControl->SetBooleanResourceValue (XmNsensitive, GetEffectiveLive ());
	Ensure (fTextEditControl->GetBooleanResourceValue (XmNsensitive) == GetEffectiveLive ());

	if ((fTextEditControl->GetWidget () != Nil) and active) {
		::XmProcessTraversal (fTextEditControl->GetWidget (), XmTRAVERSE_CURRENT);
	}
}

void	TextEdit_MotifUI_Native::Draw (const Region& update)
{
#if		qUseWrapper
Rect bounds = EnclosureToLocal (GetVisibleArea ()).GetBounds () * GetLocalExtent ();
//if (bounds != fTextEditControl->GetExtent ()) {
gDebugStream << "in DRAW: adjusting bounds of TE control to " << bounds << "from " << fTextEditControl->GetExtent () << newline;
//}

	fTextEditControl->SetOrigin (bounds.GetOrigin (), eNoUpdate);
	fTextEditControl->SetSize (bounds.GetSize (), eNoUpdate);
//	fTextEditControl->ProcessLayout ();// ProcessLayout NOW since we have have changed size, and be about to draw these guys...
ProcessLayout ();
#endif

	AbstractTextEdit::Draw (update);
}

Point	TextEdit_MotifUI_Native::CalcDefaultSize_ (const Point& defaultSize) const
{
// This code is terrible!!! Way off - LGP March 16, 1992

	Font	f = GetEffectiveFont ();

	Point	newSize	=	defaultSize;

//	if ((newSize.GetV () == 0) or (not GetMultiLine ())) {
//		newSize.SetV (f.GetLineHeight ());
//	}
	UInt32	lineCount	=	GetLineCount ();
	if (lineCount == 0) { lineCount = 1; }
	newSize.SetV (f.GetLineHeight () * lineCount);

//	if (newSize.GetH () < GetLength ()*12) {
if (GetTablet () == Nil) {
	newSize.SetH (GetLength ()*12);
}
else {
	newSize.SetH (TextWidth (GetText (), f, kZeroPoint));
if ((GetBorder ().GetH () == 0) and (GetMargin ().GetH () == 0)) {
newSize.SetH (newSize.GetH () + 8); // totally black magic hack
}
}
//	}

	return (newSize + GetBorder ()*2 + GetMargin ()*2);
}

CollectionSize	TextEdit_MotifUI_Native::GetLength () const
{
	AssertNotNil (fTextEditControl);

	if (fTextEditControl->GetOSRepresentation () == Nil) {
		return (fText_UnRealized.GetLength ());
	}
	else {
		int length = ::XmTextGetLastPosition (fTextEditControl->GetOSRepresentation ());
		return (length);
	}
	AssertNotReached ();
}

TextSelection	TextEdit_MotifUI_Native::GetSelection () const
{
	AssertNotNil (fTextEditControl);
	if (fTextEditControl->GetOSRepresentation () == Nil) {
		return (fSelection);
	}

	XmTextPosition	left = 0;
	XmTextPosition	right = 0;
	XmTextGetSelectionPosition (fTextEditControl->GetOSRepresentation (), &left, &right);
	return (TextSelection (left + 1, right + 1));	
}

void	TextEdit_MotifUI_Native::SetSelection (UpdateMode updateMode)
{
	SetSelection (TextSelection (1, GetLength ()+1), updateMode);
}

void	TextEdit_MotifUI_Native::SetSelection (const TextSelection& selection, UpdateMode updateMode)
{
	Require (selection.GetFrom () >= 1);

	AssertNotNil (fTextEditControl);
	if (fTextEditControl->GetOSRepresentation () == Nil) {
		fSelection = selection;
	}
	else {
		XmTextPosition	left = selection.GetFrom () - 1;
		XmTextPosition	right = selection.GetTo () -1;
		XmTextSetSelection (fTextEditControl->GetOSRepresentation (), left, right, CurrentTime);
	}
	
	Ensure (GetSelection () == selection);
}

String	TextEdit_MotifUI_Native::GetText_ ()	const
{
	if (fTextEditControl->GetOSRepresentation () == Nil) {
		return (fText_UnRealized);
	}
	else {
		AssertNotNil (fTextEditControl->GetOSRepresentation ());
		char*	tmp = ::XmTextGetString (fTextEditControl->GetOSRepresentation ());
 		AssertNotNil (tmp);
 		String answer = tmp;
		::XFree (tmp);
		return (answer);
	}
}

String	TextEdit_MotifUI_Native::GetText_ (const TextSelection& selection) const
{
	AssertNotNil (fTextEditControl);

	String	theText = GetText_ ();
	if (selection.TextIsSelected ()) {
		CollectionSize	stringTo = selection.GetTo () - 1;
		Assert (stringTo >= selection.GetFrom ());
		Assert (stringTo <= theText.GetLength ());
		return (theText.SubString (selection.GetFrom (), stringTo-selection.GetFrom ()+1));
	}
	return (kEmptyString);
}

void	TextEdit_MotifUI_Native::SetText_ (const String& s, UpdateMode updateMode)
{
	if (s.GetLength () > GetMaxLength ()) {
		s.SetLength (GetMaxLength ());
	}

	if (fTextEditControl->GetOSRepresentation () == Nil) {
		fText_UnRealized = s;
	}
	else {
		char*	tmp = s.ToCString ();
		AssertNotNil (fTextEditControl->GetOSRepresentation ());
		::XmTextSetString (fTextEditControl->GetOSRepresentation (), tmp);
		delete (tmp);
	}
// hack city - mac code did thise, and it appears we must too..
	PlusChangeCount ();
}

void	TextEdit_MotifUI_Native::SetText_ (const String& s, const TextSelection& selection, UpdateMode updateMode)
{
	if (s.GetLength () > GetMaxLength ()) {
		s.SetLength (GetMaxLength ());
	}

	String	theText = GetText ();
	
	String	prepend;
	String	append;
	
	Assert (selection.GetFrom () <= (theText.GetLength ()+1));
	Assert (selection.GetTo () <= (theText.GetLength ()+1));
	if (selection.GetFrom () > 1) {
		prepend = theText.SubString (1, selection.GetFrom ()-1);
	}
	if (selection.GetTo () <= theText.GetLength ()) {
		append  = theText.SubString (selection.GetTo (), theText.GetLength ()-selection.GetTo ()+1);
	}
	theText = prepend + s + append;

	if (fTextEditControl->GetOSRepresentation () == Nil) {
		fText_UnRealized = theText;
		// hack city - mac code did thise, and it appears we must too..
		PlusChangeCount ();
	}
	else {
		SetText_ (theText, updateMode);
	}
}

TextStyle	TextEdit_MotifUI_Native::GetStyle (CollectionSize i) const
{
	return (TextStyle (GetEffectiveFont (), kBlackColor));
}

void	TextEdit_MotifUI_Native::SetStyle (const TextStyle& style, UpdateMode update)
{
	SetStyle (style, TextSelection (1, GetLength () + 1), update);
}

void	TextEdit_MotifUI_Native::SetStyle (const TextStyle& style, const TextSelection& selection, UpdateMode update)
{
	Font f = style.GetFont ();
	SetFont (&f);
}

TextRun		TextEdit_MotifUI_Native::GetTextRun (CollectionSize index) const
{
	return (TextRun (GetStyle (1), TextSelection (1, GetLength () + 1)));
}

TextSelection	TextEdit_MotifUI_Native::Find (const String& pattern, CollectionSize i)
{
AssertNotReached ();
return (TextSelection (1, 1));
}

TextSelection	TextEdit_MotifUI_Native::Replace (const String& pattern, const String& newText, CollectionSize i)
{
AssertNotReached ();
return (TextSelection (1, 1));
} 

void	TextEdit_MotifUI_Native::ReplaceAll (const String& pattern, const String& newText)
{
AssertNotReached ();
}

Justification	TextEdit_MotifUI_Native::GetJustification_ () const
{
	return (fJustification);
}

void	TextEdit_MotifUI_Native::SetJustification_ (Justification justification, UpdateMode updateMode)
{
	fJustification = justification;
#if		qXmToolkit & 0
	// Motif doesn't seem to support justification for XmText or XmTextField - only for XmLabel!!!!
	// not too sure what we should do about this???
	switch (fJustification) {
		case	eJustLeft:		fTextEditControl->SetUnsignedCharResourceValue (XmNalignment, XmALIGNMENT_BEGINNING); break;
		case	eJustCenter:	fTextEditControl->SetUnsignedCharResourceValue (XmNalignment, XmALIGNMENT_CENTER); break;
		case	eJustRight:		fTextEditControl->SetUnsignedCharResourceValue (XmNalignment, XmALIGNMENT_END); break;
		case	eJustFull:		fTextEditControl->SetUnsignedCharResourceValue (XmNalignment, XmALIGNMENT_BEGINNING); break;
		case eSystemDefined:	fTextEditControl->SetUnsignedCharResourceValue (XmNalignment, XmALIGNMENT_BEGINNING); break;
		default:	RequireNotReached ();
	}
#endif
	Refresh (updateMode);
}

Boolean	TextEdit_MotifUI_Native::GetWordWrap_ () const
{
	return (fWordWrap);
}

void	TextEdit_MotifUI_Native::SetWordWrap_ (Boolean wordWrap, UpdateMode updateMode)
{
	fWordWrap = wordWrap;
	AssertNotNil (fTextEditControl);
	fTextEditControl->SetBooleanResourceValue (XmNwordWrap, wordWrap);	
	Refresh (updateMode);
}

Point	TextEdit_MotifUI_Native::GetPosition (CollectionSize i) const
{
	if ((fTextEditControl != Nil) and (fTextEditControl->GetOSRepresentation () != Nil)) {
		XmTextPosition position = i - 1;
		osPosition x, y;
		if (::XmTextPosToXY (fTextEditControl->GetOSRepresentation (), position, &x, &y)) {
			return (Point (y, x));
		}
	}
	
	return (kZeroPoint);
}

CollectionSize	TextEdit_MotifUI_Native::GetOffset (const Point& p) const
{
	if ((fTextEditControl != Nil) and (fTextEditControl->GetOSRepresentation () != Nil)) {
		osPosition x  = p.GetH ();
		osPosition y  = p.GetV ();
		
		CollectionSize pos = ::XmTextXYToPos (fTextEditControl->GetOSRepresentation (), x, y);
		return (pos + 1);
	}
	
	return (1);
}

String	TextEdit_MotifUI_Native::GetNearestWord (const Point& p) const
{
	return (GetNearestWord (GetOffset (p)));
}

String	TextEdit_MotifUI_Native::GetNearestWord (CollectionSize offset) const
{
	String	s = GetText ();
	CollectionSize	length = s.GetLength ();
	if ((offset > 0) and (offset <= length)) {
		if (isalnum (s[offset])) {
			for (CollectionSize	startOffset = offset-1; startOffset > 1; startOffset--) {
				if (not isalnum (s[startOffset])) {
					startOffset++;
					break;
				}
			}
			for (CollectionSize	endOffset = offset; endOffset < length; endOffset++) {
				if (not isalnum (s[endOffset])) {
					endOffset--;
					break;
				}
			}
			Assert (startOffset <= endOffset);
			return (s.SubString (startOffset, endOffset-startOffset+1));
		}
	}
	return (kEmptyString);
}

CollectionSize	TextEdit_MotifUI_Native::GetLineCount () const
{
// quick hack - not really right...
	String			s		=	GetText ();
	CollectionSize	length	=	s.GetLength ();
	CollectionSize	nLines	=	0;
	for (CollectionSize i = 1; i <= length; i++) {
		if (s[i] == '\n') {
			nLines++;
		}
	}

// if no text, say zero lines, otherwise count as a line something wiht no newline at end - not quite rifgh
// but hopefully close - make same as mac definition - rething defintiion carefully!!
	return ((length==0)? 0: (nLines+1));
}

String		TextEdit_MotifUI_Native::GetLine (CollectionSize i) const
{
	AssertNotImplemented ();
}

TextSelection	TextEdit_MotifUI_Native::GetLineLocation (CollectionSize line) const
{
	AssertNotImplemented ();
}

void	TextEdit_MotifUI_Native::AdjustSize (UpdateMode updateMode)
{
	Point	size		=	GetSize ();
	Point	defaultSize =	CalcDefaultSize (size);

	if (GetAutoSize (Point::eVertical)) {
		size.SetV (defaultSize.GetV ());
	}
	if (GetAutoSize (Point::eHorizontal)) {
		const	Coordinate	kPageWidth	=	32000;		// hack copied from apple version
		size.SetH (kPageWidth);
	}
	SetSize (size, updateMode);
}

void	TextEdit_MotifUI_Native::DoCut (UpdateMode updateMode)
{
	AssertNotNil (fTextEditControl->GetOSRepresentation ());
	::XmTextCut (fTextEditControl->GetOSRepresentation (), CurrentTime);
	PlusChangeCount ();
}

void	TextEdit_MotifUI_Native::DoCopy (UpdateMode /*updateMode*/)
{
	AssertNotNil (fTextEditControl->GetOSRepresentation ());
	::XmTextCopy (fTextEditControl->GetOSRepresentation (), CurrentTime);
	MenuOwner::SetMenusOutOfDate ();
}

void	TextEdit_MotifUI_Native::DoPaste (UpdateMode updateMode)
{
	AssertNotNil (fTextEditControl->GetOSRepresentation ());
	::XmTextPaste (fTextEditControl->GetOSRepresentation ());
	PlusChangeCount ();
}

void	TextEdit_MotifUI_Native::DoClear (UpdateMode updateMode)
{
	Assert (GetEffectiveLive ());
	AssertNotNil (fTextEditControl->GetOSRepresentation ());
	::XmTextRemove (fTextEditControl->GetOSRepresentation ());
	PlusChangeCount ();
}

void	TextEdit_MotifUI_Native::DoKeyPress (Character c, UpdateMode /*updateMode*/)
{

/// HACK - we dont have right info here!!!
// UGGGG
KeyComposeState composeState;
(void)fTextEditControl->DispatchKeyEvent (c, False, GetKeyBoard (), composeState);
	PlusChangeCount ();
}

void	TextEdit_MotifUI_Native::SetBorder_ (const Point& border, UpdateMode updateMode)
{
	AbstractTextEdit::SetBorder_ (border, eNoUpdate);
	if (fTextEditControl != Nil) {
		fTextEditControl->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorderHeight (), GetBorderWidth ()));
	}
	Refresh (updateMode);
}

void	TextEdit_MotifUI_Native::SetMargin_ (const Point& margin, UpdateMode updateMode)
{
	AbstractTextEdit::SetMargin_ (margin, eNoUpdate);
	if (fTextEditControl != Nil) {
		fTextEditControl->SetDimensionResourceValue (XmNmarginHeight, (osDimension)GetMarginHeight ());
    	fTextEditControl->SetDimensionResourceValue (XmNmarginWidth, (osDimension)GetMarginWidth ());
	}
	Refresh (updateMode);
}

void	TextEdit_MotifUI_Native::SetMaxLength_ (CollectionSize maxLength)
{
	AbstractTextEdit::SetMaxLength_ (maxLength);
	AssertNotNil (fTextEditControl);
	fTextEditControl->SetIntResourceValue (XmNmaxLength, GetMaxLength ());
}

void	TextEdit_MotifUI_Native::SetMultiLine_ (Boolean multiLine)
{
	AbstractTextEdit::SetMultiLine_ (multiLine);
	AssertNotNil (fTextEditControl);
}

void	TextEdit_MotifUI_Native::Realize (osWidget* parent)
{
	// Dont call inherited version since we call our childs realize directly...
	RequireNotNil (parent);
	AssertNotNil (fTextEditControl);

#if		qUseWrapper
	fWrapperControl->Realize (parent);
#endif

	String	text	=	GetText_ ();		// call before we realize so GetText code knows to use unrealized value
#if		qUseWrapper
	fTextEditControl->Realize (fWrapperControl->GetOSRepresentation ());
#else
	fTextEditControl->Realize (parent);
#endif
	AssertNotNil (fTextEditControl->GetOSRepresentation ());
	SetText_ (text, eNoUpdate);
	SetJustification_ (GetJustification_ (), eNoUpdate);
	SetWordWrap_ (GetWordWrap_ (), eNoUpdate);
	fTextEditControl->SetBooleanResourceValue (XmNsensitive, GetEffectiveLive ());
	fTextEditControl->SetIntResourceValue (XmNeditMode, XmMULTI_LINE_EDIT);	
	fTextEditControl->SetDimensionResourceValue (XmNmarginHeight, (osDimension)GetMarginHeight ());
    fTextEditControl->SetDimensionResourceValue (XmNmarginWidth, (osDimension)GetMarginWidth ());
	fTextEditControl->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorderHeight (), GetBorderWidth ()));
	fTextEditControl->SetIntResourceValue (XmNmaxLength, GetMaxLength ());
	
	SetSelection (fSelection);	// now that I'm realized, will take my temp value and stick it into widget
}

void	TextEdit_MotifUI_Native::UnRealize ()
{
	AssertNotNil (fTextEditControl);

	if (fTextEditControl->GetWidget () != Nil) {	// otherwise we already are unrealized
		String	text	=	GetText_ ();		// call before we unrealize so GetText code knows to use realized value
		TextSelection selection = GetSelection ();
	
		fTextEditControl->UnRealize ();
#if		qUseWrapper
		fWrapperControl->UnRealize ();
#endif
		Assert (fTextEditControl->GetOSRepresentation () == Nil);
		SetText_ (text, eNoUpdate);
		SetSelection (selection);
	}
}
#endif	/*qXmToolkit*/












/*
 ********************************************************************************
 ******************************* TextRunIterator ********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	Implement (Iterator, TextRun);
#endif




TextRunIterator::TextRunIterator (AbstractTextEdit&	textEdit, TextSelection selection) :
	Iterator(TextRun) (),
	fSelection (selection),
	fTextEdit (textEdit),
	fCurrentRun (textEdit.GetTextRun (selection.GetFrom ())),
	fDone (False)
{
}

Boolean	TextRunIterator::Done () const
{
	return (fDone);
}

void	TextRunIterator::Next ()
{
	Require (not fDone);
	if (fCurrentRun.GetSelection ().GetTo () >= fSelection.GetTo ()) {
		fDone = True;
	}
	else {
		fCurrentRun = fTextEdit.GetTextRun (fCurrentRun.GetSelection ().GetTo () + 1);
	}
}

TextRun	TextRunIterator::Current () const
{
	return (fCurrentRun);
}




/*
 ********************************************************************************
 *********************************** TextEdit_MacUI *****************************
 ********************************************************************************
 */
TextEdit_MacUI::TextEdit_MacUI (TextController* controller) :
#if		qNative && qMacToolkit
	TextEdit_MacUI_Native (controller)
#else
	// should be portable TE engine
	TextEdit_MacUI_Native (controller)
#endif
{
}
		
#if		qSoleInlineVirtualsNotStripped
TextEdit_MacUI::~TextEdit_MacUI ()
{
}
#endif

void	TextEdit_MacUI::Draw (const Region& update)
{
	DrawMacBorder (GetLocalExtent (), GetBorder ());
#if		qNative && qMacToolkit
	TextEdit_MacUI_Native::Draw (update);
#else
	TextEdit_MacUI_Native::Draw (update);
#endif
}


/*
 ********************************************************************************
 ********************************* TextEdit_MotifUI *****************************
 ********************************************************************************
 */
TextEdit_MotifUI::TextEdit_MotifUI (TextController* controller) :
#if		qNative && qXmToolkit
	TextEdit_MotifUI_Native (controller)
#else
	// should be portable TE engine
	TextEdit_MacUI_Native (controller)
#endif
{
}


#if		qSoleInlineVirtualsNotStripped
TextEdit_MotifUI::~TextEdit_MotifUI ()
{
}
#endif

void	TextEdit_MotifUI::Draw (const Region& update)
{
#if		qNative && qXmToolkit
	TextEdit_MotifUI_Native::Draw (update);
#else
	Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawMotifBorder (GetLocalExtent (), GetBorder (), c, GetPlane ());
	TextEdit_MacUI_Native::Draw (update);
#endif
}




/*
 ********************************************************************************
 ********************************* TextEdit_WinUI *****************************
 ********************************************************************************
 */
TextEdit_WinUI::TextEdit_WinUI (TextController* controller) :
	// should be portable TE engine
	TextEdit_MacUI_Native (controller)
{
}


#if		qSoleInlineVirtualsNotStripped
TextEdit_WinUI::~TextEdit_WinUI ()
{
}
#endif

void	TextEdit_WinUI::Draw (const Region& update)
{
	Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawMotifBorder (GetLocalExtent (), GetBorder (), c, GetPlane ());
	TextEdit_MacUI_Native::Draw (update);
}







/*
 ********************************************************************************
 ***************************************** TextEdit *****************************
 ********************************************************************************
 */

TextEdit::TextEdit (TextController* controller):
#if		qMacUI
	TextEdit_MacUI (controller)
#elif	qMotifUI
	TextEdit_MotifUI (controller)
#elif	qWinUI
	TextEdit_WinUI (controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
TextEdit::~TextEdit () {}
#endif








/*
 ********************************************************************************
 *************************************** TECommand ******************************
 ********************************************************************************
 */

TECommand::TECommand (AbstractTextEdit& te, CommandNumber cmdNumber):
	Command (cmdNumber),
	fTextEdit (te),
	fSelection (fTextEdit.GetSelection ()),
	fOldChangeCount (te.GetChangeCount ()),
	fOldSelectedText (te.GetText (te.GetSelection ())),
	fNewTextSelection (1,1)
{
#if		qXmToolkit
// LGP hack - since broken - March 16, 1992
	SetUndoable (False);
#endif
}

void	TECommand::DoIt ()
{
	Command::DoIt ();		
	fTextEdit.SetSelection (fSelection);
	DoTextEdit ();
	fNewTextSelection = TextSelection (fSelection.GetTo (), fTextEdit.GetSelection ().GetFrom ());
}

void	TECommand::UnDoIt ()
{
	Command::UnDoIt ();
	if (fNewTextSelection.GetTo () > 1) {
		fTextEdit.SetText (fOldSelectedText, fNewTextSelection);				
	}
	else {
		fTextEdit.SetText (fOldSelectedText);				
	}
	fTextEdit.SetSelection (fSelection);
	fTextEdit.SetChangeCount (fOldChangeCount);
}







/*
 ********************************************************************************
 ************************************ TEKeyCommand ******************************
 ********************************************************************************
 */
TEKeyCommand::TEKeyCommand (AbstractTextEdit& te, Character c) :
	TECommand (te, CommandHandler::eKeyPress),
	fCharacter (c),
	fCommitsLastCommand (True),
	fDidUndo (False)
{
	SetUndoable (True);
}

void	TEKeyCommand::DoTextEdit ()
{
	fTextEdit.DoKeyPress (fCharacter, (fDidUndo? View::eDelayedUpdate: View::eImmediateUpdate));
}

void	TEKeyCommand::UnDoIt ()
{
	fDidUndo = True;
	TECommand::UnDoIt ();
}

nonvirtual	void	TEKeyCommand::SetCommitsLast (Boolean commits)
{
	fCommitsLastCommand = commits;
}

Boolean		TEKeyCommand::CommitsLastCommand () const
{
	return (fCommitsLastCommand);
}




/*
 ********************************************************************************
 ************************************ TEClipCommand *****************************
 ********************************************************************************
 */
TEClipCommand::TEClipCommand (AbstractTextEdit& te, CommandNumber cmdNumber):
	TECommand (te, cmdNumber)
{
	switch (GetCommandNumber ()) {
		case	CommandHandler::eCut:		SetUndoable (True); break;
		case	CommandHandler::eCopy:		SetUndoable (False); break;
		case	CommandHandler::ePaste:		SetUndoable (True); break;
		case	CommandHandler::eClear:		SetUndoable (True); break;
		default:							RequireNotReached ();
	}
}

void	TEClipCommand::DoTextEdit ()
{
	switch (GetCommandNumber ()) {
		case	CommandHandler::eCut:		fTextEdit.DoCut (View::eImmediateUpdate); break;
		case	CommandHandler::eCopy:		fTextEdit.DoCopy (View::eImmediateUpdate); break;
		case	CommandHandler::ePaste:		fTextEdit.DoPaste (View::eImmediateUpdate); break;
		case	CommandHandler::eClear:		fTextEdit.DoClear (View::eImmediateUpdate); break;
		default:							AssertNotReached ();
	}
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

