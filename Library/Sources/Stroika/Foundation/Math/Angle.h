/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Math_Angle_h_
#define	_Stroika_Foundation_Math_Angle_h_	1

#include	"../StroikaPreComp.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Math {

			class	Angle {
				public:
					enum AngleFormat { eRadians, eDegrees, eGradians };
					Angle ();
					Angle (double angle, AngleFormat angleFormat = eRadians);

					nonvirtual	double	AsRadians () const;
					nonvirtual	double	AsDegrees () const;
					nonvirtual	double	AsGradians () const;

					nonvirtual	const	Angle&	operator+= (const Angle& rhs);
					nonvirtual	const	Angle&	operator-= (const Angle& rhs);
					nonvirtual	const	Angle&	operator*= (double rhs);
					nonvirtual	const	Angle&	operator/= (double rhs);

				private:
					double	fAngleInRadians;
			};


			/*
			 * Arithmatic
			 */
			extern	Angle	operator+ (const Angle& lhs, const Angle& rhs);
			extern	Angle	operator- (const Angle& lhs, const Angle& rhs);
			extern	Angle	operator* (const Angle& lhs, double rhs);
			extern	Angle	operator* (double lhs, const Angle& rhs);
			extern	Angle	operator/ (const Angle& lhs, double rhs);


			/*
			 * Comparisons
			 */
			extern	bool	operator== (const Angle& lhs, const Angle& rhs);
			extern	bool	operator!= (const Angle& lhs, const Angle& rhs);
			extern	bool	operator<  (const Angle& lhs, const Angle& rhs);
			extern	bool	operator<= (const Angle& lhs, const Angle& rhs);
			extern	bool	operator>  (const Angle& lhs, const Angle& rhs);
			extern	bool	operator>= (const Angle& lhs, const Angle& rhs);


			extern	Angle	Min (const Angle& a1, const Angle& a2);
			extern	Angle	Max (const Angle& a1, const Angle& a2);


		}
	}
}

#if 0
/*
 * iostream support. -- NOT SURE IF/HOW TODO WITH STROIKA - DONT WANT TO INTROUCE DEPENDENCY HERE??? MAYBE SEP FILE FOR ANGLE_IOSTREAM?
 */
extern	std::ostream&	operator<< (std::ostream& out, const Stroika::Foundation::Math::Angle& a);
extern	std::istream&	operator>> (std::istream& in, Stroika::Foundation::Math::Angle& a);
#endif

#endif	/*_Stroika_Foundation_Math_Angle_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Angle.inl"
