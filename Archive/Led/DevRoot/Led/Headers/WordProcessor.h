/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__WordProcessor_h__
#define	__WordProcessor_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/WordProcessor.h,v 2.253 2004/02/10 19:46:19 lewis Exp $
 */

/*
@MODULE:	WordProcessor
@DESCRIPTION:
				<p>This module contains all the classes needed to implement a typical
			word-processor UI. These things include per-paragraph tab stops, justification,
			margins, etc. The main class you should be interested in is the @'WordProcessor' class.
			</p>
 */

/*
 * Changes:
 *	$Log: WordProcessor.h,v $
 *	Revision 2.253  2004/02/10 19:46:19  lewis
 *	SPR#1635: made Table::EmbeddedTableWordProcessor::TableCMD into Table::Embedde::TableCMD (change nesting) - to work around compiler issue (bug?) where I cannot declare a:b:c a friend of a inside a when b is defined outsid of a (but declared inside a). No matter. Also - added qNameLookupXXXInBaseClassWhenItIsNestedSometimesFailsBug. All above to get compiling with MSVC6
 *	
 *	Revision 2.252  2004/01/25 23:24:40  lewis
 *	SPR#1621: problem with CHecKForUpdatesOnWeb was cuz code to map associated cmd#s returns zero when a failed mapping occurs, and we had bad association between cmds# between Led internal and LedIt/ActiveLedIt 'system-dependent' values for Win32. Fix those for kFirstPrivateEmbedding_CmdID/kLastSelectedEmbedding_CmdID - and clean up first/last private range inside of it.
 *	
 *	Revision 2.251  2004/01/23 21:41:08  lewis
 *	a few small changes for MSVC6 compat (declare a couple of - I think NEEDED - friends, and qNameLookupXXXInBaseClassWhenItIsNestedSometimesFailsBug, and use no-arg Led_TWIPS_Rect CTOR since explicit one failed in MSVC60 (and not needed)
 *	
 *	Revision 2.250  2004/01/09 15:27:58  lewis
 *	SPR#1615: Fixed select-all behavior when there is only a single table (and nothing else in the document. Also - revised behavior so selectall first selects all in cell, and only if already selected - selects all in document
 *	
 *	Revision 2.249  2003/11/26 19:55:36  lewis
 *	doccomments and re-arrange asserts slightly
 *	
 *	Revision 2.248  2003/11/06 16:07:04  lewis
 *	fix a few cmd names (adding set margins AND first-indent cmd). Completes fix for SPR#1559: Store fCachedFarthestRightMarginInDocument in TWIPS and change GetFarthestRightMargin* functions to return TWIPS (cuz now cached in PDB - which has no tablet info). Moved fCachedFarthestRightMarginInDocument to PDB - and updated (inval) when margins change.
 *	
 *	Revision 2.247  2003/11/06 03:20:17  lewis
 *	SPR#1560: lose WordProcessorHScrollbarHelper<> template and merge its functionality into WordProcessor.
 *	SPR#1559: revise ComputeMaxHScrollPos () and add CalculateFarthestRightMargin () etc functions, and
 *	changed how it caches info.
 *	
 *	Revision 2.246  2003/05/23 15:49:53  lewis
 *	SPR#1411: Added various debugging flags to WordProcessorTextIOSinkStream to assure the right calling
 *	semantics for TableOpen/Close/CellOpen/Close/RowOpen/Close.
 *	
 *	Revision 2.245  2003/05/21 15:50:09  lewis
 *	SPR#1398: Added AllowUpdateInfoPropagationContext class and supported by fForTable.fCellUpdatePropationUpdater
 *	and hitting it in the CellRep::AboutTo/DIdUpdate calls. Used the AllowUpdateInfoPropagationContext to make this
 *	conditioanl for performance reasons (test T3 from SPR#1496 went from 4 secs to 10 just always propagating,
 *	but stayed at 4 with this conditional code)
 *	
 *	Revision 2.244  2003/05/20 23:57:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.243  2003/05/20 23:40:28  lewis
 *	qNameLookupInBaseClassWhenItIsNestedSometimesFailsBug
 *	
 *	Revision 2.242  2003/05/20 23:24:46  lewis
 *	small cleanups and use LED_DECLARE_USE_BLOCK_ALLOCATION() for some of the new table command objects
 *	
 *	Revision 2.241  2003/05/20 23:10:17  lewis
 *	SPR#1493: react to changes in Undo support (InteractiveUndoHelperMakeTextRep etc). Mostly SPR#1422: support
 *	undo for tables (which is what inspired most of the work for SPR#1493). Added class SavedTextRepWSel to
 *	restore table selections (along with its new ApplySelection () method). Override 
 *	EmbeddedTableWordProcessor::PostInteractiveUndoPostHelper () etc. TableCMD class to be able to find right
 *	table object to set selections on. TemporarilyAllocateCellWithTablet and a couple related classes/methods
 *	take captureChangesForUndo flag.WordProcessor::InteractiveUndoHelperMakeTextRep () uses SavedTextRepWSel
 *	to preseve table selection. Lots of misc changes to various SavedTextRep classes etc to support this big
 *	change
 *	
 *	Revision 2.240  2003/05/15 12:54:12  lewis
 *	SPR#1487: make OnSelectedEmbeddingExtendedCommand return bool (so I can tell if performed) and revise
 *	Table::OnUpdateCommand/OnPerformUpdate code so if embedded table doesn't enable the kSelectedEmbeddingProperties_CmdID
 *	(then we do - and if it doesn't perform the command - then we do. This way - embedded URLs and pictures
 *	(and eventually other tables) take precendence on an innermost first basis, but if there is none - then
 *	the first table inside-out gets a crack. This makes it more intuiitive for users to click in a table
 *	cell and then still see the table properties command enabled.
 *	
 *	Revision 2.239  2003/05/13 20:44:49  lewis
 *	SPR#1399: progress checking on table getproperties dialog support
 *	
 *	Revision 2.238  2003/05/12 17:17:43  lewis
 *	SPR#1401: Added GetDialogSupport ().AddNewTableDialog () and Led_StdDialogHelper_AddNewTableDialog.
 *	
 *	Revision 2.237  2003/05/08 00:07:19  lewis
 *	SPR#1467: added kSelectTableIntraCellAll_CmdID
 *	
 *	Revision 2.236  2003/05/07 21:11:01  lewis
 *	SPR#1467: implemented various select commands (word/row/paragraph/table/cell/row/col)
 *	
 *	Revision 2.235  2003/05/06 00:58:07  lewis
 *	SPR#1464: lose EmbeddedTableWordProcessor::SetSelection () override and grab selection in 
 *	EmbeddedTableWordProcessor::SaveMiscActiveFocusInfo () instead
 *	
 *	Revision 2.234  2003/05/06 00:26:57  lewis
 *	SPR#1452: fix bug with table reading with diff #s of cols per row (added optional arg to InsertRow)
 *	
 *	Revision 2.233  2003/05/05 13:26:39  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.232  2003/05/02 15:54:28  lewis
 *	SPR#1459: fixed remaining update problem by adding SuppressCellUpdatePropagationContext class and fixing 
 *	SupressRefreshCalls code to call fWP.NoteWindowPartiallyUpdated () when done with a supress context
 *	
 *	Revision 2.231  2003/05/01 23:24:44  lewis
 *	SPR#1456: Added DisableRefreshContext to EmbeddedTableWordProcessor code so we can supress refresh 
 *	from RestoreMiscActiveFocusInfo code
 *	
 *	Revision 2.230  2003/05/01 22:15:34  lewis
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
 *	Revision 2.229  2003/05/01 20:06:41  lewis
 *	SPR#1396: added RTF writing support for table spacing and margins
 *	
 *	Revision 2.228  2003/05/01 01:18:20  lewis
 *	SPR#1396: Added RTF reading support for table cell spacing (trspd{t,l,b,r}). Not perfect (see SPR#1454).
 *	
 *	Revision 2.227  2003/04/30 22:01:30  lewis
 *	SPR#1396: Added RTF READING support for trpadd{b,l.,t.r} and \trgaph tags which specify table margins.
 *	Fully supported in WP class SinkStream as well. Now reading these margins in from existing files looks
 *	fine. Had to add PRELIMINARY \trleft support to reader to make this work fully. Added new SPR#1453
 *	to take care of \trleft spinnoff issue.
 *	
 *	Revision 2.226  2003/04/21 16:17:27  lewis
 *	qCannotStaticlyInitializeConstantsDataMembersInClassDeclaration/qTroubleLookingUpBaseClassInNestedClassDeclaration_VC6
 *	for MSVC60 compat
 *	
 *	Revision 2.225  2003/04/18 22:43:09  lewis
 *	Minor tweeks for MSVC80 (VC++ .Net 2003) compatability
 *	
 *	Revision 2.224  2003/04/18 17:19:18  lewis
 *	small code cleanups to things complained about by new gcc 2.96
 *	
 *	Revision 2.223  2003/04/18 00:11:43  lewis
 *	SPR#1443: add fSavedIntraCellInfoValid flag, and InvalidateIntraCellContextInfo/Save/RestoreIntraCellContextInfo
 *	methods. Call InvalidateIntraCellContextInfo in a few places to avoid accessing invalid info (esp selection -
 *	this was basically SPR#1442, and EmptyStyleSelection - which was SPR#1443)
 *	
 *	Revision 2.222  2003/04/17 19:12:20  lewis
 *	SPR#1396: Added Table::GetCellEditorBounds (), and fully supported (except for IO) cell spacing and margins.
 *	Decently tested.
 *	
 *	Revision 2.221  2003/04/17 16:26:12  lewis
 *	SPR#1396: cleaned up semantics of cell borders etc for tables. Added preliminary cell spacing/margins support
 *	
 *	Revision 2.220  2003/04/17 13:43:46  lewis
 *	SPR#1421: Added fFontSizeChange_Other* strings to WP::CommandNames and used them in
 *	WP::OnUpdateFontSizeChangeCommand ()
 *	
 *	Revision 2.219  2003/04/16 15:16:01  lewis
 *	SPR#1430: added 'cut selected cells' handling support
 *	
 *	Revision 2.218  2003/04/16 14:42:58  lewis
 *	SPR#1437: made OnInsertTableColAfterCommand methods of Table - rather than WordProcessor
 *	
 *	Revision 2.217  2003/04/16 13:13:13  lewis
 *	SPR#1435: added ReValidateSelection support
 *	
 *	Revision 2.216  2003/04/15 22:58:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.215  2003/04/15 22:18:14  lewis
 *	SPR#1425: added RemoveTableRows/Columns command
 *	
 *	Revision 2.214  2003/04/14 14:36:25  lewis
 *	added back WordProcessorTextIOSinkStream (WordProcessor* wp,...) CTOR since used in
 *	AL, and others
 *	
 *	Revision 2.213  2003/04/13 23:07:04  lewis
 *	SPR#1427: add qNestedTablesSupported flag and assure we dont get nested tables and handle
 *	paste of tables inside table cells gracefull
 *	
 *	Revision 2.212  2003/04/12 21:08:14  lewis
 *	SPR#1428: save/restore GetEmptySelectionStyle for embedded Cell WPs
 *	
 *	Revision 2.211  2003/04/12 20:49:07  lewis
 *	SPR#1347: support SetOverwriteTableMode for Internalizer and use in InternalizeBestFlavor(CUR_SELECTION). Now when
 *	you paste - you overwrite table cells instead of blowing the entire table away. Also, fixed bug in various places
 *	that iterated over table cells to handle case of diff # of cells per row (so some rows dont appear in selection
 *	range). And delete unneeded WordProcessorTextIOSinkStream::CTOR overload
 *	
 *	Revision 2.210  2003/04/11 19:56:04  lewis
 *	SPR#1329: Fixed TableIOMapper and surrounding code so WordProcessor:ExternalizeFlavors() only writes the
 *	cells selected in the table (fixes COPY command with table cells selected)
 *	
 *	Revision 2.209  2003/04/10 19:12:37  lewis
 *	SPR#1329- handle passing alot many commands to a REGION of selected table cells. Also -
 *	pass along InteractiveSetFont/AssureCurSelFontCacheValid so that the toolbar displays the
 *	right cached font info
 *	
 *	Revision 2.208  2003/04/10 14:03:31  lewis
 *	SPR#1329: override EmbeddedTableWordProcessor::OnCopy/Paste_Before/After code to pass back to owning WP
 *	
 *	Revision 2.207  2003/04/10 13:09:57  lewis
 *	SPR#1329: Added and used WordProcessor::PassAlongCommandToIntraCellModeTableCell to mark/pass
 *	along certain cmds to the table cells WPs.
 *	
 *	Revision 2.206  2003/04/09 19:15:04  lewis
 *	SPR#1413: Being more careful - and always constructing the EMBWP (except in the layout case)
 *	in WINDOW coords and using the new TableCoordinates2Window/WindowCoordinates2Table routines
 *	as appopriate fixed this problem, and worked fine even with H-scrolling.
 *	
 *	Revision 2.205  2003/04/09 16:49:35  lewis
 *	SPR#1413: Added Table::{TableCoordinates2Window/WindowCoordinates2Table}. Also DOCCOMMENTS
 *	
 *	Revision 2.204  2003/04/09 16:27:42  lewis
 *	SPR#1419: lose Refresh_ and Update_ overloaded virtual overrides from EmbeddedTableWordProcessor
 *	
 *	Revision 2.203  2003/04/09 14:26:32  lewis
 *	SPR#1404: finish adding (basic) editing cell support: added table code and hooked from
 *	WP code CalculateCaretRect and GetCaretShownSituation
 *	
 *	Revision 2.202  2003/04/09 02:04:23  lewis
 *	minor tweeks to get compiling for GCC
 *	
 *	Revision 2.201  2003/04/08 21:45:12  lewis
 *	SPR#1404: more progress on intra table cell editing - keeping and displaying the
 *	selection within a cell, and even handling most mouse clicking within a cell
 *	
 *	Revision 2.200  2003/04/07 22:38:00  lewis
 *	SPR#1403/SPR#1404: working on intra cell selection and OnTypedNormalCharacter
 *	
 *	Revision 2.199  2003/04/07 16:29:40  lewis
 *	SPR#1329: now new Table::OnUpdateCommand/Table::OnPerformCommand hooked into the
 *	command chain (but still do nothing)
 *	
 *	Revision 2.198  2003/04/04 14:41:43  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.197  2003/04/03 16:41:25  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated
 *	command classes, just builtin to TextInteractor/WordProcessor (and instead of template params
 *	use new TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.196  2003/04/01 18:17:58  lewis
 *	SPR#1322: Added 'invalidRect' argument to StyledTextImager::StyleMarker::DrawSegment () and
 *	subclasses to be able to do logical clipping in tables
 *	
 *	Revision 2.195  2003/03/31 22:29:34  lewis
 *	Override WordProcessorCommonCommandHelper<BASECLASS,CMD_INFO,CMD_ENABLER>::OnTypedNormalCharacter ()
 *	to handle tab characters as calling DoIndent (TRUE/FALSE) only under
 *	same circumstances where those same commands would have been enabled
 *	in the menu - if the entire selection is list style.
 *	
 *	Revision 2.194  2003/03/31 20:22:04  lewis
 *	SPR#1389: Lose fColumns - and even fCellX - and keep list of Cells inside the RowInfo record.
 *	GetColumnCount and new SetColumnCount() take ROW# argument - so we can have different# of cols
 *	for diff rows. That made stuff like SplitMergeTableCell () obsolete (and so deleted). Seems to
 *	be working pretty well - at least as well as before
 *	
 *	Revision 2.193  2003/03/28 17:15:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.192  2003/03/27 15:49:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.191  2003/03/27 00:53:35  lewis
 *	SPR#1369 - add CF_SCALABLEONLY to ::ChooseFont () dialog call to prevent bad font choices
 *	
 *	Revision 2.190  2003/03/21 13:59:39  lewis
 *	SPR#1364- fix setting of drag anchor. Moved it from Led_XXX(Win32/MacOS/Gtk) to
 *	TextInteractor::ProcessSimpleClick (adding extra arg). Also lots of other small
 *	code cleanups
 *	
 *	Revision 2.189  2003/03/20 21:49:47  lewis
 *	SPR#1365 - Fix WordProcessor::WPIdler::SpendIdleTime so that when no tables invalidated -
 *	we disable the idle task (sort of)
 *	
 *	Revision 2.188  2003/03/20 20:58:32  lewis
 *	SPR#1363- Implement WordProcessor::DrawBefore ()
 *	
 *	Revision 2.187  2003/03/20 16:28:46  lewis
 *	fix problems with gcc compilation
 *	
 *	Revision 2.186  2003/03/20 16:22:18  lewis
 *	fix small problems with gcc compilation
 *	
 *	Revision 2.185  2003/03/20 15:59:59  lewis
 *	SPR#1360- lose OnEnterIdleCallback hook for doing PerformLayout and replace it
 *	with (a still preliminary) WPIdler::SpendIdleTime approach
 *	
 *	Revision 2.184  2003/03/19 18:31:05  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.183  2003/03/19 13:54:49  lewis
 *	SPR#1349- added fTotalHeight to Table object, and SPR#1351 - LayoutFlag simplification,
 *	and SPR#1352: LED_DECLARE_USE_BLOCK_ALLOCATION (CellRep)
 *	
 *	Revision 2.182  2003/03/17 20:52:53  lewis
 *	SPR#1348 - don't call Table::LayoutIfNeeded () in MeasureSegmentWidth etc but only from
 *	WordProcessor::OnEnterIdleCallback ().
 *	
 *	Revision 2.181  2003/03/16 16:15:29  lewis
 *	SPR#1345 - as a small part of this SPR - switch to using SimpleUpdater in place of direct calls
 *	to DoAboutToUpdateCalls()
 *	
 *	Revision 2.180  2003/03/15 23:16:45  lewis
 *	SPR#1344 - Fix OnInsertTableColAfterCommand and OnUpdateInsertTableColAfterCommand ETC commands
 *	(before/after and row/col). Fixed to oerate on new notion of table (a single embedding). Fix to
 *	properly call Table::GetSelection.
 *	
 *	Revision 2.179  2003/03/11 19:34:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.178  2003/03/07 19:56:10  lewis
 *	SPR#1330 - Be more careful in one case about grabbing REAL cell instead of boundsrect of mergecell
 *	
 *	Revision 2.177  2003/03/07 19:07:18  lewis
 *	SPR#1324- minor table cleanups - and finished table selection display and basic function.
 *	Still todo is making commands respect this table selection
 *	
 *	Revision 2.176  2003/03/07 00:40:41  lewis
 *	SPR#1324 - got basic cell selection from mouse clicks working. Still not very good (like drag
 *	select not working).
 *	
 *	Revision 2.175  2003/03/06 20:49:01  lewis
 *	SPR#1324- beginnings of Get/SetSelection support for Table
 *	
 *	Revision 2.174  2003/03/04 21:34:26  lewis
 *	SPR#1315 - lose qCacheEMWPInCellRep code - since other changes to PM caching make it unneeded/unhelpful
 *	
 *	Revision 2.173  2003/02/28 21:03:22  lewis
 *	SPR#1314- get rid of memory leak (Table::Row change from earlier only halfway complete) -
 *	now Table::RowInfo
 *	
 *	Revision 2.172  2003/02/28 17:37:48  lewis
 *	SPR#1315- checkin temporarily disabled qCacheEMWPInCellRep cache code.
 *	
 *	Revision 2.171  2003/02/27 15:04:35  lewis
 *	finished SPR#1311 (except defered parts). Now horizontal merge cells work pretty well
 *	
 *	Revision 2.170  2003/02/27 14:18:16  lewis
 *	SPR#1311- Lots more cleanups to table handing to support horizontally merged cells.
 *	Reading and writing of them now mostly works
 *	
 *	Revision 2.169  2003/02/26 20:46:52  lewis
 *	SPR#1311- progress supporting reading RTF from tables for mergecells. Works for some easy cases
 *	
 *	Revision 2.168  2003/02/26 15:01:57  lewis
 *	SPR#1311 - work on Merge/UnMergeCells functions
 *	
 *	Revision 2.167  2003/02/25 21:22:39  lewis
 *	SPR#1311 - lots of table code cleanups - back to single embedding style approach.
 *	Beginning support for MERGECELLS
 *	
 *	Revision 2.166  2003/02/24 18:12:05  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.165  2003/02/19 23:37:51  lewis
 *	SPR#1305- start going abck to stategy where ONE CHAR (sentinal) per table and no extras per row.
 *	Roughly stable working with that strategy now - but then lost the scrolling boundary benefit
 *	of the old strategy
 *	
 *	Revision 2.164  2003/02/12 16:39:59  lewis
 *	SPR#1299- a few small fixes to InteractiveModeUpdater/READONLY handling.
 *	
 *	Revision 2.163  2003/02/03 21:02:38  lewis
 *	qTemplateProcessorDoesntConsiderBaseClassIfItsTemplatedBug BWA
 *	
 *	Revision 2.162  2003/01/31 23:08:25  lewis
 *	SPR#1278- added InteractiveUpdadeMode and InteractiveModeUpdater so things like AboutToUpdate()
 *	can tell if its a USER change or a program change to text its about to make (and so whether
 *	or not to allow/disallow)
 *	
 *	Revision 2.161  2003/01/29 19:48:33  lewis
 *	be sure <cstdio> is include NOT inside Led namespace. Not sure why that never caused me any trouble!
 *	
 *	Revision 2.160  2003/01/29 19:15:11  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.159  2003/01/29 17:59:50  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont
 *	REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the 'default font'
 *	
 *	Revision 2.158  2003/01/21 13:26:55  lewis
 *	SPR#1186 - first cut at Indents dialog support (MacOS dialog still not done)
 *	
 *	Revision 2.157  2003/01/17 19:28:54  lewis
 *	get compiling with CW8Pro for MacOS
 *	
 *	Revision 2.156  2003/01/15 18:36:44  lewis
 *	SPR#1211- added WordProcessor::G(S)etSmartQuoteMode and WordProcessor::OnTypedNormalCharacter
 *	to implement it
 *	
 *	Revision 2.155  2003/01/11 19:28:40  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.154  2003/01/07 17:15:56  lewis
 *	use name Table::GetRowCount/GetColumnCount instead of GetRows/GetColumns. Add Table::CollectRowCount method as
 *	part of speed tweeks. Speed tweek Table::CollectRows. Sped up opening RTF1.4 spec.rtf by about 30%,
 *	but still EXTREEMLY slow
 *	
 *	Revision 2.153  2002/12/02 15:57:57  lewis
 *	SPR#1185 - move DrawRow,DrawRowSegments,DrawRowHilight,DrawInterlineSpace from MRTI/SimpleTextImager to TextImager
 *	(share code. Changed their args slightly. For BIDI work.
 *	
 *	Revision 2.152  2002/11/27 15:58:34  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 2.151  2002/11/27 15:13:51  lewis
 *	As part of SPR#1183 - BIDI project- used TextLayoutBlock class instead of Led_tChar* as arg to DrawRow
 *	and DrawRowSegments() routines. Much more of this sort to come! NB: NOT BACKWARD COMPAT!
 *	
 *	Revision 2.150  2002/11/21 14:09:13  lewis
 *	SPR#1180 - got rid of RowInfo and fRows array. Compute dynamically with CollectAllMarkersInRange.
 *	Gets rid of crasher bug on destroy (cuz of random order of destrunction of table vs. tablerows).
 *	No obvious negative performance impact (though need to profile!!!)
 *
 *	Revision 2.149  2002/11/20 17:35:50  lewis
 *	Added qStaticConstInitializerInClassDeclarationNYIBug and qUsingMemberNameToOverloadInTemplateClassBug
 *	bug workarounds
 *	
 *	Revision 2.148  2002/11/19 19:51:35  lewis
 *	SPR#1176 - Added insert row/col before/after commands and a few minor table fixups which were releated
 *	(not 100% - see spr#1177/78 for details)
 *	
 *	Revision 2.147  2002/11/19 17:55:40  lewis
 *	SPR#1175 kTryToIncorporateInternalizeIntoAdjacentTable support (PRELIM) and fix crash on failed add
 *	of new table (as in cuz in middle of an existing table in old code)
 *	
 *	Revision 2.146  2002/11/19 14:04:56  lewis
 *	misc cleanups
 *	
 *	Revision 2.145  2002/11/19 13:15:05  lewis
 *	SPR#1174 - major change to table code. Add a new Table::Row class and make IT the embedding, and make
 *	Table class simply wrap a bunch of Table::Rows. Not 100% solid, and needs some cleanups, but seems to
 *	be pretty functional now
 *	
 *	Revision 2.144  2002/10/31 13:33:58  lewis
 *	added missing ShowAddURLEmbeddingInfoDialog () default implementation
 *	
 *	Revision 2.143  2002/10/25 17:16:10  lewis
 *	SPR#1146 - Added 'Insert Symbol' menu item and fix Led_MFC D&D code so it worked with
 *	CharacterMap.exe (for some wierd reason - I need to hack it to pass along the full FORMATC???).
 *	
 *	Revision 2.142  2002/10/23 18:05:07  lewis
 *	small tweeks to get compiling on GCC
 *	
 *	Revision 2.141  2002/10/22 00:38:53  lewis
 *	Add URL context/menu command - SPR#1136
 *	
 *	Revision 2.140  2002/10/21 16:21:35  lewis
 *	SPR#1133 - more cleanups of new WPPartition code and PMCacheMgr etc. Still more todo
 *	
 *	Revision 2.139  2002/10/21 12:55:48  lewis
 *	SPR#1135 - Add SummarizeFontAndColorTable method to TextIO::SrcStream and use that to generate
 *	RTF color and font tables
 *	
 *	Revision 2.138  2002/10/20 19:38:10  lewis
 *	SPR#1128 and SPR#1129 - BIG change. Get rid of MultiRowPartitionMarker and MultiRowPartition.
 *	Still coded very slopily (with idefs) and needs major cleanups/tweeks. But - its now functional
 *	and testable
 *	
 *	Revision 2.137  2002/10/14 23:37:39  lewis
 *	SPR#1126 - LOTS of work (very ugly right now and MAYBE still buggy) to support new partition code so
 *	that tables apepar in other own partition element. Added PartHELPER<> and WordProcessor::WPPartition and 
 *	XXWPPartition. ALSO - fixed SPR#1127- UndoableContextHelper for insert embedding
 *	
 *	Revision 2.136  2002/10/11 01:04:53  lewis
 *	getter/setter for table border width and (SB) paying attention to borderwidth setting from RTF
 *	
 *	Revision 2.135  2002/10/09 14:01:13  lewis
 *	SPR#1121- add cellRep to owning TextStore as markerOwner, and then override its DidUpdateText()
 *	call to do InvalidateLayout on owning fForTable (and call SimpleUpdater to get partitionmarkers cache invalidated)
 *	
 *	Revision 2.134  2002/10/08 16:37:59  lewis
 *	SPR#1119- Added CheckForSplits/NeedToCoalesce to LineBasedPartition. Add WordProcessor::WPPartition.
 *	Add / use MakeDefaultPartition () so we create the new partition for WPs.
 *	
 *	Revision 2.133  2002/10/08 01:10:23  lewis
 *	cleanups to paragraphdatabaserep
 *	
 *	Revision 2.132  2002/10/08 00:49:24  lewis
 *	SPR#1118- paragraphdatabase now uses abstractparagraphdatabase api instead of public inherit from MarkerCover<>
 *	
 *	Revision 2.131  2002/09/28 18:12:05  lewis
 *	SPR#1116 - progress on rtf writing for embedded tables
 *	
 *	Revision 2.130  2002/09/23 22:13:27  lewis
 *	SPR#1111 - Fixed bug where we didn't process cellx specs for first row, and added clcbpat
 *	(cell abackground color) support. VERY primitive cruddy implementation. Cleanup soon.
 *	
 *	Revision 2.129  2002/09/23 15:06:52  lewis
 *	fix missing friend declarations
 *	
 *	Revision 2.128  2002/09/22 21:05:18  lewis
 *	SPR#1109- New InvalidateLayout/PerformLayout code. New TemporarilySetOwningWP class to help you know who
 *	is the table owner, and use that to compute fitted row-heights. Primitive, inefficient implemenation,
 *	which also doesn't take into account 'preferred-height-type' rows and much more - but a good start
 *	
 *	Revision 2.127  2002/09/22 16:25:24  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.126  2002/09/22 15:48:29  lewis
 *	DrawsBorders takes extra WP argument so it (and drawCEllCode) can use it to take into account imager->GetHScrollPos()
 *	from owning image to draw in the right place when horizontally scrolled (SPR#1108)
 *	
 *	Revision 2.125  2002/09/21 20:38:19  lewis
 *	change table code so Cell stores a styledatabase etc instead of a WP, and we dynamically create teh WP on an
 *	as-needed basis. This saves a HUGE amount of RAM. Plus - use SimpleTextStore instead of whatever textstore
 *	is being used by the main WP. This ALSO saves a huge amount of RAM (and should be fine performance wise since
 *	I cannot imagine a very large cell). (SPR#1105)
 *	
 *	Revision 2.124  2002/09/20 22:59:24  lewis
 *	add EmbeddedTableWordProcessor::GetDesiredHeight () in prep for proper row-height computation.
 *	EmbeddedTableWordProcessor::GetLayoutMargins () added to (roughly) get margins for cell WP's right.
 *	Added TemporarilyUseOwningWP.
 *	
 *	Revision 2.123  2002/09/20 15:09:34  lewis
 *	lots of name scoping cleanups and bug workarounds to get Led compiling on MWERKS and GCC and Borland -
 *	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug/qGCC_MiscNestedClassNameLookupInTemplateBug/
 *	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug)
 *	
 *	Revision 2.122  2002/09/20 13:03:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.121  2002/09/20 00:27:13  lewis
 *	fix friend bug (only showed up on MWERKS)
 *	
 *	Revision 2.120  2002/09/19 23:39:29  lewis
 *	qAccessChecksSometimesBreakForNestedClasses for Borland C++ to work
 *	
 *	Revision 2.119  2002/09/19 15:30:35  lewis
 *	get working on VC++6 - added qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
 *	
 *	Revision 2.118  2002/09/19 14:14:24  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.117  2002/09/18 04:02:47  lewis
 *	more progress on tables - now support cellx to specify cell widths, and more related stuff -
 *	at least PARTLY supported
 *	
 *	Revision 2.116  2002/09/17 16:20:50  lewis
 *	SPR#1095 (table support) - got BASIC RTF reading working (right # cells and rows and contents read in;
 *	but no attributes like color, size etc for cells
 *	
 *	Revision 2.115  2002/09/14 01:33:54  lewis
 *	Table support now works decently. I think Add/Remove Row/Column now create WP cells appropraite. I hacked in
 *	demo code that set background colors and text for each cell. Fixed bug with layout. Now looks very good.
 *	Next stage - I think - is to add RTF support. And after that - editing support.
 *	
 *	Revision 2.114  2002/09/13 15:31:05  lewis
 *	more table support - now at the point where the cells all display, and a single embedded WP object
 *	displays in a particular cell
 *	
 *	Revision 2.113  2002/09/11 04:20:22  lewis
 *	SPR#1094- added VERY preliminary Table support - table class and new InsertTable command
 *	
 *	Revision 2.112  2002/05/06 21:33:40  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.111  2001/11/27 00:29:48  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.110  2001/10/20 13:38:58  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.109  2001/10/18 13:18:43  lewis
 *	cleanup DocComments
 *	
 *	Revision 2.108  2001/10/17 20:42:55  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.107  2001/10/17 01:52:30  lewis
 *	DocComments
 *	
 *	Revision 2.106  2001/09/26 15:41:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.105  2001/09/24 16:28:59  lewis
 *	SPR#0993- made InvalidateAllCaches virtual and override in WordProcessorHScrollbarHelper<BASECLASS>
 *	as part of attempt to fix quirky mac sbar code on threshold between showing/hiding scrollbars
 *	
 *	Revision 2.104  2001/09/19 14:43:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.103  2001/09/12 17:23:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.102  2001/09/05 23:48:04  lewis
 *	SPR#1014- hook new SinkStream::EndOfBuffer () to detect EOB. Added flag IgnoreLastParaAttributes
 *	with WordProcessorTextIOSinkStream. Then set that when we create a sinkstream for D&D/CopyPaste
 *	etc (file IO creates its own). Then - fix flush code to clip the specification of paragraph
 *	info just past the last NL if at EOB and with that flag set. Also - fixed side bug I noticed
 *	at the same time - that we were missing default settings for the SpaceBefore/After attributes
 *	in the WordProcessorTextIOSinkStream
 *	
 *	Revision 2.101  2001/09/05 00:01:14  lewis
 *	SPR#1011- added WordProcessor::EmptySelectionParagraphSavedTextRep to fix problem with
 *	undo after empty selection not preserving paragraph info
 *	
 *	Revision 2.100  2001/09/03 19:14:31  lewis
 *	SPR#0660,SPR#0762- rewrite much of the MarkerCover<> code to be simpler, clearer, and faster
 *	
 *	Revision 2.99  2001/08/29 23:01:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.98  2001/08/28 18:43:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.97  2001/08/16 18:53:42  lewis
 *	SPR#0959- more work on 'GetInfo' support for embeddings.
 *	
 *	Revision 2.96  2001/07/31 15:15:51  lewis
 *	static_cast<> to silence MSVC70B2 compiler warning
 *	
 *	Revision 2.95  2001/07/20 00:54:56  lewis
 *	SPR#0959- fix GetInfo cmd support / Led_URLD for FULL_UNICODE builds
 *	
 *	Revision 2.94  2001/07/19 23:43:40  lewis
 *	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug support
 *	
 *	Revision 2.93  2001/07/19 19:54:20  lewis
 *	lost :16 on fListStyle type declaration cuz makes MWERK compiler unhappy
 *	with treat enum as int option on (needed for netledit)
 *	
 *	Revision 2.92  2001/07/17 19:09:37  lewis
 *	SPR#0959- added preliminary RightClick-to-show-edit-embedding-properties support
 *	
 *	Revision 2.91  2001/07/13 18:57:02  lewis
 *	SPR#0906- got basic (rtf only for now) bullet list support working
 *	
 *	Revision 2.90  2001/07/12 23:06:08  lewis
 *	SPR#0906- support reading indent levels in RTF
 *	
 *	Revision 2.89  2001/07/12 22:42:32  lewis
 *	SPR#0906- Mostly implement RTF reading for bullet-lists. Noticed (but not yet
 *	fixed) SPR#0952.
 *	
 *	Revision 2.88  2001/07/12 13:55:27  lewis
 *	SPR#0906- hack override WordProcessor::PostReplace() so typing at end of buffer with
 *	listmode on gets new empty lines bullet drawn/erased
 *	
 *	Revision 2.87  2001/07/11 21:51:02  lewis
 *	SPR#0906- Partly implemented bullet/list support. Just did basic support (bullet lists,
 *	no other style lists). Did support indent levels (including new Increase/Decrease indent
 *	level commands). Still no RTF/HTML support, and a small display bug on following lines
 *	when I add bullet attribute.
 *	
 *	Revision 2.86  2001/05/29 22:59:49  lewis
 *	added new color names (kLimeGreen/Fusia/Aqua) to rationalize with names we use elsewhere.
 *	Fix what WordPRocessor cmdnames/color coordination. Sync with 'Web Design in a Nutshell' -
 *	HTML reference name/values. Part of SPR#0943
 *	
 *	Revision 2.85  2001/05/24 14:16:41  lewis
 *	SPR#-939- fix proper checking of color items in color menu, and handle case where no
 *	common color in users selection, and then you invoke a color-setting command
 *	
 *	Revision 2.84  2001/05/15 16:41:19  lewis
 *	add more colors to default colors handled in FontColorToCmd() etc functions (so we get same
 *	set of colors on Mac/Win/Linux)- no longer system dep
 *	
 *	Revision 2.83  2001/05/09 22:39:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.82  2001/04/30 19:21:00  lewis
 *	vaguely related to SPR#0885- implement more default (assert out - but at least link)
 *	implementations of methods for WordProcessorCommonCommandHelper_DefaultCmdImpl<BASECLASS>.
 *	Helpful cuz then programmers can avoid implementing them when the dont need to and avoid
 *	the calls by just setting the CMD# to 0.
 *	
 *	Revision 2.81  2001/04/27 15:23:54  lewis
 *	fix Led_RequireNotNil -> Led_Require in a couple places
 *	
 *	Revision 2.80  2001/01/03 14:48:15  Lewis
 *	use template<> specifier - required by BorlandC++
 *	
 *	Revision 2.79  2000/10/23 01:10:05  lewis
 *	must use typename in a few templates to get compiling with GCC
 *	
 *	Revision 2.78  2000/10/18 20:39:55  lewis
 *	Added Led_StdDialogHelper_FindDialog support. Works on Windows and prelim for Mac
 *	
 *	Revision 2.77  2000/10/16 00:14:53  lewis
 *	call new Led_CenterWindowInParent() instead of inlining code
 *	
 *	Revision 2.76  2000/10/04 18:29:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.75  2000/10/04 15:45:40  lewis
 *	finish with putting into WordProcessorCommonCommandHelper_DefaultCmdImpl<BASECLASS>
 *	the default impl methods that can reasonably go there (choosefont/color, font2/fromcolorcmd,
 *	etc). Related to SPR#0839
 *	
 *	Revision 2.74  2000/10/04 13:18:41  lewis
 *	work on embellishing WordProcessorCommonCommandHelper_DefaultCmdImpl<BASECLASS> class.
 *	Still much more todo
 *	
 *	Revision 2.73  2000/10/03 21:50:19  lewis
 *	Lots more work relating to SPR#0839- moved stuff into WordProcessorCommonCommandHelper<> template,
 *	and used that better throughout Led_PP code etc. Broke out WP-specific stuff from Led_PP to
 *	separate file.Lots of cleanups of PP wrapper code
 *	
 *	Revision 2.72  2000/10/03 13:36:36  lewis
 *	SPR#0839- reorganize the WordProcessorCommonCommandHelper/TextInteractorCommonCommandHelper<> code.
 *	Now MFC wrapper support MUCH simpler, and all this stuff much easier to use from other platforms.
 *	OnPerformCommand/OnUpdateCommand simple virtual overrides, and _MSG simple single msg hooks for MFC.
 *	
 *	Revision 2.71  2000/09/30 19:35:01  lewis
 *	Added TextInteractorCommandHelper_DefaultCmdInfo/TextInteractorCommonCommandHelper<BASECLASS,CMD_INFO,CMD_ENABLER>.
 *	Added TextInteractor::OnSelectAllCommand. Moved Led_MFC_TmpCmdUpdater to Led_MFC.h
 *	
 *	Revision 2.70  2000/08/31 19:52:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.69  2000/08/28 20:24:24  lewis
 *	SPR#0825- Handle UNDO better of hide/unhide selection stuff.  SPR#0826- new class
 *	WordProcessorCommonCommandHelper_DefaultCmdInfo. new class WordProcessor::CommandNames (SPR#0827)
 *	
 *	Revision 2.68  2000/08/13 04:15:05  lewis
 *	SPR#0784- rewrite hidden text code based on setting segwidth to zero, and patching
 *	computeNExtCursorPosition. Not 100% done - but mostly. VERY big change -  and many
 *	bug fixes related to this hidden text stuff folded in as well - but not complete
 *	
 *	Revision 2.67  2000/06/17 07:47:08  lewis
 *	Added WordProcessor::MeasureMinSegDescent () to help bugs with Led_LineSpacing::eExactTWIPSSpacing
 *	text - and the WordProcessor::MeasureSegmentBaseLine () code. This was uncovered as a result
 *	of SPR#0792- but also pertains to SPR#0760 (and uses a file - from that SPR folder to reproduce it)
 *	
 *	Revision 2.66  2000/06/16 19:48:07  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.65  2000/06/15 20:03:32  lewis
 *	SPR#0785
 *	
 *	Revision 2.64  2000/06/13 22:15:22  lewis
 *	SPR#0785- revised InteractiveSetParagraphSpacing API. Added
 *	WordProcessorCommonCommandHelper<BASECLASS,CMD_INFO,CMD_ENABLER>::OnParagraphSpacingChangeCommand ().
 *	Added kParagraphSpacingCommand_CmdID(WHICH USERS MSUT SPECIFY). And require new
 *	CMD_INFO::PickNewParagraphLineSpacing() method (to implement said)
 *	
 *	Revision 2.63  2000/06/13 21:17:50  lewis
 *	SPR#0785- added InteractiveSetParagraphSpacing- infrastructure for UI for paraspacing setting
 *	
 *	Revision 2.62  2000/06/13 20:13:54  lewis
 *	hack to get compiling with GCC/Linux
 *	
 *	Revision 2.61  2000/06/12 22:49:02  lewis
 *	SPR#0767- more work converting stuff (\fi\ri\li\margX etc) to TWIPS - instead of pixels. Tested
 *	a snapshot at this point - and LedItMFC seemed to work OK (thats all I've tested so far)
 *	
 *	Revision 2.60  2000/06/12 20:11:39  lewis
 *	SPR#0760- convert code to do SpaceBefore/AfterLineSpacing to using new Led_TWIPS and
 *	Led_LineSpacing structs - rather than using PIXELS
 *	
 *	Revision 2.59  2000/06/12 16:25:14  lewis
 *	convert SpaceBefore to be saved in Led_TWIPS - instead of pixels. Soon todo much
 *	more like this- SPR#0767
 *	
 *	Revision 2.58  2000/05/30 22:53:02  lewis
 *	SPR#0760- added preliminary support (all but RTF) for SpaceAfter/Before/BetweenLines support
 *	
 *	Revision 2.57  2000/05/30 22:04:29  lewis
 *	SPR#0760- Added preliminary support for word-processor line spacing. RTF reader support,
 *	and internal display support done. Still todo - RTF Writer support, and GUI for setting
 *	common spacing values
 *	
 *	Revision 2.56  2000/05/02 00:31:21  lewis
 *	fix GetContinuous_ to handle TextColor () - and fix OnUpdateUICOmmandFOrColor() to
 *	call AssureCurSelFotnCacheValid - SPR#
 *	
 *	Revision 2.55  2000/04/25 23:00:48  lewis
 *	more work on SPR#0731 and SPR#0732- must apply Hidden stuff at the end - not in the flush -
 *	and even THAT may not be late enough. And must re-call AssureColorTableBuilt_FillIn/FONTTABLEONE
 *	TOO for sub-call to reader - cuz that changes avialable colors/fonts (if diff ones used in
 *	hidden text than in the main body text)
 *	
 *	Revision 2.54  2000/04/25 22:01:43  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.53  2000/04/25 16:16:29  lewis
 *	partial work on SPR#0722- added HidableTextMarkerOwner::GetHidableRegionsWithData () and
 *	started using that.
 *	
 *	Revision 2.52  2000/04/24 22:29:32  lewis
 *	added @TextInteractor::HookExternalizerChanged - and use it in WordProcessor to sync up
 *	with the hidableText database - related to SPR#0724
 *	
 *	Revision 2.51  2000/04/24 21:33:32  lewis
 *	fix const typo (only got warning on MWEKRS/Mac
 *	
 *	Revision 2.50  2000/04/24 16:47:02  lewis
 *	Very substantial change to FlavorPackage stuff - now NOT mixed into TextInteractor - but
 *	simply owned using Led_RefCntPtr<>. Also - did all the proper subclassing in WP class etc
 *	to make this work (almost - still must fix OLE embeddings) as before. SPR# 0742
 *	
 *	Revision 2.49  2000/04/15 14:32:37  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.48  2000/04/14 22:40:23  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.47  2000/03/31 23:45:36  lewis
 *	SPR#0717- got first cut (imperfect) at saving hidableText in RTF (problem is assumes its
 *	currently being shown). Other cleanups - esp to hidable text code (invariants)
 *	
 *	Revision 2.46  2000/03/31 00:50:04  lewis
 *	more work on SPR#0717: support RTF reading of \v tags - hidden text. Integrated HiddenText
 *	support into WordProcessor class (in a maner patterend after how we treat WordProcessor::ParagraphDatabasePtr
 *	
 *	Revision 2.45  1999/12/28 17:06:20  lewis
 *	make command methods of WordProcessorCommonCommandHelper<> virtual - so they can easily
 *	be hooked/replaced while still giving users good default behavior
 *	
 *	Revision 2.44  1999/12/28 13:08:41  lewis
 *	use if/else skipchain instead of swtich statement so some CMD_INFO guys can be set to
 *	sentinal value meaning cmd-not-supported
 *	
 *	Revision 2.43  1999/12/28 11:31:27  lewis
 *	SPR#0669- finished it - by adding DrawSegment() override to draw the tab glyph
 *	
 *	Revision 2.42  1999/12/27 17:31:35  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command
 *	is autohandled by WordProcessorCommonCommandHelper<> template. And adjusted the
 *	characters output (at least for UNICODE).
 *	
 *	Revision 2.41  1999/12/27 16:28:36  lewis
 *	renamed WordProcessorSelectionCommandHelper<> -> WordProcessorCommonCommandHelper<>
 *	
 *	Revision 2.40  1999/12/27 16:03:59  lewis
 *	SPR#0667- SoftLineBreak support
 *	
 *	Revision 2.39  1999/12/21 03:36:52  lewis
 *	SPR#0669- Preliminary version of ShowHiddenCharacters API implemeantion.
 *	
 *	Revision 2.38  1999/12/14 18:07:05  lewis
 *	use typename in templates so compiles with GCC
 *	
 *	Revision 2.37  1999/12/12 16:16:20  lewis
 *	spr#0651 - added in OnChooseFontCommand () to WordProcessorSelectionCommandHelper/
 *	WordProcessorSelectionCommandHelper_MFC templates (not used on Mac/PP)
 *	
 *	Revision 2.36  1999/12/09 03:24:23  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char).
 *	And fix bug where I forgot to call enabler->SetEnabled (true) for OTHERFONTSIZE command.
 *	
 *	Revision 2.35  1999/12/08 17:45:36  lewis
 *	reverse args to WordProcessorSelectionCommandHelper<> template so we can make
 *	subclass with default args in Led_MFC.h etc.
 *	
 *	Revision 2.34  1999/12/08 03:19:00  lewis
 *	use new Led_FontSpecification::FontNameSpecifier as return value for CmdNumToFontName
 *	
 *	Revision 2.33  1999/12/07 23:30:23  lewis
 *	add support to WordProcessorSelectionCommandHelper<BASECLASS,CMD_ENABLER,CMD_INFO>
 *	for mac-specific styles
 *	
 *	Revision 2.32  1999/12/07 21:02:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.31  1999/12/07 20:09:29  lewis
 *	Add WordProcessorSelectionCommandHelper<BASECLASS,CMD_ENABLER,CMD_INFO> (based on
 *	LedItMFC/LedItView code) - to address part of SPR#0606
 *	
 *	Revision 2.30  1999/11/13 16:32:20  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.29  1999/07/16 21:06:24  lewis
 *	move some code from LedItMFC\LedItView to the template WordProcessorHScrollbarHelper<>,
 *	so that it can be shared in ActiveLedIt. REALLY - we should do much more of this sharing
 *	of code, but this is a start
 *	
 *	Revision 2.28  1999/07/13 22:39:16  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various BugWorkArounds that
 *	were only needed for these old compilers
 *	
 *	Revision 2.27  1999/06/29 01:48:52  lewis
 *	several subtle changes to the MarkerCover<> NeedExtraUpdateCheckMode caching code -
 *	see spr#0599 for details
 *	
 *	Revision 2.26  1999/05/03 22:05:05  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.25  1999/03/24 22:59:14  lewis
 *	spr#0534 - mostly fixed - add InteractiveSETXXX methods which do the undo wrapper code -
 *	though not well
 *	
 *	Revision 2.24  1999/03/24 16:23:31  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.23  1999/03/24 15:47:22  lewis
 *	spr#0561 fixed- TexImager::NoTabletAvailable now subclasses from
 *	TextImager::NotFullyInitialized etc
 *	
 *	Revision 2.22  1999/03/22 19:41:54  lewis
 *	spr#0560 - override WordProcessor::ResetTabStops to take into account margins properly
 *	
 *	Revision 2.21  1999/03/10 23:21:59  lewis
 *	add STLDefCTORDeclare_BWA so it will compile on the mac
 *	
 *	Revision 2.20  1999/03/10 19:00:15  lewis
 *	fix spr#0558- WordProcessorTextIOSinkStream::DTOR() must call flush cuz base class
 *	version DTOR calls flush but binds to base class version of FLUSH cuz of quirk of
 *	how DTOR/vtables work
 *	
 *	Revision 2.19  1999/03/10 17:19:32  lewis
 *	MAJOR speedup to reading RTF(spr#0546). Lots of code cleanups to the WordProcessorTextIOSinkStream code.
 *	But biggest change is to override AppendText() and simply CACHE the  fSavedParaInfo records entirely,
 *	and write them all in the FLush(). Turns out to be much simpler that what i had before, and almost
 *	identical to what we do in base class for style info. And MUCH faster cuz many fewer
 *	(maybe just one?) Flush.
 *	
 *	Revision 2.18  1999/02/21 21:54:37  lewis
 *	add WordProcessor::ParagraphDatabaseRep::GetStaticDefaultParagraphInfo and call from
 *	CTOR of paragraphdatabase, so we specify default as arg to base class and avoid SetInfo
 *	call which was causing undesirable (though not wrong) DidUpdates and therefore setdirty
 *	calls (spr#0550)
 *	
 *	Revision 2.17  1999/02/21 20:04:16  lewis
 *	override Writer::SRC::GetFirstIndent/GetMargins() calls so we can write out RTF /
 *	paragraph margin information
 *	
 *	Revision 2.16  1999/02/21 13:56:23  lewis
 *	Added SetFirstIndent/SetLeftMargin/SetRightMargin/RTF\li\ri\fi, and amended\pard support
 *	for these RTF tags (READING ONLY). Works (in preliminary testing), but with a few small
 *	sprs noted in dbase (validation and fixing defaults)
 *	
 *	Revision 2.15  1999/02/10 16:19:59  lewis
 *	added wordprocessor::Get/Set/Margins/FirstIndent methods (maybe want to rethink these apis?)
 *	
 *	Revision 2.14  1999/02/09 16:35:06  lewis
 *	added more(nearly all) support for margins, and first indent. Added support to ParagraphInfo
 *	object. Use it from GetMarginInfo WordWrappedTextImager call. Put in more decent default-value
 *	code for margins. Support chaning Partition of ParagrapDatabaseRep better (resync). Some DOCS. More?
 *	
 *	Revision 2.13  1999/02/08 22:30:20  lewis
 *	remove some tmphacks I had put in (qsupportoldgetlayoutwidthgovers=1 etc) so I could get
 *	2.3b3/4 out the door without it all working/thought out. Now really fix things
 *	
 *	Revision 2.12  1998/10/30 14:14:45  lewis
 *	Lots of cleanups, including ongoing work on supporting margins.
 *	Also - cleanup use of nested classes due to new fixes in compilers.
 *	(msvc60)
 *	MSVC60 compiler warnign support cahnged.
 *	GetLayoutMargin instead of GetLayoutWidth() change.
 *	More?
 *	
 *	Revision 2.11  1998/04/25  01:27:09  lewis
 *	Finished support for IncrementalParagraphInfo - by adding 'valid' bools, and
 *	fixing Mergin to use them.
 *	Added support for GetTabStopList with ParagraphInfo markers - so now we
 *	fully support RTF-style tabstops (well - at least simple
 *	kind).
 *
 *	Revision 2.10  1998/04/09  01:29:22  lewis
 *	Docs
 *	And override SetInfo() to fix/cleanup auto-alignment of Info markers to partitions
 *
 *	Revision 2.9  1998/04/08  01:42:31  lewis
 *	Support for new PartitionPtr (separable partitions).
 *	WordProcessor::WordProcessorTextIOSrcStream and WordProcessor::WordProcessorTextIOSinkStream
 *	(remodularized StyledTextIO code).
 *
 *	Revision 2.8  1998/03/04  20:15:53  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1997/12/24  03:28:37  lewis
 *	Playing with DidUpdateText_CheckMarkerBounaryConstraints
 *	Still not adequate to solve the problem - try again!!! Must fix for 2.3 release!!!
 *
 *	Revision 2.5  1997/09/29  14:56:13  lewis
 *	new per-paragraph support for justification, in new WordProcessor class.
 *	Use new MarkerCover template.
 *	And use new Led_RefCntPtr code to maintain ParagraphDatabaseRep.
 *	Redid StyledDatabase to use same mechanism.
 *
 *	Revision 2.4  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.3  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.2  1997/07/12  20:07:10  lewis
 *	AutoDoc support.
 *
 *	Revision 2.1  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/06/18  02:44:24  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== CODE MOVED HERE FROM TextInteractorMixins.h ==========>
 *
 *
 *
 */

