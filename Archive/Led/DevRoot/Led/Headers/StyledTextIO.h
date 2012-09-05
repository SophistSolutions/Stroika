/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__StyledTextIO_h__
#define	__StyledTextIO_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/StyledTextIO.h,v 1.112 2003/09/22 22:06:10 lewis Exp $
 */


/*
@MODULE:	StyledTextIO
@DESCRIPTION:
		<p>A portable attempt at abstracting away the details of styled text file IO and all
	the different formats for styled text.</p>
		<p>This code defines APIs which should allow for reading any different styled text format.
	And dumping it to any different output (eg. text buffer).</p>
		<p>The only real LED-SPECIFIC parts of this are that I only provide concrete output (aka sinks)
	implementations to Led StandardStyledTextImagers. And that some of the Src/Sink APIs are oriented towards what would be
	helpful for a Led-based editor (in other words, features not supported by Led aren't communicated to/from the src/sinks).</p>
		<p>The <em>big picture</em> for this module is that there are two main basic subdivisions. There are
	@'StyledTextIOReader' subclasses, and @'StyledTextIOWriter' subclasses. The readers are for READING some file format,
	and converting it to a stream of method calls (on a sink to be described later). And writers are for writing those
	formatted data files, based on results of method calls on an abstract source class.</p>
		<p>Though @'StyledTextIOReader' and @'StyledTextIOWriter' share no common base class, they <em>do</em> follow
	a very similar design pattern. They both define abstract 'sources' and 'sinks' for their operation.</p>
		<p>For a @'StyledTextIOReader', it reads its data from a @'StyledTextIOReader::SrcStream' (typically maybe a file),
	and writes it to a @'StyledTextIOReader::SinkStream' (typically a Led-text-buffer/view).</p>
		<p>A @'StyledTextIOWriter', writes data extracted from a @'StyledTextIOWriter::SrcStream'
	(typically view/textstore, much like a @'StyledTextIOReader::SinkStream'),
	and writes it to a @'StyledTextIOWriter::SinkStream' (typically an output file).</p>
		<p>These abstract sources and sinks are defined to just the minimal pure virtual APIs needed to extract/write bare bytes,
	or formatted text in a format Led can understand. Several concrete instantiations of each are provided by Led (some here, and
	some in other modules, as appropriate).</p>
		<p>Subclasses of @'StyledTextIOReader' and @'StyledTextIOWriter' are where the knowledge of particular file formats resides.
	For example, the knowledge of how to read RTF is in @'StyledTextIOReader_RTF' and the knowledge of how to write HTML is in
	@'StyledTextIOWriter_HTML'.</p>
 */

