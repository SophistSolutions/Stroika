/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/TextInteractor.cpp,v 2.194 2004/02/11 23:57:56 lewis Exp $
 *
 * Changes:
 *	$Log: TextInteractor.cpp,v $
 *	Revision 2.194  2004/02/11 23:57:56  lewis
 *	SPR#1576: Update: added ReplaceAllInSelection functionality (except must re-layout MacOS dialog)
 *	
 *	Revision 2.193  2004/02/11 22:41:29  lewis
 *	SPR#1576: update: added 'Replace Again' command to LedIt, LedLineItMFC, ActiveLedIt
 *	
 *	Revision 2.192  2004/02/10 03:06:05  lewis
 *	SPR#1634: Added EnterIdler class to get notified on OnEnterIdle messages. Also - added IdleManager::NonIdleContext class to notify idle-manager of non-idle (eg executing a command) contexts (lots of these usages).
 *	
 *	Revision 2.191  2004/01/27 03:37:57  lewis
 *	SPR#1597: re-fix bug SPR#1232. Real fix was to be careful about EOR in wrapped lines in TextInteractor::GetCharAtClickLocation ()
 *	
 *	Revision 2.190  2004/01/26 20:03:33  lewis
 *	SPR#1604: RecomputeSelectionGoalColumn () on selchange, and use GoalColumnRecomputerControlContext class to avoid that recompute in the special case of using arrow keys to nagivate up/down (part of new, better goal-coumn support)
 *	
 *	Revision 2.189  2004/01/25 21:46:50  lewis
 *	SPR#1284: InternalizeFlavor_Specific/OnPasteCommand_PasteFlavor_Specific methods added to support 'PasteAsText' method for ActiveLedIt. Also (for same reason) - added PasteLooksLikeSmartCNP () helper and used in OnPasteCommand_PasteBestFlavor (code sharing)
 *	
 *	Revision 2.188  2003/12/31 04:13:13  lewis
 *	SPR#1580: new TextInteractor::SearchStrings class to replace (subclass from) TextStore::SearchParaemeters.
 *	Adds fRecentFindStrings. Changed override of TextInteractor::DialogSupport::DisplayFindDialog/
 *	TextInteractor::DialogSupport::DisplayReplaceDialog to take extra parameter. Added MergeRecentFindStrings ()
 *	method to combine search strings into non-infinite (non-redundant) list of recent strings.
 *	
 *	Revision 2.187  2003/12/17 20:57:55  lewis
 *	changed SpellCheckEngine::GetTextBreaksUsed() (returning Led_RefCntPtr<>) to PeekAtTextBreaksUsed ()
 *	returning TextBreaks*. Reason is we don't want to have to return a long-lived object. Interferes
 *	with COM stuff (be returning object part of COM object - and that could go away. Could add ref
 *	from that - but... not worth it here)
 *	
 *	Revision 2.186  2003/12/12 01:43:08  lewis
 *	use arg to AddWordToUD callback instead of re-grabbing text from buffer
 *	
 *	Revision 2.185  2003/12/11 19:18:29  lewis
 *	SPR#1593: add SpellCheckEngine::GetTextBreaksUsed () method so I can utilize the right
 *	TextBreaks routine from the spellcheck loop. Then use that in MyCallback::GetNextMisspelling ()
 *	so that we backup to the start of each word when beginning a chunk spellchecking, and more importantly -
 *	fix the code to actaully check successive chunks of no misspellings found in the first one
 *	
 *	Revision 2.184  2003/12/11 02:58:02  lewis
 *	SPR#1590: respoect AddToDictioanryEnabled callback value to disable the addtodict dialog
 *	button. Also - added similar OptionsDIalogEnabled callback and respected that so button
 *	disabled (til we've had time to implement an options dialog)
 *	
 *	Revision 2.183  2003/12/11 01:32:03  lewis
 *	SPR#1589: Implemented 'lookup on web' button in spell check dialog. Only tested/tuned
 *	dialog resource for Win32. Must test/tune for MacOS/XWin.
 *	
 *	Revision 2.182  2003/12/09 20:49:30  lewis
 *	SPR#1585: added UD support (add word to UD, new SpellCheckEngine_Basic_Simple, major revision
 *	to SpellCheckEngine_Basic::EditableDictionary, etc)
 *	
 *	Revision 2.181  2003/12/07 19:38:50  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.180  2003/11/27 03:59:56  lewis
 *	change default redo format string for Windows to CTRL+Y instead of +R
 *	
 *	Revision 2.179  2003/11/27 02:58:36  lewis
 *	SPR#1569: on typed NL - BreakInGroupedCommands ()
 *	
 *	Revision 2.178  2003/11/27 01:53:08  lewis
 *	SPR#1050: fix TextInteractor::DoSingleCharCursorEdit () to directly use UndoableContextHelper and
 *	InteractiveReplace_ () instead of calling SetSelection/InteractiveReplace. Use CLEAR cmd name for
 *	the command on a bakcspace. Use new BreakInGroupedCommandsIfDifferentCommand () so we get a clear
 *	break in cmd groups between typing and backsapces (which show up as clear cmds)
 *	
 *	Revision 2.177  2003/11/26 23:18:51  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.176  2003/11/26 20:04:37  lewis
 *	SPR#1564: major change to improve undo memory usage. Added SimplePlainTextInsertOptimization flag
 *	and when set (typing a single character) - we first try to just append the character to the 'afterRep',
 *	and if that fails (cuz its not of the right type) - then at least create a simple afterRep (PlainTextRep)
 *	for this special case. Note - we had to modify the smantics (backward compat code note!) of
 *	InteractiveUndoHelperMakeTextRep to return a bool - indicating if there was any change.
 *	
 *	Revision 2.175  2003/06/03 20:29:00  lewis
 *	SPR#1513: added support for ignoreall button, clicking in the listbox, and double clicking in it.
 *	Also implemetned the Change functionality and ChangeAll (all for spellcheck dialog)
 *	
 *	Revision 2.174  2003/06/02 23:14:39  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.173  2003/06/02 17:57:45  lewis
 *	fixed TextInteractor::SetSelection () to refresh if GetSelectionShown () OR GetUseSecondaryHilight () -
 *	found part of spellcheck work (SPR#1513)
 *	
 *	Revision 2.172  2003/06/02 16:14:07  lewis
 *	SPR#1513: first draft of (windows only so far) simple spellchecking dialog. Basically working (75%).
 *	A few fixes to generic dialog support code
 *	
 *	Revision 2.171  2003/05/30 11:42:36  lewis
 *	SPR#1517: Added static member and API to keep track of 'ReplaceParameters'. Methods are virtual so
 *	apps COULD persist them, but I'm not sure thats useful (where it clearly is for find parameters)
 *	
 *	Revision 2.170  2003/05/30 03:10:17  lewis
 *	SPR#1517: preliminary version of REPLACE DIALOG support (WIN32 only so far). Also added a few
 *	BreakInGroupedCommands() calls for OnCutCommand/OnClearCmd () etc...
 *	
 *	Revision 2.169  2003/05/21 20:47:03  lewis
 *	SPR#1494: Added TextInteractor::SupressCommandBreaksContext stack-based class to allow some commands to
 *	be grouped together (when acting on a bunch of table cells)
 *	
 *	Revision 2.168  2003/05/21 12:53:14  lewis
 *	SPR#1495: rename UndoableContextHelper::GetSelStart/End to UndoableContextHelper::GetUndoRegionStart/End
 *	
 *	Revision 2.167  2003/05/20 22:50:45  lewis
 *	(most of last checkin was for SPR#1493)
 *	
 *	Revision 2.166  2003/05/20 22:50:01  lewis
 *	SPR#1492: react to change in CTOR args for TempMarker.  TextInteractor::InteractiveUndoHelperMakeTextRep ()
 *	and objects it creates now keep separate tabs on regionStart/End vs. selStartEnd.
 *	CommandHandler::DoUndo/DoRedo now take TextInteractor arg- and InteractiveReplaceCommand no
 *	longer take a TextInteractor arg. No longer peek/poke at before/afterRep's fSelStart/End -
 *	now those are specified through constructor args to their respective objects, and passed
 *	in though new extra args to InteractiveUndoHelperMakeTextRep. Much of this code change for
 *	the undo support is not backward compatable - but end-users are unlikely to run into probelms
 *	since they are unlikely to have used it directly
 *	
 *	Revision 2.165  2003/05/07 21:11:04  lewis
 *	SPR#1467: implemented various select commands (word/row/paragraph/table/cell/row/col)
 *	
 *	Revision 2.164  2003/05/07 18:41:06  lewis
 *	SPR#1465: wrap ti.PreInteractiveUndoHelper with check for CommandHandler
 *	
 *	Revision 2.163  2003/05/06 21:47:00  lewis
 *	SPR#1465: TextInteractor::Pre/PostInteraciveUndoHelper REQUIRE fCommandHandler != NULL.
 *	Then fix UndoHelper class, and InteractiveWPHelper1<> to check first if CommandHandler != NULL.
 *	Thus we don't leak command objects which might contain a reference to a cell paraDB that has
 *	already gone away!!!
 *	
 *	Revision 2.162  2003/05/01 22:16:27  lewis
 *		(SPR#1455)Changed EmbeddedTableWordProcessor::UpdateWindowRect_ ()
 *	so it throws new CannotUpdateNow () exception, and revised TextInteractor::Update/UpdateWindowRect
 *	so they take an optional parameter saying if it should automatically eat
 *	these new 'CannotUpdateNow' exceptions.
 *		Also changed TextInteractor::PreScrollHelper so it handles throws (calls with false
 *	for Update ingore flag) - and resets fTryTodoScrollbits.
 *		Also, override EmbeddedTableWordProcessor::SetDefaultUpdateMode () to force
 *	always to use eDelayedUpdate in place of immediateUpdate. That should greatly reduce
 *	the number of places people try to call Update ().
 *	
 *	Revision 2.161  2003/04/18 00:51:51  lewis
 *	SPR#1444: fix small end-of-buffer bug case in OptionallyExpandSelectionForSmartCutAndPasteModeDeletes
 *	
 *	Revision 2.160  2003/04/16 14:04:16  lewis
 *	SPR#1437: cosmetic cleanups - nest code and scope in in braces inside of UndoContext
 *	instances, and use new TextInteractor::BreakInGroupedCommands () wrapper (and other
 *	small cleanups)
 *	
 *	Revision 2.159  2003/04/10 14:02:22  lewis
 *	(vaguely related to work on SPR#1329)- cleanup doccomments and org of TextInteractor::OnCopy/
 *	Paste_Before/After. Moved mac/Win specific code exclusely to _MacOS/_Win32 subclasses(tempaltes).
 *	Improved docs
 *	
 *	Revision 2.158  2003/04/09 18:35:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.157  2003/04/08 23:34:56  lewis
 *	SPR#1404: added TextInteractor::GetCaretShownSituation () method to wrap selStart==selEnd
 *	check (so we can override that for table code blinking caret. Updated each Led_XXX classlib
 *	wrapper to call that instead of selStart==selEnd
 *	
 *	Revision 2.156  2003/04/08 22:37:09  lewis
 *	SPR#1404: move WhileTrackingConstrainSelection () override from Led_Gtk/Win32/MacOS templates
 *	to default impl for TextInteractor (so its used in Embedded WP class so cells handle double
 *	clicks properly
 *	
 *	Revision 2.155  2003/04/07 15:17:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.154  2003/04/07 15:15:49  lewis
 *	SPR#1409: added fUndoFormatString / fRedoFormatString command strings, and use that with
 *	Format() calls instead of hardwired strgins in TextInteractor::OnUpdateUndoRedoCommand ()
 *	
 *	Revision 2.153  2003/04/03 16:41:28  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using
 *	templated command classes, just builtin to TextInteractor/WordProcessor (and instead of
 *	emplate params use new TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.152  2003/04/01 21:28:01  lewis
 *	SPR#1359: Fixed TextInteractor::CalculateCaretRect () to check if GetCharWindowLocation is
 *	outside of the WindowRect and if so - just return empty-rect for CaretRect.
 *	
 *	Revision 2.151  2003/03/21 14:48:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.150  2003/03/21 13:59:41  lewis
 *	SPR#1364- fix setting of drag anchor. Moved it from Led_XXX(Win32/MacOS/Gtk) to
 *	TextInteractor::ProcessSimpleClick (adding extra arg). Also lots of other small code cleanups
 *	
 *	Revision 2.149  2003/03/21 02:54:58  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.148  2003/03/20 21:13:05  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.147  2003/03/20 15:58:19  lewis
 *	SPR#1360 - lose OnEnterIdleCallback and added TextInteractor::DrawBefore () to replace it
 *	(as a way to assure scrollbars updated before draw - though details of that could easily
 *	change soon)
 *	
 *	Revision 2.146  2003/03/17 21:37:44  lewis
 *	SPR#1350 - qSupportEnterIdleCallback is now automatically true - and not a conditional
 *	varaible anymore
 *	
 *	Revision 2.145  2003/03/10 19:10:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.144  2003/03/10 19:09:12  lewis
 *	SPR#1286 - OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds
 *	OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds to better handle case of
 *	when to insert smart spaces.
 *	
 *	Revision 2.143  2003/03/07 15:00:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.142  2003/02/28 19:09:21  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.141  2003/02/27 15:49:35  lewis
 *	had to rename CharacterProperties::isspace etc to CharacterProperties::IsSpace() to avoid name
 *	conflicts with GCC MACRO implemeantion of ISXXXX functions
 *	
 *	Revision 2.140  2003/02/24 18:31:38  lewis
 *	SPR#1306- Added CharacterProperties class and verison of isspace etc
 *	
 *	Revision 2.139  2003/02/12 16:40:01  lewis
 *	SPR#1299- a few small fixes to InteractiveModeUpdater/READONLY handling.
 *	
 *	Revision 2.138  2003/01/31 23:08:28  lewis
 *	SPR#1278- added InteractiveUpdadeMode and InteractiveModeUpdater so things like AboutToUpdate()
 *	can tell if its a USER change or a program change to text its about to make (and so whether or
 *	not to allow/disallow)
 *	
 *	Revision 2.137  2003/01/30 21:48:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.136  2003/01/11 18:01:58  lewis
 *	doccomments
 *	
 *	Revision 2.135  2002/12/17 23:41:46  lewis
 *	SPR#1211 - fix TextInteractor::GetCharAtClickLocation to support BIDI
 *	
 *	Revision 2.134  2002/12/13 18:37:46  lewis
 *	SPR#1203- Used new TextImager::GetTextDirection API from
 *	TextInteractor::CalculateCaretRect (). Not 100% sure this is right -
 *	but pretty close.
 *	
 *	Revision 2.133  2002/12/12 16:50:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.132  2002/10/30 13:29:35  lewis
 *	SPR#1155 - Added new Rect_Base<> template and made Led_Rect a simple typedef of that. Then TRIED (failed)
 *	to make the top/left/bottom/right fields private by adding accessors. Added the accessors and used them
 *	in tons of places - but still TONS more required. Leave that battle for another day. I'm not even SURE
 *	its a good idea. Anyhow - also added a new typedef for Led_TWIPS_Rect (this was what I was after to
 *	begin with with all this nonsese)
 *	
 *	Revision 2.131  2002/09/19 14:14:47  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.130  2002/09/19 12:53:01  lewis
 *	DOCCOMMENT
 *	
 *	Revision 2.129  2002/05/06 21:33:58  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.128  2001/11/27 00:30:03  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.127  2001/10/20 13:38:59  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.126  2001/10/17 20:43:03  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.125  2001/10/11 23:27:22  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.124  2001/10/09 21:10:32  lewis
 *	SPR#1058- qSupportLed23CompatAPI support
 *	
 *	Revision 2.123  2001/09/28 14:04:21  lewis
 *	SPR#1051- added flag to TextInteractor::ScrollToSelection () to make cursoring behavior work a bit better
 *	
 *	Revision 2.122  2001/09/26 15:41:22  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.121  2001/09/26 14:00:27  lewis
 *	SPR#1044- fix TextInteractor::DoSingleCharCursorEdit () to ignore smart-cut-and-paste
 *	flag when backspacing over text (with empty selection)
 *	
 *	Revision 2.120  2001/09/20 00:38:46  lewis
 *	SPR#1031- several small changes to SmartCutAndPaste routines to make them work a
 *	bit better. Still kludy
 *	
 *	Revision 2.119  2001/09/12 16:05:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.118  2001/09/12 14:53:33  lewis
 *	SPR#0987- Added conditional UseSecondaryHilight support (macos&win32 only so far).
 *	Redo Led_Region support for MacOS so now more like with PC (no operator conversion to
 *	macregion and auto constructs OSRegion except when called with region arg - in whcih
 *	case we dont OWN region and dont delete it on DTOR
 *	
 *	Revision 2.117  2001/09/11 22:29:16  lewis
 *	SPR#1018- preliminary CWPro7 support
 *	
 *	Revision 2.116  2001/09/05 16:29:25  lewis
 *	SPR#1016- cleanup semantics of ReaderFlavorPackage::GetFlavorSize/ReaderFlavorPackage::ReadFlavorData
 *	
 *	Revision 2.115  2001/09/05 00:07:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.114  2001/09/05 00:04:35  lewis
 *	SPR#1013- A few small changes to TextInteractor::UndoableContextHelper to enable its use in Led_MFC_
 *	and Led_PP D&D code. And added UndoableContextHelper::SetBeforeUndoSelection () to fix a small
 *	trouble with that code.
 *	
 *	Revision 2.113  2001/08/29 23:36:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.112  2001/08/28 18:43:44  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.111  2001/08/27 14:47:34  lewis
 *	SPR#0973- revise the display of hilighting (inversion) for the space before
 *	the LHS margin. Only invert that region if the selection extends OVER it in both
 *	directions OR if the entire line is selected. Works very well now - except its
 *	slightly quirking when the whole line is selected EXCEPT the end adn then you
 *	select the end. But I think this is the best I can do
 *	
 *	Revision 2.110  2001/08/01 23:47:17  lewis
 *	SPR#0965- TextInteractor::BadUserInput class, and throw that from
 *	TextInteractor::OnBadUserInput () instead of directly calling Led_BeepNotify ();
 *	
 *	Revision 2.109  2001/07/31 15:08:26  lewis
 *	use static_cast<> to quiet a compiler warning
 *	
 *	Revision 2.108  2001/07/19 23:43:41  lewis
 *	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug support
 *	
 *	Revision 2.107  2001/07/19 02:21:48  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.106  2001/07/12 13:55:27  lewis
 *	SPR#0906- hack override WordProcessor::PostReplace() so typing at end of
 *	buffer with listmode on gets new empty lines bullet drawn/erased
 *	
 *	Revision 2.105  2001/05/24 14:24:51  lewis
 *	SPR#0940- fix weird scrolltoselection corner case
 *	
 *	Revision 2.104  2001/05/23 22:01:33  lewis
 *	SPR#0926- instead of crashing/asserting - changed TextInteractor::OnUndo/RedoCommands()
 *	to check if CanUndo before doing so - and beep if cannot. Some toolkits (notably Gtk)
 *	make it hard to avoid getting commands sent when they should be illegal/ignored
 *	
 *	Revision 2.103  2001/05/18 20:56:13  lewis
 *	work around qGCC_OptBugWithLocalClassesScopedInFunction bug. And - SPR#0922- call
 *	Led_Tablet_::ScrollBitsAndInvalRevealed () instead of Win32/MacOS APIs directly
 *	
 *	Revision 2.102  2001/05/07 22:24:03  lewis
 *	since TextInteractor::DidUpdateText () doesn't allow throws through it - and it
 *	calls PostReplace () which does - must catch those and handle them (was triggered by bug in
 *	xindows code - but could still happen otherwise and we shouldnt terminate)
 *	
 *	Revision 2.101  2001/05/07 14:48:02  lewis
 *	SPR#0904- rename mkDefaultCommandNames ()->MakeDefaultCommandNames ()
 *	
 *	Revision 2.100  2001/05/03 22:07:14  lewis
 *	related to SPR#0887- Xwindows blinking caret support. TextInteractor::GetTickCountBetweenBlinks
 *	now returns a float (seconds)
 *	
 *	Revision 2.99  2001/05/01 16:07:43  lewis
 *	fixed bad names - Get/SetCurClickClickCount to just one CLICK
 *	
 *	Revision 2.98  2001/04/26 16:39:32  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.97  2001/04/18 20:45:19  lewis
 *	lose prefixing :: from iscntrl() calls - so they compile fine with GCC
 *	
 *	Revision 2.96  2001/04/17 22:48:22  lewis
 *	SPR#0871- Moved a bunch of code from Led_MacOS and Led_Win32 to TextInteractor.
 *	Code was all relating to TextInteractor::GetCurClickClickCount () etc. Also -
 *	improved logic for double click detection (now shared across platforms).
 *	
 *	Revision 2.95  2001/04/17 18:41:59  lewis
 *	SPR#0870- added ingore-control-keys fature (defaults on) - so ctrl chars not mapped
 *	to commands aren't interpretted as garbage characters getting inserted
 *	
 *	Revision 2.94  2001/04/09 14:22:40  lewis
 *	SPR#0857- CTRL-backspace should be a backup-and-delete-word commnad - as
 *	on most Win word-processors
 *	
 *	Revision 2.93  2001/01/04 14:27:51  lewis
 *	a few small changes to be compatable with an older version of Borland C++-
 *	requested by a customer - he proovded the patches- Cladio Nold
 *	
 *	Revision 2.92  2001/01/03 16:00:02  lewis
 *	qFailToLookupFunctionNameWhenCompilingFunctionLocalClassMethodCompilerBug
 *	
 *	Revision 2.91  2001/01/03 14:26:10  Lewis
 *	use inherited typedef to work around diff with Borland C++ compiler. And lose
 *	some pointless asserts (eg unsigned num >=0)
 *	
 *	Revision 2.90  2000/10/06 03:34:12  lewis
 *	moved code/datatypes for controlling show/hide display of scrollbars for Led_Win32/Led_MacOS to -
 *	mostly - TextInteractor. Still some logic in the OS-specific wrappers. Unified their APIs -
 *	supporting autohide of sbars on both (though mac maybe still slightly buggy)
 *	
 *	Revision 2.89  2000/09/30 19:35:02  lewis
 *	Added TextInteractorCommandHelper_DefaultCmdInfo/TextInteractorCommonCommandHelper<BASECLASS,CMD_INFO,CMD_ENABLER>.
 *	Added TextInteractor::OnSelectAllCommand. Moved Led_MFC_TmpCmdUpdater to Led_MFC.h
 *	
 *	Revision 2.88  2000/09/25 21:58:56  lewis
 *	when turning OFF ShowCaret - must invalRect (on mac) the area where rect was -
 *	if caret was last displayed ON
 *	
 *	Revision 2.87  2000/08/28 20:33:59  lewis
 *	SPR#0828- changed args to Pre/PostInteractiveUndoHelper, and added (used) new
 *	UndoableContextHelper helper class. SPR#0827- use new TextInteractor::CommandNames
 *	struct to collect together a bunch of command names instead of seperate commandname accessors.
 *	
 *	Revision 2.86  2000/08/13 04:15:07  lewis
 *	SPR#0784- rewrite hidden text code based on setting segwidth to zero, and patching
 *	computeNExtCursorPosition. Not 100% done - but mostly. VERY big change -  and many
 *	bug fixes related to this hidden text stuff folded in as well - but not complete
 *	
 *	Revision 2.85  2000/07/26 05:46:36  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner arg
 *	to CollectAllMarkersInRagne() APIs. And a few related changes. See the SPRs for a table of speedups.
 *	
 *	Revision 2.84  2000/06/17 07:48:16  lewis
 *	SPR#0792 - Fixed TextInteractor::CalculateCaretRect (). First had to
 *	addTextImager::GetRowRelativeBaselineOfRowContainingPosition () API -
 *	and fix a few other bugs - before this worked well. But I think it now works well
 *	
 *	Revision 2.83  2000/06/12 16:20:45  lewis
 *	support SPR#0780- Led_Tablet_ support
 *	
 *	Revision 2.82  2000/05/01 19:26:50  lewis
 *	TextInteractor::HookGainedNewTextStore_ () now calls SetIn/Externalizer instead
 *	of directly setting fields so hook function gets called
 *	
 *	Revision 2.81  2000/04/26 21:05:58  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.80  2000/04/26 14:10:23  lewis
 *	cleanup / small semantic changes - added assert - to TextInteractor::InternalizeBestFlavor () -
 *	having tosdo with selection maintainance.
 *	
 *	Revision 2.79  2000/04/24 22:29:33  lewis
 *	added @TextInteractor::HookExternalizerChanged - and use it in WordProcessor to
 *	sync up with the hidableText database - related to SPR#0724
 *	
 *	Revision 2.78  2000/04/24 21:11:28  lewis
 *	use new TempMarker helper class - instead of manually creating stack based markers,
 *	and removing them.
 *	
 *	Revision 2.77  2000/04/24 16:47:03  lewis
 *	Very substantial change to FlavorPackage stuff - now NOT mixed into TextInteractor -
 *	but simply owned using Led_RefCntPtr<>. Also - did all the proper subclassing in WP
 *	class etc to make this work (almost - still must fix OLE embeddings) as before. SPR# 0742
 *	
 *	Revision 2.76  2000/04/16 14:39:39  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.75  2000/04/14 22:40:28  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.74  2000/04/14 19:15:49  lewis
 *	SPR##0739- react breakout into FlavorPackage code of that stuff from TextInteractor.
 *	And clean it up a bit - re-arranging who does the updateCursorPos stuff from the
 *	differnt flavor-package-formats
 *	
 *	Revision 2.73  2000/03/11 22:04:49  lewis
 *	SPR#0711 - new GetRealEndOfRow* () routines. And fixed PositionWouldFitInWindowWithThisTopRow ()
 *	to call it. And other small fixes to TextInteractor::ScrollToSelection ()  and Simple/MultiRow::ScrollSoShowing ()
 *	
 *	Revision 2.72  2000/01/22 16:52:31  lewis
 *	Lose last default arg (ASSUME ALWAYS TRUE) for TextInteractor::PreInteractiveUndoHelper
 *	(part of cleanup and making this more understandable). And cleanup DOCCOMMENTS a bit more
 *	
 *	Revision 2.71  2000/01/22 16:35:42  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.70  2000/01/22 16:22:01  lewis
 *	DOCCOMMENTS about UNDO - and fixed SPR#0703- TextInteractor::PostInteractiveUndoHelper ()
 *	must save proper selection range
 *	
 *	Revision 2.69  2000/01/22 03:42:53  lewis
 *	update logic in TextInteractor::GetCharAtClickLocation () for SPR#0702- don't
 *	interpret click in line terminated with a SOFT-RETURN (really any zero-width
 *	character(s) as being after the zero-width characters.
 *	
 *	Revision 2.68  1999/12/29 03:12:32  lewis
 *	SPR#0628- Lose #define qUseGDIScrollbitsForScrolling. Instead have methods
 *	G/SetUseBitmapScrollingOptimization ()
 *	
 *	Revision 2.67  1999/12/21 20:55:29  lewis
 *	Use new Led_ThrowIfNull instead of manual check
 *	
 *	Revision 2.66  1999/12/19 16:26:45  lewis
 *	SPR#0668- Added args to virtual TextInteractor::OnTypedNormalCharacter ()
 *	
 *	Revision 2.65  1999/12/14 18:12:20  lewis
 *	fix some c++ coding things GCC gave warnings about - and hacks so qXWindows at least compiles.
 *	
 *	Revision 2.64  1999/12/11 21:44:03  lewis
 *	numerous small cleanups/changes to Copy/Paste code for UNICODE compliance. Getting
 *	small details of converstions and use of char vs. Led_tChar right.
 *	
 *	Revision 2.63  1999/12/09 03:27:41  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.62  1999/11/29 18:55:24  lewis
 *	update DOCHELP
 *	
 *	Revision 2.61  1999/11/28 16:49:41  lewis
 *	include <cctype> - needed to compile on new mwerks compiler
 *	
 *	Revision 2.60  1999/11/15 21:33:57  lewis
 *	Avoid MFC wrappers for CWnd/CDC etc (ScrollWindow). So this code compiles without MFC on Win32 now
 *	
 *	Revision 2.59  1999/11/13 16:32:24  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.58  1999/07/16 21:58:32  lewis
 *	remove some uneeded includes that caused this to not compile with MWERKS on Mac targetting PC
 *	
 *	Revision 2.57  1999/07/02 14:46:12  lewis
 *	spr#0604- Lose StandardStyledTextInteractor::InteractiveReplace_ and replace with new
 *	StandardStyledTextInteractor::InteractiveReplaceEarlyPostReplaceHook () which overrides
 *	new TextInteractor::InteractiveReplaceEarlyPostReplaceHook(). Also - had to add revised
 *	hack fEmptySelectionStyleSuppressMode to StandardStyledTextInteractor to make this fix work
 *	
 *	Revision 2.56  1999/05/03 22:05:20  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.55  1999/05/03 21:41:36  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.54  1999/05/03 20:05:05  lewis
 *	spr#0576- fix bug in handing backspace key in conjunction with smart cut and paste
 *	
 *	Revision 2.53  1999/04/28 20:06:55  lewis
 *	work around quirky behavior of iswalnum() for idiogram characters
 *	
 *	Revision 2.52  1999/04/15 22:16:11  lewis
 *	add extra arg to PreInteractiveUndoHelper and tweeks to OptionallyExpand...
 *	SmartCut..., to fix spr#0570
 *	
 *	Revision 2.51  1999/04/01 18:58:45  lewis
 *	several fixes to smart cut and paste. First - I had the wide/narrow versions of
 *	isalnum etc reversed in IsShouldBeSepWithWhitespaceWordChar. Second - I no longer
 *	count ispuct as this class of character, since that usually acts as a separator.
 *	And most subtly, I changed the code in TextInteractor::OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds
 *	to look one caharacter FURTHER BACK for deciding whether to insert spaces. Was looking in the wrong place.
 *	And that change was slightly tricky cuz still needed to update the selection properly so undo would work
 *	properly. Hopefully this is all done now
 *	
 *	Revision 2.50  1999/03/29 21:14:09  lewis
 *	fix spr#0567- smart cut and paste now works a bit more senisbly, is much better commented/documented
 *	how it works, and should NOT do bad things to strings of Japanese characters anylonger
 *	(untested - but I hope I got this check right)
 *	
 *	Revision 2.49  1999/03/25 22:28:35  lewis
 *	Add extra overload of PostInteractiveUndoHelper and use that in first, and make args
 *	reference args so they can handle NULL-ing out pointer easier. And for better code
 *	sharing when I need to make the after REP another way
 *	
 *	Revision 2.48  1999/03/24 15:47:26  lewis
 *	spr#0561 fixed- TexImager::NoTabletAvailable now subclasses from TextImager::NotFullyInitialized etc
 *	
 *	Revision 2.47  1999/02/12 17:01:02  lewis
 *	fix subtle bug in horizontal scrolling (TextInteractor::ScrollToSelection)
 *	
 *	Revision 2.46  1999/02/09 16:30:45  lewis
 *	in PreReplace code - if we get an excpetion cuz NoTabletAvailable - just eat it,
 *	and set updateMode to noUpdate
 *	
 *	Revision 2.45  1999/02/05 01:55:17  lewis
 *	in InvalidateCaretState - call RefreshWindowRect with eDelayedUpdate, rather
 *	than default updatemode, cuz default COULD be immediate update. That was causing
 *	problems in TA SBSMode, during scrolling (due to syncronizeation). And for the
 *	invalidate code - delayed is really always the right answer
 *	
 *	Revision 2.44  1998/10/30 14:33:45  lewis
 *	MSVC60 warnings pragma changes.
 *	Lose old compiler BWAs.
 *	Use vector<> instead of Led_Array.
 *	
 *	Revision 2.43  1998/07/24  01:15:02  lewis
 *	added docs, and fixed MS-windows problem - stripping NUL chars from
 *	middle of externalized text (copy/paste).
 *	other small changes to trackconstrian stuff
 *
 *	Revision 2.42  1998/06/03  01:53:22  lewis
 *	qWorkAroundWin95BrokenUNICODESupport
 *
 *	Revision 2.41  1998/05/05  00:30:50  lewis
 *	Support MWERKS CWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.40  1998/04/25  01:38:12  lewis
 *	In Clipboard calls - multiple size of void* bytes added by sizeof (Led_tChar) -
 *	so works better with UNICODE - though
 *	still not right (cuz we haven't straightedn out CF_TEXT vs CF_UNICODETEXT
 *	stuff; fix soon!).
 *	Use LED_TCHAR_OF() for const strings.
 *	Other UNICODE fixes
 *
 *	Revision 2.39  1998/04/08  02:09:17  lewis
 *	small cleanups, and one small SetSaretShownAfterPos () fix (call to it change).
 *	I hope it was a fix?
 *
 *	Revision 2.38  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.37  1997/09/29  15:47:10  lewis
 *	Lose qSupportLed21CompatAPI and qLedFirstIndex.
 *	ExpandedFromAndToInPostReplace modulartity to make easier support for differnt justifiactions.
 *	New cleaner approcah to handling UpdateMode stuff - using nonvirtual function with UpdateMode arg and no
 *	override of virtual from TextImager - then that final overrider can peek at 'defaut update mode' if it
 *	wants.
 *
 *	Revision 2.36  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.35  1997/07/23  23:10:26  lewis
 *	docs changes
 *
 *	Revision 2.34  1997/07/15  05:26:36  lewis
 *	AutoDoc content.
 *
 *	Revision 2.33  1997/07/13  02:18:34  lewis
 *	Renamed TextInteractor_ to TextInteractor.
 *	Lose qSupportWholeWindowInvalidOptimization define - assume always true.
 *	AutoDoc support.
 *	Fix a couple bugs with TextInteractor::DoSingleCharCursorEdit () - by row end cases.
 *	qLedFirstIndex = 0 support.
 *	Fix crasher bug in WhileTrackingConstrainSelection_ForWholeRows at end of buffer.
 *	command names use string class, not char* now.
 *
 *	Revision 2.32  1997/06/28  17:19:02  lewis
 *	InteractiveUndoHelperMakeTextRep () function to cleanup Undo code, and to allow easier fixing of
 *	StyledTextInteracor undo bug.
 *	Other related cleanups too, like making FlavorSavorTextRep scoped in function.
 *	Make exception handling safer with undo code.
 *	Make sDoingUpdateModeReplaceOn static now a field fDoing... so thread-safe (ActiveX control).
 *
 *	Revision 2.31  1997/06/23  16:17:09  lewis
 *	Speed tweek TextInteractor_::InvalidateCaretState () - avoid
 *	calculating CaretRect if wholewindowinvalid - speed tweek reading large files on
 *	windows.
 *
 *	Revision 2.30  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.29  1997/06/18  03:11:56  lewis
 *	qCWSIREADUndefinedBug
 *	WhileSimpleMouseTracking () utility.
 *	(and do hscrolling autoscrolling in it).
 *	qIncludePrefixFile,
 *
 *	Revision 2.28  1997/03/23  00:42:35  lewis
 *	more cleanups/fixes to smart cutnpaste stuff.
 *	WhileTrackingConstrainSelection () support.
 *	Reviese (hopefully now fully fixed) scrollwindow/scrollbits code.
 *	more.
 *
 *	Revision 2.27  1997/03/04  20:10:32  lewis
 *	SmartCutAndPasteMode
 *
 *	Revision 2.26  1997/01/20  05:29:09  lewis
 *	Mostly support for new UpdateMode - eDefaultUpdate.
 *
 *	Revision 2.25  1997/01/10  03:12:28  lewis
 *	throw specifiers.
 *	AboutToUpdateText returns voi.d
 *	New Pre/PostScrollHelper and scrollbits code.
 *	SetHScrollPos ().
 *	Mor scrolling stuff.
 *
 *	Revision 2.24  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.23  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.22  1996/10/15  18:33:15  lewis
 *	Externalize_TEXT() - fixed PC bug - write extra NUL char.
 *
 *	Revision 2.21  1996/10/04  16:40:34  lewis
 *	Fixed bugs (mostly performance problems) here and there.
 *	Dont refreshrect on caretrect if non-empoty selection.
 *	PrePostReplace() code optimized a bit better - fewer calls to GetTextWindowBoundingRect ().
 *	Fixed inval code in SetSelection (was too conservative - now more aggressive).
 *
 *	Revision 2.20  1996/09/30  14:29:35  lewis
 *	Support for qSupportEnterIdleCallback.
 *	Lose old AdjustBounds() code and replace with new InvalidateScrollBarParameters ()/
 *	UpdateScrollBars () code.
 *	qSupportWholeWindowInvalidOptimization.
 *	On Undo/RedoCommand helpers.
 *	Big change to the PrePostReplace code to workaround/fix SPR#0384.
 *	Go back soon and cleanup that code.
 *
 *	Revision 2.19  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.18  1996/07/19  16:43:50  lewis
 *	Code cleanups - lost commneted out old code.
 *
 *	Revision 2.17  1996/06/01  02:13:39  lewis
 *	Mic cleanups and CommandHander::BreakInGroupedCommands() patches/fixes.
 *	New fLeftSideOfSelectionInteresting - to deal with better interpretting cursoring.
 *	more?
 *
 *	Revision 2.16  1996/05/23  19:47:32  lewis
 *	Revised (hopefulyl for the better) the scrollbar paging delay code.
 *	Added new InternalizeFlavor_FILE/InternalizeFlavor_FILEData support.
 *	Tweek TextInteractor_::SetDefaultFont () code to optimize case of font spec outside visible window
 *	(helps file read time).
 *	Generally better error checking (throws on clipboard IO code).
 *	Led_IncrementalFontSpecification support.
 *	More/cleanups...
 *
 *	Revision 2.15  1996/05/14  20:28:17  lewis
 *	change caret width on PC to 1.
 *	Hook HookLosingTextStore to cleanup.
 *	Totally new PreReplace/PostReplace code. Completely revamped, and now
 *	FAR more aggressive in making minimal redraws. Hopefully it is all
 *	correct.
 *	:-):-):-)
 *	Also, made SetSelection much more aggressive in its optimizations
 *	(qAgressivelyOptimizeSetSelectionImmedi).
 *
 *	Revision 2.14  1996/05/04  21:07:25  lewis
 *	Added ::EnumClipboardFormats/::GetClipboardFormatName hack for windows
 *	Copy/Paste debugging (ifdefed 0 - just there to easy turn on for debugging).
 *
 *	Revision 2.13  1996/04/18  15:39:39  lewis
 *	Lots of changes to integrate Flavor/Drag/Drop/Cut/Paste code with undo.
 *	And to fix bugs with all of the above.
 *
 *	Revision 2.12  1996/03/16  18:47:07  lewis
 *	No more H()/V() functions.
 *
 *	Revision 2.11  1996/03/05  18:31:56  lewis
 *	Re-implemented old Copy/Paste support around new Internalize/Externalize
 *	of FlavorPackages. Then used this new support for drag/drop as well.
 *
 *	Revision 2.10  1996/03/04  07:49:24  lewis
 *	Major cleanup of Copy/Paste code.
 *	Plus added TextInteractor_::ProcessSimpleClick () which allows us
 *	to portable (without much code dup) hook into embedding Select/click on
 *	and open messages, across platforms.
 *
 *	Revision 2.9  1996/02/26  22:05:27  lewis
 *	Reanmed TextInteracter --> TextInteractor.
 *	Use Led_Min etc instead of Min()
 *	Revised/embellished OnCopyCommand support. Make multiple virtual subparts
 *	so you can hook in more easily and do differnt phases differently, and share
 *	more code.
 *	Override AboutToUpdateText and (for now conditioanl on qUseNewTmpUpdateCrap)
 *	do fancy, more restrictive invalidating on updates (esp font style updates).
 *
 *	Revision 2.8  1996/02/05  04:25:01  lewis
 *	Use inherited: typedef - cleanups
 *
 *	Revision 2.7  1996/01/22  05:25:26  lewis
 *	Lose qTmpHackNoTryCatch workarounds.
 *	Added fCommandHandler/Undo support to InteractiveReplace code.
 *	TextInteractor_::DidUpdateText () does refresh - like I had done in
 *	LedPart - now only done here. So multiple synced views can be kept uptodate.
 *	(cannot remember where else this came up?).
 *
 *	Revision 2.6  1995/12/13  06:00:35  lewis
 *	Moved mac/OpenDoc specific code about caret updating out. Instead do
 *	as we've always done on pc - just have an invalidate style API.
 *	This turns out much easier for OpenDoc anyhow cuz of their funkies with
 *	facets, and having them, and the right ones handy.
 *
 *	Revision 2.5  1995/12/06  01:37:10  lewis
 *	Use utility UpdateIfNoKeysPending () instead of manually doing
 *	its implementation inline.,
 *
 *	Revision 2.4  1995/11/25  00:29:18  lewis
 *	 Lots of changes to the caret-updating code. Mostly moved it out
 *	of a number of places, and put in INTO InteractiveReplace() as an
 *	optional feature, and broke that out into a virtual and an _under_bar_
 *	function version.
 *
 *	Revision 2.3  1995/11/02  22:40:04  lewis
 *	Move some OnPasteCommand code into new shared util
 *	OnPasteCommand_ so can be used in subclasses doing styled TE
 *	paste support.
 *
 *	Revision 2.2  1995/10/09  22:42:03  lewis
 *	Repalce with UpdateMode now here instead of pure virutal. everything can now
 *	be done here directly since we dont' need to hook in stuff about finalizing markers (now done
 *	by new texstore add/didupdate text callbacks).
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.26  1995/06/08  05:18:12  lewis
 *	Code cleanups
 *
 *	Revision 1.25  1995/06/05  21:29:29  lewis
 *	Redo TextInteracer_:SelectWholeLineAfter() to use GetStartOfRowContainingPosition etc
 *	so we don't need to override using rowreferences.
 *	Then fixed so only get next character after end of row if row ended
 *	with NEWLINE.
 *	(SPR#0320).
 *
 *	Revision 1.24  1995/05/31  09:37:44  lewis
 *	Moved calc of Caret rect into new function CalculateCaretRect
 *	and parameterize better so can be used on PC/Mac. Then use
 *	in DrawCaret.
 *
 *	Revision 1.23  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.22  1995/05/29  23:50:30  lewis
 *	Code cleanups, and no longer need explicit TmpMarker class - SPR 0295.
 *
 *	Revision 1.21  1995/05/20  04:58:41  lewis
 *	Fix use of WindowRect/ImageRect - See SPR 0263.
 *
 *	Revision 1.20  1995/05/18  08:54:28  lewis
 *	Fixed typo.
 *
 *	Revision 1.19  1995/05/18  08:50:50  lewis
 *	Worked around qMSVC_LocalClassesCannotHaveAnyFunctionM
 *
 *	Revision 1.18  1995/05/18  08:15:37  lewis
 *	Fixed LVEJ NewSBSMode problems with adjusting selection after inserts/pastes
 *	by keeping tmp stack marker before insertion, and letting it keep track
 *	of how much text inserted / deleted (takes into account sentinals), so
 *	we can properly adjust at the end the final selection. LEDSPR#0252.
 *	Lose old CW5.* bug workarounds (SPR#0249).
 *
 *	Revision 1.17  1995/05/16  06:42:20  lewis
 *	Added InteractiveReplace () for typing/paste calls to Replace.
 *	Do SetCaretShownAfterPos () there instead of other places.
 *	Check for NULL in a couple of places (mallocs) and raise exceptions.
 *
 *	Revision 1.16  1995/05/09  23:28:48  lewis
 *	On NULL from Windows get clipboard call - raise exception - SPR#0239.
 *	Moved fMultiByteInputCharBuf code here from Led_TCL and Led_MFC. Added
 *	dded OnTypedNormalCharacter/HandledMByteCharTyping/QueryInputKeyStrokesPending
 *	LedSPR#0242
 *	Code cleanups
 *
 *	Revision 1.15  1995/05/08  03:15:47  lewis
 *	Fix (hopefully) calls to SetCaretShownAfterPos for SPR 0235.
 *	ScrollSoShowing/ScrollByIfRoom no longer return value - SPR 0236.
 *	Other ScrollSoShowing cleanups - for SPR 0236.
 *
 *	Revision 1.14  1995/05/06  19:45:34  lewis
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *	Use Led_tChar instead of char for UNICODE support - SPR 0232.
 *
 *	Revision 1.13  1995/05/05  19:52:13  lewis
 *	Led_SmallStackBuffer now templated class.
 *	Fixed PC paste bug - cannot use data.GetDataLength() which used ::GlobalSie
 *	of the handle. Must use strlen on the text in the clipboard.
 *	Bug HAD occurred only under win32s. SPR# 0223.
 *
 *	Revision 1.12  1995/04/25  03:32:22  lewis
 *	On PC paste - ignore NUL byte.
 *
 *	Revision 1.11  1995/04/25  02:57:02  lewis
 *	SPR 0222
 *	SPR 0221 - TextInteractor_::GetCharAtClickLocation ()
 *	SPR 0219 - TextInteractor::DrawCaret()
 *	Remove assertion on cursor movement code that we move at most
 *	2Led_tChars on a cursor-by-char backup and delete (LVEJ sbsmode smart cursors
 *	do more).
 *	Smarten use of SetCaretShownAfterPos().
 *
 *	Revision 1.10  1995/04/20  06:49:35  lewis
 *	Default for fCaretShownAfterPos changed.
 *
 *	Revision 1.9  1995/04/18  00:22:03  lewis
 *	More work on SPR 0204 - fixing where we blink caret - before/after.
 *
 *	Revision 1.8  1995/04/16  19:34:50  lewis
 *	Got rid of SimpleTextInteractor - SPR 205.
 *	Added Get/Set CaretShownAfterPos () support - SPR 204.
 *
 *	Revision 1.7  1995/04/11  02:57:51  lewis
 *	On copy command - don't do scrolltoselection.
 *
 *	Revision 1.6  1995/03/29  21:05:25  lewis
 *	Renamed UpdateCaretState () to InvalidateCaretState ().
 *	Got rid of case where it was called (on pc only) and shouldn't have
 *	been needed (see if I screwed things up?).
 *	Call OnBadUserInput () for failures in doing pastes etc (SPR0192).
 *	SPR#0195, 0193, 0192.
 *
 *	Revision 1.5  1995/03/23  03:56:52  lewis
 *	Fix TextInteractor_::SelectWholeLineAfter () - SPR 172/179.
 *
 *	Revision 1.4  1995/03/17  03:05:14  lewis
 *	Added DelaySomeForScrollBarClick () to share code between mac/pc for
 *	scrolling.
 *	Tweek PreReplace/PostReplace () for eNoUpdate case SPR#170.
 *
 *	Revision 1.3  1995/03/13  03:26:22  lewis
 *	Renamed NLtoNative() etc to be prefixed with Led_.
 *
 *	Revision 1.2  1995/03/02  05:46:24  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:46  lewis
 *	Initial revision
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<cctype>
#include	<set>

#include	"LedConfig.h"


#include	"CodePage.h"
#include	"Command.h"
#include	"IdleManager.h"
#include	"Marker.h"
#include	"TextStore.h"
#include	"TextInteractor.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif





#if		qLedUsesNamespaces
	namespace	Led {
#endif



#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (disable : 4786)		//qQuiteAnnoyingDebugSymbolTruncationWarnings
#endif











typedef	InteractiveReplaceCommand::SavedTextRep	SavedTextRep;



namespace {
	class	FlavorSavorTextRep : public SavedTextRep {
		private:
			typedef	SavedTextRep	inherited;
		public:
			FlavorSavorTextRep (TextInteractor* interactor, size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd):
					inherited (selStart, selEnd),
					fSavedText (),
					fTextLength (regionEnd-regionStart)
				{
					#if		!qFailToLookupFunctionNameWhenCompilingFunctionLocalClassMethodCompilerBug
						Led_RequireNotNil (interactor);
					#endif
					interactor->GetExternalizer ()->ExternalizeBestFlavor (fSavedText, regionStart, regionEnd);
				}
			override	size_t	GetLength () const
				{
					return fTextLength;
				}
			override	void	InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite)
				{
					#if		!qFailToLookupFunctionNameWhenCompilingFunctionLocalClassMethodCompilerBug
						Led_RequireNotNil (interactor);
					#endif
					interactor->GetInternalizer ()->InternalizeBestFlavor (fSavedText, at, at + nBytesToOverwrite);
				}

		private:
			size_t						fTextLength;
			ReadWriteMemBufferPackage	fSavedText;
	};
}




namespace {
	// Only these chars count as whitespce for smart cut/n/paste
	inline	bool	IsSmartSpace (Led_tChar c)
		{
			return (c == ' ' or c == '\t');
		}
	inline	bool	IsShouldBeSepWithWhitespaceWordChar (Led_tChar c)
		{
			// iswalnum vectors to GetStringTypeW (CT_CTYPE1) and then checks for C1_ALPHA, which
			// matches all alphabetic, numeric, AND Idiogram characters. I THINK the right thing to use
			// is to directly call GetStringTypeW (CT_CTYPE2, and check for C2_SEGMENTSEPARATOR, or something
			// like that (will have to read up on UNICODE more, and/or experiemnet). Anyhow - none of that will happen
			// for this release, and this should be good enuf to prevent smart-copy-paste from happing with
			// idiogram characters
			return CharacterProperties::IsAlnum (c) and c < 127;
		}
}





namespace {
	class	MyCallback : public TextInteractor::DialogSupport::SpellCheckDialogCallback {
		private:
			typedef	TextInteractor::DialogSupport::SpellCheckDialogCallback	inherited;

		public:
			MyCallback (TextInteractor& ti):
				inherited (),
				fTI (ti),
				fIgnoredWords ()
				{
				}

		public:
			override	MisspellingInfo*	GetNextMisspelling ()
				{
					SpellCheckEngine*	sce	=	fTI.GetSpellCheckEngine ();
					if (sce != NULL) {
						Led_tChar	charBuf[10*1024];		// buffer size doesn't matter much - but just has to be larger than the largest undef word we ever want to find...
						bool		firstTry	=	true;
						size_t		startRegion	=	fTI.GetSelectionEnd ();
				SecondTry:
						{
							// regardless of the startRegion - back up the search to the start of the interesected word. The only
							// exception is if the size of our charBuf isn't big enough to go past the startRegion position (cuz
							// then we'd be going backwards, and risk never moving forwards, in case of a large word)
							size_t	wordStart	=	0;
							size_t	wordEnd		=	0;
							bool	wordReal	=	false;
							fTI.GetTextStore ().FindWordBreaks (startRegion, &wordStart, &wordEnd, &wordReal, sce->PeekAtTextBreaksUsed ());
							if (wordReal and wordStart + Led_NEltsOf (charBuf) > startRegion) {
								startRegion = wordStart;
							}
						}
						size_t		endRegion	=	min (startRegion + Led_NEltsOf (charBuf), fTI.GetEnd ());
						fTI.CopyOut (startRegion, endRegion-startRegion, charBuf);
						const Led_tChar*	cursor		=	NULL;
						const Led_tChar*	wordStart	=	NULL;
						const Led_tChar*	wordEnd		=	NULL;
						if (sce->ScanForUndefinedWord (charBuf, charBuf + (endRegion-startRegion), &cursor, &wordStart, &wordEnd)) {
							/*
							 * If the undefined word ends at the end of the buffer region we looked at - it might be an artifact of our 
							 * chunking. Retry the word (being careful about the specail case where the 'word' is as big as our buffer).
							 */
							if (wordStart != charBuf and wordEnd == charBuf + (endRegion-startRegion)) {
								startRegion += (wordStart-charBuf);	// advance to the start of this word and try again
								goto SecondTry;
							}

							Led_tString	undefinedWord	=	Led_tString (wordStart, wordEnd);
							if (fIgnoredWords.find (undefinedWord) != fIgnoredWords.end ()) {
								// push startRegion a bit forward over this word, and try again, but don't
								// set 'second try' flag cuz we haven't wrapped at the end of the document...
								startRegion += (wordEnd-charBuf);
								goto SecondTry;
							}
							MisspellingInfo*	mi	=	new MisspellingInfo ();
							mi->fUndefinedWord = undefinedWord;
							mi->fSuggestions = sce->GenerateSuggestions (mi->fUndefinedWord);
							size_t	selStart	=	startRegion + wordStart-charBuf;
							size_t	selEnd		=	selStart + (wordEnd-wordStart);
							fTI.SetSelection (selStart, selEnd);
							fTI.ScrollToSelection ();
							return mi;
						}
						else if (endRegion < fTI.GetEnd ()) {
							// no undefined words in that region, so try the next chunk. Stopping at word boundaries taken
							// care of with logic already above (for first case)
							startRegion = endRegion;
							goto SecondTry;
						}
						else if (firstTry) {
							// Wrap around...
							startRegion = 0;
							firstTry = false;
							goto SecondTry;
						}
					}
					return NULL;
				}
			override	void	DoIgnore ()
				{
					fTI.SetSelection (fTI.GetSelectionEnd (), fTI.GetSelectionEnd ());
					fTI.ScrollToSelection ();
				}
			override	void	DoIgnoreAll ()
				{
					{
						size_t							origSelStart	=	fTI.GetSelectionStart ();
						size_t							origSelEnd		=	fTI.GetSelectionEnd ();
						Led_SmallStackBuffer<Led_tChar>	text (origSelEnd-origSelStart+1);
						fTI.CopyOut (origSelStart, origSelEnd-origSelStart, text);
						Led_tString	ignoredWord = Led_tString (text, origSelEnd-origSelStart);
						fIgnoredWords.insert (ignoredWord);
					}
					DoIgnore ();
				}
			override	void	DoChange (const Led_tString& changeTo)
				{
					size_t						origSelStart	=	fTI.GetSelectionStart ();
					size_t						origSelEnd		=	fTI.GetSelectionEnd ();
					TextInteractor::SearchParameters	sp;
					{
						Led_SmallStackBuffer<Led_tChar>	text (origSelEnd-origSelStart+1);
						fTI.CopyOut (origSelStart, origSelEnd-origSelStart, text);
						sp.fMatchString = Led_tString (text, origSelEnd-origSelStart);
					}
					fTI.SetSelection (origSelStart, origSelStart);	// cuz OnDoReplaceCommand () looks from selectionEND
					fTI.OnDoReplaceCommand (sp, changeTo);
				}
			override	void	DoChangeAll (const Led_tString& changeTo)
				{
					size_t								origSelStart	=	fTI.GetSelectionStart ();
					size_t								origSelEnd		=	fTI.GetSelectionEnd ();
					TextInteractor::SearchParameters	sp;
					{
						Led_SmallStackBuffer<Led_tChar>	text (origSelEnd-origSelStart+1);
						fTI.CopyOut (origSelStart, origSelEnd-origSelStart, text);
						sp.fMatchString = Led_tString (text, origSelEnd-origSelStart);
					}
					fTI.OnDoReplaceAllCommand (sp, changeTo);
				}
			override	bool	AddToDictionaryEnabled () const
				{
					SpellCheckEngine*	sce	=	fTI.GetSpellCheckEngine ();
					if (sce != NULL) {
						SpellCheckEngine::UDInterface*	udi	=		sce->GetUDInterface ();
						if (udi != NULL) {
							return udi->AddWordToUserDictionarySupported ();
						}
					}
					return false;
				}
			override	void	AddToDictionary (const Led_tString& newWord)
				{
					SpellCheckEngine*	sce	=	fTI.GetSpellCheckEngine ();
					if (sce != NULL) {
						SpellCheckEngine::UDInterface*	udi	=		sce->GetUDInterface ();
						if (udi != NULL) {
							udi->AddWordToUserDictionary (newWord);
						}
					}

					fTI.SetSelection (fTI.GetSelectionEnd (), fTI.GetSelectionEnd ());
					fTI.ScrollToSelection ();
				}
			override	void	LookupOnWeb (const Led_tString& word)
				{
					const char	kURLBase[]	=	"http://dictionary.reference.com/search?q=";
					Led_URLManager::Get ().Open (kURLBase + Led_tString2ANSIString (word));
				}
			override	bool	OptionsDialogEnabled () const
				{
					// cuz no implementation of OptionsDialog () callback...
					return false;
				}
			override	void	OptionsDialog ()
				{
					// NYI. When enabled - change result of OptionsDialogEnabled ()
				}
		private:
			TextInteractor&		fTI;
			set<Led_tString>	fIgnoredWords;	// note we intentionally dont keep this around so that the
												// ignored words list stays around (lives) as long as the dialog...
	};
}







/*
 ********************************************************************************
 ************************* TextInteractor::DialogSupport ************************
 ********************************************************************************
 */
void	TextInteractor::DialogSupport::DisplayFindDialog (Led_tString* /*findText*/, const vector<Led_tString>& /*recentFindSuggestions*/, bool* /*wrapSearch*/, bool* /*wholeWordSearch*/, bool* /*caseSensative*/, bool* /*pressedOK*/)
{
	/*
	 *	You may want to use code something like this in your override:
	 *
		Led_StdDialogHelper_FindDialog	findDialog (::AfxGetResourceHandle (), NULL);

		findDialog.fFindText = *findText;
		findDialog.fRecentFindTextStrings = recentFindSuggestions;
		findDialog.fWrapSearch = *wrapSearch;
		findDialog.fWholeWordSearch = *wholeWordSearch;
		findDialog.fCaseSensativeSearch = *caseSensative;

		findDialog.DoModal ();

		*findText = findDialog.fFindText;
		*wrapSearch = findDialog.fWrapSearch;
		*wholeWordSearch = findDialog.fWholeWordSearch;
		*caseSensative = findDialog.fCaseSensativeSearch;
		*pressOK = findDialog.fPressedOK;
	*/
	Led_Assert (false);	// to use this - you must override this routine, and provide your own implementation, perhaps akin to the above.
}

TextInteractor::DialogSupport::ReplaceButtonPressed	TextInteractor::DialogSupport::DisplayReplaceDialog (Led_tString* /*findText*/, const vector<Led_tString>& /*recentFindSuggestions*/, Led_tString* /*replaceText*/, bool* /*wrapSearch*/, bool* /*wholeWordSearch*/, bool* /*caseSensative*/)
{
	Led_Assert (false);	// to use this - you must override this routine, and provide your own implementation, perhaps akin to the above.
	return eReplaceButton_Cancel;
}

void	TextInteractor::DialogSupport::DisplaySpellCheckDialog (SpellCheckDialogCallback& /*callback*/)
{
	Led_Assert (false);	// to use this - you must override this routine, and provide your own implementation, perhaps akin to the above.
}





/*
 ********************************************************************************
 ******************** TextInteractor::UndoableContextHelper *********************
 ********************************************************************************
 */
TextInteractor::UndoableContextHelper::UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, bool allowSmartCNPExpansion):
	fSimplePlainTextInsertOptimization (false),
	fTextInteractor (ti),
	fCmdName (cmdName),
	fSelStart (ti.GetSelectionStart ()),
	fSelEnd (ti.GetSelectionEnd ()),
	fBefore (NULL),
	fCommandComplete (false)
{
	if (allowSmartCNPExpansion) {
		ti.OptionallyExpandSelectionForSmartCutAndPasteModeDeletes (&fSelStart, &fSelEnd);
	}
	if (ti.GetCommandHandler () != NULL) {
		ti.PreInteractiveUndoHelper (&fBefore, fSelStart, fSelEnd, ti.GetSelectionStart (), ti.GetSelectionEnd ());
	}
}

TextInteractor::UndoableContextHelper::UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, size_t regionAndSelStart, size_t regionAndSelEnd, bool allowSmartCNPExpansion):
	fSimplePlainTextInsertOptimization (false),
	fTextInteractor (ti),
	fCmdName (cmdName),
	fSelStart (regionAndSelStart),
	fSelEnd (regionAndSelEnd),
	fBefore (NULL),
	fCommandComplete (false)
{
	if (allowSmartCNPExpansion) {
		ti.OptionallyExpandSelectionForSmartCutAndPasteModeDeletes (&fSelStart, &fSelEnd);
	}
	if (ti.GetCommandHandler () != NULL) {
		ti.PreInteractiveUndoHelper (&fBefore, fSelStart, fSelEnd, regionAndSelStart, regionAndSelEnd);
	}
}

TextInteractor::UndoableContextHelper::UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd, bool allowSmartCNPExpansion):
	fSimplePlainTextInsertOptimization (false),
	fTextInteractor (ti),
	fCmdName (cmdName),
	fSelStart (regionStart),
	fSelEnd (regionEnd),
	fBefore (NULL),
	fCommandComplete (false)
{
	if (allowSmartCNPExpansion) {
		ti.OptionallyExpandSelectionForSmartCutAndPasteModeDeletes (&fSelStart, &fSelEnd);
	}
	if (ti.GetCommandHandler () != NULL) {
		ti.PreInteractiveUndoHelper (&fBefore, fSelStart, fSelEnd, selStart, selEnd);
	}
}

TextInteractor::UndoableContextHelper::~UndoableContextHelper ()
{
	if (not fCommandComplete) {
		delete fBefore;
	}
}

/*
@METHOD:		TextInteractor::UndoableContextHelper::CommandComplete
@ACCESS:		public
@DESCRIPTION:	<p>There are two overloaded versions of this function. Both require knowledge of
			the END of the updated (inserted) region. The one with no arguments assumes the selectionEnd
			is the end of the inserted region. The second overload allows you to explicitly specify the
			end of the inserted region.
			</p>
				<p>Call this function when a command has been completed (See the @'TextInteractor::UndoableContextHelper' docs
			for how to structure the CTOR/CommandComplete calls). Failure to call this after constructing a UndoableContextHelper
			object results in the command not being recorded (as is appropriate if there is an exception thrown and the command
			is not performed).
			</p>
*/
void	TextInteractor::UndoableContextHelper::CommandComplete ()
{
	fTextInteractor.ScrollToSelection ();
	if (fTextInteractor.GetCommandHandler () != NULL) {
		if (GetSimplePlainTextInsertOptimization ()) {
			fTextInteractor.PostInteractiveSimpleCharInsertUndoHelper (&fBefore, fSelStart, fTextInteractor.GetSelectionEnd (), fCmdName);
		}
		else {
			fTextInteractor.PostInteractiveUndoHelper (&fBefore, fSelStart, fTextInteractor.GetSelectionEnd (), fCmdName);
		}
	}
	fCommandComplete = true;
}

void	TextInteractor::UndoableContextHelper::CommandComplete (size_t endOfInsert)
{
	fTextInteractor.ScrollToSelection ();
	if (fTextInteractor.GetCommandHandler () != NULL) {
		if (GetSimplePlainTextInsertOptimization ()) {
			fTextInteractor.PostInteractiveSimpleCharInsertUndoHelper (&fBefore, fSelStart, endOfInsert, fCmdName);
		}
		else {
			fTextInteractor.PostInteractiveUndoHelper (&fBefore, fSelStart, endOfInsert, fCmdName);
		}
	}
	fCommandComplete = true;
}







/*
 ********************************************************************************
 ************************* TextInteractor::PreReplaceInfo ***********************
 ********************************************************************************
 */
TextInteractor::PreReplaceInfo::PreReplaceInfo ():
	fTextInteractor (NULL),
	fUpdateMode (TextInteractor::eNoUpdate),
	fFrom (0),
	fTo (0),
	fWithWhatCharCount (0),
	fBoundingUpdateMarker (),
	fBoundingUpdateHeight (0),
	fStableTypingRegionHeight (0)
{
}

TextInteractor::PreReplaceInfo::~PreReplaceInfo ()
{
	if (fTextInteractor != NULL) {
		fTextInteractor->GetTextStore ().RemoveMarker (&fBoundingUpdateMarker);
		fTextInteractor = NULL;
	}
}

TextInteractor::UpdateMode	TextInteractor::PreReplaceInfo::GetUpdateMode () const
{
	return fUpdateMode;
}

size_t	TextInteractor::PreReplaceInfo::GetFrom () const
{
	return fFrom;
}

size_t	TextInteractor::PreReplaceInfo::GetTo () const
{
	return fTo;
}






/*
 ********************************************************************************
 ******************************** TextInteractor ********************************
 ********************************************************************************
 */

TextInteractor::CommandNames		TextInteractor::sCommandNames		=		TextInteractor::MakeDefaultCommandNames ();
TextInteractor::DialogSupport*		TextInteractor::sDialogSupport		=		NULL;
TextInteractor::SearchParameters	TextInteractor::sSearchParameters;
TextInteractor::ReplaceParameters	TextInteractor::sReplaceParameters;

TextInteractor::TextInteractor ():
	TextImager (),
	fCommandHandler (NULL),
	fSpellCheckEngine (NULL),
	fSupressCommandBreaksContext (false),
	fDefaultUpdateMode (eDelayedUpdate),
	fSmartCutAndPasteMode (true),
	fClickCount (0),
	fLastClickedAt (0.0f),
	fLastMouseDownAt (Led_Point (0, 0)),
	fWholeWindowInvalid (false),
	fUseSecondaryHilight (false),
#if		defined (qUseGDIScrollbitsForScrolling) && qUseGDIScrollbitsForScrolling==0
	fUseBitmapScrollingOptimization (false),
#else
	fUseBitmapScrollingOptimization (true),
#endif
	fSupressTypedControlCharacters (true),
	fInteractiveUpdadeMode (eIndeterminateInteractiveUpdateMode),
	fTmpPreReplaceInfo (),
	fDoingUpdateModeReplaceOn (NULL),
	fCaretShown (false),
	fLeftSideOfSelectionInteresting (false),
	fCaretShownAfterPos (true),
	fInternalizer (),
	fExternalizer (),
	//fScrollBarType (),
	fScrollBarParamsValid (false),
	fLastScrolledAt (0.0f)
#if		qMultiByteCharacters
	//fMultiByteInputCharBuf (),
#endif
{
#if		qMultiByteCharacters
	fMultiByteInputCharBuf[0] = '\0';
#endif
	fScrollBarType[h] = eScrollBarNever;
	fScrollBarType[v] = eScrollBarNever;
}

TextInteractor::~TextInteractor ()
{
	Led_Assert (fCommandHandler == NULL);	// must be set to NULL before we are destroyed...
											// just sanity check - no real reason...
	Led_Assert (fSpellCheckEngine == NULL);	// DITTO
}

TextInteractor::CommandNames	TextInteractor::MakeDefaultCommandNames ()
{
	TextInteractor::CommandNames	cmdNames;
	cmdNames.fTypingCommandName					=	Led_SDK_TCHAROF ("Typing");
	cmdNames.fCutCommandName					=	Led_SDK_TCHAROF ("Cut");
	cmdNames.fClearCommandName					=	Led_SDK_TCHAROF ("Clear");
	cmdNames.fPasteCommandName					=	Led_SDK_TCHAROF ("Paste");
	cmdNames.fUndoFormatString					=	Led_SDK_TCHAROF ("Undo %s");
	cmdNames.fRedoFormatString					=	Led_SDK_TCHAROF ("ReDo %s");
	#if		qWindows
		cmdNames.fUndoFormatString				+=	Led_SDK_TCHAROF ("\tCtrl+Z");
		cmdNames.fRedoFormatString				+=	Led_SDK_TCHAROF ("\tCtrl+Y");
	#endif
	cmdNames.fReplaceCommandName				=	Led_SDK_TCHAROF ("Replace");
	cmdNames.fReplaceAllCommandName				=	Led_SDK_TCHAROF ("Replace All");
	cmdNames.fReplaceAllInSelectionCommandName	=	Led_SDK_TCHAROF ("Replace All In Selection");
	return cmdNames;
}

/*
@METHOD:		TextInteractor::OnUpdateCommand
@ACCESS:		public
@DESCRIPTION:	<p>
			</p>
*/
bool	TextInteractor::OnUpdateCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	switch (enabler->GetCmdID ()) {
		case	kSelectAll_CmdID:			{	enabler->SetEnabled (true);					return true;	}
		case	kCut_CmdID:					{	OnUpdateCutCopyClearCommand (enabler);		return true;	}
		case	kCopy_CmdID:				{	OnUpdateCutCopyClearCommand (enabler);		return true;	}
		case	kPaste_CmdID:				{	OnUpdatePasteCommand (enabler);				return true;	}
		case	kClear_CmdID:				{	OnUpdateCutCopyClearCommand (enabler);		return true;	}
		case	kUndo_CmdID:				{	OnUpdateUndoRedoCommand (enabler);			return true;	}
		case	kRedo_CmdID:				{	OnUpdateUndoRedoCommand (enabler);			return true;	}
		case	kFind_CmdID:				{	OnUpdateFindCommands (enabler);				return true;	}
		case	kFindAgain_CmdID:			{	OnUpdateFindCommands (enabler);				return true;	}
		case	kEnterFindString_CmdID:		{	OnUpdateFindCommands (enabler);				return true;	}
		case	kReplace_CmdID:				{	OnUpdateFindCommands (enabler);				return true;	}
		case	kReplaceAgain_CmdID:		{	OnUpdateFindCommands (enabler);				return true;	}
		case	kSpellCheck_CmdID:			{	OnUpdateSpellCheckCommand (enabler);		return true;	}
		case	kSelectWord_CmdID:
		case	kSelectTextRow_CmdID:
		case	kSelectParagraph_CmdID:		{	OnUpdateSelectTextCommand (enabler);		return true;	}
	}
	return false;
}

/*
@METHOD:		TextInteractor::OnPerformCommand
@ACCESS:		public
@DESCRIPTION:	<p>
			</p>
*/
bool	TextInteractor::OnPerformCommand (CommandNumber commandNumber)
{
	switch (commandNumber) {
		case	kSelectAll_CmdID:			{	OnSelectAllCommand ();		return true;	}
		case	kCut_CmdID:					{	OnCutCommand ();			return true;	}
		case	kCopy_CmdID:				{	OnCopyCommand ();			return true;	}
		case	kPaste_CmdID:				{	OnPasteCommand ();			return true;	}
		case	kClear_CmdID:				{	OnClearCommand ();			return true;	}
		case	kUndo_CmdID:				{	OnUndoCommand ();			return true;	}
		case	kRedo_CmdID:				{	OnRedoCommand ();			return true;	}
		case	kFind_CmdID:				{	OnFindCommand ();			return true;	}
		case	kFindAgain_CmdID:			{	OnFindAgainCommand ();		return true;	}
		case	kEnterFindString_CmdID:		{	OnEnterFindString ();		return true;	}
		case	kReplace_CmdID:				{	OnReplaceCommand ();		return true;	}
		case	kReplaceAgain_CmdID:		{	OnReplaceAgainCommand ();	return true;	}
		case	kSpellCheck_CmdID:			{	OnSpellCheckCommand ();		return true;	}
		case	kSelectWord_CmdID:
		case	kSelectTextRow_CmdID:
		case	kSelectParagraph_CmdID:		{	OnPerformSelectTextCommand (commandNumber);	return true;	}
	}
	return false;
}

void	TextInteractor::OnUpdateCutCopyClearCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	size_t	start;
	size_t	end;
	static_cast<TextImager*> (this)->GetSelection (&start, &end);
	enabler->SetEnabled (start != end);
}

void	TextInteractor::OnUpdatePasteCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	//tmphack
	enabler->SetEnabled (true);
}

void	TextInteractor::OnUpdateUndoRedoCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	if (GetCommandHandler () == NULL) {
		enabler->SetEnabled (false);
	}
	else {
		if (enabler->GetCmdID () == kUndo_CmdID) {
			enabler->SetEnabled (GetCommandHandler ()->CanUndo ());

			Led_SDK_String	menuItemText	=	Format (GetCommandNames ().fUndoFormatString.c_str (), GetCommandHandler ()->GetUndoCmdName ());
			enabler->SetText (menuItemText.c_str ());
		}
		else if (enabler->GetCmdID () == kRedo_CmdID)  {
			enabler->SetEnabled (GetCommandHandler ()->CanRedo ());

			Led_SDK_String	menuItemText	=	Format (GetCommandNames ().fRedoFormatString.c_str (), GetCommandHandler ()->GetRedoCmdName ());
			enabler->SetText (menuItemText.c_str ());
		}
	}
}

