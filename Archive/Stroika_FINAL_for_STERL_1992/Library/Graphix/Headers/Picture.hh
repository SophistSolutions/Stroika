/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Picture__
#define	__Picture__

/*
 * $Header: /fuji/lewis/RCS/Picture.hh,v 1.4 1992/11/25 22:40:00 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Picture.hh,v $
 *		Revision 1.4  1992/11/25  22:40:00  lewis
 *		Start conversion to new Genclass based template support.
 *
 *		Revision 1.3  1992/09/11  18:36:49  sterling
 *		use new Shared implementation
 *
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.9  1992/06/09  12:37:42  lewis
 *		Renamed SmartPointer back to Shared.
 *
 *		Revision 1.8  92/06/09  03:17:41  03:17:41  lewis (Lewis Pringle)
 *		Merged in Sterls changes for Sterls new SmartPointer based ReferenceCounting.
 *		
 *		Revision 1.7  92/05/13  00:15:06  00:15:06  lewis (Lewis Pringle)
 *		Comment on PictureSharedPart::DTOR.
 *		
 *
 *
 */
#include	"Exception.hh"

#include	"Rect.hh"
#include	"Tablet.hh"
#include	"Region.hh"
#include	"PixelMap.hh"


#if		qMacGDI
struct osPicture;
#endif	/*qMacGDI*/


// Moved in front cuz of qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
		class	PictureSharedPart {
			public:
#if		qMacGDI
				osPicture**	fPicHandle;
#elif	qXGDI
				int		fPicHandle;
#endif	/*qGDI*/

#if		qMacGDI
				nonvirtual	PictureSharedPart (osPicture** osPict);
#elif	qXGDI
				nonvirtual	PictureSharedPart (int osPict);
#endif	/*qGDI*/

				nonvirtual	~PictureSharedPart ();
		};

#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include SharedOfPictureSharedPart_hh
#endif

class	Picture {
	public:
		Picture ();
		Picture (const Picture& fromPicture);
#if		qMacGDI
		Picture (osPicture** copyFrom, Boolean duplicate = True);

		Picture (const String& resourceName);
		Picture (int resourceID);
#endif	/*qMacGDI*/

		const Picture& operator= (const Picture& rhs);
		~Picture ();

		nonvirtual	Rect	GetFrame () const;

#if		qMacGDI
		nonvirtual	osPicture**				GetOSRepresentation ();
		nonvirtual	const osPicture**		PeekOSRepresentation () const;
#endif	/*qMacGDI*/

		nonvirtual	Region		ToRegion () const;
		nonvirtual	PixelMap	ToPixelMap () const;

	private:
#if		qRealTemplatesAvailable
		Shared<PictureSharedPart>	fSharedPart;
#else
		Shared(PictureSharedPart)	fSharedPart;
#endif

		nonvirtual	void	Assure1Reference ();
		nonvirtual	void	BreakReferences ();
};

extern	Boolean	operator!=(const Picture& lhs, const Picture& rhs);
extern	Boolean	operator==(const Picture& lhs, const Picture& rhs);

extern	const	Picture	kEmptyPicture;

/*
 * Stream inserters and extractors.
 */
class ostream;
class istream;
extern	ostream&	operator<< (ostream& out, const Picture& picture);
extern	istream&	operator>> (istream& in, Picture& picture);



class	PictureTablet : public Tablet {
	public:
		PictureTablet (const Point& size, UInt8 depth = eMaxDeviceDepth);

	protected:
#if 	qMacGDI
		// If given a grafPtr, subclasses responsibilty to destroy
		PictureTablet (struct osGrafPort* osGrafPtr);
#endif	/*qMacGDI*/

	public:
		override	Region	GetBounds () const;
		nonvirtual	Picture	GetResult ();

#if		qMacGDI
	private:
		struct osPicture**	fTempPic;
#endif	/*qMacGDI*/
};





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Picture__*/



