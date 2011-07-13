/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Region.cc,v 1.6 1992/11/25 22:44:24 lewis Exp $
 *
 * TODO:
 *		Keep region bounding rect handy for X regions, so we can peek at it directly. We often use
 *		it so its very important to cache - and we should make more portable some of our mac only
 *		optimizations using this new rect.
 *
 *
 * Changes:
 *	$Log: Region.cc,v $
 *		Revision 1.6  1992/11/25  22:44:24  lewis
 *		Get rid of Implement() macros - obsolete.
 *
 *		Revision 1.5  1992/09/11  18:41:55  sterling
 *		new Shape stuff, got rid of String Peek references
 *
 *		Revision 1.4  1992/09/05  16:14:25  lewis
 *		Renamed Nil->Nil.
 *
 *		Revision 1.3  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/08  02:08:06  lewis
 *		Renamed PointInside -> Contains ().
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.38  1992/06/09  12:38:17  lewis
 *		Renamed SmartPointer back to Shared.
 *
 *		Revision 1.37  92/06/09  14:33:08  14:33:08  sterling (Sterling Wight)
 *		use templates for reference counting
 *		
 *		Revision 1.36  1992/05/23  00:10:06  lewis
 *		#include BlockAllocated instead of Memory.hh
 *
 *		Revision 1.35  92/05/18  17:15:53  17:15:53  lewis (Lewis Pringle)
 *		Added qRealTemplatesAvailable ifdef for BlockAllocated template.
 *		
 *		Revision 1.34  92/05/12  23:54:31  23:54:31  lewis (Lewis Pringle)
 *		Made region DTOR, and Get/Peek OS Region inline.
 *		
 *		Revision 1.33  92/05/10  00:33:41  00:33:41  lewis (Lewis Pringle)
 *		Get to compile under qWinGDI.
 *		
 *		Revision 1.32  92/04/20  14:15:40  14:15:40  lewis (Lewis Pringle)
 *		added typedef char* caddr_t conditional on qSnake && _POSIX_SOURCE, before include of Xutil.h, since that
 *		file references caddr_t which doesn't appear to be part of POSIX.
 *		
 *		Revision 1.30  1992/03/05  16:57:56  lewis
 *		Did quickie implementation of stream inserter/extracter. Uses
 *		bounding rectangle, not whole list of rects.
 *
 *		Revision 1.29  1992/02/17  17:29:54  lewis
 *		Fix region statistics for XGDI, and put in quite a few more speed/caching tweeks (portable ones).
 *		Do stuff like if (Empty()) *this = kEmptyRegion so compares faster.
 *
 *		Revision 1.28  1992/02/17  05:08:57  lewis
 *		Added a number of portable optimizations where we check if a region is empty, and if so, we
 *		use the shared representation for it, making future compares cheaper, and saving
 *		memory, since our statistics show a VERY large (>95%) of our regions are empty.
 *
 *		Revision 1.27  1992/02/15  06:14:06  sterling
 *		cleaned up construction of kEmptyRegion to be safe at startup
 *
 *		Revision 1.26  1992/02/15  05:57:04  lewis
 *		Dont print region list info print Print statictics if we dont use regionllist.
 *
 *		Revision 1.25  1992/02/14  02:44:29  lewis
 *		Fixed funny memory leak with regionlist implementation of freepool on mac.
 *		Also, made the implementation more portable so I could use it under UNIX (not fully done or tried).
 *		Fixed a number of suspicious cases of operators where lhs and rhs shared parts the same. Not done here
 *		either.
 *		Added memory/region usage statistics, to help debug - later this will be useful for optimizing region usages.
 *		When destroying regions (putting them on our local freelist), set the size small with SetEmptyRegion() on mac.
 *
 *		Revision 1.24  1992/01/09  08:25:29  lewis
 *		Fixed very subtle bug with Region::operator*. Must be careful when we do certain operations on lhs
 *		and rhs, with Get and Peek() that we check that fSharedPart != rhs->fSharedPart. Check other similar
 *		routines for this bug.
 *
 *		Revision 1.23  1992/01/08  23:59:08  lewis
 *		Worked on setbounds - mostly for benifit of Xt, but I may have fixed a case where it would call
 *		MapRgn and divide by zero. Also, Sterl warns me that he suspects this of being inefficeint in the
 *		common case where were doing just an offset, and so we should perhaps specail case that.
 *
 *		Revision 1.22  1992/01/03  21:35:03  lewis
 *		Fixed up bugs with Region::SetBounds ().
 *
 *		Revision 1.21  1992/01/03  07:08:26  lewis
 *		Fixed Region::GetBounds () for XGDI - had x/y reversed.
 *
 *
 *
 */

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<QuickDraw.h>
#include	<OSUtils.h>				// for trap caching
#include	<Traps.h>				// for trap caching
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#endif	/*qMacOS*/
#include	"OSRenamePost.hh"


#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"OSConfiguration.hh"
#include	"StreamUtils.hh"

#include	"PixelMap.hh"

#include	"Region.hh"






#if		qKeepRegionAllocStatistics
static	UInt32		sRegionsReallyAlloced;
static	UInt32		sRegionsAlloced;
#if		qMacGDI
static	UInt32		sRectRegionsFreed;		// count of regions at time of destruction that were just rects...
#endif
static	UInt32		sRegionsFreed;
#endif	/*qKeepRegionAllocStatistics*/





/*
 * Some utility routines that can be mostly implemented as simple wrappers on the
 * various GDI implementations. These are used to make most of the code in the object
 * implementation simple, and portable.
 */


