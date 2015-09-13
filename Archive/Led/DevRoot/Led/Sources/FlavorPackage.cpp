/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/FlavorPackage.cpp,v 2.31 2004/02/10 17:09:10 lewis Exp $
 *
 * Changes:
 *	$Log: FlavorPackage.cpp,v $
 *	Revision 2.31  2004/02/10 17:09:10  lewis
 *	cleanup compiler (argsused) warnings
 *	
 *	Revision 2.30  2003/09/22 22:06:12  lewis
 *	rename kLedRTFClipType to ClipFormat (& likewise for LedPrivate/HTML). Add FlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromStartOfData (and subclass override to check RTF/HTML etc file fomrats) when were given no good file suffix etc and must peek at file data to guess type. Allow NULL SinkStream to CTOR for a Reader - since we could be just calling QuickLookAppearsToBeRightFormat () which doesnt need an output stream. All for SPR#1552 - so we use same code in OpenDoc code as in when you drop a file on an open Led window (and so we handle read-only files etc better)
 *	
 *	Revision 2.29  2003/09/22 20:20:57  lewis
 *	for SPR#1552: add StyledTextFlavorPackageInternalizer::InternalizeFlavor_HTML. Lose StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEDataRawBytes and replace with StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromName (and base class code to call InternalizeBestFlavor with new suggestedClipType). Moved kRTFClipType from FlavorPackage module to StandardStyledTextInteractor module - since not needed in FlavorPackage code.
 *	
 *	Revision 2.28  2003/09/22 15:46:52  lewis
 *	Added InternalizeFlavor_FILEGuessFormatsFromName().  Changed args to InternalizeFlavor_FILEDataRawBytes and InternalizeFlavor_FILEData (). Much better implemenation - now using FileReader class. Still a bit more to go on this - but now we use same code in FILEIO part of LedLineIt as we have here for internalize for opening files. must do likewise for LedIt (so D&D of file operation works better). Done as part of bug#1552
 *	
 *	Revision 2.27  2003/02/28 19:03:23  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.26  2002/05/06 21:33:42  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.25  2001/11/27 00:29:49  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.24  2001/10/17 20:42:57  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.23  2001/09/26 15:41:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.22  2001/09/12 17:23:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.21  2001/09/11 12:26:42  lewis
 *	SPR#1022- add BOM flag (&CTOR overload) to CodePageConverter class to fix problems
 *	with BOM handling - esp UTF7/8
 *	
 *	Revision 2.20  2001/09/09 23:05:53  lewis
 *	SPR#1021- Fix FlavorPackageInternalizer::InternalizeFlavor_FILEDataRawBytes () to better handle
 *	BOM / UNICODE files (and autodetecting code pages etc)
 *	
 *	Revision 2.19  2001/09/05 16:24:14  lewis
 *	SPR#1016- cleanup semantics of ReaderFlavorPackage::GetFlavorSize/ReaderFlavorPackage::ReadFlavorData
 *	
 *	Revision 2.18  2001/08/28 19:08:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.17  2001/08/28 18:43:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.16  2001/07/19 02:21:46  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.15  2001/05/25 21:09:22  lewis
 *	SPR#0899- Support RTF clipboard type, and much more (related). Significant
 *	reorg of clipboard (Gtk-specific) support. Now uses more subeventloops 
 *	(now that I discovered that I can).
 *	
 *	Revision 2.14  2001/05/08 21:41:04  lewis
 *	SPR#0890- related - get basic XWindows support in for internalize of file obejcts
 *	
 *	Revision 2.13  2001/05/03 20:56:01  lewis
 *	SPR#0894- added clipboard support for X-Windows - by HERE creating a sPrivateClipData
 *	field. Then - in Led_Gtk<> template - that is munged and read and used to
 *	communicate with the XSelection mechanism
 *	
 *	Revision 2.12  2001/01/05 16:06:06  lewis
 *	Minor Borland tweek
 *	
 *	Revision 2.11  2001/01/04 14:27:50  lewis
 *	a few small changes to be compatable with an older version of Borland C++-
 *	requested by a customer - he proovded the patches- Cladio Nold
 *	
 *	Revision 2.10  2000/11/17 17:00:57  lewis
 *	SPR#0847- use &* to workaround bug in particular version of STL used by one customer (Remo)
 *	
 *	Revision 2.9  2000/07/09 11:38:50  lewis
 *	fix typo so compiles on NONUNICODE
 *	
 *	Revision 2.8  2000/06/23 21:43:12  lewis
 *	Broke InternalizeFlavor_FILEData into two - that routine and InternalizeFlavor_FILEDataRawBytes -
 *	so easier to subclass just the one part. And if (guessedClipFormat == kBadClipFormat)
 *	do a better job of guessing file format (file suffix)
 *	
 *	Revision 2.7  2000/04/26 21:08:57  lewis
 *	check error results on ::SetClipboardData() - and pass GMEM_MOVEABLE flag in
 *	WriterClipboardFlavorPackage::AddFlavorData GlobalAlloc () call -
 *	cuz latest docs say to (for SetClipboardData()
 *	
 *	Revision 2.6  2000/04/25 19:00:57  lewis
 *	added kLedPrivateClipType/kRTFClipType. And made GetFlavorSize/ReadFlavorData() CONST
 *	
 *	Revision 2.5  2000/04/25 16:10:07  lewis
 *	fix ReadWriteFlavorPackage code - I now see why I had the private COPY CTOR etc.
 *	It was buggy in that case - but trivial to fix. Just use vector<> for data rather than void* data.
 *	
 *	Revision 2.4  2000/04/24 16:47:03  lewis
 *	Very substantial change to FlavorPackage stuff - now NOT mixed into TextInteractor -
 *	but simply owned using Led_RefCntPtr<>. Also - did all the proper subclassing in WP
 *	class etc to make this work (almost - still must fix OLE embeddings) as before. SPR# 0742
 *	
 *	Revision 2.3  2000/04/16 14:39:36  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.2  2000/04/14 22:40:24  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.1  2000/04/14 19:12:20  lewis
 *	SPR#0739- move code from TextInteractor to this separate module- and cleanup a bit
 *	
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"LedConfig.h"

#if		qMacOS
	#include	<Drag.h>
	#include	<Errors.h>
	#include	<Scrap.h>
#elif	qWindows
	#include	<fcntl.h>
	#include	<io.h>
#elif	qXWindows
	#include	<sys/types.h>
	#include	<sys/stat.h>
	#include	<fcntl.h>
	#include	<unistd.h>
#endif


#include	"CodePage.h"
#include	"Marker.h"
#include	"TextStore.h"

#include	"FlavorPackage.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif





#if		qLedUsesNamespaces
	namespace	Led {
#endif






/*
 ********************************************************************************
 *************************** FlavorPackageExternalizer **************************
 ********************************************************************************
 */

TextStore*	FlavorPackageExternalizer::PeekAtTextStore () const
{
	return &fTextStore;
}

/*
@METHOD:		FlavorPackageExternalizer::ExternalizeFlavors
@DESCRIPTION:	<p>Externalize to the given @'FlavorPackageExternalizer::WriterFlavorPackage' the text in the
	given range. This can be for a Drag&Drop package, a ClipBoard package, or whatever
	(see @'FlavorPackageExternalizer::ExternalizeBestFlavor').</p>
*/
void	FlavorPackageExternalizer::ExternalizeFlavors (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
	ExternalizeFlavor_TEXT (flavorPackage, from, to);
}

/*
@METHOD:		FlavorPackageExternalizer::ExternalizeBestFlavor
@DESCRIPTION:	<p>Externalize to the given @'FlavorPackageExternalizer::WriterFlavorPackage' the text in the
	given range. This can be for a Drag&Drop package, a ClipBoard package, or whatever
	(see @'FlavorPackageExternalizer::ExternalizeFlavors').</p>
*/
void	FlavorPackageExternalizer::ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
	ExternalizeFlavor_TEXT (flavorPackage, from, to);
}

/*
@METHOD:		FlavorPackageExternalizer::ExternalizeFlavor_TEXT
@DESCRIPTION:	<p>Externalize to the given @'FlavorPackageExternalizer::WriterFlavorPackage' the text in the
	given range. This can be for a Drag&Drop package, a ClipBoard package, or whatever
	(see @'FlavorPackageExternalizer::ExternalizeFlavors' and @'FlavorPackageExternalizer::ExternalizeBestFlavor').</p>
		<p>This method externalizes in the native OS text format (with any embedded NUL-characters
	in the text eliminated).</p>
*/
void	FlavorPackageExternalizer::ExternalizeFlavor_TEXT (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
	size_t	start	=	from;
	size_t	end		=	to;
	Led_Require (start >= 0);
	Led_Require (end <= GetTextStore ().GetEnd ());
	Led_Require (start <= end);
	size_t	length	=	end - start;
	#if		qMacOS || qXWindows
		Led_SmallStackBuffer<Led_tChar> buf (length);
	#elif	qWindows
		Led_SmallStackBuffer<Led_tChar> buf (2*length+1);
	#endif
	if (length != 0) {
		Led_SmallStackBuffer<Led_tChar> buf2 (length);
		GetTextStore ().CopyOut (start, length, buf2);
		#if		qMacOS || qXWindows
			length = Led_NLToNative (buf2, length, buf, length);
		#elif	qWindows
			length = Led_NLToNative (buf2, length, buf, 2*length + 1);
		#endif
	}

	// At least for MS-Windows - NUL-chars in the middle of the text just cause confusion in other
	// apps. Don't know about Mac. But I doubt they help any there either. Since for this stuff
	// we aren't going todo the right thing for sentinals ANYHOW - we may as well eliminate them
	// (or any other NUL-chars)
	length = Led_SkrunchOutSpecialChars (buf, length, '\0');

	#if		qWindows
		buf[length] = '\0';		// Windows always expects CF_TEXT to be NUL char terminated
		length++;				// so AddFlavorData() writes out the NUL-byte
	#endif
	flavorPackage.AddFlavorData (kTEXTClipFormat, length * sizeof (Led_tChar), buf);

	#if		qWideCharacters && qWorkAroundWin95BrokenUNICODESupport
		Led_SmallStackBuffer<char>	bufwa (length * sizeof (Led_tChar));
		size_t	nextPackageSize	=	::WideCharToMultiByte (GetACP (), 0, buf, length, bufwa, length * sizeof (Led_tChar), NULL, NULL);
		flavorPackage.AddFlavorData (CF_TEXT, nextPackageSize, bufwa);
	#endif
}


















/*
 ********************************************************************************
 *************************** FlavorPackageInternalizer **************************
 ********************************************************************************
 */
TextStore*	FlavorPackageInternalizer::PeekAtTextStore () const
{
	return &fTextStore;
}

bool	FlavorPackageInternalizer::InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
	if (InternalizeFlavor_FILE (flavorPackage, from, to)) {
		return true;
	}
	else if (InternalizeFlavor_TEXT (flavorPackage, from, to)) {
		return true;
	}
	return false;
}