void	TextInteractor::OnUpdateSelectTextCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	enabler->SetEnabled (true);
}

void	TextInteractor::OnPerformSelectTextCommand (CommandNumber commandNumber)
{
	// Note: these are intentionally made not undoable, since they don't modify data
	size_t	oldSelStart	=	GetSelectionStart ();
	size_t	oldSelEnd	=	GetSelectionEnd ();
	size_t	newSelStart	=	oldSelStart;
	size_t	newSelEnd	=	oldSelEnd;
	switch (commandNumber) {
		case	kSelectWord_CmdID: {
			size_t	wordStart	=	0;
			size_t	wordEnd		=	0;
			bool	wordReal	=	false;
			GetTextStore ().FindWordBreaks (oldSelStart, &wordStart, &wordEnd, &wordReal);
			if (wordReal) {
				Led_Assert (wordStart <= oldSelStart);
				newSelStart	=	wordStart;
			}
			else {
				// See if we were just after a word
				GetTextStore ().FindWordBreaks (FindPreviousCharacter (oldSelStart), &wordStart, &wordEnd, &wordReal);
				if (wordReal) {
					Led_Assert (wordStart <= oldSelStart);
					newSelStart	=	wordStart;
				}
			}

			GetTextStore ().FindWordBreaks (oldSelEnd, &wordStart, &wordEnd, &wordReal);
			if (wordReal) {
				newSelEnd	=	wordEnd;
			}
		}
		break;
		case	kSelectTextRow_CmdID: {
			newSelStart = GetStartOfRowContainingPosition (oldSelStart);
			// check if oldSelEnd is ALREADY the end of a row - and if so - don't change it,
			// but otherwise, do so.
			if (oldSelStart == oldSelEnd or oldSelEnd != GetStartOfRowContainingPosition (oldSelEnd)) {
				newSelEnd = GetEndOfRowContainingPosition (oldSelEnd);
			}
		}
		break;
		case	kSelectParagraph_CmdID: {
			newSelStart = GetTextStore ().GetStartOfLineContainingPosition (oldSelStart);

			// check if oldSelEnd is ALREADY the end of a paragraph - and if so - don't change it,
			// but otherwise, do so.
			if (oldSelStart == oldSelEnd or oldSelEnd != GetTextStore ().GetStartOfLineContainingPosition (oldSelEnd)) {
				newSelEnd = GetTextStore ().GetEndOfLineContainingPosition (oldSelEnd);
				// grab one past endofline - so we select the NEWLINE at the end...
				if (newSelEnd < GetEnd ()) {
					Led_tChar	c;
					GetTextStore ().CopyOut (newSelEnd, 1, &c);
					if (c == '\n') {
						newSelEnd = FindNextCharacter (newSelEnd);
					}
				}
			}
		}
		break;
	}
	SetSelection (newSelStart, newSelEnd);
}