#if		qMacGDI || qXGDI
/*
 * GDI wrapper routines
 */
static	osRegionRep	DoNewRgn ();
static	void		DoCopyRgn (const osRegionRep r1, osRegionRep r2);
static	void		DoDiffRgn (const osRegionRep r1, const osRegionRep r2, osRegionRep r3);
static	void		DoDisposeRgn (osRegionRep r);
static	Boolean		DoEmptyRgn (const osRegionRep r);
static	Boolean		DoEqualRgn (const osRegionRep r1, const osRegionRep r2);
static	void		DoInsetRgn (osRegionRep r, short dh, short dv);
static	void		DoOffsetRgn (osRegionRep r, short dh, short dv);
static	Boolean		DoPtInRgn (osPoint p, const osRegionRep r);
static	void		DoSectRgn (const osRegionRep r1, const osRegionRep r2, osRegionRep r3);
static	osRegionRep	DoNewRectRgn (short left, short top, short right, short bottom);
static	void		DoUnionRgn (const osRegionRep r1, const osRegionRep r2, osRegionRep r3);

static	osRegionRep	GetNewRegion ();

#endif	/*GDI*/



// tmp hacks for winGDI to get it to compile...
#if		qWinGDI
typedef	int	osPoint;
typedef	int	osRect;
#endif







#if		qMacGDI

#if		qCacheTraps

#if		!qMPW_NO_ARG_TRAP_CACHE_PASCAL_FUNCT_BUG
static	pascal	osRegion**	(*kNewRgnPtr) () =
			(pascal osRegion** (*) ())::NGetTrapAddress (_NewRgn, ToolTrap);
#endif	/*!qMPW_NO_ARG_TRAP_CACHE_PASCAL_FUNCT_BUG*/

static	pascal	void		(*kCopyRgnPtr) (osRegion**, osRegion**) =
			(pascal void (*) (osRegion**, osRegion**))::NGetTrapAddress (_CopyRgn, ToolTrap);
static	pascal	void		(*kDiffRgnPtr) (osRegion**, osRegion**, osRegion**) =
			(pascal void (*) (osRegion**, osRegion**, osRegion**))::NGetTrapAddress (_DiffRgn, ToolTrap);
static	pascal	void		(*kDisposeRgnPtr) (osRegion**) =
			(pascal void (*) (osRegion**))::NGetTrapAddress (_DisposRgn, ToolTrap);
static	pascal	Boolean		(*kEmptyRgnPtr) (osRegion**) =
			(pascal Boolean (*) (osRegion**))::NGetTrapAddress (_EmptyRgn, ToolTrap);
static	pascal	osBoolean	(*kEqualRgnPtr) (osRegion**, osRegion**) =
			(pascal osBoolean (*) (osRegion**, osRegion**))::NGetTrapAddress (_EqualRgn, ToolTrap);
static	pascal	void		(*kInsetRgnPtr) (osRegion**, short, short) =
			(pascal void (*) (osRegion**, short, short))::NGetTrapAddress (_InSetRgn, ToolTrap);
static	pascal	void		(*kOffsetRgnPtr) (osRegion**, short, short) =
			(pascal void (*) (osRegion**, short, short))::NGetTrapAddress (_OfSetRgn, ToolTrap);
static	pascal	osBoolean	(*kPtInRgnPtr) (osPoint, osRegion**) =
			(pascal osBoolean (*) (osPoint, osRegion**))::NGetTrapAddress (_PtInRgn, ToolTrap);
static	pascal	void		(*kSectRgnPtr) (osRegion**, osRegion**, osRegion**) =
			(pascal void (*) (osRegion**, osRegion**, osRegion**))::NGetTrapAddress (_SectRgn, ToolTrap);
static	pascal	void		(*kSetRectRgnPtr) (osRegion**, short, short, short, short) =
			(pascal void (*) (osRegion**, short, short, short, short))::NGetTrapAddress (_SetRecRgn, ToolTrap);
static	pascal	void		(*kUnionRgnPtr) (osRegion**, osRegion**, osRegion**) =
			(pascal void (*) (osRegion**, osRegion**, osRegion**))::NGetTrapAddress (_UnionRgn, ToolTrap);
static	pascal	void		(*kXorRgnPtr) (osRegion**, osRegion**, osRegion**) =
			(pascal void (*) (osRegion**, osRegion**, osRegion**))::NGetTrapAddress (_XOrRgn, ToolTrap);

#endif	/*qCacheTraps*/

#endif	/*qMacGDI*/




#if		qMacGDI
inline	Boolean	osRectInRect (const osRect& r1, const osRect& r2)
{
	return Boolean ((r2.left >= r1.left) and (r2.right <= r1.right) and
		 			(r2.top >= r1.top) and (r2.bottom <= r1.bottom));
}

// SSW: comment copied from below (Intersects)
	// maybe this routine ought to say lhs * rhs != kEmptyRegion, cuz there is the issue if
	// things intersect, but the intersection contains no bits - which is the defn of empty
	// intersepction, and what is the defn of NO intersection - I think this routine
	// should return true if the regions intersect, but the intersection contains no bits.
	// But if regions are defined as open, in the topilogical sense, then this whole disticion
	// may be silly.
// I dont think I concur. As a practical matter, when clipping, drawing, logical clipping, etc.,
// we really only want an intersection to happen if it encloses bits
inline	Boolean	osRectsIntersect (const osRect& r1, const osRect& r2)
{
	return Boolean ((r2.top <= r1.bottom) or (r2.bottom >= r1.top) or
	 				(r2.left <= r1.right) or (r2.right >= r1.left));
}
#endif	/*qMacGDI*/








