/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/Led_MFC_WordProcessor.cpp,v 2.22 2003/12/31 16:52:37 lewis Exp $
 *
 * Changes:
 *	$Log: Led_MFC_WordProcessor.cpp,v $
 *	Revision 2.22  2003/12/31 16:52:37  lewis
 *	change DoWriteToOLE1Stream arg to Byte** from void** so we can avoid a cast before doing delete (and avoid warning of illegal delete of void*)
 *	
 *	Revision 2.21  2003/04/29 18:21:20  lewis
 *	SPR#1450: quirky hack so embedded widgets show up immediately at the right size
 *	
 *	Revision 2.20  2003/04/03 16:41:27  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated command classes, just builtin to TextInteractor/WordProcessor (and instead of template params use new TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.19  2003/04/01 18:17:59  lewis
 *	SPR#1322: Added 'invalidRect' argument to StyledTextImager::StyleMarker::DrawSegment ()
 *	and subclasses to be able to do logical clipping in tables
 *	
 *	Revision 2.18  2003/03/27 15:57:05  lewis
 *	cleanup ifdefed out code
 *	
 *	Revision 2.17  2003/03/16 16:15:31  lewis
 *	SPR#1345 - as a small part of this SPR - switch to using SimpleUpdater in place of
 *	direct calls to DoAboutToUpdateCalls()
 *	
 *	Revision 2.16  2003/01/11 19:28:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.15  2002/11/28 02:31:08  lewis
 *	A couple simple name changes (GetVirtualText->PeekAtVirtualText and TextLayoutBlock_Subset to
 *	TextLayoutBlock_VirtualSubset
 *	
 *	Revision 2.14  2002/11/27 15:58:43  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 2.13  2002/10/30 01:29:50  lewis
 *	SPR#1155 - use Point_Base template for Led_TWIPS_Point
 *	
 *	Revision 2.12  2002/05/06 21:33:46  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.11  2001/11/27 00:29:52  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.10  2001/09/12 17:23:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  2001/09/06 20:25:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.8  2001/08/31 21:04:35  lewis
 *	SPR#0995- Skip WordProcessorCommonCommandHelper_MFC<BASECLASS,CMD_INFO,CMD_ENABLER>::OnPaint ()
 *	handling when reading in OLE objects (but re-inval the area). Speed tweek.
 *	
 *	Revision 2.7  2001/08/29 23:36:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2001/08/28 18:43:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  2001/08/16 18:53:42  lewis
 *	SPR#0959- more work on 'GetInfo' support for embeddings.
 *	
 *	Revision 2.4  2001/07/31 15:13:55  lewis
 *	use static_cast<> to silence warning
 *	
 *	Revision 2.3  2001/05/04 17:08:50  lewis
 *	SPR#0902: use TextImager::GetEffectiveDefaultTextColor () instead of
 *	Led_GetTextColor ()/Led_GetTextBackgroundColor ()
 *	
 *	Revision 2.2  2000/06/13 16:26:14  lewis
 *	SPR#0767 - last tidbit - unknown embeddings - and RTFOLE embeddings - must use Led_TWIPS_Point
 *	
 *	Revision 2.1  2000/04/24 17:29:23  lewis
 *	SPR#0743- added new Led_MFC_WordProcessor module - and broke some stuff from Led_MFC out and into it
 *	
 *
 *
 *	<========== CODE based on Led_MFC.cpp - moved some code here ==========>
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<climits>

#include	<afxext.h>
#include	<afxole.h>

#include	"Led_MFC_WordProcessor.h"







#if		qLedUsesNamespaces
	namespace	Led {
#endif




#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4786)		//qQuiteAnnoyingDebugSymbolTruncationWarnings
	#pragma	warning (4 : 4800)		//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif








#if		qSupportOLEControlEmbedding


	inline	SIZE	HIMETRICtoDP (SIZE s)
		{
			// Crazy convention of passing NULL as this, used in MFC alot and the routine
			// checks for NULL this??? - WOW!
			((CDC*)NULL)->HIMETRICtoDP (&s);
			return s;
		}
	inline	SIZE	DPtoHIMETRIC (SIZE s)
		{
			// Crazy convention of passing NULL as this, used in MFC alot and the routine
			// checks for NULL this??? - WOW!
			((CDC*)NULL)->DPtoHIMETRIC (&s);
			return s;
		}


/*
 ********************************************************************************
 ***************************** Led_MFC_ControlItem ******************************
 ********************************************************************************
 */
COleDocument*	Led_MFC_ControlItem::DocContextDefiner::sDoc	=	NULL;

// Note sure this is the right type????
const	Led_ClipFormat			Led_MFC_ControlItem::kClipFormat		=	(CLIPFORMAT)::RegisterClipboardFormat (_T("Object Descriptor"));
const	Led_PrivateEmbeddingTag	Led_MFC_ControlItem::kEmbeddingTag		=	"OLE2Embed";



IMPLEMENT_SERIAL(Led_MFC_ControlItem, COleClientItem, 0)

Led_MFC_ControlItem::Led_MFC_ControlItem (COleDocument* pContainer):
	COleClientItem (pContainer),
	fSize (Led_Size (0, 0))
{
}

Led_MFC_ControlItem::~Led_MFC_ControlItem ()
{
}

SimpleEmbeddedObjectStyleMarker*	Led_MFC_ControlItem::mkLed_MFC_ControlItemStyleMarker (const char* embeddingTag, const void* data, size_t len)
{
	Led_Require (
				memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0 or
				memcmp (embeddingTag, RTFIO::RTFOLEEmbedding::kEmbeddingTag, sizeof (RTFIO::RTFOLEEmbedding::kEmbeddingTag)) == 0
				);

	Led_RequireNotNil (DocContextDefiner::GetDoc ());	// See doc in header for class DocContextDefiner.
														// must declare one of these on call stack before calling this
														// method...
	Led_MFC_ControlItem*	e = new Led_MFC_ControlItem (DocContextDefiner::GetDoc ());

	return mkLed_MFC_ControlItemStyleMarker_ (embeddingTag, data, len, e);
}

SimpleEmbeddedObjectStyleMarker*	Led_MFC_ControlItem::mkLed_MFC_ControlItemStyleMarker (ReaderFlavorPackage& flavorPackage)
{
	Led_RequireNotNil (DocContextDefiner::GetDoc ());	// See doc in header for class DocContextDefiner.
														// must declare one of these on call stack before calling this
														// method...
	Led_MFC_ControlItem*	e = new Led_MFC_ControlItem (DocContextDefiner::GetDoc ());

	return mkLed_MFC_ControlItemStyleMarker_ (flavorPackage, e);
}



	struct	MyOLEStream_input : OLESTREAM {
		OLESTREAMVTBL	theVTbl;
		const byte*	start;
		const byte*	end;
		const byte*	cur;
		static	DWORD	__stdcall	MyOLE1STREAMGetter (LPOLESTREAM lpoleStr, void* data, DWORD nb)
			{
				MyOLEStream_input*	myStream	=	(MyOLEStream_input*)lpoleStr;
				DWORD			bytesLeft		=	myStream->end - myStream->cur;
				DWORD			bytesToRead		=	min (bytesLeft, nb);
				memcpy (data, myStream->cur, bytesToRead);
				myStream->cur += bytesToRead;
				return bytesToRead;
			}
		MyOLEStream_input (const void* data, size_t nBytes):
			start ((BYTE*)data),
			end ((BYTE*)data + nBytes),
			cur ((BYTE*)data)
			{
				lpstbl = &theVTbl;
				theVTbl.Get = MyOLE1STREAMGetter;
				theVTbl.Put = NULL;
			}
	};
SimpleEmbeddedObjectStyleMarker*	Led_MFC_ControlItem::mkLed_MFC_ControlItemStyleMarker_ (const char* embeddingTag, const void* data, size_t len, Led_MFC_ControlItem* builtItem)
{
	Led_Require (
				memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0 or
				memcmp (embeddingTag, RTFIO::RTFOLEEmbedding::kEmbeddingTag, sizeof (RTFIO::RTFOLEEmbedding::kEmbeddingTag)) == 0
				);
// Need todo try/catch eror handling...??? Or does caller delete embedding? On error? Decide!!!
//&&&&&&&
	if (memcmp (embeddingTag, kEmbeddingTag, sizeof (kEmbeddingTag)) == 0) {
		Led_SmallStackBuffer<char>	buf (len);
		CMemFile	memFile ((unsigned char*)data, len);
		CArchive	archive (&memFile, CArchive::load);
		builtItem->Serialize (archive);
	}
	else if (memcmp (embeddingTag, RTFIO::RTFOLEEmbedding::kEmbeddingTag, sizeof (RTFIO::RTFOLEEmbedding::kEmbeddingTag)) == 0) {
		/*
		 * NB: The RTF 1.4 spec says the contents are in a format where all you need todo is to call
		 *	::OleLoadFromStream. But this is apparantly not so. The data is apparantly in OLE1 format, and
		 *	so must be converted via OleConvertOLESTREAMToIStorage (). This  - in turn - is extrememly badly
		 *	documented. But I've managed to cobble something together which appears to work.
		 *
		 *	Then - there is the matter of patching all the right MFC COleClientItem member variables so they
		 *	are all happy. MFC (4.2? version with MSVC50) doesn't make this very clear. But what I did was trace
		 *	through the code which was executed during a "Serialize" call. And I do most of that stuff.
		 */

		// Magic from COleClientItem::Serialize()
		builtItem->m_dwItemNumber = builtItem->GetNewItemNumber ();
		builtItem->GetItemStorage ();


		// Build my own OLE1 OLESTREAM
		MyOLEStream_input	myStream (data, len);

		// Docs don't seem to say passing NULL for DVTARGETDEVICE, but - seems to work - and I don't know which one to use?
		builtItem->CheckGeneral (::OleConvertOLESTREAMToIStorage (&myStream, builtItem->m_lpStorage, NULL));

		Led_Assert (Led_MFC_ControlItem::DocContextDefiner::sWindowsWhichHadDisplaySupressed.empty ());	// if someplace ELSE is adding these windows,
																										// we must be careful - else could be using bogus windows!
		/* 
		 *	See COleClientItem::ReadFlat code - next query OleXXX interface, and assign that to some
		 *	data member, and then call finish create???
		 *
		 *	NB: unlike ReadFlat, we cannot call OleLoadFromStream - since this appears to assume an OLE2 format stream.
		 */
		IUnknown*	pUnk	=	NULL;
		builtItem->CheckGeneral (::OleLoad (builtItem->m_lpStorage, IID_IUnknown, builtItem->GetClientSite (), (LPVOID*)&pUnk));
		Led_Assert (pUnk != NULL);

		{
			/*
			 *	Building OLE controls CAN cause the MFC COleMessageFilter to pass WM_PAINT messages to us. That is a big
			 *	performance no-no (reading in large files, because it disables the IsWholeWindowInvalid () optimization).
			 *	So - inside of WordProcessorCommonCommandHelper_MFC<BASECLASS,CMD_INFO>::OnPaint () we check
			 *	and see that we are not creating OLE objects, and if we are, then skip the paint event and save the window
			 *	so we can RE-INVALIDATE it (here).
			 */
			for (set<HWND>::const_iterator i = Led_MFC_ControlItem::DocContextDefiner::sWindowsWhichHadDisplaySupressed.begin ();
				i != Led_MFC_ControlItem::DocContextDefiner::sWindowsWhichHadDisplaySupressed.end ();
				++i) {
				Led_Verify (::InvalidateRect (*i, NULL, true) != 0);
			}
			Led_MFC_ControlItem::DocContextDefiner::sWindowsWhichHadDisplaySupressed.clear ();
		}

		if (pUnk->QueryInterface (IID_IOleObject, (LPVOID*)&builtItem->m_lpObject) != S_OK) {
			pUnk->Release ();
			throw bad_alloc ();
		}
		pUnk->Release ();

		if (not builtItem->FinishCreate (S_OK)) {
			AfxThrowArchiveException (CArchiveException::generic);
		}

		SIZE	size;
		builtItem->GetExtent (&size);
		builtItem->fSize = AsLedSize (HIMETRICtoDP (size));
	}
	return builtItem;
}

SimpleEmbeddedObjectStyleMarker*	Led_MFC_ControlItem::mkLed_MFC_ControlItemStyleMarker_ (ReaderFlavorPackage& flavorPackage, Led_MFC_ControlItem* builtItem)
{
	Led_MFC_ControlItem*	e = builtItem;


// MUST BE MORE EXCPETION FRIENDLY HERE!!! - DELETE e if FIALURE AND ALSO BE CAREFUL OF CLIP OWNER!!!
// LGP 960412

	ReaderClipboardFlavorPackage*	rcfp = dynamic_cast<ReaderClipboardFlavorPackage*> (&flavorPackage);
	if (rcfp != NULL) {
		// fairly complex what MFC does here... Rather than reproduce it all, close the clipboard, and let MFC take care of it all.
		// Reopen it so we our later close code doesn't fail...
		HWND	oldClipOwner	=	::GetOpenClipboardWindow ();
		Led_AssertNotNil (oldClipOwner);
		(void)::CloseClipboard ();

		// Let MFC do the OLE clipboard copy...
		e->CreateFromClipboard ();

		// now restore things so we don't fail later on our attempt to close...
		(void)::OpenClipboard (oldClipOwner);
	}
	else {
		Led_MFCReaderDAndDFlavorPackage* 	dndfp = dynamic_cast<Led_MFCReaderDAndDFlavorPackage*> (&flavorPackage);
		Led_AssertNotNil (dndfp);	// if not from clip, must be from Drag and Drop!
		if (e->CreateFromData (dndfp->GetOleDataObject ()) == 0) {
			Led_ThrowBadFormatDataException ();
		}
	}

	{
		CSize	size;
		if (e->GetCachedExtent (&size)) {
			e->fSize = AsLedSize (HIMETRICtoDP (size));
		}
	}

	return e;
}

void	Led_MFC_ControlItem::OnChange (OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID (this);

	COleClientItem::OnChange (nCode, dwParam);
	// When an item is being edited (either in-place or fully open)
	//  it sends OnChange notifications for changes in the state of the
	//  item or visual appearance of its content.

	// TODO: invalidate the item by calling UpdateAllViews
	//  (with hints appropriate to your application)

	GetDocument ().UpdateAllViews (NULL);
	// for now just update ALL views/no hints
}

BOOL	Led_MFC_ControlItem::OnChangeItemPosition (const CRect& rectPos)
{
	ASSERT_VALID (this);

	// During in-place activation Led_MFC_ControlItem::OnChangeItemPosition
	//  is called by the server to change the position of the in-place
	//  window.  Usually, this is a result of the data in the server
	//  document changing such that the extent has changed or as a result
	//  of in-place resizing.
	//
	// The default here is to call the base class, which will call
	//  COleClientItem::SetItemRects to move the item
	//  to the new position.

	if (!COleClientItem::OnChangeItemPosition (rectPos)) {
		return FALSE;
	}

	if (fSize != AsLedSize (rectPos.Size ())) {
		TextStore&					textStore	=	*GetOwner ()->PeekAtTextStore ();
		TextStore::SimpleUpdater	updater (textStore, GetStart (), GetEnd ());
		fSize = AsLedSize (rectPos.Size ());
	}

	return TRUE;
}

void	Led_MFC_ControlItem::OnGetItemPosition (CRect& rPosition)
{
	ASSERT_VALID (this);

	// During in-place activation, Led_MFC_ControlItem::OnGetItemPosition
	//  will be called to determine the location of this item.  The default
	//  implementation created from AppWizard simply returns a hard-coded
	//  rectangle.  Usually, this rectangle would reflect the current
	//  position of the item relative to the view used for activation.
	//  You can obtain the view by calling Led_MFC_ControlItem::GetActiveView.

	// return correct rectangle (in pixels) in rPosition
	rPosition = AsCRect (GetActiveView ().GetCharWindowLocation (GetStart ()));
}

BOOL	Led_MFC_ControlItem::DoVerb (LONG nVerb, CView* pView, LPMSG lpMsg)
{
	BOOL	result	=	COleClientItem::DoVerb (nVerb, pView, lpMsg);
	if (nVerb == OLEIVERB_SHOW or nVerb == OLEIVERB_OPEN) {
		SIZE	size;
		const_cast<Led_MFC_ControlItem*>(this)->GetExtent (&size);
		if (fSize != AsLedSize (HIMETRICtoDP (size))) {
			TextStore&					textStore	=	*GetOwner ()->PeekAtTextStore ();
			TextStore::SimpleUpdater	updater (textStore, GetStart (), GetEnd ());
			fSize = AsLedSize (HIMETRICtoDP (size));

			// SPR#1450 - quirky hack so embedded widgets show up immediately at the right size.
			const_cast<Led_MFC_ControlItem*>(this)->SetExtent (size);
		}
	}
	return result;
}

void	Led_MFC_ControlItem::OnActivate ()
{
    // Allow only one inplace activate item per frame
	Led_MFC&	pView = GetActiveView ();
	COleClientItem* pItem = GetDocument ().GetInPlaceActiveItem (&pView);
	if (pItem != NULL && pItem != this) {
		pItem->Close ();
    }
    COleClientItem::OnActivate ();
}

void	Led_MFC_ControlItem::OnDeactivateUI (BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI (bUndoable);

    // Hide the object if it is not an outside-in object
    DWORD dwMisc = 0;
    m_lpObject->GetMiscStatus (GetDrawAspect(), &dwMisc);
    if (dwMisc & OLEMISC_INSIDEOUT) {
        DoVerb (OLEIVERB_HIDE, NULL);
	}
}

void	Led_MFC_ControlItem::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
											Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										)
{
	Led_Require (to-from == 1);
	Led_Require (text.PeekAtVirtualText ()[0] == kEmbeddingSentinalChar);

	Led_Color	foreColor	=	imager->GetEffectiveDefaultTextColor (TextImager::eDefaultTextColor);;
	Led_Color	backColor	=	imager->GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor);
	Led_Win_Obj_Selector	pen (tablet, ::GetStockObject (WHITE_PEN));
	tablet->SetTextColor (foreColor.GetOSRep ());
	tablet->SetBkColor (backColor.GetOSRep ());

	Led_Rect	ourBoundsRect = drawInto;
	ourBoundsRect.right = ourBoundsRect.left + fSize.h + 2*kDefaultEmbeddingMargin.h;
	Led_Coordinate	embedBottom	=	useBaseLine;
	Led_Coordinate	embedTop	=	embedBottom - fSize.v;
	Led_Assert (embedTop >= drawInto.top);
	Led_Assert (embedBottom <= drawInto.bottom);
	Led_Rect	innerBoundsRect = Led_Rect (Led_Point (embedTop, drawInto.left + kDefaultEmbeddingMargin.h), fSize);
	
	if (pixelsDrawn != NULL) {
		*pixelsDrawn = ourBoundsRect.GetWidth ();
	}

	COleClientItem::Draw (Led_MFC_CDCFromTablet (tablet), CRect (AsRECT (innerBoundsRect)));
}

void	Led_MFC_ControlItem::MeasureSegmentWidth (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to,
												const Led_tChar* text,
												Led_Distance* distanceResults
											) const
{
	Led_Assert (from + 1 == to);
	Led_Assert (text[0] == kEmbeddingSentinalChar);
	distanceResults[0]	=	fSize.h + 2*kDefaultEmbeddingMargin.h;
}

Led_Distance	Led_MFC_ControlItem::MeasureSegmentHeight (const StyledTextImager* /*imager*/, const RunElement& /*runElement*/, size_t from, size_t to) const
{
	Led_Assert (from + 1 == to);
	return fSize.v + 2*kDefaultEmbeddingMargin.v;
}

void	Led_MFC_ControlItem::Write (SinkStream& sink)
{
	CMemFile	memFile;
	CArchive	archive (&memFile, CArchive::store);
	Serialize (archive);
	archive.Close ();
	size_t	nBytes	=	static_cast<size_t> (memFile.GetLength ());
	BYTE*	written	=	memFile.Detach ();
	if (written != NULL) {
		sink.write (written, nBytes);
		::free (written);
	}
}

void	Led_MFC_ControlItem::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
	WriterClipboardFlavorPackage*	wcfp = dynamic_cast<WriterClipboardFlavorPackage*> (&flavorPackage);
	if (wcfp != NULL) {
		// fairly complex what MFC does here... Rather than reproduce it all, close the clipboard, and let MFC take care of it all.
		// Reopen it so we our later close code doesn't fail...
		HWND	oldClipOwner	=	::GetOpenClipboardWindow ();
		Led_AssertNotNil (oldClipOwner);
		(void)::CloseClipboard ();
		
		// Let MFC do the OLE clipboard copy...
		CopyToClipboard ();

		// now restore things so we don't fail later on our attempt to close...
		(void)::OpenClipboard (oldClipOwner);
	}
	else {
		Led_MFCWriterDAndDFlavorPackage* 	dndfp = dynamic_cast<Led_MFCWriterDAndDFlavorPackage*> (&flavorPackage);
		Led_AssertNotNil (dndfp);
		GetClipboardData (dndfp->GetOleDataSource ());
	}
}