/*
 * Changes:
 *	$Log: StyledTextIO.h,v $
 *	Revision 1.112  2003/09/22 22:06:10  lewis
 *	rename kLedRTFClipType to ClipFormat (& likewise for LedPrivate/HTML). Add FlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromStartOfData (and subclass override to check RTF/HTML etc file fomrats) when were given no good file suffix etc and must peek at file data to guess type. Allow NULL SinkStream to CTOR for a Reader - since we could be just calling QuickLookAppearsToBeRightFormat () which doesnt need an output stream. All for SPR#1552 - so we use same code in OpenDoc code as in when you drop a file on an open Led window (and so we handle read-only files etc better)
 *	
 *	Revision 1.111  2003/05/22 21:03:25  lewis
 *	clarify docs for StyledTextIOReader::SrcStream::read () for case of zero bytes and fixed implemenations to all support a read of zero bytes (came up reading HTML files)
 *	
 *	Revision 1.110  2003/05/01 20:06:40  lewis
 *	SPR#1396: added RTF writing support for table spacing and margins
 *	
 *	Revision 1.109  2003/05/01 01:18:18  lewis
 *	SPR#1396: Added RTF reading support for table cell spacing (trspd{t,l,b,r}). Not perfect (see SPR#1454).
 *	
 *	Revision 1.108  2003/04/30 22:01:29  lewis
 *	SPR#1396: Added RTF READING support for trpadd{b,l.,t.r} and \trgaph tags which specify table margins. Fully supported in WP class SinkStream as well. Now reading these margins in from existing files looks fine. Had to add PRELIMINARY \trleft support to reader to make this work fully. Added new SPR#1453 to take care of \trleft spinnoff issue.
 *	
 *	Revision 1.107  2003/04/18 17:01:35  lewis
 *	small code cleanups to things complained about by new gcc 2.96
 *	
 *	Revision 1.106  2003/04/11 19:53:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.105  2003/03/31 20:12:13  lewis
 *	SPR#1389- lose SplitMergeTableCell and change GetDimensions to GetRows/GetColumns(row). Point is to allow different # of columns per row in a given table
 *	
 *	Revision 1.104  2003/03/21 16:44:27  lewis
 *	SPR#1371 - fix code in StyledTextIOReader_PlainText::Read and StyledTextIOReader_STYLText::Read to read a bit at a time and NOT assume you can seek to MAXINT and the say current_offset () to find out how big the file is. Also - fixed a bug in StyledTextIOReader::BufferedIndirectSrcStream::read to seek before read() on target indirect srcStream
 *	
 *	Revision 1.103  2003/03/20 20:34:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.102  2003/03/20 20:05:36  lewis
 *	SPR#1361- fix (hopefully) BufferedIndirectSrcStream::read () to return the full amount
 *	requested if possible
 *	
 *	Revision 1.101  2003/03/19 01:58:41  lewis
 *	SPR#1352 - lots of speed tweeks including adding read1() method, tweeking existing code for
 *	membuffer::read code and added BufferedIndirectSrcStream and used it from the READER code (still
 *	must fix this to be optional - SPR#1353
 *	
 *	Revision 1.100  2003/02/27 14:18:15  lewis
 *	SPR#1311- Lots more cleanups to table handing to support horizontally merged cells. Reading
 *	and writing of them now mostly works
 *	
 *	Revision 1.99  2003/02/26 20:46:51  lewis
 *	SPR#1311- progress supporting reading RTF from tables for mergecells. Works for some easy cases
 *	
 *	Revision 1.98  2003/01/11 19:28:38  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.97  2002/11/19 12:52:32  lewis
 *	add StyledTextIOWriter::SrcStream::Table::GetOffsetEnd and WriterContext::fCharsToSkip to support
 *	chnages in Table structure in RTF writing (SPR#1174)
 *	
 *	Revision 1.96  2002/10/23 01:00:50  lewis
 *	SPR#1140 - add support for writing cell background color (clcbpat), and adding that color to
 *	the summarizefontandcolortable() routine
 *	
 *	Revision 1.95  2002/10/21 12:55:42  lewis
 *	SPR#1135 - Add SummarizeFontAndColorTable method to TextIO::SrcStream and use that to generate
 *	RTF color and font tables
 *	
 *	Revision 1.94  2002/09/28 18:12:00  lewis
 *	SPR#1116 - progress on rtf writing for embedded tables
 *	
 *	Revision 1.93  2002/09/23 22:13:20  lewis
 *	SPR#1111 - Fixed bug where we didn't process cellx specs for first row, and added clcbpat (cell background
 *	color) support. VERY primitive cruddy implementation. Cleanup soon.
 *	
 *	Revision 1.92  2002/09/19 14:14:14  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 1.91  2002/09/18 04:02:42  lewis
 *	more progress on tables - now support cellx to specify cell widths, and more related stuff -
 *	at least PARTLY supported
 *	
 *	Revision 1.90  2002/09/17 16:20:45  lewis
 *	SPR#1095 (table support) - got BASIC RTF reading working (right # cells and rows and contents read in;
 *	but no attributes like color, size etc for cells
 *	
 *	Revision 1.89  2002/05/06 21:33:34  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.88  2001/11/27 00:29:45  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.87  2001/10/17 20:42:52  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 1.86  2001/09/05 23:44:59  lewis
 *	Added GetSinkStream ().EndOfBuffer () method as part of fix for SPR#1014
 *	
 *	Revision 1.85  2001/08/29 23:00:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.84  2001/08/28 18:43:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.83  2001/07/16 20:07:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.82  2001/07/13 18:57:01  lewis
 *	SPR#0906- got basic (rtf only for now) bullet list support working
 *	
 *	Revision 1.81  2001/07/12 23:06:08  lewis
 *	SPR#0906- support reading indent levels in RTF
 *	
 *	Revision 1.80  2001/07/12 22:42:32  lewis
 *	SPR#0906- Mostly implement RTF reading for bullet-lists. Noticed (but not yet fixed) SPR#0952.
 *	
 *	Revision 1.79  2001/04/13 16:31:08  lewis
 *	cleanup borland C++ warnings
 *	
 *	Revision 1.78  2001/01/03 14:52:26  Lewis
 *	qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS
 *	
 *	Revision 1.77  2000/09/20 08:54:31  lewis
 *	SPR#0837- Add new StyledTextIOReader::BadInputHandler class to replace
 *	StyledTextIOReader::HandleBadlyFormattedInput -
 *	and related changes
 *	
 *	Revision 1.76  2000/08/13 04:15:04  lewis
 *	SPR#0784- rewrite hidden text code based on setting segwidth to zero, and
 *	patching computeNExtCursorPosition. Not 100% done -
 *	but mostly. VERY big change -  and many bug fixes related to this hidden
 *	text stuff folded in as well - but not complete
 *	
 *	Revision 1.75  2000/06/12 22:49:01  lewis
 *	SPR#0767- more work converting stuff (\fi\ri\li\margX etc) to TWIPS -
 *	instead of pixels. Tested a snapshot at this point -
 *	and LedItMFC seemed to work OK (thats all I've tested so far)
 *	
 *	Revision 1.74  2000/06/12 20:11:39  lewis
 *	SPR#0760- convert code to do SpaceBefore/AfterLineSpacing to using new
 *	Led_TWIPS and Led_LineSpacing structs -
 *	rather than using PIXELS
 *	
 *	Revision 1.73  2000/06/12 16:25:13  lewis
 *	convert SpaceBefore to be saved in Led_TWIPS - instead of pixels. Soon
 *	todo much more like this- SPR#0767
 *	
 *	Revision 1.72  2000/05/30 23:34:58  lewis
 *	moved more default (empty) implemtantions into StyledTextIOReader::SinkStream
 *	and StyledTextIOWriter::SrcStream
 *	so various subclasses are simpler - and so as we add methods to these bases -
 *	we only need todo overrides where we
 *	actually know about the extra data (simple code cleanup)
 *	
 *	Revision 1.71  2000/05/30 22:53:02  lewis
 *	SPR#0760- added preliminary support (all but RTF) for SpaceAfter/Before/BetweenLines support
 *	
 *	Revision 1.70  2000/05/30 22:04:28  lewis
 *	SPR#0760- Added preliminary support for word-processor line spacing. RTF reader
 *	support, and internal display support
 *	done. Still todo - RTF Writer support, and GUI for setting common spacing values
 *	
 *	Revision 1.69  2000/04/25 21:58:57  lewis
 *	a bit more work on SPR#0722
 *	
 *	Revision 1.68  2000/04/25 16:16:29  lewis
 *	partial work on SPR#0722- added HidableTextMarkerOwner::GetHidableRegionsWithData ()
 *	and started using that.
 *	
 *	Revision 1.67  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 1.66  2000/04/14 22:40:22  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 1.65  2000/03/31 23:45:36  lewis
 *	SPR#0717- got first cut (imperfect) at saving hidableText in RTF (problem is
 *	assumes its currently being shown).
 *	Other cleanups - esp to hidable text code (invariants)
 *	
 *	Revision 1.64  2000/03/31 00:50:03  lewis
 *	more work on SPR#0717: support RTF reading of \v tags - hidden text. Integrated
 *	HiddenText support into WordProcessor
 *	class (in a maner patterend after how we treat WordProcessor::ParagraphDatabasePtr
 *	
 *	Revision 1.63  1999/12/27 16:03:13  lewis
 *	SPR#0667- SoftLineBreak support in RTF
 *	
 *	Revision 1.62  1999/12/18 03:56:45  lewis
 *	0662- break up StyledTextIO module into many _parts
 *	
 *	Revision 1.61  1999/12/14 18:05:03  lewis
 *	include <memory> - needed by GCC - and add initialized value in CTOR and
 *	cleanups (warnings from GCC)
 *	
 *	Revision 1.60  1999/12/11 02:50:59  lewis
 *	add and use new Led_NEltsOf macro - to count #elts in a C++ array -
 *	was common bug using sizeof inappropriately
 *	with Led_tChars! (for UNICODE)
 *	
 *	Revision 1.59  1999/12/09 03:23:20  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using
 *	new Led_SDK_String/Led_SDK_Char). Still
 *	MUCH more todo to get this really right. Just a patchup job.
 *	
 *	Revision 1.58  1999/11/29 21:03:40  lewis
 *	Cleanup
 *	
 *	Revision 1.57  1999/11/13 16:32:19  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 1.56  1999/08/28 02:24:01  lewis
 *	spr#0621- change HTML reader space/newline handling problem. Basicly -
 *	instead of hack to guess context -
 *	KEEP context in intstance variable
 *	
 *	Revision 1.55  1999/07/13 22:39:14  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various
 *	BugWorkArounds that were only
 *	needed for these old compilers
 *	
 *	Revision 1.54  1999/06/28 20:30:20  lewis
 *	STLDefCTORDeclare_BWA to work around mwerks compiler bug
 *	
 *	Revision 1.53  1999/06/16 16:24:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.52  1999/06/16 16:09:55  lewis
 *	spr#0591- handle &#9; as a tab character in HTML reader
 *	
 *	Revision 1.51  1999/06/16 13:45:34  lewis
 *	spr#0588- add StyledTextIOReader/Writer_HTML::GetEntityRefMapTable ()
 *	virtuals, and use them. And fix
 *	nbsp to only be used on READING - not on writing.
 *	
 *	Revision 1.50  1999/06/15 22:26:03  lewis
 *	spr#0586 - extra spaces output in HTML reader for NL characters -
 *	still kludgely but now works a little better
 *	
 *	Revision 1.49  1999/05/03 22:05:00  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 1.48  1999/04/28 16:18:49  lewis
 *	Add html support for strikeout and underline (was some)
 *	
 *	Revision 1.47  1999/04/28 15:40:16  lewis
 *	add RTF support for Strikeout==\strike. And delete some obsolete(commented out) code
 *	
 *	Revision 1.46  1999/03/09 16:36:52  lewis
 *	add forgotten virtual DTOR for ...Destination_
 *	
 *	Revision 1.45  1999/03/09 16:29:39  lewis
 *	move StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination and ...Desintation_ classes out of class
 *	declaration (still in scope). Use auto_ptr to keep track of fDefaultDestination (so can be moved). Make Flush
 *	(of destination) virtual. ALWAYS keep around an RTFInfo in the reader (though its still an optional arg).
 *	Pass it to the SinkStreamDestination, and lose ITS copy of the fDefaultTabStops. Instead - use the copy in the
 *	RTFInfo object it now owns. Positioing todo same soon for margins/paperwidth
 *	
 *	Revision 1.44  1999/03/08 22:44:41  lewis
 *	Break out RTFInfo stuff which was really just a repository for type declarations,
 *	and store that into new type RTFIO
 *	
 *	Revision 1.43  1999/03/08 21:07:41  lewis
 *	preliminary support for RTF tags - margl/margt\margr\marg...etc and papwerw/paperh -
 *	just the tags and hooks supported so far
 *	
 *	Revision 1.42  1999/03/08 20:28:16  lewis
 *	hack up enuf to at least COMPILE with qWideCharacters - though not nearly enuf to work properly
 *	
 *	Revision 1.41  1999/03/02 22:39:26  lewis
 *	fix spr#0554- had h/v reversed reading in OLE objects and scaling, and had NO code
 *	to write out sizes.
 *	Still not working great (see spr#0555), but much better (added Led_MFC_ControlItem::GetSize()
 *	to help as well)
 *	
 *	Revision 1.40  1999/03/02 04:27:00  lewis
 *	move StyledTextIOReader_RTF::ReaderContext::GroupContext texutally out of class declaration -
 *	though still scoped inside
 *	
 *	Revision 1.39  1999/02/21 20:03:15  lewis
 *	added StyledTextIOWriter::SrcStream::GetFirstIndent/StyledTextIOWriter::SrcStream::GetMargins,
 *	and call them to write out margins/first indent in RTF output
 *	
 *	Revision 1.38  1999/02/21 17:08:48  lewis
 *	lots of cleanups of DOCS (esp abstract src/sinks for readers/writers and overall module docs)
 *	
 *	Revision 1.37  1999/02/21 13:56:22  lewis
 *	Added SetFirstIndent/SetLeftMargin/SetRightMargin/RTF\li\ri\fi, and amended\pard support
 *	for these RTF tags (READING ONLY). Works (in preliminary testing), but with a few small
 *	sprs noted in dbase (validation and fixing defaults)
 *	
 *	Revision 1.36  1999/02/20 23:47:06  lewis
 *	added support for '\fi' rtf tag (though no deep / full support yet
 *	
 *	Revision 1.35  1998/10/30 14:18:52  lewis
 *	Use vector<> template instead of Led_Array<>
 *	
 *	Revision 1.34  1998/05/05  00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *
 *	Revision 1.33  1998/04/25  01:24:18  lewis
 *	Cleanups (mostly RTF).
 *	Support StandardStabstopList, and default tabstop attribte in RTFInfo.
 *
 *	Revision 1.32  1998/04/08  01:39:45  lewis
 *	A little better wrapping of data struc tures (eg.StyledTextIOSrcStream_FileDescriptor::GetBufferSize ()), but
 *	mostly moved StyledTextIOWriterSrcStream_StandardStyledTextImager and StyledTextIOSinkStream_StandardStyledTextImager
 *	to StandardStyledTextIneractor (and WordPRocesor) modules.
 *	For better modularity.
 *
 *	Revision 1.31  1998/03/04  20:17:10  lewis
 *	*** empty log message ***
 *
 *	Revision 1.30  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 1.29  1997/12/24  03:27:35  lewis
 *	Cleanups - but mostly adding support to read/write OLE embeddings in RTF, and arbitrary "Led" objects also.
 *
 *	Revision 1.28  1997/10/01  01:56:38  lewis
 *	Added Get/Set PlainFont() member to RTF reader class.
 *	Added fCachedFontSize - windows-only speed tweek to RTF reader class.
 *	Added fSavedStyleInfo cache and fCachedText caches to StyledTextIOReader::Sink....
 *	(was a major speedup - roughly 4x - see spr#0490.
 *
 *	Revision 1.27  1997/09/29  14:44:26  lewis
 *	Lose qSupportLed21CompatAPI and qLedFirstIndex support.
 *	Added support for per-paragraph justification (using new MarkerCover template).
 *
 *	Revision 1.26  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 1.25  1997/07/23  23:05:08  lewis
 *	docs changes
 *
 *	Revision 1.24  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 1.23  1997/07/12  20:04:20  lewis
 *	Lose _ in a few names, like StyledTextIOReader, SinkStream, TextStore, etc...
 *	AutoDoc.
 *
 *	Revision 1.22  1997/06/28  17:12:31  lewis
 *	Support RTF tags for \li\qc\qr\ri\tx
 *
 *	Revision 1.21  1997/06/23  16:12:50  lewis
 *	Change ControlWordNameMap from map<T> to vector<T>, as speed tweek for RTF reading (on windows).
 *
 *	Revision 1.20  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 1.19  1997/06/18  02:35:29  lewis
 *	RTFReader changes: atomize control words, Keep color table, support coloring of text.
 *	A few new rtf tags.
 *	Lose obsolete/unused StyledTextIOReader_CheckFileAndQueryUserDesiredFormat.
 *	Led_FILE_FORMAT, #6.
 *
 *	Revision 1.18  1997/03/04  20:06:39  lewis
 *	*** empty log message ***
 *
 *	Revision 1.17  1997/01/10  03:01:24  lewis
 *	Made StyledTextIOReader_RTF::ReaderContext public - at perg@wordworks request,
 *	for easier subclassing (Destination_).
 *
 *	Revision 1.16  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 1.15  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 1.14  1996/10/31  00:08:50  lewis
 *	Add ExtractHTMLTagIntoTagNameBuf helper.
 *
 *	Revision 1.13  1996/10/15  18:25:07  lewis
 *	Added qThrowAwayMostUnknownHTMLTags.
 *	(and related support to reader).
 *	Add half-way decent implementation of HTML writer.
 *	(including HTMLInfo support).
 *
 *	Revision 1.12  1996/10/04  16:34:21  lewis
 *	Cleanup/reorganize the RTF reader code around _xxxtag_ paradigm. Several other minor
 *	chagnes/fixes to rtf code.
 *	Many changes to HTML reader. Now it works at least passably.
 *	Lose ability/code for Write_Version4 of Led-private-format.
 *
 *	Revision 1.11  1996/09/30  14:18:46  lewis
 *	Added StyledTextIOReader_::SrcStreamSeekSaver.
 *	A bunch of small changes to RTF code (partly code sharing with new HTML code).
 *	And new HTML code (very preliminary - doesn't really do anything yet).
 *	Slightly better RTF/charset support.
 *
 *	Revision 1.10  1996/09/03  15:02:10  lewis
 *	Added RTF support (based on 2.0 code I wrote for WW). Fixed a few bugs since that old code
 *	and cleaned up soem of the Led StyledTestIO stuff so RTF code worked better (like buffered FileIO).
 *	Also, added InsertEmbedding/AddMarker/current_offset() methods to SinkStreams for reader to make
 *	RTF stuff simpler and less kludgy.
 *
 *	Revision 1.9  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 1.8  1996/06/01  02:04:36  lewis
 *	Version_4/Version_5 support for LedPrivateFormat.
 *
 *	Revision 1.7  1996/05/23  19:24:44  lewis
 *	Use new Led_FontSpecification.
 *
 *	Revision 1.6  1996/05/04  19:18:07  lewis
 *	Added UpdateEOF () method to avoid raising excpetion inside DTOR
 *	(with mwerks cw9, does unexpected then?).
 *
 *	Revision 1.5  1996/04/18  15:11:00  lewis
 *	~StyledTextIOWriterSinkStream_Memory () to plug memory leak.
 *	a few cosmetic changes.
 *
 *	Revision 1.4  1996/03/04  07:40:51  lewis
 *	Added StyledTextIOSinkStream_StandardStyledTextImager::GetCountOfTCharsInserted ().
 *	Moved class EmbeddingSinkStream so it could be shared.
 *
 *	Revision 1.3  1996/02/26  18:40:33  lewis
 *	Add StyledTextIOSrcStream_FileDescriptor and StyledTextIOWriterSinkStream_FileDescriptor.
 *	Use kBadIndex instead of size_t (-1)
 *
 *	Revision 1.2  1996/01/03  23:54:14  lewis
 *	Massive changes. Had supported reading pretty well before. Now support
 *	writing. And added (reading and writing) version of new
 *	LedPrivateFormat. This supports embedding and is what we current use
 *	in ODPart, and for OLE2 writing. And supported for
 *	general style reading.
 *	(in class used lots of places).
 *
 *	Revision 1.1  1995/11/04  23:09:23  lewis
 *	Initial revision
 *
 *
 *
 *
 *
 */

