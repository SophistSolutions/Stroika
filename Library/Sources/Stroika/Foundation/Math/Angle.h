/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Angle_h_
#define _Stroika_Foundation_Math_Angle_h_ 1

#include "../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#include "../Common/Compare.h"
#include "../Configuration/Common.h"
#include "../Configuration/Enumeration.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
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

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual partial_ordering operator<=> (const Angle& rhs) const = default;
#endif

    public:
        nonvirtual const Angle& operator+= (const Angle& rhs);
        nonvirtual const Angle& operator-= (const Angle& rhs);
        nonvirtual const Angle& operator*= (RepType rhs);
        nonvirtual const Angle& operator/= (RepType rhs);

    public:
        struct ThreeWayComparer;

    private:
        double fAngleInRadians_;
    };

    /**
     */
    struct Angle::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        constexpr Common::strong_ordering operator() (const Angle& lhs, const Angle& rhs) const;
    };

#if __cpp_impl_three_way_comparison < 201907
    /**
     *  Basic operator overloads with the obivous meaning, and simply indirect to @Common::ThreeWayCompare ()
     */
    constexpr bool operator< (const Angle& lhs, const Angle& rhs);
    constexpr bool operator<= (const Angle& lhs, const Angle& rhs);
    constexpr bool operator== (const Angle& lhs, const Angle& rhs);
    constexpr bool operator!= (const Angle& lhs, const Angle& rhs);
    constexpr bool operator>= (const Angle& lhs, const Angle& rhs);
    constexpr bool operator> (const Angle& lhs, const Angle& rhs);
#endif

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

    /**
     */
    constexpr Angle operator"" _deg (long double n) noexcept;
    constexpr Angle operator"" _deg (unsigned long long int n) noexcept;

    /**
     */
    constexpr Angle operator"" _rad (long double n) noexcept;
    constexpr Angle operator"" _rad (unsigned long long int n) noexcept;

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
