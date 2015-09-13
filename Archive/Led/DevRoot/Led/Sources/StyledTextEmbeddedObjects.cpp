/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/StyledTextEmbeddedObjects.cpp,v 2.96 2003/05/17 21:27:34 lewis Exp $
 *
 * Changes:
 *	$Log: StyledTextEmbeddedObjects.cpp,v $
 *	Revision 2.96  2003/05/17 21:27:34  lewis
 *	SPR#1489: use new helper Led_GetDIBBitsPointer () instead of directly adding offsets
 *	
 *	Revision 2.95  2003/04/18 21:03:02  lewis
 *	misc cleanups to get things compiling with gcc 2.96 on Linux RedHat 7.3
 *	
 *	Revision 2.94  2003/04/01 18:18:00  lewis
 *	SPR#1322: Added 'invalidRect' argument to StyledTextImager::StyleMarker::DrawSegment () and subclasses to be able to do logical clipping in tables
 *	
 *	Revision 2.93  2003/03/21 14:49:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.92  2003/03/16 16:15:33  lewis
 *	SPR#1345 - as a small part of this SPR - switch to using SimpleUpdater in place of direct
 *	calls to DoAboutToUpdateCalls()
 *	
 *	Revision 2.91  2003/03/07 00:22:51  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.90  2003/02/28 19:10:14  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.89  2003/02/19 20:31:25  lewis
 *	use TextStore::SimpleUpdater instead of explicit collectall/DoDidUpdate/etc
 *	
 *	Revision 2.88  2003/01/11 19:28:45  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.87  2002/11/28 02:31:14  lewis
 *	A couple simple name changes (GetVirtualText->PeekAtVirtualText and TextLayoutBlock_Subset to
 *	TextLayoutBlock_VirtualSubset
 *	
 *	Revision 2.86  2002/11/27 15:58:52  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 2.85  2002/11/18 21:11:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.84  2002/10/30 13:29:27  lewis
 *	SPR#1155 - Added new Rect_Base<> template and made Led_Rect a simple typedef of that. Then TRIED (failed)
 *	to make the top/left/bottom/right fields private by adding accessors. Added the accessors and used them
 *	in tons of places - but still TONS more required. Leave that battle for another day. I'm not even SURE
 *	its a good idea. Anyhow - also added a new typedef for Led_TWIPS_Rect (this was what I was after to begin
 *	with with all this nonsese)
 *	
 *	Revision 2.83  2002/10/30 01:29:54  lewis
 *	SPR#1155 - use Point_Base template for Led_TWIPS_Point
 *	
 *	Revision 2.82  2002/10/22 17:53:00  lewis
 *	SPR#1131 - StandardURLStyleMarker::GetDisplayString () strips tabs and replacing them with spaces
 *	
 *	Revision 2.81  2002/09/22 15:46:30  lewis
 *	SPR#1108- fix display of embeddings when we horizontally scroll (call GetHScrollPos).
 *	
 *	Revision 2.80  2002/05/06 21:33:51  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.79  2001/11/27 00:29:56  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.78  2001/10/17 20:43:00  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.77  2001/09/26 15:41:21  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.76  2001/09/26 15:03:56  lewis
 *	SPR#0996- fix EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes () to not
 *	add DIB-types for X-Windows (since not yet implemented)
 *	
 *	Revision 2.75  2001/09/18 14:03:17  lewis
 *	shorten define name from qStandardURLStyleMarkerNewDisplayMode to qURLStyleMarkerNewDisplayMode
 *	(avoid MSDEV60 warnings for define name length)
 *	
 *	Revision 2.74  2001/09/12 00:20:24  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.73  2001/09/05 16:28:54  lewis
 *	SPR#1016- cleanup semantics of ReaderFlavorPackage::GetFlavorSize/ReaderFlavorPackage::ReadFlavorData
 *	
 *	Revision 2.72  2001/09/05 00:18:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.71  2001/08/29 23:36:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.70  2001/08/28 18:43:39  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.69  2001/08/27 23:26:34  lewis
 *	5% instead of 10% size inclrease for pointsize of URL icon/text display
 *	
 *	Revision 2.68  2001/08/20 22:22:29  lewis
 *	SPR#0818- new display of URL objects (now just blue underlined slightly
 *	enlarged text). Can use qStandardURLStyleMarkerNewDisplayMode to get old code
 *	
 *	Revision 2.67  2001/08/17 17:04:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.66  2001/08/16 23:59:14  lewis
 *	fix display bug on MacOS with URL objects - cuz must set port later -
 *	just before draws. Really should be fixed by moving the SetPort stuff
 *	into the tablet APIs.
 *	
 *	Revision 2.65  2001/08/16 18:53:43  lewis
 *	SPR#0959- more work on 'GetInfo' support for embeddings.
 *	
 *	Revision 2.64  2001/07/19 02:21:47  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.63  2001/07/17 19:09:38  lewis
 *	SPR#0959- added preliminary RightClick-to-show-edit-embedding-properties support
 *	
 *	Revision 2.62  2001/05/30 01:57:08  lewis
 *	SPR#0944- set fore/back color before copybits to Black/White (not arg fore/back
 *	colors). Avoid colorization done by CopyBits
 *	
 *	Revision 2.61  2001/05/04 17:08:25  lewis
 *	SPR#0902: use TextImager::GetEffectiveDefaultTextColor () instead of Led_GetTextColor ()
 *	/Led_GetTextBackgroundColor ()
 *	
 *	Revision 2.60  2001/04/26 16:39:30  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.59  2001/04/23 17:21:05  lewis
 *	put in (bogus) value for XWINDOWS StandardURLStyleMarker::kURLDClipFormat so links
 *	
 *	Revision 2.58  2001/01/03 14:44:59  Lewis
 *	Use inherited keyword instead of StyleMarker directly (cleanup - and works around
 *	problem with borlandC++). Reorg a little more code so simpler - and now owrks with
 *	Borland. ANd comment out some unused code BoralandC++ caught
 *	
 *	Revision 2.57  2000/09/02 21:55:15  lewis
 *	use Led_CloneDIB() instead of manually cloning dibs. Then fix delete code to NOT
 *	delete as array of chars - but as single DIB object
 *	
 *	Revision 2.56  2000/08/29 13:35:38  lewis
 *	SPR#0819- handle reading '<a href=foo>bar</a>' properly - creating a URL
 *	object with that 'bar' text as the enclosed text.
 *	
 *	Revision 2.55  2000/08/13 04:06:44  lewis
 *	SPR#0821- in measuresegmentwidth - text[0] may not be the sentinal
 *	
 *	Revision 2.54  2000/07/09 04:31:30  lewis
 *	Added optional DIB associated with StandardUnknownTypeStyleMarker object (for SPR#0498)
 *	
 *	Revision 2.53  2000/06/13 20:07:24  lewis
 *	hack to get compiling for GCC/XWindows
 *	
 *	Revision 2.52  2000/06/13 16:26:15  lewis
 *	SPR#0767 - last tidbit - unknown embeddings - and RTFOLE embeddings - must use Led_TWIPS_Point
 *	
 *	Revision 2.51  2000/06/12 16:21:26  lewis
 *	support SPR#0780- Led_Tablet_ support
 *	
 *	Revision 2.50  2000/04/16 14:39:37  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.49  2000/04/16 13:58:55  lewis
 *	small tweeks to get recent changes compiling on Mac
 *	
 *	Revision 2.48  2000/04/15 14:32:38  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.47  2000/04/14 22:40:26  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.46  2000/04/14 19:17:35  lewis
 *	SPR#0739- react to change where we move much TextIntearctor flavor package code into
 *	its own module, and reorg the updateCursorPos etc stuff into separate code from code
 *	specific to each flavor type
 *	
 *	Revision 2.45  2000/04/03 18:54:50  lewis
 *	SPR#0729- fixed bug where NULL-name caused crash in UNICODE URL Embedding objects
 *	
 *	Revision 2.44  2000/03/28 03:41:05  lewis
 *	more work on qOldStyleMarkerBackCompatHelperCode/SPR#0718. Now StyleMarker class
 *	takes extra RunElement argument. Now this new code is called by default StyledTextImager::DrawText()
 *	etc overrides. And now HiddenText code properly merges at least its own drawing and
 *	StandardStyledTextImager::StyleMarker. All this respecting qOldStyleMarkerBackCompatHelperCode -
 *	so old code (not new hidden text code) should still compile and work roughly as before with this turned on.
 *	
 *	Revision 2.43  1999/12/21 20:55:28  lewis
 *	Use new Led_ThrowIfNull instead of manual check
 *	
 *	Revision 2.42  1999/12/14 18:10:52  lewis
 *	Add preliminary qXWindows support
 *	
 *	Revision 2.41  1999/12/09 03:27:40  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.40  1999/11/15 21:31:05  lewis
 *	Use new Led_XXX wrappers for Led_FontObject/LedPen etc instead of CFont/CPen - so no more MFC
 *	dependencies in any of this code (so can build for Win32 without MFC)
 *	
 *	Revision 2.39  1999/11/13 16:32:23  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.38  1999/07/13 23:06:55  lewis
 *	fixed broken new Lookup () command - use arg - not old hardwired value from cloned code
 *	
 *	Revision 2.37  1999/06/16 15:22:49  lewis
 *	add EmbeddedObjectCreatorRegistry::Lookup helper method as part of fix to spr# 0590
 *	
 *	Revision 2.36  1999/05/03 22:05:15  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.35  1999/05/03 21:41:31  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.34  1998/10/30 14:38:34  lewis
 *	Use vector<> instead of Led_Array
 *	
 *	Revision 2.33  1998/07/24  01:13:12  lewis
 *	UNICODE support. Cleanups.
 *
 *	Revision 2.32  1998/04/08  02:05:43  lewis
 *	Use SetMarkerStart() wrapper.
 *
 *	Revision 2.31  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.30  1997/12/24  03:33:20  lewis
 *	Use Led_GetMacPictSize instead of Hgiht/Width accessor.
 *	Support new fShownSize attribute.
 *	Misc related cleanups
 *
 *	Revision 2.29  1997/09/29  15:14:36  lewis
 *	Fix mac URL-embedding draw icon code.
 *	Use new/revised ::InsertEmbeddingForExistingSentinal code.
 *	cleanups.
 *
 *	Revision 2.28  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.27  1997/07/14  00:43:57  lewis
 *	Renamed TextInteractor_ to TextInteractor, TextStore_ to TextStore, SinkStream_ to SinkStream, etc.
 *
 *	Revision 2.26  1997/06/24  03:32:40  lewis
 *	Lose old test code.
 *
 *	Revision 2.25  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.24  1997/06/18  03:02:59  lewis
 *	qIncludePrefixFile.
 *	Use new CollectAllMarkersInRange_OrSurroundings support
 *
 *	Revision 2.23  1997/01/20  05:27:28  lewis
 *	Support colored text, and Led_Color class cahnges.
 *
 *	Revision 2.22  1997/01/10  03:16:28  lewis
 *	Moved qUseQuicktimeForWindows to LedConfig, and added throw specifiers.
 *
 *	Revision 2.21  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.20  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1996/10/31  00:27:18  lewis
 *	Must reset TextFace(0) when imaging/measure text for URLEmbdding on mac. And fix on pc - don't
 *	call textout with NULL embedding title (mfc asserts out, even though length==0).
 *
 *	Revision 2.18  1996/10/15  18:34:04  lewis
 *	For StandardURLStyleMarker class - get rid of kClipFormat and replace it with kURLDClipFormat
 *	and kWin32URLClipFormat.
 *	A few minor changes to support kWin32URLClipFormat.
 *
 *	Revision 2.17  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.16  1996/07/24  21:59:24  lewis
 *	Fix a couple small (but very serious) bugs with PICT/URL embeddings
 *	being read in from a stream (array of memory). Off by a few bytes
 *	here and there, and pretty soon, you have memory nasties... :-)
 *
 *	Revision 2.15  1996/07/03  01:14:45  lewis
 *	Moved most of the URL/Browser specific code into utility classes Led_URLD and
 *	Led_URLManager in LedSupport now.
 *
 *	Revision 2.14  1996/06/01  02:23:19  lewis
 *	Renamed Led_ThrowOutBadFormatDataException->Led_ThrowBadFormatDataException
 *	kDDETIMEOUT 2 seconds.
 *
 *	Revision 2.13  1996/05/23  20:06:35  lewis
 *	Mostly changed DrawSegment() arg from areaLeft to pixelsDrawn.
 *	Lots more diffs than this, probably due to cleanups/reformatting.
 *	Some obsolete code now deleted since no longer needed (due to new way of handling erasing)
 *
 *	Revision 2.12  1996/05/14  20:34:49  lewis
 *	Cleanups, DrawSegment() args changed- now takes tablet, and don't need to
 *	call Tablet_Acquirer object.
 *
 *	Revision 2.11  1996/05/04  21:15:41  lewis
 *	HUGE changes to image relalted code, and nearly all on PC.
 *	Supported DIBs (on mac and PC).
 *	Supported PICTS on PC (already had on Mac).
 *	Redid things (static routines) to share code.
 *	Support DDE to open URLs On PC (already had done for mac).
 *	Support Netscape 3.0 "Netscape Bookmark" as alias for URLD.
 *	Most of these markers didn't display right on the PC. Now I think
 *	they ALL do (static DIBs and PICTS for both Mac and PC for special messages
 *	etc and "unknown embedding" all display right now).
 *
 *	Revision 2.10  1996/04/18  15:31:48  lewis
 *	new shared picture code in MacPictureDrawSegment () etc so we can use
 *	in several places where we draw pictures.
 *	Reorganized lots of code to get better code sharing (still more to be done).
 *	Now embeddings can have multiple flavors. Implemented one such embedding
 *	(StandardMacPictureWithURLStyleMarker).
 *
 *	Revision 2.9  1996/03/16  18:51:29  lewis
 *	Some changes for tabstop support imacted here.
 *	Fixed baseline calculations and drawing from baseline, and make sure
 *	we always fully erase. Redid hilighing of drawing of pictures to use
 *	invertrect (we get some slight flicker - but at least we get nicer
 *	selection appearance for many cases this way).
 *	Later maybe use GWorld to reduce flicker.
 *	Cleanup StandardURLStyleMarker::FindBrowser () code a bit.
 *
 *	Revision 2.8  1996/03/05  18:33:54  lewis
 *	LedClip...::ClipFormat renamed -> Led_ClipFormat - similarly for TEXT/PICT
 *	constants.
 *	Cleaned up URL support, and supported Internet Config - for Macintosh only.
 *
 *	Revision 2.7  1996/03/04  07:47:42  lewis
 *	Lots of changes.
 *	Fixed draw of picture - had vertical/horizntal sizes backwards.
 *	Added new StandardUnknownTypeStyleMarker.
 *	Added mew StandardURLStyleMarker - incluiding basic support
 *	for openURL - to netscape (must cleanup and use InternetConfig, etc).
 *	New HandleClick/Open support for embeddings.
 *
 *	Revision 2.6  1996/02/26  22:24:48  lewis
 *	Use kEmbeddingSentinalChar instead of hardwired '\0';
 *	AddEmbedding() takes optional ownerForEmbdding instead of
 *	a StyleDatabase.
 *
 *	Revision 2.5  1996/01/11  08:19:26  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1996/01/03  23:59:42  lewis
 *	Added AddEmbedding() helper function.
 *
 *	Revision 2.3  1995/12/15  02:18:43  lewis
 *	Xtra arg for DrawSegment() for logical clipping. Use it in Picture embedding
 *	to avoid drawing picture if not needed.
 *
 *	Revision 2.2  1995/12/13  05:58:39  lewis
 *	Don't pass NULL for text in MeasureSegmentWidth(). Cuz we assert
 *	text[0] == '\0'.
 *
 *	Revision 2.1  1995/12/06  01:35:14  lewis
 *	Fixed DidUpdate () code to correct RIGHT side of marker when
 *	text is inserted.
 *	And fixed to not assume type of owner, but call new PeekAtTextStore()
 *	method of MarkerOwner_ instead.
 *
 *	Revision 2.0  1995/11/25  00:27:42  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/11/25  00:27:15  lewis
 *	Initial revision
 *
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#if		qMacOS
	#include	<TextEdit.h>		// for Apple TE scrap format and TEContinuous etc compatability
	#include	<QDOffscreen.h>
