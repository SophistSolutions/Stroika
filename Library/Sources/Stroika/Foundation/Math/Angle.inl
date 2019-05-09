/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Angle_inl_
#define _Stroika_Foundation_Math_Angle_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Configuration/Enumeration.h"
#include "../Debug/Assertions.h"
#include "Common.h"

namespace Stroika::Foundation::Math {

    /*
     ********************************************************************************
     *********************************** Math::Angle ********************************
     ********************************************************************************
     */
    inline constexpr Angle::Angle ()
        : fAngleInRadians_{0}
    {
    }
    inline constexpr Angle::Angle (double angle, AngleFormat angleFormat)
        : fAngleInRadians_{
              (angleFormat == AngleFormat::eRadians)
                  ? angle
                  : (
                        (angleFormat == AngleFormat::eDegrees)
                            ? (angle * (2 * kPi) / 360.0)
                            : (
                                  angle * (2 * kPi) / 400.0))}
    {
        using namespace Configuration;
        Require (ToInt (AngleFormat::eSTART) <= ToInt (angleFormat) and ToInt (angleFormat) < ToInt (AngleFormat::eEND));
    }
    inline constexpr double Angle::AsRadians () const
    {
        return fAngleInRadians_;
    }
    inline constexpr double Angle::AsDegrees () const
    {
        return fAngleInRadians_ * 360.0 / (2 * kPi);
    }
    inline constexpr double Angle::AsGradians () const
    {
        return fAngleInRadians_ * 400.0 / (2 * kPi);
    }
    inline const Angle& Angle::operator+= (const Angle& rhs)
    {
        fAngleInRadians_ += rhs.AsRadians ();
        return *this;
    }
    inline const Angle& Angle::operator-= (const Angle& rhs)
    {
        fAngleInRadians_ -= rhs.AsRadians ();
        return *this;
    }
    inline const Angle& Angle::operator*= (double rhs)
    {
        fAngleInRadians_ *= rhs;
        return *this;
    }
    inline const Angle& Angle::operator/= (double rhs)
    {
        fAngleInRadians_ /= rhs;
        return *this;
    }

    /*
     ********************************************************************************
     ************************* Angle::ThreeWayComparer ******************************
     ********************************************************************************
     */
    constexpr int Angle::ThreeWayComparer::operator() (const Angle& lhs, const Angle& rhs) const
    {
        return lhs.AsRadians () - rhs.AsRadians ();
    }

    /*
     ********************************************************************************
     ******************** Math::Angle relationship operators ************************
     ********************************************************************************
     */
    constexpr bool operator< (const Angle& lhs, const Angle& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) < 0;
    }
    constexpr bool operator<= (const Angle& lhs, const Angle& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) <= 0;
    }
    constexpr bool operator== (const Angle& lhs, const Angle& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) == 0;
    }
    constexpr bool operator!= (const Angle& lhs, const Angle& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) != 0;
    }
    constexpr bool operator>= (const Angle& lhs, const Angle& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) >= 0;
    }
    constexpr bool operator> (const Angle& lhs, const Angle& rhs)
    {
        return Common::ThreeWayCompare (lhs, rhs) > 0;
    }

    /*
     ********************************************************************************
     ************************* Math::Angle operators ********************************
     ********************************************************************************
     */
    constexpr Angle operator+ (const Angle& lhs, const Angle& rhs)
    {
        return Angle (lhs.AsRadians () + rhs.AsRadians ());
    }
    constexpr Angle operator- (const Angle& lhs, const Angle& rhs)
    {
        return Angle (lhs.AsRadians () - rhs.AsRadians ());
    }
    constexpr Angle operator* (const Angle& lhs, double rhs)
    {
        return Angle (lhs.AsRadians () * rhs);
    }
    constexpr Angle operator/ (const Angle& lhs, double rhs)
    {
        return Angle (lhs.AsRadians () / rhs);
    }
    constexpr Angle operator* (double lhs, const Angle& rhs)
    {
        return Angle (lhs * rhs.AsRadians ());
    }
    constexpr Angle operator* (const Angle& lhs, const Angle& rhs)
    {
        return Angle (lhs.AsRadians () * rhs.AsRadians ());
    }

    /*
     ********************************************************************************
     *********************************** Math::Min **********************************
     ********************************************************************************
     */
    constexpr Angle Min (const Angle& a1, const Angle& a2)
    {
        return (a1 < a2) ? a1 : a2;
    }

    /*
     ********************************************************************************
     *********************************** Math::Max **********************************
     ********************************************************************************
     */
    constexpr Angle Max (const Angle& a1, const Angle& a2)
    {
        return (a1 > a2) ? a1 : a2;
    }

}

#endif /*_Stroika_Foundation_Math_Angle_inl_*/
