/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Range_h_
#define _Stroika_Foundation_Traversal_Range_h_ 1

#include "../StroikaPreComp.h"

#include <limits>
#include <optional>
#include <type_traits>

#include "../Characters/String.h"
#include "../Characters/ToString.h"
#include "../Configuration/Common.h"

#include "Iterator.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  \em Design Note
 *      This module was inspired by Ruby Range class, but in the end, it was mostly based on HealthFrame's
 *      DateRangeType/DateTimeRangeType code.
 *
 *      I tried to find a way to implement a TRAITS based solution for including MIN-MAX automatically, but
 *      this was very difficult due to the restrictions in C++11 that non-type parameters must be
 *      (essentially) integers. There is the ability to use const-reference data, but attempts to
 *      use that triggered issues with inter-module construction order.
 *
 *      Just explicitly defining traits objects with min/max seems like the way to do for now...
 *          -- LGP 2013-10-17
 *
 *  TODO:
 *          @todo   Carefully review intersection/unionbounds code for new open/closed parameters. Either make sure
 *                  it works or at least more carefully document in method headers the quirks of the
 *                  chosen definition.
 *
 *          @todo   Consider if we want to re-instate Range<T, TRAITS>::Overlaps - but think through and document
 *                  definition clearly.
 *
 *          @todo   Document why no operator< support (compare interface) - cuz no obvious well-ordering?
 *                  Could well order on LHS, and then when equal on RHS, but that wouldn't make sense for
 *                  all applicaitons.
 */

namespace Stroika {
    namespace Foundation {
        namespace Traversal {

            /**
             *  Openness is used to define whether an end of a range is open or closed. Open means
             *  not containing the endpoint, and closed means containing the endpoint.
             *
             *  \note   Configuration::DefaultNames<> supported
             */
            enum class Openness {
                eOpen,
                eClosed,

                Stroika_Define_Enum_Bounds (eOpen, eClosed)
            };

            /**
             */
            namespace RangeTraits {

                /**
                 *  ExplicitRangeTraitsWithoutMinMax<> can be used to specify in line line (type) all the details for the range functionality
                 *  for a given type T.
                 */
                template <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
                struct ExplicitRangeTraitsWithoutMinMax {
                    using value_type             = T;
                    using SignedDifferenceType   = SIGNED_DIFF_TYPE;
                    using UnsignedDifferenceType = UNSIGNED_DIFF_TYPE;

                    static constexpr Openness kLowerBoundOpenness = LOWER_BOUND_OPEN;
                    static constexpr Openness kUpperBoundOpenness = UPPER_BOUND_OPEN;

                    /**
                     *  Return the Next possible value, or if already at the end of the range, the same value.
                     */
                    template <typename SFINAE = value_type>
                    static value_type GetNext (value_type i, enable_if_t<std::is_integral<SFINAE>::value>* = 0);
                    template <typename SFINAE = value_type>
                    static value_type GetNext (value_type i, enable_if_t<std::is_floating_point<SFINAE>::value>* = 0);

                    /**
                     *  Return the Previous possible value, or if already at the end of the range, the same value.
                     */
                    template <typename SFINAE = value_type>
                    static value_type GetPrevious (value_type i, enable_if_t<std::is_integral<SFINAE>::value>* = 0);
                    template <typename SFINAE = value_type>
                    static value_type GetPrevious (value_type i, enable_if_t<std::is_floating_point<SFINAE>::value>* = 0);