bool	FlavorPackageInternalizer::InternalizeFlavor_TEXT (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
	if (flavorPackage.GetFlavorAvailable_TEXT ()) {
		size_t			length		=	flavorPackage.GetFlavorSize (kTEXTClipFormat);
		Led_ClipFormat	textFormat	=	kTEXTClipFormat;
		#if		qWideCharacters && qWorkAroundWin95BrokenUNICODESupport
		if (length == 0) {
			textFormat = CF_TEXT;
			length = flavorPackage.GetFlavorSize (textFormat);
		}
		#endif
		Led_SmallStackBuffer<char> buf (length * sizeof (Led_tChar));		// data read from flavor package is just an array of bytes (not Led_tChar)
																			// but allocate enuf space for converting TO UNICODE - in case of
																			// qWorkAroundWin95BrokenUNICODESupport workaround below - we may
																			// want to copy UNICODE chars in there instead.
		length	=	flavorPackage.ReadFlavorData (textFormat, length, buf);

		Led_tChar*	buffp			= reinterpret_cast<Led_tChar*> (static_cast<char*> (buf));	// INTERPRET array of bytes as Led_tChars
		#if		qWideCharacters && qWorkAroundWin95BrokenUNICODESupport
			if (textFormat != kTEXTClipFormat) {
				// then we must manually convert the clipboard text to UNICODE
				string	tmp			=	string (static_cast<char*> (buf), length);
				length	= ::MultiByteToWideChar (CP_ACP, 0, tmp.c_str (), tmp.length (), buffp, length) * sizeof (Led_tChar);
			}
		#endif
		size_t	nTChars	=	length / sizeof (Led_tChar);
		#if		qWindows
			if (nTChars > 0) {
				// On Windows - CF_TEXT always GUARANTEED to be NUL-terminated, and the
				// length field is often wrong (rounded up to some chunk size, with garbage
				// text at the end...
				nTChars = Led_Min (nTChars, Led_tStrlen (buffp));	// do Led_Min in case clip data corrupt
			}
		#endif

		size_t	start	=	from;
		size_t	end		=	to;
		Led_Require (start <= end);

		nTChars = Led_NormalizeTextToNL (buffp, nTChars, buffp, nTChars);
		GetTextStore ().Replace (start, end, buffp, nTChars);
		return true;
	}
	else {
		return false;
	}
}

