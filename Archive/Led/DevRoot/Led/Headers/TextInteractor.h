/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__TextInteractor_h__
#define	__TextInteractor_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/TextInteractor.h,v 2.148 2004/02/11 23:57:52 lewis Exp $
 */

/*
@MODULE:	TextInteractor
@DESCRIPTION:
		<p>This module defines the @'TextInteractor' class, which subclasses from @'TextImager'
	and introduces user-interaction
	behavior - such as keyclicks, mouse tracking, and the notion of @'TextInteractor::UpdateMode' s.</p>
 */


/*
 * Changes:
 *	$Log: TextInteractor.h,v $
 *	Revision 2.148  2004/02/11 23:57:52  lewis
 *	SPR#1576: Update: added ReplaceAllInSelection functionality (except must re-layout MacOS dialog)
 *	
 *	Revision 2.147  2004/02/11 22:41:26  lewis
 *	SPR#1576: update: added 'Replace Again' command to LedIt, LedLineItMFC, ActiveLedIt
 *	
 *	Revision 2.146  2004/01/25 21:41:38  lewis
 *	SPR#1284: InternalizeFlavor_Specific/OnPasteCommand_PasteFlavor_Specific methods added to support 'PasteAsText' method for ActiveLedIt
 *	
 *	Revision 2.145  2003/12/31 04:13:11  lewis
 *	SPR#1580: new TextInteractor::SearchStrings class to replace (subclass from) TextStore::SearchParaemeters.
 *	Adds fRecentFindStrings. Changed override of TextInteractor::DialogSupport::DisplayFindDialog/
 *	TextInteractor::DialogSupport::DisplayReplaceDialog to take extra parameter. Added MergeRecentFindStrings ()
 *	method to combine search strings into non-infinite (non-redundant) list of recent strings.
 *	
 *	Revision 2.144  2003/12/11 02:57:55  lewis
 *	SPR#1590: respoect AddToDictioanryEnabled callback value to disable the addtodict dialog button. Also -
 *	added similar OptionsDIalogEnabled callback and respected that so button disabled (til we've had
 *	time to implement an options dialog)
 *	
 *	Revision 2.143  2003/12/11 01:31:54  lewis
 *	SPR#1589: Implemented 'lookup on web' button in spell check dialog. Only tested/tuned dialog resource for
 *	Win32. Must test/tune for MacOS/XWin.
 *	
 *	Revision 2.142  2003/12/09 20:49:20  lewis
 *	SPR#1585: added UD support (add word to UD, new SpellCheckEngine_Basic_Simple, major revision to
 *	SpellCheckEngine_Basic::EditableDictionary, etc)
 *	
 *	Revision 2.141  2003/11/27 01:50:24  lewis
 *	SPR#1050: added CommandHandler::BreakInGroupedCommandsIfDifferentCommand () to help
 *	
 *	Revision 2.140  2003/11/26 20:04:32  lewis
 *	SPR#1564: major change to improve undo memory usage. Added SimplePlainTextInsertOptimization flag and
 *	when set (typing a single character) - we first try to just append the character to the 'afterRep',
 *	and if that fails (cuz its not of the right type) - then at least create a simple afterRep (PlainTextRep)
 *	for this special case. Note - we had to modify the smantics (backward compat code note!) of
 *	InteractiveUndoHelperMakeTextRep to return a bool - indicating if there was any change.
 *	
 *	Revision 2.139  2003/06/02 16:13:53  lewis
 *	SPR#1513: first draft of (windows only so far) simple spellchecking dialog. Basically working (75%).
 *	A few fixes to generic dialog support code
 *	
 *	Revision 2.138  2003/05/30 11:42:34  lewis
 *	SPR#1517: Added static member and API to keep track of 'ReplaceParameters'. Methods are virtual so apps
 *	COULD persist them, but I'm not sure thats useful (where it clearly is for find parameters)
 *	
 *	Revision 2.137  2003/05/30 03:06:50  lewis
 *	SPR#1517: preliminary version of REPLACE DIALOG support (WIN32 only so far).
 *	
 *	Revision 2.136  2003/05/21 20:47:01  lewis
 *	SPR#1494: Added TextInteractor::SupressCommandBreaksContext stack-based class to allow some commands
 *	to be grouped together (when acting on a bunch of table cells)
 *	
 *	Revision 2.135  2003/05/21 12:53:11  lewis
 *	SPR#1495: rename UndoableContextHelper::GetSelStart/End to UndoableContextHelper::GetUndoRegionStart/End
 *	
 *	Revision 2.134  2003/05/20 22:46:00  lewis
 *	SPR#1493: lose UndoableContextHelper::SetBeforeUndoSelection and replace function
 *	with new CTORs. Now careful about distinction between selStart and regionStart (though
 *	simple CTOR to support common case where both the same).
 *	
 *	Revision 2.133  2003/05/15 12:50:29  lewis
 *	SPR#1487: Add a 'GetEnabled' method to TextInteractor::CommandUpdater and subclasses
 *	(to help with SPR)
 *	
 *	Revision 2.132  2003/05/07 21:11:00  lewis
 *	SPR#1467: implemented various select commands (word/row/paragraph/table/cell/row/col)
 *	
 *	Revision 2.131  2003/05/01 23:24:43  lewis
 *	SPR#1456: Added DisableRefreshContext to EmbeddedTableWordProcessor code so we can
 *	supress refresh from RestoreMiscActiveFocusInfo code
 *	
 *	Revision 2.130  2003/05/01 22:15:33  lewis
 *		(SPR#1455)Changed EmbeddedTableWordProcessor::UpdateWindowRect_ () so it throws new
 *	CannotUpdateNow () exception, and revised TextInteractor::Update/UpdateWindowRect
 *	so they take an optional parameter saying if it should automatically eat
 *	these new 'CannotUpdateNow' exceptions.
 *		Also changed TextInteractor::PreScrollHelper so it handles throws (calls with false
 *	for Update ingore flag) - and resets fTryTodoScrollbits.
 *		Also, override EmbeddedTableWordProcessor::SetDefaultUpdateMode () to force
 *	always to use eDelayedUpdate in place of immediateUpdate. That should greatly reduce
 *	the number of places people try to call Update ().
 *	
 *	Revision 2.129  2003/04/30 14:54:39  lewis
 *	tweeks to get compiling nicely with GCC 3.2 (RedHat 8.0)
 *	
 *	Revision 2.128  2003/04/30 14:26:30  lewis
 *	fix typename usage in tempalte (so compat with GCC 3.2)
 *	
 *	Revision 2.127  2003/04/21 16:17:26  lewis
 *	qCannotStaticlyInitializeConstantsDataMembersInClassDeclaration/qTroubleLookingUpBaseClassInNestedClassDeclaration_VC6
 *	for MSVC60 compat
 *	
 *	Revision 2.126  2003/04/18 17:01:36  lewis
 *	small code cleanups to things complained about by new gcc 2.96
 *	
 *	Revision 2.125  2003/04/16 14:04:12  lewis
 *	SPR#1437: cosmetic cleanups - nest code and scope in in braces inside of UndoContext instances,
 *	and use new TextInteractor::BreakInGroupedCommands () wrapper (and other small cleanups)
 *	
 *	Revision 2.124  2003/04/15 23:08:30  lewis
 *	qAccessCheckAcrossInstancesSometimesWrong
 *	
 *	Revision 2.123  2003/04/15 22:58:04  lewis
 *	qAccessCheckAcrossInstancesSometimesWrong GCC/Linux BWA
 *	
 *	Revision 2.122  2003/04/10 14:02:21  lewis
 *	(vaguely related to work on SPR#1329)- cleanup doccomments and org of TextInteractor::OnCopy/
 *	Paste_Before/After. Moved mac/Win specific code exclusely to _MacOS/_Win32 subclasses(tempaltes).
 *	mproved docs
 *	
 *	Revision 2.121  2003/04/09 18:35:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.120  2003/04/09 16:31:03  lewis
 *	SPR#1419: lose Refresh_ and Update_ overloaded pure virtuals. Lost TextInteractor::Refresh
 *	(const Led_Rect& area, UpdateMode updateMode) and TextInteractor::Update (const Led_Rect& area)
 *	methods as they are never used and really are unwise and should be discouraged. For Refresh(UPDATEMODE) -
 *	just call RefreshWindowREct_(GetWindowREct, UpdateMode) inernally. Shouldn't impact on any code users
 *	have written (at least its unlikely)
 *	
 *	Revision 2.119  2003/04/09 14:25:21  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.118  2003/04/09 02:04:22  lewis
 *	minor tweeks to get compiling for GCC
 *	
 *	Revision 2.117  2003/04/08 23:34:54  lewis
 *	SPR#1404: added TextInteractor::GetCaretShownSituation () method to wrap selStart==selEnd
 *	check (so we can override that for table code blinking caret. Updated each Led_XXX
 *	classlib wrapper to call that instead of selStart==selEnd
 *	
 *	Revision 2.116  2003/04/08 21:35:42  lewis
 *	SPR#1329/SPR#1404: need enum CursorMovementAction do I can handle cursor movements
 *	in Table class
 *	
 *	Revision 2.115  2003/04/07 15:17:12  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.114  2003/04/07 15:15:47  lewis
 *	SPR#1409: added fUndoFormatString / fRedoFormatString command strings, and use that with
 *	Format() calls instead of hardwired strgins in TextInteractor::OnUpdateUndoRedoCommand ()
 *	
 *	Revision 2.113  2003/04/04 19:52:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.112  2003/04/04 16:26:02  lewis
 *	SPR#1407- getting new command processing code working on X-Windows
 *	
 *	Revision 2.111  2003/04/04 02:38:19  lewis
 *	SPR#1407: cleanup work on new command strategy (no templates)
 *	
 *	Revision 2.110  2003/04/04 00:55:22  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly
 *	MacOS/Linux now)
 *	
 *	Revision 2.109  2003/04/03 22:48:29  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.108  2003/04/03 21:52:46  lewis
 *	SPR#1407: more cleanups of new CommandNumberMapping<> stuff and other new command# processing support
 *	
 *	Revision 2.107  2003/04/03 16:41:24  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated command
 *	classes, just builtin to TextInteractor/WordProcessor (and instead of template params use new
 *	TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.106  2003/03/21 13:59:38  lewis
 *	SPR#1364- fix setting of drag anchor. Moved it from Led_XXX(Win32/MacOS/Gtk) to
 *	TextInteractor::ProcessSimpleClick (adding extra arg). Also lots of other small code cleanups
 *	
 *	Revision 2.105  2003/03/20 20:56:08  lewis
 *	DrawBefore/DrawAfter are now protected
 *	
 *	Revision 2.104  2003/03/20 15:52:29  lewis
 *	SPR#1360 - added DrawBefore () API and comment out 'EnterIdleCallback' support
 *	
 *	Revision 2.103  2003/03/17 21:40:28  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.102  2003/03/17 21:37:42  lewis
 *	SPR#1350 - qSupportEnterIdleCallback is now automatically true - and not a conditional
 *	varaible anymore
 *	
 *	Revision 2.101  2003/03/10 19:09:04  lewis
 *	SPR#1286 - OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds
 *	OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds to better handle case of
 *	when to insert smart spaces.
 *	
 *	Revision 2.100  2003/02/03 20:35:44  lewis
 *	minor tweek to compile with gcc 1.95
 *	
 *	Revision 2.99  2003/02/01 00:32:34  lewis
 *	add SimpleCommandUpdater<> helper class as a small part of SPR#1270.
 *	
 *	Revision 2.98  2003/01/31 23:08:24  lewis
 *	SPR#1278- added InteractiveUpdadeMode and InteractiveModeUpdater so things like AboutToUpdate() can
 *	tell if its a USER change or a program change to text its about to make (and so whether or not to
 *	allow/disallow)
 *	
 *	Revision 2.97  2003/01/29 19:15:10  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.96  2003/01/11 19:28:39  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.95  2002/10/06 00:13:44  lewis
 *	DOCCOMEMNT
 *	
 *	Revision 2.94  2002/09/19 15:30:35  lewis
 *	get working on VC++6 - added qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
 *	
 *	Revision 2.93  2002/09/19 14:14:19  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like
 *	qOldStyleMarkerBackCompatHelperCode, qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.92  2002/05/06 21:33:38  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.91  2001/11/27 00:29:47  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.90  2001/10/20 13:38:57  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.89  2001/10/19 20:47:14  lewis
 *	DocComments
 *	
 *	Revision 2.88  2001/10/18 13:18:43  lewis
 *	cleanup DocComments
 *	
 *	Revision 2.87  2001/10/17 20:42:55  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.86  2001/10/09 21:02:53  lewis
 *	SPR#1058- qSupportLed23CompatAPI support
 *	
 *	Revision 2.85  2001/09/28 14:04:20  lewis
 *	SPR#1051- added flag to TextInteractor::ScrollToSelection () to make cursoring
 *	behavior work a bit better
 *	
 *	Revision 2.84  2001/09/12 17:23:24  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.83  2001/09/12 14:53:30  lewis
 *	SPR#0987- Added conditional UseSecondaryHilight support (macos&win32 only so far).
 *	Redo Led_Region support for MacOS so now more like with PC (no operator conversion
 *	to macregion and auto constructs OSRegion except when called with region arg - in whcih
 *	case we dont OWN region and dont delete it on DTOR
 *	
 *	Revision 2.82  2001/09/05 00:07:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.81  2001/09/05 00:04:34  lewis
 *	SPR#1013- A few small changes to TextInteractor::UndoableContextHelper to enable its use in Led_MFC_
 *	and Led_PP D&D code. And added UndoableContextHelper::SetBeforeUndoSelection () to fix a small
 *	trouble with that code.
 *	
 *	Revision 2.80  2001/08/29 23:01:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.79  2001/08/28 18:43:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.78  2001/08/01 23:47:17  lewis
 *	SPR#0965- TextInteractor::BadUserInput class, and throw that from TextInteractor::OnBadUserInput ()
 *	instead of directly calling Led_BeepNotify ();
 *	
 *	Revision 2.77  2001/07/19 23:43:40  lewis
 *	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug support
 *	
 *	Revision 2.76  2001/07/12 13:55:26  lewis
 *	SPR#0906- hack override WordProcessor::PostReplace() so typing at end of buffer
 *	with listmode on gets new empty lines bullet drawn/erased
 *	
 *	Revision 2.75  2001/05/03 22:07:13  lewis
 *	related to SPR#0887- Xwindows blinking caret support. TextInteractor::GetTickCountBetweenBlinks
 *	now returns a float (seconds)
 *	
 *	Revision 2.74  2001/05/01 16:07:43  lewis
 *	fixed bad names - Get/SetCurClickClickCount to just one CLICK
 *	
 *	Revision 2.73  2001/04/17 22:48:21  lewis
 *	SPR#0871- Moved a bunch of code from Led_MacOS and Led_Win32 to TextInteractor. Code was
 *	all relating to TextInteractor::GetCurClickClickCount () etc. Also - improved logic for
 *	double click detection (now shared across platforms).
 *	
 *	Revision 2.72  2001/04/17 18:45:27  lewis
 *	SPR#0870- SupressTypedControlCharacters support
 *	
 *	Revision 2.71  2001/01/03 14:59:01  Lewis
 *	qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS
 *	
 *	Revision 2.70  2000/10/23 01:10:05  lewis
 *	must use typename in a few templates to get compiling with GCC
 *	
 *	Revision 2.69  2000/10/19 16:38:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.68  2000/10/18 20:39:55  lewis
 *	Added Led_StdDialogHelper_FindDialog support. Works on Windows and prelim for Mac
 *	
 *	Revision 2.67  2000/10/14 14:29:52  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.66  2000/10/06 03:34:12  lewis
 *	moved code/datatypes for controlling show/hide display of scrollbars for Led_Win32/Led_MacOS to -
 *	mostly - TextInteractor. Still some logic in the OS-specific wrappers. Unified their APIs -
 *	supporting autohide of sbars on both (though mac maybe still slightly buggy)
 *	
 *	Revision 2.65  2000/10/04 13:18:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.64  2000/10/03 21:50:19  lewis
 *	Lots more work relating to SPR#0839- moved stuff into WordProcessorCommonCommandHelper<> template,
 *	and used that better throughout Led_PP code etc. Broke out WP-specific stuff from Led_PP to separate
 *	file.Lots of cleanups of PP wrapper code
 *	
 *	Revision 2.63  2000/10/03 13:36:36  lewis
 *	SPR#0839- reorganize the WordProcessorCommonCommandHelper/TextInteractorCommonCommandHelper<> code.
 *	Now MFC wrapper support MUCH simpler, and all this stuff much easier to use from other platforms.
 *	OnPerformCommand/OnUpdateCommand simple virtual overrides, and _MSG simple single msg hooks for MFC.
 *	
 *	Revision 2.62  2000/09/30 19:35:01  lewis
 *	Added TextInteractorCommandHelper_DefaultCmdInfo/TextInteractorCommonCommandHelper<BASECLASS,CMD_INFO,CMD_ENABLER>.
 *	Added TextInteractor::OnSelectAllCommand. Moved Led_MFC_TmpCmdUpdater to Led_MFC.h
 *	
 *	Revision 2.61  2000/08/28 20:20:42  lewis
 *	SPR#0828- TextInteractor::UndoableContextHelper. SPR#0827- TextInteractor::GetCommandNames ().
 *	
 *	Revision 2.60  2000/04/24 22:29:32  lewis
 *	added @TextInteractor::HookExternalizerChanged - and use it in WordProcessor to sync up with
 *	the hidableText database - related to SPR#0724
 *	
 *	Revision 2.59  2000/04/24 16:47:02  lewis
 *	Very substantial change to FlavorPackage stuff - now NOT mixed into TextInteractor -
 *	but simply owned using Led_RefCntPtr<>. Also - did all the proper subclassing in WP
 *	class etc to make this work (almost - still must fix OLE embeddings) as before. SPR# 0742
 *	
 *	Revision 2.58  2000/04/15 14:32:36  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.57  2000/04/14 22:40:23  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.56  2000/04/14 19:15:48  lewis
 *	SPR##0739- react breakout into FlavorPackage code of that stuff from TextInteractor.
 *	And clean it up a bit - re-arranging who does the updateCursorPos stuff from the differnt
 *	flavor-package-formats
 *	
 *	Revision 2.55  2000/01/22 16:52:31  lewis
 *	Lose last default arg (ASSUME ALWAYS TRUE) for TextInteractor::PreInteractiveUndoHelper (part
 *	of cleanup and making this more understandable). And cleanup DOCCOMMENTS a bit more
 *	
 *	Revision 2.54  1999/12/29 03:25:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.53  1999/12/29 03:12:32  lewis
 *	SPR#0628- Lose #define qUseGDIScrollbitsForScrolling. Instead have methods
 *	G/SetUseBitmapScrollingOptimization ()
 *	
 *	Revision 2.52  1999/12/21 03:32:27  lewis
 *	update DOCCOMMENTS
 *	
 *	Revision 2.51  1999/12/19 16:28:01  lewis
 *	Lose qSupportLed23CompatAPI workaround for SPR#0668- cuz will probably do more harm
 *	that good - making things compile - but not really making them WORK right (since override wont be caught).
 *	
 *	Revision 2.50  1999/12/19 16:26:45  lewis
 *	SPR#0668- Added args to virtual TextInteractor::OnTypedNormalCharacter ()
 *	
 *	Revision 2.49  1999/12/15 01:05:42  lewis
 *	Slight speed tweek in TextInteractor::UpdateIfNoKeysPending () - SPR#0659
 *	
 *	Revision 2.48  1999/12/14 18:05:46  lewis
 *	qXWindows support
 *	
 *	Revision 2.47  1999/12/09 03:23:42  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.46  1999/11/13 16:32:20  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.45  1999/07/13 22:39:15  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds
 *	that were only needed for these old compilers
 *	
 *	Revision 2.44  1999/07/02 14:46:10  lewis
 *	spr#0604- Lose StandardStyledTextInteractor::InteractiveReplace_ and replace with new
 *	StandardStyledTextInteractor::InteractiveReplaceEarlyPostReplaceHook () which overrides
 *	new TextInteractor::InteractiveReplaceEarlyPostReplaceHook(). Also - had to add revised
 *	hack fEmptySelectionStyleSuppressMode to StandardStyledTextInteractor to make this fix work
 *	
 *	Revision 2.43  1999/05/03 22:05:03  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.42  1999/04/15 22:16:10  lewis
 *	add extra arg to PreInteractiveUndoHelper and tweeks to OptionallyExpand...SmartCut...,
 *	to fix spr#0570
 *	
 *	Revision 2.41  1999/03/29 21:14:07  lewis
 *	fix spr#0567- smart cut and paste now works a bit more senisbly, is much better
 *	commented/documented how it works, and should NOT do bad things to strings of Japanese
 *	characters anylonger (untested - but I hope I got this check right)
 *	
 *	Revision 2.40  1999/03/25 22:28:34  lewis
 *	Add extra overload of PostInteractiveUndoHelper and use that in first, and make args
 *	reference args so they can handle NULL-ing out pointer easier. And for better code
 *	sharing when I need to make the after REP another way
 *	
 *	Revision 2.39  1999/03/24 22:58:41  lewis
 *	make pre/post undable change methods public instead of protected so can be used from
 *	a templated helper class- as part of spr#0534 - not sure a good idea, but I could see no better way
 *	
 *	Revision 2.38  1998/10/30 14:17:54  lewis
 *	Use vector <> instead of Led_Array<> and new MSVC60 warning support
 *	
 *	Revision 2.37  1998/07/24  01:03:46  lewis
 *	ExternalizeFlavor_TEXT () virtual (so I can fix UNICODE/Win95 bugs).
 *
 *	Revision 2.36  1998/06/03  01:30:58  lewis
 *	_TEXT versions of a couple GetFlaver... guys for qWorkAroundWin95BrokenUNICODESupport.
 *	Moved TextInteractor::ReadWriteMemBufferPackage etc declarations textually outside
 *	main struct definition (forward declared/
 *	still scoped). Just for readability
 *
 *	Revision 2.35  1998/05/05  00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.34  1998/04/25  01:26:19  lewis
 *	Docs
 *
 *	Revision 2.33  1998/04/08  01:41:41  lewis
 *	*** empty log message ***
 *
 *	Revision 2.32  1998/03/04  20:16:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.31  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.30  1997/09/29  14:52:15  lewis
 *	Lose qLedFirstIndex and qSupportLed21CompatAPI.
 *	Redo virtual/nonvirtual UpdateMode/non-updatemode style overriding for a bunch of routines:
 *		SetTopRowInWindow/ScrollByIfRoom/ScrollSoShowing/SetDefaultFont/SetWindowRect/... etc..
 *
 *	Revision 2.29  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.28  1997/07/23  23:06:34  lewis
 *	docs changes
 *
 *	Revision 2.27  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.26  1997/07/12  20:08:56  lewis
 *	Renamed TextInteractor_ to TextInteractor.
 *	AutoDoc support.
 *	Use string class instead of char* for command names.
 *	more...
 *
 *	Revision 2.25  1997/06/28  17:13:45  lewis
 *	Add InteractiveUndoHelperMakeTextRep () method. Make sDoingUpdateModeReplaceOn into field fDoingUpdateModeReplaceOn.
 *
 *	Revision 2.24  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.23  1997/06/18  02:37:27  lewis
 *	Added WhileSimpleMouseTracking helper.
 *
 *	Revision 2.22  1997/03/22  13:44:35  lewis
 *	Moved qDoVerticalScrollbitsHack to LedConfig, and renamed qUseGDIScrollbitsForScrolling.
 *	WhileTrackingConstrainSelection () new support.
 *	new LooksLikeSmartPastableText () routine.
 *
 *	Revision 2.21  1997/03/04  20:07:29  lewis
 *	SmartCutAndPasteMode
 *
 *	Revision 2.20  1997/01/20  05:20:51  lewis
 *	One major change. Added eDefaultUpdate to UpdateMode enum. And added Get/SetDefaultUpdateMode()
 *	which defines what this enum maps to.
 *	Added further support for this, like a resource-allocation-style setter class (TemporarilySetUpdateMode)
 *	etc. Used the new UpdateMode in most places where eDelayedUpdate was used before (as a default).
 *
 *	Revision 2.19  1997/01/10  02:51:07  lewis
 *	AboutToUpdateText () returns void, DidUpdate throw specifier.
 *	New TextInteractor_::PreScrollInfo and related support routines: Pre/Post scroll helpers.
 *	qSupportLed21CompatAPI support for obsolete SetMarkerPositionOfStartOfWindowIfRoom.
 *	Moved (was ...StyledWrapped??xxxTextIntercotpr::ScrollBy that had scrollbits stuff.
 *	Now here, and controlled by qDoVerticalScrollbitsHack (not qScrollbitsHack).
 *	SetHScrollPos () stupport. Other scrolling related cleanups.
 *
 *	Revision 2.18  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.17  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.16  1996/09/30  14:21:03  lewis
 *	Added support for qSupportEnterIdleCallback (for speed hacks).
 *	Added support for qSupportWholeWindowInvalidOptimization (speed hack).
 *	Redid scrolling code/AdjustBounds-->InvalidateScrollbarparams...
 *
 *	Revision 2.15  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.14  1996/06/01  02:05:25  lewis
 *	Cleanups plus fLeftSideOfSelectionInteresting (was fLastCursoredLeft).
 *
 *	Revision 2.13  1996/05/23  19:28:49  lewis
 *	Use Led_IncrementalFontSpecification isntead of old FontSpecication.
 *	Add new InternalizeFlavor_FILE/InternalizeFlavor_FILEData() routines.
 *
 *	Revision 2.12  1996/05/14  20:22:54  lewis
 *	Redid PostReplace/PreReplace() stuff to use new
 *	PreReplaceInfo
 *	And made the code much more aggressive in its optimiations.
 *	more...
 *
 *	Revision 2.11  1996/04/18  15:17:22  lewis
 *	Lots of reworking of the undo support, and drag/drop and flavor package stuff
 *	so they all work together, and cross-platform.
 *
 *	Revision 2.10  1996/03/05  18:24:36  lewis
 *	Revamped Copy/Paste support around concept of internalizing
 *	and externalizing 'flavor packages'. Then used this also to support
 *	drag/drop.
 *
 *	Revision 2.9  1996/03/04  07:42:59  lewis
 *	Major re-org of Copy/Paste command support.
 *	New ProcessSimpleClick() hook/helper so we can handle click in embeddings
 *	portably acorss all the OSs.
 *
 *	Revision 2.8  1996/02/26  18:44:34  lewis
 *	 TextInteracter_ --> TextInteractor_.
 *	Breakup OnCopyCommand () into parts, so more easy modular overriding of
 *	various parts (and start at new flavor stuff for copy).
 *	SetMarkerPositionOfStartOfWindow
 *	IfRoom() override / wrapper to add UpdateMode.
 *	Override AboutToUpdateText and fix DidUpdate and add tmpRect to allow
 *	updates from style changes to only redraw affected text.
 *
 *	Revision 2.7  1996/02/05  04:13:14  lewis
 *	typedef TextImager     inherited
 *
 *	Revision 2.6  1996/01/22  05:11:56  lewis
 *	Add new CommandHandler support, and override DidUpdateText().
 *
 *	Revision 2.5  1995/12/13  05:54:43  lewis
 *	Change caret API here for Mac/OpenDoc to be more like the
 *	windows one. Just a InvalidateCaretState() method. Rest done
 *	in subclasses.
 *
 *	Revision 2.4  1995/11/25  00:21:42  lewis
 *	Revised meaning of InteractiveReplace(), and now added _ method.
 *	It updates cursor pos.
 *
 *	Revision 2.3  1995/11/02  22:21:15  lewis
 *	Added OnPasteCommand_ helper function
 *
 *	Revision 2.2  1995/10/09  22:21:49  lewis
 *	Now Replace is not pure virutal, but implemented here, and it calls
 *	pre/post replace stuff directly.
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.17  1995/06/08  05:10:34  lewis
 *	Code cleanups
 *
 *	Revision 1.16  1995/06/02  06:17:58  lewis
 *	Add utility TextInteractor_::UpdateIfNoKeysPending () to help in
 *	SPR 307.
 *
 *	Revision 1.15  1995/05/31  09:36:29  lewis
 *	SPR 303 - add CalculateCaretRect ().
 *
 *	Revision 1.14  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.13  1995/05/21  17:03:13  lewis
 *	Lose InsertAfter/DeleteAfter. Have people just use Replace() instead.
 *	SPR 0270.
 *
 *	Revision 1.12  1995/05/20  04:49:05  lewis
 *	Added UpdateMode for SetWindowRect() (did override and created new guy)
 *
 *	Added virtual SetTopRowInWindowByMarkerPosition. (see SPR0260).
 *
 *	Revision 1.11  1995/05/18  08:07:24  lewis
 *	Lose old CW5.* bug workarounds (SPR#0249).
 *
 *	Revision 1.10  1995/05/16  06:38:22  lewis
 *	Added InteractiveReplace () virtual.
 *
 *	Revision 1.9  1995/05/09  23:24:37  lewis
 *	Lose unused withWhat parameter to Pre/PostReplace - SPR 0240.
 *	Added OnTypedNormalCharacter/HandledMByteCharTyping/QueryInputKeyStrokesPending to
 *	vector more code to one spot from Led_TCL/Led_MFC - cuz I was going to have
 *	to override thier typechar stuff in LVEJ - and was needless spread
 *	across routines in differnt classes. LedSPR#0242.
 *
 *	Revision 1.8  1995/05/08  03:11:01  lewis
 *	ScrollByIfRoom/ScrollSoShowing now return nothing - SPR 0236.
 *
 *	Revision 1.7  1995/05/06  19:35:38  lewis
 *	Use Led_tChar insetad of char for UNICODE support - SPR 0232.
 *
 *	Revision 1.6  1995/04/16  19:25:07  lewis
 *	Got rid of SimpleTextInteractor (SPR# 205).
 *	Added Get/Set CaretShownAfterPos and instance variable for
 *	SPR# 204.
 *
 *	Revision 1.5  1995/03/29  21:00:50  lewis
 *	Renamed UpdateCaretState() to InvalidateCaretState ().
 *
 *	Revision 1.4  1995/03/17  02:59:45  lewis
 *	Added DelaySomeForScrollBarClick () helper function.
 *
 *	Revision 1.3  1995/03/15  01:03:59  lewis
 *	Made DoSingleCharCursorEdit virtual to make overriding cursor behavior
 *	in subclasses easier.
 *
 *	Revision 1.2  1995/03/02  05:45:35  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:22  lewis
 *	Initial revision
 *
 *
 *
 */