#endif

#include	"StandardStyledTextImager.h"

#include	"StyledTextEmbeddedObjects.h"







#if		qUseQuicktimeForWindows
	#include	"QTW.H"
#endif



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
	namespace	Led {
#endif





// Hack to free up registry data on program exit. No real point to this but
// to shutup MFC leak detector. That has SOME value, since it makes it easier to see REAL
// leaks.
static	struct	FooBarBlatzRegistryCleanupHack {
	~FooBarBlatzRegistryCleanupHack ()
		{
			delete EmbeddedObjectCreatorRegistry::sThe;
			EmbeddedObjectCreatorRegistry::sThe = NULL;
		}
}	sOneOfThese;



#if		qMacOS || qWindows
static	void	MacPictureDrawSegment (StandardMacPictureStyleMarker::PictureHandle pictureHandle,
											Led_Tablet tablet,
											Led_Color foreColor, Led_Color backColor,
											const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn,
											const Led_Size& imageSize,
											const Led_Size& margin = kDefaultEmbeddingMargin
										) throw ();
#endif
static	void	DIBDrawSegment (const Led_DIB* dib,
											Led_Tablet tablet,
											Led_Color foreColor, Led_Color backColor,
											const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn,
											const Led_Size& imageSize,
											const Led_Size& margin = kDefaultEmbeddingMargin
										) throw ();
#if		qMacOS
static	PixMap**	MakePixMapFromDIB (const Led_DIB* dib);
#endif

struct	UnsupportedFormat {
};











/*
 ********************************************************************************
 ************************** EmbeddedObjectCreatorRegistry ***********************
 ********************************************************************************
 */
EmbeddedObjectCreatorRegistry*	EmbeddedObjectCreatorRegistry::sThe	=	NULL;

void	EmbeddedObjectCreatorRegistry::AddStandardTypes ()
{
	#if		qMacOS || qWindows
		AddAssoc (StandardMacPictureStyleMarker::kClipFormat, StandardMacPictureStyleMarker::kEmbeddingTag, &StandardMacPictureStyleMarker::mk, &StandardMacPictureStyleMarker::mk);
		AddAssoc (StandardDIBStyleMarker::kClipFormat, StandardDIBStyleMarker::kEmbeddingTag, &StandardDIBStyleMarker::mk, &StandardDIBStyleMarker::mk);
	#endif

	AddAssoc (StandardURLStyleMarker::kURLDClipFormat, StandardURLStyleMarker::kEmbeddingTag, &StandardURLStyleMarker::mk, &StandardURLStyleMarker::mk);
	#if		qWindows
		AddAssoc (StandardURLStyleMarker::kWin32URLClipFormat, StandardURLStyleMarker::kEmbeddingTag, &StandardURLStyleMarker::mk, &StandardURLStyleMarker::mk);
	#endif

	#if		qMacOS || qWindows
		AddAssoc (StandardMacPictureWithURLStyleMarker::kClipFormats, StandardMacPictureWithURLStyleMarker::kClipFormatCount, StandardMacPictureWithURLStyleMarker::kEmbeddingTag, &StandardMacPictureWithURLStyleMarker::mk, &StandardMacPictureWithURLStyleMarker::mk);
		AddAssoc (StandardMacPictureWithURLStyleMarker::kOld1EmbeddingTag, &StandardMacPictureWithURLStyleMarker::mk);
		AddAssoc (StandardDIBWithURLStyleMarker::kClipFormats, StandardDIBWithURLStyleMarker::kClipFormatCount, StandardDIBWithURLStyleMarker::kEmbeddingTag, &StandardDIBWithURLStyleMarker::mk, &StandardDIBWithURLStyleMarker::mk);
	#endif
}

bool	EmbeddedObjectCreatorRegistry::Lookup (const char* embeddingTag, Assoc* result) const
{
	Led_RequireNotNil (embeddingTag);
	Led_RequireNotNil (result);
	const vector<EmbeddedObjectCreatorRegistry::Assoc>&	types	=	GetAssocList ();
	for (size_t i = 0; i < types.size (); i++) {
		EmbeddedObjectCreatorRegistry::Assoc	assoc	=	types[i];
		if (memcmp (assoc.fEmbeddingTag, embeddingTag, sizeof (assoc.fEmbeddingTag)) == 0) {
			*result = assoc;
			return true;
		}
	}
	return false;
}









/*
 ********************************************************************************
 ********************* SimpleEmbeddedObjectStyleMarker **************************
 ********************************************************************************
 */
SimpleEmbeddedObjectStyleMarker::CommandNames	SimpleEmbeddedObjectStyleMarker::sCommandNames		=		SimpleEmbeddedObjectStyleMarker::MakeDefaultCommandNames ();

SimpleEmbeddedObjectStyleMarker::SimpleEmbeddedObjectStyleMarker ():
	inherited ()
{
}

int	SimpleEmbeddedObjectStyleMarker::GetPriority () const
{
	return eEmbeddedObjectPriority;
}

Led_Distance	SimpleEmbeddedObjectStyleMarker::MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
{
	// Baseline for embeddings should be very bottom of the embedding (less the bottom margin)
	Led_Require (from + 1 == to);
	return (MeasureSegmentHeight (imager, runElement, from, to) - 1*kDefaultEmbeddingMargin.v);
}

void	SimpleEmbeddedObjectStyleMarker::DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) throw ()
{
	Led_RequireNotNil (GetOwner ());
	Led_RequireNotNil (GetOwner ()->PeekAtTextStore ());
	size_t	newLength	=	GetLength ();
	if (newLength == 0) {
		GetOwner ()->PeekAtTextStore ()->RemoveMarker (this);
		delete this;
	}
	else if (newLength == 1) {
		inherited::DidUpdateText (updateInfo);
	}
	else {
		Led_Assert (GetEnd () > 0);	// cuz otherwise we couldn't have a non-zero length!
		GetOwner ()->PeekAtTextStore ()->SetMarkerStart (this, GetEnd ()-1);
		inherited::DidUpdateText (updateInfo);
	}
}

/*
@METHOD:		SimpleEmbeddedObjectStyleMarker::HandleClick
@DESCRIPTION:	<p>Called when a user clicks on the given embedding. 'clickedAt' is relative to the embedding itself.</p>
*/
bool	SimpleEmbeddedObjectStyleMarker::HandleClick (Led_Point /*clickedAt*/, unsigned clickCount)
{
	if (clickCount == 2) {
		return HandleOpen ();
	}
	return true;
}

