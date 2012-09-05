/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/StandardStyledTextInteractor.cpp,v 2.124 2004/02/04 14:17:08 lewis Exp $
 *
 *	$Log: StandardStyledTextInteractor.cpp,v $
 *	Revision 2.124  2004/02/04 14:17:08  lewis
 *	SPR#1627: fixed StandardStyledTextInteractor::SetSelection () code. Earlier fix part of SPR#1604 was erronious.
 *	
 *	Revision 2.123  2004/01/26 20:02:15  lewis
 *	SPR#1604: loose (part of) tweek for SetSelection() override - so we don't optimize away base class call if there is no chnage. Important cuz sometimes the baseclass will still do something useful (e.g. setting the goal-column for arrows occasionally)
 *	
 *	Revision 2.122  2003/11/26 19:57:01  lewis
 *	rename args withWhatBytes->withWhatCharCount. And changed InteractiveReplaceEarlyPostReplaceHook return value to boolean (and compute if a change), and only do the SetSyleInfo if charCount == 1 and if there is a real change - as part of SPR#1564)
 *	
 *	Revision 2.121  2003/09/22 22:06:15  lewis
 *	rename kLedRTFClipType to ClipFormat (& likewise for LedPrivate/HTML). Add FlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromStartOfData (and subclass override to check RTF/HTML etc file fomrats) when were given no good file suffix etc and must peek at file data to guess type. Allow NULL SinkStream to CTOR for a Reader - since we could be just calling QuickLookAppearsToBeRightFormat () which doesnt need an output stream. All for SPR#1552 - so we use same code in OpenDoc code as in when you drop a file on an open Led window (and so we handle read-only files etc better)
 *	
 *	Revision 2.120  2003/09/22 20:21:00  lewis
 *	for SPR#1552: add StyledTextFlavorPackageInternalizer::InternalizeFlavor_HTML. Lose 
 *	StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEDataRawBytes and replace with 
 *	StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromName (and base class 
 *	code to call InternalizeBestFlavor with new suggestedClipType). Moved kRTFClipType from 
 *	FlavorPackage module to StandardStyledTextInteractor module - since not needed in FlavorPackage code.
 *	
 *	Revision 2.119  2003/05/21 12:53:13  lewis
 *	SPR#1495: rename UndoableContextHelper::GetSelStart/End to UndoableContextHelper::GetUndoRegionStart/End
 *	
 *	Revision 2.118  2003/05/20 22:57:38  lewis
 *	SPR#1492: TempMarker::CTOR change in meaning of 3rd arg. SPR#1493:
 *	TextInteractor::InteractiveUndoHelperMakeTextRep () args change (regionStart/End
 *	AND selStart/End). EmptySelStyleTextRep::CTOR now takes selStart/End
 *	
 *	Revision 2.117  2003/04/16 14:04:15  lewis
 *	SPR#1437: cosmetic cleanups - nest code and scope in in braces inside of UndoContext
 *	instances, and use new TextInteractor::BreakInGroupedCommands () wrapper (and other
 *	small cleanups)
 *	
 *	Revision 2.116  2003/04/12 20:41:11  lewis
 *	Lose unneeded extra CTOR for StandardStyledTextIOSinkStream and some other old
 *	ifdefed out code
 *	
 *	Revision 2.115  2003/04/10 19:11:05  lewis
 *	DOCCOMMENT
 *	
 *	Revision 2.114  2003/03/27 16:19:53  lewis
 *	delete ifdefed out code. SPR#1384: StandardStyledTextInteractor::SetEmptySelectionStyle () calls
 *	SimpleUpdater (realUpdateContent=false) since that shouldn't make doc dirty
 *	
 *	Revision 2.113  2003/03/26 16:21:49  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.112  2003/03/25 15:49:07  lewis
 *	SPR#1375- added (back) StandardStyledTextInteractor::SetDefaultFont override - for very
 *	LIMITED purpose. Only set EmptySel (basically) if the current selection is empty.
 *	Helpful occasionally - like when user creates a NEW edit control and says 'set default font'.
 *	Makes for sensible default behavior.
 *	
 *	Revision 2.111  2003/03/21 14:49:10  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.110  2003/03/21 13:59:40  lewis
 *	SPR#1364- fix setting of drag anchor. Moved it from Led_XXX(Win32/MacOS/Gtk) to
 *	TextInteractor::ProcessSimpleClick (adding extra arg). Also lots of other small
 *	code cleanups
 *	
 *	Revision 2.109  2003/03/19 17:09:20  lewis
 *	use SimpleUpdater in one more place where i had been calling old CallDidUpdate...etc...
 *	manual code
 *	
 *	Revision 2.108  2003/03/19 14:44:49  lewis
 *	SPR#1355 - Lose MarkersOfATypeMarkerSink<> since its identical to MarkersOfATypeMarkerSink2Vector<>.
 *	Use the later instead
 *	
 *	Revision 2.107  2003/03/16 16:15:32  lewis
 *	SPR#1345 - as a small part of this SPR - switch to using SimpleUpdater in place of
 *	direct calls to DoAboutToUpdateCalls()
 *	
 *	Revision 2.106  2003/03/14 14:38:55  lewis
 *	SPR#1343 - Get rid of funny code for setting anchor, and override StandardStyledTextInteractor::WhileSimpleMouseTracking
 *	to properly handle tracking over embeddings and fix the WordProcessor::WhileSimpleMouseTracking
 *	to do likewise (it was already correct except for cosmetics). Net effect is that clicking on
 *	embedding works correctly again on Win32 (was OK on ohter platforms) - and is now clearer on ALL
 *	
 *	Revision 2.105  2003/03/11 19:34:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.104  2003/02/28 19:07:43  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.103  2003/02/24 22:02:04  lewis
 *	SPR#1306- fix a few small errors so compiles with qWideCharacters
 *	
 *	Revision 2.102  2003/01/31 23:08:27  lewis
 *	SPR#1278- added InteractiveUpdadeMode and InteractiveModeUpdater so things like AboutToUpdate()
 *	can tell if its a USER change or a program change to text its about to make (and so whether or
 *	not to allow/disallow)
 *	
 *	Revision 2.101  2003/01/29 17:59:54  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont
 *	REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the 'default font'
 *	
 *	Revision 2.100  2003/01/11 19:28:44  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.99  2002/11/20 19:19:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.98  2002/11/18 21:12:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.97  2002/11/14 17:01:12  lewis
 *	SPR#1171- added SetEmptySelectionStyle () overload (with no args). Call from SetSelection_ and do what we
 *	used to do there. Then call in a number of other places in application code to get the seleciton grabbed
 *	
 *	Revision 2.96  2002/10/23 21:28:39  lewis
 *	use qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug for GCC bug workaround
 *	
 *	Revision 2.95  2002/10/22 00:39:03  lewis
 *	Add URL context/menu command - SPR#1136
 *	
 *	Revision 2.94  2002/10/21 12:55:55  lewis
 *	SPR#1135 - Add SummarizeFontAndColorTable method to TextIO::SrcStream and use that to generate
 *	RTF color and font tables
 *	
 *	Revision 2.93  2002/10/14 23:27:30  lewis
 *	use TextStore::SimpleUpdater in StandardStyledTextIOSinkStream::InsertMarker so the marker addition
 *	immediately causes an update.
 *	Was fixed as pat of SPR#1126)
 *	
 *	Revision 2.92  2002/09/28 18:12:15  lewis
 *	SPR#1116 - progress on rtf writing for embedded tables
 *	
 *	Revision 2.91  2002/09/19 14:14:39  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.90  2002/09/17 16:18:47  lewis
 *	SPR#1095 (table support) - added Push/Pop context support in SinkStream to support redirecting contents
 *	to embedded table
 *	
 *	Revision 2.89  2002/05/06 21:33:51  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.88  2001/11/27 00:29:56  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.87  2001/10/17 20:43:00  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.86  2001/09/26 15:41:20  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.85  2001/09/26 00:59:58  lewis
 *	SPR#1045 - when setting the fEmptySelectionStyle field - if you are at the end of
 *	the buffer - then set teh style for that empty bit in the MarkerCover<> at the end
 *	of the buffer to match
 *	
 *	Revision 2.84  2001/09/12 17:23:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.83  2001/09/05 16:26:36  lewis
 *	SPR#1016- cleanup semantics of ReaderFlavorPackage::GetFlavorSize/ReaderFlavorPackage::ReadFlavorData
 *	
 *	Revision 2.82  2001/09/04 21:45:58  lewis
 *	related (though not directly) to SPR#1011. Moved EmptySelStyleTextRep to be a nested class,
 *	rather than a private detail in .cpp file (so could be subclassed to fix SPR#1011).
 *	
 *	Revision 2.81  2001/08/29 23:36:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.80  2001/08/28 18:43:39  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.79  2001/07/20 20:49:25  lewis
 *	improve workaround for qRuntimeCrashMaybeCodeGenBugWithAppStartupBug-
 *	based on suggestion by Gordon Tilman in email dated 2001-07-20
 *	
 *	Revision 2.78  2001/07/19 23:43:41  lewis
 *	qRuntimeCrashMaybeCodeGenBugWithAppStartupBug support
 *	
 *	Revision 2.77  2001/04/26 16:39:30  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.76  2001/02/27 14:53:09  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.75  2001/01/03 14:43:32  Lewis
 *	use  typedef InteractiveReplaceCommand::SavedTextRep inherited(cleanup -
 *	and worked around ambiguity that made BorlandC++ unhappy. And lose some unused code
 *	
 *	Revision 2.74  2000/08/29 15:41:15  lewis
 *	SPR#0810- added qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
 *	define
 *	
 *	Revision 2.73  2000/08/28 20:28:54  lewis
 *	SPR#0828- uyse new UndoableContextHelper helper. RElated changes to PostInteractiveUndoHelper.
 *	SPR#0827- new StandardStyledTextInteractor::CommandNames support.
 *	
 *	Revision 2.72  2000/07/26 05:46:36  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner arg to
 *	CollectAllMarkersInRagne() APIs. And a few related changes. See the SPRs for a table
 *	of speedups.
 *	
 *	Revision 2.71  2000/07/11 15:39:39  lewis
 *	Add a few assertions
 *	
 *	Revision 2.70  2000/07/10 02:51:44  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.69  2000/07/08 01:23:49  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.68  2000/06/23 21:45:14  lewis
 *	SPR#0806- Hook InternalizeFlavor_FILEDataRawBytes () to handle reading in
 *	RTF files (d&d etc)
 *	
 *	Revision 2.67  2000/05/30 23:34:58  lewis
 *	moved more default (empty) implemtantions into StyledTextIOReader::SinkStream
 *	and StyledTextIOWriter::SrcStream so various subclasses are simpler - and so
 *	as we add methods to these bases - we only need todo overrides where we
 *	actually know about the extra data (simple code cleanup)
 *	
 *	Revision 2.66  2000/05/01 19:29:11  lewis
 *	fix StandardStyledTextInteractor::HookStyleDatabaseChanged () to not call SetIn/Externalizer ()
 *	if PeekAtTextStore () == NULL - cuz they get called anyhow in HookGainedTextStore - and cuz this
 *	can get called in virtual DTOR - and thats an unsafe time to be calling more virtual methods
 *	(seems different between MSVC and MWERKS - not sure quite which one was right - but not worth knowing
 *	
 *	Revision 2.65  2000/04/26 21:10:23  lewis
 *	work around wierd RTF quirk with Word2000 occasionally
 *	
 *	Revision 2.64  2000/04/26 14:53:14  lewis
 *	in StyledTextFlavorPackageInternalizer::InternalizeFlavor_Native and _RTF - buffer should be of
 *	type CHAR - not Led_tChar - its a memory buffer - not REALLY an array of characters
 *	
 *	Revision 2.63  2000/04/25 19:02:24  lewis
 *	moved kLedPrivateClipType/kRTFClipType to FlavorPackage module(EXTERN)
 *	
 *	Revision 2.62  2000/04/25 16:16:30  lewis
 *	partial work on SPR#0722- added HidableTextMarkerOwner::GetHidableRegionsWithData () and
 *	started using that.
 *	
 *	Revision 2.61  2000/04/24 22:36:53  lewis
 *	fix a small (fatal) bug in StyledTextFlavorPackageExternalizer::ExternalizeFlavor_STYL
 *	
 *	Revision 2.60  2000/04/24 22:30:50  lewis
 *	bad coment - really fixed mac typon in port of new functionality
 *	
 *	Revision 2.59  2000/04/24 22:29:32  lewis
 *	added @TextInteractor::HookExternalizerChanged - and use it in WordProcessor to sync up with
 *	the hidableText database - related to SPR#0724
 *	
 *	Revision 2.58  2000/04/24 21:34:04  lewis
 *	(hopefully) fix the problems that were Mac-specific
 *	
 *	Revision 2.57  2000/04/24 16:47:03  lewis
 *	Very substantial change to FlavorPackage stuff - now NOT mixed into TextInteractor -
 *	but simply owned using Led_RefCntPtr<>. Also - did all the proper subclassing in WP
 *	class etc to make this work (almost - still must fix OLE embeddings) as before. SPR# 0742
 *	
 *	Revision 2.56  2000/04/16 13:58:55  lewis
 *	small tweeks to get recent changes compiling on Mac
 *	
 *	Revision 2.55  2000/04/14 22:40:26  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.54  2000/04/14 19:17:35  lewis
 *	SPR#0739- react to change where we move much TextIntearctor flavor package code into its
 *	own module, and reorg the updateCursorPos etc stuff into separate code from code specific to each flavor type
 *	
 *	Revision 2.53  2000/04/12 05:53:49  lewis
 *	if Appendtext() called with zero-length text - then don't append a fSavedStyleInfo record
 *	(mostly to avoid assert in DTOR - but at least no point in creating zero-length entry in
 *	that last - and perhaps unwise
 *	
 *	Revision 2.52  2000/03/31 23:45:37  lewis
 *	SPR#0717- got first cut (imperfect) at saving hidableText in RTF (problem is assumes its
 *	currently being shown). Other cleanups - esp to hidable text code (invariants)
 *	
 *	Revision 2.51  2000/03/31 00:50:04  lewis
 *	more work on SPR#0717: support RTF reading of \v tags - hidden text. Integrated HiddenText
 *	support into WordProcessor class (in a maner patterend after how we treat WordProcessor::ParagraphDatabasePtr
 *	
 *	Revision 2.50  1999/12/27 16:03:59  lewis
 *	SPR#0667- SoftLineBreak support
 *	
 *	Revision 2.49  1999/12/18 04:03:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.48  1999/12/18 03:56:47  lewis
 *	0662- break up StyledTextIO module into many _parts
 *	
 *	Revision 2.47  1999/12/14 18:10:28  lewis
 *	Added preliminary qXWindows support
 *	
 *	Revision 2.46  1999/12/09 03:25:30  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.45  1999/12/07 20:07:24  lewis
 *	make StandardStyledTextInteractor::GetContinuousStyleInfo const
 *	
 *	Revision 2.44  1999/11/13 16:32:23  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.43  1999/07/02 14:46:11  lewis
 *	spr#0604- Lose StandardStyledTextInteractor::InteractiveReplace_ and replace with new
 *	StandardStyledTextInteractor::InteractiveReplaceEarlyPostReplaceHook () which overrides new
 *	TextInteractor::InteractiveReplaceEarlyPostReplaceHook(). Also - had to add revised hack
 *	fEmptySelectionStyleSuppressMode to StandardStyledTextInteractor to make this fix work
 *	
 *	Revision 2.42  1999/06/25 00:45:35  lewis
 *	manually call StandardStyledTextIOSinkStream::Flush () in StandardStyledTextInteractor::InternalizeFlavor_*
 *	instead of counting on DTOR todo it automatically, so we can properly handle exceptions (DTOR eats them).
 *	Part of fixing spr# 0595
 *	
 *	Revision 2.41  1999/05/03 22:05:14  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.40  1999/04/28 14:58:08  lewis
 *	Added new methods StandardStyledTextInteractor::Get/SetEmptySelectionStyle- and used within
 *	StandardStyledTextInteractor::SetDefaultFont to simplify (and was needed in LedItView::OnInitialUpdate()
 *	to fix bug where we didnt reset things properly on NEWDOCUMENT call
 *	
 *	Revision 2.39  1999/03/25 02:46:53  lewis
 *	add new method InteractiveSetFont - as a replacement for what people SHOULD be calling to get
 *	the undoable behavior on SetDefaultFont. Later - once people have had time to convert their
 *	calls - we will change the sematnics of SetDefaultFont
 *	
 *	Revision 2.38  1999/03/12 01:38:40  lewis
 *	Improve some doc/comments
 *	
 *	Revision 2.37  1999/03/10 17:17:39  lewis
 *	as part of cleanups - it turned out fSavedInfoStartAt was unneeded. Just use fInsertionStart
 *	
 *	Revision 2.36  1999/03/10 16:48:12  lewis
 *	fix AppendText() method for the case where sometimes called WITH fontSpec and otehrwise NO
 *	
 *	Revision 2.35  1999/03/10 15:40:08  lewis
 *	add assert that a particular condition exists, and if it does for a while, then
 *	simplify the code deleting the test
 *	
 *	Revision 2.34  1999/03/09 22:45:31  lewis
 *	Keep fCached text as direct-object vector instread of a pointer as a simplification / speed tweek.
 *	
 *	Revision 2.33  1999/03/09 22:35:26  lewis
 *	lose fSavedStyleTCharsCount - always can use GetCachedTextSize() instead - must be same.
 *	And re-arange someme code in Flush() code as speed tweek (tested carefully so SHOULD be safe)
 *	
 *	Revision 2.32  1999/03/08 19:21:15  lewis
 *	convert to supporting Led_tChar/UNICODE (untested, but compiles)
 *	
 *	Revision 2.31  1999/02/21 20:04:17  lewis
 *	override Writer::SRC::GetFirstIndent/GetMargins() calls so we can write out RTF /
 *	paragraph margin information
 *	
 *	Revision 2.30  1999/02/21 13:56:24  lewis
 *	Added SetFirstIndent/SetLeftMargin/SetRightMargin/RTF\li\ri\fi, and amended\pard
 *	support for these RTF tags (READING ONLY). Works (in preliminary testing), but with
 *	a few small sprs noted in dbase (validation and fixing defaults)
 *	
 *	Revision 2.29  1998/10/30 14:41:05  lewis
 *	Use vector<> instead of Led_Array.
 *	new MSVC60 compiler warnign workaroudns
 *	
 *	Revision 2.28  1998/05/05  00:30:50  lewis
 *	Support MWERKS CWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.27  1998/04/25  01:33:44  lewis
 *	Get/Set StandardTabTopList calls in Src/SinkStreams
 *
 *	Revision 2.26  1998/04/08  01:50:29  lewis
 *	Added new StandardStyledTextIOSrcStream - based on code moved here from StyledTextIO.
 *	Added mkStandardStyledTextIOSinkStream () etc methods, and used them with auto_ptrs to
 *	get dynamic typing and shared code for creation of srcs (some which know about
 *	justification and some which don't).
 *	But we still share the code for INternalize/Extenralzie etc...
 *
 *
 *
 *
 *		<<RENAMED StandardStyledTextInteractor.cpp FROM StyledTextInteractor.cpp -
 *			and note quirky name due to Mac file name limitation.>>
 *
 *
 *
 *	Revision 2.25  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.24  1997/12/24  03:34:22  lewis
 *	Support RTF as the primary format - not LedPrivateFormat.
 *	Not sure this is 100% straight - will need to test a bit. Also worry about performance problems
 *	this has introduced!!!
 *
 *	Revision 2.23  1997/09/29  15:38:31  lewis
 *	Lose qLedFirstIndex support.
 *	Revised StyleDatabase support to use new Led_RefCntPtr, and MarkerCover<> -
 *	react to small name etc changes.
 *	Change how SetDefaultFOnt() works with Updatemode stuff (see other like
 *	changes in TextImager/TextInteractor).
 *
 *	Revision 2.22  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.21  1997/07/12  20:13:30  lewis
 *	Renamed TextInteractor_ to TextInteractor.
 *	Use string class for command names instead of char*.
 *
 *	Revision 2.20  1997/06/28  17:17:08  lewis
 *	Take advantage of new InteractiveUndoHelperMakeTextRep API, and use that to fix bug with UNDO for empty
 *	selection (like change font with empty selection).
 *	Also related, optimize both case of oringal, and target being empty selection for object we save for undo.
 *	And fix another bug with changing fonts on empty selection - Be sure to call the Doing/Did Update calls.
 *
 *	Revision 2.19  1997/06/24  03:33:04  lewis
 *	Lose old test code
 *
 *	Revision 2.18  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.17  1997/06/18  03:10:36  lewis
 *	CollectAllMarkersInRangeInto cleanups using new MarkersOfATypeMarkerSink
 *	qIncludePrefixFile
 *	_OrSupproundsings Collect cleanups
 *
 *	Revision 2.16  1997/01/10  03:15:30  lewis
 *	throw specifier.
 *
 *	Revision 2.15  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.14  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.13  1996/10/31  00:13:07  lewis
 *	on externalizing best flavor, disable trick where we only write single selcted embedding
 *	for case of RTF-picture, externalizes as unknown type not readable back in (so internal
 *	D&D fails).
 *
 *	Revision 2.12  1996/10/15  18:38:44  lewis
 *	Add Interanlize/Externalize RTF support.
 *
 *	Revision 2.11  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.10  1996/07/19  16:42:23  lewis
 *	Add embeddings with owner=ShdaredStyleDBaze.
 *	In our DTOR, only delete emebddings we own.
 *	And added code in StyleDBASE DTOR to delete its owned emebddings (when it
 *	loses text store - I htink).
 *
 *	Revision 2.9  1996/06/01  02:20:47  lewis
 *	Lose no-RTTI-support.
 *	Fix SetSelection to pay attention to (fLeftSideOfSelectionInteresting) in
 *	updating fEmptyStyle...
 *	FontChangeCommandName and wrap command around font change (quickie hack implementation, but
 *	hopefully adequate for this release).
 *
 *	Revision 2.8  1996/05/23  20:04:36  lewis
 *	Mostly changes for FontSpecification-->Led_FontSpecification - calling accessors isntead
 *	of directly accessing fields mostly.
 *	More code vectoring in InternalizeFlavor_Best.
 *	StandardStyledTextImager::DoContinuousStyle --> StandardStyledTextImager::GetContinuoisStyleInfo()
 *
 *	Revision 2.7  1996/05/14  20:33:10  lewis
 *	Use new Pre/PostReplace APIs (new arg - tmp structure).
 *	Other minor cleanups
 *
 *	Revision 2.6  1996/05/04  21:14:36  lewis
 *	If no clip-formats in a registry entry, then clipAvailForAll = false.
 *
 *	Revision 2.5  1996/04/18  15:36:42  lewis
 *	Lots of changes for new integrated internalize/extenralize flavor drag
 *	drop and cut/paste support, and integrating it with undo.
 *	Most of the code for that is here.
 *
 *	Revision 2.4  1996/03/16  18:48:21  lewis
 *	Revise ProcessSImpleClick() so DidClick/DidOpen stuff works better for
 *	MFC.
 *	Fixed clicked on embedding code to check if click is contained in embedding (was
 *	checking only rhs - so we used to get false posatives).
 *	And now inset click-check rgn a bit so easier to click in between two
 *	adjacent embeddings.
 *
 *	Revision 2.3  1996/03/05  18:33:23  lewis
 *	Re-implemented old Copy/Paste support around new Internalize/Externalize
 *	of FlavorPackages. Then used this new support for drag/drop as well.
 *
 *	Revision 2.2  1996/03/04  07:54:00  lewis
 *	Lots of changes.
 *	Support for StandardStyledTextInteractor::ProcessSimpleClick () to
 *	pass along click/open messages to embeddings.
 *	Totally revamped copy/paste support, including support for native
 *	flavor copy/paste. Support for - when single embedding selected - copy its
 *	native format to clip in specific type. Use our registry of formats
 *	dbase.
 *
 *	Revision 2.1  1996/02/26  22:20:52  lewis
 *	Renamed TextInteracter --> TextInteractor.
 *	Use kEmbeddingSentinalChar - instead of hardwired '\0';
 *	More elaborate support for OnCopyCommand () to make sharing code easier.
 *	Beginnings of support for CopyNativeFormat(), etc...
 *	and flavors.
 *	HookLosingTextStore() to remove our embeddings.
 *
 *	Revision 2.0  1996/02/05  04:17:25  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"StyledTextEmbeddedObjects.h"
#include	"StyledTextIO_HTML.h"
#include	"StyledTextIO_RTF.h"

#include	"StandardStyledTextInteractor.h"




#if		qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
	#include	"StyledTextIO_LedNative.h"
#endif




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	namespace	Led {
#endif




#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (disable : 4786)		//qQuiteAnnoyingDebugSymbolTruncationWarnings
#endif







#if		qMacOS
	const	Led_ClipFormat	kLedPrivateClipFormat	=	'LedP';
	const	Led_ClipFormat	kRTFClipFormat			=	'RTF ';
	const	Led_ClipFormat	kHTMLClipFormat			=	'HTML';
#elif	qWindows
	const	TCHAR	kLedPrivateClipTypeName[]		=	_T ("Led Rich Text Format");
	const	Led_ClipFormat	kLedPrivateClipFormat	=	::RegisterClipboardFormat (kLedPrivateClipTypeName);
	const	TCHAR	kRTFClipTypeName[]				=	_T ("Rich Text Format");
	const	Led_ClipFormat	kRTFClipFormat			=	::RegisterClipboardFormat (kRTFClipTypeName);
	const	TCHAR	kHTMLClipTypeName[]				=	_T ("HTML");									/// MAYBE A BAD NAME - SEE IF ANY WINDOWS STANDARD NAME???
	const	Led_ClipFormat	kHTMLClipFormat			=	::RegisterClipboardFormat (kHTMLClipTypeName);
#elif	qXWindows
			// Toolkit-specific code (e.g. Led_Gtk<>) must reset these to good values. Cannot be constants
			// and cannot be filled in here, cuz we require a DISPLAY object to register the contants on.
			Led_ClipFormat	kLedPrivateClipFormat	=	0;
			Led_ClipFormat	kRTFClipFormat			=	0;
			Led_ClipFormat	kHTMLClipFormat			=	0;
#endif











/*
 ********************************************************************************
 *************************** StandardStyledTextInteractor ***********************
 ********************************************************************************
 */
#if		qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
StandardStyledTextInteractor::CommandNames&	StandardStyledTextInteractor::sCommandNames ()
{
	static	CommandNames	commandNames		=		MakeDefaultCommandNames ();
	return commandNames;
}
#else
StandardStyledTextInteractor::CommandNames	StandardStyledTextInteractor::sCommandNames		=		StandardStyledTextInteractor::MakeDefaultCommandNames ();
#endif

StandardStyledTextInteractor::StandardStyledTextInteractor ():
	TextInteractor (),
	StandardStyledTextImager (),
	fEmptySelectionStyleSuppressMode (false),
	fEmptySelectionStyle (GetStaticDefaultFont ())
{
}

StandardStyledTextInteractor::~StandardStyledTextInteractor ()
{
}

StandardStyledTextInteractor::CommandNames		StandardStyledTextInteractor::MakeDefaultCommandNames ()
{
	StandardStyledTextInteractor::CommandNames	cmdNames;
	cmdNames.fFontChangeCommandName		=	Led_SDK_TCHAROF ("Font Change");
	return cmdNames;
}

void	StandardStyledTextInteractor::HookLosingTextStore ()
{
	TextInteractor::HookLosingTextStore ();
	StandardStyledTextImager::HookLosingTextStore ();
	HookLosingTextStore_ ();
}

void	StandardStyledTextInteractor::HookLosingTextStore_ ()
{
	// Remove all embeddings...
	vector<SimpleEmbeddedObjectStyleMarker*>	embeddings	=	CollectAllEmbeddingMarkersInRange (0, GetLength ());
	for (size_t i = 0; i < embeddings.size (); i++) {
		SimpleEmbeddedObjectStyleMarker*	e	=	embeddings[i];
		if (e->GetOwner () == this) {
			PeekAtTextStore ()->RemoveMarker (e);
			delete e;
		}
	}
}

void	StandardStyledTextInteractor::HookGainedNewTextStore ()
{
	TextInteractor::HookGainedNewTextStore ();
	StandardStyledTextImager::HookGainedNewTextStore ();
	HookGainedNewTextStore_ ();
}

void	StandardStyledTextInteractor::HookGainedNewTextStore_ ()
{
}

/*
@METHOD:		StandardStyledTextInteractor::SetDefaultFont
@DESCRIPTION:	<p>Override @'TextImager::SetDefaultFont' to provide a moderately sensible interpretation
			of that method.</p>
				<p>Note that this definition is significantly different than in Led 3.0. To get
			the Led 3.0 behavior, you should call InteractiveSetFont ()</p>
 */
void	StandardStyledTextInteractor::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont)
{
	size_t	selStart	=	GetSelectionStart ();
	size_t	selEnd		=	GetSelectionEnd ();
	if (selStart == selEnd) {
		size_t					selLength				=	selEnd - selStart;
		Led_FontSpecification	newEmptySelectionStyle	=	fEmptySelectionStyle;
		newEmptySelectionStyle.MergeIn (defaultFont);
		SetStyleInfo (selStart, selLength, defaultFont);
		SetEmptySelectionStyle (newEmptySelectionStyle);
		Refresh ();
	}
	StandardStyledTextImager::SetDefaultFont (defaultFont);
}

/*
@METHOD:		StandardStyledTextInteractor::InteractiveSetFont
@DESCRIPTION:	<p>Applies the given incremental font
			specification to the current selection. If the selection is empty - it stores the specification in
			a special variable used to represent the font of the empty selection. Then when the user types, the
			newly typed characters are in that font.</p>
				<p>This function is meant to implement the underlying semantics of the standard word-processor
			font/style selection menu.</p>
				<p>As such, it is also entered by name in the command UNDO list. If you do <em>not</em> want your
			font changes treated that way, use @'StandardStyledTextImager::SetStyleInfo'.</p>
				<p>Note that this functionality USED to be provided by @'StandardStyledTextInteractor::SetDefaultFont'
			(before Led 3.1a4) - but that is now obsolete. @'TextImager::SetDefaultFont' just sets a default font
			object associated with the imager, and that has little or no effect when used with this class.
 */
void	StandardStyledTextInteractor::InteractiveSetFont (const Led_IncrementalFontSpecification& defaultFont)
{
	InteractiveModeUpdater	iuMode (*this);
	Led_RequireNotNil (PeekAtTextStore ());	// Must specify TextStore before calling this, or any routine that calls it.

	BreakInGroupedCommands ();

	UndoableContextHelper	undoContext (*this, GetCommandNames ().fFontChangeCommandName, false);
		{
			/*
			 *	NB: The SetStyleInfo() call will notify markers via AboutTo/DidUpdate, so long as that code
			 *	percieves there is any change. But if the selection is empty, no style runs will change!
			 *
			 *	Similarly, if there is no selection, SetEmptySelectionStyle () will take care of the notification
			 *	of change.
			 */
			size_t					selLength				=	undoContext.GetUndoRegionEnd () - undoContext.GetUndoRegionStart ();
			Led_FontSpecification	newEmptySelectionStyle	=	fEmptySelectionStyle;
			newEmptySelectionStyle.MergeIn (defaultFont);
			SetStyleInfo (undoContext.GetUndoRegionStart (), selLength, defaultFont);
			SetEmptySelectionStyle (newEmptySelectionStyle);
			Refresh ();
		}
	undoContext.CommandComplete ();
}

Led_IncrementalFontSpecification	StandardStyledTextInteractor::GetContinuousStyleInfo (size_t from, size_t nTChars) const
{
	if (nTChars == 0 and from == GetSelectionStart ()) {
		vector<InfoSummaryRecord>	summaryInfo;
		summaryInfo.push_back (InfoSummaryRecord (fEmptySelectionStyle, 0));
		return (GetContinuousStyleInfo_ (summaryInfo));
	}
	else {
		return StandardStyledTextImager::GetContinuousStyleInfo (from, nTChars);
	}
}

void	StandardStyledTextInteractor::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	TextInteractor::DidUpdateText (updateInfo);
	StandardStyledTextImager::DidUpdateText (updateInfo);
	/*
	 *	SPR#1171 - note that we cannot call SetEmptySelectionStyle () here because it is TOO aggressive.
	 *	changes occur all the time (including setting the font in an empty selection to italics or something
	 *	which trigger a DidUpdate call. Easier and safer to just do the SetEmptySelectionStyle () in the few
	 *	places where I notice its needed.
	 */
}

bool	StandardStyledTextInteractor::ShouldEnablePasteCommand () const
{
	if (TextInteractor::ShouldEnablePasteCommand ()) {
		return true;
	}
	if (Led_ClipboardObjectAcquire::FormatAvailable (kLedPrivateClipFormat)) {
		return true;
	}
	if (Led_ClipboardObjectAcquire::FormatAvailable (kRTFClipFormat)) {
		return true;
	}

	const vector<EmbeddedObjectCreatorRegistry::Assoc>&	types	=	EmbeddedObjectCreatorRegistry::Get ().GetAssocList ();
	for (size_t i = 0; i < types.size (); i++) {
		EmbeddedObjectCreatorRegistry::Assoc	assoc	=	types[i];
		bool	clipAvailForAll	=	true;
		for (size_t j = 0; j < assoc.fFormatTagCount; j++) {
			if (not Led_ClipboardObjectAcquire::FormatAvailable (assoc.GetIthFormat (j))) {
				clipAvailForAll = false;
				break;
			}
		}
		if (clipAvailForAll) {
			return true;
		}
	}
	return false;
}

bool	StandardStyledTextInteractor::CanAcceptFlavor (Led_ClipFormat clipFormat) const
{
	if (TextInteractor::CanAcceptFlavor (clipFormat)) {
		return true;
	}
	if (clipFormat == kLedPrivateClipFormat) {
		return true;
	}
	if (clipFormat == kRTFClipFormat) {
		return true;
	}

	const vector<EmbeddedObjectCreatorRegistry::Assoc>&	types	=	EmbeddedObjectCreatorRegistry::Get ().GetAssocList ();
	for (size_t i = 0; i < types.size (); i++) {
		EmbeddedObjectCreatorRegistry::Assoc	assoc	=	types[i];

		// This may sometimes false-posative - since we may (in priciple) require several other formats at the
		// same time. But this will at least return true whenever we CAN accept the format...
		// Maybe we should redesign/reimplement this API for a future release? LGP 960416
		for (size_t j = 0; j < assoc.fFormatTagCount; j++) {
			if (assoc.GetIthFormat (j) == clipFormat) {
				return true;
			}
		}
	}
	return false;
}

void	StandardStyledTextInteractor::HookStyleDatabaseChanged ()
{
	StandardStyledTextImager::HookStyleDatabaseChanged ();
	if (PeekAtTextStore () != NULL) {
		// if no TextStore - no problem - TextInteractor::HookGainedTextStore () will recreate these.
		SetExternalizer (MakeDefaultExternalizer ());
		SetInternalizer (MakeDefaultInternalizer ());
	}
}

Led_RefCntPtr<FlavorPackageInternalizer>	StandardStyledTextInteractor::MakeDefaultInternalizer ()
{
	return new StyledTextFlavorPackageInternalizer (GetTextStore (), GetStyleDatabase ());
}

Led_RefCntPtr<FlavorPackageExternalizer>	StandardStyledTextInteractor::MakeDefaultExternalizer ()
{
	return new StyledTextFlavorPackageExternalizer (GetTextStore (), GetStyleDatabase ());
}

/*
@METHOD:		StandardStyledTextInteractor::ProcessSimpleClick
@ACCESS:		protected
@DESCRIPTION:	<p>Override @'TextInteractor::ProcessSimpleClick' to handle embeddings.</p>
*/
bool	StandardStyledTextInteractor::ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor)
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
		vector<SimpleEmbeddedObjectStyleMarker*>	embeddingList	=	CollectAllEmbeddingMarkersInRange (clickedOnChar, clickedOnChar + 1);
		Led_Assert (embeddingList.size () == 0 or embeddingList.size () == 1);

		if (embeddingList.size () == 1) {
			SimpleEmbeddedObjectStyleMarker*	embedding	=	embeddingList[0];
			Led_AssertMember (embedding, SimpleEmbeddedObjectStyleMarker);
			switch (clickCount) {
				case	1: {
					if (not extendSelection) {
						SetSelection (clickedOnChar, clickedOnChar+1);
						// select the entire embedding, and then process the rest as usual...
						if (clickCount == 1) {
							// In this case - it really doesn't matter if we pick the LHS or RHS of the embedding
							// as the drag anchor...
							*dragAnchor = clickedOnChar;
						}
						return embedding->HandleClick (clickedAt, 1);
					}
				}
				break;
		
				case	2: {
					//	If we dbl-click on an embedding, then be sure it is still selected, and then try to open it.
					if (not extendSelection) {
						SetSelection (clickedOnChar, clickedOnChar+1);
						// DO OPEN? Anyhow - even if no open, the treating the object as a word makes good sense...
						if (clickCount == 1) {
							// In this case - it really doesn't matter if we pick the LHS or RHS of the embedding
							// as the drag anchor...
							*dragAnchor = clickedOnChar;
						}
						return embedding->HandleClick (clickedAt, 2);
					}
				}
				break;
		
				default: {
					// others are ignored
				}
				break;
			}
		}
	}
	return TextInteractor::ProcessSimpleClick (clickedAt, clickCount, extendSelection, dragAnchor);
}

void	StandardStyledTextInteractor::WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor)
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
		vector<SimpleEmbeddedObjectStyleMarker*>	embeddingList	=	CollectAllEmbeddingMarkersInRange (clickedOnChar, clickedOnChar + 1);
		if (embeddingList.size () == 1) {
			SimpleEmbeddedObjectStyleMarker*	embedding	=	embeddingList[0];
			Led_AssertMember (embedding, SimpleEmbeddedObjectStyleMarker);
			return;
		}
	}
	TextInteractor::WhileSimpleMouseTracking (newMousePos, dragAnchor);
}