#include	<set>

#include	"LedSupport.h"
#include	"StyledTextImager.h"
#include	"StandardStyledTextImager.h"		//	For StandardStyledTextImager::InfoSummaryRecord declaration
#include	"StyledTextEmbeddedObjects.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif



/*
@CLASS:			StyledTextIOReader
@DESCRIPTION:	<p>Abstract base class for styled text reading. Subclasses know about various styled text file formats, and
	take care of the details of mapping streams of bytes into Led internal data structures with that styled text.</p>
*/
class	StyledTextIOReader {
	public:
		/*
		@CLASS:			StyledTextIOReader::SrcStream
		@DESCRIPTION:	<p>A StyledTextIOReader needs a pointer to a function which is a source of raw bytes
			to be interpretted as text which will be inserted into a text buffer. SrcStream is an abstract class
			defining this API.</p>
		*/
		class	SrcStream {
			public:
				virtual	~SrcStream () {}

			public:
				/*
				@METHOD:		StyledTextIOReader::SrcStream::current_offset
				@DESCRIPTION:	<p>Return the current seekPos</p>
				*/
				virtual	size_t	current_offset () const					=	0;

			public:
				/*
				@METHOD:		StyledTextIOReader::SrcStream::seek_to
				@DESCRIPTION:	<p>Sets the current read-pointer to the given position (often used to scan backwards, but can be any direction).
					Note if 'to' is past end, this just pins one past end of buffer.</p>
				*/
				virtual	void	seek_to (size_t to)						=	0;