#include	"Command.h"
#include	"FlavorPackage.h"
#include	"SpellCheckEngine.h"
#include	"TextImager.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif




#if		qSilenceAnnoyingCompilerWarnings && __MWERKS__
	// Shut-off SetWindowRect() warning
	#pragma	warn_hidevirtual	off
#endif
#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4786)		//qQuiteAnnoyingDebugSymbolTruncationWarnings
#endif


#if		defined(qSupportEnterIdleCallback)
	#pragma warn "OBSOLETE - use new IdleManager class (as of Led 3.1a4)"
#endif


/*
@CONFIGVAR:		qUseGDIScrollbitsForScrolling
@DESCRIPTION:	<p>OBSOLETE (as of Led 3.0d3, 991228). Instead - use @'TextInteractor::SetUseBitmapScrollingOptimization'</p>
 */
#if		defined (qUseGDIScrollbitsForScrolling)
#pragma warn "OBSOLETE - just call TextInteractor::SetUseBitmapScrollingOptimization"
#endif







#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
	#pragma push
	#pragma warn -8022
#endif




// Private Led Macro utility to define command numbers
#define	RoundUpToNearest_CMDNUMs_MACRO(x,n)	(((x + 1) & ~(n-1)) + n)




/*
@CLASS:			TextInteractor
@BASES:			virtual @'TextImager'
@DESCRIPTION:	<p>TextInteractors are special @'TextImager's which respond to events
			(such as keyclicks), handle mouse tracking, 
			and are designed to work with some sort of windowing system. They have a
			concept of Update events, and having their content data out of sync with
			that which is displayed in the window (inval/validate region). They also
			provide support for things like a selection, and cursoring through the
			selection, and word selection, etc.</p>

				<p>Note that TextInteractors are still abstract classes, and know nothing
			about particular windowing systems, or class libraries. They do what can
			be done genericly, without having yet made a choice about these things.
			TextInteractor mainly serves to collect common code/functionality which
			can be shared accross (for example) MFC/OpenDoc/TCL/PowerPlant/PowerPlant, etc.
			See @'Led_PPView', @'Led_MacOS_Helper<BASE_INTERACTOR>', @'Led_MFC', @'Led_Win32_Helper<BASE_INTERACTOR>'
			@'Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>', etc for more information on class-library-specific
			integration.</p>
*/
class	TextInteractor : public virtual TextImager {
	protected:
		TextInteractor ();
	public:
		virtual ~TextInteractor ();


