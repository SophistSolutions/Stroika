/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ClipBoard.cc,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *		-		Fix use of HLock in this file (and elesehwere) to use HGetState/HSetState(), as
 *				the Lock/Unlock style is dangerous!!!
 *
 * Changes:
 *	$Log: ClipBoard.cc,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.2  1992/07/02  04:48:29  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/01/28  20:52:34  lewis
 *		Added ClipBoardEventHandler to handle import/export messages - moved from application.
 *		Also, changed Get method to autoconstruct a clipboard, and added set method to allow others
 *		to override the (subclass the) clipboard. Got rid of some compiler warnings by scoping type references.
 *		Got rid of some old g++ backward compat.
 *		Now this class is independent of Application.
 *
 *		Revision 1.6  1992/01/22  15:18:29  sterling
 *		use TextView
 *
 *		Revision 1.5  1992/01/21  21:01:34  lewis
 *		Use the ForEach () macro for iteration.
 *
 *
 */



#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Memory.h>
#include	<Scrap.h>
#include	<QuickDraw.h>
#endif	/*qMacOS*/
#include	"OSRenamePost.hh"

#include	"StreamUtils.hh"

#include	"EventManager.hh"
#include	"MenuOwner.hh"
#include	"TextView.hh"

#include	"ClipBoard.hh"



#if		!qRealTemplatesAvailable
	Implement (Iterator, ClipCamPtr);
	Implement (Collection, ClipCamPtr);
	Implement (AbBag, ClipCamPtr);
	Implement (Array, ClipCamPtr);
	Implement (Bag_Array, ClipCamPtr);
	Implement (Bag, ClipCamPtr);
#endif	/*!qRealTemplatesAvailable*/





class	ClipBoardEventHandler : public EventHandler {
	public:
		ClipBoardEventHandler ();
		~ClipBoardEventHandler ();

		override	void	ProcessImportClipboard (const osEventRecord& eventRecord);
		override	void	ProcessExportClipboard (const osEventRecord& eventRecord);

	private:
//		typedef	EventHandler	inherited;
};







/*
 ********************************************************************************
 *********************************** ClipBoard **********************************
 ********************************************************************************
 */

ClipBoard*	ClipBoard::sThe	=	Nil;

ClipBoard::ClipBoard ():
	fClipCameras (),
	fPrivateScrap (Nil),
	fLastChangeCountWeKnewAbout (GetScrapChangeCount ()-1)
{
	AddClipCamera (new TEXTClipCamera ());
	AddClipCamera (new PICTClipCamera ());
	fEventHandler = new ClipBoardEventHandler ();
}

ClipBoard::~ClipBoard ()
{
	if (sThe == this) {
		sThe = Nil;			// in case somebody deletes a clipboard which is current...
	}
	if (fPrivateScrap != Nil) {
		delete (fPrivateScrap);
		fPrivateScrap = Nil;
	}
	ForEach (ClipCamPtr, it, fClipCameras) {
		ClipCamera* cc	=	it.Current ();
		AssertNotNil (cc);
		delete cc;
	}
	delete (fEventHandler);
}

CollectionSize	ClipBoard::CountTypes ()
{
	if (fPrivateScrap == Nil) {
		CollectionSize	types	=	1;
		AssertNotReached ();			// NYI
		return (types);
	}
	else {
		return (fPrivateScrap->CountTypes ());
	}
}

ClipBoard::ClipType	ClipBoard::GetIthType (CollectionSize i)
{
	if (fPrivateScrap == Nil) {
		AssertNotReached ();			// NYI
		return (eText);
	}
	else {
		return (fPrivateScrap->GetIthType (i));
	}
}

size_t		ClipBoard::GetIthSize (CollectionSize i)
{
	if (fPrivateScrap == Nil) {
		AssertNotReached ();			// NYI
		return (eText);
	}
	else {
		return (fPrivateScrap->GetIthSize (i));
	}
}

size_t	ClipBoard::GetSize ()
{
	if (fPrivateScrap == Nil) {
#if		qMacOS
		::LoadScrap ();
		// check errors!!!
		ScrapStuff*	s	=	::InfoScrap ();
		AssertNotNil (s);
		AssertNotNil (s->scrapHandle);		// loadscrap should set this!!!
		return (::GetHandleSize (s->scrapHandle));
#endif	/*qMacOS*/
	}
	else {
		return (fPrivateScrap->GetSize ());
	}
}

