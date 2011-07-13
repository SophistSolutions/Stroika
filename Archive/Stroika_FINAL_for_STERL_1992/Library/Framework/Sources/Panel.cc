/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Panel.cc,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Panel.cc,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/08  03:24:58  lewis
 *		Renamed PointInside-> Contains.
 *
 *		Revision 1.2  1992/07/04  14:20:25  lewis
 *		Take advantage of new shape letter/envelope support- pass by value, instead of reference.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.16  1992/06/09  16:38:34  lewis
 *		Fixed unsafe construction order problem with sDefaultShape.
 *
 *		Revision 1.15  92/05/18  16:37:47  16:37:47  lewis (Lewis Pringle)
 *		Fix const usage to avoid compiler warnings.
 *		
 *		Revision 1.14  92/04/07  12:50:23  12:50:23  lewis (Lewis Pringle)
 *		Panel::GetSubEnclosureIterator () should return ptr to interator.
 *		
 *		Revision 1.13  92/03/26  18:35:54  18:35:54  lewis (Lewis Pringle)
 *		Cleanup comments, and add ensures.
 *		
 *		Revision 1.12  1992/03/26  09:45:33  lewis
 *		Made GetVisibleArea () const, and got rid of oldVisible arg to EffectiveVisibiltyChanged ().
 *
 *		Revision 1.11  1992/03/22  21:40:31  lewis
 *		Perform some optimizations based on profiling on the mac. Take a close look at where we currently call Refresh () - talk over with sterl.
 *		I think we should seriously consider only doing refreshes from our EffectiveXXXChanged () callbacks in
 *		concrete classes, such as PushButton_MacUI_Portable - IE ONLY WHERE WE OVERRIDE DRAW!!!
 *
 *		Revision 1.10  1992/02/15  05:26:30  sterling
 *		added EffectiveVisibility Changed
 *
 *		Revision 1.8  1992/01/15  10:05:08  lewis
 *		Moved some functionality to a base class Enclosure.
 *
 *		Revision 1.6  1992/01/14  23:28:54  lewis
 *		Started adding support for extra arg to LocalToEnclosure, et al.
 *
 *		Revision 1.5  1992/01/14  06:03:59  lewis
 *		Put tablet related stuff into TabletOwner.
 *
 *
 */



#include	"Debug.hh"

#include	"Shape.hh"

#include	"Panel.hh"



static	const	Shape&	GetDefaultShape ()
{
	static	Rectangle*	sDefaultShape		=	new Rectangle ();
	EnsureNotNil (sDefaultShape);
	return (*sDefaultShape);
}




/*
 ********************************************************************************
 ************************************** Panel ***********************************
 ********************************************************************************
 */

const	Boolean		Panel::kVisible		=	True;

Panel::Panel ():
	fVisible (kVisible),
	fOrigin (kZeroPoint),
	fSize (kZeroPoint),
	fShape (GetDefaultShape ())
{
}

Panel::~Panel ()
{
}

void	Panel::SetVisible (Boolean visible)
{
	if (visible != GetVisible ()) {
		SetVisible_ (visible, eDelayedUpdate);
	}
	Ensure (visible == GetVisible ());
}

void	Panel::SetVisible (Boolean visible, UpdateMode updateMode)
{
	if (visible != GetVisible ()) {
		SetVisible_ (visible, updateMode);
	}
	Ensure (visible == GetVisible ());
}

void	Panel::SetVisible_ (Boolean visible, UpdateMode updateMode)
{
	/*
	 * Note: the reason we do the Refresh () here, rather than in the EffectiveVisibilityChanged() call is
	 * cuz its much more efficient to just do it once here, than in EACH sub panel whose effective visibility
	 * may have changed. Equally importantly, it really doesn't make sense as something to specify on a subclassing
	 * basis - Its hard to imagine not wanting to call Refresh () on making a view visible or invisible.
	 * Thats still possible to do in a number of fasions, but hardly worth worrying about.
	 */
	Require (visible != GetVisible ());

	/*
	 * Note we appear to call Refresh twice, but really do only once - since oldVisible is different that
	 * newVisible, and each call is wrapped with "if GetVisible()" we really only do it once. It is important
	 * that we do it while we ARE visible, since otherwise Refresh will have no effect.
	 */
	if (GetVisible () and (updateMode != eNoUpdate)) {
		Refresh (updateMode);
	}
	fVisible = visible;
	
	EffectiveVisibilityChanged (visible, updateMode);

	if (GetVisible () and (updateMode != eNoUpdate)) {
		Refresh (updateMode);
	}

	Ensure (visible == GetVisible ());
}