namespace	{
	vector<Led_tString>	MergeRecentFindStrings (const Led_tString& s, const vector<Led_tString>& oldRecents);
}

void	TextInteractor::OnFindCommand ()
{
	SearchParameters	parameters	=	GetSearchParameters ();
	bool				pressedOK	=	false;
	GetDialogSupport ().DisplayFindDialog (&parameters.fMatchString, parameters.fRecentFindStrings, &parameters.fWrapSearch, &parameters.fWholeWordSearch, &parameters.fCaseSensativeSearch, &pressedOK);
	parameters.fRecentFindStrings = MergeRecentFindStrings (parameters.fMatchString, parameters.fRecentFindStrings);
	SetSearchParameters (parameters);
	if (pressedOK) {
		OnFindAgainCommand ();
	}
}

void	TextInteractor::OnReplaceCommand ()
{
	SearchParameters	parameters	=	GetSearchParameters ();
	ReplaceParameters	rParameters	=	GetReplaceParameters ();
	DialogSupport::ReplaceButtonPressed	pressed	=	GetDialogSupport ().DisplayReplaceDialog (&parameters.fMatchString, parameters.fRecentFindStrings, &rParameters.fReplaceWith, &parameters.fWrapSearch, &parameters.fWholeWordSearch, &parameters.fCaseSensativeSearch);
	parameters.fRecentFindStrings = MergeRecentFindStrings (parameters.fMatchString, parameters.fRecentFindStrings);
	SetSearchParameters (parameters);
	SetReplaceParameters (rParameters);
	switch (pressed) {
		case	TextInteractor::DialogSupport::eReplaceButton_Find:						OnFindAgainCommand (); break;
		case	TextInteractor::DialogSupport::eReplaceButton_Replace:					OnDoReplaceCommand (parameters, rParameters.fReplaceWith); break;
		case	TextInteractor::DialogSupport::eReplaceButton_ReplaceAll:				OnDoReplaceAllCommand (parameters, rParameters.fReplaceWith); break;
		case	TextInteractor::DialogSupport::eReplaceButton_ReplaceAllInSelection:	OnDoReplaceAllInSelectionCommand (parameters, rParameters.fReplaceWith); break;
	}
}