Boolean	ClipBoard::GetClip (ClipType type, void*& data, size_t& size)
{
	/*
	 * First check private scrap.
	 */
	if (fPrivateScrap != Nil) {
		if (fPrivateScrap->GetClip (type, data, size)) {
			return (True);
		}
	}
#if		qMacOS
	/*
	 * Then check OS scrap.
	 */
	data = Nil;
	size = 0;
	osHandle	h		=	::NewHandle (0);
	long	offset		=	0;
	long	gsResult	=	::GetScrap (h, type, &offset);
	if (gsResult <= 0) {
		// do throw?
		// maybe only return false if noTypeErr - that type not present??
		return (False);
	}
	else {
		Assert (gsResult == ::GetHandleSize (h));
		size = size_t (gsResult);
		data = new char [size];
		::HLock (h);
		memcpy (data, *h, size);
		::HUnlock (h);
		::DisposHandle (h);
		return (True);
	}
#endif	/*qMacOS*/
}

Boolean	ClipBoard::Contains (ClipType type)
{
	if (fPrivateScrap != Nil) {
		if (fPrivateScrap->Contains (type)) {
			return (True);
		}
	}
	else {
#if		qMacOS
		long	offset		=	0;
		long	gsResult	=	::GetScrap (Nil, type, &offset);
		return Boolean (gsResult >= 0);
#endif	/*qMacOS*/
	}
}

void	ClipBoard::ClearClip ()
{
#if		qMacOS
	::ZeroScrap ();
#endif	/*qMacOS*/
	fLastChangeCountWeKnewAbout = GetScrapChangeCount ();
}

void	ClipBoard::AddClip (ClipType type, const void* data, size_t size)
{
#if		qMacOS
	long	err	=	::PutScrap (size, type, osPtr (data));
	Assert (err == noErr);
#endif	/*qMacOS*/
	fLastChangeCountWeKnewAbout = GetScrapChangeCount ();
}

void	ClipBoard::SetPrivateScrap (PrivateScrap* ps)
{
#if		qMacOS
	::ZeroScrap ();				// so scrap change count changes, and system knows we changed the scrap
#endif	/*qMacOS*/
	if (fPrivateScrap != Nil) {
		delete (fPrivateScrap);
	}
	fPrivateScrap = ps;
	fLastChangeCountWeKnewAbout = GetScrapChangeCount ();
}

Picture	ClipBoard::RenderAsPicture ()
{
	/*
	 * First iterate over types on the clipboard, and then for each of those, see if we can make
	 * a picture out of it.
	 */

	// since thats not implemented, fake it for now and iterate over clipcameras...

	Try {
		ForEach (ClipCamPtr, it, fClipCameras) {
			ClipCamera* cc	=	it.Current ();
			AssertNotNil (cc);
			
			volatile	void* data	=	Nil;
			size_t size	=	0;
#if		qMPW_NO_VOLATILES_BUG
			void**	_hack_	=	&data;
#endif	/*qMPW_NO_VOLATILES_BUG*/
			Try {
				if (GetClip (cc->GetTypeSupported (), data, size)) {
					Picture	picture;

					Try {
						if (RenderTypeAsPicture (cc->GetTypeSupported (), data, size, picture)) {
							delete (data); data = Nil;
							return (picture);
						}
						else {
							AssertNotReached ();
							delete (data); data = Nil;
							return (picture);
						}
					}
					Catch () {
						_this_catch_->Raise ();
					}
				}
			}
			Catch () {
				if (data != Nil) {
					delete (data);
				}
				_this_catch_->Raise ();
			}
		}
	}
	Catch () {
AssertNotReached ();	//we must do a different trick to get this baby destroyed!!!
//it.IndirectIterator(ClipCamPtr)::~IndirectIterator(ClipCamPtr) ();
#if		qDebug
		DebugMessage ("caught exception in ClipBoard::RenderAsPicture");
#endif	/*qDebug*/
		_this_catch_->Raise ();
	}

	/*
	 * We dont support that clip type.
	 */
	OStringStream	buf;
	buf << "Unknown item on clipboard # " << GetScrapChangeCount ();
	PictureTablet	pc	=	PictureTablet (Point (40, 100));
	pc.DrawText (buf, kSystemFont, Point (10, 10));
	return (pc.GetResult ());
}

