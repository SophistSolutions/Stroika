/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Point.cc,v 1.3 1992/11/25 22:41:05 lewis Exp $
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: Point.cc,v $
 *		Revision 1.3  1992/11/25  22:41:05  lewis
 *		Rename Real -> double - real obsolete.
 *
 *		Revision 1.2  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.7  1992/04/17  01:14:44  lewis
 *		Added static method to point to get me the opposite direction of a given direction.
 *
 *		Revision 1.5  1992/03/11  23:09:19  lewis
 *		Got rid of old qOldStreamLibOnly support.
 *
 *
 *
 */

#include	<math.h>

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<QuickDraw.h>
#endif	/*qMacGDI*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"Math.hh"
#include	"StreamUtils.hh"

#include	"Point.hh"




/*
 ********************************************************************************
 ************************************* Point ************************************
 ********************************************************************************
 */

Coordinate	Point::operator[] (Direction d) const
{
	Require ((d == eVertical) or (d == eHorizontal));
	return ((d == eVertical) ? fV : fH);
}

void	Point::SetVH (Direction d, Coordinate newVal)
{
	Require ((d == eVertical) or (d == eHorizontal));
	if (d == eVertical) {
		fV = newVal;
	}
	else {
		fH = newVal;
	}
}

const	Point&	Point::operator+= (const Point& delta)
{
	fV += delta.fV;
	fH += delta.fH;
	return (*this);
}

const	Point&	Point::operator-= (const Point& delta)
{
	fV -= delta.fV;
	fH -= delta.fH;
	return (*this);
}

const	Point&	Point::operator*= (double rhs)
{
	fV *= rhs;
	fH *= rhs;
	return (*this);
}

const	Point&	Point::operator/= (double rhs)
{
	fV /= rhs;
	fH /= rhs;
	return (*this);
}




/*
 ********************************************************************************
 ******************************** OtherDirections *******************************
 ********************************************************************************
 */

Point::Direction	Point::OtherDirection (Direction direction)
{
	return ((direction == eVertical)? eHorizontal: eVertical);
}




/*
 ********************************************************************************
 ******************************** OS Conversions ********************************
 ********************************************************************************
 */
#if		qMacGDI
const struct osPoint&	os_cvt (const Point& from, struct osPoint& to)
{
	Require (from.GetH () == (short)from.GetH ());		// assure no loss of precision
	Require (from.GetV () == (short)from.GetV ());		// assure no loss of precision
	to.h = (short)from.GetH ();
	to.v = (short)from.GetV ();
	return (to);
}

Point	os_cvt (const osPoint& from)
{
	return (Point (from.v, from.h));
}
#endif	/*qMacGDI*/


/*
 ********************************************************************************
 ******************************** iostream support ******************************
 ********************************************************************************
 */
class ostream&	operator<< (class ostream& out, const Point& p)
{
	out << lparen << p.GetV () << ' ' << p.GetH () << rparen;
	return (out);
}

