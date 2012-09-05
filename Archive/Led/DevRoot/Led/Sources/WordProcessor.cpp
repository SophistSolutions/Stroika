/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/WordProcessor.cpp,v 2.295 2004/02/10 19:46:21 lewis Exp $
 *
 * Changes:
 *	$Log: WordProcessor.cpp,v $
 *	Revision 2.295  2004/02/10 19:46:21  lewis
 *	SPR#1635: made Table::EmbeddedTableWordProcessor::TableCMD into Table::Embedde::TableCMD (change nesting) - to work around compiler issue (bug?) where I cannot declare a:b:c a friend of a inside a when b is defined outsid of a (but declared inside a). No matter. Also - added qNameLookupXXXInBaseClassWhenItIsNestedSometimesFailsBug. All above to get compiling with MSVC6
 *	
 *	Revision 2.294  2004/02/10 17:09:11  lewis
 *	cleanup compiler (argsused) warnings
 *	
 *	Revision 2.293  2004/01/09 15:28:06  lewis
 *	SPR#1615: Fixed select-all behavior when there is only a single table (and nothing else in the document. Also - revised behavior so selectall first selects all in cell, and only if already selected - selects all in document
 *	
 *	Revision 2.292  2004/01/09 02:28:27  lewis
 *	SPR#1614: CalculateFarthestRightMarginInWindow/CalculateFarthestRightMarginInDocument now use GetLayoutMargins () instead of GetMargins - so they react to its override (like for wrap-to-window)
 *	
 *	Revision 2.291  2003/11/06 16:07:08  lewis
 *	fix a few cmd names (adding set margins AND first-indent cmd). Completes fix for SPR#1559: Store fCachedFarthestRightMarginInDocument in TWIPS and change GetFarthestRightMargin* functions to return TWIPS (cuz now cached in PDB - which has no tablet info). Moved fCachedFarthestRightMarginInDocument to PDB - and updated (inval) when margins change.
 *	
 *	Revision 2.290  2003/11/06 03:27:30  lewis
 *	SPR#1560: moved WordProcessorHScrollbarHelper<>::ComputeMaxHScrollPos () to WordProcesor. Then
 *	SPR#1559: add CalculateFarthestRightMargin () etc functions, and changed how it caches info,
 *	and how ComputeMaxHScrollPos works. Still not COMPLETE - since fCachedFarthestRightMarginInDocument
 *	isn't reset in all appropriate places (see SPR#1559 for more details)
 *	
 *	Revision 2.289  2003/11/03 21:14:10  lewis
 *	SPR#1558- WordProcessorTextIOSinkStream::PushContext () changed to flush if cachedTextSize != 0 -
 *	instead of asserting
 *	
 *	Revision 2.288  2003/05/23 16:06:27  lewis
 *	SPR#1411: Fixed those table open/row open etc debugging flags.
 *	
 *	Revision 2.287  2003/05/23 15:49:54  lewis
 *	SPR#1411: Added various debugging flags to WordProcessorTextIOSinkStream to assure the right calling
 *	semantics for TableOpen/Close/CellOpen/Close/RowOpen/Close.
 *	
 *	Revision 2.286  2003/05/23 01:18:02  lewis
 *	SPR#1141: as part of allowing HTML Table IO support, WordProcessorTextIOSinkStream::EndTableRow
 *	allows no SetCellWidths calls
 *	
 *	Revision 2.285  2003/05/22 01:08:04  lewis
 *	SPR#1499: Fixed Table::GetClosestCell () to take into border
 *	
 *	Revision 2.284  2003/05/21 20:47:04  lewis
 *	SPR#1494: Added TextInteractor::SupressCommandBreaksContext stack-based class to allow some commands to
 *	be grouped together (when acting on a bunch of table cells)
 *	
 *	Revision 2.283  2003/05/21 15:50:11  lewis
 *	SPR#1398: Added AllowUpdateInfoPropagationContext class and supported by fForTable.fCellUpdatePropationUpdater
 *	and hitting it in the CellRep::AboutTo/DIdUpdate calls. Used the AllowUpdateInfoPropagationContext to make this
 *	conditioanl for performance reasons (test T3 from SPR#1496 went from 4 secs to 10 just always propagating, but
 *	stayed at 4 with this conditional code)
 *	
 *	Revision 2.282  2003/05/21 12:53:15  lewis
 *	SPR#1495: rename UndoableContextHelper::GetSelStart/End to UndoableContextHelper::GetUndoRegionStart/End
 *	
 *	Revision 2.281  2003/05/20 23:24:49  lewis
 *	small cleanups and use LED_DECLARE_USE_BLOCK_ALLOCATION() for some of the
 *	new table command objects
 *	
 *	Revision 2.280  2003/05/20 23:10:19  lewis
 *	SPR#1493: react to changes in Undo support (InteractiveUndoHelperMakeTextRep etc).
 *	Mostly SPR#1422: support undo for tables (which is what inspired most of the work
 *	for SPR#1493). Added class SavedTextRepWSel to restore table selections (along with
 *	its new ApplySelection () method). Override EmbeddedTableWordProcessor::PostInteractiveUndoPostHelper ()
 *	etc. TableCMD class to be able to find right table object to set selections on.
 *	TemporarilyAllocateCellWithTablet and a couple related classes/methods take captureChangesForUndo
 *	flag.WordProcessor::InteractiveUndoHelperMakeTextRep () uses SavedTextRepWSel to preseve
 *	table selection. Lots of misc changes to various SavedTextRep classes etc to support this
 *	big change.
 *	
 *	Revision 2.279  2003/05/15 12:54:17  lewis
 *	SPR#1487: make OnSelectedEmbeddingExtendedCommand return bool (so I can tell if performed)
 *	and revise Table::OnUpdateCommand/OnPerformUpdate code so if embedded table doesn't enable
 *	the kSelectedEmbeddingProperties_CmdID (then we do - and if it doesn't perform the command -
 *	then we do. This way - embedded URLs and pictures (and eventually other tables) take precendence
 *	on an innermost first basis, but if there is none - then the first table inside-out gets a
 *	crack. This makes it more intuiitive for users to click in a table cell and then still see
 *	the table properties command enabled.
 *	
 *	Revision 2.278  2003/05/14 22:11:42  lewis
 *	SPR#1485: erase area outside cell editor (cell margins) with cell background color
 *	
 *	Revision 2.277  2003/05/14 20:46:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.276  2003/05/13 22:50:44  lewis
 *	only add Alt+Enter to command names if qWindows
 *	
 *	Revision 2.275  2003/05/13 20:44:52  lewis
 *	SPR#1399: progress checking on table getproperties dialog support
 *	
 *	Revision 2.274  2003/05/12 17:17:46  lewis
 *	SPR#1401: Added GetDialogSupport ().AddNewTableDialog () and Led_StdDialogHelper_AddNewTableDialog.
 *	
 *	Revision 2.273  2003/05/08 14:04:16  lewis
 *	no longer pass along kSelectAll_CmdID to IntraCellMode cell cuz now there is an
 *	intracellselectall cmd
 *	
 *	Revision 2.272  2003/05/08 00:07:22  lewis
 *	SPR#1467: added kSelectTableIntraCellAll_CmdID
 *	
 *	Revision 2.271  2003/05/07 21:11:05  lewis
 *	SPR#1467: implemented various select commands (word/row/paragraph/table/cell/row/col)
 *	
 *	Revision 2.270  2003/05/07 16:54:11  lewis
 *	SPR#1466: Fix WordProcessor::CalcSpaceToEat () to get the margins from the right row,
 *	not always the first row
 *	
 *	Revision 2.269  2003/05/07 01:29:01  lewis
 *	SPR#1445: Implement drawing of paragraph end icon in WordProcessor::DrawRowSegments ().
 *	Could have considered getting old mechanism to work again, but it didn't seem worth
 *	the trouble
 *	
 *	Revision 2.268  2003/05/06 21:47:01  lewis
 *	SPR#1465: TextInteractor::Pre/PostInteraciveUndoHelper REQUIRE fCommandHandler != NULL.
 *	Then fix UndoHelper class, and InteractiveWPHelper1<> to check first if CommandHandler != NULL.
 *	Thus we don't leak command objects which might contain a reference to a cell paraDB that
 *	has already gone away!!!
 *	
 *	Revision 2.267  2003/05/06 21:28:33  lewis
 *	SPR#1432: finished fixing table cell click code so that you can click in the margin area
 *	around a cell to select the entire cell
 *	
 *	Revision 2.266  2003/05/06 19:42:48  lewis
 *	SPR#1432 (incomplete): work on fixing process simple click code so if you click in margin -
 *	dont go into intracell mode
 *	
 *	Revision 2.265  2003/05/06 13:14:20  lewis
 *	SPR#1465: added small hack to Led_RefCntPtr<> to help debug SPR#1465
 *	
 *	Revision 2.264  2003/05/06 00:58:10  lewis
 *	SPR#1464: lose EmbeddedTableWordProcessor::SetSelection () override and grab selection in
 *	EmbeddedTableWordProcessor::SaveMiscActiveFocusInfo () instead
 *	
 *	Revision 2.263  2003/05/06 00:26:59  lewis
 *	SPR#1452: fix bug with table reading with diff #s of cols per row (added optional arg
 *	to InsertRow)
 *	
 *	Revision 2.262  2003/05/05 20:50:33  lewis
 *	SPR#1457: call SetEmptySelectionStart(void) when RestoreIntraCellContextInfo () so we grab
 *	empty style info from surrounding text
 *	
 *	Revision 2.261  2003/05/05 19:00:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.260  2003/05/02 15:54:30  lewis
 *	SPR#1459: fixed remaining update problem by adding SuppressCellUpdatePropagationContext
 *	class and fixing SupressRefreshCalls code to call fWP.NoteWindowPartiallyUpdated ()
 *	when done with a supress context
 *	
 *	Revision 2.259  2003/05/02 11:46:42  lewis
 *	SPR#1458: fix Table::InteractiveSetFont () to handle IntraCellMode case. Also
 *	change Table::Query...KeyAvail to just return true (hack to avoid throws)
 *	
 *	Revision 2.258  2003/05/01 23:24:47  lewis
 *	SPR#1456: Added DisableRefreshContext to EmbeddedTableWordProcessor code so
 *	we can supress refresh from RestoreMiscActiveFocusInfo code
 *	
 *	Revision 2.257  2003/05/01 22:16:28  lewis
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
 *	Revision 2.256  2003/05/01 21:03:09  lewis
 *	SPR#1455: maybe workaround infinite recurse bug with table selection and notification
 *	
 *	Revision 2.255  2003/05/01 20:06:44  lewis
 *	SPR#1396: added RTF writing support for table spacing and margins
 *	
 *	Revision 2.254  2003/05/01 01:18:23  lewis
 *	SPR#1396: Added RTF reading support for table cell spacing (trspd{t,l,b,r}). Not perfect (see SPR#1454).
 *	
 *	Revision 2.253  2003/04/30 22:01:34  lewis
 *	SPR#1396: Added RTF READING support for trpadd{b,l.,t.r} and \trgaph tags which
 *	specify table margins. Fully supported in WP class SinkStream as well. Now reading
 *	these margins in from existing files looks fine. Had to add PRELIMINARY \trleft support
 *	to reader to make this work fully. Added new SPR#1453 to take care of \trleft
 *	spinnoff issue.
 *	
 *	Revision 2.252  2003/04/21 16:16:48  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.251  2003/04/18 00:11:45  lewis
 *	SPR#1443: add fSavedIntraCellInfoValid flag, and InvalidateIntraCellContextInfo/
 *	Save/RestoreIntraCellContextInfo methods. Call InvalidateIntraCellContextInfo in a
 *	few places to avoid accessing invalid info (esp selection - this was basically
 *	SPR#1442, and EmptyStyleSelection - which was SPR#1443)
 *	
 *	Revision 2.250  2003/04/17 19:12:22  lewis
 *	SPR#1396: Added Table::GetCellEditorBounds (), and fully supported (except for IO)
 *	cell spacing and margins. Decently tested.
 *	
 *	Revision 2.249  2003/04/17 16:26:14  lewis
 *	SPR#1396: cleaned up semantics of cell borders etc for tables. Added preliminary
 *	cell spacing/margins support
 *	
 *	Revision 2.248  2003/04/17 13:43:48  lewis
 *	SPR#1421: Added fFontSizeChange_Other* strings to WP::CommandNames and used
 *	them in WP::OnUpdateFontSizeChangeCommand ()
 *	
 *	Revision 2.247  2003/04/16 15:16:04  lewis
 *	SPR#1430: added 'cut selected cells' handling support
 *	
 *	Revision 2.246  2003/04/16 14:43:01  lewis
 *	SPR#1437: made OnInsertTableColAfterCommand methods of Table - rather than WordProcessor
 *	
 *	Revision 2.245  2003/04/16 14:04:17  lewis
 *	SPR#1437: cosmetic cleanups - nest code and scope in in braces inside of UndoContext
 *	instances, and use new TextInteractor::BreakInGroupedCommands () wrapper (and other
 *	small cleanups)
 *	
 *	Revision 2.244  2003/04/16 13:13:16  lewis
 *	SPR#1435: added ReValidateSelection support
 *	
 *	Revision 2.243  2003/04/15 23:52:34  lewis
 *	SPR#1438: on select all cells, and do delete rows or delete cols command - must REALLY
 *	do a clear of whole table
 *	
 *	Revision 2.242  2003/04/15 22:20:29  lewis
 *	SPR#1425: added RemoveTableRows/Columns command. Also SPR#1431: Table::OnTypedNormalCharacter ()
 *	now clears all selected cells before setting the selection to the top left cell and doing the
 *	type behavior there.
 *	
 *	Revision 2.241  2003/04/14 14:36:28  lewis
 *	added back WordProcessorTextIOSinkStream (WordProcessor* wp,...) CTOR since used in AL, and others
 *	
 *	Revision 2.240  2003/04/13 23:10:25  lewis
 *	on a backspace key for a selected table - clear selected table cells
 *	
 *	Revision 2.238  2003/04/12 21:08:24  lewis
 *	SPR#1428: save/restore GetEmptySelectionStyle for embedded Cell WPs
 *	
 *	Revision 2.237  2003/04/12 20:49:18  lewis
 *	SPR#1347: support SetOverwriteTableMode for Internalizer and use in InternalizeBestFlavor(CUR_SELECTION).
 *	Now when you paste - you overwrite table cells instead of blowing the entire table away. Also, fixed bug
 *	in various places that iterated over table cells to handle case of diff # of cells per row (so some rows
 *	dont appear in selection range). And delete unneeded WordProcessorTextIOSinkStream::CTOR overload
 *	
 *	Revision 2.236  2003/04/11 19:56:06  lewis
 *	SPR#1329: Fixed TableIOMapper and surrounding code so WordProcessor:ExternalizeFlavors()
 *	only writes the cells selected in the table (fixes COPY command with table cells selected)
 *	
 *	Revision 2.235  2003/04/10 19:12:39  lewis
 *	SPR#1329- handle passing alot many commands to a REGION of selected table cells. Also -
 *	pass along InteractiveSetFont/AssureCurSelFontCacheValid so that the toolbar displays
 *	the right cached font info
 *	
 *	Revision 2.234  2003/04/10 14:03:33  lewis
 *	SPR#1329: override EmbeddedTableWordProcessor::OnCopy/Paste_Before/After code to
 *	pass back to owning WP
 *	
 *	Revision 2.233  2003/04/10 13:09:59  lewis
 *	SPR#1329: Added and used WordProcessor::PassAlongCommandToIntraCellModeTableCell to mark/pass
 *	along certain cmds to the table cells WPs.
 *	
 *	Revision 2.232  2003/04/09 21:11:01  lewis
 *	SPR#1417: fix placement of setting fIntraCellMode before Refresh()
 *	
 *	Revision 2.231  2003/04/09 19:15:06  lewis
 *	SPR#1413: Being more careful - and always constructing the EMBWP (except in the layout case)
 *	in WINDOW coords and using the new TableCoordinates2Window/WindowCoordinates2Table routines
 *	as appopriate fixed this problem, and worked fine even with H-scrolling.
 *	
 *	Revision 2.230  2003/04/09 16:49:35  lewis
 *	SPR#1413: Added Table::{TableCoordinates2Window/WindowCoordinates2Table}. Also DOCCOMMENTS
 *	
 *	Revision 2.229  2003/04/09 16:27:43  lewis
 *	SPR#1419: lose Refresh_ and Update_ overloaded virtual overrides from EmbeddedTableWordProcessor
 *	
 *	Revision 2.228  2003/04/09 14:26:34  lewis
 *	SPR#1404: finish adding (basic) editing cell support: added table code and hooked from WP
 *	code CalculateCaretRect and GetCaretShownSituation
 *	
 *	Revision 2.227  2003/04/09 00:05:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.226  2003/04/08 22:39:56  lewis
 *	SPR#1404: update clickcount for embedded WP, and don't reset cell selection
 *	(SetCellSelection) in WordProcessor::SetSelection () if there is no change
 *	in selection
 *	
 *	Revision 2.225  2003/04/08 21:45:14  lewis
 *	SPR#1404: more progress on intra table cell editing - keeping and displaying the
 *	selection within a cell, and even handling most mouse clicking within a cell.
 *	
 *	Revision 2.224  2003/04/07 22:38:02  lewis
 *	SPR#1403/SPR#1404: working on intra cell selection and OnTypedNormalCharacter
 *	
 *	Revision 2.223  2003/04/07 16:29:42  lewis
 *	SPR#1329: now new Table::OnUpdateCommand/Table::OnPerformCommand hooked into the
 *	command chain (but still do nothing)
 *	
 *	Revision 2.222  2003/04/03 21:52:48  lewis
 *	SPR#1407: more cleanups of new CommandNumberMapping<> stuff and other new command#
 *	processing support
 *	
 *	Revision 2.221  2003/04/03 16:41:29  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated
 *	command classes, just builtin to TextInteractor/WordProcessor (and instead of template params
 *	use new TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.220  2003/04/01 18:26:36  lewis
 *	SPR#1322: Table::DrawSegment () now does logical clipping.
 *	
 *	Revision 2.219  2003/04/01 15:17:55  lewis
 *	SPR#1394: fixed WordProcessor::DrawRowSegments and WordProcessor::GetLayoutMargins to not
 *	display bullet even if a table has the bullet style
 *	
 *	Revision 2.218  2003/04/01 14:53:16  lewis
 *	SPR#1393: fix bug in code for Table::GetRowHilightRects () so it grabs before/after table
 *	hilight rects
 *	
 *	Revision 2.217  2003/04/01 01:39:44  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.216  2003/03/31 22:26:48  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.215  2003/03/31 20:22:07  lewis
 *	SPR#1389: Lose fColumns - and even fCellX - and keep list of Cells inside the RowInfo record.
 *	GetColumnCount and new SetColumnCount() take ROW# argument - so we can have different# of cols
 *	for diff rows. That made stuff like SplitMergeTableCell () obsolete (and so deleted). Seems to
 *	be working pretty well - at least as well as before
 *	
 *	Revision 2.214  2003/03/28 17:15:05  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.213  2003/03/27 16:27:51  lewis
 *	SPR#1384: ParagraphDatabaseRep::CheckMarkerBounaryConstraints() creates UpdateInfo (realContentUpdate=false)
 *	cuz no real content changes (by this - generally by something else). Also, Table::PerformLayout ()
 *	creates UpdateInfo (realContentUpdate=false) cuz Update triggered is purely for display cache info
 *	update - not a real content update.
 *	
 *	Revision 2.212  2003/03/21 13:59:42  lewis
 *	SPR#1364- fix setting of drag anchor. Moved it from Led_XXX(Win32/MacOS/Gtk) to
 *	TextInteractor::ProcessSimpleClick (adding extra arg). Also lots of other small
 *	code cleanups
 *	
 *	Revision 2.211  2003/03/20 21:49:47  lewis
 *	SPR#1365 - Fix WordProcessor::WPIdler::SpendIdleTime so that when no tables invalidated - we disable
 *	the idle task (sort of)
 *	
 *	Revision 2.210  2003/03/20 20:58:33  lewis
 *	SPR#1363- Implement WordProcessor::DrawBefore ()
 *	
 *	Revision 2.209  2003/03/20 16:00:01  lewis
 *	SPR#1360- lose OnEnterIdleCallback hook for doing PerformLayout and replace it with (a still preliminary)
 *	WPIdler::SpendIdleTime approach
 *	
 *	Revision 2.208  2003/03/19 18:31:07  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.207  2003/03/19 14:44:50  lewis
 *	SPR#1355 - Lose MarkersOfATypeMarkerSink<> since its identical to MarkersOfATypeMarkerSink2Vector<>.
 *	Use the later instead
 *	
 *	Revision 2.206  2003/03/19 14:01:49  lewis
 *	SPR#1349/SPR#1351 - cleanup Table::PerformLayout () and calls to it (just from the onidle code
 *	for now). Also do all the variable setting inside the scope of the SimpleUpdater. Strictly use the
 *	fTotalHeight instead of recomputing it here and there on the fly. Simplified LayoutFlag processing.
 *	SPR#1352: speed tweek EmbeddedWP::GetLayoutMargins().
 *	
 *	Revision 2.205  2003/03/17 20:52:55  lewis
 *	SPR#1348 - don't call Table::LayoutIfNeeded () in MeasureSegmentWidth etc but only from
 *	WordProcessor::OnEnterIdleCallback ().
 *	
 *	Revision 2.204  2003/03/16 16:48:00  lewis
 *	SPR#1345 - use SimpleUpdater instead of direct DoDidUpdateCalls() call. A bit tricky for
 *	case of MarkerCover<> usage!
 *	
 *	Revision 2.203  2003/03/15 23:19:04  lewis
 *	SPR#1344 - use TextStore::SimpleUpdater in Table::InsertColumn/Table::DeleteColumn/Table::InsertRow/
 *	Table::DeleteRow so when you do AddRowBefore command the row actually appears
 *	
 *	Revision 2.202  2003/03/15 17:06:15  lewis
 *	SPR#1336 - Fixed WordProcessor::MeasureSegmentHeight () and Fixed WordProcessor::MeasureSegmentHeight ()
 *	and on a table, and if so, then ignore the space before/after and line spacing. Also -
 *	got rid of some ifdefed out old code in the same routines
 *	
 *	Revision 2.201  2003/03/14 14:38:56  lewis
 *	SPR#1343 - Get rid of funny code for setting anchor, and override
 *	StandardStyledTextInteractor::WhileSimpleMouseTracking to properly handle tracking over embeddings
 *	and fix the WordProcessor::WhileSimpleMouseTracking to do likewise (it was already correct except
 *	for cosmetics). Net effect is that clicking on embedding works correctly again on Win32 (was OK on
 *	other platforms) - and is now clearer on ALL
 *	
 *	Revision 2.200  2003/03/11 19:34:40  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.199  2003/03/07 19:56:13  lewis
 *	SPR#1330 - Be more careful in one case about grabbing REAL cell instead of boundsrect of mergecell
 *	
 *	Revision 2.198  2003/03/07 19:07:21  lewis
 *	SPR#1324- minor table cleanups - and finished table selection display and basic function.
 *	Still todo is making commands respect this table selection
 *	
 *	Revision 2.197  2003/03/07 00:40:44  lewis
 *	SPR#1324 - got basic cell selection from mouse clicks working. Still not very good
 *	(like drag select not working).
 *	
 *	Revision 2.196  2003/03/06 21:09:58  lewis
 *	SPR#1324- updated code for drawing row hilights (Table::GetRowHilightRects) to handle selection.
 *	Now must actually SET the selected cell
 *	
 *	Revision 2.195  2003/03/06 20:49:03  lewis
 *	SPR#1324- beginnings of Get/SetSelection support for Table
 *	
 *	Revision 2.194  2003/03/06 14:02:28  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.193  2003/03/05 22:47:33  lewis
 *	SPR#1320 - fix Table::DrawSegment ()
 *	
 *	Revision 2.192  2003/03/05 17:45:34  lewis
 *	SPR#1319 - fix a number of things (SpecifyTextStore related mostly) to speed tweek
 *	tables and other display
 *	
 *	Revision 2.191  2003/03/04 21:34:28  lewis
 *	SPR#1315 - lose qCacheEMWPInCellRep code - since other changes to PM caching
 *	make it unneeded/unhelpful
 *	
 *	Revision 2.190  2003/02/28 21:03:24  lewis
 *	SPR#1314- get rid of memory leak (Table::Row change from earlier only halfway
 *	complete) - now Table::RowInfo
 *	
 *	Revision 2.189  2003/02/28 19:08:20  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.188  2003/02/28 17:38:12  lewis
 *	SPR#1315- checkin temporarily disabled qCacheEMWPInCellRep cache code.
 *	Also SetImageUsingOffscreenBitmaps (false) for EmbeddedWordProcessor
 *	
 *	Revision 2.187  2003/02/27 21:32:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.186  2003/02/27 15:49:35  lewis
 *	had to rename CharacterProperties::isspace etc to CharacterProperties::IsSpace()
 *	to avoid name conflicts with GCC MACRO implemeantion of ISXXXX functions
 *	
 *	Revision 2.185  2003/02/27 15:04:38  lewis
 *	finished SPR#1311 (except defered parts). Now horizontal merge cells work pretty well
 *	
 *	Revision 2.184  2003/02/27 14:18:19  lewis
 *	SPR#1311- Lots more cleanups to table handing to support horizontally merged cells.
 *	Reading and writing of them now mostly works
 *	
 *	Revision 2.183  2003/02/26 20:46:56  lewis
 *	SPR#1311- progress supporting reading RTF from tables for mergecells. Works for some
 *	easy cases
 *	
 *	Revision 2.182  2003/02/26 15:46:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.181  2003/02/26 15:02:00  lewis
 *	SPR#1311 - work on Merge/UnMergeCells functions
 *	
 *	Revision 2.180  2003/02/25 21:22:41  lewis
 *	SPR#1311 - lots of table code cleanups - back to single embedding style approach.
 *	Beginning support for MERGECELLS
 *	
 *	Revision 2.179  2003/02/24 18:31:39  lewis
 *	SPR#1306- Added CharacterProperties class and verison of isspace etc
 *	
 *	Revision 2.178  2003/02/19 23:38:01  lewis
 *	SPR#1305- start going abck to stategy where ONE CHAR (sentinal) per table and
 *	no extras per row. Roughly stable working with that strategy now - but then lost
 *	the scrolling boundary benefit of the old strategy
 *	
 *	Revision 2.177  2003/02/12 16:40:02  lewis
 *	SPR#1299- a few small fixes to InteractiveModeUpdater/READONLY handling.
 *	
 *	Revision 2.176  2003/01/29 17:55:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.175  2003/01/15 18:36:46  lewis
 *	SPR#1211- added WordProcessor::G(S)etSmartQuoteMode and WordProcessor::OnTypedNormalCharacter
 *	to implement it
 *	
 *	Revision 2.174  2003/01/12 21:30:51  lewis
 *	another qGCC_OptBugWithLocalClassesScopedInFunction BWA
 *	
 *	Revision 2.173  2003/01/12 20:52:56  lewis
 *	qGCC_OptBugWithLocalClassesScopedInFunction workaround for GCC
 *	
 *	Revision 2.172  2003/01/11 19:28:48  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.171  2003/01/07 17:15:58  lewis
 *	use name Table::GetRowCount/GetColumnCount instead of GetRows/GetColumns. Add Table::CollectRowCount
 *	method as part of speed tweeks. Speed tweek Table::CollectRows. Sped up opening RTF1.4 spec.rtf by
 *	about 30%, but still EXTREEMLY slow
 *	
 *	Revision 2.170  2002/12/17 17:36:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.169  2002/12/17 17:34:49  lewis
 *	SPR#1210- StyledTextImager::DrawSegment/WordProcessor::DrawRowSegments be a bit more careful
 *	about drawInto rect (not 100% fixed but close enuf for now)
 *	
 *	Revision 2.168  2002/12/06 15:17:25  lewis
 *	lose qTmpHackDontUseWPPartitionTilItsFixed hack
 *	
 *	Revision 2.167  2002/12/04 15:37:48  lewis
 *	SPR#1193- replace calls to CalcSegmentSizeWithImmediateText with just CalcSegmentSize
 *	
 *	Revision 2.166  2002/12/02 15:58:15  lewis
 *	SPR#1185 - move DrawRow,DrawRowSegments,DrawRowHilight,DrawInterlineSpace from MRTI/SimpleTextImager
 *	to TextImager (share code. Changed their args slightly. For BIDI work.
 *	
 *	Revision 2.165  2002/11/28 02:31:22  lewis
 *	A couple simple name changes (GetVirtualText->PeekAtVirtualText and TextLayoutBlock_Subset
 *	to TextLayoutBlock_VirtualSubset
 *	
 *	Revision 2.164  2002/11/27 15:58:59  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with
 *	a 'TextLayoutBlock& text' argument
 *	
 *	Revision 2.163  2002/11/27 15:14:06  lewis
 *	As part of SPR#1183 - BIDI project- used TextLayoutBlock class instead of Led_tChar* as arg to
 *	DrawRow and DrawRowSegments() routines. Much more of this sort to come! NB: NOT BACKWARD COMPAT!
 *	
 *	Revision 2.162  2002/11/21 14:09:25  lewis
 *	SPR#1180 - got rid of RowInfo and fRows array. Compute dynamically with CollectAllMarkersInRange.
 *	Gets rid of crasher bug on destroy (cuz of random order of destrunction of table vs. tablerows).
 *	No obvious negative performance impact (though need to profile!!!)
 *	
 *	Revision 2.161  2002/11/19 20:28:17  lewis
 *	SPR#1175 - fix internalizaiton code for tables so if we try to internalize tables next to an existing table -
 *	we append/join them. Not 100% sure this is always best - but it is pretty often (e.g. for undo)
 *	
 *	Revision 2.160  2002/11/19 19:51:52  lewis
 *	SPR#1176 - Added insert row/col before/after commands and a few minor table fixups which were releated
 *	(not 100% - see spr#1177/78 for details)
 *	
 *	Revision 2.159  2002/11/19 17:56:00  lewis
 *	SPR#1175 kTryToIncorporateInternalizeIntoAdjacentTable support (PRELIM) and fix crash on failed add of
 *	new table (as in cuz in middle of an existing table in old code)
 *	
 *	Revision 2.158  2002/11/19 14:17:19  lewis
 *	misc cleanups
 *	
 *	Revision 2.157  2002/11/19 13:27:16  lewis
 *	SPR#1174 - major change to table code. Add a new Table::Row class and make IT the embedding, and make
 *	Table class simply wrap a bunch of Table::Rows. Not 100% solid, and needs some cleanups, but seems to
 *	be pretty functional now
 *	
 *	Revision 2.156  2002/10/30 13:29:38  lewis
 *	SPR#1155 - Added new Rect_Base<> template and made Led_Rect a simple typedef of that. Then TRIED (failed)
 *	to make the top/left/bottom/right fields private by adding accessors. Added the accessors and used them in
 *	tons of places - but still TONS more required. Leave that battle for another day. I'm not even SURE its a
 *	good idea. Anyhow - also added a new typedef for Led_TWIPS_Rect (this was what I was after to begin with
 *	with all this nonsese)
 *	
 *	Revision 2.155  2002/10/24 00:17:12  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.154  2002/10/23 23:48:38  lewis
 *	get compiling with gcc (qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksB)
 *	
 *	Revision 2.153  2002/10/23 21:30:25  lewis
 *	use qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug for GCC bug workaround
 *	
 *	Revision 2.152  2002/10/23 01:01:04  lewis
 *	SPR#1140 - add support for writing cell background color (clcbpat), and adding that color to the
 *	summarizefontandcolortable() routine
 *	
 *	Revision 2.151  2002/10/22 00:39:07  lewis
 *	Add URL context/menu command - SPR#1136
 *	
 *	Revision 2.150  2002/10/21 16:21:48  lewis
 *	SPR#1133 - more cleanups of new WPPartition code and PMCacheMgr etc. Still more todo
 *	
 *	Revision 2.149  2002/10/21 14:41:50  lewis
 *	SPR#1134 - partial cleanup of new PartitionElementCacheInfo and PMInfoCacheMgr code
 *	
 *	Revision 2.148  2002/10/21 12:56:03  lewis
 *	SPR#1135 - Add SummarizeFontAndColorTable method to TextIO::SrcStream and use that to
 *	generate RTF color and font tables
 *	
 *	Revision 2.147  2002/10/20 19:38:24  lewis
 *	SPR#1128 and SPR#1129 - BIG change. Get rid of MultiRowPartitionMarker and MultiRowPartition. Still
 *	coded very slopily (with idefs) and needs major cleanups/tweeks. But - its now functional and testable
 *	
 *	Revision 2.146  2002/10/17 17:42:05  lewis
 *	fix CalcDesiredHeight to set fDesiredHeight instance var
 *	
 *	Revision 2.145  2002/10/14 23:37:53  lewis
 *	SPR#1126 - LOTS of work (very ugly right now and MAYBE still buggy) to support new partition code so that
 *	tables apepar in other own partition element. Added PartHELPER<> and WordProcessor::WPPartition and XXWPPartition.
 *	ALSO - fixed SPR#1127- UndoableContextHelper for insert embedding
 *	
 *	Revision 2.144  2002/10/11 01:42:59  lewis
 *	SPR#1125 fix small bug with WPPartition code (assert in invariant). Call Invariant (in Partition code) more often.
 *	
 *	Revision 2.143  2002/10/11 01:05:05  lewis
 *	getter/setter for table border width and (SB) paying attention to borderwidth setting from RTF
 *	
 *	Revision 2.142  2002/10/11 00:38:18  lewis
 *	SPR#1119 - Finished checking for table objects and fixed Invariant_()
 *	
 *	Revision 2.141  2002/10/09 14:01:24  lewis
 *	SPR#1121- add cellRep to owning TextStore as markerOwner, and then override its DidUpdateText()
 *	call to do InvalidateLayout on owning fForTable (and call SimpleUpdater to get partitionmarkers cache invalidated)
 *	
 *	Revision 2.140  2002/10/08 16:38:12  lewis
 *	SPR#1119- Added CheckForSplits/NeedToCoalesce to LineBasedPartition. Add WordProcessor::WPPartition.
 *	Add / use MakeDefaultPartition () so we create the new partition for WPs.
 *	
 *	Revision 2.139  2002/10/08 01:10:40  lewis
 *	cleanups to paragraphdatabaserep
 *	
 *	Revision 2.138  2002/10/08 00:49:35  lewis
 *	SPR#1118- paragraphdatabase now uses abstractparagraphdatabase api instead of public inherit from MarkerCover<>
 *	
 *	Revision 2.137  2002/09/28 18:12:23  lewis
 *	SPR#1116 - progress on rtf writing for embedded tables
 *	
 *	Revision 2.136  2002/09/23 22:13:48  lewis
 *	SPR#1111 - Fixed bug where we didn't process cellx specs for first row, and added clcbpat (cell
 *	background color) support. VERY primitive cruddy implementation. Cleanup soon.
 *	
 *	Revision 2.135  2002/09/23 15:15:28  lewis
 *	use Led_Max () instead of max() cuz std::max sometimes doesn't handle mixing int and unsigned int
 *	
 *	Revision 2.134  2002/09/22 21:05:35  lewis
 *	SPR#1109- New InvalidateLayout/PerformLayout code. New TemporarilySetOwningWP class to help you
 *	know who is the table owner, and use that to compute fitted row-heights. Primitive, inefficient implemenation,
 *	which also doesn't take into account 'preferred-height-type' rows and much more - but a good start
 *	
 *	Revision 2.133  2002/09/22 15:48:39  lewis
 *	DrawsBorders takes extra WP argument so it (and drawCEllCode) can use it to take into account imager->GetHScrollPos()
 *	from owning image to draw in the right place when horizontally scrolled (SPR#1108)
 *	
 *	Revision 2.132  2002/09/21 20:38:30  lewis
 *	change table code so Cell stores a styledatabase etc instead of a WP, and we dynamically create teh WP
 *	on an as-needed basis. This saves a HUGE amount of RAM. Plus - use SimpleTextStore instead of whatever
 *	textstore is being used by the main WP. This ALSO saves a huge amount of RAM (and should be fine performance wise
 *	since I cannot imagine a very large cell). (SPR#1105)
 *	
 *	Revision 2.131  2002/09/20 22:59:33  lewis
 *	add EmbeddedTableWordProcessor::GetDesiredHeight () in prep for proper row-height computation.
 *	EmbeddedTableWordProcessor::GetLayoutMargins () added to (roughly) get margins for cell
 *	WP's right. Added TemporarilyUseOwningWP.
 *	
 *	Revision 2.130  2002/09/20 15:09:36  lewis
 *	lots of name scoping cleanups and bug workarounds to get Led compiling on MWERKS and GCC and
 *	Borland - qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug/
 *	qGCC_MiscNestedClassNameLookupInTemplateBug/
 *	qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug)
 *	
 *	Revision 2.129  2002/09/20 12:45:25  lewis
 *	qConstNonConstPtrConversionsWithTemplatedMemberFunctionBug workaround for MWERKS CWPro7 compiler
 *	
 *	Revision 2.128  2002/09/19 14:14:52  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.127  2002/09/18 04:03:01  lewis
 *	more progress on tables - now support cellx to specify cell widths, and more related stuff -
 *	at least PARTLY supported
 *	
 *	Revision 2.126  2002/09/17 16:21:04  lewis
 *	SPR#1095 (table support) - got BASIC RTF reading working (right # cells and rows and contents
 *	read in; but no attributes like color, size etc for cells
 *	
 *	Revision 2.125  2002/09/14 01:34:03  lewis
 *	Table support now works decently. I think Add/Remove Row/Column now create WP cells appropraite. I hacked
 *	in demo code that set background colors and text for each cell. Fixed bug with layout. Now looks very good.
 *	Next stage - I think - is to add RTF support. And after that - editing support.
 *	
 *	Revision 2.124  2002/09/13 15:31:15  lewis
 *	more table support - now at the point where the cells all display, and a single embedded WP
 *	object displays in a particular cell
 *	
 *	Revision 2.123  2002/09/11 04:20:34  lewis
 *	SPR#1094- added VERY preliminary Table support - table class and new InsertTable command
 *	
 *	Revision 2.122  2002/09/04 01:53:19  lewis
 *	Lose qCannotExpandTemplateOfTypeWithNoExternalLinkage comment
 *	
 *	Revision 2.121  2002/05/06 21:33:59  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.120  2001/11/27 00:30:05  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.119  2001/11/08 21:33:19  lewis
 *	SPR#1074- special case at end of buffer inWordProcessorTextIOSinkStream::Flush
 *	
 *	Revision 2.118  2001/11/08 20:04:58  lewis
 *	SPR#1076- call CullZerod () in CheckMarkerBounaryConstraints ()
 *	
 *	Revision 2.117  2001/11/08 16:39:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.116  2001/11/08 16:39:20  lewis
 *	SPR#1072- in ParagraphDatabaseRep::CheckMarkerBounaryConstraints () - don't set the info
 *	for the whole stylerun - instead call SetInfoInnerLoop() on just the region of the last
 *	partition element. AND - revise alot of the comments to clarify the decision process
 *	about why things work this way.
 *	
 *	Revision 2.115  2001/10/20 13:39:00  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.114  2001/10/17 20:43:04  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.113  2001/10/17 01:52:31  lewis
 *	DocComments
 *	
 *	Revision 2.112  2001/10/09 21:10:44  lewis
 *	SPR#1058- qSupportLed23CompatAPI support
 *	
 *	Revision 2.111  2001/09/26 22:13:56  lewis
 *	lose an unused variable, and fix a couple asserts that said partitionElt_end = isntead
 *	of ==. TEST CAREFULLY to assure no bugs covered up by that mistake
 *	
 *	Revision 2.110  2001/09/26 15:41:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.109  2001/09/19 21:18:54  lewis
 *	SPR#1038- ParagraphDatabaseRep::CheckMarkerBounaryConstraints () must keep the paragraph
 *	info from the second ofthe combined markers (info associated with teh terminal NL)
 *
 *	Revision 2.108  2001/09/19 14:44:31  lewis
 *	SPR#1029 - speed tweek
 *	
 *	Revision 2.107  2001/09/12 16:20:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.106  2001/09/05 23:48:05  lewis
 *	SPR#1014- hook new SinkStream::EndOfBuffer () to detect EOB. Added flag IgnoreLastParaAttributes
 *	with WordProcessorTextIOSinkStream. Then set that when we create a sinkstream for D&D/CopyPaste
 *	etc (file IO creates its own). Then - fix flush code to clip the specification of paragraph
 *	info just past the last NL if at EOB and with that flag set. Also - fixed side bug I noticed
 *	at the same time - that we were missing default settings for the SpaceBefore/After attributes
 *	in the WordProcessorTextIOSinkStream
 *	
 *	Revision 2.105  2001/09/05 00:01:14  lewis
 *	SPR#1011- added WordProcessor::EmptySelectionParagraphSavedTextRep to fix problem with
 *	undo after empty selection not preserving paragraph info
 *
 *	Revision 2.104  2001/09/03 19:14:32  lewis
 *	SPR#0660,SPR#0762- rewrite much of the MarkerCover<> code to be simpler, clearer, and faster
 *	
 *	Revision 2.103  2001/08/28 18:43:45  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.102  2001/08/27 16:26:44  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.101  2001/08/23 17:02:34  lewis
 *	SPR#0971- Fixed the baseline I was using, and made the minimum size of the dot
 *	be 14 point (since thats what MSWord2k appeared to be doing - empicically)
 *	
 *	Revision 2.100  2001/08/23 16:45:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.99  2001/08/23 13:54:38  lewis
 *	SPR#0968- make bullet draw code use staticdefaultfont - not font of next char
 *	
 *	Revision 2.98  2001/07/19 23:43:42  lewis
 *	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug support
 *	
 *	Revision 2.97  2001/07/17 19:09:38  lewis
 *	SPR#0959- added preliminary RightClick-to-show-edit-embedding-properties support
 *	
 *	Revision 2.96  2001/07/16 20:07:02  lewis
 *	hoepfully fix bulletchar used on MacOS (non-UNICODE)
 *	
 *	Revision 2.95  2001/07/13 20:46:16  lewis
 *	SPR#0953- fix WordProcessor::AutoFormatIndentedText () to have the same behavior as MSWord2k
 *	
 *	Revision 2.94  2001/07/13 18:57:04  lewis
 *	SPR#0906- got basic (rtf only for now) bullet list support working
 *	
 *	Revision 2.93  2001/07/12 23:06:10  lewis
 *	SPR#0906- support reading indent levels in RTF
 *	
 *	Revision 2.92  2001/07/12 22:42:34  lewis
 *	SPR#0906- Mostly implement RTF reading for bullet-lists. Noticed (but not yet fixed) SPR#0952.
 *	
 *	Revision 2.91  2001/07/12 13:55:28  lewis
 *	SPR#0906- hack override WordProcessor::PostReplace() so typing at end of buffer with
 *	listmode on gets new empty lines bullet drawn/erased
 *	
 *	Revision 2.90  2001/07/11 21:56:40  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.89  2001/07/11 21:51:03  lewis
 *	SPR#0906- Partly implemented bullet/list support. Just did basic support (bullet lists, no other style lists).
 *	Did support indent levels (including new Increase/Decrease indent level commands). Still no RTF/HTML support,
 *	and a small display bug on following lines when I add bullet attribute.
 *	
 *	Revision 2.88  2001/05/04 17:07:28  lewis
 *	SPR#0902: use TextImager::GetEffectiveDefaultTextColor () instead of Led_GetTextColor ()
 *	
 *	Revision 2.87  2001/04/30 22:35:16  lewis
 *	SPR#0886- moved HilightARectangle_SolidHelper from TextImager class to Led_Tablet_
 *	
 *	Revision 2.86  2001/04/26 16:39:32  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.85  2001/01/03 14:24:33  Lewis
 *	use inherited typedef to work around diff with Borland C++ compiler
 *	
 *	Revision 2.84  2000/09/20 09:02:39  lewis
 *	silcence a few warnings with qSilenceAnnoyingCompilerWarnings
 *	
 *	Revision 2.83  2000/08/31 20:50:21  lewis
 *	use Led_Pen::kBlackPattern
 *	
 *	Revision 2.82  2000/08/28 20:35:45  lewis
 *	SPR#0828- changed args to Pre/PostInteractiveUndoHelper, and added (used) new UndoableContextHelper helper class.
 *	SPR#0827- use new TextInteractor::CommandNames struct to collect together a bunch of command names instead of seperate
 *	commandname accessors. SPR#0723- WordProcessor::InterectiveSetRegionHidable- and lots more...
 *	
 *	Revision 2.81  2000/08/13 04:15:07  lewis
 *	SPR#0784- rewrite hidden text code based on setting segwidth to zero, and patching computeNExtCursorPosition.
 *	Not 100% done - but mostly. VERY big change -  and many bug fixes related to this hidden text stuff folded in as well -
 *	but not complete
 *	
 *	Revision 2.80  2000/06/17 07:46:50  lewis
 *	Added WordProcessor::MeasureMinSegDescent () to help bugs with Led_LineSpacing::eExactTWIPSSpacing text -
 *	and the WordProcessor::MeasureSegmentBaseLine () code. This was uncovered as a result of SPR#0792- but also
 *	pertains to SPR#0760 (and uses a file - from that SPR folder to reproduce it)Headers/WordProcessor.h
 *	
 *	Revision 2.79  2000/06/17 05:04:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.78  2000/06/16 19:51:53  lewis
 *	SPR#0790- Change WordProcessor::GetJustification (size_t from, size_t to, Led_Justification* justification)
 *	family of functions to share common code - CheckForCommonParaValue<> TEMPLATE - rather than duplicating a WRONG algorithm.
 *	Then fixed that algorithm in that routine - to properly compare all the values.
 *	
 *	Revision 2.77  2000/06/13 22:15:51  lewis
 *	SPR#0785- revised InteractiveSetParagraphSpacing API.
 *	
 *	Revision 2.76  2000/06/13 21:17:51  lewis
 *	SPR#0785- added InteractiveSetParagraphSpacing- infrastructure for UI for paraspacing setting
 *	
 *	Revision 2.75  2000/06/13 20:27:10  lewis
 *	hacks to get compiling on GCC/XWindows again
 *	
 *	Revision 2.74  2000/06/13 15:31:44  lewis
 *	SPR#0767- convert tabstop support to store TWIPS internally
 *	
 *	Revision 2.73  2000/06/13 14:12:03  lewis
 *	cleanup - and make Led_Tablet_::CvtFromTWIPSXXX() arg a Led_TWIPS - instead of int
 *	
 *	Revision 2.72  2000/06/12 22:49:03  lewis
 *	SPR#0767- more work converting stuff (\fi\ri\li\margX etc) to TWIPS - instead of pixels. Tested a snapshot
 *	at this point - and LedItMFC seemed to work OK (thats all I've tested so far)
 *	
 *	Revision 2.71  2000/06/12 20:11:40  lewis
 *	SPR#0760- convert code to do SpaceBefore/AfterLineSpacing to using new Led_TWIPS and Led_LineSpacing structs -
 *	rather than using PIXELS
 *	
 *	Revision 2.70  2000/06/12 16:25:15  lewis
 *	convert SpaceBefore to be saved in Led_TWIPS - instead of pixels. Soon todo much more like this- SPR#0767
 *	
 *	Revision 2.69  2000/06/09 14:16:44  lewis
 *	futz with interpretting line spacing values we get back from MSWOrd RTF... must rething/rework!
 *	
 *	Revision 2.68  2000/05/31 15:06:47  lewis
 *	SPR#0763- ignore trailing whitespace in computation of how mcuh to shift centered/right justified text
 *	
 *	Revision 2.67  2000/05/31 12:27:57  lewis
 *	don't display extra linespacing if multipleLineSpacing flag not set - cuz not yet supported (see SPR#0761)
 *	
 *	Revision 2.66  2000/05/30 22:53:03  lewis
 *	SPR#0760- added preliminary support (all but RTF) for SpaceAfter/Before/BetweenLines support
 *	
 *	Revision 2.65  2000/05/30 22:28:36  lewis
 *	cleanup
 *	
 *	Revision 2.64  2000/05/30 22:27:13  lewis
 *	cleanup
 *	
 *	Revision 2.63  2000/05/30 22:04:29  lewis
 *	SPR#0760- Added preliminary support for word-processor line spacing. RTF reader support, and internal
 *	display support done. Still todo - RTF Writer support, and GUI for setting common spacing values
 *	
 *	Revision 2.62  2000/04/26 14:51:34  lewis
 *	Fix the flushing code for hidden text to REVERSE teh order of hiding - so the offsets work out right.
 *	Then move that code (temporarily - but hopefully permanantly) back to the flush call - rather than the DTOR.
 *	
 *	Revision 2.61  2000/04/25 23:00:49  lewis
 *	more work on SPR#0731 and SPR#0732- must apply Hidden stuff at the end - not in the flush - and even THAT may
 *	not be late enough. And must re-call AssureColorTableBuilt_FillIn/FONTTABLEONE TOO for sub-call to reader -
 *	cuz that changes avialable colors/fonts (if diff ones used in hidden text than in the main body text)
 *	
 *	Revision 2.60  2000/04/25 22:01:43  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.59  2000/04/25 16:16:30  lewis
 *	partial work on SPR#0722- added HidableTextMarkerOwner::GetHidableRegionsWithData () and started using that.
 *	
 *	Revision 2.58  2000/04/24 22:29:33  lewis
 *	added @TextInteractor::HookExternalizerChanged - and use it in WordProcessor to sync up with the hidableText database -
 *	related to SPR#0724
 *	
 *	Revision 2.57  2000/04/24 21:33:32  lewis
 *	fix const typo (only got warning on MWEKRS/Mac
 *	
 *	Revision 2.56  2000/04/24 21:22:27  lewis
 *	ALMOST got code in to set internalizer for hiddentextdatabase to that of this INTERACTOR - but for mutual
 *	reference and REFCOUNTING problem it introduces. Must be a bit more careful
 *	
 *	Revision 2.55  2000/04/24 16:47:04  lewis
 *	Very substantial change to FlavorPackage stuff - now NOT mixed into TextInteractor - but simply owned using Led_RefCntPtr<>.
 *	Also - did all the proper subclassing in WP class etc to make this work (almost -
 *	still must fix OLE embeddings) as before. SPR# 0742
 *	
 *	Revision 2.54  2000/04/16 13:58:56  lewis
 *	small tweeks to get recent changes compiling on Mac
 *	
 *	Revision 2.53  2000/04/14 22:40:29  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.52  2000/03/31 23:45:37  lewis
 *	SPR#0717- got first cut (imperfect) at saving hidableText in RTF (problem is assumes its currently being shown).
 *	Other cleanups - esp to hidable text code (invariants)
 *	
 *	Revision 2.51  2000/03/31 00:50:05  lewis
 *	more work on SPR#0717: support RTF reading of \v tags - hidden text. Integrated HiddenText support into WordProcessor
 *	class (in a maner patterend after how we treat WordProcessor::ParagraphDatabasePtr
 *	
 *	Revision 2.50  2000/03/17 22:31:49  lewis
 *	use sort () directly on LessThan<MARKER> - rather than trivial 'SortMarker' method. And moved
 *	LessThan<MARKER> to Marker.h from MarkerCover<>
 *	
 *	Revision 2.49  1999/12/28 16:52:50  lewis
 *	fix mac typo
 *	
 *	Revision 2.48  1999/12/28 14:05:23  lewis
 *	SPR#0685- and fixed other small bug in WordProcessor::DrawSegment()/show tabstops code
 *	
 *	Revision 2.47  1999/12/28 11:31:28  lewis
 *	SPR#0669- finished it - by adding DrawSegment() override to draw the tab glyph
 *	
 *	Revision 2.46  1999/12/27 23:23:38  lewis
 *	fix some small mac-compatabilty problems with new show/hide invisibles code (SPR#0669)
 *	
 *	Revision 2.45  1999/12/27 17:31:35  lewis
 *	SPR#0669 - work on this, but not done. Just added most of the support so this command is autohandled by
 *	WordProcessorCommonCommandHelper<> template. And adjusted the characters output (at least for UNICODE).
 *	
 *	Revision 2.44  1999/12/27 16:03:59  lewis
 *	SPR#0667- SoftLineBreak support
 *	
 *	Revision 2.43  1999/12/21 03:36:52  lewis
 *	SPR#0669- Preliminary version of ShowHiddenCharacters API implemeantion.
 *	
 *	Revision 2.42  1999/12/15 01:10:05  lewis
 *	Work around SPR#0659- Added  fNeedExtraUpdateCheck_UpdateInfo, and use that with fNeedExtraUpdateCheck.
 *	Track cleanup of this hack in SPR#0660
 *	
 *	Revision 2.41  1999/12/14 18:12:50  lewis
 *	don't use default 'int' for const (GCC warnings)
 *	
 *	Revision 2.40  1999/12/09 03:28:04  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.39  1999/11/15 21:34:32  lewis
 *	Use Led_WindowDC instead of CWindowDC - so code compiles using LEdGDI wrappers instead of MFC -
 *	so can build Led-based apps without MFC
 *	
 *	Revision 2.38  1999/11/13 16:32:24  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.37  1999/06/29 16:01:55  lewis
 *	spr#0600- must adjust RHS of rectangles for LHS margin as well
 *	
 *	Revision 2.36  1999/06/29 01:48:53  lewis
 *	several subtle changes to the MarkerCover<> NeedExtraUpdateCheckMode caching code - see spr#0599 for details
 *	
 *	Revision 2.35  1999/06/24 21:42:27  lewis
 *	move assertion in WordProcessorTextIOSinkStream::DTOR. Only assert size=0 IF Flush didn't throw. If it does
 *	throw (as in spr#0595), then assert would have failed - but not a problem
 *	
 *	Revision 2.34  1999/05/03 22:05:23  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.33  1999/04/28 14:21:50  lewis
 *	change default margin code from 0/1 to 6 inches. Important for reading old formats with no margin support
 *	(like HTML or old Led format)
 *	
 *	Revision 2.32  1999/03/25 22:58:57  lewis
 *	fix typo, and fix problem where I had passed wrong args to MarkerInfo::UpdaetInfo CTOR etc in the undo helper.
 *	And HACK WORKAROUND FOR SPR#0565
 *	
 *	Revision 2.31  1999/03/25 22:31:38  lewis
 *	react to name change in MarkerCover::GetInfo->GetInfoMarkers. And added new ParagraphInfoChangeTextRep class
 *	to handle the undo information, and use that in the template InteractiveWPHelper1. Should make this much
 *	faster keeping track of undo info, and fixes spr#0563 where undo of para style change had no effect in empty document
 *	
 *	Revision 2.30  1999/03/25 00:03:23  lewis
 *	fix spr#0562
 *	
 *	Revision 2.29  1999/03/24 22:59:15  lewis
 *	spr#0534 - mostly fixed - add InteractiveSETXXX methods which do the undo wrapper code - though not well
 *	
 *	Revision 2.28  1999/03/24 16:46:51  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.27  1999/03/24 16:23:32  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.26  1999/03/24 15:47:27  lewis
 *	spr#0561 fixed- TexImager::NoTabletAvailable now subclasses from TextImager::NotFullyInitialized etc
 *	
 *	Revision 2.25  1999/03/22 19:47:45  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.24  1999/03/22 19:41:55  lewis
 *	spr#0560 - override WordProcessor::ResetTabStops to take into account margins properly
 *	
 *	Revision 2.23  1999/03/22 17:17:13  lewis
 *	cleanups - use typedef to map nested names to top level names in this file
 *	
 *	Revision 2.22  1999/03/10 19:00:16  lewis
 *	fix spr#0558- WordProcessorTextIOSinkStream::DTOR() must call flush cuz base class version DTOR calls flush but
 *	binds to base class version of FLUSH cuz of quirk of how DTOR/vtables work
 *	
 *	Revision 2.21  1999/03/10 18:43:37  lewis
 *	small speed tweeks(caching in automatic variables) of WordProcessor::ParagraphDatabaseRep::CheckMarkerBounaryConstraints
 *	
 *	Revision 2.20  1999/03/10 17:19:33  lewis
 *	MAJOR speedup to reading RTF(spr#0546). Lots of code cleanups to the WordProcessorTextIOSinkStream code. But biggest
 *	change is to override AppendText() and simply CACHE the  fSavedParaInfo records entirely, and write them all in
 *	the FLush(). Turns out to be much simpler that what i had before, and almost identical to what we do in base
 *	class for style info. And MUCH faster cuz many fewer (maybe just one?) Flush.
 *	
 *	Revision 2.19  1999/03/08 15:19:02  lewis
 *	workaround/fix spr#0556
 *	
 *	Revision 2.18  1999/03/02 04:25:54  lewis
 *	speed tweek flush code to not flush in dataSize == 0, and to not flush in WordProcessorTextIOSinkStream::SetLeftMargin
 *	etc if margin (arg) not changed
 *	
 *	Revision 2.17  1999/02/21 23:14:02  lewis
 *	forgot to initialize fCurFirstIndent/fCurLeft/RightMargin for second CTOR of dbase
 *	
 *	Revision 2.16  1999/02/21 21:54:38  lewis
 *	add WordProcessor::ParagraphDatabaseRep::GetStaticDefaultParagraphInfo and call from CTOR of paragraphdatabase,
 *	so we specify default as arg to base class and avoid SetInfo call which was causing undesirable (though not wrong)
 *	DidUpdates and therefore setdirty calls (spr#0550)
 *	
 *	Revision 2.15  1999/02/21 20:04:17  lewis
 *	override Writer::SRC::GetFirstIndent/GetMargins() calls so we can write out RTF / paragraph margin information
 *	
 *	Revision 2.14  1999/02/21 13:56:26  lewis
 *	Added SetFirstIndent/SetLeftMargin/SetRightMargin/RTF\li\ri\fi, and amended\pard support for these RTF tags
 *	(READING ONLY). Works (in preliminary testing), but with a few small sprs noted in dbase (validation and fixing defaults)
 *	
 *	Revision 2.13  1999/02/11 02:31:46  lewis
 *	add one more hack throw NoTabletAvailable call to get working of LedItMac. Rethink this. Not a bad idea, just badly executed
 *	
 *	Revision 2.12  1999/02/10 16:20:00  lewis
 *	added wordprocessor::Get/Set/Margins/FirstIndent methods (maybe want to rethink these apis?)
 *	
 *	Revision 2.11  1999/02/09 16:35:07  lewis
 *	added more(nearly all) support for margins, and first indent. Added support to ParagraphInfo object.
 *	Use it from GetMarginInfo WordWrappedTextImager call. Put in more decent default-value code for margins.
 *	Support chaning Partition of ParagrapDatabaseRep better (resync). Some DOCS. More?
 *	
 *	Revision 2.10  1999/02/08 22:30:21  lewis
 *	remove some tmphacks I had put in (qsupportoldgetlayoutwidthgovers=1 etc) so I could get 2.3b3/4
 *	out the door without it all working/thought out. Now really fix things
 *	
 *	Revision 2.9  1998/10/30 14:32:29  lewis
 *	More support (not done yet) for GetLayoutMargins instead of GetLayoutWidth - and margins support.
 *	
 *	Revision 2.8  1998/04/25  01:40:52  lewis
 *	Get/Set StandardTabStopList support in Src/SinkStreams.
 *	TabStopList support in WordProcessor itself.
 *	fParagraphDatabase->SetPartition (NULL); KLUDGE - so I don't need similar kludge in the LedItMFC applet.
 *
 *	Revision 2.7  1998/04/09  01:30:23  lewis
 *	Rewrote code to align PM markers with ParagrpahInfo marks. Now I think it works right.
 *	Now override SetInfo() - instead of duplicating its code in teh various callers to align requests on PM boudaries.
 *
 *	Revision 2.6  1998/04/08  02:10:02  lewis
 *	Lots of chagnes - ne wWordProcessorTextIOSrcStream/WordProcessorTextIOSinkStream classes (for better modularity).
 *	Worked on ParagraphDbase code - but still somewhat broken/fragile.
 *
 *	Revision 2.5  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1997/12/24  03:32:46  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1997/09/29  15:50:30  lewis
 *	New WordProcessor support - added per-para support for justicication.
 *	and para database (using Led_RefCntPtr and MarkerCover).
 *
 *	Revision 2.2  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/06/18  03:00:47  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *	<========== CODE MOVED HERE FROM TextInteractorMixins.cpp ==========>
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<cctype>

#include	"LedConfig.h"

#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4786)		//qQuiteAnnoyingDebugSymbolTruncationWarnings
#endif

#include	"CodePage.h"
#include	"SimpleTextStore.h"
#include	"StyledTextEmbeddedObjects.h"

#include	"WordProcessor.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	namespace	Led {
#endif







class	ParagraphInfoChangeTextRep : public InteractiveReplaceCommand::SavedTextRep {
	private:
		typedef	InteractiveReplaceCommand::SavedTextRep	inherited;
	public:
		typedef	WordProcessor::ParagraphDatabasePtr	ParagraphDatabasePtr;
		typedef	WordProcessor::ParagraphInfo		ParagraphInfo;
		typedef	pair<ParagraphInfo,size_t>			ParaInfoNSize;
	public:
		ParagraphInfoChangeTextRep (WordProcessor* interactor, size_t from, size_t to):
				inherited (from, to),
				fSavedInfo ()
			{
				fSavedInfo = interactor->GetParagraphDatabase ()->GetParagraphInfo (from, to-from);
				Led_Assert (GetLength () == to-from);
			}
		override	size_t	GetLength () const
			{
				size_t	len	=	0;
				for (vector<ParaInfoNSize>::const_iterator i = fSavedInfo.begin (); i != fSavedInfo.end (); ++i) {
					len += (*i).second;
				}
				return len;
			}
		override	void	InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite)
			{
				Led_RequireNotNil (dynamic_cast<WordProcessor*> (interactor));
				WordProcessor*	wp	=	dynamic_cast<WordProcessor*> (interactor);
				Led_RequireNotNil (wp);
				Led_Arg_Unused (nBytesToOverwrite);
				Led_Assert (nBytesToOverwrite == GetLength ());	// For THIS particular kind of update, the length cannot change since we don't save the text
				ParagraphDatabasePtr	paraDBase	=	wp->GetParagraphDatabase ();
				paraDBase->SetParagraphInfo (at, fSavedInfo);
			}

	private:
		vector<ParaInfoNSize>	fSavedInfo;
};




// Somewhat kludgy way to share code. Tried a member template, but that caused MSVC60SP1 to crash.
template	<typename	SPECIALIZER, typename T1>
	void	InteractiveWPHelper1 (WordProcessor* wp, T1 arg1)
	{
		TextInteractor::InteractiveModeUpdater	iuMode (*wp);
		typedef	InteractiveReplaceCommand::SavedTextRep	SavedTextRep;
		wp->BreakInGroupedCommands ();
		size_t			selStart				=	wp->GetSelectionStart ();
		size_t			selEnd					=	wp->GetSelectionEnd ();
		SavedTextRep*	before					=	NULL;
		SavedTextRep*	after					=	NULL;
		try {
			if (wp->GetCommandHandler () != NULL) {
				before = new ParagraphInfoChangeTextRep (wp, selStart, selEnd);
			}
			SPECIALIZER::DoIt (wp, selStart, selEnd, arg1);
			if (wp->GetCommandHandler () != NULL) {
				after = new ParagraphInfoChangeTextRep (wp, selStart, selEnd);
				wp->PostInteractiveUndoPostHelper (&before, &after, selStart, SPECIALIZER::GetName (wp));
			}
		}
		catch (...) {
			delete before;
			delete after;
			throw;
		}
		wp->BreakInGroupedCommands ();
	}
struct	DoIt_SetJustification {
	static	void	DoIt (WordProcessor* wp, size_t selStart, size_t selEnd, Led_Justification justification)
		{
			wp->SetJustification (selStart, selEnd, justification);
		}
	static	Led_SDK_String	GetName (WordProcessor* wp) { return wp->GetCommandNames ().fJustificationCommandName; }
};
struct	DoIt_SetStandardTabStopList {
	static	void	DoIt (WordProcessor* wp, size_t selStart, size_t selEnd, WordProcessor::StandardTabStopList tabStops)
		{
			wp->SetStandardTabStopList (selStart, selEnd, tabStops);
		}
	static	Led_SDK_String	GetName (WordProcessor* wp) { return wp->GetCommandNames ().fStandardTabStopListCommandName; }
};
struct	DoIt_SetMargins {
	struct	Margins { Led_TWIPS fLHS; Led_TWIPS fRHS; Margins (Led_TWIPS l, Led_TWIPS r): fLHS (l), fRHS(r) {} };
	static	void	DoIt (WordProcessor* wp, size_t selStart, size_t selEnd, Margins margins)
		{
			wp->SetMargins (selStart, selEnd, margins.fLHS, margins.fRHS);
		}
	static	Led_SDK_String	GetName (WordProcessor* wp) { return wp->GetCommandNames ().fMarginsCommandName; }
};
struct	DoIt_SetFirstIndent {
	static	void	DoIt (WordProcessor* wp, size_t selStart, size_t selEnd, Led_TWIPS firstIndent)
		{
			wp->SetFirstIndent (selStart, selEnd, firstIndent);
		}
	static	Led_SDK_String	GetName (WordProcessor* wp) { return wp->GetCommandNames ().fFirstIndentCommandName; }
};
struct	DoIt_SetMarginsAndFirstIndent {
	struct	MarginsAndFirstIndent { Led_TWIPS fLHS; Led_TWIPS fRHS; Led_TWIPS fFirstIndent; MarginsAndFirstIndent (Led_TWIPS l, Led_TWIPS r, Led_TWIPS firstIndent): fLHS (l), fRHS(r), fFirstIndent (firstIndent) {} };
	static	void	DoIt (WordProcessor* wp, size_t selStart, size_t selEnd, MarginsAndFirstIndent marginsEtc)
		{
			wp->SetMargins (selStart, selEnd, marginsEtc.fLHS, marginsEtc.fRHS);
			wp->SetFirstIndent (selStart, selEnd, marginsEtc.fFirstIndent);
		}
	static	Led_SDK_String	GetName (WordProcessor* wp) { return wp->GetCommandNames ().fMarginsAndFirstIndentCommandName; }
};
struct	DoIt_SetParagraphSpacing {
	struct	AllSpacingArgs { Led_TWIPS fSpaceBefore; Led_TWIPS fSpaceAfter; Led_LineSpacing fLineSpacing; bool fSBValid, fSAValid, fSLValid; AllSpacingArgs (Led_TWIPS sb, bool sbValid, Led_TWIPS sa, bool saValid, Led_LineSpacing sl, bool slValid): fSpaceBefore (sb), fSBValid (sbValid), fSpaceAfter (sa), fSAValid (saValid), fLineSpacing (sl), fSLValid (slValid) {} };
	static	void	DoIt (WordProcessor* wp, size_t selStart, size_t selEnd, AllSpacingArgs spacingArgs)
		{
			if (spacingArgs.fSBValid) {
				wp->SetSpaceBefore (selStart, selEnd, spacingArgs.fSpaceBefore);
			}
			if (spacingArgs.fSAValid) {
				wp->SetSpaceAfter (selStart, selEnd, spacingArgs.fSpaceAfter);
			}
			if (spacingArgs.fSLValid) {
				wp->SetLineSpacing (selStart, selEnd, spacingArgs.fLineSpacing);
			}
		}
	static	Led_SDK_String	GetName (WordProcessor* wp) { return wp->GetCommandNames ().fParagraphSpacingCommandName; }
};
struct	DoIt_SetListStyle {
	static	void	DoIt (WordProcessor* wp, size_t selStart, size_t selEnd, ListStyle listStyle)
		{
			wp->SetListStyle (selStart, selEnd, listStyle, true);
		}
	static	Led_SDK_String	GetName (WordProcessor* wp) { return wp->GetCommandNames ().fSetListStyleCommandName; }
};
struct	DoIt_IndentUnIndentList {
	static	void	DoIt (WordProcessor* wp, size_t selStart, size_t selEnd, bool indent)
		{
			unsigned char	indentLevel	=	wp->GetListIndentLevel (selStart);
			if (indent) {
				if (indentLevel < 8) {
					indentLevel++;
				}
				else {
					Led_BeepNotify ();
				}
			}
			else {
				if (indentLevel > 0) {
					indentLevel--;
				}
				else {
					Led_BeepNotify ();
				}
			}
			wp->SetListIndentLevel (selStart, selEnd, indentLevel, true);
		}
	static	Led_SDK_String	GetName (WordProcessor* wp) { return wp->GetCommandNames ().fIndentLevelChangeCommandName; }
};








/*
 ********************************************************************************
 ********************************* ParagraphInfo ********************************
 ********************************************************************************
 */
typedef	WordProcessor::ParagraphInfo	ParagraphInfo;
ParagraphInfo::ParagraphInfo ():
	fJustification (eLeftJustify),
	fTabStops (),
	fLeftMargin (0),
	fRightMargin (1),
	fFirstIndent (0),
	fSpaceBefore (Led_TWIPS (0)),
	fSpaceAfter (Led_TWIPS (0)),
	fLineSpacing (),
	fListStyle (eListStyle_None),
	fListIndentLevel (0)
{
}




/*
 ********************************************************************************
 ****************************** IncrementalParagraphInfo ************************
 ********************************************************************************
 */
typedef	WordProcessor::IncrementalParagraphInfo	IncrementalParagraphInfo;





/*
 ********************************************************************************
 **************************** ParagraphDatabaseRep ******************************
 ********************************************************************************
 */
typedef	WordProcessor::ParagraphDatabaseRep	ParagraphDatabaseRep;
ParagraphDatabaseRep::ParagraphDatabaseRep (TextStore& textStore):
	inheritedMC (textStore, GetStaticDefaultParagraphInfo ()),
	fPartition ()
{
	//tmphack test - see if this fixes SPR#1129
	// LGP 2002-10-19 - didnt appear to work so probably get rid of it - but test some more!!!
	SetPartition (new WordProcessor::WPPartition (GetTextStore (), *this));
}

void	ParagraphDatabaseRep::SetPartition (const PartitionPtr& partitionPtr)
{
	if (fPartition != partitionPtr) {
		fPartition = partitionPtr;
		// Re-align all the partition elts as close as possible to those appropriate for the new partition.
		CheckMarkerBounaryConstraints (0, GetTextStore ().GetEnd ());
	}
}

/*
@METHOD:		WordProcessor::ParagraphDatabaseRep::GetStaticDefaultParagraphInfo
@DESCRIPTION:
	<p>Return the default @'WordProcessor::ParagraphInfo' object used when the paragraph database object is created.</p>
*/
ParagraphInfo	ParagraphDatabaseRep::GetStaticDefaultParagraphInfo ()
{
	ParagraphInfo	defaultPi;
	const	int	kDefaultInches	=	6;
	defaultPi.SetMargins (Led_TWIPS (0), Led_TWIPS (kDefaultInches * 1440));
	defaultPi.SetJustification (eLeftJustify);
	return defaultPi;
}

const ParagraphInfo&	ParagraphDatabaseRep::GetParagraphInfo (size_t charAfterPos) const
{
	return GetInfo (charAfterPos);
}

vector<pair<WordProcessor::ParagraphInfo,size_t> >	ParagraphDatabaseRep::GetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing) const
{
	return GetInfo (charAfterPos, nTCharsFollowing);
}

void	ParagraphDatabaseRep::SetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers)
{
	if (infoForMarkers.GetMargins_Valid ()) {
		fCachedFarthestRightMarginInDocument = kBadCachedFarthestRightMarginInDocument;
	}
	SetInfo (charAfterPos, nTCharsFollowing, infoForMarkers);
}

void	ParagraphDatabaseRep::SetParagraphInfo (size_t charAfterPos, const vector<pair<WordProcessor::IncrementalParagraphInfo,size_t> >& infoForMarkers)
{
	for (vector<pair<WordProcessor::IncrementalParagraphInfo,size_t> >::const_iterator i = infoForMarkers.begin (); i != infoForMarkers.end (); ++i) {
		if ((*i).first.GetMargins_Valid ()) {
			fCachedFarthestRightMarginInDocument = kBadCachedFarthestRightMarginInDocument;
			break;
		}
	}
	SetInfos (charAfterPos, infoForMarkers);
}

void		ParagraphDatabaseRep::SetParagraphInfo (size_t charAfterPos, const vector<pair<ParagraphInfo,size_t> >& infoForMarkers)
{
	fCachedFarthestRightMarginInDocument = kBadCachedFarthestRightMarginInDocument;
	SetInfos2 (charAfterPos, infoForMarkers);
}

/*
@METHOD:		WordProcessor::ParagraphDatabaseRep::SetInfo
@DESCRIPTION:	<p>Override @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::SetInfos' to assure we have a partition
			associated with us before allowing changes. This partition can be changed later.</p>
*/
void	ParagraphDatabaseRep::SetInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers)
{
	Led_Assert (not fPartition.IsNull ());
	inheritedMC::SetInfo (charAfterPos, nTCharsFollowing, infoForMarkers);
}

void	ParagraphDatabaseRep::SetInfos (size_t charAfterPos, const vector<pair<WordProcessor::IncrementalParagraphInfo,size_t> >& infoForMarkers)
{
	Led_Assert (not fPartition.IsNull ());
	inheritedMC::SetInfos (charAfterPos, infoForMarkers);
}

/*
@METHOD:		WordProcessor::ParagraphDatabaseRep::NoteCoverRangeDirtied
@ACCESS:		protected
@DESCRIPTION:	<p>Override @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::NoteCoverRangeDirtied' 
			to assure we check boundary contraints (assure paragraph cover markers end on paragraph boundaries,
			as defined by our partition).</p>
*/
void	ParagraphDatabaseRep::NoteCoverRangeDirtied (size_t from, size_t to, const MarkerVector& rangeAndSurroundingsMarkers)
{
	inheritedMC::NoteCoverRangeDirtied (from, to, rangeAndSurroundingsMarkers);
	CheckMarkerBounaryConstraints (rangeAndSurroundingsMarkers);
}

void	ParagraphDatabaseRep::ConstrainSetInfoArgs (size_t* charAfterPos, size_t* nTCharsFollowing)
{
	Led_RequireNotNil (charAfterPos);
	Led_RequireNotNil (nTCharsFollowing);

	size_t	from	=	*charAfterPos;
	size_t	to		=	from + *nTCharsFollowing;
	PartitionMarker*	startPara	=	fPartition->GetPartitionMarkerContainingPosition (from);
	PartitionMarker*	endPara		=	(to <= startPara->GetEnd ())? startPara: fPartition->GetPartitionMarkerContainingPosition (to);
	// If the 'to' position happens to be the exact start of a paritition, it would have also been the end of the previous
	// parition. Take it to be the end of the previous one (unless that would imply no selection)
	if (startPara != endPara and endPara->GetStart () == to) {
		endPara = endPara->GetPrevious ();
	}
	size_t				bigFrom		=	startPara->GetStart ();
	size_t				bigEnd		=	min (endPara->GetEnd (), GetTextStore ().GetEnd ()+1);

	*charAfterPos = bigFrom;
	*nTCharsFollowing = bigEnd-bigFrom;
}

/*
@METHOD:		WordProcessor::ParagraphDatabaseRep::CheckMarkerBounaryConstraints
@ACCESS:		private
@DESCRIPTION:	<p>Called internally to check that all the paragraph info records in the MarkerCover
			respect the contraint that they start and end on paragraph boundaries.</p>
*/
void	ParagraphDatabaseRep::CheckMarkerBounaryConstraints (size_t from, size_t to) throw ()
{
	if (not fPartition.IsNull ()) {
		MarkerVector	markers	=	CollectAllInRange_OrSurroundings (from, to);
		sort (markers.begin (), markers.end (), LessThan<ParagraphInfoMarker> ());
		CheckMarkerBounaryConstraints (markers);
	}
}

void	ParagraphDatabaseRep::CheckMarkerBounaryConstraints (const MarkerVector& rangeAndSurroundingsMarkers) throw ()
{
	/*
	 *	For each paragraph style run, check if its edges fall on paragraph (as specified by the partition) boundaries.
	 *	If not - then adjust the style runs so they do.
	 */
	if (not fPartition.IsNull ()) {
		for (MarkerVector::const_iterator i = rangeAndSurroundingsMarkers.begin (); i != rangeAndSurroundingsMarkers.end (); ++i) {
			ParagraphInfoMarker*	m			=	*i;
			Led_AssertNotNil (m);
			size_t					m_start;		// Use these temporaries as speed tweeks -LGP990310
			size_t					m_end;
			m->GetRange (&m_start, &m_end);
			size_t					m_length	=	m_end - m_start;

			// Ignore zero-length ParagraphInfoMarkers - they will soon be culled...
			Led_Assert (m->GetLength () == m_length);
			if (m_length != 0) {
				Led_Assert (m_start == m->GetStart ());
				PartitionMarker*	partitionElt	=	fPartition->GetPartitionMarkerContainingPosition (m_start);
				Led_Assert (partitionElt->GetStart () == m->GetStart ());	// cuz we fix these up in order, and can only adjust ends of
																			// cur marker, and start of following one
				Led_Assert (m->GetEnd () == m_end);
				size_t	partitionElt_end;
				for (;(partitionElt_end = partitionElt->GetEnd ()) < m_end;) {
					partitionElt = partitionElt->GetNext ();
					Led_AssertNotNil (partitionElt);	// must get to end by markerpos before last marker
				}
				Led_Assert (partitionElt_end == partitionElt->GetEnd ());

				/*
				 *		We've now walked all the pms in this ParagraphInfoMarker, and if the last pm end was the same as
				 *	our ParagraphInfoMarker end, were all set. If they are unequal (cuz of how we walked the list), then
				 *	the PM extends PAST our ParagraphInfoMarker end. If so, we must adjust the boundaries between the
				 *	current ParagraphInfoMarker and the following one.
				 *	
				 *		Now we have an arbitrary choice to make. Do we move the ParagraphInfoMarker to the right, or to
				 *	the left. This is completely arbitrary, and the only significance is that if we move to the right
				 *	then we lose ParagraphInfo styling of the following sentence when merging them together, and
				 *	if we go the other way, we get the opposite effect.
				 *	
				 *		For now, chose to move the ParagraphInfoMarker to the RIGHT.
				 *
				 *		Note - this arbitrary choice has some UI consequences. I've tried (as part of SPR#1072 (2001-11-08)) to
				 *	compare what Led does with MSWord2k, and WordPad. WordPad and MSWord2k behave identically, but somewhat inconsistently
				 *	within themselves. If you have two paras with different style info, if you select the SINGLE NL between the two and delete
				 *	it, the paras are merged with the info from the first paragraph. If you select TWO chars - then the paras are merged
				 *	with the info from the second. VERY strange.
				 *
				 *		Anyhow - for now - with Led - I've decided to adopt the principle that the info resides (logically) in the
				 *	paragraph-termitating character. That implies that when the paragraphs are merged - we copy the info from the
				 *	FOLLOWING paragraph. -- LGP 2001-11-08- SPR#1072.
				 */
				Led_Assert (m->GetEnd () == m_end);
				if (partitionElt_end != m_end) {
					MarkerVector	markers	=	CollectAllNonEmptyInRange (m_end, m_end + 1);
					Led_Assert (markers.size () == 1);		// Better be exactly ONE following marker
					ParagraphInfoMarker*	followingMarker	=	markers[0];
					Led_Assert (m_end == followingMarker->GetStart ());				// Assure these markers I'm operating on are already continguous
					Led_Assert (partitionElt_end == partitionElt->GetEnd ());
					GetTextStore ().SetMarkerEnd (m, partitionElt_end);
					Led_Assert (followingMarker->GetEnd () >= partitionElt->GetEnd ());		// Cannot set negative length
					Led_Assert (partitionElt_end == partitionElt->GetEnd ());
					GetTextStore ().SetMarkerStart (followingMarker, partitionElt_end);
					IncrementalParagraphInfo	followingInfo	=	IncrementalParagraphInfo (followingMarker->GetInfo ());
					if (followingMarker->GetLength () == 0) {
						fMarkersToBeDeleted.AccumulateMarkerForDeletion (followingMarker);
						Led_Assert (partitionElt_end == m->GetEnd ());
						CheckForMerges (partitionElt_end);
					}
					/*
					 *	When we combine two paragraph markers, we must choose which paragraphs info to keep.
					 *	We choose to keep the marker info from the second marker. SPR#1038.
					 *
					 *	But, do this carefully - just to the last partition element's worth of text - not to the last
					 *	paragraph style run element (which could be multiple paragraphs). This was the crux of the fix
					 *	in SPR#1072 (2001-11-08).
					 */
					SetInfoInnerLoop (partitionElt->GetStart (), partitionElt->GetEnd (), followingInfo,
									UpdateInfo (partitionElt->GetStart (), partitionElt->GetEnd (), LED_TCHAR_OF (""), 0, false, false),
									NULL
								);
					CullZerod (partitionElt->GetStart ());
					CullZerod (partitionElt->GetEnd ());
				}
			}
		}
	}
}

#if		qDebug
void	ParagraphDatabaseRep::Invariant_ () const
{
	inheritedMC::Invariant_ ();

	// Check partition in-sync with our marker alignments
	if (not fPartition.IsNull ()) {
		// all effected text is diff if we did a replace or not - if no, then from-to,
		// else from to from+textInserted (cuz from-to deleted)
		MarkerVector	markers	=	CollectAllInRange_OrSurroundings (0, GetTextStore ().GetLength () + 1);
		sort (markers.begin (), markers.end (), LessThan<ParagraphInfoMarker> ());
		PartitionMarker*		lastPartitionElt		=	NULL;
		ParagraphInfoMarker*	lastParagraphInfoMarker	=	NULL;
		for (MarkerVector::const_iterator i = markers.begin (); i != markers.end (); ++i) {
			ParagraphInfoMarker*	m	=	*i;
			Led_Assert (m->GetLength () != 0);
			PartitionMarker*	curPartitionElt	=	fPartition->GetPartitionMarkerContainingPosition (m->GetStart ());
			Led_Assert (curPartitionElt != lastPartitionElt);				// cuz then we would have multiple ParagraphInfos in a single PartitionElt
			Led_Assert (curPartitionElt->GetStart () == m->GetStart ());	// partElt boundary must always match start
			Led_Assert (curPartitionElt->GetEnd () <= m->GetEnd ());		// ParagraphInfo contains either one or more (but not less
																			// or partial) partition elts
			lastPartitionElt = curPartitionElt;
			lastParagraphInfoMarker = m;
		}
	}
}
#endif






/*
 ********************************************************************************
 ********************************* WordProcessor ********************************
 ********************************************************************************
 */
WordProcessor::WPIdler::WPIdler ():
	fWP (NULL)
{
}

void	WordProcessor::WPIdler::SpendIdleTime ()
{
	/*
	 *	Just randomly grab tables, and lay them out. Don't spend more than kMaxTime per
	 *	idle time call. First check if 'fSomeInvalidTables' as a performance hack (SPR#1365).
	 */
	AbstractParagraphDatabaseRep*	pdbRep	=	fWP->GetParagraphDatabase ();
	Led_AssertNotNil (pdbRep);
	if (pdbRep->fSomeInvalidTables) {
		const	float	kMaxTime	=	0.2f;
		float			startTime	=	Led_GetTickCount ();
		float			endTime		=	startTime + kMaxTime;
		Led_AssertNotNil (fWP);
		typedef	WordProcessor::Table	Table;
		vector<Table*>	tables = fWP->GetTablesInRange (0, fWP->GetEnd ());
		bool			maybeMoreTables	=	false;
		for (vector<Table*>::iterator i = tables.begin (); i != tables.end (); ++i) {
			Table*	t	=	*i;
			if (t->fNeedLayout != Table::eDone) {
				Table::TemporarilySetOwningWP	owningWPSetter (*t, *fWP);
				t->PerformLayout ();
				if (endTime < Led_GetTickCount ()) {
					maybeMoreTables = true;
					break;
				}
			}
		}
		pdbRep->fSomeInvalidTables = maybeMoreTables;
	}
}










/*
 ********************************************************************************
 ******************* WordProcessor::DialogSupport *******************************
 ********************************************************************************
 */
WordProcessor::DialogSupport::FontNameSpecifier	WordProcessor::DialogSupport::CmdNumToFontName (CommandNumber /*cmdNum*/)
{
	
	Led_Assert (false);	// must be overriden - or don't include / enable commands that refer to this method.
	return FontNameSpecifier ();

}

bool	WordProcessor::DialogSupport::IsPredefinedFontSize (Led_Distance fontSize)
{
	switch (fontSize) {
		case	9:			return true;
		case	10:			return true;
		case	12:			return true;
		case	14:			return true;
		case	18:			return true;
		case	24:			return true;
		case	36:			return true;
		case	48:			return true;
		case	72:			return true;
		default:			return false;
	}
}

Led_Distance	WordProcessor::DialogSupport::FontCmdToSize (CommandNumber commandNum)
{
	switch (commandNum) {
		case	kFontSize9_CmdID:			return 9;
		case	kFontSize10_CmdID:			return 10;
		case	kFontSize12_CmdID:			return 12;
		case	kFontSize14_CmdID:			return 14;
		case	kFontSize18_CmdID:			return 18;
		case	kFontSize24_CmdID:			return 24;
		case	kFontSize36_CmdID:			return 36;
		case	kFontSize48_CmdID:			return 48;
		case	kFontSize72_CmdID:			return 72;
	}
	return 0;
}

Led_Distance	WordProcessor::DialogSupport::PickOtherFontHeight (Led_Distance /*origHeight*/)
{
	Led_Assert (false);	// You must implement this yourself in your own subclass - or don't enable commands that call it.
	return 0;
}

bool	WordProcessor::DialogSupport::PickNewParagraphLineSpacing (Led_TWIPS* /*spaceBefore*/, bool* /*spaceBeforeValid*/, Led_TWIPS* /*spaceAfter*/, bool* /*spaceAfterValid*/, Led_LineSpacing* /*lineSpacing*/, bool* /*lineSpacingValid*/)
{
	Led_Assert (false);	// You must implement this yourself in your own subclass - or don't enable commands that call it.
	return false;
}

bool	WordProcessor::DialogSupport::PickNewParagraphMarginsAndFirstIndent (Led_TWIPS* /*leftMargin*/, bool* /*leftMarginValid*/, Led_TWIPS* /*rightMargin*/, bool* /*rightMarginValid*/, Led_TWIPS* /*firstIndent*/, bool* /*firstIndentValid*/)
{
	Led_Assert (false);	// You must implement this yourself in your own subclass - or don't enable commands that call it.
	return false;
}

Led_Color	WordProcessor::DialogSupport::FontCmdToColor (CommandNumber cmd)
{
	switch (cmd) {
		case	kFontColorBlack_CmdID:		return Led_Color::kBlack;
		case	kFontColorMaroon_CmdID:		return Led_Color::kMaroon;
		case	kFontColorGreen_CmdID:		return Led_Color::kGreen;
		case	kFontColorOlive_CmdID:		return Led_Color::kOlive;
		case	kFontColorNavy_CmdID:		return Led_Color::kNavyBlue;
		case	kFontColorPurple_CmdID:		return Led_Color::kPurple;
		case	kFontColorTeal_CmdID:		return Led_Color::kTeal;
		case	kFontColorGray_CmdID:		return Led_Color::kGray;
		case	kFontColorSilver_CmdID:		return Led_Color::kSilver;
		case	kFontColorRed_CmdID:		return Led_Color::kRed;
		case	kFontColorLime_CmdID:		return Led_Color::kLimeGreen;
		case	kFontColorYellow_CmdID:		return Led_Color::kYellow;
		case	kFontColorBlue_CmdID:		return Led_Color::kBlue;
		case	kFontColorFuchsia_CmdID:	return Led_Color::kFuchsia;
		case	kFontColorAqua_CmdID:		return Led_Color::kAqua;
		case	kFontColorWhite_CmdID:		return Led_Color::kWhite;
	}
	Led_Assert (false);						return Led_Color::kBlack;
}

WordProcessor::DialogSupport::CommandNumber	WordProcessor::DialogSupport::FontColorToCmd (Led_Color color)
{
	if (color == Led_Color::kBlack) {
		return kFontColorBlack_CmdID;
	}
	else if (color == Led_Color::kMaroon) {
		return kFontColorMaroon_CmdID;
	}
	else if (color == Led_Color::kGreen) {
		return kFontColorGreen_CmdID;
	}
	else if (color == Led_Color::kOlive) {
		return kFontColorOlive_CmdID;
	}
	else if (color == Led_Color::kNavyBlue) {
		return kFontColorNavy_CmdID;
	}
	else if (color == Led_Color::kPurple) {
		return kFontColorPurple_CmdID;
	}
	else if (color == Led_Color::kTeal) {
		return kFontColorTeal_CmdID;
	}
	else if (color == Led_Color::kGray) {
		return kFontColorGray_CmdID;
	}
	else if (color == Led_Color::kSilver) {
		return kFontColorSilver_CmdID;
	}
	else if (color == Led_Color::kRed) {
		return kFontColorRed_CmdID;
	}
	else if (color == Led_Color::kGreen) {
		return kFontColorLime_CmdID;
	}
	else if (color == Led_Color::kYellow) {
		return kFontColorYellow_CmdID;
	}
	else if (color == Led_Color::kBlue) {
		return kFontColorBlue_CmdID;
	}
	else if (color == Led_Color::kFuchsia) {
		return kFontColorFuchsia_CmdID;
	}
	else if (color == Led_Color::kAqua) {
		return kFontColorAqua_CmdID;
	}
	else if (color == Led_Color::kWhite) {
		return kFontColorWhite_CmdID;
	}
	else {
		// Now - double check. User could have overridden our FontCmdToColor () method to specify new commands.
		// catch those as well (at a slight performance cost)
		for (CommandNumber i = kBaseFontColor_CmdID; i <= kLastNamedFontColor_CmdID; ++i) {
			if (FontCmdToColor (i) == color) {
				return i;
			}
		}
		return kFontColorOther_CmdID;
	}
}

bool	WordProcessor::DialogSupport::PickOtherFontColor (Led_Color* color)
{
	Led_RequireNotNil (color);

	#if		qMacOS
		RGBColor	oldColor	=	color->GetOSRep ();
		RGBColor	newColor	=	oldColor;
		Point		where 		= { 0, 0};
		if (::GetColor (where, "\pPick new color", &oldColor, &newColor)) {
			*color = Led_Color (newColor);
			return true;
		}
	#elif	qWindows
		CHOOSECOLOR	cc;
		memset (&cc, 0, sizeof(cc));
		cc.lStructSize = sizeof (cc);
		cc.Flags |= CC_ANYCOLOR;
		cc.rgbResult = color->GetOSRep ();
		cc.Flags |= CC_RGBINIT;
		cc.Flags |= CC_FULLOPEN;

		cc.Flags |= CC_ENABLEHOOK;
		cc.lpfnHook = ColorPickerINITPROC;

		static	COLORREF	sCustomColors[16];
		cc.lpCustColors = sCustomColors;

		cc.hwndOwner = ::GetActiveWindow ();		// Not a great choice - but the best I can come up with from here...

		if (::ChooseColor (&cc)) {
			*color = Led_Color (cc.rgbResult);
			return true;
		}
	#endif
	return false;
}

#if		qWindows
UINT CALLBACK	WordProcessor::DialogSupport::ColorPickerINITPROC (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (hWnd != NULL and message == WM_INITDIALOG) {
		Led_CenterWindowInParent (hWnd);
	}
	return 0;
}
#endif

bool	WordProcessor::DialogSupport::ChooseFont (Led_IncrementalFontSpecification* font)
{
	Led_RequireNotNil (font);

	#if		qWindows
		// Copy each valid attribute into the LOGFONT to initialize the CFontDialog
		LOGFONT	lf;
		(void)::memset (&lf, 0, sizeof (lf));
		if (font->GetFontNameSpecifier_Valid ()) {
			(void)::_tcscpy (lf.lfFaceName, font->GetFontNameSpecifier ().fName);
			Led_Assert (::_tcslen (lf.lfFaceName) < Led_NEltsOf (lf.lfFaceName));	// cuz our cached entry - if valid - always short enuf...
		}
		lf.lfWeight = (font->GetStyle_Bold_Valid () and font->GetStyle_Bold ())? FW_BOLD: FW_NORMAL;
		lf.lfItalic = (font->GetStyle_Italic_Valid () and font->GetStyle_Italic ());
		lf.lfUnderline = (font->GetStyle_Underline_Valid () and font->GetStyle_Underline ());
		lf.lfStrikeOut = (font->GetStyle_Strikeout_Valid () and font->GetStyle_Strikeout ());

		if (font->GetPointSize_Valid ()) {
			lf.lfHeight = font->PeekAtTMHeight ();
		}

		CHOOSEFONT	cc;
		memset (&cc, 0, sizeof(cc));
		cc.lStructSize = sizeof (cc);
		cc.Flags |= CF_SCREENFONTS | CF_NOVERTFONTS | CF_EFFECTS | CF_SCALABLEONLY;

		cc.hwndOwner = ::GetActiveWindow ();		// Not a great choice - but the best I can come up with from here...

		cc.lpLogFont = &lf;
		cc.Flags |= CF_INITTOLOGFONTSTRUCT;

		if (font->GetTextColor_Valid ()) {
			cc.rgbColors = font->GetTextColor ().GetOSRep ();
		}

		if (::ChooseFont (&cc)) {
			*font	=	Led_FontSpecification (*cc.lpLogFont);
			font->SetTextColor (Led_Color (cc.rgbColors));
			return true;
		}
	#else
		Led_Arg_Unused (font);
	#endif
	return false;
}

void	WordProcessor::DialogSupport::ShowSimpleEmbeddingInfoDialog (const Led_SDK_String& /*embeddingTypeName*/)
{
	Led_Assert (false);	// You must implement this yourself in your own subclass - or don't enable commands that call it.
}

bool	WordProcessor::DialogSupport::ShowURLEmbeddingInfoDialog (const Led_SDK_String& /*embeddingTypeName*/, Led_SDK_String* /*urlTitle*/, Led_SDK_String* /*urlValue*/)
{
	Led_Assert (false);	// You must implement this yourself in your own subclass - or don't enable commands that call it.
	return false;
}

bool	WordProcessor::DialogSupport::ShowAddURLEmbeddingInfoDialog (Led_SDK_String* /*urlTitle*/, Led_SDK_String* /*urlValue*/)
{
	Led_Assert (false);	// You must implement this yourself in your own subclass - or don't enable commands that call it.
	return false;
}

bool	WordProcessor::DialogSupport::AddNewTableDialog (size_t* nRows, size_t* nCols)
{
	Led_RequireNotNil (nRows);
	Led_RequireNotNil (nCols);
	// In case no AddNewTable dialog implemented - just default to simple basic table
	*nRows = 3;
	*nCols = 4;
	return true;
}

bool	WordProcessor::DialogSupport::EditTablePropertiesDialog (TableSelectionPropertiesInfo* tableProperties)
{
	Led_RequireNotNil (tableProperties);
	return false;	// You must implement this yourself in your own subclass - or don't enable commands that call it.
}









/*
 ********************************************************************************
 ********************************* WordProcessor ********************************
 ********************************************************************************
 */
WordProcessor::CommandNames		WordProcessor::sCommandNames		=		WordProcessor::MakeDefaultCommandNames ();
WordProcessor::DialogSupport*	WordProcessor::sDialogSupport		=		NULL;


	template	<class	T, class EXTRACTOR>
		bool	CheckForCommonParaValue (EXTRACTOR /*INGORED_BUT_HERE_FOR_OVERLOADING*/, const WordProcessor::ParagraphDatabasePtr& paraDB, size_t from, size_t to, T* commonValue)
			{
				Led_RequireNotNil (commonValue);
				if (paraDB.IsNull ()) {
					throw WordProcessor::NoParagraphDatabaseAvailable ();
				}
				vector<pair<WordProcessor::ParagraphInfo,size_t> >	v	=	paraDB->GetParagraphInfo (from, to-from);
				Led_Assert (v.size () != 0);
				if (v.size () >= 1) {
					T	maybeCommonValue = EXTRACTOR () (v[0].first);
					for (vector<pair<WordProcessor::ParagraphInfo,size_t> >::const_iterator i = v.begin () + 1; i != v.end (); ++i) {
						if (EXTRACTOR () ((*i).first) != maybeCommonValue) {
							return false;
						}
					}
					*commonValue = maybeCommonValue;
					return true;
				}
				else {
					return false;
				}
			}
	struct	JustificationExtractor {
		Led_Justification operator () (const WordProcessor::ParagraphInfo& from)
			{
				return from.GetJustification ();
			}
	};
	struct	TabStopExtractor {
		TextImager::StandardTabStopList operator () (const WordProcessor::ParagraphInfo& from)
			{
				return from.GetTabStopList ();
			}
	};
	struct	FirstIndentExtractor {
		Led_TWIPS operator () (const WordProcessor::ParagraphInfo& from)
			{
				return from.GetFirstIndent ();
			}
	};
	struct	MarginsRec {
		MarginsRec (): fLHS (Led_TWIPS (0)), fRHS (Led_TWIPS (0)) {}
		MarginsRec (Led_TWIPS lhs, Led_TWIPS rhs): fLHS (lhs), fRHS (rhs) {}

		Led_TWIPS	fLHS;
		Led_TWIPS	fRHS;

		inline	bool operator!= (const MarginsRec& rhs)		{	return fLHS != rhs.fLHS or fRHS != rhs.fRHS; }
	};
	struct	MarginsRecExtractor {
		MarginsRec operator () (const WordProcessor::ParagraphInfo& from)
			{
				return MarginsRec (from.GetLeftMargin (), from.GetRightMargin ());
			}
	};
	struct	SpaceBeforeExtractor {
		Led_TWIPS operator () (const WordProcessor::ParagraphInfo& from)
			{
				return from.GetSpaceBefore ();
			}
	};
	struct	SpaceAfterExtractor {
		Led_TWIPS operator () (const WordProcessor::ParagraphInfo& from)
			{
				return from.GetSpaceAfter ();
			}
	};
	struct	LineSpacingExtractor {
		Led_LineSpacing operator () (const WordProcessor::ParagraphInfo& from)
			{
				return from.GetLineSpacing ();
			}
	};
	struct	ListStyleExtractor {
		ListStyle operator () (const WordProcessor::ParagraphInfo& from)
			{
				return from.GetListStyle ();
			}
	};
	struct	ListIndentLevelExtractor {
		unsigned char operator () (const WordProcessor::ParagraphInfo& from)
			{
				return from.GetListIndentLevel ();
			}
	};


const	Led_TWIPS	WordProcessor::kBadCachedFarthestRightMarginInDocument	=	Led_TWIPS (-1);

WordProcessor::WordProcessor ():
	inherited (),
#if		qWideCharacters
	fSmartQuoteMode (true),
#endif
	fParagraphDatabase (NULL),
	fICreatedParaDB (false),
	fHidableTextDatabase (NULL),
	fICreatedHidableTextDB (false),
	fWPIdler (),
	fCachedCurSelFontSpec (),
	fCachedCurSelJustification (eLeftJustify),
	fCachedCurSelJustificationUnique (false),
	fCachedCurSelFontSpecValid (false),
	fShowParagraphGlyphs (false),
	fShowTabGlyphs (false),
	fShowSpaceGlyphs (false)
{
	fWPIdler.fWP = this;
	IdleManager::Get ().AddIdler (&fWPIdler);
	IdleManager::Get ().SetIdlerFrequncy (&fWPIdler, 0.25f);
}

WordProcessor::~WordProcessor ()
{
	IdleManager::Get ().RemoveIdler (&fWPIdler);
}

void	WordProcessor::HookLosingTextStore ()
{
	HookLosingTextStore_ ();
	inherited::HookLosingTextStore ();
}

void	WordProcessor::HookLosingTextStore_ ()
{
	{
		MarkersOfATypeMarkerSink2Vector<Table>	tables;
		GetTextStore ().CollectAllMarkersInRangeInto (GetTextStore ().GetStart (), GetTextStore ().GetEnd (), this, tables);
		#if		qConstNonConstPtrConversionsWithTemplatedMemberFunctionBug
			Table**			t	=	&*tables.fResultArray.begin ();
			Table*const*	tt	=	t;
			GetTextStore ().RemoveAndDeleteMarkers (tt, tables.fResult.size ());
		#else
			GetTextStore ().RemoveAndDeleteMarkers (&*tables.fResult.begin (), tables.fResult.size ());
		#endif
	}

	// NB: We only set the fParagraphDatabase/fHidableTextDatabase to NULL here if we created it because if the USER
	// created it - its up to THEM to properly bind it to the right TextStore. And when we are told to use a particular
	// database, someones call to change our TextStore shouldn't make us ignore that earlier request (to use a particular database).
	//		We bother to delete it here if we HAD created it ourselves - only because the TextStore WE created it with
	//	could be destroyed after this call returns - and then we'd have a database with a bogus pointer to a TextStore.
	if (fICreatedParaDB) {
		fICreatedParaDB = false;
		if (not fParagraphDatabase.IsNull ()) {
			fParagraphDatabase = NULL;	// Cannot call WordProcessor::SetParagraphDatabase (NULL) cuz that might build a NEW one
			HookParagraphDatabaseChanged ();
		}
	}
	if (fICreatedHidableTextDB) {
		SetHidableTextDatabase (NULL);
		fICreatedHidableTextDB = false;
	}
	//to try to avoid circular links that cause things to not get freed. - LGP 2000/04/24
	if (not fHidableTextDatabase.IsNull ()) {
		fHidableTextDatabase->SetInternalizer (NULL);
		fHidableTextDatabase->SetExternalizer (NULL);
	}
}

void	WordProcessor::HookGainedNewTextStore ()
{
	/*
	 *	Note - we must check if the ParagraphDatabase has already been set - and use its Partition. Do this before
	 *	calling inherited::HookGainedNewTextStore () to avoid redundant creation of a 'default' partition (speed tweek).
	 */
	if (not fParagraphDatabase.IsNull ()) {
		SetPartition (fParagraphDatabase->GetPartition ());
	}
	inherited::HookGainedNewTextStore ();
	HookGainedNewTextStore_ ();
}

void	WordProcessor::HookGainedNewTextStore_ ()
{
	if (fParagraphDatabase.IsNull ()) {
		SetParagraphDatabase (NULL);	// fills in default value since we have e textstore...
	}
	if (fHidableTextDatabase.IsNull ()) {
		SetHidableTextDatabase (new UniformHidableTextMarkerOwner (GetTextStore ()));	// fills in default value since we have e textstore...
		fICreatedHidableTextDB = true;													// do this AFTER above call - cuz WordProcessor::SetHidableTextDatabase () sets flag FALSE (so for case when others call it)
	}
}

PartitioningTextImager::PartitionPtr	WordProcessor::MakeDefaultPartition () const
{
// Probably no point in overriding this anymore - LGP 2002-10-20 -- RETHINK??? Perhaps no harm - either...
	Led_RequireNotNil (PeekAtTextStore ());
	if (fParagraphDatabase.IsNull ()) {
		return new LineBasedPartition (GetTextStore ());
	}
	else {
		const MarkerOwner*	mo	=	fParagraphDatabase;
		return (new WPPartition (GetTextStore (), *const_cast<MarkerOwner*> (mo)));
	}
}

/*
@METHOD:		WordProcessor::SetParagraphDatabase
@DESCRIPTION:	<p>This method allows the caller to specify the database of paragraph information associated
	with the given word processor. If not called, a default will be used, and automatically deleted.</p>
		<p>This API exists so that you can share a single database @'WordProcessor::ParagraphDatabasePtr'
	with multiple views. And so you can save it associated with a document (or some such object), and dynamically
	create/destroy views using that data. Also - so you can subclass it, and provide your own virtual replacement
	database.</p>
*/
void	WordProcessor::SetParagraphDatabase (const ParagraphDatabasePtr& paragraphDatabase)
{
	fParagraphDatabase = paragraphDatabase;
	fICreatedParaDB = false;
	if (fParagraphDatabase.IsNull () and PeekAtTextStore () != NULL) {
		fParagraphDatabase = new ParagraphDatabaseRep (GetTextStore ());
		fICreatedParaDB = true;
	}
	//Any newly assigned fParagraphDatabase better share the same Partition we do!
	HookParagraphDatabaseChanged ();
}

/*
@METHOD:		WordProcessor::HookParagraphDatabaseChanged
@DESCRIPTION:	<p>Called whenever the @'WordProcessor::ParagraphDatabasePtr' associated with this @'WordProcessor'
	is changed. This means when a new one is provided, created, or disassociated. It does NOT mean that its called when any of the
	data in the paragphrase database changes.</p>
		<p>Usually called by @'WordProcessor::SetParagraphDatabase'. By default, it calls @'WordProcessor::HookParagraphDatabaseChanged_'.</p>
*/
void	WordProcessor::HookParagraphDatabaseChanged ()
{
	if (PeekAtTextStore () != NULL) {
		HookParagraphDatabaseChanged_ ();
	}
}

/*
@METHOD:		WordProcessor::HookParagraphDatabaseChanged_
@DESCRIPTION:	<p>Default implementation of @'WordProcessor::HookParagraphDatabaseChanged'.</p>
*/
void	WordProcessor::HookParagraphDatabaseChanged_ ()
{
	/*
	 *	At LEAST by default - we want the paragraphDB's partition to be the same as the one our imager is using. Which
	 *	should be preferred (ie which way do we do the copy?)? Since you can have multiple imagers associated with a single
	 *	ParagraphDatabase, and since you can operate on a paragraphdatabase without a WP/imager (say with a document),
	 *	it makes sense to assume THAT is primary. -- LGP 2002-10-20
	 */
	if (not fParagraphDatabase.IsNull ()) {
		SetPartition (fParagraphDatabase->GetPartition ());
	}
	SetExternalizer (MakeDefaultExternalizer ());
	SetInternalizer (MakeDefaultInternalizer ());
}

/*
@METHOD:		WordProcessor::SetHidableTextDatabase
@DESCRIPTION:	<p>This method allows the caller to specify the database of hidden-text information associated
	with the given word processor. If not called, a default will be used, and automatically deleted.</p>
		<p>This API exists so that you can share a single database @'WordProcessor::HidableTextDatabasePtr'
	with multiple views. And so you can save it associated with a document (or some such object), and dynamically
	create/destroy views using that data. Also - so you can subclass it, and provide your own virtual replacement
	database, or other subclass of the hidable text API.</p>
		<p>To disable hidden text support, just call this method from your @'TextImager::HookGainedNewTextStore ()' override,
	and pass NULL. Do this after the  @'WordProcessor::HookGainedNewTextStore ()' override - since that method
	will create one of these by default.</p>
*/
void	WordProcessor::SetHidableTextDatabase (const HidableTextDatabasePtr& hidableTextDatabase)
{
	//to try to avoid circular links that cause things to not get freed. - LGP 2000/04/24
	if (not fHidableTextDatabase.IsNull ()) {
		fHidableTextDatabase->SetInternalizer (NULL);
		fHidableTextDatabase->SetExternalizer (NULL);
	}

	fHidableTextDatabase = hidableTextDatabase;
	fICreatedHidableTextDB = false;
	HookHidableTextDatabaseChanged ();
}

/*
@METHOD:		WordProcessor::HookHidableTextDatabaseChanged
@DESCRIPTION:	<p>Called whenever the @'WordProcessor::HidableTextDatabasePtr' associated with this @'WordProcessor'
	is changed. This means when a new one is provided, created, or disassociated. It does NOT mean that its called when any of the
	data in the hidable text database changes.</p>
		<p>Usually called by @'WordProcessor::SetHidableTextDatabase'. By default, it calls @'WordProcessor::HookHidableTextDatabaseChanged_'.</p>
*/
void	WordProcessor::HookHidableTextDatabaseChanged ()
{
	HookHidableTextDatabaseChanged_ ();
}

/*
@METHOD:		WordProcessor::HookHidableTextDatabaseChanged_
@DESCRIPTION:	<p>Default implementation of @'WordProcessor::HookHidableTextDatabaseChanged'. Assures that when we change the hidableText database,
	we re-create the our internalizer and externalizers (cuz those can depend on the hidden text database). And be sure to notify any
	newly created hidable text database of our current internalizer and externalizer (the reverse).</p>
*/
void	WordProcessor::HookHidableTextDatabaseChanged_ ()
{
	if (PeekAtTextStore () != NULL) {
		SetExternalizer (MakeDefaultExternalizer ());
		SetInternalizer (MakeDefaultInternalizer ());
	}
}

Led_RefCntPtr<FlavorPackageInternalizer>	WordProcessor::MakeDefaultInternalizer ()
{
	return new WordProcessorFlavorPackageInternalizer (GetTextStore (), GetStyleDatabase (), GetParagraphDatabase (), GetHidableTextDatabase ());
}

Led_RefCntPtr<FlavorPackageExternalizer>	WordProcessor::MakeDefaultExternalizer ()
{
	return new WordProcessorFlavorPackageExternalizer (GetTextStore (), GetStyleDatabase (), GetParagraphDatabase (), GetHidableTextDatabase ());
}

/*
@METHOD:		WordProcessor::HookInternalizerChanged
@DESCRIPTION:	<p>Override @TextInteractor::HookInternalizerChanged' to sync up with our HidableText database.</p>
*/
void	WordProcessor::HookInternalizerChanged ()
{
	inherited::HookInternalizerChanged ();
	if (not fHidableTextDatabase.IsNull ()) {
		fHidableTextDatabase->SetInternalizer (GetInternalizer ());
	}
}

/*
@METHOD:		WordProcessor::HookExternalizerChanged
@DESCRIPTION:	<p>Override @TextInteractor::HookExternalizerChanged' to sync up with our HidableText database.</p>
*/
void	WordProcessor::HookExternalizerChanged ()
{
	inherited::HookExternalizerChanged ();
	if (not fHidableTextDatabase.IsNull ()) {
		fHidableTextDatabase->SetExternalizer (GetExternalizer ());
	}
}


/*
@METHOD:		WordProcessor::InternalizeBestFlavor
@DESCRIPTION:	<p>Override @'TextInteractor::InternalizeBestFlavor' and set the internalizer (and so the source stream)
			to overwrite mode.</p>
*/
void	WordProcessor::InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage,
													bool updateCursorPosition, bool autoScroll, UpdateMode updateMode
												)
{
	Table*	t	=	GetActiveTable ();
	if (t != NULL) {
		WordProcessorFlavorPackageInternalizer*	internalizerRep	=
								dynamic_cast<WordProcessorFlavorPackageInternalizer*> (
										static_cast<FlavorPackageInternalizer*> (GetInternalizer ())
									);
		Led_AssertNotNil (internalizerRep);

		bool	oldFlagVal	=	internalizerRep->GetOverwriteTableMode ();
		internalizerRep->SetOverwriteTableMode (true);
		try {
			size_t	selEnd		=	GetSelectionEnd ();
			Led_Assert (selEnd - GetSelectionStart () == 1);		// cuz GetActiveTable should assure this

			// pass in ONLY selEnd to selEnd (not selStart to selEnd) because with pastes (or D&D) into a
			// selected table - we DONT want to delete first (replace) the table itself
			bool	good	=	GetInternalizer ()->InternalizeBestFlavor (flavorPackage, selEnd, selEnd);
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
			internalizerRep->SetOverwriteTableMode (oldFlagVal);
		}
		catch (...) {
			internalizerRep->SetOverwriteTableMode (oldFlagVal);
			throw;
		}
	}
	else {
		inherited::InternalizeBestFlavor (flavorPackage, updateCursorPosition, autoScroll, updateMode);
	}
}

/*
@METHOD:		WordProcessor::ExternalizeFlavors
@DESCRIPTION:	<p>Override @'TextInteractor::ExternalizeFlavors' but also restrict table externalizing to just
			the selected portion.</p>
*/
void	WordProcessor::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
	WordProcessorFlavorPackageExternalizer*	externalizerRep	=
							dynamic_cast<WordProcessorFlavorPackageExternalizer*> (
									static_cast<FlavorPackageExternalizer*> (GetExternalizer ())
								);
	Led_AssertNotNil (externalizerRep);

	bool	oldFlagVal	=	externalizerRep->GetUseTableSelection ();
	externalizerRep->SetUseTableSelection (true);
	try {
		inherited::ExternalizeFlavors (flavorPackage);
		externalizerRep->SetUseTableSelection (oldFlagVal);
	}
	catch (...) {
		externalizerRep->SetUseTableSelection (oldFlagVal);
		throw;
	}
}

/*
@METHOD:		WordProcessor::InterectiveSetRegionHidable
@DESCRIPTION:	<p>Interactively set the given region to be hidable or not. Interactively means that the action
	is considered an undoable command.</p>
*/
void	WordProcessor::InterectiveSetRegionHidable (bool hidable)
{
	Led_RequireNotNil (PeekAtTextStore ());	// Must specify TextStore before calling this, or any routine that calls it.

	BreakInGroupedCommands ();

	UndoableContextHelper	undoContext (*this, hidable?
													GetCommandNames ().fHideCommandName:
													GetCommandNames ().fUnHideCommandName, 
												false
											);
		{
			if (hidable) {
				GetHidableTextDatabase ()->MakeRegionHidable (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd ());
			}
			else {
				GetHidableTextDatabase ()->MakeRegionUnHidable (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd ());
			}
		}
	undoContext.CommandComplete ();
}

/*
@METHOD:		WordProcessor::GetJustification
@DESCRIPTION:
	<p>Return the @'Led_Justification' setting for the paragraph containing the character characterPos</p>
*/
Led_Justification	WordProcessor::GetJustification (size_t characterPos) const
{
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	return fParagraphDatabase->GetParagraphInfo (characterPos).GetJustification ();
}

/*
@METHOD:		WordProcessor::GetJustification
@DESCRIPTION:
	<p>Return true iff there is a unique answer, and only then to we set out justification param (@'Led_Justification').</p>
*/
bool	WordProcessor::GetJustification (size_t from, size_t to, Led_Justification* justification) const
{
	Led_RequireNotNil (justification);
	return CheckForCommonParaValue (JustificationExtractor (), fParagraphDatabase, from, to, justification);
}

/*
@METHOD:		WordProcessor::SetJustification
@DESCRIPTION:	<p>Set the justification to <code>justification</code> for all paragraphs
	between <code>from</code> and <code>to</code>.</p>
*/
void	WordProcessor::SetJustification (size_t from, size_t to, Led_Justification justification)
{
	Led_Require (from <= to);
	IncrementalParagraphInfo pi;
	pi.SetJustification (justification);
	fParagraphDatabase->SetParagraphInfo (from, to-from, pi);
}

TextImager::StandardTabStopList	WordProcessor::GetDefaultStandardTabStopList ()
{
	return StandardTabStopList ();
}

/*
@METHOD:		WordProcessor::GetStandardTabStopList
@DESCRIPTION:
	<p>Return the tabstops list setting for the paragraph containing the character characterPos</p>
*/
TextImager::StandardTabStopList	WordProcessor::GetStandardTabStopList (size_t characterPos) const
{
	return fParagraphDatabase->GetParagraphInfo (characterPos).GetTabStopList ();
}

/*
@METHOD:		WordProcessor::GetStandardTabStopList
@DESCRIPTION:
	<p>Return true iff there is a unique answer, and only then to we set out <code>tabStops</code> param</p>
*/
bool	WordProcessor::GetStandardTabStopList (size_t from, size_t to, StandardTabStopList* tabStops) const
{
	Led_RequireNotNil (tabStops);
	return CheckForCommonParaValue (TabStopExtractor (), fParagraphDatabase, from, to, tabStops);
}

/*
@METHOD:		WordProcessor::SetStandardTabStopList
@DESCRIPTION:	<p>Set the tabstops to <code>tabStops</code> for all paragraphs
	between <code>from</code> and <code>to</code>.</p>
*/
void	WordProcessor::SetStandardTabStopList (size_t from, size_t to, StandardTabStopList tabStops)
{
	Led_Require (from <= to);
	IncrementalParagraphInfo pi;
	pi.SetTabStopList (tabStops);
	fParagraphDatabase->SetParagraphInfo (from, to-from, pi);
}

/*
@METHOD:		WordProcessor::GetMargins
@DESCRIPTION:
	<p>Return the left and right margin settings for the paragraph containing the character characterPos</p>
*/
void	WordProcessor::GetMargins (size_t characterPos, Led_TWIPS* leftMargin, Led_TWIPS* rightMargin) const
{
	Led_RequireNotNil (leftMargin);
	Led_RequireNotNil (rightMargin);
	const ParagraphInfo&	pi	=	fParagraphDatabase->GetParagraphInfo (characterPos);	// Be careful holding onto reference here.
																							// I do so cuz its a big optimization in my MacOS
																							// profiling, as it avoids CTOR/DTOR for vector of
																							// tabstops (SPR#1029)
	*leftMargin = pi.GetLeftMargin ();
	*rightMargin = pi.GetRightMargin ();
}

bool	WordProcessor::GetMargins (size_t from, size_t to, Led_TWIPS* leftMargin, Led_TWIPS* rightMargin) const
{
	Led_RequireNotNil (leftMargin);
	Led_RequireNotNil (rightMargin);
	MarginsRec mrResult;
	bool	result	= CheckForCommonParaValue (MarginsRecExtractor (), fParagraphDatabase, from, to, &mrResult);
	*leftMargin = mrResult.fLHS;
	*rightMargin = mrResult.fRHS;
	return result;
}

/*
@METHOD:		WordProcessor::SetMargins
@DESCRIPTION:	<p>See @'WordProcessor::GetMargins'.</p>
*/
void	WordProcessor::SetMargins (size_t from, size_t to, Led_TWIPS leftMargin, Led_TWIPS rightMargin)
{
	Led_Require (from <= to);
	IncrementalParagraphInfo pi;
	pi.SetMargins (leftMargin, rightMargin);
	fParagraphDatabase->SetParagraphInfo (from, to-from, pi);
}

/*
@METHOD:		WordProcessor::GetFirstIndent
@DESCRIPTION:	<p>Get the 'first indent' property for the paragraph containing the
			given character position.</p>
*/
Led_TWIPS	WordProcessor::GetFirstIndent (size_t characterPos) const
{
	return fParagraphDatabase->GetParagraphInfo (characterPos).GetFirstIndent ();
}

/*
@METHOD:		WordProcessor::GetFirstIndent
@DESCRIPTION:	<p>Get the 'first indent' property for the paragraphs bounded by the given range, if it is
			unique over that range, and return true. If it is not unqique over that range, return false.</p>
*/
bool	WordProcessor::GetFirstIndent (size_t from, size_t to, Led_TWIPS* firstIndent) const
{
	Led_RequireNotNil (firstIndent);
	return CheckForCommonParaValue (FirstIndentExtractor (), fParagraphDatabase, from, to, firstIndent);
}

/*
@METHOD:		WordProcessor::SetFirstIndent
@DESCRIPTION:
*/
void	WordProcessor::SetFirstIndent (size_t from, size_t to, Led_TWIPS firstIndent)
{
	Led_Require (from <= to);
	IncrementalParagraphInfo pi;
	pi.SetFirstIndent (firstIndent);
	fParagraphDatabase->SetParagraphInfo (from, to-from, pi);
}

/*
@METHOD:		WordProcessor::GetSpaceBefore
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetSpaceBefore'</p>
*/
Led_TWIPS	WordProcessor::GetSpaceBefore (size_t characterPos) const
{
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	return fParagraphDatabase->GetParagraphInfo (characterPos).GetSpaceBefore ();
}

/*
@METHOD:		WordProcessor::GetSpaceBefore
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetSpaceBefore'</p>
*/
bool	WordProcessor::GetSpaceBefore (size_t from, size_t to, Led_TWIPS* sb) const
{
	Led_RequireNotNil (sb);
	return CheckForCommonParaValue (SpaceBeforeExtractor (), fParagraphDatabase, from, to, sb);
}

/*
@METHOD:		WordProcessor::SetSpaceBefore
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetSpaceBefore'</p>
*/
void	WordProcessor::SetSpaceBefore (size_t from, size_t to, Led_TWIPS sb)
{
	Led_Require (from <= to);
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	IncrementalParagraphInfo pi;
	pi.SetSpaceBefore (sb);
	fParagraphDatabase->SetParagraphInfo (from, to-from, pi);
}

/*
@METHOD:		WordProcessor::GetSpaceAfter
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetSpaceAfter'</p>
*/
Led_TWIPS	WordProcessor::GetSpaceAfter (size_t characterPos) const
{
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	return fParagraphDatabase->GetParagraphInfo (characterPos).GetSpaceAfter ();
}

/*
@METHOD:		WordProcessor::GetSpaceAfter
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetSpaceAfter'</p>
*/
bool	WordProcessor::GetSpaceAfter (size_t from, size_t to, Led_TWIPS* sa) const
{
	Led_RequireNotNil (sa);
	return CheckForCommonParaValue (SpaceAfterExtractor (), fParagraphDatabase, from, to, sa);
}

/*
@METHOD:		WordProcessor::SetSpaceAfter
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetSpaceAfter'</p>
*/
void	WordProcessor::SetSpaceAfter (size_t from, size_t to, Led_TWIPS sa)
{
	Led_Require (from <= to);
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	IncrementalParagraphInfo pi;
	pi.SetSpaceAfter (sa);
	fParagraphDatabase->SetParagraphInfo (from, to-from, pi);
}

/*
@METHOD:		WordProcessor::GetLineSpacing
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetLineSpacing'</p>
*/
Led_LineSpacing	WordProcessor::GetLineSpacing (size_t characterPos) const
{
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	return fParagraphDatabase->GetParagraphInfo (characterPos).GetLineSpacing ();
}

/*
@METHOD:		WordProcessor::GetLineSpacing
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetLineSpacing'</p>
*/
bool	WordProcessor::GetLineSpacing (size_t from, size_t to, Led_LineSpacing* sl) const
{
	Led_RequireNotNil (sl);
	return CheckForCommonParaValue (LineSpacingExtractor (), fParagraphDatabase, from, to, sl);
}

/*
@METHOD:		WordProcessor::SetLineSpacing
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::SetLineSpacing'</p>
*/
void	WordProcessor::SetLineSpacing (size_t from, size_t to, Led_LineSpacing sl)
{
	Led_Require (from <= to);
	IncrementalParagraphInfo pi;
	pi.SetLineSpacing (sl);
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	fParagraphDatabase->SetParagraphInfo (from, to-from, pi);
}

/*
@METHOD:		WordProcessor::GetListStyle
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetListStyle'</p>
*/
ListStyle	WordProcessor::GetListStyle (size_t characterPos) const
{
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	return fParagraphDatabase->GetParagraphInfo (characterPos).GetListStyle ();
}

/*
@METHOD:		WordProcessor::GetListStyle
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetListStyle'</p>
*/
bool	WordProcessor::GetListStyle (size_t from, size_t to, ListStyle* listStyle) const
{
	Led_RequireNotNil (listStyle);
	return CheckForCommonParaValue (ListStyleExtractor (), fParagraphDatabase, from, to, listStyle);
}

/*
@METHOD:		WordProcessor::SetListStyle
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::SetListStyle'</p>
*/
void	WordProcessor::SetListStyle (size_t from, size_t to, ListStyle listStyle, bool autoFormat)
{
	Led_Require (from <= to);
	IncrementalParagraphInfo pi;
	pi.SetListStyle (listStyle);
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	fParagraphDatabase->SetParagraphInfo (from, to-from, pi);

	if (autoFormat) {
		AutoFormatIndentedText (from, to);
	}
}

/*
@METHOD:		WordProcessor::GetListIndentLevel
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetListIndentLevel'</p>
*/
unsigned char	WordProcessor::GetListIndentLevel (size_t characterPos) const
{
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	return fParagraphDatabase->GetParagraphInfo (characterPos).GetListIndentLevel ();
}

/*
@METHOD:		WordProcessor::GetListIndentLevel
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::GetListIndentLevel'</p>
*/
bool	WordProcessor::GetListIndentLevel (size_t from, size_t to, unsigned char* indentLevel) const
{
	Led_RequireNotNil (indentLevel);
	return CheckForCommonParaValue (ListIndentLevelExtractor (), fParagraphDatabase, from, to, indentLevel);
}

/*
@METHOD:		WordProcessor::SetListIndentLevel
@DESCRIPTION:
	<p>See @'WordProcessor::ParagraphInfo::SetListIndentLevel'</p>
*/
void	WordProcessor::SetListIndentLevel (size_t from, size_t to, unsigned char indentLevel, bool autoFormat)
{
	Led_Require (from <= to);
	IncrementalParagraphInfo pi;
	pi.SetListIndentLevel (indentLevel);
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	fParagraphDatabase->SetParagraphInfo (from, to-from, pi);
	if (autoFormat) {
		AutoFormatIndentedText (from, to);
	}
}

/*
@METHOD:		WordProcessor::AutoFormatIndentedText
@DESCRIPTION:
	<p>Examine each paragraph from 'from' to 'to', and set their various paragraph properties to fit their indent
	level. If I supported style sheets, this would be a natural place to use them (just applying a predefined
	style sheet).</p>
*/
void	WordProcessor::AutoFormatIndentedText (size_t from, size_t to)
{
	for (PartitionMarker* pm = GetPartitionMarkerContainingPosition (from); pm != NULL and pm->GetStart () <= to; pm = pm->GetNext ()) {
		Led_AssertNotNil (pm);
		ParagraphInfo	pi	=	fParagraphDatabase->GetParagraphInfo (pm->GetStart ());
		IncrementalParagraphInfo	newPI;
		if (pi.GetListStyle () == eListStyle_None) {
			newPI.SetFirstIndent (Led_TWIPS (0));
			newPI.SetMargins (Led_TWIPS (0), pi.GetRightMargin ());
			newPI.SetTabStopList (StandardTabStopList ());
		}
		else {
			const	int	kTWIPSIncrement	=	1440 / 4;
			Led_TWIPS	marginAt		=	Led_TWIPS (kTWIPSIncrement * 2 * (pi.GetListIndentLevel () + 1));
			newPI.SetFirstIndent (Led_TWIPS (-kTWIPSIncrement));
			newPI.SetMargins (marginAt, pi.GetRightMargin ());
			StandardTabStopList	tabStops;
			tabStops.fTabStops.push_back (marginAt);
			newPI.SetTabStopList (tabStops);
		}
		fParagraphDatabase->SetParagraphInfo (pm->GetStart (), pm->GetLength (), newPI);
	}
}

/*
@METHOD:		WordProcessor::SetSelection
@DESCRIPTION:	<p>Override @'TextImager::SetSelection' to handle updating selection of embedded tables.</p>
*/
void	WordProcessor::SetSelection (size_t start, size_t end)
{
	size_t	oldSelStart	=	0;
	size_t	oldSelEnd	=	0;
	GetSelection (&oldSelStart, &oldSelEnd);
	inherited::SetSelection (start, end);

	// For the area we have ADDED to the selection region, we must set the embedded tables in that region to be
	// fully selected. Note that we need not worry about the selection range within an UNSELECTED table
	// because that is ignored, and its forcibly reset upon new selection (or at least should be) - LGP 2003-03-17
	if (oldSelStart != start or oldSelEnd != end) {
		vector<Table*>	tables;
		size_t			checkRangeStart1	=	start;
		size_t			checkRangeEnd1		=	oldSelStart;
		if (checkRangeStart1 < checkRangeEnd1) {
			tables = GetTablesInRange (checkRangeStart1, checkRangeEnd1);
		}

		size_t			checkRangeStart2	=	FindPreviousCharacter (oldSelEnd);	// back one to handle the case where we had one char selected
		size_t			checkRangeEnd2		=	end;
		if (checkRangeStart2 < checkRangeEnd2) {
			vector<Table*>	tables2	=	GetTablesInRange (checkRangeStart2, checkRangeEnd2);
			tables.insert (tables.end (), tables2.begin (), tables2.end ());	// append the vectors
		}
		for (vector<Table*>::iterator i = tables.begin (); i != tables.end (); ++i) {
			Table*	t	=	*i;
			Table::TemporarilySetOwningWP	owningWPSetter (*t, *const_cast<WordProcessor*> (this));
			t->SetCellSelection (0, t->GetRowCount (), 0, t->GetColumnCount ());
		}
	}

	fCachedCurSelFontSpecValid = false;
}

/*
@METHOD:		WordProcessor::GetCaretShownSituation
@DESCRIPTION:	<p>Override @'TextInteractor::GetCaretShownSituation' to handle tables.</p>
*/
bool	WordProcessor::GetCaretShownSituation () const
{
	if (inherited::GetCaretShownSituation ()) {
		return true;
	}

	Table*	table = GetActiveTable ();
	if (table != NULL) {
		return table->GetCaretShownSituation ();
	}
	return false;
}

/*
@METHOD:		WordProcessor::CalculateCaretRect
@DESCRIPTION:	<p>Override @'TextInteractor::CalculateCaretRect' to handle tables.</p>
*/
Led_Rect	WordProcessor::CalculateCaretRect () const
{
	Table*	table = GetActiveTable ();
	if (table != NULL) {
		Table::TemporarilySetOwningWP	owningWPSetter (*table, *const_cast<WordProcessor*> (this));
		return table->CalculateCaretRect ();
	}
	return inherited::CalculateCaretRect ();
}

/*
@METHOD:		WordProcessor::OnTypedNormalCharacter
@DESCRIPTION:	<p>Override @'TextInteractor::OnTypedNormalCharacter' to handle smart quotes
			(@'WordProcessor::GetSmartQuoteMode'), tab/shift-tab indents (lists) and tables.</p>
*/
void	WordProcessor::OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed)
{
	Table*	table = GetActiveTable ();
	if (table != NULL) {
		Table::TemporarilySetOwningWP	owningWPSetter (*table, *const_cast<WordProcessor*> (this));
		if (table->OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed)) {
			return;
		}
	}

	// if the entire region is in the 'list' style - then assume a tab is meant to INDENT or UNINDENT
	if (theChar == '\t' and not (optionPressed or commandPressed or controlPressed or altKeyPressed)) {
		ListStyle	ls	=	eListStyle_None;
		if (GetListStyle (GetSelectionStart (), GetSelectionEnd (), &ls) and ls != eListStyle_None) {
			InteractiveDoIndentChange (not shiftPressed);
			return;
		}
	}
#if		qWideCharacters
	if (theChar == '"' and
		GetSmartQuoteMode () and
		not (optionPressed or commandPressed or controlPressed or altKeyPressed)
		) {
		const	wchar_t	kSpecialOpenQuote	=	8220;
		const	wchar_t	kSpecialCloseQuote	=	8221;
		bool	isAQuoteToClose	=	false;
		size_t	selStart		=	GetSelectionStart ();
		{
			// Walk backwards and see if we can find a recent OPEN-quote
			const size_t	kScanBackSize	=	1024;
			size_t			scanBackTo	=	static_cast<size_t> (max (0, static_cast<int> (selStart)-static_cast<int>(kScanBackSize)));
			Led_SmallStackBuffer<Led_tChar>	buf (kScanBackSize);
			size_t			scanBackCount	=	selStart - scanBackTo;
			CopyOut (scanBackTo, scanBackCount, buf);
			for (size_t i = scanBackCount; i != 0; --i) {
				if (buf[i-1] == kSpecialCloseQuote) {
					// then last thing was a close quote - so next is OPEN
					break;
				}
				else if (buf[i-1] == kSpecialOpenQuote) {
					isAQuoteToClose = true;
					break;
				}
			}
		}
		wchar_t	quoteChar	=	isAQuoteToClose? kSpecialCloseQuote: kSpecialOpenQuote;
		inherited::OnTypedNormalCharacter (quoteChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
	}
	else
#endif
	{
		inherited::OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);
	}
}

bool	WordProcessor::ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor)
{
	Led_RequireNotNil (dragAnchor);
	size_t		clickedOnChar	=	GetCharAtWindowLocation (clickedAt);
	Led_Rect	charRect		=	GetCharWindowLocation (clickedOnChar);

	// Only if click is on an embedding character cell, and fully within it (not in case just past it as at when at
	// end of line) - then we look at if it needs special processing
	//
	// Actually - better to check that the click isn't too near the edges of the embedding,
	// cuz then its hard to click and make an insertion point in between two embeddings.
	// So only do this click-selects somewhere near the middle of the embedding.
	Led_Rect	tstClickRect	=	charRect;
	const	Led_Distance	kHMargin	=	3;
	tstClickRect.left += kHMargin;
	tstClickRect.right -= kHMargin;
	if (tstClickRect.Contains (clickedAt)) {
		vector<Table*>	tables	=	GetTablesInRange (clickedOnChar, clickedOnChar + 1);
		Led_Assert (tables.size () == 0 or tables.size () == 1);
		if (tables.size () == 1) {
			Table*	t	=	tables[0];
			Table::TemporarilySetOwningWP	owningWPSetter (*t, *const_cast<WordProcessor*> (this));
			if (clickCount == 1) {
				// In this case - it really doesn't matter if we pick the LHS or RHS of the embedding
				// as the drag anchor...
				*dragAnchor = clickedOnChar;
			}
			return t->ProcessSimpleClick (clickedAt - charRect.GetOrigin (), clickCount, extendSelection);
		}
	}
	return inherited::ProcessSimpleClick (clickedAt, clickCount, extendSelection, dragAnchor);
}

/*
@METHOD:		WordProcessor::WhileSimpleMouseTracking
@DESCRIPTION:	<p>Override @'TextInteractor::WhileSimpleMouseTracking' to handle tables.</p>
*/
void	WordProcessor::WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor)
{
	size_t	clickedOnChar	=	GetCharAtWindowLocation (newMousePos);
	size_t	oldSelStart		=	GetSelectionStart ();
	size_t	oldSelEnd		=	GetSelectionEnd ();

	/*
	 *	If the drag anchor is coincident with the LHS or RHS of the clicked on character and the selection length
	 *	is one (we clicked on an embedding) - then just eat that mousetracking - and prevent the selection from
	 *	changing.
	 */
	if ((clickedOnChar == dragAnchor or clickedOnChar+1 == dragAnchor) and (oldSelEnd - oldSelStart == 1)) {
		vector<Table*>	tables	=	GetTablesInRange (clickedOnChar, clickedOnChar + 1);
		if (tables.size () == 1) {
			Table*		t				=	tables[0];
			Led_Rect	charRect		=	GetCharWindowLocation (t->GetStart ());
			Table::TemporarilySetOwningWP	owningWPSetter (*t, *const_cast<WordProcessor*> (this));
// must adjust/be careful about charRect.GetOrigin () in case out of range... not sure what args to REALLY pass in !
			t->WhileSimpleMouseTracking (newMousePos - charRect.GetOrigin ());
			return;
		}
	}
	inherited::WhileSimpleMouseTracking (newMousePos, dragAnchor);
#if		0
LedDebugTrace ("WordProcessor::WhileSimpleMouseTracking (tickCount=%f, newMousePos=(%d,%d), clickedOnChar=%d, dragAnchor=%d)\n",
				Led_GetTickCount (), newMousePos.v, newMousePos.h, clickedOnChar, dragAnchor
		);
#endif
}

/*
@METHOD:		WordProcessor::InsertTable
@DESCRIPTION:	<p>Create a @'Table' object at the given location in the document. The table object is returned you that you can then
			call specific methods to add rows and columns etc.</p>
*/
WordProcessor::Table*	WordProcessor::InsertTable (size_t at)
{
	Table*	t	=	new Table (fParagraphDatabase, at);
	t->SetDimensions (1, 1);	//tmphack so we have at least one sentinal - auto-delete table when it becomes empty?
								//like the embeddings it owns!
								// LGP 2002-11-15
	return t;
}

/*
@METHOD:		WordProcessor::GetTablesInRange
@DESCRIPTION:	<p>Generate a list of all tables obejcts in the given range. Grabs all tables from 'from' to 'to' with from defaulting
			to the start of the buffer, and 'to' defaulting to the end of the buffer.</p>
*/
vector<WordProcessor::Table*>	WordProcessor::GetTablesInRange (size_t from, size_t to) const
{
	if (to == static_cast<size_t> (-1)) {
		to = GetTextStore ().GetLength ();
	}
	Led_Require (from <= to);
	Led_Require (to <= GetTextStore ().GetLength () + 1);
	MarkersOfATypeMarkerSink2Vector<Table>	result;
	if (not fParagraphDatabase.IsNull ()) {
		GetTextStore ().CollectAllMarkersInRangeInto (from, to, fParagraphDatabase, result);
	}
	return result.fResult;
}

/*
@METHOD:		WordProcessor::GetTableAt
@DESCRIPTION:	<p>Return the table which starts at offset 'from'. If there is no table there - return NULL.</p>
*/
WordProcessor::Table*	WordProcessor::GetTableAt (size_t from) const
{
	size_t	to	=	from + 1;
	Led_Require (to <= GetTextStore ().GetLength () + 1);
	MarkerOfATypeMarkerSink<Table>	result;
	if (not fParagraphDatabase.IsNull ()) {
		GetTextStore ().CollectAllMarkersInRangeInto (from, to, fParagraphDatabase, result);
	}
	return result.fResult;
}

WordProcessor::CommandNames	WordProcessor::MakeDefaultCommandNames ()
{
	WordProcessor::CommandNames	cmdNames;
	cmdNames.fJustificationCommandName				=	Led_SDK_TCHAROF ("Justification Change");
	cmdNames.fStandardTabStopListCommandName		=	Led_SDK_TCHAROF ("Set Tabs");
	cmdNames.fMarginsCommandName					=	Led_SDK_TCHAROF ("Set Margins");
	cmdNames.fFirstIndentCommandName				=	Led_SDK_TCHAROF ("Set First Indent");
	cmdNames.fMarginsAndFirstIndentCommandName		=	Led_SDK_TCHAROF ("Set Margins and First Indent");
	cmdNames.fParagraphSpacingCommandName			=	Led_SDK_TCHAROF ("Change Paragraph Spacing");
	cmdNames.fHideCommandName						=	Led_SDK_TCHAROF ("Hide");
	cmdNames.fUnHideCommandName						=	Led_SDK_TCHAROF ("UnHide");
	cmdNames.fSetListStyleCommandName				=	Led_SDK_TCHAROF ("Change List Style");
	cmdNames.fIndentLevelChangeCommandName			=	Led_SDK_TCHAROF ("Change Indent Level");
	cmdNames.fInsertTableCommandName				=	Led_SDK_TCHAROF ("Insert Table");
	cmdNames.fInsertTableRowAboveCommandName		=	Led_SDK_TCHAROF ("Insert Table Row Above");
	cmdNames.fInsertTableRowBelowCommandName		=	Led_SDK_TCHAROF ("Insert Table Row Below");
	cmdNames.fInsertTableColBeforeCommandName		=	Led_SDK_TCHAROF ("Insert Table Column Before");
	cmdNames.fInsertTableColAfterCommandName		=	Led_SDK_TCHAROF ("Insert Table Column After");
	cmdNames.fInsertURLCommandName					=	Led_SDK_TCHAROF ("Insert URL");
	cmdNames.fRemoveTableRowsCommandName			=	Led_SDK_TCHAROF ("Remove Rows");
	cmdNames.fRemoveTableColumnsCommandName			=	Led_SDK_TCHAROF ("Remove Columns");
	cmdNames.fEmbeddingTypeName_ImageDIB			=	Led_SDK_TCHAROF ("image (DIB)");
	cmdNames.fEmbeddingTypeName_URL					=	Led_SDK_TCHAROF ("URL");
	cmdNames.fEmbeddingTypeName_ImageMacPict		=	Led_SDK_TCHAROF ("image (MacPICT)");
	cmdNames.fEmbeddingTypeName_Table				=	Led_SDK_TCHAROF ("table");
	cmdNames.fEmbeddingTypeName_Unknown				=	Led_SDK_TCHAROF ("unknown");
	cmdNames.fFontSizeChange_Other_NoArg			=	Led_SDK_TCHAROF ("Other...");
	cmdNames.fFontSizeChange_Other_OneArg			=	Led_SDK_TCHAROF ("Other (%d)...");
	cmdNames.fTablePropertiesCommandName			=	Led_SDK_TCHAROF ("Table Properties...")
																	#if		qWindows
																			Led_SDK_TCHAROF ("\tAlt+Enter")
																	#endif
																;
	cmdNames.fGenericEmbeddingPropertiesCommandName	=	Led_SDK_TCHAROF ("Properties")
																	#if		qWindows
																			Led_SDK_TCHAROF ("\tAlt+Enter")
																	#endif
																;
	cmdNames.fChangeTablePropertiesCommandName		=	Led_SDK_TCHAROF ("Change table properties");
	return cmdNames;
}

/*
@METHOD:		WordProcessor::ComputeMaxHScrollPos ()
@DESCRIPTION:	<p>Override @'TextImager::ComputeMaxHScrollPos' to call
			@'WordProcessor::CalculateFarthestRightMargin ()' and
			cache the results (for performance reasons).</p>
*/
Led_Distance	WordProcessor::ComputeMaxHScrollPos () const
{
	Led_Distance	cachedLayoutWidth	=	0;
	{
		/*
		 *	Figure the largest amount we might need to scroll given the current windows contents.
		 *	But take into account where we've scrolled so far, and never invalidate that
		 *	scroll amount. Always leave at least as much layout-width as needed to
		 *	preserve the current scroll-to position.
		 */
		TextInteractor::Tablet_Acquirer	tablet_ (this);
		Led_Tablet		tablet				=	tablet_;
		Led_Distance	width	=	tablet->CvtFromTWIPSH (CalculateFarthestRightMargin ());
		if (GetHScrollPos () != 0) {
			width = Led_Max (width, GetHScrollPos () + GetWindowRect ().GetWidth ());
		}
		cachedLayoutWidth = Led_Max (width, 1);
	}
	Led_Distance	wWidth	=	GetWindowRect ().GetWidth ();
	if (cachedLayoutWidth > wWidth) {
		return (cachedLayoutWidth - wWidth);
	}
	else {
		return 0;
	}
}

/*
@METHOD:		WordProcessor::CalculateFarthestRightMarginInDocument ()
@DESCRIPTION:	<p>Calculate how wide an effective margin must be used for specifying the parameters for
			a horizontal scrollbar. By default - asks the max row width/margins for all the rows displayed in the
			current window (so this value can change when we scroll or edit text).</p>
				<p>See also @'WordProcessor::CalculateFarthestRightMarginInWindow'
			</p>
*/
Led_TWIPS	WordProcessor::CalculateFarthestRightMarginInDocument () const
{
	Led_Coordinate	longestRowWidth	=	0;
	RowReference	curRow			=	RowReference (GetFirstPartitionMarker (), 0);
	do {
		Led_Coordinate	rhsMargin	=	0;
		GetLayoutMargins (curRow, NULL, &rhsMargin);
		longestRowWidth = max (longestRowWidth, rhsMargin);
	}
	while (GetNextRowReference (&curRow));
	Tablet_Acquirer	tablet_ (this);
	Led_Tablet		tablet		=	tablet_;
	return tablet->CvtToTWIPSH (longestRowWidth);
}

/*
@METHOD:		WordProcessor::GetFarthestRightMarginInDocument ()
@DESCRIPTION:	<p>See also @'WordProcessor::CalculateFarthestRightMarginInWindow'
			</p>
*/
Led_TWIPS	WordProcessor::GetFarthestRightMarginInDocument () const
{
	AbstractParagraphDatabaseRep*	pdbRep	=	GetParagraphDatabase ();
	Led_RequireNotNil (pdbRep);	// this shouldn't be called with a null PDB?
	if (pdbRep->fCachedFarthestRightMarginInDocument == kBadCachedFarthestRightMarginInDocument) {
		pdbRep->fCachedFarthestRightMarginInDocument = CalculateFarthestRightMarginInDocument ();
	}
	return pdbRep->fCachedFarthestRightMarginInDocument;
}

/*
@METHOD:		WordProcessor::CalculateFarthestRightMarginInWindow ()
@DESCRIPTION:	<p>Calculate how wide an effective margin must be used for specifying the parameters for
			a horizontal scrollbar. By default - asks the max row width/margins for all the rows displayed in the
			current window (so this value can change when we scroll or edit text).</p>
				<p>NB: prior to Led 3.1d8 - this method returned a Led_Distance - and it now returns a Led_TWIPS.
			</p>
*/
Led_TWIPS	WordProcessor::CalculateFarthestRightMarginInWindow () const
{
	Led_Coordinate	longestRowWidth	=	0;
	size_t			rowsLeftInWindow	=	GetTotalRowsInWindow_ ();
	RowReference	curRow				=	GetTopRowReferenceInWindow ();
	do {
		Led_Coordinate	rhsMargin	=	0;
		GetLayoutMargins (curRow, NULL, &rhsMargin);
		longestRowWidth = max (longestRowWidth, rhsMargin);
	}
	while (rowsLeftInWindow-- > 0 and GetNextRowReference (&curRow));
	Tablet_Acquirer	tablet_ (this);
	Led_Tablet		tablet		=	tablet_;
	return tablet->CvtToTWIPSH (longestRowWidth);
}

/*
@METHOD:		WordProcessor::CalculateFarthestRightMargin ()
@DESCRIPTION:	<p>Typically this will call either @'WordProcessor::GetFarthestRightMarginInDocument' (the default) or
			@'WordProcessor::CalculateFarthestRightMarginInWindow'. This is typically called by @'WordProcessor::ComputeMaxHScrollPos'.
			</p>
*/
Led_TWIPS	WordProcessor::CalculateFarthestRightMargin () const
{
	return GetFarthestRightMarginInDocument ();
}

void	WordProcessor::InvalidateAllCaches ()
{
	inherited::InvalidateAllCaches ();
	ParagraphDatabasePtr	pdb	=	GetParagraphDatabase ();
	if (not pdb.IsNull ()) {
		static_cast<AbstractParagraphDatabaseRep*> (pdb)->fCachedFarthestRightMarginInDocument = kBadCachedFarthestRightMarginInDocument;
	}
}

void	WordProcessor::TabletChangedMetrics ()
{
	inherited::TabletChangedMetrics ();
	ParagraphDatabasePtr	pdb	=	GetParagraphDatabase ();
	if (not pdb.IsNull ()) {
		static_cast<AbstractParagraphDatabaseRep*> (pdb)->fCachedFarthestRightMarginInDocument = kBadCachedFarthestRightMarginInDocument;
	}
}

void	WordProcessor::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	inherited::DidUpdateText (updateInfo);
	fCachedCurSelFontSpecValid = false;
}

void	WordProcessor::AssureCurSelFontCacheValid () const
{
	if (not fCachedCurSelFontSpecValid) {
		Led_Assert (GetSelectionEnd () >= GetSelectionStart ());
		size_t	selectionLength	=	GetSelectionEnd () - GetSelectionStart ();

		Table*	aT = GetActiveTable ();
		if (aT != NULL) {
			Table::TemporarilySetOwningWP	owningWPSetter (*aT, *const_cast<WordProcessor*> (this));
			aT->AssureCurSelFontCacheValid (&fCachedCurSelFontSpec);
			fCachedCurSelJustification = eLeftJustify;
			fCachedCurSelJustificationUnique = false;
		}
		else {
			fCachedCurSelFontSpec = GetContinuousStyleInfo (GetSelectionStart (), selectionLength);
			fCachedCurSelJustificationUnique = GetJustification (GetSelectionStart (), GetSelectionEnd (), &fCachedCurSelJustification);
		}
		fCachedCurSelFontSpecValid = true;
	}
}

void	WordProcessor::DoSingleCharCursorEdit (CursorMovementDirection direction, CursorMovementUnit movementUnit, CursorMovementAction action,
											UpdateMode updateMode, bool scrollToSelection)
{
	Table*	table = GetActiveTable ();
	if (table != NULL) {
		Table::TemporarilySetOwningWP	owningWPSetter (*table, *const_cast<WordProcessor*> (this));
		if (table->DoSingleCharCursorEdit (direction, movementUnit, action, updateMode, false)) {
			if (scrollToSelection) {
				ScrollToSelection ();
			}
			return;
		}
	}
	inherited::DoSingleCharCursorEdit (direction, movementUnit, action, updateMode, scrollToSelection);
}

bool	WordProcessor::OnUpdateCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);

	Table* aT	=	GetActiveTable ();
	if (aT != NULL) {
		Table::TemporarilySetOwningWP	owningWPSetter (*aT, *const_cast<WordProcessor*> (this));
		if (aT->OnUpdateCommand (enabler)) {
			return true;
		}
	}

	switch (enabler->GetCmdID ()) {
		case	kFontStylePlain_CmdID:			{	OnUpdateFontStylePlainCommand (enabler);			return true;		}
		case	kFontStyleBold_CmdID:			{	OnUpdateFontStyleBoldCommand (enabler);				return true;		}
		case	kFontStyleItalic_CmdID:			{	OnUpdateFontStyleItalicCommand (enabler);			return true;		}
		case	kFontStyleUnderline_CmdID:		{	OnUpdateFontStyleUnderlineCommand (enabler);		return true;		}
	#if		qWindows
		case	kFontStyleStrikeout_CmdID:		{	OnUpdateFontStyleStrikeoutCommand (enabler);		return true;		}
	#endif
	#if		qMacOS
		case	kFontStyleOutline_CmdID:		{	OnUpdateFontStyleOutlineCommand (enabler);			return true;		}
		case	kFontStyleShadow_CmdID:			{	OnUpdateFontStyleShadowCommand (enabler);			return true;		}
		case	kFontStyleCondensed_CmdID:		{	OnUpdateFontStyleCondensedCommand (enabler);		return true;		}
		case	kFontStyleExtended_CmdID:		{	OnUpdateFontStyleExtendedCommand (enabler);			return true;		}
	#endif
		case	kSubScriptCommand_CmdID:		{	OnUpdateFontStyleSubscriptCommand (enabler);		return true;		}
		case	kSuperScriptCommand_CmdID:		{	OnUpdateFontStyleSuperscriptCommand (enabler);		return true;		}
		case	kChooseFontCommand_CmdID:		{	OnUpdateChooseFontCommand (enabler);				return true;		}
		case	kInsertTable_CmdID:				{	OnUpdateInsertTableCommand (enabler);				return true;		}
		case	kInsertURL_CmdID:				{	OnUpdateInsertURLCommand (enabler);					return true;		}
		case	kInsertSymbol_CmdID:			{	OnUpdateInsertSymbolCommand (enabler);				return true;		}
		case	kHideSelection_CmdID:			{	OnUpdateHideSelectionCommands (enabler);			return true;		}
		case	kUnHideSelection_CmdID:			{	OnUpdateHideSelectionCommands (enabler);			return true;		}
		case	kParagraphSpacingCommand_CmdID:	{	OnUpdateParagraphSpacingChangeCommand (enabler);	return true;		}
		case	kParagraphIndentsCommand_CmdID:	{	OnUpdateParagraphIndentsChangeCommand (enabler);	return true;		}
		case	kIncreaseIndent_CmdID:			{	OnUpdateIndentCommand (enabler);					return true;		}
		case	kDecreaseIndent_CmdID:			{	OnUpdateIndentCommand (enabler);					return true;		}
	}

	if (kFontMenuFirst_CmdID <= enabler->GetCmdID () and enabler->GetCmdID () <= kFontMenuLast_CmdID) {
		OnUpdateFontNameChangeCommand (enabler);
		return true;
	}
	else if (kBaseFontSize_CmdID <= enabler->GetCmdID () and enabler->GetCmdID () <= kLastFontSize_CmdID) {
		OnUpdateFontSizeChangeCommand (enabler);
		return true;
	}
	else if (kBaseFontColor_CmdID <= enabler->GetCmdID () and enabler->GetCmdID () <= kLastFontColor_CmdID) {
		OnUpdateFontColorChangeCommand (enabler);
		return true;
	}
	else if (kFirstSelectedEmbedding_CmdID <= enabler->GetCmdID () and enabler->GetCmdID () <= kLastSelectedEmbedding_CmdID) {
		OnUpdateSelectedEmbeddingExtendedCommand (enabler);
		return true;
	}
	else if (kFirstJustification_CmdID <= enabler->GetCmdID () and enabler->GetCmdID () <= kLastJustification_CmdID) {
		OnUpdateParagraphJustificationCommand (enabler);
		return true;
	}
	else if (kFirstShowHideGlyph_CmdID <= enabler->GetCmdID () and enabler->GetCmdID () <= kLastShowHideGlyph_CmdID) {
		OnUpdateShowHideGlyphCommand (enabler);
		return true;
	}
	else if (kFirstListStyle_CmdID <= enabler->GetCmdID () and enabler->GetCmdID () <= kLastListStyle_CmdID) {
		OnUpdateListStyleChangeCommand (enabler);
		return true;
	}

	return inherited::OnUpdateCommand (enabler);
}

bool	WordProcessor::OnPerformCommand (CommandNumber commandNumber)
{
	Table* aT	=	GetActiveTable ();
	if (aT != NULL) {
		Table::TemporarilySetOwningWP	owningWPSetter (*aT, *const_cast<WordProcessor*> (this));
		if (aT->OnPerformCommand (commandNumber)) {
			return true;
		}
	}

	switch (commandNumber) {
		case	kFontStylePlain_CmdID:			{	OnFontStylePlainCommand ();				return true;		}
		case	kFontStyleBold_CmdID:			{	OnFontStyleBoldCommand ();				return true;		}
		case	kFontStyleItalic_CmdID:			{	OnFontStyleItalicCommand ();			return true;		}
		case	kFontStyleUnderline_CmdID:		{	OnFontStyleUnderlineCommand ();			return true;		}
	#if		qWindows
		case	kFontStyleStrikeout_CmdID:		{	OnFontStyleStrikeoutCommand ();			return true;		}
	#endif
	#if		qMacOS
		case	kFontStyleOutline_CmdID:		{	OnFontStyleOutlineCommand ();			return true;		}
		case	kFontStyleShadow_CmdID:			{	OnFontStyleShadowCommand ();			return true;		}
		case	kFontStyleCondensed_CmdID:		{	OnFontStyleCondensedCommand ();			return true;		}
		case	kFontStyleExtended_CmdID:		{	OnFontStyleExtendedCommand ();			return true;		}
	#endif
		case	kSubScriptCommand_CmdID:		{	OnFontStyleSubscriptCommand ();			return true;		}
		case	kSuperScriptCommand_CmdID:		{	OnFontStyleSuperscriptCommand ();		return true;		}
		case	kChooseFontCommand_CmdID:		{	OnChooseFontCommand ();					return true;		}
		case	kInsertTable_CmdID:				{	OnInsertTableCommand ();				return true;		}
		case	kInsertURL_CmdID:				{	OnInsertURLCommand ();					return true;		}
		case	kInsertSymbol_CmdID:			{	OnInsertSymbolCommand ();				return true;		}
		case	kHideSelection_CmdID:			{	OnHideSelection ();						return true;		}
		case	kUnHideSelection_CmdID:			{	OnUnHideSelection ();					return true;		}
		case	kParagraphSpacingCommand_CmdID:	{	OnParagraphSpacingChangeCommand ();		return true;		}
		case	kParagraphIndentsCommand_CmdID:	{	OnParagraphIndentsChangeCommand ();		return true;		}
		case	kIncreaseIndent_CmdID:			{	OnIndentCommand (commandNumber);		return true;		}
		case	kDecreaseIndent_CmdID:			{	OnIndentCommand (commandNumber);		return true;		}
	}

	if (kFontMenuFirst_CmdID <= commandNumber and commandNumber <= kFontMenuLast_CmdID) {
		OnFontNameChangeCommand (commandNumber);
		return true;
	}
	else if (kBaseFontSize_CmdID <= commandNumber and commandNumber <= kLastFontSize_CmdID) {
		OnFontSizeChangeCommand (commandNumber);
		return true;
	}
	else if (kBaseFontColor_CmdID <= commandNumber and commandNumber <= kLastFontColor_CmdID) {
		OnFontColorChangeCommand (commandNumber);
		return true;
	}
	else if (kFirstSelectedEmbedding_CmdID <= commandNumber and commandNumber <= kLastSelectedEmbedding_CmdID) {
		return OnSelectedEmbeddingExtendedCommand (commandNumber);
	}
	else if (kFirstJustification_CmdID <= commandNumber and commandNumber <= kLastJustification_CmdID) {
		OnParagraphJustificationCommand (commandNumber);
		return true;
	}
	else if (kFirstShowHideGlyph_CmdID <= commandNumber and commandNumber <= kLastShowHideGlyph_CmdID) {
		OnShowHideGlyphCommand (commandNumber);
		return true;
	}
	else if (kFirstListStyle_CmdID <= commandNumber and commandNumber <= kLastListStyle_CmdID) {
		OnListStyleChangeCommand (commandNumber);
		return true;
	}

	return inherited::OnPerformCommand (commandNumber);
}

bool	WordProcessor::PassAlongCommandToIntraCellModeTableCell (CommandNumber commandNumber)
{
	switch (commandNumber) {
		// TextInteractor commands
		case	kCut_CmdID:							return true;
		case	kCopy_CmdID:						return true;
		case	kPaste_CmdID:						return true;
		case	kClear_CmdID:						return true;

		case	kSelectAll_CmdID: {
			/*
			 *	If we get a select-all command, then first select the cell contents. If that
			 *	is already done - then pass on the command to our parent (which will select a wider
			 *	unit). (see SPR#1615).
			 */
			Table* aT	=	GetActiveTable ();
			size_t	row	=	0;
			size_t	col	=	0;
			if (aT != NULL and aT->GetIntraCellMode (&row, &col)) {
				size_t	intraCellStart	=	0;
				size_t	intraCellEnd	=	0;
				aT->GetIntraCellSelection (&intraCellStart, &intraCellEnd);
				if (intraCellStart != 0) {
					return true;
				}
				size_t	cellEnd	=	aT->GetCell (row, col).GetTextStore ().GetEnd ();
				if (intraCellEnd != cellEnd) {
					return true;
				}
			}
		}
		break;

		// WordProcessor commands
		case	kParagraphSpacingCommand_CmdID:		return true;
		case	kParagraphIndentsCommand_CmdID:		return true;
		case	kIncreaseIndent_CmdID:				return true;
		case	kDecreaseIndent_CmdID:				return true;
		case	kChooseFontCommand_CmdID:			return true;
		case	kInsertURL_CmdID:					return true;
		case	kHideSelection_CmdID:				return true;
		case	kUnHideSelection_CmdID:				return true;
	}

	// Ranged WordProcessor commands
	if (kBaseFontSize_CmdID <= commandNumber and commandNumber <= kLastFontSize_CmdID)						{	return true; }
	if (kBaseFontColor_CmdID <= commandNumber and commandNumber <= kLastFontColor_CmdID)					{	return true; }
	if (kFirstJustification_CmdID <= commandNumber and commandNumber <= kLastJustification_CmdID)			{	return true; }
	if (kFirstListStyle_CmdID <= commandNumber and commandNumber <= kLastListStyle_CmdID)					{	return true; }
	if (kFontMenuFirst_CmdID <= commandNumber and commandNumber <= kFontMenuLast_CmdID)						{	return true; }
	if (kFontStyleCommand_FirstCmdId <= commandNumber and commandNumber <= kFontStyleCommand_LastCmdId)		{	return true; }
	if (kFirstSelectedEmbedding_CmdID <= commandNumber and commandNumber <= kLastSelectedEmbedding_CmdID)	{	return true; }

	return false;
}

bool	WordProcessor::PassAlongCommandToEachSelectedTableCell (CommandNumber commandNumber)
{
	switch (commandNumber) {
		// TextInteractor commands
		case	kClear_CmdID:						return true;

		// WordProcessor commands
		case	kHideSelection_CmdID:				return true;
		case	kUnHideSelection_CmdID:				return true;
	}

	// Ranged WordProcessor commands
	if (kBaseFontSize_CmdID <= commandNumber and commandNumber <= kLastFontSize_CmdID)						{	return true; }
	if (kBaseFontColor_CmdID <= commandNumber and commandNumber <= kLastFontColor_CmdID)					{	return true; }
	if (kFirstJustification_CmdID <= commandNumber and commandNumber <= kLastJustification_CmdID)			{	return true; }
	if (kFirstListStyle_CmdID <= commandNumber and commandNumber <= kLastListStyle_CmdID)					{	return true; }
	if (kFontMenuFirst_CmdID <= commandNumber and commandNumber <= kFontMenuLast_CmdID)						{	return true; }
	if (kFontStyleCommand_FirstCmdId <= commandNumber and commandNumber <= kFontStyleCommand_LastCmdId)		{	return true; }

	return false;
}

/*
@METHOD:		WordProcessor::OnSelectAllCommand
@DESCRIPTION:	<p>Override @'TextInteractor::OnSelectAllCommand' () to also make sure we've selected all the cells
			in a table - if its the only thing in the document (if we aren't changing the selection - and
			have a table selected - then its cells don't automatically get selected otherwise).</p>
*/
void	WordProcessor::OnSelectAllCommand ()
{
	inherited::OnSelectAllCommand ();
	if (GetLength () == 1) {
		(void)OnPerformCommand (kSelectTable_CmdID);
	}
}

void	WordProcessor::OnUpdateFontNameChangeCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_IncrementalFontSpecification	fontSpec	=	GetCurSelFontSpec ();
	// check the item iff it is the currently selected font.
	// But always enable them...
	enabler->SetChecked (fontSpec.GetFontNameSpecifier_Valid () and
						(GetDialogSupport ().CmdNumToFontName (enabler->GetCmdID ()) == fontSpec.GetFontNameSpecifier ())
					);
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontNameChangeCommand (CommandNumber cmdNum)
{
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetFontNameSpecifier (GetDialogSupport ().CmdNumToFontName (cmdNum));
	InteractiveSetFont (applyFontSpec);
}

void	WordProcessor::OnUpdateFontStylePlainCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_IncrementalFontSpecification	fontSpec	=	GetCurSelFontSpec ();
	enabler->SetChecked (fontSpec.GetStyle_Plain_Valid () and fontSpec.GetStyle_Plain ());
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStylePlainCommand ()
{
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_Plain ();
	InteractiveSetFont (applyFontSpec);
}

void	WordProcessor::OnUpdateFontStyleBoldCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_IncrementalFontSpecification	fontSpec	=	GetCurSelFontSpec ();
	enabler->SetChecked (fontSpec.GetStyle_Bold_Valid () and fontSpec.GetStyle_Bold ());
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStyleBoldCommand ()
{
	Led_IncrementalFontSpecification	applyFontSpec;
	Led_IncrementalFontSpecification	fontSpec	=	GetCurSelFontSpec ();
	applyFontSpec.SetStyle_Bold (not (fontSpec.GetStyle_Bold_Valid () and fontSpec.GetStyle_Bold ()));
	InteractiveSetFont (applyFontSpec);
}

void	WordProcessor::OnUpdateFontStyleItalicCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_IncrementalFontSpecification	fontSpec	=	GetCurSelFontSpec ();
	enabler->SetChecked (fontSpec.GetStyle_Italic_Valid () and fontSpec.GetStyle_Italic ());
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStyleItalicCommand ()
{
	Led_IncrementalFontSpecification	applyFontSpec;
	Led_IncrementalFontSpecification	fontSpec	=	GetCurSelFontSpec ();
	applyFontSpec.SetStyle_Italic (not (fontSpec.GetStyle_Italic_Valid () and fontSpec.GetStyle_Italic ()));
	InteractiveSetFont (applyFontSpec);
}

void	WordProcessor::OnUpdateFontStyleUnderlineCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	AssureCurSelFontCacheValid ();
	enabler->SetChecked (fCachedCurSelFontSpec.GetStyle_Underline_Valid () and fCachedCurSelFontSpec.GetStyle_Underline ());
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStyleUnderlineCommand ()
{
	AssureCurSelFontCacheValid ();
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_Underline (not (fCachedCurSelFontSpec.GetStyle_Underline_Valid () and fCachedCurSelFontSpec.GetStyle_Underline ()));
	InteractiveSetFont (applyFontSpec);
}

#if		qMacOS
void	WordProcessor::OnUpdateFontStyleOutlineCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	AssureCurSelFontCacheValid ();
	enabler->SetChecked (fCachedCurSelFontSpec.GetStyle_Outline_Valid () and fCachedCurSelFontSpec.GetStyle_Outline ());
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStyleOutlineCommand ()
{
	AssureCurSelFontCacheValid ();
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_Outline (not (fCachedCurSelFontSpec.GetStyle_Outline_Valid () and fCachedCurSelFontSpec.GetStyle_Outline ()));
	InteractiveSetFont (applyFontSpec);
}

void	WordProcessor::OnUpdateFontStyleShadowCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	AssureCurSelFontCacheValid ();
	enabler->SetChecked (fCachedCurSelFontSpec.GetStyle_Shadow_Valid () and fCachedCurSelFontSpec.GetStyle_Shadow ());
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStyleShadowCommand ()
{
	AssureCurSelFontCacheValid ();
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_Shadow (not (fCachedCurSelFontSpec.GetStyle_Shadow_Valid () and fCachedCurSelFontSpec.GetStyle_Shadow ()));
	InteractiveSetFont (applyFontSpec);
}

void	WordProcessor::OnUpdateFontStyleCondensedCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	AssureCurSelFontCacheValid ();
	enabler->SetChecked (fCachedCurSelFontSpec.GetStyle_Condensed_Valid () and fCachedCurSelFontSpec.GetStyle_Condensed ());
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStyleCondensedCommand ()
{
	AssureCurSelFontCacheValid ();
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_Condensed (not (fCachedCurSelFontSpec.GetStyle_Condensed_Valid () and fCachedCurSelFontSpec.GetStyle_Condensed ()));
	InteractiveSetFont (applyFontSpec);
}

void	WordProcessor::OnUpdateFontStyleExtendedCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	AssureCurSelFontCacheValid ();
	enabler->SetChecked (fCachedCurSelFontSpec.GetStyle_Extended_Valid () and fCachedCurSelFontSpec.GetStyle_Extended ());
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStyleExtendedCommand ()
{
	AssureCurSelFontCacheValid ();
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_Extended (not (fCachedCurSelFontSpec.GetStyle_Extended_Valid () and fCachedCurSelFontSpec.GetStyle_Extended ()));
	InteractiveSetFont (applyFontSpec);
}

#elif	qWindows

void	WordProcessor::OnUpdateFontStyleStrikeoutCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	AssureCurSelFontCacheValid ();
	enabler->SetChecked (fCachedCurSelFontSpec.GetStyle_Strikeout_Valid () and fCachedCurSelFontSpec.GetStyle_Strikeout ());
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStyleStrikeoutCommand ()
{
	AssureCurSelFontCacheValid ();
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_Strikeout (not (fCachedCurSelFontSpec.GetStyle_Strikeout_Valid () and fCachedCurSelFontSpec.GetStyle_Strikeout ()));
	InteractiveSetFont (applyFontSpec);
}

#endif

void	WordProcessor::OnUpdateFontStyleSubscriptCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	AssureCurSelFontCacheValid ();
	enabler->SetChecked (fCachedCurSelFontSpec.GetStyle_SubOrSuperScript_Valid () and fCachedCurSelFontSpec.GetStyle_SubOrSuperScript () == Led_FontSpecification::eSubscript);
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStyleSubscriptCommand ()
{
	AssureCurSelFontCacheValid ();
	Led_IncrementalFontSpecification	applyFontSpec;
			applyFontSpec.SetStyle_SubOrSuperScript (
					(fCachedCurSelFontSpec.GetStyle_SubOrSuperScript_Valid () and
						fCachedCurSelFontSpec.GetStyle_SubOrSuperScript () == Led_FontSpecification::eSubscript
						)?
					Led_FontSpecification::eNoSubOrSuperscript:
					Led_FontSpecification::eSubscript
				);
	InteractiveSetFont (applyFontSpec);
}

void	WordProcessor::OnUpdateFontStyleSuperscriptCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	AssureCurSelFontCacheValid ();
	enabler->SetChecked (fCachedCurSelFontSpec.GetStyle_SubOrSuperScript_Valid () and fCachedCurSelFontSpec.GetStyle_SubOrSuperScript () == Led_FontSpecification::eSuperscript);
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontStyleSuperscriptCommand ()
{
	AssureCurSelFontCacheValid ();
	Led_IncrementalFontSpecification	applyFontSpec;
	applyFontSpec.SetStyle_SubOrSuperScript (
			(fCachedCurSelFontSpec.GetStyle_SubOrSuperScript_Valid () and
				fCachedCurSelFontSpec.GetStyle_SubOrSuperScript () == Led_FontSpecification::eSuperscript
				)?
			Led_FontSpecification::eNoSubOrSuperscript:
			Led_FontSpecification::eSuperscript
		);
	InteractiveSetFont (applyFontSpec);
}

void	WordProcessor::OnUpdateChooseFontCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	enabler->SetEnabled (true);
}

void	WordProcessor::OnChooseFontCommand ()
{
	Led_IncrementalFontSpecification	curSelFontSpec	=	GetCurSelFontSpec ();
	if (GetDialogSupport ().ChooseFont (&curSelFontSpec)) {
		InteractiveSetFont (curSelFontSpec);
	}
}

void	WordProcessor::OnUpdateFontSizeChangeCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_Distance	chosenFontSize	=	GetDialogSupport ().FontCmdToSize (enabler->GetCmdID ());

	AssureCurSelFontCacheValid ();
	if (chosenFontSize == 0) {
		switch (enabler->GetCmdID ()) {
			case	kFontSizeSmaller_CmdID:
			case	kFontSizeLarger_CmdID: {
				enabler->SetEnabled (true);
			}
			break;

			case	kFontSizeOther_CmdID: {
				enabler->SetEnabled (true);
				if (fCachedCurSelFontSpec.GetPointSize_Valid ()) {
					int	pointSize	=	fCachedCurSelFontSpec.GetPointSize ();
					if (not GetDialogSupport ().IsPredefinedFontSize (pointSize)) {
						enabler->SetChecked (true);
						enabler->SetText (Format (GetCommandNames ().fFontSizeChange_Other_OneArg.c_str (), pointSize).c_str ());
						return;
					}
				}
				enabler->SetChecked (false);
				enabler->SetText (GetCommandNames ().fFontSizeChange_Other_NoArg.c_str ());
			}
			break;
		}
	}
	else {
		enabler->SetChecked (fCachedCurSelFontSpec.GetPointSize_Valid () and fCachedCurSelFontSpec.GetPointSize () == chosenFontSize);
		enabler->SetEnabled (true);
	}
}

void	WordProcessor::OnFontSizeChangeCommand (CommandNumber cmdNum)
{
	Led_Distance	chosenFontSize	=	GetDialogSupport ().FontCmdToSize (cmdNum);
	if (chosenFontSize == 0) {
		switch (cmdNum) {
			case	kFontSizeSmaller_CmdID: {
				Led_IncrementalFontSpecification	applyFontSpec;
				applyFontSpec.SetPointSizeIncrement (-1);
				InteractiveSetFont (applyFontSpec);
				return;
			}
			break;
			case	kFontSizeLarger_CmdID: {
				Led_IncrementalFontSpecification	applyFontSpec;
				applyFontSpec.SetPointSizeIncrement (1);
				InteractiveSetFont (applyFontSpec);
				return;
			}
			break;
			case	kFontSizeOther_CmdID: {
				Led_Distance	oldSize	=	fCachedCurSelFontSpec.GetPointSize_Valid ()? fCachedCurSelFontSpec.GetPointSize (): 0;
				chosenFontSize = GetDialogSupport ().PickOtherFontHeight (oldSize);
			}
			break;
		}
	}
	if (chosenFontSize != 0) {
		Led_IncrementalFontSpecification	applyFontSpec;
		applyFontSpec.SetPointSize (static_cast<unsigned short> (chosenFontSize));
		InteractiveSetFont (applyFontSpec);
	}
}

void	WordProcessor::OnUpdateFontColorChangeCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_Require (enabler->GetCmdID () >= kBaseFontColor_CmdID and enabler->GetCmdID () <= kLastFontColor_CmdID);
	AssureCurSelFontCacheValid ();
	if (fCachedCurSelFontSpec.GetTextColor_Valid ()) {
		if (enabler->GetCmdID () == kFontColorOther_CmdID) {
			// should check all other predefined colors, but for now, just don't check it...
			enabler->SetChecked (GetDialogSupport ().FontColorToCmd (fCachedCurSelFontSpec.GetTextColor ()) == kFontColorOther_CmdID);
		}
		else {
			enabler->SetChecked (fCachedCurSelFontSpec.GetTextColor () == GetDialogSupport ().FontCmdToColor (enabler->GetCmdID ()));
		}
	}
	else {
		enabler->SetChecked (false);
	}
	enabler->SetEnabled (true);
}

void	WordProcessor::OnFontColorChangeCommand (CommandNumber cmdNum)
{
	Led_Require (cmdNum >= kBaseFontColor_CmdID and cmdNum <= kLastFontColor_CmdID);

	AssureCurSelFontCacheValid ();
	Led_IncrementalFontSpecification	applyFontSpec;
	if (cmdNum == kFontColorOther_CmdID) {
		Led_Color	originalColor	=	GetDefaultFont ().GetTextColor ();
		if (fCachedCurSelFontSpec.GetTextColor_Valid ()) {
			originalColor = fCachedCurSelFontSpec.GetTextColor ();
		}
		Led_Color	chosenColor = originalColor;
		if (GetDialogSupport ().PickOtherFontColor (&chosenColor)) {
			applyFontSpec.SetTextColor (chosenColor);
		}
	}
	else {
		// Treat color selection like style selection. That is, if text is already red, and you select
		// Red, then treat that as 'turning off red' (ie go to black). Otherwise - just treat the command
		// as setting the whole wange to that color.
		Led_Color	chosenColor	=	GetDialogSupport ().FontCmdToColor (cmdNum);
		if (fCachedCurSelFontSpec.GetTextColor_Valid () and fCachedCurSelFontSpec.GetTextColor () == chosenColor) {
			applyFontSpec.SetTextColor (Led_Color::kBlack);
		}
		else {
			applyFontSpec.SetTextColor (chosenColor);
		}
	}
	InteractiveSetFont (applyFontSpec);
}

void	WordProcessor::OnUpdateInsertTableCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	// only allow insert table on empty selection (for now)... Maybe in the future - if there
	// is text in the selection - create a table smartly around it???? Looking for tabs etc??
	enabler->SetEnabled (GetSelectionStart () == GetSelectionEnd ());
}

void	WordProcessor::OnInsertTableCommand ()
{
	size_t	rows	=	2;	// 2x3 default row/col count....
	size_t	cols	=	3;
	if (GetDialogSupport ().AddNewTableDialog (&rows, &cols)) {
		Led_Assert (rows > 0);
		Led_Assert (cols > 0);
		Led_Assert (rows < 1000);	//	not strictly required - but would be rediculous...
		Led_Assert (cols < 100);	//	''

		InteractiveModeUpdater	iuMode (*this);

		// force early failure in abouttoupdate if read-only... before we constrct any objects or bring up and dialogs...
		TextStore::SimpleUpdater updater1 (GetTextStore (), GetSelectionStart (), GetSelectionEnd ());

		BreakInGroupedCommands ();
		UndoableContextHelper	context (*this, GetCommandNames ().fInsertTableCommandName, false);
			{
				// Put up dialog ROW/COL and then insert columns...
				Table*	t	=	InsertTable (GetSelectionStart ());	// ignore return result cuz kept track of internally and automatically deleted...
				//tmphack - SHOULD come from dialog we launch in here to let user specify
				t->SetDimensions (rows, cols);
				{
					for (size_t r = 0; r < rows; ++r) {
						for (size_t c = 0; c < cols; ++c) {
							Led_TWIPS	targetWidth	=	Led_TWIPS (0);
							{
								Led_Coordinate	lhs	=	0;
								Led_Coordinate	rhs	=	0;
								GetLayoutMargins (GetRowReferenceContainingPosition (t->GetStart ()), &lhs, &rhs);
								Led_Assert (lhs < rhs);
								targetWidth = Led_CvtScreenPixelsToTWIPSH (rhs - lhs);
								targetWidth = Led_TWIPS ((targetWidth / 5) * 4);		// just make it a bit smaller than total possible width
							}
							t->SetColumnWidth (r, c, Led_TWIPS (targetWidth / cols));

							TextStore*	ts	=	NULL;
							t->GetCellWordProcessorDatabases (r, c, &ts);
						}
					}
				}
				TextStore::SimpleUpdater updater (GetTextStore (), t->GetStart (), t->GetEnd ());
				SetSelection (t->GetEnd (), t->GetEnd ());
			}
		context.CommandComplete ();
		BreakInGroupedCommands ();
	}
}

void	WordProcessor::OnUpdateInsertURLCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	enabler->SetEnabled (GetSelectionStart () == GetSelectionEnd ());	// for now only support on empty selection
				// later revise so that if we do this to a non-empty selection, we grab that text and try to make
				// it into a URL???
}

void	WordProcessor::OnInsertURLCommand ()
{
	InteractiveModeUpdater	iuMode (*this);
	BreakInGroupedCommands ();
	Led_SDK_String	title;
	Led_SDK_String	url;
	if (GetDialogSupport ().ShowAddURLEmbeddingInfoDialog (&title, &url)) {
		UndoableContextHelper	context (*this, GetCommandNames ().fInsertURLCommandName, false);
			{
				SimpleEmbeddedObjectStyleMarker*	e	=	new StandardURLStyleMarker (Led_URLD (Led_SDKString2ANSI (url).c_str (), Led_SDKString2ANSI (title).c_str ()));
				AddEmbedding (e, GetTextStore (), GetSelectionStart (), GetStyleDatabase ());
				SetSelection (e->GetEnd (), e->GetEnd ());
			}
		context.CommandComplete ();
	}
	BreakInGroupedCommands ();
}

void	WordProcessor::OnUpdateInsertSymbolCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	#if		qWindows
		enabler->SetEnabled (true);
	#else
		Led_Arg_Unused (enabler);
		Led_Assert (false);	//NYI
	#endif
}

void	WordProcessor::OnInsertSymbolCommand ()
{
	#if		qWindows
		(void)::ShellExecute (NULL, Led_SDK_TCHAROF ("open"), Led_SDK_TCHAROF ("CHARMAP.EXE"), NULL, Led_SDK_TCHAROF (""), SW_SHOWNORMAL);
	#else
		Led_Assert (false);	//NYI
	#endif
}

void	WordProcessor::OnUpdateSelectedEmbeddingExtendedCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	SimpleEmbeddedObjectStyleMarker*	embedding	=	GetSoleSelectedEmbedding ();
	if (enabler->GetCmdID () == kSelectedEmbeddingProperties_CmdID) {
		enabler->SetEnabled (embedding != NULL);
		Table*	t	=	GetActiveTable ();
		enabler->SetText (t == NULL? GetCommandNames ().fGenericEmbeddingPropertiesCommandName.c_str (): GetCommandNames ().fTablePropertiesCommandName.c_str ());
	}
	else {
		typedef	SimpleEmbeddedObjectStyleMarker::PrivateCmdNumber	PrivateCmdNumber;
		PrivateCmdNumber	cmdNum		=	static_cast<PrivateCmdNumber> (enabler->GetCmdID () - kFirstPrivateEmbedding_CmdID + SimpleEmbeddedObjectStyleMarker::eMinPrivateCmdNum);
		enabler->SetEnabled (embedding != NULL and embedding->IsCmdEnabled (cmdNum));
		if (embedding != NULL) {
			enabler->SetText (embedding->GetCmdText (cmdNum).c_str ());
		}
	}
}

bool	WordProcessor::OnSelectedEmbeddingExtendedCommand (CommandNumber cmdNum)
{
	Led_Require (cmdNum >= kFirstSelectedEmbedding_CmdID and cmdNum <= kLastSelectedEmbedding_CmdID);
	SimpleEmbeddedObjectStyleMarker*	embedding	=	GetSoleSelectedEmbedding ();
	if (embedding == NULL) {
		return false;
	}
	if (cmdNum == kSelectedEmbeddingProperties_CmdID) {
		if (dynamic_cast<StandardURLStyleMarker*> (embedding) != NULL) {
			StandardURLStyleMarker*		e		=	dynamic_cast<StandardURLStyleMarker*> (embedding);
			Led_SDK_String				title	=	Led_ANSI2SDKString (e->GetURLData ().GetTitle ());
			Led_SDK_String				url		=	Led_ANSI2SDKString (e->GetURLData ().GetURL ());
			if (GetDialogSupport ().ShowURLEmbeddingInfoDialog (GetPrettyTypeName (embedding), &title, &url)) {
				// Change URL contents...
				{
					TextStore::SimpleUpdater	updater (GetTextStore (), e->GetStart (), e->GetEnd ());
					e->SetURLData (Led_URLD (Led_SDKString2ANSI (url).c_str (), Led_SDKString2ANSI (title).c_str ()));
				}
			}
		}
		else if (dynamic_cast<Table*> (embedding) != NULL) {
			OnEditTablePropertiesDialog ();
		}
		else {
			// unknown embedding...
			GetDialogSupport ().ShowSimpleEmbeddingInfoDialog (GetPrettyTypeName (embedding));
		}
	}
	else {
		typedef	SimpleEmbeddedObjectStyleMarker::PrivateCmdNumber	PrivateCmdNumber;
		PrivateCmdNumber	pCmdNum		=	static_cast<PrivateCmdNumber> (cmdNum - kFirstPrivateEmbedding_CmdID + SimpleEmbeddedObjectStyleMarker::eMinPrivateCmdNum);
		embedding->DoCommand (pCmdNum);
	}
	return true;
}

void	WordProcessor::OnEditTablePropertiesDialog ()
{
	Table*	t	=	GetActiveTable ();
	Led_AssertNotNil (t);

	/*
	 *	Fill in the selected cells/table properties, and then invoke the dialog.
	 *	Then if the dialog returns TRUE, apply the changes.
	 */
	DialogSupport::TableSelectionPropertiesInfo	info;
	info.fTableBorderWidth = t->GetTableBorderWidth ();
	info.fTableBorderColor = t->GetTableBorderColor ();
	t->GetDefaultCellMargins (&info.fDefaultCellMargins.top, &info.fDefaultCellMargins.left, &info.fDefaultCellMargins.bottom, &info.fDefaultCellMargins.right);
	info.fCellSpacing = t->GetCellSpacing ();
	{
		size_t	rowSelStart		=	0;
		size_t	rowSelEnd		=	0;
		size_t	colSelStart		=	0;
		size_t	colSelEnd		=	0;
		t->GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
		info.fCellWidth_Common = false;
		info.fCellBackgroundColor_Common = false;
		for (size_t r = rowSelStart; r < rowSelEnd; ++r) {
			for (size_t c = colSelStart; c < colSelEnd; ++c) {
				if (c < t->GetColumnCount (r)) {
					Led_TWIPS	cellWidth	=	t->GetColumnWidth (r, c);
					Led_Color	cellBkgrnd	=	t->GetCellColor (r, c);
					if (r == rowSelStart and c == colSelStart) {
						info.fCellWidth_Common = true;
						info.fCellWidth = cellWidth;
						info.fCellBackgroundColor_Common = true;
						info.fCellBackgroundColor = cellBkgrnd;
					}
					else {
						if (info.fCellWidth_Common) {
							if (info.fCellWidth != cellWidth) {
								info.fCellWidth_Common = false;
							}
						}
						if (info.fCellBackgroundColor_Common) {
							if (info.fCellBackgroundColor != cellBkgrnd) {
								info.fCellBackgroundColor_Common = false;
							}
						}
					}
				}
			}
		}
	}
	if (GetDialogSupport ().EditTablePropertiesDialog (&info)) {
		UndoableContextHelper	context (*this, GetCommandNames ().fChangeTablePropertiesCommandName, false);
			{
				t->SetTableBorderWidth (info.fTableBorderWidth);
				t->SetTableBorderColor (info.fTableBorderColor);
				t->SetDefaultCellMargins (info.fDefaultCellMargins.top, info.fDefaultCellMargins.left, info.fDefaultCellMargins.bottom, info.fDefaultCellMargins.right);
				t->SetCellSpacing (info.fCellSpacing);
				{
					size_t	rowSelStart		=	0;
					size_t	rowSelEnd		=	0;
					size_t	colSelStart		=	0;
					size_t	colSelEnd		=	0;
					t->GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
					for (size_t r = rowSelStart; r < rowSelEnd; ++r) {
						for (size_t c = colSelStart; c < colSelEnd; ++c) {
							if (c < t->GetColumnCount (r)) {
								if (info.fCellWidth_Common) {
									t->SetColumnWidth (r, c, info.fCellWidth);
								}
								if (info.fCellBackgroundColor_Common) {
									t->SetCellColor (r, c, info.fCellBackgroundColor);
								}
							}
						}
					}
				}
			}
		context.CommandComplete ();
	}
}

void	WordProcessor::OnUpdateHideSelectionCommands (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	bool	enabled	=	GetSelectionStart () != GetSelectionEnd ();
	if (enabled) {
		// check if range already had hidden etc - like GetContiguous for STYLE support....
		if (enabler->GetCmdID () == kHideSelection_CmdID) {
			enabled = not GetHidableTextDatabase ()->GetHidableRegionsContiguous (GetSelectionStart (), GetSelectionEnd (), true);
		}
		else if (enabler->GetCmdID () == kUnHideSelection_CmdID) {
			enabled = not GetHidableTextDatabase ()->GetHidableRegionsContiguous (GetSelectionStart (), GetSelectionEnd (), false);
		}
	}
	enabler->SetEnabled (enabled);
}

void	WordProcessor::OnHideSelection ()
{
	InterectiveSetRegionHidable (true);
}

void	WordProcessor::OnUnHideSelection ()
{
	InterectiveSetRegionHidable (false);
}

void	WordProcessor::OnUpdateParagraphJustificationCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);

	AssureCurSelFontCacheValid ();

	switch (enabler->GetCmdID ()) {
		case	kJustifyLeft_CmdID: {
			enabler->SetChecked (fCachedCurSelJustificationUnique and (fCachedCurSelJustification == eLeftJustify));
		}
		break;
		case	kJustifyCenter_CmdID: {
			enabler->SetChecked (fCachedCurSelJustificationUnique and (fCachedCurSelJustification == eCenterJustify));
		}
		break;
		case	kJustifyRight_CmdID: {
			enabler->SetChecked (fCachedCurSelJustificationUnique and (fCachedCurSelJustification == eRightJustify));
		}
		break;
		case	kJustifyFull_CmdID: {
			enabler->SetChecked (fCachedCurSelJustificationUnique and (fCachedCurSelJustification == eFullyJustify));
		}
		break;
	}
	enabler->SetEnabled (true);
}

void	WordProcessor::OnParagraphJustificationCommand (CommandNumber cmdNum)
{
	switch (cmdNum) {
		case	kJustifyLeft_CmdID: {
			InteractiveSetJustification (eLeftJustify);
		}
		break;
		case	kJustifyCenter_CmdID: {
			InteractiveSetJustification (eCenterJustify);
		}
		break;
		case	kJustifyRight_CmdID: {
			InteractiveSetJustification (eRightJustify);
		}
		break;
		case	kJustifyFull_CmdID: {
			InteractiveSetJustification (eFullyJustify);
		}
		break;
		default: {
			Led_Assert (false);	// shouldn't have been mapped to this function
		}
		break;
	}
}

void	WordProcessor::OnUpdateParagraphSpacingChangeCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	enabler->SetEnabled (true);
}

void	WordProcessor::OnParagraphSpacingChangeCommand ()
{
	Led_TWIPS		spaceBefore	=	Led_TWIPS (0);
	Led_TWIPS		spaceAfter	=	Led_TWIPS (0);
	Led_LineSpacing	lineSpacing;

	bool	spaceBeforeValid	=	GetSpaceBefore (GetSelectionStart (), GetSelectionEnd (), &spaceBefore);
	bool	spaceAfterValid		=	GetSpaceAfter (GetSelectionStart (), GetSelectionEnd (), &spaceAfter);
	bool	lineSpacingValid	=	GetLineSpacing (GetSelectionStart (), GetSelectionEnd (), &lineSpacing);

	if (GetDialogSupport ().PickNewParagraphLineSpacing (&spaceBefore, &spaceBeforeValid, &spaceAfter, &spaceAfterValid, &lineSpacing, &lineSpacingValid)) {
		if (spaceBeforeValid) {
		}
		InteractiveSetParagraphSpacing (spaceBefore, spaceBeforeValid, spaceAfter, spaceAfterValid, lineSpacing, lineSpacingValid);
	}
}

void	WordProcessor::OnUpdateParagraphIndentsChangeCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	enabler->SetEnabled (true);
}

void	WordProcessor::OnParagraphIndentsChangeCommand ()
{
	Led_TWIPS	leftMargin	=	Led_TWIPS (0);
	Led_TWIPS	rightMargin	=	Led_TWIPS (0);
	Led_TWIPS	firstIndent	=	Led_TWIPS (0);

// Should retrieve the 'isvalid' flags for margins separately so if we update one we aren't forced to update the other!
	bool	leftMarginValid		=	GetMargins (GetSelectionStart (), GetSelectionEnd (), &leftMargin, &rightMargin);
	bool	rightMarginValid	=	leftMarginValid;
	bool	firstIndentValid	=	GetFirstIndent (GetSelectionStart (), GetSelectionEnd (), &firstIndent);

	if (GetDialogSupport ().PickNewParagraphMarginsAndFirstIndent (&leftMargin, &leftMarginValid, &rightMargin, &rightMarginValid, &firstIndent, &firstIndentValid)) {
// Similarly (as above - we should pass in a flag saying whether or not to change each of the given margins/etc. Maybe pass in by
// pointer, and treat NULL ptr as meaning no change?
// for now just call if ANYTHING has changed
		InteractiveSetMarginsAndFirstIndent (leftMargin, rightMargin, firstIndent);
	}
}

void	WordProcessor::OnUpdateListStyleChangeCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	ListStyle	listStyle		=	eListStyle_None;
	bool		listStyleValid	=	GetListStyle (GetSelectionStart (), GetSelectionEnd (), &listStyle);
	if (enabler->GetCmdID () == kListStyle_None_CmdID) {
		enabler->SetChecked (listStyleValid and listStyle == eListStyle_None);
	}
	else if (enabler->GetCmdID () == kListStyle_Bullet_CmdID) {
		enabler->SetChecked (listStyleValid and listStyle == eListStyle_Bullet);
	}
	else {
		Led_Assert (false);	// shouldn't have been mapped to this function
	}
	enabler->SetEnabled (true);
}

void	WordProcessor::OnListStyleChangeCommand (CommandNumber cmdNum)
{
	if (cmdNum == kListStyle_None_CmdID) {
		InteractiveSetListStyle (eListStyle_None);
	}
	else if (cmdNum == kListStyle_Bullet_CmdID) {
		InteractiveSetListStyle (eListStyle_Bullet);
	}
	else {
		Led_Assert (false);	// shouldn't have been mapped to this function
	}
}

void	WordProcessor::OnUpdateIndentCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	ListStyle	listStyle		=	eListStyle_None;
	bool		listStyleValid	=	GetListStyle (GetSelectionStart (), GetSelectionEnd (), &listStyle);
	if (enabler->GetCmdID () == kIncreaseIndent_CmdID) {
		enabler->SetEnabled (listStyleValid and listStyle != eListStyle_None);
	}
	else if (enabler->GetCmdID () == kDecreaseIndent_CmdID) {
		enabler->SetEnabled (listStyleValid and listStyle != eListStyle_None);
	}
	else {
		Led_Assert (false);	// shouldn't have been mapped to this function
	}
}

void	WordProcessor::OnIndentCommand (CommandNumber cmdNum)
{
	if (cmdNum == kIncreaseIndent_CmdID) {
		InteractiveDoIndentChange (true);
	}
	else if (cmdNum == kDecreaseIndent_CmdID) {
		InteractiveDoIndentChange (false);
	}
	else {
		Led_Assert (false);	// shouldn't have been mapped to this function
	}
}

void	WordProcessor::OnUpdateShowHideGlyphCommand (CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_Require (enabler->GetCmdID () >= kFirstShowHideGlyph_CmdID);
	Led_Require (enabler->GetCmdID () <= kLastShowHideGlyph_CmdID);
	// NB: we use an if/else skip-chain instead of switch statement so that X constants
	// can be set to magic numbers like zero - to indicate the commands will not be supported.
	if (enabler->GetCmdID () == kShowHideParagraphGlyphs_CmdID) {
		enabler->SetChecked (GetShowParagraphGlyphs ());
	}
	else if (enabler->GetCmdID () == kShowHideTabGlyphs_CmdID) {
		enabler->SetChecked (GetShowTabGlyphs ());
	}
	else if (enabler->GetCmdID () == kShowHideSpaceGlyphs_CmdID) {
		enabler->SetChecked (GetShowSpaceGlyphs ());
	}
	else {
		Led_Assert (false);	// shouldn't have been mapped to this function
	}
	enabler->SetEnabled (true);
}

void	WordProcessor::OnShowHideGlyphCommand (CommandNumber cmdNum)
{
	Led_Require (cmdNum >= kFirstShowHideGlyph_CmdID);
	Led_Require (cmdNum <= kLastShowHideGlyph_CmdID);
	// NB: we use an if/else skip-chain instead of switch statement so that X constants
	// can be set to magic numbers like zero - to indicate the commands will not be supported.
	if (cmdNum == kShowHideParagraphGlyphs_CmdID) {
		SetShowParagraphGlyphs (not GetShowParagraphGlyphs ());
	}
	else if (cmdNum == kShowHideTabGlyphs_CmdID) {
		SetShowTabGlyphs (not GetShowTabGlyphs ());
	}
	else if (cmdNum == kShowHideSpaceGlyphs_CmdID) {
		SetShowSpaceGlyphs (not GetShowSpaceGlyphs ());
	}
	else {
		Led_Assert (false);	// shouldn't have been mapped to this function
	}
}

Led_SDK_String	WordProcessor::GetPrettyTypeName (SimpleEmbeddedObjectStyleMarker* m)
{
	#if		qGCC_MiscNestedClassNameLookupInTemplateBug
		typedef	WordProcessor::Table	Table;
	#endif
	if (dynamic_cast<StandardDIBStyleMarker*> (m) != NULL) {
		return GetCommandNames ().fEmbeddingTypeName_ImageDIB;
	}
	else if (dynamic_cast<StandardURLStyleMarker*> (m) != NULL) {
		return GetCommandNames ().fEmbeddingTypeName_URL;
	}
	#if		qMacOS || qWindows
	else if (dynamic_cast<StandardMacPictureStyleMarker*> (m) != NULL) {
		return GetCommandNames ().fEmbeddingTypeName_ImageMacPict;
	}
	#endif
	else if (dynamic_cast<Table*> (m) != NULL) {
		return GetCommandNames ().fEmbeddingTypeName_Table;
	}
	else {
		return GetCommandNames ().fEmbeddingTypeName_Unknown;
	}
}

SimpleEmbeddedObjectStyleMarker*	WordProcessor::GetSoleSelectedEmbedding () const
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

/*
@METHOD:		WordProcessor::GetListLeader
@DESCRIPTION:
	<p>Return the string (computed often, as with roman numeral lists) which gets inserted to designate the
	list marker.</p>
*/
Led_tString	WordProcessor::GetListLeader (size_t paragraphMarkerPos) const
{
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	ParagraphInfo	pi	=	fParagraphDatabase->GetParagraphInfo (paragraphMarkerPos);
	if (pi.GetListStyle () == eListStyle_None) {
		return Led_tString ();
	}
	else {
		#if		qWideCharacters
			const	Led_tChar	kBulletChar	=	0x2022;
		#else
			#if		qMacOS
				const	Led_tChar	kBulletChar	=	'¥';
			#else
				const	Led_tChar	kBulletChar	=	'·';
			#endif
		#endif
		// In a future release, pay attention to RTF \levelfollowN (0 tab, 1 space, 2 nothing)
		// For now, sample RTF docs with MSWord 2k appear to write out 0 (tab) by default.
		return Led_tString (&kBulletChar, 1) + LED_TCHAR_OF ("\t");
	}
}

/*
@METHOD:		WordProcessor::GetListLeaderLength
@DESCRIPTION:
	<p>Return the width (Led_Distance) of the leader. Based on result from @'WordProcessor::GetListLeader'.</p>
*/
Led_Distance	WordProcessor::GetListLeaderLength (size_t paragraphMarkerPos) const
{
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}

	ParagraphInfo	pi			=	fParagraphDatabase->GetParagraphInfo (paragraphMarkerPos);
	Led_TWIPS		lhsTWIPS	=	pi.GetLeftMargin ();
	lhsTWIPS += pi.GetFirstIndent ();

	Tablet_Acquirer	tablet_ (this);
	Led_Tablet		tablet		=	tablet_;
	Led_Coordinate	lhs =	tablet->CvtFromTWIPSH (lhsTWIPS);
	Led_tString	leader	=	GetListLeader (paragraphMarkerPos);

	size_t			len			=	leader.length ();
	if (len == 0) {
		return 0;
	}
	else {
		Led_SmallStackBuffer<Led_Distance>	distanceResults (len);
		Led_FontSpecification				nextCharsFontStyle	=	GetStyleInfo (paragraphMarkerPos);
		Led_FontSpecification				useBulletFont		=	GetStaticDefaultFont ();
		useBulletFont.SetPointSize (Led_Max (14, nextCharsFontStyle.GetPointSize ()));
		MeasureSegmentWidth_ (useBulletFont, paragraphMarkerPos, paragraphMarkerPos + len,
								leader.c_str (), distanceResults
							);
		(void)ResetTabStopsWithMargin (lhs, paragraphMarkerPos, leader.c_str (), len, distanceResults, 0);
		Led_Distance	result	=	distanceResults[len-1];
		return (result);
	}
}

InteractiveReplaceCommand::SavedTextRep*	WordProcessor::InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd)
{
	typedef	InteractiveReplaceCommand::SavedTextRep	SavedTextRep;
	if (regionStart == regionEnd) {
		return new EmptySelectionParagraphSavedTextRep (this, selStart, selEnd, regionStart);
	}
	else {
		SavedTextRep*	basicRep	=	inherited::InteractiveUndoHelperMakeTextRep (regionStart, regionEnd, selStart, selEnd);
		Table*			aT			=	GetActiveTable ();
		if (aT != NULL) {
			return new Table::SavedTextRepWSel (basicRep, *aT, Table::SavedTextRepWSel::eWPAbove);
		}
		return basicRep;
	}
}

/*
@METHOD:		WordProcessor::InteractiveSetFont
@DESCRIPTION:	<p>Override @'StandardStyledTextInteractor::InteractiveSetFont' to handle embedded tables.</p>
*/
void	WordProcessor::InteractiveSetFont (const Led_IncrementalFontSpecification& defaultFont)
{
	Table*	aT = GetActiveTable ();
	if (aT != NULL) {
		Table::TemporarilySetOwningWP	owningWPSetter (*aT, *this);
		aT->InteractiveSetFont (defaultFont);
		return;
	}
	inherited::InteractiveSetFont (defaultFont);
}

/*
@METHOD:		WordProcessor::InteractiveSetJustification
@DESCRIPTION:
*/
void	WordProcessor::InteractiveSetJustification (Led_Justification justification)
{
	Led_Justification	oldJustification;
	if (not GetJustification (GetSelectionStart (), GetSelectionEnd (), &oldJustification) or
		(oldJustification != justification)
		) {
		InteractiveWPHelper1<DoIt_SetJustification> (this, justification);
	}
}

/*
@METHOD:		WordProcessor::InteractiveSetStandardTabStopList
@DESCRIPTION:
*/
void	WordProcessor::InteractiveSetStandardTabStopList (StandardTabStopList tabStops)
{
	StandardTabStopList	oldTabs;
	if (not GetStandardTabStopList (GetSelectionStart (), GetSelectionEnd (), &oldTabs) or
		(oldTabs != tabStops)
		) {
		InteractiveWPHelper1<DoIt_SetStandardTabStopList> (this, tabStops);
	}
}

/*
@METHOD:		WordProcessor::InteractiveSetMargins
@DESCRIPTION:
*/
void	WordProcessor::InteractiveSetMargins (Led_TWIPS leftMargin, Led_TWIPS rightMargin)
{
	Led_TWIPS	oldLeftMargin	=	Led_TWIPS (0);
	Led_TWIPS	oldRightMargin	=	Led_TWIPS (0);
	if (not GetMargins (GetSelectionStart (), GetSelectionEnd (), &oldLeftMargin, &oldRightMargin) or
		((oldLeftMargin != leftMargin) or (oldRightMargin != rightMargin))
		) {
		InteractiveWPHelper1<DoIt_SetMargins> (this, DoIt_SetMargins::Margins (leftMargin, rightMargin));
	}
}

/*
@METHOD:		WordProcessor::InteractiveSetFirstIndent
@DESCRIPTION:
*/
void	WordProcessor::InteractiveSetFirstIndent (Led_TWIPS firstIndent)
{
	Led_TWIPS	oldFirstIndent	=	Led_TWIPS (0);
	if (not GetFirstIndent (GetSelectionStart (), GetSelectionEnd (), &oldFirstIndent) or
		(oldFirstIndent != firstIndent)
		) {
		InteractiveWPHelper1<DoIt_SetFirstIndent> (this, firstIndent);
	}
}

/*
@METHOD:		WordProcessor::InteractiveSetMarginsAndFirstIndent
@DESCRIPTION:	<p>Roughly equivilent to @'WordProcessor::InteractiveSetMargins' followed by @'WordProcessor::InteractiveSetFirstIndent'
	except that they are bundled together into one action, as far as UNDO is concerned.</p>
*/
void	WordProcessor::InteractiveSetMarginsAndFirstIndent (Led_TWIPS leftMargin, Led_TWIPS rightMargin, Led_TWIPS firstIndent)
{
	Led_TWIPS	oldLeftMargin	=	Led_TWIPS (0);
	Led_TWIPS	oldRightMargin	=	Led_TWIPS (0);
	Led_TWIPS	oldFirstIndent	=	Led_TWIPS (0);
	if (not GetMargins (GetSelectionStart (), GetSelectionEnd (), &oldLeftMargin, &oldRightMargin) or
		((oldLeftMargin != leftMargin) or (oldRightMargin != rightMargin)) or
		not GetFirstIndent (GetSelectionStart (), GetSelectionEnd (), &oldFirstIndent) or
		(oldFirstIndent != firstIndent)
		) {
		InteractiveWPHelper1<DoIt_SetMarginsAndFirstIndent> (this, DoIt_SetMarginsAndFirstIndent::MarginsAndFirstIndent (leftMargin, rightMargin, firstIndent));
	}
}

/*
@METHOD:		WordProcessor::InteractiveSetParagraphSpacing
@DESCRIPTION:
*/
void	WordProcessor::InteractiveSetParagraphSpacing (Led_TWIPS spaceBefore, bool spaceBeforeValid, Led_TWIPS spaceAfter, bool spaceAfterValid, Led_LineSpacing lineSpacing, bool lineSpacingValid)
{
	/*
	 *	If any of these things have changed - do the command.
	 */
	Led_TWIPS		oldSpaceBefore	=	Led_TWIPS (0);
	Led_TWIPS		oldSpaceAfter	=	Led_TWIPS (0);
	Led_LineSpacing	oldLineSpacing;
	if (
		not GetSpaceBefore (GetSelectionStart (), GetSelectionEnd (), &oldSpaceBefore) or
		(oldSpaceBefore != spaceBefore) or
		not GetSpaceAfter (GetSelectionStart (), GetSelectionEnd (), &oldSpaceAfter) or
		(oldSpaceAfter != spaceAfter) or
		not GetLineSpacing (GetSelectionStart (), GetSelectionEnd (), &oldLineSpacing) or
		(oldLineSpacing != lineSpacing)
		) {
		InteractiveWPHelper1<DoIt_SetParagraphSpacing> (this, DoIt_SetParagraphSpacing::AllSpacingArgs (spaceBefore, spaceBeforeValid, spaceAfter, spaceAfterValid, lineSpacing, lineSpacingValid));
	}
}

/*
@METHOD:		WordProcessor::InteractiveSetListStyle
@DESCRIPTION:
*/
void	WordProcessor::InteractiveSetListStyle (ListStyle listStyle)
{
	/*
	 *	If any of these things have changed - do the command.
	 */
	ListStyle		oldListStyle	=	eListStyle_None;
	if (not GetListStyle (GetSelectionStart (), GetSelectionEnd (), &oldListStyle) or
		oldListStyle != listStyle
		) {
		InteractiveWPHelper1<DoIt_SetListStyle> (this, listStyle);
	}
}

/*
@METHOD:		WordProcessor::InteractiveDoIndentChange
@DESCRIPTION:
*/
void	WordProcessor::InteractiveDoIndentChange (bool increase)
{
	InteractiveWPHelper1<DoIt_IndentUnIndentList> (this, increase);
}

/*
@METHOD:		WordProcessor::GetTabStopList
@DESCRIPTION:	<p>Override @'TextImager::GetTabStopList' - to return the tabstoplist associated with this paragraph.</p>
*/
const TextImager::TabStopList&	WordProcessor::GetTabStopList (size_t containingPos) const
{
	return fParagraphDatabase->GetParagraphInfo (containingPos).GetTabStopList ();
}

void	WordProcessor::DrawBefore (const Led_Rect& subsetToDraw, bool printing)
{
	size_t	winStart	=	GetMarkerPositionOfStartOfWindow ();
	size_t	winEnd		=	GetMarkerPositionOfEndOfWindow ();		// note that this COULD cause lots of word-wrapping
																	// and computation

	// Check the current window-display region has no unprocessed tables, and process any if needed
	vector<Table*>	tables = GetTablesInRange (winStart, winEnd);
	for (vector<Table*>::iterator i = tables.begin (); i != tables.end (); ++i) {
		Table*	t	=	*i;
		if (t->fNeedLayout != Table::eDone) {
			Table::TemporarilySetOwningWP	owningWPSetter (*t, *this);
			t->PerformLayout ();
		}
	}
	inherited::DrawBefore (subsetToDraw, printing);
}

/*
@METHOD:		WordProcessor::DrawRowSegments
@DESCRIPTION:	<p>Override @'TextImager::DrawRowSegments' to support things like indents, and justification.</p>
*/
void	WordProcessor::DrawRowSegments (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
									const TextLayoutBlock& text, size_t rowStart, size_t rowEnd
								)
{
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}
	RowReference	row	=	GetRowReferenceContainingPosition (rowStart);
	ParagraphInfo	pi	=	fParagraphDatabase->GetParagraphInfo (rowStart);

	Led_Rect		adjustedDrawInto	=	currentRowRect;
	{
		Led_Coordinate	lhsMargin	=	0;
		GetLayoutMargins (row, &lhsMargin, NULL);
		adjustedDrawInto += Led_Point (0, lhsMargin);
	}

	{
		if (row.GetSubRow () == 0) {
			ListStyle	ls	=	pi.GetListStyle ();
			if (ls != eListStyle_None) {
				/*
				 *	For tables - ignore the list style - SPR#1394.
				 */
				bool	allowLists	=	true;
				if (rowEnd - rowStart == 1) {
					vector<Table*>	tables	=	GetTablesInRange (rowStart, rowEnd);
					if (tables.size () == 1) {
						allowLists = false;
					}
				}
				if (allowLists) {
					// For now - treat ALL lists as bullet lists...
					Led_Rect	xxx	=	adjustedDrawInto;
					Led_tString	listLeader	=	GetListLeader (rowStart);
					xxx.SetLeft (xxx.GetLeft () - GetListLeaderLength (rowStart));
					Led_FontSpecification				nextCharsFontStyle	=	GetStyleInfo (rowStart);
					Led_FontSpecification				useBulletFont		=	GetStaticDefaultFont ();
					useBulletFont.SetPointSize (Led_Max (14, nextCharsFontStyle.GetPointSize ()));
					Led_Coordinate	baseLine	=	xxx.GetTop () + MeasureSegmentBaseLine (rowStart, rowStart);
					DrawSegment_ (tablet, useBulletFont,
									rowStart, rowStart+listLeader.length (),
									TextLayoutBlock_Basic (listLeader.c_str (), listLeader.c_str () + listLeader.length ()), 
									xxx, baseLine, NULL
								);
				}
			}
		}
	}

	switch (pi.GetJustification ()) {
		case	eCenterJustify: {
			adjustedDrawInto.SetLeft (adjustedDrawInto.GetLeft () + CalcSpaceToEat (rowStart)/2);
		}
		break;
		case	eRightJustify: {
			adjustedDrawInto.SetLeft (adjustedDrawInto.GetLeft () + CalcSpaceToEat (rowStart));
		}
		break;
	}
	if (not adjustedDrawInto.IsEmpty ()) {
		inherited::DrawRowSegments (tablet, adjustedDrawInto, invalidRowRect, text, rowStart, rowEnd);
		if (GetShowParagraphGlyphs ()) {
			// check for the last row of a partitionelement, and if we hit it - patch the text and rowEnd guys..
			PartitionElementCacheInfo	pmCacheInfo		=	GetPartitionElementCacheInfo (GetPartitionMarkerContainingPosition (rowStart));
			RowReference				row				=	GetRowReferenceContainingPosition (rowStart);
			if (row.GetSubRow () + 1 ==  pmCacheInfo.GetRowCount () and rowEnd < GetEnd ()) {
				const Led_tChar	newline	=	'\n';
				Led_FontSpecification	nextCharsFontStyle	=	GetStyleInfo (rowEnd);
				Led_FontSpecification	useFont				=	GetStaticDefaultFont ();
				useFont.SetPointSize (Led_Max (14, nextCharsFontStyle.GetPointSize ()));
				useFont.SetTextColor (Led_Color::kGray);
				Led_Rect		yyy	=	adjustedDrawInto;
				Led_Distance	segmentWidth		=	CalcSegmentSize (rowStart, rowEnd);
				yyy.left += segmentWidth;
				Led_Coordinate	baseLine	=	yyy.GetTop () + MeasureSegmentBaseLine (rowStart, rowStart);
				DrawSegment_ (tablet, useFont, rowEnd, rowEnd+1,
							TextLayoutBlock_Basic (&newline, &newline + 1), 
							yyy, baseLine, NULL
						);
			}
		}
	}
}

/*
@METHOD:		WordProcessor::GetRowHilightRects
@DESCRIPTION:	<p>Override @'TextImager::GetRowHilightRects' to support tables.</p>
*/
vector<Led_Rect>	WordProcessor::GetRowHilightRects (const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, size_t hilightStart, size_t hilightEnd) const
{
	size_t	len	=	rowEnd-rowStart;
	if (len == 1 and text.PeekAtRealText ()[0] == kEmbeddingSentinalChar) {
		vector<Table*>	tables	=	GetTablesInRange (rowStart, rowEnd);
		Led_Assert (tables.size () <= 1);
		if (not tables.empty ()) {
			Table*							table		=	tables[0];
			Table::TemporarilySetOwningWP	owningWPSetter (*table, *const_cast<WordProcessor*> (this));
			return table->GetRowHilightRects ();
		}
	}
	return inherited::GetRowHilightRects (text, rowStart, rowEnd, hilightStart, hilightEnd);
}

void	WordProcessor::DrawSegment (Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
											Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										)
{
	Led_Distance	pixelsDrawn_;
	if (GetShowTabGlyphs ()) {
		if (pixelsDrawn == NULL) {
			pixelsDrawn_ = 0;
			pixelsDrawn = &pixelsDrawn_;
		}
	}

	inherited::DrawSegment (tablet, from, to, text, drawInto, invalidRect, useBaseLine, pixelsDrawn);

	// Find all tabs in the segment, and draw them
	if (GetShowTabGlyphs () and GetJustification (from) == eLeftJustify) {
		for (size_t i = from; i < to; i++) {
			if (text.PeekAtVirtualText ()[i-from] == '\t') {
				// Then I need to find distance from last char-pos to this one to draw my glyph
				Led_Distance	beforeTabPos	=	drawInto.GetLeft () + CalcSegmentSize (from, i);
				Led_AssertNotNil (pixelsDrawn);
				Led_Distance	afterTabPos		=	drawInto.GetLeft () + CalcSegmentSize (from, i+1);
				Led_Assert (beforeTabPos < afterTabPos);
				Led_Rect		tabRect			=	drawInto;
				tabRect.SetLeft (beforeTabPos);
				tabRect.SetRight (afterTabPos);
				Led_Assert (tabRect.GetLeft () < tabRect.GetRight ());
				#if		1
					{
						Led_Color	arrowColor	=	GetEffectiveDefaultTextColor (eDefaultTextColor);
						// Draw a line down the middle (with a couple pixel sluff on either side)
						Led_Rect	arrowBody	=	tabRect;
						arrowBody.left += 2;
						arrowBody.right -= 2;
						arrowBody.top += arrowBody.GetHeight () / 2;
						arrowBody.bottom = arrowBody.top + 1;
						arrowBody.bottom = min (arrowBody.bottom, drawInto.bottom);	// just in case...
						tablet->EraseBackground_SolidHelper (arrowBody, arrowColor);

						// Create the arrow head. Make a list of points. Then create a region from those points, and fill it.
						const Led_Coordinate	kArrowHSize	=	8;
						const Led_Coordinate	kArrowVSize	=	8;
						Led_Point		tip				=	arrowBody.GetTopRight ();
						Led_Coordinate	hTriangleBase	=	max (arrowBody.right - kArrowHSize, arrowBody.left);
						Led_Point		topPt			=	Led_Point (max (arrowBody.GetTop () - kArrowVSize/2, tabRect.top), hTriangleBase);
						Led_Point		botPt			=	Led_Point (min (arrowBody.GetTop () + kArrowVSize/2, tabRect.bottom), hTriangleBase);
						#if		qWindows
							Led_Brush				backgroundBrush (arrowColor.GetOSRep ());
							Led_Win_Obj_Selector	pen (tablet, ::GetStockObject (NULL_PEN));
							Led_Win_Obj_Selector	brush (tablet, backgroundBrush);
							POINT	pts[3];
							pts[0] = AsPOINT (tip);
							pts[1] = AsPOINT (topPt);
							pts[2] = AsPOINT (botPt);
							Led_Verify (::Polygon (*tablet, pts, Led_NEltsOf (pts)));
						#elif	qMacOS
							PolyHandle	ph	=	::OpenPoly ();
							::MoveTo (tip.h, tip.v);
							::LineTo (topPt.h, topPt.v);
							::LineTo (botPt.h, botPt.v);
							::LineTo (tip.h, tip.v);
							::ClosePoly ();
							if (ph != NULL) {
								Led_Assert (*tablet == Led_GetCurrentGDIPort ());
								GDI_RGBForeColor (arrowColor.GetOSRep ());
								::FillPoly (ph, &Led_Pen::kBlackPattern);
								::KillPoly (ph);
							}
						#else
							//NYI - but not too serious - cuz looks pretty reasonable without arrow head - just the line...
						#endif
					}
				#else
					//TMPHACK SO WE CAN SEE WEVE GOT RIGHT AREA:
					tablet->EraseBackground_SolidHelper (tabRect, Led_Color::kRed);
				#endif
			}
		}
	}
}

Led_Distance	WordProcessor::MeasureSegmentHeight (size_t from, size_t to) const
{
	Tablet_Acquirer	tablet_ (this);
	Led_Tablet		tablet				=	tablet_;
	Led_Distance	d					=	inherited::MeasureSegmentHeight (from, to);

	if (d == 0) {
		HidableTextDatabasePtr	hdb	=	GetHidableTextDatabase ();
		if (not hdb.IsNull ()) {
			DiscontiguousRun<bool>	regions	=	hdb->GetHidableRegions (from, to);
			if (not regions.empty ()) {
				if (not regions[0].fData) {
					return 0;
				}
			}
		}
	}

	PartitionMarker*	pm		=	GetPartitionMarkerContainingPosition (from);
	size_t				pmStart	=	pm->GetStart ();
	size_t				pmEnd	=	pm->GetEnd ();

	/*
	 *	For tables - ignore the line spacing and row spacing parameters. Check stuff like pmStart/End to avoid
	 *	expensive call to GetTablesInRange () except when needed. See SPR#1336.
	 */
	if (pmEnd - pmStart == 1) {
		vector<Table*>	tables	=	GetTablesInRange (pmStart, pmEnd);
		if (tables.size () == 1) {
			#if		qDebug
				Table*	t	=	tables[0];
				Led_Assert (t->GetStart () == pmStart);
				Led_Assert (t->GetEnd () == pmEnd);
			#endif
			return d;
		}
	}

	ParagraphInfo		pi		=	fParagraphDatabase->GetParagraphInfo (from);
	{
		Led_LineSpacing	sl	=	pi.GetLineSpacing ();
		switch (sl.fRule) {
			case	Led_LineSpacing::eOnePointFiveSpace:		d *= 3; d /= 2; break;
			case	Led_LineSpacing::eDoubleSpace:				d *= 2; break;
			case	Led_LineSpacing::eAtLeastTWIPSSpacing:		d = max (Led_Distance (tablet->CvtFromTWIPSV (Led_TWIPS (sl.fArg))), d); break;
			case	Led_LineSpacing::eExactTWIPSSpacing:		d = tablet->CvtFromTWIPSV (Led_TWIPS (sl.fArg)); break;
			case	Led_LineSpacing::eExactLinesSpacing:		d *= sl.fArg; d /= 20; break;

			default:	// Treat as Single space
			case	Led_LineSpacing::eSingleSpace: break;
		}
	}
	{
		bool	isStartOfPara	=	pmStart == from;		// Not a great way to check - but hopefully good enough? LGP 2000/05/30
		if (isStartOfPara) {
			d += tablet->CvtFromTWIPSV (pi.GetSpaceBefore ());
		}
	}
	{
		bool	isEndOfPara		=	pmEnd <= to + 1;		// Not a great way to check - but hopefully good enough? LGP 2000/05/30
		if (isEndOfPara) {
			d += tablet->CvtFromTWIPSV (pi.GetSpaceAfter ());
		}
	}
	return d;
}

Led_Distance	WordProcessor::MeasureMinSegDescent (size_t from, size_t to) const
{
	/*
	 *	See StyledTextImager::MeasureSegmentHeight () for something similar - that this code is paterned after.
	 */

	Led_Require (from <= to);
	if (from == to) {			// HACK/TMP? SO WE GET AT LEAST ONE SUMMARY RECORD?? LGP 951018
		to = from + 1;
	}

	vector<RunElement>	outputSummary	=	SummarizeStyleMarkers (from, to);

	size_t			outputSummaryLength	=	outputSummary.size ();
	Led_Assert (outputSummaryLength != 0);
	Led_Distance	minHeightBelow		=	0;
	size_t			indexIntoText		=	0;
	for (size_t i = 0; i < outputSummaryLength; i++) {
		const RunElement&	re	=	outputSummary[i];
		size_t	reFrom		=	indexIntoText + from;
		size_t	reLength	=	re.fLength;
		size_t	reTo		=	reFrom + reLength;
		Led_Assert (indexIntoText <= to-from);
		Led_Distance	itsBaseline;
		Led_Distance	itsHeight;
		if (re.fMarker == NULL) {
			itsBaseline = MeasureSegmentBaseLine_ (GetDefaultFont (), reFrom, reTo);
			itsHeight =	MeasureSegmentHeight_ (GetDefaultFont (), reFrom, reTo);
		}
		else {
			itsBaseline = re.fMarker->MeasureSegmentBaseLine (this, re, reFrom, reTo);
			itsHeight =	re.fMarker->MeasureSegmentHeight (this, re, reFrom, reTo);
		}
		minHeightBelow = Led_Min (minHeightBelow, (itsHeight-itsBaseline));
		indexIntoText += reLength;
	}
	return minHeightBelow;
}

void	WordProcessor::AdjustBestRowLength (size_t textStart, const Led_tChar* text, const Led_tChar* end, size_t* rowLength)
{
	Led_RequireNotNil (text);
	Led_RequireNotNil (end);
	Led_Require (text < end);
	Led_RequireNotNil (rowLength);
	Led_Require (*rowLength > 0);
	inherited::AdjustBestRowLength (textStart, text, end, rowLength);
	for (const Led_tChar* cur = &text[0]; cur < end; cur = Led_NextChar (cur)) {
		if (*cur == kEmbeddingSentinalChar) {
			// Check if its inside a table - and if yes - then rowLength=1
			vector<Table*>	tables	=	GetTablesInRange (textStart + cur - text, textStart + cur - text + 1);
			if (not tables.empty ()) {
				Led_Assert (cur == text);
				size_t	newBestRowLength	=	(cur-text)+1;
				Led_Assert (newBestRowLength <= *rowLength + 1);	// Assure newBestRowLength is less than it would have been without the
																	// softlinebreak character, EXCEPT if the softlinebreak char is already
																	// at the spot we would have broken - then the row gets bigger by the
																	// one softlinebreak char length...
																	// LGP 2001-05-09 (see SPR707 test file-SimpleAlignDivTest.html)
				Led_Assert (newBestRowLength >= 1);
				*rowLength = newBestRowLength;
				break;
			}
		}
		// SINCE THE WHOLE PM MUST BE A TABLE OR NOT IN THE PM AT ALL, WE CAN DO THIS...
		break;
	}
}

Led_Distance	WordProcessor::MeasureSegmentBaseLine (size_t from, size_t to) const
{
	/*
	 *	The default algorithm will ask each display marker for its baseline - and that will
	 *	return - basicly - the MAX of the asents of all the segments.
	 */
	Led_Distance	d		=	inherited::MeasureSegmentBaseLine (from, to);

	if (d == 0) {
		HidableTextDatabasePtr	hdb	=	GetHidableTextDatabase ();
		if (not hdb.IsNull ()) {
			DiscontiguousRun<bool>	regions	=	hdb->GetHidableRegions (from, to);
			if (not regions.empty ()) {
				if (not regions[0].fData) {
					return 0;
				}
			}
		}
	}

	PartitionMarker*	pm		=	GetPartitionMarkerContainingPosition (from);
	size_t				pmStart	=	pm->GetStart ();
	size_t				pmEnd	=	pm->GetEnd ();

	/*
	 *	For tables - ignore the line spacing and row spacing parameters. Check stuff like pmStart/End to avoid
	 *	expensive call to GetTablesInRange () except when needed. See SPR#1336.
	 */
	if (pmEnd - pmStart == 1) {
		vector<Table*>	tables	=	GetTablesInRange (pmStart, pmEnd);
		if (tables.size () == 1) {
			#if		qDebug
				Table*	t	=	tables[0];
				Led_Assert (t->GetStart () == pmStart);
				Led_Assert (t->GetEnd () == pmEnd);
			#endif
			return d;
		}
	}

	/*
	 *		Then - in one special case of line spacing - we can actually REDUCE the amount of line height.
	 *	In that case - we will make a corresponding reduction in the baseline (so we trim the TOP of the
	 *	text - not the bottom).
	 *		The question is exactly how? Where do we steal the pixels from.
	 *		Again - following what MSWord 2000 does - it seems to compute the MINIMAL decent for all the text in the region. And use that.
	 *		Then - it takes what height it has been allowed (by the Led_LineSpacing::eExactTWIPSSpacing or whatever) - and then computes
	 *	the baseline based on the given height and the MINIMAL (rather than the normal maximal) decent.
	 */
	ParagraphInfo	pi	=	fParagraphDatabase->GetParagraphInfo (from);
	{
		Led_LineSpacing	sl	=	pi.GetLineSpacing ();
		switch (sl.fRule) {
			case	Led_LineSpacing::eExactTWIPSSpacing: {
				Tablet_Acquirer	tablet_ (this);
				Led_Tablet		tablet				=	tablet_;
				Led_Distance	revisedSegHeight	=	tablet->CvtFromTWIPSV (Led_TWIPS (sl.fArg));
				Led_Distance	mhb					=	MeasureMinSegDescent (from, to);	// aka decent
				d = revisedSegHeight - mhb;
			}
			break;
			case	Led_LineSpacing::eExactLinesSpacing: {
				if (sl.fArg < 20) {
					Tablet_Acquirer	tablet_ (this);
					Led_Distance	normalSegHeight		=	inherited::MeasureSegmentHeight (from, to);
					Led_Distance	revisedSegHeight	=	normalSegHeight * sl.fArg / 20; 
					Led_Distance	mhb					=	MeasureMinSegDescent (from, to);	// aka decent
					d = revisedSegHeight - mhb;
				}
			}
			break;
		}
	}

	/*
	 *	Then - we must add in any SPACE - BEFORE.
	 */
	bool	isStartOfPara	=	pmStart == from;		// Not a great way to check - but hopefully good enough? LGP 2000/05/30
	if (isStartOfPara) {
		Led_TWIPS		sb		=	pi.GetSpaceBefore ();
		if (sb != 0) {
			Tablet_Acquirer	tablet_ (this);
			Led_Tablet		tablet	=	tablet_;
			d += tablet->CvtFromTWIPSV (sb);
		}
	}
	return d;
}

/*
@METHOD:		WordProcessor::SetShowParagraphGlyphs
@DESCRIPTION:	<p>The 'ShowParagraphGlyphs' property determines whether or not a glyph (image)
			is shown on the screen where the end of a paragraph (newline character) would be if it were visible.</p>
				<p>This defaults to <em>off</em></p>
				<p>See also @'WordProcessor::GetShowParagraphGlyphs'.</p>
*/
void	WordProcessor::SetShowParagraphGlyphs (bool showParagraphGlyphs)
{
	if (fShowParagraphGlyphs != showParagraphGlyphs) {
		fShowParagraphGlyphs = showParagraphGlyphs;
		InvalidateAllCaches ();	// Can change font metrics for some segments (as well as screen display)
		Refresh ();
	}
}

/*
@METHOD:		WordProcessor::SetShowTabGlyphs
@DESCRIPTION:	<p>The 'ShowTabGlyphs' property determines whether or not a glyph (image)
			is shown on the screen where the tab characters would be if they were visible.</p>
				<p>This defaults to <em>off</em></p>
				<p>See also @'WordProcessor::GetShowTabGlyphs'.</p>
*/
void	WordProcessor::SetShowTabGlyphs (bool showTabGlyphs)
{
	if (fShowTabGlyphs != showTabGlyphs) {
		fShowTabGlyphs = showTabGlyphs;
		Refresh ();		// just invalidates the screen display - but not the cached font metrics...
	}
}

/*
@METHOD:		WordProcessor::SetShowSpaceGlyphs
@DESCRIPTION:	<p>The 'ShowSpaceGlyphs' property determines whether or not a glyph (image)
			is shown on the screen where the space characters would be if they were visible.</p>
				<p>This defaults to <em>off</em></p>
				<p>See also @'WordProcessor::GetShowSpaceGlyphs'.</p>
*/
void	WordProcessor::SetShowSpaceGlyphs (bool showSpaceGlyphs)
{
	if (fShowSpaceGlyphs != showSpaceGlyphs) {
		fShowSpaceGlyphs = showSpaceGlyphs;
		InvalidateAllCaches ();	// Can change font metrics for some segments (as well as screen display)
		Refresh ();
	}
}

size_t	WordProcessor::ComputeRelativePosition (size_t fromPos, CursorMovementDirection direction, CursorMovementUnit movementUnit)
{
	size_t	result	=	inherited::ComputeRelativePosition (fromPos, direction, movementUnit);

	/*
	 *	Quicky implementation to skip over hidable text when using arrow keys.
	 *	Note sure this is right - esp about to-end/to-start stuff?? But seems to work
	 *	halfway decently. -- LGP 2000-08-07
	 */
	HidableTextDatabasePtr	hdb	=	GetHidableTextDatabase ();
	if (not hdb.IsNull ()) {
Again:
		if (direction == eCursorBack or direction == eCursorToStart) {
			DiscontiguousRun<bool>	regions	=	hdb->GetHidableRegions (FindPreviousCharacter (result), result);
			if (not regions.empty ()) {
				//should walk list???
									
				if (not regions[0].fData) {
					result = FindPreviousCharacter (result);
					goto Again;
				}
			}
		}
		else {
			DiscontiguousRun<bool>	regions	=	hdb->GetHidableRegions (result, FindNextCharacter (result));
			if (not regions.empty ()) {
				//should walk list???
									
				if (not regions[0].fData) {
					result = FindNextCharacter (result);
					goto Again;
				}
			}
		}
	}

	return result;
}

/*
@METHOD:		WordProcessor::ContainsMappedDisplayCharacters
@DESCRIPTION:	Override @'TextImager::ContainsMappedDisplayCharacters' to optionally map '\n' etc characters.
*/
bool	WordProcessor::ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const
{
	if (fShowParagraphGlyphs and nTChars > 0 and text[nTChars-1] == '\n') {
		return true;
	}
	if (fShowSpaceGlyphs and nTChars > 0 and ContainsMappedDisplayCharacters_HelperForChar (text, nTChars, ' ')) {
		return true;
	}
	return inherited::ContainsMappedDisplayCharacters (text, nTChars);
}

/*
@METHOD:		WordProcessor::ReplaceMappedDisplayCharacters
@DESCRIPTION:	Override @'TextImager::ContainsMappedDisplayCharacters' to optionally map '\n' etc characters.
*/
void	WordProcessor::ReplaceMappedDisplayCharacters (const Led_tChar* srcText, Led_tChar* copyText, size_t nTChars) const
{
	inherited::ReplaceMappedDisplayCharacters (srcText, copyText, nTChars);
	if (fShowParagraphGlyphs and nTChars > 0 and srcText[nTChars-1] == '\n') {
		// Windoze-specific char - whats equiv on Mac?
		#if		qWideCharacters
			const	Led_tChar	kReplacementChar	=	0x00b6;
		#elif	qMacOS
			const	Led_tChar	kReplacementChar	=	166;
		#else
			const	Led_tChar	kReplacementChar	=	'¶';
		#endif
		copyText[nTChars-1] = kReplacementChar;
	}
	if (fShowSpaceGlyphs) {
		// NOT SURE WHAT CHAR (on any platform) to replace with. Maybe can do best with UNICODE?
		#if		qWideCharacters
			const	Led_tChar	kReplacementChar	=	0x00b7;
		#elif	qMacOS
			const	Led_tChar	kReplacementChar	=	215;
		#else
			const	Led_tChar	kReplacementChar	=	'·';
		#endif
		ReplaceMappedDisplayCharacters_HelperForChar (copyText, nTChars, ' ', kReplacementChar);
	}
}

/*
@METHOD:		WordProcessor::GetCharLocationRowRelative
@DESCRIPTION:	Override @'TextImager::GetCharLocationRowRelative' to adjust calculations for
	things like indents, and justification.
*/
Led_Rect	WordProcessor::GetCharLocationRowRelative (size_t afterPosition, RowReference topRow, size_t maxRowsToCheck) const
{
	Led_Rect	r	=	inherited::GetCharLocationRowRelative (afterPosition, topRow, maxRowsToCheck);

	{
		Led_Coordinate	lhsMargin	=	0;
		RowReference	row			=	GetRowReferenceContainingPosition (afterPosition);
		GetLayoutMargins (row, &lhsMargin, NULL);
		r.left += lhsMargin;
		r.right += lhsMargin;
	}

	Led_Justification	justification	=	GetJustification (afterPosition);
	if (justification == eCenterJustify or justification == eRightJustify) {
		switch (justification) {
			case	eCenterJustify: {
				Led_Distance	d	=	CalcSpaceToEat (afterPosition)/2;
				r.left += d;
				r.right += d;
			}
			break;
			case	eRightJustify: {
				Led_Distance	d	=	CalcSpaceToEat (afterPosition);
				r.left += d;
				r.right += d;
			}
			break;
		}
	}
	return r;
}

/*
@METHOD:		WordProcessor::GetCharAtLocationRowRelative
@DESCRIPTION:	<p>Override @'TextImager::GetCharAtLocationRowRelative' to adjust calculations for
	things like indents, and justification.</p>
*/
size_t		WordProcessor::GetCharAtLocationRowRelative (const Led_Point& where, RowReference topRow, size_t maxRowsToCheck) const
{
	// First find the right row. Justification etc will only effect where we are in the row.
	size_t			posInRow		=	inherited::GetCharAtLocationRowRelative (where, topRow, maxRowsToCheck);
	RowReference	row				=	GetRowReferenceContainingPosition (posInRow);
	Led_Point		adjustedWhere	=	where;
	{
		Led_Coordinate	lhsMargin	=	0;
		GetLayoutMargins (row, &lhsMargin, NULL);
		adjustedWhere.h -= lhsMargin;
	}
	Led_Justification	justification	=	GetJustification (posInRow);
	switch (justification) {
		case	eCenterJustify: {
			adjustedWhere.h -= CalcSpaceToEat (posInRow)/2;
		}
		break;
		case	eRightJustify: {
			adjustedWhere.h -= CalcSpaceToEat (posInRow);
		}
		break;
	}
	return inherited::GetCharAtLocationRowRelative (adjustedWhere, topRow, maxRowsToCheck);
	Led_Assert (false);	/*NotReached*/ return 0;
}

/*
@METHOD:		WordProcessor::ResetTabStops
@DESCRIPTION:	Override @'PartitioningTextImager::ResetTabStops' to adjust the tabstop computation to take
	into account the left hand side margin.
*/
size_t	WordProcessor::ResetTabStops (size_t from, const Led_tChar* text, size_t nTChars, Led_Distance* charLocations, size_t startSoFar) const
{
	Led_Coordinate	lhsMargin	=	0;
	{
		RowReference	row				=	GetRowReferenceContainingPosition (from);
		GetLayoutMargins (row, &lhsMargin, NULL);
	}
	return ResetTabStopsWithMargin (lhsMargin, from, text, nTChars, charLocations, startSoFar);
	
}

size_t		WordProcessor::ResetTabStopsWithMargin (Led_Distance lhsMargin, size_t from, const Led_tChar* text, size_t nTChars, Led_Distance* charLocations, size_t startSoFar) const
{
	Led_RequireNotNil (charLocations);
	size_t			lastTabIndex	=	0;
	Led_Coordinate	tabAdjust		=	0;
	Led_Distance	widthAtStart	=	(startSoFar==0?0:charLocations[startSoFar-1]);
	for (size_t i = startSoFar; i < startSoFar+nTChars; i++) {
		if (text[i] == '\t') {
			Led_Distance	widthSoFar	=	(i==0?0:charLocations[i-1]);
			tabAdjust = widthAtStart + GetTabStopList (from).ComputeTabStopAfterPosition (Tablet_Acquirer (this), widthSoFar - widthAtStart + lhsMargin) - lhsMargin - charLocations[i];
			lastTabIndex = i;
		}
		charLocations[i] += tabAdjust;
	}
	return (lastTabIndex);
}

/*
@METHOD:		WordProcessor::GetLayoutMargins
@DESCRIPTION:	Override @'WordWrappedTextImager::GetLayoutMargins' to take into account paragraph info, like
	margins, etc.
*/
void	WordProcessor::GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const
{
	if (fParagraphDatabase.IsNull ()) {
		throw NoParagraphDatabaseAvailable ();
	}

	PartitionMarker*	pm		=	row.GetPartitionMarker ();
	size_t				pmStart	=	pm->GetStart ();

	Tablet_Acquirer	tablet_ (this);
	Led_Tablet		tablet		=	tablet_;

	ParagraphInfo	pi			=	fParagraphDatabase->GetParagraphInfo (pmStart);
	if (lhs != NULL) {
		Led_TWIPS	lhsTWIPS	=	pi.GetLeftMargin ();
		if (row.GetSubRow () == 0) {
			lhsTWIPS += pi.GetFirstIndent ();
		}
		*lhs = tablet->CvtFromTWIPSH (lhsTWIPS);
		if (row.GetSubRow () == 0) {
			ListStyle	ls	=	pi.GetListStyle ();
			if (ls != eListStyle_None) {
				/*
				 *	For tables - ignore the list style - SPR#1394.
				 */
				bool	allowLists	=	true;
				size_t	pmEnd	=	pm->GetEnd ();
				if (pmEnd - pmStart == 1) {
					vector<Table*>	tables	=	GetTablesInRange (pmStart, pmEnd);
					if (tables.size () == 1) {
						allowLists = false;
					}
				}
				if (allowLists) {
					*lhs += GetListLeaderLength (pmStart);
				}
			}
		}
	}
	if (rhs != NULL) {
		*rhs = tablet->CvtFromTWIPSH (pi.GetRightMargin ());
	}
}

void	WordProcessor::ExpandedFromAndToInPostReplace (size_t from, size_t newTo,
										size_t stableTypingRegionStart, size_t stableTypingRegionEnd,
										size_t startPositionOfRowWhereReplaceBegins, size_t	startPositionOfRowAfterReplaceEnds,
										size_t* expandedFrom, size_t* expandedTo
									)
{
	Led_Justification	justification	=	GetJustification (from);
	if (justification != eLeftJustify) {
		from = GetStartOfRowContainingPosition (from);
	}
	inherited::ExpandedFromAndToInPostReplace (from, newTo,
										stableTypingRegionStart, stableTypingRegionEnd,
										startPositionOfRowWhereReplaceBegins, startPositionOfRowAfterReplaceEnds,
										expandedFrom, expandedTo
									);
}

void	WordProcessor::PostReplace (PreReplaceInfo& preReplaceInfo)
{
	inherited::PostReplace (preReplaceInfo);
	UpdateMode	updateMode	=	preReplaceInfo.GetUpdateMode ();
	if (updateMode != eNoUpdate) {
		if (preReplaceInfo.GetTo () + 2 >= GetEnd ()) {
			/*
			 *	Normal invalidation mechanism only invalidates where text could be drawn. But we draw the bullet/list
			 *	markers just outside of this region. If there are any such list attributes - assure we invalidated
			 *	all those markers on the side. This only seems to come up when typing into a list (or deleting text from a list)
			 *	at the end of the text buffer. -- LGP 2001-07-12 (SPR#0906)
			 */
			ListStyle	ls	=	eListStyle_None;
			if (not WordProcessor::GetListStyle (preReplaceInfo.GetFrom (), GetEnd (), &ls) or ls != eListStyle_None) {
				Refresh (updateMode);
			}
		}
	}
}

Led_Distance	WordProcessor::CalcSpaceToEat (size_t rowContainingCharPos) const
{
	size_t	rowStart	=	GetStartOfRowContainingPosition (rowContainingCharPos);
	size_t	rowEnd		=	GetEndOfRowContainingPosition (rowStart);
	Led_Assert (rowEnd == GetEndOfRowContainingPosition (rowContainingCharPos));

	{
		size_t	lenOfText		=	rowEnd-rowStart;
		Led_SmallStackBuffer<Led_tChar>	buf (lenOfText);
		CopyOut (rowStart, lenOfText, buf);
		// Throw away trailing space characters
		while (rowStart < rowEnd) {
			size_t	i	=	rowEnd-rowStart-1;
			if (CharacterProperties::IsSpace (buf[i])) {
				rowEnd--;
			}
			else {
				break;
			}
		}
	}

	Led_Distance	segmentWidth		=	CalcSegmentSize (rowStart, rowEnd);
	Led_Distance	layoutWidth;
	{
		Led_Coordinate	lhsMargin	=	0;
		Led_Coordinate	rhsMargin	=	0;
		GetLayoutMargins (GetRowReferenceContainingPosition (rowStart), &lhsMargin, &rhsMargin);
		Led_Assert (lhsMargin < rhsMargin);
		layoutWidth = rhsMargin - lhsMargin;
	}

//HACK WORKAROUND FOR SPR#0565
	if (layoutWidth < segmentWidth) {
		return 0;
	}

	Led_Assert (layoutWidth >= segmentWidth);	// is this always so? Maybe not with trailing whitespace???? Should that be ignored for center justification? Etc?
	Led_Distance	xtra	=	layoutWidth-segmentWidth;
	return (xtra);
}








/*
 ********************************************************************************
 *************************** WordProcessorTextIOSinkStream **********************
 ********************************************************************************
 */
inline	Led_TWIPS	CalcDefaultRHSMargin ()
	{
		const	int	kRTF_SPEC_DefaultInches	=	6;	// HACK - see comments in SinkStreamDestination::SetRightMargin ()
		int	rhsTWIPS	=	 kRTF_SPEC_DefaultInches	* 1440;
		return Led_TWIPS (rhsTWIPS);
	}

typedef	WordProcessor::WordProcessorTextIOSinkStream	WordProcessorTextIOSinkStream;
WordProcessorTextIOSinkStream::WordProcessorTextIOSinkStream (TextStore* textStore,
				const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
				const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
				const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase,
				size_t insertionStart
			):
	inherited (textStore, textStyleDatabase, insertionStart),
	fOverwriteTableMode (false),
#if		!qNestedTablesSupported
	fNoTablesAllowed (false),
#endif
	fSavedContexts (),
	fParagraphDatabase (paragraphDatabase),
	fHidableTextDatabase (hidableTextDatabase),
	fSavedParaInfo (),
	fNewParagraphInfo (),
	fTextHidden (false),
	fHidableTextRuns (),
	fEndOfBuffer (false),
	fIgnoreLastParaAttributes (false),
	fCurrentTable (NULL),
	fCurrentTableCellWidths (),
	fCurrentTableCellColor (Led_Color::kWhite),
	fCurrentTableColSpanArray (),
	fTableStack (),
	fNextTableRow (0),
	fNextTableCell (0),
	fCurrentTableCell (size_t (-1))
#if		qDebug
	,fTableOpenLevel (0),
	fTableRowOpen (false),
	fTableCellOpen (false)
#endif
{
	CTOR_COMMON ();
}

WordProcessorTextIOSinkStream::WordProcessorTextIOSinkStream (WordProcessor* wp, size_t insertionStart):
	inherited (&wp->GetTextStore (), wp->GetStyleDatabase (), insertionStart),
	fOverwriteTableMode (false),
#if		!qNestedTablesSupported
	fNoTablesAllowed (false),
#endif
	fSavedContexts (),
	fParagraphDatabase (wp->GetParagraphDatabase ()),
	fHidableTextDatabase (wp->GetHidableTextDatabase ()),
	fSavedParaInfo (),
	fNewParagraphInfo (),
	fTextHidden (false),
	fHidableTextRuns (),
	fEndOfBuffer (false),
	fIgnoreLastParaAttributes (false),
	fCurrentTable (NULL),
	fCurrentTableCellWidths (),
	fCurrentTableCellColor (Led_Color::kWhite),
	fCurrentTableColSpanArray (),
	fTableStack (),
	fNextTableRow (0),
	fNextTableCell (0),
	fCurrentTableCell (size_t (-1))
#if		qDebug
	,fTableOpenLevel (0),
	fTableRowOpen (false),
	fTableCellOpen (false)
#endif
{
	CTOR_COMMON ();
}

WordProcessorTextIOSinkStream::~WordProcessorTextIOSinkStream ()
{
	Led_Assert (fTableOpenLevel == 0);
	Led_Assert (not fTableRowOpen);
	Led_Assert (not fTableCellOpen);
	try {
		Flush ();
		Led_Ensure (GetCachedTextSize () == 0);		// If flush succeeds, then these must be zero
		Led_Ensure (fSavedParaInfo.size () == 0);
	}
	catch (...) {
		// ignore, cuz cannot fail out of DTOR
	}
}

void	WordProcessorTextIOSinkStream::CTOR_COMMON ()
{
	fNewParagraphInfo.SetJustification (eLeftJustify);
	fNewParagraphInfo.SetTabStopList (WordProcessor::GetDefaultStandardTabStopList ());
	fNewParagraphInfo.SetFirstIndent (Led_TWIPS (0));
	fNewParagraphInfo.SetMargins (Led_TWIPS (0), CalcDefaultRHSMargin ());
	fNewParagraphInfo.SetListStyle (eListStyle_None);
	fNewParagraphInfo.SetSpaceBefore (Led_TWIPS (0));
	fNewParagraphInfo.SetSpaceAfter (Led_TWIPS (0));
}

void	WordProcessorTextIOSinkStream::AppendText (const Led_tChar* text, size_t nTChars, const Led_FontSpecification* fontSpec)
{
	Led_RequireNotNil (text);
	inherited::AppendText (text, nTChars, fontSpec);
	if (fSavedParaInfo.size () == 0) {
		fSavedParaInfo.push_back (ParaInfoNSize (fNewParagraphInfo, nTChars));
	}
	else if (fSavedParaInfo.back ().first == fNewParagraphInfo) {
		fSavedParaInfo.back ().second += nTChars;
	}
	else {
		fSavedParaInfo.push_back (ParaInfoNSize (fNewParagraphInfo, nTChars));
	}
	if (fHidableTextRuns.size () != 0 and fHidableTextRuns.back ().fData == fTextHidden) {
		fHidableTextRuns.back ().fElementLength += nTChars;
	}
	else {
		fHidableTextRuns.push_back (DiscontiguousRunElement<bool> (0, nTChars, fTextHidden));
	}
}

void	WordProcessorTextIOSinkStream::AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding)
{
	Led_RequireNotNil (embedding);
	if (GetCachedTextSize () != 0) {
		Flush ();
	}
	size_t	whereToStartHiddenArea	=	GetInsertionStart ();
	inherited::AppendEmbedding (embedding);
	if (fTextHidden) {
		fHidableTextDatabase->MakeRegionHidable (whereToStartHiddenArea, whereToStartHiddenArea + 1);
	}
}

void	WordProcessorTextIOSinkStream::AppendSoftLineBreak ()
{
	AppendText (&WordWrappedTextImager::kSoftLineBreakChar, 1, NULL);
}

void	WordProcessorTextIOSinkStream::SetJustification (Led_Justification justification)
{
	fNewParagraphInfo.SetJustification (justification);
}

void	WordProcessorTextIOSinkStream::SetStandardTabStopList (const TextImager::StandardTabStopList& tabStops)
{
	fNewParagraphInfo.SetTabStopList (tabStops);
}

void	WordProcessorTextIOSinkStream::SetFirstIndent (Led_TWIPS tx)
{
	fNewParagraphInfo.SetFirstIndent (tx);
}

void	WordProcessorTextIOSinkStream::SetLeftMargin (Led_TWIPS lhs)
{
	fNewParagraphInfo.SetMargins (lhs, max (Led_TWIPS (lhs+1), fNewParagraphInfo.GetRightMargin ()));
}

void	WordProcessorTextIOSinkStream::SetRightMargin (Led_TWIPS rhs)
{
	fNewParagraphInfo.SetMargins (fNewParagraphInfo.GetLeftMargin (), max (Led_TWIPS (fNewParagraphInfo.GetLeftMargin ()+1), rhs));
}

void	WordProcessorTextIOSinkStream::SetSpaceBefore (Led_TWIPS sb)
{
	fNewParagraphInfo.SetSpaceBefore (sb);
}

void	WordProcessorTextIOSinkStream::SetSpaceAfter (Led_TWIPS sa)
{
	fNewParagraphInfo.SetSpaceAfter (sa);
}

void	WordProcessorTextIOSinkStream::SetLineSpacing (Led_LineSpacing sl)
{
	fNewParagraphInfo.SetLineSpacing (sl);
}

void	WordProcessorTextIOSinkStream::SetTextHidden (bool hidden)
{
	fTextHidden = hidden;
}

void	WordProcessorTextIOSinkStream::StartTable ()
{
#if		qDebug
	// NB: because of nested tables - we COULD be starting a table inside another table.
	// If we are - then we must be inside a table cell (so row/cell must be open). Otherwise, they
	// must both be closed
	if (fTableOpenLevel == 0) {
		// we are NOT in a non-nested case, and so NONE should be open
		Led_Assert (fTableOpenLevel == 0);
		Led_Assert (not fTableRowOpen);
		Led_Assert (not fTableCellOpen);
	}
	else {
		// we must be in a nested case, and so BOTH should be open
		Led_Assert (fTableRowOpen);
		Led_Assert (fTableCellOpen);
	}
	// either way - one we start a new table - they must both be closed
	fTableRowOpen = false;
	fTableCellOpen = false;
	fTableOpenLevel++;
#endif
#if		!qNestedTablesSupported
	if (GetNoTablesAllowed ()) {
		return;
	}
#endif

	if (GetCachedTextSize () != 0) {
		// Because we address directly into the textstore here - we need any pending writes to actually go in so the right stuff
		// is already in the textstore.
		Flush ();
	}

#if		qNestedTablesSupported
	if (fCurrentTable != NULL) {
// when we support nested tables for REAL - we need to also save other context like fCurrentTableCellWidths / fCurrentTableColSpanArray
		fTableStack.push_back (fCurrentTable);
	}
	fCurrentTable = NULL;
#endif

	if (GetOverwriteTableMode ()) {
		TextStore&						ts				=	GetTextStore ();
		size_t							realCoordStart	=	GetInsertionStart ();
		MarkerOfATypeMarkerSink<Table>	maybeTable;
		Led_Assert (realCoordStart <= ts.GetEnd ());
		ts.CollectAllMarkersInRangeInto (ts.FindPreviousCharacter (realCoordStart), realCoordStart, fParagraphDatabase, maybeTable);
		if (maybeTable.fResult != NULL) {
			fCurrentTable = maybeTable.fResult;
			size_t	rowSelStart	=	0;
			size_t	colSelStart	=	0;
			fCurrentTable->GetCellSelection (&rowSelStart, NULL, &colSelStart, NULL);
			fNextTableRow = rowSelStart;
			fNextTableCell = colSelStart;
			fCurrentTableCell = size_t (-1);
		}
	}

	if (fCurrentTable == NULL) {
		fCurrentTable = new Table (fParagraphDatabase, current_offset () + GetOriginalStart ());
		SetInsertionStart (GetInsertionStart () + 1);	// cuz we added a row which adds a sentinal
		fNextTableRow = 0;
		fNextTableCell = 0;
		fCurrentTableCell = size_t (-1);
	}
}

void	WordProcessorTextIOSinkStream::EndTable ()
{
#if		qDebug
	Led_Require (fTableOpenLevel >= 1);
	Led_Require (not fTableRowOpen);	// caller must close row/cell before closing table
	Led_Require (not fTableCellOpen);
	// if this is a nested table, then the parent scope must have had an open cell/row, and otherwise,
	// NO
	fTableOpenLevel--;
	if (fTableOpenLevel > 0) {
		fTableRowOpen = true;
		fTableCellOpen = true;
	}
	else {
		fTableRowOpen = false;
		fTableCellOpen = false;
	}
#endif
#if		!qNestedTablesSupported
	if (GetNoTablesAllowed ()) {
		AppendText (LED_TCHAR_OF ("\n"), 1, NULL);
		return;
	}
#endif
	if (fCurrentTable != NULL) {
		// Be careful to protect against unbalanced start/ends cuz of bad StyledTextIO input data
		fCurrentTable = NULL;
#if		qNestedTablesSupported
		if (not fTableStack.empty ()) {
			fCurrentTable = fTableStack.back ();
			fTableStack.pop_back ();
		}
#endif
	}
}

void	WordProcessorTextIOSinkStream::StartTableRow ()
{
#if		qDebug
	Led_Require (fTableOpenLevel >= 1);
	Led_Require (not fTableRowOpen);
	Led_Require (not fTableCellOpen);
	fTableRowOpen = true;
#endif
#if		!qNestedTablesSupported
	if (GetNoTablesAllowed ()) {
		fNextTableCell = 0;
		AppendText (LED_TCHAR_OF ("\n"), 1, NULL);
		return;
	}
#endif
	Led_RequireNotNil (fCurrentTable);
	fNextTableRow++;
	if (GetOverwriteTableMode ()) {
		if (fNextTableRow > fCurrentTable->GetRowCount ()) {
			fCurrentTable->InsertRow (fNextTableRow-1, 1);
		}
		size_t	colSelStart	=	0;
		fCurrentTable->GetCellSelection (NULL, NULL, &colSelStart, NULL);
		fNextTableCell = min (fCurrentTable->GetColumnCount (fNextTableRow-1)-1, colSelStart);
	}
	else {
		fNextTableCell = 0;
		fCurrentTable->InsertRow (fNextTableRow-1, 1);
	}
	fCurrentTableColSpanArray.clear ();
}

void	WordProcessorTextIOSinkStream::EndTableRow ()
{
#if		qDebug
	Led_Require (fTableOpenLevel >= 1);
	Led_Require (fTableRowOpen);
	Led_Require (not fTableCellOpen);
	fTableRowOpen = false;
#endif
#if		!qNestedTablesSupported
	if (GetNoTablesAllowed ()) {
		return;
	}
#endif
	Led_AssertNotNil (fCurrentTable);
	size_t	nCellsInThisRow	=	fCurrentTableCellWidths.size ();

	while (nCellsInThisRow > fCurrentTableColSpanArray.size ()) {
		// treat missing startcell/endcell pairs as just colWidth = 1
		fCurrentTableColSpanArray.push_back (1);
	}

	if (nCellsInThisRow != fCurrentTableColSpanArray.size ()) {
		// For HTML readers - they don't (generally) call SetCellWidths - and so
		// we don't get extra cells generated in that way. -- LGP 2003-05-22
		nCellsInThisRow = min (nCellsInThisRow, fCurrentTableColSpanArray.size ());
	}
	fCurrentTable->SetColumnCount (fNextTableRow-1, max (nCellsInThisRow, fCurrentTable->GetColumnCount (fNextTableRow-1)));

	size_t col = 0;
	for (size_t cellIdx = 0; cellIdx < nCellsInThisRow; ++cellIdx) {
		size_t	nColsInThisCell	=	fCurrentTableColSpanArray[cellIdx];
		Led_Assert (nColsInThisCell >= 1);
		Led_Assert (col < fCurrentTable->GetColumnCount ());

		Led_TWIPS	thisCellWidth	=	fCurrentTableCellWidths[cellIdx];
		if (nColsInThisCell == 1) {
			Led_Assert (fNextTableRow > 0);
			fCurrentTable->SetColumnWidth (fNextTableRow-1, col, thisCellWidth);
		}
		else {
			// not sure what to do in this case. All we know is the width of some SET of columns. We don't know how to apportion it between
			// columns. Assume that if it matters - it was specified elsewhere. I COULD use the info based on the existing colwidths
			// to at least set properly the last colwidth...
			Led_TWIPS	prevColWidths	=	Led_TWIPS (0);
			for (size_t i = col; i < col + nColsInThisCell; ++i) {
				Led_Assert (fNextTableRow > 0);
				prevColWidths += fCurrentTable->GetColumnWidth (fNextTableRow-1, i);
			}
			if (prevColWidths < thisCellWidth) {
				Led_Assert (fNextTableRow > 0);
				fCurrentTable->SetColumnWidth (fNextTableRow-1, col, thisCellWidth - prevColWidths);
			}
		}

		col += nColsInThisCell;
	}
}

void	WordProcessorTextIOSinkStream::StartTableCell (size_t colSpan)
{
#if		qDebug
	Led_Require (fTableOpenLevel >= 1);
	Led_Require (fTableRowOpen);
	Led_Require (not fTableCellOpen);
	fTableCellOpen = true;
#endif

#if		!qNestedTablesSupported
	if (GetNoTablesAllowed ()) {
		if (fNextTableCell >= 1) {
			AppendText (LED_TCHAR_OF ("\t"), 1, NULL);
		}
		fNextTableCell ++;
		return;
	}
#endif
	Led_Require (colSpan >= 1);

	fCurrentTableCell = fNextTableCell;
	fCurrentTableColSpanArray.push_back (colSpan);
	fNextTableCell += colSpan;

	Led_AssertNotNil (fCurrentTable);
	fCurrentTable->SetColumnCount (fNextTableRow-1, max (fNextTableCell, fCurrentTable->GetColumnCount (fNextTableRow-1)));

	Led_Assert (fNextTableRow > 0);
	Led_Assert (fNextTableCell > 0);

	TextStore*				ts	=	NULL;
	StyleDatabasePtr		styleDatabase;
	ParagraphDatabasePtr	paragraphDatabase;
	HidableTextDatabasePtr	hidableTextDatabase;
	fCurrentTable->GetCellWordProcessorDatabases (fNextTableRow-1, fCurrentTableCell, &ts, &styleDatabase, &paragraphDatabase, &hidableTextDatabase);
	PushContext (ts, styleDatabase, paragraphDatabase, hidableTextDatabase, 0);
	if (GetOverwriteTableMode ()) {
		ts->Replace (0, ts->GetLength (), NULL, 0);
	}
}

void	WordProcessorTextIOSinkStream::EndTableCell ()
{
#if		qDebug
	Led_Require (fTableOpenLevel >= 1);
	Led_Require (fTableRowOpen);
	Led_Require (fTableCellOpen);
	fTableCellOpen = false;
#endif
#if		!qNestedTablesSupported
	if (GetNoTablesAllowed ()) {
		return;
	}
#endif
	Flush ();
	fCurrentTable->SetCellColor (fNextTableRow-1, fCurrentTableCell, fCurrentTableCellColor);
	PopContext ();
}

void	WordProcessorTextIOSinkStream::SetListStyle (ListStyle listStyle)
{
	fNewParagraphInfo.SetListStyle (listStyle);
}

void	WordProcessorTextIOSinkStream::SetListIndentLevel (unsigned char indentLevel)
{
	fNewParagraphInfo.SetListIndentLevel (indentLevel);
}

void	WordProcessorTextIOSinkStream::SetIgnoreLastParaAttributes (bool ignoreLastParaAttributes)
{
	fIgnoreLastParaAttributes = ignoreLastParaAttributes;
}

void	WordProcessorTextIOSinkStream::SetTableBorderColor (Led_Color c)
{
	if (fCurrentTable != NULL) {
		fCurrentTable->SetTableBorderColor (c);
	}
}

void	WordProcessorTextIOSinkStream::SetTableBorderWidth (Led_TWIPS bWidth)
{
	if (fCurrentTable != NULL) {
		fCurrentTable->SetTableBorderWidth (bWidth);
	}
}

void	WordProcessorTextIOSinkStream::SetCellWidths (const vector<Led_TWIPS>& cellWidths)
{
	if (fCurrentTable != NULL) {
		fCurrentTableCellWidths = cellWidths;
	}
}

void	WordProcessorTextIOSinkStream::SetCellBackColor (const Led_Color c)
{
	if (fCurrentTable != NULL) {
		fCurrentTableCellColor = c;
	}
}

void	WordProcessorTextIOSinkStream::SetDefaultCellMarginsForCurrentRow (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right)
{
	// RTF spec seems to indicate that default cell margins can be specified on
	// a per-row basis, whereas the MSWord XP UI seems to do it on a per-table basis. Anyhow, no
	// matter - as for now - all WE support is on a per table basis, so just update that
	//				-- LGP 2003-04-30
	if (fCurrentTable != NULL) {
		fCurrentTable->SetDefaultCellMargins (top, left, bottom, right);
	}
}

void	WordProcessorTextIOSinkStream::SetDefaultCellSpacingForCurrentRow (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right)
{
	// RTF spec seems to indicate that default cell spacing can be specified on
	// a per-row basis, whereas the MSWord XP UI seems to do it on a per-table basis. Anyhow, no
	// matter - as for now - all WE support is on a per table basis, so just update that
	// Also, the spec allows for a separate value for top/left/bottom/right. Our WP table layout
	// class just uses a single spacing value, so compress them into one for now... (see SPR#1396)
	//				-- LGP 2003-04-30
	if (fCurrentTable != NULL) {
		Led_TWIPS	aveSpacing	=	Led_TWIPS ((top + left + bottom + right)/4);
		fCurrentTable->SetCellSpacing (aveSpacing);
	}
}

void	WordProcessorTextIOSinkStream::PushContext (TextStore* ts,
				const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
				const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
				const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase,
				size_t insertionStart
			)
{
	if (GetCachedTextSize () != 0) {			// must flush before setting/popping context
		Flush ();
	}
	inherited::PushContext (ts, textStyleDatabase, insertionStart);
	Context	c;
	c.fHidableTextDatabase = fHidableTextDatabase;
	c.fParagraphDatabase = fParagraphDatabase;
	fSavedContexts.push_back (c);
	fHidableTextDatabase = hidableTextDatabase;
	fParagraphDatabase = paragraphDatabase;
}

void	WordProcessorTextIOSinkStream::PopContext ()
{
	Led_Require (GetCachedTextSize () == 0);	// must flush before setting/popping context
	Led_Require (not fSavedContexts.empty ());
	inherited::PopContext ();
	fHidableTextDatabase = fSavedContexts.back ().fHidableTextDatabase;
	fParagraphDatabase = fSavedContexts.back ().fParagraphDatabase;
	fSavedContexts.pop_back ();
}

void	WordProcessorTextIOSinkStream::EndOfBuffer ()
{
	fEndOfBuffer = true;
}

void	WordProcessorTextIOSinkStream::Flush ()
{
	size_t	stripParaCharCount	=	0;
	if (fEndOfBuffer and fIgnoreLastParaAttributes) {
		const vector<Led_tChar>&	t	=	GetCachedText ();
		{
			for (vector<Led_tChar>::const_reverse_iterator i = t.rbegin (); i != t.rend (); ++i) {
				if (*i == '\n') {
					break;
				}
				else {
					stripParaCharCount++;
				}
			}
		}
	}

	size_t	dataSize		=	GetCachedTextSize ();
	size_t	whereToInsert	=	GetInsertionStart () - dataSize;
	inherited::Flush ();

	// Flush the cached paragraph info
	{
		#if		qDebug
			{
				size_t	curInsert	=	whereToInsert;
				for (vector<ParaInfoNSize>::const_iterator i = fSavedParaInfo.begin (); i != fSavedParaInfo.end (); ++i) {
					curInsert += (*i).second;
				}
				Led_Assert (curInsert == GetInsertionStart ());
			}
		#endif
		if (stripParaCharCount != 0) {
			Led_Assert (fSavedParaInfo.size () > 0);
			vector<ParaInfoNSize>::iterator i = fSavedParaInfo.end () - 1;
			if ((*i).second > stripParaCharCount) {
				(*i).second -= stripParaCharCount;
			}
			else {
				fSavedParaInfo.resize (fSavedParaInfo.size () - 1);
			}
		}
		fParagraphDatabase->SetParagraphInfo (whereToInsert, fSavedParaInfo);
		fSavedParaInfo.clear ();
	}

	/*
	 *	Somewhat of an inelegant hack to work around SPR#1074. The issue is that when we read in an RTF (or other)
	 *	document, we don't SET the region just past the end of the document. Still - we use this internally to
	 *	store the paragraph info of the last paragraph if it has zero characters (really just if it has
	 *	no terminating NL).
	 *
	 *	For a future release, consider finding a more elegant solution to this. This is the most we can hope
	 *	todo so late in the development cycle.
	 *
	 *		LGP 2001-11-09 - SPR#1074.
	 */
	if (fEndOfBuffer and not fIgnoreLastParaAttributes and whereToInsert == fParagraphDatabase->GetTextStore ().GetEnd ()) {
		fParagraphDatabase->SetParagraphInfo (whereToInsert, 1, IncrementalParagraphInfo (fParagraphDatabase->GetParagraphInfo (fParagraphDatabase->GetTextStore ().FindPreviousCharacter (whereToInsert))));
	}


	// Flush the cached hidable text info
	{
		vector<pair<size_t,size_t> >	hidePairs;
#if		qNoSupportForNewForLoopScopingRules
		{
#endif
			size_t	curInsert	=	whereToInsert;
			for (DiscontiguousRun<bool>::const_iterator i = fHidableTextRuns.begin (); i != fHidableTextRuns.end (); ++i) {
				if ((*i).fData) {
					hidePairs.push_back (pair<size_t,size_t> (curInsert, curInsert + (*i).fElementLength));
				}
				curInsert += (*i).fElementLength;
			}
#if		qNoSupportForNewForLoopScopingRules
		}
#endif
		for (vector<pair<size_t,size_t> >::reverse_iterator i = hidePairs.rbegin (); i != hidePairs.rend (); ++i) {
			fHidableTextDatabase->MakeRegionHidable ((*i).first, (*i).second);
		}
		fHidableTextRuns.clear ();
	}
}







/*
 ********************************************************************************
 *************************** WordProcessorTextIOSrcStream ***********************
 ********************************************************************************
 */
typedef	WordProcessor::WordProcessorTextIOSrcStream	WordProcessorTextIOSrcStream;
WordProcessorTextIOSrcStream::WordProcessorTextIOSrcStream (TextStore* textStore,
				const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
				const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
				const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase,
				size_t selectionStart, size_t selectionEnd
			):
	inherited (textStore, textStyleDatabase, selectionStart, selectionEnd),
	fUseTableSelection (false),
	fParagraphDatabase (paragraphDatabase),
	fHidableTextRuns ()
{

	if (not hidableTextDatabase.IsNull ()) {
		fHidableTextRuns = hidableTextDatabase->GetHidableRegions (selectionStart, selectionEnd);
	}
}

WordProcessorTextIOSrcStream::WordProcessorTextIOSrcStream (WordProcessor* textImager, size_t selectionStart, size_t selectionEnd):
	inherited (textImager, selectionStart, selectionEnd),
	fUseTableSelection (false),
	fParagraphDatabase (textImager->GetParagraphDatabase ()),
	fHidableTextRuns ()
{
	WordProcessor::HidableTextDatabasePtr	hidableTextDatabase	=	textImager->GetHidableTextDatabase ();
	if (not hidableTextDatabase.IsNull ()) {
		fHidableTextRuns = hidableTextDatabase->GetHidableRegions (selectionStart, selectionEnd);
	}
}

Led_Justification	WordProcessorTextIOSrcStream::GetJustification ()	const
{
	if (fParagraphDatabase.IsNull ()) {
		return inherited::GetJustification ();
	}
	else {
		return fParagraphDatabase->GetParagraphInfo (GetCurOffset ()).GetJustification ();
	}
}

TextImager::StandardTabStopList	WordProcessorTextIOSrcStream::GetStandardTabStopList ()	const
{
	if (fParagraphDatabase.IsNull ()) {
		return inherited::GetStandardTabStopList ();
	}
	else {
		return fParagraphDatabase->GetParagraphInfo (GetCurOffset ()).GetTabStopList ();
	}
}

Led_TWIPS	WordProcessorTextIOSrcStream::GetFirstIndent ()	const
{
	if (fParagraphDatabase.IsNull ()) {
		return inherited::GetFirstIndent ();
	}
	else {
		return fParagraphDatabase->GetParagraphInfo (GetCurOffset ()).GetFirstIndent ();
	}
}

void	WordProcessorTextIOSrcStream::GetMargins (Led_TWIPS* lhs, Led_TWIPS* rhs)	const
{
	Led_RequireNotNil (lhs);
	Led_RequireNotNil (rhs);
	if (fParagraphDatabase.IsNull ()) {
		inherited::GetMargins (lhs, rhs);
	}
	else {
		ParagraphInfo	pi	=	fParagraphDatabase->GetParagraphInfo (GetCurOffset ());
		*lhs = pi.GetLeftMargin ();
		*rhs = pi.GetRightMargin ();
	}
}

/*
@METHOD:		WordProcessor::WordProcessorTextIOSrcStream::GetSpaceBefore
@DESCRIPTION:
*/
Led_TWIPS		WordProcessorTextIOSrcStream::GetSpaceBefore () const
{
	if (fParagraphDatabase.IsNull ()) {
		return inherited::GetSpaceBefore ();
	}
	else {
		ParagraphInfo	pi	=	fParagraphDatabase->GetParagraphInfo (GetCurOffset ());
		return pi.GetSpaceBefore ();
	}
}

/*
@METHOD:		WordProcessor::WordProcessorTextIOSrcStream::GetSpaceAfter
@DESCRIPTION:
*/
Led_TWIPS			WordProcessorTextIOSrcStream::GetSpaceAfter () const
{
	if (fParagraphDatabase.IsNull ()) {
		return inherited::GetSpaceAfter ();
	}
	else {
		ParagraphInfo	pi	=	fParagraphDatabase->GetParagraphInfo (GetCurOffset ());
		return pi.GetSpaceAfter ();
	}
}

/*
@METHOD:		WordProcessor::WordProcessorTextIOSrcStream::GetLineSpacing
@DESCRIPTION:
*/
Led_LineSpacing			WordProcessorTextIOSrcStream::GetLineSpacing () const	
{
	if (fParagraphDatabase.IsNull ()) {
		return inherited::GetLineSpacing ();
	}
	else {
		ParagraphInfo	pi	=	fParagraphDatabase->GetParagraphInfo (GetCurOffset ());
		return pi.GetLineSpacing ();
	}
}

/*
@METHOD:		WordProcessor::WordProcessorTextIOSrcStream::GetListStyleInfo
@DESCRIPTION:
*/
void		WordProcessor::WordProcessorTextIOSrcStream::GetListStyleInfo (ListStyle* listStyle, unsigned char* indentLevel) const
{
	Led_RequireNotNil (listStyle);
	Led_RequireNotNil (indentLevel);
	if (fParagraphDatabase.IsNull ()) {
		inherited::GetListStyleInfo (listStyle, indentLevel);
	}
	else {
		ParagraphInfo	pi	=	fParagraphDatabase->GetParagraphInfo (GetCurOffset ());
		*listStyle = pi.GetListStyle ();
		*indentLevel = pi.GetListIndentLevel ();
	}
}

Led_tChar	WordProcessorTextIOSrcStream::GetSoftLineBreakCharacter () const
{
	return WordWrappedTextImager::kSoftLineBreakChar;
}

DiscontiguousRun<bool>	WordProcessorTextIOSrcStream::GetHidableTextRuns () const
{
	return fHidableTextRuns;
}

WordProcessorTextIOSrcStream::Table*	WordProcessorTextIOSrcStream::GetTableAt (size_t at) const
{
	Led_Require (not fParagraphDatabase.IsNull ());
	TextStore&										ts				=	fParagraphDatabase->GetTextStore ();
	size_t											realCoordStart	=	GetEmbeddingMarkerPosOffset () + at;
	MarkerOfATypeMarkerSink<WordProcessor::Table>	maybeTable;
	ts.CollectAllMarkersInRangeInto (realCoordStart, realCoordStart + 1, fParagraphDatabase, maybeTable);
	if (maybeTable.fResult == NULL) {
		return NULL;
	}
	else {
		/*
		 *	Make sure we create a TableIOMapper for just the subset of the document selected. For now, this just
		 *	applies to ROWS (no support yet for selecting columns).
		 */
		size_t	realCoordEnd	=	Led_Min (maybeTable.fResult->GetEnd (), GetSelEnd ());
		Led_Assert (realCoordStart < realCoordEnd);
		if (fUseTableSelection) {
			size_t	rowSelStart		=	0;
			size_t	rowSelEnd		=	0;
			size_t	colSelStart		=	0;
			size_t	colSelEnd		=	0;
			maybeTable.fResult->GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
			return new TableIOMapper (*maybeTable.fResult, rowSelStart, rowSelEnd, colSelStart, colSelEnd);
		}
		else {
			return new TableIOMapper (*maybeTable.fResult);
		}
	}
}

void	WordProcessorTextIOSrcStream::SummarizeFontAndColorTable (set<Led_SDK_String>* fontNames, set<Led_Color>* colorsUsed) const
{
	inherited::SummarizeFontAndColorTable (fontNames, colorsUsed);

	{
		typedef	WordProcessor::Table	Table;
		TextStore&								ts			=	fParagraphDatabase->GetTextStore ();
		MarkersOfATypeMarkerSink2Vector<Table>	tables;
		ts.CollectAllMarkersInRangeInto (GetSelStart (), GetSelEnd (), fParagraphDatabase, tables);
		for (vector<Table*>::iterator i = tables.fResult.begin (); i != tables.fResult.end (); ++i) {
			TableIOMapper tiom (**i);
			size_t	rows	=	tiom.GetRows ();
			for (size_t r = 0; r < rows; ++r) {
				size_t	columns	=	tiom.GetColumns (r);
				for (size_t c = 0; c < columns; ++c) {
					auto_ptr<StyledTextIOWriter::SrcStream>	subSrcStream (tiom.MakeCellSubSrcStream (r, c));
					if (subSrcStream.get () != NULL) {
						subSrcStream.get ()->SummarizeFontAndColorTable (fontNames, colorsUsed);
					}
				}
				if (colorsUsed != NULL) {
					#if		qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug
						typedef	StyledTextIOWriter::SrcStream::Table::CellInfo	CellInfo;
					#else
						//using	StyledTextIOWriter::SrcStream::Table::CellInfo;
						typedef StyledTextIOWriter::SrcStream::Table::CellInfo CellInfo;
					#endif
					vector<CellInfo>	cellInfos;
					tiom.GetRowInfo (r, &cellInfos);
					for (vector<CellInfo>::const_iterator i = cellInfos.begin (); i != cellInfos.end (); ++i) {
						colorsUsed->insert ((*i).f_clcbpat);
					}
				}
			}
		}
	}
}








/*
 ********************************************************************************
 *********** WordProcessor::WordProcessorTextIOSrcStream::TableIOMapper *********
 ********************************************************************************
 */
WordProcessorTextIOSrcStream::TableIOMapper::TableIOMapper (WordProcessor::Table& realTable,
									size_t startRow, size_t endRow,
									size_t startCol, size_t endCol
								):
	fRealTable (realTable),
	fStartRow (startRow),
	fEndRow (endRow),
	fStartCol (startCol),
	fEndCol (endCol)
{
	if (fEndRow == static_cast<size_t> (-1)) {
		fEndRow = fRealTable.GetRowCount ();
	}
	if (fEndCol == static_cast<size_t> (-1)) {
		fEndCol = fRealTable.GetColumnCount ();
	}

	Led_Ensure (fStartRow < fEndRow);	// must be at least one row
	Led_Ensure (fStartCol < fEndCol);	// ditto for columns
	Led_Ensure (fEndRow <= fRealTable.GetRowCount ());
	Led_Ensure (fEndCol <= fRealTable.GetColumnCount ());
}

size_t		WordProcessorTextIOSrcStream::TableIOMapper::GetRows () const
{
	return fEndRow - fStartRow;
}

size_t		WordProcessorTextIOSrcStream::TableIOMapper::GetColumns (size_t row) const
{
	size_t	vRow			=	row + fStartRow;
	size_t	realColCount	=	fRealTable.GetColumnCount (vRow);
	size_t	pinnedColEnd	=	min (realColCount, fEndCol);

	Led_Assert (pinnedColEnd > fStartCol);	// not sure how to deal with this failing - can it?
											// I guess we just pin result at zero??? - LGP 2003-04-11
	return pinnedColEnd - fStartCol;
}

void	WordProcessorTextIOSrcStream::TableIOMapper::GetRowInfo (size_t row, vector<CellInfo>* cellInfos)
{
	Led_Require (row < GetRows ());

	size_t	vRow	=	row + fStartRow;

	Led_RequireNotNil (cellInfos);
	size_t	columns	=	GetColumns (row);
	cellInfos->clear ();
	for (size_t c = 0; c < columns; ++c) {
		size_t	vCol	=	c + fStartCol;
		if (fRealTable.GetCellFlags (vRow, vCol) == WordProcessor::Table::ePlainCell) {
			CellInfo	cellInfo;
			cellInfo.f_clcbpat = fRealTable.GetCellColor (vRow, vCol);
			cellInfo.f_cellx = fRealTable.GetColumnWidth (vRow, vCol);
			cellInfos->push_back (cellInfo);
		}
		else {
			// if any previous real cell, append this guys colwidth to him...
			if (not cellInfos->empty ()) {
				cellInfos->back ().f_cellx += fRealTable.GetColumnWidth (vRow, vCol);
			}
		}
	}
}

StyledTextIOWriter::SrcStream*	WordProcessorTextIOSrcStream::TableIOMapper::MakeCellSubSrcStream (size_t row, size_t column)
{
	Led_Require (row < GetRows ());
	Led_Require (column < GetColumns (row));

	size_t	vRow	=	row + fStartRow;
	size_t	vCol	=	column + fStartCol;

	if (fRealTable.GetCellFlags (vRow, vCol) == WordProcessor::Table::ePlainCell) {
		TextStore*									ts	=	NULL;
		StandardStyledTextImager::StyleDatabasePtr	styleDatabase;
		WordProcessor::ParagraphDatabasePtr			paragraphDatabase;
		WordProcessor::HidableTextDatabasePtr		hidableTextDatabase;
		fRealTable.GetCellWordProcessorDatabases (vRow, vCol, &ts, &styleDatabase, &paragraphDatabase, &hidableTextDatabase);
		return new WordProcessorTextIOSrcStream (ts, styleDatabase, paragraphDatabase, hidableTextDatabase);
	}
	else {
		return NULL;
	}
}

size_t	WordProcessorTextIOSrcStream::TableIOMapper::GetOffsetEnd () const
{
	// The current implemenation of tables uses a single embedding object with a single sentinal character
	// for the entire table (no matter how many rows)
	return 1;
}

Led_TWIPS_Rect	WordProcessorTextIOSrcStream::TableIOMapper::GetDefaultCellMarginsForRow (size_t /*row*/) const
{
	// Right now - our table implemenation just has ONE value for the entire table
	Led_TWIPS_Rect	cellMargins	=	Led_TWIPS_Rect (Led_TWIPS (0), Led_TWIPS (0), Led_TWIPS (0), Led_TWIPS (0));
	fRealTable.GetDefaultCellMargins (&cellMargins.top, &cellMargins.left, &cellMargins.bottom, &cellMargins.right);
	return cellMargins;
}

Led_TWIPS_Rect	WordProcessorTextIOSrcStream::TableIOMapper::GetDefaultCellSpacingForRow (size_t /*row*/) const
{
	// Right now - our table implemenation just has ONE value for the entire table
	Led_TWIPS	cellSpacing	=	fRealTable.GetCellSpacing ();
	return Led_TWIPS_Rect (cellSpacing, cellSpacing, Led_TWIPS (0), Led_TWIPS (0));	// carefull - TLBR sb cellSpacing and last 2 args to Led_TWIPS_Rect::CTOR are height/width!
}








/*
 ********************************************************************************
 ********************* WordProcessorFlavorPackageInternalizer *******************
 ********************************************************************************
 */
typedef	WordProcessor::WordProcessorFlavorPackageInternalizer	WordProcessorFlavorPackageInternalizer;

WordProcessorFlavorPackageInternalizer::WordProcessorFlavorPackageInternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase,
												const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
												const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase
											):
	FlavorPackageInternalizer (ts),
	inherited (ts, styleDatabase),
	fOverwriteTableMode (false),
#if		!qNestedTablesSupported
	fNoTablesAllowed (false),
#endif
	fParagraphDatabase (paragraphDatabase),
	fHidableTextDatabase (hidableTextDatabase)
{
}

WordProcessor::StandardStyledTextIOSinkStream*	WordProcessorFlavorPackageInternalizer::mkStandardStyledTextIOSinkStream (size_t insertionStart)
{
	WordProcessorTextIOSinkStream*	sinkStream	= new WordProcessorTextIOSinkStream (PeekAtTextStore (), fStyleDatabase, fParagraphDatabase, fHidableTextDatabase, insertionStart);
	sinkStream->SetIgnoreLastParaAttributes (true);
	sinkStream->SetOverwriteTableMode (GetOverwriteTableMode ());
#if		!qNestedTablesSupported
	sinkStream->SetNoTablesAllowed (GetNoTablesAllowed ());
#endif

	return sinkStream;
}









/*
 ********************************************************************************
 **************************** WordProcessor::WPPartition ************************
 ********************************************************************************
 */
WordProcessor::WPPartition::WPPartition (TextStore& textStore, MarkerOwner& tableMarkerOwner):
	inherited (textStore, eSpecialHackToDisableInit),
	fTableMarkerOwner (tableMarkerOwner)
{
	FinalConstruct ();
	Invariant ();
}

vector<WordProcessor::Table*>	WordProcessor::WPPartition::GetTablesInRange (size_t from, size_t to) const
{
	if (to == static_cast<size_t> (-1)) {
		to = GetTextStore ().GetLength ();
	}
	Led_Require (from <= to);
	Led_Require (to <= GetTextStore ().GetLength () + 1);
	MarkersOfATypeMarkerSink2Vector<Table>	result;
	GetTextStore ().CollectAllMarkersInRangeInto (from, to, &fTableMarkerOwner, result);
	return result.fResult;
}

WordProcessor::Table*	WordProcessor::GetActiveTable () const
{
	size_t	selStart	=	0;
	size_t	selEnd		=	0;
	GetSelection (&selStart, &selEnd);
	if (selEnd - selStart == 1) {
		vector<WordProcessor::Table*>	tables	=	GetTablesInRange (selStart, selEnd);
		Led_Assert (tables.size () <= 1);
		if (tables.size () == 1) {
			Led_EnsureNotNil (tables[0]);
			return tables[0];
		}
	}
	return NULL;
}

void	WordProcessor::WPPartition::FinalConstruct ()
{
// MUST FIX SO WE DO SOMETHING HERE (old dohandleupdate code maybe eliminated)
	inherited::FinalConstruct ();
	DoHandleUpdateForTableRangeCheck (0, GetTextStore ().GetLength ());
}

void	WordProcessor::WPPartition::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
// cuz random ordering of whether table DidUpdateText() gets called first or PartitionElt::DidUpdateText () - so we msut
// do our checks HERE - to make sure size of table has been adjusted.
	{
		DoHandleUpdateForTableRangeCheck (updateInfo.fReplaceFrom, updateInfo.GetResultingRHS ());
	}

	inherited::DidUpdateText (updateInfo);
}

void	WordProcessor::WPPartition::DoHandleUpdateForTableRangeCheck (size_t from, size_t to) throw ()
{
	TextStore&	ts	=	GetTextStore ();

// must go one forward/back to make sure we get new chars inserted BEFORE a table or just after one
//	vector<Table*>	tables	=	GetTablesInRange (from, to);
	vector<Table*>	tables	=	GetTablesInRange (ts.FindPreviousCharacter (from), ts.FindNextCharacter (to));
	for (vector<Table*>::iterator i = tables.begin (); i != tables.end (); ++i) {
		Table*	t	=	*i;
		if (t->GetLength () != 0) {
			size_t	tableEnd	=	t->GetEnd ();
			// may need logic similar to that below
			// maybe try this:
			size_t				tableStart	=	t->GetStart ();
			PartitionMarker*	pm			=	GetPartitionMarkerContainingPosition (tableStart);
			/*
			 *	Since Partition::Split always leaves 'pm' pointing to the BEGINNING of the range, its OK to do two splits in a row
			 *	so long as we do the one further to the right first.
			 */
			if (tableEnd < pm->GetEnd () and tableEnd > pm->GetStart ()) {
				Split (pm, tableEnd);
			}
			if (tableStart > pm->GetStart () and tableStart < pm->GetEnd ()) {
				Split (pm, tableStart);
			}

			// See if after insertion of that text this PM needs to be coalesed with the next
			bool	coalesce	=	NeedToCoalesce (pm);
			if (coalesce) {
				Coalece (pm);		// 'pm' is DELETED BY THIS SO DO NOTHING to it AFTERWARDS!!!
			}
			pm	=	pm->GetPrevious ();
			if (pm != NULL) {
				coalesce	=	NeedToCoalesce (pm);
				if (coalesce) {
					Coalece (pm);		// 'pm' is DELETED BY THIS SO DO NOTHING to it AFTERWARDS!!!
				}
				pm	=	pm->GetPrevious ();
				if (pm != NULL) {
					coalesce	=	NeedToCoalesce (pm);
					if (coalesce) {
						Coalece (pm);		// 'pm' is DELETED BY THIS SO DO NOTHING to it AFTERWARDS!!!
					}
				}
			}
		}
	}

	PartitionMarker*	pm	=	GetPartitionMarkerContainingPosition (from);
	// See if after insertion of that text this PM needs to be coalesed with the next
	bool	coalesce	=	NeedToCoalesce (pm);
	if (coalesce) {
		Coalece (pm);		// 'pm' is DELETED BY THIS SO DO NOTHING to it AFTERWARDS!!!
	}
	pm	=	pm->GetPrevious ();
	if (pm != NULL) {
		coalesce	=	NeedToCoalesce (pm);
		if (coalesce) {
			Coalece (pm);		// 'pm' is DELETED BY THIS SO DO NOTHING to it AFTERWARDS!!!
		}
		pm	=	pm->GetPrevious ();
		if (pm != NULL) {
			coalesce	=	NeedToCoalesce (pm);
			if (coalesce) {
				Coalece (pm);		// 'pm' is DELETED BY THIS SO DO NOTHING to it AFTERWARDS!!!
			}
		}
	}
}

bool	WordProcessor::WPPartition::NeedToCoalesce (PartitionMarker* pm) throw ()
{
	Led_RequireNotNil (pm);

	bool	coalesce	=	inherited::NeedToCoalesce (pm);
	if (coalesce) {
		/*
		 *	If default implementation said to coalese - it could have been for good reasons, or bad. One good reason would be
		 *	an empty marker. Another would be if this marker didn't end with a table (and some other conditions were met).
		 *	We just need to make sure it wasn't mistaken becasue the PM either IS the end of a table or comes just before  one.
		 *	In those cases, we negate the decision of the default code.
		 */
		if (pm->GetLength () != 0) {
			size_t	end		=	pm->GetEnd ();
			size_t	trStart	=	end-1;
			size_t	trEnd	=	end;
			if (pm->GetNext () != NULL) {
				trEnd ++;
			}

			vector<Table*>	tables	=	GetTablesInRange (trStart, trEnd);
			if (not tables.empty ()) {
				if (tables.size () == 2) {
					// then we must split between the two and so NO need to coalese
					return false;
				}
				else if (tables.size () == 1) {
					Table*	table	=	tables[0];
					// If table contains this point - then coalese - otherwise dont
					if (table->GetStart () == pm->GetEnd ()) {
						return false;
					}
					else if (table->GetEnd () == pm->GetEnd ()) {
						return false;
					}
				}
			}
		}
	}
	return coalesce;
}

#if		qDebug
void	WordProcessor::WPPartition::Invariant_ () const
{
	Partition::Invariant_ ();	//	Cannot call LineBasedPartition::Invariant_ () - AKA inherited::Invariant_ () because
								//	that assumes when a PM ends its cuz of a newline. BUT - it COULD be because of a
								//	table instead.

	/*
	 *	Assure that for ALL PMs, there are no tables in the middle, and no newlines, and that the ends
	 *	of  PMs are marked by either the EOB, or a newline or a table.
	 */
	for (PartitionMarker* cur = GetFirstPartitionMarker (); cur != NULL; cur = cur->GetNext ()) {
		Led_AssertNotNil (cur);
		size_t	start	=	cur->GetStart ();
		size_t	end		=	cur->GetEnd ();
		size_t	len		=	end-start;

		if (end > GetEnd ()) {
			len--;	// Last partition extends past end of text
		}
		Led_SmallStackBuffer<Led_tChar>	buf (len);
		CopyOut (start, len, buf);
		for (size_t i = 1; i < len; i++) {
			Led_Assert (buf[i-1] != '\n');
			vector<Table*>	tables	=	GetTablesInRange (start+i-1, start+i);
			if (not tables.empty ()) {
				Led_Assert (tables.size () == 1);
				Table*	t	=	tables[0];
				if (t->GetLength () != 0) {
					Led_Assert (t->GetStart () == start);
					Led_Assert (t->GetLength () == len);
				}
			}
		}
		if (cur->GetNext () != NULL) {		// All but the last partition must be NL terminated...
			Led_Assert (buf[len-1] == '\n' or
						(not GetTablesInRange (start+len-1, start+len).empty ()) or
						(start + len + 1 <= GetEnd () and not GetTablesInRange (start+len, start+len+1).empty ())
						);
		}
	}
	/*
	 *	Assure that for ALL existing tables, their starts and ends correspond to PM start/ends.
	 */
	vector<Table*>	tables	=	GetTablesInRange (0, GetTextStore ().GetLength ());
	for (vector<Table*>::iterator i = tables.begin (); i != tables.end (); ++i) {
		Table*				t	=	*i;
		if (t->GetLength () != 0) {
			PartitionMarker*	pm	=	GetPartitionMarkerContainingPosition (t->GetStart ());
			Led_Assert (t->GetStart () == pm->GetStart ());
			Led_Assert (t->GetEnd () == pm->GetEnd ());
		}
	}
}
#endif







/*
 ********************************************************************************
 ********************* WordProcessorFlavorPackageExternalizer *******************
 ********************************************************************************
 */
typedef	WordProcessor::WordProcessorFlavorPackageExternalizer	WordProcessorFlavorPackageExternalizer;

WordProcessorFlavorPackageExternalizer::WordProcessorFlavorPackageExternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase,
												const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
												const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase
											):
	FlavorPackageExternalizer (ts),
	inherited (ts, styleDatabase),
	fUseTableSelection (false),
	fParagraphDatabase (paragraphDatabase),
	fHidableTextDatabase (hidableTextDatabase)
{
}

WordProcessor::StandardStyledTextIOSrcStream*	WordProcessorFlavorPackageExternalizer::mkStandardStyledTextIOSrcStream (size_t selectionStart, size_t selectionEnd)
{
	WordProcessorTextIOSrcStream*	stream	=	new WordProcessorTextIOSrcStream (PeekAtTextStore (), fStyleDatabase, fParagraphDatabase, fHidableTextDatabase, selectionStart, selectionEnd);
	stream->SetUseTableSelection (GetUseTableSelection ());
	return stream;
}








/*
 ********************************************************************************
 ************************************* Table ************************************
 ********************************************************************************
 */
	typedef	WordProcessor::Table	Table;

	class	WordProcessor::Table::TableCMD	: public InteractiveReplaceCommand {
		private:
			typedef	InteractiveReplaceCommand	inherited;

		public:
			LED_DECLARE_USE_BLOCK_ALLOCATION(TableCMD);

		public:
			TableCMD (size_t tableAt, size_t tRow, size_t tCol, SavedTextRep* beforeRegion, SavedTextRep* afterRegion, size_t at, const Led_SDK_String& cmdName):
				inherited (beforeRegion, afterRegion, at, cmdName),
				fTableAt (tableAt),
				fTableRow (tRow),
				fTableColumn (tCol)
				{
				}

		public:
			override	void	Do (TextInteractor& interactor)
				{
					WordProcessor&	owningWP	=	dynamic_cast<WordProcessor&> (interactor);
					Table*			aT			=	owningWP.GetTableAt (fTableAt);
					Led_AssertNotNil (aT);
					Table::TemporarilySetOwningWP				owningWPSetter (*aT, owningWP);
					Table::TemporarilyAllocateCellWithTablet	wp (*aT, fTableRow, fTableColumn);
					inherited::Do (*wp);
				}
			override	void	UnDo (TextInteractor& interactor)
				{
					WordProcessor&	owningWP	=	dynamic_cast<WordProcessor&> (interactor);
					Table*			aT			=	owningWP.GetTableAt (fTableAt);
					Led_AssertNotNil (aT);
					Table::TemporarilySetOwningWP				owningWPSetter (*aT, owningWP);
					Table::TemporarilyAllocateCellWithTablet	wp (*aT, fTableRow, fTableColumn);
					inherited::UnDo (*wp);
				}
			override	void	ReDo (TextInteractor& interactor)
				{
					WordProcessor&	owningWP	=	dynamic_cast<WordProcessor&> (interactor);
					Table*			aT			=	owningWP.GetTableAt (fTableAt);
					Led_AssertNotNil (aT);
					Table::TemporarilySetOwningWP				owningWPSetter (*aT, owningWP);
					Table::TemporarilyAllocateCellWithTablet	wp (*aT, fTableRow, fTableColumn);
					inherited::ReDo (*wp);
				}

		protected:
			size_t	fTableAt;
			size_t	fTableRow;
			size_t	fTableColumn;
	};



/*
 *	can only be called inside the context of WordProcessor::Table::TemporarilySetOwningWP
 *	since that is what provides our external (window) coordinate system
 */
#define	Led_Require_CurrentOwningWP()\
			Led_RequireNotNil (fCurrentOwningWP)


/*
@METHOD:		WordProcessor::Table::Table
@DESCRIPTION:	<p>You generally don't construct a table object directly, but rather using 
			@'WordProcessor::InsertTable'.</p>
*/
Table::Table (WordProcessor::AbstractParagraphDatabaseRep* tableOwner, size_t addAt):
	inherited (),
	fCellSpacing (Led_TWIPS (0)),
	fDefaultCellMargins (Led_TWIPS (15), Led_TWIPS (90), Led_TWIPS (0), Led_TWIPS (0)),	// LHS and RHS both 90 TWIPS (tricky CTOR - last arg is WIDTH - not RHS).
	fTrackingAnchor_Row (0),
	fTrackingAnchor_Col (0),
	fSuppressCellUpdatePropagationContext (false),
	fAllowUpdateInfoPropagationContext (false),
	fCellUpdatePropationUpdater (NULL),
	fRowSelStart (0),
	fRowSelEnd (0),
	fColSelStart (0),
	fColSelEnd (0),
	fIntraCellMode (false),
	fIntraSelStart (0),
	fIntraSelEnd (0),
	fIntraCellDragAnchor (0),
	fSavedLeftSideOfSelectionInteresting (false),
	fSavedIntraCellSelectionEmptySelFontSpecification (),
	fSavedIntraCellInfoValid (false),
	fCurrentOwningWP (NULL),
	fNeedLayout (eNeedFullLayout),
	fRows (),
	fBorderWidth (Led_CvtScreenPixelsToTWIPSH (1)),
	fBorderColor (Led_Color::kSilver),
	fTotalWidth (0),
	fTotalHeight (0)
{
	FinalizeAddition (tableOwner, addAt);
}

Table::~Table ()
{
	if (fCellUpdatePropationUpdater != NULL) {
		Led_Assert (false);		// This should only happen if an earlier update was aborted (throw). NOT really a bug
								// if this gets triggered. Just for informational purposes (debugging) only
		fCellUpdatePropationUpdater->Cancel ();
		delete fCellUpdatePropationUpdater;
	}
	Led_Assert (fCurrentOwningWP == NULL);
}

void	Table::FinalizeAddition (WordProcessor::AbstractParagraphDatabaseRep* o, size_t addAt)
{
	Led_RequireNotNil (o);
	TextStore&					ts	=	o->GetTextStore ();
	TextStore::SimpleUpdater	updater (ts, addAt, addAt + 1);
	ts.ReplaceWithoutUpdate (addAt, addAt, &kEmbeddingSentinalChar, 1);
	ts.AddMarker (this, addAt, 1, o);
}

void	Table::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
											Led_Coordinate /*useBaseLine*/, Led_Distance* pixelsDrawn
										)
{
	Led_RequireMember (const_cast<StyledTextImager*> (imager), WordProcessor);
	Led_Assert (from + 1 == to);
	Led_RequireNotNil (text.PeekAtVirtualText ());
	Led_Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);

	typedef	EmbeddedTableWordProcessor::TemporarilyUseTablet	TemporarilyUseTablet;

	WordProcessor&	owningWP	=	*dynamic_cast<WordProcessor*> (const_cast<StyledTextImager*> (imager));

	Table::TemporarilySetOwningWP	owningWPSetter (*this, owningWP);

	Led_Distance	bwv		=	Led_CvtScreenPixelsFromTWIPSV (fBorderWidth);
	Led_Rect		rowRect	=	drawInto;

	size_t	nRows	=	fRows.size ();
	for (size_t ri = 0; ri < nRows; ++ri) {
		// MUST FIX THIS FOR MULTI-ROW CELLS!!! -- maybe????
		// vertical merge cells will NOT be supported for Led 3.1 -- LGP 2003-04-17
		size_t	nCols	=	GetColumnCount (ri);
		rowRect.bottom = rowRect.top + fRows[ri].fHeight;
		for (size_t ci = 0; ci < nCols; ci++) {
			if (GetCellFlags (ri, ci) == ePlainCell) {
				Led_Rect	scrolledCBWR		=	TableCoordinates2Window (GetCellBounds (ri, ci));
				if (Intersect (scrolledCBWR, invalidRect)) {
					Led_Rect					scrolledEditorCBWR	=	TableCoordinates2Window (GetCellEditorBounds (ri, ci));

					// SPR#1485: erase the cell margins as well as the cell editor rectangle...
					// We can erase the whole rect - cuz we do it before we draw the cell editor portion itself...
					if (scrolledCBWR != scrolledEditorCBWR) {
						tablet->EraseBackground_SolidHelper (scrolledCBWR, GetCellColor (ri, ci));
					}

					TemporarilyAllocateCellWP	wp (*this, owningWP, ri, ci, scrolledEditorCBWR);
					TemporarilyUseTablet		tmpUseTablet (*wp, tablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall);
					wp->Draw (scrolledEditorCBWR, false);
					DrawCellBorders (tablet, ri, ci, scrolledCBWR);
				}
				else {
					// If the table elements bottom is above the invalid region or
					// the tables top is BELOW the invalid region, we can skip this entire row. If
					// its BELOW - we can even skip any successive rows
					if (scrolledCBWR.bottom < invalidRect.top) {
						break;
					}
					if (scrolledCBWR.top > invalidRect.bottom) {
						goto Done;
					}
				}
			}
		}
		rowRect.top = rowRect.bottom + bwv;
	}

Done:
	DrawTableBorders (owningWP, tablet, drawInto);

	if (pixelsDrawn != NULL) {
		*pixelsDrawn = fTotalWidth;
	}
}

void	Table::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to,
												const Led_tChar* text,
												Led_Distance* distanceResults
											) const
{
	Led_RequireMember (const_cast<StyledTextImager*> (imager), WordProcessor);
	Led_Assert (from + 1 == to);
	Led_RequireNotNil (text);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);

	distanceResults[0] = fTotalWidth;
}

Led_Distance	Table::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to) const
{
	Led_RequireMember (const_cast<StyledTextImager*> (imager), WordProcessor);
	Led_Assert (from + 1 == to);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);

	// don't return zero-height as that could cause problems... even if not layed out yet...
// LGP 2003-03-17 - not sure - maybe its OK to return zero if not layed out yet??
	return fTotalHeight==0? 1: fTotalHeight;
}

/*
@METHOD:		WordProcessor::Table::GetRowHilightRects
@DESCRIPTION:	<p>Provide table-specific selection hilight behavior (so only the selected cells
			or rows or columns are hilighted)</p>
*/
vector<Led_Rect>	Table::GetRowHilightRects () const
{
	Led_Require_CurrentOwningWP ();

	vector<Led_Rect>	result;

	size_t	rowStart			=	GetStart ();
	size_t	rowEnd				=	GetEnd ();
	size_t	hilightStart		=	fCurrentOwningWP->GetSelectionStart ();
	size_t	hilightEnd			=	fCurrentOwningWP->GetSelectionEnd ();
	bool	segmentHilighted	=	max (rowStart, hilightStart) < min (rowEnd, hilightEnd);

	if (segmentHilighted) {
		Led_Rect			tableRect		=	fCurrentOwningWP->GetIntraRowTextWindowBoundingRect (rowStart, rowEnd);
		vector<Led_Rect>	hilightRects	=	fCurrentOwningWP->TextImager::GetRowHilightRects (
																TextLayoutBlock_Basic (&kEmbeddingSentinalChar, &kEmbeddingSentinalChar+1),
																rowStart, rowEnd,
																hilightStart, hilightEnd
															);


		// If all the WHOLE table is hilighted, then display that selection as the entire table hilighted.
		// No need to walk through just the cells etc...
		if (rowStart != hilightStart or rowEnd != hilightEnd) {
			return hilightRects;
		}

		/*
		 *	Add the hilight rect BEFORE and AFTER (if needed) the table - but leave out the
		 *	table rect itself to handle separately.
		 */
		for (vector<Led_Rect>::const_iterator i = hilightRects.begin (); i != hilightRects.end (); ++i) {
			if (tableRect != *i) {
				if (not (*i).IsEmpty ()) {
					result.push_back (*i);
				}
			}
		}

		/*
		 *	Add the actual table's hilight.
		 */
		{
			size_t	rowSelStart		=	0;
			size_t	rowSelEnd		=	0;
			size_t	colSelStart		=	0;
			size_t	colSelEnd		=	0;
			GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);

			// If all the cells hilighted, then display that selection as the entire table hilighted.
			if (rowSelStart == 0 and rowSelEnd == GetRowCount () and
				colSelStart == 0 and colSelEnd == GetColumnCount ()
				) {
				return hilightRects;
			}


			if (rowSelEnd - rowSelStart == 1 and colSelEnd - colSelStart == 1 and GetIntraCellMode ()) {
				TemporarilyAllocateCellWithTablet	wp (*const_cast<Table*> (this), rowSelStart, colSelStart);
				vector<Led_Rect>	cellHilightRegions	=	wp->GetSelectionWindowRects (wp->GetSelectionStart (), wp->GetSelectionEnd ());
				for (vector<Led_Rect>::const_iterator i = cellHilightRegions.begin (); i != cellHilightRegions.end (); ++i) {
					result.push_back (*i);
				}
			}
			else {
				for (size_t ri = rowSelStart; ri < rowSelEnd; ++ri) {
					size_t	thisRowEnd	=	min (colSelEnd, GetColumnCount (ri));
					for (size_t ci = colSelStart; ci < thisRowEnd; ++ci) {
						if (GetCellFlags (ri, ci) == ePlainCell) {
	// doesn't include cell margins/borders...
							Led_Rect	wRelCellRect	=	TableCoordinates2Window (GetCellBounds (ri, ci));
							if (not wRelCellRect.IsEmpty ()) {
								result.push_back (wRelCellRect);
							}
						}
					}
				}
			}
		}
	}

	#if		qDebug
		{
			// Make sure rectangles don't overlap with one another (can share an edge) -- SPR#1226
			for (vector<Led_Rect>::const_iterator orit = result.begin (); orit != result.end (); ++orit) {
				Led_Ensure ((*orit).GetWidth () > 0);
				Led_Ensure ((*orit).GetHeight () > 0);
				for (vector<Led_Rect>::const_iterator irit = orit+1; irit != result.end (); ++irit) {
					Led_Rect	hr	=	*irit;
					Led_Ensure (hr.GetWidth () > 0);
					Led_Ensure (hr.GetHeight () > 0);
					Led_Ensure (not Intersect (hr, *orit));
				}
			}
		}
	#endif

	return result;
}

/*
@METHOD:		WordProcessor::Table::DrawTableBorders
@ACCESS:		protected
@DESCRIPTION:	<p></p>
*/
void		Table::DrawTableBorders (WordProcessor& owningWP, Led_Tablet tablet, const Led_Rect& drawInto)
{
	#if 0
		//Don't delete this code - cuz we MAY want to display (somehow) the border for the table as
		// a whole in some special color when it is SELECTED!

		// Also - this is REALLY needed - when we have spacing (and in the future we may have other stuff drawn to adorn the
		// table - like I said above - like a funny color for hilight state or something???
		return;
	#endif
	Led_Distance	bwh		=	Led_CvtScreenPixelsFromTWIPSH (fBorderWidth);
	Led_Distance	bwv		=	Led_CvtScreenPixelsFromTWIPSV (fBorderWidth);

	Led_Rect		bounds	=	drawInto - Led_Point (0, owningWP.GetHScrollPos ());
	bounds.right = bounds.left + fTotalWidth;
	bounds.bottom = bounds.top + fTotalHeight;
	tablet->FrameRectangle (bounds, fBorderColor, bwh);
}

/*
@METHOD:		WordProcessor::Table::DrawCellBorders
@ACCESS:		protected
@DESCRIPTION:	<p>Draw the borders on the given cell with bounds (in tablet drawing coordinates - corrected for scrolling
			and the location of the table of the given cell). Note it is assumed the cellBounds argument does NOT take
			into account space for the border itself. We draw the border just OUTSIDE the cell.</p>
*/
void		Table::DrawCellBorders (Led_Tablet tablet, size_t /*row*/, size_t /*column*/, const Led_Rect& cellBounds)
{
	Led_Coordinate	bw		=	Led_CvtScreenPixelsFromTWIPSH (fBorderWidth);
	// Draw outside of the frame of the cell.
	tablet->FrameRectangle (InsetRect (cellBounds, -bw, -bw), fBorderColor, bw);
}

/*
@METHOD:		WordProcessor::Table::GetCellBounds
@ACCESS:		public
@DESCRIPTION:	<p>Retrieve the bounding rectangle for the given cell, NOT including its border.
			The rectange is relative to the table itself. Note that the border is drawn
			just outside the cell bounds.</p>
				<p>See also @'WordProcessor::Table::GetCellEditorBounds'</p>
*/
Led_Rect	Table::GetCellBounds (size_t row, size_t column) const
{
	Led_Require (GetCellFlags (row, column) == ePlainCell);
	return GetCell (row, column).GetCachedBoundsRect ();
}

/*
@METHOD:		WordProcessor::Table::GetCellEditorBounds
@ACCESS:		public
@DESCRIPTION:	<p>Similar to @'WordProcessor::Table::GetCellBounds' but it takes into account the cell margin,
			and insets the cell bounds to return just where the embedded WP bounds lie.</p>
*/
Led_Rect	Table::GetCellEditorBounds (size_t row, size_t column) const
{
	Led_Require (GetCellFlags (row, column) == ePlainCell);
	Led_Rect	cellBounds		=	GetCellBounds (row, column);
	Led_Rect	cellEditBounds	=	cellBounds;
	Led_TWIPS_Rect	defaultCellMarginTWIPS;
	GetDefaultCellMargins (&defaultCellMarginTWIPS.top, &defaultCellMarginTWIPS.left, &defaultCellMarginTWIPS.bottom, &defaultCellMarginTWIPS.right);
	cellEditBounds.top += Led_CvtScreenPixelsFromTWIPSV (defaultCellMarginTWIPS.top);
	cellEditBounds.left += Led_CvtScreenPixelsFromTWIPSH (defaultCellMarginTWIPS.left);
	cellEditBounds.bottom -= Led_CvtScreenPixelsFromTWIPSV (defaultCellMarginTWIPS.bottom);
	cellEditBounds.right -= Led_CvtScreenPixelsFromTWIPSH (defaultCellMarginTWIPS.right);
	// now assure bounds not empty...
	cellEditBounds.bottom = max (cellEditBounds.bottom, cellEditBounds.top + 1);
	cellEditBounds.right = max (cellEditBounds.right, cellEditBounds.left + 1);
	return cellEditBounds;
}

/*
@METHOD:		WordProcessor::Table::GetClosestCell
@ACCESS:		public
@DESCRIPTION:	<p>Point 'p' must be relative to the table bounds itself.</p>
*/
void	Table::GetClosestCell (const Led_Point& p, size_t* row, size_t* col) const
{
	Led_RequireNotNil (row);
	Led_RequireNotNil (col);

	Led_Size		border	=	Led_Size (Led_CvtScreenPixelsFromTWIPSV (fBorderWidth), Led_CvtScreenPixelsFromTWIPSH (fBorderWidth));
	Led_Distance	spacing	=	Led_CvtScreenPixelsFromTWIPSV (GetCellSpacing ());

	// find row...
	size_t			rowCount	=	GetRowCount ();
	Led_Assert (rowCount > 0);
	Led_Coordinate	top			=	spacing + border.v;
	size_t			ri			=	0;
	for (; ri < rowCount; ri++) {
		Led_Distance	h	=	fRows[ri].fHeight;
		Led_Coordinate	bottom	=	top + h;
		// Treat special case of above entire table as being row zero..
		if (p.v < bottom) {
			break;
		}
		top += h;
		top += spacing + border.v;
	}
	if (ri >= rowCount) {	// if PAST end of table - then treat that as the last row
		ri = rowCount - 1;
	}
	*row = ri;

	// Now find the right column (cell)
	size_t			colCount	=	GetColumnCount (ri);
	Led_Assert (colCount > 0);
	size_t			ci			=	0;
	for (; ci < colCount; ci++) {
		size_t	rri	=	ri;
		size_t	cci	=	ci;
		GetRealCell (&rri, &cci);
		Led_Rect	bounds	=	GetCellBounds (rri, cci);
		// Treat special case of above entire table as being row zero..
		if (p.h < bounds.GetRight ()) {
			break;
		}
	}
	if (ci >= colCount) {
		ci = colCount - 1;
	}
	*col = ci;
}

Led_Point	Table::TableCoordinates2Window (const Led_Point& p) const
{
	Led_Require_CurrentOwningWP ();
	Led_Point	tableWROrigin	=	fCurrentOwningWP->GetCharWindowLocation (GetStart ()).GetTopLeft ();
	return p + tableWROrigin;
}

Led_Rect	Table::TableCoordinates2Window (const Led_Rect& r) const
{
	return Led_Rect (TableCoordinates2Window (r.GetOrigin ()), r.GetSize ());
}

Led_Point	Table::WindowCoordinates2Table (const Led_Point& p) const
{
	Led_Require_CurrentOwningWP ();
	Led_Point	tableWROrigin	=	fCurrentOwningWP->GetCharWindowLocation (GetStart ()).GetTopLeft ();
	return p - tableWROrigin;
}

Led_Rect	Table::WindowCoordinates2Table (const Led_Rect& r) const
{
	return Led_Rect (WindowCoordinates2Table (r.GetOrigin ()), r.GetSize ());
}

bool	Table::GetCaretShownSituation () const
{
	if (GetIntraCellMode ()) {
		size_t	selStart	=	0;
		size_t	selEnd		=	0;
		GetIntraCellSelection (&selStart, &selEnd);
		return selStart == selEnd;
	}
	return false;
}

/*
@METHOD:		Table::CalculateCaretRect
@DESCRIPTION:	<p></p>
*/
Led_Rect	Table::CalculateCaretRect () const
{
	Led_Require_CurrentOwningWP ();
	if (GetIntraCellMode ()) {
		size_t	selStart	=	0;
		size_t	selEnd		=	0;
		GetIntraCellSelection (&selStart, &selEnd);
		if (selStart == selEnd) {
			size_t	row	=	0;
			size_t	col	=	0;
			(void)GetIntraCellMode (&row, &col);
			TemporarilyAllocateCellWithTablet	wp (*const_cast<Table*> (this), row, col);
			return wp->CalculateCaretRect ();
		}
	}
	return (Led_Rect (0, 0, 0, 0));
}

bool	Table::OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed)
{
	Led_Require_CurrentOwningWP ();

	size_t	rowSelStart	=	0;
	size_t	rowSelEnd	=	0;
	size_t	colSelStart	=	0;
	size_t	colSelEnd	=	0;
	GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);

	if (theChar == '\b') {
		// Treat a selection of the entire table and a hackspace as deleting the entire table...
		if (rowSelStart == 0 and rowSelEnd == GetRowCount () and
			colSelStart == 0 and colSelEnd == GetColumnCount () and
			not fIntraCellMode
			) {
			return false;	// so will be handled by higher level - deleting the entire table.
		}
	}

	InteractiveModeUpdater				iuMode (*fCurrentOwningWP);
	AllowUpdateInfoPropagationContext	AUIPC (*this);
	if (not fIntraCellMode) {
		// save all the cleared text for all the selected cells in one command object, but give it the
		// typingCommand name so it will be lumped with the typeing command generated by the below wp->OnTypedNormalCharacter ().
		UndoableContextHelper	undoContext (*fCurrentOwningWP, TextInteractor::GetCommandNames ().fTypingCommandName, false);
			{
				(void)OnPerformCommand_ApplyToEachSelectedCell (TextInteractor::kClear_CmdID, false);
			}
		undoContext.CommandComplete ();
		SetIntraCellMode (rowSelStart, colSelStart);
	}

	Led_Assert (fIntraCellMode);
	TemporarilyAllocateCellWithTablet	wp (*this, rowSelStart, colSelStart);
	wp->OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, altKeyPressed);

	return true;	// handled
}

bool	Table::DoSingleCharCursorEdit (TextInteractor::CursorMovementDirection direction, TextInteractor::CursorMovementUnit movementUnit, TextInteractor::CursorMovementAction action,
											TextInteractor::UpdateMode updateMode, bool scrollToSelection
										)
{
	size_t	row	=	0;
	size_t	col	=	0;
	if (GetIntraCellMode (&row, &col)) {
// VERY PRELIMINARY!!!
		AllowUpdateInfoPropagationContext	AUIPC (*this);
		TemporarilyAllocateCellWithTablet	wp (*this, row, col);
		wp->DoSingleCharCursorEdit (direction, movementUnit, action, updateMode, scrollToSelection);
		return true;	// handled
	}
	return false;
}

bool	Table::OnUpdateCommand (TextInteractor::CommandUpdater* enabler)
{
	Led_Require_CurrentOwningWP ();
	Led_RequireNotNil (enabler);
	
	size_t	row	=	0;
	size_t	col	=	0;
	if (GetIntraCellMode (&row, &col)) {
		if (fCurrentOwningWP->PassAlongCommandToIntraCellModeTableCell (enabler->GetCmdID ())) {
			TemporarilyAllocateCellWithTablet	wp (*this, row, col);
			bool	result	=	wp->OnUpdateCommand (enabler);
			if (enabler->GetCmdID () == kSelectedEmbeddingProperties_CmdID and not enabler->GetEnabled ()) {
				// SPR#1487: so default command handling will take care of it and we'll see the properties command
				return false;
			}
			return true;	// if in a table cell - say the command was eaten here regardless- cut off other commands
		}
	}

	switch (enabler->GetCmdID ()) {
		case	kCut_CmdID:						{	OnUpdateCutCommand (enabler);						return true;		}
		case	kInsertTableRowAbove_CmdID:		{	OnUpdateInsertTableRowAboveCommand (enabler);		return true;		}
		case	kInsertTableRowBelow_CmdID:		{	OnUpdateInsertTableRowBelowCommand (enabler);		return true;		}
		case	kInsertTableColBefore_CmdID:	{	OnUpdateInsertTableColBeforeCommand (enabler);		return true;		}
		case	kInsertTableColAfter_CmdID:		{	OnUpdateInsertTableColAfterCommand (enabler);		return true;		}
		case	kRemoveTableColumns_CmdID:		{	OnUpdateRemoveTableColumnsCommand (enabler);		return true;		}
		case	kRemoveTableRows_CmdID:			{	OnUpdateRemoveTableRowsCommand (enabler);			return true;		}
		case	kSelectTableIntraCellAll_CmdID:
		case	kSelectTableCell_CmdID:
		case	kSelectTableRow_CmdID:
		case	kSelectTableColumn_CmdID:
		case	kSelectTable_CmdID:				{	OnUpdateSelectTablePartsCommand (enabler);			return true;		}
	}

	if (fCurrentOwningWP->PassAlongCommandToEachSelectedTableCell (enabler->GetCmdID ())) {
		return OnUpdateCommand_ApplyToEachSelectedCell (enabler);
	}

	return false;
}

bool	Table::OnPerformCommand (TextInteractor::CommandNumber commandNumber)
{
	Led_Require_CurrentOwningWP ();

	AllowUpdateInfoPropagationContext	AUIPC (*this);

	size_t	row	=	0;
	size_t	col	=	0;
	if (GetIntraCellMode (&row, &col)) {
		if (fCurrentOwningWP->PassAlongCommandToIntraCellModeTableCell (commandNumber)) {
			TemporarilyAllocateCellWithTablet	wp (*this, row, col);
			return wp->OnPerformCommand (commandNumber);
		}
	}

	switch (commandNumber) {
		case	kCut_CmdID:						{	OnCutCommand ();							return true;		}
		case	kInsertTableRowAbove_CmdID:		{	OnInsertTableRowAboveCommand ();			return true;		}
		case	kInsertTableRowBelow_CmdID:		{	OnInsertTableRowBelowCommand ();			return true;		}
		case	kInsertTableColBefore_CmdID:	{	OnInsertTableColBeforeCommand ();			return true;		}
		case	kInsertTableColAfter_CmdID:		{	OnInsertTableColAfterCommand ();			return true;		}
		case	kRemoveTableColumns_CmdID:		{	OnRemoveTableColumnsCommand ();				return true;		}
		case	kRemoveTableRows_CmdID:			{	OnRemoveTableRowsCommand ();				return true;		}
		case	kSelectTableIntraCellAll_CmdID:
		case	kSelectTableCell_CmdID:
		case	kSelectTableRow_CmdID:
		case	kSelectTableColumn_CmdID:
		case	kSelectTable_CmdID:				{	OnPerformTablePartsCommand (commandNumber);	return true;		}
	}

	if (fCurrentOwningWP->PassAlongCommandToEachSelectedTableCell (commandNumber)) {
		return OnPerformCommand_ApplyToEachSelectedCell (commandNumber);
	}

	return false;
}

void	Table::BreakInGroupedCommands ()
{
	Led_Require_CurrentOwningWP ();
	fCurrentOwningWP->BreakInGroupedCommands ();
}

bool	Table::OnUpdateCommand_ApplyToEachSelectedCell (TextInteractor::CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_Require_CurrentOwningWP ();

	bool	result	=	false;
	size_t	rowSelStart	=	0;
	size_t	rowSelEnd	=	0;
	size_t	colSelStart	=	0;
	size_t	colSelEnd	=	0;
	GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
	for (size_t ri = rowSelStart; ri < rowSelEnd; ++ri) {
		size_t	thisRowEnd	=	min (colSelEnd, GetColumnCount (ri));
		for (size_t ci = colSelStart; ci < thisRowEnd; ++ci) {
			TemporarilyAllocateCellWithTablet	wp (*this, ri, ci);
			wp->SetSelection (0, wp->GetEnd (), TextInteractor::eNoUpdate);
			result = result or wp->OnUpdateCommand (enabler);
		}
	}
	return result;
}

bool	Table::OnPerformCommand_ApplyToEachSelectedCell (TextInteractor::CommandNumber commandNumber, bool captureChangesForUndo)
{
	Led_Require_CurrentOwningWP ();
	if (captureChangesForUndo) {
		fCurrentOwningWP->BreakInGroupedCommands ();
	}
	bool	result	=	false;
	size_t	rowSelStart	=	0;
	size_t	rowSelEnd	=	0;
	size_t	colSelStart	=	0;
	size_t	colSelEnd	=	0;
	GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
	for (size_t ri = rowSelStart; ri < rowSelEnd; ++ri) {
		size_t	thisRowEnd	=	min (colSelEnd, GetColumnCount (ri));
		for (size_t ci = colSelStart; ci < thisRowEnd; ++ci) {
			TemporarilyAllocateCellWithTablet	wp (*this, ri, ci, captureChangesForUndo);
			wp->SetSelection (0, wp->GetEnd (), TextInteractor::eNoUpdate);
			TextInteractor::SupressCommandBreaksContext	SCBC (*wp);
			wp->OnPerformCommand (commandNumber);
			result = true;
		}
	}
	if (captureChangesForUndo) {
		fCurrentOwningWP->BreakInGroupedCommands ();
	}
	return result;
}

void	Table::OnUpdateCutCommand (TextInteractor::CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_Require_CurrentOwningWP ();
	size_t	rowSelStart	=	0;
	size_t	rowSelEnd	=	0;
	size_t	colSelStart	=	0;
	size_t	colSelEnd	=	0;
	GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
	enabler->SetEnabled (rowSelStart != rowSelEnd and colSelStart != colSelEnd);
}

void	Table::OnCutCommand ()
{
	Led_Require_CurrentOwningWP ();
	AllowUpdateInfoPropagationContext	AUIPC (*this);
	InteractiveModeUpdater				iuMode (*fCurrentOwningWP);
	Led_Assert (fCurrentOwningWP->GetSelectionEnd () - fCurrentOwningWP->GetSelectionStart () == 1);
	fCurrentOwningWP->OnCopyCommand ();
	UndoableContextHelper	undoContext (*fCurrentOwningWP, TextInteractor::GetCommandNames ().fCutCommandName, true);
		{
			(void)OnPerformCommand_ApplyToEachSelectedCell (TextInteractor::kClear_CmdID, false);
		}
	undoContext.CommandComplete ();
}

void	Table::OnUpdateInsertTableRowAboveCommand (TextInteractor::CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_Require_CurrentOwningWP ();
	enabler->SetEnabled (true);
}

void	Table::OnInsertTableRowAboveCommand ()
{
	Led_Require_CurrentOwningWP ();
	AllowUpdateInfoPropagationContext	AUIPC (*this);
	InteractiveModeUpdater				iuMode (*fCurrentOwningWP);
	BreakInGroupedCommands ();
	UndoableContextHelper	context (*fCurrentOwningWP, WordProcessor::GetCommandNames ().fInsertTableRowAboveCommandName, false);
		{
			// See our current row
			size_t	curRow	=	0;
			GetCellSelection (&curRow, NULL, NULL, NULL);

			Led_Assert (curRow <= GetRowCount ());
			InsertRow (curRow);
		}
	context.CommandComplete ();
	BreakInGroupedCommands ();
}

void	Table::OnUpdateInsertTableRowBelowCommand (TextInteractor::CommandUpdater* enabler)
{
	Led_RequireNotNil (enabler);
	Led_Require_CurrentOwningWP ();
	enabler->SetEnabled (true);
}

void	Table::OnInsertTableRowBelowCommand ()
{
	Led_Require_CurrentOwningWP ();
	AllowUpdateInfoPropagationContext	AUIPC (*this);
	InteractiveModeUpdater				iuMode (*fCurrentOwningWP);
	BreakInGroupedCommands ();
	UndoableContextHelper	context (*fCurrentOwningWP, WordProcessor::GetCommandNames ().fInsertTableRowAboveCommandName, false);
		{
			// See our current row
			size_t	curRow	=	0;
			GetCellSelection (NULL, &curRow, NULL, NULL);
			Led_Assert (curRow <= GetRowCount ());
			InsertRow (curRow);
		}
	context.CommandComplete ();
	BreakInGroupedCommands ();
}

void	Table::OnUpdateInsertTableColBeforeCommand (TextInteractor::CommandUpdater* enabler)
{
	Led_Require_CurrentOwningWP ();
	Led_RequireNotNil (enabler);
	enabler->SetEnabled (true);
}

void	Table::OnInsertTableColBeforeCommand ()
{
	Led_Require_CurrentOwningWP ();
	AllowUpdateInfoPropagationContext	AUIPC (*this);
	InteractiveModeUpdater				iuMode (*fCurrentOwningWP);
	BreakInGroupedCommands ();
	UndoableContextHelper	context (*fCurrentOwningWP, WordProcessor::GetCommandNames ().fInsertTableColBeforeCommandName, false);
		{
			size_t	curCol	=	0;
			GetCellSelection (NULL, NULL, &curCol, NULL);
			Led_Assert (curCol <= GetColumnCount ());
			InsertColumn (curCol);
		}
	context.CommandComplete ();
	BreakInGroupedCommands ();
}

void	Table::OnUpdateInsertTableColAfterCommand (TextInteractor::CommandUpdater* enabler)
{
	Led_Require_CurrentOwningWP ();
	Led_RequireNotNil (enabler);
	enabler->SetEnabled (true);
}

void	Table::OnInsertTableColAfterCommand ()
{
	Led_Require_CurrentOwningWP ();
	AllowUpdateInfoPropagationContext	AUIPC (*this);
	InteractiveModeUpdater				iuMode (*fCurrentOwningWP);
	BreakInGroupedCommands ();
	UndoableContextHelper	context (*fCurrentOwningWP, WordProcessor::GetCommandNames ().fInsertTableColAfterCommandName, false);
		{
			size_t	curCol	=	0;
			GetCellSelection (NULL, NULL, NULL, &curCol);
			Led_Assert (curCol <= GetColumnCount ());
			InsertColumn (curCol);
		}
	context.CommandComplete ();
	BreakInGroupedCommands ();
}

void	Table::OnUpdateRemoveTableRowsCommand (TextInteractor::CommandUpdater* pCmdUI)
{
	Led_Require_CurrentOwningWP ();
	Led_RequireNotNil (pCmdUI);
	size_t	rowSelStart	=	0;
	size_t	rowSelEnd	=	0;
	size_t	colSelStart	=	0;
	size_t	colSelEnd	=	0;
	GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
	pCmdUI->SetEnabled (colSelStart == 0 and colSelEnd == GetColumnCount (rowSelStart, rowSelEnd));
}

void	Table::OnRemoveTableRowsCommand ()
{
	Led_Require_CurrentOwningWP ();
	AllowUpdateInfoPropagationContext	AUIPC (*this);
	InteractiveModeUpdater				iuMode (*fCurrentOwningWP);
	BreakInGroupedCommands ();
	UndoableContextHelper	context (*fCurrentOwningWP, WordProcessor::GetCommandNames ().fRemoveTableRowsCommandName, false);
		{
			size_t	rowSelStart	=	0;
			size_t	rowSelEnd	=	0;
			size_t	colSelStart	=	0;
			size_t	colSelEnd	=	0;
			GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
			if (colSelStart == 0 and colSelEnd == GetColumnCount (rowSelStart, rowSelEnd)) {
				if (rowSelStart == 0 and rowSelEnd == GetRowCount ()) {
					fCurrentOwningWP->OnClearCommand ();		// handled by TextInteractor (will delete whole table)
					return;										// aborts command we'd started here...
				}
				size_t	nRowsToDelete	=	rowSelEnd - rowSelStart;
				while (nRowsToDelete > 0) {
					DeleteRow (rowSelStart);
					--nRowsToDelete;
				}
			}
			else {
				fCurrentOwningWP->OnBadUserInput ();
			}
		}
	context.CommandComplete ();
}

void	Table::OnUpdateRemoveTableColumnsCommand (TextInteractor::CommandUpdater* pCmdUI)
{
	Led_Require_CurrentOwningWP ();
	Led_RequireNotNil (pCmdUI);
	size_t	rowSelStart	=	0;
	size_t	rowSelEnd	=	0;
	size_t	colSelStart	=	0;
	size_t	colSelEnd	=	0;
	GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
	pCmdUI->SetEnabled (rowSelStart == 0 and rowSelEnd == GetRowCount ());
}

void	Table::OnRemoveTableColumnsCommand ()
{
	Led_Require_CurrentOwningWP ();
	AllowUpdateInfoPropagationContext	AUIPC (*this);
	InteractiveModeUpdater				iuMode (*fCurrentOwningWP);
	BreakInGroupedCommands ();
	UndoableContextHelper	context (*fCurrentOwningWP, WordProcessor::GetCommandNames ().fRemoveTableColumnsCommandName, false);
		{
			size_t	rowSelStart	=	0;
			size_t	rowSelEnd	=	0;
			size_t	colSelStart	=	0;
			size_t	colSelEnd	=	0;
			GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
			if (rowSelStart == 0 and rowSelEnd == GetRowCount ()) {
				if (colSelStart == 0 and colSelEnd == GetColumnCount ()) {
					fCurrentOwningWP->OnClearCommand ();		// handled by TextInteractor (will delete whole table)
					return;										// aborts command we'd started here...
				}
				size_t	nColsToDelete	=	colSelEnd - colSelStart;
				while (nColsToDelete > 0) {
					DeleteColumn (colSelStart);
					--nColsToDelete;
				}
			}
			else {
				fCurrentOwningWP->OnBadUserInput ();
			}
		}
	context.CommandComplete ();
	BreakInGroupedCommands ();
}

void	Table::OnUpdateSelectTablePartsCommand (TextInteractor::CommandUpdater* enabler)
{
	switch (enabler->GetCmdID ()) {
		case	kSelectTableIntraCellAll_CmdID: {
			enabler->SetEnabled (GetIntraCellMode ());
		}
		break;
		case	kSelectTableCell_CmdID: {
			enabler->SetEnabled (GetIntraCellMode ());
		}
		break;
		case	kSelectTableRow_CmdID: {
			size_t	rowSelStart	=	0;
			size_t	rowSelEnd	=	0;
			size_t	colSelStart	=	0;
			size_t	colSelEnd	=	0;
			GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
			size_t	maxColSelEnd = GetColumnCount (rowSelStart, rowSelEnd);
			enabler->SetEnabled (colSelStart != 0 or colSelEnd != maxColSelEnd);
		}
		break;
		case	kSelectTableColumn_CmdID: {
			size_t	rowSelStart	=	0;
			size_t	rowSelEnd	=	0;
			size_t	colSelStart	=	0;
			size_t	colSelEnd	=	0;
			GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
			enabler->SetEnabled (rowSelStart != 0 or rowSelEnd != GetRowCount ());
		}
		break;
		case	kSelectTable_CmdID: {
			size_t	rowSelStart	=	0;
			size_t	rowSelEnd	=	0;
			size_t	colSelStart	=	0;
			size_t	colSelEnd	=	0;
			GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
			enabler->SetEnabled (rowSelStart != 0 or colSelStart != 0 or rowSelEnd != GetRowCount () or colSelEnd != GetColumnCount ());
		}
		break;
	}
}

void	Table::OnPerformTablePartsCommand (TextInteractor::CommandNumber commandNumber)
{
	switch (commandNumber) {
		case	kSelectTableIntraCellAll_CmdID: {
			size_t	row	=	0;
			size_t	col	=	0;
			if (GetIntraCellMode (&row, &col)) {
				TemporarilyAllocateCellWithTablet	wp (*this, row, col);
				wp->OnPerformCommand (TextInteractor::kSelectAll_CmdID);
			}
			else {
				Led_BeepNotify ();
			}
		}
		break;
		case	kSelectTableCell_CmdID: {
			UnSetIntraCellMode ();
		}
		break;
		case	kSelectTableRow_CmdID: {
			size_t	rowSelStart	=	0;
			size_t	rowSelEnd	=	0;
			size_t	colSelStart	=	0;
			size_t	colSelEnd	=	0;
			GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
			colSelStart = 0;
			colSelEnd = GetColumnCount (rowSelStart, rowSelEnd);
			SetCellSelection (rowSelStart, rowSelEnd, colSelStart, colSelEnd);
		}
		break;
		case	kSelectTableColumn_CmdID: {
			size_t	rowSelStart	=	0;
			size_t	rowSelEnd	=	0;
			size_t	colSelStart	=	0;
			size_t	colSelEnd	=	0;
			GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
			rowSelStart = 0;
			rowSelEnd = GetRowCount ();
			SetCellSelection (rowSelStart, rowSelEnd, colSelStart, colSelEnd);
		}
		break;
		case	kSelectTable_CmdID: {
			SetCellSelection (0, GetRowCount (), 0, GetColumnCount ());
		}
		break;
	}
}

void	Table::AssureCurSelFontCacheValid (Led_IncrementalFontSpecification* curSelFontSpec)
{
	Led_RequireNotNil (curSelFontSpec);
	{
		size_t	row	=	0;
		size_t	col	=	0;
		if (GetIntraCellMode (&row, &col)) {
			TemporarilyAllocateCellWithTablet	wp (*this, row, col);
			*curSelFontSpec = wp->GetCurSelFontSpec ();
			return;
		}
	}

	size_t	rowSelStart	=	0;
	size_t	rowSelEnd	=	0;
	size_t	colSelStart	=	0;
	size_t	colSelEnd	=	0;
	GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
	for (size_t ri = rowSelStart; ri < rowSelEnd; ++ri) {
		size_t	thisRowEnd	=	min (colSelEnd, GetColumnCount (ri));
		for (size_t ci = colSelStart; ci < thisRowEnd; ++ci) {
			TemporarilyAllocateCellWithTablet	wp (*this, ri, ci);
			wp->SetSelection (0, wp->GetEnd (), TextInteractor::eNoUpdate);
			Led_IncrementalFontSpecification	iSpec	=	wp->GetCurSelFontSpec ();
			if (ri == rowSelStart and ci == colSelStart) {
				*curSelFontSpec = iSpec;
			}
			else {
				*curSelFontSpec = Intersection (*curSelFontSpec, iSpec);
			}
		}
	}
}

/*
@METHOD:		WordProcessor::Table::InteractiveSetFont
@DESCRIPTION:	<p>Apply the given font specification to the selectable table cells.</p>
*/
void	Table::InteractiveSetFont (const Led_IncrementalFontSpecification& defaultFont)
{
	Led_Require_CurrentOwningWP ();

	{
		// Must fix to handle UNDO support...
		size_t	row	=	0;
		size_t	col	=	0;
		if (GetIntraCellMode (&row, &col)) {
			TemporarilyAllocateCellWithTablet	wp (*this, row, col);
			wp->InteractiveSetFont (defaultFont);
			return;
		}
	}

	AllowUpdateInfoPropagationContext	AUIPC (*this);

	fCurrentOwningWP->BreakInGroupedCommands ();
	UndoableContextHelper	undoContext (*fCurrentOwningWP, StandardStyledTextInteractor::GetCommandNames ().fFontChangeCommandName, false);
		{
			size_t	rowSelStart	=	0;
			size_t	rowSelEnd	=	0;
			size_t	colSelStart	=	0;
			size_t	colSelEnd	=	0;
			GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
			for (size_t ri = rowSelStart; ri < rowSelEnd; ++ri) {
				size_t	thisRowEnd	=	min (colSelEnd, GetColumnCount (ri));
				for (size_t ci = colSelStart; ci < thisRowEnd; ++ci) {
					TemporarilyAllocateCellWithTablet	wp (*this, ri, ci);
					wp->SetStyleInfo (0, wp->GetEnd (), defaultFont);
				}
			}
		}
	undoContext.CommandComplete ();
}

void	Table::Write (SinkStream& sink)
{
//	sink.write (fData, fLength);
}

void	Table::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
	// save done another way - AS RTF - not sure why this is never called - but
	// probably lose the whole SimpleEmbedding guy for tables - and just handle directly what
	// is done through them now...
//	flavorPackage.AddFlavorData (fFormat, fLength, fData);
}

const char*	Table::GetTag () const
{
	//tmphack
	return "table";
//	return fEmbeddingTag;
}

/*
@METHOD:		WordProcessor::Table::ProcessSimpleClick
@DESCRIPTION:	<p></p>
*/
bool	Table::ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection)
{
	#if		0
		LedDebugTrace ("ENTERING Table::ProcessSimpleClick (this= 0x%x, clickedAt=(%d,%d), clickCount=%d, rowSelStart=%d, rowSelEnd=%d, colSelStart=%d, colSelEnd=%d, intraCellMode=%d intraCellStart=%d, intraCellEnd=%d)\n",
						this, clickedAt.v, clickedAt.h, clickCount, fRowSelStart, fRowSelEnd, fColSelStart, fColSelEnd, fIntraCellMode, fIntraSelStart, fIntraSelStart
					);
	#endif
	Led_Require_CurrentOwningWP ();

	size_t	clickRow	=	0;
	size_t	clickCol	=	0;
	GetClosestCell (clickedAt, &clickRow, &clickCol);

	fTrackingAnchor_Row = clickRow;
	fTrackingAnchor_Col = clickCol;

	bool	forceSelectAllCells	=	false;
	if (extendSelection) {
		size_t	selStart	=	fCurrentOwningWP->GetSelectionStart ();
		size_t	selEnd		=	fCurrentOwningWP->GetSelectionEnd ();
		selStart = min (selStart, GetStart ());
		selEnd = max (selEnd, GetEnd ());
		forceSelectAllCells = (selEnd - selStart != 1);
		fCurrentOwningWP->SetSelection (selStart, selEnd);
	}
	else {
		fCurrentOwningWP->SetSelection (GetStart (), GetEnd ());
	}

	if (forceSelectAllCells) {
		SetCellSelection (0, GetRowCount (), 0, GetColumnCount ());
	}
	else if (extendSelection) {
		size_t	rowSelStart		=	0;
		size_t	rowSelEnd		=	0;
		size_t	colSelStart		=	0;
		size_t	colSelEnd		=	0;
		GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
		rowSelStart = min (rowSelStart, clickRow);
		rowSelEnd = max (rowSelEnd, clickRow + 1);
		colSelStart = min (colSelStart, clickCol);
		colSelEnd = max (colSelEnd, clickCol + 1);
		SetCellSelection (rowSelStart, rowSelEnd, colSelStart, colSelEnd);
	}
	else {
		SetCellSelection (clickRow, clickRow + 1, clickCol, clickCol + 1);
	}

	{
		size_t	rowSelStart		=	0;
		size_t	rowSelEnd		=	0;
		size_t	colSelStart		=	0;
		size_t	colSelEnd		=	0;
		GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
		if (rowSelEnd - rowSelStart == 1 and colSelEnd - colSelStart == 1) {
			Led_Rect	cellBounds			=	GetCellBounds (rowSelStart, colSelStart);
			Led_Rect	cellEditorBounds	=	GetCellEditorBounds (rowSelStart, colSelStart);
			
			// Only if we click inside the margins do we treat this as intra-cell activation. Otherwise, the user
			// just selects the entire cell.
			if (cellEditorBounds.Contains (clickedAt)) {
				SetIntraCellMode ();

				// pass along click to embedded WP
				TemporarilyAllocateCellWithTablet	wp (*this, rowSelStart, colSelStart);
				wp->SetCurClickCount (fCurrentOwningWP->GetCurClickCount (), Led_GetTickCount ());
				Led_Assert (fCurrentOwningWP->GetCurClickCount () == clickCount);
				wp->ProcessSimpleClick (TableCoordinates2Window (clickedAt), clickCount, extendSelection, &fIntraCellDragAnchor);
			}
			else {
				UnSetIntraCellMode ();
			}
		}
	}
	#if		0
		LedDebugTrace ("EXITING Table::ProcessSimpleClick (this= 0x%x, rowSelStart=%d, rowSelEnd=%d, colSelStart=%d, colSelEnd=%d, intraCellMode=%d intraCellStart=%d, intraCellEnd=%d)\n",
						this, fRowSelStart, fRowSelEnd, fColSelStart, fColSelEnd, fIntraCellMode, fIntraSelStart, fIntraSelStart
					);
	#endif
	return true;
}

void	Table::WhileSimpleMouseTracking (Led_Point newMousePos)
{
	#if		0
		LedDebugTrace ("ENTERING Table::WhileSimpleMouseTracking (this= 0x%x, rowSelStart=%d, rowSelEnd=%d, colSelStart=%d, colSelEnd=%d, intraCellMode=%d intraCellStart=%d, intraCellEnd=%d)\n",
						this, fRowSelStart, fRowSelEnd, fColSelStart, fColSelEnd, fIntraCellMode, fIntraSelStart, fIntraSelStart
					);
	#endif
	Led_Require_CurrentOwningWP ();

	if (fCurrentOwningWP->GetSelectionEnd () - fCurrentOwningWP->GetSelectionStart () == 1) {
		/*
		 *	If dragging WITHIN a table - then EXTEND the selection to include the area selected.
		 */
		size_t	clickRow	=	0;
		size_t	clickCol	=	0;
		GetClosestCell (newMousePos, &clickRow, &clickCol);

		size_t	rowSelStart = min (fTrackingAnchor_Row, clickRow);
		size_t	rowSelEnd = max (fTrackingAnchor_Row + 1, clickRow + 1);
		size_t	colSelStart = min (fTrackingAnchor_Col, clickCol);
		size_t	colSelEnd = max (fTrackingAnchor_Col + 1, clickCol + 1);
		SetCellSelection (rowSelStart, rowSelEnd, colSelStart, colSelEnd);
	}
	else if (fCurrentOwningWP->GetSelectionEnd () - fCurrentOwningWP->GetSelectionStart () > 1) {
		SetCellSelection (0, GetRowCount (), 0, GetColumnCount ());
	}

	{
		size_t	rowSelStart		=	0;
		size_t	rowSelEnd		=	0;
		size_t	colSelStart		=	0;
		size_t	colSelEnd		=	0;
		GetCellSelection (&rowSelStart, &rowSelEnd, &colSelStart, &colSelEnd);
		if (rowSelEnd - rowSelStart == 1 and colSelEnd - colSelStart == 1) {
			Led_Rect	cellBounds			=	GetCellBounds (rowSelStart, colSelStart);
			Led_Rect	cellEditorBounds	=	GetCellEditorBounds (rowSelStart, colSelStart);
			
			// Only if we click inside the margins do we treat this as intra-cell activation. Otherwise, the user
			// just selects the entire cell.
			if (cellEditorBounds.Contains (newMousePos)) {
				if (fTrackingAnchor_Row == rowSelStart and rowSelStart + 1 == rowSelEnd and
					fTrackingAnchor_Col == colSelStart and colSelStart + 1 == colSelEnd
					) {
					// Don't reset to IntraCell mode when tracking if the selected cell is other than the original
					// clicked in one. The fIntraCellDragAnchor value would be invalid, and the UI wouldn't make
					// much sense anyhow...
					SetIntraCellMode ();
				}
				// pass along click to embedded WP
				TemporarilyAllocateCellWithTablet	wp (*this, rowSelStart, colSelStart);
				wp->SetCurClickCount (fCurrentOwningWP->GetCurClickCount (), Led_GetTickCount ());
				wp->WhileSimpleMouseTracking (TableCoordinates2Window (newMousePos), fIntraCellDragAnchor);
			}
		}
	}
	#if		0
		LedDebugTrace ("EXITING Table::WhileSimpleMouseTracking (this= 0x%x, rowSelStart=%d, rowSelEnd=%d, colSelStart=%d, colSelEnd=%d, intraCellMode=%d intraCellStart=%d, intraCellEnd=%d)\n",
						this, fRowSelStart, fRowSelEnd, fColSelStart, fColSelEnd, fIntraCellMode, fIntraSelStart, fIntraSelStart
					);
	#endif
}

Led_Color	Table::GetTableBorderColor () const
{
	return fBorderColor;
}

void	Table::SetTableBorderColor (Led_Color c)
{
	fBorderColor = c;
}

Led_TWIPS	Table::GetTableBorderWidth () const
{
	return fBorderWidth;
}

void	Table::SetTableBorderWidth (Led_TWIPS w)
{
	fBorderWidth = w;
}

Led_TWIPS	Table::GetColumnWidth (size_t row, size_t column) const
{
	if (GetCellFlags (row, column) != ePlainCell) {
		return Led_TWIPS (0);// NOT REALLY SURE WHAT THIS SHOULD DO!!!
	}

	return GetCell (row, column).GetCellXWidth ();
}

void	Table::SetColumnWidth (size_t row, size_t column, Led_TWIPS colWidth)
{
	GetCell (row, column).SetCellXWidth (colWidth);
	InvalidateLayout ();
}

Led_Color	Table::GetCellColor (size_t row, size_t column) const
{
	return GetCell (row, column).GetBackColor ();
}

void	Table::SetCellColor (size_t row, size_t column, const Led_Color& c)
{
	Cell	cell	=	GetCell (row, column);
	cell.SetBackColor (c);
}

/*
@METHOD:		WordProcessor::Table::GetRealCell
@DESCRIPTION:	<p>Take the given row/column and modify them in place to assure they refer to the appropriate REAL cell.
			If they refer to a merge-cell, refer back to the owning REAL cell. </p>
*/
void	Table::GetRealCell (size_t* row, size_t* column) const
{
	Led_RequireNotNil (row);
	Led_RequireNotNil (column);
	size_t	r	=	*row;
	size_t	c	=	*column;
	for (;;) {
		CellMergeFlags	flags	=	GetCell (r, c).GetCellMergeFlags ();
		if (flags == ePlainCell) {
			*row = r;
			*column = c;
			return;
		}
		else {
			#if		qDebug
				bool	changed	=	false;
			#endif
			if (flags & eMergeCellLeft) {
				Led_Assert (c > 0);
				c--;
				#if		qDebug
					changed = true;
				#endif
			}
			if (flags & eMergeCellUp) {
				Led_Assert (r > 0);
				r--;
				#if		qDebug
					changed = true;
				#endif
			}
			Led_Assert (changed);
		}
	}
}

const Table::Cell&	Table::GetRealCell (size_t row, size_t column) const
{
	GetRealCell (&row, &column);
	return GetCell (row, column);
}

bool	Table::CanMergeCells (size_t fromRow, size_t fromCol, size_t toRow, size_t toCol)
{
	Led_Require (fromRow <= toRow);
	Led_Require (fromCol <= toCol);
	Led_Require (toRow <= GetRowCount ());
	Led_Require (toCol <= GetColumnCount ());
	Led_Arg_Unused (toRow);
	Led_Arg_Unused (toCol);
	// for now - our only requirements are that the region to merge is square (and this doesn't need to be tested
	// for because of my API), and that the top-left is a plain cell, and not already merged into something else.
	return GetCellFlags (fromRow, fromCol) == ePlainCell;
}

/*
@METHOD:		WordProcessor::Table::MergeCells
@ACCESS:		public
@DESCRIPTION:	<p>Merge all the cells in the given range. The row positions bound a range of cells, so for example,
			if <code>toCol-fromCol==2</code> then 2 columns are being merged, and if <code>toRow-fromRow==1</code>
			then we are operating on a single row (all thats really fully implemented at this time - Led 3.1a4).
			</p>
				<p>Note - though the low-level support for merge-cells is mostly in place - I found it was not needed
			to implement most of the table functionality I wanted for 3.1. I was going to use it to handle different
			width columns - but since I've now revised the code to directly support having different width columns
			(and different numbers of columns) per row, its no longer needed. At some point in the future - this will
			probably be more fully supported.
			</p>
*/
void	Table::MergeCells (size_t fromRow, size_t fromCol, size_t toRow, size_t toCol)
{
	Led_Require (fromRow <= toRow);
	Led_Require (fromCol <= toCol);
	Led_Require (toRow <= GetRowCount ());
	Led_Require (toCol <= GetColumnCount ());
	Led_Require (CanMergeCells (fromRow, fromCol, toRow, toCol));
	bool	madeChange	=	false;
	for (size_t r = fromRow; r < toRow; ++r) {
		for (size_t c = fromCol; c < toCol; ++c) {
			// All but the first cell get merged (into the first)
			if (not (r == fromRow and c == fromCol)) {
				fRows[r].fCells[c] = Cell (*this, static_cast<CellMergeFlags> (((r>fromRow)?eMergeCellUp:0) | ((c>fromCol)?eMergeCellLeft:0) ));
				madeChange = true;
			}
		}
	}
	if (madeChange) {
		InvalidateLayout ();
	}
}

void	Table::UnMergeCells (size_t fromRow, size_t fromCol, size_t toRow, size_t toCol)
{
	Led_Require (fromRow <= toRow);
	Led_Require (fromCol <= toCol);
	Led_Require (toRow < GetRowCount ());
	Led_Require (toCol < GetColumnCount ());
	bool	madeChange	=	false;
	for (size_t r = fromRow; r < toRow; ++r) {
		for (size_t c = fromCol; c < toCol; ++c) {
			// don't overwrite the cell object (losing all its data) if its already a plain cell)
			if (GetCellFlags (r, c) != ePlainCell) {
				fRows[r].fCells[c] = Cell (*this, ePlainCell);
				madeChange = true;
			}
		}
	}
	if (madeChange) {
		InvalidateLayout ();
	}
}

/*
@METHOD:		WordProcessor::Table::SetCellSelection
@DESCRIPTION:	<p>See @'WordProcessor::Table::GetCellSelection'.</p>
*/
void	Table::SetCellSelection (size_t rowSelStart, size_t rowSelEnd, size_t colSelStart, size_t colSelEnd)
{
	Led_Ensure (rowSelStart <= rowSelEnd);
	Led_Ensure (rowSelEnd <= GetRowCount ());
	Led_Ensure (colSelStart <= colSelEnd);
	Led_Ensure (colSelEnd <= GetColumnCount ());
	bool	changed	=	(fRowSelStart != rowSelStart) or
						(fRowSelEnd != rowSelEnd) or
						(fColSelStart != colSelStart) or
						(fColSelEnd != colSelEnd)
					;
	if (changed) {
		fRowSelStart = rowSelStart;
		fRowSelEnd = rowSelEnd;
		fColSelStart = colSelStart;
		fColSelEnd = colSelEnd;
		fIntraCellMode = false;
		InvalidateIntraCellContextInfo ();
		if (fCurrentOwningWP != NULL) {
			fCurrentOwningWP->fCachedCurSelFontSpecValid = false;
			fCurrentOwningWP->Refresh (GetStart (), GetEnd ());
		}
	}
	#if		0
		LedDebugTrace ("Table::SetCellSelection (table=0x%x, tickCount=%f, rs=%d, re=%d, cs=%d, ce=%d, changed=%d)\n",
						this, Led_GetTickCount (), rowSelStart, rowSelEnd, colSelStart, colSelEnd, changed
				);
	#endif
}

void	Table::SetIntraCellMode ()
{
	Led_Require (fRowSelEnd - fRowSelStart == 1);
	Led_Require (fColSelEnd - fColSelStart == 1);
	if (not fIntraCellMode) {
		TextStore*	ts	=	NULL;
		GetCellWordProcessorDatabases (fRowSelStart, fColSelStart, &ts);
		Led_AssertNotNil (ts);
		SetIntraCellSelection (0, ts->GetLength ());
		fIntraCellMode = true;
		if (fCurrentOwningWP != NULL) {
			fCurrentOwningWP->Refresh (GetStart (), GetEnd ());
		}
	}
}

void	Table::SetIntraCellMode (size_t row, size_t col)
{
	if (not fIntraCellMode) {
		SetCellSelection (row, row + 1, col, col + 1);
		SetIntraCellMode ();
	}
}

void	Table::UnSetIntraCellMode ()
{
	if (fIntraCellMode) {
		fIntraCellMode = false;
		if (fCurrentOwningWP != NULL) {
			fCurrentOwningWP->Refresh (GetStart (), GetEnd ());
		}
	}
}

void	Table::SetIntraCellSelection (size_t selStart, size_t selEnd)
{
	if (fIntraSelStart != selStart or fIntraSelEnd != selEnd) {
		#if		0
			LedDebugTrace ("Table::SetIntraCellSelection (selStart = %d, selEnd = %d)- oldSel=(%d,%d), tickcount=%f\n", selStart, selEnd, fIntraSelStart, fIntraSelEnd, Led_GetTickCount ());
		#endif
		if (fCurrentOwningWP != NULL) {
			fCurrentOwningWP->fCachedCurSelFontSpecValid = false;
		}
		fIntraSelStart = selStart;
		fIntraSelEnd = selEnd;
	}
}

/*
@METHOD:		WordProcessor::Table::ConstructEmbeddedTableWordProcessor
@ACCESS:		protected
@DESCRIPTION:	<p>Called internally by the @'WordProcessor::Table' code to instantiate mini embedded
			word processor objects (@'EmbeddedTableWordProcessor') to be in each cell. Objects created
			with this method should be released with a call to
			@'WordProcessor::Table::ReleaseEmbeddedTableWordProcessor'.</p>
*/
Table::EmbeddedTableWordProcessor*	Table::ConstructEmbeddedTableWordProcessor (WordProcessor& forWordProcessor, size_t forRow, size_t forColumn, const Led_Rect& cellWindowRect, bool captureChangesForUndo)
{
	size_t						cellModeRow			=	0;
	size_t						cellModeCol			=	0;
	bool						activeFocusedCell	=	GetIntraCellMode (&cellModeRow, &cellModeCol) and cellModeRow == forRow and cellModeCol == forColumn;
	EmbeddedTableWordProcessor*	e					=	new EmbeddedTableWordProcessor (forWordProcessor, *this, forRow, forColumn, activeFocusedCell);
	try {
		TextStore*				ts	=	NULL;
		StyleDatabasePtr		styleDatabase;
		ParagraphDatabasePtr	paragraphDatabase;
		HidableTextDatabasePtr	hidableTextDatabase;
		GetCellWordProcessorDatabases (forRow, forColumn, &ts, &styleDatabase, &paragraphDatabase, &hidableTextDatabase);
		e->SetStyleDatabase (styleDatabase);
		e->SetParagraphDatabase (paragraphDatabase);
		e->SetHidableTextDatabase (hidableTextDatabase);
		e->SpecifyTextStore (ts);
		e->SetWindowRect (cellWindowRect);
		e->SetDefaultTextColor (eDefaultBackgroundColor, GetCellColor (forRow, forColumn));
		if (captureChangesForUndo) {
			e->SetCommandHandler (forWordProcessor.GetCommandHandler ());
		}

		if (activeFocusedCell) {
			typedef	EmbeddedTableWordProcessor::TemporarilyUseTablet	TemporarilyUseTablet;

			Led_AssertNotNil (fCurrentOwningWP);
			Tablet_Acquirer				tablet (fCurrentOwningWP);
			TemporarilyUseTablet		tmpUseTablet (*e, tablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall);

			e->SetSelectionShown (true, TextInteractor::eNoUpdate);		// set TRUE so stuff that changes the selection does the proper invalidation
			e->RestoreMiscActiveFocusInfo ();
		}
	}
	catch (...) {
		e->SpecifyTextStore (NULL);
		delete e;
	}
	return e;
}

/*
@METHOD:		WordProcessor::Table::ReleaseEmbeddedTableWordProcessor
@ACCESS:		protected
@DESCRIPTION:	<p>Release word @'WordProcessor::Table::EmbeddedTableWordProcessor' objects 
			allocated with @'WordProcessor::Table::ConstructEmbeddedTableWordProcessor'.
			This may not neccesarily DELETE them as they could
			be cached (for example - if they are the currently active cell, and are blinking the caret etc...</p>
*/
void	Table::ReleaseEmbeddedTableWordProcessor (EmbeddedTableWordProcessor* e)
{
	Led_RequireNotNil (e);
	e->SaveMiscActiveFocusInfo ();
	e->SetCommandHandler (NULL);
	e->SpecifyTextStore (NULL);
	delete e;
}

/*
@METHOD:		WordProcessor::Table::GetCellWordProcessorDatabases
@ACCESS:		public
@DESCRIPTION:	<p>Retrieve  the various databases (textstore, style etc) associated
			with the given cell. Arguments CAN be null. Only non-null pointer values
			are filled in.</p>
*/
void	Table::GetCellWordProcessorDatabases (size_t row, size_t column, TextStore** ts, StandardStyledTextImager::StyleDatabasePtr* styleDatabase, WordProcessor::ParagraphDatabasePtr* paragraphDatabase,	WordProcessor::HidableTextDatabasePtr* hidableTextDatabase)
{
	Led_Require (row < GetRowCount ());
	Led_Require (column < GetColumnCount (row));
	const Cell&	c	=	GetCell (row, column);
	if (ts != NULL) {
		*ts = &c.GetTextStore ();
	}
	if (styleDatabase != NULL) {
		*styleDatabase = c.GetStyleDatabase ();
	}
	if (paragraphDatabase != NULL) {
		*paragraphDatabase = c.GetParagraphDatabase ();
	}
	if (hidableTextDatabase != NULL) {
		*hidableTextDatabase = c.GetHidableTextDatabase ();
	}
}

/*
@METHOD:		WordProcessor::Table::PerformLayout
@ACCESS:		protected
@DESCRIPTION:	<p></p>
*/
void		Table::PerformLayout ()
{
	Led_Require (fNeedLayout != eDone);

	if (fCurrentOwningWP != NULL) {
		TextStore&					ts	=	GetOwner ()->GetTextStore ();
		TextStore::SimpleUpdater	updater (ts, GetStart (), GetEnd (), false);

		Led_Size		border	=	Led_Size (Led_CvtScreenPixelsFromTWIPSV (fBorderWidth), Led_CvtScreenPixelsFromTWIPSH (fBorderWidth));
		Led_Distance	spacing	=	Led_CvtScreenPixelsFromTWIPSV (GetCellSpacing ());
		Led_Rect		defaultCellMargin;
		{
			Led_TWIPS_Rect	defaultCellMarginTWIPS;
			GetDefaultCellMargins (&defaultCellMarginTWIPS.top, &defaultCellMarginTWIPS.left, &defaultCellMarginTWIPS.bottom, &defaultCellMarginTWIPS.right);
			defaultCellMargin.top = Led_CvtScreenPixelsFromTWIPSV (defaultCellMarginTWIPS.top);
			defaultCellMargin.left = Led_CvtScreenPixelsFromTWIPSH (defaultCellMarginTWIPS.left);
			defaultCellMargin.bottom = Led_CvtScreenPixelsFromTWIPSV (defaultCellMarginTWIPS.bottom);
			defaultCellMargin.right = Led_CvtScreenPixelsFromTWIPSH (defaultCellMarginTWIPS.right);
		}

		// Need to be more careful her about updating the row heights records. We COULD not  be able to do this, and then we want to set a flag
		// saying to re-layout when we are RE-associated with a word-processor object (temporarily)

		size_t	rows	=	GetRowCount ();

		Tablet_Acquirer	tablet (fCurrentOwningWP);

		Led_Distance	maxTableWidth	=	0;

		Led_Distance	runningHeight	=	0;
		for (size_t r = 0; r < rows; ++r) {
			/*
			 *	Compute REAL (non-merge) cells widths.
			 */
			size_t								cols		=	GetColumnCount (r);
			Led_SmallStackBuffer<Led_Distance>	realCellWidths (cols);	// cell widths for this row - only for REAL (plain - non-merge) cells
			Led_Distance						rowWidth	=	0;
			{
				size_t	lastRealCellIdx	=	0;
				for (size_t c = 0; c < cols; ++c) {
					Led_Distance	thisColWidth	=	Led_CvtScreenPixelsFromTWIPSH (GetColumnWidth (r, c));
					if (GetCellFlags (r, c) == ePlainCell) {
						realCellWidths[c] = thisColWidth;
						lastRealCellIdx = c;
						rowWidth += thisColWidth;
					}
					else {
						// and ... if its a 'merge left' cell - then add this width to the width of the REAL rect
						realCellWidths[lastRealCellIdx] += thisColWidth;
					}
				}
			}

			/*
			 *	Compute their row (cell) heights.
			 */
			Led_Distance	rowHeight	=	0;
			for (size_t c = 0; c < cols; ++c) {
				if (GetCellFlags (r, c) == ePlainCell) {
					typedef	EmbeddedTableWordProcessor::TemporarilyUseTablet	TemporarilyUseTablet;

					Led_Distance				totalCellMargin	=	defaultCellMargin.left + defaultCellMargin.right;
					Led_Distance				wpWidth			=	(totalCellMargin < realCellWidths[c]) ? realCellWidths[c] - totalCellMargin: 1;
					TemporarilyAllocateCellWP	wp (*this, *fCurrentOwningWP, r, c, Led_Rect (0, 0, 1000, wpWidth));
					TemporarilyUseTablet		tmpUseTablet (*wp, tablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall);
					rowHeight = max (rowHeight, wp->GetDesiredHeight ());
				}
			}
			/*
			 *	Note - since cell width is usaully pre-specified - we don't let the margin determine that.
			 *	But - since we're COMPUTING the appropriate cell HEIGHT - we must add in the top/bottom cell margins.
			 */
			rowHeight += defaultCellMargin.top + defaultCellMargin.bottom;
			rowHeight = Led_Max (rowHeight, 5);		// assure some min height
			fRows[r].fHeight = rowHeight;
			runningHeight += rowHeight;

			Led_Distance	rowWidthWithSpacingNBorders	=	rowWidth + (cols+1)* (spacing + border.h);
			maxTableWidth = max (maxTableWidth, rowWidthWithSpacingNBorders);

			/*
			 *	Store away the resulting cell rectangles (again - only for REAL - non-merge cells).
			 */
			{
				Led_Distance	runningWidth	=	0;		// not including spacing - cuz that added separately...
				size_t			lastRealCellIdx	=	0;
				for (size_t c = 0; c < cols; ++c) {
					if (GetCellFlags (r, c) == ePlainCell) {
						Cell		cell		=	GetCell (r, c);
						Led_Rect	cellRect	=	Led_Rect (runningHeight - rowHeight, runningWidth, rowHeight, realCellWidths[c]);

						cellRect += Led_Point ((r+1)*border.v, (c+1)*border.h);

						// add in cell spacing
						cellRect += Led_Point ((r+1)*spacing, (c+1)*spacing);

						cell.SetCachedBoundsRect (cellRect);
						lastRealCellIdx = c;
						runningWidth += realCellWidths[c];
					}
				}
			}
		}

		{
			Led_Distance	totalHeight	=	0;
			for (vector<RowInfo>::const_iterator i = fRows.begin (); i != fRows.end (); ++i) {
				totalHeight += (*i).fHeight;
			}
			fTotalHeight = totalHeight + (spacing + border.v) * (fRows.size () + 1);
		}

		fTotalWidth = maxTableWidth;
	
		fNeedLayout = eDone;
	}
}

size_t	Table::GetColumnCount (size_t row) const
{
	Led_Require (row < GetRowCount ());
	return fRows[row].fCells.size ();
}

size_t	Table::GetColumnCount (size_t rowStart, size_t rowEnd) const
{
	Led_Require (rowStart <= GetRowCount ());
	Led_Require (rowEnd <= GetRowCount ());
	size_t	colCount	=	0;
	for (size_t ri = rowStart; ri < rowEnd; ++ri) {
		colCount = max (colCount, fRows[ri].fCells.size ());
	}
	return colCount;
}

void	Table::SetColumnCount (size_t row, size_t columns)
{
	Led_Require (row < GetRowCount ());
	size_t	curColCount	=	fRows[row].fCells.size ();
	if (curColCount != columns) {
		vector<Cell>&	rowCells	=	fRows[row].fCells;
		while (curColCount < columns) {
			Cell	cell (*this, ePlainCell);
			rowCells.push_back (cell);
			curColCount++;
		}
		while (curColCount > columns) {
			curColCount--;
			rowCells.erase (rowCells.begin () + curColCount);
		}
	}
}

/*
@METHOD:		WordProcessor::Table::GetDimensions
@DESCRIPTION:	<p>Return the number of rows and columns in the given table. Pointer parameters CAN be null,
			and that value is just not returned.</p>
*/
void		Table::GetDimensions (size_t* rows, size_t* columns) const
{
	if (rows != NULL) {
		*rows = fRows.size ();
	}
	if (columns != NULL) {
		size_t	maxCols	=	0;
		for (size_t ri = 0; ri < fRows.size (); ++ri) {
			maxCols = max (maxCols, fRows[ri].fCells.size ());
		}
		*columns = maxCols;
	}
}

/*
@METHOD:		WordProcessor::Table::SetDimensions
@DESCRIPTION:	<p>Specifies the number of rows and columns desired. If rows or columns need to be created, they will be
			appended. If rows/columns need to be destroyed - they will be desroyed from the end (right/bottom). If you need
			more specific control, then call @'WordProcessor::Table::InsertRow', @'WordProcessor::Table::DeleteRow',
			@'WordProcessor::Table::InsertColumn', or @'WordProcessor::Table::DeleteColumn' directly.</p>
*/
void		Table::SetDimensions (size_t rows, size_t columns)
{
	size_t	oldRows		=	0;
	size_t	oldColumns	=	0;
	GetDimensions (&oldRows, &oldColumns);

	// delete first
	while (oldRows > rows) {
		DeleteRow (oldRows - 1);
		--oldRows;
	}
	while (oldColumns > columns) {
		DeleteColumn (oldColumns - 1);
		--oldColumns;
	}

	// then append any needed rows/columns
	while (oldRows < rows) {
		InsertRow (oldRows);
		++oldRows;
	}
	while (oldColumns < columns) {
		InsertColumn (oldColumns);
		++oldColumns;
	}
	#if		qDebug
		GetDimensions (&oldRows, &oldColumns);
		Led_Assert (oldRows == rows);
		Led_Assert (oldColumns == columns);
	#endif
}

/*
@METHOD:		WordProcessor::Table::SetDimensionsAtLeast
@DESCRIPTION:	<p>Calls @'WordProcessor::Table::GetDimensions' and
			@'WordProcessor::Table::SetDimensions' to assure
			there are <em>at least</em> the given number of rows and columns.</p>
*/
void	Table::SetDimensionsAtLeast (size_t rows, size_t columns)
{
	size_t	r	=	0;
	size_t	c	=	0;
	GetDimensions (&r, &c);
	r = max (rows, r);
	c = max (columns, c);
	SetDimensions (r, c);
}

/*
@METHOD:		WordProcessor::Table::InsertRow
@DESCRIPTION:	<p>Insert a new blank row at the given 'at' offset. Must be
			between 0 and the current number of rows (inclusive). Get the
			number of columns and the size of those columns from the adjacent
			row, but copying a maximum of 'maxRowCopyCount' columns.</p>
*/
void		Table::InsertRow (size_t at, size_t maxRowCopyCount)
{
	Led_Require (at <= GetRowCount ());
	Led_Require (maxRowCopyCount >= 1);

	TextStore&					ts	=	GetOwner ()->GetTextStore ();
	TextStore::SimpleUpdater	updater (ts, GetStart (), GetEnd ());

	RowInfo	newRow;

	// Copy row count and row width from adjoining row. COULD be smarter about this - taking a HINT as
	// to whether to copy from LHS or RHS - but this is pretty reasonable...
	size_t	rowToCopy	=	at>0? at-1: at;
	size_t	colCount	=	0;
	if (rowToCopy < GetRowCount ()) {
		colCount = GetColumnCount (rowToCopy);
	}
	colCount = min (colCount, maxRowCopyCount);

	for (size_t c = 0; c < colCount; ++c) {
		Cell	cell (*this, ePlainCell);
		cell.SetCellXWidth (GetColumnWidth (rowToCopy, c));
		newRow.fCells.push_back (cell);
	}

	fRows.insert (fRows.begin () + at, newRow);

	InvalidateIntraCellContextInfo ();
	InvalidateLayout ();
}

/*
@METHOD:		WordProcessor::Table::DeleteRow
@DESCRIPTION:	<p>Delete the given row at the given 'at' offset.</p>
*/
void	Table::DeleteRow (size_t at)
{
	Led_Require (at < GetRowCount ());
	
	TextStore&					ts	=	GetOwner ()->GetTextStore ();
	TextStore::SimpleUpdater	updater (ts, GetStart (), GetEnd ());
	
	fRows.erase (fRows.begin () + at);
	InvalidateIntraCellContextInfo ();
	InvalidateLayout ();
	ReValidateSelection ();
}

/*
@METHOD:		WordProcessor::Table::InsertColumn
@DESCRIPTION:	<p>Insert a new blank column at the given 'at' offset. Must be between
			0 and the current number of columns (inclusive).</p>
*/
void		Table::InsertColumn (size_t at)
{
	Led_Require (at <= GetColumnCount ());

	TextStore&					ts	=	GetOwner ()->GetTextStore ();
	TextStore::SimpleUpdater	updater (ts, GetStart (), GetEnd ());

	// Grab default size for new column from first row/prev (or next) col
	Led_TWIPS	newColWidth	=	Led_CvtScreenPixelsToTWIPSH (100);
	if (fRows.size () > 0) {
		size_t	nColsInRow	=	GetColumnCount (0);
		size_t	useCol		=	at;
		if (at == nColsInRow and at > 0) {
			newColWidth = GetColumnWidth (0, at-1);
		}
		else if (at < nColsInRow) {
			newColWidth = GetColumnWidth (0, at);
		}
	}

	vector<Cell>	newCol;
	for (size_t r = 0; r < fRows.size (); ++r) {
		Cell	cell (*this, ePlainCell);
		cell.SetCellXWidth (newColWidth);
		newCol.push_back (cell);
	}

	size_t	rowCount	=	fRows.size ();
	for (size_t ri = 0; ri < rowCount; ++ri) {
		vector<Cell>&	rowCells	=	fRows[ri].fCells;
		rowCells.insert (rowCells.begin () + at, newCol[ri]);
	}

	InvalidateIntraCellContextInfo ();
	InvalidateLayout ();
}

/*
@METHOD:		WordProcessor::Table::DeleteColumn
@DESCRIPTION:	<p>Delete the given column at the given 'at' offset.</p>
*/
void		Table::DeleteColumn (size_t at)
{
	Led_Require (at < GetColumnCount ());
	// BUT - allow for the fact that some rows may have fewer columns than GetColumnCount ()...

	TextStore&					ts	=	GetOwner ()->GetTextStore ();
	TextStore::SimpleUpdater	updater (ts, GetStart (), GetEnd ());

	size_t	rowCount	=	fRows.size ();
	for (size_t ri = 0; ri < rowCount; ++ri) {
		vector<Cell>&	rowCells	=	fRows[ri].fCells;
		if (at < rowCells.size ()) {
			rowCells.erase (rowCells.begin () + at);
		}
	}
	InvalidateIntraCellContextInfo ();
	InvalidateLayout ();
	ReValidateSelection ();
}

/*
@METHOD:		WordProcessor::Table::ReValidateSelection
@ACCESS:		protected
@DESCRIPTION:	<p>Called internally when something happens that could invalidate the selection. Assure its
			still valid.</p>
*/
void	Table::ReValidateSelection ()
{
	size_t	rowCount	=	GetRowCount ();

	size_t	rowSelStart	=	fRowSelStart;	// Cannot call GetCellSelection () cuz it asserts selection valid
	size_t	rowSelEnd	=	fRowSelEnd;
	size_t	colSelStart	=	fColSelStart;
	size_t	colSelEnd	=	fColSelEnd;
	
	if (rowSelStart >= rowCount) {
		rowSelStart = 0;
		rowSelEnd = 0;
	}
	if (rowSelEnd > rowCount) {
		rowSelEnd = rowCount;
	}
	size_t	colCount	=	GetColumnCount (rowSelStart, rowSelEnd);
	if (colSelStart >= colCount) {
		colSelStart = 0;
		colSelEnd = 0;
	}
	if (colSelEnd >= colCount) {
		colSelEnd = colCount;
	}
	SetCellSelection (rowSelStart, rowSelEnd, colSelStart, colSelEnd);
}







/*
 ********************************************************************************
 ************************** WordProcessor::Table::Cell **************************
 ********************************************************************************
 */
WordProcessor::Table::Cell::Cell (Table& forTable, CellMergeFlags mergeFlags):
	fCellRep (mergeFlags == ePlainCell? (new CellRep (forTable)): NULL),
	fCellMergeFlags (mergeFlags)
{
}

/*
@METHOD:		WordProcessor::Table::Cell::GetCellWordProcessorDatabases
@ACCESS:		public
@DESCRIPTION:	<p>Retrieve  the various databases (textstore, style etc) associated with the given cell. Arguments
			CAN be null. Only non-null pointer valeus
			are filled in.</p>
*/
void	WordProcessor::Table::Cell::GetCellWordProcessorDatabases (TextStore** ts, StandardStyledTextImager::StyleDatabasePtr* styleDatabase, WordProcessor::ParagraphDatabasePtr* paragraphDatabase, WordProcessor::HidableTextDatabasePtr* hidableTextDatabase)
{
	Led_Require (fCellMergeFlags == ePlainCell);
	if (ts != NULL) {
		*ts = &GetTextStore ();
	}
	if (styleDatabase != NULL) {
		*styleDatabase = GetStyleDatabase ();
	}
	if (paragraphDatabase != NULL) {
		*paragraphDatabase = GetParagraphDatabase ();
	}
	if (hidableTextDatabase != NULL) {
		*hidableTextDatabase = GetHidableTextDatabase ();
	}
}

TextStore& WordProcessor::Table::Cell::GetTextStore () const
{
	Led_Require (fCellMergeFlags == ePlainCell);
	Led_EnsureNotNil (fCellRep->fTextStore);
	return *fCellRep->fTextStore;
}

StandardStyledTextImager::StyleDatabasePtr WordProcessor::Table::Cell::GetStyleDatabase () const
{
	Led_Require (fCellMergeFlags == ePlainCell);
	return fCellRep->fStyleDatabase;
}

WordProcessor::ParagraphDatabasePtr WordProcessor::Table::Cell::GetParagraphDatabase () const
{
	Led_Require (fCellMergeFlags == ePlainCell);
	return fCellRep->fParagraphDatabase;
}

WordProcessor::HidableTextDatabasePtr WordProcessor::Table::Cell::GetHidableTextDatabase () const
{
	Led_Require (fCellMergeFlags == ePlainCell);
	return fCellRep->fHidableTextDatabase;
}

Led_Color	WordProcessor::Table::Cell::GetBackColor () const
{
	Led_Require (fCellMergeFlags == ePlainCell);
	return fCellRep->fBackColor;
}

void	WordProcessor::Table::Cell::SetBackColor (Led_Color c)
{
	Led_Require (fCellMergeFlags == ePlainCell);
	fCellRep->fBackColor = c;
}








/*
 ********************************************************************************
 ************************ WordProcessor::Table::CellRep *************************
 ********************************************************************************
 */
WordProcessor::Table::CellRep::CellRep (Table& forTable):
	fForTable (forTable),
	fTextStore (NULL),
	fStyleDatabase (),
	fParagraphDatabase (),
	fHidableTextDatabase (),
	fBackColor (Led_Color::kWhite),
	fCachedBoundsRect (Led_Rect (0, 0, 0, 0)),
	fCellXWidth (Led_CvtScreenPixelsToTWIPSH (75))	// This should be overridden someplace - depending on how the table is constructed - but
													// in case its not, and until it is, leave in a vaguely reasonable number - LGP 2003-04-17
{
	fTextStore = new SimpleTextStore ();
	fTextStore->AddMarkerOwner (this);
	fStyleDatabase = new StyleDatabaseRep (*fTextStore);
	fParagraphDatabase	= new ParagraphDatabaseRep (*fTextStore);
	fHidableTextDatabase = new UniformHidableTextMarkerOwner (*fTextStore);
}

WordProcessor::Table::CellRep::~CellRep ()
{
	Led_Require (fStyleDatabase.CurrentRefCount () == 1);		// hack to debug SPR#1465
	Led_Require (fParagraphDatabase.CurrentRefCount () == 1);	// ''
	Led_Require (fHidableTextDatabase.CurrentRefCount () == 1);	// ''
	fStyleDatabase = NULL;
	fParagraphDatabase	= NULL;
	fHidableTextDatabase = NULL;
	if (fTextStore != NULL) {
		fTextStore->RemoveMarkerOwner (this);
	}
	delete fTextStore;
}

TextStore*	WordProcessor::Table::CellRep::PeekAtTextStore () const
{
	return fTextStore;
}

void	WordProcessor::Table::CellRep::AboutToUpdateText (const UpdateInfo& updateInfo)
{
	inherited::AboutToUpdateText (updateInfo);
	if (fForTable.fAllowUpdateInfoPropagationContext and updateInfo.fRealContentUpdate) {
		if (fForTable.fCellUpdatePropationUpdater != NULL) {
			Led_Assert (false);		// This should only happen if an earlier update was aborted (throw). NOT really a bug
									// if this gets triggered. Just for informational purposes (debugging) only
			fForTable.fCellUpdatePropationUpdater->Cancel ();
			delete fForTable.fCellUpdatePropationUpdater;
			fForTable.fCellUpdatePropationUpdater = NULL;
		}
		fForTable.fCellUpdatePropationUpdater = new TextStore::SimpleUpdater (fForTable.GetOwner ()->GetTextStore (), fForTable.GetStart (), fForTable.GetEnd ());
	}
}

void	WordProcessor::Table::CellRep::DidUpdateText (const TextInteractor::UpdateInfo& updateInfo) throw ()
{
	inherited::DidUpdateText (updateInfo);

	if (not fForTable.fSuppressCellUpdatePropagationContext) {
		fForTable.InvalidateLayout ();
	}
	if (fForTable.fAllowUpdateInfoPropagationContext and updateInfo.fRealContentUpdate) {
		Led_AssertNotNil (fForTable.fCellUpdatePropationUpdater);
		delete fForTable.fCellUpdatePropationUpdater;		// NB: This calls the DidUpdate calls for the table itself and its owners...
		fForTable.fCellUpdatePropationUpdater = NULL;
	}
}









/*
 ********************************************************************************
 ************************** EmbeddedTableWordProcessor **************************
 ********************************************************************************
 */

/*
@METHOD:		WordProcessor::Table::EmbeddedTableWordProcessor::EmbeddedTableWordProcessor
@DESCRIPTION:	<p></p>
*/
WordProcessor::Table::EmbeddedTableWordProcessor::EmbeddedTableWordProcessor (WordProcessor& owningWordProcessor, Table& owningTable, size_t tRow, size_t tCol, bool activeEditCell):
	inherited (),
	fOwningWordProcessor (owningWordProcessor),
	fOwningTable (owningTable),
	fTableRow (tRow),
	fTableColumn (tCol),
	fUpdateTablet (NULL),
	fDesiredHeight (0),
	fDesiredHeightValid (false),
	fActiveEditCell (activeEditCell),
	fSupressRefreshCalls (false)
{
	SetImageUsingOffscreenBitmaps (false);
}

WordProcessor::Table&	WordProcessor::Table::EmbeddedTableWordProcessor::GetOwningTable () const
{
	return fOwningTable;
}

WordProcessor&	WordProcessor::Table::EmbeddedTableWordProcessor::GetOwningWordProcessor () const
{
	return fOwningWordProcessor;
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::SaveMiscActiveFocusInfo ()
{
	if (fActiveEditCell) {
		fOwningTable.SetIntraCellSelection (GetSelectionStart (), GetSelectionEnd ());
		fOwningTable.SaveIntraCellContextInfo (fLeftSideOfSelectionInteresting, GetEmptySelectionStyle ());
	}
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::RestoreMiscActiveFocusInfo ()
{
	if (fActiveEditCell) {
		DisableRefreshContext					DFR (*this);
		SuppressCellUpdatePropagationContext	SCUP (fOwningTable);

		Led_FontSpecification					emptySelFont;
		bool									leftSideOfSelectionInteresting	=	false;
		if (fOwningTable.RestoreIntraCellContextInfo (&leftSideOfSelectionInteresting, &emptySelFont)) {
			size_t		intraCellSelStart	=	0;
			size_t		intraCellSelEnd		=	0;
			fOwningTable.GetIntraCellSelection (&intraCellSelStart, &intraCellSelEnd);
			SetSelection (intraCellSelStart, intraCellSelEnd, TextInteractor::eNoUpdate);
			fLeftSideOfSelectionInteresting = leftSideOfSelectionInteresting;
			SetEmptySelectionStyle (emptySelFont);
		}
		else {
			SetEmptySelectionStyle ();
		}
	}
}

#if		!qNestedTablesSupported
void	WordProcessor::Table::EmbeddedTableWordProcessor::HookInternalizerChanged ()
{
	inherited::HookInternalizerChanged ();
	WordProcessorFlavorPackageInternalizer*	internalizerRep	=
							dynamic_cast<WordProcessorFlavorPackageInternalizer*> (
									static_cast<FlavorPackageInternalizer*> (GetInternalizer ())
								);
	Led_AssertNotNil (internalizerRep);
	internalizerRep->SetNoTablesAllowed (true);
}
#endif

bool	WordProcessor::Table::EmbeddedTableWordProcessor::OnCopyCommand_Before ()
{
	return fOwningWordProcessor.OnCopyCommand_Before ();
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::OnCopyCommand_After ()
{
	fOwningWordProcessor.OnCopyCommand_After ();
}

bool	WordProcessor::Table::EmbeddedTableWordProcessor::OnPasteCommand_Before ()
{
	return fOwningWordProcessor.OnPasteCommand_Before ();
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::OnPasteCommand_After ()
{
	fOwningWordProcessor.OnPasteCommand_After ();
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::DrawRowHilight (Led_Tablet /*tablet*/, const Led_Rect& /*currentRowRect*/, const Led_Rect& /*invalidRowRect*/,
							const TextLayoutBlock& /*text*/, size_t /*rowStart*/, size_t /*rowEnd*/
						)
{
	// Do nothing... - taken care if via owning Table and override of GetRowHilightRects
}

Led_Tablet	WordProcessor::Table::EmbeddedTableWordProcessor::AcquireTablet () const
{
	if (fUpdateTablet != NULL) {
		return fUpdateTablet;
	}
	Led_Assert (false);	// we shouldn't need this anymore - get rid... LGP 2003-03-18
//tmphack - probably needs to be slightly differnet
	return fOwningWordProcessor.AcquireTablet ();
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::ReleaseTablet (Led_Tablet tablet) const
{
	if (tablet == fUpdateTablet) {
		return;
	}
	Led_Assert (false);	// we shouldn't need this anymore - get rid... LGP 2003-03-18
//tmphack - probably needs to be slightly differnet
	fOwningWordProcessor.ReleaseTablet (tablet);
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const
{
	#if		0
		LedDebugTrace ("EmbeddedTableWordProcessor::RefreshWindowRect_ (supressRefreshCalls = %d, updadeMode=%d)\n", fSupressRefreshCalls, updateMode);
	#endif
	if (not fSupressRefreshCalls) {
		// See SPR#1455- really never a need to do IMMEDIATE update and for these embedded WP's can get into a
		// deadly embrace. This may not be the BEST solution to that deadly embrace, but it appears to be a
		// safe and adequate one... LGP 2003-05-01
		UpdateMode	useUpdateMode	=	(updateMode == eImmediateUpdate)? eDelayedUpdate: updateMode;
		fOwningWordProcessor.RefreshWindowRect_ (windowRectArea, useUpdateMode);
	}
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::UpdateWindowRect_ (const Led_Rect& /*windowRectArea*/) const
{
	throw CannotUpdateNow ();
}

bool	WordProcessor::Table::EmbeddedTableWordProcessor::QueryInputKeyStrokesPending () const
{
	return true;	// a bit of a hack to encourage display code to do a refresh instead of an Update () call
					// (which would be OK - but it generates a needless exception) - LGP 2003-05-02
	//return fOwningWordProcessor.QueryInputKeyStrokesPending ();
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::SetDefaultUpdateMode (UpdateMode defaultUpdateMode)
{
	if (defaultUpdateMode == eImmediateUpdate) {
		defaultUpdateMode = eDelayedUpdate;
	}
	inherited::SetDefaultUpdateMode (defaultUpdateMode);
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const
{
	if (rhs != NULL) {
		inherited::GetLayoutMargins (row, lhs, NULL);
	}

// MAYBE replace this logic with changing the LHS/RHS according to the cell padding (or what is the term?)
// Anyhow - this hack should work MUCH better than the current display!

	// Make the layout width of each line (paragraph) equal to the windowrect. Ie, wrap to the
	// edge of the window. NB: because of this choice, we must 'InvalidateAllCaches' when the
	// WindowRect changes in our SetWindowRect() override.
	if (rhs != NULL) {
		*rhs = (Led_Max (GetWindowRect ().GetWidth (), 1));
	}
}

void	WordProcessor::Table::EmbeddedTableWordProcessor::PostInteractiveUndoPostHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, InteractiveReplaceCommand::SavedTextRep** afterRep, size_t startOfInsert, const Led_SDK_String& cmdName)
{
	Led_RequireNotNil (beforeRep);
	Led_RequireNotNil (afterRep);
	CommandHandler*	ch	=	GetCommandHandler ();
	Led_RequireNotNil (ch);
	try {
		if (*beforeRep != NULL and *afterRep != NULL) {
			// We are careful to set things to NULL at each stage to prevent double
			// deletes in the event of a badly timed exception
			InteractiveReplaceCommand*	cmd	=	new TableCMD (fOwningTable.GetStart (), fTableRow, fTableColumn, *beforeRep, *afterRep, startOfInsert, cmdName);
			*beforeRep = NULL;
			*afterRep = NULL;
			ch->Post (cmd);
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

InteractiveReplaceCommand::SavedTextRep*	WordProcessor::Table::EmbeddedTableWordProcessor::InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd)
{
	InteractiveReplaceCommand::SavedTextRep*	tableStateRep = inherited::InteractiveUndoHelperMakeTextRep (regionStart, regionEnd, selStart, selEnd);
	return new SavedTextRepWSel (tableStateRep, fOwningTable, SavedTextRepWSel::eWPDirect);
}

Led_Distance	WordProcessor::Table::EmbeddedTableWordProcessor::GetDesiredHeight () const
{
	if (not fDesiredHeightValid) {
		RowReference	startingRow	=	GetRowReferenceContainingPosition (0);
		RowReference	endingRow	=	GetRowReferenceContainingPosition (GetEnd ());
		/*
		*	Always take one more row than they asked for, since they will expect if you start and end on a given row - you'll get
		*	the height of that row.
		*/
		fDesiredHeightValid = true;
		fDesiredHeight = GetHeightOfRows (startingRow, CountRowDifference (startingRow, endingRow) + 1);
	}
	return fDesiredHeight;
}






/*
 ********************************************************************************
 ****************** WordProcessor::Table::SavedTextRepWSel **********************
 ********************************************************************************
 */

#if		qNameLookupXXXInBaseClassWhenItIsNestedSometimesFailsBug
	typedef	InteractiveReplaceCommand::SavedTextRep	SavedTextRep;
#endif
WordProcessor::Table::SavedTextRepWSel::SavedTextRepWSel (SavedTextRep* delegateTo, Table& table, WPRelativeFlag wPRelativeFlag):
	inherited (table.GetStart (), table.GetEnd ()),
	fWPRelativeFlag (wPRelativeFlag),
	fRealRep (delegateTo),
	fRowSelStart (0),
	fRowSelEnd (0),
	fColSelStart (0),
	fColSelEnd (0),
	fIntraCellMode (false),
	fIntraCellSelStart (0),
	fIntraCellSelEnd (0)
{
	Led_RequireNotNil (delegateTo);
	table.GetCellSelection (&fRowSelStart, &fRowSelEnd, &fColSelStart, &fColSelEnd);
	fIntraCellMode = table.GetIntraCellMode ();
	if (fIntraCellMode) {
		table.GetIntraCellSelection (&fIntraCellSelStart, &fIntraCellSelEnd);
	}
}

size_t	WordProcessor::Table::SavedTextRepWSel::GetLength () const
{
	return fRealRep->GetLength ();
}

void	WordProcessor::Table::SavedTextRepWSel::InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite)
{
	fRealRep->InsertSelf (interactor, at, nBytesToOverwrite);
}

void	WordProcessor::Table::SavedTextRepWSel::ApplySelection (TextInteractor* interactor)
{
	fRealRep->ApplySelection (interactor);

	Table*		aT	=	NULL;
	if (fWPRelativeFlag == eWPDirect) {
		EmbeddedTableWordProcessor*	wp	=	dynamic_cast<EmbeddedTableWordProcessor*> (interactor);
		aT = &wp->GetOwningTable ();
	}
	else {
		WordProcessor*	wp	=	dynamic_cast<WordProcessor*> (interactor);
		Led_AssertNotNil (wp);
		aT = wp->GetActiveTable ();	// above fRealRep->ApplySelection should have selected just the right table...
	}
	Led_AssertNotNil (aT);

	Led_AssertNotNil (aT);
	aT->SetCellSelection (fRowSelStart, fRowSelEnd, fColSelStart, fColSelEnd);
	if (fIntraCellMode) {
		aT->SetIntraCellMode ();
		aT->SetIntraCellSelection (fIntraCellSelStart, fIntraCellSelEnd);
	}
	else {
		aT->UnSetIntraCellMode ();
	}	
}







/*
 ********************************************************************************
 ************************ EmptySelectionParagraphSavedTextRep *******************
 ********************************************************************************
 */
typedef	WordProcessor::EmptySelectionParagraphSavedTextRep	EmptySelectionParagraphSavedTextRep;

EmptySelectionParagraphSavedTextRep::EmptySelectionParagraphSavedTextRep (WordProcessor* interactor, size_t selStart, size_t selEnd, size_t at):
	inherited (interactor, selStart, selEnd),
	fSavedStyleInfo (interactor->GetParagraphDatabase ()->GetParagraphInfo (at))
{
}

void	EmptySelectionParagraphSavedTextRep::InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite)
{
	inherited::InsertSelf (interactor, at, nBytesToOverwrite);
	WordProcessor*	wp	=	dynamic_cast<WordProcessor*> (interactor);
	Led_RequireNotNil (wp);
	wp->GetParagraphDatabase ()->SetParagraphInfo (at, 1, IncrementalParagraphInfo (fSavedStyleInfo));
}







#if		qLedUsesNamespaces
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

