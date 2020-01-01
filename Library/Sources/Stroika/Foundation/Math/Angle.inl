/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
    inline constexpr Angle::Angle (RepType angle, AngleFormat angleFormat)
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
    inline constexpr Angle::RepType Angle::AsRadians () const
    {
        return fAngleInRadians_;
    }
    inline constexpr Angle::RepType Angle::AsDegrees () const
    {
        return fAngleInRadians_ * 360.0 / (2 * kPi);
    }
    inline constexpr Angle::RepType Angle::AsGradians () const
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
    inline const Angle& Angle::operator*= (RepType rhs)
    {
        fAngleInRadians_ *= rhs;
        return *this;
    }
    inline const Angle& Angle::operator/= (RepType rhs)
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
        return Common::ThreeWayCompareNormalizer (lhs.AsRadians (), rhs.AsRadians ());
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
        return Angle (lhs.AsRadians () + rhs.AsRadians (), Angle::eRadians);
    }
    constexpr Angle operator- (const Angle& lhs, const Angle& rhs)
    {
        return Angle (lhs.AsRadians () - rhs.AsRadians (), Angle::eRadians);
    }
    constexpr Angle operator* (const Angle& lhs, Angle::RepType rhs)
    {
        return Angle (lhs.AsRadians () * rhs, Angle::eRadians);
    }
    constexpr Angle operator/ (const Angle& lhs, Angle::RepType rhs)
    {
        return Angle (lhs.AsRadians () / rhs, Angle::eRadians);
    }
    constexpr Angle operator* (double lhs, const Angle& rhs)
    {
        return Angle (lhs * rhs.AsRadians (), Angle::eRadians);
    }
    constexpr Angle operator* (const Angle& lhs, const Angle& rhs)
    {
        return Angle (lhs.AsRadians () * rhs.AsRadians (), Angle::eRadians);
    }
    constexpr Angle operator""_deg (long double n) noexcept
    {
        return Angle{static_cast<Angle::RepType> (n), Angle::eDegrees};
    }
    constexpr Angle operator""_deg (unsigned long long int n) noexcept
    {
        return Angle{static_cast<Angle::RepType> (n), Angle::eDegrees};
    }
    constexpr Angle operator""_rad (long double n) noexcept
    {
        return Angle{static_cast<Angle::RepType> (n), Angle::eRadians};
    }
    constexpr Angle operator""_rad (unsigned long long int n) noexcept
    {
        return Angle{static_cast<Angle::RepType> (n), Angle::eRadians};
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