void	TextInteractor::OnReplaceAgainCommand ()
{
	SearchParameters	parameters	=	GetSearchParameters ();
	ReplaceParameters	rParameters	=	GetReplaceParameters ();
	OnDoReplaceCommand (parameters, rParameters.fReplaceWith);
}

void	TextInteractor::OnDoReplaceCommand (const SearchParameters& searchFor, const Led_tString& replaceWith)
{
	BreakInGroupedCommands ();
	// search for last text entered into find dialog (could have been a find again).
	size_t	origSelStart	=	GetSelectionStart ();
	size_t	origSelEnd		=	GetSelectionEnd ();
	size_t	whereTo	=	GetTextStore ().Find (searchFor, origSelEnd);
	if ((whereTo == kBadIndex) or (whereTo == origSelStart and whereTo + searchFor.fMatchString.length () == origSelEnd)) {
		Led_BeepNotify ();
	}
	else {
		InteractiveModeUpdater	iuMode (*this);
		size_t	replaceStart	=	whereTo;
		size_t	replaceEnd		=	whereTo + searchFor.fMatchString.length ();
		UndoableContextHelper	undoContext (*this, GetCommandNames ().fReplaceCommandName, replaceStart, replaceEnd, GetSelectionStart (), GetSelectionEnd (), false);
			{
				InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), replaceWith.c_str (), replaceWith.length ());
				SetSelection (whereTo, whereTo + replaceWith.length ());
			}
		undoContext.CommandComplete ();
		ScrollToSelection ();
	}
	BreakInGroupedCommands ();
}

void	TextInteractor::OnDoReplaceAllCommand (const SearchParameters& searchFor, const Led_tString& replaceWith)
{
	InteractiveModeUpdater	iuMode (*this);
	BreakInGroupedCommands ();
	size_t	startAt	=	0;
	while (true) {
		size_t	whereTo	=	GetTextStore ().Find (searchFor, startAt, GetTextStore ().GetEnd ());
		if (whereTo == kBadIndex) {
			break;
		}
		else {
			size_t	replaceStart	=	whereTo;
			size_t	replaceEnd		=	whereTo + searchFor.fMatchString.length ();
			UndoableContextHelper	undoContext (*this, GetCommandNames ().fReplaceAllCommandName, replaceStart, replaceEnd, GetSelectionStart (), GetSelectionEnd (), false);
				{
					InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), replaceWith.c_str (), replaceWith.length ());
					SetSelection (whereTo, whereTo + replaceWith.length ());
					startAt = whereTo + replaceWith.length ();
				}
			undoContext.CommandComplete ();
		}
	}
	ScrollToSelection ();
	BreakInGroupedCommands ();
}

void	TextInteractor::OnDoReplaceAllInSelectionCommand (const SearchParameters& searchFor, const Led_tString& replaceWith)
{
	InteractiveModeUpdater	iuMode (*this);
	BreakInGroupedCommands ();
	size_t	startAt	=	GetSelectionStart ();
	TempMarker	selectionRegion (GetTextStore (), startAt, GetSelectionEnd ());
	while (true) {
		Led_Assert (startAt <= selectionRegion.GetEnd ());
		size_t	whereTo	=	GetTextStore ().Find (searchFor, startAt, selectionRegion.GetEnd ());
		if (whereTo == kBadIndex) {
			break;
		}
		else {
			size_t	replaceStart	=	whereTo;
			size_t	replaceEnd		=	whereTo + searchFor.fMatchString.length ();
			UndoableContextHelper	undoContext (*this, GetCommandNames ().fReplaceAllInSelectionCommandName, replaceStart, replaceEnd, GetSelectionStart (), GetSelectionEnd (), false);
				{
					InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), replaceWith.c_str (), replaceWith.length ());
					SetSelection (whereTo, whereTo + replaceWith.length ());
					startAt = whereTo + replaceWith.length ();
				}
			undoContext.CommandComplete ();
		}
	}
	ScrollToSelection ();
	BreakInGroupedCommands ();
}

void	TextInteractor::OnFindAgainCommand ()
{
	TextStore::SearchParameters	parameters	=	GetSearchParameters ();

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

void	TextInteractor::OnEnterFindString ()
{
	SearchParameters	parameters	=	GetSearchParameters ();

	size_t	selStart	=	GetSelectionStart ();
	size_t	selEnd		=	GetSelectionEnd ();
	size_t	selLength	=	selEnd-selStart;

	Led_SmallStackBuffer<Led_tChar>	buf (selLength);
	CopyOut (selStart, selLength, buf);
	parameters.fMatchString = Led_tString (buf, selLength);
	parameters.fRecentFindStrings = MergeRecentFindStrings (parameters.fMatchString, parameters.fRecentFindStrings);
	SetSearchParameters (parameters);
}

void	TextInteractor::OnUpdateFindCommands (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	if (enabler->GetCmdID () == kFind_CmdID) {
		enabler->SetEnabled (true);
	}
	else if (enabler->GetCmdID () == kFindAgain_CmdID) {
		enabler->SetEnabled (GetSearchParameters ().fMatchString.length () != 0);
	}
	else if (enabler->GetCmdID () == kEnterFindString_CmdID) {
		enabler->SetEnabled (GetSelectionStart () != GetSelectionEnd ());
	}
	else if (enabler->GetCmdID () == kReplace_CmdID) {
		enabler->SetEnabled (true);
	}
	else if (enabler->GetCmdID () == kReplaceAgain_CmdID) {
		enabler->SetEnabled (GetSearchParameters ().fMatchString.length () != 0);
	}
}

TextInteractor::SearchParameters	TextInteractor::GetSearchParameters () const
{
	return sSearchParameters;
}

void	TextInteractor::SetSearchParameters (const SearchParameters& sp)
{
	sSearchParameters = sp;
}

TextInteractor::ReplaceParameters	TextInteractor::GetReplaceParameters () const
{
	return sReplaceParameters;
}

void	TextInteractor::SetReplaceParameters (const ReplaceParameters& rp)
{
	sReplaceParameters = rp;
}

vector<Led_tString>	TextInteractor::MergeRecentFindStrings (const Led_tString& s, const vector<Led_tString>& oldRecents)
{
	const	unsigned	int	kMaxEntries	=	20;
	vector<Led_tString>	result	=	oldRecents;
	// See if the given string appears in the list. If so - erase it (so it only appears once). Prepend the new string
	// (most recent) to the top of the list. Truncate any entries beyond some magic #.
	vector<Led_tString>::iterator	i	=	std::find (result.begin (), result.end (), s);
	if (i != result.end ()) {
		result.erase (i);
	}
	result.insert (result.begin (), s);
	if (result.size () > kMaxEntries) {
		result.erase (result.begin () + kMaxEntries, result.end ());
	}
	return result;
}

void	TextInteractor::OnSpellCheckCommand ()
{
	if (GetSpellCheckEngine () == NULL) {
		Led_BeepNotify ();
	}
	else {
		MyCallback	cb (*this);
		GetDialogSupport ().DisplaySpellCheckDialog (cb);
	}
}

void	TextInteractor::OnUpdateSpellCheckCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	enabler->SetEnabled (GetSpellCheckEngine () != NULL);
}

/*
@METHOD:		TextInteractor::SetDefaultUpdateMode
@ACCESS:		protected
@DESCRIPTION:	<p>TextInteractor's have an associated default UpdateMode. This is the update mode which is
			used by methods which used the eDefaultUpdateMode argument (most default UpdateMode args in Led are this value).</p>
				<p>This should <em>not</em> be set directly. Instead, instantiate a @'TextInteractor::TemporarilySetUpdateMode' object on
			the stack.</p>
*/
void	TextInteractor::SetDefaultUpdateMode (UpdateMode defaultUpdateMode)
{
	if (defaultUpdateMode != eDefaultUpdate) {	// setting update mode to 'eDefaultUpdate' is synonomous with a NO-OP
		fDefaultUpdateMode = defaultUpdateMode;
	}
}


/*
@METHOD:		TextInteractor::LooksLikeSmartPastableText
@ACCESS:		protected
@DESCRIPTION:	<p>This is called internally when text is pasted or dropped in a Led text buffer.
			Right now - it looks at the selection range to see if the text surrounding the selection boundaries
			has space characters and sets this information into @'TextInteractor::SmartCNPInfo'.
			</p>
				<p>This code is still far from perfect, but now (SPR#1286) appears to work pretty decently.
			It may still see significant revision at some point as part of SPR #1040.
			</p>
*/
bool	TextInteractor::LooksLikeSmartPastableText (const Led_tChar* text, size_t /*nTextTChars*/, SmartCNPInfo* smartCNPInfo) const
{
	Led_RequireNotNil (text);
	Led_RequireNotNil (smartCNPInfo);
	Led_Arg_Unused (text);
	if (GetSmartCutAndPasteMode ()) {
		size_t	selStart	=	GetSelectionStart ();
		size_t	selEnd		=	GetSelectionEnd ();

		// If both current and prev chars where non-space, insert a space between.
		if (selStart == 0 or selStart >= GetEnd ()) {
			smartCNPInfo->fWordBreakAtSelStart = true;
		}
		else {
			size_t		prev	=	FindPreviousCharacter (selStart);
			Led_tChar	prevC;
			CopyOut (prev, 1, &prevC);
			Led_tChar	c;
			CopyOut (selStart, 1, &c);
			smartCNPInfo->fWordBreakAtSelStart = (IsShouldBeSepWithWhitespaceWordChar (c) != IsShouldBeSepWithWhitespaceWordChar (prevC));
		}

		if (selEnd == 0 or selEnd >= GetEnd ()) {
			smartCNPInfo->fWordBreakAtSelEnd = true;
		}
		else {
			size_t		prev	=	FindPreviousCharacter (selEnd);
			Led_tChar	prevC;
			CopyOut (prev, 1, &prevC);
			Led_tChar	c;
			CopyOut (selEnd, 1, &c);
			smartCNPInfo->fWordBreakAtSelEnd = (IsShouldBeSepWithWhitespaceWordChar (c) != IsShouldBeSepWithWhitespaceWordChar (prevC));
		}
	}
	return true;
}

/*
@METHOD:		TextInteractor::OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds
@ACCESS:		protected
@DESCRIPTION:	<p>See @'TextInteractor::GetSmartCutAndPasteMode'.</p>
				<p>The way in which which we handle smart cut-and-paste is twofold. We have sometimes add extra spaces
			(which is done in this routine), and sometimes we must delete extra spaces
			(which is done in @'TextInteractor::OptionallyExpandSelectionForSmartCutAndPasteModeDeletes').</p>
				<p>We must be careful todo our space insertion in such a way that it is friendly to the UNDO system (doesn't
			appear as a separate undo event etc).</p>
				<p>Note - this routine is called after the update has taken place. And it looks at the text
			at the LHS of where the insertion was done, and at the RHS of where it finihsed. The RHS of where
			it finished is always assumed to be the result of a call to @'TextImager::GetSelectionEnd'. And the start
			is given by the 'selStart' argument.</p>
*/
void	TextInteractor::OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (size_t selStart, const SmartCNPInfo& smartCNPInfo)
{
	size_t	selEnd	=	GetSelectionEnd ();

	Led_Require (0 <= selStart);
	Led_Require (selStart <= selEnd);
	Led_Require (selEnd <= GetTextStore ().GetEnd ());

	if (GetSmartCutAndPasteMode ()) {
		if (selEnd > 0 and selEnd < GetTextStore ().GetEnd ()) {
			size_t		prev	=	FindPreviousCharacter (selEnd);
			Led_tChar	prevC;
			CopyOut (prev, 1, &prevC);
			Led_tChar	c;
			CopyOut (selEnd, 1, &c);
			if (smartCNPInfo.fWordBreakAtSelEnd and IsShouldBeSepWithWhitespaceWordChar (c) and IsShouldBeSepWithWhitespaceWordChar (prevC)) {
				InteractiveReplace_ (selEnd, selEnd, LED_TCHAR_OF (" "), 1, false);
				// Cannot pass 'updateSelection' flag in the above InteractiveReplace_ () call because
				// that code only works under restrictive circumstances (see require calls in that code).
				// Still - we MUST advance the end of the selection to take into account the space added. Not
				// just for cosmetic reasons, but because the current (as of Led 2.3b8) - and probably all future versions-
				// Undo code uses the selection end at the end of insertions to see how much text needs to be
				// saved for undoing purposes.
				// LGP990401
				selEnd++;
				SetSelection (selEnd, selEnd);
			}
		}
		// If both current and prev chars where non-space, insert a space between.
		if (selStart > 0 and selStart < GetTextStore ().GetEnd ()) {
			size_t		prev	=	FindPreviousCharacter (selStart);
			Led_tChar	prevC;
			CopyOut (prev, 1, &prevC);
			Led_tChar	c;
			CopyOut (selStart, 1, &c);
			if (smartCNPInfo.fWordBreakAtSelStart and IsShouldBeSepWithWhitespaceWordChar (c) and IsShouldBeSepWithWhitespaceWordChar (prevC)) {
				// Programming Confusion Note:
				//
				//	Not quite sure why we pass the updateCursorPosition=false param here. Seems like it should
				//	be true. But that seems to screw up the enclosing undo information.
				//	Well, I guess if it aint broke, don't fix it. Least not right now... --LGP 970315.
				InteractiveReplace_ (selStart, selStart, LED_TCHAR_OF (" "), 1, false);
			}
			else {
				if (IsSmartSpace (c) and IsSmartSpace (prevC)) {
					InteractiveReplace_ (selStart, FindNextCharacter (selStart), NULL, 0, false);
				}
			}
		}
	}
}

/*
@METHOD:		TextInteractor::OptionallyExpandSelectionForSmartCutAndPasteModeDeletes
@DESCRIPTION:	<p>See @'TextInteractor::GetSmartCutAndPasteMode'.</p>
*/
void	TextInteractor::OptionallyExpandSelectionForSmartCutAndPasteModeDeletes (size_t* selStart, size_t* selEnd)
{
	Led_RequireNotNil (selStart);
	Led_RequireNotNil (selEnd);
	Led_Require (0 <= *selStart);
	Led_Require (*selStart <= *selEnd);
	Led_Require (*selEnd <= GetTextStore ().GetEnd ());

	if (GetSmartCutAndPasteMode ()) {
		size_t	realStart		=	*selStart;
		size_t	realEnd			=	*selEnd;
		size_t	newStart		=	realStart;
		size_t	newEnd			=	realEnd;

		// if selStart is in whitespace, or in the middle of a word,
		// or selSend is just after whitespace or in the middle of a word then don't be smart.
		// LGP 970315 (amended LGP990415)
		{
			if (realStart < GetEnd ()) {
				Led_tChar	c;
				CopyOut (realStart, 1, &c);
				if (CharacterProperties::IsSpace (c)) {
					return;
				}
				if (realStart > 0) {
					CopyOut (FindPreviousCharacter (realStart), 1, &c);
					if (IsShouldBeSepWithWhitespaceWordChar (c)) {
						return;
					}
				}
			}
			
			if (realStart < realEnd and realEnd < GetEnd ()) {
				Led_tChar	c;
				CopyOut (FindPreviousCharacter (realEnd), 1, &c);
				if (CharacterProperties::IsSpace (c)) {
					return;
				}
				if (realEnd < GetEnd ()) {
					CopyOut (realEnd, 1, &c);
					if (IsShouldBeSepWithWhitespaceWordChar (c)) {
						return;
					}
				}
			}
		}

		// Now see if at the cursor we have extra spaces - scan first backward, and then forward. Should be at most one.
		Led_tChar	c;
		// back
		{
			if (newStart > 0) {
				size_t	prev	=	FindPreviousCharacter (newStart);
				CopyOut (prev, 1, &c);
				size_t	prevprev	=	FindPreviousCharacter (prev);
				Led_tChar	prevprevC;
				CopyOut (prevprev, 1, &prevprevC);
				if (prevprev != prev and IsSmartSpace (c) and not (IsSmartSpace (prevprevC) or prevprevC == '\n')) {
					newStart = prev;
				}
			}
		}
		// and forth
		{
			if (newEnd < GetEnd ()) {
				CopyOut (newEnd, 1, &c);
				Led_tChar	charBeforeStart	=	'\0';
				if (newStart != 0) {
					CopyOut (FindPreviousCharacter (newStart), 1, &charBeforeStart);
				}
				if (IsSmartSpace (c)) {
					Led_Assert (newEnd < FindNextCharacter (newEnd));	// Assert not looping!
					newEnd = FindNextCharacter (newEnd);
					if (newEnd < GetEnd ()) {
						// As a result of this change - don't put two chars together that should be separated by whitespace
						Led_tChar	nextChar	=	'\0';
						CopyOut (newEnd, 1, &nextChar);
						if (IsShouldBeSepWithWhitespaceWordChar (charBeforeStart) and IsShouldBeSepWithWhitespaceWordChar (nextChar)) {
							newEnd = FindPreviousCharacter (newEnd);
						}
					}
				}
			}
		}

		*selStart = newStart;
		*selEnd = newEnd;
	}
}

