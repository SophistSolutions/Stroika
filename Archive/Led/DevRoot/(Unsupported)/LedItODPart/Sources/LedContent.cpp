/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1996.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/LedContent.cpp,v 2.14 1996/12/13 18:10:13 lewis Exp $
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
 *	$Log: LedContent.cpp,v $
 *	Revision 2.14  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.13  1996/12/05  21:12:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.11  1996/06/01  02:29:29  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1996/05/23  20:35:50  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1996/04/18  16:15:39  lewis
 *	react to led name change ((EmbeddingTag->Led_PrivateEmbeddingTag).
 *
 *	Revision 2.8  1996/03/04  08:13:37  lewis
 *	Don't write tag any longer in ExternalizeEmbedding override since now done
 *	in routine which calls us.
 *
 *	Revision 2.7  1996/02/26  23:12:40  lewis
 *	AddEmbedding no longer takes StyleDatabase arg.
 *	Use kBadIndex instead of size_t (-1)
 *
 *	Revision 2.6  1996/02/05  05:08:16  lewis
 *	A few cleanups (losing debugstrs) and try to get merge/drop-insert-as
 *	stuff working... (based at looking at DR4 final DrawShapes example).
 *
 *	Revision 2.5  1996/01/22  05:53:56  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1996/01/04  00:27:34  lewis
 *	Massive changes. Lose old Pub/Sub links stuff. Unused and probably didn't
 *	work right anyhow.
 *	Separate out backward compat file IO into sepearate file (for private format).
 *	And support NEW file format via new lib code in Led (StyledTextIO code).
 *	Trashed lots of code that wasn't (apparntly) needed.
 *
 *	Revision 2.3  1995/12/15  04:11:17  lewis
 *	Lots of changes. But mainly added new support
 *	to ExternalizePlainText and have Externalize do focusing itself, and
 *	manually call the right sub-externalize methods.
 *	Also, LedContent now ones the shapeList, instead of LedPart.
 *
 *	Revision 2.2  1995/12/13  06:17:01  lewis
 *	Lots of cleanups/changes.
 *	lost Subscribe/Publish support.
 *	Merged out various subclasses of content. Now just part content.
 *	Lose most/many Environemt* args.
 *	Use TmpIterators.
 *	Don't support PICT directly anymore. Just let it get embedded like
 *	everything else.
 *
 *	Revision 2.1  1995/12/06  02:03:11  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
 *
 *
 *
 *
 */


// so long as we use this, it must be included first!!!
#include "AltPoint.h"


#include	<StdProps.xh>

#include	"Embedding.h"


// -- OpenDoc Includes --

#ifndef SOM_ODDraft_xh
#include <Draft.xh>
#endif

#ifndef SOM_ODShape_xh
#include <Shape.xh>
#endif

#ifndef SOM_ODFacet_xh
#include <Facet.xh>
#endif

#ifndef SOM_ODFrame_xh
#include <Frame.xh>
#endif

#ifndef SOM_ODStorageUnit_xh
#include <StorageU.xh>
#endif

#ifndef SOM_ODSession_xh
#include <ODSessn.xh>
#endif

#ifndef SOM_ODFrameFacetIterator_xh
#include "FrFaItr.xh"
#endif

// -- OpenDoc Utilities --

#ifndef _STDTYPIO_
#include "StdTypIO.h"
#endif

#ifndef _ODUTILS_
#include "ODUtils.h"
#endif
#include <Translt.xh>

#ifndef _STORUTIL_
#include <StorUtil.h>
#endif

#ifndef _FOCUSLIB_
#include "FocusLib.h"
#endif

#ifndef _ORDCOLL_
#include "OrdColl.h"
#endif

#ifndef _ODMEMORY_
#include "ODMemory.h"
#endif

#ifndef _ODDEBUG_
#include "ODDebug.h"
#endif

#include "FlipEnd.h"

// -- Toolbox Includes --

#ifndef mathRoutinesIncludes
#include <math routines.h>
#endif

#ifndef __MEMORY__
#include <memory.h>
#endif

#ifndef __DRAG__
#include <Drag.h>
#endif

#include "UseRsrcM.h"

#include	"SampleCollections.h"


#include	"StyledTextIO.h"
#include	"StyledTextEmbeddedObjects.h"


#include "LedPart.h"

#include	"LedPartCommands.h"
#include	"LedFrameEditor.h"

#include	"LedContent.h"


const	Rect	kZeroRect	=	{0, 0, 0, 0};



inline	Environment*	SafeGetEnvironment ()
	{
		Environment*	ev	=	::somGetGlobalEnvironment ();
		Led_AssertNotNil (ev);
		return ev;
	}















#if		qMacOS
#include	<Controls.h>
#include	<Dialogs.h>
struct	popupPrivateData {
	MenuHandle	mHandle;	/* the popup menu handle */
	short		mID;       	/* the popup menu ID */
};
inline	MenuHandle GetPopUpMenuHandle(ControlHandle thisControl)
	{
		popupPrivateData** theMenuData = (popupPrivateData**)(*thisControl)->contrlData;
		return((*theMenuData)->mHandle);
	}
inline	ControlHandle SnatchHandle (DialogPtr thebox, short theGetItem)
	{
	    short itemtype;
	    Rect itemrect;
	    Handle thandle;
	    GetDialogItem (thebox, theGetItem, &itemtype, &thandle, &itemrect);
	    return((ControlHandle)thandle);
	}
#endif

class	MyReader : public StyledTextIOReader_CheckFileAndQueryUserDesiredFormat {
	public:
		MyReader (SrcStream_* srcStream, SinkStream_* sinkStream, const char* fileName):
			StyledTextIOReader_CheckFileAndQueryUserDesiredFormat (false, srcStream, sinkStream),
			fFileName (fileName)
			{
				Led_AssertNotNil (fileName);
			}

		#if		qMacOS
		static	bool					sFirstTime;
		static	Led_Array<const char*> 	sFormats;
		static	int						sPopupItemChosen;
		#endif

		override	size_t	UserPickFormat (const Led_Array<const char*>& formats)
			{
				#if		qMacOS
					sFirstTime = true;
					sFormats = formats;

					CUsingLibraryResources	tmpUseLibRes;

					Str255	tmp;
					tmp[0] = ::strlen (fFileName);
					memcpy (&tmp[1], fFileName, tmp[0]);
					::ParamText (tmp, "\p", "\p", "\p");
					InitCursor();
					sPopupItemChosen = 1;
					#if		GENERATINGCFM
						static	RoutineDescriptor myDlgProcHookRD = BUILD_ROUTINE_DESCRIPTOR (uppModalFilterProcInfo, DlgFilterProc);
						::Alert (kPickInputFileFormat_AlertID, &myDlgProcHookRD);
					#else
						::Alert (kPickInputFileFormat_AlertID, DlgFilterProc);
					#endif
	
					return qLedFirstIndex + (sPopupItemChosen-1);
				#endif
			}
	const char* fFileName;

		#if		qMacOS
			static	pascal Boolean	DlgFilterProc (DialogPtr dialog, EventRecord* theEvent, short* itemHit)
			{
				Handle	h;
				short	i;
				Rect	r;
		
				::GetDialogItem (dialog, 5, &i, &h, &r);
				if (sFirstTime) {
					MenuHandle		mm	=	GetPopUpMenuHandle ((ControlHandle)h);
					for (size_t i = 0; i < sFormats.GetLength (); i++) {
						Str255	tmp;
						tmp[0] = strlen (sFormats[i + qLedFirstIndex]);
						memcpy (&tmp[1], sFormats[i + qLedFirstIndex], tmp[0]);
						AppendMenu (mm, "\pHiMom");
						SetMenuItemText (mm, i+1, tmp);
					}
					::SetControlMinimum ((ControlHandle)h, 1);
					::SetControlMaximum ((ControlHandle)h, i);
					::SetControlValue ((ControlHandle)h, 1);
					sFirstTime = false;
				}
				else {
					sPopupItemChosen = ::GetControlValue ((ControlHandle)h);
				}

				return StdFilterProc (dialog, theEvent, itemHit);
			}
		#endif

};
#if		qMacOS
	bool	MyReader::sFirstTime;
	Led_Array<const char*> 	MyReader::sFormats;
	int	MyReader::sPopupItemChosen;
#endif












	/*
	 * New file format (VERSION 2 of LedPartKind - as of November 1, 1995),
	 *	is:
	 *		MAGIC COOKIE
	 *		LENGTH OF TEXT (ULONG)
	 *		TEXT (variable length)
	 *		LENGTH OF STYLE RUN LIST (number of elemenbts - stored as ULONG)
	 *		Actual Style run elements, stored as PortableStyleRunData
	 *		MAGIC COOKIE - AGAIN - JUST FOR GOOD LUCK!!!
	 *
	 *	Note this format is TEMPORARY, since it makes style run elements HUGE - so we can write font names
	 *	out instead of IDs so we are portable across mac and even to windows systems. Now we are careful
	 *	to byteswap all this shit.
	 */
	typedef	char	LedFormatMagicCookie[16];
	const	LedFormatMagicCookie	kLedPartFormatVersion_2_MagicNumber	=	"\01Version2";	// always keep this the same size
	const	LedFormatMagicCookie	kLedPartFormatVersion_3_MagicNumber	=	"\01Version3";	// always keep this the same size

// TMP HACK - later use dictionary of font specs, and store IDs
	struct	PortableStyleRunData {
		char	fFontName[256];
		char	fItalic;
		char	fBold;
		char	fUnderline;
		short	fFontSize;
		size_t	fLength;
	};
	inline	PortableStyleRunData	mkPortableStyleRunData (const StandardStyledTextImager::InfoSummaryRecord& isr)
		{
			PortableStyleRunData	data;
			memset (&data, 0, sizeof data);
			Str255	fontName;
#if 1
			short	fontID		=	0;
			short	fontSize	=	0;
			Style	fontStyle	=	0;
			isr.GetOSRep (&fontID, &fontSize, &fontStyle);
			GetFontName (fontID, fontName);
			memcpy (&data.fFontName, &fontName[1], fontName[0]);
			data.fFontName[fontName[0]] = '\0';
			data.fItalic = (fontStyle & italic);
			data.fBold = (fontStyle & bold);
			data.fUnderline = (fontStyle & underline);
			data.fFontSize = ConvertODSShortToStd (fontSize);
#else
			GetFontName (isr.fFontID, fontName);
			memcpy (&data.fFontName, &fontName[1], fontName[0]);
			data.fFontName[fontName[0]] = '\0';
			data.fItalic = (isr.fFontStyle & italic);
			data.fBold = (isr.fFontStyle & bold);
			data.fUnderline = (isr.fFontStyle & underline);
			data.fFontSize = ConvertODSShortToStd (isr.fFontSize);
#endif
			data.fLength = ConvertODULongToStd (isr.fLength);
			return data;
		}
	inline	StandardStyledTextImager::InfoSummaryRecord	mkInfoSummaryRecordFromPortData (const PortableStyleRunData& srcData)
		{
			Led_IncrementalFontSpecification	fsp;
			Str255	fontName;
			fontName[0] = strlen (srcData.fFontName);
			memcpy (&fontName[1], srcData.fFontName, fontName[0]);
			short	fontNum	=	0;
			::GetFNum (fontName, &fontNum);
#if 1
			fsp.SetOSRep (fontNum, ConvertODSShortFromStd (srcData.fFontSize),
							(srcData.fItalic? italic: 0) |
							(srcData.fBold? bold: 0) |
							(srcData.fUnderline? underline: 0)
							);
#else
			fsp.fFontID = fontNum;
			fsp.fFontIDValid = true;
			fsp.fFontStyle =
							(srcData.fItalic? italic: 0) |
							(srcData.fBold? bold: 0) |
							(srcData.fUnderline? underline: 0)
							;
			fsp.fFontStyleValid = true;
			fsp.fFontSize = ConvertODSShortFromStd (srcData.fFontSize);;
			fsp.fFontSizeValid = true;
#endif

			return (StandardStyledTextImager::InfoSummaryRecord	(fsp, ConvertODULongFromStd (srcData.fLength)));
		}

	inline	void	OutputStandardToStorageUnit (Environment* ev, ODStorageUnit* storageUnit, ODULong n)
		{
			n = ConvertODULongToStd (n);
			StorageUnitSetValue (storageUnit, ev, sizeof(n), (ODValue)&n);
		}
	inline	ODULong InputStandardFromStorageUnit_ULONG (Environment* ev, ODStorageUnit* storageUnit)
		{
			ODULong	buf;
			StorageUnitGetValue(storageUnit, ev, sizeof (buf), &buf);
			return (ConvertODULongFromStd (buf));
		}




class	ODStorageUnitWriterSinkStream : public StyledTextIOWriter_::SinkStream_ {
	public:
		ODStorageUnitWriterSinkStream (ODStorageUnit* storageUnit);	// must be pre-focused!

		override	size_t	current_offset () const;
		override	void	seek_to (size_t to);
		override	void	write (const void* buffer, size_t bytes);

	private:
		ODStorageUnit*	fStorageUnit;
		ODULong			fStartOffset;
		
};
ODStorageUnitWriterSinkStream::ODStorageUnitWriterSinkStream (ODStorageUnit* storageUnit):
	fStorageUnit (storageUnit),
	fStartOffset (storageUnit->GetOffset (SafeGetEnvironment ()))
{
}

size_t	ODStorageUnitWriterSinkStream::current_offset () const
{
	ODULong	curOffset	=	fStorageUnit->GetOffset (SafeGetEnvironment ());
	Led_Assert (curOffset >= fStartOffset);
	return (curOffset - fStartOffset + qLedFirstIndex);
}

void	ODStorageUnitWriterSinkStream::seek_to (size_t to)
{
	Led_Require (to >= qLedFirstIndex);
	fStorageUnit->SetOffset (SafeGetEnvironment (), to - qLedFirstIndex + fStartOffset);
}

void	ODStorageUnitWriterSinkStream::write (const void* buffer, size_t bytes)
{
	StorageUnitSetValue (fStorageUnit, SafeGetEnvironment (), bytes, buffer);
}





class	ODStorageUnitReaderSrcStream : public StyledTextIOReader_::SrcStream_ {
	public:
		ODStorageUnitReaderSrcStream (ODStorageUnit* storageUnit);	// must be pre-focused!

		override	size_t	current_offset () const;
		override	void	seek_to (size_t to);
		override	size_t	read (void* buffer, size_t bytes);
		#if		qMacOS
		override	Handle	GetAUXResourceHandle () const;
		#endif

	private:
		ODStorageUnit*	fStorageUnit;
		ODULong			fStartOffset;
};
ODStorageUnitReaderSrcStream::ODStorageUnitReaderSrcStream (ODStorageUnit* storageUnit):
	fStorageUnit (storageUnit),
	fStartOffset (storageUnit->GetOffset (SafeGetEnvironment ()))
{
}

size_t	ODStorageUnitReaderSrcStream::current_offset () const
{
	ODULong	curOffset	=	fStorageUnit->GetOffset (SafeGetEnvironment ());
	Led_Assert (curOffset >= fStartOffset);
	return (curOffset - fStartOffset + qLedFirstIndex);
}

void	ODStorageUnitReaderSrcStream::seek_to (size_t to)
{
	Led_Require (to >= qLedFirstIndex);
	fStorageUnit->SetOffset (SafeGetEnvironment (), to - qLedFirstIndex + fStartOffset);
}

size_t	ODStorageUnitReaderSrcStream::read (void* buffer, size_t bytes)
{
	return StorageUnitGetValue (fStorageUnit, SafeGetEnvironment (), bytes, buffer);
}

#if		qMacOS
Handle	ODStorageUnitReaderSrcStream::GetAUXResourceHandle () const
{
	return NULL;
}
#endif














	typedef	char	OLD_BACK_COMPAT_EmbeddingTag[10];
	const	OLD_BACK_COMPAT_EmbeddingTag	kOLD_BACK_COMPAT_PictTag	=	"Pict";
	const	OLD_BACK_COMPAT_EmbeddingTag	kOLD_BACK_COMPAT_ODEmbedTag	=	"ODEmbed";



/*
 ********************************************************************************
 *********************************** LedContent *********************************
 ********************************************************************************
 */
LedContent::LedContent (LedPart* editor) 
{
	fLedPart = editor;
	fShapeList = new COrderedList ();

	fStyleDatabase.SpecifyTextStore (&fTextStore);

	fStyleDatabase.AddReference ();	// so never goes to zero and so never destroyed
									// actually, since it is our direct member it is destroyed
									// when WE are, and not when any of the imagers is destroyed
}

LedContent::~LedContent()
{
	fStyleDatabase.SpecifyTextStore (NULL);
	delete fShapeList;
}

COrderedList* LedContent::GetShapeList() const
{
	return fShapeList;
}

void	LedContent::Externalize (ODStorageUnit* storageUnit, CloneInfo* info)
{

// Unclear why this must be done first, and just this way, with no other externalize, but if we do
// any externalize first (or after?) we hang. So just stick with this!!!
if (info != NULL and info->fScopeFrame != NULL) {
	Environment*	ev	=	SafeGetEnvironment ();
	Embedding* tShape = fLedPart->LookupFrameEditor (info->fScopeFrame).IsOneEmbeddedShape (info->fFrom, info->fTo);
	if (tShape != NULL) {
		ODFrame* embeddedFrame = tShape->GetEmbeddedFacet(info->fScopeFrame)->GetFrame(ev);		
		ExternalizeSingleEmbeddedFrame(storageUnit, info, embeddedFrame);
		return;
	}
}

	ExternalizeNativeContent (storageUnit, info);


	ExternalizePlainText (storageUnit, info);
}



class	NativeFormatWriterWithODEmebddings : public StyledTextIOWriter_LedNativeFileFormat {
	public:
		NativeFormatWriterWithODEmebddings (
					ODStorageUnit* storageUnit, CloneInfo* info,
					SrcStream_* srcStream, SinkStream_* sinkStream
				):
			StyledTextIOWriter_LedNativeFileFormat (srcStream, sinkStream),
			fStorageUnit (storageUnit),
			fInto (info)
			{
			}
		override	void	ExternalizeEmbedding (SimpleEmbeddedObjectStyleMarker*	embedding)
			{
				if (Embedding*	e	=	dynamic_cast<Embedding*>(embedding)) {
					Environment*	ev	=	SafeGetEnvironment ();
//					StorageUnitSetValue (fStorageUnit, ev, sizeof kODEmbedTag, kODEmbedTag);
					// Write the shape
					e->Write(ev, fStorageUnit, fInto);
				}
				else {
					StyledTextIOWriter_LedNativeFileFormat::ExternalizeEmbedding (embedding);
				}
			}
	private:
		ODStorageUnit*	fStorageUnit;
		CloneInfo*		fInto;
};



void	LedContent::ExternalizeNativeContent (ODStorageUnit* storageUnit, CloneInfo* info)
{
	Environment*	ev	=	SafeGetEnvironment ();

	ODSUForceFocus(ev, storageUnit, kODPropContents, kLedPartKind);
	ODBoolean weBeganClone = kODFalse;


	// Externalize the content
	TRY {
		ODDraft* dstDraft = storageUnit->GetDraft(ev);
		// If there is clone info then begin a clone
		if ((info != NULL) && (info->fKey == NULL)) {
			info->fKey = info->fFromDraft->BeginClone (ev, dstDraft, info->fScopeFrame, info->fCloneKind);
			weBeganClone = kODTrue;
		}

		{
			size_t	from	=	(info == NULL)? qLedFirstIndex: info->fFrom;
			size_t	to		=	(info == NULL)? kBadIndex: info->fTo;
			Led_RequireNotNil (storageUnit);

			if (to == kBadIndex) {
				to = GetTextStore ().GetLength () + qLedFirstIndex;	// past end of text
			}
		
			Led_Require (from >= qLedFirstIndex);
			Led_Require (from <= to);
			Led_Require (to <= GetTextStore ().GetLength () + qLedFirstIndex);

			StyledTextIOWriterSrcStream_StandardStyledTextImager	source (&fTextStore, &fStyleDatabase, from, to);
			ODStorageUnitWriterSinkStream							sink (storageUnit);
			NativeFormatWriterWithODEmebddings						textWriter (storageUnit, info, &source, &sink);
			textWriter.Write ();
		}

		if (weBeganClone) {
			info->fFromDraft->EndClone(ev, info->fKey);
		}
	}
	CATCH_ALL {
		// Abort clone operation
		if (weBeganClone)
			info->fFromDraft->AbortClone (ev, info->fKey);
	
		RERAISE;
	}
	ENDTRY
}

void	LedContent::ExternalizePlainText (ODStorageUnit* storageUnit, CloneInfo* info)
{
	Environment*	ev	=	SafeGetEnvironment ();
	ODSUForceFocus(ev, storageUnit, kODPropContents, gGlobals->fScrapTextValue);

	size_t	from	=	(info == NULL)? qLedFirstIndex: info->fFrom;
	size_t	to		=	(info == NULL)? kBadIndex: info->fTo;
	Led_RequireNotNil (storageUnit);

	if (to == kBadIndex) {
		to = GetTextStore ().GetLength () + 1;	// past end of text
	}

	Led_Require (from >= qLedFirstIndex);
	Led_Require (from <= to);
	Led_Require (to <= GetTextStore ().GetLength () + 1);

	// Write the text
	{
		size_t	bufEnd	=	GetTextStore ().GetLength ()+qLedFirstIndex;
		size_t	textTo	=	(to>bufEnd)? bufEnd: to;
		size_t	len	=	textTo-from;
		Led_SmallStackBuffer<Led_tChar>	buf (len);
		GetTextStore ().CopyOut (from, len, buf);
		Led_SmallStackBuffer<Led_tChar>	buf2 (len);
		Led_NLToNative (buf, len, buf2, len);
		StorageUnitSetValue (storageUnit, ev, len, (ODValue)buf2);
	}

	// Write the style runs
	ODSUForceFocus(ev, storageUnit, kODPropContents, gGlobals->fSTYLKind);
	{
		size_t	len	=	to-from;
		Led_Array<StandardStyledTextImager::InfoSummaryRecord>	styleRunInfo	=	GetStyleDatabase ().GetStyleInfo (from, len);
		size_t													nStyleRuns		=	styleRunInfo.GetLength ();
		Led_Assert (offsetof (StScrpRec, scrpStyleTab) == sizeof (short));	// thats why we add sizeof (short)
		StScrpHandle	macStyleHandle	=	(StScrpHandle)::NewHandle (sizeof (short) + nStyleRuns*sizeof (ScrpSTElement));
		HLock (Handle (macStyleHandle));
		(*macStyleHandle)->scrpNStyles = nStyleRuns;
		StandardStyledTextImager::Convert (styleRunInfo, (*macStyleHandle)->scrpStyleTab);
		StorageUnitSetValue (storageUnit, ev, sizeof (short) + nStyleRuns*sizeof (ScrpSTElement), (ODValue)*macStyleHandle);
		HUnlock (Handle (macStyleHandle));
	}
	
}

void	LedContent::ExternalizeSingleEmbeddedFrame (ODStorageUnit* storage, CloneInfo* info, ODFrame* embeddedFrame)
{	
	Environment*	ev	=	SafeGetEnvironment ();

	// We should not be calling this method without valid clone info
	THROW_IF_NULL(info);


	TRY {
		// Start a clone operation
		if (info->fKey==kODNULL) {
			info->fKey = info->fFromDraft->BeginClone(ev, storage->GetDraft(ev), info->fScopeFrame, info->fCloneKind);
		}

		// Add the content frame property now, but don't clone the 
		// frame until after the part is cloned
		storage->AddProperty(ev, kODPropContentFrame);
		storage->AddValue(ev, kODWeakStorageUnitRef);
			
		// Write a kODPropCloneKind so that any embedded shape which chooses
		// to promise can record the cloneKind for use when fulfilling the promise.  Otherwise,
		// a link update of a single embedded frame which promises in CloneInto, will fail
		// when the promise is fulfilled due to using the wrong cloneKind.
		
		// This property/value must be provided so that an embedded frame which writes
		// a promise within our clone block, can fulfill that promise using the same cloneKind
		
		storage->AddProperty(ev, kODPropCloneKindUsed);
		storage->AddValue(ev, kODCloneKind);
		StorageUnitSetValue(storage, ev, sizeof(ODCloneKind), &info->fCloneKind);
		
		// Clone the embedded part
		ODPart* embeddedPart = embeddedFrame->AcquirePart(ev);
		
		info->fFromDraft->Clone(ev, 
									 info->fKey, 
									 embeddedPart->GetID(ev),
									 storage->GetID(ev), 
									 embeddedFrame->GetID(ev));
		ODReleaseObject(ev, embeddedPart);
		
		// Clone the embedded frame to storage; must be done AFTER the part
		// is cloned because the frame strongly references the part
		ODID toFrameID = info->fFromDraft->Clone(ev, 
														info->fKey, 
														embeddedFrame->GetID(ev), 0,
														embeddedFrame->GetID(ev));
														
		// Weakly reference the  frame so it must be explicitly cloned
		// into the receiving draft
		ODSUForceFocus(ev, storage, kODPropContentFrame, kODWeakStorageUnitRef);
		ODStorageUnitRef aSURef; 
		storage->GetWeakStorageUnitRef(ev, toFrameID, aSURef);
		StorageUnitSetValue(storage, ev, sizeof(ODStorageUnitRef), aSURef);

		
		info->fFromDraft->EndClone(ev, info->fKey);
	}
	CATCH_ALL {
		info->fFromDraft->AbortClone(ev, info->fKey);
		RERAISE;
	}
	ENDTRY
}

bool	LedContent::HelpInternalizeOldFormats1to3 (ODStorageUnit* storageUnit, CloneInfo* info, COrderedList* tCollection, size_t insertAt)
{
	Environment*	ev	=	SafeGetEnvironment ();

	storageUnit->Focus(ev, kODPropContents, kODPosUndefined, kLedPartKind, 0, kODPosUndefined);
	LedFormatMagicCookie	cookie;
	StorageUnitGetValue(storageUnit, ev, sizeof (cookie), cookie);
	bool isVersion2 = memcmp (cookie, kLedPartFormatVersion_2_MagicNumber, sizeof (kLedPartFormatVersion_2_MagicNumber)) == 0;
	bool isVersion3 = memcmp (cookie, kLedPartFormatVersion_3_MagicNumber, sizeof (kLedPartFormatVersion_3_MagicNumber)) == 0;
	bool isFormatGood = isVersion2 or isVersion3;


	// if not good, then throw...
	if (not isFormatGood) {
		return false;
	}
	{
		size_t len = InputStandardFromStorageUnit_ULONG (ev, storageUnit);
		Led_SmallStackBuffer<Led_tChar>	buf (len);
		StorageUnitGetValue(storageUnit, ev, len, buf);
		GetTextStore ().Replace (insertAt, insertAt, buf, len);
	
	// REALLY NEED NEW TO TO UPDATE SELECTION AS WELL - FIX THAT LATER!!!
	// QUICKIE HACK COMPUTE OF NEW TO!!!
	size_t	newTo	=	insertAt + len;
	
	// now read the style runs!!!
		{
			size_t	nStyleRuns	=	InputStandardFromStorageUnit_ULONG (ev, storageUnit);
			if (nStyleRuns > len+1) {
				// clearly bogus!!!
				// DO ALERT!
			}
			Led_Assert (storageUnit->GetSize (ev) >= storageUnit->GetOffset (ev));
			size_t	sizeLeft	=	storageUnit->GetSize (ev) - storageUnit->GetOffset (ev);
	
			if (nStyleRuns > sizeLeft/sizeof (PortableStyleRunData)) {
				// clearly bogus!!!
				// DO ALERT!
			}
			Led_SmallStackBuffer<PortableStyleRunData>	portStyleRuns (nStyleRuns);
			StorageUnitGetValue(storageUnit, ev, nStyleRuns*sizeof (PortableStyleRunData), portStyleRuns);
	
			Led_Array<StandardStyledTextImager::InfoSummaryRecord>	styleRunInfo;
			for (size_t i = 0; i < nStyleRuns; i++) {
				StandardStyledTextImager::InfoSummaryRecord	isr	=	mkInfoSummaryRecordFromPortData (portStyleRuns[i]);
				styleRunInfo.Append (isr);
			}
			GetStyleDatabase ().SetStyleInfo (insertAt, newTo, styleRunInfo);
		}
	}


	// Read the embedded objects (including, but not only opendoc embeddings)
	if (isVersion3) {
		size_t	nEmbeddings	=	InputStandardFromStorageUnit_ULONG (ev, storageUnit);

		for (size_t i = 0; i < nEmbeddings; i++) {
			size_t	whereAt			=	InputStandardFromStorageUnit_ULONG (ev, storageUnit);

			size_t	howManyBytes	=	InputStandardFromStorageUnit_ULONG (ev, storageUnit);

			OLD_BACK_COMPAT_EmbeddingTag	tag;
			StorageUnitGetValue (storageUnit, ev, sizeof (tag), tag);

			SimpleEmbeddedObjectStyleMarker*	newEmbedding	=	NULL;
			if (strcmp (tag, kOLD_BACK_COMPAT_PictTag) == 0) {
#if		qSupportPictureAsNativeContent
				Led_SmallStackBuffer<char>	pictBuf (howManyBytes);
				StorageUnitGetValue (storageUnit, ev, howManyBytes, pictBuf);
				newEmbedding = new StandardPictureStyleMarker ((Picture*)(char*)pictBuf);
#else
				// silently skip this guy...
				continue;	//???
#endif
			}
			else if (strcmp (tag, kOLD_BACK_COMPAT_ODEmbedTag) == 0) {
				Embedding*	odEmbedding	=	new Embedding (fLedPart);
				// Read the shape
				odEmbedding->Read(ev, storageUnit, info);
				tCollection->AddFirst(odEmbedding);
				newEmbedding = odEmbedding;
			}
			else {
				// skip bogus entry
				Led_Assert (false);	// just so I get warning... - also must write code to skip over!!!
			}

			GetTextStore ().Replace (insertAt + whereAt, insertAt + whereAt, "", 0);	// erase already read sentinel
			::AddEmbedding (newEmbedding, GetTextStore (), insertAt + whereAt);
		}
	}

	// check for extra cookie at the end...
	StorageUnitGetValue(storageUnit, ev, sizeof (cookie), cookie);

	return true;
}

class	NativeFormatReaderWithODEmebddings : public StyledTextIOReader_LedNativeFileFormat {
	public:
		NativeFormatReaderWithODEmebddings (LedPart* ledPart, ODStorageUnit* storageUnit, CloneInfo* info,
											COrderedList* tCollection, 
											SrcStream_* srcStream, SinkStream_* sinkStream
										):
			StyledTextIOReader_LedNativeFileFormat (srcStream, sinkStream),
			fLedPart (ledPart),
			fStorageUnit (storageUnit),
			fInfo (info),
			fCollection (tCollection)
			{
			}

		override		SimpleEmbeddedObjectStyleMarker*	InternalizeEmbedding (Led_PrivateEmbeddingTag tag, size_t howManyBytes)
			{
				if (strcmp (tag, kODEmbedTag) == 0) {
					Embedding*	odEmbedding	=	new Embedding (fLedPart);
					// Read the shape
					odEmbedding->Read (SafeGetEnvironment (), fStorageUnit, fInfo);
					fCollection->AddFirst (odEmbedding);
					return odEmbedding;
				}
				else {
					return StyledTextIOReader_LedNativeFileFormat::InternalizeEmbedding (tag, howManyBytes);
				}
			}

	private:
		LedPart*		fLedPart;
		ODStorageUnit*	fStorageUnit;
		CloneInfo*		fInfo;
		COrderedList*	fCollection;
};

void	LedContent::InternalizeOurContent (ODStorageUnit* storageUnit,  CloneInfo* info, size_t insertAt)
{
	Environment*	ev	=	SafeGetEnvironment ();

	ODDraft* dstDraft = fLedPart->GetDraft ();

	if (storageUnit->Exists(ev, kODPropContents, kLedPartKind, 0)) {
		ODBoolean weBeganClone = kODFalse;
		
		TRY
			// Start a clone operation, if necessary
			if (info && info->fKey==kODNULL)
			{
				ODDraftKey key = info->fFromDraft->BeginClone(ev, dstDraft, info->fScopeFrame, info->fCloneKind);
				info->fKey = key;
				weBeganClone = kODTrue;
			}
			

COrderedList tCollection;	// tmp hack so below embeding code works...

/// TMP HACK using if test here - use throw better? Rturns false for timing being on bad version data.
// to avoid CW problems with throw - LGP 951230
			if (not HelpInternalizeOldFormats1to3 (storageUnit, info, &tCollection, insertAt)) {
				storageUnit->Focus (ev, kODPropContents, kODPosUndefined, kLedPartKind, 0, kODPosUndefined);
				ODStorageUnitReaderSrcStream					source (storageUnit);
				StyledTextIOSinkStream_StandardStyledTextImager	sink (&fTextStore, &fStyleDatabase, insertAt);
				NativeFormatReaderWithODEmebddings				textReader (fLedPart, storageUnit, info, &tCollection, &source, &sink);
				textReader.Read ();
			}

			if (weBeganClone) {
				info->fFromDraft->EndClone(ev, info->fKey);
				info->fKey = 0;
			}

// I DON'T BELIEVE ANY OF THIS CODE CALLS GETFRAME - SO MAYBE OK TODO INLINE DIRECTLY NOW!!!
			// Can only add shapes to content after the clone has ended
			// because GetFrame can only be called after a clone is complete.
			COrdListIterator iter(&tCollection);
			for (Embedding *shape = (Embedding*)iter.First(); iter.IsNotComplete(); shape = (Embedding*)iter.Next()) {
				AddEmbedding (shape);
			}

		CATCH_ALL
			if (weBeganClone)
			{
				if (info->fKey != 0)
					info->fFromDraft->AbortClone(ev, info->fKey);
			}
		ENDTRY
	}
	else
	{
		Led_Assert (false);	//	Storage unit should have a content property here
	}
	
}

void	LedContent::InternalizeSingleEmbeddedFrame(ODStorageUnit* storageUnit,  CloneInfo* info)
{
	Environment*	ev	=	SafeGetEnvironment ();

	ODDraft* toDraft = fLedPart->GetDraft ();
	ODID newFrameID = 0;
	ODID newPartID = 0;
	ODPart* newPart = kODNULL;
	Rect tShapeRect;
	ODBoolean weBeganClone = kODFalse;


	TRY
		// Gotta be cloning in this case
		THROW_IF_NULL(info);
		
		// Start a clone operation, if necessary
		if (info->fKey==kODNULL)
		{
			info->fKey = info->fFromDraft->BeginClone(ev, toDraft, info->fScopeFrame, info->fCloneKind);
			weBeganClone = kODTrue;
		}
	
		// Clone the part
		ODID newPartID = info->fFromDraft->Clone(ev, info->fKey, storageUnit->GetID(ev), kODNULLID, kODNULLID);
		
		// If there is a content frame then use it
		if (ODSUExistsThenFocus(ev, storageUnit, kODPropContentFrame, kODWeakStorageUnitRef))
		{
			// Clone the frame
			ODStorageUnitRef aSURef;
			StorageUnitGetValue(storageUnit, ev, sizeof(aSURef), aSURef);
			
			if (storageUnit->IsValidStorageUnitRef(ev, aSURef))
			{
				ODID storageUnitID = storageUnit->GetIDFromStorageUnitRef(ev, aSURef);
				newFrameID = info->fFromDraft->Clone(ev, info->fKey, storageUnitID, 0, 0);
			}
			else {
				THROW(kODErrInvalidStorageUnitRef);
			}
		}
		else
		// otherwise, use the suggested frame shape
		if (ODSUExistsThenFocus(ev, storageUnit, kODPropSuggestedFrameShape, kODNULL))
		{
			ODShape* tShape = info->fScopeFrame->CreateShape(ev);
			tShape->ReadShape(ev, storageUnit);
			ODRgnHandle tODRegion = tShape->GetQDRegion(ev);
			tShapeRect = (*tODRegion)->rgnBBox;
			
			// Release acquired geometry
			ODReleaseObject(ev, tShape);
		}
		else
		{
			Led_Assert (false);		//	Storage unit needs content frame of shape here.
		}
			
		// End the clone operation
		if (weBeganClone)
		{
			info->fFromDraft->EndClone(ev, info->fKey);
			info->fKey = 0;
		}
		
		// Get the part 
		newPart = toDraft->AcquirePart(ev, newPartID);
		THROW_IF_NULL(newPart);

		// Create a shape.
		Embedding* tShape = new Embedding (fLedPart);
		tShape->SetBoundingBox(ev, kZeroRect);

		// Size the shape if we read the suggested frame shape property
		if (newFrameID==0)
		{
			tShape->SetBoundingBox(ev, tShapeRect);
		}

		// Embed it
		tShape->Embed(ev, newPart, info->fScopeFrame, newFrameID);


		::AddEmbedding (tShape, GetTextStore (), info->fFrom);

		// Add the shape to the part's content list
		AddEmbedding (tShape);

		// Release acquired part
		ODReleaseObject(ev, newPart);
	
	CATCH_ALL
		// Something is amiss, abort the clone
		if (weBeganClone && info->fKey!=0)
			info->fFromDraft->AbortClone(ev, info->fKey);
		
		// Release acquired part
		ODSafeReleaseObject(newPart);
		
		RERAISE;
	ENDTRY
			
}

void	LedContent::HandleInternalizeContent (ODStorageUnit* storage, CloneInfo* info, size_t insertAt)
{
	Environment*	ev		=	SafeGetEnvironment ();
	ODSession*		session	=	storage->GetSession (ev);

	HFSFlavor	hfsFlavor;

	// Is it a single embedded frame?
	if (storage->Exists(ev, kODPropContentFrame, kODWeakStorageUnitRef, 0))
	{
		InternalizeSingleEmbeddedFrame(storage, info);
	}
	else
	// Is it our native kind?
	if (storage->Exists(ev, kODPropContents, kLedPartKind, 0))
	{
		storage->Focus(ev, kODPropContents, kODPosUndefined, kLedPartKind, 0, kODPosFirstSib);
		InternalizeOurContent(storage, info, insertAt);
	}
	else
#if 1
	// Then try standard Apple Roman TEXT format
	if (ODSUExistsThenFocus(ev, storage, kODPropContents, gGlobals->fScrapTextValue)) {
		InternalizeFromStorageUnitReplacingRange_PlainTextFormatAllFocused_ (storage, insertAt);
	} 
	// And finally check to see if someone dropped a TEXT file, and just slurp in its entire data fork.
	else if (ODSUExistsThenFocus(ev, storage, kODPropContents, gGlobals->fAppleHFSFlavor) and
			(StorageUnitGetValue (storage, ev, sizeof (hfsFlavor), (ODValue)&hfsFlavor),(hfsFlavor.fileType == 'TEXT'))
		) {
		(void)ODSUExistsThenFocus(ev, storage, kODPropContents, gGlobals->fAppleHFSFlavor);
		InternalizeFromStorageUnitReplacingRange_ExternalFileFormatAllFocused_ (storage, insertAt);
	}
	else
#endif

	// We don't know what it is, try to embed
	if (storage->Exists(ev, kODPropContents, kODNULL, 0)) {
		storage->Focus(ev, kODPropContents, kODPosUndefined, kODNULL, 0, kODPosFirstSib);
		InternalizeForeignContent (storage, info);
	}
	
}

void	LedContent::InternalizeFromStorageUnitReplacingRange_PlainTextFormatAllFocused_ (ODStorageUnit *storageUnit, size_t insertAt)
{
// ALL THIS CODE MUST BE CLEANED UP TO TAKE INTO ACCOUNT DATA INPUT VALIDATION!!!!

	Led_RequireNotNil (storageUnit);

	Led_Require (insertAt >= qLedFirstIndex);
	Led_Require (insertAt <= GetTextStore ().GetLength () + 1);

	Environment*	ev = ::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	StScrpHandle		stylHandle = NULL;
	OSErr error			= noErr;
	Size textSize		= 0;

	TRY
		textSize = storageUnit->GetSize(ev);
		Led_SmallStackBuffer<Led_tChar>	textData (textSize);
		StorageUnitGetValue(storageUnit, ev, textSize, (ODValue)textData);
	
		if (ODSUExistsThenFocus (ev, storageUnit, kODPropContents, gGlobals->fSTYLKind)) {
			Size stylSize = storageUnit->GetSize(ev);
			stylHandle = (StScrpHandle) ODNewHandle(stylSize);
			StorageUnitGetValue(storageUnit, ev, stylSize, (ODValue) ODLockHandle((ODHandle) stylHandle));
			ODUnlockHandle((ODHandle) stylHandle);
		}

		StyledTextIOSrcStream_Memory					source (textData, textSize, Handle (stylHandle));
		StyledTextIOSinkStream_StandardStyledTextImager	sink (&GetTextStore (), &GetStyleDatabase (), insertAt);
		MyReader	textReader (&source, &sink, "<from ???>");
		GetTextStore ().Replace (insertAt, insertAt, "", 0);
		textReader.Read ();

		if (stylHandle != NULL) {
			ODDisposeHandle((ODHandle) stylHandle);
		}

	CATCH_ALL
		if (stylHandle != NULL) {
			ODDisposeHandle((ODHandle) stylHandle);
		}
	ENDTRY
}

void	LedContent::InternalizeFromStorageUnitReplacingRange_ExternalFileFormatAllFocused_ (ODStorageUnit *storageUnit, size_t insertAt)
{
	Led_RequireNotNil (storageUnit);

	Led_Require (insertAt >= qLedFirstIndex);
	Led_Require (insertAt <= GetTextStore ().GetLength () + 1);

	Environment*	ev = ::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	StScrpHandle stylHandle = NULL;
	OSErr error				= noErr;
	short fileRefNum		= 0;
	Size textSize			= 0;
	ODPtr textData			= NULL;
	ODPtr fileData			= NULL;
	int		resFile			=	0;

	TRY
// SHOULD ALSO TRY TO READ RESOURCE FORK FOR STYL OR STXT RESOURCES!!!!!!!!!!

		long fileSize = storageUnit->GetSize(ev);		// really hfsflavor size
		fileData = ODNewPtr(fileSize, kDefaultHeapID);			// really hfsflavor data
		StorageUnitGetValue(storageUnit, ev, fileSize, (ODValue)fileData);
		
		THROW_IF_ERROR(::FSpOpenDF((FSSpec*)&(((HFSFlavor*)fileData)->fileSpec), fsRdPerm, &fileRefNum));
		THROW_IF_ERROR(::GetEOF(fileRefNum, &textSize));
		if (textSize == 0)
			THROW(kODErrUndefined);
			
		textData = ODNewPtr(textSize, kDefaultHeapID);

		THROW_IF_ERROR(::SetFPos(fileRefNum,fsFromStart,0));
		THROW_IF_ERROR(::FSRead(fileRefNum,&textSize,textData));
		THROW_IF_ERROR(::FSClose(fileRefNum));

		resFile	=	::FSpOpenResFile ((FSSpec*)&(((HFSFlavor*)fileData)->fileSpec), fsRdPerm);
		stylHandle		=	 (StScrpHandle)::Get1Resource ('styl', 128);

		StyledTextIOSrcStream_Memory					source (textData, textSize, Handle (stylHandle));
		StyledTextIOSinkStream_StandardStyledTextImager	sink (&GetTextStore (), &GetStyleDatabase (), insertAt);
		MyReader	textReader (&source, &sink, "<from ???>");
		GetTextStore ().Replace (insertAt, insertAt, "", 0);
		textReader.Read ();

		if (resFile != 0) {
			::FSClose(resFile);
			resFile = 0;
		}

	CATCH_ALL
		if (fileData != NULL)
			ODDisposePtr(fileData);
		if (stylHandle != NULL)
			ODDisposeHandle((ODHandle) stylHandle);
		if (fileRefNum != 0)
			::FSClose(fileRefNum);
		if (resFile != 0)
			::FSClose(resFile);
	ENDTRY
}

#if		qSupportPictureAsNativeContent
void	LedContent::InternalizeFromStorageUnitReplacingRange_FromPictureAllFocused_ (ODStorageUnit *storageUnit, size_t insertAt)
{
	Led_RequireNotNil (storageUnit);

	Led_Require (insertAt >= qLedFirstIndex);
	Led_Require (insertAt <= GetTextStore ().GetLength () + 1);

	Environment*	ev = ::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	OSErr error			= noErr;
	Size textSize		= 0;

	TRY
		textSize = storageUnit->GetSize(ev);
		Led_SmallStackBuffer<char>	textData (textSize);
		StorageUnitGetValue(storageUnit, ev, textSize, (ODValue)textData);
		SimpleEmbeddedObjectStyleMarker*	marker	=	StandardPictureStyleMarker::mkStandardPictureStyleMarker (Led_ClipboardObjectAcquire::PICT, textData, textSize);

		::AddEmbedding (marker, GetTextStore (), GetStyleDatabase (), insertAt);

	CATCH_ALL
	ENDTRY
}
#endif


void	LedContent::HandleTranslateContent (ODStorageUnit* storage, CloneInfo* info, ODBoolean embedOrMerge)
{
	Environment*	ev		=	SafeGetEnvironment ();

	ODBoolean canMerge = storage->Exists (ev, kODPropContents, kLedPartKind, 0);

	// Is it a single embedded frame? 
	if (storage->Exists(ev, kODPropContentFrame, kODWeakStorageUnitRef, 0)) {
		// Merge if we're forcing a merge and content can be merged.
		if (embedOrMerge || !canMerge) {
			InternalizeSingleEmbeddedFrame(storage, info);
		}
		else {
			InternalizeOurContent(storage, info, qLedFirstIndex);
		}
	}
	else
	// Is it our native kind? And are we not forcing embed?
	if (canMerge && !embedOrMerge) {
		storage->Focus(ev, kODPropContents, kODPosUndefined, kLedPartKind, 0, kODPosFirstSib);
		InternalizeOurContent (storage, info, qLedFirstIndex);
	}
	else
	// We don't know what it is, or we are forcing embed = try to embed
	if (storage->Exists(ev, kODPropContents, kODNULL, 0)) {
		storage->Focus(ev, kODPropContents, kODPosUndefined, kODNULL, 0, kODPosFirstSib);
		InternalizeForeignContent (storage, info);
	}
	
}

void	LedContent::InternalizeForeignContent (ODStorageUnit* contentSU, CloneInfo* info)
{
	Led_Require (info->fCloneKind != kODCloneFromLink);

	Environment*	ev		=	SafeGetEnvironment ();

	// Prepare to clone the item
	ODDraft*	fromDraft = contentSU->GetDraft(ev);
	ODDraft*	toDraft = fLedPart->GetDraft ();

	ODID		newPartID;
	ODID		newFrameID = kODNULL;
	ODShape*	newFrameShape = kODNULL;

	// These variables are used in a catch block, and so must be 
	// declared volatile
	
	TRY
		// Clone the root storage unit
		info->fKey = fromDraft->BeginClone(ev, toDraft, info->fScopeFrame, info->fCloneKind);
				
		newPartID = fromDraft->Clone(ev, info->fKey, contentSU->GetID(ev), 0, 0);
	
		// If a frame already exists for this part then clone it too.
		if (contentSU->Exists(ev, kODPropContentFrame, kODWeakStorageUnitRef, 0)) {
			ODStorageUnitRef aSURef;
			contentSU->Focus(ev, kODPropContentFrame, kODPosSame, kODWeakStorageUnitRef, 0, kODPosUndefined);
			StorageUnitGetValue(contentSU, ev, sizeof(ODStorageUnitRef), aSURef);
			if ( contentSU->IsValidStorageUnitRef(ev, aSURef) )
				newFrameID = fromDraft->Clone(ev, info->fKey, contentSU->GetIDFromStorageUnitRef(ev, aSURef), 0, 0);				
			else {
				Led_Assert (false);	//	Invalid reference to content frame
			}
		}
		
		fromDraft->EndClone(ev, info->fKey);
	
	CATCH_ALL
		if (info->fKey!=0)
			fromDraft->AbortClone(ev, info->fKey);
	ENDTRY

	// Cloning is done, now embed the part
	ODPart* newPart = toDraft->AcquirePart(ev, newPartID);
	THROW_IF_NULL(newPart);
	
	// Create a content shape
	Embedding* newShape = new Embedding (fLedPart);
	newShape->SetBoundingBox(ev, kZeroRect);

	// We need a shape for the shape ( hee hee )
	// If a frame was not provided then see if there is a suggested frame shape
	if (newFrameID==kODNULL) {
		newFrameShape = info->fScopeFrame->CreateShape(ev);
		THROW_IF_NULL(newFrameShape);
	
		if (contentSU->Exists(ev, kODPropSuggestedFrameShape, kODNULL, 0)) {
			contentSU->Focus(ev, kODPropSuggestedFrameShape, kODPosUndefined, NULL, 1, kODPosUndefined);
			newFrameShape->ReadShape(ev, contentSU);
		}
		else {
			// $$$$$ Need constant
			ODRect tRect;
			tRect.SetInt(0,0,80,80);
			
			newFrameShape->SetRectangle(ev, &tRect);
		}
	
		// Size the shape
		newShape->SetBoundingBox(ev, newFrameShape);
		
		// Release acquired geometry
		ODReleaseObject(ev, newFrameShape);
	}
	else {
		ODFrame* frame = toDraft->AcquireFrame(ev, newFrameID);
		ODShape* frameShape = frame->AcquireFrameShape(ev, kODNULL);
		
		// Size the shape
		newShape->SetBoundingBox(ev, frameShape);
	}
	
	// Embed the part, Embedding::Embed will release the part passed to it.
	newShape->Embed (ev, newPart, info->fScopeFrame, newFrameID);

	::AddEmbedding (newShape, GetTextStore (), info->fFrom);

	// Release the newly created part
	ODReleaseObject(ev, newPart);
	
	// Add the shape to the content list
	AddEmbedding (newShape);
}

ODULong LedContent::Count()
{
	return fShapeList->Count();
}

void 	LedContent::AddEmbedding (Embedding* shape)
{
	fLedPart->AddShape (shape);
}

Led_Array<SimpleEmbeddedObjectStyleMarker*>	LedContent::CollectAllEmbeddingMarkersInRange (const TextStore_& textStore, size_t from, size_t to)
{
	Led_Array<SimpleEmbeddedObjectStyleMarker*>	result;

	/*
	 *	Walk through all the markers in existence, and throw away all our standard
	 *	style markers. This is an inefficeint approach. It would be far
	 *	faster to keep a linked, or doubly linked list of all these guys.
	 *	But this approach saves a bit of memory, and til we see this as a problem, lets just
	 *	live with it.
	 */
	Led_Array<Marker*>		markers	=	textStore.CollectAllMarkersInRange (from, to);
	for (size_t i = 0; i < markers.GetLength (); i++) {
		Marker*	m	=	markers[i+qLedFirstIndex];
		Led_AssertNotNil (m);
		SimpleEmbeddedObjectStyleMarker*	stdEmbedMrk		=	dynamic_cast<SimpleEmbeddedObjectStyleMarker*>(m);
		if (stdEmbedMrk != NULL) {
			result.Append (stdEmbedMrk);
		}
	}
	return result;
}