bool	SimpleEmbeddedObjectStyleMarker::HandleOpen ()
{
	return true;
}

/*
@METHOD:		SimpleEmbeddedObjectStyleMarker::GetCmdNumbers
@DESCRIPTION:	<p>Return a list of all private commands (such as eOpenCmdNum) which this embedding object can support.
			See also @'SimpleEmbeddedObjectStyleMarker::IsCmdEnabled' (), @'SimpleEmbeddedObjectStyleMarker::GetCmdText' (),
			and @'SimpleEmbeddedObjectStyleMarker::DoCommand' () which also must be supported for each of those commands.</p>
*/
vector<SimpleEmbeddedObjectStyleMarker::PrivateCmdNumber>	SimpleEmbeddedObjectStyleMarker::GetCmdNumbers () const
{
	return vector<PrivateCmdNumber> ();
}

/*
@METHOD:		SimpleEmbeddedObjectStyleMarker::IsCmdEnabled
@DESCRIPTION:	<p>For all commands specified in overrides of @'SimpleEmbeddedObjectStyleMarker::GetCmdNumbers' (ie all private commands
			supported) return whether or not that command is currently enabled.</p>
*/
bool	SimpleEmbeddedObjectStyleMarker::IsCmdEnabled (PrivateCmdNumber /*cmd*/) const
{
	return false;	// default to disabled - subclasses override SimpleEmbeddedObjectStyleMarker::GetCmdNumbers () && SimpleEmbeddedObjectStyleMarker::IsCmdEnabled ()
}

/*
@METHOD:		SimpleEmbeddedObjectStyleMarker::GetCmdText
@DESCRIPTION:	<p>For all commands specified in overrides of @'SimpleEmbeddedObjectStyleMarker::GetCmdNumbers' (ie all private commands
			supported) return the command name text.</p>
*/
Led_SDK_String	SimpleEmbeddedObjectStyleMarker::GetCmdText (PrivateCmdNumber cmd)
{
	switch (cmd) {
		case	eOpenCmdNum:	return GetCommandNames ().fOpenCommandName;
		default:				Led_Assert (false); return Led_SDK_String ();
	}
}

/*
@METHOD:		SimpleEmbeddedObjectStyleMarker::DoCommand
@DESCRIPTION:	<p>For all commands specified in overrides of @'SimpleEmbeddedObjectStyleMarker::GetCmdNumbers' (ie all private commands
			supported) perform that command. This is invoked by the menu commands, etc (for example). The only command automatically
			handled by this implementation (@'SimpleEmbeddedObjectStyleMarker::DoCommand') is for eOpenCmdNum, and that just invokes
			@'SimpleEmbeddedObjectStyleMarker::HandleOpen' (). For other command numbers, the subclasser must handle the
			commands themselves.</p>
*/
void	SimpleEmbeddedObjectStyleMarker::DoCommand (PrivateCmdNumber cmd)
{
	switch (cmd) {
		case	eOpenCmdNum:	HandleOpen (); break;
		default:				Led_Assert (false); break;
	}
}

SimpleEmbeddedObjectStyleMarker::CommandNames	SimpleEmbeddedObjectStyleMarker::MakeDefaultCommandNames ()
{
	SimpleEmbeddedObjectStyleMarker::CommandNames	cmdNames;
	cmdNames.fOpenCommandName		=	Led_SDK_TCHAROF ("Open Embedding");
	return cmdNames;
}







#if		qMacOS || qWindows
/*
 ********************************************************************************
 ************************** StandardMacPictureStyleMarker ***********************
 ********************************************************************************
 */
#if		qMacOS
	const	Led_ClipFormat		StandardMacPictureStyleMarker::kClipFormat		=	kPICTClipFormat;
#elif	qWindows
	// Surprising, the QuickTime for windows 2.1 picture viewer doesn't appear to export (on copy) a
	// native format rep of the picture. So I've no guess what the RIGHT arg is to RegisterClipboardFormat.
	// So - this is as good a guess as any. At least then Led can xfer pictures among instances of itself.
	// LGP 960429
	const	Led_ClipFormat		StandardMacPictureStyleMarker::kClipFormat		=	::RegisterClipboardFormat (_T ("Mac PICT Format"));
#endif
const	Led_PrivateEmbeddingTag	StandardMacPictureStyleMarker::kEmbeddingTag	=	"Pict2";

#if		qWindows
const Led_DIB*	StandardMacPictureStyleMarker::sUnsupportedFormatPict	=	NULL;
#endif

StandardMacPictureStyleMarker::StandardMacPictureStyleMarker (const Led_Picture* pictData, size_t picSize):
	SimpleEmbeddedObjectStyleMarker (),
	fPictureHandle (NULL)
	#if		qWindows
	,fPictureSize (0)
	#endif
{
	Led_RequireNotNil (pictData);
	#if		qWindows
		Led_RequireNotNil (sUnsupportedFormatPict);
	#endif
	#if		qMacOS
		fPictureHandle = (PictureHandle)Led_DoNewHandle (picSize);
	#elif	qWindows
		fPictureSize = picSize;
		fPictureHandle = ::GlobalAlloc (GMEM_MOVEABLE, picSize);
		Led_ThrowIfNull (fPictureHandle);
	#endif
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	memcpy (locker.GetPointer (), pictData, picSize);
}

StandardMacPictureStyleMarker::~StandardMacPictureStyleMarker ()
{
	Led_AssertNotNil (fPictureHandle);
	#if		qMacOS
		::DisposeHandle (Handle (fPictureHandle));
	#else
		::GlobalFree (fPictureHandle);
	#endif
}

SimpleEmbeddedObjectStyleMarker*	StandardMacPictureStyleMarker::mk (const char* embeddingTag, const void* data, size_t len)
{
	Led_Require (memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0);
	Led_Arg_Unused (embeddingTag);
	return (new StandardMacPictureStyleMarker ((Led_Picture*)data, len));
}

SimpleEmbeddedObjectStyleMarker*	StandardMacPictureStyleMarker::mk (ReaderFlavorPackage& flavorPackage)
{
	size_t	length		=	flavorPackage.GetFlavorSize (kClipFormat);
	Led_SmallStackBuffer<Led_tChar> buf (length);
	length = flavorPackage.ReadFlavorData (kClipFormat, length, buf);
	return (mk (kEmbeddingTag, buf, length));
}

void	StandardMacPictureStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text,
											const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										)
{
	Led_Assert (from + 1 == to);
	Led_Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	MacPictureDrawSegment (GetPictureHandle (), tablet,
							imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
							drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, Led_GetMacPictSize ((Led_Picture*)locker.GetPointer ())
						);
}

void	StandardMacPictureStyleMarker::MeasureSegmentWidth (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	Led_Assert (from + 1 == to);
	Led_RequireNotNil (text);
	/*
	 *	Though we generally require that:
	 *
	 *		Led_Require (text[0] == kEmbeddingSentinalChar);
	 *
	 *	we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
	 *	in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
	 *	when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
	 *	character here. We know the right width here anyhow.
	 *		See SPR#0821.
	 */
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	distanceResults[0] = Led_GetMacPictWidth ((Led_Picture*)locker.GetPointer ()) + 2*kDefaultEmbeddingMargin.h;
}

Led_Distance	StandardMacPictureStyleMarker::MeasureSegmentHeight (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to) const
{
	Led_Assert (from + 1 == to);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	return (Led_GetMacPictHeight ((Led_Picture*)locker.GetPointer ()) + 2*kDefaultEmbeddingMargin.v);
}

void	StandardMacPictureStyleMarker::Write (SinkStream& sink)
{
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	sink.write (locker.GetPointer (), GetPictureByteSize ());
}

void	StandardMacPictureStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	flavorPackage.AddFlavorData (kClipFormat, GetPictureByteSize (), (Led_Picture*)locker.GetPointer ());
}

const char*	StandardMacPictureStyleMarker::GetTag () const
{
	return kEmbeddingTag;
}
#endif











/*
 ********************************************************************************
 ************************** StandardDIBStyleMarker ******************************
 ********************************************************************************
 */
#if		qMacOS
	// I don't know of any standard type for this, so just make one up...
	// LGP 960429
	const	Led_ClipFormat			StandardDIBStyleMarker::kClipFormat		=	'DIB ';
#elif	qWindows
	const	Led_ClipFormat			StandardDIBStyleMarker::kClipFormat		=	CF_DIB;
#elif	qXWindows
	const	Led_ClipFormat			StandardDIBStyleMarker::kClipFormat		=	666;		// X-TMP-HACK-LGP991214
#endif
const	Led_PrivateEmbeddingTag		StandardDIBStyleMarker::kEmbeddingTag	=	"DIB";

#if		qMacOS
Led_Picture**	StandardDIBStyleMarker::sUnsupportedFormatPict	=	NULL;
#endif

StandardDIBStyleMarker::StandardDIBStyleMarker (const Led_DIB* pictData):
	SimpleEmbeddedObjectStyleMarker (),
	fDIBData (NULL)
{
	#if		qMacOS
		Led_RequireNotNil (sUnsupportedFormatPict);	// see class declaration for descriptio
	#endif
	Led_RequireNotNil (pictData);
	fDIBData = Led_CloneDIB (pictData);
}

StandardDIBStyleMarker::~StandardDIBStyleMarker ()
{
	Led_AssertNotNil (fDIBData);
	delete fDIBData;
}

SimpleEmbeddedObjectStyleMarker*	StandardDIBStyleMarker::mk (const char* embeddingTag, const void* data, size_t len)
{
	Led_Require (memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0);
	Led_Arg_Unused (embeddingTag);
	if (len < 40) {
		// This is less than we need to peek and see size of DIB...
		Led_ThrowBadFormatDataException ();
	}

	size_t	picSize	=	Led_GetDIBImageByteCount ((Led_DIB*)data);
	// It appears that sometimes (at least on NT 4.0 beta) that the length of the flavor package is longer
	// than we expect. I'm guessing this is some optional additional information MS now passes along.
	// The picts APPEAR to come out looking fine if we ignore this stuff of the end. So continue doing so...
	if (len != picSize) {
		// Set a breakpoint here if this worries you...
		if (len < picSize) {
			// This is definitely bad!!!
			Led_ThrowBadFormatDataException ();
		}
		else {
			// we'll just ignore the stuff off the end... Hope thats OK - LGP 960429
		}
	}
	return (new StandardDIBStyleMarker ((Led_DIB*)data));
}

SimpleEmbeddedObjectStyleMarker*	StandardDIBStyleMarker::mk (ReaderFlavorPackage& flavorPackage)
{
	size_t	length		=	flavorPackage.GetFlavorSize (kClipFormat);
	Led_SmallStackBuffer<Led_tChar> buf (length);
	length = flavorPackage.ReadFlavorData (kClipFormat, length, buf);
	return (mk (kEmbeddingTag, buf, length));
}

void	StandardDIBStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text,
											const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										)
{
	Led_Assert (from + 1 == to);
	Led_Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);
	DIBDrawSegment (fDIBData, tablet, 
							imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
							drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, Led_GetDIBImageSize (fDIBData)
						);
}

void	StandardDIBStyleMarker::MeasureSegmentWidth (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	Led_Assert (from + 1 == to);
	Led_RequireNotNil (text);
	/*
	 *	Though we generally require that:
	 *
	 *		Led_Require (text[0] == kEmbeddingSentinalChar);
	 *
	 *	we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
	 *	in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
	 *	when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
	 *	character here. We know the right width here anyhow.
	 *		See SPR#0821.
	 */
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);
	distanceResults[0] = Led_GetDIBImageSize (GetDIBData ()).h + 2*kDefaultEmbeddingMargin.h;
}

