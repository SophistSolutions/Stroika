/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/StyledTextIO_STYLText.cpp,v 2.13 2003/03/21 17:30:47 lewis Exp $
 *
 * Changes:
 *	$Log: StyledTextIO_STYLText.cpp,v $
 *	Revision 2.13  2003/03/21 17:30:47  lewis
 *	patch last fix so compiles on MacOS/Linux and non-UNICODE Win32 config
 *	
 *	Revision 2.12  2003/03/21 16:44:30  lewis
 *	SPR#1371 - fix code in StyledTextIOReader_PlainText::Read and StyledTextIOReader_STYLText::Read to read a bit at a time and NOT assume you can seek to MAXINT and the say current_offset () to find out how big the file is. Also - fixed a bug in StyledTextIOReader::BufferedIndirectSrcStream::read to seek before read() on target indirect srcStream
 *	
 *	Revision 2.11  2003/02/28 19:12:03  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.10  2003/02/24 21:57:00  lewis
 *	SPR#1306- use CodePageConverter in a few places instead of direct WideCharToMultibyte
 *	etc calls so MacOS can compile for UNICODE
 *	
 *	Revision 2.9  2002/05/06 21:33:55  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.8  2001/11/27 00:30:01  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.7  2001/09/05 23:44:23  lewis
 *	Added GetSinkStream ().EndOfBuffer () calls as part of fix for SPR#1014
 *	
 *	Revision 2.6  2001/08/28 18:43:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  2000/04/16 14:39:39  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.4  2000/04/16 13:58:56  lewis
 *	small tweeks to get recent changes compiling on Mac
 *	
 *	Revision 2.3  2000/04/14 22:40:27  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.2  1999/12/21 03:15:54  lewis
 *	*** empty log message ***
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

#include	"CodePage.h"

#include	"StyledTextIO_STYLText.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	namespace	Led {
#endif







#if		qMacOS
/*
 ********************************************************************************
 ************************** StyledTextIOReader_STYLText *************************
 ********************************************************************************
 */
StyledTextIOReader_STYLText::StyledTextIOReader_STYLText (SrcStream* srcStream, SinkStream* sinkStream):
	StyledTextIOReader (srcStream, sinkStream)
{
}

void	StyledTextIOReader_STYLText::Read ()
{
	size_t	totalRead	=	0;
	{
#if	1
		// Read into a contiguous block of memory since it makes the dealing with CRLF
		// strattling a buffer-bounary problem go away. Note that the Led_SmallStackBuffer<>::GrowToSize()
		// code grows exponentially so that we minimize buffer copies on grows...
		Led_SmallStackBuffer<char>	buf (totalRead);
		while (true) {
			size_t	kTryToReadThisTime	=	16 * 1024;
			buf.GrowToSize (totalRead + kTryToReadThisTime);
			void*	readPtr	=	static_cast<char*> (buf) + totalRead;
			size_t bytesRead = GetSrcStream ().read (readPtr, kTryToReadThisTime);
			totalRead += bytesRead;
			if (bytesRead == 0) {
				break;
			}
		}
#else
		// read all at once. Requires contiguous block of memory, but is significantly faster
		// (for reasons I don't completely understand, but don't care so much about either...)
		// LGP 960515
		size_t	oldPos	=	GetSrcStream ().current_offset ();
		GetSrcStream ().seek_to (UINT_MAX);
		size_t	endPos	=	GetSrcStream ().current_offset ();
		Led_Assert (endPos >= oldPos);
		GetSrcStream ().seek_to (oldPos);
		size_t	len	=	endPos-oldPos;
		Led_SmallStackBuffer<char>	buf (len);
		if ( (totalRead = GetSrcStream ().read (buf, len)) != len ) {
			Led_ThrowBadFormatDataException ();
		}
#endif
		#if		qWideCharacters
			Led_SmallStackBuffer<Led_tChar>	tCharBuf (totalRead);
			CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (buf, totalRead, tCharBuf, &totalRead);
			totalRead = Led_NormalizeTextToNL (tCharBuf, totalRead, tCharBuf, totalRead);
			GetSinkStream ().AppendText (tCharBuf, totalRead, NULL);
		#else
			totalRead = Led_NormalizeTextToNL (buf, totalRead, buf, totalRead);
			GetSinkStream ().AppendText (buf, totalRead, NULL);
		#endif
	}
	Led_Assert (GetSrcStream ().GetAUXResourceHandle ());	// If QuickLookAppearsToBeRightFormat () we shouldn't get here?
	if (GetSrcStream ().GetAUXResourceHandle () != NULL) {
		StScrpHandle	stylHandle	=	(StScrpHandle)GetSrcStream ().GetAUXResourceHandle ();
		HLock (Handle (stylHandle));
		vector<StandardStyledTextImager::InfoSummaryRecord>	ledStyleInfo	=	StandardStyledTextImager::Convert ((*stylHandle)->scrpStyleTab, (*stylHandle)->scrpNStyles);
		HUnlock (Handle (stylHandle));
		GetSinkStream ().ApplyStyle (0, totalRead, ledStyleInfo);
	}
	GetSinkStream ().EndOfBuffer ();
}

bool	StyledTextIOReader_STYLText::QuickLookAppearsToBeRightFormat ()
{
// Probably should check even more about the handle... - but this will do for now...
	if (GetSrcStream ().GetAUXResourceHandle () == NULL) {
		return false;
	}

	SrcStreamSeekSaver	savePos (GetSrcStream ());

	Led_tChar	buf[1024];
	size_t	bytesRead	= GetSrcStream ().read (buf, Led_NEltsOf (buf));

	return (ValidateTextForCharsetConformance (buf, bytesRead));
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