void	StandardStyledTextInteractor::InteractiveReplace (const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode)
{
	UpdateMode	useUpdateMode	=	updateMode==eImmediateUpdate? eDelayedUpdate: updateMode;
	Led_Assert (not fEmptySelectionStyleSuppressMode);
	fEmptySelectionStyleSuppressMode = true;
	try {
		TextInteractor::InteractiveReplace (withWhat, withWhatCharCount, useUpdateMode);
		fEmptySelectionStyleSuppressMode = false;
	}
	catch (...) {
		fEmptySelectionStyleSuppressMode = false;
		throw;
	}
	if (updateMode == eImmediateUpdate) {
		Update ();
	}
}

void	StandardStyledTextInteractor::SetSelection (size_t start, size_t end)
{
	bool	changed	=	(GetSelectionStart () != start) or (GetSelectionEnd () != end);
	TextInteractor::SetSelection (start, end);
	if (changed) {
		StandardStyledTextInteractor::SetSelection_ (start, end);
	}
}

void	StandardStyledTextInteractor::SetSelection_ (size_t start, size_t end)
{
	Led_Arg_Unused (start);
	Led_Arg_Unused (end);
	// SetEmptySelectionStyle () assumes selection already set - uses set one - assure that we're called
	// at the right time and that it already HAS been set
	Led_Require (start == GetSelectionStart ());
	Led_Require (end == GetSelectionEnd ());
	if (not fEmptySelectionStyleSuppressMode) {
		SetEmptySelectionStyle ();
	}
}

