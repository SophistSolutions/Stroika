/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Common.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Debug/Assertions.h"

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
        : fAngleInRadians_{(angleFormat == AngleFormat::eRadians)
                               ? angle
                               : ((angleFormat == AngleFormat::eDegrees) ? (angle * (2 * kPi) / 360.0) : (angle * (2 * kPi) / 400.0))}
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

    /*
     ********************************************************************************
     ****************************** Math::Literals **********************************
     ********************************************************************************
     */
    inline namespace Literals {
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
