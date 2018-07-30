/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
     *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     */
    class Angle {
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
        constexpr Angle (double angle, AngleFormat angleFormat = AngleFormat::eRadians);

    public:
        /**
         */
        nonvirtual constexpr double AsRadians () const;

    public:
        /**
         */
        nonvirtual constexpr double AsDegrees () const;

    public:
        /**
         */
        nonvirtual constexpr double AsGradians () const;

    public:
        nonvirtual const Angle& operator+= (const Angle& rhs);
        nonvirtual const Angle& operator-= (const Angle& rhs);
        nonvirtual const Angle& operator*= (double rhs);
        nonvirtual const Angle& operator/= (double rhs);

    private:
        double fAngleInRadians_;
    };

    /**
     */
    constexpr bool operator< (const Angle& lhs, const Angle& rhs);

    /**
     */
    constexpr bool operator<= (const Angle& lhs, const Angle& rhs);

    /**
     */
    constexpr bool operator== (const Angle& lhs, const Angle& rhs);

    /**
     */
    constexpr bool operator!= (const Angle& lhs, const Angle& rhs);

    /**
     */
    constexpr bool operator>= (const Angle& lhs, const Angle& rhs);

    /**
     */
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
    constexpr Angle operator* (const Angle& lhs, double rhs);
    constexpr Angle operator* (const Angle& lhs, Angle& rhs);

    /**
     */
    Angle operator/ (const Angle& lhs, double rhs);

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