Led_Distance	StandardDIBStyleMarker::MeasureSegmentHeight (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to) const
{
	Led_Assert (from + 1 == to);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	return (Led_GetDIBImageSize (GetDIBData ()).v + 2*kDefaultEmbeddingMargin.v);
}

void	StandardDIBStyleMarker::Write (SinkStream& sink)
{
	const Led_DIB*	dib	=	GetDIBData ();
	size_t	dibSize	=	Led_GetDIBImageByteCount (dib);
	sink.write (dib, dibSize);
}

void	StandardDIBStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
	const Led_DIB*	dib	=	GetDIBData ();
	size_t	dibSize	=	Led_GetDIBImageByteCount (dib);
	flavorPackage.AddFlavorData (kClipFormat, dibSize, dib);
}

const char*	StandardDIBStyleMarker::GetTag () const
{
	return kEmbeddingTag;
}







/*
 ********************************************************************************
 ****************************** StandardURLStyleMarker **************************
 ********************************************************************************
 */
#if		qMacOS
	const	Led_ClipFormat			StandardURLStyleMarker::kURLDClipFormat		=	'URLD';
#elif	qWindows
	// Netscape USED to have some sort of predefined name like Netscsape Bookmark, or something like that.
	// Apparently not any more. Will have to investigate further to see what todo for Netscape...
	// LGP 960429
	const	Led_ClipFormat			StandardURLStyleMarker::kURLDClipFormat		=	::RegisterClipboardFormat (_T ("Netscape Bookmark"));
	const	Led_ClipFormat			StandardURLStyleMarker::kWin32URLClipFormat	=	::RegisterClipboardFormat (_T ("UniformResourceLocator"));
#elif	qXWindows
	const	Led_ClipFormat			StandardURLStyleMarker::kURLDClipFormat		=	'URLD';	//??? NOT SURE WHAT RIGHT ANSWER SB HERE!!!
#endif

#if		!qURLStyleMarkerNewDisplayMode
#if		qMacOS
Led_Picture**					StandardURLStyleMarker::sURLPict		=	NULL;
#elif	qWindows
const Led_DIB*					StandardURLStyleMarker::sURLPict		=	NULL;
#endif

#endif
const	Led_PrivateEmbeddingTag	StandardURLStyleMarker::kEmbeddingTag	=	"URL";




StandardURLStyleMarker::StandardURLStyleMarker (const Led_URLD& urlData):
	SimpleEmbeddedObjectStyleMarker (),
	fURLData (urlData)
{
	#if		!qURLStyleMarkerNewDisplayMode
		#if		qMacOS || qWindows
			Led_RequireNotNil (sURLPict);	// If this is ever triggered, see class declaration where we delcare this field
		#endif
	#endif
}

StandardURLStyleMarker::~StandardURLStyleMarker ()
{
}

SimpleEmbeddedObjectStyleMarker*	StandardURLStyleMarker::mk (const char* embeddingTag, const void* data, size_t len)
{
	Led_Require (memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0);
	Led_Arg_Unused (embeddingTag);
	return (new StandardURLStyleMarker (Led_URLD (data, len)));
}

SimpleEmbeddedObjectStyleMarker*	StandardURLStyleMarker::mk (ReaderFlavorPackage& flavorPackage)
{	
	/*
	 *	First try URLD format, and then Win32URL format.
	 */
	if (flavorPackage.GetFlavorAvailable (kURLDClipFormat)) {
		size_t	length		=	flavorPackage.GetFlavorSize (kURLDClipFormat);
		Led_SmallStackBuffer<Led_tChar> buf (length);
		length = flavorPackage.ReadFlavorData (kURLDClipFormat, length, buf);
		return (mk (kEmbeddingTag, buf, length));
	}
	#if		qWindows
		if (flavorPackage.GetFlavorAvailable (kWin32URLClipFormat)) {
			size_t	length		=	flavorPackage.GetFlavorSize (kWin32URLClipFormat);
			Led_SmallStackBuffer<char> buf (length);
			length = flavorPackage.ReadFlavorData (kWin32URLClipFormat, length, buf);
			// tmp/medium term hack.. Seems both formats look roughly the same. URL first. Then title. At least for now,
			// we can take advtangage of this and share code on read. If not, we can read/reformat to fit so this below works.
			// LGP 961012
			return (mk (kEmbeddingTag, buf, length));
		}
	#endif
	Led_ThrowBadFormatDataException ();
	Led_Assert (false); return NULL;
}

void	StandardURLStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text,
											const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										)
{
	Led_RequireNotNil (imager);

	#if		qURLStyleMarkerNewDisplayMode
		Led_Arg_Unused (to);
		Led_Arg_Unused (text);
		Led_FontSpecification	fsp			=	GetDisplayFont (runElement);
		Led_tString				displayText	=	GetDisplayString ();
		imager->DrawSegment_ (tablet, fsp, from, from + displayText.length (), TextLayoutBlock_Basic (displayText.c_str (), displayText.c_str () + displayText.length ()), 
								drawInto, useBaseLine, pixelsDrawn
							);
	#else
		Led_Distance	width	=	0;
		MeasureSegmentWidth (imager, runElement, from, to, text, &width);
		width -= 2*kDefaultEmbeddingMargin.h;
		Led_Distance	height	=	MeasureSegmentHeight (imager, runElement, from, to) - 2*kDefaultEmbeddingMargin.v;
		
		Led_Rect	ourBoundsRect = drawInto - Led_Point (0, imager->GetHScrollPos ());
		ourBoundsRect.right = ourBoundsRect.left + width + 2*kDefaultEmbeddingMargin.h;
		Led_Coordinate	embedBottom	=	useBaseLine;
		Led_Coordinate	embedTop	=	embedBottom - height;
		Led_Assert (embedTop >= drawInto.top);
		Led_Assert (embedBottom <= drawInto.bottom);
		Led_Rect	innerBoundsRect = Led_Rect (Led_Point (embedTop, ourBoundsRects.left + kDefaultEmbeddingMargin.h), Led_Size (height, width));

		Led_Color	foreColor	=	imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor);
		Led_Color	backColor	=	imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor);

		#if		qMacOS
			tablet->SetPort ();
		#endif

		#if		qMacOS
			GDI_RGBForeColor (foreColor.GetOSRep ());
			GDI_RGBBackColor (backColor.GetOSRep ());
		#elif	qWindows
			tablet->SetTextColor (foreColor.GetOSRep ());
			tablet->SetBkColor (backColor.GetOSRep ());
		#endif

		const	char*	url			=	fURLData.PeekAtURLD ();
		const	char*	urlEnd		=	::strchr (url, '\r');
		size_t			urlStrLen	=	(urlEnd == NULL)? strlen (url): (urlEnd-url);
		const	char*	name	=	::strchr (fURLData.PeekAtURLD (), '\r');
		if (name != NULL) {
			name++;	// skip CR
		}
		size_t			nameStrLen	=	(name == NULL)? 0: ::strlen (name);

		/*
		 *	Draw the marker on the baseline (to from the top of the drawing area).
		 *	And be sure to erase everything in the draw rect!
		 */
		#if		qMacOS || qWindows
			Led_AssertNotNil (sURLPict);
		#endif
		#if		qMacOS
			Rect	rr	=	AsQDRect (innerBoundsRect);
			PenNormal ();
			PenSize (2,2);
			::FrameRoundRect (&rr, 2, 2);
			Rect	iconRect	= rr;
			iconRect.bottom = iconRect.top + Led_GetMacPictHeight (sURLPict);
			iconRect.right = iconRect.left + Led_GetMacPictWidth (sURLPict);
			::OffsetRect (&iconRect, 2, 2);	// take into account border

			::DrawPicture (sURLPict, &iconRect);

			::MoveTo (iconRect.right + 3, iconRect.top + 16);
			::TextFont (kFontIDTimes);
			::TextSize (14);
			::TextFace (0);
			::DrawText (name, 0, nameStrLen);

			::MoveTo (iconRect.right + 3, iconRect.top + 28);
			::TextFont (kFontIDGeneva);
			::TextSize (9);
			::DrawText (url, 0, urlStrLen);
		#elif	qWindows
			Led_Pen					pen (PS_SOLID, 2, RGB (0,0,0));
			Led_Win_Obj_Selector	penWrapper (tablet, pen);
			Led_Win_Obj_Selector	brush (tablet, ::GetStockObject (NULL_BRUSH));
			tablet->RoundRect (innerBoundsRect.left, innerBoundsRect.top, innerBoundsRect.right, innerBoundsRect.bottom, 2, 2);

			Led_Rect	iconRect	= innerBoundsRect;
			Led_Size	dibImageSize	=	Led_GetDIBImageSize (sURLPict);
			iconRect.bottom = iconRect.top + dibImageSize.v;
			iconRect.right = iconRect.left + dibImageSize.h;
			//const BITMAPINFOHEADER&	hdr			=	sURLPict->bmiHeader;
			const void*				lpBits		=	Led_GetDIBBitsPointer (sURLPict);
			//const char*				lpBits		=	((const char*)sURLPict) + Led_ByteSwapFromWindows (hdr.biSize) + Led_GetDIBPalletByteCount (sURLPict);
			::StretchDIBits (tablet->m_hDC, iconRect.left, iconRect.top, iconRect.GetWidth (), iconRect.GetHeight (), 0, 0, dibImageSize.h, dibImageSize.v, lpBits, sURLPict, DIB_RGB_COLORS, SRCCOPY);

			Led_FontObject			font1;
			{
				LOGFONT	lf;
				memset (&lf, 0, sizeof lf);
				_tcscpy (lf.lfFaceName, _T ("System"));
				Led_Verify (font1.CreateFontIndirect (&lf));
			}
			Led_Win_Obj_Selector	font1Selector (tablet, font1);
			if (nameStrLen != 0) {
				::TextOutA (*tablet, iconRect.right + 3, iconRect.top + 2, name, nameStrLen);
			}

			Led_FontObject			font2;
			{
				LOGFONT	lf;
				memset (&lf, 0, sizeof lf);
				_tcscpy (lf.lfFaceName, _T ("Arial"));
				Led_Verify (font2.CreateFontIndirect (&lf));
				lf.lfHeight = -8;
			}
			Led_Win_Obj_Selector	font2Selector (tablet, font2);
			if (urlStrLen != 0) {
				::TextOutA (*tablet, iconRect.right + 3, iconRect.top + 16, url, urlStrLen);
			}
		#endif
		if (pixelsDrawn != NULL) {
			*pixelsDrawn = ourBoundsRect.GetWidth ();
		}
	#endif
}

