/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Angle_h_
#define _Stroika_Foundation_Math_Angle_h_   1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Configuration/Enumeration.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Math {


            /**
             */
            class   Angle {
            public:
                enum class AngleFormat : uint8_t {
                    eRadians,
                    eDegrees,
                    eGradians,

                    Stroika_Define_Enum_Bounds(eRadians, eGradians)
                };

            public:
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                Angle ();
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                Angle (double angle, AngleFormat angleFormat = AngleFormat::eRadians);

            public:
                nonvirtual  constexpr double  AsRadians () const;

            public:
                nonvirtual  constexpr double  AsDegrees () const;

            public:
                nonvirtual  constexpr double  AsGradians () const;

            public:
                /*
                 * Arithmatic
                 */
                nonvirtual  Angle   operator+ (const Angle& rhs) const;
                nonvirtual  Angle   operator- (const Angle& rhs) const;
                nonvirtual  Angle   operator* (double rhs) const;
                nonvirtual  Angle   operator* (Angle& rhs) const;
                nonvirtual  Angle   operator/ (double rhs) const;

            public:
                /*
                 * Comparisons
                 */
                nonvirtual  bool    operator== (const Angle& rhs) const;
                nonvirtual  bool    operator!= (const Angle& rhs) const;
                nonvirtual  bool    operator<  (const Angle& rhs) const;
                nonvirtual  bool    operator<= (const Angle& rhs) const;
                nonvirtual  bool    operator>  (const Angle& rhs) const;
                nonvirtual  bool    operator>= (const Angle& rhs) const;

            public:
                nonvirtual  const   Angle&  operator+= (const Angle& rhs);
                nonvirtual  const   Angle&  operator-= (const Angle& rhs);
                nonvirtual  const   Angle&  operator*= (double rhs);
                nonvirtual  const   Angle&  operator/= (double rhs);

            private:
                double  fAngleInRadians_;
            };

            Angle   operator* (double lhs, const Angle& rhs);

            Angle   Min (const Angle& a1, const Angle& a2);
            Angle   Max (const Angle& a1, const Angle& a2);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Angle.inl"

#endif  /*_Stroika_Foundation_Math_Angle_h_*/