	/*
	 *	By default we have none. If you set one, it will be notified/used for undo
	 *	support. It is callers responsability to free the command handler, and must be
	 *	done BEFORE we are destroyed, and we must be notified (via SetCommandHandler(NULL))
	 *	BEFORE CommandHandler is destroyed.
	 */
	public:
		nonvirtual	CommandHandler*	GetCommandHandler () const;
		virtual		void			SetCommandHandler (CommandHandler* commandHandler);
	private:
		CommandHandler*	fCommandHandler;

	public:
		nonvirtual	void	BreakInGroupedCommands ();
		nonvirtual	void	BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName);

	public:
		class	SupressCommandBreaksContext;
	private:
		bool	fSupressCommandBreaksContext;

	public:
		struct	CommandNames;

	// This class builds commands with command names. The UI may wish to change these
	// names (eg. to customize for particular languages, etc)
	// Just patch these strings here, and commands will be created with these names.
	// (These names appear in text of undo menu item)
	public:
		static	const CommandNames&	GetCommandNames ();
		static	void				SetCommandNames (const CommandNames& cmdNames);
		static	CommandNames		MakeDefaultCommandNames ();
	private:
		static	CommandNames	sCommandNames;


	public:
		typedef	int	CommandNumber;

	public:
		#if		qCannotStaticlyInitializeConstantsDataMembersInClassDeclaration
			enum { kNoCommand_CmdID			=	0 };
		#else
			static	const CommandNumber	kNoCommand_CmdID			=	0;
		#endif

	public:
		enum {
			kTextInteractorCommand_First		=	0x100,
			
			kUndo_CmdID							=	kTextInteractorCommand_First,
			kRedo_CmdID,
			kSelectAll_CmdID,
			kCut_CmdID,
			kCopy_CmdID,
			kPaste_CmdID,
			kClear_CmdID,

			kSelectWord_CmdID,
			kSelectTextRow_CmdID,
			kSelectParagraph_CmdID,

			kFind_CmdID,
			kFindAgain_CmdID,
			kEnterFindString_CmdID,
			kReplace_CmdID,
			kReplaceAgain_CmdID,

			kSpellCheck_CmdID,

			kTextInteractorCommand_Last			=	kTextInteractorCommand_First + 0xff
		};

	public:
		class    CommandUpdater;

	public:
		class    DialogSupport;
		static	DialogSupport&	GetDialogSupport ();
		static	void			SetDialogSupport (DialogSupport* ds);
	private:
		static	DialogSupport*	sDialogSupport;


	public:
		virtual	bool	OnUpdateCommand (CommandUpdater* enabler);
		virtual	bool	OnPerformCommand (CommandNumber commandNumber);

	public:
		virtual	void	OnUpdateCutCopyClearCommand (CommandUpdater* enabler);
		virtual	void	OnUpdatePasteCommand (CommandUpdater* enabler);
		virtual	void	OnUpdateUndoRedoCommand (CommandUpdater* enabler);
	
	public:
		virtual	void	OnUpdateSelectTextCommand (CommandUpdater* enabler);
		virtual	void	OnPerformSelectTextCommand (CommandNumber commandNumber);

	// Find related commands
	public:
		class	SearchParameters;
		class	ReplaceParameters;
	public:
		virtual	void	OnFindCommand ();
		virtual	void	OnFindAgainCommand ();
		virtual	void	OnEnterFindString ();
		virtual	void	OnReplaceCommand ();
		virtual	void	OnReplaceAgainCommand ();
		virtual	void	OnDoReplaceCommand (const SearchParameters& searchFor, const Led_tString& replaceWith);
		virtual	void	OnDoReplaceAllCommand (const SearchParameters& searchFor, const Led_tString& replaceWith);
		virtual	void	OnDoReplaceAllInSelectionCommand (const SearchParameters& searchFor, const Led_tString& replaceWith);
		virtual	void	OnUpdateFindCommands (CommandUpdater* enabler);	
	private:
		static	SearchParameters	sSearchParameters;
	public:
		virtual	SearchParameters	GetSearchParameters () const;
		virtual	void				SetSearchParameters (const SearchParameters& sp);

	private:
		static	ReplaceParameters	sReplaceParameters;
	public:
		virtual	ReplaceParameters	GetReplaceParameters () const;
		virtual	void				SetReplaceParameters (const ReplaceParameters& rp);

	protected:
		virtual	vector<Led_tString>	MergeRecentFindStrings (const Led_tString& s, const vector<Led_tString>& oldRecents);

	public:
		virtual	void	OnSpellCheckCommand ();
		virtual	void	OnUpdateSpellCheckCommand (CommandUpdater* enabler);	


	public:
		nonvirtual	SpellCheckEngine*	GetSpellCheckEngine () const;
		nonvirtual	void				SetSpellCheckEngine (SpellCheckEngine* spellCheckEngine);
	private:
		SpellCheckEngine*	fSpellCheckEngine;

	protected:
		class	UndoableContextHelper;

		friend	class	UndoableContextHelper;

	public:
	/*
	@CLASS:			TextInteractor::UpdateMode
	@DESCRIPTION:	<p>Support for controlling how/when the window is updated. This is a key difference between
		a @'TextImager' and a @'TextInteractor'. A @'TextInteractor' introduces the concept of a real GUI
		window that can get out of data, and needs to be updated. These updateModes control
		the basic updating/timing strategies for redrawing portions of the textimager.</p>
	*/
		enum	UpdateMode { eDelayedUpdate, eImmediateUpdate, eNoUpdate, eDefaultUpdate };


	/*
	 #if 0
	 *	Sometimes its convenient to globally (perhaps modally is a better word) disable
	 *	updates to a TextImager. This is convenient in code-reuse, occasionally (where
	 *	you want to re-use a version of a routine with an UpdateMode, but you will handle
	 *	the updating yourself, and you cannot call the routine with the Updatemode directly).
	 *
	 *	To avoid the possability (likelihood) of accidentally leaving updates disabled, you cannot set this
	 *	directly, but only via a stack-based helper class. Just instantiate a TemporarilySetUpdateMode
	 *	to disable updates.
	 #endif
	 */
	public:
		nonvirtual	UpdateMode	GetDefaultUpdateMode () const;
		nonvirtual	UpdateMode	RealUpdateMode (UpdateMode updateMode) const;
	protected:
		virtual	void	SetDefaultUpdateMode (UpdateMode defaultUpdateMode);
	private:
		UpdateMode	fDefaultUpdateMode;
	public:
		class	TemporarilySetUpdateMode;
		friend	class	TemporarilySetUpdateMode;

	public:
		nonvirtual	void	Refresh (UpdateMode updateMode = eDefaultUpdate) const;
		nonvirtual	void	RefreshWindowRect (const Led_Rect& windowRectArea, UpdateMode updateMode = eDefaultUpdate) const;
		nonvirtual	void	Refresh (size_t from, size_t to, UpdateMode updateMode = eDefaultUpdate) const;
		nonvirtual	void	Refresh (const Marker* range, UpdateMode updateMode = eDefaultUpdate) const;

	protected:
		#if		qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
			public:
		#endif
		/*
		@METHOD:		TextInteractor::RefreshWindowRect_
		@DESCRIPTION:	<p>pure virtual method called by @'TextInteractor::RefreshWindowRect_'. Generally
					override in SDK-specific wrapper subclass.</p>
						<p>The given 'windowRectArea' is given in the same coordinates as the window rect
					specified by @'TextImager::SetWindowRect': it is not relative to that rectangle.</p>
		*/
		virtual		void	RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const	=	0;


	/*
	 *	When doing "Cut" (and related commands like drag), remove extra surrounding whitespace.
	 *	And when doing "Paste" (and related commands like drop), add in any appropriate surrounding whitespace.
	 */
	public:
		nonvirtual	bool	GetSmartCutAndPasteMode () const;
		nonvirtual	void	SetSmartCutAndPasteMode (bool smartCutAndPasteMode);
	private:
		bool	fSmartCutAndPasteMode;

	protected:
		struct	SmartCNPInfo {
			SmartCNPInfo ();

			bool	fWordBreakAtSelStart;
			bool	fWordBreakAtSelEnd;
		};
	protected:
		nonvirtual	bool	LooksLikeSmartPastableText (const Led_tChar* text, size_t nTextTChars, SmartCNPInfo* smartCNPInfo) const;
		nonvirtual	void	OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (size_t selStart, const SmartCNPInfo& smartCNPInfo);
		nonvirtual	void	OptionallyExpandSelectionForSmartCutAndPasteModeDeletes (size_t* selStart, size_t* selEnd);


	public:
		override	void	SetSelectionShown (bool shown);
		virtual		void	SetSelectionShown (bool shown, UpdateMode updateMode);

	public:
		nonvirtual	void	ScrollToSelection (UpdateMode updateMode = eDefaultUpdate, bool forceShowSelectionEndpoint = false);


	protected:
		override	void	HookLosingTextStore ();
		nonvirtual	void	HookLosingTextStore_ ();
		override	void	HookGainedNewTextStore ();
		nonvirtual	void	HookGainedNewTextStore_ ();


	/*
	 *	Mouse clicks.
	 */
	protected:
		virtual	bool	ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor);

	public:
		nonvirtual	unsigned	GetCurClickCount () const;
		nonvirtual	void		SetCurClickCount (unsigned curClickCount, float lastClickAt);
	protected:
		nonvirtual	void		IncrementCurClickCount (float lastClickAt);
		nonvirtual	void		UpdateClickCount (float clickAtTime, const Led_Point& clickAtLocation);
		virtual		bool		ClickTimesAreCloseForDoubleClick (float thisClick);
		virtual		bool		PointsAreCloseForDoubleClick (const Led_Point& p);
	private:
		unsigned	fClickCount;
		float		fLastClickedAt;
		Led_Point	fLastMouseDownAt;


	protected:
		virtual	void	WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor);

	protected:
		virtual	void	WhileTrackingConstrainSelection (size_t* selStart, size_t* selEnd);
		virtual	void	WhileTrackingConstrainSelection_ForWholeWords (size_t* selStart, size_t* selEnd);
		virtual	void	WhileTrackingConstrainSelection_ForWholeRows (size_t* selStart, size_t* selEnd);

	public:
		virtual		size_t	GetCharAtClickLocation (const Led_Point& where) const;


	public:
		nonvirtual	bool	IsWholeWindowInvalid () const;
		
		override	void	Draw (const Led_Rect& subsetToDraw, bool printing);
	protected:
		nonvirtual	void	NoteWholeWindowIsInvalid ();		// be very careful calling this - be sure its really true!
		nonvirtual	void	NoteWindowPartiallyUpdated ();
	private:
		bool	fWholeWindowInvalid;


	public:
		/*
		@CLASS:			TextInteractor::CannotUpdateNow
		@DESCRIPTION:	<p>Can be thrown indirectly by @'TextInteractor::Update' or @'TextInteractor::UpdateWindowRect'
					calls if updates are for some reason (perhaps temporarily) unavailable. Can usually be safely
					ignored.</p>
		*/
		class	CannotUpdateNow {};
		nonvirtual	void	Update (bool ignoreCannotUpdateNowErrors = true) const;
		nonvirtual	void	UpdateWindowRect (const Led_Rect& windowRectArea, bool ignoreCannotUpdateNowErrors = true) const;

	protected:
		#if		qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
			public:
		#endif
		// These are to be overriden in the actual class library mixin to hook into its
		// update mechanism.
		virtual		void	UpdateWindowRect_ (const Led_Rect& windowRectArea) const							=	0;


