/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Common_inl_
#define _Stroika_Foundation_Math_Common_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Math {


            /*
             ********************************************************************************
             ************************************ Math::nan *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  T  nan ()
            {
                return numeric_limits<T>::quiet_NaN ();
            }


            /*
             ********************************************************************************
             ****************************** Math::RoundUpTo *********************************
             ********************************************************************************
             */
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


            /*
             ********************************************************************************
             **************************** Math::RoundDownTo *********************************
             ********************************************************************************
             */
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


            /*
             ********************************************************************************
             **************************** Math::NearlyEquals ********************************
             ********************************************************************************
             */
            template    <typename   T>
            inline  bool    NearlyEquals (T l, T r, T epsilon, typename std::enable_if<std::is_floating_point<T>::value >::type*)
            {
                return std::fabs ( l - r ) <= epsilon;
            }
            template    <typename   T>
            inline  bool    NearlyEquals (T l, T r, T epsilon, typename std::enable_if<std::is_integral<T>::value >::type*)
            {
                return std::abs ( l - r ) <= epsilon;
            }
#if     qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy
            inline  bool   NearlyEquals (float l, float r, float epsilon = (10000 * numeric_limits<float>::epsilon ()))
            {
                return NearlyEquals<float> (l, r, epsilon);
            }
            inline  bool    NearlyEquals (double l, double r, double epsilon = (10000 * numeric_limits<double>::epsilon ()))
            {
                return NearlyEquals<double> (l, r, epsilon);
            }
            inline  bool    NearlyEquals (long double l, long double r, double epsilon = (10000 * numeric_limits<long double>::epsilon ()))
            {
                return NearlyEquals<double> (l, r, epsilon);
            }
#endif


            /*
             ********************************************************************************
             *********************** Math::PinToSpecialPoint ********************************
             ********************************************************************************
             */
            template    <typename   T>
            T   PinToSpecialPoint (T p, T special, T epsilon)
            {
                if (Math::NearlyEquals (p, special, epsilon)) {
                    return special;
                }
                return p;
            }
#if     qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompiler_Buggy
            inline float   PinToSpecialPoint (float p, float special, float epsilon = (100 * numeric_limits<float>::epsilon ()))
            {
                return PinToSpecialPoint<float> (p, special, epsilon);
            }
            inline double   PinToSpecialPoint (double p, double special, double epsilon = (100 * numeric_limits<double>::epsilon ()))
            {
                return PinToSpecialPoint<double> (p, special, epsilon);
            }
            inline long double   PinToSpecialPoint (long double p, long double special, long double epsilon = (100 * numeric_limits<long double>::epsilon ()))
            {
                return PinToSpecialPoint<long double> (p, special, epsilon);
            }
#endif


            /*
             ********************************************************************************
             ***************************** Math::PinInRange *********************************
             ********************************************************************************
             */
            template    <typename   T>
            T   PinInRange (T initialValue, T lowerBound, T upperBound)
            {
                Require (lowerBound <= upperBound);
                return  max (lowerBound, min (upperBound, initialValue));
            }


            /*
             ********************************************************************************
             ************************** Math::PinToMaxForType *******************************
             ********************************************************************************
             */
            template    <typename NEW_T, typename   T>
            NEW_T   PinToMaxForType (T initialValue)
            {
                using   LargerType  =   decltype (NEW_T () + T ()); // maybe should use conditional<> for this?
                return static_cast<NEW_T> (min<LargerType> (initialValue, numeric_limits<NEW_T>::max ()));
            }


            /*
             ********************************************************************************
             ********************************** Math::IsOdd *********************************
             ********************************************************************************
             */
            template    <typename   T>
            bool    IsOdd (T v)
            {
                return v % 2 == 1;
            }


            /*
             ********************************************************************************
             ********************************* Math::IsEven *********************************
             ********************************************************************************
             */
            template    <typename   T>
            bool    IsEven (T v)
            {
                return v % 2 == 0;
            }


            /*
             ********************************************************************************
             ******************************** Math::IsPrime *********************************
             ********************************************************************************
             */
            template    <typename   T>
            bool    IsPrime (T v)
            {
                Require  (v >= 0);  // no negative numbers
                // @todo - redo this as http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes but this is simpler and
                // good enuf for me to test...
                if (v == 1) {
                    return false;
                }
                if (v == 2) {
                    return true;    // special case
                }
                T   checkUpTo = T (sqrt (v) + 1);
                // Check each number from 3 up to checkUpTo and see if its a divisor
                for (T d = 2; d <= checkUpTo; ++d) {
                    if (v % d == 0) {
                        return false;
                    }
                }
                return true;
            }


        }
    }
}



#endif  /*_Stroika_Foundation_Math_Common_inl_*/

