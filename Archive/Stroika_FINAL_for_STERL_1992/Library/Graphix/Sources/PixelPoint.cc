/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelPoint.cc,v 1.2 1992/09/01 15:36:53 sterling Exp $
 *
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: PixelPoint.cc,v $
 *		Revision 1.2  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *
 */

#include	<math.h>

#include	"Debug.hh"
#include	"Math.hh"

#include	"PixelPoint.hh"





/*
 ********************************************************************************
 ************************************ PixelPoint ********************************
 ********************************************************************************
 */

void	PixelPoint::operator+= (const PixelPoint& delta)
{
	fV += delta.fV;
	fH += delta.fH;
}

void	PixelPoint::operator-= (const PixelPoint& delta)
{
	fV -= delta.fV;
	fH -= delta.fH;
}

void	PixelPoint::operator*= (Real rhs)
{
	fV = PixelCoordinate (fV * rhs);
	fH = PixelCoordinate (fH * rhs);
}

void	PixelPoint::operator/= (Real rhs)
{
	fV = PixelCoordinate (fV / rhs);
	fH = PixelCoordinate (fH / rhs);
}





/*
 ********************************************************************************
 *********************************** operator+ **********************************
 ********************************************************************************
 */

PixelPoint	operator+ (const PixelPoint& lhs, const PixelPoint& rhs)
{
	return (PixelPoint (lhs.GetV () + rhs.GetV (), lhs.GetH () + rhs.GetH ()));
}




/*
 ********************************************************************************
 ************************************ operator- *********************************
 ********************************************************************************
 */
PixelPoint	operator- (const PixelPoint& lhs, const PixelPoint& rhs)
{
	return (PixelPoint (lhs.GetV () - rhs.GetV (), lhs.GetH () - rhs.GetH ()));
}



/*
 ********************************************************************************
 ********************************** operator* ***********************************
 ********************************************************************************
 */
PixelPoint	operator* (const PixelPoint& lhs, Real rhs)
{
	return (PixelPoint (PixelCoordinate (lhs.GetV () * rhs), PixelCoordinate (lhs.GetH () * rhs)));
}

PixelPoint	operator* (Real lhs, const PixelPoint&  rhs)
{
	return (PixelPoint (PixelCoordinate (rhs.GetV () * lhs), PixelCoordinate (rhs.GetH () * lhs)));
}





/*
 ********************************************************************************
 ************************************* operator/ ********************************
 ********************************************************************************
 */
PixelPoint	operator/ (const PixelPoint& lhs, Real rhs)
{
	return (PixelPoint (PixelCoordinate (lhs.GetV () / rhs), PixelCoordinate (lhs.GetH () / rhs)));
}






/*
 ********************************************************************************
 ************************************* operator< ********************************
 ********************************************************************************
 */
Boolean	operator< (const PixelPoint& lhs, const PixelPoint& rhs)
{
	return Boolean ((lhs <= rhs) and (lhs != rhs));
}



/*
 ********************************************************************************
 ************************************* operator> ********************************
 ********************************************************************************
 */
Boolean	operator> (const PixelPoint& lhs, const PixelPoint& rhs)
{
	return Boolean ((lhs >= rhs) and (lhs != rhs));
}




/*
 ********************************************************************************
 **************************************** Abs ***********************************
 ********************************************************************************
 */
PixelPoint	Abs (const PixelPoint& p)
{
	return (PixelPoint (Abs (p.GetV ()), Abs (p.GetH ())));
}





/*
 ********************************************************************************
 ************************************* Norm *************************************
 ********************************************************************************
 */
Real	Norm (const PixelPoint& p)
{
	return (sqrt (p.GetV () * p.GetV () + p.GetH () * p.GetH ()));
}



/*
 ********************************************************************************
 ******************************* EuclideanDistance ******************************
 ********************************************************************************
 */
Real	EuclideanDistance (const PixelPoint& a, const PixelPoint& b)
{
	return (Norm (a-b));
}



/*
 ********************************************************************************
 ********************************** AbsoluteDistance ****************************
 ********************************************************************************
 */
PixelCoordinate	AbsoluteDistance (const PixelPoint& a, const PixelPoint& b)
{
	return (Abs (a.GetV () - b.GetV ()) + Abs (a.GetH () - b.GetH ()));
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