class istream&	operator>> (class istream& in, Point& p)
{
	char	ch;
	in >> ch;
	if (ch != lparen) {					// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}
	Coordinate vc;
	in >> vc;
	Coordinate hc;
	in >> hc;
	in >> ch;
	if (ch != rparen) {					// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}
	Assert (in);
	p = Point (vc, hc);
	return (in);
}





/*
 ********************************************************************************
 ************************************* operator+ ********************************
 ********************************************************************************
 */

Point	operator+ (const Point& lhs, const Point& rhs)
{
	return (Point (lhs.GetV () + rhs.GetV (), lhs.GetH () + rhs.GetH ()));
}




/*
 ********************************************************************************
 ************************************* operator- ********************************
 ********************************************************************************
 */
Point	operator- (const Point& rhs)
{
	return (Point (-rhs.GetV (), -rhs.GetH ()));
}

Point	operator- (const Point& lhs, const Point& rhs)
{
	return (Point (lhs.GetV () - rhs.GetV (), lhs.GetH () - rhs.GetH ()));
}





/*
 ********************************************************************************
 ************************************* operator* ********************************
 ********************************************************************************
 */
Point	operator* (const Point& lhs, double rhs)
{
	return (Point (lhs.GetV () * rhs, lhs.GetH () * rhs));
}

Point	operator* (double lhs, const Point&  rhs)
{
	return (Point (rhs.GetV () * lhs, rhs.GetH () * lhs));
}

Point	operator* (const Point& lhs, const Point& rhs)
{
	return (Point (lhs.GetV () * rhs.GetV (), lhs.GetH () * rhs.GetH ()));
}





/*
 ********************************************************************************
 ************************************* operator/ ********************************
 ********************************************************************************
 */
Point	operator/ (const Point& lhs, double rhs)
{
	return (Point (lhs.GetV () / rhs, lhs.GetH () / rhs));
}






/*
 ********************************************************************************
 ************************************* operator% ********************************
 ********************************************************************************
 */
Point	operator% (const Point& lhs, UInt32 rhs)
{
	return (Point (lhs.GetV () % rhs, lhs.GetH () % rhs));
}

Point	operator% (const Point& lhs, const Point& rhs)
{
	return (Point (lhs.GetV () % rhs.GetV (), lhs.GetH () % rhs.GetH ()));
}





/*
 ********************************************************************************
 ************************************* RoundBy **********************************
 ********************************************************************************
 */

Point	RoundBy (const Point& lhs, UInt32 rhs)
{
	const	UInt32	rounder = rhs/2;
	Coordinate	v = lhs.GetV ();
	v += ((v < 0) ? -rounder : rounder);
	
	Coordinate	h = lhs.GetH ();
	h += ((h < 0) ? -rounder : rounder);

	return (Point (v / rhs * rhs, h / rhs * rhs));
}

Point	RoundBy (const Point& lhs, const Point& rhs)
{
	const	Point	rounder = rhs/2;
	Coordinate	v = lhs.GetV ();
	v += ((v < 0) ? -rounder.GetV () : rounder.GetV ());
	
	Coordinate	h = lhs.GetH ();
	h += ((h < 0) ? -rounder.GetH () : rounder.GetH ());

	return (Point (
		(v / rhs.GetV ()) * rhs.GetV (),
		(h / rhs.GetH ()) * rhs.GetH ()));
}





/*
 ********************************************************************************
 ************************************* operator< ********************************
 ********************************************************************************
 */
Boolean	operator< (const Point& lhs, const Point& rhs)
{
	return Boolean ((lhs <= rhs) and (lhs != rhs));
}




/*
 ********************************************************************************
 ************************************* operator> ********************************
 ********************************************************************************
 */
Boolean	operator> (const Point& lhs, const Point& rhs)
{
	return Boolean ((lhs >= rhs) and (lhs != rhs));
}





/*
 ********************************************************************************
 **************************************** Min ***********************************
 ********************************************************************************
 */
Point	Min (const Point& lhs, const Point& rhs)
{
		return (Point (Min (lhs.GetV (), rhs.GetV ()), Min (lhs.GetH (), rhs.GetH ())));
}




/*
 ********************************************************************************
 **************************************** Min ***********************************
 ********************************************************************************
 */
Point	Max (const Point& lhs, const Point& rhs)
{
		return (Point (Max (lhs.GetV (), rhs.GetV ()), Max (lhs.GetH (), rhs.GetH ())));
}




/*
 ********************************************************************************
 **************************************** Abs ***********************************
 ********************************************************************************
 */
Point	Abs (const Point& p)
{
	return (Point (Abs (p.GetV ()), Abs (p.GetH ())));
}





/*
 ********************************************************************************
 ************************************* Norm *************************************
 ********************************************************************************
 */
double	Norm (const Point& p)
{
	return (sqrt (p.GetV () * p.GetV () + p.GetH () * p.GetH ()));
}




/*
 ********************************************************************************
 ******************************* EuclideanDistance ******************************
 ********************************************************************************
 */
double	EuclideanDistance (const Point& a, const Point& b)
{
	return (Norm (a-b));
}




/*
 ********************************************************************************
 ********************************** AbsoluteDistance ****************************
 ********************************************************************************
 */
Coordinate	AbsoluteDistance (const Point& a, const Point& b)
{
	return (Abs (a.GetV () - b.GetV ()) + Abs (a.GetH () - b.GetH ()));
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