bool	FlavorPackageInternalizer::InternalizeFlavor_FILE (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
	// For now, we ingore any files beyond the first one (Mac&PC)...LGP 960522
	if (flavorPackage.GetFlavorAvailable (kFILEClipFormat)) {
		size_t	fileSpecBufferLength		=	flavorPackage.GetFlavorSize (kFILEClipFormat);
		Led_SmallStackBuffer<char> fileSpecBuffer (fileSpecBufferLength);
		fileSpecBufferLength = flavorPackage.ReadFlavorData (kFILEClipFormat, fileSpecBufferLength, fileSpecBuffer);

		// Unpack the filename
		#if		qMacOS
			HFSFlavor	flavorData;
			memset (&flavorData, 0, sizeof flavorData);
			memcpy (&flavorData, fileSpecBuffer, Led_Min (sizeof flavorData, fileSpecBufferLength));
			const FSSpec*	realFileName		=	&flavorData.fileSpec;
			Led_ClipFormat	suggestedClipFormat	=	flavorData.fileType;
		#elif	qWindows
			TCHAR	realFileName[_MAX_PATH + 1];
			{
				HDROP	hdrop	=	(HDROP)::GlobalAlloc (GMEM_FIXED, fileSpecBufferLength);
				Led_ThrowIfNull (hdrop);
				(void)::memcpy (hdrop, fileSpecBuffer, fileSpecBufferLength);
				size_t	nChars	=	::DragQueryFile (hdrop, 0, NULL, 0);
				Led_Verify (::DragQueryFile (hdrop, 0, realFileName, nChars+1) == nChars);
				::GlobalFree (hdrop);
			}
			Led_ClipFormat	suggestedClipFormat	=	kBadClipFormat;	// no guess - examine text later
		#elif	qXWindows
			char	realFileName[1000];	// use MAX_PATH? ??? NO - GET REAL SIZE !!! X-TMP-HACK-LGP991213
			realFileName[0] = '\0';
			Led_ClipFormat	suggestedClipFormat	=	kBadClipFormat;	// no guess - examine text later
		#endif

		return InternalizeFlavor_FILEData (realFileName, &suggestedClipFormat, NULL, from, to);
	}
	else {
		return false;
	}
}

