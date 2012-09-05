/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Sources/LedItView.cpp,v 2.41 2000/10/19 15:51:35 lewis Exp $
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
 *	$Log: LedItView.cpp,v $
 *	Revision 2.41  2000/10/19 15:51:35  lewis
 *	use new portable About code and Find code. Further cleanups/improvements.
 *	
 *	Revision 2.40  2000/10/06 03:41:59  lewis
 *	call SetScrollBarType()
 *	
 *	Revision 2.39  2000/10/04 15:50:51  lewis
 *	cleanups and remove unneeded CMDINFO overrides - due to moving stuff to common code (SPR#0839).
 *	
 *	Revision 2.38  2000/10/03 21:51:12  lewis
 *	Lots more work relating to SPR#0839. Eliminated much duplicated code
 *	
 *	Revision 2.37  2000/06/16 20:27:37  lewis
 *	SPR#0785- got code to view ParaSpacing dialog working decently
 *	
 *	Revision 2.36  2000/06/16 02:20:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.35  2000/06/15 21:57:25  lewis
 *	a little closer to having SPR#0785 working - setting spacing from Mac test app
 *	
 *	Revision 2.34  2000/06/15 20:08:17  lewis
 *	preliminary work on UI for paragraph spacing - SPR#0785
 *	
 *	Revision 2.33  2000/06/14 13:59:50  lewis
 *	get (hopefully) compiling with new PickNewParagraphLineSpacing() cmd enabled- SPR#0785
 *	
 *	Revision 2.32  2000/06/12 20:15:20  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.31  2000/06/08 20:46:48  lewis
 *	SPR#0772- fix LedItView::OnSearchAgain - so if caret just before word being searched for - we actualyl find it
 *	
 *	Revision 2.30  2000/05/01 20:16:09  lewis
 *	lose DTOR call to SetStyleDatabase(NULL) - cuz triggers build of extra style database and internalizer/externalziers that trigger other cleanup bug - and dont help us any
 *	
 *	Revision 2.29  1999/12/28 19:35:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.28  1999/12/08 19:43:00  lewis
 *	Finish removing code that is replaced by new WordProcessorSelectionCommandHelper_PP(X)<> template - SPR#0606
 *	
 *	Revision 2.27  1999/12/08 17:46:13  lewis
 *	move some stuff to Led_PP.h and comment out more code as wev'e templateized it
 *	
 *	Revision 2.26  1999/12/08 03:17:54  lewis
 *	More progress towards getting new WordProcessorSelectionCommandHelper_PPX stuff working (SPR#0606).
 *	
 *	Revision 2.25  1999/12/07 22:38:56  lewis
 *	add LedItViewCommandInfo.  Related to spr#0606
 *	
 *	Revision 2.24  1999/03/25 23:31:29  lewis
 *	fix typo
 *	
 *	Revision 2.23  1999/03/25 23:28:32  lewis
 *	call new 'Interactive' version of commands to undo stuff works
 *	
 *	Revision 2.22  1998/10/30 15:04:11  lewis
 *	Use vector - not Led_Array.
 *	And start at kWrapToWindow and GetLayoutMargin() support
 *	
 *	Revision 2.21  1998/04/08  02:36:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.20  1997/12/24  04:42:23  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1997/12/24  03:57:16  lewis
 *	Fix for new compiler (Led_Color/explicit)
 *
 *	Revision 2.18  1997/09/29  17:58:16  lewis
 *	Lose qLedFirstIndex support.
 *	Add Justification menu support.
 *	New style database and paragraph database support.
 *	Revised SetWindowRect() override for change in base class APIs.
 *
 *	Revision 2.17  1997/07/27  16:02:25  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.16  1997/07/14  01:24:27  lewis
 *	Renamed TextStore_ to TextStore, and Led_PPView_ to Led_PPView.
 *
 *	Revision 2.15  1997/06/24  03:37:16  lewis
 *	Now override LedItView::GetLayoutWidth () to say wrap-to-window.
 *	And in SetWindowRect() hook - now we must call InvalidateAllCaches () since
 *	no more inheirted SetLayoutWidth() routine todo it.
 *
 *	Revision 2.14  1997/06/18  03:32:24  lewis
 *	Sub/Superscript command support.
 *
 *	Revision 2.13  1997/03/04  20:36:25  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1997/01/20  05:43:15  lewis
 *	Added font color support.
 *
 *	Revision 2.11  1997/01/10  03:46:04  lewis
 *	throw specifiers.
 *	override SetWindowRect () to set LayoutWidth for new scrolling support
 *
 *	Revision 2.10  1996/12/13  18:11:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.9  1996/12/05  21:25:04  lewis
 *	*** empty log message ***
 *
 *	Revision 2.8  1996/09/30  15:11:45  lewis
 *	No more need for OnExitFromThisFunctionDoThis - use DidUpdateText() override instead.
 *	Use LedItApplication::GetSaerchParameters() insetad of old per-view instance variables.
 *
 *	Revision 2.7  1996/09/01  15:45:40  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.6  1996/07/03  01:43:26  lewis
 *	Lose DLOG/DITL based dialogs, and use PPob-based ones now.
 *	Also, made smaller/larger font size stuff work with adjusting all sizes in run.
 *
 *	Revision 2.5  1996/05/23  20:47:06  lewis
 *	Lots of chagnes. Almost totally rewrote the font menu updating code. Now use new Led_IncremntalFontSpeciation,
 *	and GetContinuousStyelInfo() and cache that style info (later do better on caching code, and better on when/how to invalidate).
 *	Now properly handles cases like doing a bold style run in the middle of an existing italic one (one style valid flag wasn't enuf).
 *
 *	Revision 2.4  1996/05/14  20:50:17  lewis
 *	Added support for GetCommandHandler () == NULL case (mainly so I can
 *	do profiling with and without to test performance stuff, but cannot
 *	hurt materially todo the test...).
 *
 *	Revision 2.3  1996/05/05  14:56:31  lewis
 *	Fixed sense (backwards) of code associating fCaseSensativeSearch with
 *	the dialog.
 *
 *	Revision 2.2  1996/04/18  16:04:27  lewis
 *	Now update menu names for undo/redo.
 *
 *	Revision 2.1  1996/03/16  19:21:26  lewis
 *	Added support for OnEnterFindStringCommand ()
 *	Fixed minor bug with setting font styles (must set fontSpec.fFontStyleValid = true;)
 *
 *	Revision 2.0  1996/02/26  23:29:57  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#include	<stdlib.h>

#include	<AERegistry.h>
#include	<ColorPicker.h>
#include	<Dialogs.h>
#include	<TextUtils.h>

#include	<PP_Messages.h>
#include	<LMenu.h>
#include	<LMenuBar.h>
#include	<LEditField.h>
#include	<LStdControl.h>
#include	<UAppleEventsMgr.h>
#include	<UModalDialogs.h>
#include	<LDialogBox.h>

#include	"LedStdDialogs.h"
#include	"StyledTextEmbeddedObjects.h"

#include	"LedItResources.h"
#include	"LedItApplication.h"

#include	"LedItView.h"












/*
 ********************************************************************************
 ******************************* LedItViewCommandInfo ***************************
 ********************************************************************************
 */
void	LedItViewCommandInfo::DisplayFindDialog (Led_tString* findText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK)
{
	Led_StdDialogHelper_FindDialog	findDialog;

	findDialog.fFindText = *findText;
	findDialog.fWrapSearch = *wrapSearch;
	findDialog.fWholeWordSearch = *wholeWordSearch;
	findDialog.fCaseSensativeSearch = *caseSensative;

	findDialog.DoModal ();

	*findText = findDialog.fFindText;
	*wrapSearch = findDialog.fWrapSearch;
	*wholeWordSearch = findDialog.fWholeWordSearch;
	*caseSensative = findDialog.fCaseSensativeSearch;
	*pressedOK = findDialog.fPressedOK;
}

LedItViewCommandInfo::FontNameSpecifier	LedItViewCommandInfo::CmdNumToFontName (CommandNumber cmd)
{
	static	LMenu*			fontMenu	=	LMenuBar::GetCurrentMenuBar()->FetchMenu (cmd_FontMenu);
	static	vector<short>	sFontIDMapCache;	// OK to keep static cuz never changes during run of app

	Led_Require (cmd >= kBaseFontNameCmdID);
	Led_Require (cmd <= kLastFontNameCmdID);
	size_t	idx	=	cmd-kBaseFontNameCmdID;

	// Pre-fill cache - at least to the cmd were looking for...
	for (size_t i = sFontIDMapCache.size (); i <= idx; i++) {
		Str255		pFontName = {0};
		Int16		menuItem		=	fontMenu->IndexFromCommand (i+kBaseFontNameCmdID);
		::GetMenuItemText (fontMenu->GetMacMenuH (), menuItem, pFontName);
		short	familyID	=	0;
		::GetFNum (pFontName, &familyID);
		sFontIDMapCache.push_back (familyID);
	}
	return sFontIDMapCache[idx];
}

Led_Distance	LedItViewCommandInfo::PickOtherFontHeight (Led_Distance origHeight)
{
	StDialogHandler	findDialog (kOtherFontSize_DialogID, NULL);

	LEditField*		sizeText	=	(LEditField*)findDialog.GetDialog ()->FindPaneByID ('size');
	Led_AssertNotNil (sizeText);

	if (origHeight == 0) {
		sizeText->SetDescriptor ("\p");
	}
	else {
		sizeText->SetValue (origHeight);
	}

	sizeText->SelectAll (); SwitchTarget (sizeText);

	bool	hitOK	=	false;
	while (true) {
		MessageT	hitMessage = findDialog.DoDialog ();
		hitOK = (hitMessage == 'ok  ');
		if (hitMessage != 0) {
			break;
		}
	}

	return hitOK? sizeText->GetValue (): 0;	// returns zero if left empty, implying no change
}

	inline	static	bool	IsEmptyField (LEditField* f)
		{
			Led_RequireNotNil (f);
			Str255 s;
			f->GetDescriptor (s);
			return s[0] == 0;
		}
	inline	static	void	MyAppendMenuItem (MenuHandle h, ConstStr255Param text)
		{
			::AppendMenu (h, "\pXXX");
			::SetMenuItemText (h, ::CountMItems (h), text);
		}
bool	LedItViewCommandInfo::PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid)
{
	StDialogHandler	dlg (kParagraphSpacing_DialogID, NULL);

	LEditField*		sbText		=	dynamic_cast<LEditField*> (dlg.GetDialog ()->FindPaneByID ('sb  '));
	Led_AssertNotNil (sbText);

	LEditField*		saText		=	dynamic_cast<LEditField*> (dlg.GetDialog ()->FindPaneByID ('sa  '));
	Led_AssertNotNil (saText);

	LStdPopupMenu*	slPopup	=	dynamic_cast<LStdPopupMenu*> (dlg.GetDialog ()->FindPaneByID ('sl  '));
	Led_AssertNotNil (slPopup);

	LEditField*		slArgText	=	dynamic_cast<LEditField*> (dlg.GetDialog ()->FindPaneByID ('slag'));
	Led_AssertNotNil (slArgText);

	if (*spaceBeforeValid)	{	sbText->SetValue (*spaceBefore);	}		else {	sbText->SetDescriptor ("\p");	}
	if (*spaceAfterValid)	{	saText->SetValue (*spaceAfter);		}		else {	saText->SetDescriptor ("\p");	}

	sbText->SelectAll (); SwitchTarget (sbText);

	MenuHandle	mhPopup	=	slPopup->GetMacMenuH ();
	Led_AssertNotNil (mhPopup);
	MyAppendMenuItem (mhPopup, "\pSingle");
	MyAppendMenuItem (mhPopup, "\p1.5 lines");
	MyAppendMenuItem (mhPopup, "\pDouble");
	MyAppendMenuItem (mhPopup, "\pAt Least (TWIPS)");
	MyAppendMenuItem (mhPopup, "\pExact (TWIPS)");
	MyAppendMenuItem (mhPopup, "\pExact (1/20 lines)");
	if (*lineSpacingValid) {
		::SetControlMinimum (slPopup->GetMacControl (), 1);
		slPopup->SetMinValue (1);
		::SetControlMaximum (slPopup->GetMacControl (), 6);
		slPopup->SetMaxValue (6);
		::SetControlValue (slPopup->GetMacControl (), lineSpacing->fRule + 1);
		if (lineSpacing->fRule == Led_LineSpacing::eAtLeastTWIPSSpacing or lineSpacing->fRule == Led_LineSpacing::eExactTWIPSSpacing or lineSpacing->fRule == Led_LineSpacing::eExactLinesSpacing) {
			slArgText->SetValue (lineSpacing->fArg);
		}
	}

	bool	hitOK	=	false;
	while (true) {
		MessageT	hitMessage = dlg.DoDialog ();
		hitOK = (hitMessage == 'ok  ');
		if (hitMessage != 0) {
			break;
		}
	}

	if (hitOK) {
		*spaceBeforeValid = not IsEmptyField (sbText);
		if (*spaceBeforeValid) {
			*spaceBefore = Led_TWIPS (sbText->GetValue ());
		}

		*spaceAfterValid = not IsEmptyField (saText);
		if (*spaceAfterValid) {
			*spaceAfter = Led_TWIPS (saText->GetValue ());
		}

		int	r	=	::GetControlValue (slPopup->GetMacControl ());
		if (r >= 1 and r <= 6) {
			*lineSpacingValid = true;
			lineSpacing->fRule = Led_LineSpacing::Rule (r-1);
			if (lineSpacing->fRule == Led_LineSpacing::eAtLeastTWIPSSpacing or lineSpacing->fRule == Led_LineSpacing::eExactTWIPSSpacing or lineSpacing->fRule == Led_LineSpacing::eExactLinesSpacing) {
				lineSpacing->fArg = slArgText->GetValue ();
				if (not IsEmptyField (slArgText)) {
					*lineSpacingValid = false;
				}
			}
		}
	}
	return hitOK;
}






/*
 ********************************************************************************
 ********************************* LedItView ************************************
 ********************************************************************************
 */
LedItView::LedItView ():
	inherited ()
{
	SetSmartCutAndPasteMode (qSupportSmartCutNPaste);
	SetScrollBarType (h, kWrapToWindow? eScrollBarNever: eScrollBarAsNeeded);
	SetScrollBarType (v, eScrollBarAlways);
}

LedItView::~LedItView()
{
	SetCommandHandler (NULL);
	SpecifyTextStore (NULL);
}

void	LedItView::GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const
{
	if (kWrapToWindow) {
		// Make the layout width of each line (paragraph) equal to the windowrect. Ie, wrap to the
		// edge of the window. NB: because of this choice, we must 'InvalidateAllCaches' when the
		// WindowRect changes in our SetWindowRect() override.
		if (lhs != NULL) {
			*lhs = 0;
		}
		if (rhs != NULL) {
			*rhs = (Led_Max (GetWindowRect ().GetWidth (), 1));
		}
	}
	else {
		inherited::GetLayoutMargins (row, lhs, rhs);
	}
}

void	LedItView::SetWindowRect (const Led_Rect& windowRect)
{
	if (kWrapToWindow) {
		// Hook all changes in the window width, so we can invalidate the word-wrap info (see LedItView::GetLayoutMargins)
		if (windowRect != GetWindowRect ()) {
			WordWrappedTextImager::SetWindowRect (windowRect);
			InvalidateAllCaches ();
		}
	}
	else {
		//NB: Here and above - we call WordWrappedTextImager::SetWindowRect() instead of 'inherited' cuz a call to inherited
		// would invoke the INTERACTOR version which would recurse back to us infinitely. We must disambiguate, and pick the right
		// image one (should be done better in Led somehow? LGP 980908)
		WordWrappedTextImager::SetWindowRect (windowRect);
	}
}

void	LedItView::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, Char16& outMark, Str255 outName)
{
	outUsesMark = false;
	switch (inCommand) {
#if 0
		case	cmdFind:			outEnabled = true;											break;
		case	cmdFindAgain:		outEnabled = (LedItApplication::Get ().GetSearchParameters ().fMatchString.length () != 0);						break;
		case	cmdEnterFindString:	outEnabled = (GetSelectionStart () != GetSelectionEnd ());	break;
#endif
		case	cmd_FontMenu:			outEnabled = true;											break;
		case	cmd_SizeMenu:			outEnabled = true;											break;
		case	cmd_StyleMenu:			outEnabled = true;											break;
		case	cmd_ColorMenu:			outEnabled = true;											break;
		case	cmd_JustificationMenu:	outEnabled = true;											break;

		default: {
			inherited::FindCommandStatus (inCommand, outEnabled, outUsesMark, outMark, outName);
		}
		break;
	}
}

#if 0
Boolean	LedItView::ObeyCommand (CommandT inCommand, void* ioParam)
{
	switch (inCommand) {
		case cmdFind:				OnFindCommand ();				return true;
		case cmdFindAgain:			OnFindAgainCommand ();			return true;
		case cmdEnterFindString:	OnEnterFindStringCommand ();	return true;
		default: {
			return inherited::ObeyCommand (inCommand, ioParam);
		}
	}
	Led_Assert (false);	return false;
}
#endif

SimpleEmbeddedObjectStyleMarker*	LedItView::GetSoleSelectedEmbedding () const
{
	size_t	selStart	=	GetSelectionStart ();
	size_t	selEnd		=	GetSelectionEnd ();
	Led_Require (selStart <= selEnd);
	if (selEnd - selStart == 1) {
		vector<SimpleEmbeddedObjectStyleMarker*>	embeddings	=	CollectAllEmbeddingMarkersInRange (selStart, selEnd);
		if (embeddings.size () == 1) {
			return (dynamic_cast<SimpleEmbeddedObjectStyleMarker*> (embeddings[0]));
		}
		else {
			return NULL;
		}
	}
	else {
		return NULL;
	}
}

LedItView::SearchParameters	LedItView::GetSearchParameters () const
{
	return LedItApplication::Get ().GetSearchParameters ();
}

void	LedItView::SetSearchParameters (const SearchParameters& sp)
{
	LedItApplication::Get ().SetSearchParameters (sp);
}

#if 0
void	LedItView::OnFindCommand ()
{
	TextStore::SearchParameters	parameters	=	LedItApplication::Get ().GetSearchParameters ();

	StDialogHandler	findDialog (kFind_DialogID, NULL);

	LEditField*		findText	=	(LEditField*)findDialog.GetDialog ()->FindPaneByID ('ftxt');
	Led_AssertNotNil (findText);
	LStdCheckBox*	wrapAtEnd	=	(LStdCheckBox*)findDialog.GetDialog ()->FindPaneByID ('wrap');
	Led_AssertNotNil (wrapAtEnd);
	LStdCheckBox*	wholeWord	=	(LStdCheckBox*)findDialog.GetDialog ()->FindPaneByID ('whol');
	Led_AssertNotNil (wholeWord);
	LStdCheckBox*	ignoreCase	=	(LStdCheckBox*)findDialog.GetDialog ()->FindPaneByID ('igcs');
	Led_AssertNotNil (ignoreCase);
	{
		Str255	searchText;
		searchText[0] = parameters.fMatchString.length ();
		(void)::memcpy (&searchText[1], parameters.fMatchString.c_str (), searchText[0]);
		findText->SetDescriptor (searchText);
	}
	wrapAtEnd->SetValue (parameters.fWrapSearch);
	wholeWord->SetValue (parameters.fWholeWordSearch);
	ignoreCase->SetValue (not parameters.fCaseSensativeSearch);
	findText->SelectAll (); SwitchTarget (findText);

	bool	hitOK	=	false;
	while (true) {
		MessageT	hitMessage = findDialog.DoDialog ();
		hitOK = (hitMessage == 'find');
		if (hitMessage != 0) {
			break;
		}
	}

	{
		Str255	searchText;
		findText->GetDescriptor (searchText);
		parameters.fMatchString = string ((char*)&searchText[1], searchText[0]);
	}
	parameters.fWrapSearch = wrapAtEnd->GetValue ()==1;
	parameters.fWholeWordSearch = wholeWord->GetValue ()==1;
	parameters.fCaseSensativeSearch = ignoreCase->GetValue ()==0;

	LedItApplication::Get ().SetSearchParameters (parameters);

	if (hitOK) {
		OnFindAgainCommand ();
	}
}

void	LedItView::OnFindAgainCommand ()
{
	TextStore::SearchParameters	parameters	=	LedItApplication::Get ().GetSearchParameters ();

	// search for last text entered into find dialog (could have been a find again).
	size_t	origSelStart	=	GetSelectionStart ();
	size_t	origSelEnd		=	GetSelectionEnd ();
	size_t	whereTo	=	GetTextStore ().Find (parameters, origSelEnd);
	if ((whereTo == kBadIndex) or (whereTo == origSelStart and whereTo + parameters.fMatchString.length () == origSelEnd)) {
		Led_BeepNotify ();
	}
	else {
		SetSelection (whereTo, whereTo + parameters.fMatchString.length ());
		ScrollToSelection ();
	}
}

void	LedItView::OnEnterFindStringCommand ()
{
	TextStore::SearchParameters	parameters	=	LedItApplication::Get ().GetSearchParameters ();

	// Make currently selected text the new find string
	size_t	selStart	=	GetSelectionStart ();
	size_t	selEnd		=	GetSelectionEnd ();
	size_t	selLength	=	selEnd-selStart;

	Led_SmallStackBuffer<Led_tChar>	buf (selLength);
	CopyOut (selStart, selLength, buf);
	parameters.fMatchString = string (buf, selLength);
	LedItApplication::Get ().SetSearchParameters (parameters);
}

#endif





// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