void	StandardURLStyleMarker::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	Led_Assert (from + 1 == to);
	Led_RequireNotNil (text);
	/*
	 *	Though we generally require that:
	 *
	 *		Led_Require (text[0] == kEmbeddingSentinalChar);
	 *
	 *	we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
	 *	in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
	 *	when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
	 *	character here. We know the right width here anyhow.
	 *		See SPR#0821.
	 */
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);

	#if		qURLStyleMarkerNewDisplayMode
		Led_FontSpecification				fsp			=	GetDisplayFont (runElement);
		Led_tString							displayText	=	GetDisplayString ();
		if (displayText.empty ()) {
			distanceResults[0] = 0;
		}
		else {
			Led_SmallStackBuffer<Led_Distance>	distRes (displayText.length ());
			imager->MeasureSegmentWidth_ (fsp, from, from + displayText.length (), displayText.c_str (), distRes);
			distanceResults[0] = distRes[displayText.length () - 1];
		}
	#else
		const	char*	url	=	fURLData.PeekAtURLD ();
		const	char*	urlEnd	=		::strchr (url, '\r');
		size_t			urlStrLen	=	(urlEnd == NULL)? strlen (url): (urlEnd-url);
		const	char*	name	=	::strchr (fURLData.PeekAtURLD (), '\r');
		if (name != NULL) {
			name++;	// skip CR
		}
		size_t			nameStrLen	=	(name == NULL)? 0: ::strlen (name);

		TextInteractor::Tablet_Acquirer	tablet_ (imager);
		Led_Tablet						tablet		=	tablet_;

		#if		qMacOS
			tablet->SetPort ();
		#elif	qWindows
			Led_Tablet	dc	=	tablet;
		#endif

		#if		qMacOS
			Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) sURLPict);
			// First add in width of picture
			distanceResults[0] = Led_GetMacPictWidth (sURLPict);
			
			distanceResults[0] += 2*3;	// leave room for spacing around text

			// Leave room for text - largest of two strings
			::TextFont (kFontIDTimes);
			::TextSize (14);
			::TextFace (0);
			Led_Distance	string1Width	=		::TextWidth (name, 0, nameStrLen);

			::TextFont (kFontIDGeneva);
			::TextSize (9);
			Led_Distance	string2Width	=		::TextWidth (url, 0, urlStrLen);

			distanceResults[0] += Led_Max (string1Width, string2Width) + 2*kDefaultEmbeddingMargin.h;

		#elif	qWindows
			// First add in width of picture
			distanceResults[0] = Led_GetDIBImageSize (sURLPict).h;
			
			distanceResults[0] += 2*3;	// leave room for spacing around text


			// Leave room for text - largest of two strings
			Led_FontObject			font1;
			{
				LOGFONT	lf;
				memset (&lf, 0, sizeof lf);
				_tcscpy (lf.lfFaceName, _T ("System"));
				Led_Verify (font1.CreateFontIndirect (&lf));
			}
			Led_Win_Obj_Selector	font1Selector (tablet, font1);
			Led_Distance			string1Width	=	name==NULL? 0: dc->GetTextExtent (Led_ANSI2SDKString (name).c_str (), nameStrLen).cx;

			Led_FontObject			font2;
			{
				LOGFONT	lf;
				memset (&lf, 0, sizeof lf);
				_tcscpy (lf.lfFaceName, _T ("Arial"));
				Led_Verify (font2.CreateFontIndirect (&lf));
				lf.lfHeight = -8;
			}
			Led_Win_Obj_Selector	font2Selector (tablet, font2);
			Led_Distance	string2Width	=	dc->GetTextExtent (Led_ANSI2SDKString (url).c_str (), urlStrLen).cx;

			distanceResults[0] += Led_Max (string1Width, string2Width) + 2*kDefaultEmbeddingMargin.h;
		#endif
	#endif
}

Led_Distance	StandardURLStyleMarker::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const
{
	Led_Assert (from + 1 == to);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);

	#if		qURLStyleMarkerNewDisplayMode
		Led_FontSpecification				fsp			=	GetDisplayFont (runElement);
		Led_tString							displayText	=	GetDisplayString ();
		return imager->MeasureSegmentHeight_ (fsp, from, from + displayText.length ());
	#else
		// this '36' is something of a hack. Really we should set the font into a grafport, and measure font metrics etc.
		// but this is good enuf - I believe - LGP 960314
		#if		qMacOS
			Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) sURLPict);
			return Led_Max (Led_GetMacPictHeight (sURLPict), 36) + 2*kDefaultEmbeddingMargin.v;
		#elif	qWindows
			return Led_Max (Led_GetDIBImageSize (sURLPict).v, 36) + 2*kDefaultEmbeddingMargin.v;
		#endif
	#endif
}

void	StandardURLStyleMarker::Write (SinkStream& sink)
{
	sink.write (fURLData.PeekAtURLD (), fURLData.GetURLDLength ());
}

void	StandardURLStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
	flavorPackage.AddFlavorData (kURLDClipFormat, fURLData.GetURLDLength (), fURLData.PeekAtURLD ());
	#if		qWindows
	size_t	len	=	fURLData.GetURLLength () + 1;
	Led_SmallStackBuffer<char>	hackBuf(len);
	memcpy (hackBuf, fURLData.PeekAtURL (), len-1);
	hackBuf[len] = '\0';
	flavorPackage.AddFlavorData (kWin32URLClipFormat, len, hackBuf);
	#endif
}

const char*	StandardURLStyleMarker::GetTag () const
{
	return kEmbeddingTag;
}

bool	StandardURLStyleMarker::HandleOpen ()
{
	Led_URLManager::Get ().Open (fURLData.GetURL ());
	return false;	// indicate double click 'eaten'
//	return (HandleOpenURL (fURLData.PeekAtURLD ()));
}

vector<StandardURLStyleMarker::PrivateCmdNumber>	StandardURLStyleMarker::GetCmdNumbers () const
{
	vector<PrivateCmdNumber>	x;
	x.push_back (eOpenCmdNum);
	return x;
}

bool	StandardURLStyleMarker::IsCmdEnabled (PrivateCmdNumber cmd) const
{
	switch (cmd) {
		case	eOpenCmdNum:	return true;
		default:				return inherited::IsCmdEnabled (cmd);
	}
}

const Led_URLD&	StandardURLStyleMarker::GetURLData () const
{
	return fURLData;
}

void	StandardURLStyleMarker::SetURLData (const Led_URLD& urlData)
{
	fURLData = urlData;
}

Led_tString	StandardURLStyleMarker::GetDisplayString () const
{
	string	displayText	=	fURLData.GetTitle ();
	if (displayText.empty ()) {
		displayText = fURLData.GetURL ();
	}
	/*
	 *	Replace tab characters with space characters, since tabs won't get aligned properly, and wouldn't make much
	 *	sense anyhow (SPR#1131).
	 */
	for (string::iterator i = displayText.begin (); i != displayText.end (); ++i) {
		if (*i == '\t') {
			*i = ' ';
		}
	}
	return Led_ANSIString2tString (displayText);
}

Led_FontSpecification	StandardURLStyleMarker::GetDisplayFont (const RunElement& runElement) const
{
	Led_FontSpecification	fsp;
	if (dynamic_cast<StandardStyledTextImager::StandardStyleMarker*> (runElement.fMarker) != NULL) {
		StandardStyledTextImager::StandardStyleMarker*	sm	=	dynamic_cast<StandardStyledTextImager::StandardStyleMarker*> (runElement.fMarker);
		fsp = sm->fFontSpecification;
	}
	else {
		for (vector<StyledTextImager::StyleMarker*>::const_iterator i = runElement.fSupercededMarkers.begin (); i != runElement.fSupercededMarkers.end (); ++i) {
			StandardStyledTextImager::StandardStyleMarker*	sm	=	dynamic_cast<StandardStyledTextImager::StandardStyleMarker*> (*i);
			if (sm != NULL) {
				fsp = sm->fFontSpecification;
				break;
			}
		}
	}
	fsp.SetPointSize (static_cast<unsigned short> (fsp.GetPointSize () * 1.05));
	fsp.SetTextColor (Led_Color::kBlue);
	fsp.SetStyle_Underline (true);
	return fsp;
}


















#if		qMacOS || qWindows
/*
 ********************************************************************************
 ************************ StandardMacPictureWithURLStyleMarker ******************
 ********************************************************************************
 */
const	Led_ClipFormat			StandardMacPictureWithURLStyleMarker::kClipFormats[]		=	{
														kPICTClipFormat,
														StandardURLStyleMarker::kURLDClipFormat
								};
const	size_t					StandardMacPictureWithURLStyleMarker::kClipFormatCount	=	sizeof (kClipFormats)/sizeof(kClipFormats[0]);
const	Led_PrivateEmbeddingTag	StandardMacPictureWithURLStyleMarker::kEmbeddingTag		=	"Pict&URL";
const	Led_PrivateEmbeddingTag	StandardMacPictureWithURLStyleMarker::kOld1EmbeddingTag	=	"PictWEmbd";	// Only used in Led 2.0b7 - I believe - LGP 960427


StandardMacPictureWithURLStyleMarker::StandardMacPictureWithURLStyleMarker (const Led_Picture* pictData, size_t picSize, const Led_URLD& urlData):
	SimpleEmbeddedObjectStyleMarker (),
	fPictureHandle (NULL),
	#if		qWindows
	fPictureSize (0),
	#endif
	fURLData (urlData)
{
	Led_RequireNotNil (pictData);
	#if		qMacOS
		fPictureHandle = (StandardMacPictureStyleMarker::PictureHandle)Led_DoNewHandle (picSize);
	#elif	qWindows
		fPictureSize = picSize;
		fPictureHandle = ::GlobalAlloc (GMEM_MOVEABLE, picSize);
		Led_ThrowIfNull (fPictureHandle);
	#endif
	{
		Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
		memcpy (locker.GetPointer (), pictData, picSize);
	}
}

StandardMacPictureWithURLStyleMarker::~StandardMacPictureWithURLStyleMarker ()
{
	Led_AssertNotNil (fPictureHandle);
	#if		qMacOS
		::DisposeHandle (Handle (fPictureHandle));
	#elif	qWindows
		::GlobalFree (fPictureHandle);
	#endif
}

SimpleEmbeddedObjectStyleMarker*	StandardMacPictureWithURLStyleMarker::mk (const char* embeddingTag, const void* data, size_t len)
{
	Led_Require (memcmp (embeddingTag, kOld1EmbeddingTag, sizeof (kOld1EmbeddingTag)) == 0 or memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0);

	if (memcmp (embeddingTag, kOld1EmbeddingTag, sizeof (kOld1EmbeddingTag)) == 0) {
		Led_Picture*	picBuf	=	(Led_Picture*)data;
		size_t			picSize	=	Led_ByteSwapFromMac (picBuf->picSize);

		if (picSize >= len) {
			Led_ThrowBadFormatDataException ();
		}
		const char* url	=	((char*)data) + picSize;
		size_t		urlSize	=	len - picSize;
		Led_Assert (urlSize > 0);	// cuz of above throw-test above...

		return new StandardMacPictureWithURLStyleMarker (picBuf, Led_ByteSwapFromMac (picBuf->picSize), Led_URLD (url, urlSize));
	}
	else {
		if (len < 4 + 1 + sizeof (Led_Picture)) {
			Led_ThrowBadFormatDataException ();
		}

		size_t	picSize	=	*(size_t*)data;
		picSize = Led_BufToULONG (&picSize);

		Led_Picture*	picBuf	=	(Led_Picture*)((char*)data + 4);

		if (picSize + 4 >= len) {	// must leave room for ULRD.
			Led_ThrowBadFormatDataException ();
		}
		const char* url	=	((char*)picBuf) + picSize;
		size_t		urlSize	=	len - 4 - picSize;
		Led_Assert (urlSize > 0);	// cuz of above throw-test above...

		return new StandardMacPictureWithURLStyleMarker (picBuf, picSize, Led_URLD (url, urlSize));
	}
}

