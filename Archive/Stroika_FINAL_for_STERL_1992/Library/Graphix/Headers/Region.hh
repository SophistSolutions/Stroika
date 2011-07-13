/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Region__
#define	__Region__

/*
 * $Header: /fuji/lewis/RCS/Region.hh,v 1.5 1992/11/25 22:42:44 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *		What do we do about making shapes into regions?
 *		On Mac, can say FrameOval () to get an oval into a region.
 *		Can fake this in X, with bitmaps, and probably elsewhere.
 *		Whats the interface.  Does stuff at this level need to
 *		know about shapes???  Then shapes need to be low level!
 *		maybe not so bad...
 *
 *		Get rid of friend functions. Instead peek at OSREgion as optimization
 *		instead of sharedPart.
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: Region.hh,v $
 *		Revision 1.5  1992/11/25  22:42:44  lewis
 *		Started conversion to new Genclass based template support.
 *
 *		Revision 1.4  1992/09/11  18:36:49  sterling
 *		use new Shared implementation
 *
 *		Revision 1.3  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/08  01:37:44  lewis
 *		Rename PointInside -> Contains for consistency with Rectangle/Shape.
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.15  1992/06/09  12:37:44  lewis
 *		Renamed SmartPointer back to ReferenceCounted.
 *
 *		Revision 1.14  92/06/09  14:51:57  14:51:57  sterling (Sterling Wight)
 *		use templates for references counting
 *		
 *		Revision 1.13  1992/05/23  00:08:51  lewis
 *		Included ReferenceCounted instead of Memory.hh
 *
 *		Revision 1.12  92/05/13  00:16:30  00:16:30  lewis (Lewis Pringle)
 *		Comment on RegionSharedPart::DTOR.
 *		
 *		Revision 1.11  92/05/12  23:51:41  23:51:41  lewis (Lewis Pringle)
 *		Made region DTOR, and Get/Peek OS Region inline.
 *		
 *		Revision 1.10  92/05/10  00:06:19  00:06:19  lewis (Lewis Pringle)
 *		Start qWinGDI support.
 *		
 *		Revision 1.8  1992/02/14  02:40:18  lewis
 *		Moved Region exception into scope, and got rid of g++ backward compatability.
 *		Also, added static member function PrintStatistics () if we are keeping them.
 *
 *
 *
 *
 */

#include	"Debug.hh"
#include	"Exception.hh"
#include	"Shared.hh"

#include	"Rect.hh"



#if		qMacGDI || qXGDI
struct	osRegion;
#endif	/*GDI*/
#if		qMacGDI
typedef	osRegion**	osRegionRep;
#elif	qXGDI
typedef	osRegion*	osRegionRep;
#elif	qWinGDI
typedef	short		osRegionRep;
#endif	/*GDI*/


class	RegionSharedPart;

#if		!qRealTemplatesAvailable
	#include "TFileMap.hh"
	#include SharedOfRegionSharedPart_hh
#endif

class	PixelMap;
class	Region {
	public:
		Region ();
		Region (const Rect& r);
		Region (const Region& r);
		Region (const PixelMap& p);

#if		qMacGDI || qXGDI
		Region (const osRegionRep anOSRegion);		// copies representation
#endif	/*GDI*/

		~Region ();

		nonvirtual	Region&	operator= (const Region& rhs);

		nonvirtual	Boolean	Empty () const;
		nonvirtual	Rect	GetBounds () const;
		nonvirtual	void	SetBounds (const Rect& newBounds);

		nonvirtual	Boolean	Contains (const Point& p) const;

		nonvirtual	const Region& 	InsetBy (const Point& by);

		nonvirtual	const Region& 	operator+= (const Point& delta);
		nonvirtual	const Region& 	operator+= (const Region& rhs);
		nonvirtual	const Region& 	operator-= (const Point& delta);
		nonvirtual	const Region& 	operator-= (const Region& rhs);
		nonvirtual	const Region& 	operator*= (const Region& rhs);
		nonvirtual	const Region& 	operator^= (const Region& rhs);