void	Panel::EffectiveVisibilityChanged (Boolean /*newVisible*/, UpdateMode /*updateMode*/)
{
	// consider making inline, since people probably call inherited::EffectiveVisibilityChanged () and that
	// could be optimized away - current generation of compilers don't seem to be doing a great job
	// of this sort of thing so wait awhile - LGP March 24, 1992
}

void	Panel::SetOrigin (const Point& newOrigin, UpdateMode updateMode)	
{ 
	if (newOrigin != GetOrigin ()) {
		SetOrigin_ (newOrigin, updateMode);
	}
	Ensure (fOrigin == newOrigin);
}

void	Panel::SetOrigin (const Point& newOrigin)	
{ 
	if (newOrigin != GetOrigin ()) {
		SetOrigin_ (newOrigin, eDelayedUpdate);
	}
	Ensure (fOrigin == newOrigin);
}

void	Panel::SetOrigin_ (const Point& newOrigin, UpdateMode updateMode)
{
	Require (newOrigin != GetOrigin ());

	if ((updateMode == eNoUpdate) or (GetParentPanel () == Nil)) {
		fOrigin = newOrigin;
	}
	else {
		/*
		 * Note: we are careful to use the enclosure coordinates, since
		 * we are changing out own origin!
		 */
		Region	updateRegion	=	LocalToEnclosure (GetVisibleArea ());
		fOrigin = newOrigin;
		updateRegion += LocalToEnclosure (GetVisibleArea ());
		GetParentPanel ()->Refresh (updateRegion, updateMode);
	}
	Ensure (fOrigin == newOrigin);
}


void	Panel::SetSize (const Point& newSize, UpdateMode updateMode)		
{ 
	if (newSize != GetSize ()) {
		SetSize_ (newSize, updateMode);
	}
	Ensure (fSize == newSize);
}

void	Panel::SetSize (const Point& newSize)		
{ 
	if (newSize != GetSize ()) {
		SetSize_ (newSize, eDelayedUpdate); 
	}
	Ensure (fSize == newSize);
}

void	Panel::SetSize_ (const Point& newSize, UpdateMode updateMode)
{
	Require (newSize.GetV () >= 0);		// really we should change the type to unsigned and call it dimension!!!
	Require (newSize.GetH () >= 0);
	Require (newSize != GetSize ());

	if ((updateMode == eNoUpdate) or (GetParentPanel () == Nil)) {
		fSize = newSize;
	}
	else {
		Region	updateRegion	=	GetVisibleArea ();
		fSize = newSize;
		updateRegion += GetVisibleArea ();
		GetParentPanel ()->Refresh (LocalToEnclosure (updateRegion), updateMode);
	}
	Ensure (fSize == newSize);
}

Rect	Panel::GetExtent () const
{
	return (Rect (GetOrigin (), GetSize ()));
}

Point	Panel::GetOrigin () const
{
	return (fOrigin);
}

Point	Panel::GetSize () const
{
	return (fSize);
}

Enclosure*	Panel::GetEnclosure () const
{
	return (GetParentPanel ());
}

Iterator(EnclosurePtr)*	Panel::GetSubEnclosureIterator () const
{
	AssertNotImplemented (); /* hack til we get around it */
	// make funky iterator based on subPanel iterator!!!
}

void	Panel::SetExtent (Coordinate top, Coordinate left, Coordinate height, Coordinate width)
{
	SetExtent (Rect (Point (top, left), Point (height, width)));
}

void	Panel::SetExtent (Coordinate top, Coordinate left, Coordinate height, Coordinate width, UpdateMode updateMode)
{
	SetExtent (Rect (Point (top, left), Point (height, width)), updateMode);
}