bool	FlavorPackageInternalizer::InternalizeFlavor_FILEData (
									#if		qMacOS
										const FSSpec* fileName,
									#elif	qWindows || qXWindows
										const Led_SDK_Char* fileName,
									#endif
										Led_ClipFormat* suggestedClipFormat,
										CodePage* suggestedCodePage,
										size_t from, size_t to
								)
{
	FileReader	fileReader (fileName);
	const Byte* fileBuf = fileReader.GetFileStart ();
	size_t		fileLen	= fileReader.GetFileEnd () - fileReader.GetFileStart ();

	InternalizeFlavor_FILEGuessFormatsFromName (fileName, suggestedClipFormat, suggestedCodePage);

	// If we still don't have a good guess of the clip format, then peek at the data, and 
	// try to guess...
	InternalizeFlavor_FILEGuessFormatsFromStartOfData (suggestedClipFormat, suggestedCodePage, fileBuf, fileBuf + fileLen);

	// By this point, we've read in the file data, and its stored in fileReader. And, we've been given guesses,
	// and adjusted our guesses about the format (without looking at the data). Now just
	// internalize the bytes according to our format guesses.
	return InternalizeFlavor_FILEDataRawBytes (suggestedClipFormat, suggestedCodePage, from, to, fileBuf, fileLen);
}