/*
@METHOD:		StandardStyledTextInteractor::GetEmptySelectionStyle
@DESCRIPTION:	<p>Return the style applied to newly typed text (or interactively entered text) at the current
	selection. This font defaults to the same as the surrounding text. But can be changed under user-control,
	in order to implement the usual semantics of a font / style menu.</p>
		<p>See @'StandardStyledTextInteractor::SetEmptySelectionStyle'.</p>
*/
Led_FontSpecification	StandardStyledTextInteractor::GetEmptySelectionStyle () const
{
	return fEmptySelectionStyle;
}

/*
@METHOD:		StandardStyledTextInteractor::SetEmptySelectionStyle_OVLD
@DESCRIPTION:	<p>Same as @'StandardStyledTextInteractor::SetEmptySelectionStyle' but always grabs style info
			from the surrounding text (called from @'StandardStyledTextInteractor::SetSelection_').</p>
		<p>See @'StandardStyledTextInteractor::GetEmptySelectionStyle'.</p>
*/
void	StandardStyledTextInteractor::SetEmptySelectionStyle ()
{
	size_t	start	=	0;
	size_t	end		=	0;
	GetSelection (&start, &end);
	if (fLeftSideOfSelectionInteresting) {
		if (start < GetTextStore ().GetEnd ()) {
			fEmptySelectionStyle = GetStyleInfo (start);
		}
	}
	else {
		fEmptySelectionStyle = GetStyleInfo (FindPreviousCharacter (end));
	}
	if (end == GetEnd ()) {
		SetStyleInfo (GetEnd (), 1, fEmptySelectionStyle);
	}
}

