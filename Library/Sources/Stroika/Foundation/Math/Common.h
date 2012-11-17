/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Basic_h_
#define _Stroika_Foundation_Math_Basic_h_   1

#include    "../StroikaPreComp.h"

#include    <cmath>         // though perhaps not strictly needed, probably wanted if including Stroika/Foundation/Math/Common.h
#include    <limits>


/*
 *  TODO:
 *      o   This rounding code doesn't work for floating point numbers. That should either be fixed or documented better why.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Math {

            double  nan ();

            const   double  kE  =   2.71828182845904523536;
            const   double  kPi =   3.14159265358979323846;


            /*
             * RoundUpTo() - round towards positive infinity.
             * T can be any of int, long, or long long, or unsigned int, or unsigned long, or unsigned long long
             *     (after review of this API/implementation, probably should add short/char with unsigned variants to this list)
             */
            template    <typename T>
            T   RoundUpTo (T x, T toNearest);

            /*
             * RoundDownTo() - round towards negative infinity.
             * T can be any of int, long, or long long, or unsigned int, or unsigned long, or unsigned long long
             *      (after review of this API/implementation, probably should add short/char with unsigned variants to this list)
             */
            template    <typename T>
            T   RoundDownTo (T x, T toNearest);


// Til I can test on GCC...
#ifndef qSomeWeirdMSVCTemplateCompilerBug
#define qSomeWeirdMSVCTemplateCompilerBug  defined (_MSC_VER)
#endif // !qSomeWeirdMSVCTemplateCompilerBug


            /**
             *  NearlyEquals() can be used as a utility for floating point comparisons.
             */
#if  qSomeWeirdMSVCTemplateCompilerBug
            template    <typename   T>
            T   NearlyEquals (T l, T r, T epsilon = (1000 * numeric_limits<float>::epsilon()));
#else
            template    <typename   T>
            T   NearlyEquals (T l, T r, T epsilon = (1000 * numeric_limits<T>::epsilon()));
#endif


        }
    }
}

#if     !qCompilerAndStdLib_isnan
// SB in std namespace
namespace   std {
    bool    isnan (float f);
    bool    isnan (double d);
}
#endif


#endif  /*_Stroika_Foundation_Math_Basic_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Common.inl"