#if		qMacOS
	#include	<ColorPicker.h>
#endif

#include	"HiddenText.h"
#include	"IdleManager.h"
#include	"MarkerCover.h"
#include	"StandardStyledTextInteractor.h"
#include	"WordWrappedTextInteractor.h"
#include	"TextInteractorMixins.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif



#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4250)		//qQuiteAnnoyingDominanceWarnings
#endif
#if		qSilenceAnnoyingCompilerWarnings && __MWERKS__
	// Shut-off SetWindowRect() warning instantiating InteractorInteractorMixinHelper<>
	#pragma	warn_hidevirtual	off
#endif






/*
@CLASS:			StandardStyledWordWrappedTextInteractor
@BASES:			@'InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>', (INTERACTOR1=@'StandardStyledTextInteractor',INTERACTOR2=@'WordWrappedTextInteractor')
@DESCRIPTION:
		<p>A simple mixin of @'WordWrappedTextInteractor' and @'StandardStyledTextInteractor' using @'InteractorInteractorMixinHelper'.
	Takes care of the nitty-gritty of combining their behaviors.</p>
*/
class	StandardStyledWordWrappedTextInteractor : public InteractorInteractorMixinHelper<StandardStyledTextInteractor, WordWrappedTextInteractor> {
	private:
		typedef	InteractorInteractorMixinHelper<StandardStyledTextInteractor, WordWrappedTextInteractor>	inherited;