#if		qSupportLed30CompatAPI
	public:
		nonvirtual	void	Refresh (const Led_Rect& area, UpdateMode updateMode = eDefaultUpdate) const
			{
				RefreshWindowRect (GetWindowRect (), updateMode);
			}
		nonvirtual	void	Update (const Led_Rect& area) const
			{
				UpdateWindowRect (GetWindowRect ());
			}
#endif

	public:
		nonvirtual	bool	GetUseSecondaryHilight () const;
		nonvirtual	void	SetUseSecondaryHilight (bool useSecondaryHilight);
	private:
		bool	fUseSecondaryHilight;

	protected:
		virtual	void	DrawBefore (const Led_Rect& subsetToDraw, bool printing);
		virtual	void	DrawAfter (const Led_Rect& subsetToDraw, bool printing);

	/*
	 *	Overrides to add optional UpdateMode argument.
	 */
	public:
		override	void		SetSelection (size_t start, size_t end);
		nonvirtual	void		SetSelection (size_t start, size_t end, UpdateMode updateMode);
		nonvirtual	void		SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode = eDefaultUpdate);
		nonvirtual	void		ScrollByIfRoom (long downBy, UpdateMode updateMode = eDefaultUpdate);
		nonvirtual	void		ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos = 0, UpdateMode updateMode = eDefaultUpdate);
		nonvirtual	void		SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont, UpdateMode updateMode = eDefaultUpdate);
		nonvirtual	void		SetWindowRect (const Led_Rect& windowRect, UpdateMode updateMode = eDefaultUpdate);
		override	void		SetHScrollPos (Led_Coordinate hScrollPos);
		nonvirtual	void		SetHScrollPos (Led_Coordinate hScrollPos, UpdateMode updateMode);

	/*
	 *	Same as SetTopRowInWindow, but uses a marker position instead of a row#. This can be MUCH
	 *	more efficient, since it doesn't force word-wrapping in subclasses which support that.
	 */
	public:
		virtual		void	SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode = eDefaultUpdate)			=	0;


	public:
		nonvirtual	bool	GetUseBitmapScrollingOptimization () const;
		nonvirtual	void	SetUseBitmapScrollingOptimization (bool useBitmapScrollingOptimization);
	private:
		bool	fUseBitmapScrollingOptimization;


	public:
		nonvirtual	bool	GetSupressTypedControlCharacters () const;
		nonvirtual	void	SetSupressTypedControlCharacters (bool supressTypedControlCharacters);
	private:
		bool	fSupressTypedControlCharacters;


	public:
		/*
		@CLASS:			TextInteractor::InteractiveUpdadeMode
		@DESCRIPTION:	<p>Update modes used with @'TextInteractor::InteractiveModeUpdater' and
					@'TextInteractor::CheckIfCurrentUpdateIsInteractive' to see if a given update is
					interactive. This can be used to distinguish user text changes (which might be disallowed)
					from code-based text changes (which a users logic might want to allow).</p>
		*/
		enum	InteractiveUpdadeMode {
			eInteractiveUpdateMode,
			eNonInteractiveUpdateMode,
			eIndeterminateInteractiveUpdateMode
		};
		nonvirtual	InteractiveUpdadeMode	GetInteractiveUpdateMode () const;
		nonvirtual	void					SetInteractiveUpdateMode (InteractiveUpdadeMode interactive);
		nonvirtual	bool					CheckIfCurrentUpdateIsInteractive () const;

	private:
		InteractiveUpdadeMode	fInteractiveUpdadeMode;

	public:
		class	InteractiveModeUpdater;


	protected:
		class	PreScrollInfo;
		nonvirtual	void	PreScrollHelper (UpdateMode updateMode, PreScrollInfo* preScrollInfo);
		nonvirtual	void	PostScrollHelper (PreScrollInfo preScrollInfo);

	public:
		virtual		void	Replace (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode = eDefaultUpdate);

	protected:
		class	PreReplaceInfo;
		virtual	void	PreReplace (size_t from, size_t to, size_t withWhatCharCount, UpdateMode updateMode, PreReplaceInfo* preReplaceInfo);
		virtual	void	PostReplace (PreReplaceInfo& preReplaceInfo);
		virtual	void	AbortReplace (PreReplaceInfo& preReplaceInfo);
		virtual	void	ExpandedFromAndToInPostReplace (size_t from, size_t newTo,
									size_t stableTypingRegionStart, size_t stableTypingRegionEnd,
									size_t startPositionOfRowWhereReplaceBegins, size_t	startPositionOfRowAfterReplaceEnds,
									size_t* expandedFrom, size_t* expandedTo
								);

	protected:
		// NB: PreReplaceInfo declared here instead of outside TextInteractor class cuz an instance is
		// a data member of TextInteractor.
		class	PreReplaceInfo {
			public:
				PreReplaceInfo ();
				~PreReplaceInfo ();	// must get destroyed before owned fTextInteractor, and must either call
									// PostReplace or AbortReplace or ~PreReplaceInfo before ever losing
									// fTextInteractor's TextStore...

			public:
				nonvirtual	UpdateMode	GetUpdateMode () const;
				nonvirtual	size_t		GetFrom () const;
				nonvirtual	size_t		GetTo () const;
			private:
				TextInteractor*	fTextInteractor;
				UpdateMode		fUpdateMode;
				size_t			fFrom;
				size_t			fTo;
				size_t			fWithWhatCharCount;
				Marker			fBoundingUpdateMarker;
				Led_Distance	fBoundingUpdateHeight;
				Led_Distance	fStableTypingRegionHeight;

			private:
			#if		qFriendDeclsCannotReferToEnclosingClassInNestedClassDeclBug
				friend	class	TextInteractor;
			#else
				friend	void	TextInteractor::PreReplace (size_t from, size_t to, size_t withWhatCharCount, UpdateMode updateMode, PreReplaceInfo* preReplaceInfo);
				friend	void	TextInteractor::PostReplace (PreReplaceInfo& preReplaceInfo);
				friend	void	TextInteractor::AbortReplace (PreReplaceInfo& preReplaceInfo);
			#endif
		};



	/*
	 *	Simply calls 'Replace ()'. Only purpose is that it is sometimes convenient to override in just
	 *	one place to filter user typing/text updates. And not have to worry about if the particular
	 *	bottleneck routine also gets used for 'programming-sourced' text updates. So all typing, and
	 *	user-insertions from things like paste (things you might want to validate) go through this
	 *	procedure. And you neededn't override 'Replace' itself, unless your refinements are intended to
	 *	apply even to programming-based text updates. NB: text updates stemming from apple-events/OLE
	 *	automation are considered 'user-updates' - and so vector through here. This is because they 
	 *	would likely want to be validated.
** DOCS INVALID - MEANING OF InteractiveReplace() REVISED
	 */
	public:
		virtual		void	InteractiveReplace (const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode = eDefaultUpdate);

	protected:
		nonvirtual	void	InteractiveReplace_ (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount, bool updateCursorPosition = true, bool validateTextForCharsetConformance = true, UpdateMode updateMode = eDefaultUpdate);

	protected:
		virtual		bool	InteractiveReplaceEarlyPostReplaceHook (size_t withWhatCharCount);

	// utilities to help with undo
	public:
		virtual	void	PreInteractiveUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd);
		virtual	void	PostInteractiveUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t startOfInsert, size_t endOfInsert, const Led_SDK_String& cmdName);
		virtual	void	PostInteractiveSimpleCharInsertUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t startOfInsert, size_t endOfInsert, const Led_SDK_String& cmdName);
		virtual	void	PostInteractiveUndoPostHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, InteractiveReplaceCommand::SavedTextRep** afterRep, size_t startOfInsert, const Led_SDK_String& cmdName);
	protected:
		virtual	InteractiveReplaceCommand::SavedTextRep*		InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd);

	protected:
		nonvirtual	void	OnUndoCommand ();
		nonvirtual	void	OnRedoCommand ();


	// Utility routine for lots of editing cases - like backspace, cursor arrows, etc...
	public:
		enum	CursorMovementAction	{ eCursorDestroying, eCursorMoving, eCursorExtendingSelection };
	protected:
		virtual		void	DoSingleCharCursorEdit (CursorMovementDirection direction, CursorMovementUnit movementUnit, CursorMovementAction action,
													UpdateMode updateMode = eDefaultUpdate, bool scrollToSelection = true
											);


	public:
		override	void	AboutToUpdateText (const UpdateInfo& updateInfo);
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();
	private:
		PreReplaceInfo 	fTmpPreReplaceInfo;
		TextInteractor*	fDoingUpdateModeReplaceOn;


	// Clipboard commands
	public:
		virtual	void	OnCutCommand ();
		virtual	void	OnCopyCommand ();
		virtual	void	OnPasteCommand ();
		virtual	void	OnClearCommand ();

	// helper function for implementing OnCopyCommand
	protected:
		#if		qAccessCheckAcrossInstancesSometimesWrong
		public:
		#endif
		virtual		bool	OnCopyCommand_Before ();
		virtual		void	OnCopyCommand_After ();
		virtual		void	OnCopyCommand_CopyFlavors ();

	// helper function, to share code between this and subclasses which need different paste behavior
	protected:
		#if		qAccessCheckAcrossInstancesSometimesWrong
		public:
		#endif
		virtual		bool	ShouldEnablePasteCommand () const;
		virtual		bool	OnPasteCommand_Before ();
		virtual		void	OnPasteCommand_After ();
		virtual		void	OnPasteCommand_PasteBestFlavor ();
		virtual		void	OnPasteCommand_PasteFlavor_Specific (Led_ClipFormat format);

	protected:
		nonvirtual	bool	PasteLooksLikeSmartCNP (SmartCNPInfo* scnpInfo) const;

	public:
		virtual	void	OnSelectAllCommand ();

	// Drag & Drop support
	protected:
		virtual	bool	CanAcceptFlavor (Led_ClipFormat clipFormat) const;


	// Internalizing
	public:
		virtual		void	InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage,
													bool updateCursorPosition = true, bool autoScroll = true, UpdateMode updateMode = eDefaultUpdate
												);
		virtual		void	InternalizeFlavor_Specific (ReaderFlavorPackage& flavorPackage, Led_ClipFormat format,
													bool updateCursorPosition = true, bool autoScroll = true, UpdateMode updateMode = eDefaultUpdate
												);
	public:
		nonvirtual	Led_RefCntPtr<FlavorPackageInternalizer>	GetInternalizer () const;
		nonvirtual	void										SetInternalizer (const Led_RefCntPtr<FlavorPackageInternalizer>& i);
	private:
		Led_RefCntPtr<FlavorPackageInternalizer>	fInternalizer;

	protected:
		virtual	Led_RefCntPtr<FlavorPackageInternalizer>	MakeDefaultInternalizer ();
		virtual	void	HookInternalizerChanged ();


	// Externalizing
	public:
		virtual	void	ExternalizeFlavors (WriterFlavorPackage& flavorPackage);
		virtual	void	ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage);

	public:
		nonvirtual	Led_RefCntPtr<FlavorPackageExternalizer>	GetExternalizer () const;
		nonvirtual	void										SetExternalizer (const Led_RefCntPtr<FlavorPackageExternalizer>& e);
	private:
		Led_RefCntPtr<FlavorPackageExternalizer>	fExternalizer;

	protected:
		virtual	Led_RefCntPtr<FlavorPackageExternalizer>	MakeDefaultExternalizer ();
		virtual	void	HookExternalizerChanged ();


	// Notifications
	public:
		class	BadUserInput;
		virtual	void	OnBadUserInput ();			// called - for example - when user types bad SJIS - default is to sysbeep()


	// Scrollbar support
	public:
		enum	VHSelect { v, h, eFirst=v, eLast=h };
		enum	ScrollBarType	{ eScrollBarNever, eScrollBarAsNeeded, eScrollBarAlways };
		nonvirtual	ScrollBarType	GetScrollBarType (VHSelect vh) const;
		virtual		void			SetScrollBarType (VHSelect vh, ScrollBarType scrollBarType);
	protected:
		nonvirtual	void			SetScrollBarType_ (VHSelect vh, ScrollBarType scrollBarType);
	private:
		ScrollBarType	fScrollBarType[2];


	/*
	 * Fancy support for delayed scrollbar updatating...
	 */
	public:
		override	void	InvalidateScrollBarParameters ();
	protected:
		nonvirtual	void	InvalidateScrollBarParameters_ ();
		virtual		void	UpdateScrollBars ();
		nonvirtual	void	UpdateScrollBars_ ();	// must be called cuz marks sbar parameters valid
	private:
		bool	fScrollBarParamsValid;



	/*
	 *	Helper for implementing scrolling.
	 */
	public:
		virtual		bool	DelaySomeForScrollBarClick ();
	private:
		float	fLastScrolledAt;


	public:
		nonvirtual	bool	GetCaretShown () const;
		nonvirtual	void	SetCaretShown (bool shown);
	private:
		bool		fCaretShown;	// caret is active (though possibly not REALLY shown since drawn in
									// off state...

	public:
		virtual	bool	GetCaretShownSituation () const;

	public:
		nonvirtual	bool	GetCaretShownAfterPos () const;
		nonvirtual	void	SetCaretShownAfterPos (bool shownAfterPos);

	protected:
		virtual	Led_Rect	CalculateCaretRect () const;

	// Can be overriden to update other things like IME position
	protected:
		virtual		void	InvalidateCaretState ();


	protected:
		bool		fLeftSideOfSelectionInteresting;	// Occasionally (like in ScrollToSelection ()) it is helpful to know
														// which end of the selection the user is most interseted in seeing.
														// Also - we might - for some UI's take this into account in other
														// places like if we extend one side of a selection, or reduce the
														// other. Also used in cursoring for extended selection. Must be updated
														// in subclasses for things like mouse dragging...
	private:
		bool		fCaretShownAfterPos;	// If true show after, else before


	public:
		virtual		void	OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed);