/*
@METHOD:		TextInteractor::SetSelectionShown
@DESCRIPTION:	<p>See TextInteractor::GetSelectionShown. Typically this method isn't called directly by user code, but
	from within the class library wrappers (e.g. Led_MFC) on gain/lose focus events.</p>
*/
void	TextInteractor::SetSelectionShown (bool shown)
{
	SetSelectionShown (shown, eDefaultUpdate);
}

void	TextInteractor::SetSelectionShown (bool shown, UpdateMode updateMode)
{
	if (GetSelectionShown () != shown) {
		TextImager::SetSelectionShown (shown);
		Refresh (GetSelectionStart (), GetSelectionEnd (), updateMode);
	}
}

/*
@METHOD:		TextInteractor::SetSelection
@DESCRIPTION:	<p>See @'TextInteractor::GetSelection'.</p>
*/
void	TextInteractor::SetSelection (size_t start, size_t end)
{
	Led_Assert (end <= GetEnd ());
#if		qMultiByteCharacters
	Assert_CharPosDoesNotSplitCharacter (start);
	Assert_CharPosDoesNotSplitCharacter (end);
#endif

	UpdateMode	updateMode	=	GetDefaultUpdateMode ();

	// This isn't quite perfect for the case of eImmediateUpdate- but should be close enough...
	if (start != GetSelectionStart () or end != GetSelectionEnd ()) {
		IdleManager::NonIdleContext	nonIdleContext;

		size_t	oldSelectionStart	=	GetSelectionStart ();
		size_t	oldSelectionEnd		=	GetSelectionEnd ();

		/*
		 *	Update fLeftSideOfSelectionInteresting. if start changed, we are interested in LHS, and
		 *	if end changed we are interested in RHS. But if BOTH changed then just assume interested
		 *	in RHS.
		 */
		if (start == GetSelectionStart ()) {
			fLeftSideOfSelectionInteresting = false;
		}
		else if (end == GetSelectionEnd ()) {
			fLeftSideOfSelectionInteresting = true;
		}
		else if ((start < GetSelectionStart ()) == (end < GetSelectionEnd ())) {
			fLeftSideOfSelectionInteresting = (start < GetSelectionStart ());
		}

		/*
		 *	Note we must invalidate the caret state before and after changing the selction so
		 *	that we erase the old position, and draw in the new.
		 */
		InvalidateCaretState ();

		TextImager::SetSelection (start, end);

		if ((GetSelectionShown () or GetUseSecondaryHilight ()) and updateMode != eNoUpdate) {
			/*
			 *	What we REALLY want to invalidate is the CHANGE in hilight region. Not the UNION.
			 *
			 *	The difference (invalid area) is Union (a,b)-Intersection(a,b). Now with the facilities at our
			 *	disposal, there doesn't appear any obvious way to compute this. So we hack it out
			 *	a bit.
			 */
			UpdateMode	useUpdateMode	=	(updateMode==eImmediateUpdate)? eDelayedUpdate: updateMode;

			size_t	lhsOuter	=	Led_Min (oldSelectionStart, GetSelectionStart ());	// left of ALL 4
			size_t	rhsOuter	=	Led_Max (oldSelectionEnd, GetSelectionEnd ());		// right of ALL 4
			size_t	lhsInner	=	Led_Max (oldSelectionStart, GetSelectionStart ());	// the two inner ones - lhsInner could be >=< rhsInner
			size_t	rhsInner	=	Led_Min (oldSelectionEnd, GetSelectionEnd ());
			Led_Assert (lhsOuter <= rhsOuter);	// See!- left of ALL 4
			Led_Assert (lhsOuter <= lhsInner);
			Led_Assert (lhsOuter <= rhsInner);
			Led_Assert (lhsOuter <= rhsOuter);	// See!- right of ALL 4
			Led_Assert (lhsInner <= rhsOuter);
			Led_Assert (rhsInner <= rhsOuter);
			/*
			 * SPR#0973 - added in the FindPrev/FindNext calls to expand the region we update slightly. Reason is cuz
			 * with new hilight display code - expanding the selection slightly at the end can affect the other end by
			 *	up to one character (rarely - but best to over-invalidate than under-invalidate).
			 */
			Refresh (FindPreviousCharacter (lhsOuter), FindNextCharacter (lhsInner), useUpdateMode);
			Refresh (FindPreviousCharacter (rhsInner), FindNextCharacter (rhsOuter), useUpdateMode);
		}

		InvalidateCaretState ();
	}
	if (updateMode == eImmediateUpdate) {
		Update ();
	}
	RecomputeSelectionGoalColumn ();
}

void	TextInteractor::SetSelection (size_t start, size_t end, UpdateMode updateMode)
{
	TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
	TextImager*	tim	=	this;	// Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
	tim->SetSelection (start, end);
}

/*
@METHOD:		TextInteractor::ScrollToSelection
@DESCRIPTION:	<p>Scroll the current window, so that the selection is showing. Calls TextInteractor::ScrollSoShowing ().
	Since TextInteractor::ScrollSoShowing () requires two marker-positions to try to show, we use the instance variable
	fLeftSideOfSelectionInteresting to guess which side of the selection the user is probably most interested in.
	This routine also pays attention to @'TextImager::GetCaretShownAfterPos' so that it shows the right part of
	the screen in ambiguous cases.</p>
		<p>See TextImager::ScrollSoShowing for more details. But basicly this tries to show the entire selection, if
	possible. And uses the flag to pick which side of the selection to use if only part of it can be made visible in the
	current window at once.</p>
*/
void	TextInteractor::ScrollToSelection (UpdateMode updateMode, bool forceShowSelectionEndpoint)
{
	size_t	selStart	=	GetSelectionStart ();
	size_t	selEnd		=	GetSelectionEnd ();


	/*
	 * Based on SPR#0940. If some of the selection is showing - its not totally clear the user would want us
	 * to scroll the entire screen to show another part of the selection. Maybe yes - maybe no. But clearly - if the
	 * ENTIRE SCREEN is selected (as in this SPR - then we don't want to scroll to show a different part of the selection.
	 * I THINK so anyhow... LGP 2001-05-24
	 *
	 *	PLUS - fix SPR#1051 - added forceShowSelectionEndpoint flag, so when using cursors, we can force a scroll.
	 */
	if (not forceShowSelectionEndpoint and selStart <= GetMarkerPositionOfStartOfWindow () and selEnd >= GetMarkerPositionOfEndOfWindow ()) {
		return;
	}

	/*
	 *	Why is this so complicated? And is it right?
	 *
	 *	There are a couple of issues here. One is that the semantics of @'TextImager::ScrollSoShowing'
	 *	are very precise, but perhaps slightly unintuitive. We try to show the CHARACTERS (not marker positions)
	 *	given by the marker-positions specified.
	 *
	 *	But when you are cursoring around, what you want to see are the CHARACTERS - not the MARKER POSITIONS.
	 *
	 *	In these ambiguous cases (like at a wrap point), we want to make sure we get the interpretation of which
	 *	character to show correct. And when the user must choose (cannot show whole selection) we want to make sure
	 *	we AT LEAST show the IMPORTANT side of the selection.
	 *
	 *	The 'lastCharShown	=	FindPreviousCharacter (selEnd)' bit is cuz we want to show the character BEFORE that
	 *	marker position. And for most APIs the marker pos names the character after that marker position.
	 *
	 *		--	LGP990212
	 */
	size_t	firstCharShown	=	selStart;
	size_t	lastCharShown	=	FindPreviousCharacter (selEnd);
	if (fLeftSideOfSelectionInteresting) {
		if (firstCharShown != lastCharShown and GetStartOfRowContainingPosition (firstCharShown) != GetStartOfRowContainingPosition (lastCharShown)) {
			lastCharShown = firstCharShown;
		}
		ScrollSoShowing (firstCharShown, lastCharShown, updateMode);
	}
	else {
		if (GetCaretShownAfterPos ()) {
			lastCharShown = FindNextCharacter (lastCharShown);
		}
		if (firstCharShown != lastCharShown and GetStartOfRowContainingPosition (firstCharShown) != GetStartOfRowContainingPosition (lastCharShown)) {
			firstCharShown = lastCharShown;
		}
		ScrollSoShowing (lastCharShown, firstCharShown, updateMode);
	}
}

void	TextInteractor::HookLosingTextStore ()
{
	HookLosingTextStore_ ();
	TextImager::HookLosingTextStore ();
}

void	TextInteractor::HookLosingTextStore_ ()
{
	AbortReplace (fTmpPreReplaceInfo);
	fExternalizer = NULL;
	fInternalizer = NULL;
}

void	TextInteractor::HookGainedNewTextStore ()
{
	HookGainedNewTextStore_ ();
	TextImager::HookGainedNewTextStore ();
}

void	TextInteractor::HookGainedNewTextStore_ ()
{
	if (fExternalizer.IsNull ()) {
		SetExternalizer (MakeDefaultExternalizer ());
	}
	if (fInternalizer.IsNull ()) {
		SetInternalizer (MakeDefaultInternalizer ());
	}
}

/*
@METHOD:		TextInteractor::ProcessSimpleClick
@ACCESS:		protected
@DESCRIPTION:	<p>Bad name. Basicly does a small portion of the click handling code. Vectored out
	not only for code sharing, but to provide a hook we can use in the StyledTextInteractor code
	to handle embeddings (without massive cut/n/paste duplication of code).</p>
		<p>What this does is to convert the click (or double or tripple click into an appropriate
	selection. The anchor only has meaning for double clicks with exteneded selection on.
		<p>This routine uses the existing selection, and just adjusts that selection.</p>
		<p>This routine returns true if all went well, and you should continue with interpretting
	click. It returns false if you should assume something clicked on swallowed up the click.</p>
		<p>This is not a GREAT line to have drawn for the API boundary. But its a start.</p>
		<p>LGP 960303</p>
*/
bool	TextInteractor::ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor)
{
	Led_RequireNotNil (dragAnchor);
	switch (clickCount) {
		case	1: {
			size_t	newPos			=	GetCharAtClickLocation (clickedAt);
			size_t	newSelStart		=	newPos;
			size_t	newSelEnd		=	newPos;
			{
				WhileTrackingConstrainSelection (&newSelStart, &newSelEnd);
				newPos	=	newSelStart;
			}
			if (extendSelection) {
				newSelStart = Led_Min (newSelStart, GetSelectionStart ());
				newSelEnd = Led_Max (newSelEnd, GetSelectionEnd ());
			}

			/*
			 *	Set the drag anchor:
			 *
			 *	Note - we use @'TextInteractor::GetCharAtClickLocation' instead of @'TextImager::GetCharAtWindowLocation'
			 *	so we get the right intended click location between two characters (for an anchor point for drag selection).
			 *
			 *	Code that is looking to see if the user clicked on an embedding itself will directly call
			 *	@'TextImager::GetCharAtWindowLocation'.
			 */
			if (extendSelection) {
				// if we're extending the selection, the anchor should be the OTHER end of the selection (SPR#1364)
				if (newPos == newSelStart) {
					*dragAnchor = newSelEnd;
				}
				else {
					*dragAnchor = newSelStart;
				}
			}
			else {
				*dragAnchor = newPos;
			}

			// Set flag for how to display caret based on where we clicked
			if (not extendSelection) {
				SetCaretShownAfterPos (GetCharWindowLocation (newPos).top <= clickedAt.v);
			}

			WhileTrackingConstrainSelection (&newSelStart, &newSelEnd);
			SetSelection (newSelStart, newSelEnd);
		}
		break;

		default: {
			// others are ignored
		}
		break;
	}
	#if		0
		LedDebugTrace ("TextInteractor::ProcessSimpleClick (tickCount=%f, newMousePos=(%d,%d), clickCount=%d, extendSel=%d, newSelStart=%d, newSelEnd=%d)\n",
						Led_GetTickCount (), clickedAt.v, clickedAt.h, clickCount, extendSelection, GetSelectionStart (), GetSelectionEnd ()
					);
	#endif
	return true;
}

/*
@METHOD:		TextInteractor::UpdateClickCount
@DESCRIPTION:	<p>Helper to implemented best feeling UI for double click detection.</p>
*/
void	TextInteractor::UpdateClickCount (float clickAtTime, const Led_Point& clickAtLocation)
{
	if (ClickTimesAreCloseForDoubleClick (clickAtTime) and PointsAreCloseForDoubleClick (clickAtLocation)) {
		IncrementCurClickCount (clickAtTime);
	}
	else {
		SetCurClickCount (1, clickAtTime);
	}
	fLastMouseDownAt = clickAtLocation;
}

/*
@METHOD:		TextInteractor::ClickTimesAreCloseForDoubleClick
@DESCRIPTION:	<p>Helper to implemented best feeling UI for double click detection. See also @'TextInteractor::UpdateClickCount' ().</p>
*/
bool	TextInteractor::ClickTimesAreCloseForDoubleClick (float thisClick)
{
	return (fLastClickedAt + Led_GetDoubleClickTime () >=  thisClick);
}

/*
@METHOD:		TextInteractor::PointsAreCloseForDoubleClick
@DESCRIPTION:	<p>Helper to implemented best feeling UI for double click detection. See also @'TextInteractor::UpdateClickCount' ().</p>
*/
bool	TextInteractor::PointsAreCloseForDoubleClick (const Led_Point& p)
{
	const Led_Coordinate		kMultiClickDistance = 4;
	Led_Coordinate	hDelta = p.h - fLastMouseDownAt.h;
	if (hDelta < 0) {
		hDelta = -hDelta;
	}
	Led_Coordinate	vDelta = p.v - fLastMouseDownAt.v;
	if (vDelta < 0) {
		vDelta = -vDelta;
	}
	return ((hDelta <= kMultiClickDistance) and (vDelta <= kMultiClickDistance));
}

/*
@METHOD:		TextInteractor::WhileSimpleMouseTracking
@DESCRIPTION:	<p>Helper to share code among implementations. Call this while mouse is down to handle autoscrolling,
	and selection updating.</p>
*/
void	TextInteractor::WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor)
{
	#if		qDynamiclyChooseAutoScrollIncrement
		float	now	=	Led_GetTickCount ();
		static	float	sLastTimeThrough	=	0.0f;
		const	float	kClickThreshold		=	Led_GetDoubleClickTime ()/3;
		bool	firstClick	=	(now - sLastTimeThrough > kClickThreshold);

		int		increment	=	firstClick? 1: 2;
	#else
		const	int		increment	=	1;
	#endif

	size_t	rhsPos	=	GetCharAtClickLocation (newMousePos);
	{
		size_t	ignored	=	rhsPos;
		WhileTrackingConstrainSelection (&rhsPos, &ignored);
	}

	/*
	 *	Handle vertical autoscrolling, if necessary.
	 */
	if (rhsPos < GetMarkerPositionOfStartOfWindow ()) {
		ScrollByIfRoom (-1, eImmediateUpdate);
		rhsPos = GetMarkerPositionOfStartOfWindow ();
	}
	else if (rhsPos > GetMarkerPositionOfEndOfWindow ()) {
		ScrollByIfRoom (1, eImmediateUpdate);
		rhsPos = FindNextCharacter (GetMarkerPositionOfEndOfWindow ());		// So we select past to end of window
	}

	/*
	 *	And horizontal.
	 */
	const	int	kHScrollIncrementFactor	=	4;
	if (newMousePos.h < GetWindowRect ().left) {
		if (GetHScrollPos () > 0) {
			SetHScrollPos (Led_Max (0, int (GetHScrollPos ()) - increment*kHScrollIncrementFactor));
		}
	}
	else if (newMousePos.h > GetWindowRect ().right) {
		SetHScrollPos (Led_Min (static_cast<size_t> (GetHScrollPos () + increment*kHScrollIncrementFactor), ComputeMaxHScrollPos ()));
	}

	size_t	newSelStart	=	Led_Min (rhsPos, dragAnchor);
	size_t	newSelEnd	=	Led_Max (rhsPos, dragAnchor);
	WhileTrackingConstrainSelection (&newSelStart, &newSelEnd);
	SetSelection (newSelStart, newSelEnd, eImmediateUpdate);

	#if		qDynamiclyChooseAutoScrollIncrement
		sLastTimeThrough = now;
	#endif
	#if		0
		LedDebugTrace ("TextInteractor::WhileSimpleMouseTracking (tickCount=%f, newMousePos=(%d,%d), dragAnchor=%d, newSelStart=%d, newSelEnd=%d)\n",
						Led_GetTickCount (), newMousePos.v, newMousePos.h, dragAnchor, newSelStart, newSelEnd
					);
	#endif
}

/*
@METHOD:		TextInteractor::WhileTrackingConstrainSelection
@DESCRIPTION:	<p>Override this to provide unusual selection behavior during
		tracking. For example, if you want to only allow selection of whole lines when
		the caps-lock key is down (OK, silly, but its an example), you could check for that
		state in this routine, and assure selStart and selEnd are adjusted to meet those
		criteria.</p>
			<p>By default this calls calls @'TextInteractor::WhileTrackingConstrainSelection_ForWholeWords',
		for double clicks and @'TextInteractor::WhileTrackingConstrainSelection_ForWholeRows' for 3 or more clicks.</p>
*/
void	TextInteractor::WhileTrackingConstrainSelection (size_t* selStart, size_t* selEnd)
{
	Led_RequireNotNil (selStart);
	Led_RequireNotNil (selEnd);
	Led_Require (GetCurClickCount () > 0);
	switch (GetCurClickCount ()) {
		case	1:	{
			// No constraints on a single click
		}
		break;

		case	2: {
			WhileTrackingConstrainSelection_ForWholeWords (selStart, selEnd);
		}
		break;

		default: {
			// any more than 3 and we constrain to whole rows
			WhileTrackingConstrainSelection_ForWholeRows (selStart, selEnd);
		}
		break;
	}
}

/*
@METHOD:		TextInteractor::WhileTrackingConstrainSelection_ForWholeWords
@DESCRIPTION:	<p>See @'TextInteractor::WhileTrackingConstrainSelection'</p>
*/
void	TextInteractor::WhileTrackingConstrainSelection_ForWholeWords (size_t* selStart, size_t* selEnd)
{
	Led_RequireNotNil (selStart);
	Led_RequireNotNil (selEnd);

	size_t	wordStart	=	0;
	size_t	wordEnd		=	0;
	bool	wordReal	=	false;
	GetTextStore ().FindWordBreaks (*selStart, &wordStart, &wordEnd, &wordReal);
	*selStart = wordStart;

	GetTextStore ().FindWordBreaks (*selEnd, &wordStart, &wordEnd, &wordReal);
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
	#endif
	*selEnd = wordEnd;
}

/*
@METHOD:		TextInteractor::WhileTrackingConstrainSelection_ForWholeRows
@DESCRIPTION:	<p>See @'TextInteractor::WhileTrackingConstrainSelection'</p>
*/
void	TextInteractor::WhileTrackingConstrainSelection_ForWholeRows (size_t* selStart, size_t* selEnd)
{
	Led_RequireNotNil (selStart);
	Led_RequireNotNil (selEnd);
	Led_Require (*selStart <= *selEnd);

	size_t	origSelStart	=	*selStart;
	size_t	origSelEnd		=	*selEnd;

	*selStart = GetStartOfRowContainingPosition (origSelStart);
	*selEnd = GetStartOfNextRowFromRowContainingPosition (*selEnd);
	// if at end of buffer, go to end of buffer. Else, select to start of next row
	if (*selEnd <= origSelEnd) {
		*selEnd = GetEndOfRowContainingPosition (origSelEnd);
	}
}

/*
@METHOD:		TextInteractor::GetCharAtClickLocation
@DESCRIPTION:	<p>Just like @'TextImager::GetCharAtWindowLocation'(), but doesn't try to
	find the character which encloses the particular point. Rather - we try to
	find the character the user was probably trying
	to click at to position the cursor. This is typically either the same, or the character
	just just following. If we are clicking towards the end of the character - we probably 
	wanted to click just <em>before</em> the next character. Note this works for BIDI characters.</p>
*/
size_t	TextInteractor::GetCharAtClickLocation (const Led_Point& where) const
{
	size_t	clickedOnChar		=	GetCharAtWindowLocation (where);
	size_t	endOfClickedRow		=	GetEndOfRowContainingPosition (clickedOnChar);

	/*
	 *	SPR#1597 (was originally SPR#1232). A click past the end of the wrap point in a wrapped
	 *	line has a value EQUAL to the end of the row. However - its also EQUAL to the start of
	 *	the following row. The trouble is that GetEndOfRowContainingPosition (THAT_POSITION)
	 *	will return the end of the FOLLOWING row in that case.
	 *
	 *	Since in order for this to happen there must be 1 or more characters in the row (to be a wrap point),
	 *	we can look at the PRECEEDING character, and compute ITS End-of-Row. If thats the same as the
	 *	returned click-position - we must have clicked on the end-of-row, and so we correct
	 *	the value of 'endOfClickedRow'.
	 */
	if (GetEndOfRowContainingPosition (FindPreviousCharacter (clickedOnChar)) == clickedOnChar) {
		endOfClickedRow = clickedOnChar;
	}

	Led_Assert (GetStartOfRowContainingPosition (clickedOnChar) <= clickedOnChar);
	Led_Assert (clickedOnChar <= endOfClickedRow);

	if (clickedOnChar < endOfClickedRow) {		// Don't wrap cuz click past end - LGP 950424
		Led_Rect	charRect			=	GetCharWindowLocation (clickedOnChar);
		bool		clickedToLHSOfChar	=	(where.h <= charRect.left + Led_Coordinate (charRect.GetWidth ())/2 and charRect.GetWidth () != 0);
		if (GetTextDirection (clickedOnChar) == eLeftToRight) {
			if (not clickedToLHSOfChar) {
				clickedOnChar = FindNextCharacter (clickedOnChar);
			}
		}
		else {
			if (clickedToLHSOfChar) {
				clickedOnChar = FindNextCharacter (clickedOnChar);
			}
		}
	}
	return (clickedOnChar);
}