	protected:
		StandardStyledWordWrappedTextInteractor ();
};







/*
@CLASS:			WordProcessor
@BASES:			@'StandardStyledWordWrappedTextInteractor'
@DESCRIPTION:	<p>A fancy word processor. Add high-end editing, more word-processor like features here.
			For example, paragraph formating, justification, margins, remembered per-paragraph tabstops settings, etc.</p>
*/
class	WordProcessor : public StandardStyledWordWrappedTextInteractor {
	private:
		typedef	StandardStyledWordWrappedTextInteractor	inherited;
	protected:
		WordProcessor ();
	public:
		~WordProcessor ();

	protected:
		override	void	HookLosingTextStore ();
		nonvirtual	void	HookLosingTextStore_ ();
		override	void	HookGainedNewTextStore ();
		nonvirtual	void	HookGainedNewTextStore_ ();

#if		qWideCharacters
	public:
		nonvirtual	bool	GetSmartQuoteMode () const;
		nonvirtual	void	SetSmartQuoteMode (bool smartQuoteMode);
	private:
		bool	fSmartQuoteMode;
#endif

	public:
		class	WordProcessorTextIOSinkStream;
		class	WordProcessorTextIOSrcStream;

	public:
		class	WordProcessorFlavorPackageInternalizer;
		class	WordProcessorFlavorPackageExternalizer;

	public:
		#if		qTroubleLookingUpBaseClassInNestedClassDeclaration_VC6
			class	NoParagraphDatabaseAvailable : public TextImager::NotFullyInitialized {};
		#else
			/*
			@CLASS:			WordProcessor::NoParagraphDatabaseAvailable
			@BASES:			@'TextImager::NotFullyInitialized'
			@DESCRIPTION:	<p>Thrown by @'WordProcessor::GetLayoutMargins' etc when no @'WordProcessor::ParagraphDatabasePtr' available.</p>
			*/
			class	NoParagraphDatabaseAvailable : public NotFullyInitialized {};
		#endif

	public:
		class	WPPartition;

		override	PartitionPtr	MakeDefaultPartition () const;

	public:
		class		ParagraphInfo;
		class		IncrementalParagraphInfo;
		class		ParagraphInfoMarker;
		class		AbstractParagraphDatabaseRep;
		class		ParagraphDatabaseRep;
		/*
		@CLASS:			WordProcessor::ParagraphDatabasePtr
		@BASES:			@'Led_RefCntPtr<T>', (T=@'WordProcessor::AbstractParagraphDatabaseRep')
		@DESCRIPTION:	<p>A Led_RefCntPtr (smart pointer) to a @'WordProcessor::AbstractParagraphDatabaseRep'.</p>
		*/
		typedef	Led_RefCntPtr<AbstractParagraphDatabaseRep>	ParagraphDatabasePtr;
		nonvirtual	ParagraphDatabasePtr			GetParagraphDatabase () const;
		nonvirtual	void							SetParagraphDatabase (const ParagraphDatabasePtr& paragraphDatabase);
	private:
		ParagraphDatabasePtr	fParagraphDatabase;
		bool					fICreatedParaDB;

	protected:
		virtual		void		HookParagraphDatabaseChanged ();
		nonvirtual	void		HookParagraphDatabaseChanged_ ();


	public:
		/*
		@CLASS:			WordProcessor::HidableTextDatabasePtr
		@BASES:			@'Led_RefCntPtr<T>', (T=@'HidableTextMarkerOwner')
		@DESCRIPTION:	<p>A Led_RefCntPtr (smart pointer) to a @'HidableTextMarkerOwner'.</p>
		*/
		typedef	Led_RefCntPtr<HidableTextMarkerOwner>	HidableTextDatabasePtr;
		nonvirtual	HidableTextDatabasePtr	GetHidableTextDatabase () const;
		nonvirtual	void					SetHidableTextDatabase (const HidableTextDatabasePtr& hidableTextDatabase);
	private:
		HidableTextDatabasePtr	fHidableTextDatabase;
		bool					fICreatedHidableTextDB;

	protected:
		virtual		void		HookHidableTextDatabaseChanged ();
		nonvirtual	void		HookHidableTextDatabaseChanged_ ();

	protected:
		override	Led_RefCntPtr<FlavorPackageInternalizer>	MakeDefaultInternalizer ();
		override	Led_RefCntPtr<FlavorPackageExternalizer>	MakeDefaultExternalizer ();
	protected:
		override	void	HookInternalizerChanged ();
		override	void	HookExternalizerChanged ();

