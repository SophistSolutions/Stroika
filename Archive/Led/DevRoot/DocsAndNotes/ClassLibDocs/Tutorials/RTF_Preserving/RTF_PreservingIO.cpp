/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/DocsAndNotes/ClassLibDocs/Tutorials/RTF_Preserving/RTF_PreservingIO.cpp,v 2.8 2001/11/27 00:37:16 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: RTF_PreservingIO.cpp,v $
 *	Revision 2.8  2001/11/27 00:37:16  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.7  2001/10/15 16:01:05  lewis
 *	get compiling and roughly working with Led 3.0. Maybe delete for future releases of Led - since I think this tutorial is not very good and even less useful
 *	
 *	Revision 2.6  2001/08/30 01:10:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  1999/07/19 19:28:53  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1999/07/19 18:24:33  lewis
 *	react to changes in underlying RTF code from 2.2 to 2.3
 *	
 *	Revision 2.3  1999/03/08 23:11:02  lewis
 *	react to various led changes over the last year or so
 *	
 *	Revision 2.2  1997/06/18 02:26:12  lewis
 *	Now base Led code reads/writes color table, so we keep it as object, not string.
 *	
 *	Revision 2.1  1996/12/05  18:53:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1996/10/04  16:30:02  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *
 */

#include	<ctype.h>
#include	<limits.h>
#include	<stdio.h>

#include	"RTF_PreservingIO.h"