SimpleEmbeddedObjectStyleMarker*	StandardMacPictureWithURLStyleMarker::mk (ReaderFlavorPackage& flavorPackage)
{
	size_t	pictLength		=	flavorPackage.GetFlavorSize (kPICTClipFormat);
	Led_SmallStackBuffer<char> buf1 (pictLength);
	pictLength = flavorPackage.ReadFlavorData (kPICTClipFormat, pictLength, buf1);
	Led_Picture*	picBuf	=	(Led_Picture*)(char*)buf1;

	size_t	urlSize		=	flavorPackage.GetFlavorSize (StandardURLStyleMarker::kURLDClipFormat);
	Led_SmallStackBuffer<Led_tChar> buf2 (urlSize);
	urlSize = flavorPackage.ReadFlavorData (StandardURLStyleMarker::kURLDClipFormat, urlSize, buf2);
	return new StandardMacPictureWithURLStyleMarker (picBuf, pictLength, Led_URLD (buf2, urlSize));
}

void	StandardMacPictureWithURLStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text,
											const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										)
{
	Led_Assert (from + 1 == to);
	Led_Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	MacPictureDrawSegment (GetPictureHandle (), tablet,
								imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
								drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, Led_GetMacPictSize ((Led_Picture*)locker.GetPointer ())
							);
}

void	StandardMacPictureWithURLStyleMarker::MeasureSegmentWidth (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	Led_Assert (from + 1 == to);
	Led_RequireNotNil (text);
	/*
	 *	Though we generally require that:
	 *
	 *		Led_Require (text[0] == kEmbeddingSentinalChar);
	 *
	 *	we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
	 *	in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
	 *	when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
	 *	character here. We know the right width here anyhow.
	 *		See SPR#0821.
	 */
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	distanceResults[0] = Led_GetMacPictWidth ((Led_Picture*)locker.GetPointer ()) + 2*kDefaultEmbeddingMargin.h;
}

Led_Distance	StandardMacPictureWithURLStyleMarker::MeasureSegmentHeight (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to) const
{
	Led_Assert (from + 1 == to);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	return (Led_GetMacPictHeight ((Led_Picture*)locker.GetPointer ()) + 2*kDefaultEmbeddingMargin.v);
}

void	StandardMacPictureWithURLStyleMarker::Write (SinkStream& sink)
{
	{
		size_t	picSize	=	GetPictureByteSize ();
		Led_ULONGToBuf (picSize, &picSize);
		Led_Assert (sizeof (picSize) == 4);
		sink.write (&picSize, sizeof (picSize));
	}
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	sink.write ((Led_Picture*)locker.GetPointer (), GetPictureByteSize ());
	sink.write (fURLData.PeekAtURLD (), fURLData.GetURLDLength ());
}

void	StandardMacPictureWithURLStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) GetPictureHandle ());
	flavorPackage.AddFlavorData (StandardMacPictureStyleMarker::kClipFormat, GetPictureByteSize (), (Led_Picture*)locker.GetPointer ());
	flavorPackage.AddFlavorData (StandardURLStyleMarker::kURLDClipFormat, fURLData.GetURLDLength (), fURLData.PeekAtURLD ());
}

bool	StandardMacPictureWithURLStyleMarker::HandleOpen ()
{
	Led_URLManager::Get ().Open (fURLData.GetURL ());
	return false;	// indicate double click 'eaten'
}

vector<StandardMacPictureWithURLStyleMarker::PrivateCmdNumber>	StandardMacPictureWithURLStyleMarker::GetCmdNumbers () const
{
	vector<PrivateCmdNumber>	x;
	x.push_back (eOpenCmdNum);
	return x;
}

bool	StandardMacPictureWithURLStyleMarker::IsCmdEnabled (PrivateCmdNumber cmd) const
{
	switch (cmd) {
		case	eOpenCmdNum:	return true;
		default:				return inherited::IsCmdEnabled (cmd);
	}
}

const Led_URLD&	StandardMacPictureWithURLStyleMarker::GetURLData () const
{
	return fURLData;
}

void	StandardMacPictureWithURLStyleMarker::SetURLData (const Led_URLD& urlData)
{
	fURLData = urlData;
}

const char*	StandardMacPictureWithURLStyleMarker::GetTag () const
{
	return kEmbeddingTag;
}
#endif











/*
 ********************************************************************************
 **************************** StandardDIBWithURLStyleMarker *********************
 ********************************************************************************
 */
const	Led_ClipFormat			StandardDIBWithURLStyleMarker::kClipFormats[]		=	{
														StandardDIBStyleMarker::kClipFormat,
														StandardURLStyleMarker::kURLDClipFormat
								};
const	size_t					StandardDIBWithURLStyleMarker::kClipFormatCount	=	sizeof (kClipFormats)/sizeof(kClipFormats[0]);
const	Led_PrivateEmbeddingTag	StandardDIBWithURLStyleMarker::kEmbeddingTag	=	"DIB&URL";


StandardDIBWithURLStyleMarker::StandardDIBWithURLStyleMarker (const Led_DIB* dibData, const Led_URLD& urlData):
	SimpleEmbeddedObjectStyleMarker (),
	fDIBData (NULL),
	fURLData (urlData)
{
	#if		qMacOS
		Led_RequireNotNil (StandardDIBStyleMarker::sUnsupportedFormatPict);	// see class declaration for descriptio
	#endif
	Led_RequireNotNil (dibData);
	fDIBData = Led_CloneDIB (dibData);
}

StandardDIBWithURLStyleMarker::~StandardDIBWithURLStyleMarker ()
{
	Led_AssertNotNil (fDIBData);
	delete fDIBData;
}

SimpleEmbeddedObjectStyleMarker*	StandardDIBWithURLStyleMarker::mk (const char* embeddingTag, const void* data, size_t len)
{
	Led_Require (memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0);
	Led_Arg_Unused (embeddingTag);

	if (len < 4 + 40) {
		// This is less than we need to peek and see size of DIB...
		Led_ThrowBadFormatDataException ();
	}

	size_t	picSize	=	*(size_t*)data;
	picSize = Led_BufToULONG (&picSize);

	Led_DIB*	picBuf	=	(Led_DIB*)((char*)data + 4);

	if (len < picSize) {
		Led_ThrowBadFormatDataException ();
	}

	if (picSize + 4 >= len) {	// must leave room for ULRD.
		Led_ThrowBadFormatDataException ();
	}
	const char* url	=	((char*)picBuf) + picSize;
	size_t		urlSize	=	len - 4 - picSize;
	Led_Assert (urlSize > 0);	// cuz of above throw-test above...

	return new StandardDIBWithURLStyleMarker (picBuf, Led_URLD (url, urlSize));
}

SimpleEmbeddedObjectStyleMarker*	StandardDIBWithURLStyleMarker::mk (ReaderFlavorPackage& flavorPackage)
{
	size_t	length		=	flavorPackage.GetFlavorSize (StandardDIBStyleMarker::kClipFormat);
	Led_SmallStackBuffer<char> buf (length);
	length = flavorPackage.ReadFlavorData (StandardDIBStyleMarker::kClipFormat, length, buf);
	if (length < 40) {
		// This is less than we need to peek and see size of DIB...
		Led_ThrowBadFormatDataException ();
	}

	size_t	picSize	=	Led_GetDIBImageByteCount ((Led_DIB*)(char*)buf);
	// It appears that sometimes (at least on NT 4.0 beta) that the length of the flavor package is longer
	// than we expect. I'm guessing this is some optional additional information MS now passes along.
	// The picts APPEAR to come out looking fine if we ignore this stuff of the end. So continue doing so...
	if (length != picSize) {
		// Set a breakpoint here if this worries you...
		if (length < picSize) {
			// This is definitely bad!!!
			Led_ThrowBadFormatDataException ();
		}
		else {
			// we'll just ignore the stuff off the end... Hope thats OK - LGP 960429
		}
	}

	size_t	urlSize		=	flavorPackage.GetFlavorSize (StandardURLStyleMarker::kURLDClipFormat);
	Led_SmallStackBuffer<char> buf2 (urlSize);
	urlSize = flavorPackage.ReadFlavorData (StandardURLStyleMarker::kURLDClipFormat, urlSize, buf2);

	return new StandardDIBWithURLStyleMarker ((const Led_DIB*)(char*)buf, Led_URLD (buf2, urlSize));
}

void	StandardDIBWithURLStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text,
											const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										)
{
	Led_Assert (from + 1 == to);
	Led_Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);
	DIBDrawSegment (fDIBData, tablet, 
						imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
						drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, Led_GetDIBImageSize (fDIBData)
					);
}

void	StandardDIBWithURLStyleMarker::MeasureSegmentWidth (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	Led_Assert (from + 1 == to);
	Led_RequireNotNil (text);
	/*
	 *	Though we generally require that:
	 *
	 *		Led_Require (text[0] == kEmbeddingSentinalChar);
	 *
	 *	we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
	 *	in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
	 *	when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
	 *	character here. We know the right width here anyhow.
	 *		See SPR#0821.
	 */
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);
	distanceResults[0] = Led_GetDIBImageSize (GetDIBData ()).h + 2*kDefaultEmbeddingMargin.h;
}

Led_Distance	StandardDIBWithURLStyleMarker::MeasureSegmentHeight (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to) const
{
	Led_Assert (from + 1 == to);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	return (Led_GetDIBImageSize (GetDIBData ()).v + 2*kDefaultEmbeddingMargin.v);
}

void	StandardDIBWithURLStyleMarker::Write (SinkStream& sink)
{
	const Led_DIB*	dib	=	GetDIBData ();
	{
		size_t	dibSize	=	Led_GetDIBImageByteCount (dib);
		Led_ULONGToBuf (dibSize, &dibSize);
		Led_Assert (sizeof (dibSize) == 4);
		sink.write (&dibSize, sizeof (dibSize));
	}
	sink.write (dib, Led_GetDIBImageByteCount (dib));
	sink.write (fURLData.PeekAtURLD (), fURLData.GetURLDLength ());
}

void	StandardDIBWithURLStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
	const Led_DIB*	dib	=	GetDIBData ();
	size_t	dibSize	=	Led_GetDIBImageByteCount (dib);
	flavorPackage.AddFlavorData (StandardDIBStyleMarker::kClipFormat, dibSize, dib);
	flavorPackage.AddFlavorData (StandardURLStyleMarker::kURLDClipFormat, fURLData.GetURLDLength (), fURLData.PeekAtURLD ());
}

bool	StandardDIBWithURLStyleMarker::HandleOpen ()
{
	Led_URLManager::Get ().Open (fURLData.GetURL ());
	return false;	// indicate double click 'eaten'
}

vector<StandardDIBWithURLStyleMarker::PrivateCmdNumber>	StandardDIBWithURLStyleMarker::GetCmdNumbers () const
{
	vector<PrivateCmdNumber>	x;
	x.push_back (eOpenCmdNum);
	return x;
}

bool	StandardDIBWithURLStyleMarker::IsCmdEnabled (PrivateCmdNumber cmd) const
{
	switch (cmd) {
		case	eOpenCmdNum:	return true;
		default:				return inherited::IsCmdEnabled (cmd);
	}
}

const Led_URLD&	StandardDIBWithURLStyleMarker::GetURLData () const
{
	return fURLData;
}

void	StandardDIBWithURLStyleMarker::SetURLData (const Led_URLD& urlData)
{
	fURLData = urlData;
}

const char*	StandardDIBWithURLStyleMarker::GetTag () const
{
	return kEmbeddingTag;
}










