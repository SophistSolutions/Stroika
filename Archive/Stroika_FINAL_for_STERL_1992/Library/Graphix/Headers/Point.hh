/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef		__Point__
#define		__Point__

/*
 * $Header: /fuji/lewis/RCS/Point.hh,v 1.3 1992/11/25 22:40:47 lewis Exp $
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
 *	$Log: Point.hh,v $
 *		Revision 1.3  1992/11/25  22:40:47  lewis
 *		Rename Read->double - obsolete.
 *
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.7  1992/04/17  01:14:23  lewis
 *		Added static method to point to get me the opposite direction of a given direction.
 *
 *
 *
 *
 */


#include	"Config-Graphix.hh"


typedef	Int32	Coordinate;
const	Coordinate	kMinCoordinate	=	kMinInt32;
const	Coordinate	kMaxCoordinate	=	kMaxInt32;

class	Point {
	public:
		enum Direction {
			eVertical,
			eHorizontal,
		};
		static	Direction	OtherDirection (Direction direction);

		Point ();
		Point (const Point& from);
		Point (Coordinate v, Coordinate h);

		nonvirtual	const	Point&	operator= (const Point& rhs);

		nonvirtual	Coordinate	GetV () const;
		nonvirtual	void		SetV (Coordinate v);
		nonvirtual	Coordinate	GetH () const;
		nonvirtual	void		SetH (Coordinate h);

		nonvirtual	Coordinate	operator[] (Direction d) const;
		nonvirtual	void		SetVH (Direction d, Coordinate newVal);

		nonvirtual	const	Point&	operator+= (const Point& delta);
		nonvirtual	const	Point&	operator-= (const Point& delta);
		nonvirtual	const	Point&	operator*= (double rhs);
		nonvirtual	const	Point&	operator/= (double rhs);

	private:
		Coordinate	fV;
		Coordinate	fH;
};



#if		qMacGDI
/*
 * OS Conversions.
 *		NB:	the reason for the asymetry here is that the one arg style is easier, but the
 *			two arg style is necessary when we dont know the size of the return type (as is the
 *			case with osPoint since most people dont include the os headers.
 */
extern	const struct osPoint&	os_cvt (const Point& from, struct osPoint& to);
extern	Point					os_cvt (const osPoint& from);
#endif	/*qMacGDI*/


/*
 * iostream support.
 */
extern	class ostream&	operator<< (class ostream& out, const Point& p);
extern	class istream&	operator>> (class istream& in, Point& p);


extern	const	Point	kZeroPoint;
extern	const	Point	kMinPoint;
extern	const	Point	kMaxPoint;


extern	Point	operator+ (const Point& lhs, const Point& rhs);
extern	Point	operator- (const Point& rhs);
extern	Point	operator- (const Point& lhs, const Point& rhs);
extern	Point	operator* (const Point& lhs, double rhs);
extern	Point	operator* (double lhs, const Point&  rhs);
extern	Point	operator* (const Point& lhs, const Point& rhs);
extern	Point	operator/ (const Point& lhs, double rhs);
extern	Point	operator% (const Point& lhs, UInt32 rhs);
extern	Point	operator% (const Point& lhs, const Point& rhs);

extern	Point	RoundBy (const Point& lhs, UInt32 rhs);
extern	Point	RoundBy (const Point& lhs, const Point& rhs);




/*
 * comparisons.
 *		NB that the plane is not well-ordered, so not (a<b) does not
 *		neccessarily imply (a>=b).
 */
inline	Boolean	operator== (const Point& lhs, const Point& rhs);
inline	Boolean	operator!= (const Point& lhs, const Point& rhs);
inline	Boolean	operator<= (const Point& lhs, const Point& rhs);
inline	Boolean	operator>= (const Point& lhs, const Point& rhs);
extern	Boolean	operator< (const Point& lhs, const Point& rhs);
extern	Boolean	operator> (const Point& lhs, const Point& rhs);


extern	Point		Min (const Point& lhs, const Point& rhs);		// min of each component
extern	Point		Max (const Point& lhs, const Point& rhs);		// max of each component

extern	Point		Abs (const Point& p);
extern	double		Norm (const Point& p);

extern	double		EuclideanDistance (const Point& a, const Point& b);
extern	Coordinate	AbsoluteDistance (const Point& a, const Point& b);
inline	double		Distance (const Point& a, const Point& b);




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
// DONT WANT Point::Point () BUT NEED TIL I FIX ARRAY<T>
inline	Point::Point () : fV (0), fH (0) 								{}
inline	Point::Point (const Point& from): fV (from.fV), fH (from.fH) 	{}
inline	Point::Point (Coordinate v, Coordinate h): fV (v), fH (h) 		{}
inline	const Point& Point::operator= (const Point& rhs) 				{	fV = rhs.fV; fH = rhs.fH; return (*this);  }
inline	Coordinate	Point::GetV () const								{	return (fV);	}
inline	void		Point::SetV (Coordinate v)							{ 	fV = v;	}
inline	Coordinate	Point::GetH () const								{	return (fH);	}
inline	void		Point::SetH (Coordinate h)							{	fH = h;	}
inline	Boolean	operator== (const Point& lhs, const Point& rhs)
	{
		return Boolean ((lhs.GetV () == rhs.GetV ()) and (lhs.GetH () == rhs.GetH ()));
	}
inline	Boolean	operator!= (const Point& lhs, const Point& rhs)
	{
		return Boolean ((lhs.GetV () != rhs.GetV ()) or (lhs.GetH () != rhs.GetH ()));
	}
inline	Boolean	operator<= (const Point& lhs, const Point& rhs)
	{
		return Boolean ((lhs.GetV () <= rhs.GetV ()) and (lhs.GetH () <= rhs.GetH ()));
	}
inline	Boolean	operator>= (const Point& lhs, const Point& rhs)
	{
		return Boolean ((lhs.GetV () >= rhs.GetV ()) and (lhs.GetH () >= rhs.GetH ()));
	}
inline	double	Distance (const Point& a, const Point& b)			{ return (EuclideanDistance (a, b)); }


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif		/*__Point__*/