Boolean	ClipBoard::RenderTypeAsPicture (ClipType type, const void* data, size_t size, Picture& picture)
{
	Try {
		ForEach (ClipCamPtr, it, fClipCameras) {
			ClipCamera* cc	=	it.Current ();
			AssertNotNil (cc);
			
			if (cc->GetTypeSupported () == type) {
				picture = cc->ClickClick (data, size);
				return (True);
			}
		}
	}
	Catch () {
AssertNotReached ();// will have to clean up differently!!!
//		it.IndirectIterator(ClipCamPtr)::~IndirectIterator(ClipCamPtr) ();
#if		qDebug
		DebugMessage ("caught exception in ClipBoard::RenderTypeAsPicture");
#endif	/*qDebug*/
		_this_catch_->Raise ();
	}
	return (False);
}

void	ClipBoard::AddClipCamera (ClipCamera* newCamera)
{
	RequireNotNil (newCamera);
	fClipCameras.Add (newCamera);
}

void	ClipBoard::RemoveClipCamera (ClipCamera* oldCamera)
{
	RequireNotNil (oldCamera);
	fClipCameras.Remove (oldCamera);
}

void	ClipBoard::CheckOSScrap ()
{
	/*
	 * If the os changed the scrap, private scrap blown away.  No point in retreiving it
	 * now - wait til somebody - like the ClipWindow - asks.
	 */
	if (GetScrapChangeCount () != fLastChangeCountWeKnewAbout) {
		if (fPrivateScrap != Nil) {
			delete (fPrivateScrap);
			fPrivateScrap = Nil;
		}
	}
}

void	ClipBoard::UpdateOSScrap ()
{
	/*
	 * If we are checking out, and have a private scrap, let the os know.  if it was not
	 * a private scrap, the os would already know.
	 */
	if (fPrivateScrap != Nil) {
		ClearClip ();
		fPrivateScrap->ConvertToPublic ();
		fLastChangeCountWeKnewAbout = GetScrapChangeCount ();
	}
}

UInt32	ClipBoard::GetScrapChangeCount () const
{
#if		qMacOS
	AssertNotNil (::InfoScrap ());
	return (::InfoScrap ()->scrapCount);
#endif	/*qMacOS*/
}

ClipBoard&	ClipBoard::Get ()
{
	if (sThe == Nil) {
		SetThe (new ClipBoard ());
	}
	EnsureNotNil (sThe);
	return (*sThe);
}

void	ClipBoard::SetThe (ClipBoard* newClipBoard)
{
	Require (sThe != newClipBoard or newClipBoard == Nil);	// careful of (Set (Get()) should not be done!!! or at least shouldn't crash
	delete (sThe); Assert (sThe == Nil);
	sThe = newClipBoard;		// can be nil!!!
}







/*
 ********************************************************************************
 ********************************* PrivateScrap *********************************
 ********************************************************************************
 */

PrivateScrap::PrivateScrap ()
{
}

PrivateScrap::~PrivateScrap ()
{
}

Boolean	PrivateScrap::Contains (ClipBoard::ClipType type)
{
	for (CollectionSize	scrapType = 1; scrapType <= CountTypes (); scrapType++) {
		if (GetIthType (scrapType) == type) {
			return (True);
		}
	}
	return (False);
}

size_t	PrivateScrap::GetSize ()
{
	size_t	totalSize = 0;
	for (CollectionSize	scrapType = 1; scrapType <= CountTypes (); scrapType++) {
		totalSize += GetIthSize (scrapType);
	}
	return (totalSize);
}






/*
 ********************************************************************************
 ********************************* PictureScrap *********************************
 ********************************************************************************
 */
PictureScrap::PictureScrap () :
	fPicture ()
{
}

CollectionSize		PictureScrap::CountTypes ()					
{ 
	return (1); 
}

ClipBoard::ClipType	PictureScrap::GetIthType (CollectionSize i)	
{ 
	Require (i == 1); 
	return (ClipBoard::ePicture); 
}