                    /**
                     *  Compute the difference between two elements of type T for the Range
                     */
                    template <typename TYPE2CHECK = value_type, typename SFINAE_CAN_CONVERT_TYPE_TO_SIGNEDDIFFTYPE = enable_if_t<std::is_enum<TYPE2CHECK>::value or is_convertible_v<TYPE2CHECK, SignedDifferenceType>>>
                    static constexpr SignedDifferenceType Difference (Configuration::ArgByValueType<value_type> lhs, Configuration::ArgByValueType<value_type> rhs, SFINAE_CAN_CONVERT_TYPE_TO_SIGNEDDIFFTYPE* = 0);
                    template <typename TYPE2CHECK = value_type, typename SFINAE_CANNOT_CONVERT_TYPE_TO_SIGNEDDIFFTYPE = enable_if_t<not(std::is_enum<TYPE2CHECK>::value or is_convertible_v<TYPE2CHECK, SignedDifferenceType>)>>
                    static constexpr SignedDifferenceType Difference (Configuration::ArgByValueType<value_type> lhs, Configuration::ArgByValueType<value_type> rhs, ...);
                };

                /**
                 *  Shorthand helper to generate a traits object with an explicit min/max. Due to limitations
                 *  in c++ (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf -
                 *  14.3.2 Template non-type arguments - about integral types).
                 */
                template <typename T, T MIN, T MAX, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
                struct ExplicitRangeTraits_Integral : public ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
                    static constexpr T kLowerBound = MIN;
                    static constexpr T kUpperBound = MAX;
                };

                namespace Private_ {
                    template <typename T>
                    using BaseDifferenceType_ = decltype (T{} - T{});
                    template <typename T>
                    using UnsignedDifferenceType_ = BaseDifferenceType_<T>;
                }

                /**
                 *  DefaultRangeTraits<> is generally used automatically - when you construct a Range<> object without
                 *  specifying traits.
                 *
                 *  This defaults to a half-open (lhs closed, rhs-open) range.
                 */
                template <typename T>
                struct DefaultRangeTraits : enable_if_t<
                                                is_arithmetic<T>::value,
                                                ExplicitRangeTraitsWithoutMinMax<T, Openness::eClosed, Openness::eOpen, decltype (T{} - T{}), Private_::UnsignedDifferenceType_<T>>> {
                    static constexpr T kLowerBound = numeric_limits<T>::lowest ();
                    static constexpr T kUpperBound = numeric_limits<T>::max ();
                };
            }

            template <typename T, typename RANGE_TYPE>
            class DisjointRange;

            /**
             *  A Range<> is analagous to a mathematical range. It's left and and its right sides can
             *  be optionally open or closed.
             *
             *  This Range<> template is similar to Ruby range.
             *
             *  Somewhat inspired by, and at least influenced by, the definition in
             *      http://ruby-doc.org/core-2.0/Range.html
             *  However - where Ruby has one type "Range" - we have "Range" and DiscreteRange" - and some ruby Range methods/properties
             *  are expressed only in DiscreteRange<>. Also Stroika Range has openneess as an optional/configurable
             *  feature of both endpoints, whereas in Ruby, the LHS is always closed and its RHS is optionally open.
             *
             *  Note - you can do Range<float>, but cannot do DiscreteRange<float> - but can do DiscreteRange<int>.
             *
             *  A note about an empty range. All empty ranges (of the same type) are equal to one another. It is illegal
             *  to ask for the start or end of an empty range. Empty ranges contain no points.
             *
             *  Since a range is half/open/closed by default, this means that
             *      Range<int> (1,1) == Range<int> (3,3) would be true, since the are both empty.
             *
             *  @see DiscreteRange
             *  @see DisjointRange
             *  @see DisjointDiscreteRange
             */
            template <typename T, typename TRAITS = RangeTraits::DefaultRangeTraits<T>>
            class Range {
            public:
                /**
                 */
                using value_type = T;

            public:
                /**
                 */
                using TraitsType = TRAITS;

            public:
                /**
                 */
                using SignedDifferenceType = typename TraitsType::SignedDifferenceType;

            public:
                /**
                 */
                using UnsignedDifferenceType = typename TraitsType::UnsignedDifferenceType;