	public:
		override	void	InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage, bool updateCursorPosition, bool autoScroll, UpdateMode updateMode);
		override	void	ExternalizeFlavors (WriterFlavorPackage& flavorPackage);

	public:
		virtual		void	InterectiveSetRegionHidable (bool hidable);

	public:
		nonvirtual	Led_Justification	GetJustification (size_t characterPos) const;
		nonvirtual	bool				GetJustification (size_t from, size_t to, Led_Justification* justification) const;
		nonvirtual	void				SetJustification (size_t from, size_t to, Led_Justification justification);

	public:
		static		StandardTabStopList	GetDefaultStandardTabStopList ();
		nonvirtual	StandardTabStopList	GetStandardTabStopList (size_t characterPos) const;
		nonvirtual	bool				GetStandardTabStopList (size_t from, size_t to, StandardTabStopList* tabStops) const;
		nonvirtual	void				SetStandardTabStopList (size_t from, size_t to, StandardTabStopList tabStops);

	public:
		nonvirtual	void				GetMargins (size_t characterPos, Led_TWIPS* leftMargin, Led_TWIPS* rightMargin) const;
		nonvirtual	bool				GetMargins (size_t from, size_t to, Led_TWIPS* leftMargin, Led_TWIPS* rightMargin) const;
		nonvirtual	void				SetMargins (size_t from, size_t to, Led_TWIPS leftMargin, Led_TWIPS rightMargin);

	public:
		nonvirtual	Led_TWIPS			GetFirstIndent (size_t characterPos) const;
		nonvirtual	bool				GetFirstIndent (size_t from, size_t to, Led_TWIPS* firstIndent) const;
		nonvirtual	void				SetFirstIndent (size_t from, size_t to, Led_TWIPS firstIndent);

	public:
		nonvirtual	Led_TWIPS			GetSpaceBefore (size_t characterPos) const;
		nonvirtual	bool				GetSpaceBefore (size_t from, size_t to, Led_TWIPS* sb) const;
		nonvirtual	void				SetSpaceBefore (size_t from, size_t to, Led_TWIPS sb);
		nonvirtual	Led_TWIPS			GetSpaceAfter (size_t characterPos) const;
		nonvirtual	bool				GetSpaceAfter (size_t from, size_t to, Led_TWIPS* sa) const;
		nonvirtual	void				SetSpaceAfter (size_t from, size_t to, Led_TWIPS sa);
		nonvirtual	Led_LineSpacing		GetLineSpacing (size_t characterPos) const;
		nonvirtual	bool				GetLineSpacing (size_t from, size_t to, Led_LineSpacing* sl) const;
		nonvirtual	void				SetLineSpacing (size_t from, size_t to, Led_LineSpacing sl);
		nonvirtual	ListStyle			GetListStyle (size_t characterPos) const;
		nonvirtual	bool				GetListStyle (size_t from, size_t to, ListStyle* listStyle) const;
		nonvirtual	void				SetListStyle (size_t from, size_t to, ListStyle listStyle, bool autoFormat = false);
		nonvirtual	unsigned char		GetListIndentLevel (size_t characterPos) const;
		nonvirtual	bool				GetListIndentLevel (size_t from, size_t to, unsigned char* indentLevel) const;
		nonvirtual	void				SetListIndentLevel (size_t from, size_t to, unsigned char indentLevel, bool autoFormat = false);

	protected:
		virtual	Led_tString		GetListLeader (size_t paragraphMarkerPos) const;
		virtual	Led_Distance	GetListLeaderLength (size_t paragraphMarkerPos) const;
		virtual	void			AutoFormatIndentedText (size_t from, size_t to);

	private:
		class	WPIdler : public Idler {
			public:
				WPIdler ();
				override	void	SpendIdleTime ();
				WordProcessor*	fWP;
		};
		friend	class	WPIdler;
		WPIdler	fWPIdler;

	public:
		override	void	SetSelection (size_t start, size_t end);
		#if		qUsingMemberNameToOverloadInTemplateClassBug
			void	SetSelection (size_t start, size_t end, TextInteractor::UpdateMode updateMode)
				{
					TextInteractor::SetSelection (start, end, updateMode);
				}
		#else
			using	TextInteractor::SetSelection;
		#endif

	public:
		override	bool		GetCaretShownSituation () const;
		override	Led_Rect	CalculateCaretRect () const;

	public:
		override	void	OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed);

	public:
		override	bool	ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor);
		override	void	WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor);

	// Table support
	public:
		class	Table;
		virtual	Table*	InsertTable (size_t at);

		nonvirtual	vector<Table*>	GetTablesInRange (size_t from, size_t to = static_cast<size_t> (-1)) const;
		nonvirtual	Table*			GetTableAt (size_t from) const;

	public:
		nonvirtual	Table*	GetActiveTable () const;

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
		enum	{
			kWordProcessorCommand_First				=	kTextInteractorCommand_Last+1,

			kSelectTableIntraCellAll_CmdID			=	kWordProcessorCommand_First,
			kSelectTableCell_CmdID,
			kSelectTableRow_CmdID,
			kSelectTableColumn_CmdID,
			kSelectTable_CmdID,

			kBaseFontSize_CmdID						=	RoundUpToNearest_CMDNUMs_MACRO(kSelectTable_CmdID+1, 0x100),
				kFontSize9_CmdID					=	kBaseFontSize_CmdID,
				kFontSize10_CmdID,
				kFontSize12_CmdID,
				kFontSize14_CmdID,
				kFontSize18_CmdID,
				kFontSize24_CmdID,
				kFontSize36_CmdID,
				kFontSize48_CmdID,
				kFontSize72_CmdID,
				kFontSizeOther_CmdID,
				kFontSizeSmaller_CmdID,
				kFontSizeLarger_CmdID,
			kLastFontSize_CmdID						=	kFontSizeLarger_CmdID,


			kBaseFontColor_CmdID					=	RoundUpToNearest_CMDNUMs_MACRO(kLastFontSize_CmdID+1, 0x100),
				kFontColorBlack_CmdID				=	kBaseFontColor_CmdID,
				kFontColorMaroon_CmdID,
				kFontColorGreen_CmdID,
				kFontColorOlive_CmdID,
				kFontColorNavy_CmdID,
				kFontColorPurple_CmdID,
				kFontColorTeal_CmdID,
				kFontColorGray_CmdID,
				kFontColorSilver_CmdID,
				kFontColorRed_CmdID,
				kFontColorLime_CmdID,
				kFontColorYellow_CmdID,
				kFontColorBlue_CmdID,
				kFontColorFuchsia_CmdID,
				kFontColorAqua_CmdID,
				kFontColorWhite_CmdID,
			kLastNamedFontColor_CmdID				=	kFontColorWhite_CmdID,
				kFontColorOther_CmdID,
			kLastFontColor_CmdID					=	kFontColorOther_CmdID,

			kFirstJustification_CmdID				=	RoundUpToNearest_CMDNUMs_MACRO(kLastFontColor_CmdID+1, 0x100),
				kJustifyLeft_CmdID					=	kFirstJustification_CmdID,
				kJustifyCenter_CmdID,
				kJustifyRight_CmdID,
				kJustifyFull_CmdID,
			kLastJustification_CmdID				=	kJustifyFull_CmdID,

			kParagraphSpacingCommand_CmdID,
			kParagraphIndentsCommand_CmdID,
			kFirstListStyle_CmdID,
				kListStyle_None_CmdID				=	kFirstListStyle_CmdID,
				kListStyle_Bullet_CmdID,
			kLastListStyle_CmdID					=	kListStyle_Bullet_CmdID,
			kIncreaseIndent_CmdID,
			kDecreaseIndent_CmdID,
			
			kFontMenuFirst_CmdID					=	RoundUpToNearest_CMDNUMs_MACRO (kDecreaseIndent_CmdID+1, 0x100),
			kFontMenuLast_CmdID						=	kFontMenuFirst_CmdID + 0x400,


			kFontStylePlain_CmdID,
			kFontStyleBold_CmdID,
			kFontStyleItalic_CmdID,
			kFontStyleUnderline_CmdID,
			#if		qMacOS
				kFontStyleOutline_CmdID,
				kFontStyleShadow_CmdID,
				kFontStyleCondensed_CmdID,
				kFontStyleExtended_CmdID,
			#elif	qWindows
				kFontStyleStrikeout_CmdID,
			#endif
			kSubScriptCommand_CmdID,
			kSuperScriptCommand_CmdID,
			kFontStyleCommand_FirstCmdId			=	kFontStylePlain_CmdID,
			kFontStyleCommand_LastCmdId				=	kSuperScriptCommand_CmdID,

			kChooseFontCommand_CmdID,

			kInsertTable_CmdID						=	RoundUpToNearest_CMDNUMs_MACRO (kChooseFontCommand_CmdID+1, 0x100),
			kInsertTableRowAbove_CmdID,
			kInsertTableRowBelow_CmdID,
			kInsertTableColBefore_CmdID,
			kInsertTableColAfter_CmdID,
			kInsertURL_CmdID,
			kInsertSymbol_CmdID,

			kRemoveTableColumns_CmdID,
			kRemoveTableRows_CmdID,

			kFirstSelectedEmbedding_CmdID			=	RoundUpToNearest_CMDNUMs_MACRO (kRemoveTableRows_CmdID+1, 0x100),
				kSelectedEmbeddingProperties_CmdID	=	kFirstSelectedEmbedding_CmdID,
				kFirstPrivateEmbedding_CmdID,
				kLastPrivateEmbedding_CmdID			=	kFirstPrivateEmbedding_CmdID + 5,
			kLastSelectedEmbedding_CmdID			=	kLastPrivateEmbedding_CmdID,

			kHideSelection_CmdID					=	RoundUpToNearest_CMDNUMs_MACRO (kLastSelectedEmbedding_CmdID+1, 0x100),
			kUnHideSelection_CmdID,

			kFirstShowHideGlyph_CmdID				=	RoundUpToNearest_CMDNUMs_MACRO (kUnHideSelection_CmdID+1, 0x100),
			kShowHideParagraphGlyphs_CmdID			=	kFirstShowHideGlyph_CmdID,
			kShowHideTabGlyphs_CmdID,
			kShowHideSpaceGlyphs_CmdID,
			kLastShowHideGlyph_CmdID				=	kShowHideSpaceGlyphs_CmdID,

			kWordProcessorCommand_Last				=	RoundUpToNearest_CMDNUMs_MACRO (kShowHideSpaceGlyphs_CmdID+1, 0x100)
		};

	public:
		class    DialogSupport;
		static	DialogSupport&	GetDialogSupport ();
		static	void			SetDialogSupport (DialogSupport* ds);
	private:
		static	DialogSupport*	sDialogSupport;

	public:
		override	Led_Distance	ComputeMaxHScrollPos () const;
	protected:
		nonvirtual	Led_TWIPS		CalculateFarthestRightMarginInDocument () const;
		nonvirtual	Led_TWIPS		GetFarthestRightMarginInDocument () const;
		nonvirtual	Led_TWIPS		CalculateFarthestRightMarginInWindow () const;
		virtual		Led_TWIPS		CalculateFarthestRightMargin () const;

	private:
		static	const	Led_TWIPS	kBadCachedFarthestRightMarginInDocument;


	protected:
		override	void	InvalidateAllCaches ();
		override	void	TabletChangedMetrics ();

	public:
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();


	public:
		nonvirtual	Led_IncrementalFontSpecification	GetCurSelFontSpec () const;
	private:
		mutable	Led_IncrementalFontSpecification	fCachedCurSelFontSpec;
		mutable	Led_Justification					fCachedCurSelJustification;
		mutable	bool								fCachedCurSelJustificationUnique;
		mutable	bool								fCachedCurSelFontSpecValid;
	
		nonvirtual	void	AssureCurSelFontCacheValid () const;


	protected:
		override	void	DoSingleCharCursorEdit (CursorMovementDirection direction, CursorMovementUnit movementUnit, CursorMovementAction action,
													UpdateMode updateMode = eDefaultUpdate, bool scrollToSelection = true
											);

	public:
		override	bool	OnUpdateCommand (CommandUpdater* enabler);
		override	bool	OnPerformCommand (CommandNumber commandNumber);

	protected:
		virtual	bool	PassAlongCommandToIntraCellModeTableCell (CommandNumber commandNumber);
		virtual	bool	PassAlongCommandToEachSelectedTableCell (CommandNumber commandNumber);

	public:
		override	void	OnSelectAllCommand ();

	public:
		virtual	void	OnUpdateFontNameChangeCommand (CommandUpdater* enabler);
		virtual	void	OnFontNameChangeCommand (CommandNumber cmdNum);
		virtual	void	OnUpdateFontStylePlainCommand (CommandUpdater* enabler);
		virtual	void	OnFontStylePlainCommand ();
		virtual	void	OnUpdateFontStyleBoldCommand (CommandUpdater* enabler);
		virtual	void	OnFontStyleBoldCommand ();
		virtual	void	OnUpdateFontStyleItalicCommand (CommandUpdater* enabler);
		virtual	void	OnFontStyleItalicCommand ();
		virtual	void	OnUpdateFontStyleUnderlineCommand (CommandUpdater* enabler);
		virtual	void	OnFontStyleUnderlineCommand ();
		#if		qMacOS
			virtual	void	OnUpdateFontStyleOutlineCommand (CommandUpdater* enabler);
			virtual	void	OnFontStyleOutlineCommand ();
			virtual	void	OnUpdateFontStyleShadowCommand (CommandUpdater* enabler);
			virtual	void	OnFontStyleShadowCommand ();
			virtual	void	OnUpdateFontStyleCondensedCommand (CommandUpdater* enabler);
			virtual	void	OnFontStyleCondensedCommand ();
			virtual	void	OnUpdateFontStyleExtendedCommand (CommandUpdater* enabler);
			virtual	void	OnFontStyleExtendedCommand ();
		#elif	qWindows
			virtual	void	OnUpdateFontStyleStrikeoutCommand (CommandUpdater* enabler);
			virtual	void	OnFontStyleStrikeoutCommand ();
		#endif
		virtual	void	OnUpdateFontStyleSubscriptCommand (CommandUpdater* enabler);
		virtual	void	OnFontStyleSubscriptCommand ();
		virtual	void	OnUpdateFontStyleSuperscriptCommand (CommandUpdater* enabler);
		virtual	void	OnFontStyleSuperscriptCommand ();
		virtual	void	OnUpdateChooseFontCommand (CommandUpdater* enabler);
		virtual	void	OnChooseFontCommand ();
		virtual	void	OnUpdateFontSizeChangeCommand (CommandUpdater* enabler);
		virtual	void	OnFontSizeChangeCommand (CommandNumber cmdNum);
		virtual	void	OnUpdateFontColorChangeCommand (CommandUpdater* enabler);
		virtual	void	OnFontColorChangeCommand (CommandNumber cmdNum);
		virtual	void	OnUpdateInsertTableCommand (CommandUpdater* pCmdUI);
		virtual	void	OnInsertTableCommand ();
		virtual	void	OnUpdateInsertURLCommand (CommandUpdater* pCmdUI);
		virtual	void	OnInsertURLCommand ();
		virtual	void	OnUpdateInsertSymbolCommand (CommandUpdater* pCmdUI);
		virtual	void	OnInsertSymbolCommand ();
		virtual	void	OnUpdateSelectedEmbeddingExtendedCommand (CommandUpdater* pCmdUI);
		virtual	bool	OnSelectedEmbeddingExtendedCommand (CommandNumber cmdNum);

		virtual	void	OnEditTablePropertiesDialog ();

		virtual	void	OnUpdateHideSelectionCommands (CommandUpdater* enabler);
		virtual	void	OnHideSelection ();
		virtual	void	OnUnHideSelection ();
		virtual	void	OnUpdateParagraphJustificationCommand (CommandUpdater* enabler);
		virtual	void	OnParagraphJustificationCommand (CommandNumber cmdNum);		
		virtual	void	OnUpdateParagraphSpacingChangeCommand (CommandUpdater* enabler);
		virtual	void	OnParagraphSpacingChangeCommand ();
		virtual	void	OnUpdateParagraphIndentsChangeCommand (CommandUpdater* enabler);
		virtual	void	OnParagraphIndentsChangeCommand ();
		virtual	void	OnUpdateListStyleChangeCommand (CommandUpdater* enabler);
		virtual	void	OnListStyleChangeCommand (CommandNumber cmdNum);
		virtual	void	OnUpdateIndentCommand (CommandUpdater* enabler);
		virtual	void	OnIndentCommand (CommandNumber cmdNum);
		virtual	void	OnUpdateShowHideGlyphCommand (CommandUpdater* enabler);
		virtual	void	OnShowHideGlyphCommand (CommandNumber cmdNum);

	protected:
		virtual		Led_SDK_String						GetPrettyTypeName (SimpleEmbeddedObjectStyleMarker* m);
		nonvirtual	SimpleEmbeddedObjectStyleMarker*	GetSoleSelectedEmbedding () const;


	public:
		class	EmptySelectionParagraphSavedTextRep;

	protected:
		override	InteractiveReplaceCommand::SavedTextRep*	InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd);

	public:
		override	void	InteractiveSetFont (const Led_IncrementalFontSpecification& defaultFont);

	public:
		nonvirtual	void	InteractiveSetJustification (Led_Justification justification);
		nonvirtual	void	InteractiveSetStandardTabStopList (StandardTabStopList tabStops);
		nonvirtual	void	InteractiveSetMargins (Led_TWIPS leftMargin, Led_TWIPS rightMargin);
		nonvirtual	void	InteractiveSetFirstIndent (Led_TWIPS firstIndent);
		nonvirtual	void	InteractiveSetMarginsAndFirstIndent (Led_TWIPS leftMargin, Led_TWIPS rightMargin, Led_TWIPS firstIndent);
		nonvirtual	void	InteractiveSetParagraphSpacing (Led_TWIPS spaceBefore, bool spaceBeforeValid, Led_TWIPS spaceAfter, bool spaceAfterValid, Led_LineSpacing lineSpacing, bool lineSpacingValid);
		nonvirtual	void	InteractiveSetListStyle (ListStyle listStyle);
		nonvirtual	void	InteractiveDoIndentChange (bool increase);

	protected:
		override	const TextImager::TabStopList&	GetTabStopList (size_t containingPos) const;

	protected:
		override	void	DrawBefore (const Led_Rect& subsetToDraw, bool printing);

	protected:
		override	void	DrawRowSegments (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
									const TextLayoutBlock& text, size_t rowStart, size_t rowEnd
								);

	protected:
		override	vector<Led_Rect>	GetRowHilightRects (const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, size_t hilightStart, size_t hilightEnd) const;

	public:
		override	void	DrawSegment (Led_Tablet tablet,
									size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
									Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
								);

	protected:
		override	Led_Distance	MeasureSegmentHeight (size_t from, size_t to) const;
		override	Led_Distance	MeasureSegmentBaseLine (size_t from, size_t to) const;


	private:
		nonvirtual	Led_Distance	MeasureMinSegDescent (size_t from, size_t to) const;

	protected:
		override	void	AdjustBestRowLength (size_t textStart, const Led_tChar* text, const Led_tChar* end, size_t* rowLength);

	public:
		nonvirtual	bool	GetShowParagraphGlyphs () const;
		nonvirtual	void	SetShowParagraphGlyphs (bool showParagraphGlyphs);
	private:
		bool	fShowParagraphGlyphs;

	public:
		nonvirtual	bool	GetShowTabGlyphs () const;
		nonvirtual	void	SetShowTabGlyphs (bool showTabGlyphs);
	private:
		bool	fShowTabGlyphs;

	public:
		nonvirtual	bool	GetShowSpaceGlyphs () const;
		nonvirtual	void	SetShowSpaceGlyphs (bool showSpaceGlyphs);
	private:
		bool	fShowSpaceGlyphs;

	public:
		override	size_t	ComputeRelativePosition (size_t fromPos, CursorMovementDirection direction, CursorMovementUnit movementUnit);

	protected:
		override	bool	ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const;
		override	void	ReplaceMappedDisplayCharacters (const Led_tChar* srcText, Led_tChar* copyText, size_t nTChars) const;

	public:
		override	Led_Rect	GetCharLocationRowRelative (size_t afterPosition, RowReference topRow, size_t maxRowsToCheck)	const;
		override	size_t		GetCharAtLocationRowRelative (const Led_Point& where, RowReference topRow, size_t maxRowsToCheck) const;

	protected:
		override	size_t		ResetTabStops (size_t from, const Led_tChar* text, size_t nTChars, Led_Distance* charLocations, size_t startSoFar) const;

	private:
		nonvirtual	size_t		ResetTabStopsWithMargin (Led_Distance lhsMargin, size_t from, const Led_tChar* text, size_t nTChars, Led_Distance* charLocations, size_t startSoFar) const;

	public:
		override	void		GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const;

	protected:
		override	void	ExpandedFromAndToInPostReplace (size_t from, size_t newTo,
										size_t stableTypingRegionStart, size_t stableTypingRegionEnd,
										size_t startPositionOfRowWhereReplaceBegins, size_t	startPositionOfRowAfterReplaceEnds,
										size_t* expandedFrom, size_t* expandedTo
									);
		override	void	PostReplace (PreReplaceInfo& preReplaceInfo);

	private:
		nonvirtual	Led_Distance	CalcSpaceToEat (size_t rowContainingCharPos) const;


	private:
		friend	class	Table;
		friend	class	AbstractParagraphDatabaseRep;
		friend	class	ParagraphDatabaseRep;
};





/*
@CLASS:			WordProcessor::ParagraphInfo
@DESCRIPTION:	<p>An object which captures the per-paragraph information we store especially in the
			@'WordProcessor' class. The attributes stored include:
				<ul>
					<li>Justification</li>
					<li>TabStopList</li>
					<li>Margin (left and right - NB: we require left < right)</li>
					<li>FirstIdent (nb this is ADDED to the LHS margin, and CAN BE NEGATIVE</li>
				</ul>
				</p>
*/
class	WordProcessor::ParagraphInfo {
	public:
		typedef	TextImager::StandardTabStopList	StandardTabStopList;
	public:
		ParagraphInfo ();

	public:
		nonvirtual	Led_Justification	GetJustification () const;
		nonvirtual	void				SetJustification (Led_Justification justification);
	private:
		Led_Justification	fJustification;

	public:
		nonvirtual	const StandardTabStopList&	GetTabStopList () const;
		nonvirtual	void						SetTabStopList (const StandardTabStopList& tabStops);
	private:
		StandardTabStopList	fTabStops;


	public:
		nonvirtual	Led_TWIPS	GetLeftMargin () const;
		nonvirtual	Led_TWIPS	GetRightMargin () const;
		nonvirtual	void		SetMargins (Led_TWIPS lhs, Led_TWIPS rhs);
	private:
		Led_TWIPS	fLeftMargin;
		Led_TWIPS	fRightMargin;

	public:
		nonvirtual	Led_TWIPS		GetFirstIndent () const;
		nonvirtual	void			SetFirstIndent (Led_TWIPS firstIndent);
	private:
		Led_TWIPS	fFirstIndent;


	public:
		nonvirtual	Led_TWIPS		GetSpaceBefore () const;
		nonvirtual	void			SetSpaceBefore (Led_TWIPS sb);
		nonvirtual	Led_TWIPS		GetSpaceAfter () const;
		nonvirtual	void			SetSpaceAfter (Led_TWIPS sa);
		nonvirtual	Led_LineSpacing	GetLineSpacing () const;
		nonvirtual	void			SetLineSpacing (Led_LineSpacing sl);
	private:
		Led_TWIPS		fSpaceBefore;
		Led_TWIPS		fSpaceAfter;
		Led_LineSpacing	fLineSpacing;

	public:
		nonvirtual	ListStyle	GetListStyle () const;
		nonvirtual	void		SetListStyle (ListStyle listStyle);
	private:
		ListStyle	fListStyle;

	public:
		nonvirtual	unsigned char	GetListIndentLevel () const;
		nonvirtual	void			SetListIndentLevel (unsigned char indentLevel);
	private:
		unsigned char	fListIndentLevel;

	public:
		nonvirtual	void	MergeIn (const IncrementalParagraphInfo& incParaInfo);

	public:
		nonvirtual	bool	operator== (const ParagraphInfo& rhs) const;
		nonvirtual	bool	operator!= (const ParagraphInfo& rhs) const;
};





/*
@CLASS:			WordProcessor::IncrementalParagraphInfo
@BASES:			@'WordProcessor::ParagraphInfo'
@DESCRIPTION:	<p>Overload (hide) the @'WordProcessor::ParagraphInfo' methods, to assert that the
			attribute is valid (for getters) and to set a ValidFlag for the setters. And add methods for
			each attribute to test for validity, and to invalidate.</p>
				<p>These are used if you want to set just part of a @'WordProcessor::ParagraphInfo'.</p>
*/
class	WordProcessor::IncrementalParagraphInfo : public WordProcessor::ParagraphInfo {
	private:
		typedef	ParagraphInfo	inherited;

	public:
		IncrementalParagraphInfo ();
		explicit IncrementalParagraphInfo (const ParagraphInfo& pi);

	public:
		nonvirtual	Led_Justification	GetJustification () const;
		nonvirtual	void				SetJustification (Led_Justification justification);
		nonvirtual	bool				GetJustification_Valid () const;
		nonvirtual	void				InvalidateJustification ();
	private:
		bool	fJustificationValid;

	public:
		nonvirtual	const StandardTabStopList&	GetTabStopList () const;
		nonvirtual	void						SetTabStopList (const StandardTabStopList& tabStops);
		nonvirtual	bool						GetTabStopList_Valid () const;
		nonvirtual	void						InvalidateTabStopList ();
	private:
		bool	fTabStopListValid;

	public:
		nonvirtual	Led_TWIPS	GetLeftMargin () const;
		nonvirtual	Led_TWIPS	GetRightMargin () const;
		nonvirtual	void		SetMargins (Led_TWIPS lhs, Led_TWIPS rhs);
		nonvirtual	bool		GetMargins_Valid () const;
		nonvirtual	void		InvalidateMargins ();
	private:
		bool	fMarginsValid;

	public:
		nonvirtual	Led_TWIPS	GetFirstIndent () const;
		nonvirtual	void		SetFirstIndent (Led_TWIPS firstIndent);
		nonvirtual	bool		GetFirstIndent_Valid () const;
		nonvirtual	void		InvalidateFirstIndent ();
	private:
		bool	fFirstIndentValid;

	public:
		nonvirtual	Led_TWIPS		GetSpaceBefore () const;
		nonvirtual	void			SetSpaceBefore (Led_TWIPS sb);
		nonvirtual	bool			GetSpaceBefore_Valid () const;
		nonvirtual	void			InvalidateSpaceBefore ();
		nonvirtual	Led_TWIPS		GetSpaceAfter () const;
		nonvirtual	void			SetSpaceAfter (Led_TWIPS sa);
		nonvirtual	bool			GetSpaceAfter_Valid () const;
		nonvirtual	void			InvalidateSpaceAfter ();
		nonvirtual	Led_LineSpacing	GetLineSpacing () const;
		nonvirtual	void			SetLineSpacing (Led_LineSpacing sl);
		nonvirtual	bool			GetLineSpacing_Valid () const;
		nonvirtual	void			InvalidateLineSpacing ();

		nonvirtual	ListStyle		GetListStyle () const;
		nonvirtual	void			SetListStyle (ListStyle listStyle);
		nonvirtual	bool			GetListStyle_Valid () const;
		nonvirtual	void			InvalidateListStyle ();

		nonvirtual	unsigned char	GetListIndentLevel () const;
		nonvirtual	void			SetListIndentLevel (unsigned char indentLevel);
		nonvirtual	bool			GetListIndentLevel_Valid () const;
		nonvirtual	void			InvalidateListIndentLevel ();

	private:
		bool	fSpaceBeforeValid;
		bool	fSpaceAfterValid;
		bool	fLineSpacingValid;	
		bool	fListStyleValid;	
		bool	fListIndentLevelValid;	

	public:
		nonvirtual	bool	operator== (const IncrementalParagraphInfo& rhs) const;
		nonvirtual	bool	operator!= (const IncrementalParagraphInfo& rhs) const;
};





/*
@CLASS:			WordProcessor::ParagraphInfoMarker
@BASES:			@'Marker'
@DESCRIPTION:
*/
class	WordProcessor::ParagraphInfoMarker : public Marker {
	public:
		ParagraphInfoMarker (ParagraphInfo paragraphInfo = ParagraphInfo ());

		nonvirtual	const ParagraphInfo&	GetInfo () const;
		nonvirtual	void					SetInfo (ParagraphInfo paragraphInfo);
	private:
		ParagraphInfo	fParagraphInfo;
};



/*
@CLASS:			WordProcessor::AbstractParagraphDatabaseRep
@BASES:			virtual @'MarkerOwner'
@DESCRIPTION:	<p>.</p>
*/
class	WordProcessor::AbstractParagraphDatabaseRep :	public virtual MarkerOwner {
	private:
		typedef	MarkerOwner	inherited;

	public:
		AbstractParagraphDatabaseRep ();

	public:
		typedef	Led_RefCntPtr<Partition>	PartitionPtr;
		virtual	PartitionPtr	GetPartition () const								= 0;
		virtual	void			SetPartition (const PartitionPtr& partitionPtr)		= 0;

	public:
		virtual	const ParagraphInfo&	GetParagraphInfo (size_t charAfterPos) const																= 0;
		virtual	vector<pair<WordProcessor::ParagraphInfo,size_t> >	GetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing) const			= 0;

		virtual	void	SetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers)				=	0;
		virtual	void	SetParagraphInfo (size_t charAfterPos, const vector<pair<WordProcessor::IncrementalParagraphInfo,size_t> >& infoForMarkers)	=	0;
		virtual	void	SetParagraphInfo (size_t charAfterPos, const vector<pair<ParagraphInfo,size_t> >& infoForMarkers) = 0;

	private:
		bool			fSomeInvalidTables;
	protected:
		Led_TWIPS		fCachedFarthestRightMarginInDocument;

	private:
		friend	class	WordProcessor;
		friend	class	WordProcessor::Table;
		friend	class	WordProcessor::WPIdler;
};