size_t		PictureScrap::GetIthSize (CollectionSize i)	
{ 
	Require (i == 1); 
#if		qMacOS
	return (::GetHandleSize ((osHandle)fPicture.PeekOSRepresentation ()));
#endif	/*qMacOS*/
}

Boolean	PictureScrap::GetClip (ClipBoard::ClipType type, void*& data, size_t& size)
{
	switch (type) {
		case	ClipBoard::ePicture: {
#if		qMacOS
			const	osPicture**	osPict = fPicture.PeekOSRepresentation ();
			size = GetIthSize (1);
			data	=	new char [size];
			::HLock ((osHandle)osPict);
			memcpy (data, *osPict, size);
			::HUnlock ((osHandle)osPict);
#endif	/*qMacOS*/
			return (True);
		}
		break;
		default:
			return (False);
	}
	AssertNotReached (); return (False);
}

void	PictureScrap::ConvertToPublic ()
{
#if		qMacOS
	const	osPicture**	osPict = fPicture.PeekOSRepresentation ();
	::HLock ((osHandle)osPict);
	long	err	=	::PutScrap (GetIthSize (1), GetIthType (1), (osPtr)*osPict);
	::HUnlock ((osHandle)osPict);
	Assert (err == 0);			// SB some kind of exception....
#endif	/*qMacOS*/
}







/*
 ********************************************************************************
 *********************************** ClipCamera *********************************
 ********************************************************************************
 */

ClipCamera::ClipCamera (ClipBoard::ClipType typeSupported):
	fTypeSupported (typeSupported)
{
}

ClipBoard::ClipType	ClipCamera::GetTypeSupported () const
{
	return (fTypeSupported);
}






/*
 ********************************************************************************
 ******************************* TEXTClipCamera *********************************
 ********************************************************************************
 */
TEXTClipCamera::TEXTClipCamera ():
	ClipCamera (ClipBoard::eText)
{
}

Picture		TEXTClipCamera::ClickClick (const void* clipItem, size_t size)
{
	// badly done - want
//	const	Point	kDefaultSize	=	Point (400, 600);	// some random figures - change to one page!!!
	const	Point	kDefaultSize	=	Point (200, 300);	// some random figures - change to one page!!!
// hack defualt size to soemthing small for now since that is all that gets used by the
// clip stuff since it only pays attention to the size of the picture and we cannot change that(easily)


	PictureTablet	pc (kDefaultSize);
	TextView		st;
	st.SetTablet (&pc);
	st.SetText (String (clipItem, size));
	st.SetFont (&kSystemFont);
	st.SetWordWrap (True);
	st.SetSize (st.CalcDefaultSize (kDefaultSize));
#if 1
	st.ProcessLayout ();
	st.Render (st.GetDrawnRegion ());
#else
	st.Update ();	// CAN WE DO THIS??? NOT IN A WINDOW!!!
#endif
	return (pc.GetResult ());
}







/*
 ********************************************************************************
 ******************************* PICTClipCamera *********************************
 ********************************************************************************
 */
PICTClipCamera::PICTClipCamera ():
	ClipCamera (ClipBoard::ePicture)
{
}

Picture		PICTClipCamera::ClickClick (const void* clipItem, size_t size)
{
	RequireNotNil (clipItem);
	RequireNotNil (size > 0);
#if		qMacOS
	osPicture**	osPict	=	(osPicture**)::NewHandle (size);
	HLock ((osHandle)osPict);
	memcpy (*osPict, clipItem, size);
	HUnlock ((osHandle)osPict);
	return (Picture (osPict, False));
#endif	/*qMacOS*/
}










/*
 ********************************************************************************
 ******************************* ClipBoardEventHandler **************************
 ********************************************************************************
 */

ClipBoardEventHandler::ClipBoardEventHandler ():
	EventHandler ()
{
	Activate ();
}

ClipBoardEventHandler::~ClipBoardEventHandler ()
{
	Deactivate ();
}

void	ClipBoardEventHandler::ProcessImportClipboard (const osEventRecord& /*eventRecord*/)
{
	ClipBoard::Get ().CheckOSScrap ();
	MenuOwner::SetMenusOutOfDate ();		// not sure this should be done here???
}

void	ClipBoardEventHandler::ProcessExportClipboard (const osEventRecord& /*eventRecord*/)
{
	ClipBoard::Get ().UpdateOSScrap ();
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