/*
@METHOD:		StandardStyledTextInteractor::SetEmptySelectionStyle
@DESCRIPTION:	<p>Set the @'Led_FontSpecification' applied to newly typed text (or interactively entered text) at the current
	selection. This font defaults to the same as the surrounding text. But can be changed under user-control,
	in order to implement the usual semantics of a font / style menu.</p>
		<p>Note: if the selection is currently empty, this routine will make appropriate AboutToUpdate/DidUpdate calls to
	notifie anyone interested of the change (so - for example - the cached font metrics of text can change).</p>
		<p>See @'StandardStyledTextInteractor::GetEmptySelectionStyle'.</p>
*/
void	StandardStyledTextInteractor::SetEmptySelectionStyle (Led_FontSpecification newEmptyFontSpec)
{
	if (fEmptySelectionStyle != newEmptyFontSpec) {
		/*
		 *	If we change the empty style selection, this change can affect menus etc (since they show
		 *	the currently selected font which - with an empty selection is really just 'fEmptySelectionStyle').
		 *
		 *	Also - this can change the size of the current row of text (when you change the font of the empty style to make
		 *	it big - even if you type no characters - the row immediately gets bigger).
		 */
		size_t selStart = 0;
		size_t selEnd = 0;
		GetSelection (&selStart, &selEnd);
		if (selStart == selEnd) {
			{
				// not sure if we really need this scoping operator - just added to assure preserving semeantics - for now - LGP 2003-03-16
				TextStore::SimpleUpdater	updater (GetTextStore (), selStart, selEnd, false);
				fEmptySelectionStyle = newEmptyFontSpec;
			}
			if (selEnd == GetEnd ()) {
				SetStyleInfo (GetEnd (), 1, fEmptySelectionStyle);
			}
		}
	}
}

