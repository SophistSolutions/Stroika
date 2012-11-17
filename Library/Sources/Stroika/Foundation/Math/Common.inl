/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Basic_inl_
#define _Stroika_Foundation_Math_Basic_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Math {
            using   namespace   std;
            inline  double  nan ()
            {
                return numeric_limits<double>::quiet_NaN ();
            }

            namespace   Private {
                template    <typename T>
                T   RoundUpTo_UnSignedHelper_ (T x, T toNearest);
                template    <typename T, typename UNSIGNED_T>
                inline  T   RoundUpTo_SignedHelper_ (T x, T toNearest)
                {
                    Require (toNearest > 0);
                    if (x < 0) {
                        return (- static_cast<T> (RoundDownTo (static_cast<UNSIGNED_T> (-x), static_cast<UNSIGNED_T> (toNearest))));
                    }
                    else {
                        return static_cast<T> (RoundUpTo_UnSignedHelper_<UNSIGNED_T> (x, toNearest));
                    }
                }
                template    <typename T>
                inline  T   RoundUpTo_UnSignedHelper_ (T x, T toNearest)
                {
                    return (((x + toNearest - 1u) / toNearest) * toNearest);
                }
                template    <typename T, typename UNSIGNED_T>
                inline  T   RoundDownTo_SignedHelper_ (T x, T toNearest)
                {
                    Require (toNearest > 0);
                    if (x < 0) {
                        return (- static_cast<T> (RoundUpTo (static_cast<UNSIGNED_T> (-x), static_cast<UNSIGNED_T> (toNearest))));
                    }
                    else {
                        return (RoundDownTo (static_cast<UNSIGNED_T> (x), static_cast<UNSIGNED_T> (toNearest)));
                    }
                }
                template    <typename T>
                inline  T   RoundDownTo_UnSignedHelper_ (T x, T toNearest)
                {
                    return ((x / toNearest) * toNearest);
                }
            }



            template    <>
            inline  int RoundUpTo (int x, int toNearest)
            {
                return Private::RoundUpTo_SignedHelper_<int, unsigned int> (x, toNearest);
            }
            template    <>
            inline  long    RoundUpTo (long x, long toNearest)
            {
                return Private::RoundUpTo_SignedHelper_<long, unsigned long> (x, toNearest);
            }
            template    <>
            inline  long long   RoundUpTo (long long x, long long toNearest)
            {
                return Private::RoundUpTo_SignedHelper_<long long, unsigned long long> (x, toNearest);
            }
            template    <>
            inline  unsigned int    RoundUpTo (unsigned int x, unsigned int toNearest)
            {
                return Private::RoundUpTo_UnSignedHelper_<unsigned int> (x, toNearest);
            }
            template    <>
            inline  unsigned long   RoundUpTo (unsigned long x, unsigned long toNearest)
            {
                return Private::RoundUpTo_UnSignedHelper_<unsigned long> (x, toNearest);
            }
            template    <>
            inline  unsigned long long  RoundUpTo (unsigned long long x, unsigned long long toNearest)
            {
                return Private::RoundUpTo_UnSignedHelper_<unsigned long long> (x, toNearest);
            }



            template    <>
            inline  int RoundDownTo (int x, int toNearest)
            {
                return Private::RoundDownTo_SignedHelper_<int, unsigned int> (x, toNearest);
            }
            template    <>
            inline  long    RoundDownTo (long x, long toNearest)
            {
                return Private::RoundDownTo_SignedHelper_<long, unsigned long> (x, toNearest);
            }
            template    <>
            inline  long long   RoundDownTo (long long x, long long toNearest)
            {
                return Private::RoundDownTo_SignedHelper_<long long, unsigned long long> (x, toNearest);
            }
            template    <>
            inline  unsigned int    RoundDownTo (unsigned int x, unsigned int toNearest)
            {
                return Private::RoundDownTo_UnSignedHelper_<unsigned int> (x, toNearest);
            }
            template    <>
            inline  unsigned long   RoundDownTo (unsigned long x, unsigned long toNearest)
            {
                return Private::RoundDownTo_UnSignedHelper_<unsigned long> (x, toNearest);
            }
            template    <>
            inline  unsigned long long  RoundDownTo (unsigned long long x, unsigned long long toNearest)
            {
                return Private::RoundDownTo_UnSignedHelper_<unsigned long long> (x, toNearest);
            }




            template    <typename   FloatType>
            inline  FloatType   NearlyEquals (FloatType l, FloatType r, FloatType epsilon)
            {
                return std::fabs ( l - r ) < epsilon;
            }

        }
    }
}

#if     !qCompilerAndStdLib_isnan
namespace   std {
    inline  bool    isnan (float f)
    {
#if     qCompilerAndStdLib__isnan
        return static_cast<bool> (!!_isnan (f));
#else
        return f != f;
#endif
    }
    inline  bool    isnan (double f)
    {
#if     qCompilerAndStdLib__isnan
        return static_cast<bool> (!!_isnan (f));
#else
        return f != f;
#endif
    }
}
#endif

#endif  /*_Stroika_Foundation_Math_Basic_inl_*/



