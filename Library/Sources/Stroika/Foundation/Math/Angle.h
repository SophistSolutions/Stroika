/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Angle_h_
#define _Stroika_Foundation_Math_Angle_h_   1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

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
                    eGradians
                };
                Angle ();
                Angle (double angle, AngleFormat angleFormat = AngleFormat::eRadians);

                nonvirtual  double  AsRadians () const;
                nonvirtual  double  AsDegrees () const;
                nonvirtual  double  AsGradians () const;

                nonvirtual  const   Angle&  operator+= (const Angle& rhs);
                nonvirtual  const   Angle&  operator-= (const Angle& rhs);
                nonvirtual  const   Angle&  operator*= (double rhs);
                nonvirtual  const   Angle&  operator/= (double rhs);

            private:
                double  fAngleInRadians_;
            };


            /*
             * Arithmatic
             */
            extern  Angle   operator+ (const Angle& lhs, const Angle& rhs);
            extern  Angle   operator- (const Angle& lhs, const Angle& rhs);
            extern  Angle   operator* (const Angle& lhs, double rhs);
            extern  Angle   operator* (double lhs, const Angle& rhs);
            extern  Angle   operator/ (const Angle& lhs, double rhs);


            /*
             * Comparisons
             */
            extern  bool    operator== (const Angle& lhs, const Angle& rhs);
            extern  bool    operator!= (const Angle& lhs, const Angle& rhs);
            extern  bool    operator<  (const Angle& lhs, const Angle& rhs);
            extern  bool    operator<= (const Angle& lhs, const Angle& rhs);
            extern  bool    operator>  (const Angle& lhs, const Angle& rhs);
            extern  bool    operator>= (const Angle& lhs, const Angle& rhs);


        }
    }
}


#endif  /*_Stroika_Foundation_Math_Angle_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Angle.inl"