	public:
		static	Exception	sException;


#if		qKeepRegionAllocStatistics
	public:
		static	void	PrintStatictics ();
#endif


#if		qMacGDI || qXGDI
	public:
		nonvirtual	osRegionRep			GetOSRegion ();				// Back door access.
		nonvirtual	const osRegionRep	PeekAtOSRegion () const;	// Back door access.
#endif	/*GDI*/


	public:
		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);


	private:
#if		qRealTemplatesAvailable
		Shared<RegionSharedPart>	fSharedPart;
#else
		Shared(RegionSharedPart)	fSharedPart;
#endif

		nonvirtual	void	Assure1Reference ();
		nonvirtual	void	BreakReferences ();

	friend	Boolean	operator== (const Region& lhs, const Region& rhs);
	friend	Boolean	operator!= (const Region& lhs, const Region& rhs);
	friend	Region	operator+ (const Region& lhs, const Region& rhs);
	friend	Region	operator- (const Region& lhs, const Region& rhs);
	friend	Region	operator* (const Region& lhs, const Region& rhs);		// Intersection
	friend	Region	operator^ (const Region& lhs, const Region& rhs);		// XOR
};
extern	const	Region	kEmptyRegion;
extern	const	Region	kBigRegion;

extern	Region	operator+ (const Region& lhs, const Point& rhs);
extern	Region	operator- (const Region& lhs, const Point& rhs);

extern	Boolean	operator== (const Region& lhs, const Region& rhs);
extern	Boolean	operator!= (const Region& lhs, const Region& rhs);

extern	Region	operator+ (const Rect& lhs, const Rect& rhs);

extern	Region	operator+ (const Region& lhs, const Region& rhs);
extern	Region	operator- (const Region& lhs, const Region& rhs);
extern	Region	operator* (const Region& lhs, const Region& rhs);		// Intersection
extern	Region	operator^ (const Region& lhs, const Region& rhs);		// XOR

extern	Boolean	Intersects (const Region& lhs, const Region& rhs);


/*
 * Stream inserters and extractors.
 */
extern	class ostream&	operator<< (class ostream& out, const Region& r);
extern	class istream&	operator>> (class istream& in, Region& r);




// Moved in front cuz of qMPW_CFRONT_BUG__LoadDumpBreaksNestedClassDefns
class	RegionSharedPart {
	public:
#if		qMacGDI || qXGDI
		osRegionRep	fOSRegion;
#endif	/*qMacGDI || qXGDI*/

		static	void*	operator new (size_t n);
		static	void	operator delete (void* p);

#if		qMacGDI || qXGDI
		nonvirtual	RegionSharedPart (const osRegionRep anOSRegion, Boolean makeCopy = True);
#endif	/*GDI*/

		/*
		 * Note that we are not using virtual DTORs here, and so its important that we be careful not to
		 * keep pointers to this guys base class so DTOR called properly... We really SB private base class
		 * but then we'd need to forward declare half the methods from ReferenceCounted - do eventually...
		 */
		nonvirtual	~RegionSharedPart ();
		nonvirtual	void	Invariant () const;

	protected:
#if		qDebug
		override	void	Invariant_ () const;
#endif	/*qDebug*/
};







/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	void	RegionSharedPart::Invariant () const
{
#if		qDebug
	Invariant_ ();
#endif	/*qDebug*/
}

inline	Region::Region (const Region& r):
	fSharedPart (r.fSharedPart)
{
	fSharedPart->Invariant ();
}

inline	Region::~Region ()
{
}
	
inline	osRegionRep	Region::GetOSRegion ()				
{	
	Assure1Reference ();
	fSharedPart->Invariant ();
	return (fSharedPart->fOSRegion);		
}
	
inline	const osRegionRep	Region::PeekAtOSRegion () const				
{	
	fSharedPart->Invariant ();
	return (fSharedPart->fOSRegion);		
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Region__*/

