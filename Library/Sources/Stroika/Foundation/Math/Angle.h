/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Math_Angle_h_
#define _Stroika_Foundation_Math_Angle_h_ 1

#include "../StroikaPreComp.h"

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
     *  @todo https://stroika.atlassian.net/browse/STK-692 - debug threewaycompare/spaceship operator and replicate
     */
    struct Angle::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        constexpr int operator() (const Angle& lhs, const Angle& rhs) const;
    };

    /**
     *  Basic operator overloads with the obivous meaning, and simply indirect to @Common::ThreeWayCompare ()
     *
     *  @todo https://stroika.atlassian.net/browse/STK-692 - debug threewaycompare/spaceship operator and replicate
     */
    constexpr bool operator< (const Angle& lhs, const Angle& rhs);
    constexpr bool operator<= (const Angle& lhs, const Angle& rhs);
    constexpr bool operator== (const Angle& lhs, const Angle& rhs);
    constexpr bool operator!= (const Angle& lhs, const Angle& rhs);
    constexpr bool operator>= (const Angle& lhs, const Angle& rhs);
    constexpr bool operator> (const Angle& lhs, const Angle& rhs);

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