const char*	Led_MFC_ControlItem::GetTag () const
{
	return kEmbeddingTag;
}

void	Led_MFC_ControlItem::Serialize (CArchive& ar)
{
	COleClientItem::Serialize (ar);
	if (not ar.IsStoring()) {
		// then grab our size from the GetExtent() method to see the fSize field...
		// Don't need to notify owning textstore etc cuz not yet even owned (probably?).
		SIZE	size;
		const_cast<Led_MFC_ControlItem*>(this)->GetExtent (&size);
		fSize = AsLedSize (HIMETRICtoDP (size));
	}
}

void	Led_MFC_ControlItem::DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo)
{
	if (GetLength () == 0) {
		Led_AssertNotNil (GetOwner ()->PeekAtTextStore ());
		GetOwner ()->PeekAtTextStore ()->RemoveMarker (this);
		Delete ();	// calls 'delete this', after doing much other needed cleanup
	}
	else {
		SimpleEmbeddedObjectStyleMarker::DidUpdateText (updateInfo);
	}	
}

void	Led_MFC_ControlItem::PostCreateSpecifyExtraInfo (Led_TWIPS_Point size)
{
	//					SetExtent (DPtoHIMETRIC (AsSIZE (size)));
	// Cannot call SetExtent() here. Only after we've called Run () - otherwise exception.
	// Really I think all my current logic for deadling with sizes of embeddings is questionable. I just don't
	// understand it very well.
	//
	// Probably, all needs to be redone. Anyhow, for now, setting fSize field seems to work pretty well.
	// LGP 971017
	fSize = Led_Size (Led_CvtScreenPixelsFromTWIPSV (size.v), Led_CvtScreenPixelsFromTWIPSH (size.h));
}

