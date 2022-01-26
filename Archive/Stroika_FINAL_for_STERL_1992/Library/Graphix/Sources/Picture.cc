/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Picture.cc,v 1.4 1992/09/11 18:41:55 sterling Exp $
 *
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: Picture.cc,v $
 *		Revision 1.4  1992/09/11  18:41:55  sterling
 *		new Shape stuff, got rid of String Peek references
 *
 *		Revision 1.3  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.2  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.15  1992/06/09  12:38:16  lewis
 *		Renamed SmartPointer back to Shared.
 *
 *		Revision 1.14  92/06/09  03:19:39  03:19:39  lewis (Lewis Pringle)
 *		Merged in Sterls changes for Sterls new SmartPointer based ReferenceCounting.
 *		
 *		Revision 1.10  1992/02/12  03:36:07  lewis
 *		Use GDIConfig rather than OSConfig.
 *
 */

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<QuickDraw.h>
#include	<Memory.h>
#include	<OSUtils.h>
#include	<Resources.h>
#include	<SysEqu.h>
#endif	/*qMacGDI*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"File.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"

#include	"Picture.hh"



#if		!qRealTemplatesAvailable
	Implement (Iterator, Picture);
	Implement (Collection, Picture);
	Implement (AbSequence, Picture);
	Implement (Array, Picture);
	Implement (Sequence_Array, Picture);
	Implement (Sequence, Picture);
#endif

#if		!qRealTemplatesAvailable
	Implement (Shared, PictureSharedPart);
#endif


#if		qMacGDI
// make our own tablet here - not necesarily offscreen since we never scribble in it!!!???
static	osGrafPort*	MakeGrafPort ()
	{
		/*
		 * DONT USE WMgrPort!!!
		 * VERY UNSAFE.  ALWAYS CREATE OUR OWN EQUIVILENT.
		 */
		if (GDIConfiguration ().ColorQDAvailable ()) {
			osCGrafPort*	wMgrCPort	=	new osCGrafPort;
			::OpenCPort (wMgrCPort);
//			::CopyRgn (*(struct osRegion***)GrayRgn, wMgrCPort->visRgn); 
			// HERE WE MUST SUBTRACT OUT MENUBAR
			return ((osGrafPort*)wMgrCPort);
		}
		else {
//IS GrayRgn around on old systems?

			osGrafPort*	wMgrPort	=	new osGrafPort;
			::OpenPort (wMgrPort);
//			::CopyRgn (*(struct osRegion***)GrayRgn, wMgrPort->visRgn); 
			// HERE WE MUST SUBTRACT OUT MENUBAR
			return (wMgrPort);
		}
	}
#endif	/*qMacGDI*/


/*
 ********************************************************************************
 ************************************ Picture ***********************************
 ********************************************************************************
 */
const	Picture	kEmptyPicture = Picture ();

Picture::Picture ():
	fSharedPart (Nil)
{
	static	Shared(PictureSharedPart)*	emptyPicture = Nil;
	if (emptyPicture == Nil) {
#if		qMacGDI
		const	osRect	kZeroMacRect	=	 {0,0,0,0};
		osPicture**	anOSPicture = (osPicture**)::NewHandle (sizeof (osPicture));
		if (anOSPicture == Nil) {
			gMemoryException.Raise (MemError ());
		}
		(*anOSPicture)->picSize = 0;
		(*anOSPicture)->picFrame = kZeroMacRect;
#elif	qXGDI
int	anOSPicture = 0;
#endif	/*qMacGDI*/
		emptyPicture = new Shared(PictureSharedPart) (new PictureSharedPart (anOSPicture));
	}
	EnsureNotNil (emptyPicture);

	fSharedPart = *emptyPicture;
}

Picture::Picture (const Picture& fromPicture):
	fSharedPart (fromPicture.fSharedPart)
{
}

#if		qMacGDI
Picture::Picture (osPicture** copyFrom, Boolean duplicate):
	fSharedPart (Nil)
{
	RequireNotNil (copyFrom);
	if (duplicate) {
		OSErr	err = ::HandToHand ((osHandle*)&copyFrom);
		if (err != noErr) {
			gMemoryException.Raise (err);
		}
	}
	fSharedPart = new PictureSharedPart (copyFrom);
}

