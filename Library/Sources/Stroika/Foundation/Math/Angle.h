/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Angle_h_
#define _Stroika_Foundation_Math_Angle_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Math {

    /**
     *  \todo Consider if this class should automatically normalize (0..2pi) or at least have a normalize method.
     */
    class Angle {
    public:
        using RepType = double;

    public:
        enum class AngleFormat : uint8_t {
            eRadians,
            eDegrees,
            eGradians,

            Stroika_Define_Enum_Bounds (eRadians, eGradians)
        };

    public:
        static constexpr AngleFormat eRadians  = AngleFormat::eRadians;
        static constexpr AngleFormat eDegrees  = AngleFormat::eDegrees;
        static constexpr AngleFormat eGradians = AngleFormat::eGradians;

    public:
        /**
         */
        constexpr Angle ();
        constexpr Angle (RepType angle, AngleFormat angleFormat);

    public:
        /**
         */
        nonvirtual constexpr RepType AsRadians () const;

    public:
        /**
         */
        nonvirtual constexpr RepType AsDegrees () const;

    public:
        /**
         */
        nonvirtual constexpr RepType AsGradians () const;

    public:
        /**
         *
         *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
         *      o   static_assert (totally_ordered<Angle>);
         *      o   partial_ordering because floating point admits only partial ordering.
         *      
         */
        nonvirtual partial_ordering operator<=> (const Angle& rhs) const = default;

    public:
        nonvirtual const Angle& operator+= (const Angle& rhs);
        nonvirtual const Angle& operator-= (const Angle& rhs);
        nonvirtual const Angle& operator*= (RepType rhs);
        nonvirtual const Angle& operator/= (RepType rhs);

    private:
        double fAngleInRadians_;
    };
    static_assert (totally_ordered<Angle>);

    /**
     */
    constexpr Angle operator+ (const Angle& lhs, const Angle& rhs);

    /**
     */
    constexpr Angle operator- (const Angle& lhs, const Angle& rhs);

    /**
     */
    constexpr Angle operator* (double lhs, const Angle& rhs);
    constexpr Angle operator* (const Angle& lhs, Angle::RepType rhs);
    constexpr Angle operator* (const Angle& lhs, Angle& rhs);

    /**
     */
    constexpr Angle operator/ (const Angle& lhs, Angle::RepType rhs);

    inline namespace Literals {
        /**
     */
        constexpr Angle operator"" _deg (long double n) noexcept;
        constexpr Angle operator"" _deg (unsigned long long int n) noexcept;

        /**
     */
        constexpr Angle operator"" _rad (long double n) noexcept;
        constexpr Angle operator"" _rad (unsigned long long int n) noexcept;
    }

    /**
     */
    constexpr Angle Min (const Angle& a1, const Angle& a2);

    /**
     */
    constexpr Angle Max (const Angle& a1, const Angle& a2);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Angle.inl"

#endif /*_Stroika_Foundation_Math_Angle_h_*/
