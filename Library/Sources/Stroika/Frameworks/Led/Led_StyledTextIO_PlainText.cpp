/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/StyledTextIO_PlainText.cpp,v 2.17 2003/03/21 17:30:47 lewis Exp $
 *
 * Changes:
 *	$Log: StyledTextIO_PlainText.cpp,v $
 *	Revision 2.17  2003/03/21 17:30:47  lewis
 *	patch last fix so compiles on MacOS/Linux and non-UNICODE Win32 config
 *	
 *	Revision 2.16  2003/03/21 16:44:30  lewis
 *	SPR#1371 - fix code in StyledTextIOReader_PlainText::Read and StyledTextIOReader_STYLText::Read
 *	to read a bit at a time and NOT assume you can seek to MAXINT and the say current_offset ()
 *	to find out how big the file is. Also - fixed a bug in StyledTextIOReader::BufferedIndirectSrcStream::read
 *	to seek before read() on target indirect srcStream
 *	
 *	Revision 2.15  2003/02/28 19:11:31  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.14  2003/02/24 21:57:00  lewis
 *	SPR#1306- use CodePageConverter in a few places instead of direct WideCharToMultibyte etc
 *	calls so MacOS can compile for UNICODE
 *	
 *	Revision 2.13  2002/05/06 21:33:54  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.12  2001/11/27 00:30:00  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.11  2001/09/20 00:39:28  lewis
 *	SPR#1041 - call cpc.SetHandleBOM () in plain-text reader class for codepage conversion
 *	
 *	Revision 2.10  2001/09/12 17:23:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  2001/09/09 23:15:04  lewis
 *	SPR#0698- fix StyledTextIOReader_PlainText to use new codepage guessing/mapping
 *	support (small part of 698 - rest defered)
 *	
 *	Revision 2.8  2001/09/05 23:44:22  lewis
 *	Added GetSinkStream ().EndOfBuffer () calls as part of fix for SPR#1014
 *	
 *	Revision 2.7  2001/08/29 23:36:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2001/08/28 18:43:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  2000/06/23 21:44:38  lewis
 *	SPR#0807 Handle reading in wide_char UNICODE
 *	
 *	Revision 2.4  2000/04/16 14:39:39  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.3  2000/04/14 22:40:27  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.2  2000/04/12 05:54:52  lewis
 *	fix StyledTextIO_PlainText_Writer - to convert to ANSI (maybe this
 *	should be optional - but to be consistent with reader - and how this
 *	is generlly used - we must do that!
 *	
 *	Revision 2.1  1999/12/18 03:56:48  lewis
 *	0662- break up StyledTextIO module into many _parts
 *	
 *
 *	<========== CODE MOVED HERE FROM StyledTextIO.cpp ==========>
 *
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"Led_CodePage.h"

#include	"Led_StyledTextIO_PlainText.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif





#if		qLedUsesNamespaces
	namespace	Led {
#endif










/*
 ********************************************************************************
 ************************** StyledTextIOReader_PlainText ************************
 ********************************************************************************
 */
StyledTextIOReader_PlainText::StyledTextIOReader_PlainText (SrcStream* srcStream, SinkStream* sinkStream):
	StyledTextIOReader (srcStream, sinkStream)
{
}

void	StyledTextIOReader_PlainText::Read ()
{
#if	1
	// Read into a contiguous block of memory since it makes the dealing with CRLF
	// strattling a buffer-bounary problem go away. Note that the Led_SmallStackBuffer<>::GrowToSize()
	// code grows exponentially so that we minimize buffer copies on grows...
	size_t	len	=	0;
	Led_SmallStackBuffer<char>	buf (len);
	while (true) {
		size_t	kTryToReadThisTime	=	16 * 1024;
		buf.GrowToSize (len + kTryToReadThisTime);
		void*	readPtr	=	static_cast<char*> (buf) + len;
		size_t bytesRead = GetSrcStream ().read (readPtr, kTryToReadThisTime);
		len += bytesRead;
		if (bytesRead == 0) {
			break;
		}
	}
#else
	// read all at once. Requires contiguous block of memory, but is significantly faster
	// (for reasons I don't completely understand, but don't care so much about either...)
	// LGP 960515
	// Also note: this makes the dealing with CRLF strattling a buffer-bounary problem go away.
	// If we tried to read in chunks, we'd need to be more careful about the Led_NormalizeTextToNL () code.
	size_t	oldPos	=	GetSrcStream ().current_offset ();
	GetSrcStream ().seek_to (UINT_MAX);
	size_t	endPos	=	GetSrcStream ().current_offset ();
	Led_Assert (endPos >= oldPos);
	GetSrcStream ().seek_to (oldPos);
	size_t	len	=	endPos-oldPos;
	Led_SmallStackBuffer<char>	buf (len);
	size_t	bytesRead	=	0;
	if ( (bytesRead = GetSrcStream ().read (buf, len)) != len ) {
		Led_ThrowBadFormatDataException ();
	}
#endif
#if		qWideCharacters
	CodePage						useCodePage =	CodePagesGuesser ().Guess (buf, len);
	CodePageConverter				cpc			=	CodePageConverter (useCodePage);
	size_t							outCharCnt	=	cpc.MapToUNICODE_QuickComputeOutBufSize (static_cast<const char*> (buf), len + 1);
	Led_SmallStackBuffer<Led_tChar>	wbuf (outCharCnt);
	cpc.SetHandleBOM (true);
	cpc.MapToUNICODE (static_cast<const char*> (buf), len, static_cast<wchar_t*> (wbuf), &outCharCnt);
	size_t	charsRead = outCharCnt;
	Led_Assert (charsRead <= len);
	Led_tChar*	useBuf	=	wbuf;
#else
	size_t		charsRead	=	len;
	Led_tChar*	useBuf		=	buf;
#endif
	charsRead = Led_NormalizeTextToNL (useBuf, charsRead, useBuf, charsRead);
	GetSinkStream ().AppendText (useBuf, charsRead, NULL);
	GetSinkStream ().EndOfBuffer ();
}

bool	StyledTextIOReader_PlainText::QuickLookAppearsToBeRightFormat ()
{
#if		qWideCharacters
	return true;
#else
	SrcStreamSeekSaver	savePos (GetSrcStream ());

	char	buf[1024];
	size_t	bytesRead	= GetSrcStream ().read (buf, sizeof (buf));

	return (ValidateTextForCharsetConformance (buf, bytesRead));
#endif
}







/*
 ********************************************************************************
 ************************** StyledTextIOWriter_PlainText ************************
 ********************************************************************************
 */
StyledTextIOWriter_PlainText::StyledTextIOWriter_PlainText (SrcStream* srcStream, SinkStream* sinkStream):
	StyledTextIOWriter (srcStream, sinkStream)
{
}

void	StyledTextIOWriter_PlainText::Write ()
{
	Led_tChar	buf[8*1024];
	size_t	bytesRead	=	0;
	while ( (bytesRead = GetSrcStream ().readNTChars (buf, Led_NEltsOf (buf))) != 0 ) {
		#if		qMacOS || qXWindows
			Led_tChar	buf2[Led_NEltsOf (buf)];
		#elif	qWindows
			Led_tChar	buf2[2*Led_NEltsOf (buf)];
		#endif
		bytesRead = Led_NLToNative (buf, bytesRead, buf2, Led_NEltsOf (buf2));
		#if		qWideCharacters
			Led_SmallStackBuffer<char>	ansiBuf (bytesRead * sizeof (Led_tChar));
#if 1
			size_t	nChars	=	bytesRead * sizeof (Led_tChar);
			CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (buf2, bytesRead, ansiBuf, &nChars);
			bytesRead = nChars;
#else
			bytesRead = ::WideCharToMultiByte (CP_ACP, 0, buf2, bytesRead, ansiBuf, bytesRead*sizeof (Led_tChar), NULL, NULL);
#endif
			write (ansiBuf, bytesRead);
		#else
			write (buf2, bytesRead);
		#endif
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


