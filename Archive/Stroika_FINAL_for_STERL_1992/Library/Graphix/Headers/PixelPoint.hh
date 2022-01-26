/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef		__PixelPoint__
#define		__PixelPoint__

/*
 * $Header: /fuji/lewis/RCS/PixelPoint.hh,v 1.2 1992/09/01 15:34:49 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: PixelPoint.hh,v $
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *
 */

#include	"Config-Graphix.hh"

typedef	UInt16	PixelCoordinate;		// only allow up to 64K coordinates...

class	PixelPoint {
	public:
		inline	PixelPoint () { fV = 0; fH = 0; };		// DONT WANT THIS BUT NEED TIL I FIX ARRAY<T>

		inline	PixelPoint (const PixelPoint& from);
		inline	PixelPoint (PixelCoordinate v, PixelCoordinate h);

		inline	const	PixelPoint&	operator= (const PixelPoint& rhs);

		inline		PixelCoordinate	GetV () const					{ return (fV); }
		inline		void			SetV (PixelCoordinate v)		{ fV = v; }
		inline		PixelCoordinate	GetH () const					{ return (fH); }
		inline		void			SetH (PixelCoordinate h)		{ fH = h; }

		nonvirtual	void	operator+= (const PixelPoint& delta);
		nonvirtual	void	operator-= (const PixelPoint& delta);
		nonvirtual	void	operator*= (Real rhs);
		nonvirtual	void	operator/= (Real rhs);

	private:
		PixelCoordinate	fV;
		PixelCoordinate	fH;
};

PixelPoint	kZeroPixelPoint	=	PixelPoint (0, 0);

extern	PixelPoint	operator+ (const PixelPoint& lhs, const PixelPoint& rhs);
extern	PixelPoint	operator- (const PixelPoint& lhs, const PixelPoint& rhs);
extern	PixelPoint	operator* (const PixelPoint& lhs, Real rhs);
extern	PixelPoint	operator* (Real lhs, const PixelPoint&  rhs);
extern	PixelPoint	operator/ (const PixelPoint& lhs, Real rhs);



/*
 * comparisons.
 *		NB that the plane is not well-ordered, so not (a<b) does not
 *		neccessarily imply (a>=b).
 */
inline	Boolean	operator== (const PixelPoint& lhs, const PixelPoint& rhs);
inline	Boolean	operator!= (const PixelPoint& lhs, const PixelPoint& rhs);
inline	Boolean	operator<= (const PixelPoint& lhs, const PixelPoint& rhs);
inline	Boolean	operator>= (const PixelPoint& lhs, const PixelPoint& rhs);
extern	Boolean	operator< (const PixelPoint& lhs, const PixelPoint& rhs);
extern	Boolean	operator> (const PixelPoint& lhs, const PixelPoint& rhs);


extern	PixelPoint		Abs (const PixelPoint& p);
extern	Real			Norm (const PixelPoint& p);

extern	Real			EuclideanDistance (const PixelPoint& a, const PixelPoint& b);
extern	PixelCoordinate	AbsoluteDistance (const PixelPoint& a, const PixelPoint& b);
inline	Real			Distance (const PixelPoint& a, const PixelPoint& b)	{ return (EuclideanDistance (a, b)); }




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	PixelPoint::PixelPoint (const PixelPoint& from):
	fV (from.fV),
	fH (from.fH)
{
}

inline	PixelPoint::PixelPoint (PixelCoordinate v, PixelCoordinate h):
	fV (v),
	fH (h)
{
}

inline	const	PixelPoint&	PixelPoint::operator= (const PixelPoint& rhs)
{
	fV = rhs.fV;
	fH = rhs.fH;
	return (*this);
}





inline	Boolean	operator== (const PixelPoint& lhs, const PixelPoint& rhs)
{
	return Boolean ((lhs.GetV () == rhs.GetV ()) and (lhs.GetH () == rhs.GetH ()));
}

inline	Boolean	operator!= (const PixelPoint& lhs, const PixelPoint& rhs)
{
	return Boolean ((lhs.GetV () != rhs.GetV ()) or (lhs.GetH () != rhs.GetH ()));
}

inline	Boolean	operator<= (const PixelPoint& lhs, const PixelPoint& rhs)
{
	return Boolean ((lhs.GetV () <= rhs.GetV ()) and (lhs.GetH () <= rhs.GetH ()));
}

inline	Boolean	operator>= (const PixelPoint& lhs, const PixelPoint& rhs)
{
	return Boolean ((lhs.GetV () >= rhs.GetV ()) and (lhs.GetH () >= rhs.GetH ()));
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif		/*__PixelPoint__*/
