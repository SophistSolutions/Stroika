/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Common_h_
#define _Stroika_Foundation_Math_Common_h_   1

#include    "../StroikaPreComp.h"

#include    <cmath>         // though perhaps not strictly needed, probably wanted if including Stroika/Foundation/Math/Common.h
#include    <limits>
#include    <type_traits>


/**
 *  TODO:
 *      @todo   This rounding code doesn't work for floating point numbers. That should either be fixed
 *              or documented better why.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Math {


            using   std::numeric_limits;


            double  nan ();


            constexpr   double  kE  =   2.71828182845904523536;
            constexpr   double  kPi =   3.14159265358979323846;


            /**
             * RoundUpTo() - round towards positive infinity.
             *
             *  T can be any of int, long, or long long, or unsigned int, or unsigned long, or unsigned long long
             *  (after review of this API/implementation, probably should add short/char with unsigned variants
             *  to this list)
             */
            template    <typename T>
            T   RoundUpTo (T x, T toNearest);


            /**
             *  RoundDownTo() - round towards negative infinity.
             *
             *  T can be any of int, long, or long long, or unsigned int, or unsigned long, or unsigned long long
             *  (after review of this API/implementation, probably should add short/char with unsigned variants
             *  to this list)
             */
            template    <typename T>
            T   RoundDownTo (T x, T toNearest);


            /**
             *  NearlyEquals() can be used as a utility for floating point comparisons.
             */
#if  qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompilerBug
            template    <typename   T>
            T   NearlyEquals (T l, T r, T epsilon = (1000 * numeric_limits<float>::epsilon()), typename std::enable_if<std::is_floating_point<T>::value >::type* = 0);
#else
            template    <typename   T>
            T   NearlyEquals (T l, T r, T epsilon = (1000 * numeric_limits<T>::epsilon()), typename std::enable_if<std::is_floating_point<T>::value >::type* = 0);
#endif
            template    <typename   T>
            T   NearlyEquals (T l, T r, T epsilon = 1, typename std::enable_if<std::is_integral<T>::value >::type* = 0);



            /*
             *  \brief  PinToSpecialPoint() returns its first argument, or someting NearlyEqual() to it (but better)
             *
             *  Sometimes with floating point arithmatic you get points to move slightly. For example, if you
             *  want a number between 0.0 and 1.0, you might do some arithmatic and get -0.000000001; This might
             *  cause the valeu to be out of the range 0..1. You might want to assert/assure the value never goes below
             *  zero or above one, but this is outside that range.
             *
             *  This helper allows values near a special value (like the endpoint of that range) to 'pin' to be
             *  exactly that endpoint.
             *
             *  But PinToSpecialPoint() always returns its first argument, or someting NearlyEqual() to it.
             */
#if  qCompilerAndStdLib_TemplateCompileWithNumericLimitsCompilerBug
            template    <typename   T>
            T   PinToSpecialPoint (T p, T special, T epsilon = (1000 * numeric_limits<float>::epsilon()));
#else
            template    <typename   T>
            T   PinToSpecialPoint (T p, T special, T epsilon = (1000 * numeric_limits<T>::epsilon()));
#endif

            /**
             */
            template    <typename   T>
            T   PinInRange (T initialValue, T lowerBound, T upperBound);


            /**
             *  Note - when we port stroika bignum package - this shoudl support those bignums.
             */
            template    <typename   T>
            bool    IsOdd (T v);


            /**
             *  Note - when we port stroika bignum package - this shoudl support those bignums.
             */
            template    <typename   T>
            bool    IsEven (T v);


            /**
             *  Note - when we port stroika bignum package - this shoudl support those bignums.
             *
             *  @todo   Also - we want multiple algiorths for this - including simple search (done now), sieve, and
             *          partitioned sieves (so use less ram).
             *
             *  \req v >= 0
             */
            template    <typename   T>
            bool    IsPrime (T v);


        }
    }
}


#if     !qCompilerAndStdLib_Supports_isnan
namespace   std {
    // Must be in std namespace
    bool    isnan (float f);
    bool    isnan (double d);
}
#endif



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Common.inl"

#endif  /*_Stroika_Foundation_Math_Common_h_*/
