/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Rect.cc,v 1.4 1992/11/25 22:42:20 lewis Exp $
 *
 * TODO:
 *			-	Make clearer definitions when size = 0, and what we do in that case. Are all rects of
 *				size zero == kZeroRect - I think NO, alghtouh we make this true for regions.... Must hammer out details!!!
 *
 * Changes:
 *	$Log: Rect.cc,v $
 *		Revision 1.4  1992/11/25  22:42:20  lewis
 *		Rename Real -> double.
 *
 *		Revision 1.3  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/16  15:44:49  sterling
 *		fixed BoundingRect
 *
 *		Revision 1.11  92/04/16  17:02:58  17:02:58  lewis (Lewis Pringle)
 *		Added default constructor so we could create arrays of these guys - apparently C++ requires this.
 *		Too bad theres no plausible syntax for passing args to construction of array of items :-(.
 *		
 *		Revision 1.9  92/04/14  12:51:10  12:51:10  lewis (Lewis Pringle)
 *		Fix Rect	Rect::InsetBy (const Point& delta)
 *		 to pin to kZeroRect when size gets too small.
 *		
 *		Revision 1.7  92/04/02  11:20:50  11:20:50  lewis (Lewis Pringle)
 *		Add intersection operator member function operator *=.
 *		Add requirement that fSize >= kZeroPoint. Could cause some trouble, But I think its the right
 *		idea, and we should face the bugs sooner rather than later.
 *		
 *		Revision 1.5  1992/03/05  16:55:01  lewis
 *		Minor cleanups.
 *
 *
 *
 */

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<QuickDraw.h>
#endif	/*qMacGDI*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"Math.hh"
#include	"StreamUtils.hh"

#include	"Rect.hh"





/*
 ********************************************************************************
 *************************************** Rect ***********************************
 ********************************************************************************
 */

Rect::Rect ():
	fOrigin (kZeroPoint),
	fSize (kZeroPoint)
{
}

Rect::Rect (const Rect& r):
	fOrigin (r.fOrigin),
	fSize (r.fSize)
{
	Require (r.fSize >= kZeroPoint);
}

Rect::Rect (const Point& origin, const Point& size):
	fOrigin (origin),
	fSize (size)
{
	Require (size >= kZeroPoint);
}

const Rect& Rect::operator= (const Rect& rhs)
{
	Require (rhs.fSize >= kZeroPoint);
	fOrigin = rhs.fOrigin;
	fSize = rhs.fSize;
	return (*this);
}

void	Rect::SetOrigin (const Point& origin)
{
	fOrigin = origin;
}

void	Rect::SetSize (const Point& size)
{
	Require (size >= kZeroPoint);
	fSize = size;
}

Point	Rect::GetTopLeft () const
{
	return (GetOrigin ());
}

Point	Rect::GetTopRight () const
{
	return (Point (GetTop (), GetRight ()));
}

Point	Rect::GetBotRight () const
{
	return (Point (GetBottom (), GetRight ()));
}

Point	Rect::GetBotLeft () const
{
	return (Point (GetBottom (), GetLeft ()));
}

Boolean	Rect::Empty () const
{
	return (Boolean ((GetHeight () <= 0) or (GetWidth () <= 0)));
}

Boolean	Rect::Contains (const Point& p) const
{
	return Boolean ((p >= GetTopLeft ()) and (p <= GetBotRight ()));
}

Rect	Rect::InsetBy (const Point& delta)
{
	Require (fSize >= kZeroPoint);

	Point	sizeAdjust	=	2*delta;
	if (fSize >= sizeAdjust) {
		fOrigin += delta;
		fSize -= sizeAdjust;
	}
	else {
		*this = kZeroRect;
	}

	Ensure (fSize >= kZeroPoint);
	return (*this);
}

Rect	Rect::InsetBy (Coordinate dv, Coordinate dh)
{
	return (InsetBy (Point (dv, dh)));
}

void	Rect::GetTLBR (Coordinate& top, Coordinate& left, Coordinate& bottom, Coordinate& right) const
{
	top = GetTop ();
	left = GetLeft ();
	bottom = GetBottom ();
	right = GetRight ();
	Ensure (fSize >= kZeroPoint);
}

void	Rect::SetTLBR (Coordinate top, Coordinate left, Coordinate bottom, Coordinate right)
{
	Require (fSize >= kZeroPoint);

	fOrigin.SetV (top);
	fOrigin.SetH (left);
	fSize.SetV (bottom-top);
	fSize.SetH (right-left);

	Ensure (fSize >= kZeroPoint);
}

void	Rect::operator+= (const Point& delta)
{
	SetOrigin (fOrigin + delta);
}

void	Rect::operator-= (const Point& delta)
{
	SetOrigin (fOrigin - delta);
}

void	Rect::operator*= (const Rect& intersectWith)
{
	Point	topLeft		=	Max (GetTopLeft (), intersectWith.GetTopLeft ());
	Point	botRight	=	Min (GetBotRight (), intersectWith.GetBotRight ());
	Point newSize = botRight-topLeft;
	if (newSize >= kZeroPoint) {
		SetOrigin (topLeft);
		SetSize (newSize);
	}
	else {
		*this = kZeroRect;
	}

	Ensure (fSize >= kZeroPoint);
}






/*
 ********************************************************************************
 ******************************** OS Conversions ********************************
 ********************************************************************************
 */
#if		qMacGDI
const	struct	osRect&	os_cvt (const Rect& from, struct osRect& to)
{
	Require (from.GetTop () == (short)from.GetTop ());			// assure no loss of precision
	Require (from.GetLeft () == (short)from.GetLeft ());			// assure no loss of precision
	Require (from.GetBottom () == (short)from.GetBottom ());		// assure no loss of precision
	Require (from.GetRight () == (short)from.GetRight ());		// assure no loss of precision
	to.top = (short)from.GetTop ();
	to.left = (short)from.GetLeft ();
	to.bottom = (short)from.GetBottom ();
	to.right = (short)from.GetRight ();
	return (to);
}

Rect	os_cvt (const osRect& from)
{
	return (Rect (os_cvt (*(osPoint*)&from), Point (from.bottom - from.top, from.right - from.left)));
}
#endif	/*qMacGDI*/




/*
 ********************************************************************************
 ******************************** iostream support ******************************
 ********************************************************************************
 */
ostream&	operator<< (ostream& out, const Rect& r)
{
	out << lparen << r.GetOrigin () << ' ' << r.GetSize () << rparen;
	return (out);
}

istream&	operator>> (istream& in, Rect& r)
{
	char	ch;
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
	Assert (in);
	r = Rect (origin, size);
	return (in);
}





/*
 ********************************************************************************
 ************************************* operator+ ********************************
 ********************************************************************************
 */
Rect	operator+ (const Rect& lhs, const Point& rhs)
{
	return (Rect (lhs.GetOrigin () + rhs, lhs.GetSize ()));
}

Rect	operator+ (const Point& lhs, const Rect& rhs)
{
	return (Rect (rhs.GetOrigin () + lhs, rhs.GetSize ()));
}



/*
 ********************************************************************************
 *********************************** operator- **********************************
 ********************************************************************************
 */
Rect	operator- (const Rect& lhs, const Point& rhs)
{
	return (Rect (lhs.GetOrigin () - rhs, lhs.GetSize ()));
}

Rect	operator- (const Point& lhs, const Rect& rhs)
{
	return (Rect (rhs.GetOrigin () - lhs, rhs.GetSize ()));
}



/*
 ********************************************************************************
 *********************************** operator* **********************************
 ********************************************************************************
 */
Rect	operator* (const Rect& lhs, const Rect& rhs)
{
	Point	topLeft		=	Max (lhs.GetTopLeft (), rhs.GetTopLeft ());
	Point	botRight	=	Min (lhs.GetBotRight (), rhs.GetBotRight ());

	Point newSize = botRight-topLeft;
	if (newSize >= kZeroPoint) {
		return (Rect (topLeft, botRight-topLeft));
	}
	return (kZeroRect);	// may contain no bits if Not (topLeft <= botRight)
}




/*
 ********************************************************************************
 ********************************** operator== **********************************
 ********************************************************************************
 */
Boolean	operator== (const Rect& lhs, const Rect& rhs)
{
	return Boolean ((lhs.GetOrigin () == rhs.GetOrigin ()) and (lhs.GetSize () == rhs.GetSize ()));
}






/*
 ********************************************************************************
 ********************************* Intersects ***********************************
 ********************************************************************************
 */
// should rects intersect if they touch at border???
// I dont think so for pragmatic reasons. Also, if the rectangle is drawn inside, then
// the boundary seems to not properly be part of the set
Boolean	Intersects (const Rect& lhs, const Rect& rhs)
{
	if (rhs.GetTop () >= lhs.GetBottom ()) {
		return (False);
	}
	else if (rhs.GetBottom () <= lhs.GetTop ()) {
		return (False);
	}
	else if (rhs.GetLeft () >= lhs.GetRight ()) {
		return (False);
	}
	else if (rhs.GetRight () <= lhs.GetLeft ()) {
		return (False);
	}
	
	return (True);
}



/*
 ********************************************************************************
 ************************************* operator!= *******************************
 ********************************************************************************
 */
Boolean	operator!= (const Rect& lhs, const Rect& rhs)
{
	return Boolean ((lhs.GetOrigin () != rhs.GetOrigin ()) or (lhs.GetSize () != rhs.GetSize ()));
}




/*
 ********************************************************************************
 *************************************** InsetBy ********************************
 ********************************************************************************
 */
Rect	InsetBy (const Rect& r, const Point& delta)
{
	Rect	r2	=	r;
	return (r2.InsetBy (delta));
}

Rect	InsetBy (const Rect& r, Coordinate dv, Coordinate dh)
{
	Rect	r2	=	r;
	return (r2.InsetBy (dv, dh));
}



/*
 ********************************************************************************
 ***************************** CenterRectAroundPoint ****************************
 ********************************************************************************
 */
/*
 * bunch of utility frobs, not sure if some of these should be methods, but
 * kind of doubt it
 */
 
Rect	CenterRectAroundPoint (const Rect& r, const Point& newCenter)
{
	return (Rect (newCenter - Point (r.GetHeight ()/2, r.GetWidth ()/2), r.GetSize ()));
}
 
 
 
/*
 ********************************************************************************
 ****************************** CenterRectAroundRect ****************************
 ********************************************************************************
 */
Rect	CenterRectAroundRect (const Rect& r, const Rect& newCenter)
{
	return (CenterRectAroundPoint (r, CalcRectCenter (newCenter)));
}



/*
 ********************************************************************************
 *********************************** BoundingRect *******************************
 ********************************************************************************
 */
Rect	BoundingRect (const Rect& r1, const Rect& r2)
{
	Rect	result = kZeroRect;
	result.SetTLBR (
		Min (r1.GetTop (), r2.GetTop ()),
		Min (r1.GetLeft (), r2.GetLeft ()),
		Max (r1.GetBottom (), r2.GetBottom ()),
		Max (r1.GetRight (), r2.GetRight ()));
	return (result);
}

Rect	BoundingRect (const Point& p1, const Point& p2)
{
	return (Rect (
		Point (Min (p1.GetV (), p2.GetV ()), Min (p1.GetH (), p2.GetH ())),
		Point (Abs (p1.GetV () - p2.GetV ()), Abs (p1.GetH () - p2.GetH ()))));
}


/*
 ********************************************************************************
 ********************************** CalcRectPoint *******************************
 ********************************************************************************
 */
 // vScale and hScale should be 0 <= scale <= 1 or point will not be 
 // within Rect
 Point	CalcRectPoint (const Rect& r, double vScale, double hScale)
 {
 	return (Point (
		r.GetTop () + r.GetHeight () * vScale,
		r.GetLeft () + r.GetWidth () * hScale));
 }




/*
 ********************************************************************************
 ********************************** CalcRectCenter ******************************
 ********************************************************************************
 */
 Point	CalcRectCenter (const Rect& r)
 {
 	return (CalcRectPoint (r, 0.5, 0.5));
 }





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