#if		qMultiByteCharacters
	protected:
		char		fMultiByteInputCharBuf[2];	// the 'multi' in 'multibyte' better be 2!!

		virtual		bool	HandledMByteCharTyping (char theChar);
#endif


	/*
	 *	Query of the windowing system if there are any pending keystrokes.
	 */
	protected:
		#if		qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
			public:
		#endif
		virtual		bool	QueryInputKeyStrokesPending () const		=				0;

	protected:
		nonvirtual	void	UpdateIfNoKeysPending ();		// utility


#if		qMacOS || qXWindows
	public:
		static	float		GetTickCountBetweenBlinks ();
#endif

	private:
		friend	class	SupressCommandBreaksContext;
};


#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
	#pragma pop
#endif





/*
@CLASS:			TextInteractor::SupressCommandBreaksContext
@ACCESS:		public
@DESCRIPTION:	<p>When one of these is created on the stack for the @'TextInteractor' ti, then
			commands on that TextInteractor to @'TextInteractor::BreakInGroupedCommands ()' are
			ignored.</p>
*/
class	TextInteractor::SupressCommandBreaksContext {
	public:
		SupressCommandBreaksContext (TextInteractor& ti);
		~SupressCommandBreaksContext ();

	private:
		TextInteractor&	fTextInteractor;
		bool			fOldVal;
};




/*
@CLASS:			TextInteractor::CommandNames
@DESCRIPTION:	<p>Command names for each of the user-visible commands produced by the @'TextInteractor' module.
			This name is used used in the constructed Undo command name, as
			in, "Undo Paste". You can replace this name with whatever you like.
			You change this value with @'TextInteractor::SetCommandNames'.</p>
				<p> The point of this is to allow for different UI-language localizations,
			without having to change Led itself.</p>
				<p>See also @'TextInteractor::GetCommandNames'.</p>
				<p>See also @'WordProcessor::CommandNames'.</p>
*/
struct	TextInteractor::CommandNames {
	Led_SDK_String	fTypingCommandName;
	Led_SDK_String	fCutCommandName;
	Led_SDK_String	fClearCommandName;
	Led_SDK_String	fPasteCommandName;
	Led_SDK_String	fUndoFormatString;
	Led_SDK_String	fRedoFormatString;
	Led_SDK_String	fReplaceCommandName;
	Led_SDK_String	fReplaceAllCommandName;
	Led_SDK_String	fReplaceAllInSelectionCommandName;
};







/*
@CLASS:			TextInteractor::CommandUpdater
@DESCRIPTION:	<p></p>
*/
class    TextInteractor::CommandUpdater {
	public:
		typedef	TextInteractor::CommandNumber	CommandNumber;
	
	public:
		virtual    CommandNumber GetCmdID () const						=	0;
		virtual    bool          GetEnabled () const					=	0;
		virtual    void          SetEnabled (bool enabled)				=	0;
		virtual    void          SetChecked (bool checked)				=	0;
		virtual    void          SetText (const Led_SDK_Char* text)		=	0;
};









/*
@CLASS:			TextInteractor::DialogSupport
@DESCRIPTION:	<p></p>
*/
class	TextInteractor::DialogSupport {
	public:
		typedef	TextInteractor::CommandNumber	CommandNumber;

	public:
		virtual	void	DisplayFindDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK);
	public:
		enum	ReplaceButtonPressed { eReplaceButton_Cancel, eReplaceButton_Find, eReplaceButton_Replace, eReplaceButton_ReplaceAll, eReplaceButton_ReplaceAllInSelection };
		virtual	ReplaceButtonPressed	DisplayReplaceDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, Led_tString* replaceText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative);
	public:
		class	SpellCheckDialogCallback;
		virtual	void	DisplaySpellCheckDialog (SpellCheckDialogCallback& callback);
};




/*
@CLASS:			TextInteractor::DialogSupport::SpellCheckDialogCallback
@DESCRIPTION:	<p>This interface is called by the actual spellcheck dialog implematation back to the implementer
			of the real spellchecking functionality.</p>
*/
class	TextInteractor::DialogSupport::SpellCheckDialogCallback {
	public:
		struct	MisspellingInfo {
			Led_tString			fUndefinedWord;
			vector<Led_tString>	fSuggestions;
		};
	public:
		virtual	MisspellingInfo*	GetNextMisspelling ()				=	0;

	public:
		virtual	void	DoIgnore ()										=	0;
		virtual	void	DoIgnoreAll ()									=	0;
		virtual	void	DoChange (const Led_tString& changeTo)			=	0;
		virtual	void	DoChangeAll (const Led_tString& changeTo)		=	0;
		virtual	bool	AddToDictionaryEnabled () const					=	0;
		virtual	void	AddToDictionary (const Led_tString& newWord)	=	0;
		virtual	void	LookupOnWeb (const Led_tString& word)			=	0;
		virtual	bool	OptionsDialogEnabled () const					=	0;
		virtual	void	OptionsDialog ()								=	0;
};



/*
@CLASS:			TextInteractor::SearchParameters
@DESCRIPTION:	<p>
				</p>
*/
class	TextInteractor::SearchParameters : public TextStore::SearchParameters {
	private:
		typedef	TextStore::SearchParameters	inherited;

	public:
		SearchParameters ();

	public:
		vector<Led_tString>	fRecentFindStrings;
};


/*
@CLASS:			TextInteractor::ReplaceParameters
@DESCRIPTION:	<p>
				</p>
*/
class	TextInteractor::ReplaceParameters {
	public:
		ReplaceParameters ();

	public:
		Led_tString	fReplaceWith;
};





/*
@CLASS:			TextInteractor::UndoableContextHelper
@DESCRIPTION:	<p>Use this helper class in a context where you are creating/doing an undoable
			command. This class is a simpler wrapper/helper for @'TextInteractor::PreInteractiveUndoHelper'
			and @'TextInteractor::PostInteractiveUndoHelper'.</p>
				<p>A simple example usage would be:<br>
			<code><pre>
				UndoableContextHelper	undoContext (*this, GetCommandNames ().fClearCommandName, true);
					{
						//Actual command guts
						InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0);
					}
				undoContext.CommandComplete ();
			</pre></code>
				</p>
				<p>Note - the 'allowSmartCNPExpansion' should generally be true if the undoable action could
			have invoked smartcutandpaste space additions/removealls. Specifing true expands the region we capture to
			take this possability into account.
				</p>
*/
class	TextInteractor::UndoableContextHelper {
	public:
		typedef	InteractiveReplaceCommand::SavedTextRep	SavedTextRep;
	public:
		UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, bool allowSmartCNPExpansion);
		UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, size_t regionAndSelStart, size_t regionAndSelEnd, bool allowSmartCNPExpansion);
		UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd, bool allowSmartCNPExpansion);
		~UndoableContextHelper ();

	public:
		nonvirtual	void	CommandComplete ();
		nonvirtual	void	CommandComplete (size_t endOfInsert);

	public:
		nonvirtual	size_t	GetUndoRegionStart () const;
		nonvirtual	size_t	GetUndoRegionEnd () const;

	public:
		nonvirtual	bool	GetSimplePlainTextInsertOptimization () const;
		nonvirtual	void	SetSimplePlainTextInsertOptimization (bool simplePlainTextInsertOptimizationFlag);
	private:
		bool	fSimplePlainTextInsertOptimization;

#if		qSupportLed30CompatAPI
	public:
		nonvirtual	size_t	GetSelStart () const;
		nonvirtual	size_t	GetSelEnd () const;
#endif

	private:
		TextInteractor&	fTextInteractor;
		Led_SDK_String	fCmdName;
		size_t			fSelStart;
		size_t			fSelEnd;
		SavedTextRep*	fBefore;
		bool			fCommandComplete;
};