Picture::Picture (const String& resourceName):
	fSharedPart (Nil)
{
	osStr255	name;
	resourceName.ToPStringTrunc (name, sizeof (name));
	osHandle	h = ::GetNamedResource ('PICT', name);
	if (h == Nil) {
		FileSystem::sResourceNotFound.Raise ();
	}
	::DetachResource (h);
	Assert (::ResError () == noErr);
	fSharedPart = new PictureSharedPart ((struct osPicture**)h);
}

Picture::Picture (int resourceID):
	fSharedPart (Nil)
{
	osHandle	h = ::GetResource ('PICT', resourceID);
	if (h == Nil) {
		FileSystem::sResourceNotFound.Raise ();
	}
	::DetachResource (h);
	Assert (::ResError () == noErr);
	fSharedPart = new PictureSharedPart ((struct osPicture**)h);
}
#endif	/*qMacGDI*/

const Picture& Picture::operator= (const Picture& rhs)
{
	fSharedPart = rhs.fSharedPart;
	return (*this);
}

Picture::~Picture ()
{
}

Rect	Picture::GetFrame () const
{
#if		qMacGDI
	return (os_cvt ((*PeekOSRepresentation ())->picFrame));
#endif	/*qMacGDI*/
}

#if		qMacGDI
const osPicture**	Picture::PeekOSRepresentation () const
{
	EnsureNotNil (fSharedPart->fPicHandle);
	return (fSharedPart->fPicHandle);
}

osPicture**	Picture::GetOSRepresentation ()
{
	Assure1Reference ();
	return ((osPicture**)PeekOSRepresentation ());
}
#endif	/*qMacGDI*/

PixelMap	Picture::ToPixelMap () const
{
#if		qMacGDI
// If I get wrapping tablets around pixmaps working again, then this can be simpilfied, and
// renederd less memory intensive!!!

	Rect		localFrame	=	Rect (kZeroPoint, GetFrame ().GetSize ());
	PixelMap	pm			=	PixelMap (localFrame);

	Try {
		Tablet	t			=	Tablet (GetFrame ().GetSize (), 1);

		Try {
			t.DrawPicture (*this, localFrame);
			t.BitBlit (localFrame, pm, localFrame, eCopyTMode);
		}
		Catch () {
			t.Tablet::~Tablet ();
			_this_catch_->Raise ();
		}
	}
	Catch () {
			pm.PixelMap::~PixelMap ();
		_this_catch_->Raise ();
	}
	return (pm);
#endif	/*qMacGDI*/
}

Region	Picture::ToRegion () const
{
	return (Region (ToPixelMap ()));		// probably should show more care for exceptions here!
}

void	Picture::Assure1Reference ()
{
	if (fSharedPart.CountReferences () != 1) {
		BreakReferences ();
	}
}

void	Picture::BreakReferences ()
{
	/*
	 * This is usually called when someone is about to change the underlying Picture, and multiple
	 * Picture have references to the shared part. We must break these reference so only the
	 * intended copy gets changed.
	 */
	Require (fSharedPart.CountReferences () > 1);

	Shared(PictureSharedPart)		oldSharedPart	=	fSharedPart;
	fSharedPart = Nil;		// in case of failure...

#if		qMacGDI
	osPicture**	newPicHandle	=	oldSharedPart->fPicHandle;
	OSErr	err = ::HandToHand ((osHandle*)&newPicHandle);
	if (err != noErr) {
		gMemoryException.Raise (err);
	}
	fSharedPart = new PictureSharedPart (newPicHandle);
#endif	/*qMacGDI*/
	Ensure (fSharedPart.CountReferences () == 1);
}

Boolean	operator!= (const Picture& lhs, const Picture& rhs)
{
#if		qMacGDI
	return Boolean (lhs.PeekOSRepresentation () != rhs.PeekOSRepresentation ());
#endif	/*qMacGDI*/
}

Boolean	operator== (const Picture& lhs, const Picture& rhs)
{
#if		qMacGDI
	return Boolean (lhs.PeekOSRepresentation () == rhs.PeekOSRepresentation ());
#endif	/*qMacGDI*/
}