bool	StandardStyledTextInteractor::InteractiveReplaceEarlyPostReplaceHook (size_t withWhatCharCount)
{
	Led_Assert (GetSelectionStart () >= withWhatCharCount);
	if (withWhatCharCount == 1) {
		// If we just typed a single extra char - apply our fEmptySelectionStyle to that extra char typed. Return true iff
		// that caused a font-style change.
		size_t					charAt		=	FindPreviousCharacter (GetSelectionStart ());	// start of char we just added
		Led_FontSpecification	prevStyle	=	GetStyleInfo (charAt);
		if (prevStyle != fEmptySelectionStyle) {
			SetStyleInfo (charAt, withWhatCharCount, Led_IncrementalFontSpecification (fEmptySelectionStyle));
			return true;
		}
	}
	return false;
}

vector<SimpleEmbeddedObjectStyleMarker*>	StandardStyledTextInteractor::CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const
{
	/*
	 *	Walk through all the markers in existence, and throw away all but our
	 *	SimpleEmbeddedObjectStyleMarker markers. This is an inefficient approach. It would be far
	 *	faster to keep a linked, or doubly linked list of all these guys.
	 *	But this approach saves a bit of memory, and til we see this as a problem, lets just
	 *	live with it.
	 */
	MarkersOfATypeMarkerSink2Vector<SimpleEmbeddedObjectStyleMarker>	result;
	GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, result);
	return result.fResult;
}

InteractiveReplaceCommand::SavedTextRep*	StandardStyledTextInteractor::InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd)
{
	if (regionStart == regionEnd) {
		// optimization, cuz these are smaller
		return new EmptySelStyleTextRep (this, selStart, selEnd);
	}
	else {
		return TextInteractor::InteractiveUndoHelperMakeTextRep (regionStart, regionEnd, selStart, selEnd);
	}
}












/*
 ********************************************************************************
 ************************* StandardStyledTextIOSinkStream ***********************
 ********************************************************************************
 */
typedef	StandardStyledTextInteractor::StandardStyledTextIOSinkStream	StandardStyledTextIOSinkStream;
StandardStyledTextIOSinkStream::StandardStyledTextIOSinkStream (
			TextStore* textStore,
			const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
			size_t insertionStart
		):
	inherited (),
	fSavedContexts (),
	fTextStore (textStore),
	fStyleRunDatabase (textStyleDatabase),
	fOriginalStart (insertionStart),
	fInsertionStart (insertionStart),
	fSavedStyleInfo (),
	fCachedText ()
{
	Led_RequireNotNil (textStore);
	Led_Require (not textStyleDatabase.IsNull ());
}

StandardStyledTextIOSinkStream::~StandardStyledTextIOSinkStream ()
{
	try {
		Flush ();
	}
	catch (...) {
		// ignore, cuz cannot fail out of DTOR
	}
}

size_t	StandardStyledTextIOSinkStream::current_offset () const
{
	return (fInsertionStart-fOriginalStart);
}

void	StandardStyledTextIOSinkStream::AppendText (const Led_tChar* text, size_t nTChars, const Led_FontSpecification* fontSpec)
{
	Led_RequireNotNil (text);
	Led_AssertNotNil (fTextStore);

	//	If caching, append the text to an array. Coun't on the array to have efficient
	//	growing properties (does for MSVC50 - grows by factor of two, so log-n append times).
	fCachedText.insert (fCachedText.end (), text, text + nTChars);

	/*
	 * Keep track of the style changes that need to be applied later. If there is NO spec specified, use the one from
	 * the previous section. Check for 'nTChars == 0' - don't append empty info-summary records.
	 */
	if (nTChars != 0) {
		if (fontSpec == NULL) {
			if (fSavedStyleInfo.size () == 0) {
				fSavedStyleInfo.push_back (StandardStyledTextImager::InfoSummaryRecord (fStyleRunDatabase->GetStyleInfo (fOriginalStart, 0)[0], nTChars));
			}
			else {
				fSavedStyleInfo.back ().fLength += nTChars;
			}
		}
		else {
			fSavedStyleInfo.push_back (StandardStyledTextImager::InfoSummaryRecord (*fontSpec, nTChars));
		}
	}
	fInsertionStart += nTChars;
}

void	StandardStyledTextIOSinkStream::ApplyStyle (size_t from, size_t to, const vector<StandardStyledTextImager::InfoSummaryRecord>& styleRuns)
{
	Led_Require (from <= to);
	if (GetCachedTextSize () != 0) {
		Flush ();
	}
	fStyleRunDatabase->SetStyleInfo (fOriginalStart + from, to-from, styleRuns);
}