/*
@CLASS:			TextInteractor::TemporarilySetUpdateMode
@DESCRIPTION:	<p>See @'TextInteractor::GetDefaultUpdateMode'.</p>
*/
class	TextInteractor::TemporarilySetUpdateMode {
	public:
		TemporarilySetUpdateMode (TextInteractor& ti, TextInteractor::UpdateMode tmpUpdateMode);
		~TemporarilySetUpdateMode ();
	private:
		TextInteractor&	fTextInteractor;
		UpdateMode		fOldValue;
	private:
		TemporarilySetUpdateMode (const TemporarilySetUpdateMode&);	// DONT
		void operator= (const TemporarilySetUpdateMode&);			// DONT
		static	void* operator new (size_t);						// DONT
};






/*
@CLASS:			TextInteractor::InteractiveModeUpdater
@DESCRIPTION:	<p>Calls @'TextInteractor::SetInteractiveUpdateMode' to save/restore the interactive update
			mode for</p>
*/
class	TextInteractor::InteractiveModeUpdater {
	public:
		InteractiveModeUpdater (TextInteractor& ti, bool interactive = true);
		~InteractiveModeUpdater ();
	private:
		TextInteractor&			fTextInteractor;
		InteractiveUpdadeMode	fSavedMode;
};






/*
@CLASS:			TextInteractor::PreScrollInfo
@DESCRIPTION:	<p></p>
*/
class	TextInteractor::PreScrollInfo {
	public:
		PreScrollInfo ();

	private:
		UpdateMode		fUpdateMode;
		size_t			fOldWindowStart;
		Led_Coordinate	fOldHScrollPos;
		bool			fTryTodoScrollbits;
		size_t			fOldLastRowStart;

	private:
	#if		qFriendDeclsCannotReferToEnclosingClassInNestedClassDeclBug
		friend	class	TextInteractor;
	#else
		friend	void	TextInteractor::PreScrollHelper (UpdateMode updateMode, PreScrollInfo* preScrollInfo);
		friend	void	TextInteractor::PostScrollHelper (PreScrollInfo preScrollInfo);
	#endif
};





/*
@CLASS:			TextInteractor::BadUserInput
@DESCRIPTION:	<p>See @'TextInteractor::OnBadUserInput'.</p>
*/
class	TextInteractor::BadUserInput {
};





/*
@CLASS:			CommandNumberMapping<TARGET_COMMAND_NUMBER>
@DESCRIPTION:	<p></p>
*/
template	<typename	TARGET_COMMAND_NUMBER>
	class	CommandNumberMapping {
		public:
			typedef	TextInteractor::CommandNumber	CommandNumber;
		public:
			CommandNumberMapping ();
			~CommandNumberMapping ();

		public:
			static	CommandNumberMapping<TARGET_COMMAND_NUMBER>&	Get ();

		public:
			nonvirtual	void					AddAssociation (TARGET_COMMAND_NUMBER externalCommandNumber, CommandNumber internalCommandNumber);
			nonvirtual	void					AddRangeAssociation (TARGET_COMMAND_NUMBER externalRangeStart, TARGET_COMMAND_NUMBER externalRangeEnd, CommandNumber internalRangeStart, CommandNumber internalRangeEnd);
			virtual	CommandNumber				Lookup (TARGET_COMMAND_NUMBER tcn) const;
			virtual	TARGET_COMMAND_NUMBER		ReverseLookup (CommandNumber cmdNum) const;

		private:
			typedef	map<TARGET_COMMAND_NUMBER, CommandNumber>	MAP_TYPE;
			MAP_TYPE		fMap;
			struct	RangeElt {
				pair<TARGET_COMMAND_NUMBER,TARGET_COMMAND_NUMBER>	fExternalCmds;
				pair<CommandNumber,CommandNumber>					fInternalCmds;
			};
			typedef	vector<RangeElt>	RANGE_VEC_TYPE;
			RANGE_VEC_TYPE	fRanges;

		private:
			static	CommandNumberMapping<TARGET_COMMAND_NUMBER>*	sThe;
	};











/*
@CLASS:			SimpleCommandUpdater
@BASES:			TextInteractor::CommandUpdater
@DESCRIPTION:	<p></p>
*/
class    SimpleCommandUpdater : public TextInteractor::CommandUpdater {
	public:
		SimpleCommandUpdater (CommandNumber cmdNum);

	public:
		override    CommandNumber GetCmdID () const;
		override    bool          GetEnabled () const;
		override    void          SetEnabled (bool enabled);
		override    void          SetChecked (bool checked);
		override    void          SetText (const Led_SDK_Char* text);

	public:
		CommandNumber	fCommandNumber;
		bool			fEnabled;
		bool			fChecked;
		Led_SDK_String	fText;
};








/*
@CLASS:			TrivialImager_Interactor<TEXTSTORE,IMAGER>
@DESCRIPTION:	<p>Handy little template, if you want to use the power of Led, but just to wrap a particular imager,
	in a localized, one-time fasion, say todo printing, or some such. Not for interactors.</p>
		<p>The usuage can be as simple as:
		<code>
			void	SomeAppDrawCall (Led_Tablet t, const Led_Rect& r)
			{
				TrivialImager_Interactor<ChunkedArrayTextStore, WordProcessor> (t, r, LED_TCHAR_OF ("Hi mom")).Draw ();
			}
		</code>
		</p>
		<p>See also @'TrivialImager<TEXTSTORE,IMAGER>', and @'TrivialWordWrappedImager<TEXTSTORE,IMAGER>'.</p>
 */
template	<typename TEXTSTORE, typename	IMAGER>
	class	TrivialImager_Interactor : public TrivialImager<TEXTSTORE,IMAGER> {
	private:
		typedef	TrivialImager<TEXTSTORE,IMAGER>	inherited;
		public:
			TrivialImager_Interactor (Led_Tablet t): inherited (t) {}
			TrivialImager_Interactor (Led_Tablet t, Led_Rect bounds, const Led_tString& initialText = LED_TCHAR_OF("")): inherited (t)
			{
				// Kooky I cannot just call base class CTOR that does all this - but then it invokes calls to RefreshWindowRect_ etc, before
				// THIS CTOR is done, so they call pure-virtual version!
				SnagAttributesFromTablet ();
				SetWindowRect (bounds);
				GetTextStore ().Replace (0, 0, initialText.c_str (), initialText.length ());
			}


		// In case the imager is a TextInteractor - provide dummy implemenations...
		public:
			typedef	typename	IMAGER::UpdateMode	UpdateMode;
			override		bool	QueryInputKeyStrokesPending () const	{	return false; };
			override		void	RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const{}
			override		void	UpdateWindowRect_ (const Led_Rect& windowRectArea) const {}
	};










/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//	class	TextInteractor::SupressCommandBreaksContext
	inline	TextInteractor::SupressCommandBreaksContext::SupressCommandBreaksContext (TextInteractor& ti):
			fTextInteractor (ti),
			fOldVal (ti.fSupressCommandBreaksContext)
			{
				ti.fSupressCommandBreaksContext = true;
			}
	inline	TextInteractor::SupressCommandBreaksContext::~SupressCommandBreaksContext ()
		{
			fTextInteractor.fSupressCommandBreaksContext = fOldVal;
		}

		
		
		
		
//	class	TextInteractor::PreScrollInfo
	inline	TextInteractor::PreScrollInfo::PreScrollInfo ():
		fUpdateMode (eDefaultUpdate),
		fOldWindowStart (0),
		fOldHScrollPos (0),
		fTryTodoScrollbits (false),
		fOldLastRowStart (0)
		{
		}




//	class	CommandNumberMapping<TARGET_COMMAND_NUMBER>
	template	<typename	TARGET_COMMAND_NUMBER>
		CommandNumberMapping<TARGET_COMMAND_NUMBER>*	CommandNumberMapping<TARGET_COMMAND_NUMBER>::sThe	=	NULL;
	template	<typename	TARGET_COMMAND_NUMBER>
		CommandNumberMapping<TARGET_COMMAND_NUMBER>::CommandNumberMapping ():
			fMap (),
			fRanges ()
			{
				Led_Assert (sThe == NULL);
				sThe = this;
			}
	template	<typename	TARGET_COMMAND_NUMBER>
		CommandNumberMapping<TARGET_COMMAND_NUMBER>::~CommandNumberMapping ()
			{
				Led_Assert (sThe == this);
				sThe = NULL;
			}
	template	<typename	TARGET_COMMAND_NUMBER>
		inline	CommandNumberMapping<TARGET_COMMAND_NUMBER>&	CommandNumberMapping<TARGET_COMMAND_NUMBER>::Get ()
			{
				Led_AssertNotNil (sThe);		//	Applications using Led_MFC must instantiate a subclass of MFC_CommandNumberMapping
												//	BEFORE any access to this class
				return *sThe;
			}
	template	<typename	TARGET_COMMAND_NUMBER>
		inline	void	CommandNumberMapping<TARGET_COMMAND_NUMBER>::AddAssociation (TARGET_COMMAND_NUMBER externalCommandNumber, CommandNumber internalCommandNumber)
			{
				fMap.insert (MAP_TYPE::value_type (externalCommandNumber, internalCommandNumber));
			}
	template	<typename	TARGET_COMMAND_NUMBER>
		inline	void	CommandNumberMapping<TARGET_COMMAND_NUMBER>::AddRangeAssociation (TARGET_COMMAND_NUMBER externalRangeStart, TARGET_COMMAND_NUMBER externalRangeEnd, CommandNumber internalRangeStart, CommandNumber internalRangeEnd)
			{
				Led_Require ((externalRangeEnd-externalRangeStart) == (internalRangeEnd-internalRangeStart));	// ranges same length.

				RangeElt	re;
				re.fExternalCmds.first = externalRangeStart;
				re.fExternalCmds.second = externalRangeEnd;
				re.fInternalCmds.first = internalRangeStart;
				re.fInternalCmds.second = internalRangeEnd;
				fRanges.push_back (re);
			}
	template	<typename	TARGET_COMMAND_NUMBER>
		TextInteractor::CommandNumber	CommandNumberMapping<TARGET_COMMAND_NUMBER>::Lookup (TARGET_COMMAND_NUMBER externalCommandNumber) const
			{
				typename MAP_TYPE::const_iterator	i	=	fMap.find (externalCommandNumber);
				if (i == fMap.end ()) {
					for (typename RANGE_VEC_TYPE::const_iterator j = fRanges.begin (); j != fRanges.end (); ++j) {
						const RangeElt&	re	=	*j;
						if (re.fExternalCmds.first <= externalCommandNumber and externalCommandNumber <= re.fExternalCmds.second) {
							size_t							offset	=	externalCommandNumber - re.fExternalCmds.first;
							TextInteractor::CommandNumber	cmdNum	=	re.fInternalCmds.first + offset;
							Led_Assert (cmdNum <= re.fInternalCmds.second);
							return cmdNum;
						}
					}
					return TextInteractor::kNoCommand_CmdID;
				}
				else {
					return (*i).second;
				}
			}
	template	<typename	TARGET_COMMAND_NUMBER>
		TARGET_COMMAND_NUMBER	CommandNumberMapping<TARGET_COMMAND_NUMBER>::ReverseLookup (CommandNumber cmdNum) const
			{
				for (typename MAP_TYPE::const_iterator i	=	fMap.begin (); i != fMap.end (); ++i) {
					if ((*i).second == cmdNum) {
						return (*i).first;
					}
				}
				for (typename RANGE_VEC_TYPE::const_iterator j = fRanges.begin (); j != fRanges.end (); ++j) {
					const RangeElt&	re	=	*j;
					if (re.fInternalCmds.first <= cmdNum and cmdNum <= re.fInternalCmds.second) {
						size_t					offset	=	cmdNum - re.fInternalCmds.first;
						TARGET_COMMAND_NUMBER	tarCmd	=	re.fExternalCmds.first + offset;
						Led_Assert (tarCmd <= re.fExternalCmds.second);
						return tarCmd;
					}
				}
				// Treat a command which hasn't been registered as the special value 0 indicating 'no command'
				return 0;
			}



			
			
//	class	SimpleCommandUpdater
	inline	SimpleCommandUpdater::SimpleCommandUpdater (CommandNumber cmdNum):
			fCommandNumber (cmdNum),
			fEnabled (false),
			fChecked (false),
			fText ()
			{
			}
	inline	SimpleCommandUpdater::CommandNumber SimpleCommandUpdater::GetCmdID () const
		{
			return fCommandNumber;
		}
	inline	bool    SimpleCommandUpdater::GetEnabled () const
		{
			return fEnabled;
		}
	inline	void	SimpleCommandUpdater::SetEnabled (bool enabled)
		{
			fEnabled = enabled;
		}
	inline	void	SimpleCommandUpdater::SetChecked (bool checked)
		{
			fChecked = checked;
		}
	inline	void	SimpleCommandUpdater::SetText (const Led_SDK_Char* text)
		{
			Led_RequireNotNil (text);
			fText = text;
		}






//	class	TextInteractor::InteractiveModeUpdater
	inline	TextInteractor::InteractiveModeUpdater::InteractiveModeUpdater (TextInteractor& ti, bool interactive):
		fTextInteractor (ti),
		fSavedMode (ti.GetInteractiveUpdateMode ())
		{
			InteractiveUpdadeMode	newMode	=	interactive? eInteractiveUpdateMode: eNonInteractiveUpdateMode;
			if (ti.GetInteractiveUpdateMode () == eIndeterminateInteractiveUpdateMode) {
				ti.SetInteractiveUpdateMode (newMode);
			}
			else {
				/*
				 *	If the original mode was already set - make sure our new mode agrees.
				 */
				Led_Assert (ti.GetInteractiveUpdateMode () == newMode);
			}
		}
	inline	TextInteractor::InteractiveModeUpdater::~InteractiveModeUpdater ()
		{
			fTextInteractor.SetInteractiveUpdateMode (fSavedMode);
		}