/*
 ********************************************************************************
 ******************************** iostream support ******************************
 ********************************************************************************
 */

class ostream&	operator<< (class ostream& out, const Picture& picture)
{
#if		qMacGDI
	osHandle	osRep	=	(osHandle)picture.PeekOSRepresentation ();
	size_t		size	=	size_t (::GetHandleSize (osRep));
	::HLock (osRep);
	WriteBytes (out, size, *osRep);
	::HUnlock (osRep);					// really should save state of handle...
#endif	/*qMacGDI*/
	return (out);
}

class istream&	operator>> (class istream& in, Picture& picture)
{
#if		qMacGDI
	size_t	size	=	0;
	void*	data	=	Nil;
	if (ReadBytes (in, size, data)) {
		osPicture**	aPicHandle = (osPicture**)::NewHandle (size);
		if (aPicHandle == Nil) {
			gMemoryException.Raise (MemError ());
		}
		::HLock (osHandle (aPicHandle));
		memcpy (*aPicHandle, data, size);
		::HUnlock (osHandle (aPicHandle));
		delete (data);
		picture = Picture (aPicHandle, False);
		Assert (in.good ());
		return (in);
	}
#endif	/*qMacGDI*/
	return (in);
}


/*
 ********************************************************************************
 ******************************** PictureTablet *********************************
 ********************************************************************************
 */

#if		!qMacGDI
PictureTablet::PictureTablet (const Point& size, UInt8 depth) :
	Tablet ((PixelMap*)Nil)
{
	AssertNotImplemented ();
}
#endif	/*!qMacGDI*/

#if		qMacGDI
PictureTablet::PictureTablet (const Point& size, UInt8 depth):
//	Tablet (size, depth),		temp hack
	Tablet (MakeGrafPort ()),
	fTempPic (Nil)
{
	Require (short (size.GetH ()) == size.GetH ());		// no roundoff
	Require (short (size.GetV ()) == size.GetV ());		// no roundoff
	Tablet::xDoSetPort (GetOSGrafPtr ());
	::PortSize (short (size.GetH ()), short (size.GetV ()));
	osRect	osr;
	os_cvt (Rect (kZeroPoint, size), osr);

#if 0	
// STERL, Why not use PICT2 - Let me know?
	if (GDIConfiguration ().ColorQDAvailable ()) {
		OpenCPicParams	params;
		params.srcRect = osr;
		params.hRes = 72;
		params.vRes = 72;
		params.version = -2;
		params.reserved1 = 0;
		params.reserved2 = 0;
		fTempPic = ::OpenCPicture (&params);
	}
	else {
		fTempPic = ::OpenPicture (&osr);
	}
#else
	fTempPic = ::OpenPicture (&osr);
#endif
	AssertNotNil (fTempPic);			// sb throw...
}
#endif	/*qMacGDI*/

#if		qMacGDI
PictureTablet::PictureTablet (struct osGrafPort* osGrafPtr):
	Tablet (osGrafPtr),
	fTempPic (Nil)
{
	AssertNotReached ();
}
#endif	/*qMacGDI*/

Region	PictureTablet::GetBounds () const
{
	return (Tablet::GetBounds ());
}

Picture	PictureTablet::GetResult ()
{
#if		qMacGDI
	::ClosePicture ();
	Picture	result	=	Picture (fTempPic, False);
//	fTempPic = ::OpenPicture (&osr);		// in case someone tries to take another picture...
	fTempPic = Nil;	// hack for now...
	return (result);
#endif	/*qMacGDI*/
}






/*
 ********************************************************************************
 ******************************** PictureSharedPart *****************************
 ********************************************************************************
 */

#if		qMacGDI
PictureSharedPart::PictureSharedPart (osPicture** osPict):
#elif	qXGDI
PictureSharedPart::PictureSharedPart (int osPict):
#endif	/*GDI*/
	fPicHandle (osPict)
{
}

PictureSharedPart::~PictureSharedPart ()
{
#if		qMacGDI
	RequireNotNil (fPicHandle);
	::KillPicture (fPicHandle);
#endif	/*qMacGDI*/
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