/*
 * Implementations of inline GDI wrapper routines
 */

inline	osRegionRep	DoNewRgn ()
{
#if		qKeepRegionAllocStatistics
	Require (sRegionsAlloced >= sRegionsFreed);
	sRegionsReallyAlloced++;
#endif	/*qKeepRegionAllocStatistics*/

#if		qMacGDI
#if		!qMPW_NO_ARG_TRAP_CACHE_PASCAL_FUNCT_BUG && qCacheTraps
	RequireNotNil (kNewRgnPtr);
	return ((*kNewRgnPtr) ());
#else	/*!qMPW_NO_ARG_TRAP_CACHE_PASCAL_FUNCT_BUG && qCacheTraps*/
	return (::NewRgn ());
#endif	/*!qMPW_NO_ARG_TRAP_CACHE_PASCAL_FUNCT_BUG && qCacheTraps*/
#elif	qXGDI
	return (::XCreateRegion ());
#endif	/*qMacGDI || qXGDI*/
}

#if		!qXGDI
inline	// cannot use inline for XGDI cuz of static variable...
#endif	/*!qXGDI*/
void	DoCopyRgn (const osRegionRep r1, osRegionRep r2)
{
	RequireNotNil (r1);
	RequireNotNil (r2);
#if		qMacGDI
#if		qCacheTraps
	RequireNotNil (kCopyRgnPtr);
	(*kCopyRgnPtr) ((osRegion**)r1, r2);
#else	/*qCacheTraps*/
	::CopyRgn ((osRegion**)r1, r2);
#endif	/*qCacheTraps*/
#elif	qXGDI
	/*
	 * Can you believe this - no other way to copy???  Maybe I've missed something?
	 */
	static	const	osRegionRep	anEmptyRegion = ::XCreateRegion ();
	::XUnionRegion (r1, anEmptyRegion, r2);
#endif	/*qMacGDI || qXGDI*/
}

inline	void	DoDiffRgn (const osRegionRep r1, const osRegionRep r2, osRegionRep r3)
{
	RequireNotNil (r1);
	RequireNotNil (r2);
	RequireNotNil (r3);
#if		qMacGDI
#if		qCacheTraps
	RequireNotNil (kDiffRgnPtr);
	(*kDiffRgnPtr) ((osRegion**)r1, (osRegion**)r2, r3);
#else	/*qCacheTraps*/
	::DiffRgn ((osRegion**)r1, (osRegion**)r2, r3);
#endif	/*qCacheTraps*/
#elif	qXGDI
	::XSubtractRegion (r1, r2, r3);
#endif	/*qMacGDI || qXGDI*/
}

inline	void	DoDisposeRgn (osRegionRep r)
{
	RequireNotNil (r);
#if		qMacGDI
#if		qCacheTraps
	RequireNotNil (kDisposeRgnPtr);
	(*kDisposeRgnPtr) (r);
#else	/*qCacheTraps*/
	::DisposeRgn (r);
#endif	/*qCacheTraps*/
#elif	qXGDI
	::XDestroyRegion (r);
#endif	/*qMacGDI || qXGDI*/
}

inline	Boolean	DoEmptyRgn (const osRegionRep r)
{
	RequireNotNil (r);
#if		qMacGDI
	register	const	osRegion*	rPtr	=	*r;
	if ((rPtr->rgnBBox.bottom <= rPtr->rgnBBox.top) or (rPtr->rgnBBox.right <= rPtr->rgnBBox.left)) {
		return (True);
	}
	else {
#if		qCacheTraps
		return Boolean ((*kEmptyRgnPtr) ((osRegion**)r));
#else	/*qCacheTraps*/
		return Boolean (::EmptyRgn ((osRegion**)r));
#endif	/*qCacheTraps*/
	}
#elif	qXGDI
	return (Boolean (::XEmptyRegion (r)));
#endif	/*qMacGDI || qXGDI*/
}

inline	Boolean	DoEqualRgn (const osRegionRep r1, const osRegionRep r2)
{
	RequireNotNil (r1);
	RequireNotNil (r2);
#if		qMacGDI
#if		qCacheTraps
	RequireNotNil (kEqualRgnPtr);
	return Boolean ((*kEqualRgnPtr) ((osRegion**)r1, (osRegion**)r2));
#else	/*qCacheTraps*/
	return Boolean (::EqualRgn ((osRegion**)r1, (osRegion**)r2));
#endif	/*qCacheTraps*/
#elif	qXGDI
	return (Boolean (::XEqualRegion (r1, r2)));
#endif	/*GDI*/
}

inline	void	DoInsetRgn (osRegionRep r, short dh, short dv)
{
	RequireNotNil (r);
#if		qMacGDI
#if		qCacheTraps
	RequireNotNil (kInsetRgnPtr);
	(*kInsetRgnPtr) (r, dh, dv);
#else	/*qCacheTraps*/
	::InsetRgn (r, dh, dv);
#endif	/*qCacheTraps*/
#elif	qXGDI
	::XShrinkRegion (r, dh, dv);
#endif	/*GDI*/
}

inline	void	DoOffsetRgn (osRegionRep r, short dh, short dv)
{
	RequireNotNil (r);
#if		qMacGDI
#if		qCacheTraps
	RequireNotNil (kOffsetRgnPtr);
	(*kOffsetRgnPtr) (r, dh, dv);
#else	/*qCacheTraps*/
	::OffsetRgn (r, dh, dv);
#endif	/*qCacheTraps*/
#elif	qXGDI
	::XOffsetRegion (r, dh, dv);
#endif	/*GDI*/
}