Led_FontSpecification	StandardStyledTextIOSinkStream::GetDefaultFontSpec () const
{
	return TextImager::GetStaticDefaultFont ();
}

void	StandardStyledTextIOSinkStream::InsertEmbeddingForExistingSentinal (SimpleEmbeddedObjectStyleMarker* embedding, size_t at)
{
	Led_RequireNotNil (embedding);
	if (GetCachedTextSize () != 0) {
		Flush ();
	}
	size_t		effectiveFrom	=	fOriginalStart + at;
	Led_tChar	testSentinal;
	Led_AssertNotNil (fTextStore);
	fTextStore->CopyOut (effectiveFrom, 1, &testSentinal);
	if (testSentinal != kEmbeddingSentinalChar) {
		Led_ThrowBadFormatDataException ();
	}
	#if		qLedUsesNamespaces
		Led::InsertEmbeddingForExistingSentinal (embedding, *fTextStore, effectiveFrom, fStyleRunDatabase);
	#else
		::InsertEmbeddingForExistingSentinal (embedding, *fTextStore, effectiveFrom, fStyleRunDatabase);
	#endif
}

void	StandardStyledTextIOSinkStream::AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding)
{
	Led_RequireNotNil (embedding);
	Led_AssertNotNil (fTextStore);
	if (GetCachedTextSize () != 0) {
		Flush ();
	}
	AddEmbedding (embedding, *fTextStore, fInsertionStart, fStyleRunDatabase);
	fInsertionStart++;
}

void	StandardStyledTextIOSinkStream::AppendSoftLineBreak ()
{
	// Bogus implementation - usually overriden...
	AppendText (LED_TCHAR_OF ("\n"), 1, NULL);
}

void	StandardStyledTextIOSinkStream::InsertMarker (Marker* m, size_t at, size_t length, MarkerOwner* markerOwner)
{
	Led_Require (at <= current_offset ());
	Led_RequireNotNil (m);
	Led_RequireNotNil (markerOwner);
	Led_AssertNotNil (fTextStore);
	if (GetCachedTextSize () != 0) {
		Flush ();
	}
	{
		TextStore::SimpleUpdater	u (*fTextStore, fOriginalStart + at, fOriginalStart + at + length);
		fTextStore->AddMarker (m, fOriginalStart + at, length, markerOwner);
	}
}

void	StandardStyledTextIOSinkStream::Flush ()
{
	if (GetCachedTextSize () != 0) {
		Led_AssertNotNil (fTextStore);
		size_t	dataSize		=	fCachedText.size ();
		size_t	whereToInsert	=	fInsertionStart - dataSize;
		fTextStore->Replace (whereToInsert, whereToInsert, &*fCachedText.begin (), dataSize);
		fCachedText.clear ();

		// Flush the cached style info
		fStyleRunDatabase->SetStyleInfo (whereToInsert, dataSize, fSavedStyleInfo.size (), &*fSavedStyleInfo.begin ());
		fSavedStyleInfo.clear ();
	}
	Led_Ensure (fSavedStyleInfo.size () == 0);
}

void	StandardStyledTextIOSinkStream::PushContext (TextStore* ts,
											const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
											size_t insertionStart
								)
{
	Led_Require (GetCachedTextSize () == 0);	// must flush before setting/popping context

	Context	c;
	c.fTextStore = fTextStore;
	c.fStyleRunDatabase = fStyleRunDatabase;
	c.fInsertionStart = fInsertionStart;
	c.fOriginalStart = fOriginalStart;
	fSavedContexts.push_back (c);
	fTextStore = ts;
	fStyleRunDatabase = textStyleDatabase;
	fInsertionStart = insertionStart;
	fOriginalStart = insertionStart;
}

void	StandardStyledTextIOSinkStream::PopContext ()
{
	Led_Require (GetCachedTextSize () == 0);	// must flush before setting/popping context
	Led_Require (not fSavedContexts.empty ());
	fTextStore = fSavedContexts.back ().fTextStore;
	fStyleRunDatabase = fSavedContexts.back ().fStyleRunDatabase;
	fInsertionStart = fSavedContexts.back ().fInsertionStart;
	fOriginalStart = fSavedContexts.back ().fOriginalStart;
	fSavedContexts.pop_back ();
}







/*
 ********************************************************************************
 **************************** StandardStyledTextIOSrcStream *********************
 ********************************************************************************
 */
typedef	StandardStyledTextInteractor::StandardStyledTextIOSrcStream	StandardStyledTextIOSrcStream;
StandardStyledTextIOSrcStream::StandardStyledTextIOSrcStream (
			TextStore* textStore,
			const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
			size_t selectionStart, size_t selectionEnd
		):
	inherited (),
	fTextStore (textStore),
	fStyleRunDatabase (textStyleDatabase),
	fCurOffset (selectionStart),
	fSelStart (selectionStart),
	fSelEnd (selectionEnd)
{
	Led_RequireNotNil (textStore);
	Led_Require (not textStyleDatabase.IsNull ());
	Led_Require (fSelStart >= 0);
	Led_Require (fSelEnd >= 0);
	fSelEnd = Led_Min (fSelEnd, textStore->GetEnd ());
}

StandardStyledTextIOSrcStream::StandardStyledTextIOSrcStream (StandardStyledTextImager* textImager, size_t selectionStart, size_t selectionEnd):
	inherited (),
	fTextStore (textImager->PeekAtTextStore ()),
	fStyleRunDatabase (textImager->GetStyleDatabase ()),
	fCurOffset (selectionStart),
	fSelStart (selectionStart),
	fSelEnd (selectionEnd)
{
	Led_RequireNotNil (textImager);
	Led_RequireNotNil (fTextStore);
	Led_Require (not fStyleRunDatabase.IsNull ());
	Led_Require (fSelStart >= 0);
	Led_Require (fSelEnd >= 0);
	fSelEnd = Led_Min (fSelEnd, fTextStore->GetEnd ());
}

size_t	StandardStyledTextIOSrcStream::readNTChars (Led_tChar* intoBuf, size_t maxTChars)
{
	Led_AssertNotNil (intoBuf);
	size_t	bytesToRead	=	Led_Min (maxTChars, fSelEnd-fCurOffset);
	Led_Assert (bytesToRead <= maxTChars);
	fTextStore->CopyOut (fCurOffset, bytesToRead, intoBuf);
	fCurOffset += bytesToRead;
	return (bytesToRead);
}

size_t	StandardStyledTextIOSrcStream::current_offset () const
{
	return (fCurOffset-fSelStart);
}

void	StandardStyledTextIOSrcStream::seek_to (size_t to)
{
	Led_Require (to >= 0);
	to += fSelStart;
	to = Led_Min (to, fSelEnd);
	fCurOffset = to;
	Led_Ensure (fCurOffset >= fSelStart);
	Led_Ensure (fCurOffset <= fSelEnd);
}

size_t	StandardStyledTextIOSrcStream::GetTotalTextLength () const
{
	Led_Assert (fSelEnd >= fSelStart);
	return (fSelEnd - fSelStart);
}

vector<StandardStyledTextImager::InfoSummaryRecord>	StandardStyledTextIOSrcStream::GetStyleInfo (size_t from, size_t len) const
{
	size_t	effectiveFrom	=	from + fSelStart;
	#if		qDebug
	size_t	effectiveTo		=	effectiveFrom + len;
	#endif
	Led_Require (effectiveFrom >= fSelStart);
	Led_Require (effectiveFrom <= fSelEnd);
	Led_Require (effectiveTo >= fSelStart);
	Led_Require (effectiveTo <= fSelEnd);
	return (fStyleRunDatabase->GetStyleInfo (effectiveFrom, len));
}

vector<SimpleEmbeddedObjectStyleMarker*>	StandardStyledTextIOSrcStream::CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const
{
	size_t	effectiveFrom	=	from + fSelStart;
	size_t	effectiveTo		=	to + fSelStart;
	Led_Require (effectiveFrom >= fSelStart);
	Led_Require (effectiveFrom <= fSelEnd);
	Led_Require (effectiveTo >= fSelStart);
	Led_Require (effectiveTo <= fSelEnd);

	MarkersOfATypeMarkerSink2Vector<SimpleEmbeddedObjectStyleMarker>	result;
	Led_AssertNotNil (fTextStore);
	fTextStore->CollectAllMarkersInRangeInto (effectiveFrom, effectiveTo, TextStore::kAnyMarkerOwner, result);
	return result.fResult;
}

StandardStyledTextIOSrcStream::Table*	StandardStyledTextIOSrcStream::GetTableAt (size_t /*at*/) const
{
	return NULL;
}

void	StandardStyledTextIOSrcStream::SummarizeFontAndColorTable (set<Led_SDK_String>* fontNames, set<Led_Color>* colorsUsed) const
{
	#if		qUsingDeclarationsToIntroduceNameIntoScopeFailWhereTypedefWorksBug
		typedef	StandardStyledTextImager::InfoSummaryRecord	InfoSummaryRecord;
	#else
//		using	StandardStyledTextImager::InfoSummaryRecord;
	#endif
	if (fontNames != NULL or colorsUsed != NULL) {
		size_t						totalTextLength	=	GetTotalTextLength ();
		vector<StandardStyledTextImager::InfoSummaryRecord>	styleRuns;
		if (totalTextLength != 0) {
			styleRuns = GetStyleInfo (0, totalTextLength);
		}
		for (vector<StandardStyledTextImager::InfoSummaryRecord>::const_iterator i = styleRuns.begin (); i != styleRuns.end (); ++i) {
			if (fontNames != NULL) {
				fontNames->insert ((*i).GetFontName ());
			}
			if (colorsUsed != NULL) {
				colorsUsed->insert ((*i).GetTextColor ());
			}
		}
	}
}