#if		qLedUsesNamespaces
	namespace	Led {
#endif




/*
 ********************************************************************************
 ******************************** RTFInfo_Preserving ****************************
 ********************************************************************************
 */
RTFInfo_Preserving::RTFInfo_Preserving (TextStore* textStore):
	RTFInfo (),
	MarkerOwner (),
	fTextStore (NULL),
	fDocumentCharacterSet (kCodePage_ANSI),	// ANSI default, according to RTF spec
	fDefaultFontNumber (-1),
	fDefLangNumber (-1),
	fFontTable (),
	fColorTable (),
	fSavedStyleSheetGroup (),
	fSavedInfoGroup (),
	fSavedAuthorGroup (),
	fFirstHiddenControlMarker (NULL),
	fLastHiddenControlMarker (NULL)
{
	SetTextStore (textStore);
}

RTFInfo_Preserving::~RTFInfo_Preserving ()
{
	SetTextStore (NULL);
}

void	RTFInfo_Preserving::SetTextStore (TextStore* textStore)
{
	if (fTextStore != textStore) {
		if (fTextStore != NULL) {
			DeleteAllInfo ();
			fTextStore->RemoveMarkerOwner (this);
		}
		fTextStore = textStore;
		if (fTextStore != NULL) {
			fTextStore->AddMarkerOwner (this);
		}
	}
}

TextStore*	RTFInfo_Preserving::PeekAtTextStore () const
{
	return fTextStore;
}

void	RTFInfo_Preserving::DeleteAllInfo ()
{
	Led_Require (fTextStore);

	fFontTable.fEntries.clear ();
	fColorTable.fEntries.clear ();
	fSavedStyleSheetGroup = "";
	fSavedInfoGroup = "";
	fSavedAuthorGroup = "";

	{
		vector<Marker*>		deleteEm;
		for (HiddenRTFControlMarker* m = fFirstHiddenControlMarker; m != NULL; m = m->fNext) {
			deleteEm.push_back (m);
		}
		fTextStore->RemoveMarkers (&deleteEm.front (), deleteEm.size ());
		for (size_t i = 0; i < deleteEm.size (); i++) {
			Marker*	m	=	deleteEm[i];
			Led_AssertNotNil (m);
			delete m;
		}
		fFirstHiddenControlMarker = NULL;
		fLastHiddenControlMarker = NULL;
	}
}










/*
 ********************************************************************************
 ***************** RTFInfo_Preserving::HiddenRTFControlMarker *******************
 ********************************************************************************
 */
RTFInfo_Preserving::HiddenRTFControlMarker::HiddenRTFControlMarker (const ControlWord& controlWord):
	Marker (),
	fText (""),
	fNext (NULL)
{
	WriteControlWord (controlWord);
}

RTFInfo_Preserving::HiddenRTFControlMarker::HiddenRTFControlMarker (const string& text):
	Marker (),
	fText (text),
	fNext (NULL)
{
}

void	RTFInfo_Preserving::HiddenRTFControlMarker::WriteControlWord (const ControlWord& controlWord)
{
	char	buf[1024];
	if (controlWord.fHasArg) {
		(void)::sprintf (buf, "\\%s%d ", RTFIO::GetAtomName (controlWord.fWord).c_str (), controlWord.fValue);
	}
	else {
		(void)::sprintf (buf, "\\%s ", RTFIO::GetAtomName (controlWord.fWord).c_str ());
	}
	fText = buf;
}










/*
 ********************************************************************************
 ********************** StyledTextIOReader_RTF_Preserving ***********************
 ********************************************************************************
 */
StyledTextIOReader_RTF_Preserving::StyledTextIOReader_RTF_Preserving (SrcStream* srcStream, SinkStream* sinkStream, RTFInfo_Preserving* rtfInfo):
	StyledTextIOReader_RTF (srcStream, sinkStream, rtfInfo),
	fRTFInfo (rtfInfo)
{
}

void	StyledTextIOReader_RTF_Preserving::Read ()
{
	ReaderContext	readerContext (*this);
	try {
		ReadGroup (readerContext);
	}
	catch (ReadEOFException& /*eof*/) {
		// Signifies un unimportant error - cruft past the end of file. Don't treat this as an
		// error - at least for now. Just eat it (I wonder if this is a mistake? - LGP 960827)
		// If it IS a mistake, we should replace this IGNORE with a 'Led_ThrowBadFormatDataException ();'
	}
	readerContext.fDefaultDestination->Flush ();
	fRTFInfo->fDocumentCharacterSet = readerContext.fDocumentCharacterSet;
	if (readerContext.fFontTable != NULL) {
		fRTFInfo->fFontTable = *readerContext.fFontTable;
	}
	if (readerContext.fColorTable != NULL) {
		fRTFInfo->fColorTable = *readerContext.fColorTable;
	}
}

bool	StyledTextIOReader_RTF_Preserving::HandleControlWord_author (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
	if (readerContext.GetCurrentGroupContext () == NULL) {
		Led_ThrowBadFormatDataException ();
	}
	GetSrcStream ().seek_to (readerContext.GetCurrentGroupContext ()->fCurrentGroupStartIdx);
	Led_RequireNotNil (fRTFInfo);
	fRTFInfo->fSavedAuthorGroup	=	ReadInGroupAndSave ();
	return true;
}

bool	StyledTextIOReader_RTF_Preserving::HandleControlWord_info (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
	if (readerContext.GetCurrentGroupContext () == NULL) {
		Led_ThrowBadFormatDataException ();
	}
	GetSrcStream ().seek_to (readerContext.GetCurrentGroupContext ()->fCurrentGroupStartIdx);
	Led_RequireNotNil (fRTFInfo);
	fRTFInfo->fSavedInfoGroup	=	ReadInGroupAndSave ();
	return true;
}

bool	StyledTextIOReader_RTF_Preserving::HandleControlWord_stylesheet (ReaderContext& readerContext, const RTFIO::ControlWord& /*controlWord*/)
{
	if (readerContext.GetCurrentGroupContext () == NULL) {
		Led_ThrowBadFormatDataException ();
	}
	GetSrcStream ().seek_to (readerContext.GetCurrentGroupContext ()->fCurrentGroupStartIdx);
	Led_RequireNotNil (fRTFInfo);
	fRTFInfo->fSavedStyleSheetGroup	=	ReadInGroupAndSave ();
	return true;
}

void	StyledTextIOReader_RTF_Preserving::AboutToStartBody (ReaderContext& readerContext)
{
	inherrited::AboutToStartBody (readerContext);
	fRTFInfo->fDefaultFontNumber = readerContext.fDefaultFontNumber;
}

void	StyledTextIOReader_RTF_Preserving::ReadGroup (ReaderContext& readerContext)
{
	Led_AssertNotNil (fRTFInfo);

	if (readerContext.GetCurrentGroupContext () == NULL) {
		// we don't need to save the outer braces.
		
		// probably other cases, like font groups etc we don't need, but I don't think this does any harm...
		inherrited::ReadGroup (readerContext);
	}
	else {
		AddHiddenRTFMarker (readerContext, new RTFInfo_Preserving::HiddenRTFControlMarker ("{"));

		inherrited::ReadGroup (readerContext);

		AddHiddenRTFMarker (readerContext, new RTFInfo_Preserving::HiddenRTFControlMarker ("}"));
	}
}

bool	StyledTextIOReader_RTF_Preserving::HandleControlWord_row (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
	// IGNORE. Default implementation translates this into an NL. But then on output
	// we'd generate both \par and a \row. We really only want the \row.
	// We could invent some new marker kind, or some other mechanism for us to insert to
	// handle this case, but the simplest thing todo for now is to disable mapping \row
	// to NLs.
	return HandleControlWord_UnknownControlWord (readerContext, controlWord);
}

bool	StyledTextIOReader_RTF_Preserving::HandleControlWord_UnknownControlWord (ReaderContext& readerContext, const RTFIO::ControlWord& controlWord)
{
	if (controlWord.fWord == RTFIO::eControlAtom_deflang) {
		if (controlWord.fHasArg) {
			fRTFInfo->fDefLangNumber = controlWord.fValue;
		}
		return false;
	}

	bool	isOneWeNeedToPreserve	=	true;
	for (size_t i = 0; i < sizeof (kMappings)/sizeof (kMappings[0]); i++) {
		if (controlWord.fWord == kMappings[i].fControlWordName) {
			isOneWeNeedToPreserve = false;
			break;
		}
	}

	bool	result	=	inherrited::HandleControlWord_UnknownControlWord (readerContext, controlWord);
	
	if (isOneWeNeedToPreserve) {
		AddHiddenRTFMarker (readerContext, new RTFInfo_Preserving::HiddenRTFControlMarker (controlWord));
	}
	return result;
}

void	StyledTextIOReader_RTF_Preserving::AddHiddenRTFMarker (ReaderContext& readerContext, RTFInfo_Preserving::HiddenRTFControlMarker* m)
{
	// add zero-length marker right here!!!
	// Maintain linked list. APPEND to the linked list (even though singly linked, keep ptr to head and tail
	// for cheap appends)
	//
	// Note - this could be made considerably more efficient if we just appended text to the end of an existing
	// marker if it was at the same position. Just trying to get this functionally right first - LGP 960831.
	Led_AssertNotNil (fRTFInfo);
	StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination*	ssd	=	dynamic_cast <StyledTextIOReader_RTF::ReaderContext::SinkStreamDestination*> (readerContext.fDefaultDestination.get ());
	if (ssd != NULL) {
		ssd->InsertMarker (m, ssd->current_offset (), 0, fRTFInfo);
	}
	if (fRTFInfo->fFirstHiddenControlMarker == NULL) {
		Led_Assert (fRTFInfo->fLastHiddenControlMarker == NULL);
		fRTFInfo->fFirstHiddenControlMarker = m;
		fRTFInfo->fLastHiddenControlMarker = m;
	}
	else {
		Led_AssertNotNil (fRTFInfo->fLastHiddenControlMarker);
		Led_Assert (fRTFInfo->fLastHiddenControlMarker->fNext == NULL);
		fRTFInfo->fLastHiddenControlMarker->fNext = m;
		fRTFInfo->fLastHiddenControlMarker = m;
	}
}










/*
 ********************************************************************************
 ************************ StyledTextIOReader_RTF_Preserving *********************
 ********************************************************************************
 */
StyledTextIOWriter_RTF_Preserving::StyledTextIOWriter_RTF_Preserving (SrcStream* srcStream, SinkStream* sinkStream, RTFInfo_Preserving* rtfInfo):
	StyledTextIOWriter_RTF (srcStream, sinkStream, rtfInfo),
	fRTFInfo (rtfInfo)
{
	Led_RequireNotNil (rtfInfo);
	fDocumentCharacterSet = fRTFInfo->fDocumentCharacterSet;
}

void	StyledTextIOWriter_RTF_Preserving::WriteHeader (WriterContext& /*writerContext*/)
{
	write ("\\rtf1");
	WriteDocCharset ();
	Led_AssertNotNil (fRTFInfo);
	if (fRTFInfo->fDefaultFontNumber != -1) {
		char	buf[1024];
		(void)::sprintf (buf, "\\deff%d", fRTFInfo->fDefaultFontNumber);
		write (buf);
	}
	if (fRTFInfo->fDefLangNumber != -1) {
		char	buf[1024];
		(void)::sprintf (buf, "\\deflang%d", fRTFInfo->fDefLangNumber);
		write (buf);
	}

	write (" ");
	WriteFontTable ();
	WriteColorTable ();

	// Write the other header groups.
	write (fRTFInfo->fSavedStyleSheetGroup);
	write (fRTFInfo->fSavedInfoGroup);
	write (fRTFInfo->fSavedAuthorGroup);
}

void	StyledTextIOWriter_RTF_Preserving::WriteBody (WriterContext& writerContext)
{
	/*
	 *	NOTE: This is similar to inherrited::WriteBody (), except that we inject extra groups
	 *	and control words saved from when we read things in.
	 */

	/*
	 *	Walk through the characters, and output them one at a time. Walk
	 *	SIMULTANEOUSLY through the style run information, and output new controlling
	 *	tags on the fly.
	 */
	AssureStyleRunSummaryBuilt ();
	Led_AssertNotNil (fStyleRunSummary);

	RTFInfo_Preserving::HiddenRTFControlMarker* 	curControlMarker		= fRTFInfo->fFirstHiddenControlMarker;

	writerContext.fLastEmittedISR = StandardStyledTextImager::InfoSummaryRecord (Led_IncrementalFontSpecification (), 0);
	writerContext.fNextStyleChangeAt = 0;
	writerContext.fIthStyleRun = 0;
	Led_tChar	c	=	'\0';

	bool	forceInvalidFontChangeInfo	=	false;
// We COULD simplify this code in Led 2.1d3 by adding an INVALIDATEFontChangeInfo() method to the Writer!!!
// Should do!!! - LGP 961001
	while (GetSrcStream ().readNTChars (&c, 1) != 0) {
		// -1 for PREV character, NOT 0
		while (curControlMarker != NULL and curControlMarker->GetStart () == GetSrcStream ().current_offset ()-1) {
			write (curControlMarker->fText);
			if (curControlMarker->fText == "}") {
				forceInvalidFontChangeInfo = true;
			}
			curControlMarker = curControlMarker->fNext;
		}
		
		if (forceInvalidFontChangeInfo) {
			forceInvalidFontChangeInfo = false;
			
			// if we're about to emit new font info stuff anyhow, don't bother. But otherwise, output what we did last time.
			if (GetSrcStream ().current_offset ()-1 != writerContext.fNextStyleChangeAt) {
				Led_Assert (writerContext.fIthStyleRun >= 1);	// cuz else we'd have been at a change point!
				// grab PREVIOUS style run cuz index already pointing to next one to emit...
				const StandardStyledTextImager::InfoSummaryRecord&	nextStyleRun	=	(*fStyleRunSummary)[writerContext.fIthStyleRun-1];
				EmitBodyFontInfoChange (writerContext, nextStyleRun);
			}
		}
		WriteBodyCharacter (writerContext, c);
	}

	// Write any trailing (after the text) controls
	while (curControlMarker != NULL) {
		Led_Assert (curControlMarker->GetStart () == GetSrcStream ().current_offset ());
		write (curControlMarker->fText);
		curControlMarker= curControlMarker->fNext;
	}
}

void	StyledTextIOWriter_RTF_Preserving::AssureColorTableBuilt ()
{
	if (fColorTable == NULL) {
		// Copy OLD colortable, and add new items. The reason we must do this is cuz we don't parse the style table, and that
		// refers back to font table entries. This is the only way (short of parsing that stuff) to properly preserve the meaning
		// of style tables...
		Led_AssertNotNil (fRTFInfo);
		fColorTable = new RTFIO::ColorTable (fRTFInfo->fColorTable);
		AssureColorTableBuilt_FillIn ();
	}
}

void	StyledTextIOWriter_RTF_Preserving::AssureFontTableBuilt ()
{
	if (fFontTable == NULL) {
		// Copy OLD fonttable, and add new items. The reason we must do this is cuz we don't parse the style table, and that
		// refers back to font table entries. This is the only way (short of parsing that stuff) to properly preserve the meaning
		// of style tables...
		Led_AssertNotNil (fRTFInfo);
		fFontTable = new RTFIO::FontTable (fRTFInfo->fFontTable);
		AssureFontTableBuilt_FillIn ();
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