/*
@CLASS:			WordProcessor::ParagraphDatabaseRep
@BASES:			@'public WordProcessor::AbstractParagraphDatabaseRep' @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>', (MARKER=@'WordProcessor::ParagraphInfoMarker',MARKERINFO=@'WordProcessor::ParagraphInfo',INCREMENTALMARKERINFO=@'WordProcessor::IncrementalParagraphInfo')
@DESCRIPTION:	<p>A MarkerCover which associates @'WordProcessor::ParagraphInfo' with each paragraph in the next (merging together adjacent
			identical ones). Paragraphs are defined by the @'Partition' object associated
			(@'WordProcessor::ParagraphDatabaseRep::SetPartition').</p>
*/
class	WordProcessor::ParagraphDatabaseRep :	public WordProcessor::AbstractParagraphDatabaseRep, private MarkerCover<WordProcessor::ParagraphInfoMarker,WordProcessor::ParagraphInfo,WordProcessor::IncrementalParagraphInfo> {
	private:
		typedef	MarkerCover<ParagraphInfoMarker,ParagraphInfo,IncrementalParagraphInfo>	inheritedMC;
	public:
		ParagraphDatabaseRep (TextStore& textStore);

	public:
		typedef	Led_RefCntPtr<Partition>	PartitionPtr;
		override	PartitionPtr	GetPartition () const;
		override	void			SetPartition (const PartitionPtr& partitionPtr);
	private:
		PartitionPtr	fPartition;

	public:
		static	ParagraphInfo	GetStaticDefaultParagraphInfo ();

	// override the AbstractParagraphDatabase API
	public:
		override	const ParagraphInfo&								GetParagraphInfo (size_t charAfterPos) const;
		override	vector<pair<WordProcessor::ParagraphInfo,size_t> >	GetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing) const;
		override	void	SetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers);
		override	void	SetParagraphInfo (size_t charAfterPos, const vector<pair<WordProcessor::IncrementalParagraphInfo,size_t> >& infoForMarkers);
		override	void	SetParagraphInfo (size_t charAfterPos, const vector<pair<ParagraphInfo,size_t> >& infoForMarkers);

	public:
		override	void	SetInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers);
		override	void	SetInfos (size_t charAfterPos, const vector<pair<WordProcessor::IncrementalParagraphInfo,size_t> >& infoForMarkers);

	protected:
		override	void	NoteCoverRangeDirtied (size_t from, size_t to, const MarkerVector& rangeAndSurroundingsMarkers);
		override	void	ConstrainSetInfoArgs (size_t* charAfterPos, size_t* nTCharsFollowing);

	private:
		nonvirtual	void	CheckMarkerBounaryConstraints (size_t from, size_t to) throw ();
		nonvirtual	void	CheckMarkerBounaryConstraints (const MarkerVector& rangeAndSurroundingsMarkers) throw ();

	#if		qDebug
	protected:
			override	void	Invariant_ () const;
	#endif
};



/*
@CLASS:			WordProcessor::WordProcessorTextIOSinkStream
@BASES:			@'StandardStyledTextInteractor::StandardStyledTextIOSinkStream'
@DESCRIPTION:	<p>A @'StandardStyledTextInteractor::StandardStyledTextIOSinkStream', for use with the StyledTextIO module,
			which adds support for a @'WordProcessor::ParagraphDatabasePtr'.</p>
*/
class	WordProcessor::WordProcessorTextIOSinkStream : public StandardStyledTextInteractor::StandardStyledTextIOSinkStream {
	private:
		typedef	StandardStyledTextInteractor::StandardStyledTextIOSinkStream	inherited;
	public:
		WordProcessorTextIOSinkStream (TextStore* textStore,
				const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
				const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
				const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase,
				size_t insertionStart = 0
			);
		WordProcessorTextIOSinkStream (WordProcessor* wp, size_t insertionStart = 0);
		~WordProcessorTextIOSinkStream ();

	private:
		nonvirtual	void	CTOR_COMMON ();

	public:
		nonvirtual	bool	GetOverwriteTableMode () const;
		nonvirtual	void	SetOverwriteTableMode (bool overwriteTableMode);
	private:
		bool	fOverwriteTableMode;

#if		!qNestedTablesSupported
	public:
		nonvirtual	bool	GetNoTablesAllowed () const;
		nonvirtual	void	SetNoTablesAllowed (bool noTablesAllowed);
	private:
		bool	fNoTablesAllowed;
#endif

	public:
		override	void	AppendText (const Led_tChar* text, size_t nTChars, const Led_FontSpecification* fontSpec);
		override	void	AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding);
		override	void	AppendSoftLineBreak ();
		override	void	SetJustification (Led_Justification justification);
		override	void	SetStandardTabStopList (const TextImager::StandardTabStopList& tabStops);
		override	void	SetFirstIndent (Led_TWIPS tx);
		override	void	SetLeftMargin (Led_TWIPS lhs);
		override	void	SetRightMargin (Led_TWIPS rhs);
		override	void	SetSpaceBefore (Led_TWIPS sb);
		override	void	SetSpaceAfter (Led_TWIPS sa);
		override	void	SetLineSpacing (Led_LineSpacing sl);	
		override	void	SetTextHidden (bool hidden);
		override	void	StartTable ();
		override	void	EndTable ();
		override	void	StartTableRow ();
		override	void	EndTableRow ();
		override	void	StartTableCell (size_t colSpan);
		override	void	EndTableCell ();
		override	void	SetListStyle (ListStyle listStyle);
		override	void	SetListIndentLevel (unsigned char indentLevel);
		override	void	SetTableBorderColor (Led_Color c);
		override	void	SetTableBorderWidth (Led_TWIPS bWidth);
		override	void	SetCellWidths (const vector<Led_TWIPS>& cellWidths);
		override	void	SetCellBackColor (const Led_Color c);
		override	void	SetDefaultCellMarginsForCurrentRow (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right);
		override	void	SetDefaultCellSpacingForCurrentRow (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right);
		override	void	EndOfBuffer ();
		override	void	Flush ();

	public:
		nonvirtual	void	SetIgnoreLastParaAttributes (bool ignoreLastParaAttributes);

	protected:
		nonvirtual	void	PushContext (TextStore* ts,
											const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
											const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
											const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase,
											size_t insertionStart
										);
		nonvirtual	void	PopContext ();
	private:
		struct	Context {
			WordProcessor::ParagraphDatabasePtr		fParagraphDatabase;
			WordProcessor::HidableTextDatabasePtr	fHidableTextDatabase;
		};
		vector<Context>		fSavedContexts;

	private:
		typedef	pair<IncrementalParagraphInfo,size_t>	ParaInfoNSize;
		WordProcessor::ParagraphDatabasePtr				fParagraphDatabase;
		WordProcessor::HidableTextDatabasePtr			fHidableTextDatabase;
		vector<ParaInfoNSize>							fSavedParaInfo;
		IncrementalParagraphInfo						fNewParagraphInfo;
		bool											fTextHidden;
		DiscontiguousRun<bool>							fHidableTextRuns;
		bool											fEndOfBuffer;
		bool											fIgnoreLastParaAttributes;
		WordProcessor::Table*							fCurrentTable;
		vector<Led_TWIPS>								fCurrentTableCellWidths;
		Led_Color										fCurrentTableCellColor;
		vector<size_t>									fCurrentTableColSpanArray;
		vector<WordProcessor::Table*>					fTableStack;	// for nesting
		size_t											fNextTableRow;
		size_t											fNextTableCell;
		size_t											fCurrentTableCell;

#if		qDebug
	private:
		unsigned int	fTableOpenLevel;
		bool			fTableRowOpen;
		bool			fTableCellOpen;
#endif
};





/*
@CLASS:			WordProcessor::WordProcessorTextIOSrcStream
@BASES:			@'StandardStyledTextInteractor::StandardStyledTextIOSrcStream'
@DESCRIPTION:	<p>A @'StandardStyledTextInteractor::StandardStyledTextIOSrcStream', for use with the StyledTextIO module,
			which adds support for a @'WordProcessor::ParagraphDatabasePtr'.</p>
*/
class	WordProcessor::WordProcessorTextIOSrcStream : public StandardStyledTextInteractor::StandardStyledTextIOSrcStream {
	private:
		typedef	StandardStyledTextInteractor::StandardStyledTextIOSrcStream	inherited;
	public:
		WordProcessorTextIOSrcStream (TextStore* textStore,
				const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
				const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
				const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase,
				size_t selectionStart = 0, size_t selectionEnd = kBadIndex
			);
		WordProcessorTextIOSrcStream (WordProcessor* textImager, size_t selectionStart = 0, size_t selectionEnd = kBadIndex);

	public:
		nonvirtual	bool	GetUseTableSelection () const;
		nonvirtual	void	SetUseTableSelection (bool useTableSelection);
	private:
		bool	fUseTableSelection;

	public:
		override	Led_Justification							GetJustification ()	const;
		override	StandardTabStopList							GetStandardTabStopList () const;
		override	Led_TWIPS									GetFirstIndent () const;
		override	void										GetMargins (Led_TWIPS* lhs, Led_TWIPS* rhs) const;
		override	Led_TWIPS									GetSpaceBefore () const;
		override	Led_TWIPS									GetSpaceAfter () const;
		override	Led_LineSpacing								GetLineSpacing () const;	
		override	void										GetListStyleInfo (ListStyle* listStyle, unsigned char* indentLevel) const;
		override	Led_tChar									GetSoftLineBreakCharacter () const;
		override	DiscontiguousRun<bool>						GetHidableTextRuns () const;
		override	Table*										GetTableAt (size_t at) const;
		override	void										SummarizeFontAndColorTable (set<Led_SDK_String>* fontNames, set<Led_Color>* colorsUsed) const;

	protected:
		class	TableIOMapper;

	private:
		WordProcessor::ParagraphDatabasePtr	fParagraphDatabase;
		DiscontiguousRun<bool>				fHidableTextRuns;
};






/*
@CLASS:			WordProcessor::WordProcessorTextIOSrcStream::TableIOMapper
@ACCESS:		protected
@BASES:			@'StyledTextIOWriter::SrcStream::Table'
@DESCRIPTION:	<p></p>
*/
class	WordProcessor::WordProcessorTextIOSrcStream::TableIOMapper : public StyledTextIOWriter::SrcStream::Table {
	private:
		typedef	StyledTextIOWriter::SrcStream::Table	inherited;
	public:
		TableIOMapper (WordProcessor::Table& realTable,
									size_t startRow = 0, size_t endRow = static_cast<size_t> (-1),
									size_t startCol = 0, size_t endCol = static_cast<size_t> (-1)
							);

	public:
		override	size_t							GetRows () const;
		override	size_t							GetColumns (size_t row) const;
		override	void							GetRowInfo (size_t row, vector<CellInfo>* cellInfos);
		override	StyledTextIOWriter::SrcStream*	MakeCellSubSrcStream (size_t row, size_t column);
		override	size_t							GetOffsetEnd () const;
		override	Led_TWIPS_Rect					GetDefaultCellMarginsForRow (size_t row) const;
		override	Led_TWIPS_Rect					GetDefaultCellSpacingForRow (size_t row) const;

	private:
		WordProcessor::Table&	fRealTable;
		size_t					fStartRow;
		size_t					fEndRow;
		size_t					fStartCol;
		size_t					fEndCol;
};







/*
@CLASS:			WordProcessor::WordProcessorFlavorPackageInternalizer
@BASES:			virtual @'StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer'
@DESCRIPTION:	<p>Add paragraph etc support support by creating a @'WordProcessor::WordProcessorTextIOSinkStream'.</p>
*/
class	WordProcessor::WordProcessorFlavorPackageInternalizer : public StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer {
	private:
		typedef	StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer	inherited;

	public:
		WordProcessorFlavorPackageInternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase,
												const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
												const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase
											);

	public:
		nonvirtual	bool	GetOverwriteTableMode () const;
		nonvirtual	void	SetOverwriteTableMode (bool overwriteTableMode);
	private:
		bool	fOverwriteTableMode;

#if		!qNestedTablesSupported
	public:
		nonvirtual	bool	GetNoTablesAllowed () const;
		nonvirtual	void	SetNoTablesAllowed (bool noTablesAllowed);
	private:
		bool	fNoTablesAllowed;
#endif

	public:
		override	StandardStyledTextIOSinkStream*	mkStandardStyledTextIOSinkStream (size_t insertionStart);

	protected:
		WordProcessor::ParagraphDatabasePtr		fParagraphDatabase;
		WordProcessor::HidableTextDatabasePtr	fHidableTextDatabase;
};







/*
@CLASS:			WordProcessor::WPPartition
@BASES:			@'LineBasedPartition'
@DESCRIPTION:	<p>A partition object which respects the funny rules for tables - and makes sure tables appear within their
			own partition element.</p>
*/
class	WordProcessor::WPPartition : public LineBasedPartition {
	public:
		typedef	PartitioningTextImager::PartitionMarker	PartitionMarker;
		typedef	MarkerOwner::UpdateInfo					UpdateInfo;
		typedef	WordProcessor::Table					Table;
	private:
		typedef	LineBasedPartition	inherited;
	public:
		WPPartition (TextStore& textStore, MarkerOwner& tableMarkerOwner);
	protected:
		virtual	vector<Table*>	GetTablesInRange (size_t from, size_t to = static_cast<size_t> (-1)) const;

	protected:
		override	void	FinalConstruct ();

	public:
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();

	protected:
		nonvirtual	void	DoHandleUpdateForTableRangeCheck (size_t from, size_t to) throw ();

	protected:
		override	bool	NeedToCoalesce (PartitionMarker* pm) throw ();

	#if		qDebug
	protected:
		override	void	Invariant_ () const;
	#endif

	private:
		MarkerOwner&	fTableMarkerOwner;

	private:
		friend	class	WordProcessor::Table;
};






/*
@CLASS:			WordProcessor::WordProcessorFlavorPackageExternalizer
@BASES:			virtual @'StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer'
@DESCRIPTION:	<p>Add paragraph etc support support by creating a @'WordProcessor::WordProcessorTextIOSrcStream'.</p>
*/
class	WordProcessor::WordProcessorFlavorPackageExternalizer : public StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer {
	private:
		typedef	StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer	inherited;

	public:
		WordProcessorFlavorPackageExternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase,
												const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
												const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase
											);

	public:
		nonvirtual	bool	GetUseTableSelection () const;
		nonvirtual	void	SetUseTableSelection (bool useTableSelection);
	private:
		bool	fUseTableSelection;

	protected:
		override	StandardStyledTextIOSrcStream*	mkStandardStyledTextIOSrcStream (size_t selectionStart, size_t selectionEnd);

	protected:
		WordProcessor::ParagraphDatabasePtr		fParagraphDatabase;
		WordProcessor::HidableTextDatabasePtr	fHidableTextDatabase;
};




	
/*
@CLASS:			WordProcessor::Table
@BASES:			@'SimpleEmbeddedObjectStyleMarker'
@DESCRIPTION:	<p>A table is an object that can be embedded in a document, and it contains multiple cells.
			Each cell is basically another word-processor instance.</p>
				<p>Since a table is assocated with a paragraph database, and can be viewed and edited
			simultaneously be multiple views (WordProcessors) - we cannot make long-term assumptions
			about association of a table with an owning WordProcessor object. Instead - during UI operations
			like typing and so on - you temporarily associate an owning WP with the given table using
			@'WordProcessor::Table::TemporarilySetOwningWP'</p>
				<p>We support having any number of rows, and each row can have a different number of
			columns (cells). Often however - the number of columns for each row of a table will be the
			same (as will their widths be equal).</p>
				<p>This class may not remain a subclass of simpleembeddedObject stuff - but maybe.</p>
				<p>Note that most coordiantes in this routine are presumed to be table relative. Either that - or
			they are window-rect relative. You can use the methods @'WordProcessor::Table::TableCoordinates2Window' and
			@'WordProcessor::Table::WindowCoordinates2Table' to map between them.
				</p>
				<p>Each cell is surrounded by spacing (see @'WordProcessor::Table::GetCellSpacing'.
			This defaults to zero. If you set it to non-zero - then the borders drawn for cells don't touch the
			borders for the table itself.</p>
				<p>The cell bounds for each cell (@'WordProcessor::Table::GetCellBounds') is the area that
			gets a box drawn around it (on the outside bounds). These 'cell bounds' may not exactly touch each other on the
			edges by so that they SHARE the border separating each from its sibling. Also - they may not touch because
			of cell spacing (@'WordProcessor::Table::GetCellSpacing').</p>
				<p>The actaul 'edit window' within a cell is inset by its cell margins. This value defaults
			to that provided by @'WordProcessor::Table::GetDefaultCellMargins', but at some point (maybe for 3.1?)
			will be overridable on a per-cell basis.</p>
*/
class	WordProcessor::Table : public SimpleEmbeddedObjectStyleMarker {
	private:
		typedef	SimpleEmbeddedObjectStyleMarker	inherited;

	public:
		Table (AbstractParagraphDatabaseRep* tableOwner, size_t addAt);
		~Table ();

	protected:
		virtual	void	FinalizeAddition (AbstractParagraphDatabaseRep* o, size_t addAt);

	public:
		class	Cell;

	protected:
		class	CellRep;

	private:
		friend	class	Led_RefCntPtr<CellRep>;

