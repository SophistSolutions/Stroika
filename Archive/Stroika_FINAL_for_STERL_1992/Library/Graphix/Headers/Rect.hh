/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef		__Rect__
#define		__Rect__

/*
 * $Header: /fuji/lewis/RCS/Rect.hh,v 1.3 1992/11/25 22:42:04 lewis Exp $
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
 *	$Log: Rect.hh,v $
 *		Revision 1.3  1992/11/25  22:42:04  lewis
 *		Rename Real -> double.
 *
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.8  92/04/16  17:01:53  17:01:53  lewis (Lewis Pringle)
 *		Added default constructor so we could create arrays of these guys - apparently C++ requires this.
 *		Too bad theres no plausible syntax for passing args to construction of array of items :-(.
 *		
 *		Revision 1.7  92/04/02  11:19:49  11:19:49  lewis (Lewis Pringle)
 *		Add intersection operator member function operator *=.
 *		Add requirement that fSize >= kZeroPoint. Could cause some trouble, But I think its the right
 *		idea, and we should face the bugs sooner rather than later.
 *		
 *
 */


#include	"Point.hh"

class	Rect {
	public:
		Rect ();			// really did not want to have a default constructor, but needed to make an array - init to zero
		Rect (const Rect& r);
		Rect (const Point& origin, const Point& size);

		const Rect& operator= (const Rect& rhs);

		nonvirtual	Point		GetOrigin () const;
		nonvirtual	Point		GetSize () const;

		nonvirtual	void		SetOrigin (const Point& origin);
		nonvirtual	void		SetSize (const Point& size);

		nonvirtual	Coordinate	GetTop () const;
		nonvirtual	Coordinate	GetLeft () const;
		nonvirtual	Coordinate	GetBottom () const;
		nonvirtual	Coordinate	GetRight () const;

		nonvirtual	Coordinate	GetHeight () const;
		nonvirtual	Coordinate	GetWidth () const;

		nonvirtual	Point		GetTopLeft () const;
		nonvirtual	Point		GetTopRight () const;
		nonvirtual	Point		GetBotRight () const;
		nonvirtual	Point		GetBotLeft () const;

		nonvirtual	Boolean	Empty () const;		// contains any bits...

		nonvirtual	Boolean	Contains (const Point& p) const;
		nonvirtual	Rect	InsetBy (const Point& delta);
		nonvirtual	Rect	InsetBy (Coordinate dv, Coordinate dh);

		nonvirtual	void	GetTLBR (Coordinate& top, Coordinate& left, Coordinate& bottom, Coordinate& right) const;
		nonvirtual	void	SetTLBR (Coordinate top, Coordinate left, Coordinate bottom, Coordinate right);

		nonvirtual	void	operator+= (const Point& delta);
		nonvirtual	void	operator-= (const Point& delta);
		nonvirtual	void	operator*= (const Rect& intersectWith);

	private:
		Point	fOrigin;
		Point	fSize;
};






#if		qMacGDI
/*
 * OS Conversions
 *		NB:	the reason for the asymetry here is that the one arg style is easier, but the
 *			two arg style is necessary when we dont know the size of the return type (as is the
 *			case with osRect since most people dont include the os headers.
 */
extern	const	struct	osRect&	os_cvt (const Rect& from, struct osRect& to);
extern	Rect					os_cvt (const osRect& from);
#endif	/*qMacGDI*/



/*
 * iostream support.
 */
extern	class ostream&	operator<< (class ostream& out, const Rect& r);
extern	class istream&	operator>> (class istream& in, Rect& r);



extern	const	Rect	kZeroRect;


extern	Rect	operator+ (const Rect& lhs, const Point& rhs);
extern	Rect	operator+ (const Point& lhs, const Rect& rhs);
extern	Rect	operator- (const Rect& lhs, const Point& rhs);
extern	Rect	operator- (const Point& lhs, const Rect& rhs);
extern	Rect	operator* (const Rect& lhs, const Rect& rhs);		// intersection



/*
 * comparisons.
 */
extern	Boolean	operator== (const Rect& lhs, const Rect& rhs);
extern	Boolean	operator!= (const Rect& lhs, const Rect& rhs);
extern	Rect	Intersection (const Rect& lhs, const Rect& rhs);
extern	Boolean	Intersects (const Rect& lhs, const Rect& rhs);



/*
 * bunch of utility frobs, not sure if some of these should be methods, but
 * kind of doubt it
 */
extern	Rect	InsetBy (const Rect& r, const Point& delta);
extern	Rect	InsetBy (const Rect& r, Coordinate dv, Coordinate dh);

extern	Rect	CenterRectAroundPoint (const Rect& r, const Point& newCenter);
extern	Rect	CenterRectAroundRect (const Rect& r, const Rect& newCenter);
extern	Rect	BoundingRect (const Rect& r1, const Rect& r2);
extern	Rect	BoundingRect (const Point& p1, const Point& p2);

// vScale and hScale should be 0 <= scale <= 1 or point will not be 
// within Rect
extern	Point	CalcRectPoint (const Rect& r, double vScale, double hScale);
extern	Point	CalcRectCenter (const Rect& r);


/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Point	Rect::GetOrigin ()  const		{	return (fOrigin);	}
inline	Point	Rect::GetSize ()  const			{	Ensure (fSize >= kZeroPoint); return (fSize);		}

inline	Coordinate	Rect::GetTop () const		{	return (fOrigin.GetV ()); }
inline	Coordinate	Rect::GetLeft () const		{	return (fOrigin.GetH ()); }
inline	Coordinate	Rect::GetBottom () const	{	return (fOrigin.GetV () + fSize.GetV ()); }
inline	Coordinate	Rect::GetRight () const		{	return (fOrigin.GetH () + fSize.GetH ()); }

inline	Coordinate	Rect::GetHeight () const	{	return (fSize.GetV ()); }
inline	Coordinate	Rect::GetWidth () const		{	return (fSize.GetH ()); }


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Rect__*/