            public:
                /**
                 *  Range () creates an empty range.
                 *
                 *  optional values - if omitted - are replaced with the TRAITS::kLowerBound and TRAITS::kUpperBound values (as well as openness).
                 *
                 *  \req begin <= end (after substitution of optional values)
                 *
                 *  \par Example Usage
                 *      \code
                 *              Range<double> r1 (3, 5);
                 *              VerifyTestResult (r1.Contains (3) or not r1.Contains (3));  // depends on TRAITS openness
                 *              Range<double> r2 { 3, 5, Openness::eOpen, Openness::eOpen };
                 *              VerifyTestResult (not r2.Contains (3));
                 *      \endcode
                 */
                constexpr explicit Range ();
                template <typename T2, typename TRAITS2>
                constexpr explicit Range (const Range<T2, TRAITS>& src);
                constexpr explicit Range (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end);
                explicit Range (const optional<T>& begin, const optional<T>& end);
                constexpr explicit Range (Openness lhsOpen, Openness rhsOpen);
                constexpr explicit Range (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end, Openness lhsOpen, Openness rhsOpen);
                explicit Range (const optional<T>& begin, const optional<T>& end, Openness lhsOpen, Openness rhsOpen);

            public:
                /** 
                 */
                static Range<T, TRAITS> Circle (Configuration::ArgByValueType<T> center, Configuration::ArgByValueType<UnsignedDifferenceType> radius, Openness lhsOpen = TRAITS::kLowerBoundOpenness, Openness rhsOpen = TRAITS::kUpperBoundOpenness);

            public:
                /**
                 *  This returns begin>end? EMPTY else Range<T, TRAITS> (begin,  end);
                 *
                 *  The Range(begin/end) CTOR REQUIRES begin<=end). This does not, and just produces an empty range in that case.
                 */
                static constexpr Range<T, TRAITS> ContainedRange (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end);

            public:
/**
                 *  This returns Range<T, TRAITS> (
                 *              TraitsType::kLowerBound, TraitsType::kUpperBound,
                 *              TraitsType::kLowerBoundOpenness, TraitsType::kUpperBoundOpenness
                 *          );
                 */
#if qCompilerAndStdLib_constexpr_somtimes_cannot_combine_constexpr_with_constexpr_Buggy
                static Range<T, TRAITS> FullRange ();
#else
                static constexpr Range<T, TRAITS> FullRange ();
#endif

            public:
                /**
                 *  A Range is considered empty if it contains no points. If GetLowerBound () < GetUpperBound (),
                 *  then clearly this is non-empty. If created with Range/0() - then the range this is empty.
                 *
                 *  But if GetLowerBound () == GetUpperBound () - this is a trickier case. With both ends CLOSED -
                 *  that means the GetLowerBound () value is contained in the range, so that is not empty.
                 *
                 *  if GetLowerBound () == GetUpperBound (), and both ends open, then there are no points contained.
                 *
                 *  But if GetLowerBound () == GetUpperBound (), and one one side is open, and the other closed,
                 *  the one closed point endpoint is in the range, so the range is non-empty.
                 */
                nonvirtual constexpr bool empty () const;

            public:
                /**
                 *  GetUpperBound ()-GetLowerBound (), or distance from GetLowerBound () to end of the range.
                 *  If this is empty (), then GetDistanceSpanned () will be zero but the GetDistanceSpanned CAN be zero without the
                 *  range being empty (if both ends are closed).
                 *
                 *  \note   this just uses TraitsType::Difference ()
                 *
                 *  \note   For discrete Ranges, this does NOT correspond to the number of points (this is one less)
                 */
                nonvirtual constexpr UnsignedDifferenceType GetDistanceSpanned () const;

            public:
                /**
                 */
                nonvirtual constexpr T GetMidpoint () const;

            public:
                /**
                 *  Compare v with the upper and lower bounds of this range, and return a value as close as
                 *  possible to v but in range.
                 *
                 *  If 'v' is not in range, and this Range is open, GetNext or GetPrevious maybe used to find a value
                 *  in range.
                 *
                 *  \req not empty ()
                 *
                 *  @see Math::PinInRange ()
                 */
                nonvirtual T Pin (T v) const;