void	TextInteractor::Draw (const Led_Rect& /*subsetToDraw*/, bool /*printing*/)
{
	NoteWindowPartiallyUpdated ();
	// don't call cuz there is none defined - pure virtual...TextImager::Draw (subsetToDraw, printing);
}

void	TextInteractor::DrawBefore (const Led_Rect& /*subsetToDraw*/, bool /*printing*/)
{
	// LGP 2003-03-20 - not 100% sure this is necessary or a good idea - but done to keep backward
	// compat with old behavior. Once new idle code in place - test without this. See SPR#1366.
	if (not fScrollBarParamsValid) {
		UpdateScrollBars ();
	}
}

void	TextInteractor::DrawAfter (const Led_Rect& /*subsetToDraw*/, bool printing)
{
	if (GetUseSecondaryHilight () and not GetSelectionShown () and not printing) {
		Led_Region	r;
		GetSelectionWindowRegion (&r, GetSelectionStart (), GetSelectionEnd ());
		Tablet_Acquirer	tablet_ (this);
		Led_Tablet		tablet		=	tablet_;
		tablet->FrameRegion (r, Led_GetSelectedTextBackgroundColor ());
	}
}

void	TextInteractor::SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode)
{
	TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
	TextImager*	tim	=	this;	// Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
	tim->SetTopRowInWindow (newTopRow);
}

void	TextInteractor::ScrollByIfRoom (long downBy, UpdateMode updateMode)
{
	TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
	TextImager*	tim	=	this;	// Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
	tim->ScrollByIfRoom (downBy);
}

void	TextInteractor::ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos, UpdateMode updateMode)
{
	TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
	TextImager*	tim	=	this;	// Dynamicly bind to 2-arg version. Direct call would select overloaded version from this class!
	tim->ScrollSoShowing (markerPos, andTryToShowMarkerPos);
}

void	TextInteractor::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont, UpdateMode updateMode)
{
	TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
	TextImager*	tim	=	this;	// Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
	tim->SetDefaultFont (defaultFont);
	Refresh ();
}

void	TextInteractor::SetWindowRect (const Led_Rect& windowRect, UpdateMode updateMode)
{
	TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
	TextImager*	tim	=	this;	// Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
	tim->SetWindowRect (windowRect);
}

void	TextInteractor::SetHScrollPos (Led_Coordinate hScrollPos)
{
	PreScrollInfo	preScrollInfo;
	PreScrollHelper (eDefaultUpdate, &preScrollInfo);
	TextImager::SetHScrollPos (hScrollPos);
	PostScrollHelper (preScrollInfo);
}

void	TextInteractor::SetHScrollPos (Led_Coordinate hScrollPos, UpdateMode updateMode)
{
	TemporarilySetUpdateMode	updateModeSetter (*this, updateMode);
	TextImager*	tim	=	this;	// Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
	tim->SetHScrollPos (hScrollPos);
}

void	TextInteractor::PreScrollHelper (UpdateMode updateMode, PreScrollInfo* preScrollInfo)
{
	UpdateMode	realUpdateMode = RealUpdateMode (updateMode);
	preScrollInfo->fUpdateMode = realUpdateMode;
	if (realUpdateMode != eNoUpdate) {
		preScrollInfo->fOldWindowStart = GetMarkerPositionOfStartOfWindow ();
		preScrollInfo->fOldHScrollPos = GetHScrollPos ();
		preScrollInfo->fTryTodoScrollbits	=	GetUseBitmapScrollingOptimization () and bool (realUpdateMode == eImmediateUpdate);
		if (preScrollInfo->fTryTodoScrollbits) {
			preScrollInfo->fOldLastRowStart	=	GetMarkerPositionOfStartOfLastRowOfWindow ();
			try {
				Update (false);	// so the stuff we scroll-bits is up to date...
							// Wouldn't want to scroll stale bits :-)
			}
			catch (...) {
				preScrollInfo->fTryTodoScrollbits = false;
			}
		}
	}
}

void	TextInteractor::PostScrollHelper (PreScrollInfo preScrollInfo)
{
	/*
	 *	Only if we scrolled do we need to refresh screen.
	 */
	size_t	newStartOfWindow	=	GetMarkerPositionOfStartOfWindow ();
	preScrollInfo.fUpdateMode = RealUpdateMode (preScrollInfo.fUpdateMode);
	if (preScrollInfo.fUpdateMode != eNoUpdate and
			((preScrollInfo.fOldWindowStart != newStartOfWindow) or
			 (preScrollInfo.fOldHScrollPos != GetHScrollPos ())
			 )
		) {

		// Don't try this except if we only got vertical scrolling - at least for now...
		if (preScrollInfo.fTryTodoScrollbits and preScrollInfo.fOldHScrollPos == GetHScrollPos ()) {
			Led_Rect	windowRect		=	GetWindowRect ();

			Tablet_Acquirer	tablet_ (this);
			Led_Tablet		tablet		=	tablet_;
			if (preScrollInfo.fOldWindowStart > newStartOfWindow) {
				/*                                    *
				 *	Move text (bits) DOWN screen (UP /|\ ARROW).
				 *									  |
				 *									  |
				 */
				Led_Coordinate	newPos				=	GetCharWindowLocation (GetStartOfRowContainingPosition (preScrollInfo.fOldWindowStart)).top;
				if (newPos > 0) {
					try {
						tablet->ScrollBitsAndInvalRevealed (windowRect, newPos-windowRect.top);
					}
					catch (...) {
						// Ignore any errors - just don't do scrollbits then...
						preScrollInfo.fTryTodoScrollbits = false;
					}
				}
				else {
					preScrollInfo.fTryTodoScrollbits = false;	// can happen, for example, if only one row fits in window...
				}
				if (preScrollInfo.fTryTodoScrollbits) {
					/*
					 *	Very subtle speed hack. It turns out, in this case, we may need to update a sliver on the top
					 *	of the window (exposed by scrollbits), and the bottom of the window (cuz we don't show partial
					 *	rows). So the BOUNDING RECTANLGE of the update region is the whole window. Why is this
					 *	a problem? In our update code, we use the bounding rectangle of the update region for
					 *	logical clipping. This means we end up drawing (though clipped out) every row of text.
					 *	This can make the scroll operation needlessly slow. By simply doing the update of the
					 *	top sliver first, we assure we always have a nice rectangular update region, so our
					 *	later optimizations work better. -- LGP 961030
					 */
					Update();


					// Now we may not want to allow the partial line to be displayed. Leave that choice to the
					// logic in the imager, and repaint the area past the end of the last row
					Led_Coordinate	lastRowBottom	=	GetCharWindowLocation (GetMarkerPositionOfStartOfLastRowOfWindow ()).bottom;
					Led_Rect		eraser	=	windowRect;
					eraser.top = lastRowBottom;
					RefreshWindowRect (eraser, preScrollInfo.fUpdateMode);
				}
			}
			else {
				/*
				 *	Move text (bits) UP screen (DOWN | ARROW).
				 *									 |
				 *									\|/
				 *		                             *
				 */
				Led_Coordinate	newPos	=	GetCharLocationRowRelativeByPosition (newStartOfWindow, preScrollInfo.fOldWindowStart, 5).top;

				if (newPos > 0) {
					try {
						tablet->ScrollBitsAndInvalRevealed (windowRect, -newPos);
					}
					catch (...) {
						// Ignore any errors - just don't do scrollbits then...
						preScrollInfo.fTryTodoScrollbits = false;
					}
				}
				else {
					preScrollInfo.fTryTodoScrollbits = false;	// can happen, for example, if only one row fits in window...
				}

				if (preScrollInfo.fTryTodoScrollbits) {
					// now refresh the exposed portion at the bottom. Note that much of it may have been exposed
					// already by the InvalRgn/ScrollWindow calls. But we may have to invalidate even more cuz we don't
					// show entire bottom lines.
					Led_Coordinate	lastRowBottom	=	GetCharWindowLocation (preScrollInfo.fOldLastRowStart).bottom;
					Led_Rect		eraser	=	windowRect;
					eraser.top = lastRowBottom;
					RefreshWindowRect (eraser, eDelayedUpdate);
				}

				if (preScrollInfo.fUpdateMode == eImmediateUpdate) {
					Update ();
				}
			}

			// ScrollBitsing succeeded, so we can return now, and avoid the below REFRESH.
			// Otherwise, fall through, and handle things the old-fashioned way...
			if (preScrollInfo.fTryTodoScrollbits) {
				return;
			}
		}

		Refresh (preScrollInfo.fUpdateMode);
	}
}

void	TextInteractor::Replace (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode)
{
	if (from != to or withWhatCharCount != 0) {
		Led_Assert (fDoingUpdateModeReplaceOn == NULL);
		fDoingUpdateModeReplaceOn = this;
		try {
			PreReplaceInfo preReplaceInfo;
			PreReplace (from, to, withWhatCharCount, updateMode, &preReplaceInfo);
			GetTextStore ().Replace (from, to, withWhat, withWhatCharCount);
			PostReplace (preReplaceInfo);
			Led_Assert (fDoingUpdateModeReplaceOn == this);
		}
		catch (...) {
			Led_Assert (fDoingUpdateModeReplaceOn == this);
			fDoingUpdateModeReplaceOn = NULL;
			throw;
		}
		fDoingUpdateModeReplaceOn = NULL;
	}
}

void	TextInteractor::AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo)
{
	TextImager::AboutToUpdateText (updateInfo);
	if (fDoingUpdateModeReplaceOn != this) {
		//	Sometimes a textstore sends notifications about updates beyond the end of the text (e.g. when setting
		//	styles which apply to newly typed characters). We only pay attention up to the end of the text
		//	(since that is all we display)
		PreReplace (updateInfo.fReplaceFrom, min (updateInfo.fReplaceTo, GetEnd ()), updateInfo.fTextLength, eDefaultUpdate, &fTmpPreReplaceInfo);
	}
}

void	TextInteractor::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	TextImager::DidUpdateText (updateInfo);
	if (fDoingUpdateModeReplaceOn != this) {
		try {
			PostReplace (fTmpPreReplaceInfo);
		}
		catch (...) {
			Refresh ();	// shouldn't happen? But if it does - this is probably the best we can do to handle it...
		}
	}
}

void	TextInteractor::PreReplace (size_t from, size_t to, size_t withWhatCharCount, UpdateMode updateMode, PreReplaceInfo* preReplaceInfo)
{
	Led_RequireNotNil (preReplaceInfo);

	if (preReplaceInfo->fTextInteractor != NULL) {// used as flag to indicate cleaned up state (marker removed)
		AbortReplace (*preReplaceInfo);
	}

	updateMode = RealUpdateMode (updateMode);

	preReplaceInfo->fFrom = from;
	preReplaceInfo->fTo = to;
	preReplaceInfo->fWithWhatCharCount = withWhatCharCount;
	preReplaceInfo->fUpdateMode = updateMode;

	if (updateMode == eNoUpdate) {
		return;
	}

	if (updateMode == eDelayedUpdate and IsWholeWindowInvalid ()) {
		preReplaceInfo->fUpdateMode = eNoUpdate;
		return;
	}


	try {
		/*
		 *	Every once in a while, somebody will try todo an edit operation outside of
		 *	the visible window. Though this is rare, when it DOES happen, there are frequently
		 *	several of them (as in reading in a file). So we attempt to specially tweek this
		 *	case. Do a preliminary and then secondary test so we don't slow needlessly waste time
		 *	for the more common case.
		 *
		 *	Note - we could also just as reasonably tweek the case where we are inserting BEFORE
		 *	the start of the window, but I've found no cases where that comes up, so I don't bother
		 *	for now.
		 *
		 *	LGP 960515.
		 */
		{
			size_t	endOfWindow	=	GetMarkerPositionOfEndOfWindow ();
			if (from > endOfWindow) {
				/*
				 *	Adding text JUST after the end of the window could - in principle - affect
				 *	the word-breaks of the end of window, so we must be a little more careful.
				 */
				size_t	endOfNextRow	= GetEndOfRowContainingPosition (FindNextCharacter (endOfWindow));
				if (from > endOfNextRow) {
					updateMode = eNoUpdate;
					preReplaceInfo->fUpdateMode = eNoUpdate;
					return;
				}
			}
		}


		Led_Assert (updateMode != eNoUpdate);
		/*
		 *	Grab a range that includes totally all the rows between the start selection and the end.
		 *
		 *	Save this region in a Marker (so it gets its bounds adjusted for the edit). Then compute
		 *	its pixelBounds (really we just need its HEIGHT). We will use this later to see
		 *	if the edit has caused a change in the pixel-height of the region, which would mean we
		 *	have to draw not just that region, but all the way to the end of the window.
		 *
		 *	Also we will use it later to decide if the starting or ending row (word-breaks) have
		 *	changed.
		 */
		size_t	startPositionOfRowWhereReplaceBegins	= GetStartOfRowContainingPosition (from);
		size_t	startPositionOfRowAfterReplaceEnds		= GetEndOfRowContainingPosition (to);
		if (startPositionOfRowAfterReplaceEnds < GetTextStore ().GetEnd ()) {
			startPositionOfRowAfterReplaceEnds = GetStartOfRowContainingPosition (FindNextCharacter (startPositionOfRowAfterReplaceEnds));
			Led_Assert (GetEndOfRowContainingPosition (to) <= startPositionOfRowAfterReplaceEnds);
		}
		Led_Assert (startPositionOfRowWhereReplaceBegins <= startPositionOfRowAfterReplaceEnds);

		preReplaceInfo->fBoundingUpdateHeight	=	GetTextWindowBoundingRect (startPositionOfRowWhereReplaceBegins, startPositionOfRowAfterReplaceEnds).GetHeight ();

		/*
		 *	In case the above changes, we may be able to get away with only updating the stable
		 *	typing region, assuming its height hasn't changed.
		 */
		{
			size_t	expandedFromMarkerPos	=	0;
			size_t	expandedToMarkerPos		=	0;
			GetStableTypingRegionContaingMarkerRange (from, to, &expandedFromMarkerPos, &expandedToMarkerPos);
			if (expandedFromMarkerPos == startPositionOfRowWhereReplaceBegins and expandedToMarkerPos == startPositionOfRowAfterReplaceEnds) {
				// Speed tweek. Avoid expensive call to GetTextWindowBoundingRect () - and use old value...
				preReplaceInfo->fStableTypingRegionHeight	=	preReplaceInfo->fBoundingUpdateHeight;
				Led_Assert (preReplaceInfo->fStableTypingRegionHeight == GetTextWindowBoundingRect (expandedFromMarkerPos, expandedToMarkerPos).GetHeight ());
			}
			else {
				preReplaceInfo->fStableTypingRegionHeight	=	GetTextWindowBoundingRect (expandedFromMarkerPos, expandedToMarkerPos).GetHeight ();
			}
		}

		/*
		 *	The marker is one past the end of the final row so any typing just after the end of the row
		 *	gets included.
		 */
		GetTextStore ().AddMarker (&preReplaceInfo->fBoundingUpdateMarker, startPositionOfRowWhereReplaceBegins, (startPositionOfRowAfterReplaceEnds-startPositionOfRowWhereReplaceBegins)+1, this);
		preReplaceInfo->fTextInteractor = this;	// assign after add, cuz this var serves as flag to indicate addMarker call done...
	}
	catch (NotFullyInitialized&) {
		// Fine - we can ignore that..
		preReplaceInfo->fUpdateMode = eNoUpdate;
		return;
	}
	catch (...) {
		throw;
	}
}

void	TextInteractor::PostReplace (PreReplaceInfo& preReplaceInfo)
{
	UpdateMode	updateMode	=	preReplaceInfo.fUpdateMode;
	if (updateMode != eNoUpdate) {
		size_t from					=	preReplaceInfo.fFrom;
		size_t withWhatCharCount	=	preReplaceInfo.fWithWhatCharCount;

		size_t	newTo				=	from + withWhatCharCount;

		// Subtract one from end cuz we added one earlier so chars appended would grow marker...
		size_t	startPositionOfRowWhereReplaceBegins	= preReplaceInfo.fBoundingUpdateMarker.GetStart ();
		size_t	startPositionOfRowAfterReplaceEnds		= preReplaceInfo.fBoundingUpdateMarker.GetEnd ()-1;
		Led_Assert (startPositionOfRowWhereReplaceBegins <= startPositionOfRowAfterReplaceEnds);

		size_t	stableTypingRegionStart		=	0;
		size_t	stableTypingRegionEnd		=	0;
		GetStableTypingRegionContaingMarkerRange (from, newTo, &stableTypingRegionStart, &stableTypingRegionEnd);

		size_t	expandedFromMarkerPos	=	0;
		size_t	expandedToMarkerPos		=	0;
		ExpandedFromAndToInPostReplace (from, newTo,
				stableTypingRegionStart, stableTypingRegionEnd,
				startPositionOfRowWhereReplaceBegins, startPositionOfRowAfterReplaceEnds,
				&expandedFromMarkerPos, &expandedToMarkerPos
			);

		Led_Rect	windowRect	=	GetWindowRect ();
		Led_Rect	expandedFromToMarkerRect	=	GetTextWindowBoundingRect (expandedFromMarkerPos, expandedToMarkerPos);
		Led_Rect	updateRect	=	expandedFromToMarkerRect;

		// we must ALWAYS draw to the end of the row (including space after last character)
		updateRect.right = windowRect.right;

		// Now if we've changed the height of the bounding rows region, we need to repaint to end.
		{
			Led_Rect	revisedRect	=	expandedFromToMarkerRect;		// Speed tweek. Avoid expensive call to GetTextWindowBoundingRect () - and use old value...
			if (expandedFromMarkerPos != startPositionOfRowWhereReplaceBegins or expandedToMarkerPos != startPositionOfRowAfterReplaceEnds) {
				revisedRect	=	GetTextWindowBoundingRect (startPositionOfRowWhereReplaceBegins, startPositionOfRowAfterReplaceEnds);
			}
			Led_Assert (revisedRect == GetTextWindowBoundingRect (startPositionOfRowWhereReplaceBegins, startPositionOfRowAfterReplaceEnds));

			if (preReplaceInfo.fBoundingUpdateHeight != revisedRect.GetHeight ()) {
				updateRect	=	Led_Rect (updateRect.top, windowRect.left, windowRect.bottom-updateRect.top, windowRect.GetWidth ());
			}
		}

		// Now if we've changed the height of the stable region, we need to repaint to end of screen.
		{
			Led_Rect	revisedRect	=	expandedFromToMarkerRect;		// Speed tweek. Avoid expensive call to GetTextWindowBoundingRect () - and use old value...
			if (expandedFromMarkerPos != stableTypingRegionStart or expandedToMarkerPos != stableTypingRegionEnd) {
				revisedRect	=	GetTextWindowBoundingRect (stableTypingRegionStart, stableTypingRegionEnd);
			}
			Led_Assert (revisedRect == GetTextWindowBoundingRect (stableTypingRegionStart, stableTypingRegionEnd));
			if (preReplaceInfo.fStableTypingRegionHeight != revisedRect.GetHeight ()) {
				updateRect	=	Led_Rect (updateRect.top, windowRect.left, windowRect.bottom-updateRect.top, windowRect.GetWidth ());
			}
		}

		RefreshWindowRect (updateRect, updateMode);
	}
}

void	TextInteractor::AbortReplace (PreReplaceInfo& preReplaceInfo)
{
	if (preReplaceInfo.fTextInteractor != NULL) {
		// remove marker, and set to NULL to indicate cleaned up.
		preReplaceInfo.fTextInteractor->GetTextStore ().RemoveMarker (&preReplaceInfo.fBoundingUpdateMarker);
		preReplaceInfo.fTextInteractor = NULL;
	}
}

void	TextInteractor::ExpandedFromAndToInPostReplace (size_t from, size_t newTo,
										size_t stableTypingRegionStart, size_t stableTypingRegionEnd,
										size_t startPositionOfRowWhereReplaceBegins, size_t	startPositionOfRowAfterReplaceEnds,
										size_t* expandedFrom, size_t* expandedTo
									)
{
	Led_RequireNotNil (expandedFrom);
	Led_RequireNotNil (expandedTo);

	// Edits in a row practically never change the word-break from the previous line, but they CAN - in
	// principle - (eg. in the second row there is a very long word, and you insert a space near the
	// beginning, the little word-let created might fit on the previous row, in which case you would
	// need to redisplay BOTH rows. But we only need to update previous rows or earlier text in this
	// row if the row-break point changes - thats what our marker-test here measures.
	size_t	expandedFromMarkerPos	=	0;
	if (GetStartOfRowContainingPosition (from) == startPositionOfRowWhereReplaceBegins) {
		expandedFromMarkerPos = from;

		// On windows - at least temporarily - editing one char can change the WIDTH/MeasureText
		// of the preceeding character, since we don't need to add on the overhang anymore.
		// So be sure to draw that previous character in that subtle case
		// - See SPR# 0340 - LGP 960516
		if (expandedFromMarkerPos > startPositionOfRowWhereReplaceBegins) {
			expandedFromMarkerPos = FindPreviousCharacter (expandedFromMarkerPos);
		}
	}
	else {
		// practically never happens....
		expandedFromMarkerPos = stableTypingRegionStart;
	}

	// Edits much more commonly (though still only a few percent of edits) can change the ending
	// row word-break line. A change here means we must redraw to the end of the stable-region
	// (typically line - aka paragraph).
	size_t	expandedToMarkerPos = GetEndOfRowContainingPosition (newTo);
	{
		size_t	nowStartOfNextRow		= expandedToMarkerPos;
		if (nowStartOfNextRow < GetTextStore ().GetEnd ()) {
			nowStartOfNextRow = GetStartOfRowContainingPosition (FindNextCharacter (nowStartOfNextRow));
			Led_Assert (expandedToMarkerPos <= nowStartOfNextRow);
		}

		if (nowStartOfNextRow != startPositionOfRowAfterReplaceEnds) {
			// then we changed word-breaks! Must go all the way to the end of the stable region.
			expandedToMarkerPos = stableTypingRegionEnd;
		}
	}

	*expandedFrom = expandedFromMarkerPos;
	*expandedTo = expandedToMarkerPos;
}

void	TextInteractor::InteractiveReplace (const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode)
{
	BreakInGroupedCommandsIfDifferentCommand (GetCommandNames ().fTypingCommandName);
	InteractiveModeUpdater	iuMode (*this);
	UndoableContextHelper	undoContext (*this, GetCommandNames ().fTypingCommandName, withWhatCharCount == 0);
		{
			InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), withWhat, withWhatCharCount, true, true, updateMode);
			bool	anyChanges	=	InteractiveReplaceEarlyPostReplaceHook (withWhatCharCount);
			if (withWhatCharCount == 1 and not anyChanges) {
				// need other tests as well???? Like same start location ?? Maybe done inside command-handler stuff????
				undoContext.SetSimplePlainTextInsertOptimization (true);
			}
		}
	undoContext.CommandComplete ();
}