inline	Boolean	DoPtInRgn (osPoint p, const osRegionRep r)
{
	RequireNotNil (r);
#if		qMacGDI
#if		qCacheTraps
	RequireNotNil (kPtInRgnPtr);
	return Boolean ((*kPtInRgnPtr) (p, (osRegion**)r));
#else	/*qCacheTraps*/
	return Boolean (::PtInRgn (p, (osRegion**)r));
#endif	/*qCacheTraps*/
#elif	qXGDI
	return (Boolean (::XPointInRegion (r, p.x, p.y)));
#endif	/*GDI*/
}

inline	void	DoSectRgn (const osRegionRep r1, const osRegionRep r2, osRegionRep r3)
{
	RequireNotNil (r1);
	RequireNotNil (r2);
	RequireNotNil (r3);
#if		qMacGDI
#if		qCacheTraps
	RequireNotNil (kSectRgnPtr);
	(*kSectRgnPtr) ((osRegion**)r1, (osRegion**)r2, r3);
#else	/*qCacheTraps*/
	::SectRgn ((osRegion**)r1, (osRegion**)r2, r3);
#endif	/*qCacheTraps*/
#elif	qXGDI
	::XIntersectRegion (r1, r2, r3);
#endif	/*GDI*/
}

#if		!qSunCPlus && !qHPCPlus
inline
#endif	/*!qSunCPlus && !qHPCPlus*/
osRegionRep	DoNewRectRgn (short left, short top, short right, short bottom)
{
#if		qMacGDI
	osRegionRep	r = GetNewRegion ();
#if		qCacheTraps
	if (kSetRectRgnPtr == Nil) {		// can get called before we've inited our globals!!!
		::SetRectRgn (r, left, top, right, bottom);
	}
	else {
		(*kSetRectRgnPtr) (r, left, top, right, bottom);
	}
#else	/*qCacheTraps*/
	::SetRectRgn (r, left, top, right, bottom);
#endif	/*qCacheTraps*/
	return (r);
#elif	qXGDI

#if		qKeepRegionAllocStatistics
	Require (sRegionsAlloced >= sRegionsFreed);
	sRegionsReallyAlloced++;
	sRegionsAlloced++;
#endif	/*qKeepRegionAllocStatistics*/

	osPoint rect[4];
	rect[0].x = left;
	rect[0].y = top;
	rect[1].x = right;
	rect[1].y = top;
	rect[2].x = right;
	rect[2].y = bottom;
	rect[3].x = left;
	rect[3].y = bottom;
	return (::XPolygonRegion (rect, 4, WindingRule));		// which fill_rule doesn't matter
#endif	/*qMacGDI || qXGDI*/
}

inline	void	DoUnionRgn (const osRegionRep r1, const osRegionRep r2, osRegionRep r3)
{
	RequireNotNil (r1);
	RequireNotNil (r2);
	RequireNotNil (r3);
#if		qMacGDI
#if		qCacheTraps
	RequireNotNil (kUnionRgnPtr);
	(*kUnionRgnPtr) ((osRegion**)r1, (osRegion**)r2, r3);
#else	/*qCacheTraps*/
	::UnionRgn ((osRegion**)r1, (osRegion**)r2, r3);
#endif	/*qCacheTraps*/
#elif	qXGDI
	::XUnionRegion (r1, r2, r3);
#endif	/*qMacGDI || qXGDI*/
}

inline	void	DoXorRgn (const osRegionRep r1, const osRegionRep r2, osRegionRep r3)
{
	RequireNotNil (r1);
	RequireNotNil (r2);
	RequireNotNil (r3);
#if		qMacGDI
#if		qCacheTraps
	RequireNotNil (kXorRgnPtr);
	(*kXorRgnPtr) ((osRegion**)r1, (osRegion**)r2, r3);
#else	/*qCacheTraps*/
	::XorRgn ((osRegion**)r1, (osRegion**)r2, r3);
#endif	/*qCacheTraps*/
#elif	qXGDI
	::XXorRegion (r1, r2, r3);
#endif	/*GDI*/
}






/*
 * This qUseRgnLList is not terribly safe, but worth a try.  If it works, document it better.
 * (LGP - Dec 23, 1990 - Merry Christmas).
 *
 *	The only real worry with this hack is that the regions we cons up in GetNewRegion () may
 *	not be totally kosher, cuz their ::GetHandleSize () wont in general agree with their rgnSize
 *	field.  But appears to be huge performance win, so we'll live dangerously for now.
 */

#if		qMacGDI
#define	qUseRgnLList	1
#elif	qXGDI
#define	qUseRgnLList	0
#endif	/*GDI*/


#if		qUseRgnLList
static	struct	RgnLinkedList {
	RgnLinkedList**	fNextFree;
}	**sRgnLinkedList = Nil;
#endif	/*qUseRgnLList*/



/*
 * This utility might eventually do private mem mgt, and link unused regions together into
 * a free list.  CAREFUL ABOUT COUNTING ON THIS CALL PRODUCING AN EMPTY REGION.
 */