			public:
				/*
				@METHOD:		StyledTextIOReader::SrcStream::read
				@DESCRIPTION:	<p>Read the given number of bytes from the is source, and fill them into the buffer. Returns
							the number of bytes read. Always return as many bytes as possible and block if they
							are not yet available. Don't return partial reads (except on EOF). 'bytes' can be zero, and then
							'read' will return immediately with a return value of zero.</p>
				*/
				virtual	size_t	read (void* buffer, size_t bytes)		=	0;

			public:
				/*
				@METHOD:		StyledTextIOReader::SrcStream::read1
				@DESCRIPTION:	<p>Read 1 character and return the number of characters read (0 or 1).
							This is a trivial wrapper on @'StyledTextIOReader::SrcStream::read' which can be overriden
							and is frequently used as a performance optimization.</p>
				*/
				virtual	size_t	read1 (char* c)			{ return read (c, 1); }

			public:
				#if		qMacOS
				/*
				@METHOD:		StyledTextIOReader::SrcStream::GetAUXResourceHandle
				@DESCRIPTION:	<p>MacOS Only. Returns the Mac handle to the resource fork, if any (used to read 'styl' resources).</p>
				*/
				virtual	Handle	GetAUXResourceHandle () const			=	0;
				#endif
		};


		class	SinkStream;
		class	BadInputHandler;

	protected:
		StyledTextIOReader (SrcStream* srcStream, SinkStream* sinkStream, const Led_RefCntPtr<BadInputHandler>& badInputHander = NULL);	// callers responsability to destroy srcStream/sinkStream


	// The Read() method must be overriden by one subclass to provide the format interpretation
	public:
		virtual	void	Read ()								=	0;
		virtual	bool	QuickLookAppearsToBeRightFormat ()	=	0;


	public:
		/*
		@CLASS:			StyledTextIOReader::BufferedIndirectSrcStream
		@BASES:			@'StyledTextIOReader::SrcStream'
		@DESCRIPTION:	<p></p>
		*/
		class	BufferedIndirectSrcStream : public SrcStream {
			public:
				BufferedIndirectSrcStream (SrcStream& realSrcStream);

			public:
				override	size_t	current_offset () const;
				override	void	seek_to (size_t to);
				override	size_t	read (void* buffer, size_t bytes);
				override	size_t	read1 (char* c);
				#if		qMacOS
				override	Handle	GetAUXResourceHandle () const;
				#endif

			private:
				nonvirtual	void	FillCache ();

			private:
				SrcStream&	fRealSrcStream;
				typedef	char	Byte;
				Byte		fWindowTop_Data[4*1024];	// buffer for how much we buffer at a time...
				size_t		fWindowTop_Offset;
				const Byte*	fWindowBottom_Data;
				size_t		fWindowBottom_Offset;
				const Byte*	fCursor_Data;
				size_t		fCursor_Offset;
		};

	public:
		nonvirtual	SrcStream&	GetSrcStream () const;
		nonvirtual	SinkStream&	GetSinkStream () const;
	private:
		mutable BufferedIndirectSrcStream	fSrcStream;
		SinkStream*							fSinkStream;

	public:
		nonvirtual	Led_RefCntPtr<BadInputHandler>	GetBadInputHandler () const;
		nonvirtual	void							SetBadInputHandler (const Led_RefCntPtr<BadInputHandler>& badInputHandler);
	private:
		Led_RefCntPtr<BadInputHandler>	fBadInputHandler;

	public:
		nonvirtual	void	HandleBadlyFormattedInput (bool unrecoverable = false) const;

	protected:
		class	SrcStreamSeekSaver;

	protected:
		struct	ReadEOFException {};	// not an error state necesserily. Just allows our Read helper routines
										// to be simpler. Don't need to worry as much about this special case,
										// and they can just return chars (and throw on eof).

		nonvirtual	void	PutBackLastChar () const;
		nonvirtual	char	GetNextChar () const;
		nonvirtual	char	PeekNextChar () const;
		nonvirtual	void	ConsumeNextChar () const;
		nonvirtual	string	GrabString (size_t from, size_t to = size_t (-1));	// doesn't move seek_to () position (rather restores it)
																				// if no end specified (-1), then grab from to current seekpos
};

// utility to auto-scroll back to place in stream where we were created on DTOR
class	StyledTextIOReader::SrcStreamSeekSaver {
	public:
		SrcStreamSeekSaver (SrcStream& srcStream);
		~SrcStreamSeekSaver ();
	private:
		SrcStream& fSrcStream;
		size_t		fSavedPos;
};







/*
@CLASS:			StyledTextIOReader::SinkStream
@DESCRIPTION:	<p>A StyledTextIOReader needs a pointer to a function which is a sink for all the styled text
			and other information read. SinkStream is an abstract class defining this API.</p>
*/
class	StyledTextIOReader::SinkStream {
	public:
		virtual	~SinkStream () {}

	public:
		/*
		@METHOD:		StyledTextIOReader::SinkStream::current_offset
		@DESCRIPTION:	<p>Return the current seekPos</p>
		*/
		virtual		size_t					current_offset () const																						=	0;	// current seekPos

	public:
		/*
		@METHOD:		StyledTextIOReader::SinkStream::AppendText
		@DESCRIPTION:	<p>Append the given text to the output text buffer. If fontSpec is NULL, use default.
			Probably later we will return and update the fontspec with @'StyledTextIOReader::SinkStream::ApplyStyle'. Note, this style
			of API is defined cuz some format readers give us a bunch of text at once, and then later (elsewhere) store the style
			information. And still others provide them together, hand-in-hand.</p>
		*/
		virtual		void					AppendText (const Led_tChar* text, size_t nTChars, const Led_FontSpecification* fontSpec)					=	0;

	public:
		/*
		@METHOD:		StyledTextIOReader::SinkStream::ApplyStyle
		@DESCRIPTION:	<p>Apply the given style information to the given range of text. See @'StyledTextIOReader::SinkStream::AppendText'.</p>
		*/
		virtual		void					ApplyStyle (size_t from, size_t to, const vector<StandardStyledTextImager::InfoSummaryRecord>& styleRuns)	=	0;

	public:
		/*
		@METHOD:		StyledTextIOReader::SinkStream::GetDefaultFontSpec
		@DESCRIPTION:
		*/
		virtual		Led_FontSpecification	GetDefaultFontSpec () const																					=	0;

	public:
		/*
		@METHOD:		StyledTextIOReader::SinkStream::InsertEmbeddingForExistingSentinal
		@DESCRIPTION:
		*/
		virtual		void					InsertEmbeddingForExistingSentinal (SimpleEmbeddedObjectStyleMarker* embedding, size_t at)					=	0;