/*
 ********************************************************************************
 ************************* StandardUnknownTypeStyleMarker ***********************
 ********************************************************************************
 */
#if		qMacOS
Led_Picture**					StandardUnknownTypeStyleMarker::sUnknownPict			=	NULL;
#elif	qWindows
const Led_DIB*					StandardUnknownTypeStyleMarker::sUnknownPict			=	NULL;
#endif
const	Led_PrivateEmbeddingTag	StandardUnknownTypeStyleMarker::kDefaultEmbeddingTag	=	"UnknwnDlf";


StandardUnknownTypeStyleMarker::StandardUnknownTypeStyleMarker (Led_ClipFormat format, const char* embeddingTag, const void* unknownTypeData, size_t nBytes, const Led_DIB* dib):
	SimpleEmbeddedObjectStyleMarker (),
//	fShownSize (Led_TWIPS_Point (Led_TWIPS (0), Led_TWIPS (0))),
	fShownSize (),
	fData (NULL),
	fLength (nBytes),
	fFormat (format),
	//fEmbeddingTag (),
	fDisplayDIB ()
{
	memcpy (fEmbeddingTag, embeddingTag, sizeof (fEmbeddingTag));
	#if		qMacOS || qWindows
		Led_RequireNotNil (sUnknownPict);	// If this is ever triggered, see class declaration where we delcare this field
	#endif
	fData = new char [nBytes];
	memcpy (fData, unknownTypeData, nBytes);

	if (dib != NULL) {
		#if		qCannotAssignRValueAutoPtrToExistingOneInOneStepBug || qTroubleOverloadingXofXRefCTORWithTemplatedMemberCTOR
			auto_ptr<Led_DIB> x = auto_ptr<Led_DIB> (Led_CloneDIB (dib));
			fDisplayDIB = x;
		#else
 			fDisplayDIB = auto_ptr<Led_DIB> (Led_CloneDIB (dib));
		#endif
	}
	fShownSize = CalcDefaultShownSize ();
}

StandardUnknownTypeStyleMarker::~StandardUnknownTypeStyleMarker ()
{
	delete[] (char*)fData;
}

/*
@METHOD:		StandardUnknownTypeStyleMarker::SetShownSize
@DESCRIPTION:	<p>See @'StandardUnknownTypeStyleMarker::GetShownSize', or
			@'StandardUnknownTypeStyleMarker::CalcDefaultShownSize'.</p>
				<p>NB: it is the CALLERs responsability to assure the appropriate
			TextInteractors/TextImagers are notified to adjust any caching of size information they may have. This can be avoided
			by setting this value BEFORE adding the embedding to the TextStore.</p>
*/
void	StandardUnknownTypeStyleMarker::SetShownSize (Led_TWIPS_Point size)
{
	fShownSize = size;
}

/*
@METHOD:		StandardUnknownTypeStyleMarker::CalcDefaultShownSize
@DESCRIPTION:	<p>See @'StandardUnknownTypeStyleMarker::GetShownSize'.</p>
				<p>Generates a reasonable default size (based on sUnknownPict size) for the embedding. Used unless
			overridden by calls to @'StandardUnknownTypeStyleMarker::SetShownSize'.</p>
*/
Led_TWIPS_Point	StandardUnknownTypeStyleMarker::CalcDefaultShownSize ()
{
	if (fDisplayDIB.get () != NULL) {
		Led_Size	pixelSize	=	 Led_GetDIBImageSize (fDisplayDIB.get ());
		return Led_TWIPS_Point (Led_CvtScreenPixelsToTWIPSV (pixelSize.v), Led_CvtScreenPixelsToTWIPSH (pixelSize.h));
	}
	return CalcStaticDefaultShownSize ();
}

Led_TWIPS_Point	StandardUnknownTypeStyleMarker::CalcStaticDefaultShownSize ()
{
	#if		qMacOS
		Led_RequireNotNil (sUnknownPict);
		Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle) sUnknownPict);
		Led_Size	pixelSize	=	 Led_GetMacPictSize (sUnknownPict);
	#elif	qWindows
		Led_RequireNotNil (sUnknownPict);
		Led_Size	pixelSize	=	 Led_GetDIBImageSize (sUnknownPict);
	#elif	qXWindows
		Led_Size	pixelSize	=	Led_Size (10,10);	//  X-TMP-HACK-LGP2000-06-13
	#endif

	return Led_TWIPS_Point (Led_CvtScreenPixelsToTWIPSV (pixelSize.v), Led_CvtScreenPixelsToTWIPSH (pixelSize.h));
}

void	StandardUnknownTypeStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text,
											const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										)
{
	Led_Assert (from + 1 == to);
	Led_Require (text.PeekAtVirtualText () [0] == kEmbeddingSentinalChar);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);
	Led_Size	shownPixelSize	=	Led_Size (tablet->CvtFromTWIPSV (fShownSize.v), tablet->CvtFromTWIPSH (fShownSize.h));;
	if (fDisplayDIB.get () != NULL) {
		DIBDrawSegment (fDisplayDIB.get (), tablet,
							imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
							drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, shownPixelSize
						);
		return;
	}
	#if		qMacOS
		MacPictureDrawSegment (sUnknownPict, tablet,
									imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
									drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, shownPixelSize
								);
	#elif	qWindows
		DIBDrawSegment (sUnknownPict, tablet,
								imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor), imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor),
								drawInto - Led_Point (0, imager->GetHScrollPos ()), useBaseLine, pixelsDrawn, shownPixelSize
						);
	#endif
}

void	StandardUnknownTypeStyleMarker::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	Led_Assert (from + 1 == to);
	Led_RequireNotNil (text);
	/*
	 *	Though we generally require that:
	 *
	 *		Led_Require (text[0] == kEmbeddingSentinalChar);
	 *
	 *	we cannot here - cuz  we provent assure we are a one-length marker surrouding a sentinal
	 *	in SimpleEmbeddedObjectStyleMarker::DidUpdateText - which may not have yet been called
	 *	when THIS is called (cuz some other update handlers may force it. No big deal. We just ignore that
	 *	character here. We know the right width here anyhow.
	 *		See SPR#0821.
	 */
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);
	Led_Arg_Unused (text);
	if (fDisplayDIB.get () != NULL) {
		distanceResults[0] = Led_GetDIBImageSize (fDisplayDIB.get ()).h + 2*kDefaultEmbeddingMargin.h;
		return;
	}
	TextInteractor::Tablet_Acquirer	tablet_ (imager);
	Led_Tablet						tablet		=	tablet_;
	distanceResults[0] = tablet->CvtFromTWIPSH (fShownSize.h) + 2*kDefaultEmbeddingMargin.h;
}

Led_Distance	StandardUnknownTypeStyleMarker::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to) const
{
	Led_Assert (from + 1 == to);
	Led_Arg_Unused (from);
	Led_Arg_Unused (to);

	if (fDisplayDIB.get () != NULL) {
		return (Led_GetDIBImageSize (fDisplayDIB.get ()).v + 2*kDefaultEmbeddingMargin.v);
	}
	TextInteractor::Tablet_Acquirer	tablet_ (imager);
	Led_Tablet						tablet		=	tablet_;
	return tablet->CvtFromTWIPSV (fShownSize.v) + 2*kDefaultEmbeddingMargin.v;
}

void	StandardUnknownTypeStyleMarker::Write (SinkStream& sink)
{
	sink.write (fData, fLength);
}

void	StandardUnknownTypeStyleMarker::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
	flavorPackage.AddFlavorData (fFormat, fLength, fData);
}

const char*	StandardUnknownTypeStyleMarker::GetTag () const
{
	return fEmbeddingTag;
}







/*
 ********************************************************************************
 ************************ InsertEmbeddingForExistingSentinal ********************
 ********************************************************************************
 */
void	InsertEmbeddingForExistingSentinal (SimpleEmbeddedObjectStyleMarker* embedding, TextStore& textStore, size_t insertAt, MarkerOwner* ownerForEmbedding)
{
	Led_RequireNotNil (embedding);
	Led_RequireNotNil (ownerForEmbedding);
	TextStore::SimpleUpdater	updater (textStore, insertAt, insertAt + 1);
	textStore.AddMarker (embedding, insertAt, 1, ownerForEmbedding);
}



/*
 ********************************************************************************
 ********************************* AddEmbedding *********************************
 ********************************************************************************
 */
void	AddEmbedding (SimpleEmbeddedObjectStyleMarker* embedding, TextStore& textStore, size_t insertAt, MarkerOwner* ownerForEmbedding)
{
	Led_RequireNotNil (embedding);
	Led_RequireNotNil (ownerForEmbedding);
	textStore.Replace (insertAt, insertAt, &kEmbeddingSentinalChar, 1);
	InsertEmbeddingForExistingSentinal (embedding, textStore, insertAt, ownerForEmbedding);
}







#if		qMacOS || qWindows
/*
 ********************************************************************************
 ************************** MacPictureDrawSegment *******************************
 ********************************************************************************
 */
static	void	MacPictureDrawSegment (StandardMacPictureStyleMarker::PictureHandle pictureHandle,
										Led_Tablet tablet, Led_Color foreColor, Led_Color backColor, const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn,
										const Led_Size& imageSize,
										const Led_Size& margin
									) throw ()
{
	Led_RequireNotNil (pictureHandle);

	Led_StackBasedHandleLocker	locker ((Led_StackBasedHandleLocker::GenericHandle)pictureHandle);

	#if		qMacOS
		tablet->SetPort ();
	#elif	qWindows
		Led_Tablet	dc	=	tablet;
	#endif

	Led_Size	pictSize	=	imageSize;

	Led_Rect	ourBoundsRect = drawInto;
	ourBoundsRect.SetRight (ourBoundsRect.GetLeft () + pictSize.h + 2*margin.h);
	Led_Coordinate	embedBottom	=	useBaseLine;
	Led_Coordinate	embedTop	=	embedBottom - pictSize.v;
	Led_Assert (embedTop >= drawInto.top);
	Led_Assert (embedBottom <= drawInto.bottom);
	Led_Rect	innerBoundsRect = Led_Rect (Led_Point (embedTop, drawInto.GetLeft () + margin.h), pictSize);


	#if		qMacOS
		GDI_RGBForeColor (foreColor.GetOSRep ());
		GDI_RGBBackColor (backColor.GetOSRep ());
	#elif	qWindows
		dc->SetTextColor (foreColor.GetOSRep ());
		dc->SetBkColor (backColor.GetOSRep ());
	#endif

	#if		qMacOS
		// Must erase above the picture, and below it. And 
		Rect	rr	=	AsQDRect (innerBoundsRect);

		// Now draw the actual picture
		::DrawPicture (pictureHandle, &rr);
	#elif	qWindows && qUseQuicktimeForWindows
		// I tried doing a LoadLibrary/GetProcAddress ("DrawPicture") so that I wouldn't need
		// to link directly against the QT library, and so I wouldn't get errors at start time
		// if the lib wasn't present. But apparantly Apple has done all that for me in the
		// staticly linked library they provide. I get no errors when I run on a system that
		// doesn't have QT installed til the call to QTInitialize(). And staticly linking
		// only adds 3K - presumably just the code which maps the APIs to the DLLs, and does
		// the checking to find the DLL.
		//
		// So we simply staticly link, and only call QTInitialize() as needed. And then call
		// QTTerminate at exit (via static DTOR hack), as needed.
		struct	QTIniter {
			QTIniter ()
				{
					fGood = (::QTInitialize (NULL) == 0);
				}
			~QTIniter ()
				{
					if (fGood) {
						::QTTerminate ();
					}
				}
			bool	fGood;
		};
		static	QTIniter	sIniter;
		RECT	rr	=	AsRECT (innerBoundsRect);
		Led_Brush	eraseBrush (backColor.GetOSRep ());
		Led_Win_Obj_Selector	brush (dc, eraseBrush);
		bool	displaySuccessful	=	false;
		if (sIniter.fGood) {
			displaySuccessful = (::DrawPicture (dc->m_hDC, (PicHandle)pictureHandle, &rr, NULL) == noErr);
		}
		if (not displaySuccessful) {
			// fill in with some other picture...
			// treat all excpetions the same. In principle, could draw different picst for memory and
			// unsupported format exceptions...
			const Led_DIB*			dib	=	StandardMacPictureStyleMarker::sUnsupportedFormatPict;
			Led_AssertNotNil (dib);
			Led_Size				dibImageSize	=	Led_GetDIBImageSize (dib);
	//		const BITMAPINFOHEADER&	hdr			=	dib->bmiHeader;
			const void*				lpBits		=	Led_GetDIBBitsPointer (dib);
 	//		const char*				lpBits		=	((const char*)dib) + Led_ByteSwapFromWindows (hdr.biSize) + Led_GetDIBPalletByteCount (dib);
			::StretchDIBits (dc->m_hDC, innerBoundsRect.left, innerBoundsRect.top, innerBoundsRect.GetWidth (), innerBoundsRect.GetHeight (), 0, 0, dibImageSize.h, dibImageSize.v, lpBits, dib, DIB_RGB_COLORS, SRCCOPY);
		}
	#endif
	if (pixelsDrawn != NULL) {
		*pixelsDrawn = ourBoundsRect.GetWidth ();
	}
}
#endif