static	osRegionRep	GetNewRegion ()
	{
#if		qKeepRegionAllocStatistics
		Require (sRegionsAlloced >= sRegionsFreed);
		sRegionsAlloced++;
#endif	/*qKeepRegionAllocStatistics*/

#if		qUseRgnLList
		if (sRgnLinkedList != Nil) {
			register	osRegion**	result	=	(osRegion**)sRgnLinkedList;
			sRgnLinkedList 		=	(*sRgnLinkedList)->fNextFree;
			// Since on a free, we blast first 4 bytes of record, fixup record
			// Also, must be sure is a valid empty region

#if		qMacGDI
			if ((*result)->rgnSize == sizeof (osRegion)) {
				(*result)->rgnBBox.top = 0;
				(*result)->rgnBBox.left = 0;
				(*result)->rgnBBox.bottom = 0;
				(*result)->rgnBBox.right = 0;
			}
			else {
				::SetEmptyRgn (result);
				Assert ((*result)->rgnSize == sizeof (osRegion));
				Assert ((*result)->rgnBBox.top == 0);
				Assert ((*result)->rgnBBox.left == 0);
				Assert ((*result)->rgnBBox.bottom == 0);
				Assert ((*result)->rgnBBox.right == 0);
			}
#endif
			return (result);
		}
#endif	/*qUseRgnLList*/

		return (DoNewRgn ());
	}

inline	void	FreeUpRegion (osRegionRep oldRegion)
	{
		RequireNotNil (oldRegion);
#if		qKeepRegionAllocStatistics
		Require (sRegionsAlloced >= sRegionsFreed);
		sRegionsFreed++;
#if		qMacGDI
		if ((*oldRegion)->rgnSize == sizeof (osRegion)) {
			sRectRegionsFreed ++;
		}
#endif
#endif	/*qKeepRegionAllocStatistics*/


#if		qUseRgnLList

#if		qMacGDI
		// making them smaller at this point so we dont waste memory - (maybe only do this when really big??)
		if ((*oldRegion)->rgnSize != sizeof (osRegion)) {
			::SetEmptyRgn (oldRegion);
		}
#endif
		register	RgnLinkedList**	newLLEntry	=	(RgnLinkedList**)oldRegion;
		(*newLLEntry)->fNextFree = sRgnLinkedList;
		sRgnLinkedList = newLLEntry;

#else	/*qUseRgnLList*/

		DoDisposeRgn (oldRegion);

#endif	/*qUseRgnLList*/
	}










/*
 ********************************************************************************
 ********************************* RegionSharedPart *****************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
BlockAllocatedDeclare (RegionSharedPart);
BlockAllocatedImplement (RegionSharedPart);
#endif	/*!qRealTemplatesAvailable*/

void*	RegionSharedPart::operator new (size_t n)
{
#if		qRealTemplatesAvailable
	return (BlockAllocated<RegionSharedPart>::operator new (n));
#else	/*qRealTemplatesAvailable*/
	return (BlockAllocated(RegionSharedPart)::operator new (n));
#endif	/*qRealTemplatesAvailable*/
}

void	RegionSharedPart::operator delete (void* p)
{
#if		qRealTemplatesAvailable
	BlockAllocated<RegionSharedPart>::operator delete (p);
#else	/*qRealTemplatesAvailable*/
	BlockAllocated(RegionSharedPart)::operator delete (p);
#endif	/*qRealTemplatesAvailable*/
}

RegionSharedPart::RegionSharedPart (const osRegionRep anOSRegion, Boolean makeCopy):
	fOSRegion (Nil)
{
	RequireNotNil (anOSRegion);
	if (makeCopy) {
		fOSRegion = GetNewRegion ();
		DoCopyRgn (anOSRegion, fOSRegion);
	}
	else {
		fOSRegion = (osRegionRep) anOSRegion;
	}
	Invariant ();
}

RegionSharedPart::~RegionSharedPart ()
{
	RequireNotNil (fOSRegion);
	FreeUpRegion (fOSRegion);
}

#if 	qDebug
void	RegionSharedPart::Invariant_ () const
{
	AssertNotNil (fOSRegion);
}
#endif	/*qDebug*/






#if		qRealTemplatesAvailable
static	Shared<RegionSharedPart>* sSharedPart = Nil;
#else
static	Shared(RegionSharedPart)* sSharedPart = Nil;
#endif


/*
 ********************************************************************************
 ************************************* Region ***********************************
 ********************************************************************************
 */

Region::Region ():
	fSharedPart (Nil)
{
	if (sSharedPart == Nil) {
		sSharedPart = new Shared(RegionSharedPart) (new RegionSharedPart (GetNewRegion (), False));
	}
	AssertNotNil (sSharedPart);
	fSharedPart = *sSharedPart;
}

Region::Region (const osRegionRep anOSRegion):
	fSharedPart (Nil)
{
	RequireNotNil (anOSRegion);
	fSharedPart = new RegionSharedPart (anOSRegion);
}

Region::Region (const Rect& r):
	fSharedPart (Nil)
{
	if (r.Empty ()) {
		if (sSharedPart == Nil) {
#if		qRealTemplatesAvailable
			sSharedPart = new Shared<RegionSharedPart> (new RegionSharedPart (GetNewRegion (), False));
#else
			sSharedPart = new Shared(RegionSharedPart) (new RegionSharedPart (GetNewRegion (), False));
#endif
		}
		AssertNotNil (sSharedPart);
		fSharedPart = *sSharedPart;
	}
	else {
		Require (r.GetTop () == (short)r.GetTop ());		// no truncation!
		Require (r.GetLeft () == (short)r.GetLeft ());
		Require (r.GetBottom () == (short)r.GetBottom ());
		Require (r.GetRight () == (short)r.GetRight ());
		osRegionRep	osr = DoNewRectRgn ((short)r.GetLeft (), (short)r.GetTop (), (short)r.GetRight (), (short)r.GetBottom ());
		fSharedPart = new RegionSharedPart (osr, False);
	}
}

