/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Angle.cc,v 1.7 1992/11/12 08:13:02 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Angle.cc,v $
 *		Revision 1.7  1992/11/12  08:13:02  lewis
 *		Use double instead of Real.
 *
 *		Revision 1.6  1992/09/25  21:09:58  lewis
 *		Get rid of Declares/Implements for containers of Angle - no longer needed
 *
 *		Revision 1.5  1992/09/18  04:08:57  sterling
 *		Got rid of class Collection, and moved nearly all the content
 *		for CollectionRep to independent abstract subclasses (eg StackRep).
 *		Made CollectionRep/CollectionView untemplated.
 *
 *		Revision 1.4  1992/09/15  17:21:31  lewis
 *		Got rid of AbSequence.
 *
 *		Revision 1.3  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.2  1992/07/08  01:11:04  lewis
 *		Cleanups - new coding style.
 *
 *		Revision 1.1  1992/06/19  22:29:57  lewis
 *		Initial revision
 *
 *		Revision 1.9  92/04/15  19:08:02  19:08:02  lewis (Lewis Pringle)
 *		Ooops - changed operatorX methods to operatorX=.
 *		
 *		Revision 1.7  92/04/15  14:29:37  14:29:37  lewis (Lewis Pringle)
 *		Added ?= operators for +-*, and / - not sure I handled case of overflow/wrapping properly - must
 *		rethink this policy.
 *		
 *		Revision 1.5  92/03/30  14:30:51  14:30:51  lewis (Lewis Pringle)
 *		Ported to Borland C++ and wrapped Implement macro calls in #ifndef templates available.
 *		
 *
 */



#include	"Debug.hh"
#include	"Math.hh"

#include	"Angle.hh"






/*
 ********************************************************************************
 ************************************** Angle ***********************************
 ********************************************************************************
 */

Angle::Angle () :
	fAngleInRadians (0)
{
}

Angle::Angle (double angle, AngleFormat angleFormat) :
	fAngleInRadians (angle)
{
	switch (angleFormat) {
		case eRadians:	
			break;

		case eDegrees:	
			fAngleInRadians *= (2*kPi) / 360.0; 
			break;

		case eGradians:	
			fAngleInRadians *= (2*kPi) / 400.0; 
			break;

		default:			
			RequireNotReached ();
	}
}

double	Angle::AsDegrees () const
{
	return (fAngleInRadians * 360.0/(2*kPi));
}

double	Angle::AsGradians () const
{
	return (fAngleInRadians * 400.0/(2*kPi));
}

const	Angle&	Angle::operator+= (const Angle& rhs)
{
	fAngleInRadians += rhs.AsRadians ();
	return (*this);
}

const	Angle&	Angle::operator-= (const Angle& rhs)
{
	fAngleInRadians -= rhs.AsRadians ();
	return (*this);
}

const	Angle&	Angle::operator*= (double rhs)
{
	fAngleInRadians *= rhs;
	return (*this);
}

const	Angle&	Angle::operator/= (double rhs)
{
	fAngleInRadians /= rhs;
	return (*this);
}










/*
 ********************************************************************************
 ********************************** operator+ ***********************************
 ********************************************************************************
 */
Angle	operator+ (const Angle& lhs, const Angle& rhs)
{
	return (Angle (lhs.AsRadians () + rhs.AsRadians ()));
}










/*
 ********************************************************************************
 ********************************** operator- ***********************************
 ********************************************************************************
 */
Angle	operator- (const Angle& lhs, const Angle& rhs)
{
	return (Angle (lhs.AsRadians () - rhs.AsRadians ()));
}










/*
 ********************************************************************************
 ********************************** operator* ***********************************
 ********************************************************************************
 */
Angle	operator* (const Angle& lhs, double rhs)
{
	return (Angle (lhs.AsRadians () * rhs));
}

Angle	operator* (double lhs, const Angle& rhs)
{
	return (Angle (lhs * rhs.AsRadians ()));
}










/*
 ********************************************************************************
 ********************************** operator/ ***********************************
 ********************************************************************************
 */
Angle	operator/ (const Angle& lhs, double rhs)
{
	return (Angle (lhs.AsRadians () / rhs));
}










/*
 ********************************************************************************
 ********************************* operator== ***********************************
 ********************************************************************************
 */
Boolean	operator== (const Angle& lhs, const Angle& rhs)
{
	return (Boolean (lhs.AsRadians () == rhs.AsRadians ()));
}










/*
 ********************************************************************************
 ********************************* operator!= ***********************************
 ********************************************************************************
 */
Boolean	operator!= (const Angle& lhs, const Angle& rhs)
{
	return (Boolean (lhs.AsRadians () != rhs.AsRadians ()));
}




/*
 ********************************************************************************
 ********************************** operator< ***********************************
 ********************************************************************************
 */
Boolean	operator< (const Angle& lhs, const Angle& rhs)
{
	return (Boolean (lhs.AsRadians () < rhs.AsRadians ()));
}










/*
 ********************************************************************************
 ********************************** operator<= **********************************
 ********************************************************************************
 */
Boolean	operator<= (const Angle& lhs, const Angle& rhs)
{
	return (Boolean (lhs.AsRadians () <= rhs.AsRadians ()));
}










/*
 ********************************************************************************
 ********************************** operator> ***********************************
 ********************************************************************************
 */
Boolean	operator> (const Angle& lhs, const Angle& rhs)
{
	return (Boolean (lhs.AsRadians () > rhs.AsRadians ()));
}










/*
 ********************************************************************************
 ********************************** operator>= **********************************
 ********************************************************************************
 */
Boolean	operator>= (const Angle& lhs, const Angle& rhs)
{
	return (Boolean (lhs.AsRadians () >= rhs.AsRadians ()));
}










/*
 ********************************************************************************
 ***************************************** Min **********************************
 ********************************************************************************
 */
Angle	Min (const Angle& a1, const Angle& a2)
{
	return ((a1 < a2)? a1: a2);
}










/*
 ********************************************************************************
 ***************************************** Max **********************************
 ********************************************************************************
 */
Angle	Max (const Angle& a1, const Angle& a2)
{
	return ((a1 > a2)? a1: a2);
}










// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