Led_SDK_String	Led_MFC_ControlItem::GetObjClassName ()	const
{
	CLSID	clsid;
	GetClassID (&clsid);
	LPOLESTR	oleStr	=	NULL;
	if (::ProgIDFromCLSID (clsid, &oleStr) == S_OK) {
		Led_SDK_String	result	=	CString (oleStr);
		::CoTaskMemFree (oleStr);
		return result;
	}
	return Led_SDK_String ();
}

	struct	MyOLEStream_output : OLESTREAM {
		OLESTREAMVTBL						theVTbl;
		vector<char>	fData;
		static	DWORD	__stdcall	MyOLE1STREAMPutter (LPOLESTREAM lpoleStr, const void* data, DWORD nb)
			{
				MyOLEStream_output*	myStream	=	(MyOLEStream_output*)lpoleStr;
				typedef	const char*	ci;
				myStream->fData.insert (myStream->fData.end (), ci (data), ci (data) + nb);
				return nb;
			}
		MyOLEStream_output ()
			{
				lpstbl = &theVTbl;
				theVTbl.Get = NULL;
				theVTbl.Put = MyOLE1STREAMPutter;
			}
	};
void	Led_MFC_ControlItem::DoWriteToOLE1Stream (size_t* nBytes, Byte** resultData)
{
	IStorage*			pStorage	=	NULL;
	IPersistStorage*	ips	=	NULL;
	try {
		// Create a tmp storage
		CheckGeneral (::StgCreateDocfile (NULL, STGM_DIRECT | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &pStorage));
		Led_AssertNotNil (pStorage);
		
		// Get IPersistStorage interface, and save data to my storage
		CheckGeneral (m_lpObject->QueryInterface (IID_IPersistStorage, (LPVOID*)&ips));
		Led_AssertNotNil (ips);
		CLSID	myCLSID;
		GetClassID (&myCLSID);
		CheckGeneral (::WriteClassStg (pStorage, myCLSID));
		CheckGeneral (ips->Save (pStorage, false));
		CheckGeneral (ips->SaveCompleted (pStorage));

		// Convert storage into array of bytes
		MyOLEStream_output	myStream;
		CheckGeneral (::OleConvertIStorageToOLESTREAM (pStorage, &myStream));
		*nBytes = myStream.fData.size ();
		*resultData = new Byte [*nBytes];
		(void)::memcpy (*resultData, &*myStream.fData.begin (), *nBytes);
	}
	catch (...) {
		if (pStorage != NULL) {
			pStorage->Release ();
		}
		throw;
	}
	pStorage->Release ();
	ips->Release ();
}