	public:
		override	void			DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
											Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										);
		override	void			MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
												const Led_tChar* text,
												Led_Distance* distanceResults
											) const;
		override	Led_Distance	MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const;

	public:
		virtual		vector<Led_Rect>	GetRowHilightRects () const;

	protected:
		virtual		void		DrawTableBorders (WordProcessor& owningWP, Led_Tablet tablet, const Led_Rect& drawInto);
		virtual		void		DrawCellBorders (Led_Tablet tablet, size_t row, size_t column, const Led_Rect& cellBounds);


	public:
		nonvirtual	Led_TWIPS	GetCellSpacing () const;
		nonvirtual	void		SetCellSpacing (Led_TWIPS cellSpacing);
	private:
		Led_TWIPS	fCellSpacing;


	public:
		nonvirtual	void		GetDefaultCellMargins (Led_TWIPS* top, Led_TWIPS* left, Led_TWIPS* bottom, Led_TWIPS* right) const;
		nonvirtual	void		SetDefaultCellMargins (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right);
	private:
		Led_TWIPS_Rect	fDefaultCellMargins;	// Not REALLY a rect - just a handy way to store 4 values... and OK since its private - not part of API

	public:
		virtual		Led_Rect	GetCellBounds (size_t row, size_t column) const;
		virtual		Led_Rect	GetCellEditorBounds (size_t row, size_t column) const;

		virtual		void		GetClosestCell (const Led_Point& p, size_t* row, size_t* col) const;


	public:
		nonvirtual	Led_Point	TableCoordinates2Window (const Led_Point& p) const;
		nonvirtual	Led_Rect	TableCoordinates2Window (const Led_Rect& r) const;
		nonvirtual	Led_Point	WindowCoordinates2Table (const Led_Point& p) const;
		nonvirtual	Led_Rect	WindowCoordinates2Table (const Led_Rect& r) const;

	public:
		virtual		bool		GetCaretShownSituation () const;
		virtual		Led_Rect	CalculateCaretRect () const;

	public:
		virtual		bool	OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed);

	protected:
		virtual		bool	DoSingleCharCursorEdit (CursorMovementDirection direction, CursorMovementUnit movementUnit, CursorMovementAction action,
													UpdateMode updateMode, bool scrollToSelection
												);

	public:
		virtual		bool	OnUpdateCommand (CommandUpdater* enabler);
		virtual		bool	OnPerformCommand (CommandNumber commandNumber);

	protected:
		nonvirtual	void	BreakInGroupedCommands ();

	protected:
		virtual		bool	OnUpdateCommand_ApplyToEachSelectedCell (CommandUpdater* enabler);
		virtual		bool	OnPerformCommand_ApplyToEachSelectedCell (CommandNumber commandNumber, bool captureChangesForUndo = true);

	public:
		virtual	void	OnUpdateCutCommand (CommandUpdater* pCmdUI);
		virtual	void	OnCutCommand ();
		virtual	void	OnUpdateInsertTableRowAboveCommand (CommandUpdater* pCmdUI);
		virtual	void	OnInsertTableRowAboveCommand ();
		virtual	void	OnUpdateInsertTableRowBelowCommand (CommandUpdater* pCmdUI);
		virtual	void	OnInsertTableRowBelowCommand ();
		virtual	void	OnUpdateInsertTableColBeforeCommand (CommandUpdater* pCmdUI);
		virtual	void	OnInsertTableColBeforeCommand ();
		virtual	void	OnUpdateInsertTableColAfterCommand (CommandUpdater* pCmdUI);
		virtual	void	OnInsertTableColAfterCommand ();
		virtual	void	OnUpdateRemoveTableRowsCommand (CommandUpdater* pCmdUI);
		virtual	void	OnRemoveTableRowsCommand ();
		virtual	void	OnUpdateRemoveTableColumnsCommand (CommandUpdater* pCmdUI);
		virtual	void	OnRemoveTableColumnsCommand ();
		virtual	void	OnUpdateSelectTablePartsCommand (CommandUpdater* enabler);
		virtual	void	OnPerformTablePartsCommand (CommandNumber commandNumber);

	protected:
		nonvirtual	void	AssureCurSelFontCacheValid (Led_IncrementalFontSpecification* curSelFontSpec);
	
	public:
		nonvirtual	void	InteractiveSetFont (const Led_IncrementalFontSpecification& defaultFont);

	// SimpleEmbeddedObjectStyleMarker overrides
	public:
		override	const char*		GetTag () const;
		override	void			Write (SinkStream& sink);
		override	void			ExternalizeFlavors (WriterFlavorPackage& flavorPackage);

	public:
		virtual	bool	ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection);
		virtual	void	WhileSimpleMouseTracking (Led_Point newMousePos);

	public:
		nonvirtual	Led_Color	GetTableBorderColor () const;
		nonvirtual	void		SetTableBorderColor (Led_Color c);

	public:
		nonvirtual	Led_TWIPS	GetTableBorderWidth () const;
		nonvirtual	void		SetTableBorderWidth (Led_TWIPS w);

	public:
		nonvirtual	Led_TWIPS	GetColumnWidth (size_t row, size_t column) const;
		nonvirtual	void		SetColumnWidth (size_t row, size_t column, Led_TWIPS colWidth);

	public:
		nonvirtual	Led_Color	GetCellColor (size_t row, size_t column) const;
		nonvirtual	void		SetCellColor (size_t row, size_t column, const Led_Color& c);

	public:
		nonvirtual	Cell&		GetCell (size_t row, size_t column);
		nonvirtual	const Cell&	GetCell (size_t row, size_t column) const;
		nonvirtual	void		GetRealCell (size_t* row, size_t* column) const;
		nonvirtual	const Cell&	GetRealCell (size_t row, size_t column) const;

	public:
		enum	CellMergeFlags { ePlainCell = 0, eMergeCellLeft = 1, eMergeCellUp = 2, eMergeCellLeftUp = eMergeCellLeft+eMergeCellUp, eInvalidCell = 99 };

	public:
		nonvirtual	CellMergeFlags	GetCellFlags (size_t row, size_t column) const;
		nonvirtual	bool			CanMergeCells (size_t fromRow, size_t fromCol, size_t toRow, size_t toCol);
		nonvirtual	void			MergeCells (size_t fromRow, size_t fromCol, size_t toRow, size_t toCol);
		nonvirtual	void			UnMergeCells (size_t fromRow, size_t fromCol, size_t toRow, size_t toCol);

	public:
		nonvirtual	void	GetCellSelection (size_t* rowSelStart, size_t* rowSelEnd, size_t* colSelStart, size_t* colSelEnd) const;
		nonvirtual	void	SetCellSelection (size_t rowSelStart, size_t rowSelEnd, size_t colSelStart, size_t colSelEnd);

	public:
		nonvirtual	bool	GetIntraCellMode (size_t* row = NULL, size_t* col = NULL) const;
		nonvirtual	void	SetIntraCellMode ();
		nonvirtual	void	SetIntraCellMode (size_t row, size_t col);
		nonvirtual	void	UnSetIntraCellMode ();
		nonvirtual	void	GetIntraCellSelection (size_t* selStart, size_t* selEnd) const;
		nonvirtual	void	SetIntraCellSelection (size_t selStart, size_t selEnd);

	private:
		size_t	fRowSelStart;
		size_t	fRowSelEnd;
		size_t	fColSelStart;
		size_t	fColSelEnd;

	private:
		bool	fIntraCellMode;
		size_t	fIntraSelStart;
		size_t	fIntraSelEnd;
		size_t	fIntraCellDragAnchor;


	protected:
		nonvirtual	void	SaveIntraCellContextInfo (
									bool leftSideOfSelectionInteresting,
									const Led_FontSpecification& intraCellSelectionEmptySelFontSpecification
								);
		nonvirtual	bool	RestoreIntraCellContextInfo (
									bool* leftSideOfSelectionInteresting,
									Led_FontSpecification* intraCellSelectionEmptySelFontSpecification
								);
		nonvirtual	void	InvalidateIntraCellContextInfo ();

	private:
		bool					fSavedLeftSideOfSelectionInteresting;
		Led_FontSpecification	fSavedIntraCellSelectionEmptySelFontSpecification;
		bool					fSavedIntraCellInfoValid;

	private:
		size_t	fTrackingAnchor_Row;
		size_t	fTrackingAnchor_Col;


	private:
		class	SuppressCellUpdatePropagationContext;
		bool	fSuppressCellUpdatePropagationContext;

	protected:
		class	AllowUpdateInfoPropagationContext;
		bool						fAllowUpdateInfoPropagationContext;
		TextStore::SimpleUpdater*	fCellUpdatePropationUpdater;

	protected:
	#if		qAccessChecksFailFromTemplatesBug
		public:
	#endif
		class	EmbeddedTableWordProcessor;
	protected:
		virtual	EmbeddedTableWordProcessor*	ConstructEmbeddedTableWordProcessor (WordProcessor& forWordProcessor, size_t forRow, size_t forColumn, const Led_Rect& cellWindowRect, bool captureChangesForUndo);
		virtual	void						ReleaseEmbeddedTableWordProcessor (EmbeddedTableWordProcessor* e);

	public:
		virtual		void	GetCellWordProcessorDatabases (size_t row, size_t column,
														TextStore** ts,
														StandardStyledTextImager::StyleDatabasePtr* styleDatabase = NULL,
														WordProcessor::ParagraphDatabasePtr* paragraphDatabase = NULL,
														WordProcessor::HidableTextDatabasePtr* hidableTextDatabase = NULL
													);
	private:
		WordProcessor*	fCurrentOwningWP;
		#if		qAccessChecksSometimesBreakForNestedClasses
			public:
		#endif
		class	TemporarilySetOwningWP;
		class	TemporarilyAllocateCellWP;
		class	TemporarilyAllocateCellWithTablet;

	protected:
		nonvirtual	void		InvalidateLayout ();
		virtual		void		PerformLayout ();
	private:
		enum	LayoutFlag { eDone, eNeedFullLayout };
		mutable	LayoutFlag	fNeedLayout;

	public:
		nonvirtual	size_t	GetRowCount () const;
		nonvirtual	size_t	GetColumnCount () const;

	public:
		nonvirtual	size_t	GetColumnCount (size_t row) const;
		nonvirtual	size_t	GetColumnCount (size_t rowStart, size_t rowEnd) const;
		nonvirtual	void	SetColumnCount (size_t row, size_t columns);

	public:
		nonvirtual	void	GetDimensions (size_t* rows, size_t* columns) const;
		virtual		void	SetDimensions (size_t rows, size_t columns);
		nonvirtual	void	SetDimensionsAtLeast (size_t rows, size_t columns);
		virtual		void	InsertRow (size_t at, size_t maxRowCopyCount = 0xffffffff);
		virtual		void	DeleteRow (size_t at);
		virtual		void	InsertColumn (size_t at);
		virtual		void	DeleteColumn (size_t at);

	protected:
		virtual	void	ReValidateSelection ();

	protected:
		/*
		@CLASS:			WordProcessor::Table::RowInfo
		@DESCRIPTION:
			<p></p>
		*/
		struct	RowInfo  {
			public:
				RowInfo ();

			public:
				vector<Cell>	fCells;

			public:
				Led_Distance	fHeight;		// height of the cell itself (not including the border)
		};
		vector<RowInfo>	fRows;

	private:
		Led_TWIPS		fBorderWidth;
		Led_Color		fBorderColor;
		Led_Distance	fTotalWidth;
		Led_Distance	fTotalHeight;

	protected:
		class	SavedTextRepWSel;

	private:
		class	TableCMD;

	private:
		friend	class	SuppressCellUpdatePropagationContext;
		friend	class	AllowUpdateInfoPropagationContext;
		friend	class	TemporarilySetOwningWP;
		friend	class	TemporarilyAllocateCellWP;
		friend	class	TemporarilyAllocateCellWithTablet;
#if 1
		friend	class	EmbeddedTableWordProcessor;
		friend	class	CellRep;
#else
		friend	class	WordProcessor::Table::EmbeddedTableWordProcessor;
		friend	class	WordProcessor::Table::CellRep;
#endif
		friend	class	TableCMD;
		friend	class	WordProcessor::WPIdler;
		friend	class	WordProcessor;
};








/*
@CLASS:			WordProcessor::Table::Cell
@DESCRIPTION:	<p>Used internally by the @'WordProcessor::Table' code.</p>
*/
class	WordProcessor::Table::Cell {
	public:
		Cell (Table& forTable, CellMergeFlags mergeFlags);

	public:
		nonvirtual	CellMergeFlags	GetCellMergeFlags () const;

	private:
		CellMergeFlags	fCellMergeFlags;

	public:
		nonvirtual	void	GetCellWordProcessorDatabases (
														TextStore** ts,
														StandardStyledTextImager::StyleDatabasePtr* styleDatabase = NULL,
														WordProcessor::ParagraphDatabasePtr* paragraphDatabase = NULL,
														WordProcessor::HidableTextDatabasePtr* hidableTextDatabase = NULL
													);
		nonvirtual	TextStore&									GetTextStore () const;
		nonvirtual	StandardStyledTextImager::StyleDatabasePtr	GetStyleDatabase () const;
		nonvirtual	WordProcessor::ParagraphDatabasePtr			GetParagraphDatabase () const;
		nonvirtual	WordProcessor::HidableTextDatabasePtr		GetHidableTextDatabase () const;

		nonvirtual	Led_Color	GetBackColor () const;
		nonvirtual	void		SetBackColor (Led_Color c);

		nonvirtual	Led_TWIPS	GetCellXWidth () const;
		nonvirtual	void		SetCellXWidth (Led_TWIPS width);

		nonvirtual	Led_Rect	GetCachedBoundsRect () const;
		nonvirtual	void		SetCachedBoundsRect (Led_Rect r);

	public:
		Led_RefCntPtr<CellRep>	fCellRep;
};




/*
@CLASS:			WordProcessor::Table::CellRep
@DESCRIPTION:	<p>Used internally by the @'WordProcessor::Table' code.</p>
*/
class	WordProcessor::Table::CellRep : public MarkerOwner {
	private:
		typedef	MarkerOwner	inherited;
	public:
		CellRep (Table& forTable);
		~CellRep ();

	public:
		LED_DECLARE_USE_BLOCK_ALLOCATION (CellRep);

	public:
		override	TextStore*	PeekAtTextStore () const;
		override	void		AboutToUpdateText (const UpdateInfo& updateInfo);
		override	void		DidUpdateText (const UpdateInfo& updateInfo) throw ();

	public:
		Table&										fForTable;
		TextStore*									fTextStore;
		StandardStyledTextImager::StyleDatabasePtr	fStyleDatabase;
		WordProcessor::ParagraphDatabasePtr			fParagraphDatabase;
		WordProcessor::HidableTextDatabasePtr		fHidableTextDatabase;
		Led_Color									fBackColor;
		Led_Rect									fCachedBoundsRect;
		Led_TWIPS									fCellXWidth;
};


/*
@CLASS:			WordProcessor::Table::SuppressCellUpdatePropagationContext
@ACCESS:		private
@DESCRIPTION:	<p>.</p>
*/
class	WordProcessor::Table::SuppressCellUpdatePropagationContext {
	public:
		SuppressCellUpdatePropagationContext (Table& t);
		~SuppressCellUpdatePropagationContext ();

	private:
		Table&	fTable;
		bool	fOldVal;
};




/*
@CLASS:			WordProcessor::Table::AllowUpdateInfoPropagationContext
@ACCESS:		protected
@DESCRIPTION:	<p>.</p>
*/
class	WordProcessor::Table::AllowUpdateInfoPropagationContext {
	public:
		AllowUpdateInfoPropagationContext (Table& t);
		~AllowUpdateInfoPropagationContext ();

	private:
		Table&	fTable;
		bool	fOldVal;
};




/*
@CLASS:			WordProcessor::Table::EmbeddedTableWordProcessor
@BASES:			@'WordProcessor'
@DESCRIPTION:	<p>Used internally by the @'WordProcessor::Table' code for mini embedded word processor objects
			in each cell. A subclass of these are constructed by @'WordProcessor::ConstructEmbeddedTableWordProcessor'</p>
				<p>Note that we choose to instantiate this WP object with a WindowRect in the same coordinates as
			the the owning tables WindowRect: not relative to that table.
			</p>
*/
class	WordProcessor::Table::EmbeddedTableWordProcessor : public WordProcessor {
	private:
		typedef	WordProcessor	inherited;

	public:
		EmbeddedTableWordProcessor (WordProcessor& owningWordProcessor, Table& owningTable, size_t tRow, size_t tCol, bool activeEditCell);

	public:
		nonvirtual	WordProcessor&	GetOwningWordProcessor () const;
		nonvirtual	Table&			GetOwningTable () const;
	private:
		WordProcessor&	fOwningWordProcessor;
		Table&			fOwningTable;
		size_t			fTableRow;
		size_t			fTableColumn;

	public:
		nonvirtual	void	SaveMiscActiveFocusInfo ();
		nonvirtual	void	RestoreMiscActiveFocusInfo ();

#if		!qNestedTablesSupported
	protected:
		override	void	HookInternalizerChanged ();
#endif

	protected:
		override	bool	OnCopyCommand_Before ();
		override	void	OnCopyCommand_After ();
		override	bool	OnPasteCommand_Before ();
		override	void	OnPasteCommand_After ();

	protected:
		override	void		DrawRowHilight (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
									const TextLayoutBlock& text, size_t rowStart, size_t rowEnd
								);

	//pure virtual overrides to make this class compile..
	public:
		override	Led_Tablet	AcquireTablet () const;
		override	void		ReleaseTablet (Led_Tablet tablet) const;

	protected:
		override	void	RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const;
		override	void	UpdateWindowRect_ (const Led_Rect& windowRectArea) const;
		override	bool	QueryInputKeyStrokesPending () const;

	protected:
		override	void	SetDefaultUpdateMode (UpdateMode defaultUpdateMode);

	public:
		override	void	GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const;

	public:
		nonvirtual	Led_Distance	GetDesiredHeight () const;
	private:
		mutable	Led_Distance	fDesiredHeight;
		mutable	bool			fDesiredHeightValid;
		bool					fActiveEditCell;

	public:
		override	void	PostInteractiveUndoPostHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, InteractiveReplaceCommand::SavedTextRep** afterRep, size_t startOfInsert, const Led_SDK_String& cmdName);
	protected:
		override	InteractiveReplaceCommand::SavedTextRep*	InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd);

	private:
		class	DisableRefreshContext;

	private:
		bool	fSupressRefreshCalls;

	private:
		#if		qAccessChecksSometimesBreakForNestedClasses
			public:
		#endif
		class	TemporarilyUseTablet;
	private:
		Led_Tablet			fUpdateTablet;		// assigned in stack-based fasion during update/draw calls.
	private:
		friend	class	WordProcessor::Table;
		friend	class	WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet;
		friend	class	WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext;
		friend	class	WordProcessor::Table::TemporarilyAllocateCellWithTablet;
};





/*
@CLASS:			InteractiveReplaceCommand::SavedTextRep
@BASES:			@'WordProcessor::Table::SavedTextRepWSel'
@DESCRIPTION:	
*/
class	WordProcessor::Table::SavedTextRepWSel : public InteractiveReplaceCommand::SavedTextRep {
	private:
		typedef	InteractiveReplaceCommand::SavedTextRep	inherited;

#if		qNameLookupInBaseClassWhenItIsNestedSometimesFailsBug
	public:
		typedef	InteractiveReplaceCommand::SavedTextRep	SavedTextRep;
#endif

	public:
		LED_DECLARE_USE_BLOCK_ALLOCATION (SavedTextRepWSel);

	public:
		enum	WPRelativeFlag { eWPDirect, eWPAbove };

	public:
		#if		qNameLookupXXXInBaseClassWhenItIsNestedSometimesFailsBug
			SavedTextRepWSel (InteractiveReplaceCommand::SavedTextRep* delegateTo, Table& table, WPRelativeFlag wPRelativeFlag);
		#else
			SavedTextRepWSel (SavedTextRep* delegateTo, Table& table, WPRelativeFlag wPRelativeFlag);
		#endif

	public:
		override	size_t	GetLength () const;
		override	void	InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite);
		override	void	ApplySelection (TextInteractor* imager);

	private:
		WPRelativeFlag			fWPRelativeFlag;
	#if		qNameLookupXXXInBaseClassWhenItIsNestedSometimesFailsBug
		auto_ptr<InteractiveReplaceCommand::SavedTextRep>	fRealRep;
	#else
		auto_ptr<SavedTextRep>	fRealRep;
	#endif
		size_t					fRowSelStart;
		size_t					fRowSelEnd;
		size_t					fColSelStart;
		size_t					fColSelEnd;
		bool					fIntraCellMode;
		size_t					fIntraCellSelStart;
		size_t					fIntraCellSelEnd;
};




/*
@CLASS:			WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet 
@DESCRIPTION:	<p>Utility class to use (with caution), to temporarily force a given tablet to be
			used for a given @'WordProcessor::Table::EmbeddedTableWordProcessor'. NB: This causes
			the @'TextImager::TabletChangedMetrics' method by default
			(unless called with special arg).</p>
*/
class	WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet {
	public:
		typedef	WordProcessor::Table::EmbeddedTableWordProcessor	EmbeddedTableWordProcessor;
	public:
		enum DoTextMetricsChangedCall { eDoTextMetricsChangedCall, eDontDoTextMetricsChangedCall };
		TemporarilyUseTablet (EmbeddedTableWordProcessor& editor, Led_Tablet t, DoTextMetricsChangedCall tmChanged = eDoTextMetricsChangedCall);
		~TemporarilyUseTablet ();
	private:
		EmbeddedTableWordProcessor& fEditor;
		Led_Tablet					fOldTablet;
		DoTextMetricsChangedCall	fDoTextMetricsChangedCall;
};






/*
@CLASS:			WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext
@ACCESS:		private
@DESCRIPTION:	<p>Utility class to use (with caution), to temporarily force a given tablet to be
			used for a given @'WordProcessor::Table::EmbeddedTableWordProcessor'. NB: This causes
			the @'TextImager::TabletChangedMetrics' method by default
			(unless called with special arg).</p>
*/
class	WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext {
	public:
		DisableRefreshContext (EmbeddedTableWordProcessor& wp);
		~DisableRefreshContext ();

	private:
		EmbeddedTableWordProcessor&	fWP;
		bool						fOldVal;
};




/*
@CLASS:			WordProcessor::Table::TemporarilySetOwningWP 
@DESCRIPTION:	<p>Since a table is assocated with a paragraph database, and can be viewed and edited
			simultaneously be multiple views (WordProcessors) - we cannot make long-term assumptions
			about association of a table with an owning WordProcessor object. Instead - during UI operations
			like typing and so on - we temporarily associate an owning WP with the given table using
			this class.</p>
*/
class	WordProcessor::Table::TemporarilySetOwningWP {
	public:
		TemporarilySetOwningWP (const Table& forTable, WordProcessor& forWordProcessor);
		~TemporarilySetOwningWP ();

	private:
		Table&						fOwningTable;
		WordProcessor*				fSavedTableOwningWP;		// in case references are nested, though I'm not sure this can happen
};





/*
@CLASS:			WordProcessor::Table::TemporarilyAllocateCellWP 
@DESCRIPTION:	<p></p>
*/
class	WordProcessor::Table::TemporarilyAllocateCellWP {
	public:
		typedef	WordProcessor::Table::EmbeddedTableWordProcessor	EmbeddedTableWordProcessor;
	public:
		TemporarilyAllocateCellWP (Table& forTable, WordProcessor& forWordProcessor, size_t forRow, size_t forColumn, const Led_Rect& cellWindowRect, bool captureChangesForUndo = true);
		~TemporarilyAllocateCellWP ();

		nonvirtual	operator	EmbeddedTableWordProcessor* ();
		nonvirtual	EmbeddedTableWordProcessor& operator& ();
		nonvirtual	EmbeddedTableWordProcessor* operator-> ();

	private:
		Table&						fOwningTable;
		EmbeddedTableWordProcessor* fCellEditor;
};






/*
@CLASS:			WordProcessor::Table::TemporarilyAllocateCellWithTablet 
@DESCRIPTION:	<p></p>
*/
class	WordProcessor::Table::TemporarilyAllocateCellWithTablet {
	public:
		typedef	TextInteractor::Tablet_Acquirer	Tablet_Acquirer;	// needed for GCC 2.96 - seems like the requirement maybe a compiler bug... LGP 2003-04-18

	public:
		TemporarilyAllocateCellWithTablet (Table& forTable, size_t row, size_t column, bool captureChangesForUndo = true);

	public:
		typedef	EmbeddedTableWordProcessor::TemporarilyUseTablet	TemporarilyUseTablet;

	public:
		nonvirtual	operator	EmbeddedTableWordProcessor* ();
		nonvirtual	EmbeddedTableWordProcessor& operator& ();
		nonvirtual	EmbeddedTableWordProcessor* operator-> ();

	private:
		TemporarilyAllocateCellWP	fWP;
		Tablet_Acquirer				fTablet;
		TemporarilyUseTablet		fTmpUseTablet;
};





/*
@CLASS:			WordProcessor::CommandNames
@DESCRIPTION:	<p>Command names for each of the user-visible commands produced by the @'WordProcessor' module.
			This name is used used in the constructed Undo command name, as
			in, "Undo Justification". You can replace this name with whatever you like.
			You change this value with @'WordProcessor::SetCommandNames'.</p>
				<p> The point of this is to allow for different UI-language localizations,
			without having to change Led itself. Note also that these strings aren't always exactly
			a command-name. Sometimes they are just other strings to be localized.</p>
				<p>See also @'WordProcessor::GetCommandNames'.</p>
*/
struct	WordProcessor::CommandNames {
	Led_SDK_String	fJustificationCommandName;
	Led_SDK_String	fStandardTabStopListCommandName;
	Led_SDK_String	fMarginsCommandName;
	Led_SDK_String	fFirstIndentCommandName;
	Led_SDK_String	fMarginsAndFirstIndentCommandName;
	Led_SDK_String	fParagraphSpacingCommandName;
	Led_SDK_String	fHideCommandName;
	Led_SDK_String	fUnHideCommandName;
	Led_SDK_String	fSetListStyleCommandName;
	Led_SDK_String	fIndentLevelChangeCommandName;
	Led_SDK_String	fInsertTableCommandName;
	Led_SDK_String	fInsertTableRowAboveCommandName;
	Led_SDK_String	fInsertTableRowBelowCommandName;
	Led_SDK_String	fInsertTableColBeforeCommandName;
	Led_SDK_String	fInsertTableColAfterCommandName;
	Led_SDK_String	fInsertURLCommandName;
	Led_SDK_String	fRemoveTableRowsCommandName;
	Led_SDK_String	fRemoveTableColumnsCommandName;
	Led_SDK_String	fEmbeddingTypeName_ImageDIB;
	Led_SDK_String	fEmbeddingTypeName_URL;
	Led_SDK_String	fEmbeddingTypeName_ImageMacPict;
	Led_SDK_String	fEmbeddingTypeName_Table;
	Led_SDK_String	fEmbeddingTypeName_Unknown;
	Led_SDK_String	fFontSizeChange_Other_NoArg;
	Led_SDK_String	fFontSizeChange_Other_OneArg;
	Led_SDK_String	fTablePropertiesCommandName;
	Led_SDK_String	fGenericEmbeddingPropertiesCommandName;
	Led_SDK_String	fChangeTablePropertiesCommandName;
};






/*
@CLASS:			WordProcessor::DialogSupport
@DESCRIPTION:	<p></p>
*/
class	WordProcessor::DialogSupport {
	public:
		typedef	TextInteractor::CommandNumber				CommandNumber;
		typedef	Led_FontSpecification::FontNameSpecifier	FontNameSpecifier;