void	FlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromName (
		#if		qMacOS
			const FSSpec* fileName,
		#elif	qWindows || qXWindows
			const Led_SDK_Char* fileName,
		#endif
			Led_ClipFormat* suggestedClipFormat,
			CodePage* suggestedCodePage
	)
{
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
			if (::_tcsicmp (ext, Led_SDK_TCHAROF (".txt")) == 0) {
				*suggestedClipFormat = kTEXTClipFormat;
			}
		}
	#endif
}

void	FlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromStartOfData (
								Led_ClipFormat* suggestedClipFormat,
								CodePage* /*suggestedCodePage*/,
								const Byte* /*fileStart*/, const Byte* /*fileEnd*/
						)

{
	if (suggestedClipFormat != NULL) {
		if (*suggestedClipFormat == kBadClipFormat) {
			// Then try guessing some different formats. Since this module only knows of one, and thats a last ditch
			// default - we don't venture a guess here. But - other formats - like RTF and HTML - have distinctive headers
			// than can be scanned for to see if we are using that format data
		}
	}
}

bool	FlavorPackageInternalizer::InternalizeFlavor_FILEDataRawBytes (
										Led_ClipFormat* suggestedClipFormat,
										CodePage* suggestedCodePage,
										size_t from, size_t to,
										const void* rawBytes, size_t nRawBytes
								)
{
	/*
	 *	If suggesedFormat UNKNOWN - treat as text.
	 *
	 *	If its TEXT, read it in here directly (using the suggested codePage).
	 *
	 *	If its anything else - then create a temporary, fake memory-based SourcePackage, and re-internalize.
	 */
	Led_ClipFormat	cf	=	(suggestedClipFormat == NULL or *suggestedClipFormat == kBadClipFormat)? kTEXTClipFormat: *suggestedClipFormat;
	if (cf != kTEXTClipFormat) {
		ReadWriteMemBufferPackage	package;
		package.AddFlavorData (cf, nRawBytes, rawBytes);
		if (InternalizeBestFlavor (package, from, to)) {
			return true;
		}
	}

	/*
	 *	If either the suggestedClipFormat was TEXT or it was something else we didn't understand - then just
	 *	import that contents as if it was plain text
	 *
	 *	We COULD just vector to InternalizeBestFlavor above for the TEXT case - but then we'd lose the passed information about the
	 *	prefered code page - so just do the read/replace here...
	 */
	#if		qWideCharacters
		CodePage						useCodePage =	(suggestedCodePage == NULL or *suggestedCodePage == kCodePage_INVALID)?
																	CodePagesGuesser ().Guess (rawBytes, nRawBytes):
																	*suggestedCodePage
																;
		if (suggestedCodePage != NULL) {
			*suggestedCodePage = useCodePage;
		}
		CodePageConverter				cpc			=	CodePageConverter (useCodePage, CodePageConverter::eHandleBOM);
		size_t							outCharCnt	=	cpc.MapToUNICODE_QuickComputeOutBufSize (reinterpret_cast<const char*> (rawBytes), nRawBytes + 1);
		Led_SmallStackBuffer<Led_tChar>	fileData2 (outCharCnt);
		cpc.MapToUNICODE (reinterpret_cast<const char*> (rawBytes), nRawBytes, static_cast<wchar_t*> (fileData2), &outCharCnt);
		size_t	charsRead = outCharCnt;
		Led_Assert (charsRead <= nRawBytes);
	#else
		Led_SmallStackBuffer<Led_tChar>	fileData2 (nRawBytes);
		memcpy (fileData2, (char*)rawBytes, nRawBytes);
		size_t	charsRead	=	nRawBytes;
	#endif
	charsRead = Led_NormalizeTextToNL (fileData2, charsRead, fileData2, charsRead);
	GetTextStore ().Replace (from, to, fileData2, charsRead);
	return true;
}