/*
@METHOD:		TextInteractor::InteractiveReplace_
@DESCRIPTION:	
*/
void	TextInteractor::InteractiveReplace_ (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount,
												bool updateCursorPosition, bool validateTextForCharsetConformance,
												UpdateMode updateMode
											)
{
	// Assert selection bounardaries valid Led_tChar boundaries
	#if		qMultiByteCharacters
		Assert_CharPosDoesNotSplitCharacter (from);
		Assert_CharPosDoesNotSplitCharacter (to);
	#endif

	if (validateTextForCharsetConformance) {
		// Then check the GIVEN text - no assert here, just bad_input if text bad...
		if (not ValidateTextForCharsetConformance (withWhat, withWhatCharCount)) {
			OnBadUserInput ();
			return;			// in case OnBadUserInput () doesn't throw
		}
	}

	TempMarker	newSel (GetTextStore (), to+1, to+1);	// NB: This marker pos MAY split a character - but that should cause
														// no problems.
														// We are only keeping this temporarily and we subtract one at the
														// end. Just want to make it past point where we do the insertion
														// so we get the right adjustment on



	Tablet_Acquirer	performanceHackTablet (this);		// sometimes acquiring/releaseing the tablet can be expensive,
														// and as a result of stuff later in this call, it happens
														// several times. By acquiring it here, we make the other calls
														// much cheaper (cuz its basicly free to acquire when already
														// acuired).




	SetCaretShownAfterPos (true);	// by default show pos after - this is what works best for NL's and is what we
									// want most of the time...
	Replace (from, to, withWhat, withWhatCharCount, updateMode);
	if (updateCursorPosition) {
		size_t	newSelection	=	newSel.GetStart ();
		if (newSelection > 0) {
			newSelection--;
		}

		SetSelection (newSelection, newSelection);
	}
}

/*
@METHOD:		TextInteractor::InteractiveReplaceEarlyPostReplaceHook
@DESCRIPTION:	<p>Hook function called AFTER the @'TextInteractor::InteractiveReplace_' in @'TextInteractor::InteractiveReplace', but
	<em>before</em> the @'TextInteractor::PreInteractiveUndoHelper' call. The need to get into this interval is if you need to update
	the text to <em>augment</em> the effect of the user typing, and what that effect to be reflected in the UNDO information.</p>
		<p>This happens, for example, with @'StandardStyledTextInteractor::InteractiveReplaceEarlyPostReplaceHook' where
	we want to set the font of the newly typed character according to the 'fEmptySelection' style. See spr#0604 for more details.</p>
		<p><em>OLD-CODE-NOTE</em>NB: This routine was changed in Led 3.1a8 to return a boolean result.</p>
		</p>The boolean return value must be true if any changes were made that could affect undo processing (i.e. that could affect
	what infomration must be saved for proper undo processing).</p>
*/
bool	TextInteractor::InteractiveReplaceEarlyPostReplaceHook (size_t /*withWhatCharCount*/)
{
	return false;
}

/*
@METHOD:		TextInteractor::PreInteractiveUndoHelper
@DESCRIPTION:	<p>This is called early on - before any change happens - to preserve the contents of a region about to be updated - so
	that the region can be restored upon an UNDO command. The argument selStart/selEnd are NOT the actual selection regions - but
	rather the region (which maybe slightly larger) that needs to be preserved (perhaps the arg names should change?).</p>
		<p>Note - this can differ from the selection region because of SmartCutAndPaste - where we expand the selection slightly for
	a command to do funky stuff with whitespace.</p>
		<p>Note that the handed in selStart/selEnd arguments can be modified by this routine (due to a call to
	@'TextInteractor::PreInteractiveUndoHelperHook').</p>
		<p>We then preserve the ACTUAL selection at the time this was called in the resulting 'beforeRep' object.</p>
		<p>NB: new in Led 3.1a6 - we require that fCommandHandler != NULL to call this.</p>
*/
void	TextInteractor::PreInteractiveUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd)
{
	Led_Require (regionStart <= regionEnd);
	Led_Require (selStart <= selEnd);
	Led_RequireNotNil (beforeRep);
	Led_Require ((*beforeRep) == NULL);
	Led_RequireNotNil (fCommandHandler);

	try {
		(*beforeRep) = InteractiveUndoHelperMakeTextRep (regionStart, regionEnd, selStart, selEnd);
	}
	catch (...) {
		// any exceptions in here are cuz we don't have enuf memory to make this
		// command undoable. No matter. Proceed anyhow, ignoring the exception...
		// And commit any existing commands to make it more likely real code
		// succeeds, and cuz we don't want any funny undo behavior where some
		// commands in sequence might get skipped.
		Led_AssertNotNil (fCommandHandler);
		fCommandHandler->Commit ();
	}
}

/*
@METHOD:		TextInteractor::PostInteractiveUndoHelper
@DESCRIPTION:	<p>This routine is called after a user action has taken place which is to be recorded for UNDOing.
			The 'startOfInsert' / 'endOfInsert' passed here refer to the region of text which must be preserved.
			The actual selection saved will be the currently selected text at the time this method is called.</p>
				<p>Note - the startOfInsert/endOfInsert can differ from the selection region because things like
			SmartCutAndPaste can expand the affected area of text to BEYOND what was actaully selected by the user.</p>
				<p>This method operatates by calling @'TextInteractor::PostInteractiveUndoPostHelper' with the
			beforeRep argument given this function and an afterRep computed herein.</p>
				<p>NB: As of Led 3.1a6 - we require that fCommandHandler != NULL to call this.</p>
*/
void	TextInteractor::PostInteractiveUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t startOfInsert, size_t endOfInsert, const Led_SDK_String& cmdName)
{
	Led_RequireNotNil (beforeRep);
	Led_RequireNotNil (*beforeRep);			// This shouldn't be called if there was a problem creating beforeRep (exception)
	Led_RequireNotNil (fCommandHandler);
	SavedTextRep*	afterRep	=	NULL;
	try {
		afterRep = InteractiveUndoHelperMakeTextRep (startOfInsert, endOfInsert, GetSelectionStart (), GetSelectionEnd ());
		PostInteractiveUndoPostHelper (beforeRep, &afterRep, startOfInsert, cmdName);
	}
	catch (...) {
		delete (*beforeRep);
		(*beforeRep) = NULL;
		delete afterRep;
		afterRep = NULL;
		throw;				// safe at this point to throw - but perhaps better to silently eat the throw?
	}
}

/*
@METHOD:		TextInteractor::PostInteractiveSimpleCharInsertUndoHelper
@DESCRIPTION:	<p>Utility function for optimized undo support - keeping smaller objects in the undo buffer, and trying re-use/tweek
			an existing one in the common case of multiple consecutive characters typed.</p>
*/
void	TextInteractor::PostInteractiveSimpleCharInsertUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t startOfInsert, size_t endOfInsert, const Led_SDK_String& cmdName)
{
	Led_RequireNotNil (beforeRep);
	Led_RequireNotNil (*beforeRep);			// This shouldn't be called if there was a problem creating beforeRep (exception)
	Led_RequireNotNil (fCommandHandler);
	if (endOfInsert - startOfInsert == 1) {
		Led_tChar	c;
		CopyOut (startOfInsert, 1, &c);
		if (fCommandHandler->PostUpdateSimpleTextInsert (startOfInsert, c)) {
			delete *beforeRep;
			*beforeRep = NULL;
			return;
		}
		// at least create a plain-text guy if we cannot update current one...
		SavedTextRep* afterRep = new InteractiveReplaceCommand::PlainTextRep (GetSelectionStart (), GetSelectionEnd (), &c, 1);
		PostInteractiveUndoPostHelper (beforeRep, &afterRep, startOfInsert, cmdName);
		Led_Assert (afterRep == NULL);	// cleared out by PostInteractiveUndoPostHelper ()
		return;
	}

	PostInteractiveUndoHelper (beforeRep, startOfInsert, endOfInsert, cmdName);
}

/*
@METHOD:		TextInteractor::PostInteractiveUndoPostHelper
@DESCRIPTION:	<p>This routine is called after a user action has taken place which is to be recorded for UNDOing.
			The routine simply posts an @'InteractiveReplaceCommand' (with the already saved before/after reps) to the current
			command handler.</p>
				<p>This method is typically called by @'TextInteractor::PostInteractiveUndoHelper'.</p>
				<p>NB: As of Led 3.1a6 - we require that fCommandHandler != NULL to call this.</p>
*/
void	TextInteractor::PostInteractiveUndoPostHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep,
													InteractiveReplaceCommand::SavedTextRep** afterRep,
													size_t startOfInsert, const Led_SDK_String& cmdName
												)
{
	Led_RequireNotNil (beforeRep);
	Led_RequireNotNil (afterRep);
	Led_RequireNotNil (fCommandHandler);
	try {
		if (*beforeRep != NULL and *afterRep != NULL) {
			// We declare temporaries here, and be careful to set things to NULL at each stage to prevent double
			// deletes in the event of a badly timed exception
			InteractiveReplaceCommand*	cmd	=	new InteractiveReplaceCommand (*beforeRep, *afterRep, startOfInsert, cmdName);
			*beforeRep = NULL;
			*afterRep = NULL;
			fCommandHandler->Post (cmd);
		}
	}
	catch (...) {
		delete *beforeRep;
		*beforeRep = NULL;
		delete *afterRep;
		*afterRep = NULL;
		throw;				// safe at this point to throw - but perhaps better to silently eat the throw?
	}
}

InteractiveReplaceCommand::SavedTextRep*	TextInteractor::InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd)
{
	if (regionStart == regionEnd) {
		// optimization, cuz these are smaller
		return new InteractiveReplaceCommand::PlainTextRep (selStart, selEnd, NULL, 0);
	}
	else {
		return new FlavorSavorTextRep (this, regionStart, regionEnd, selStart, selEnd);
	}
}

void	TextInteractor::OnUndoCommand ()
{
	InteractiveModeUpdater	iuMode (*this);
	if (GetCommandHandler () != NULL and GetCommandHandler ()->CanUndo ()) {
		GetCommandHandler ()->DoUndo (*this);
		ScrollToSelection ();
	}
	else {
		Led_BeepNotify ();
	}
}

void	TextInteractor::OnRedoCommand ()
{
	InteractiveModeUpdater	iuMode (*this);
	if (GetCommandHandler () != NULL and GetCommandHandler ()->CanRedo ()) {
		GetCommandHandler ()->DoRedo (*this);
		ScrollToSelection ();
	}
	else {
		Led_BeepNotify ();
	}
}

void	TextInteractor::Refresh (size_t from, size_t to, UpdateMode updateMode) const
{
	Led_Require (from <= to);
	updateMode = RealUpdateMode (updateMode);
	if ((updateMode != eNoUpdate) and (from != to)) {
		if (updateMode == eDelayedUpdate and IsWholeWindowInvalid ()) {
			return;
		}
		// we could be more precise - but no need. Just take the box bounding the two
		// endpoints.
		// I'd be more inclined to worry about optimizing this, but the most relevant plausible
		// usage of this routine - during typing - doesn't use it. So I believe we can get away with
		// being sloppy here - LGP 960516
		Led_Rect	refreshRect	=	GetTextWindowBoundingRect (from, to);
		RefreshWindowRect_ (refreshRect, updateMode);
	}
}

void	TextInteractor::Refresh (const Marker* range, UpdateMode updateMode) const
{
	Led_RequireNotNil (range);
	updateMode = RealUpdateMode (updateMode);
	if (updateMode != eNoUpdate) {
		if (updateMode == eDelayedUpdate and IsWholeWindowInvalid ()) {
			return;
		}
		Refresh (range->GetStart (), range->GetEnd (), updateMode);
	}
}

/*
@METHOD:		TextInteractor::DoSingleCharCursorEdit
@ACCESS:		protected
@DESCRIPTION:	<p>Helper routine for handling cursoring done by user.</p>
 */
void	TextInteractor::DoSingleCharCursorEdit (CursorMovementDirection direction, CursorMovementUnit movementUnit, CursorMovementAction action,
													UpdateMode updateMode, bool scrollToSelection)
{
	IdleManager::NonIdleContext	nonIdleContext;

	size_t	oldStartSel		=	GetSelectionStart ();
	size_t	oldEndSel		=	GetSelectionEnd ();
	Led_Assert (GetSelectionEnd () <= GetLength () + 1);

	size_t	newStartSel		=	oldStartSel;
	size_t	newEndSel		=	oldEndSel;

	UpdateMode	useUpdateMode	=	(updateMode==eImmediateUpdate)? eDelayedUpdate: updateMode;

	GoalColumnRecomputerControlContext skipRecompute (*this, action == eCursorMoving and movementUnit == eCursorByRow and (direction == eCursorBack or direction == eCursorForward));

	// In a couple of cases, we get burned by the ambiguity of location-specification at start and end
	// of row. This only matters when the users says navigate to start/end of row, and we happen to already
	// be there. In that special case, we will navigate to the start/end of the next row.
	//
	// This situation only occurs when we are at the end of a row which has been word-wrapped. This is because
	// in that special case, there is a flag - CaretShownAfterPos - which says if you show the caret before or
	// after the marker-pos of the selection.
	//
	// If we are in this situation, override the usual navigation logic.
	if (movementUnit == eCursorByRow and GetStartOfRowContainingPosition (newStartSel) == newStartSel) {
		if (GetCaretShownAfterPos ()) {
			if (direction == eCursorToEnd and GetEndOfRowContainingPosition (newStartSel) == newStartSel) {
				goto SkipNavigation;
			}
		}
		else {
			if (direction == eCursorToStart) {
				// fall through with usual 'to start of row' logic, but backup one characater within the row so
				// that code doesn't get fooled about which row we are on.
				newStartSel = FindPreviousCharacter (newStartSel);
			}
			if (direction == eCursorToEnd) {
				goto SkipNavigation;
			}
		}
	}

	//	Generally don't want to mess with the caret shown pos. But in these cases few cases,
	//	the user is indicating which he'd prefer.
	//	Perhaps there are more cases? Can we generalize/simplify?
	if (movementUnit == eCursorByChar) {
		SetCaretShownAfterPos (true);
	}
	if (movementUnit == eCursorByRow) {
		switch (direction) {
			case	eCursorToStart:	SetCaretShownAfterPos (true);	break;
			case	eCursorToEnd:	SetCaretShownAfterPos (false);	break;
		}
	}

	if (action != eCursorExtendingSelection or (oldStartSel == oldEndSel)) {
		fLeftSideOfSelectionInteresting = (direction == eCursorBack or direction == eCursorToStart);
	}
	if (fLeftSideOfSelectionInteresting) {
		newStartSel = ComputeRelativePosition (newStartSel, direction, movementUnit);
		if (action == eCursorMoving) {	// only case where we do this - destroy/extend we keep track of start
			newEndSel = newStartSel;
		}
	}
	else {
		newEndSel = ComputeRelativePosition (newEndSel, direction, movementUnit);
		if (action == eCursorMoving) {	// only case where we do this - destroy/extend we keep track of start
			newStartSel = newEndSel;
		}
	}

SkipNavigation:

	// The above can reverse start/end, so make sure that doesn't happen...
	if (newEndSel < newStartSel) {
		size_t	tmp	=	newStartSel;
		newStartSel = newEndSel;
		newEndSel = tmp;
	}
	Led_Assert (newStartSel <= newEndSel);

	/*
	 *	Now that we know the new and old selection region, we can perform the action
	 */
	switch (action) {
		case	eCursorDestroying: {
			if (oldStartSel == oldEndSel) {
				/*
				 *	In this case, then the computations above for the NEW selection
				 *	can be considered valid.
				 */
				Led_Assert (newEndSel >= newStartSel);
				size_t	howMany	=	newEndSel - newStartSel;
				if (howMany >= 1) {		// might be zero if we were backed up against the start of the buffer.
					bool	oldCutAndPaste	=	GetSmartCutAndPasteMode ();
					try {
						SetSmartCutAndPasteMode (false);	// See SPR#1044 - when user hits backspace and has empty selection, dont use smart cut and paste...
						{
							BreakInGroupedCommandsIfDifferentCommand (GetCommandNames ().fClearCommandName);
							InteractiveModeUpdater	iuMode (*this);
							UndoableContextHelper	undoContext (*this, GetCommandNames ().fClearCommandName, newStartSel, newEndSel, GetSelectionStart (), GetSelectionEnd (), true);
								{
									InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0, true, true, useUpdateMode);
								}
							undoContext.CommandComplete ();
						}
						// After the above deletion, we already end up the the selection adjustment being handled for us (and more correctly
						// than we can easily due to smart cut and paste). So just grab the current (already correct) selection, to prevent
						// our later call to SetSelection () from doing any harm.
						GetSelection (&newStartSel, &newEndSel);
						SetSmartCutAndPasteMode (oldCutAndPaste);
					}
					catch (...) {
						SetSmartCutAndPasteMode (oldCutAndPaste);
						throw;
					}
				}
			}
			else {
				/*
				 *	Otherwise, if there WAS some selection, and we get any kind of
				 *	delete key, we REALLY just want to delete the selection, and
				 *	ignore whatever computation was done above for where to put
				 *	the new selection - it goes right to the old startSel.
				 */
				Led_Assert (oldEndSel >= oldStartSel);
				Led_Assert (oldStartSel == GetSelectionStart ());
				Led_Assert (oldEndSel == GetSelectionEnd ());
				OnClearCommand ();
				// After the above deletion, we already end up the the selection adjustment being handled for us (and more correctly
				// than we can easily due to smart cut and paste). So just grab the current (already correct) selection, to prevent
				// our later call to SetSelection () from doing any harm.
				GetSelection (&newStartSel, &newEndSel);
			}
		}
		break;

		case	eCursorMoving: {
			// Nothing todo (actual setting of selection done at the end)
		}
		break;

		case	eCursorExtendingSelection: {
			// Nothing todo (actual setting of selection done at the end)
		}
		break;

		default:	Led_Assert (false);	// bad direction argument
	}

	/*
	 *	Buy this point, we've computed where we should be, and performed
	 *	any actions on the text that needed to be taken (e.g. deleting).
	 *	Nothing should have yet been redisplayed. Now we update the selection,
	 *	perform any needed scrolling, and only then - display - if prescribed.
	 */
	SetSelection (newStartSel, newEndSel, useUpdateMode);

	if (scrollToSelection) {
		ScrollToSelection (useUpdateMode, true);
	}

	if (updateMode == eImmediateUpdate) {
		Update ();
	}
}

void	TextInteractor::OnCutCommand ()
{
	InteractiveModeUpdater	iuMode (*this);
	BreakInGroupedCommands ();
	if (GetSelectionStart () != GetSelectionEnd ()) {
		OnCopyCommand ();
		UndoableContextHelper	undoContext (*this, GetCommandNames ().fCutCommandName, true);
			{
				InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0);
			}
		undoContext.CommandComplete ();
	}
	BreakInGroupedCommands ();
}

void	TextInteractor::OnCopyCommand ()
{
	size_t	start	=	GetSelectionStart ();
	size_t	end		=	GetSelectionEnd ();
	Led_Assert (start <= end);
	if (start < end) {
		BreakInGroupedCommands ();

		if (OnCopyCommand_Before ()) {
			try {
				OnCopyCommand_CopyFlavors ();
			}
			catch (...) {
				OnCopyCommand_After ();
				throw;
			}
			OnCopyCommand_After ();
		}
	}
}

void	TextInteractor::OnPasteCommand ()
{
	InteractiveModeUpdater	iuMode (*this);
	BreakInGroupedCommands ();

	if (OnPasteCommand_Before ()) {
		try {
			UndoableContextHelper	undoContext (*this, GetCommandNames ().fPasteCommandName, false);
				{
					OnPasteCommand_PasteBestFlavor ();
				}
			undoContext.CommandComplete ();
		}
		catch (...) {
			OnPasteCommand_After ();
			throw;
		}
		OnPasteCommand_After ();
	}
	BreakInGroupedCommands ();
}

void	TextInteractor::OnClearCommand ()
{
	InteractiveModeUpdater	iuMode (*this);
	BreakInGroupedCommands ();
	UndoableContextHelper	undoContext (*this, GetCommandNames ().fClearCommandName, true);
		{
			InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0);
		}
	undoContext.CommandComplete ();
	BreakInGroupedCommands ();
}

/*
@METHOD:		TextInteractor::OnCopyCommand_Before
@ACCESS:		protected
@DESCRIPTION:	<p>Hook overriden by SDK-specific classes or templates which does special things in the before
			a clipboard <em>copy</em> operation can begin (like opening a clipboard object). Generally should not be
			called directly or overridden, except when implementing new SDK wrappers.</p>
				<p>Return false or throw if fail</p>
				<p>See also @'TextInteractor::OnCopyCommand_After',
				@'TextInteractor::OnPasteCommand_Before',
				and
				@'TextInteractor::OnPasteCommand_After'
				.</p>
 */
bool	TextInteractor::OnCopyCommand_Before ()
{
	return true;
}

/*
@METHOD:		TextInteractor::OnCopyCommand_After
@ACCESS:		protected
@DESCRIPTION:	<p>See also @'TextInteractor::OnCopyCommand_Before'.</p>
 */
void	TextInteractor::OnCopyCommand_After ()
{
}

/*
@METHOD:		TextInteractor::OnCopyCommand_CopyFlavors
@ACCESS:		protected
@DESCRIPTION:	<p></p>
 */
void	TextInteractor::OnCopyCommand_CopyFlavors ()
{
	WriterClipboardFlavorPackage	writer;
	ExternalizeFlavors (writer);
}

bool	TextInteractor::ShouldEnablePasteCommand () const
{
	return (Led_ClipboardObjectAcquire::FormatAvailable_TEXT ());
}

/*
@METHOD:		TextInteractor::OnPasteCommand_Before
@DESCRIPTION:	<p>Hook overriden by SDK-specific classes or templates which does special things in the before
			a clipboard <em>paste</em> operation can begin (like opening a clipboard object). Generally should not be
			called directly or overridden, except when implementing new SDK wrappers.</p>
				<p>Return false or throw if fail</p>
				<p>See also @'TextInteractor::OnPasteCommand_After',
				@'TextInteractor::OnCopyCommand_Before',
				and
				@'TextInteractor::OnCopyCommand_After'
				.</p>
 */
bool	TextInteractor::OnPasteCommand_Before ()
{
	return true;
}

/*
@METHOD:		TextInteractor::OnPasteCommand_After
@ACCESS:		protected
@DESCRIPTION:	<p>See also @'TextInteractor::OnPasteCommand_Before'.</p>
 */
void	TextInteractor::OnPasteCommand_After ()
{
}

/*
@METHOD:		TextInteractor::OnPasteCommand_PasteBestFlavor
@ACCESS:		protected
@DESCRIPTION:	<p></p>
 */
void	TextInteractor::OnPasteCommand_PasteBestFlavor ()
{
	#if		qWindows && 0
		// A little debugging hack for windows - sometimes helpful to turn this on
		// to peek in the debugger at what is on the clipboard - LGP 960430
		
		long	clipFormat	=	0;
		while ( (clipFormat = ::EnumClipboardFormats (clipFormat)) != 0) {
			TCHAR	buf[1024];
			int		nChars	=	::GetClipboardFormatName (clipFormat, buf, Led_NEltsOf (buf));
			int		breakHere	=	0;
		}
	#endif

	ReaderClipboardFlavorPackage	clipData;

	SmartCNPInfo	smartCNPInfo;
	bool			doSmartCNP	=	PasteLooksLikeSmartCNP (&smartCNPInfo);
	size_t	savedSelStart	=	GetSelectionStart ();	// save cuz InternalizeBestFlavor () will tend to adjust selStart
	InternalizeBestFlavor (clipData);
	Led_Assert (savedSelStart <= GetSelectionStart ());	//	InternalizeBestFlavor can only adjust it FORWARD - not backward...
	if (doSmartCNP) {
		OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (savedSelStart, smartCNPInfo);
	}
}