static	void	DIBDrawSegment (const Led_DIB* dib,
								Led_Tablet tablet,
								Led_Color foreColor, Led_Color backColor,
								const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn,
								const Led_Size& imageSize,
								const Led_Size& margin
							) throw ()
{
	Led_RequireNotNil (dib);
	Led_RequireNotNil (tablet);
	Led_Arg_Unused (foreColor);
	Led_Arg_Unused (backColor);

	#if		qMacOS
		tablet->SetPort ();
	#elif	qWindows
		Led_Tablet	dc	=	tablet;
	#endif

	Led_Size	dibImageSize	=	imageSize;

	Led_Rect	ourBoundsRect = drawInto;
	ourBoundsRect.right = ourBoundsRect.left + dibImageSize.h + 2*margin.h;
	Led_Coordinate	embedBottom	=	useBaseLine;
	Led_Coordinate	embedTop	=	embedBottom - dibImageSize.v;
	Led_Assert (embedTop >= drawInto.GetTop ());
	Led_Assert (embedBottom <= drawInto.GetBottom ());
	Led_Rect	innerBoundsRect = Led_Rect (Led_Point (embedTop, drawInto.GetLeft () + margin.h), dibImageSize);

	if (pixelsDrawn != NULL) {
		*pixelsDrawn = ourBoundsRect.GetWidth ();
	}

	#if		qMacOS
		#if 1
			GDI_RGBForeColor (Led_Color::kBlack.GetOSRep ());
			GDI_RGBBackColor (Led_Color::kWhite.GetOSRep ());
		#else
			GDI_RGBForeColor (foreColor.GetOSRep ());
			GDI_RGBBackColor (backColor.GetOSRep ());
		#endif
	#elif	qWindows
		dc->SetTextColor (foreColor.GetOSRep ());
		dc->SetBkColor (backColor.GetOSRep ());
	#endif

	#if		qMacOS
		// Must erase above the picture, and below it. And 
		Rect	rr	=	AsQDRect (innerBoundsRect);

		// Turn the DIB into a pixmap, and then image it, and free it again...
		try {
			PixMap**	pm			=	MakePixMapFromDIB (dib);
			PixMap*		pmPtr		=	*pm;
			GrafPtr		destPort	=	*tablet;
			#if		TARGET_CARBON
				::CopyBits (reinterpret_cast<BitMap*> (pmPtr), GetPortBitMapForCopyBits (destPort), &pmPtr->bounds, &rr, srcCopy, NULL);
			#else
				::CopyBits (reinterpret_cast<BitMap*> (pmPtr), &destPort->portBits, &pmPtr->bounds, &rr, srcCopy, NULL);
			#endif
			delete[] (char*)pmPtr->baseAddr;
			::DisposePixMap (pm);
		}
		catch (...) {
			// treat all excpetions the same. In principle, could draw different picst for memory and
			// unsupported format exceptions...
			Led_AssertNotNil (StandardDIBStyleMarker::sUnsupportedFormatPict);
			::DrawPicture (StandardDIBStyleMarker::sUnsupportedFormatPict, &rr);
		}
	#elif	qWindows
		//const BITMAPINFOHEADER&	hdr			=	dib->bmiHeader;
		const void*	lpBits		=	Led_GetDIBBitsPointer (dib);
		//const char*				lpBits		=	((const char*)dib) + Led_ByteSwapFromWindows (hdr.biSize) + Led_GetDIBPalletByteCount (dib);
		::StretchDIBits (dc->m_hDC, innerBoundsRect.left, innerBoundsRect.top, innerBoundsRect.GetWidth (), innerBoundsRect.GetHeight (), 0, 0, dibImageSize.h, dibImageSize.v, lpBits, dib, DIB_RGB_COLORS, SRCCOPY);
	#endif
}

#if		qMacOS
static	PixMap**	MakePixMapFromDIB (const Led_DIB* dib)
{
	Led_RequireNotNil (dib);

	Led_Size	dibImageSize	=	Led_GetDIBImageSize (dib);

	const BITMAPINFOHEADER&	hdr				=	dib->bmiHeader;
	const RGBQUAD*			srcCLUT			=	(const RGBQUAD*)(((const unsigned char*)dib) + Led_ByteSwapFromWindows (hdr.biSize));
	const unsigned char*	srcBits			=	((const unsigned char*)srcCLUT) + Led_GetDIBPalletByteCount (dib);
	unsigned short			bitCount		=	Led_ByteSwapFromWindows (hdr.biBitCount);
	size_t					srcRowBytes		=	(((dibImageSize.h * bitCount + 31) & ~31) >> 3);

	if (bitCount != 8 and bitCount != 24) {	// only supported sizes, for now...
											// LGP 960430
		throw UnsupportedFormat ();
	}

	if (hdr.biCompression != 0) {			// unsupported for now, thought here is DECODE code on the 'Encyclopedia of Graphix Formats' CD
											// LGP 960430
		throw UnsupportedFormat ();
	}

	if (bitCount > 8) {
		srcCLUT = 0;
	}

	// make sure CLUT looks good...
	if (srcCLUT != NULL) {
		size_t	nColors	=	Led_ByteSwapFromWindows (hdr.biClrUsed);
		if (nColors > (1 << bitCount)) {
			throw UnsupportedFormat ();		// really bad format, probably...
		}
	}

	size_t			dstBitCount	=	bitCount;
	if (bitCount == 24) {
		dstBitCount = 32;
	}
	size_t			dstRowBytes		=	((((dstBitCount * dibImageSize.h) + 15) >> 4) << 1);
	unsigned char*	newImageData	=	new unsigned char [dstRowBytes*dibImageSize.v];
	Led_AssertNotNil (newImageData);

	PixMap**	result	=	::NewPixMap ();
	if (result == NULL) {
		delete[] (char*)newImageData;
		Led_ThrowOutOfMemoryException ();
	}
	(*result)->bounds.top = 0;
	(*result)->bounds.left = 0;
	(*result)->bounds.bottom = dibImageSize.v;
	(*result)->bounds.right = dibImageSize.h;

	(*result)->baseAddr = Ptr (newImageData);

	switch (bitCount) {
		case	8: {
			(*result)->rowBytes = 0x8000 | dstRowBytes;
			(*result)->cmpCount = 1;
			(*result)->cmpSize = 8;
			(*result)->pixelType = chunky;
			(*result)->pixelSize = 8;
		}
		break;

		case	24: {
			(*result)->rowBytes = 0x8000 | dstRowBytes;
			(*result)->cmpCount = 3;
			(*result)->cmpSize = 8;
			(*result)->pixelType = RGBDirect;
			(*result)->pixelSize = 32;
		}
		break;

		default: {
			Led_Assert (false);	// not supported - should have punted above!
		}
		break;
	}

	/*
	 *	Copy the CLUT data.
	 */
	if (srcCLUT != NULL) {// 'if' so support 24-bit and no CLUT!!!
		size_t	nColors	=	Led_ByteSwapFromWindows (hdr.biClrUsed);
		if (nColors == 0) {
			nColors	=	1 << bitCount;
		}
		Led_Assert (nColors <= (1 << bitCount));

		CTabHandle	newCLUT	=	(CTabHandle)::NewHandle (sizeof (ColorTable) + (nColors-1)*sizeof (ColorSpec));
		if (newCLUT == NULL) {
			delete[] (char*)newImageData;
			::DisposePixMap (result);
			Led_ThrowOutOfMemoryException ();
		}
		(*newCLUT)->ctSeed = ::GetCTSeed ();
		(*newCLUT)->ctFlags = 0;
		(*newCLUT)->ctSize = nColors-1;
		for (size_t i = 0; i < nColors; i++) {
			(*newCLUT)->ctTable[i].value = i;
			(*newCLUT)->ctTable[i].rgb.red = srcCLUT[i].rgbRed<<8;
			(*newCLUT)->ctTable[i].rgb.green = srcCLUT[i].rgbGreen<<8;
			(*newCLUT)->ctTable[i].rgb.blue = srcCLUT[i].rgbBlue<<8;
		}
		if ((*result)->pmTable != NULL) {
			::DisposeCTable ((*result)->pmTable);
		}
		(*result)->pmTable = newCLUT;
		::CTabChanged (newCLUT);
	}


	/*
	 *	Copy the PixMap data.
	 */
	bool	rowsReversed	=	(Led_ByteSwapFromWindows (hdr.biHeight) > 0);
	for (size_t row = 0; row < dibImageSize.v; row++) {
		const unsigned char*	srcRow	=	srcBits + (rowsReversed? (dibImageSize.v-row-1): row)*srcRowBytes;
		unsigned char*			dstRow	=	newImageData + row*dstRowBytes;

		switch (bitCount) {
			case	8: {
				// we use the same CLUT, so this should be OK
				memcpy (dstRow, srcRow, Led_Min (srcRowBytes, dstRowBytes));
			}
			break;
			
			case	24: {
				for (size_t col = 0; col < dibImageSize.h; col++) {
					const unsigned char*	srcCell	=	srcRow + 3*col;
					unsigned char*			dstCell	=	dstRow + 4*col;
					unsigned char	blueComp	=	*srcCell++;
					unsigned char	greenComp	=	*srcCell++;
					unsigned char	redComp		=	*srcCell++;
					*dstCell++ = 0;
					*dstCell++ = redComp;
					*dstCell++ = greenComp;
					*dstCell++ = blueComp;
				}
			}
			break;

			default: {
				// too bad, we don't support that size - just zero out the memory...
				memset (dstRow, 0, dstRowBytes);
			}
			break;
		}
	}

	return result;
}
#endif




#if		qLedUsesNamespaces
}
#endif



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