/*
 ********************************************************************************
 *************************** ReaderClipboardFlavorPackage ***********************
 ********************************************************************************
 */
#if		qXWindows
map<Led_ClipFormat,vector<char> >	ReaderClipboardFlavorPackage::sPrivateClipData;
#endif

bool	ReaderClipboardFlavorPackage::GetFlavorAvailable (Led_ClipFormat clipFormat) const
{
	#if		qXWindows
		map<Led_ClipFormat,vector<char> >::const_iterator i = sPrivateClipData.find (clipFormat);
		return (i != sPrivateClipData.end ());
	#else
		return Led_ClipboardObjectAcquire::FormatAvailable (clipFormat);
	#endif
}

size_t	ReaderClipboardFlavorPackage::GetFlavorSize (Led_ClipFormat clipFormat) const
{
	#if		qXWindows
		map<Led_ClipFormat,vector<char> >::const_iterator i = sPrivateClipData.find (clipFormat);
		if (i == sPrivateClipData.end ()) {
			return 0;
		}
		else {
			return i->second.size ();
		}
	#else
		Led_ClipboardObjectAcquire clip (clipFormat);
		if (clip.GoodClip ()) {
			return clip.GetDataLength ();
		}
		else {
			return 0;
		}
	#endif
}

size_t	ReaderClipboardFlavorPackage::ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const
{
	#if		qXWindows
		map<Led_ClipFormat,vector<char> >::const_iterator i = sPrivateClipData.find (clipFormat);
		if (i == sPrivateClipData.end ()) {
			return 0;
		}
		else {
			size_t	copyNBytes	=	Led_Min (bufSize, i->second.size ());
			(void)::memcpy (buf, &*(i->second.begin ()), copyNBytes);
			Led_Ensure (copyNBytes <= bufSize);
			return copyNBytes;
		}
	#else
		Led_ClipboardObjectAcquire clip (clipFormat);
		if (clip.GoodClip ()) {
			size_t	copyNBytes	=	Led_Min (bufSize, clip.GetDataLength ());
			(void)::memcpy (buf, clip.GetData (), copyNBytes);
			Led_Ensure (copyNBytes <= bufSize);
			return copyNBytes;
		}
		else {
			return 0;
		}
	#endif
}










/*
 ********************************************************************************
 **************************** WriterClipboardFlavorPackage **********************
 ********************************************************************************
 */
