/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Math_Angle_inl_
#define	_Stroika_Foundation_Math_Angle_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Math {

			inline	Angle::Angle ()
				: fAngleInRadians (0)
				{
				}
			inline	Angle::Angle (double angle, AngleFormat angleFormat)
				: fAngleInRadians (angle)
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
			inline	double	Angle::AsRadians () const
				{
					return (fAngleInRadians);
				}
			inline	double	Angle::AsDegrees () const
				{
					return (fAngleInRadians * 360.0/(2*kPi));
				}
			inline	double	Angle::AsGradians () const
				{
					return (fAngleInRadians * 400.0/(2*kPi));
				}
			inline	const	Angle&	Angle::operator+= (const Angle& rhs)
				{
					fAngleInRadians += rhs.AsRadians ();
					return (*this);
				}
			inline	const	Angle&	Angle::operator-= (const Angle& rhs)
				{
					fAngleInRadians -= rhs.AsRadians ();
					return (*this);
				}
			inline	const	Angle&	Angle::operator*= (double rhs)
				{
					fAngleInRadians *= rhs;
					return (*this);
				}
			inline	const	Angle&	Angle::operator/= (double rhs)
				{
					fAngleInRadians /= rhs;
					return (*this);
				}

			
			
			inline	Angle	operator+ (const Angle& lhs, const Angle& rhs)
				{
					return (Angle (lhs.AsRadians () + rhs.AsRadians ()));
				}
			inline	Angle	operator- (const Angle& lhs, const Angle& rhs)
				{
					return (Angle (lhs.AsRadians () - rhs.AsRadians ()));
				}
			inline	Angle	operator* (const Angle& lhs, double rhs)
				{
					return (Angle (lhs.AsRadians () * rhs));
				}
			inline	Angle	operator* (double lhs, const Angle& rhs)
				{
					return (Angle (lhs * rhs.AsRadians ()));
				}
			inline	Angle	operator/ (const Angle& lhs, double rhs)
				{
					return (Angle (lhs.AsRadians () / rhs));
				}
			inline	bool	operator== (const Angle& lhs, const Angle& rhs)
				{
					return (lhs.AsRadians () == rhs.AsRadians ());
				}
			inline	bool	operator!= (const Angle& lhs, const Angle& rhs)
				{
					return (lhs.AsRadians () != rhs.AsRadians ());
				}
			inline	bool	operator< (const Angle& lhs, const Angle& rhs)
				{
					return (lhs.AsRadians () < rhs.AsRadians ());
				}
			inline	bool	operator<= (const Angle& lhs, const Angle& rhs)
				{
					return (lhs.AsRadians () <= rhs.AsRadians ());
				}
			inline	bool	operator> (const Angle& lhs, const Angle& rhs)
				{
					return (lhs.AsRadians () > rhs.AsRadians ());
				}
			inline	bool	operator>= (const Angle& lhs, const Angle& rhs)
				{
					return (lhs.AsRadians () >= rhs.AsRadians ());
				}
			inline	Angle	Min (const Angle& a1, const Angle& a2)
				{
					return ((a1 < a2)? a1: a2);
				}
			inline	Angle	Max (const Angle& a1, const Angle& a2)
				{
					return ((a1 > a2)? a1: a2);
				}

		}
	}
}
#endif	/*_Stroika_Foundation_Math_Angle_inl_*/