/*
@METHOD:		TextInteractor::OnPasteCommand_PasteFlavor_Specific
@ACCESS:		protected
@DESCRIPTION:	<p></p>
 */
void	TextInteractor::OnPasteCommand_PasteFlavor_Specific (Led_ClipFormat format)
{
	ReaderClipboardFlavorPackage	clipData;
	SmartCNPInfo					smartCNPInfo;
	bool							doSmartCNP		=	PasteLooksLikeSmartCNP (&smartCNPInfo);
	size_t							savedSelStart	=	GetSelectionStart ();	// save cuz InternalizeBestFlavor () will tend to adjust selStart
	InternalizeFlavor_Specific (clipData, format);
	Led_Assert (savedSelStart <= GetSelectionStart ());	//	InternalizeBestFlavor can only adjust it FORWARD - not backward...
	if (doSmartCNP) {
		OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (savedSelStart, smartCNPInfo);
	}
}

/*
@METHOD:		TextInteractor::PasteLooksLikeSmartCNP
@ACCESS:		protected
@DESCRIPTION:	<p></p>
 */
bool	TextInteractor::PasteLooksLikeSmartCNP (SmartCNPInfo* scnpInfo) const
{
	Led_RequireNotNil (scnpInfo);
	ReaderClipboardFlavorPackage	clipData;
	bool							doSmartCNP	=	GetSmartCutAndPasteMode () and clipData.GetFlavorAvailable_TEXT ();
	if (doSmartCNP) {
		/*
		 *	Check if this REALLY looks like a good opportunity todo a smart-cut-and-paste whitespace adjustment.
		 */
		size_t			length		=	clipData.GetFlavorSize (kTEXTClipFormat);
		Led_ClipFormat	textFormat	=	kTEXTClipFormat;
		#if		qWideCharacters && qWorkAroundWin95BrokenUNICODESupport
		if (length == 0) {
			textFormat = CF_TEXT;
			length = clipData.GetFlavorSize (textFormat);
		}
		#endif

		Led_SmallStackBuffer<char>	buf (length);	// could use bufsize=(len+1)/sizeof (Led_tChar)
		length = clipData.ReadFlavorData (textFormat, length, buf);
		if (doSmartCNP) {
			Led_tChar*		buffp			= reinterpret_cast<Led_tChar*> (static_cast<char*> (buf));
			#if		qWideCharacters && qWorkAroundWin95BrokenUNICODESupport
				if (textFormat != kTEXTClipFormat) {
					// then we must manually convert the clipboard text to UNICODE
					string	tmp			=	string (static_cast<char*> (buf), length);
					length	= ::MultiByteToWideChar (CP_ACP, 0, tmp.c_str (), tmp.length (), buffp, length/sizeof (Led_tChar)) * sizeof (Led_tChar);
				}
			#endif
			size_t	nTChars	=	length / sizeof (Led_tChar);
			doSmartCNP = LooksLikeSmartPastableText (buffp, nTChars, scnpInfo);
		}
	}
	return doSmartCNP;
}

/*
@METHOD:		TextInteractor::OnSelectAllCommand
@DESCRIPTION:	<p>Command to implement the "Select All" UI. Trivial implementation, but nearly all UI's want it, so why
			write it each time?</p>
*/
void	TextInteractor::OnSelectAllCommand ()
{
	SetSelection (0, GetLength ());
}

bool	TextInteractor::CanAcceptFlavor (Led_ClipFormat clipFormat) const
{
       //	Led_ClipFormat	textFormat	=	kTEXTClipFormat;
	#if		qWideCharacters && qWorkAroundWin95BrokenUNICODESupport
		if (clipFormat == CF_TEXT) {
			return true;
		}
	#endif
	return (kTEXTClipFormat == clipFormat or kFILEClipFormat == clipFormat);
}

void	TextInteractor::InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage,
													bool updateCursorPosition, bool autoScroll, UpdateMode updateMode
												)
{
	size_t	start	=	GetSelectionStart ();
	size_t	end		=	GetSelectionEnd ();

	bool	good	=	false;
	{
		TempMarker	newSel (GetTextStore (), end+1, end+1);	// NB: This marker pos MAY split a multibyte character - but that should cause
															// no problems.
															// We are only keeping this temporarily and we subtract one at the
															// end. Just want to make it past point where we do the insertion
															// so we get the right adjustment on
		good	=	fInternalizer->InternalizeBestFlavor (flavorPackage, start, end);
		if (good and updateCursorPosition) {
			SetCaretShownAfterPos (true);	// by default show pos after - this is what works best for NL's and is what we
											// want most of the time...	

			// We placed a marker one past the end of the selection. Then we replaced the given selection.
			// So the marker now points one past where it really should. Backward adjust it, and set the selection there.
			size_t	newSelection	=	newSel.GetStart ();
			if (newSelection > start) {
				newSelection--;
			}

			SetSelection (newSelection, newSelection);
		}
	}

	if (good) {
		if (autoScroll) {
			ScrollToSelection ();
		}
		if (updateMode == eImmediateUpdate) {
			Update ();
		}
	}
	else {
		OnBadUserInput ();
	}
}

/*
@METHOD:		TextInteractor::InternalizeFlavor_Specific
@DESCRIPTION:	<p></p>
 */
void	TextInteractor::InternalizeFlavor_Specific (ReaderFlavorPackage& flavorPackage, Led_ClipFormat format,
													bool updateCursorPosition, bool autoScroll, UpdateMode updateMode
												)
{
	size_t	start	=	GetSelectionStart ();
	size_t	end		=	GetSelectionEnd ();

	bool	good	=	false;
	{
		TempMarker	newSel (GetTextStore (), end+1, end+1);	// NB: This marker pos MAY split a multibyte character - but that should cause
															// no problems.
															// We are only keeping this temporarily and we subtract one at the
															// end. Just want to make it past point where we do the insertion
															// so we get the right adjustment on


		if (format == kTEXTClipFormat) {
			good	=	fInternalizer->InternalizeFlavor_TEXT (flavorPackage, start, end);
		}
		else if (format == kFILEClipFormat) {
			good	=	fInternalizer->InternalizeFlavor_FILE (flavorPackage, start, end);
		}
		else {
			good	=	fInternalizer->InternalizeBestFlavor (flavorPackage, start, end);
		}

		if (good and updateCursorPosition) {
			SetCaretShownAfterPos (true);	// by default show pos after - this is what works best for NL's and is what we
											// want most of the time...	

			// We placed a marker one past the end of the selection. Then we replaced the given selection.
			// So the marker now points one past where it really should. Backward adjust it, and set the selection there.
			size_t	newSelection	=	newSel.GetStart ();
			if (newSelection > start) {
				newSelection--;
			}

			SetSelection (newSelection, newSelection);
		}
	}

	if (good) {
		if (autoScroll) {
			ScrollToSelection ();
		}
		if (updateMode == eImmediateUpdate) {
			Update ();
		}
	}
	else {
		OnBadUserInput ();
	}
}

/*
@METHOD:		TextInteractor::MakeDefaultInternalizer
@DESCRIPTION:	<p>Make a @'FlavorPackageInternalizer' which is appropriate for this text interactor. Override this
	to make a different subclass, which supports different style and file formats from the (simple) default.</p>
		<p>By default, this creates a @'FlavorPackageInternalizer'.</p>
*/
Led_RefCntPtr<FlavorPackageInternalizer>	TextInteractor::MakeDefaultInternalizer ()
{
	return new FlavorPackageInternalizer (GetTextStore ());
}

/*
@METHOD:		TextInteractor::HookInternalizerChanged
@DESCRIPTION:	<p>Called by @'TextInteractor::SetInternalizer' whenever there is a new internalizer specified.</p>
*/
void	TextInteractor::HookInternalizerChanged ()
{
}

/*
@METHOD:		TextInteractor::ExternalizeFlavors
@DESCRIPTION:	<p>Use the associated externalizer (see @'TextInteractor::SetExternalizer')  to call
			ExternalizeFlavors applied to the current selection.</p>
*/
void	TextInteractor::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
	fExternalizer->ExternalizeFlavors (flavorPackage, GetSelectionStart (), GetSelectionEnd ());
}

/*
@METHOD:		TextInteractor::ExternalizeBestFlavor
@DESCRIPTION:	<p>Use the associated externalizer (see @'TextInteractor::SetExternalizer')  to call
			ExternalizeBestFlavor applied to the current selection.</p>
*/
void	TextInteractor::ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage)
{
	fExternalizer->ExternalizeBestFlavor (flavorPackage, GetSelectionStart (), GetSelectionEnd ());
}

/*
@METHOD:		TextInteractor::MakeDefaultExternalizer
@DESCRIPTION:	<p>Make a @'FlavorPackageExternalize' which is appropriate for this text interactor. Override this
	to make a different subclass, which supports different style and file formats from the (simple) default.</p>
		<p>By default, this creates a @'FlavorPackageExternalizer'.</p>
*/
Led_RefCntPtr<FlavorPackageExternalizer>	TextInteractor::MakeDefaultExternalizer ()
{
	return new FlavorPackageExternalizer (GetTextStore ());
}

/*
@METHOD:		TextInteractor::HookExternalizerChanged
@DESCRIPTION:	<p>Called by @'TextInteractor::SetExternalizer' whenever there is a new externalizer specified.</p>
*/
void	TextInteractor::HookExternalizerChanged ()
{
}

/*
@METHOD:		TextInteractor::OnBadUserInput
@DESCRIPTION:	<p>By default this throws @'TextInteractor::BadUserInput' but it can be overriden to
		NOT throw anything, and just beep or something. BEWARE then when calling this that it may or
		may not throw, and may or may not return.</p>
*/
void	TextInteractor::OnBadUserInput ()
{
	// you may want to override this to do a staged alert, or to throw an exception???
	//Led_BeepNotify ();
	throw BadUserInput ();	// default catcher should call Led_BeepNotify ()
}

/*
@METHOD:		TextInteractor::SetScrollBarType
@DESCRIPTION:	<p>Specify whether or not the interactor will display / manage scrollbars. This really is handled in OS/ClassLib specific subclasses.
			But the API is here to keep it uniform across the platforms.</p>
				<p>The default settings for each direction (v/h) are 'TextInteractor::eScrollBarNever'
			(except than for the Windows platform, @'Led_Win32_Helper<BASE_INTERACTOR>::OnCreate_Msg'
			and @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnCreate' will set the initial value according to 
			the windows style passed into the WM_CREATE message).</p>
				<p>On windows I recall that it USED to be standard that an edit text would suddenly
			spawn a scrollbar when needed. This doesn't appear to be the case any longer.
			I always hated this behavior, but since I didn't know better, I implemented it.</p>
				<p>Even though this behavior is no longer particularly standared or common, it is sometimes
			desired, and so supported.</p>
				<p>NB: You need not turn on the WS_V/HSCROLL styles to make the scrollbars appear/disapear.
			When this is on, it is handled automagically.</p>
				<p>NB: This USED to be controlled by a compile-time variable @'qMakeWindowsScrollbarAppearOnlyIfNeeded',
			which is now obsolete.</p>
				<p>See also 'TextInteractor::GetScrollBarType' and @'TextInteractor::InvalidateScrollBarParameters'.</p>
*/
void	TextInteractor::SetScrollBarType (VHSelect vh, ScrollBarType scrollBarType)
{
	if (GetScrollBarType (vh) != scrollBarType) {
		InvalidateScrollBarParameters ();
		SetScrollBarType_ (vh, scrollBarType);
	}
}

/*
@METHOD:		TextInteractor::InvalidateScrollBarParameters
@DESCRIPTION:	<p>Mark the contents of the scrollbars as invalid. Someone, sometime later soon will call
	@'TextInteractor::UpdateScrollBars' to fix them up again.</p>
*/
void	TextInteractor::InvalidateScrollBarParameters ()
{
	InvalidateScrollBarParameters_ ();
}

/*
@METHOD:		TextInteractor::UpdateScrollBars
@DESCRIPTION:	<p>Override this to handle any update of the scrollbars you may need to. Something has happened to
	invalidate what they now display (new text added, scrolled, or whatever - someone called @'TextInteractor::InvalidateScrollBarParameters').
	This is usually taken care of in any class library wrapper code, such as @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'
	or @'Led_Win32_Helper<BASE_INTERACTOR>'.</p>
*/
void	TextInteractor::UpdateScrollBars ()
{
	UpdateScrollBars_ ();
}

/*
@METHOD:		TextInteractor::SetCaretShown
@DESCRIPTION:	<p>See also @'TextInteractor::GetCaretShownSituation' and @'TextInteractor::GetCaretShown'.</p>
*/
void	TextInteractor::SetCaretShown (bool shown)
{
	if (GetCaretShown () != shown) {
		fCaretShown = shown;
		InvalidateCaretState ();
		#if		qMacOS
		// On the mac - when it was shown, and now is not - we MAY need to force an update to get it erased - and when not shown- the
		// InvalidateCaretState () method doesn't force an update.
		if (not shown) {
			RefreshWindowRect (CalculateCaretRect ());
		}
		#endif
	}
}

/*
@METHOD:		TextInteractor::GetCaretShownSituation
@DESCRIPTION:	<p>To decide if its appropriate for the editor to display a blinking caret, we check an
			overall state variable @'TextInteractor::GetCaretShown' which is logically tied to
			whether or not the edit widget has the focus. But - thats not all that needs to be
			considered to decide if you should display the caret. You also - typically - need
			to have an empty selection. This virtual method does that particular check.</p>
*/
bool	TextInteractor::GetCaretShownSituation () const
{
	size_t	selStart	=	0;
	size_t	selEnd		=	0;
	GetSelection (&selStart, &selEnd);
	return (selStart == selEnd);
}

/*
@METHOD:		TextInteractor::SetCaretShownAfterPos
@DESCRIPTION:	<p>See @'TextInteractor::GetCaretShownAfterPos' for meaning of this flag.</p>
 */
void	TextInteractor::SetCaretShownAfterPos (bool shownAfterPos)
{
	if (GetCaretShownAfterPos () != shownAfterPos) {
		InvalidateCaretState ();	// before and after so we draw/erase in new and old places
		fCaretShownAfterPos = shownAfterPos;
		InvalidateCaretState ();
	}
}

/*
@METHOD:		TextInteractor::CalculateCaretRect
@ACCESS:		protected
@DESCRIPTION:	<p>This is complicated due to worrying about bidirectional editing, and due to the fact
			that two adjacent characters in logical order, maybe in very different parts of the screen
			(e.g. the marker position between the end of a row and the start of the next row maybe identical, or
			the marker position between one character and another WITHIN a row at a directional boundary may
			indicate two very different caret locations).</p>
				<p>The first issue to resolve is - <em>which character</em> to we care most about - the one <em>preceeding</em>
			the marker position or the one <em>following</em>. This question is arbitrated by the <em>@'TextInteractor::GetCaretShownAfterPos'</em>
			API.</p>
				<p>The second question - as to which side of the character to display the caret - is decided
			by the text direction. But WHICH text direction? The one of the preceeding or following character? Easy - we use
			the same choice as chosen by the first question.
				</p>
 */
Led_Rect	TextInteractor::CalculateCaretRect () const
{
	size_t	selEnd	=	GetSelectionEnd ();
	if (GetSelectionStart () == selEnd) {
		bool		showAfter	=	GetCaretShownAfterPos ();
		if (selEnd == 0) {
			showAfter = true;
		}

		size_t			charAfterPos	=	showAfter? selEnd: FindPreviousCharacter (selEnd);
		TextDirection	textDirection	=	GetTextDirection (charAfterPos);
		Led_Rect		caretRect		=	GetCharWindowLocation (charAfterPos);

		if (caretRect.GetBottom () < GetWindowRect ().GetTop () or caretRect.GetTop () > GetWindowRect ().GetBottom ()) {
			return (Led_Rect (0, 0, 0, 0));
		}

		Led_Rect		origCaretRect	=	caretRect;
		Led_FontMetrics	fontMetrics		=	GetFontMetricsAt (charAfterPos);

		// WE NEED THE WHOLE ROW BASELINE!!! THEN CAN COMPUTE caretrect from that!!!
		Led_Distance	baseLineFromTop	=	GetRowRelativeBaselineOfRowContainingPosition (charAfterPos);
		Led_Coordinate	realBaseLine	=	baseLineFromTop + caretRect.top;

		// now adjust caretrect to be font-metrics from the base line
		caretRect.top = realBaseLine - fontMetrics.GetAscent ();
		caretRect.bottom = realBaseLine + fontMetrics.GetDescent ();

		// Before pinning CaretRect to full rowRect, first try to adjust it so its inside.
		// It can come outside the rowRect if the baseLineFromTop is zero - for example - when
		// the preceeding text is hidden.
		if (caretRect.top < origCaretRect.top) {
			Led_Distance	diff	=	origCaretRect.GetTop () - caretRect.GetTop ();
			caretRect += Led_Point (diff, 0);
		}

		// pin CaretRect to full rowRect
		caretRect.SetTop (max (caretRect.GetTop (), origCaretRect.GetTop ()));
		caretRect.bottom = min (caretRect.GetBottom (), origCaretRect.GetBottom ());

		if (textDirection == eLeftToRight) {
			if (not showAfter) {
				caretRect.left = caretRect.right;
			}
		}
		else {
			caretRect.left = caretRect.right;
		}

		const Led_Coordinate	kCaretWidth = 1;
		// quickie hack to be sure caret doesn't go off right side of window!!!
		const Led_Coordinate	kSluff = kCaretWidth+1;
		if (caretRect.GetLeft () + kSluff > GetWindowRect ().GetRight ()) {
			caretRect.SetLeft (GetWindowRect ().GetRight () - kSluff);
		}
		caretRect.SetRight (caretRect.GetLeft () + kCaretWidth);

		Led_Ensure (not caretRect.IsEmpty ());
		return (caretRect);	
	}
	else {
		return (Led_Rect (0, 0, 0, 0));
	}
}

void	TextInteractor::InvalidateCaretState ()
{
	if (IsWholeWindowInvalid ()) {
		return;
	}
	if (GetCaretShown () and (GetSelectionStart () == GetSelectionEnd ())) {
		RefreshWindowRect (CalculateCaretRect ());
	}
}

/*
@METHOD:		TextInteractor::OnTypedNormalCharacter
@DESCRIPTION:	<p>High level handling that does basically all of the portable support for a typed character.
			This is typically what you would call (the class library integration classes call this directly) to handle
			keyboard input. You might also plausibly override this method to provide special handling for
			particular key sequences (e.g. LedLineIt! overrides this to map shift-tab when there is a selection
			to an indent command rather than an insertion of text).</p>
 */
void	TextInteractor::OnTypedNormalCharacter (Led_tChar theChar, bool /*optionPressed*/, bool /*shiftPressed*/, bool /*commandPressed*/, bool controlPressed, bool /*altKeyPressed*/)
{
	IdleManager::NonIdleContext	nonIdleContext;

	#if		qMultiByteCharacters
		if (HandledMByteCharTyping (theChar)) {
			return;
		}
	#endif

	Led_Assert (GetSelectionEnd () <= GetLength () + 1);

	if (GetSupressTypedControlCharacters ()) {
		bool	controlChar	=	CharacterProperties::IsCntrl (theChar);
		if (controlChar &&
				(theChar == '\r' || theChar == '\n' || theChar == ' ' || theChar == '\t' || theChar == '\b')
			)
		{
			controlChar = false;
		}
		if (controlChar) {
			OnBadUserInput ();
			return;
		}
	}

	switch (theChar) {
		case	'\b': {
			CursorMovementDirection	dir		=	eCursorBack;
			CursorMovementUnit		unit	=	controlPressed? eCursorByWord: eCursorByChar;
			CursorMovementAction	action	=	eCursorDestroying;
			DoSingleCharCursorEdit (dir, unit, action, eDefaultUpdate);
		}
		break;

		default: {
			if (theChar == '\n') {
				BreakInGroupedCommands ();
			}
			InteractiveReplace (&theChar, 1, eDefaultUpdate);
		}
		break;
	}

	ScrollToSelection ();
	#if		qPeekForMoreCharsOnUserTyping
		UpdateIfNoKeysPending ();
	#else
		Update ();
	#endif
}

#if		qMultiByteCharacters
bool	TextInteractor::HandledMByteCharTyping (char theChar)
{
	if (fMultiByteInputCharBuf[0] == '\0' and Led_IsLeadByte (theChar)) {
		/*
		 *	If we get a first-byte - then don't process it yet. Just save it up for the
		 *	next call.
		 */
		fMultiByteInputCharBuf[0] = theChar;
		fMultiByteInputCharBuf[1] = '\0';
		return true;			// done with processing the character...
	}
	else if (fMultiByteInputCharBuf[0] != '\0') {
		/*
		 *	If we have a PENDING first-byte - then append this to our buffer, and pretend
		 *	the user typed these two bytes. However - if we get a BAD second byte -
		 *	then call BadUserInput() (basicly does a sysbeep) and drop it on the
		 *	floor.
		 */
		fMultiByteInputCharBuf[1] = theChar;		// set it even if its bad so OnBadUserInput can peek()
		if (ValidateTextForCharsetConformance (fMultiByteInputCharBuf, 2)) {
			InteractiveReplace (fMultiByteInputCharBuf, 2);
			fMultiByteInputCharBuf[0] = '\0';
			size_t	newSelection	=	FindNextCharacter (GetSelectionStart ());
			SetSelection (newSelection, newSelection);
			ScrollToSelection ();
			#if		qPeekForMoreCharsOnUserTyping
				UpdateIfNoKeysPending ();
			#else
				Update ();
			#endif
		}
		else {
			OnBadUserInput ();
			fMultiByteInputCharBuf[0] = '\0';
		}
		return true;			// done with processing the character...
	}
	return false;	// We did nothing - handle character as usual
}
#endif

#if		qMacOS || qXWindows
float		TextInteractor::GetTickCountBetweenBlinks ()
{
#if		qMacOS
	return (::GetCaretTime () / 60.0);
#elif	qXWindows
	return (0.4f);
#endif
}
#endif

bool	TextInteractor::DelaySomeForScrollBarClick ()
{
	const	float	kDelayAfterFirstTicks	=	0.20f;		// maybe should use ::GetDblClickTime()???
	const	float	kDelayAfterOtherTicks	=	0.02f;		// This delay is so on really fast computers, text doesn't scroll too quickly
	const	int		kTimesForFirstClick		=	2;
	const	float	kLongTime				=	1.0f;		// any click after this time deemed we start again with first-tick
	static	short	sTimesThruBeforeReset;

	float	now	=	Led_GetTickCount ();
	if (fLastScrolledAt == 0 or fLastScrolledAt + kLongTime < now) {
		fLastScrolledAt = now + kDelayAfterFirstTicks;
		sTimesThruBeforeReset = 1;
		return true;	// first time through - handle click immediately
	}
	else if (fLastScrolledAt < now) {
		sTimesThruBeforeReset++;
		fLastScrolledAt = now + (sTimesThruBeforeReset<=kTimesForFirstClick? kDelayAfterFirstTicks: kDelayAfterOtherTicks);
		return true;	// enuf time has elapsed
	}
	else {
		return false;	// not enough time has elapsed
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