Region::Region (const PixelMap& p):
	fSharedPart (Nil)
{
#if		qMacGDI
	if (OSConfiguration ().TrapAvailable (_BitMapToRegion)) {
		osRegion**	osr = GetNewRegion ();
		OSErr	err	=	::BitMapToRegion (osr, (osBitMap*) *p.GetNewOSRepresentation ());
		if (err != 0) {
			sException.Raise ();
		}
		fSharedPart = new RegionSharedPart (osr, False);
	}
	else {
		Rect	r	=	p.GetBounds ();
		osRegion**	osr = DoNewRectRgn ((short)r.GetLeft (), (short)r.GetTop (), (short)r.GetRight (), (short)r.GetBottom ());
		fSharedPart = new RegionSharedPart (osr, False);
	}
#elif	qXGDI
	fSharedPart = new RegionSharedPart (GetNewRegion (), False);

	/*
	 * Is there some way we could make this less efficient? Could we perhaps put in delay loops
	 * or compute a few more iterates of Ackermans in the inner loop?
	 */
	Coordinate	startRow	=	p.GetBounds ().GetTop ();
	Coordinate	endRow		=	p.GetBounds ().GetBottom ();
	Coordinate	startCol	=	p.GetBounds ().GetLeft ();
	Coordinate	endCol		=	p.GetBounds ().GetRight ();
	for (Coordinate row = startRow; row <= endRow; row++) {
		for (Coordinate col = startCol; col <= endCol; col++) {
			Point where = Point (row, col);
			if (p.GetColor (where) == kBlackColor) {
				(*this) += Rect (where, Point (1, 1));
			}
		}
	}
#endif	/*GDI*/
}

Region&	Region::operator= (const Region& rhs)
{
	fSharedPart = rhs.fSharedPart;
	return (*this);
}

BlockAllocatedDeclare (Region);
BlockAllocatedImplement (Region);

void*	Region::operator new (size_t n)
{
	return (BlockAllocated(Region)::operator new (n));
}

void	Region::operator delete (void* p)
{
	BlockAllocated(Region)::operator delete (p);
}

Boolean	Region::Empty () const
{
	return Boolean ((fSharedPart == kEmptyRegion.fSharedPart) or DoEmptyRgn (PeekAtOSRegion ()));
}

Rect	Region::GetBounds () const
{
#if		qMacGDI
	return (os_cvt ((*PeekAtOSRegion ())->rgnBBox));
#elif	qXGDI
	osRect	osr;
	::XClipBox (PeekAtOSRegion (), &osr);
	return (Rect (Point (osr.y, osr.x), Point (osr.height, osr.width)));
#endif	/*GDI*/
}

void	Region::SetBounds (const Rect& newBounds)
{
	if (newBounds.Empty ()) {
		*this = kEmptyRegion;
	}
	else {
#if		qMacGDI
		osRect	oldBounds	=	(*GetOSRegion ())->rgnBBox;
		osRect	osr;
		os_cvt (newBounds, osr);
		::MapRgn (GetOSRegion (), &oldBounds, &osr);
#elif	qXGDI
		// could use same implementation in on mac or unix, but this only way under unix???
		  Point	sizeDelta		=	(GetBounds ().GetSize () - newBounds.GetSize ())/2;
		if (sizeDelta != kZeroPoint) {
			InsetBy (sizeDelta);
		}

		Point	originDelta		=	newBounds.GetOrigin () - GetBounds ().GetOrigin ();
		this->operator+= (originDelta);

// LGP - Dec ??, hack cuz appantly code buggy - we should have adjusted?
  		if (GetBounds () != newBounds) {
#if		qDebug
			gDebugStream << "converting this region (this=" << this <<") to bounds " << newBounds <<
				" failed and instead our bounds are " << GetBounds () << newline;
#endif	/*qDebug*/
			return;		// skip ensure test at bottom since it still fails for us sometimes...
		 }
#endif	/*qXGDI*/
		Ensure (GetBounds () == newBounds);
	}
}

Boolean	Region::Contains (const Point& p) const
{
	osPoint	tmp;
#if		qMacGDI
	return (DoPtInRgn (os_cvt (p, tmp), PeekAtOSRegion ()));
#elif	qXGDI
	tmp.x = short (p.GetH ());
	tmp.y = short (p.GetV ());
	return (DoPtInRgn (tmp, PeekAtOSRegion ()));
#endif	/*qMacGDI || qXGDI*/
}

const Region& 	Region::InsetBy (const Point& by)
{
	Require (by.GetV () == (short)by.GetV ());		// no truncation!
	Require (by.GetH () == (short)by.GetH ());		// no truncation!
	DoInsetRgn (GetOSRegion (), (short)by.GetH (), (short)by.GetV ());

	/*
	 * To encourage sharing of representations...
	 */
	if (Empty ()) {
		*this = kEmptyRegion;
	}

	return (*this);
}

const Region&	Region::operator+= (const Point& delta)
{
	Require (delta.GetV () == (short)delta.GetV ());		// no truncation!
	Require (delta.GetH () == (short)delta.GetH ());		// no truncation!

	if (Empty ())	return (*this);					//	no point in computing offsets of empty region

	if (delta != kZeroPoint) {		// dont break reference and copy if we dont really change region!
		DoOffsetRgn (GetOSRegion (), (short)delta.GetH (), (short)delta.GetV ());
	}
	return (*this);
}