size_t	StandardStyledTextIOSrcStream::GetEmbeddingMarkerPosOffset () const
{
	return (fSelStart);
}












/*
 ********************************************************************************
 ************************* StyledTextFlavorPackageInternalizer ******************
 ********************************************************************************
 */
typedef	StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer	StyledTextFlavorPackageInternalizer;
StyledTextFlavorPackageInternalizer::StyledTextFlavorPackageInternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase):
	inherited (ts),
	fStyleDatabase (styleDatabase)
{
}

void	StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromName (
		#if		qMacOS
			const FSSpec* fileName,
		#elif	qWindows || qXWindows
			const Led_SDK_Char* fileName,
		#endif
			Led_ClipFormat* suggestedClipFormat,
			CodePage* suggestedCodePage
	)
{
	inherited::InternalizeFlavor_FILEGuessFormatsFromName (fileName, suggestedClipFormat, suggestedCodePage);

	#if		qMacOS
		// Should add code here to grab file-type from OS. If called from XXX - then thats already done, but in case
		// called from elsewhere...
	#elif	qWindows
		if (suggestedClipFormat != NULL and *suggestedClipFormat == kBadClipFormat) {
			TCHAR	drive[_MAX_DRIVE];
			TCHAR	dir[_MAX_DIR];
			TCHAR	fname[_MAX_FNAME];
			TCHAR	ext[_MAX_EXT];
			::_tsplitpath (fileName, drive, dir, fname, ext);
			if (::_tcsicmp (ext, Led_SDK_TCHAROF (".rtf")) == 0) {
				*suggestedClipFormat = kRTFClipFormat;
			}
			else if (::_tcsicmp (ext, Led_SDK_TCHAROF (".htm")) == 0) {
				*suggestedClipFormat = kHTMLClipFormat;
			}
			else if (::_tcsicmp (ext, Led_SDK_TCHAROF (".html")) == 0) {
				*suggestedClipFormat = kHTMLClipFormat;
			}
			else if (::_tcsicmp (ext, Led_SDK_TCHAROF (".led")) == 0) {
				*suggestedClipFormat = kLedPrivateClipFormat;
			}
		}
	#endif
}

void	StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromStartOfData (
								Led_ClipFormat* suggestedClipFormat,
								CodePage* suggestedCodePage,
								const Byte* fileStart, const Byte* fileEnd
						)

{
	inherited::InternalizeFlavor_FILEGuessFormatsFromStartOfData (suggestedClipFormat, suggestedCodePage, fileStart, fileEnd);
	if (suggestedClipFormat != NULL) {
		if (*suggestedClipFormat == kBadClipFormat) {
			{
				StyledTextIOSrcStream_Memory	source (fileStart, fileEnd - fileStart);
				StyledTextIOReader_RTF			reader (&source, NULL);
				if (reader.QuickLookAppearsToBeRightFormat ()) {
					*suggestedClipFormat = kRTFClipFormat;
					return;
				}
			}

			{
				StyledTextIOSrcStream_Memory	source (fileStart, fileEnd - fileStart);
				StyledTextIOReader_HTML			reader (&source, NULL);
				if (reader.QuickLookAppearsToBeRightFormat ()) {
					*suggestedClipFormat = kHTMLClipFormat;
					return;
				}
			}

			{
				StyledTextIOSrcStream_Memory			source (fileStart, fileEnd - fileStart);
				StyledTextIOReader_LedNativeFileFormat	reader (&source, NULL);
				if (reader.QuickLookAppearsToBeRightFormat ()) {
					*suggestedClipFormat = kLedPrivateClipFormat;
					return;
				}
			}
		}
	}
}

bool	StyledTextFlavorPackageInternalizer::InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage,
											size_t from, size_t to
										)
{
	Led_Require (from <= GetTextStore ().GetEnd ());
	Led_Require (to <= GetTextStore ().GetEnd ());
	Led_Require (from <= to);

	if (InternalizeFlavor_RTF (flavorPackage, from, to)) {
		return true;
	}
	else if (InternalizeFlavor_HTML (flavorPackage, from, to)) {
		return true;
	}
#if		qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
	else if (InternalizeFlavor_Native (flavorPackage, from, to)) {
		return true;
	}
#endif
#if		qWindows
	// A bit of a hack. MSIE 3.0 generates both FILE and DIB objects on the clip
	// for when we drag out pictures. This allows us to grab the dibs in that case.
	// I just hope it doesn't cause too much trouble for other cases. For Led 2.2, we
	// must completely rewrite this code and find a better way to choose what to
	// grab out of a clip package...
	// LGP 961101
	else if (flavorPackage.GetFlavorAvailable (CF_DIB) and InternalizeFlavor_OtherRegisteredEmbedding (flavorPackage, from, to)) {
		return true;
	}
#endif
	else if (InternalizeFlavor_FILE (flavorPackage, from, to)) {
		return true;
	}
	else if (InternalizeFlavor_OtherRegisteredEmbedding (flavorPackage, from, to)) {
		return true;
	}
	#if		qMacOS
	else if (InternalizeFlavor_STYLAndTEXT (flavorPackage, from, to)) {
		return true;
	}
	#endif
	else if (InternalizeFlavor_TEXT (flavorPackage, from, to)) {
		return true;
	}
	return false;
}

#if		qMacOS
bool	StyledTextFlavorPackageInternalizer::InternalizeFlavor_STYLAndTEXT (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
	size_t	pasteStart	=	from;
	size_t	pasteEnd	=	to;
	Led_Assert (pasteEnd >= pasteStart);

	TempMarker	newSel (GetTextStore (), pasteStart+1, pasteStart+1);
	if (inherited::InternalizeFlavor_TEXT (flavorPackage, pasteStart, pasteEnd)) {
		if (flavorPackage.GetFlavorAvailable ('styl')) {
			size_t	length		=	flavorPackage.GetFlavorSize ('styl');
			Led_SmallStackBuffer<char> buf (length);
			length	=	flavorPackage.ReadFlavorData ('styl', length, buf);
			Led_Assert (newSel.GetStart () >= pasteStart + 1);
			size_t	pasteEndXXX	=	newSel.GetStart () - 1;
			Led_Assert (pasteEndXXX >= pasteStart);
			StScrpRec*	styleRecords	=	reinterpret_cast<StScrpRec*> (static_cast<char*> (buf));
			vector<InfoSummaryRecord>	ledStyleInfo	=	StandardStyledTextImager::Convert (styleRecords->scrpStyleTab, styleRecords->scrpNStyles);
			fStyleDatabase->SetStyleInfo (pasteStart, pasteEndXXX-pasteStart, ledStyleInfo);
		}

		// Even if we have no STYL info, we did already paste the text in, and that would be next
		// on our list to try anyhow...
		return true;
	}
	return false;
}
#endif

#if		qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
bool	StyledTextFlavorPackageInternalizer::InternalizeFlavor_Native (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
	if (flavorPackage.GetFlavorAvailable (kLedPrivateClipFormat)) {
		size_t	length		=	flavorPackage.GetFlavorSize (kLedPrivateClipFormat);
		Led_SmallStackBuffer<char> buf (length);
		length	=	flavorPackage.ReadFlavorData (kLedPrivateClipFormat, length, buf);

		size_t	start	=	from;
		size_t	end		=	to;
		Led_Assert (end >= start);

		GetTextStore ().Replace (start, end, LED_TCHAR_OF (""), 0);	// clear current selection before insert
		{
			// Be sure these guys in scope like this so caches get flusehd before we update cursor position
			StyledTextIOSrcStream_Memory				source (buf, length);
			auto_ptr<StandardStyledTextIOSinkStream>	sink (mkStandardStyledTextIOSinkStream (start));
			StyledTextIOReader_LedNativeFileFormat		textReader (&source, sink.get ());
			textReader.Read ();
			sink->Flush ();	// would be called implcitly in DTOR, but call like this so exceptions get propagates...
		}
		return true;
	}
	else {
		return false;
	}
}
#endif

bool	StyledTextFlavorPackageInternalizer::InternalizeFlavor_RTF (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
	if (flavorPackage.GetFlavorAvailable (kRTFClipFormat)) {
		size_t	length		=	flavorPackage.GetFlavorSize (kRTFClipFormat);
		if (length == 0) {
			// Bizare - but Word2000 seems to sometimes return a zero-length RTF entity when you insert a BitMap ActiveX control into the WP, and
			// try to copy it... LGP 2000/04/26
			return false;
		}
		Led_SmallStackBuffer<char> buf (length);
		length = flavorPackage.ReadFlavorData (kRTFClipFormat, length, buf);

		size_t	start	=	from;
		size_t	end		=	to;
		Led_Assert (end >= start);

		GetTextStore ().Replace (start, end, LED_TCHAR_OF (""), 0);	// clear current selection before insert
		{
			// Be sure these guys in scope like this so caches get flusehd before we update cursor position
			StyledTextIOSrcStream_Memory				source (buf, length);
			auto_ptr<StandardStyledTextIOSinkStream>	sink (mkStandardStyledTextIOSinkStream (start));
			StyledTextIOReader_RTF						textReader (&source, sink.get ());
			textReader.Read ();
			sink->Flush ();	// would be called implcitly in DTOR, but call like this so exceptions get propagates...
		}
		return true;
	}
	else {
		return false;
	}
}

