/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Range_h_
#define _Stroika_Foundation_Traversal_Range_h_ 1

#include "../StroikaPreComp.h"

#include <limits>
#include <optional>
#include <type_traits>

#include "../Characters/String.h"
#include "../Characters/ToString.h"
#include "../Common/TemplateUtilities.h"
#include "../Configuration/Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  \em Design Note
 *      This module was inspired by Ruby Range class, but in the end, it was mostly based on HealthFrame's
 *      DateRangeType/DateTimeRangeType code.
 *
 *  TODO:
 *          @todo   Better integrate with https://stroika.atlassian.net/browse/STK-779 - C++20 ranges library
 * 
 *          @todo   Carefully review intersection/unionbounds code for new open/closed parameters. Either make sure
 *                  it works or at least more carefully document in method headers the quirks of the
 *                  chosen definition.
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  Openness is used to define whether an end of a range is open or closed. Open means
     *  not containing the endpoint, and closed means containing the endpoint.
     * 
     *  \note   no support for the concepts of open and closed or neither open nor closed
     *
     *  \note   Configuration::DefaultNames<> supported
     */
    enum class Openness {
        eOpen,
        eClosed,

        Stroika_Define_Enum_Bounds (eOpen, eClosed)
    };

    /**
     *  \note - ALL these RangeTraits helper classes use template <> struct instead of template<> using because
     *          as of C++17, you cannot do template specialization of using templates (otherwise using would be better)
     */
    namespace RangeTraits {

        /**
         *  \note really just used to construct Explicit<> or ExplicitOpennessAndDifferenceType<>
         */
        template <typename DIFFERENCE_TYPE, typename UNSIGNED_DIFFERENCE_TYPE = Common::UnsignedOfIf<DIFFERENCE_TYPE>>
        struct ExplicitDifferenceTypes {
            using SignedDifferenceType   = DIFFERENCE_TYPE;
            using UnsignedDifferenceType = UNSIGNED_DIFFERENCE_TYPE;
        };

        /**
         *  \note really just used to construct Explicit<> or ExplicitOpennessAndDifferenceType<>
         */
        template <typename T>
        struct DefaultDifferenceTypes : ExplicitDifferenceTypes<Common::DifferenceType<T>> {
        };

        /**
         *  \note really just used to construct Explicit<> or ExplicitOpennessAndDifferenceType<>
         */
        template <Openness LOWER_BOUND, Openness UPPER_BOUND>
        struct ExplicitOpenness {
            static constexpr Openness kLowerBound{LOWER_BOUND};
            static constexpr Openness kUpperBound{UPPER_BOUND};
        };

        /**
         *  \brief This defines the default openness for a given type T, except for specializaitons. This is used
         *         by Explict<>, and indirectly by Range<> itself.
         *         Generally its eClosed, by default, except for the upper bound on floating point Ranges.
         *         The reason for this exception on floating point, is that its often helpful to have a series of
         *         ranges that form a partition, and that works out more easily with half-open intervals.
         *
         *  \note really just used to construct Explicit<> or ExplicitOpennessAndDifferenceType<>
         */
        template <typename T>
        struct DefaultOpenness : conditional_t<
                                     is_floating_point_v<T>,
                                     ExplicitOpenness<Openness::eClosed, Openness::eOpen>,
                                     ExplicitOpenness<Openness::eClosed, Openness::eClosed>> {
        };

        /**
         *  \note really just used to construct Explicit<>
         *
         * Sadly this doesn't work for floating point types, so you must declare your own class with kLowerBounds and kUpperBounds, and pass its
         * type.
         * 
         *      https://stackoverflow.com/questions/2183087/why-cant-i-use-float-value-as-a-template-parameter
         * 
         *  More confusingly, this limitation, though part of the C++ standard, only appears to be enforced by gcc/clang compilers, and
         *  not MSVC (as of 2020-12-11)
         */
        template <typename T, T MIN, T MAX>
        struct ExplicitBounds {
            static constexpr T kLowerBound{MIN};
            static constexpr T kUpperBound{MAX};
        };

        /**
         *  \note really just used to construct Explicit<>
         *
         * \note implementation of DefaultBounds<> cannot use ExplicitBounds<> because that wont work with floating point types
         */
        template <typename T>
        struct DefaultBounds {
            static constexpr T kLowerBound{numeric_limits<T>::lowest ()};
            static constexpr T kUpperBound{numeric_limits<T>::max ()};
        };

        /**
         *  The ONLY reason this exists (as opposed to just Explicit<> is because we cannot create templates taking arguments
         *  of BOUNDS sometimes (because the VALUES MIN/MAX cannot be used as template parameters).
         */
        template <
            typename T,
            typename OPENNESS  = DefaultOpenness<T>,
            typename DIFF_TYPE = DefaultDifferenceTypes<T>>
        struct ExplicitOpennessAndDifferenceType {
            using value_type             = T;
            using SignedDifferenceType   = typename DIFF_TYPE::SignedDifferenceType;
            using UnsignedDifferenceType = typename DIFF_TYPE::UnsignedDifferenceType;

            static constexpr Openness kLowerBoundOpenness{OPENNESS::kLowerBound};
            static constexpr Openness kUpperBoundOpenness{OPENNESS::kUpperBound};

            /**
             *  Compute the difference between two elements of type T for the Range (RHS - LHS)
             */
            template <typename TYPE2CHECK = value_type, typename SFINAE_CAN_CONVERT_TYPE_TO_SIGNEDDIFFTYPE = enable_if_t<is_enum_v<TYPE2CHECK> or is_convertible_v<TYPE2CHECK, SignedDifferenceType>>>
            static constexpr SignedDifferenceType Difference (Configuration::ArgByValueType<value_type> lhs, Configuration::ArgByValueType<value_type> rhs, SFINAE_CAN_CONVERT_TYPE_TO_SIGNEDDIFFTYPE* = nullptr);
            template <typename TYPE2CHECK = value_type, typename SFINAE_CANNOT_CONVERT_TYPE_TO_SIGNEDDIFFTYPE = enable_if_t<not(is_enum_v<TYPE2CHECK> or is_convertible_v<TYPE2CHECK, SignedDifferenceType>)>>
            static constexpr SignedDifferenceType Difference (Configuration::ArgByValueType<value_type> lhs, Configuration::ArgByValueType<value_type> rhs, ...);
        };

        /**
         *  \note Only used to construct/define a specific Range<> type
         *
         *  Explicit<> can be used to specify inline (type) all the details for the range functionality
         *  for a given type T. Also, it provides often usable default implementations of things like GetNext, GetPrevious ().
         */
        template <
            typename T,
            typename OPENNESS  = DefaultOpenness<T>,
            typename BOUNDS    = DefaultBounds<T>,
            typename DIFF_TYPE = DefaultDifferenceTypes<T>>
        struct Explicit : ExplicitOpennessAndDifferenceType<T, OPENNESS, DIFF_TYPE> {
            using inherited  = ExplicitOpennessAndDifferenceType<T, OPENNESS, DIFF_TYPE>;
            using value_type = T;
            //using value_type           = typename inherited::value_type;  // @todo debug why this doesn't work!
            using SignedDifferenceType   = typename inherited::SignedDifferenceType;
            using UnsignedDifferenceType = typename inherited::UnsignedDifferenceType;

            static constexpr T kLowerBound{BOUNDS::kLowerBound};
            static constexpr T kUpperBound{BOUNDS::kUpperBound};

            /**
             *  Return the Next possible value.
             * 
             *  \req arg != last-possible-value
             *
             *  \note its hard todo GetNext() for floating point as constexpr because underlying function in cmath not yet constexpr (as of C++17)
             */
            template <typename SFINAE = value_type>
            static constexpr value_type GetNext (value_type i, enable_if_t<is_integral_v<SFINAE> or is_enum_v<SFINAE>>* = nullptr);
            template <typename SFINAE = value_type>
            static value_type GetNext (value_type i, enable_if_t<is_floating_point_v<SFINAE>>* = nullptr);

            /**
             *  Return the Previous possible value.
             * 
             *  \req arg != first-possible-value
             *
             *  \note its hard todo GetPrevious() for floating point as constexpr because underlying function in cmath not yet constexpr (as of C++17)
             */
            template <typename SFINAE = value_type>
            static constexpr value_type GetPrevious (value_type i, enable_if_t<is_integral_v<SFINAE> or is_enum_v<SFINAE>>* = nullptr);
            template <typename SFINAE = value_type>
            static value_type GetPrevious (value_type i, enable_if_t<is_floating_point_v<SFINAE>>* = nullptr);
        };

        /**
         *  \note Only used to construct/define a specific Range<> type
         */
        template <typename T>
        struct Default_Integral : Explicit<T, ExplicitOpenness<Openness::eClosed, Openness::eClosed>> {
        };

        /**
         *  \note Only used to construct/define a specific Range<> type
         *
         *  Default_Enum<> can be used to generate an automatic traits object (with bounds)
         *  if you've applied the Stroika_Define_Enum_Bounds() macro to the given enumeration.
         */
        template <typename T>
        struct Default_Enum : Explicit<T, ExplicitOpenness<Openness::eClosed, Openness::eClosed>, ExplicitBounds<T, T::eSTART, T::eLAST>> {
        };

        /**
         *  \note Only used to construct/define a specific Range<> type
         *
         *  This defaults to a half-open (lhs closed, rhs-open) range, and should work for any arithmetic type
         *  (where you can subtract elements, etc)
         */
        template <typename T>
        struct Default_Arithmetic : Explicit<T, ExplicitOpenness<Openness::eClosed, Openness::eOpen>> {
        };

        /**
         *  Default<> contains the default traits used by a Range<> class. For most builtin types, this will
         *  be fine. For many Stroika types, specializations exist, so that you can just use Range<T> directly.
         * 
         *  But you may find it handy to define your own Range 'traits' object.
         *
         * \note The default OPENNESS for Default varies by TYPE T. Integer and enums are both
         *       fully closed by default, and other arithmetic types (floats) are half open [)
         *
         * \note Would be nice to use using syntax and not introduce a new type, but apparently
         *       using declarations cannot be specailized in C++17 (@todo add reference)
         */
        template <typename T>
        struct Default : conditional_t<
                             is_enum_v<T>, typename Common::LazyType<Default_Enum, T>::type,
                             conditional_t<
                                 is_integral_v<T>, typename Common::LazyType<Default_Integral, T>::type,
                                 conditional_t<
                                     is_arithmetic_v<T>, typename Common::LazyType<Default_Arithmetic, T>::type,
                                     void>>> {
        };

    }

    template <typename T, typename RANGE_TYPE>
    class DisjointRange;

    /**
     *  A Range<> is analagous to a mathematical range. It's left and and its right sides can
     *  be optionally open or closed.
     * 
     *  A range always has a lower and upper bound (if not specified in CTOR, its specified by the type traits) so no
     *  unbounded ranges).
     * 
     *  For a range to contain a single point, min=max, and both sides must be closed (else its a require error)
     *
     *  This Range<> template is similar to Ruby range, and fairly DIFFERENT from the std::range<> template.
     *
     *  This notion of range is **NOT THE SAME as std::range**, though is similar (obviously from the name) and
     *  @todo should consider better integration!
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
     *  It is illegal to call:
     *      Range<int>{1,1, Openness::eOpen, Openness::eOpen} since this would produce an empty range.
     *
     *      Range<int>{1,1, Openness::eClosed, Openness::eClosed} != Range<int>{3,3, Openness::eClosed, Openness::eClosed} 
     *  would be true, since neither is empty and they contain different points (1 vs 3).
     *
     * \note The default OPENNESS for Default varies by TYPE T. Integer and enums are both
     *       fully closed by default, and other arithmetic types (floats) are half open [)
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *      o   Depends on operator== being defined on T
     *      
     *      o   no operator<, operator<=> support (compare interface) - cuz no obvious well-ordering?
     *          Could well order on LHS, and then when equal on RHS, but that wouldn't make sense for
     *          all applicaitons.
     *      
     *  @see DiscreteRange
     *  @see DisjointRange
     *  @see DisjointDiscreteRange
     */
    template <typename T, typename TRAITS = RangeTraits::Default<T>>
    class Range {
    public:
        /**
         *  \brief Range::value_type is the type of the contained elements of the range (say range of integers, value_type=int)
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
         *  Range{} creates an empty range (note all empty () ranges of the same type are equal to each other).
         *
         *  optional values - if omitted - are replaced with the TRAITS::kLowerBound and TRAITS::kUpperBound values (as well as 'TRAITs' default openness).
         *  Constructors with actual numeric values (begin/end) MUST construct non-empty ranges (begin == end ==> both sides closed).
         *
         *  \req begin <= end (after substitution of optional values)
         *  \req begin < end or LHS/RHS CLOSED (after substitution of optional values)
         *
         *  \par Example Usage
         *      \code
         *          Range<double> r1{3, 5};
         *          Assert (r1.Contains (3) and not r1.Contains (3));  // because default arithmetic traits have [) half open
         *          Range<double> r2{ 3, 5, Openness::eOpen, Openness::eOpen };
         *          Assert (not r2.Contains (3));
         *      \endcode
         */
        constexpr explicit Range ();
        template <typename T2, typename TRAITS2>
        constexpr explicit Range (const Range<T2, TRAITS>& src);
        constexpr explicit Range (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end);
        constexpr explicit Range (const optional<T>& begin, const optional<T>& end);
        constexpr explicit Range (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end, Openness lhsOpen, Openness rhsOpen);
        constexpr explicit Range (const optional<T>& begin, const optional<T>& end, Openness lhsOpen, Openness rhsOpen);

    public:
        /**
         *  \brief returns a range centered around center, with the given radius (and optionally argument openness).
         */
        static constexpr Range Ball (Configuration::ArgByValueType<T> center, Configuration::ArgByValueType<UnsignedDifferenceType> radius, Openness lhsOpen = TRAITS::kLowerBoundOpenness, Openness rhsOpen = TRAITS::kUpperBoundOpenness);

    public:
        /**
         *  This returns begin>end? EMPTY else Range<T, TRAITS> (begin,  end);
         *
         *  The Range(begin/end) CTOR REQUIRES begin<=end). This does not, and just produces an empty range in that case.
         */
        static constexpr Range ContainedRange (Configuration::ArgByValueType<T> begin, Configuration::ArgByValueType<T> end);

    public:
        /**
         *  This returns Range (
         *              TraitsType::kLowerBound, TraitsType::kUpperBound,
         *              TraitsType::kLowerBoundOpenness, TraitsType::kUpperBoundOpenness
         *          );
         */
        static constexpr Range FullRange ();

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
        constexpr bool empty () const;

    public:
        /**
         *  \brief equivilent to not this->empty ();
         */
        constexpr explicit operator bool () const;

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
        constexpr UnsignedDifferenceType GetDistanceSpanned () const;

    public:
        /**
         *  \req not empty ()
         */
        constexpr T GetMidpoint () const;

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
        constexpr T Pin (T v) const;

    public:
        /**
         *  This corresponds to the mathematical set containment. When comparing with the edges
         *  of the range, we check <= if the edge is closed, and < if the edge is open.
         */
        constexpr bool Contains (Configuration::ArgByValueType<T> r) const;
        constexpr bool Contains (const Range& containee) const;

    public:
        /**
         *  Returns an identical Range to this, but including its end points.
         *
         *  \req not empty ();
         */
        nonvirtual constexpr Range Closure () const;

    public:
        /**
         *  \note All empty ranges (of the same type) are equal to each other.
         */
        constexpr bool operator== (const Range& rhs) const;

    public:
        /**
         *  Returns true iff there are any points shared in common between this range and the rhs range.
         * 
         *  \par Example Usage
         *      \code
         *          using RT = Range<int>;
         *          constexpr auto eOpen = Openness::eOpen;
         *          constexpr auto eClosed = Openness::eClosed;
         *          Assert ((RT{1, 2}.Intersects (RT{1, 2})));
         *          Assert ((not RT{1, 2, eOpen, eOpen}.Intersects (RT{2, 3, eOpen, eOpen})));
         *          Assert ((not RT{1, 2, eOpen, eClosed}.Intersects (RT{2, 3, eOpen, eOpen})));
         *          Assert ((RT{1, 2, eOpen, eClosed}.Intersects (RT{2, 3, eClosed, eOpen})));
         *      \endcode
         * 
         *  \see operator^
         */
        template <typename T2, typename TRAITS2>
        constexpr bool Intersects (const Range<T2, TRAITS2>& rhs) const;

    public:
        /**
         *      Compute the overlap/intersection between two ranges. The resulting range defines exactly the points in both places (so respects openness).
         *
         *      @see operator^ - an operator alias for this function
         */
        constexpr Range Intersection (const Range& rhs) const;

    public:
        /**
         * if two regions are disjoint, this can encompass a larger region than the actual union would
         */
        nonvirtual DisjointRange<T, Range> Union (const Range& rhs) const;

    public:
        /**
         * if two regions are disjoint, this can encompass a larger region than the actual union would
         */
        constexpr Range UnionBounds (const Range& rhs) const;

    public:
        /**
         *  \req not empty ();
         */
        constexpr T GetLowerBound () const;

    public:
        /**
         */
        constexpr Openness GetLowerBoundOpenness () const;

    public:
        /**
         *  \req not empty ();
         */
        constexpr T GetUpperBound () const;

    public:
        /**
         */
        constexpr Openness GetUpperBoundOpenness () const;

    public:
        /**
         *  \req not empty
         *  \req the Range produced by applying the given offset to *this remains valid with respect to the constraints on this Range.
         */
        constexpr Range Offset (SignedDifferenceType o) const;

    public:
        /**
         *  \req not empty
         *  \req the Range produced by applying the given factor to the upper and lower bounds.
         *  \req T has operator* (T,T) -> T defined
         */
        constexpr Range Times (T o) const;

    public:
        /**
         *  Print a displayable rendition of the given range, using the argument function to format
         *  the basic value_type.
         *
         *  \par Example Usage
         *      \code
         *          Assert (Range<int>{3, 4}.ToString () == L"[3 ... 4)");
         *          Assert (Range<int>{3, 4}.ToString ([] (int n) { return Characters::Format (L"%d", n); }) == L"[3 ... 4)");
         *      \endcode
         *
         *  @see Characters::ToString ();
         */
        nonvirtual Characters::String ToString (const function<Characters::String (T)>& elt2String = [] (T x) -> Characters::String { return Characters::ToString (x); }) const;

    private:
        T        fBegin_;
        T        fEnd_;
        Openness fBeginOpenness_;
        Openness fEndOpenness_;
    };

    /**
     *  Alias: T + RANGE => RANGE.Offset(T)
     *  Alias: RANGE + RANGE => RANGE.Union (RANGE)
     */
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> operator+ (const T& lhs, const Range<T, TRAITS>& rhs);
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> operator+ (const Range<T, TRAITS>& lhs, const T& rhs);
    template <typename T, typename TRAITS>
    DisjointRange<T, Range<T, TRAITS>> operator+ (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs);

    /**
     *  Alias: T * RANGE => RANGE.Times(T)
     *  \req T has operator* (T,T) -> T defined
     */
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> operator* (const T& lhs, const Range<T, TRAITS>& rhs);
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> operator* (const Range<T, TRAITS>& lhs, const T& rhs);

    /**
     *  Alias for lhs.Intersection (rhs)
     * 
     *  \par Example Usage
     *      \code
     *          using RT = Range<int>;
     *          constexpr auto eOpen = Openness::eOpen;
     *          constexpr auto eClosed = Openness::eClosed;
     *          Assert ((RT{1, 2, eOpen, eClosed} ^ RT{2, 3, eClosed, eOpen}));
     *          Assert (((RT{1, 2, eOpen, eClosed} ^ RT{2, 3, eClosed, eOpen}) == RT{2,2,eClosed,eClosed}));
     *      \endcode
     */
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> operator^ (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Range.inl"

#endif /*_Stroika_Foundation_Traversal_Range_h_ */