void	WriterClipboardFlavorPackage::AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf)
{
	#if		qMacOS
		#if		TARGET_CARBON
			ScrapRef			scrap	=	NULL;
			Led_ThrowIfOSStatus (::GetCurrentScrap (&scrap));
			Led_ThrowIfOSStatus (::PutScrapFlavor (scrap, clipFormat, kScrapFlavorMaskNone, bufSize, buf));
		#else
			Led_ThrowOSErr (::PutScrap (bufSize, clipFormat, Ptr (buf)));
		#endif
	#elif	qWindows
		// NOTE: FOR THE PC - it is assumed all this happens  in the context of an open/close clipboard
		// done in the Led_MFC class overrides of OnCopyCommand_Before/OnCopyCommand_After
		HANDLE	h	=	::GlobalAlloc (GHND | GMEM_MOVEABLE, bufSize);
		Led_ThrowIfNull (h);
		{
			char*	clipBuf	=	(char*)::GlobalLock (h);
			if (clipBuf == NULL) {
				::GlobalFree (h);
				Led_ThrowOutOfMemoryException ();
			}
			memcpy (clipBuf, buf, bufSize);
			::GlobalUnlock (h);
		}
		if (::SetClipboardData (clipFormat, h) == NULL) {
			DWORD	err	=	::GetLastError ();
			Led_ThrowIfErrorHRESULT (MAKE_HRESULT (SEVERITY_ERROR, FACILITY_WIN32, err));
		}
	#elif		qXWindows
		ReaderClipboardFlavorPackage::sPrivateClipData.insert (map<Led_ClipFormat,vector<char> >::value_type (clipFormat, vector<char> (reinterpret_cast<const char*> (buf), reinterpret_cast<const char*> (buf) + bufSize)));
	#endif
}










/*
 ********************************************************************************
 ***************************** ReadWriteMemBufferPackage ************************
 ********************************************************************************
 */
ReadWriteMemBufferPackage::ReadWriteMemBufferPackage ():
	ReaderFlavorPackage (),
	WriterFlavorPackage (),
	fPackages ()
{
	// Tuned to the MWERKS CWPro1 STL implemenation. If you don't call vector::reserve () it uses 361 (pagesize/size(T) apx)
	// which means this is HUGE. And since we keep several of these (one per char typed for undo), it helps mem usage
	// alot to keep this down.
	//
	// Even though tuned to Mac code, should work pretty well in any case.
	
	//fPackages.SetSlotsAlloced (1);
	// HAVENT TESTED WITH STLLIB - BUT Probably a good idea here too - LGP 980923
	fPackages.reserve (1);
}

ReadWriteMemBufferPackage::~ReadWriteMemBufferPackage ()
{
}
 			
bool	ReadWriteMemBufferPackage::GetFlavorAvailable (Led_ClipFormat clipFormat) const
{
	for (size_t i = 0; i < fPackages.size (); i++) {
		if (fPackages[i].fFormat == clipFormat) {
			return true;
		}
	}
	return false;
}

size_t	ReadWriteMemBufferPackage::GetFlavorSize (Led_ClipFormat clipFormat) const
{
	for (size_t i = 0; i < fPackages.size (); i++) {
		if (fPackages[i].fFormat == clipFormat) {
			return fPackages[i].fData.size ();
		}
	}
	Led_Assert (false); return 0;
}

size_t	ReadWriteMemBufferPackage::ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const
{
	for (size_t i = 0; i < fPackages.size (); i++) {
		if (fPackages[i].fFormat == clipFormat) {
			size_t	copyNBytes	=	Led_Min (bufSize, fPackages[i].fData.size ());
			// Note - this kookie &* stuff is to work around bugs in some STLs - that don't let you convert an iterator to a pointer.- SPR#0847
			memcpy (buf, &*fPackages[i].fData.begin (), copyNBytes);
			Led_Ensure (copyNBytes <= bufSize);
			return copyNBytes;
		}
	}
	Led_Assert (false); return 0;
}

void	ReadWriteMemBufferPackage::AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf)
{
	PackageRecord	pr;
	pr.fFormat = clipFormat;
	const char*	cb	=	reinterpret_cast<const char*> (buf);
	pr.fData = vector<char> (cb, cb+bufSize);
	fPackages.push_back (pr);
}





#if		qLedUsesNamespaces
}
#endif








// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

