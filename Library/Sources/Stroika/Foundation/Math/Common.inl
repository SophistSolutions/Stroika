/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Math_Basic_inl_
#define	_Stroika_Foundation_Math_Basic_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	<limits>



namespace	Stroika {
	namespace	Foundation {
		namespace	Math {
            using	namespace	std;
			inline	double	nan ()
				{
					return numeric_limits<double>::quiet_NaN ();
				}



            inline	int			RoundUpTo (unsigned x, unsigned toNearest)
            {
                return (((x+toNearest-1u)/toNearest)*toNearest);
            }

            inline	int			RoundDownTo (unsigned x, unsigned toNearest)
            {
                return ((x/toNearest)*toNearest);
            }

            inline	int			RoundUpTo (int x, unsigned toNearest)
            {
                if (x < 0) {
                    return (- RoundDownTo (unsigned (-x), toNearest));
                }
                else {
                    return (RoundUpTo (unsigned (x), toNearest));
                }
            }
            inline	int			RoundDownTo (int x, unsigned toNearest)
            {
                if (x < 0) {
                    return (- RoundUpTo (unsigned (-x), toNearest));
                }
                else {
                    return (RoundDownTo (unsigned (x), toNearest));
                }
            }
		}
	}
}

#if		!qCompilerAndStdLib_isnan
namespace	std {
	inline	bool	isnan (float f)
		{
			#if		qCompilerAndStdLib__isnan
				return static_cast<bool> (!!_isnan (f));
			#else
				return f != f;
			#endif
		}
	inline	bool	isnan (double f)
		{
			#if		qCompilerAndStdLib__isnan
				return static_cast<bool> (!!_isnan (f));
			#else
				return f != f;
			#endif
		}
}
#endif

#endif	/*_Stroika_Foundation_Math_Basic_inl_*/



