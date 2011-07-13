/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Enclosure__
#define	__Enclosure__

/*
 * $Header: /fuji/lewis/RCS/Enclosure.hh,v 1.3 1992/09/05 16:14:25 lewis Exp $
 *
 * Description:
 *
 *		An Enclosure is a entity whose position is defined hierarchicaly. Enclosure can be nested. They provide
 *		access to their subEnclosures (through iterators), and their parent, so that the Enclosure Tree can be
 *		traversed.
 *
 *		LocalToEnclosure's are virtual to allow for differing implementations. Once plausable one is to have origin,
 *		and size fields, and to just return them in the appropriate accessors, and simple add and subtract the origin
 *		in the conversion between local and enclosure. Another possability is to have arbitrary linear transformations
 *		between parent and child. This later approach might be useful in implementing a "Real" coordinates system
 *		in inches, rather than pixels, and give us machinery to convert. We do assume that this transformation is
 *		linear.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Enclosure.hh,v $
 *		Revision 1.3  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/04/07  10:52:08  lewis
 *		Support templates (for PC port).
 *		Also, cleaned up const of arguments to the various functions. BorlandC++ much more strict than the CFront based
 *		and gcc compilers have been. (Really old ones were wrong). Thanx borland!
 *
 *		Revision 1.2  1992/03/05  16:47:36  lewis
 *		Made CTOR/DTOR inline.
 *
 *		Revision 1.1  1992/01/15  09:57:07  lewis
 *		Initial revision
 *
 *
 */

#include	"Iterator.hh"

#include	"Rect.hh"
#include	"Region.hh"


#if		!qRealTemplatesAvailable
class	Enclosure;
typedef	Enclosure*	EnclosurePtr;
Declare (Iterator, EnclosurePtr);
#endif


class	Enclosure {
	protected:
		Enclosure ();

	public:
		virtual ~Enclosure ();

		virtual		Point	GetOrigin () const					=	Nil;
		virtual		Point	GetSize () const					=	Nil;
		virtual		Rect	GetExtent () const					=	Nil;
		nonvirtual	Rect	GetLocalExtent () const;


		virtual		Enclosure*				GetEnclosure () const					=	Nil;
#if		qRealTemplatesAvailable
		virtual		Iterator<Enclosure*>*	GetSubEnclosureIterator () const		=	Nil;
#else
		virtual		Iterator(EnclosurePtr)*	GetSubEnclosureIterator () const		=	Nil;
#endif

		/*
		 * Check if the given Enclosure is an ancestor of this. It is an error to pass nil, or this as
		 * arguments to this function.
		 */
		nonvirtual	Boolean	HasEnclosure (const Enclosure* enclosure) const;


		/*
		 * Find the least Enclosure which is an ancestor of both this, and enclosure. What is meant be
		 * least is that any other enclosure which satisfies this criteria would be an ancestor of the
		 * first. If there is no enclosure satisfying this definition, return Nil (that would mean disjoint
		 * trees).
		 */
		nonvirtual	const	Enclosure*	CommonEnclosure (const Enclosure* enclosure) const;


		/*
		 * Convert from local coordinates to the coordinates of this's enclosure. If this has no enclosure,
		 * the results of this routine may still be defined.
		 */
		nonvirtual	Point	LocalToEnclosure (const Point& p) const;
		nonvirtual	Point	EnclosureToLocal (const Point& p) const;
		nonvirtual	Rect	LocalToEnclosure (const Rect& r) const;
		nonvirtual	Rect	EnclosureToLocal (const Rect& r) const;
		nonvirtual	Region	LocalToEnclosure (const Region& r) const;
		nonvirtual	Region	EnclosureToLocal (const Region& r) const;


		/*
		 * Convert to/from local coordinates to the coordinates of the given named enclosure. It is an
		 * error to convert to the coordinates of an Enclosure that does not exist (ie is not an anteceedent of
		 * this). This call is safe, iff HasEnclosure (ancestor) returns true.
		 */
		nonvirtual	Point	LocalToAncestor (const Point& p, const Enclosure* ancestor) const;
		nonvirtual	Point	AncestorToLocal (const Point& p, const Enclosure* ancestor) const;
		nonvirtual	Rect	LocalToAncestor (const Rect& r, const Enclosure* ancestor) const;
		nonvirtual	Rect	AncestorToLocal (const Rect& r, const Enclosure* ancestor) const;
		nonvirtual	Region	LocalToAncestor (const Region& r, const Enclosure* ancestor) const;
		nonvirtual	Region	AncestorToLocal (const Region& r, const Enclosure* ancestor) const;


		/*
		 * Convert from local coordinates in this enclosure, to those of the given enclosure.
		 * The Meet of the this and the destinationEnclosure MUST exist, or it is a programming error.
		 * So, this call is safe, iff CommonEnclosure (destinationEnclosure) returns non-nil.
		 */
		nonvirtual	Point	LocalToLocal (const Point& p, const Enclosure* destinationEnclosure) const;
		nonvirtual	Rect	LocalToLocal (const Rect& r, const Enclosure* destinationEnclosure) const;
		nonvirtual	Region	LocalToLocal (const Region& r, const Enclosure* destinationEnclosure) const;

	/*
	 * Perform the actual work of transforming a point to its coordinates in this's enclosure. Note, that
	 * this is sufficient to transform any region, or rectangle, given the assumption that the transformation
	 * is linear.
	 */
	protected:
		virtual	Point	LocalToEnclosure_ (const Point& p)	const				=			Nil;
		virtual	Point	EnclosureToLocal_ (const Point& p)	const				=			Nil;
};



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Enclosure::Enclosure ()		{}
inline	Enclosure::~Enclosure ()	{}
inline	Rect	Enclosure::GetLocalExtent () const					{	return (Rect (kZeroPoint, GetSize ())); }
inline	Point	Enclosure::LocalToEnclosure (const Point& p) const	{	return (LocalToEnclosure_ (p));			}
inline	Point	Enclosure::EnclosureToLocal (const Point& p) const	{	return (EnclosureToLocal_ (p));			}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Enclosure__*/

