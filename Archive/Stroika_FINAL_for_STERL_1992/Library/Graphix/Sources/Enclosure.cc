/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Enclosure.cc,v 1.3 1992/09/05 16:14:25 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Enclosure.cc,v $
 *		Revision 1.3  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.2  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/04/07  11:30:08  lewis
 *		Be more strict about const, based on using borlandC++, and put in ifdef support
 *		for templates.
 *
 *		Revision 1.6  92/03/26  15:59:32  15:59:32  lewis (Lewis Pringle)
 *		Made LocalToEnclosure (const Point& p)/EnclosureToLocal (const Point& p) inline since they were trivial one-liners and a real bottleneck.
 *		
 *		Revision 1.4  1992/03/05  16:47:50  lewis
 *		Fixed bug in LocalToLocal where it called AncestorToLocal the same view as was supposed
 *		to be the ancestor. This could have been fixed by loosening the restriction on
 *		AncestorToLocal but it was deemed better to just use it properly here...
 *		And, made CTOR and DTOR inline.
 *
 *		Revision 1.2  1992/01/16  00:45:06  lewis
 *		Renamed local variable cuz accidentally shadowed other name and MPW C compiler barfed on that.
 *
 *
 */


#include	"Debug.hh"

#include	"Enclosure.hh"



#if		!qRealTemplatesAvailable
	Implement (Iterator, EnclosurePtr);
#endif



/*
 ********************************************************************************
 ************************************ Enclosure *********************************
 ********************************************************************************
 */

Boolean	Enclosure::HasEnclosure (const Enclosure* enclosure) const
{
	RequireNotNil (enclosure);
	Require (enclosure != this);
	for (register Enclosure* e = GetEnclosure (); e != Nil; e = e->GetEnclosure ()) {
		if (e == enclosure) {
			return (True);
		}
	}
	return (False);
}

const	Enclosure*	Enclosure::CommonEnclosure (const Enclosure* enclosure) const
{
	RequireNotNil (enclosure);
	Require (enclosure != this);

	/*
	 * First, deal with the case that one enclosure is the ancestor of the other - that case is easy.
	 */
	if (HasEnclosure (enclosure)) {
		return (enclosure);
	}
	if (enclosure->HasEnclosure (this)) {
		return (this);
	}

	/*
	 * Then, wind up the tree of enclosures, and at each one, make a recursive call (on left guy) to find the
	 * common enclosure.
	 */
	for (register Enclosure* e = GetEnclosure (); e != Nil; e = e->GetEnclosure ()) {
		if (e->HasEnclosure (enclosure)) {
			return (e);			// cuz we know e has us inside, and e is first guy to also have 
		}
	}
	return (Nil);
}

Rect	Enclosure::LocalToEnclosure (const Rect& r) const
{
	return (Rect (LocalToEnclosure (r.GetOrigin ()), r.GetSize ()));
}

Rect	Enclosure::EnclosureToLocal (const Rect& r) const
{
	return (Rect (EnclosureToLocal (r.GetOrigin ()), r.GetSize ()));
}

Region	Enclosure::LocalToEnclosure (const Region& r) const
{
	return (r + LocalToEnclosure (kZeroPoint));
}

Region	Enclosure::EnclosureToLocal (const Region& r) const
{
	return (r + EnclosureToLocal (kZeroPoint));
}

Point	Enclosure::LocalToAncestor (const Point& p, const Enclosure* ancestor) const
{
// This routine may not be quite right - it assumes all transformations are commutative ?? Check for sure...
	RequireNotNil (ancestor);
	Require (ancestor != this);
	Require (HasEnclosure (ancestor));

	/*
	 * Add our origin regardless, since we are always going back at least as far as our direct
	 * enclosure. Then, if we are looking for some ancestor, wind back thru the enclosures, testing
	 * for a match before the addition of its origin, since we are going for ITS LOCAL coordinates.
	 */
	Point	result	=	LocalToEnclosure (p);
	for (register Enclosure* e = GetEnclosure (); e != ancestor; e = e->GetEnclosure ()) {
		if (e == ancestor) {
			break;
		}
		result = e->LocalToEnclosure (result);
	}
	return (result);		// OK even if enclosure Nil
}

Point	Enclosure::AncestorToLocal (const Point& p, const Enclosure* ancestor) const
{
// This routine may not be quite right - it assumes all transformations are commutative ?? Check for sure...
	RequireNotNil (ancestor);
	Require (ancestor != this);
	Require (HasEnclosure (ancestor));

	/*
	 * Subtract our origin regardless, since we are always going back at least as far as our direct
	 * enclosure. Then, if we are looking for some ancestor, wind back thru the enclosures, testing
	 * for a match before the subtraction of its origin, since we are going from ITS LOCAL coordinates.
	 */
	Point	result	=	p - GetOrigin ();
	for (register Enclosure* e = GetEnclosure (); e != ancestor; e = e->GetEnclosure ()) {
		if (e == ancestor) {
			break;
		}
		result = e->EnclosureToLocal (result);
	}
	return (result);		// OK even if enclosure Nil
}

Rect	Enclosure::LocalToAncestor (const Rect& r, const Enclosure* ancestor) const
{
	return (Rect (LocalToAncestor (r.GetOrigin (), ancestor), r.GetSize ()));
}

Rect	Enclosure::AncestorToLocal (const Rect& r, const Enclosure* ancestor) const
{
	return (Rect (AncestorToLocal (r.GetOrigin (), ancestor), r.GetSize ()));
}

Region	Enclosure::LocalToAncestor (const Region& r, const Enclosure* ancestor) const
{
	return (r + LocalToAncestor (kZeroPoint, ancestor));
}

Region	Enclosure::AncestorToLocal (const Region& r, const Enclosure* ancestor) const
{
	return (r + AncestorToLocal (kZeroPoint, ancestor));
}

Point	Enclosure::LocalToLocal (const Point& p, const Enclosure* destinationEnclosure) const
{
	RequireNotNil (destinationEnclosure);
	RequireNotNil (CommonEnclosure (destinationEnclosure));

	/*
	 * To LocalTo "Most Global" on this, and then same todestinationEnclosure, and be sure
	 * the hit the same Enclosure. If they do not, it is a caller error, and otherwise, the
	 * conversion is straight-forward.
	 */
	for (const Enclosure* myMostEnclosingEnclosure = this; myMostEnclosingEnclosure->GetEnclosure () != Nil;
		 myMostEnclosingEnclosure = myMostEnclosingEnclosure->GetEnclosure ()) {
	}
	AssertNotNil (myMostEnclosingEnclosure);
	Assert (myMostEnclosingEnclosure->GetEnclosure () == Nil);

	// Must check if destination enclosure is my most enclosing enclosure, because AncestorToLocal prohibits
	// considering an Enclosure as an Ancestor of itself. That is a sensible definition, but inconvienient in
	// this case - may wish to reconsider????
	if (destinationEnclosure == myMostEnclosingEnclosure) {
		return (p);
	}
	else {
		return (destinationEnclosure->AncestorToLocal (LocalToAncestor (p, myMostEnclosingEnclosure), myMostEnclosingEnclosure));
	}
}

Rect	Enclosure::LocalToLocal (const Rect& r, const Enclosure* destinationEnclosure) const
{
	return (Rect (LocalToLocal (r.GetOrigin (), destinationEnclosure), r.GetSize ()));
}

Region	Enclosure::LocalToLocal (const Region& r, const Enclosure* destinationEnclosure) const
{
	return (r + LocalToLocal (kZeroPoint, destinationEnclosure));
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