	public:
		/*
		@METHOD:		StyledTextIOReader::SinkStream::AppendEmbedding
		@DESCRIPTION:
		*/
		virtual		void					AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding)												=	0;

	public:
		/*
		@METHOD:		StyledTextIOReader::SinkStream::AppendSoftLineBreak
		@DESCRIPTION:
		*/
		virtual		void					AppendSoftLineBreak ()																						=	0;

	public:
		virtual		void	StartTable ();
		virtual		void	EndTable ();
		virtual		void	StartTableRow ();
		virtual		void	EndTableRow ();
		virtual		void	StartTableCell (size_t colSpan);
		virtual		void	EndTableCell ();

	public:
		/*
		@METHOD:		StyledTextIOReader::SinkStream::InsertMarker
		@DESCRIPTION:
		*/
		virtual		void					InsertMarker (Marker* m, size_t at, size_t length, MarkerOwner* markerOwner)								=	0;

	public:
		virtual		void					SetJustification (Led_Justification justification);
		virtual		void					SetStandardTabStopList (const TextImager::StandardTabStopList& tabStops);
		virtual		void					SetFirstIndent (Led_TWIPS tx);
		virtual		void					SetLeftMargin (Led_TWIPS lhs);
		virtual		void					SetRightMargin (Led_TWIPS rhs);
		virtual		void					SetSpaceBefore (Led_TWIPS sb);
		virtual		void					SetSpaceAfter (Led_TWIPS sa);
		virtual		void					SetLineSpacing (Led_LineSpacing sl);	
		virtual		void					SetTextHidden (bool hidden);
		virtual		void					SetListStyle (ListStyle listStyle);
		virtual		void					SetListIndentLevel (unsigned char indentLevel);

	public:
		virtual		void					SetTableBorderColor (Led_Color c);
		virtual		void					SetTableBorderWidth (Led_TWIPS bWidth);
		virtual		void					SetCellWidths (const vector<Led_TWIPS>& cellWidths);
		virtual		void					SetCellBackColor (const Led_Color c);
		virtual		void					SetDefaultCellMarginsForCurrentRow (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right);
		virtual		void					SetDefaultCellSpacingForCurrentRow (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right);

	public:
		/*
		@METHOD:		StyledTextIOReader::SinkStream::EndOfBuffer
		@DESCRIPTION:	<p>Called by StyledText IO readers when end of source buffer is encountered. This means that
				the next Flush () call contains the last of the text.</p>
		*/
		virtual		void					EndOfBuffer ()	{};

	public:
		/*
		@METHOD:		StyledTextIOReader::SinkStream::Flush
		@DESCRIPTION:
		*/
		virtual		void					Flush ()																									=	0;

	public:
		nonvirtual	size_t					GetCountOfTCharsInserted () const;
};






/*
@CLASS:			StyledTextIOReader::BadInputHandler
@DESCRIPTION:
		<p>Abstract base class for styled text writing. Subclasses know about various styled text file formats, and
	take care of the details of mapping Led internal data structures with styled text into streams of bytes in that format.</p>
*/
class	StyledTextIOReader::BadInputHandler {
	public:
		virtual	void	HandleBadlyFormattedInput (const StyledTextIOReader& reader, bool unrecoverable);
};





/*
@CLASS:			StyledTextIOWriter
@DESCRIPTION:
		<p>Abstract base class for styled text writing. Subclasses know about various styled text file formats, and
	take care of the details of mapping Led internal data structures with styled text into streams of bytes in that format.</p>
*/
class	StyledTextIOWriter {
	public:
		class	SrcStream;
		class	SinkStream;

	protected:
		StyledTextIOWriter (SrcStream* srcStream, SinkStream* sinkStream);	// callers responsability to destroy srcStream/sinkStream


	// The Read() method must be overriden by one subclass to provide the format interpretation
	public:
		virtual	void	Write ()							=	0;

	public:
		nonvirtual	SrcStream&	GetSrcStream () const;
		nonvirtual	SinkStream&	GetSinkStream () const;

	private:
		SrcStream*		fSrcStream;
		SinkStream*		fSinkStream;

	// Utilities
	protected:
		nonvirtual	void	write (const void* data, size_t nBytes);
		nonvirtual	void	write (char c);
		nonvirtual	void	write (const char* str);
		nonvirtual	void	write (const string& str);
};

/*
@CLASS:			StyledTextIOWriter::SrcStream
@DESCRIPTION:	<p>Abstract base class for @'StyledTextIOWriter's to get their text content from.</p>
*/
class	StyledTextIOWriter::SrcStream {
	public:
		typedef	TextImager::StandardTabStopList	StandardTabStopList;
	public:
		virtual	~SrcStream () {}

	public:
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::readNTChars
		@DESCRIPTION:	<p>readNTChars can retun less than maxTChars before end of buffer, only to make us end on even
			mbyte char boundary.</p>
		*/
		virtual	size_t	readNTChars (Led_tChar* intoBuf, size_t maxTChars)					=	0;

	public:
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::current_offset
		@DESCRIPTION:	<p>current seekPos</p>
		*/
		virtual	size_t	current_offset () const												=	0;

	public:
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::seek_to
		@DESCRIPTION:	<p>'to' past end just pins one past end of buffer</p>
		*/
		virtual	void	seek_to (size_t to)													=	0;

	public:
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::GetTotalTextLength
		@DESCRIPTION:	<p>Total # of tChars</p>
		*/
		virtual	size_t	GetTotalTextLength () const											=	0;

	public:
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::GetStyleInfo
		@DESCRIPTION:
		*/
		virtual	vector<StandardStyledTextImager::InfoSummaryRecord>
						GetStyleInfo (size_t from, size_t len) const						=	0;

	public:
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::CollectAllEmbeddingMarkersInRange
		@DESCRIPTION:
		*/
		virtual	vector<SimpleEmbeddedObjectStyleMarker*>
						CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const	=	0;

	public:
		class	Table;

		/*
		@METHOD:		StyledTextIOWriter::SrcStream::GetTableAt
		@DESCRIPTION:	Return a @'StyledTextIOWriter::SrcStream::Table' object. Note - this is not to be
					confused with a @'WordProcessor::Table' object. Though they are closely related, this object
					contains just the API required for writing tables to files.
		*/
		virtual	Table*		GetTableAt (size_t at) const	=	0;


	public:
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::SummarizeFontAndColorTable
		@DESCRIPTION:	<p>Produce a list of all fontnames and colors used in the document. This is needed for some formats
					like RTF which require a list of all font names and colors before writing any of the rest of the document.</p>
		*/
		virtual	void	SummarizeFontAndColorTable (set<Led_SDK_String>* fontNames, set<Led_Color>* colorsUsed) const	=	0;

	public:
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::GetEmbeddingMarkerPosOffset
		@DESCRIPTION:	<p>Since we maybe externalizing a subset of the buffer, and the marker positions in the embedding object
			are absolute, we need to know this to relativize them in the externalized stream</p>
		*/
		virtual	size_t	GetEmbeddingMarkerPosOffset () const								=	0;

	public:
		virtual		Led_Justification							GetJustification ()	const;
		virtual		StandardTabStopList							GetStandardTabStopList () const;
		virtual		Led_TWIPS									GetFirstIndent () const;
		virtual		void										GetMargins (Led_TWIPS* lhs, Led_TWIPS* rhs) const;
		virtual		Led_TWIPS									GetSpaceBefore () const;
		virtual		Led_TWIPS									GetSpaceAfter () const;
		virtual		Led_LineSpacing								GetLineSpacing () const;	
		virtual		void										GetListStyleInfo (ListStyle* listStyle, unsigned char* indentLevel) const;	
		virtual		Led_tChar									GetSoftLineBreakCharacter () const;
		virtual		DiscontiguousRun<bool>						GetHidableTextRuns () const;
};