	public:
		virtual		FontNameSpecifier	CmdNumToFontName (CommandNumber cmdNum);
		virtual		bool				IsPredefinedFontSize (Led_Distance fontSize);	// return true iff the arg fontSize is one of the predefined ones from the menu
		virtual		Led_Distance		FontCmdToSize (CommandNumber commandNum);
		virtual		Led_Distance		PickOtherFontHeight (Led_Distance origHeight);	// display UI (dialog box) and query user for new height)
		virtual		bool				PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid);
		virtual		bool				PickNewParagraphMarginsAndFirstIndent (Led_TWIPS* leftMargin, bool* leftMarginValid, Led_TWIPS* rightMargin, bool* rightMarginValid, Led_TWIPS* firstIndent, bool* firstIndentValid);
		virtual		Led_Color			FontCmdToColor (CommandNumber cmd);
		virtual		CommandNumber		FontColorToCmd (Led_Color color);
		virtual		bool				PickOtherFontColor (Led_Color* color);
		virtual		bool				ChooseFont (Led_IncrementalFontSpecification* font);	// display UI (e.g. CFontDialog) to pick a new font/color
		virtual		void				ShowSimpleEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName);
		virtual		bool				ShowURLEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName, Led_SDK_String* urlTitle, Led_SDK_String* urlValue);
		virtual		bool				ShowAddURLEmbeddingInfoDialog (Led_SDK_String* urlTitle, Led_SDK_String* urlValue);
		virtual		bool				AddNewTableDialog (size_t* nRows, size_t* nCols);
		struct	TableSelectionPropertiesInfo;
		virtual		bool				EditTablePropertiesDialog (TableSelectionPropertiesInfo* tableProperties);

	#if		qWindows
	private:
		static	UINT CALLBACK	ColorPickerINITPROC (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	#endif
};



/*
@CLASS:			WordProcessor::DialogSupport::TableSelectionPropertiesInfo
@DESCRIPTION:	<p></p>
*/
struct	WordProcessor::DialogSupport::TableSelectionPropertiesInfo {
	TableSelectionPropertiesInfo ();
	
	Led_TWIPS		fTableBorderWidth;
	Led_Color		fTableBorderColor;

	Led_TWIPS_Rect	fDefaultCellMargins;
	Led_TWIPS		fCellSpacing;

	bool			fCellWidth_Common;
	Led_TWIPS		fCellWidth;

	bool			fCellBackgroundColor_Common;
	Led_Color		fCellBackgroundColor;
};





/*
@CLASS:			EmptySelectionParagraphSavedTextRep<BASECLASS>
@BASES:			@'StandardStyledTextInteractor::EmptySelStyleTextRep'
@DESCRIPTION:	
*/
class	WordProcessor::EmptySelectionParagraphSavedTextRep : public StandardStyledTextInteractor::EmptySelStyleTextRep {
	private:
		typedef	StandardStyledTextInteractor::EmptySelStyleTextRep	inherited;
	public:
		EmptySelectionParagraphSavedTextRep (WordProcessor* interactor, size_t selStart, size_t selEnd, size_t at);

	public:
		override	void	InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite);

	private:
		ParagraphInfo	fSavedStyleInfo;
};




#if		qSupportLed30CompatAPI
/*
@CLASS:			WordProcessorHScrollbarHelper<BASECLASS>
@BASES:			BASECLASS
@DESCRIPTION:	<p>Deprecated. Only available if @'qSupportLed30CompatAPI' defined.</p>
*/
template	<typename	BASECLASS>	class	WordProcessorHScrollbarHelper :
	public BASECLASS
	{
	};
#endif









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if		qLedUsesNamespaces
}
#endif
#include	<cstdio>	// for sprintf() call
#if		qLedUsesNamespaces
namespace	Led {
#endif