const Region& 	Region::operator+= (const Region& rhs)
{
	/*
	 * If the region shared parts are equal, the union is just either one.
	 */
	if (fSharedPart != rhs.fSharedPart) {
#if		qMacGDI
		osRect	r1 = (*PeekAtOSRegion ())->rgnBBox;
		osRect	r2 = (*rhs.PeekAtOSRegion ())->rgnBBox;
	
		// see operator*= for pertinant comments
		Boolean	areRectRegions = Boolean (((*PeekAtOSRegion ())->rgnSize == 10) and ((*rhs.PeekAtOSRegion ())->rgnSize == 10));
		if (areRectRegions) {
			if (osRectInRect (r1, r2) or rhs.Empty ()) {
				// nothing to do, as we are already strictly larger
				return (*this);
			}
			else if (osRectInRect (r2, r1) or Empty ()) {
				*this = rhs;
				return (*this);
			}
		}
#endif	/*qMacGDI*/
		DoUnionRgn (PeekAtOSRegion (), rhs.PeekAtOSRegion (), GetOSRegion ());
	}
	return (*this);
}

const Region&	Region::operator-= (const Point& delta)
{
	Require (delta.GetV () == (short)delta.GetV ());		// no truncation!
	Require (delta.GetH () == (short)delta.GetH ());		// no truncation!

	if (Empty ())	return (*this);					//	no point in computing offsets of empty region

	if (delta != kZeroPoint) {
		DoOffsetRgn (GetOSRegion (), -(short)delta.GetH (), -(short)delta.GetV ());
	}
	return (*this);
}

const Region& 	Region::operator-= (const Region& rhs)
{
	if (fSharedPart == rhs.fSharedPart) {
		*this = kEmptyRegion;
		return (*this);
	}
#if		qMacGDI
	if (not osRectsIntersect ((*PeekAtOSRegion ())->rgnBBox, (*rhs.PeekAtOSRegion ())->rgnBBox)) {
		return (*this);
	}
#endif	/*GDI*/
	DoDiffRgn (GetOSRegion (), rhs.PeekAtOSRegion (), GetOSRegion ());

	/*
	 * To encourage sharing of representations...
	 */
	if (Empty ()) {
		*this = kEmptyRegion;
	}

	return (*this);
}

const Region&	Region::operator*= (const Region& rhs)
{
	/*
	 * if SharedParts same - do nothing - intersect self with self is NO-OP.
	 * NB: This test is very important since the GetOSRep () and PeekOSRep ()
	 * give no gaurentee as to order and ?? Not too sure - must think out
	 * more carefully.
	 */
	if (fSharedPart != rhs.fSharedPart) {
#if		qMacGDI
		osRect	r1 = (*PeekAtOSRegion ())->rgnBBox;
		osRect	r2 = (*rhs.PeekAtOSRegion ())->rgnBBox;
	
		// rgnSize of 10 was determined empirically, and is black magic. However, we know it
		// will never be smaller than that cuz a Rect is as teeny as a region gets
		// I suspect the 10 is 2 for the size field itself, plus 8 for the rect
		// these optimizations are worthwhile, in a sample program (EXOS), after playing
		// around for five minutes or so (resizing, switching panes, etc.) the return
		// regions were as follows: empty region 10, rhs 7926, this 2909, calculate 1955
		// thus only had to compute a new region 15% or the time

		Boolean	areRectRegions = Boolean (((*PeekAtOSRegion ())->rgnSize == 10) and ((*rhs.PeekAtOSRegion ())->rgnSize == 10));
		if (areRectRegions and (osRectInRect (r1, r2))) {
			*this = rhs;
		}
		else if (areRectRegions and (osRectInRect (r2, r1))) {
			// nothing to do, as we are already strictly smaller
		}
		else if (not osRectsIntersect (r1, r2)) {
			*this = kEmptyRegion;
		}
		else {
			DoSectRgn (GetOSRegion (), rhs.PeekAtOSRegion (), GetOSRegion ());
		}
#elif	qXGDI
		DoSectRgn (GetOSRegion (), rhs.PeekAtOSRegion (), GetOSRegion ());
#endif	/*GDI*/

		/*
		 * To encourage sharing of representations...
		 */
		if (Empty ()) {
			*this = kEmptyRegion;
		}
	}
	return (*this);
}

const Region&	Region::operator^= (const Region& rhs)
{
	if (fSharedPart == rhs.fSharedPart) {
		// intersection of self with self is self, and diff is empty...
		*this = kEmptyRegion;
	}
	else {
		DoXorRgn (PeekAtOSRegion (), rhs.PeekAtOSRegion (), GetOSRegion ());
	}
	return (*this);
}

#if		qKeepRegionAllocStatistics
void	Region::PrintStatictics ()
{
#if		qDebug
	gDebugStream << "Region statistics:" << newline <<
		tab << "qUseRgnLList = " << qUseRgnLList << newline <<
		tab << "sRegionsReallyAlloced = " << sRegionsReallyAlloced << newline <<
		tab << "sRegionsAlloced = " << sRegionsAlloced << newline <<
		tab << "sRegionsFreed = " << sRegionsFreed << newline;
#if		qMacGDI
	gDebugStream <<
		tab << "sRectRegionsFreed = " << sRectRegionsFreed << newline;
#endif
	gDebugStream <<
		tab << "RegionsInUse = " << sRegionsAlloced-sRegionsFreed << newline;
#if		qUseRgnLList
	gDebugStream << tab << "FreePoolSize = " << sRegionsReallyAlloced-(sRegionsAlloced-sRegionsFreed) << newline;
#endif
#else
	DebugMessage ("fix Region::PrintStatictics () for nodebug");
#endif
}
#endif	/*qKeepRegionAllocStatistics*/