/*
@CLASS:			StyledTextIOWriter::SrcStream::Table
@DESCRIPTION:	<p>Simple abstract API so styled text IO code can ask key questions about a table object in order to persist it..</p>
*/
class	StyledTextIOWriter::SrcStream::Table {
	public:
		struct	CellInfo {
			CellInfo ();
			Led_TWIPS	f_cellx;
			Led_Color	f_clcbpat;		// cell background color
		};
	public:
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::Table::GetRows
		@DESCRIPTION:	<p>Get the number of rows in the given table.</p>
		*/
		virtual	size_t							GetRows () const										=	0;
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::Table::GetColumns
		@DESCRIPTION:	<p>Get the number of columns in the given row. Note that this can be
					different from row to row within a given table.</p>
		*/
		virtual	size_t							GetColumns (size_t row) const							=	0;
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::Table::GetRowInfo
		@DESCRIPTION:	<p>Get the @'StyledTextIOWriter::SrcStream::Table::CellInfo's for the given
					row. The number of these records is the same as the return value from
					@'StyledTextIOWriter::SrcStream::Table::GetColumns'.
					</p>
		*/
		virtual	void							GetRowInfo (size_t row, vector<CellInfo>* cellInfos)	=	0;
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::Table::MakeCellSubSrcStream
		@DESCRIPTION:	<p>Returns a @'StyledTextIOWriter::SrcStream' containing the data for the given table cell.
					Note that this can return NULL for 'merged' cells.</p>
		*/
		virtual	StyledTextIOWriter::SrcStream*	MakeCellSubSrcStream (size_t row, size_t column)		=	0;
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::Table::GetOffsetEnd
		@DESCRIPTION:	<p>Returns the source-doc offset of the given table end relative to its start. Readers should
					skip ahead that many positions after writing the tables contents.</p>
		*/
		virtual	size_t							GetOffsetEnd () const									=	0;
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::Table::GetDefaultCellMarginsForRow
		@DESCRIPTION:	<p>Return the default cell margins for the given row. Note that the @'Led_TWIPS_Rect'
					is not a true rectangle, but just a handy way to return 4 values - a top/left/bottom/right.</p>
		*/
		virtual		Led_TWIPS_Rect				GetDefaultCellMarginsForRow (size_t row) const			=	0;
		/*
		@METHOD:		StyledTextIOWriter::SrcStream::Table::GetDefaultCellSpacingForRow
		@DESCRIPTION:	<p>Return the default cell spacing for the given row. Note that the @'Led_TWIPS_Rect'
					is not a true rectangle, but just a handy way to return 4 values - a top/left/bottom/right.</p>
		*/
		virtual		Led_TWIPS_Rect				GetDefaultCellSpacingForRow (size_t row) const			=	0;
};




/*
@CLASS:			StyledTextIOWriter::SinkStream
@DESCRIPTION:	<p>Abstract base class for @'StyledTextIOWriter's to dump their text content to.</p>
*/
class	StyledTextIOWriter::SinkStream {
	public:
		virtual	~SinkStream () {}

	public:
		/*
		@METHOD:		StyledTextIOWriter::SinkStream::current_offset
		@DESCRIPTION:	<p>Return the current seekPos.</p>
		*/
		virtual	size_t	current_offset () const						=	0;

	public:
		/*
		@METHOD:		StyledTextIOWriter::SinkStream::seek_to
		@DESCRIPTION:	<p>'to' past end just pins one past end of buffer</p>
		*/
		virtual	void	seek_to (size_t to)							=	0;

	public:
		/*
		@METHOD:		StyledTextIOWriter::SinkStream::write
		@DESCRIPTION:
		*/
		virtual	void	write (const void* buffer, size_t bytes)	=	0;
};





/*
 ********* Some StyledTextIOReader::SrcStream subclasses *********
 */
/*
@CLASS:			StyledTextIOSrcStream_Memory
@BASES:			@'StyledTextIOReader::SrcStream'
@DESCRIPTION:	<p>If you have a block of memory which contains the untyped contents which will be converted by some
	reader (@'StyledTextIOReader'), you use this as the @'StyledTextIOReader::SrcStream'. Just initialize one of these
	with the appropriate data, and pass this to the appropriate @'StyledTextIOReader'.</p>
		<p>NB: This class doesn't free up, or copy the given pointer. It is up the the caller todo that, and only after
	this SrcStream object has been destroyed. Typically, this follows trivially from a sequential, stack-based allocation
	strategy, where the data comes from some object declared earlier on the stack.</p>
*/
class	StyledTextIOSrcStream_Memory : public StyledTextIOReader::SrcStream {
	public:
		StyledTextIOSrcStream_Memory (
				const void* data, size_t nBytes
				#if		qMacOS
				, Handle resourceHandle = NULL
				#endif
			);

	public:
		override	size_t	current_offset () const;
		override	void	seek_to (size_t to);
		override	size_t	read (void* buffer, size_t bytes);
		override	size_t	read1 (char* c);
		#if		qMacOS
		override	Handle	GetAUXResourceHandle () const;
		#endif

	private:	
		const	void*	fData;
		const	void*	fDataEnd;
		size_t			fBytesInBuffer;
		const	void*	fCurPtr;
		#if		qMacOS
		Handle			fResourceHandle;
		#endif
};






inline	StyledTextIOReader::BufferedIndirectSrcStream::BufferedIndirectSrcStream (SrcStream& realSrcStream):
		fRealSrcStream (realSrcStream),
		//fWindowTop_Data (),
		fWindowTop_Offset (size_t (-1)),
		fWindowBottom_Data (NULL),
		fWindowBottom_Offset (size_t (-1)),
		fCursor_Data (NULL),
		fCursor_Offset (0)
		{
		}
inline	void	StyledTextIOReader::BufferedIndirectSrcStream::FillCache ()
	{
		fWindowTop_Offset = fCursor_Offset;
		fRealSrcStream.seek_to (fWindowTop_Offset);	// probably could frequently optimize this call way if we were careful to cache last seek-offset from buffer
		size_t bytesRead = fRealSrcStream.read (fWindowTop_Data, Led_NEltsOf (fWindowTop_Data));
		fWindowBottom_Data = fWindowTop_Data + bytesRead;
		fWindowBottom_Offset = fWindowTop_Offset + bytesRead;
Led_Assert (fCursor_Offset >= fWindowTop_Offset and fCursor_Offset <= fWindowBottom_Offset);	// should only call FillCache in that case?
Led_Assert (fCursor_Offset == fWindowTop_Offset);	// should only call FillCache in that case?
		if (fCursor_Offset >= fWindowTop_Offset and fCursor_Offset <= fWindowBottom_Offset) {
			fCursor_Data = fWindowTop_Data + (fCursor_Offset - fWindowTop_Offset);
		}
		Led_Ensure (fWindowTop_Data <= fCursor_Data and fCursor_Data <= fWindowBottom_Data);
		Led_Ensure (fWindowTop_Offset <= fCursor_Offset and fCursor_Offset <= fWindowBottom_Offset);
	}
inline	size_t	StyledTextIOReader::BufferedIndirectSrcStream::current_offset () const
	{
		return fCursor_Offset;
	}
inline	void	StyledTextIOReader::BufferedIndirectSrcStream::seek_to (size_t to)
	{
		// If seekpos inside our window (at end of buffer counts as inside window even though next read may force a FillCache),
		// just update offset(s), and otherwise - mark fCursor_Data as NULL so we know cache invalid
		if (fWindowTop_Offset <= to and to <= fWindowBottom_Offset) {
			fCursor_Data = fWindowTop_Data + (to - fWindowTop_Offset);
		}
		else {
			fCursor_Data = NULL;
		}
		fCursor_Offset = to;
	}