Led_Size	Led_MFC_ControlItem::GetSize ()
{
	return fSize;
}

bool	Led_MFC_ControlItem::HandleOpen ()
{
	(void)DoVerb (OLEIVERB_PRIMARY, NULL);		// invoke in-place editing, or whatever the primary verb for the object is...
	return false;	// eat the open command
}

vector<Led_MFC_ControlItem::PrivateCmdNumber>	Led_MFC_ControlItem::GetCmdNumbers () const
{
	vector<PrivateCmdNumber>	x;
	x.push_back (eOpenCmdNum);
	return x;
}

bool	Led_MFC_ControlItem::IsCmdEnabled (PrivateCmdNumber cmd) const
{
	switch (cmd) {
		case	eOpenCmdNum:	return true;
		default:				return SimpleEmbeddedObjectStyleMarker::IsCmdEnabled (cmd);
	}
}

#if		qDebug
COleDocument&	Led_MFC_ControlItem::GetDocument () const
{
	COleDocument*	result	=	(COleDocument*)COleClientItem::GetDocument();
	Led_EnsureNotNil (result);
	ASSERT_VALID (result);
	Led_Assert (result->IsKindOf (RUNTIME_CLASS (COleDocument)));
	return *result;
}

Led_MFC&	Led_MFC_ControlItem::GetActiveView () const
{
	Led_EnsureNotNil (COleClientItem::GetActiveView());
	Led_AssertMember (COleClientItem::GetActiveView(), Led_MFC);
	return *(Led_MFC*)COleClientItem::GetActiveView();
}
#endif



/*
 ********************************************************************************
 ******************** Led_MFC_ControlItem::DocContextDefiner ********************
 ********************************************************************************
 */

set<HWND>	Led_MFC_ControlItem::DocContextDefiner::sWindowsWhichHadDisplaySupressed;

#endif



#if		qLedUsesNamespaces
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


