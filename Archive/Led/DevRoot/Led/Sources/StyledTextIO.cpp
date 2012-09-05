/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/StyledTextIO.cpp,v 2.111 2003/05/01 01:18:22 lewis Exp $
 *
 * Changes:
 *	$Log: StyledTextIO.cpp,v $
 *	Revision 2.111  2003/05/01 01:18:22  lewis
 *	SPR#1396: Added RTF reading support for table cell spacing (trspd{t,l,b,r}). Not perfect (see SPR#1454).
 *	
 *	Revision 2.110  2003/04/30 22:01:32  lewis
 *	SPR#1396: Added RTF READING support for trpadd{b,l.,t.r} and \trgaph tags which specify table margins. Fully supported in WP class SinkStream as well. Now reading these margins in from existing files looks fine. Had to add PRELIMINARY \trleft support to reader to make this work fully. Added new SPR#1453 to take care of \trleft spinnoff issue.
 *	
 *	Revision 2.109  2003/03/31 20:12:13  lewis
 *	SPR#1389- lose SplitMergeTableCell and change GetDimensions to GetRows/GetColumns(row). Point is to allow different # of columns per row in a given table
 *	
 *	Revision 2.108  2003/03/20 20:05:36  lewis
 *	SPR#1361- fix (hopefully) BufferedIndirectSrcStream::read () to return the full amount requested if possible
 *	
 *	Revision 2.107  2003/03/19 01:58:41  lewis
 *	SPR#1352 - lots of speed tweeks including adding read1() method, tweeking existing
 *	code for membuffer::read code and added BufferedIndirectSrcStream and used it from
 *	the READER code (still must fix this to be optional - SPR#1353
 *	
 *	Revision 2.106  2003/03/11 19:35:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.105  2003/02/28 19:10:14  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.104  2003/02/27 14:18:17  lewis
 *	SPR#1311- Lots more cleanups to table handing to support horizontally merged cells. Reading
 *	and writing of them now mostly works
 *	
 *	Revision 2.103  2003/02/26 20:46:54  lewis
 *	SPR#1311- progress supporting reading RTF from tables for mergecells. Works for some easy cases
 *	
 *	Revision 2.102  2003/02/26 15:46:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.101  2003/02/26 15:01:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.100  2003/01/11 19:28:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.99  2002/11/20 19:19:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.98  2002/09/23 22:13:36  lewis
 *	SPR#1111 - Fixed bug where we didn't process cellx specs for first row, and added clcbpat (cell background
 *	color) support. VERY primitive cruddy implementation. Cleanup soon.
 *	
 *	Revision 2.97  2002/09/19 14:14:41  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.96  2002/09/18 04:02:53  lewis
 *	more progress on tables - now support cellx to specify cell widths, and more related stuff -
 *	at least PARTLY supported
 *	
 *	Revision 2.95  2002/09/17 16:20:56  lewis
 *	SPR#1095 (table support) - got BASIC RTF reading working (right # cells and rows and contents
 *	read in; but no attributes like color, size etc for cells
 *	
 *	Revision 2.94  2002/05/06 21:33:52  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.93  2001/11/27 00:29:57  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.92  2001/10/17 20:43:00  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.91  2001/10/09 21:03:50  lewis
 *	SPR#1058- qSupportLed23CompatAPI support
 *	
 *	Revision 2.90  2001/09/26 15:41:21  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.89  2001/09/19 16:26:39  lewis
 *	modify StyledTextIOWriterSinkStream_Memory::write () to grow size of buffer
 *	non-linearly (avoid quadratic copies) and subtract guessed mem block overhead
 *	to get best performance from underlying memory allocator
 *	
 *	Revision 2.88  2001/08/29 23:36:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.87  2001/08/28 18:43:40  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.86  2001/07/13 18:57:03  lewis
 *	SPR#0906- got basic (rtf only for now) bullet list support working
 *	
 *	Revision 2.85  2001/07/12 23:06:09  lewis
 *	SPR#0906- support reading indent levels in RTF
 *	
 *	Revision 2.84  2001/07/12 22:42:33  lewis
 *	SPR#0906- Mostly implement RTF reading for bullet-lists.
 *	Noticed (but not yet fixed) SPR#0952.
 *	
 *	Revision 2.83  2000/09/20 08:54:31  lewis
 *	SPR#0837- Add new StyledTextIOReader::BadInputHandler class to replace
 *	StyledTextIOReader::HandleBadlyFormattedInput - and related changes
 *	
 *	Revision 2.82  2000/08/13 04:15:06  lewis
 *	SPR#0784- rewrite hidden text code based on setting segwidth to zero, and patching
 *	computeNExtCursorPosition. Not 100% done - but mostly. VERY big change -  and many
 *	bug fixes related to this hidden text stuff folded in as well - but not complete
 *	
 *	Revision 2.81  2000/06/12 22:49:02  lewis
 *	SPR#0767- more work converting stuff (\fi\ri\li\margX etc) to TWIPS - instead of pixels.
 *	Tested a snapshot at this point - and LedItMFC seemed to work OK (thats all I've tested so far)
 *	
 *	Revision 2.80  2000/06/12 20:11:40  lewis
 *	SPR#0760- convert code to do SpaceBefore/AfterLineSpacing to using new Led_TWIPS and
 *	Led_LineSpacing structs - rather than using PIXELS
 *	
 *	Revision 2.79  2000/06/12 16:25:14  lewis
 *	convert SpaceBefore to be saved in Led_TWIPS - instead of pixels. Soon todo much
 *	more like this- SPR#0767
 *	
 *	Revision 2.78  2000/05/30 23:34:58  lewis
 *	moved more default (empty) implemtantions into StyledTextIOReader::SinkStream and
 *	StyledTextIOWriter::SrcStream so various subclasses are simpler - and so as we add
 *	methods to these bases - we only need todo overrides where we actually know about
 *	the extra data (simple code cleanup)
 *	
 *	Revision 2.77  2000/05/30 22:53:02  lewis
 *	SPR#0760- added preliminary support (all but RTF) for SpaceAfter/Before/BetweenLines support
 *	
 *	Revision 2.76  2000/05/30 22:04:29  lewis
 *	SPR#0760- Added preliminary support for word-processor line spacing. RTF reader support,
 *	and internal display support done. Still todo - RTF Writer support, and GUI for
 *	setting common spacing values
 *	
 *	Revision 2.75  2000/04/16 14:39:37  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.74  2000/04/14 22:40:26  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.73  1999/12/18 03:56:47  lewis
 *	0662- break up StyledTextIO module into many _parts
 *	
 *	Revision 2.72  1999/12/15 01:04:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.71  1999/12/14 18:11:19  lewis
 *	Preliminary qXWindows support
 *	
 *	Revision 2.70  1999/12/11 02:50:59  lewis
 *	add and use new Led_NEltsOf macro - to count #elts in a C++ array - was common
 *	bug using sizeof inappropriately with Led_tChars! (for UNICODE)
 *	
 *	Revision 2.69  1999/12/09 17:25:54  lewis
 *	fix check for valid RTF format to NOT use Led_tChar in this case.
 *	
 *	Revision 2.68  1999/12/09 03:27:15  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new
 *	Led_SDK_String/Led_SDK_Char). Still MUCH more todo to get this really right.
 *	Just a patchup job.
 *	
 *	Revision 2.67  1999/11/15 21:32:01  lewis
 *	Led_MFC instead of CWindowDC - etc - lose MFC dependencies
 *	
 *	Revision 2.66  1999/11/13 16:32:23  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.65  1999/08/28 03:40:30  lewis
 *	partial, half-assed fix for spr#0622- prelim handling of color attribute
 *	in a <FONT> tag in HTML reader
 *	
 *	Revision 2.64  1999/08/28 03:03:52  lewis
 *	delete obsolete code
 *	
 *	Revision 2.63  1999/08/28 02:37:40  lewis
 *	throw away some obsolete ifdefed out code
 *	
 *	Revision 2.62  1999/08/28 02:24:01  lewis
 *	spr#0621- change HTML reader space/newline handling problem. Basicly - instead
 *	of hack to guess context - KEEP context in intstance variable
 *	
 *	Revision 2.61  1999/07/13 23:01:15  lewis
 *	add Led_Arg_Unused () to avoid some warnings
 *	
 *	Revision 2.60  1999/06/16 18:22:10  lewis
 *	typo
 *	
 *	Revision 2.59  1999/06/16 16:24:33  lewis
 *	spr#0592- treat HTML <dir> as <ul>
 *	
 *	Revision 2.58  1999/06/16 16:09:57  lewis
 *	spr#0591- handle &#9; as a tab character in HTML reader
 *	
 *	Revision 2.57  1999/06/16 15:23:40  lewis
 *	spr#0590- dont create StandardURLStyleMarker directly - but vector lookup through
 *	the EmbeddedObjectCreatorRegistry in case user subclasses
 *	
 *	Revision 2.56  1999/06/16 13:45:35  lewis
 *	spr#0588- add StyledTextIOReader/Writer_HTML::GetEntityRefMapTable () virtuals,
 *	and use them. And fix nbsp to only be used on READING - not on writing.
 *	
 *	Revision 2.55  1999/06/15 22:26:05  lewis
 *	spr#0586 - extra spaces output in HTML reader for NL characters - still kludgely
 *	but now works a little better
 *	
 *	Revision 2.54  1999/05/03 22:05:16  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.53  1999/05/03 21:41:32  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.52  1999/04/28 20:10:19  lewis
 *	wrap all Strikeout style code with #if qWindows
 *	
 *	Revision 2.51  1999/04/28 16:18:49  lewis
 *	Add html support for strikeout and underline (was some)
 *	
 *	Revision 2.50  1999/04/28 15:40:18  lewis
 *	add RTF support for Strikeout==\strike. And delete some obsolete(commented out) code
 *	
 *	Revision 2.49  1999/03/25 23:23:19  lewis
 *	cleanup warnings for unused variables
 *	
 *	Revision 2.48  1999/03/09 16:58:34  lewis
 *	hook in and actually us the paperw/paperh/margl etc values from the RTFInfo object
 *	into what we use in reading in the document (in the sinkstream)
 *	
 *	Revision 2.47  1999/03/09 16:29:40  lewis
 *	move StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination and ...Desintation_
 *	classes out of class declaration (still in scope). Use auto_ptr to keep track of
 *	fDefaultDestination (so can be moved). Make Flush(of destination) virtual.
 *	ALWAYS keep around an RTFInfo in the reader (though its still an optional arg).
 *	Pass it to the SinkStreamDestination, and lose ITS copy of the fDefaultTabStops.
 *	Instead - use the copy in the RTFInfo object it now owns. Positioing todo same
 *	soon for margins/paperwidth
 *	
 *	Revision 2.46  1999/03/08 22:44:45  lewis
 *	Break out RTFInfo stuff which was really just a repository for type declarations,
 *	and store that into new type RTFIO
 *	
 *	Revision 2.45  1999/03/08 21:07:43  lewis
 *	preliminary support for RTF tags - margl/margt\margr\marg...etc and papwerw/paperh -
 *	just the tags and hooks supported so far
 *	
 *	Revision 2.44  1999/03/08 20:28:17  lewis
 *	hack up enuf to at least COMPILE with qWideCharacters - though not nearly enuf to work properly
 *	
 *	Revision 2.43  1999/03/02 22:39:29  lewis
 *	fix spr#0554- had h/v reversed reading in OLE objects and scaling, and had NO code
 *	to write out sizes. Still not working great (see spr#0555), but much better
 *	(added Led_MFC_ControlItem::GetSize() to help as well)
 *	
 *	Revision 2.42  1999/03/02 04:28:24  lewis
 *	beginnings of fix to SinkStreamDestination::SetRightMargin () to use \paperw and
 *	\margl etc. Still only VERY VERY incomplete support for this
 *	
 *	Revision 2.41  1999/02/21 21:52:45  lewis
 *	fix mac-specific compile error
 *	
 *	Revision 2.40  1999/02/21 20:03:16  lewis
 *	added StyledTextIOWriter::SrcStream::GetFirstIndent/StyledTextIOWriter::SrcStream::GetMargins,
 *	and call them to write out margins/first indent in RTF output
 *	
 *	Revision 2.39  1999/02/21 13:56:24  lewis
 *	Added SetFirstIndent/SetLeftMargin/SetRightMargin/RTF\li\ri\fi, and amended\pard support
 *	for these RTF tags (READING ONLY). Works (in preliminary testing), but with a few small
 *	sprs noted in dbase (validation and fixing defaults)
 *	
 *	Revision 2.38  1999/02/20 23:47:07  lewis
 *	added support for '\fi' rtf tag (though no deep / full support yet
 *	
 *	Revision 2.37  1998/10/30 14:37:52  lewis
 *	Lots of misc changes.
 *	MSVC60 compiler warnings support changed.
 *	Use vector<> instead of Led_Array<>
 *	Compiler BWA changes (one new one for msvc60).
 *	More?
 *	
 *	Revision 2.36  1998/05/05  00:30:50  lewis
 *	Support MWERKS CWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.35  1998/04/25  01:34:08  lewis
 *	RTF Tabstop support.
 *
 *	Revision 2.34  1998/04/08  02:07:09  lewis
 *	Moved StyledTextIOSinkStream_StandardStyledTextImager and StyledTextIOWriterSrcStream_StandardStyledTextImager
 *	to WordProcessor/StandardStyledTextInteractor modules (not as is, but broken up appropriately).
 *
 *	Revision 2.33  1998/03/04  20:21:37  lewis
 *	Fix spr#506.
 *
 *	Revision 2.32  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.31  1997/12/24  03:35:54  lewis
 *	Massive changes. Partly for compat with new mac compiler (and new universal headers).
 *	But also added support for RTF reading/writing embeddings - including both Led private ones, and real OLE ones.
 *
 *	Revision 2.30  1997/10/01  02:00:43  lewis
 *	fCachedFontSize optiziation to RTF reader (cuz on Windoze - SetPointSize() is expensive call).
 *	Fixed a couple lingering one/zero bugs with Reader/Sink/etc stuff.
 *	Added two caches to StyledTextIOSinkStream_StandardStyledTextImager - one for inserted text, and one for
 *	style runs. Made a huge performance difference reading RTF files - roughlt 4x (see spr#490).
 *
 *	Revision 2.29  1997/09/29  15:18:22  lewis
 *	Lose qLedFirstIndex support.
 *	Added support for justification (to src/sink streams and rtf reader/writer).
 *	Use new ::AddEmbedding/::Insert... code to share embedding setup code.
 *	Fix zero-one-based bug in old Led-foramt reading/writing code.
 *
 *	Revision 2.28  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.27  1997/07/14  00:43:09  lewis
 *	Renamed TextImager_ to TextImager, SinkStream_ to SinkStream, etc.
 *	Also one minor change with GetLastItem () reacting to Led_Array API change.
 *
 *	Revision 2.26  1997/06/28  17:15:50  lewis
 *	Added hooks for RTF tags \li\qc\qr\ri\tx
 *
 *	Revision 2.25  1997/06/24  03:32:48  lewis
 *	Lose old test code.
 *
 *	Revision 2.24  1997/06/23  16:16:09  lewis
 *	Make EnterControlWord scoped in RTFInfo, and make ControlWordNameMap a vector<T> instead
 *	of map<T> as a speed tweek (profiled on Windows).
 *
 *	Revision 2.23  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.22  1997/06/18  03:07:17  lewis
 *	Lots of cleanups (lose old bug defines).
 *	Version6 LedFileFromat support (colored text, and sub/super script).
 *	Cleanups using new CollectAllMarkers... MarkerSink stuff.
 *	Lose StyledTextIOReader_CheckFileAndQueryUserDesiredFormat.
 *	Use new _OrSurroundings code cleanups.
 *	RTF (color table, colored text, atomized control words, sub/super scripts).
 *
 *	Revision 2.21  1997/03/22  15:31:14  lewis
 *	port to msvc50
 *
 *	Revision 2.20  1997/03/04  20:10:23  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1997/01/10  03:15:53  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.17  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.16  1996/10/31  00:13:56  lewis
 *	Use Led_Arg_Unused() to cleanup warnings.
 *	Add ExtractHTMLTagIntoTagNameBuf () helper.
 *	Support reading rtf versions other than rtf1 (treat same, but still accept as rtf).
 *	Fix reading html title. Other minor changes.
 *
 *	Revision 2.15  1996/10/15  19:03:36  lewis
 *	work around qCPlusPlusRuntimeLibraryLacksStringEraseMethod.
 *
 *	Revision 2.14  1996/10/15  18:35:58  lewis
 *	Mostly full/new implemenation of StyledTextIOWriter_HTML - now really writes (halfway) decent html.
 *	Some modest changes to HTML reader (mainly qThrowAwayMostUnknownHTMLTags).
 *	Moved CasedCharsEqual etc to LedSupport.
 *	Support entityrefs in html readers/writers.
 *	A bit more...
 *
 *	Revision 2.13  1996/10/04  17:26:07  lewis
 *	work around a couple msvc 4.1 problems
 *
 *	Revision 2.12  1996/10/04  16:38:47  lewis
 *	Lose old code for writing version-4 of Led-private-format (old format).
 *	Massvie re-org of rtf reader - breaking out each tag handler into sepearte routine.
 *	Makes subclasses (like RTF_Preserving) easier. A few bugs fixed with RTF.
 *	Lots of new code for HTML reader. Now works decently, though not well enuf
 *	for final release. No work done yet on writer.
 *
 *	Revision 2.11  1996/09/30  14:28:19  lewis
 *	Lots of changes to RTF code. Mostly support for improved characterset handling. But also cleanups
 *	to share more code (with HTML etc).
 *	Also added an HTML reader/writer (very preliminary).
 *
 *	Revision 2.10  1996/09/03  20:45:38  lewis
 *	wrap mac calls in FileIO stuff with #ifqMacOS
 *
 *	Revision 2.9  1996/09/03  15:06:31  lewis
 *	Added RTF support.
 *	(lots of code).
 *
 *	Revision 2.8  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.7  1996/07/19  16:38:47  lewis
 *	Code cleanups, and reacted to renamings from other modules.
 *	Fixed WinNT 3.51 appleshare server problem - SetFPos(BIGNUM) doesn't
 *	work right - doesn't clip to EOF. So we must do this MANUALLY.
 *
 *	Revision 2.6  1996/06/01  02:19:13  lewis
 *	Lots.
 *	Mainly broke led private format IO code into Version_4 and Version_5 parts.
 *	Actually first implemetned Version_5 code. Much more compact now.
 *	Can still read/write both formats.
 *	Lose XXX:BadFormat excpetion - use Led_ThrowBadFormat ... instead, so
 *	caught by various class libs properly.
 *
 *	Revision 2.5  1996/05/23  20:03:06  lewis
 *	FontSpecification --> Led_FontSpecification (not just a name change - now user wrpapers
 *	instead of setting fields. Now much more code shared between mac and PC.
 *	Portable format style IO now shares much more code between mac and PC.
 *
 *	Revision 2.4  1996/05/04  21:09:14  lewis
 *	Moved ULONGToBuf() to Led_Support and renamed Led_ULONGToBuf().
 *	Changed StyledTextIOWriterSinkStream_FileDescriptor::UpdateEOF handling
 *	so no throw from DTOR.
 *	Fixed PointSize handling in writing PC font size info to led-rich-text
 *	streams (write POINTS not tmHeight).
 *	(note that change wasn't COMPLETELY compatable with old files written
 *	but the old files weren't going to get handled right anyhow. And this new
 *	approach is close to 'right' for dealing with the old files (any onyl any
 *	prob at all for pc).
 *
 *	Revision 2.3  1996/04/18  15:30:33  lewis
 *	upped buffer sizes, plugged memory leak in ~StyledTextIOWriterSinkStream_Memory ()
 *	and several other cleanups.
 *
 *	Revision 2.2  1996/03/04  07:50:54  lewis
 *	Moved EmbeddingSinkStream implementation here, and advertised class in headers now.
 *	Cleanup InternalizeEmbedding/ExtenralizeEmbeddings code, though we keep same
 *	format (except for picts, and we did backward compat support for those).
 *	Create default embedding object, etc. Plus use registry dbase for reading
 *	in objects (our registry of supported types refering to here).
 *
 *	Revision 2.1  1996/02/26  18:56:49  lewis
 *	Upped some buffer sizes in various readers/writers.
 *	(for improved speeds with file io).
 *	Added new steam readers/writers for _FileDescriptor.
 *	Use Led_Min/Max instead of plain versions.
 *
 *	Revision 2.0  1996/01/04  00:05:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1996/01/04  00:03:25  lewis
 *	Massive changes. Added support for File WRITING. And added new
 *	LedPrivate format - which supports emebedding. Now use that for private
 *	embedding OpenDoc file format. ANd for OLE2 and use in TCL Mac App as well,
 *	for style runs (as alternate format - read but not written there yet).
 *
 *	Revision 1.1  1995/11/04  23:17:36  lewis
 *	Initial revision
 *
 *	Revision 2.1  1995/11/02  22:34:11  lewis
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<cctype>
#include	<climits>
#include	<cstdio>		// for a couple sprintf() calls - could pretty easily be avoided

#if		qMacOS
	#include	<Errors.h>
	#include	<Files.h>
	#include	<TextEdit.h>		// for Apple TE scrap format and TEContinuous etc compatability
#endif

#include	"StyledTextImager.h"
#include	"StyledTextEmbeddedObjects.h"

#include	"StyledTextIO.h"



// Include moved down here cuz of GCC lib bug...
#if		qBitSetTemplateAvailable
	#include	<bitset>
#endif



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
	namespace	Led {
#endif






#ifndef	qUseCompiledSetHack
	#if		qBitSetTemplateAvailable
		// Preliminary testing on Mac indicates this compiled set hack doesn't help. Test on PC, as well...
		// LGP 970326
		#define	qUseCompiledSetHack	1
	#else
		#define	qUseCompiledSetHack	0
	#endif
#endif





#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4800)		//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
	#pragma	warning (4 : 4786)		//qQuiteAnnoyingDebugSymbolTruncationWarnings
#endif






/*
 ********************************************************************************
 *************************** StyledTextIOReader::SinkStream *********************
 ********************************************************************************
 */

/*
@METHOD:		StyledTextIOReader::SinkStream::StartTable
@DESCRIPTION:	<p>Default is to do ignore. Override to implement tables.</p>
*/
void	StyledTextIOReader::SinkStream::StartTable ()
{
}

/*
@METHOD:		StyledTextIOReader::SinkStream::EndTable
@DESCRIPTION:	<p>Default is to do ignore. Override to implement tables.</p>
*/
void	StyledTextIOReader::SinkStream::EndTable ()
{
}

/*
@METHOD:		StyledTextIOReader::SinkStream::StartTableRow
@DESCRIPTION:	<p>Default is to do ignore. Override to implement tables.</p>
*/
void	StyledTextIOReader::SinkStream::StartTableRow ()
{
}

/*
@METHOD:		StyledTextIOReader::SinkStream::EndTableRow
@DESCRIPTION:	<p>Default is to do ignore. Override to implement tables.</p>
*/
void	StyledTextIOReader::SinkStream::EndTableRow ()
{
}

/*
@METHOD:		StyledTextIOReader::SinkStream::StartTableCell
@DESCRIPTION:	<p>Default is to do ignore. Override to implement tables.</p>
				<p>Note about how this API handles <em>merged cells</em> (merge cells).
			If two or more cells in a row are to be merged together - then the FileIO code that reads
			them will generate a SINGLE StartTableCell/EndTableCell pair, passing the number of horizontally merged cells
			as an argument to StartTableCell ('colSpan'). The total number of StartTableCell/EndTableCell
			calls should correspond to the length of the 
			@'StyledTextIOReader::SinkStream::SetCellWidths' argument.
			</p>
*/
void	StyledTextIOReader::SinkStream::StartTableCell (size_t /*colSpan*/)
{
}

/*
@METHOD:		StyledTextIOReader::SinkStream::EndTableCell
@DESCRIPTION:	<p>Default is to do ignore. Override to implement tables.</p>
*/
void	StyledTextIOReader::SinkStream::EndTableCell ()
{
}

void	StyledTextIOReader::SinkStream::SetJustification (Led_Justification /*justification*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

void	StyledTextIOReader::SinkStream::SetStandardTabStopList (const TextImager::StandardTabStopList& /*tabStops*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

void	StyledTextIOReader::SinkStream::SetFirstIndent (Led_TWIPS /*tx*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

void	StyledTextIOReader::SinkStream::SetLeftMargin (Led_TWIPS /*lhs*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

void	StyledTextIOReader::SinkStream::SetRightMargin (Led_TWIPS /*rhs*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetSpaceBefore
@DESCRIPTION:
*/
void	StyledTextIOReader::SinkStream::SetSpaceBefore (Led_TWIPS /*sb*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetSpaceAfter
@DESCRIPTION:
*/
void	StyledTextIOReader::SinkStream::SetSpaceAfter (Led_TWIPS /*sa*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetLineSpacing
@DESCRIPTION:
*/
void	StyledTextIOReader::SinkStream::SetLineSpacing (Led_LineSpacing /*sl*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetTextHidden
@DESCRIPTION:
*/
void	StyledTextIOReader::SinkStream::SetTextHidden (bool /*hidden*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetListStyle
@DESCRIPTION:
*/
void	StyledTextIOReader::SinkStream::SetListStyle (ListStyle /*listStyle*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetListIndentLevel
@DESCRIPTION:
*/
void	StyledTextIOReader::SinkStream::SetListIndentLevel (unsigned char /*indentLevel*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetTableBorderColor
@DESCRIPTION:
*/
void	StyledTextIOReader::SinkStream::SetTableBorderColor (Led_Color /*c*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetTableBorderWidth
@DESCRIPTION:
*/
void	StyledTextIOReader::SinkStream::SetTableBorderWidth (Led_TWIPS /*bWidth*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetCellWidths
@DESCRIPTION:	<p>Default is to do ignore. Override to implement tables.</p>
				<p>Note that this must be called after @'StyledTextIOReader::SinkStream::StartTableRow' and before
			and calls to @'StyledTextIOReader::SinkStream::StartTableCell'. The number and widths here must correspond to the
			number of @'StyledTextIOReader::SinkStream::StartTableCell'/@'StyledTextIOReader::SinkStream::EndTableCell' pairs
			of calls for the row. The number of actual
			cells in the table created could be larger (due to implied cell merges from the
			@'StyledTextIOReader::SinkStream::StartTableCell' call).
			</p>
*/
void	StyledTextIOReader::SinkStream::SetCellWidths (const vector<Led_TWIPS>& /*cellWidths*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetCellBackColor
@DESCRIPTION:	<p>Default is to do ignore. Override to implement tables.</p>
*/
void	StyledTextIOReader::SinkStream::SetCellBackColor (const Led_Color /*c*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetDefaultCellMarginsForCurrentRow
@DESCRIPTION:	<p>Default is to do ignore. Override to implement tables.</p>
*/
void	StyledTextIOReader::SinkStream::SetDefaultCellMarginsForCurrentRow (Led_TWIPS /*top*/, Led_TWIPS /*left*/, Led_TWIPS /*bottom*/, Led_TWIPS /*right*/)
{
	// override, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:		StyledTextIOReader::SinkStream::SetDefaultCellSpacingForCurrentRow
@DESCRIPTION:	<p>Default is to do ignore. Override to implement tables.</p>
*/
void	StyledTextIOReader::SinkStream::SetDefaultCellSpacingForCurrentRow (Led_TWIPS /*top*/, Led_TWIPS /*left*/, Led_TWIPS /*bottom*/, Led_TWIPS /*right*/)
{
	// override, and ignore, since that feature isn't supported by this class
}







/*
 ********************************************************************************
 ********************* StyledTextIOReader::BadInputHandler **********************
 ********************************************************************************
 */

/*
@METHOD:		StyledTextIOReader::BadInputHandler::HandleBadlyFormattedInput
@DESCRIPTION:	<p>This routine is called whenever this is badly formatted input text to the reader.
	By default - this routine does nothing (unless the unrecoverable argument is 'true').</p>
		<p>This is a change of behavior from Led 2.3. In Led 2.3 - format errors would always throw
	by calling @'Led_ThrowBadFormatDataException'. Instead - this method now calls this virtual method of @'StyledTextIOReader::BadInputHandler' - which your
	subclass can override to throw @'Led_ThrowBadFormatDataException' - if thats the behavior you want. You can replace the error handler associated with
	a particular @'StyledTextIOReader' by calling @'StyledTextIOReader::SetBadInputHandler'.</p>
*/
void	StyledTextIOReader::BadInputHandler::HandleBadlyFormattedInput (const StyledTextIOReader& /*reader*/, bool unrecoverable)
{
	if (unrecoverable) {
		Led_ThrowBadFormatDataException ();
	}
}






/*
 ********************************************************************************
 *************************** StyledTextIOWriter::SrcStream **********************
 ********************************************************************************
 */

/*
@METHOD:		StyledTextIOWriter::SrcStream::GetJustification
@DESCRIPTION:
*/
Led_Justification	StyledTextIOWriter::SrcStream::GetJustification ()	const
{
	return eLeftJustify;
}

/*
@METHOD:		StyledTextIOWriter::SrcStream::GetStandardTabStopList
@DESCRIPTION:
*/
TextImager::StandardTabStopList	StyledTextIOWriter::SrcStream::GetStandardTabStopList ()	const
{
	return StandardTabStopList ();
}

/*
@METHOD:		StyledTextIOWriter::SrcStream::GetFirstIndent
@DESCRIPTION:
*/
Led_TWIPS	StyledTextIOWriter::SrcStream::GetFirstIndent () const
{
	return Led_TWIPS (0);
}

/*
@METHOD:		StyledTextIOWriter::SrcStream::GetMargins
@DESCRIPTION:
*/
void	StyledTextIOWriter::SrcStream::GetMargins (Led_TWIPS* lhs, Led_TWIPS* rhs) const
{
	Led_RequireNotNil (lhs);
	Led_RequireNotNil (rhs);
	*lhs = Led_TWIPS (0);
	*rhs = Led_TWIPS (6 * 1440);	// Not sure what I should return here??? maybe special case and not write anything to output file if returns zero? LGP-990221
}

/*
@METHOD:		StyledTextIOWriter::SrcStream::GetSpaceBefore
@DESCRIPTION:
*/
Led_TWIPS	StyledTextIOWriter::SrcStream::GetSpaceBefore () const
{
	return Led_TWIPS (0);
}

/*
@METHOD:		StyledTextIOWriter::SrcStream::GetSpaceAfter
@DESCRIPTION:
*/
Led_TWIPS		StyledTextIOWriter::SrcStream::GetSpaceAfter () const
{
	return Led_TWIPS (0);
}

/*
@METHOD:		StyledTextIOWriter::SrcStream::GetLineSpacing
@DESCRIPTION:
*/
Led_LineSpacing		StyledTextIOWriter::SrcStream::GetLineSpacing () const	
{
	return Led_LineSpacing ();
}

/*
@METHOD:		StyledTextIOWriter::SrcStream::GetListStyleInfo
@DESCRIPTION:
*/
void		StyledTextIOWriter::SrcStream::GetListStyleInfo (ListStyle* listStyle, unsigned char* indentLevel) const
{
	Led_RequireNotNil (listStyle);
	Led_RequireNotNil (indentLevel);
	*listStyle = eListStyle_None;
	*indentLevel = 0;
}

/*
@METHOD:		StyledTextIOWriter::SrcStream::GetSoftLineBreakCharacter
@DESCRIPTION:	<p>Returns the special character which should be treated (in RTF output) as the SOFT-RETURN line
	break character (in RTF \line).</p>
*/
Led_tChar	StyledTextIOWriter::SrcStream::GetSoftLineBreakCharacter () const
{
	return 0;	// bogus value - this class doesn't know about this stuff... LGP 991227
}

/*
@METHOD:		StyledTextIOWriter::SrcStream::GetHidableTextRuns
@DESCRIPTION:
*/
DiscontiguousRun<bool>	StyledTextIOWriter::SrcStream::GetHidableTextRuns () const
{
	return DiscontiguousRun<bool> ();
}









/*
 ********************************************************************************
 *************************** StyledTextIOSrcStream_Memory ***********************
 ********************************************************************************
 */
StyledTextIOSrcStream_Memory::StyledTextIOSrcStream_Memory (
				const void* data, size_t nBytes
				#if		qMacOS
				, Handle resourceHandle
				#endif
			):
	StyledTextIOReader::SrcStream (),
	fData (data),
	fDataEnd (((char*)data) + nBytes),
	fBytesInBuffer (nBytes),
	fCurPtr (data)//,
//	fBytesLeft (nBytes)
	#if		qMacOS
		,fResourceHandle (resourceHandle)
	#endif
{
}

size_t	StyledTextIOSrcStream_Memory::current_offset () const
{
	return (((char*)fCurPtr-(char*)fData));
}

void	StyledTextIOSrcStream_Memory::seek_to (size_t to)
{
	Led_Require (to >= 0);
	to = min (to, fBytesInBuffer);
//	fBytesLeft = fBytesInBuffer - to;
	fCurPtr = ((char*)fData) + to;
}

size_t	StyledTextIOSrcStream_Memory::read (void* buffer, size_t bytes)
{
	Led_RequireNotNil (buffer);

#if 1
	char*	curBytePtr	=	(char*)fCurPtr;
	size_t	curIdx		=	curBytePtr - ((char*)fData);
	size_t	bytesLeft	=	fBytesInBuffer - curIdx;
	bytes = min (bytesLeft, bytes);
	::memcpy (buffer, curBytePtr, bytes);
	fCurPtr = curBytePtr + bytes;
	return bytes;
#else
	bytes = Led_Min (fBytesLeft, bytes);
	::memcpy (buffer, fCurPtr, bytes);
	fCurPtr = ((char*)fCurPtr) + bytes;
	fBytesLeft -= bytes;
	return bytes;
#endif
}

size_t	StyledTextIOSrcStream_Memory::read1 (char* c)
{
	Led_RequireNotNil (c);
	char*	curBytePtr	=	(char*)fCurPtr;
	if (curBytePtr != fDataEnd) {
		*c = *curBytePtr;
		fCurPtr = curBytePtr + 1;
		return 1;
	}
	return 0;
}

#if		qMacOS
Handle	StyledTextIOSrcStream_Memory::GetAUXResourceHandle () const
{
	return fResourceHandle;
}
#endif









/*
 ********************************************************************************
 ******************* StyledTextIOSrcStream_FileDescriptor ***********************
 ********************************************************************************
 */
StyledTextIOSrcStream_FileDescriptor::StyledTextIOSrcStream_FileDescriptor (
				int fd
				#if		qMacOS
				, Handle resourceHandle
				#endif
			):
	StyledTextIOReader::SrcStream (),
	fFileDescriptor (fd),
	fCurSeekPos (0),
	fInputBuffer (NULL),
	fInputBufferSize (0),
	fBufferWindowStart (0),
	fBufferWindowEnd (0)
	#if		qMacOS
		,fResourceHandle (resourceHandle)
	#endif
{
}

StyledTextIOSrcStream_FileDescriptor::~StyledTextIOSrcStream_FileDescriptor ()
{
	delete[] fInputBuffer;
}

size_t	StyledTextIOSrcStream_FileDescriptor::current_offset () const
{
	return fCurSeekPos;
}

void	StyledTextIOSrcStream_FileDescriptor::seek_to (size_t to)
{
	Led_Require (to >= 0);

	size_t	targetSeekPos	=	(to);
	if (targetSeekPos > fBufferWindowEnd) {
		// If we seek PAST that point, we must actually query the file system, so we don't seek
		// past EOF
		#if		qMacOS
			/*
			 *	We used to just call SetFPos(), and it would return posErr
			 *	if I seeked beyond EOF. The trouble is that WindowsNT 3.51's
			 *	AppleShare fileserver doesn't work that way. It lets you seek
			 *	past EOF. And then returns that larger EOF in ::GetFPos().
			 *	The breaks the idiom I use of seeking to BIGNUM and calling
			 *	current_offset() to measure how big the file was.
			 */
			long	logEOF	=	0;
			Led_ThrowOSErr (::GetEOF (fFileDescriptor, &logEOF));
			targetSeekPos = Led_Min (logEOF, targetSeekPos);
		#else
			Led_Assert (false);	// NYI
		#endif
	}
	fCurSeekPos = targetSeekPos;
}

size_t	StyledTextIOSrcStream_FileDescriptor::read (void* buffer, size_t bytes)
{
	Led_RequireNotNil (buffer);

	if (bytes == 0) {
		return 0;	// get out early in this case so I can assert bytesToCopyNow!=0 below... assert making progress
	}

	/*
	 *	Grab as much as we can from the existing buffered data.
	 */
	size_t	bytesCopiedSoFar	=	0;
NotherRead:
	if (fCurSeekPos >= fBufferWindowStart and fCurSeekPos < fBufferWindowEnd) {
		Led_Assert (bytes > bytesCopiedSoFar);
		size_t	bytesToCopyNow	=	Led_Min ((bytes-bytesCopiedSoFar), (fBufferWindowEnd - fCurSeekPos));
		Led_Assert (bytesToCopyNow > 0);
		Led_Assert (bytesToCopyNow <= bytes);
		memcpy (&((char*)buffer)[bytesCopiedSoFar], &fInputBuffer[fCurSeekPos-fBufferWindowStart], bytesToCopyNow);
		bytesCopiedSoFar += bytesToCopyNow;
		fCurSeekPos += bytesToCopyNow;
	}

	size_t	bytesLeftToCopy	=	bytes - bytesCopiedSoFar;
	if (bytesLeftToCopy > 0) {
		// If there is enuf room in buffer, read in an fill the window
		Led_Assert (bytesLeftToCopy <= bytes);
		if (bytesLeftToCopy < fInputBufferSize) {
			ReadInWindow (fCurSeekPos);
			if (fCurSeekPos < fBufferWindowEnd) {
				goto NotherRead;
			}
			else {
				// must have been EOF if we couldn't read anymore...
				return bytesCopiedSoFar;
			}
		}

		// Then we must do a DIRECT read, and don't bother with the buffer
		// (we could loop filling/emtpying buffer, but that would be needless copying).
		#if		qMacOS
			Led_ThrowOSErr (::SetFPos (fFileDescriptor, fsFromStart, fCurSeekPos));
			{
				long	count	=	bytesLeftToCopy;
				OSErr	err		=	::FSRead (fFileDescriptor, &count, &((char*)buffer)[bytesCopiedSoFar]);
				Led_ThrowOSErr ((err == eofErr)? noErr: err);
				bytesCopiedSoFar += count;
				fCurSeekPos += count;
			}
		#else
			Led_Assert (false);	// NYI
		#endif
	}

	return bytesCopiedSoFar;
}

void	StyledTextIOSrcStream_FileDescriptor::SetBufferSize (size_t bufSize)
{
	if (fInputBufferSize != bufSize) {
		delete[] fInputBuffer;
		fInputBuffer = NULL;

		fInputBuffer = new char [bufSize];
		fInputBufferSize = bufSize;
		fBufferWindowStart = 0;
		fBufferWindowEnd = 0;
	}
}

void	StyledTextIOSrcStream_FileDescriptor::ReadInWindow (size_t startAt)
{
	Led_RequireNotNil (fInputBuffer);
	Led_Require (fInputBufferSize > 0);
	#if		qMacOS
		Led_ThrowOSErr (::SetFPos (fFileDescriptor, fsFromStart, startAt));
		long	count	=	fInputBufferSize;
		OSErr	err		=	::FSRead (fFileDescriptor, &count, fInputBuffer);
		Led_ThrowOSErr ((err == eofErr)? noErr: err);

		fBufferWindowStart = startAt;
		fBufferWindowEnd = startAt + count;
	#else
		Led_Assert (false);	// NYI
	#endif
}

#if		qMacOS
Handle	StyledTextIOSrcStream_FileDescriptor::GetAUXResourceHandle () const
{
	return fResourceHandle;
}
#endif








/*
 ********************************************************************************
 ******************** StyledTextIOWriterSinkStream_Memory ***********************
 ********************************************************************************
 */
StyledTextIOWriterSinkStream_Memory::StyledTextIOWriterSinkStream_Memory ():
	StyledTextIOWriter::SinkStream (),
	fData (NULL),
	fBytesUsed (0),
	fBytesAllocated (0),
	fCurPtr (NULL)
{
}

StyledTextIOWriterSinkStream_Memory::~StyledTextIOWriterSinkStream_Memory ()
{
	delete[] fData;
}

size_t	StyledTextIOWriterSinkStream_Memory::current_offset () const
{
	return (fCurPtr-fData);
}

void	StyledTextIOWriterSinkStream_Memory::seek_to (size_t to)
{
	Led_Require (to >= 0);
	to = Led_Min (to, fBytesUsed);
	fCurPtr = fData + to;
}

void	StyledTextIOWriterSinkStream_Memory::write (const void* buffer, size_t bytes)
{
	const	size_t	kMemBlockOverhead	=	40;			// wild guess as to how much overhead this is in a memory request.
														// This is helpful so that we request the largest block size possible
														// from the underlying OS storage without any waste. If the number is wrong - 
														// things will still work - but will be less memory efficient.
	const	size_t	kChunkSize			=	16*1024;	// alloc in this size chunks - at least...

	Led_RequireNotNil (buffer);

	Led_Assert (fBytesUsed <= fBytesAllocated);

	/*
	 *	Now, re-alloc the pointer if we need even more space...
	 */
	size_t	curOffset	=	fCurPtr-fData;
	if (curOffset + bytes > fBytesAllocated) {
		/*
		 *	Avoid quadratic copying - so if size bigger than fixed amount, then increase size allocated by some
		 *	factor (so N*log N entries copied).
		 */
		size_t	newSize	=	((static_cast<size_t> ((fBytesAllocated + bytes)*1.5) + kChunkSize - 1 + kMemBlockOverhead)/kChunkSize) * kChunkSize - kMemBlockOverhead;	// round to next larger chunksize
		Led_Assert (newSize > fBytesAllocated);
		Led_Assert (newSize >= fBytesAllocated + bytes);
		char*	buf = new char [newSize];
		if (fData != NULL) {
			::memcpy (buf, fData, fBytesUsed);
		}
		delete[] fData;
		fData = buf;
		fCurPtr = buf + curOffset;
		fBytesAllocated = newSize;
	}

	/*
	 *	Then use up what space we have left.
	 */
	memcpy (fCurPtr, buffer, bytes);
	buffer = ((char*)buffer) + bytes;
	fCurPtr = ((char*)fCurPtr) + bytes;
	fBytesUsed = Led_Max (size_t (fCurPtr-fData), fBytesUsed);
}










/*
 ********************************************************************************
 ***************** StyledTextIOWriterSinkStream_FileDescriptor ******************
 ********************************************************************************
 */

/*
 *	Notes about buffering:
 *
 *		We implement a VERY SIMPLISTIC style of buffering. This is because when we seek,
 *	we would normally have to re-read from disk our window, and allow updates. Otherwise
 *	we don't know how much of our window is valid to write back to disk on a flush. To avoid
 *	this problem, we only buffer writes past our last write. That is - we don't buffer
 *	back-patch writes. This simplification still allows this buffering to be a useful speedup,
 *	and remain quite simple (and to NOT to needless reads).
 */
StyledTextIOWriterSinkStream_FileDescriptor::StyledTextIOWriterSinkStream_FileDescriptor (int fd):
	StyledTextIOWriter::SinkStream (),
	fFileDescriptor (fd),
	fOutputBuffer (NULL),
	fOutputBufferSize (0),
	fBufferWindowStart (0),
	fBufferWindowEnd (0),
	fFurthestDiskWriteAt (0),
	fCurSeekPos (0)
{
}

StyledTextIOWriterSinkStream_FileDescriptor::~StyledTextIOWriterSinkStream_FileDescriptor ()
{
	try {
		Flush ();
		UpdateEOF ();
	}
	catch (...) {
		// errors ignored in DTOR cuz they cause 'unexpceted' to be called - at least for mwerks
		// CW8 - LGP 960423
	}
}

size_t	StyledTextIOWriterSinkStream_FileDescriptor::current_offset () const
{
	return fCurSeekPos;
}

void	StyledTextIOWriterSinkStream_FileDescriptor::seek_to (size_t to)
{
	Led_Require (to >= 0);
	fCurSeekPos = to;
}

void	StyledTextIOWriterSinkStream_FileDescriptor::write (const void* buffer, size_t bytes)
{
	if (bytes > fOutputBufferSize or fCurSeekPos < fFurthestDiskWriteAt) {
		/*
		 *	No point in doing partial buffering. For large writes, just let 'em pass
		 *	through. Also, as we describe above, we don't even try to allow buffering of
		 *	"backpatch" writes.
		 */
		if (fOutputBufferSize != 0) {
			Flush ();
		}
		#if		qMacOS
			Led_ThrowOSErr ( ::SetFPos (fFileDescriptor, fsFromStart, fCurSeekPos));
			long	count	=	bytes;
			Led_ThrowOSErr (::FSWrite (fFileDescriptor, &count, (char*)buffer));
			fCurSeekPos += count;
			fFurthestDiskWriteAt = fCurSeekPos;
		#else
			Led_Assert (false);	// NYI
		#endif
	}
	else {
		Led_Assert (fCurSeekPos >= fFurthestDiskWriteAt);	// Can only do buffering (easily) in this case
															// (otherwise we might need to do some reads off disk)

		/*
		 * We can do SOME buffering then. First be sure buffer window setup at all.
		 */
		if (fBufferWindowStart == fBufferWindowEnd) {
			fBufferWindowStart = fCurSeekPos;
			fBufferWindowEnd = fCurSeekPos + fOutputBufferSize;
		}
		if (fCurSeekPos >= fBufferWindowStart and fCurSeekPos+bytes < fBufferWindowStart + fOutputBufferSize) {
			// then we fit in the window. Just slide the data in.
			memcpy (&fOutputBuffer[fCurSeekPos-fBufferWindowStart], buffer, bytes);
			fCurSeekPos += bytes;
		}
		else {
			// then we must have a nearly full buffer. We could either write what we can
			// into it, and then flush, and then write the rest. Or we could just flush,
			// and memcpy() into the buffer at the start. Unclear which is best, so do whats
			// simplest.
			Flush ();
			Led_Assert (fBufferWindowStart == fBufferWindowEnd);
			fBufferWindowStart = fCurSeekPos;
			fBufferWindowEnd = fCurSeekPos + fOutputBufferSize;
			Led_Assert (fCurSeekPos >= fBufferWindowStart and fCurSeekPos+bytes < fBufferWindowEnd);
			memcpy (fOutputBuffer, buffer, bytes);
			fCurSeekPos += bytes;
		}
	}
}

void	StyledTextIOWriterSinkStream_FileDescriptor::SetBufferSize (size_t bufSize)
{
	if (fOutputBufferSize != bufSize) {
		Flush ();

		delete[] fOutputBuffer;
		fOutputBuffer = NULL;

		fOutputBuffer = new char [bufSize];
		fOutputBufferSize = bufSize;
		fBufferWindowStart = 0;
		fBufferWindowEnd = 0;
	}
}

void	StyledTextIOWriterSinkStream_FileDescriptor::Flush ()
{
	if (fBufferWindowStart != fBufferWindowEnd) {
		Led_Assert (fOutputBufferSize != 0);
		size_t	bytesInWindow	=	fBufferWindowEnd - fBufferWindowStart;
		Led_Assert (fOutputBufferSize >= bytesInWindow);
		#if		qMacOS
			Led_ThrowOSErr ( ::SetFPos (fFileDescriptor, fsFromStart, fBufferWindowStart));
			long	count	=	bytesInWindow;
			Led_ThrowOSErr (::FSWrite (fFileDescriptor, &count, fOutputBuffer));
		#elif	qWindows
			Led_Assert (false);
		#endif
		fFurthestDiskWriteAt = fBufferWindowEnd;
		fBufferWindowStart = 0;	// mark as no window now
		fBufferWindowEnd = 0;
	}
}

void	StyledTextIOWriterSinkStream_FileDescriptor::UpdateEOF ()
{
	Flush ();
	#if		qMacOS
		Led_ThrowOSErr (::SetEOF (fFileDescriptor, fCurSeekPos));
	#else
		Led_Assert (false);	// NYI
	#endif
}







/*
 ********************************************************************************
 ***************************** StyledTextIOReader *******************************
 ********************************************************************************
 */

string	StyledTextIOReader::GrabString (size_t from, size_t to)
{
	size_t	onEntrySeekPos	=	GetSrcStream ().current_offset ();

	size_t	effectiveTo	=	to;
	if (effectiveTo == size_t (-1)) {
		effectiveTo = onEntrySeekPos;
	}
	Led_Require (from <= effectiveTo);

	size_t	strLen	=	effectiveTo-from;
	Led_SmallStackBuffer<char>	buf (strLen+1);
	GetSrcStream ().seek_to (from);
	if (GetSrcStream ().read (buf, strLen) != strLen) {
		GetSrcStream ().seek_to (onEntrySeekPos);
		HandleBadlyFormattedInput (true);
	}
	GetSrcStream ().seek_to (onEntrySeekPos);
	buf[strLen] = '\0';
	return string (buf);
}






/*
 ********************************************************************************
 ******************************* StyledTextIOWriter *****************************
 ********************************************************************************
 */
void	StyledTextIOWriter::write (const void* data, size_t nBytes)
{
	GetSinkStream ().write (data, nBytes);
}

void	StyledTextIOWriter::write (char c)
{
	GetSinkStream ().write (&c, 1);
}

void	StyledTextIOWriter::write (const char* str)
{
	Led_RequireNotNil (str);
	GetSinkStream ().write (str, strlen (str));
}

void	StyledTextIOWriter::write (const string& str)
{
	GetSinkStream ().write (str.c_str (), str.length ());
}





/*
 ********************************************************************************
 ***************************** EmbeddingSinkStream ******************************
 ********************************************************************************
 */
EmbeddingSinkStream::EmbeddingSinkStream (StyledTextIOWriter::SinkStream& realSinkStream):
	SimpleEmbeddedObjectStyleMarker::SinkStream (),
	fRealSinkStream (realSinkStream)
{
}

void	EmbeddingSinkStream::write (const void* buffer, size_t bytes)
{
	fRealSinkStream.write (buffer, bytes);
}




#if		qLedUsesNamespaces
}
#endif






// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