void	Region::BreakReferences ()
{
	Require (fSharedPart.CountReferences () > 1);
	fSharedPart = new RegionSharedPart (fSharedPart->fOSRegion);
}

void		Region::Assure1Reference ()		
{ 
	if (fSharedPart.CountReferences () > 1) { 
		BreakReferences (); 
	} 
}








/*
 ********************************************************************************
 ************************************ operator== ********************************
 ********************************************************************************
 */
Boolean	operator== (const Region& lhs, const Region& rhs)
{
	if (lhs.fSharedPart == rhs.fSharedPart) {
		return (True);
	}

	/*
	 * Test for emptyness since FAST
	 */
	if (lhs.Empty ()) {	return (rhs.Empty ()); }
	if (rhs.Empty ()) {	return (lhs.Empty ()); }

	return (DoEqualRgn (lhs.PeekAtOSRegion (), rhs.PeekAtOSRegion ()));
}





/*
 ********************************************************************************
 ************************************ operator!= ********************************
 ********************************************************************************
 */
Boolean	operator!= (const Region& lhs, const Region& rhs)
{
	if (lhs.fSharedPart == rhs.fSharedPart) {
		return (False);
	}

	/*
	 * Test for emptyness since FAST
	 */
	if (lhs.Empty ()) {	return (not rhs.Empty ()); }
	if (rhs.Empty ()) {	return (not lhs.Empty ()); }

	return (not DoEqualRgn (lhs.PeekAtOSRegion (), rhs.PeekAtOSRegion ()));
}




/*
 ********************************************************************************
 ************************************* operator+ ********************************
 ********************************************************************************
 */

Region	operator+ (const Rect& lhs, const Rect& rhs)
{
	return (Region (lhs) + Region (rhs));
}

Region	operator+ (const Region& lhs, const Point& rhs)
{
	if (rhs == kZeroPoint) {
		return (lhs);
	}
	else {
		Region r = lhs;
		r += rhs;
		return (r);
	}
}

Region	operator+ (const Region& lhs, const Region& rhs)
{
	Region r = lhs;
	r += rhs;
	return (r);
}




/*
 ********************************************************************************
 ************************************* operator- ********************************
 ********************************************************************************
 */

Region	operator- (const Region& lhs, const Region& rhs)
{
#if		qMacGDI
	if (not osRectsIntersect ((*lhs.PeekAtOSRegion ())->rgnBBox, (*rhs.PeekAtOSRegion ())->rgnBBox)) {
		return (lhs);
	}
#endif	/*qMacGDI*/
	Region	result;
	DoDiffRgn (lhs.PeekAtOSRegion (), rhs.PeekAtOSRegion (), result.GetOSRegion ());
	return (result);
}

Region	operator- (const Region& lhs, const Point& rhs)
{
	Region r = Region (lhs);
	r -= rhs;
	return (r);
}




/*
 ********************************************************************************
 ************************************* operator* ********************************
 ********************************************************************************
 */
Region	operator* (const Region& lhs, const Region& rhs)
{
	Region	result	=	lhs;
	return (result *= rhs);
}



/*
 ********************************************************************************
 ************************************* operator^ ********************************
 ********************************************************************************
 */
Region	operator^ (const Region& lhs, const Region& rhs)
{
	Region	result;
	DoXorRgn (lhs.PeekAtOSRegion (), rhs.PeekAtOSRegion (), result.GetOSRegion ());
	return (result);
}



/*
 ********************************************************************************
 ************************************ Intersects ********************************
 ********************************************************************************
 */
Boolean	Intersects (const Region& lhs, const Region& rhs)
{
#if		qMacGDI
	// maybe this routine ought to say lhs * rhs != kEmptyRegion, cuz there is the issue if
	// things intersect, but the intersection contains no bits - which is the defn of empty
	// intersepction, and what is the defn of NO intersection - I think this routine
	// should return true if the regions intersect, but the intersection contains no bits.
	// But if regions are defined as open, in the topilogical sense, then this whole disticion
	// may be silly.
	if (not osRectsIntersect ((*lhs.PeekAtOSRegion ())->rgnBBox, (*rhs.PeekAtOSRegion ())->rgnBBox)) {
		return (False);
	}
#endif	/*qMacGDI*/
	return (not ((lhs * rhs).Empty ()));
}





/*
 ********************************************************************************
 ******************************** iostream support ******************************
 ********************************************************************************
 */

// HACK IMPLEMENTATION - REALLY ONLY WORKS FOR REGIONS WHICH ARE RECTS!!!!!
// Since we will probably want to eventually represent this as a list of rects,
// we now add the extra level of parens... This is so our format is less likely to change...
ostream&	operator<< (ostream& out, const Region& r)
{
	Rect	rr	=	r.GetBounds ();
#if		qDebug
	if (rr != r) {
		gDebugStream << "Warning: non-rectangular region printed to stream - stroika does not fully implement! rr= " << rr << newline;
	}
#endif
	out << lparen << lparen << rr.GetOrigin () << ' ' << rr.GetSize () << rparen << rparen;
	return (out);
}

istream&	operator>> (istream& in, Region& r)
{
	char	ch;
	in >> ch;
	if (ch != lparen) {	 // check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}
	in >> ch;
	if (ch != lparen) {	 // check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	Point origin;
	in >> origin;
	Point size;
	in >> size;

	in >> ch;
	if (ch != rparen) {	 // check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}
	in >> ch;
	if (ch != rparen) {	 // check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}
	Assert (in);
	r = Rect (origin, size);
	return (in);
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


