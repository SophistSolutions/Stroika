/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Panel__
#define	__Panel__

/*
 * $Header: /fuji/lewis/RCS/Panel.hh,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 
 << REDO DOCS >>
 
 *		A panel is a entity whose position is defined hierarchicaly. Panels can be nested. They provide
 *		access to their subpanels (through iterators), and their parent, so that the Panel Tree can be
 *		traversed.
 *
 *		Panels have a Shape as an attribute, and the shape can be changed by calling
 *		SetShape() on the panel.  This allows slightly more flexibility than the inheritance
 *		scheme, and avoids those nastie virtual base class bugs.
 *
 *		Panels have a notion of visibility. The reason for including visiblity at this level is that
 *		here is a natural place to do calculations like effective visibility (ie are you obscured).
 *		It is not clearly the right place since some things are always visible (like trackers, and the desktop).
 *		Maybe we should just have GetVisible () as pure virtual, and let subclasses add SetVisible () (like view).
 *
 *
 * TODO:
 *
 *		We should have a makesubpaneliterator ().
 *
 *		Consider if we should have an GetOpaque () method - and clearly define semantics if we decide yes...
 *
 *		think about making these virtual(LocalToEnclosure - et al), and using this as a mechanism for dealing
 *		with the problem of different monitor resolutions.  Needs much thought, but
 *		could use these 8 routines as basis for scaling all graphix, text, etc.. in 
 *		a view..?????
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Panel.hh,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/08  02:14:45  lewis
 *		Renamed PointInside -> Contains () .
 *
 *		Revision 1.3  1992/07/04  14:10:50  lewis
 *		Take advantage of new shape letter/envelope support- pass by value, instead of reference.
 *
 *		Revision 1.2  1992/07/01  01:39:11  lewis
 *		Got rid of of qNestedTypesAvailable flag - assume always true.
 *		Also, got rid of #define UpdateMode Panel::UpdateMode_ since not sure what
 *		it was done for - hopefully outdated hack, otherwise will have to put it back
 *		with a better comment.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.11  1992/04/07  12:49:59  lewis
 *		GetSubEnclosure () should return ptr to interator.
 *
 *		Revision 1.10  92/03/26  09:36:15  09:36:15  lewis (Lewis Pringle)
 *		Simplified args to EffectiveLive/Visibility/Font changed - no old value, just the new value passed.
 *		
 *		Revision 1.9  1992/02/15  04:50:37  sterling
 *		supported EffectiveVisibilityChanged
 *
 *		Revision 1.8  1992/01/15  10:04:39  lewis
 *		Inherit from Enclosure, and move some functionality there.
 *
 *		Revision 1.6  1992/01/14  23:27:03  lewis
 *		Reorganized as prelude to moving update stuff to View, and doing extra arg LocalToEnclosure () calls.
 *
 *		Revision 1.5  1992/01/14  06:03:04  lewis
 *		Got rid of tablet related stuff and moved it into TabletOwner class.
 *
 *
 *
 */

#include	"Enclosure.hh"
#include	"Shape.hh"

#include	"Config-Framework.hh"


class	Panel : public Enclosure {
	public:
		enum UpdateMode {		// This really should go in View
			eNoUpdate,
			eDelayedUpdate,
			eImmediateUpdate
		};

		// Move this to view...
		nonvirtual	void	SetVisible (Boolean visible, UpdateMode updateMode);
		nonvirtual	void	SetOrigin (const Point& newOrigin, UpdateMode updateMode);
		nonvirtual	void	SetSize (const Point& newSize, UpdateMode updateMode);
		nonvirtual	void	SetExtent (Coordinate top, Coordinate left, Coordinate height, Coordinate width, UpdateMode updateMode);
		nonvirtual	void	SetExtent (const Rect& newExtent, UpdateMode updateMode);
		nonvirtual	void	SetExtent (Coordinate top, Coordinate left, Coordinate height, Coordinate width);

		nonvirtual	void	Refresh (const Region& r, UpdateMode updateMode);
		nonvirtual	void	Refresh (const Region& r);
		nonvirtual	void	Refresh (UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	void	Update ();
		nonvirtual	void	Update (const Region& updateRegion);

		nonvirtual	void	SetVisible (Boolean visible);

	protected:
		virtual		void	SetOrigin_ (const Point& newOrigin, UpdateMode updateMode);
		virtual		void	SetSize_ (const Point& newSize, UpdateMode updateMode);
		virtual		void	SetExtent_ (const Rect& newExtent, UpdateMode updateMode);
		virtual		void	SetVisible_ (Boolean visible, UpdateMode updateMode);
		virtual		void	Refresh_ (const Region& r, UpdateMode update);
		virtual		void	Update_ (const Region& updateRegion);
		virtual		void	EffectiveVisibilityChanged (Boolean newVisible, UpdateMode updateMode);

	private:
		Boolean	fVisible;

	protected:
		Panel ();

	public:
		static	const	Boolean	kVisible;

		virtual ~Panel ();

		nonvirtual	Boolean	GetVisible () const;

		/*
		 * Relative location accessors
		 */
		override	Point	GetOrigin () const;
		nonvirtual	void	SetOrigin (const Point& newOrigin);

		override	Point	GetSize () const;
		nonvirtual	void	SetSize (const Point& newSize);

		override	Rect	GetExtent () const;
		nonvirtual	void	SetExtent (const Rect& newExtent);


		nonvirtual	const Shape&	GetShape () const;
		virtual		void			SetShape (const Shape& newShape);

		nonvirtual	Region	GetLocalRegion () const;
		nonvirtual	Region	GetRegion () const;
		nonvirtual	Boolean	Contains (const Point& p) const;	// p in Enclosure coordinates


		nonvirtual	Boolean	GetEffectiveVisibility () const;	// recursively check parents
		virtual		Region	GetVisibleArea () const;			// recursively check parents and ^




virtual	Panel*	GetParentPanel () const = Nil;

override	Enclosure*	GetEnclosure () const;
override	Iterator(EnclosurePtr)*	GetSubEnclosureIterator () const;

	protected:
		override	Point	LocalToEnclosure_ (const Point& p)	const;
		override	Point	EnclosureToLocal_ (const Point& p)	const;

	private:
		Point	fOrigin;
		Point	fSize;
		Shape	fShape;
};



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	Boolean			Panel::GetVisible () const				{ 	return (fVisible); 				}
inline	const Shape&	Panel::GetShape () const				{	return (fShape);				}


// Move to View...
inline	void	Panel::Update ()								{ 	Update_ (GetVisibleArea ());	}
inline	void	Panel::Update (const Region& updateRegion)		{ 	Update_ (updateRegion);			}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***



#endif	/*__Panel__*/