void	Panel::SetExtent (const Rect& newExtent)
{
	if (newExtent != GetExtent ()) {
		SetExtent_ (newExtent, eDelayedUpdate);
	}
	Ensure (GetExtent () == newExtent);
}

void	Panel::SetExtent (const Rect& newExtent, UpdateMode updateMode)
{
	if (newExtent != GetExtent ()) {
		SetExtent_ (newExtent, updateMode);
	}
	Ensure (GetExtent () == newExtent);
}

void	Panel::SetExtent_ (const Rect& newExtent, UpdateMode updateMode)
{
	UpdateMode	tempUpdate = (updateMode == eImmediateUpdate)? eDelayedUpdate: updateMode;
	SetOrigin (newExtent.GetOrigin (), tempUpdate);
	SetSize (newExtent.GetSize (), tempUpdate);
	if (updateMode == eImmediateUpdate) {
		Update ();
	}
	
	Ensure (GetExtent () == newExtent);
}

Point	Panel::LocalToEnclosure_ (const Point& p) const
{
	/*
	 * Even if we have no enclosure, this seems to be the most sensible answer.
	 */
	return (p + GetOrigin ());
}

Point	Panel::EnclosureToLocal_ (const Point& p) const
{
	/*
	 * Even if we have no enclosure, this seems to be the most sensible answer.
	 */
	return (p - GetOrigin ());
}

void	Panel::SetShape (const Shape& newShape)
{
	fShape = newShape;
}

Region	Panel::GetLocalRegion () const
{
	return (fShape.ToRegion (GetLocalExtent ()));
}

Region	Panel::GetRegion () const
{
	return (fShape.ToRegion (GetExtent ()));
}

Boolean	Panel::Contains (const Point& p) const
{
	return (fShape.Contains (p, GetExtent ()));
}

void	Panel::Refresh (const Region& r, UpdateMode updateMode) 	
{
	if ((updateMode != eNoUpdate) and GetEffectiveVisibility ()) {
		Refresh_ (r, updateMode); 
	}
}

void	Panel::Refresh (const Region& r)						
{ 
	if (GetEffectiveVisibility ()) {
		Refresh_ (r, eDelayedUpdate);
	}
}

void	Panel::Refresh (UpdateMode updateMode)
{
	if ((updateMode != eNoUpdate) and GetEffectiveVisibility ()) {
		Refresh_ (GetVisibleArea () * GetLocalRegion (), updateMode);
	}
}

void	Panel::Refresh_ (const Region& updateRegion, UpdateMode updateMode)
{
	/*
	 * count on some enclosure (like window) to override, and provide proper behavior.
	 */
	Require (updateMode != eNoUpdate);
	Require (GetEffectiveVisibility ());
	register	Panel*	parent = GetParentPanel ();
	if (parent != Nil) {
		/*
		 * Since Refresh () is nonvirtual, we can peek at its implementation, and not bother calling it, but
		 * call Refresh_ instead - thats cuz our Visible Area already takes into account the visible area,
		 * and local region of all of my parents, and similarly with the effectiveVisibilty!!!
		 */
		parent->Refresh_ (LocalToEnclosure (updateRegion), updateMode);
	}
}

void	Panel::Update_ (const Region& updateRegion)
{
	register	Panel*	parent = GetParentPanel ();
	if (parent != Nil) {
		parent->Update (LocalToEnclosure (updateRegion));
	}
}

Boolean	Panel::GetEffectiveVisibility () const
{
	register	const Panel*	panel = this;
	while (panel != Nil) {
		if (panel->GetVisible ()) {
			panel = panel->GetParentPanel ();
		}
		else {
			return (False);
		}
	}
	return (True);
}

Region	Panel::GetVisibleArea () const
{
	// Not a very good definition, since it doesn't take into account siblings, nor opacity.
	if (GetEffectiveVisibility ()) {
		Panel*	enclosure		=	GetParentPanel ();
		if (enclosure != Nil) {
			return (EnclosureToLocal (GetRegion () * enclosure->GetVisibleArea ()));
		}
		return (GetLocalRegion ());
	}
	return (kEmptyRegion);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