bool	StyledTextFlavorPackageInternalizer::InternalizeFlavor_HTML (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
	if (flavorPackage.GetFlavorAvailable (kHTMLClipFormat)) {
		size_t	length		=	flavorPackage.GetFlavorSize (kHTMLClipFormat);
		Led_SmallStackBuffer<char> buf (length);
		length = flavorPackage.ReadFlavorData (kHTMLClipFormat, length, buf);

		size_t	start	=	from;
		size_t	end		=	to;
		Led_Assert (end >= start);

		GetTextStore ().Replace (start, end, LED_TCHAR_OF (""), 0);	// clear current selection before insert
		{
			// Be sure these guys in scope like this so caches get flusehd before we update cursor position
			StyledTextIOSrcStream_Memory				source (buf, length);
			auto_ptr<StandardStyledTextIOSinkStream>	sink (mkStandardStyledTextIOSinkStream (start));
			StyledTextIOReader_HTML						textReader (&source, sink.get ());
			textReader.Read ();
			sink->Flush ();	// would be called implcitly in DTOR, but call like this so exceptions get propagates...
		}
		return true;
	}
	else {
		return false;
	}
}

bool	StyledTextFlavorPackageInternalizer::InternalizeFlavor_OtherRegisteredEmbedding (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
	const vector<EmbeddedObjectCreatorRegistry::Assoc>&	types	=	EmbeddedObjectCreatorRegistry::Get ().GetAssocList ();
	for (size_t i = 0; i < types.size (); i++) {
		EmbeddedObjectCreatorRegistry::Assoc	assoc	=	types[i];
		bool	clipAvailForAll	=	(assoc.fFormatTagCount != 0);
		for (size_t j = 0; j < assoc.fFormatTagCount; j++) {
			if (not flavorPackage.GetFlavorAvailable (assoc.GetIthFormat (j))) {
				clipAvailForAll = false;
				break;
			}
		}
		if (clipAvailForAll) {
			SimpleEmbeddedObjectStyleMarker*	objMarker	=	(assoc.fReadFromFlavorPackage) (flavorPackage);
			{
				size_t	pasteStart	=	from;
				size_t	pasteEnd	=	to;
				Led_Assert (pasteEnd >= pasteStart);

				GetTextStore ().Replace (pasteStart, pasteEnd, &kEmbeddingSentinalChar, 1);	// clear current selection and put in embedding character

				{	// add marker, and do DID_UPDATE stuff so cached metrics and rowheights get refreshed...
					TextStore::SimpleUpdater updater (GetTextStore (), pasteStart, pasteStart + 1);
					GetTextStore ().AddMarker (objMarker, pasteStart, 1, fStyleDatabase);
				}
			}
			
			return true;
		}
	}
	return false;
}

/*
@METHOD:		StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer::mkStandardStyledTextIOSinkStream
@DESCRIPTION:	<p>Hook function so that the various Externalize_XXX methods in 
			@'StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer' can use a dynamicly typed
			SinkStream. So - for example - the externalize methods include paragraph info.</p>
*/
StandardStyledTextInteractor::StandardStyledTextIOSinkStream*	StyledTextFlavorPackageInternalizer::mkStandardStyledTextIOSinkStream (size_t insertionStart)
{
	return new StandardStyledTextIOSinkStream (PeekAtTextStore (), fStyleDatabase, insertionStart);
}









/*
 ********************************************************************************
 ************************* StyledTextFlavorPackageExternalizer ******************
 ********************************************************************************
 */
typedef	StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer	StyledTextFlavorPackageExternalizer;
StyledTextFlavorPackageExternalizer::StyledTextFlavorPackageExternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase):
	inherited (ts),
	fStyleDatabase (styleDatabase)
{
}

void	StyledTextFlavorPackageExternalizer::ExternalizeFlavors (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
	size_t	start	=	from;
	size_t	end		=	to;
	Led_Require (start >= 0);
	Led_Require (end <= GetTextStore ().GetEnd ());
	Led_Require (start <= end);

	/*
	 * Enumerate in fidelity order flavors to copy...
	 */

	// Directly write out singly selected embedding
	{
		MarkersOfATypeMarkerSink2Vector<SimpleEmbeddedObjectStyleMarker>	embeddings;
		GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, embeddings);
		if ((embeddings.fResult.size () == 1) and (start + 1 == end)) {
			/*
			 *	If we have some embedding like a pict or whatever selected, no reason
			 *	to copy to clip anything else.
			 */
			ExternalizeFlavor_SingleSelectedEmbedding (flavorPackage, embeddings.fResult[0]);
			#if		0
				// WELL, this trick (of returning here) is ALMOST right.
				// At least for the time being, some 'unknown' embeddings get externalized in
				// some format NOBODY - including LED will recognize (eg rtf pictures).
				// This isn't fatal, except
				// in a few rare cases. For example, when we try to 
				return;
			#endif
		}
	}

	// not sure we should do if single selected???? -LGP 961014
	// If NOT, we must be careful about case of unknown RTF embeddings!!! - See SPR# 0397
	ExternalizeFlavor_RTF (flavorPackage, start, end);

	ExternalizeFlavor_TEXT (flavorPackage, start, end);

	#if		qMacOS
		ExternalizeFlavor_STYL (flavorPackage, start, end);
	#endif
}

void	StyledTextFlavorPackageExternalizer::ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
	ExternalizeFlavor_RTF (flavorPackage, from, to);
}

#if		qMacOS
void	StyledTextFlavorPackageExternalizer::ExternalizeFlavor_STYL (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
	Led_Require (from <= to);
	Led_Require (to <= GetTextStore ().GetEnd ());
	size_t	length	=	to - from;

	vector<InfoSummaryRecord>	ledStyleRuns	=	fStyleDatabase->GetStyleInfo (from, length);
	size_t						nStyleRuns		=	ledStyleRuns.size ();

	Led_Assert (offsetof (StScrpRec, scrpStyleTab) == sizeof (short));	// thats why we add sizeof (short)

	size_t						nBytes	=	sizeof (short) + nStyleRuns*sizeof (ScrpSTElement);
	Led_SmallStackBuffer<char>	buf (nBytes);
	StScrpPtr					stylePtr	=	(StScrpPtr)(char*)buf;

	stylePtr->scrpNStyles = nStyleRuns;
	StandardStyledTextImager::Convert (ledStyleRuns, stylePtr->scrpStyleTab);
	flavorPackage.AddFlavorData ('styl', nBytes, stylePtr);
}
#endif

#if		qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
void	StyledTextFlavorPackageExternalizer::ExternalizeFlavor_Native (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
	Led_Require (from <= to);
	Led_Require (to <= GetTextStore ().GetEnd ());
	auto_ptr<StandardStyledTextIOSrcStream>		source (mkStandardStyledTextIOSrcStream (from, to));
	StyledTextIOWriterSinkStream_Memory			sink;
	StyledTextIOWriter_LedNativeFileFormat		textWriter (source.get (), &sink);
	textWriter.Write ();
	flavorPackage.AddFlavorData (kLedPrivateClipFormat, sink.GetLength (), sink.PeekAtData ());
}
#endif

void	StyledTextFlavorPackageExternalizer::ExternalizeFlavor_RTF (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
	Led_Require (from <= to);
	Led_Require (to <= GetTextStore ().GetEnd ());
	auto_ptr<StandardStyledTextIOSrcStream>	source (mkStandardStyledTextIOSrcStream (from, to));
	StyledTextIOWriterSinkStream_Memory		sink;
	StyledTextIOWriter_RTF					textWriter (source.get (), &sink);
	textWriter.Write ();
	flavorPackage.AddFlavorData (kRTFClipFormat, sink.GetLength (), sink.PeekAtData ());
}

void	StyledTextFlavorPackageExternalizer::ExternalizeFlavor_SingleSelectedEmbedding (WriterFlavorPackage& flavorPackage, SimpleEmbeddedObjectStyleMarker* embedding)
{
	Led_RequireNotNil (embedding);
	embedding->ExternalizeFlavors (flavorPackage);
}

/*
@METHOD:		StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer::mkStandardStyledTextIOSrcStream
@DESCRIPTION:	<p>Hook function so that the various Internalize_XXX methods in 
	@'StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer' can use a dynamicly typed
	SinkStream. So - for example - the internalize methods include paragraph info.</p>
*/
StandardStyledTextInteractor::StandardStyledTextIOSrcStream*	StyledTextFlavorPackageExternalizer::mkStandardStyledTextIOSrcStream (size_t selectionStart, size_t selectionEnd)
{
	return new StandardStyledTextIOSrcStream (PeekAtTextStore (), fStyleDatabase, selectionStart, selectionEnd);
}









/*
 ********************************************************************************
 ***************************** EmptySelStyleTextRep *****************************
 ********************************************************************************
 */
typedef	StandardStyledTextInteractor::EmptySelStyleTextRep	EmptySelStyleTextRep;

EmptySelStyleTextRep::EmptySelStyleTextRep (StandardStyledTextInteractor* interactor, size_t selStart, size_t selEnd):
	inherited (selStart, selEnd),
	fSavedStyle (interactor->fEmptySelectionStyle)
{
}

size_t	EmptySelStyleTextRep::GetLength () const
{
	return 0;
}

void	EmptySelStyleTextRep::InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite)
{
	Led_RequireNotNil (interactor);
	interactor->Replace (at, at + nBytesToOverwrite, LED_TCHAR_OF (""), 0);

	StandardStyledTextInteractor*	si	=	dynamic_cast<StandardStyledTextInteractor*> (interactor);
	Led_RequireNotNil (si);	// cannot DO with one type, and UNDO with another!

	si->SetEmptySelectionStyle (fSavedStyle);
}



#if		qLedUsesNamespaces
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