//	class	TextInteractor::SmartCNPInfo
	inline	TextInteractor::SmartCNPInfo::SmartCNPInfo ():
		fWordBreakAtSelStart (false),
		fWordBreakAtSelEnd (false)
		{
		}



//	class	TextInteractor
	/*
	@METHOD:		TextInteractor::GetCommandHandler
	@DESCRIPTION:	<p>TextInteractor's have associated an optional @'CommandHandler', used for maintaining UNDO
		information. This can return NULL, if no CommandHandler is currently associated (meaning no Undo is avialable).</p>
	*/
	inline	CommandHandler*	TextInteractor::GetCommandHandler () const
		{
			return fCommandHandler;
		}
	/*
	@METHOD:		TextInteractor::SetCommandHandler
	@DESCRIPTION:	<p>See @'TextInteractor::GetCommandHandler'.</p>
	*/
	inline	void	TextInteractor::SetCommandHandler (CommandHandler* commandHandler)
		{
			fCommandHandler = commandHandler;
		}
	/*
	@METHOD:		TextInteractor::BreakInGroupedCommands
	@DESCRIPTION:	<p>Trivial helper - delegates to @'CommandHandler::BreakInGroupedCommands' if
				@'TextInteractor::GetCommandHandler' returns non-NULL.
				See also @'TextInteractor::SupressCommandBreaksContext'</p>
	*/
	inline	void	TextInteractor::BreakInGroupedCommands ()
		{
			if (fCommandHandler != NULL and not fSupressCommandBreaksContext) {
				fCommandHandler->BreakInGroupedCommands ();
			}
		}
	/*
	@METHOD:		TextInteractor::BreakInGroupedCommandsIfDifferentCommand
	@DESCRIPTION:	<p>Trivial helper - delegates to @'CommandHandler::BreakInGroupedCommandsIfDifferentCommand' if
				@'TextInteractor::GetCommandHandler' returns non-NULL.
				See also @'TextInteractor::SupressCommandBreaksContext'</p>
	*/
	inline	void	TextInteractor::BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName)
		{
			if (fCommandHandler != NULL and not fSupressCommandBreaksContext) {
				fCommandHandler->BreakInGroupedCommandsIfDifferentCommand (cmdName);
			}
		}
	/*
	@METHOD:		TextInteractor::GetDialogSupport
	@DESCRIPTION:	<p></p>
	*/
	inline	TextInteractor::DialogSupport&	TextInteractor::GetDialogSupport ()
		{
			if (sDialogSupport == NULL) {
				static	DialogSupport	sDefSup;
				sDialogSupport = &sDefSup;
			}
			return *sDialogSupport;
		}
	/*
	@METHOD:		TextInteractor::SetDialogSupport
	@DESCRIPTION:	<p></p>
	*/
	inline	void	TextInteractor::SetDialogSupport (DialogSupport* ds)
		{
			sDialogSupport = ds;
		}
	/*
	@METHOD:		TextInteractor::GetCommandNames
	@DESCRIPTION:	<p>Returns command name for each of the user-visible commands produced by this module.
		This name is used used in the constructed Undo command name, as
		in, "Undo Paste". You can replace this name with whatever you like.You change this value with
		WordProcessor::SetCommandNames.</p>
			<p> The point of this is to allow for different UI-language localizations,
				without having to change Led itself.</p>
			<p>See also @'TextInteractor::CommandNames'.</p>
	*/
	inline	const TextInteractor::CommandNames&	TextInteractor::GetCommandNames ()
		{
			return sCommandNames;
		}
	/*
	@METHOD:		TextInteractor::SetCommandNames
	@DESCRIPTION:	<p>See @'TextInteractor::GetCommandNames'.</p>
	*/
	inline	void	TextInteractor::SetCommandNames (const TextInteractor::CommandNames& cmdNames)
		{
			sCommandNames = cmdNames;
		}
	/*
	@METHOD:		TextInteractor::GetSpellCheckEngine
	@DESCRIPTION:	<p>Return a pointer to the current @'SpellCheckEngine' if there is one. It CAN BE NULL - meaning
				that spellchecking is disabled. See @'TextInteractor::SetSpellCheckEngine'.</p>
	*/
	inline	SpellCheckEngine*	TextInteractor::GetSpellCheckEngine () const
		{
			return fSpellCheckEngine;
		}
	/*
	@METHOD:		TextInteractor::SetSpellCheckEngine
	@DESCRIPTION:	<p>This is typically called by an application or document to associate a spellcheck engine with the 
				@'TextInteractor'. That caller still 'owns' the object and must manage its lifetime, and to reset this
				value (in this class) to NULL before destorying the @'SpellCheckEngine'. If you have an option to turn on/off
				spellchecking, when you turn it off, you would typically just call this method with NULL to disable
				spellchecking command funcitonality. Be sure to set this property to NULL before destruction.</p>
					<p>See @'TextInteractor::GetSpellCheckEngine'.</p>
	*/
	inline	void	TextInteractor::SetSpellCheckEngine (SpellCheckEngine* spellCheckEngine)
		{
			fSpellCheckEngine = spellCheckEngine;
		}
	/*
	@METHOD:		TextInteractor::GetDefaultUpdateMode
	@DESCRIPTION:	<p>TextInteractor's have an associated default UpdateMode. This is the update mode which is
				used by methods which used the eDefaultUpdateMode argument (most default UpdateMode args in Led are this value).</p>
					<p>This value should <em>not</em> be set directly. Instead, instantiate a
				@'TextInteractor::TemporarilySetUpdateMode' object on
				the stack to temporarily set the default update mode.</p>
	*/
	inline	TextInteractor::UpdateMode	TextInteractor::GetDefaultUpdateMode () const
		{
			Led_Ensure (fDefaultUpdateMode != eDefaultUpdate);
			return fDefaultUpdateMode;
		}
	/*
	@METHOD:		TextInteractor::RealUpdateMode
	@DESCRIPTION:	<p>TextInteractor's have an associated default UpdateMode. This utility method takes an arbitrary
				UpdateMode, and returns either its argument, or if it was eDefaultUpdateMode, it returns the current value
				of the default update mode.</p>
	*/
	inline	TextInteractor::UpdateMode	TextInteractor::RealUpdateMode (UpdateMode updateMode) const
		{
			return updateMode==eDefaultUpdate? GetDefaultUpdateMode (): updateMode;
		}
	/*
	@METHOD:		TextInteractor::GetSmartCutAndPasteMode
	@DESCRIPTION:	<p>TextInteractor's support a UI feature called "smart cut & paste". Basicly, Led tries to guess
				what a user 'really meant todo' with surrounding whitespace when dragging, and cutting and pasting text.</p>
					<p>	This can sometimes be extremely helpful. Othertimes, it can be extremely anoying. You decide. Thats
				why its an option.</p>
					<p>And see @'TextInteractor::OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds' and
				@'TextInteractor::OptionallyExpandSelectionForSmartCutAndPasteModeDeletes' for the implementation of
				smart cut and paste.</p>
	*/
	inline	bool	TextInteractor::GetSmartCutAndPasteMode () const
		{
			return fSmartCutAndPasteMode;
		}
	/*
	@METHOD:		TextInteractor::SetSmartCutAndPasteMode
	@DESCRIPTION:	<p>Set the 'smart cut and paste' mode. See TextInteractor::GetSmartCutAndPasteMode for more details.</p>
	*/
	inline	void	TextInteractor::SetSmartCutAndPasteMode (bool smartCutAndPasteMode)
		{
			fSmartCutAndPasteMode = smartCutAndPasteMode;
		}
	/*
	@METHOD:		TextInteractor::GetCurClickCount
	@DESCRIPTION:	<p>Used to keep track of the number of clicks the user has done very recently. Used for determining
				double and triple clicks. If the user clicks, and then clicks a long time later - it wont be considered
				a double click - and so this value will remain one.</p>
	*/
	inline	unsigned	TextInteractor::GetCurClickCount () const
		{
			return fClickCount;
		}
	/*
	@METHOD:		TextInteractor::SetCurClickCount
	@DESCRIPTION:	<p>See also @'TextInteractor::GetCurClickCount' (). This is seldom called directly - except perhaps to RESET
				the count to zero or one. This is typically just called internally through @'TextInteractor::UpdateClickCount'.</p>
	*/
	inline	void	TextInteractor::SetCurClickCount (unsigned curClickCount, float lastClickAt)
		{
			fClickCount = curClickCount;
			fLastClickedAt = lastClickAt;
		}
	/*
	@METHOD:		TextInteractor::IncrementCurClickCount
	@DESCRIPTION:	<p>See also @'TextInteractor::GetCurClickCount' () and @'TextInteractor::SetCurClickCount' ().
				This is typically just called internally through @'TextInteractor::UpdateClickCount'.</p>
	*/
	inline	void	TextInteractor::IncrementCurClickCount (float lastClickAt)
		{
			fClickCount++;
			fLastClickedAt = lastClickAt;
		}
	/*
	@METHOD:		TextInteractor::IsWholeWindowInvalid
	@DESCRIPTION:	<p>Sometimes we can go through alot of computation to see just what minimal area of the window needs
				to be updated. Sometimes this can be quite helpful, and allow things to run quickly. But other times,
				like when we are making many changes to the text before ever showing anything, it would be just faster
				to mark the whole screen as dirty, and to avoid the computation about what region needs updating.
				Thats what these APIs here are about.</p>
					<p>You can tell the @'TextInteractor' that the whole window is dirty explicitly by simply doing a Refresh()
				with no arguments (really anything which causes whole window rect to become invalid).
				You can query if the whole window is invalid. This only returns true if the whole window is KNOWN
				to be invalid. It maybe that there is some complex update region which happens to cover the whole window
				or that something has happened that we don't know about to make the entire window invalid. We will
				just return false in those cases. But you can count on the fact that if IsWholeWindowInvalid() returns
				true - this means you need not do any bookkeeping/work to compute just what region you will be
				updating.</p>
					<p>We override the Draw (const Led_Rect& subsetToDraw, bool printing) method to call
					@'TextInteractor::NoteWindowPartiallyUpdated' ().
				(clearning the fWholeWindowInvalid bool). If subclassers who override Draw(), or otherwise update the
				screen (calling ValidRect ()) - should be sure to call NoteWindowPartiallyUpdated ();</p>
					<p>Led supports a redraw optimization, whereby complex calculations about what region of the screen
				needs to be invalidated can be avoided, if we keep track of, and cache the notion that the whole screen is
				already fully invalidated. This method is called internally to avoid complex calculations. You may <em>rarely</em>
				want to call it as well, if profiling yields a hot-spot in your code, in a calculation of something that need not
				be done if the screen has already been marked as invalid.</p>
	*/
	inline	bool	TextInteractor::IsWholeWindowInvalid () const
		{
			return fWholeWindowInvalid;
		}
	/*
	@METHOD:		TextInteractor::NoteWholeWindowIsInvalid
	@DESCRIPTION:	<p>See @'TextInteractor::IsWholeWindowInvalid'. This method marks the screen as
				already having been invalidated. Called internally by Led. Only call this with extreme
				caution. Should very rarely be appropriate, outside of class library wrappers.</p>
	*/
	inline	void	TextInteractor::NoteWholeWindowIsInvalid ()
		{
			fWholeWindowInvalid = true;
		}
	/*
	@METHOD:		TextInteractor::NoteWindowPartiallyUpdated
	@DESCRIPTION:	<p>See @'TextInteractor::IsWholeWindowInvalid'. This method marks the screen as having been
				at least partially redrawn,	so the 'IsWholeWindowInvalid' no longer apply.</p>
					<p>Called internally by Led. Only call this with extreme caution. Should very rarely be
				appropriate, outside of class library wrappers.</p>
	*/
	inline	void	TextInteractor::NoteWindowPartiallyUpdated ()
		{
			fWholeWindowInvalid = false;
		}
	/*
	@METHOD:		TextInteractor::Refresh
	@DESCRIPTION:	<p>Invalidate the entire window rectangle (@'TextImager::GetWindowRect') in a
				manner dependingon the 'updateMode' (see @'TextInteractor::UpdateMode'). Delegates
				implemantion to @'TextInteractor::Refresh_', which is generally	overriden in
				SDK-specific wrapper subclass.</p>
	*/
	inline	void	TextInteractor::Refresh (UpdateMode updateMode) const
		{
			updateMode = RealUpdateMode (updateMode);
			if (updateMode != eNoUpdate) {
				if (updateMode == eDelayedUpdate and IsWholeWindowInvalid ()) {
					return;
				}
				RefreshWindowRect_ (GetWindowRect (), updateMode);
				// wrong - cuz above may not have invaled anything if ??? Hmm.. maybe under some circomstances???
				if (updateMode == eDelayedUpdate) {
					const_cast<TextInteractor*>(this)->NoteWholeWindowIsInvalid ();
				}
			}
		}
	/*
	@METHOD:		TextInteractor::RefreshWindowRect_
	@DESCRIPTION:	<p>Cause the given region of the edit window to be updated. The given 'windowRectArea'
				is given in the same coordinates as the window rect
				specified by @'TextImager::SetWindowRect': it is not relative to that rectangle.</p>
					<p>This simply calls @'TextInteractor::RefreshWindowRect_' to delegate its implemenation.</p>
	*/
	inline	void	TextInteractor::RefreshWindowRect (const Led_Rect& windowRectArea, UpdateMode updateMode) const
		{
			updateMode = RealUpdateMode (updateMode);
			if (updateMode != eNoUpdate) {
				if (updateMode == eDelayedUpdate and IsWholeWindowInvalid ()) {
					return;
				}
				RefreshWindowRect_ (windowRectArea, updateMode);
				if (updateMode == eDelayedUpdate and windowRectArea.Contains (GetWindowRect ())) {
					const_cast<TextInteractor*>(this)->NoteWholeWindowIsInvalid ();
				}
			}
		}
	/*
	@METHOD:		TextInteractor::Update
	@DESCRIPTION:	<p>Calls @'TextInteractor::UpdateWindowRect' on the entire windowrect.</p>
	*/
	inline	void	TextInteractor::Update (bool ignoreCannotUpdateNowErrors) const
		{
			try {
				UpdateWindowRect_ (GetWindowRect ());
			}
			catch (CannotUpdateNow&) {
				if (not ignoreCannotUpdateNowErrors) {
					throw;
				}
			}
			catch (...) {
				throw;
			}
		}
	/*
	@METHOD:		TextInteractor::UpdateWindowRect
	@DESCRIPTION:	<p>Makes sure the given section of the window rect is updated (redisplayed). This MAY
				not be possible at a given time, in whcih case, @'TextInteractor::CannotUpdateNow' maybe
				thrown instead.</p>
	*/
	inline	void	TextInteractor::UpdateWindowRect (const Led_Rect& windowRectArea, bool ignoreCannotUpdateNowErrors) const
		{
			try {
				UpdateWindowRect_ (windowRectArea);
			}
			catch (CannotUpdateNow&) {
				if (not ignoreCannotUpdateNowErrors) {
					throw;
				}
			}
			catch (...) {
				throw;
			}
		}
	/*
	@METHOD:		TextInteractor::GetUseSecondaryHilight
	@DESCRIPTION:	<p>See @'TextInteractor::SetUseSecondaryHilight'.</p>
	*/
	inline	bool	TextInteractor::GetUseSecondaryHilight () const
		{
			return fUseSecondaryHilight;
		}
	/*
	@METHOD:		TextInteractor::SetUseSecondaryHilight
	@DESCRIPTION:	<p>If this is set true (false by default) then when a window is deactivated, it will draw
				its selection hilight as an outline - rather than just not displaying it.</p>
					<p>See also @'TextInteractor::GetUseSecondaryHilight'.</p>
	*/
	inline	void	TextInteractor::SetUseSecondaryHilight (bool useSecondaryHilight)
		{
			if (fUseSecondaryHilight != useSecondaryHilight) {
				Refresh ();
				fUseSecondaryHilight = useSecondaryHilight;
			}
		}
	inline	bool	TextInteractor::GetUseBitmapScrollingOptimization () const
		{
			return fUseBitmapScrollingOptimization;
		}
	inline	void	TextInteractor::SetUseBitmapScrollingOptimization (bool useBitmapScrollingOptimization)
		{
			fUseBitmapScrollingOptimization = useBitmapScrollingOptimization;
		}
	inline	bool	TextInteractor::GetSupressTypedControlCharacters () const
		{
			return fSupressTypedControlCharacters;
		}
	inline	void	TextInteractor::SetSupressTypedControlCharacters (bool supressTypedControlCharacters)
		{
			fSupressTypedControlCharacters = supressTypedControlCharacters;
		}
	/*
	@METHOD:		TextInteractor::GetInteractiveUpdateMode
	@DESCRIPTION:	<p>See also @'TextInteractor::CheckIfCurrentUpdateIsInteractive'.
	*/
	inline	TextInteractor::InteractiveUpdadeMode	TextInteractor::GetInteractiveUpdateMode () const
		{
			return fInteractiveUpdadeMode;
		}
	/*
	@METHOD:		TextInteractor::SetInteractiveUpdateMode
	@DESCRIPTION:	<p>See also @'TextInteractor::CheckIfCurrentUpdateIsInteractive'.
	*/
	inline	void	TextInteractor::SetInteractiveUpdateMode (InteractiveUpdadeMode interactive)
		{
			fInteractiveUpdadeMode = interactive;
		}
	/*
	@METHOD:		TextInteractor::CheckIfCurrentUpdateIsInteractive
	@DESCRIPTION:	<p>See also @'TextInteractor::InteractiveUpdadeMode' and @'TextInteractor::SetInteractiveUpdateMode' ().
	*/
	inline	bool	TextInteractor::CheckIfCurrentUpdateIsInteractive () const
		{
			// Interpret 'indeterminate' as NOT being interactive mode
			return fInteractiveUpdadeMode == eInteractiveUpdateMode;
		}
	/*
	@METHOD:		TextInteractor::GetInternalizer
	@DESCRIPTION:
	*/
	inline	Led_RefCntPtr<FlavorPackageInternalizer>	TextInteractor::GetInternalizer () const
		{
			return fInternalizer;
		}
	/*
	@METHOD:		TextInteractor::SetInternalizer
	@DESCRIPTION:	<p>Calls @'TextInteractor::HookInternalizerChanged' whenever the internalizer changes. Can be a NULL externalizer.</p>
	*/
	inline	void										TextInteractor::SetInternalizer (const Led_RefCntPtr<FlavorPackageInternalizer>& i)
		{
			fInternalizer = i;
			HookInternalizerChanged ();
		}
	/*
	@METHOD:		TextInteractor::GetExternalizer
	@DESCRIPTION:
	*/
	inline	Led_RefCntPtr<FlavorPackageExternalizer>	TextInteractor::GetExternalizer () const
		{
			return fExternalizer;
		}
	/*
	@METHOD:		TextInteractor::SetExternalizer
	@DESCRIPTION:	<p>Calls @'TextInteractor::HookExternalizerChanged' whenever the externalizer changes. Can be a NULL externalizer.</p>
	*/
	inline	void	TextInteractor::SetExternalizer (const Led_RefCntPtr<FlavorPackageExternalizer>& e)
		{
			fExternalizer = e;
			HookExternalizerChanged ();
		}
	inline	void	TextInteractor::InvalidateScrollBarParameters_ ()
		{
			fScrollBarParamsValid = false;
		}
	inline	void	TextInteractor::UpdateScrollBars_ ()
		{
			fScrollBarParamsValid = true;
		}
	/*
	@METHOD:		TextInteractor::GetCaretShown
	@DESCRIPTION:	<p>Flag to control whether or not blinking caret is shown for this editor.
				Typically it will be turned on when this widget gets the focus, and off when it
				loses it. But that is the responsability of higher level software (subclasses).
				</p>
					<p>See also @'TextInteractor::GetCaretShownSituation' and @'TextInteractor::SetCaretShown'.</p>
	*/
	inline	bool	TextInteractor::GetCaretShown () const
		{
			return (fCaretShown);
		}
	/*
	@METHOD:		TextInteractor::GetCaretShownAfterPos
	@DESCRIPTION:	<p>Given an empty selection, it seems pretty obvious where to place
				the caret - right? NOT!!!</p>
					<p>Remember that a selection position falls BETWEEN two characters. One
	 			plausible definition is to use the RIGHT edge of the preceeding character.
	 			Yet - ANOTHER plausible definition is to use the LEFT edge of the following
				character. For some particular positions (notable the beginning and ends of
	 			the text) or or the other of these definitions might not quite make sense.
				But there are obvious definitions to take care of these special cases.</p>
					<p>And - luckily - for the most part - these definitions coincide. But there
	 			is at least one place which comes up (in normal editor usage - more in
	 			fancy Led subclasses like LVEJ's side-by-side mode). In particular, when
	 			the preceeding and following characters fall on different rows. In this
	 			case, a hint is helpful in deciding which of the two approaches to use.</p>
					<p>Many editors simply pick one approach or the other. Led did this for a while.
				It always picked the leading edge of the following character. But now Led
	 			has a option. When users change the selection by clicking, this option is
				is automaticly set. And similarly when they use the arrow keys, or do other
				editing operations. The set-method for this field is virtual, so subclassers
				who prefer one definition over the other can override the set method to simply
				force a particular value.</p>
					<p>If GetCaretShownAfterPos is true, use the use the LHS of following character,
				and if false, then the RHS of the preceeding character. So the after refers to which
				character to use - the one before or after the given marker POS. The side of the charater is the opposite
				of this value.</p>
	 */
	inline	bool	TextInteractor::GetCaretShownAfterPos () const
		{
			return (fCaretShownAfterPos);
		}
	/*
	@METHOD:		TextInteractor::GetScrollBarType
	@DESCRIPTION:	
					<p>See also 'TextInteractor::SetScrollBarType'</p>
	*/
	inline	TextInteractor::ScrollBarType	TextInteractor::GetScrollBarType (VHSelect vh) const
		{
			return fScrollBarType[vh];
		}
	inline	void	TextInteractor::SetScrollBarType_ (VHSelect vh, ScrollBarType scrollBarType)
			{
				fScrollBarType[vh] = scrollBarType;
			}
	inline	void	TextInteractor::UpdateIfNoKeysPending ()
		{
			if (QueryInputKeyStrokesPending ()) {
				// Note - this is NOT strictly necesary todo. But rather - its a performance hack. This call takes
				// very little time, and it allows LATER optimizations (see TextInteractor::IsWholeWindowInvalid ());
				// This was done in response to SPR#0659. I never measured exactly how much of a speedup it produced,
				// but it eliminated stuff from my profiles so I went on to bigger things... LGP 991214
				Refresh ();
			}
			else {
				Update ();
			}
		}




	//	class	TextInteractor::SearchParameters
		inline	TextInteractor::SearchParameters::SearchParameters ():
			inherited (),
			fRecentFindStrings ()
			{
			}


	//	class	TextInteractor::ReplaceParameters
		inline	TextInteractor::ReplaceParameters::ReplaceParameters ():
			fReplaceWith ()
			{
			}



	//	class	TextInteractor::UndoableContextHelper
		/*
		@METHOD:		TextInteractor::UndoableContextHelper::GetUndoRegionStart
		@DESCRIPTION:	<p>This method was formerly called 'GetSelStart'. However - this value - though originally based on the selection -
					can be slightly different - especially in light of smart-cut-and-paste. It is the area that is being changed,
					based on the users original selection.</p>
		*/
		inline	size_t	TextInteractor::UndoableContextHelper::GetUndoRegionStart () const
			{
				return fSelStart;
			}
		/*
		@METHOD:		TextInteractor::UndoableContextHelper::GetUndoRegionEnd
		@DESCRIPTION:	<p>@'See @'TextInteractor::UndoableContextHelper::GetUndoRegionStart'</p>
		*/
		inline	size_t	TextInteractor::UndoableContextHelper::GetUndoRegionEnd () const
			{
				return fSelEnd;
			}
		/*
		@METHOD:		TextInteractor::UndoableContextHelper::GetSimplePlainTextInsertOptimization
		@DESCRIPTION:	<p>@'See @'TextInteractor::UndoableContextHelper::SetSimplePlainTextInsertOptimization'</p>
		*/
		inline	bool	TextInteractor::UndoableContextHelper::GetSimplePlainTextInsertOptimization () const
			{
				return fSimplePlainTextInsertOptimization;
			}
		/*
		@METHOD:		TextInteractor::UndoableContextHelper::SetSimplePlainTextInsertOptimization
		@DESCRIPTION:	<p>This should rarely be called. Its called internally just in the context where we are doing a special
					form of text insert to create an optimized representation for undo information (in a common context).
					</p>
						<p>If this is to be called - it <em>must</em> be called before calling @'TextInteractor::UndoableContextHelper::CommandComplete'.</p>
						<p>@'See also @'TextInteractor::UndoableContextHelper::GetSimplePlainTextInsertOptimization'</p>
		*/
		inline	void	TextInteractor::UndoableContextHelper::SetSimplePlainTextInsertOptimization (bool simplePlainTextInsertOptimizationFlag)
			{
				fSimplePlainTextInsertOptimization = simplePlainTextInsertOptimizationFlag;
			}
	#if		qSupportLed30CompatAPI
		inline	size_t	TextInteractor::UndoableContextHelper::GetSelStart () const
			{
				return fSelStart;
			}
		inline	size_t	TextInteractor::UndoableContextHelper::GetSelEnd () const
			{
				return fSelEnd;
			}
	#endif





//	class	TextInteractor::TemporarilySetUpdateMode
	inline	TextInteractor::TemporarilySetUpdateMode::TemporarilySetUpdateMode (TextInteractor& ti, UpdateMode tmpUpdateMode):
		fTextInteractor (ti),
		fOldValue (ti.GetDefaultUpdateMode ())
		{
			fTextInteractor.SetDefaultUpdateMode (tmpUpdateMode);
		}
	inline	TextInteractor::TemporarilySetUpdateMode::~TemporarilySetUpdateMode ()
		{
			fTextInteractor.SetDefaultUpdateMode (fOldValue);
		}





#if		qLedUsesNamespaces
}
#endif





#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif
#if		qSilenceAnnoyingCompilerWarnings && __MWERKS__
	#pragma	warn_hidevirtual	reset
#endif



#endif	/*__TextInteractor_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