inline	size_t	StyledTextIOReader::BufferedIndirectSrcStream::read (void* buffer, size_t bytes)
	{
		Led_RequireNotNil (buffer);

		Byte*	destCursor		=	reinterpret_cast<Byte*> (buffer);
		size_t	bytesReadSoFar	=	0;

		/*
		 *	See if the initial part of this request can be satisfied by our current buffered data
		 *	and updated 'bytesReadSoFar' to reflect how much read from that buffer.
		 */
		if (fCursor_Data != NULL and fWindowTop_Offset >= fCursor_Offset and fCursor_Offset < fWindowBottom_Offset) {
			size_t	bytesAvail		=	fWindowBottom_Offset - fCursor_Offset;	// must be > 0 UNLESS we are at EOF
			size_t	thisReadCount	=	min (bytesAvail, bytes);
			Led_AssertNotNil (fCursor_Data);
			(void)::memcpy (destCursor, fCursor_Data, thisReadCount);
			destCursor += thisReadCount;
			fCursor_Data += thisReadCount;
			fCursor_Offset += thisReadCount;
			bytesReadSoFar += thisReadCount;
		}

		/*
		 *	If we've not completed the request, see if it can be accomodated by by filling the buffer,
		 *	and trying to pull data out of that buffer. If not - then simply read the data directly.
		 */
		if (bytesReadSoFar < bytes) {
			size_t	bytesLeftToRead	=	bytes - bytesReadSoFar;
			if (bytesLeftToRead < Led_NEltsOf (fWindowTop_Data)) {
				FillCache ();
				size_t	bytesAvail		=	fWindowBottom_Offset - fCursor_Offset;	// must be > 0 UNLESS we are at EOF
				size_t	thisReadCount	=	min (bytesAvail, bytesLeftToRead);
				Led_AssertNotNil (fCursor_Data);
				(void)::memcpy (destCursor, fCursor_Data, thisReadCount);
				destCursor += thisReadCount;
				fCursor_Data += thisReadCount;
				fCursor_Offset += thisReadCount;
				bytesReadSoFar += thisReadCount;
			}
			else {
				fRealSrcStream.seek_to (fCursor_Offset);
				size_t bytesRead = fRealSrcStream.read (destCursor, bytesLeftToRead);
				bytesReadSoFar += bytesRead;
				fCursor_Offset += bytesRead;
				// Cache is invalid - so mark it so...
				fCursor_Data = NULL;
			}
		}
		return bytesReadSoFar;
	}
inline	size_t	StyledTextIOReader::BufferedIndirectSrcStream::read1 (char* c)
	{
		Led_RequireNotNil (c);
		/*
		 *	See if we can read ANY non-zero number of bytes out of our window. If yes - then just
		 *	return those (even if thats less than the user requested - following standard UNIX read
		 *	conventions). If we cannot read any bytes given our current window, refill the window, and
		 *	try again.
		 */
		if ((fCursor_Data == NULL) or (fCursor_Offset < fWindowTop_Offset or fCursor_Offset >= fWindowBottom_Offset)) {
			FillCache ();
		}
		Led_Assert (fWindowTop_Offset <= fCursor_Offset and fCursor_Offset <= fWindowBottom_Offset);
		if (fWindowBottom_Offset == fCursor_Offset) {
			return 0;
		}
		else {
			Led_AssertNotNil (fCursor_Data);
			*c = *fCursor_Data;
			fCursor_Data++;
			fCursor_Offset++;
			return 1;
		}
	}
#if		qMacOS
inline	Handle	StyledTextIOReader::BufferedIndirectSrcStream::GetAUXResourceHandle () const
	{
		return fRealSrcStream.GetAUXResourceHandle ();
	}
#endif





/*
@CLASS:			StyledTextIOSrcStream_FileDescriptor
@BASES:			@'StyledTextIOReader::SrcStream'
@DESCRIPTION:	<p>If you have a file which contains the untyped contents which will be converted by some
	reader (@'StyledTextIOReader'), you use this as the @'StyledTextIOReader::SrcStream'. Just initialize one of these
	with the open file descriptor, and pass this to the appropriate @'StyledTextIOReader'.</p>
		<p>NB: This class doesn't close the argument file descriptor. It is up the the caller todo that, and only after
	this SrcStream object has been destroyed. Typically, this follows trivially from a sequential, stack-based allocation
	strategy, where the data comes from some object declared earlier on the stack.</p>
*/
class	StyledTextIOSrcStream_FileDescriptor : public StyledTextIOReader::SrcStream {
	public:
		// NB: On the Mac - this FD refers to a mac file access path - not the result of an ::open () call.
		StyledTextIOSrcStream_FileDescriptor (
				int fd
				#if		qMacOS
				, Handle resourceHandle = NULL
				#endif
			);
		virtual ~StyledTextIOSrcStream_FileDescriptor ();

	public:
		override	size_t	current_offset () const;
		override	void	seek_to (size_t to);
		override	size_t	read (void* buffer, size_t bytes);
		#if		qMacOS
		override	Handle	GetAUXResourceHandle () const;
		#endif

	public:
		nonvirtual	size_t	GetBufferSize () const;
		nonvirtual	void	SetBufferSize (size_t bufSize);	// set to zero for unbuffered IO
	private:
		nonvirtual	void	ReadInWindow (size_t startAt);	// zero based startAt

	private:	
		int				fFileDescriptor;
		size_t			fCurSeekPos;
		char*			fInputBuffer;		// buffer is simply a performance hack...
		size_t			fInputBufferSize;
		size_t			fBufferWindowStart;
		size_t			fBufferWindowEnd;
		#if		qMacOS
		Handle			fResourceHandle;
		#endif
};





/*
 ********* Some StyledTextIOReader::SrcStream subclasses *********
 */
/*
@CLASS:			StyledTextIOWriterSinkStream_Memory
@BASES:			@'StyledTextIOWriter::SinkStream'
@DESCRIPTION:
*/
class	StyledTextIOWriterSinkStream_Memory : public StyledTextIOWriter::SinkStream {
	public:
		StyledTextIOWriterSinkStream_Memory ();
		~StyledTextIOWriterSinkStream_Memory ();
	private:	// prevent accidental copying
		StyledTextIOWriterSinkStream_Memory (const StyledTextIOWriterSinkStream_Memory&);
		void operator= (const StyledTextIOWriterSinkStream_Memory&);

	public:
		override	size_t	current_offset () const;
		override	void	seek_to (size_t to);
		override	void	write (const void* buffer, size_t bytes);

		nonvirtual	const void*	PeekAtData () const;
		nonvirtual	size_t		GetLength () const;

	private:
		char*	fData;
		size_t	fBytesUsed;
		size_t	fBytesAllocated;
		char*	fCurPtr;
};





/*
@CLASS:			StyledTextIOWriterSinkStream_FileDescriptor
@BASES:			@'StyledTextIOWriter::SinkStream'
@DESCRIPTION:
*/
class	StyledTextIOWriterSinkStream_FileDescriptor : public StyledTextIOWriter::SinkStream {
	public:
		// NB: On the Mac - this FD refers to a mac file access path - not the result of an ::open () call.
		StyledTextIOWriterSinkStream_FileDescriptor (int fd);
		// NB: flushes, but doesn't close on DTOR
		~StyledTextIOWriterSinkStream_FileDescriptor ();

	public:
		override	size_t	current_offset () const;
		override	void	seek_to (size_t to);
		override	void	write (const void* buffer, size_t bytes);

	public:
		nonvirtual	size_t	GetBufferSize () const;
		nonvirtual	void	SetBufferSize (size_t bufSize);	// set to zero for unbuffered IO

	// This is done automatically on DTOR, but we cannot throw exceptions out of there, so if you want
	// to be sure exception is thrown if this operation fails, call this EXPLICITLY.
	public:
		nonvirtual	void	Flush ();
		nonvirtual	void	UpdateEOF ();

	private:
		int			fFileDescriptor;
		char*		fOutputBuffer;		// buffer is simply a performance hack...
		size_t		fOutputBufferSize;
		size_t		fBufferWindowStart;
		size_t		fBufferWindowEnd;
		size_t		fFurthestDiskWriteAt;
		size_t		fCurSeekPos;		// zero-based
};