            public:
                /**
                 *  This corresponds to the mathematical set containment. When comparing with the edges
                 *  of the range, we check <= if the edge is closed, and < if the edge is open.
                 */
                nonvirtual constexpr bool Contains (Configuration::ArgByValueType<T> r) const;
                nonvirtual bool           Contains (const Range<T, TRAITS>& containee) const;

            public:
                /**
                 *  Returns an identical Range to this, but including its end points.
                 *
                 *  \req not empty ();
                 */
                nonvirtual constexpr Range<T, TRAITS> Closure () const;

#if 0
            public:
                /**
                 */
                nonvirtual  bool    Overlaps (const Range<T, TRAITS>& rhs) const;
#endif

            public:
                /**
                 *  If two ranges are both empty, they will both be equal.
                 */
                template <typename T2, typename TRAITS2>
                nonvirtual bool Equals (const Range<T2, TRAITS2>& rhs) const;

            public:
                /**
                 *  Returns true iff there are any points shared in common between this range and the rhs range.
                 */
                template <typename T2, typename TRAITS2>
                nonvirtual bool Intersects (const Range<T2, TRAITS2>& rhs) const;

            public:
                /**
                 *      Compute the overlap/intersection between two ranges. The resulting range defines exactly the points in both places (so respects openness).
                 *
                 *      @see operator^ - an operator alias for this function
                 */
                nonvirtual Range<T, TRAITS> Intersection (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 * if two regions are disjoint, this can encompass a larger region than the actual union would
                 */
                nonvirtual DisjointRange<T, Range<T, TRAITS>> Union (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 * if two regions are disjoint, this can encompass a larger region than the actual union would
                 */
                nonvirtual Range<T, TRAITS> UnionBounds (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual constexpr T GetLowerBound () const;

            public:
                /**
                 */
                nonvirtual constexpr Openness GetLowerBoundOpenness () const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual constexpr T GetUpperBound () const;

            public:
                /**
                 */
                nonvirtual constexpr Openness GetUpperBoundOpenness () const;

            public:
                /**
                 *  \req not empty
                 *  \req the Range produced by applying the given offset to *this remains valid with respect to the constraints on this Range.
                 */
                nonvirtual Range Offset (SignedDifferenceType o) const;

            public:
                /**
                 *  Print a displayable rendition of the given range, using the argument funciton to format
                 *  the basic value_type.
                 *
                 *  \par Example Usage
                 *      \code
                 *      Assert (Range<int> (3, 4).ToString () == L"[3 ... 4)");
                 *      Assert (Range<int> (3, 4).ToString ([] (int n) { return Characters::Format (L"%d", n); }) == L"[3 ... 4)");
                 *      \endcode
                 *
                 *  @see Characters::ToString ();
                 */
                nonvirtual Characters::String ToString (const function<Characters::String (T)>& elt2String = [](T x) -> Characters::String { return Characters::ToString (x); }) const;

            private:
                T        fBegin_;
                T        fEnd_;
                Openness fBeginOpenness_;
                Openness fEndOpenness_;
            };

            /**
             *  Alias forlhs.Union (rhs)
             */
            template <typename T, typename TRAITS>
            DisjointRange<T, Range<T, TRAITS>> operator+ (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs);

            /**
             *  Alias forlhs.Intersection (rhs)
             */
            template <typename T, typename TRAITS>
            Range<T, TRAITS> operator^ (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs);

            /**
             *      Syntactic sugar on Equals()
             */
            template <typename T, typename TRAITS>
            bool operator== (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs);

            /**
             *      Syntactic sugar on Equals()
             */
            template <typename T, typename TRAITS>
            bool operator!= (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs);
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Range.inl"

#endif /*_Stroika_Foundation_Traversal_Range_h_ */