	#if		qTemplateGeneratedMixinsSometimesCorrupted
	inline	void	InteractorInteractorMixinHelper<StandardStyledTextInteractor,WordWrappedTextInteractor>::HookLosingTextStore ()
			{
				StandardStyledTextInteractor::HookLosingTextStore ();
				WordWrappedTextInteractor::HookLosingTextStore ();
			}
	inline	void	InteractorInteractorMixinHelper<StandardStyledTextInteractor,WordWrappedTextInteractor>::HookGainedNewTextStore ()
			{
				StandardStyledTextInteractor::HookGainedNewTextStore ();
				WordWrappedTextInteractor::HookGainedNewTextStore ();
			}
	inline	void	InteractorInteractorMixinHelper<StandardStyledTextInteractor,WordWrappedTextInteractor>::DidUpdateText (const UpdateInfo& updateInfo)
			{
				StandardStyledTextInteractor::DidUpdateText (updateInfo);
				WordWrappedTextInteractor::DidUpdateText (updateInfo);
			}
	#endif



//	class	StandardStyledWordWrappedTextInteractor
	inline	StandardStyledWordWrappedTextInteractor::StandardStyledWordWrappedTextInteractor ():
			inherited ()
		{
		}



//	class	WordProcessor::ParagraphInfo
	/*
	@METHOD:		WordProcessor::ParagraphInfo::GetJustification
	@DESCRIPTION:
		<p>Return the @'Led_Justification' setting.</p>
	*/
	inline	Led_Justification	WordProcessor::ParagraphInfo::GetJustification () const
		{
			return fJustification;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::SetJustification
	@DESCRIPTION:
		<p>Set the @'Led_Justification' setting.</p>
	*/
	inline	void	WordProcessor::ParagraphInfo::SetJustification (Led_Justification justification)
		{
			fJustification = justification;
		}
	inline	const WordProcessor::ParagraphInfo::StandardTabStopList&	WordProcessor::ParagraphInfo::GetTabStopList () const
		{
			return fTabStops;
		}
	inline	void	WordProcessor::ParagraphInfo::SetTabStopList (const StandardTabStopList& tabStops)
		{
			fTabStops = tabStops;
		}
	inline	Led_TWIPS	WordProcessor::ParagraphInfo::GetLeftMargin () const
		{
			return fLeftMargin;
		}
	inline	Led_TWIPS	WordProcessor::ParagraphInfo::GetRightMargin () const
		{
			return fRightMargin;
		}
	inline	void	WordProcessor::ParagraphInfo::SetMargins (Led_TWIPS lhs, Led_TWIPS rhs)
		{
			Led_Require (lhs < rhs);
			fLeftMargin = lhs;
			fRightMargin = rhs;
		}
	inline	Led_TWIPS	WordProcessor::ParagraphInfo::GetFirstIndent () const
		{
			return fFirstIndent;
		}
	inline	void	WordProcessor::ParagraphInfo::SetFirstIndent (Led_TWIPS firstIndent)
		{
			fFirstIndent = firstIndent;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::GetSpaceBefore
	@DESCRIPTION:
					<p>Get the 'space before' attribute of the given paragraph. This is the number of TWIPS of space inserted before
				the paragraph - and defaults to zero. See the RTF \sb tag.</p>
					<p>See also @'WordProcessor::ParagraphInfo::SetSpaceBefore'.</p>
	*/
	inline	Led_TWIPS	WordProcessor::ParagraphInfo::GetSpaceBefore () const
		{
			return fSpaceBefore;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::SetSpaceBefore
	@DESCRIPTION:
			<p>See also @'WordProcessor::ParagraphInfo::GetSpaceBefore'.</p>
	*/
	inline	void	WordProcessor::ParagraphInfo::SetSpaceBefore (Led_TWIPS sb)
		{
			fSpaceBefore = sb;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::GetSpaceAfter
	@DESCRIPTION:
			<p>Get the 'space after' attribute of the given paragraph. This is the number of TWIPS of space appended after
		the paragraph - and defaults to zero. See the RTF \sa tag.</p>
			<p>See also @'WordProcessor::ParagraphInfo::SetSpaceAfter'.</p>
	*/
	inline	Led_TWIPS	WordProcessor::ParagraphInfo::GetSpaceAfter () const
		{
			return fSpaceAfter;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::SetSpaceAfter
	@DESCRIPTION:
			<p>See also @'WordProcessor::ParagraphInfo::GetSpaceAfter'.</p>
	*/
	inline	void	WordProcessor::ParagraphInfo::SetSpaceAfter (Led_TWIPS sa)
		{
			fSpaceAfter = sa;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::GetLineSpacing
	@DESCRIPTION:
			<p>Get the spacing used between rows of a given paragraph. Can either be magic value of 1000 - meaning automatic.</p>
			<p>From the RTF docs:
				<blockquote>
				If this control word is missing or if \sl1000 is used, the line spacing is automatically
				determined by the tallest character in the line; if N is a positive value, this size is
				used only if it is taller than the tallest character (otherwise, the tallest character is used);
				if N is a negative value, the absolute value of N is used, even if it is shorter than the tallest character.
				</blockquote>
			</p>
			<p>See the RTF docs for \slN and \slmultN</p>
			<p>See also @'WordProcessor::ParagraphInfo::SetLineSpacing'.</p>
	*/
	inline	Led_LineSpacing	WordProcessor::ParagraphInfo::GetLineSpacing () const
		{
			return fLineSpacing;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::SetLineSpacing
	@DESCRIPTION:
			<p>See also @'WordProcessor::ParagraphInfo::GetLineSpacing'.</p>
	*/
	inline	void	WordProcessor::ParagraphInfo::SetLineSpacing (Led_LineSpacing sl)
		{
			fLineSpacing = sl;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::GetListStyle
	@DESCRIPTION:
			<p>Retrieve the @'WordProcessor::ParagraphInfo::ListStyle' property associated with this paragraph
		(e.g. eListStyle_Bullet or eListStyle_None).</p>
	*/
	inline	ListStyle	WordProcessor::ParagraphInfo::GetListStyle () const
		{
			return fListStyle;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::SetListStyle
	@DESCRIPTION:
			<p>See also @'WordProcessor::ParagraphInfo::GetListStyle'.</p>
	*/
	inline	void	WordProcessor::ParagraphInfo::SetListStyle (ListStyle listStyle)
		{
			fListStyle = listStyle;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::GetListIndentLevel
	@DESCRIPTION:
			<p>Retrieve the indent level for the given list. Only valid value if @'WordProcessor::ParagraphInfo::GetListStyle'
		is not eListStyle_None.</p>
	*/
	inline	unsigned char	WordProcessor::ParagraphInfo::GetListIndentLevel () const
		{
			return fListIndentLevel;
		}
	/*
	@METHOD:		WordProcessor::ParagraphInfo::SetListIndentLevel
	@DESCRIPTION:
			<p>See also @'WordProcessor::ParagraphInfo::GetListIndentLevel'.</p>
	*/
	inline	void	WordProcessor::ParagraphInfo::SetListIndentLevel (unsigned char indentLevel)
		{
			fListIndentLevel = indentLevel;
		}
	inline	void	WordProcessor::ParagraphInfo::MergeIn (const IncrementalParagraphInfo& incParaInfo)
		{
			if (incParaInfo.GetJustification_Valid ()) {
				fJustification = incParaInfo.GetJustification ();
			}
			if (incParaInfo.GetTabStopList_Valid ()) {
				fTabStops = incParaInfo.GetTabStopList ();
			}
			if (incParaInfo.GetMargins_Valid ()) {
				fLeftMargin = incParaInfo.GetLeftMargin ();
				fRightMargin = incParaInfo.GetRightMargin ();
			}
			if (incParaInfo.GetFirstIndent_Valid ()) {
				fFirstIndent = incParaInfo.GetFirstIndent ();
			}
			if (incParaInfo.GetSpaceBefore_Valid ()) {
				fSpaceBefore = incParaInfo.GetSpaceBefore ();
			}
			if (incParaInfo.GetSpaceAfter_Valid ()) {
				fSpaceAfter = incParaInfo.GetSpaceAfter ();
			}
			if (incParaInfo.GetLineSpacing_Valid ()) {
				fLineSpacing = incParaInfo.GetLineSpacing ();
			}
			if (incParaInfo.GetListStyle_Valid ()) {
				fListStyle = incParaInfo.GetListStyle ();
			}
			if (incParaInfo.GetListIndentLevel_Valid ()) {
				fListIndentLevel = incParaInfo.GetListIndentLevel ();
			}
		}
	inline	bool	WordProcessor::ParagraphInfo::operator== (const ParagraphInfo& rhs) const
		{
			return	fJustification == rhs.fJustification and
					fTabStops == rhs.fTabStops and
					fLeftMargin == rhs.fLeftMargin and 
					fRightMargin == rhs.fRightMargin and 
					fFirstIndent == rhs.fFirstIndent and
					fSpaceBefore == rhs.fSpaceBefore and
					fSpaceAfter == rhs.fSpaceAfter and
					fLineSpacing == rhs.fLineSpacing and
					fListStyle == rhs.fListStyle and
					fListIndentLevel == rhs.fListIndentLevel
					;
		}
	inline	bool	WordProcessor::ParagraphInfo::operator!= (const ParagraphInfo& rhs) const
		{
			return not (*this == rhs);
		}




//	class	IncrementalParagraphInfo;
	inline	WordProcessor::IncrementalParagraphInfo::IncrementalParagraphInfo ():
			inherited (),
			fJustificationValid (false),
			fTabStopListValid (false),
			fMarginsValid (false),
			fFirstIndentValid (false),
			fSpaceBeforeValid (false),
			fSpaceAfterValid (false),
			fLineSpacingValid (false),
			fListStyleValid (false),
			fListIndentLevelValid (false)
		{
		}
	inline	WordProcessor::IncrementalParagraphInfo::IncrementalParagraphInfo (const ParagraphInfo& pi):
			inherited (pi),
			fJustificationValid (true),
			fTabStopListValid (true),
			fMarginsValid (true),
			fFirstIndentValid (true),
			fSpaceBeforeValid (true),
			fSpaceAfterValid (true),
			fLineSpacingValid (true),
			fListStyleValid (true),
			fListIndentLevelValid (true)
		{
		}
	inline	Led_Justification	WordProcessor::IncrementalParagraphInfo::GetJustification () const
		{
			Led_Require (fJustificationValid);
			return inherited::GetJustification ();
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::SetJustification (Led_Justification justification)
		{
			fJustificationValid = true;
			inherited::SetJustification (justification);
		}
	inline	bool		WordProcessor::IncrementalParagraphInfo::GetJustification_Valid () const
		{
			return fJustificationValid;
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::InvalidateJustification ()
		{
			fJustificationValid = false;
		}
	inline	const TextImager::StandardTabStopList&	WordProcessor::IncrementalParagraphInfo::GetTabStopList () const
		{
			Led_Require (fTabStopListValid);
			return inherited::GetTabStopList ();
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::SetTabStopList (const StandardTabStopList& tabStops)
		{
			fTabStopListValid = true;
			inherited::SetTabStopList (tabStops);
		}
	inline	bool	WordProcessor::IncrementalParagraphInfo::GetTabStopList_Valid () const
		{
			return fTabStopListValid;
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::InvalidateTabStopList ()
		{
			fTabStopListValid = false;
		}
	inline	Led_TWIPS	WordProcessor::IncrementalParagraphInfo::GetLeftMargin () const
		{
			Led_Require (fMarginsValid);
			return inherited::GetLeftMargin ();
		}
	inline	Led_TWIPS	WordProcessor::IncrementalParagraphInfo::GetRightMargin () const
		{
			Led_Require (fMarginsValid);
			return inherited::GetRightMargin ();
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::SetMargins (Led_TWIPS lhs, Led_TWIPS rhs)
		{
			fMarginsValid = true;
			inherited::SetMargins (lhs, rhs);
		}
	inline	bool	WordProcessor::IncrementalParagraphInfo::GetMargins_Valid () const
		{
			return fMarginsValid;
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::InvalidateMargins ()
		{
			fMarginsValid = false;
		}
	inline	Led_TWIPS	WordProcessor::IncrementalParagraphInfo::GetFirstIndent () const
		{
			Led_Require (fFirstIndentValid);
			return inherited::GetFirstIndent ();
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::SetFirstIndent (Led_TWIPS firstIndent)
		{
			fFirstIndentValid = true;
			inherited::SetFirstIndent (firstIndent);
		}
	inline	bool	WordProcessor::IncrementalParagraphInfo::GetFirstIndent_Valid () const
		{
			return fFirstIndentValid;
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::InvalidateFirstIndent ()
		{
			fFirstIndentValid = false;
		}
	inline	Led_TWIPS	WordProcessor::IncrementalParagraphInfo::GetSpaceBefore () const
		{
			Led_Require (fSpaceBeforeValid);
			return inherited::GetSpaceBefore ();
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::SetSpaceBefore (Led_TWIPS sb)
		{
			fSpaceBeforeValid = true;
			inherited::SetSpaceBefore (sb);
		}
	inline	bool	WordProcessor::IncrementalParagraphInfo::GetSpaceBefore_Valid () const
		{
			return fSpaceBeforeValid;
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::InvalidateSpaceBefore ()
		{
			fSpaceBeforeValid = false;
		}
	inline	Led_TWIPS	WordProcessor::IncrementalParagraphInfo::GetSpaceAfter () const
		{
			Led_Require (fSpaceAfterValid);
			return inherited::GetSpaceAfter ();
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::SetSpaceAfter (Led_TWIPS sa)
		{
			fSpaceAfterValid = true;
			inherited::SetSpaceAfter (sa);
		}
	inline	bool	WordProcessor::IncrementalParagraphInfo::GetSpaceAfter_Valid () const
		{
			return fSpaceAfterValid;
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::InvalidateSpaceAfter ()
		{
			fSpaceAfterValid = false;
		}
	inline	Led_LineSpacing	WordProcessor::IncrementalParagraphInfo::GetLineSpacing () const
		{
			Led_Require (fLineSpacingValid);
			return inherited::GetLineSpacing ();
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::SetLineSpacing (Led_LineSpacing sl)
		{
			fLineSpacingValid = true;
			inherited::SetLineSpacing (sl);
		}
	inline	bool	WordProcessor::IncrementalParagraphInfo::GetLineSpacing_Valid () const
		{
			return fLineSpacingValid;
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::InvalidateLineSpacing ()
		{
			fLineSpacingValid = false;
		}
	inline	ListStyle	WordProcessor::IncrementalParagraphInfo::GetListStyle () const
		{
			Led_Require (fListStyleValid);
			return inherited::GetListStyle ();
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::SetListStyle (ListStyle listStyle)
		{
			fListStyleValid = true;
			inherited::SetListStyle (listStyle);
		}
	inline	bool	WordProcessor::IncrementalParagraphInfo::GetListStyle_Valid () const
		{
			return fListStyleValid;
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::InvalidateListStyle ()
		{
			fListStyleValid = false;
		}
	inline	unsigned char	WordProcessor::IncrementalParagraphInfo::GetListIndentLevel () const
		{
			Led_Require (fListIndentLevelValid);
			return inherited::GetListIndentLevel ();
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::SetListIndentLevel (unsigned char indentLevel)
		{
			fListIndentLevelValid = true;
			inherited::SetListIndentLevel (indentLevel);
		}
	inline	bool	WordProcessor::IncrementalParagraphInfo::GetListIndentLevel_Valid () const
		{
			return fListIndentLevelValid;
		}
	inline	void	WordProcessor::IncrementalParagraphInfo::InvalidateListIndentLevel ()
		{
			fListIndentLevelValid = false;
		}
	inline	bool	WordProcessor::IncrementalParagraphInfo::operator== (const IncrementalParagraphInfo& rhs) const
		{
			//Justification
			if (GetJustification_Valid () != rhs.GetJustification_Valid ()) {
				return false;
			}
			if (GetJustification_Valid () and (GetJustification () != rhs.GetJustification ())) {
				return false;
			}

			//TabStopList
			if (GetTabStopList_Valid () != rhs.GetTabStopList_Valid ()) {
				return false;
			}
			if (GetTabStopList_Valid () and (GetTabStopList () != rhs.GetTabStopList ())) {
				return false;
			}

			//Margins
			if (GetMargins_Valid () != rhs.GetMargins_Valid ()) {
				return false;
			}
			if (GetMargins_Valid () and (GetLeftMargin () != rhs.GetLeftMargin () or GetRightMargin () != rhs.GetRightMargin ())) {
				return false;
			}

			//FirstIndent
			if (GetFirstIndent_Valid () != rhs.GetFirstIndent_Valid ()) {
				return false;
			}
			if (GetFirstIndent_Valid () and (GetFirstIndent () != rhs.GetFirstIndent ())) {
				return false;
			}

			//SpaceBefore
			if (GetSpaceBefore_Valid () != rhs.GetSpaceBefore_Valid ()) {
				return false;
			}
			if (GetSpaceBefore_Valid () and (GetSpaceBefore () != rhs.GetSpaceBefore ())) {
				return false;
			}

			//SpaceAfter
			if (GetSpaceAfter_Valid () != rhs.GetSpaceAfter_Valid ()) {
				return false;
			}
			if (GetSpaceAfter_Valid () and (GetSpaceAfter () != rhs.GetSpaceAfter ())) {
				return false;
			}

			//LineSpacing
			if (GetLineSpacing_Valid () != rhs.GetLineSpacing_Valid ()) {
				return false;
			}
			if (GetLineSpacing_Valid () and (GetLineSpacing () != rhs.GetLineSpacing ())) {
				return false;
			}

			//ListStyle/IndentLevel
			if (GetListStyle_Valid () != rhs.GetListStyle_Valid ()) {
				return false;
			}
			if (GetListStyle_Valid () and (GetListStyle () != rhs.GetListStyle ())) {
				return false;
			}
			if (GetListIndentLevel_Valid () != rhs.GetListIndentLevel_Valid ()) {
				return false;
			}
			if (GetListIndentLevel_Valid () and (GetListIndentLevel () != rhs.GetListIndentLevel ())) {
				return false;
			}

			return true;
		}
	inline	bool	WordProcessor::IncrementalParagraphInfo::operator!= (const IncrementalParagraphInfo& rhs) const
		{
			return not (*this == rhs);
		}




//	class	WordProcessor::AbstractParagraphDatabaseRep
	inline	WordProcessor::AbstractParagraphDatabaseRep::AbstractParagraphDatabaseRep ():
		fSomeInvalidTables (false),
		fCachedFarthestRightMarginInDocument (WordProcessor::kBadCachedFarthestRightMarginInDocument)
		{
		}



//	class	WordProcessor::ParagraphInfoMarker
	inline	WordProcessor::ParagraphInfoMarker::ParagraphInfoMarker (ParagraphInfo paragraphInfo):
			fParagraphInfo (paragraphInfo)
		{
		}
	inline	const WordProcessor::ParagraphInfo&	WordProcessor::ParagraphInfoMarker::GetInfo () const
		{
			return fParagraphInfo;
		}
	inline	void					WordProcessor::ParagraphInfoMarker::SetInfo (ParagraphInfo paragraphInfo)
		{
			fParagraphInfo = paragraphInfo;
		}



//	class	WordProcessor::ParagraphDatabaseRep
	inline	WordProcessor::ParagraphDatabaseRep::PartitionPtr	WordProcessor::ParagraphDatabaseRep::GetPartition () const
		{
			return fPartition;
		}





//	class	WordProcessor::WordProcessorTextIOSinkStream
	inline	bool	WordProcessor::WordProcessorTextIOSinkStream::GetOverwriteTableMode () const
		{
			return fOverwriteTableMode;
		}
	inline	void	WordProcessor::WordProcessorTextIOSinkStream::SetOverwriteTableMode (bool overwriteTableMode)
		{
			fOverwriteTableMode = overwriteTableMode;
		}
#if		!qNestedTablesSupported
	inline	bool	WordProcessor::WordProcessorTextIOSinkStream::GetNoTablesAllowed () const
		{
			return fNoTablesAllowed;
		}
	inline	void	WordProcessor::WordProcessorTextIOSinkStream::SetNoTablesAllowed (bool noTablesAllowed)
		{
			fNoTablesAllowed = noTablesAllowed;
		}
#endif





//	class	WordProcessor::WordProcessorTextIOSrcStream
	inline	bool	WordProcessor::WordProcessorTextIOSrcStream::GetUseTableSelection () const
		{
			return fUseTableSelection;
		}
	inline	void	WordProcessor::WordProcessorTextIOSrcStream::SetUseTableSelection (bool useTableSelection)
		{
			fUseTableSelection = useTableSelection;
		}





//	class	WordProcessor
#if		qWideCharacters
	/*
	@METHOD:		WordProcessor::GetSmartQuoteMode
	@DESCRIPTION:	<p>If true, then when a user types a quote character (&quot;) - it will be replaced
				with either an OPEN quote character or a CLOSE quote character (depending on text context).
					<p>Note this defaults ON if in UNICODE mode (@'qWideCharacters') and is unavailable otherwise.</p>
					<p>See also @'WordProcessor::SetSmartQuoteMode'</p>
	*/
	inline	bool	WordProcessor::GetSmartQuoteMode () const
		{
			return fSmartQuoteMode;
		}
	/*
	@METHOD:		WordProcessor::SetSmartQuoteMode
	@DESCRIPTION:	<p>See also @'WordProcessor::GetSmartQuoteMode'</p>
	*/
	inline	void	WordProcessor::SetSmartQuoteMode (bool smartQuoteMode)
		{
			fSmartQuoteMode = smartQuoteMode;
		}
#endif
	inline	WordProcessor::ParagraphDatabasePtr	WordProcessor::GetParagraphDatabase () const
		{
			return fParagraphDatabase;
		}
	inline	WordProcessor::HidableTextDatabasePtr	WordProcessor::GetHidableTextDatabase () const
		{
			return fHidableTextDatabase;
		}
	/*
	@METHOD:		WordProcessor::GetShowParagraphGlyphs
	@DESCRIPTION:	<p>See @'WordProcessor::SetShowParagraphGlyphs'.</p>
	*/
	inline	bool	WordProcessor::GetShowParagraphGlyphs () const
		{
			return fShowParagraphGlyphs;
		}
	/*
	@METHOD:		WordProcessor::GetShowTabGlyphs
	@DESCRIPTION:	<p>See @'WordProcessor::SetShowTabGlyphs'.</p>
	*/
	inline	bool	WordProcessor::GetShowTabGlyphs () const
		{
			return fShowTabGlyphs;
		}
	/*
	@METHOD:		WordProcessor::GetShowSpaceGlyphs
	@DESCRIPTION:	<p>See @'WordProcessor::SetShowSpaceGlyphs'.</p>
	*/
	inline	bool	WordProcessor::GetShowSpaceGlyphs () const
		{
			return fShowSpaceGlyphs;
		}
	/*
	@METHOD:		WordProcessor::GetCommandNames
	@DESCRIPTION:	<p>Returns command name for each of the user-visible commands produced by this module.
				This name is used used in the constructed Undo command name, as
				in, "Undo Justification". You can replace this name with whatever you like.You change this value with
				WordProcessor::SetCommandNames.</p>
					<p> The point of this is to allow for different UI-language localizations,
				without having to change Led itself.</p>
					<p>See also @'WordProcessor::CommandNames'.</p>
	*/
	inline	const WordProcessor::CommandNames&	WordProcessor::GetCommandNames ()
		{
			return sCommandNames;
		}
	/*
	@METHOD:		WordProcessor::SetCommandNames
	@DESCRIPTION:	<p>See @'WordProcessor::GetCommandNames'.</p>
	*/
	inline	void	WordProcessor::SetCommandNames (const WordProcessor::CommandNames& cmdNames)
		{
			sCommandNames = cmdNames;
		}
	/*
	@METHOD:		TextInteractor::GetDialogSupport
	@DESCRIPTION:	<p></p>
	*/
	inline	WordProcessor::DialogSupport&	WordProcessor::GetDialogSupport ()
		{
			if (sDialogSupport == NULL) {
				static	DialogSupport	sDefSup;
				sDialogSupport = &sDefSup;
			}
			return *sDialogSupport;
		}
	/*
	@METHOD:		WordProcessor::SetDialogSupport
	@DESCRIPTION:	<p></p>
	*/
	inline	void	WordProcessor::SetDialogSupport (DialogSupport* ds)
		{
			sDialogSupport = ds;
		}
	inline	Led_IncrementalFontSpecification	WordProcessor::GetCurSelFontSpec () const
		{
			if (not fCachedCurSelFontSpecValid) {
				AssureCurSelFontCacheValid ();
			}
			return fCachedCurSelFontSpec;
		}







//class	WordProcessor::Table::RowInfo
	inline	WordProcessor::Table::RowInfo::RowInfo ():
			fHeight (0)
		{
		}








//class	WordProcessor::Table::Cell
	inline	WordProcessor::Table::CellMergeFlags	WordProcessor::Table::Cell::GetCellMergeFlags () const
		{
			return fCellMergeFlags;
		}
	inline	Led_TWIPS	WordProcessor::Table::Cell::GetCellXWidth () const
		{
			return fCellRep->fCellXWidth;
		}
	inline	void		WordProcessor::Table::Cell::SetCellXWidth (Led_TWIPS width)
		{
			fCellRep->fCellXWidth = width;
		}
	inline	Led_Rect	WordProcessor::Table::Cell::GetCachedBoundsRect () const
		{
			return fCellRep->fCachedBoundsRect;
		}
	inline	void	WordProcessor::Table::Cell::SetCachedBoundsRect (Led_Rect r)
		{
			fCellRep->fCachedBoundsRect = r;
		}





//	class	WordProcessor::WordProcessorFlavorPackageInternalizer
	inline	bool	WordProcessor::WordProcessorFlavorPackageInternalizer::GetOverwriteTableMode () const
		{
			return fOverwriteTableMode;
		}
	inline	void	WordProcessor::WordProcessorFlavorPackageInternalizer::SetOverwriteTableMode (bool overwriteTableMode)
		{
			fOverwriteTableMode = overwriteTableMode;
		}
#if		!qNestedTablesSupported
	inline	bool	WordProcessor::WordProcessorFlavorPackageInternalizer::GetNoTablesAllowed () const
		{
			return fNoTablesAllowed;
		}
	inline	void	WordProcessor::WordProcessorFlavorPackageInternalizer::SetNoTablesAllowed (bool noTablesAllowed)
		{
			fNoTablesAllowed = noTablesAllowed;
		}
#endif






//	class	WordProcessor::WordProcessorFlavorPackageExternalizer
	inline	bool	WordProcessor::WordProcessorFlavorPackageExternalizer::GetUseTableSelection () const
		{
			return fUseTableSelection;
		}
	inline	void	WordProcessor::WordProcessorFlavorPackageExternalizer::SetUseTableSelection (bool useTableSelection)
		{
			fUseTableSelection = useTableSelection;
		}



//class	WordProcessor::Table
	inline	Led_TWIPS	WordProcessor::Table::GetCellSpacing () const
		{
			return fCellSpacing;

		}
	inline	void		WordProcessor::Table::SetCellSpacing (Led_TWIPS cellSpacing)
		{
			if (fCellSpacing != cellSpacing) {
				fCellSpacing = cellSpacing;
				InvalidateLayout ();
			}
		}
	inline	void	WordProcessor::Table::GetDefaultCellMargins (Led_TWIPS* top, Led_TWIPS* left, Led_TWIPS* bottom, Led_TWIPS* right) const
		{
			if (top != NULL)	{	*top = fDefaultCellMargins.GetTop ();	}
			if (left != NULL)	{	*left = fDefaultCellMargins.GetLeft ();	}
			if (bottom != NULL)	{	*bottom = fDefaultCellMargins.GetBottom ();	}
			if (right != NULL)	{	*right = fDefaultCellMargins.GetRight ();	}
		}
	inline	void	WordProcessor::Table::SetDefaultCellMargins (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right)
		{
			if (top != fDefaultCellMargins.GetTop () or
				left != fDefaultCellMargins.GetLeft () or
				bottom != fDefaultCellMargins.GetBottom () or
				right != fDefaultCellMargins.GetRight ()
				) {
				fDefaultCellMargins.top = top;
				fDefaultCellMargins.left = left;
				fDefaultCellMargins.bottom = bottom;
				fDefaultCellMargins.right = right;
				InvalidateLayout ();
			}
		}
	inline	void	WordProcessor::Table::InvalidateLayout ()
		{
			if (fNeedLayout != eNeedFullLayout) {
				AbstractParagraphDatabaseRep*	o	=	dynamic_cast<AbstractParagraphDatabaseRep*> (GetOwner ());
				Led_AssertNotNil (o);
				o->fSomeInvalidTables = true;
				fNeedLayout = eNeedFullLayout;
			}
		}
	inline	size_t	WordProcessor::Table::GetRowCount() const
		{
			size_t	rows	=	0;
			GetDimensions (&rows, NULL);
			return rows;
		}
	inline	size_t	WordProcessor::Table::GetColumnCount () const
		{
			size_t	columns	=	0;
			GetDimensions (NULL, &columns);
			return columns;
		}
	inline	WordProcessor::Table::Cell&	WordProcessor::Table::GetCell (size_t row, size_t column)
		{
			Led_Require (row < GetRowCount ());
			Led_Require (column < GetColumnCount (row));
			Led_Assert (fRows.size () == GetRowCount ());
			Led_Assert (fRows[row].fCells.size () == GetColumnCount (row));
			return fRows[row].fCells[column];
		}
	inline	const WordProcessor::Table::Cell&	WordProcessor::Table::GetCell (size_t row, size_t column) const
		{
			Led_Require (row < GetRowCount ());
			Led_Require (column < GetColumnCount (row));
			Led_Assert (fRows.size () == GetRowCount ());
			Led_Assert (fRows[row].fCells.size () == GetColumnCount (row));
			return fRows[row].fCells[column];
		}
	inline	WordProcessor::Table::CellMergeFlags	WordProcessor::Table::GetCellFlags (size_t row, size_t column) const
		{
			Led_Require (row < GetRowCount ());
			const RowInfo&	rowInfo	=	fRows[row];
			if (column < rowInfo.fCells.size ()) {
				return rowInfo.fCells[column].GetCellMergeFlags ();
			}
			else {
				return eInvalidCell;
			}
		}
	/*
	@METHOD:		WordProcessor::Table::GetCellSelection
	@DESCRIPTION:	<p>Retrieve the cell selection range for the given table. Note that we always have a rectangular
				table selection (could be in whole rows or columns or not). The special case of a single cell selection
				may indicate that the ENTIRE cell is selected, or just a subset (which is decided by ITS GetCellSelection property).
				</p>
					<p>Like marker positions and STL iterators, we use the selEnd to be just PAST the end of the selected cell,
				and so if rowSelStart==rowSelEnd then this implies NO SELECTION, and if rowSelStart + 1 == rowSelEnd and
				colSelStart + 1 = colSelEnd then we have selected a single cell.
				</p>
					<p>See @'WordProcessor::Table::SetCellSelection'.</p>
	*/
	inline	void	WordProcessor::Table::GetCellSelection (size_t* rowSelStart, size_t* rowSelEnd, size_t* colSelStart, size_t* colSelEnd) const
		{
			Led_Ensure (fRowSelStart <= fRowSelEnd);
			Led_Ensure (fRowSelEnd <= GetRowCount ());
			Led_Ensure (fColSelStart <= fColSelEnd);
			Led_Ensure (fColSelEnd <= GetColumnCount ());
			if (rowSelStart != NULL) {
				*rowSelStart = fRowSelStart;
			}
			if (rowSelEnd != NULL) {
				*rowSelEnd = fRowSelEnd;
			}
			if (colSelStart != NULL) {
				*colSelStart = fColSelStart;
			}
			if (colSelEnd != NULL) {
				*colSelEnd = fColSelEnd;
			}
		}
	/*
	@METHOD:		WordProcessor::Table::GetIntraCellMode
	@ACCESS:		public
	@DESCRIPTION:	<p>Return true if the editor is in 'intraCell' editing mode. This edit mode means that the
				current selection is a mini-editor inside of a cell. This is not the same as having selected
				a single cell. It means characters typed go to the focused WP inside the currently selected
				cell.</p>
	*/
	inline	bool	WordProcessor::Table::GetIntraCellMode (size_t* row, size_t* col) const
		{
			if (fIntraCellMode) {
				Led_Assert (fRowSelEnd == fRowSelStart + 1);
				if (row != NULL) {
					*row = fRowSelStart;
				}
				Led_Assert (fColSelEnd == fColSelStart + 1);
				if (col != NULL) {
					*col = fColSelStart;
				}
			}
			return fIntraCellMode;
		}
	inline	void	WordProcessor::Table::GetIntraCellSelection (size_t* selStart, size_t* selEnd) const
		{
			Led_RequireNotNil (selStart);
			Led_RequireNotNil (selEnd);
			*selStart = fIntraSelStart;
			*selEnd = fIntraSelEnd;
		}
	inline	void	WordProcessor::Table::SaveIntraCellContextInfo (
									bool leftSideOfSelectionInteresting,
									const Led_FontSpecification& intraCellSelectionEmptySelFontSpecification
								)
		{
			fSavedIntraCellInfoValid = true;
			fSavedLeftSideOfSelectionInteresting = leftSideOfSelectionInteresting;
			fSavedIntraCellSelectionEmptySelFontSpecification = intraCellSelectionEmptySelFontSpecification;
		}
	inline	bool	WordProcessor::Table::RestoreIntraCellContextInfo (
									bool* leftSideOfSelectionInteresting,
									Led_FontSpecification* intraCellSelectionEmptySelFontSpecification
								)
		{
			Led_RequireNotNil (leftSideOfSelectionInteresting);
			Led_RequireNotNil (intraCellSelectionEmptySelFontSpecification);
			if (fSavedIntraCellInfoValid) {
				*leftSideOfSelectionInteresting = fSavedLeftSideOfSelectionInteresting;
				*intraCellSelectionEmptySelFontSpecification = fSavedIntraCellSelectionEmptySelFontSpecification;
				return true;
			}
			else {
				return false;
			}
		}
	inline	void	WordProcessor::Table::InvalidateIntraCellContextInfo ()
		{
			fSavedIntraCellInfoValid = false;
		}




//class	WordProcessor::Table::SuppressCellUpdatePropagationContext
	inline	WordProcessor::Table::SuppressCellUpdatePropagationContext::SuppressCellUpdatePropagationContext (WordProcessor::Table& t):
		fTable (t),
		fOldVal (t.fSuppressCellUpdatePropagationContext)
			{
				t.fSuppressCellUpdatePropagationContext = true;
			}
	inline	WordProcessor::Table::SuppressCellUpdatePropagationContext::~SuppressCellUpdatePropagationContext ()
		{
			fTable.fSuppressCellUpdatePropagationContext = fOldVal;
		}





//class	WordProcessor::Table::AllowUpdateInfoPropagationContext
	inline	WordProcessor::Table::AllowUpdateInfoPropagationContext::AllowUpdateInfoPropagationContext (WordProcessor::Table& t):
		fTable (t),
		fOldVal (t.fAllowUpdateInfoPropagationContext)
			{
				t.fAllowUpdateInfoPropagationContext = true;
			}
	inline	WordProcessor::Table::AllowUpdateInfoPropagationContext::~AllowUpdateInfoPropagationContext ()
		{
			fTable.fAllowUpdateInfoPropagationContext = fOldVal;
		}




//class	WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet
	inline	WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet::TemporarilyUseTablet (WordProcessor::Table::EmbeddedTableWordProcessor& editor, Led_Tablet t, DoTextMetricsChangedCall tmChanged):
		fEditor (editor),
		fOldTablet (editor.fUpdateTablet),
		fDoTextMetricsChangedCall (tmChanged)
			{
				editor.fUpdateTablet = t;
				if (tmChanged == eDoTextMetricsChangedCall) {
					editor.TabletChangedMetrics ();
				}
			}
	inline	WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet::~TemporarilyUseTablet ()
		{
			fEditor.fUpdateTablet = fOldTablet;
			if (fDoTextMetricsChangedCall == eDoTextMetricsChangedCall) {
				fEditor.TabletChangedMetrics ();
			}
		}





//class	WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext
	inline	WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext::DisableRefreshContext (WordProcessor::Table::EmbeddedTableWordProcessor& wp):
		fWP (wp),
		fOldVal (wp.fSupressRefreshCalls)
			{
				wp.fSupressRefreshCalls = true;
			}
	inline	WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext::~DisableRefreshContext ()
		{
			if (fWP.fSupressRefreshCalls != fOldVal) {
				fWP.fSupressRefreshCalls = fOldVal;
				if (not fWP.fSupressRefreshCalls) {
					fWP.NoteWindowPartiallyUpdated ();
				}
			}
		}







//class	WordProcessor::Table::TemporarilySetOwningWP
	inline	WordProcessor::Table::TemporarilySetOwningWP::TemporarilySetOwningWP (const Table& forTable, WordProcessor& forWordProcessor):
		fOwningTable (const_cast<Table&> (forTable)),
		fSavedTableOwningWP (forTable.fCurrentOwningWP)
			{
				fOwningTable.fCurrentOwningWP = &forWordProcessor;
			}
	inline	WordProcessor::Table::TemporarilySetOwningWP::~TemporarilySetOwningWP ()
		{
			fOwningTable.fCurrentOwningWP = fSavedTableOwningWP;
		}







//class	WordProcessor::Table::TemporarilyAllocateCellWP
	inline	WordProcessor::Table::TemporarilyAllocateCellWP::TemporarilyAllocateCellWP (Table& forTable, WordProcessor& forWordProcessor, size_t forRow, size_t forColumn, const Led_Rect& cellWindowRect, bool captureChangesForUndo):
		fOwningTable (forTable),
		fCellEditor (forTable.ConstructEmbeddedTableWordProcessor (forWordProcessor, forRow, forColumn, cellWindowRect, captureChangesForUndo))
			{
			}
	inline	WordProcessor::Table::TemporarilyAllocateCellWP::~TemporarilyAllocateCellWP ()
		{
			Led_AssertNotNil (fCellEditor);
			fOwningTable.ReleaseEmbeddedTableWordProcessor (fCellEditor);
		}
	inline	WordProcessor::Table::TemporarilyAllocateCellWP::operator WordProcessor::Table::EmbeddedTableWordProcessor* ()
		{
			Led_AssertNotNil (fCellEditor);
			return fCellEditor;
		}
	inline	WordProcessor::Table::EmbeddedTableWordProcessor& WordProcessor::Table::TemporarilyAllocateCellWP::operator& ()
		{
			Led_AssertNotNil (fCellEditor);
			return *fCellEditor;
		}
	inline	WordProcessor::Table::EmbeddedTableWordProcessor* WordProcessor::Table::TemporarilyAllocateCellWP::operator-> ()
		{
			Led_AssertNotNil (fCellEditor);
			return fCellEditor;
		}




//	class	WordProcessor::Table::TemporarilyAllocateCellWithTablet
	inline	WordProcessor::Table::TemporarilyAllocateCellWithTablet::TemporarilyAllocateCellWithTablet (WordProcessor::Table& forTable, size_t row, size_t column, bool captureChangesForUndo):
		fWP (forTable, *forTable.fCurrentOwningWP, row, column, forTable.TableCoordinates2Window (forTable.GetCellEditorBounds (row, column)), captureChangesForUndo),
		fTablet (forTable.fCurrentOwningWP),
		fTmpUseTablet (*fWP, fTablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall)
		{
		}
	inline	WordProcessor::Table::TemporarilyAllocateCellWithTablet::operator WordProcessor::Table::EmbeddedTableWordProcessor* ()
		{
			return fWP;
		}
	inline	WordProcessor::Table::EmbeddedTableWordProcessor& WordProcessor::Table::TemporarilyAllocateCellWithTablet::operator& ()
		{
			return *fWP;
		}
	inline	WordProcessor::Table::EmbeddedTableWordProcessor* WordProcessor::Table::TemporarilyAllocateCellWithTablet::operator-> ()
		{
			return fWP;
		}




//	class	WordProcessor::DialogSupport::TableSelectionPropertiesInfo
	inline	WordProcessor::DialogSupport::TableSelectionPropertiesInfo::TableSelectionPropertiesInfo ():
			fTableBorderWidth (Led_TWIPS (0)),
			fTableBorderColor (Led_Color::kWhite),
			fDefaultCellMargins (),
			fCellSpacing (Led_TWIPS (0)),
			fCellWidth_Common (false),
			fCellWidth (Led_TWIPS (0)),
			fCellBackgroundColor_Common (false),
			fCellBackgroundColor (Led_Color::kWhite)
		{
		}







#if		qLedUsesNamespaces
}
#endif


#if		qSilenceAnnoyingCompilerWarnings && __MWERKS__
	#pragma	warn_hidevirtual	reset
#endif
#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif



#endif	/*__WordProcessor_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