/*
@CLASS:			EmbeddingSinkStream
@BASES:			@'SimpleEmbeddedObjectStyleMarker::SinkStream'
@DESCRIPTION:
*/
class	EmbeddingSinkStream : public SimpleEmbeddedObjectStyleMarker::SinkStream {
	public:
		EmbeddingSinkStream (StyledTextIOWriter::SinkStream& realSinkStream);

		override	void	write (const void* buffer, size_t bytes);

	private:
		StyledTextIOWriter::SinkStream&	fRealSinkStream;
};









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
// class StyledTextIOReader::SrcStreamSeekSaver
	inline	StyledTextIOReader::SrcStreamSeekSaver::SrcStreamSeekSaver (SrcStream& srcStream):
				fSrcStream (srcStream),
				fSavedPos (srcStream.current_offset ())
			{
			}
	inline	StyledTextIOReader::SrcStreamSeekSaver::~SrcStreamSeekSaver ()
			{
				try {
					fSrcStream.seek_to (fSavedPos);
				}
				catch (...) {
					// ignore errors here cuz throwing out of DTORs appears to cause havoc with 
					// MWERKS runtime?? Is it a MWERKS bug? Or mine - or has this been fixed?
					// LGP 960906
				}
			}



// class StyledTextIOReader
	inline	StyledTextIOReader::StyledTextIOReader (SrcStream* srcStream, SinkStream* sinkStream, const Led_RefCntPtr<BadInputHandler>& badInputHander):
		fSrcStream (*srcStream),
		fSinkStream (sinkStream),
		fBadInputHandler (badInputHander)
		{
			Led_RequireNotNil (srcStream);
			if (fBadInputHandler.IsNull ()) {
				fBadInputHandler = new BadInputHandler ();
			}
		}
	inline	StyledTextIOReader::SrcStream&	StyledTextIOReader::GetSrcStream () const
		{
			return fSrcStream;
		}
	inline	StyledTextIOReader::SinkStream&	StyledTextIOReader::GetSinkStream () const
		{
			Led_EnsureNotNil (fSinkStream);
			return *fSinkStream;
		}
	/*
	@METHOD:		StyledTextIOReader::GetBadInputHandler
	@DESCRIPTION:	<p>Each reader class has associated with it an error handler - of type @'StyledTextIOReader::BadInputHandler'. This is used
				to handle syntactic or logical errors in the input. By default - this class is simple
				@'StyledTextIOReader::BadInputHandler'.</p>
					<p>See also @'StyledTextIOReader::SetBadInputHandler' and @'StyledTextIOReader::HandleBadlyFormattedInput'.</p>
	*/
	inline	Led_RefCntPtr<StyledTextIOReader::BadInputHandler>	StyledTextIOReader::GetBadInputHandler () const
		{
			Led_Ensure (not fBadInputHandler.IsNull ());
			return fBadInputHandler;
		}
	/*
	@METHOD:		StyledTextIOReader::SetBadInputHandler
	@DESCRIPTION:	<p>See @'StyledTextIOReader::GetBadInputHandler'</p>
	*/
	inline	void		StyledTextIOReader::SetBadInputHandler (const Led_RefCntPtr<BadInputHandler>& badInputHandler)
		{
			fBadInputHandler = badInputHandler;
			if (fBadInputHandler.IsNull ()) {
				fBadInputHandler = new BadInputHandler ();
			}
		}
	/*
	@METHOD:		StyledTextIOReader::HandleBadlyFormattedInput
	@DESCRIPTION:	<p>This routine is called whenever this is badly formatted input text to the reader.
		This is a simple wrapper on the owned @'StyledTextIOReader::BadInputHandler', which can be gotten/set with
		@'StyledTextIOReader::GetBadInputHandler' / @'StyledTextIOReader::SetBadInputHandler'</p>
	*/
	inline	void	StyledTextIOReader::HandleBadlyFormattedInput (bool unrecoverable) const
		{
			GetBadInputHandler ()->HandleBadlyFormattedInput (*this, unrecoverable);
		}
	/*
	@METHOD:		StyledTextIOReader::PutBackLastChar
	@DESCRIPTION:	<p>Unread the last read character. Note - this can be done as many times as you want (allowing infinite unread)
				but it is a bug/error if you ever unread characters that handn't been read in the first place</p>
	*/
	inline	void	StyledTextIOReader::PutBackLastChar () const
		{
			Led_Require (fSrcStream.current_offset () > 0);
			fSrcStream.seek_to (fSrcStream.current_offset ()-1);
		}
	inline	char	StyledTextIOReader::GetNextChar () const
		{
			//char	c	=	'\0';
			char	c;		// Better to leave uninitialized for performance reasons - LGP 2003-03-17
			if (fSrcStream.read1 (&c) == 1) {
				return c;
			}
			else {
				throw ReadEOFException ();
				#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
					#pragma push
					#pragma warn -8066
					#pragma warn -8008
				#endif
				Led_Assert (false); return 0; // NOT REACHED
				#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
					#pragma pop
				#endif
			}
		}
	inline	char	StyledTextIOReader::PeekNextChar () const
		{
			//char	c	=	'\0';
			char	c;		// Better to leave uninitialized for performance reasons - LGP 2003-03-17
			if (fSrcStream.read1 (&c) == 1) {
				PutBackLastChar ();
				return c;
			}
			else {
				throw ReadEOFException ();
				#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
					#pragma push
					#pragma warn -8066
					#pragma warn -8008
				#endif
				Led_Assert (false); return 0; // NOT REACHED
				#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
					#pragma pop
				#endif
			}
		}
	inline	void	StyledTextIOReader::ConsumeNextChar () const
		{
			(void)GetNextChar ();
		}



// class StyledTextIOWriter
	inline	StyledTextIOWriter::StyledTextIOWriter (SrcStream* srcStream, SinkStream* sinkStream):
		fSrcStream (srcStream),
		fSinkStream (sinkStream)
		{
			Led_RequireNotNil (srcStream);
			Led_RequireNotNil (sinkStream);
		}
	inline	StyledTextIOWriter::SrcStream&	StyledTextIOWriter::GetSrcStream () const
		{
			Led_EnsureNotNil (fSrcStream);
			return *fSrcStream;
		}
	inline	StyledTextIOWriter::SinkStream&	StyledTextIOWriter::GetSinkStream () const
		{
			Led_EnsureNotNil (fSinkStream);
			return *fSinkStream;
		}


// class StyledTextIOReader::SinkStream
	/*
	@METHOD:		StyledTextIOReader::SinkStream::GetCountOfTCharsInserted
	@DESCRIPTION:
	*/
	inline	size_t	StyledTextIOReader::SinkStream::GetCountOfTCharsInserted () const
		{
			return current_offset ();
		}


// class StyledTextIOWriter::SrcStream::Table::CellInfo
	inline	StyledTextIOWriter::SrcStream::Table::CellInfo::CellInfo ():
		f_cellx (Led_TWIPS (0)),
		f_clcbpat (Led_Color::kWhite)
	{
	}



// class StyledTextIOSrcStream_FileDescriptor
	inline	size_t	StyledTextIOSrcStream_FileDescriptor::GetBufferSize () const
		{
			return fInputBufferSize;
		}


// class StyledTextIOWriterSinkStream_Memory
	inline	const void*	StyledTextIOWriterSinkStream_Memory::PeekAtData () const
		{
			return fData;
		}
	inline	size_t	StyledTextIOWriterSinkStream_Memory::GetLength () const
		{
			return fBytesUsed;
		}


// class StyledTextIOWriterSinkStream_FileDescriptor
	inline	size_t	StyledTextIOWriterSinkStream_FileDescriptor::GetBufferSize () const
		{
			return fOutputBufferSize;
		}


#if		qLedUsesNamespaces
}
#endif



#endif	/*__StyledTextIO_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
