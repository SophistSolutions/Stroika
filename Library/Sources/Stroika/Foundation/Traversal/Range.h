/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Range_h_
#define _Stroika_Foundation_Traversal_Range_h_  1

#include    "../StroikaPreComp.h"

#include    <limits>

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"

#include    "Iterator.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
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
 *          @todo   Correct the implementation of Range<T, TRAITS>::Contains (const Range<T, TRAITS>& containee)
 *                  for openness. And if possible, make it constexpr.
 *
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



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             *  Openness is used to define whether an end of a range is open or closed. Open means
             *  not containing the endpoint, and closed means containing the endpoint.
             */
            enum    class   Openness {
                eOpen,
                eClosed,

                Stroika_Define_Enum_Bounds(eOpen, eClosed)
            };
            constexpr   Configuration::EnumNames<Openness>    Stroika_Enum_Names(Openness)
            {
                Configuration::EnumNames<Openness>::BasicArrayInitializer {
                    {
                        { Openness::eOpen, L"Open" },
                        { Openness::eClosed, L"Closed" },
                    }
                }
            };


            /**
             */
            namespace   RangeTraits {


                /**
                 *  ExplicitRangeTraitsWithoutMinMax<> can be used to specify in line line (type) all the details for the range functionality
                 *  for a given type T.
                 */
                template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
                struct  ExplicitRangeTraitsWithoutMinMax {
                    using   ElementType             =   T;
                    using   SignedDifferenceType    =   SIGNED_DIFF_TYPE;
                    using   UnsignedDifferenceType  =   UNSIGNED_DIFF_TYPE;

                    static  constexpr   Openness    kLowerBoundOpenness     =   LOWER_BOUND_OPEN;
                    static  constexpr   Openness    kUpperBoundOpenness     =   UPPER_BOUND_OPEN;

                    /**
                     *  Return the Next possible value, or if already at the end of the range, the same value.
                     */
                    template    <typename SFINAE = ElementType>
                    static  ElementType GetNext (ElementType i, typename enable_if <std::is_integral<SFINAE>::value>::type* = 0);
                    template    <typename SFINAE = ElementType>
                    static  ElementType GetNext (ElementType i, typename enable_if <std::is_floating_point<SFINAE>::value>::type* = 0);

                    /**
                     *  Return the Previous possible value, or if already at the end of the range, the same value.
                     */
                    template    <typename SFINAE = ElementType>
                    static  ElementType GetPrevious (ElementType i, typename enable_if <std::is_integral<SFINAE>::value>::type* = 0);
                    template    <typename SFINAE = ElementType>
                    static  ElementType GetPrevious (ElementType i, typename enable_if <std::is_floating_point<SFINAE>::value>::type* = 0);

                    /**
                     *  Format as a string the given ElementType. This method is not required, and may not compile (SFINAE), but
                     *  often provides a convenient default argument to the Range<>::Format () function.
                     */
                    template    <typename SFINAE = ElementType>
                    static  Characters::String  Format (ElementType v, typename enable_if <is_integral<SFINAE>::value>::type* = 0);
                    template    <typename SFINAE = ElementType>
                    static  Characters::String  Format (ElementType v, typename enable_if <is_floating_point<SFINAE>::value>::type* = 0);
                    template    <typename SFINAE = ElementType>
                    static  Characters::String  Format (ElementType v, typename enable_if < !is_integral<SFINAE>::value and !is_floating_point<SFINAE>::value >::type* = 0);
                };


                /**
                 *  Shorthand helper to generate a traits object with an explicit min/max. Due to limitations
                 *  in c++ (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf -
                 *  14.3.2 Template non-type arguments - about integral types).
                 */
                template    <typename T, T MIN, T MAX, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
                struct  ExplicitRangeTraits_Integral : public ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
#if     qCompilerAndStdLib_constexpr_Buggy
                    static  const T kLowerBound;
                    static  const T kUpperBound;
#else
                    static  constexpr T kLowerBound    =   MIN;
                    static  constexpr T kUpperBound    =   MAX;
#endif
                };


                namespace Private_ {
                    template    <typename T>
                    using  BaseDifferenceType_ =   decltype (T {} - T {});
                    // @todo - cannot get to compile using make_unsigned or conditional<>
#if 0
                    template    <typename T>
                    using  UnsignedDifferenceType_ = typename conditional <
                                                     ((is_integral<T>::value || is_enum<T>::value) && !is_same<T, bool>::value),
                                                     typename make_unsigned < BaseDifferenceType_<T> >::type,
                                                     BaseDifferenceType_<T>
                                                     >::type;
#elif 0
                    template    <typename T>
                    using  UnsignedDifferenceType_ =   typename make_unsigned < BaseDifferenceType_<T> >::type;

#else
                    template    <typename T>
                    using  UnsignedDifferenceType_ =   BaseDifferenceType_<T>;
#endif
                }


                /**
                 *  DefaultRangeTraits<> is generally used automatically - when you construct a Range<> object without
                 *  specifying traits.
                 *
                 *  This defaults to a half-open (lhs closed, rhs-open) range.
                 */
                template    <typename T>
                struct  DefaultRangeTraits : enable_if <
                        is_arithmetic<T>::value,
                ExplicitRangeTraitsWithoutMinMax < T, Openness::eClosed, Openness::eOpen, decltype (T {} - T {}), Private_::UnsignedDifferenceType_<T> >
                >::type {
#if     qCompilerAndStdLib_constexpr_Buggy
                    static  const T kLowerBound;
                    static  const T kUpperBound;
#else
                    static  constexpr T kLowerBound    =   numeric_limits<T>::lowest ();
                    static  constexpr T kUpperBound    =   numeric_limits<T>::max ();
#endif
                };


            }


            template    <typename T, typename RANGE_TYPE>
            class   DisjointRange;


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
            template    <typename T, typename TRAITS = RangeTraits::DefaultRangeTraits<T>>
            class   Range {
            public:
                /**
                 */
                using   ElementType     =   T;

            public:
                /**
                 */
                using   TraitsType      =   TRAITS;

            public:
                /**
                 */
                using   UnsignedDifferenceType      =   typename TraitsType::UnsignedDifferenceType;

            public:
                /**
                 *  Range () creates an empty range.
                 *
                 *  Optional values - if omitted - are replaced with the TRAITS::kLowerBound and TRAITS::kUpperBound values.
                 *
                 *  \req begin <= end (after substitution of optional values)
                 */
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                explicit Range ();
                template    <typename T2, typename TRAITS2>
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                explicit Range (const Range<T2, TRAITS>& src);
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                explicit Range (const T& begin, const T& end);
                explicit Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end);
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                explicit Range (Openness lhsOpen, Openness rhsOpen);
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                explicit Range (const T& begin, const T& end, Openness lhsOpen, Openness rhsOpen);
                explicit Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end, Openness lhsOpen, Openness rhsOpen);

            public:
                /**
                 *  This returns Range<T, TRAITS> (
                 *              TraitsType::kLowerBound, TraitsType::kUpperBound,
                 *              TraitsType::kLowerBoundOpenness, TraitsType::kUpperBoundOpenness
                 *          );
                 */
                static  constexpr   Range<T, TRAITS> FullRange ();

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
                nonvirtual  constexpr   bool    empty () const;

            public:
                /**
                 *  GetUpperBound ()-GetLowerBound (), or distance from GetLowerBound () to end of the range.
                 *  If this is empty (), then GetDistanceSpanned () will be zero but the GetDistanceSpanned CAN be zero without the
                 *  range being empty (if both ends are closed).
                 */
                nonvirtual  constexpr   UnsignedDifferenceType    GetDistanceSpanned () const;

            public:
                /**
                 *  EXPERIMENTAL v2.0a21
                 */
                nonvirtual  constexpr   T    GetMidpoint () const;

            public:
                /**
                 *  This corresponds to the mathematical set containment. When comparing with the edges
                 *  of the range, we check <= if the edge is closed, and < if the edge is open.
                 */
                nonvirtual  constexpr   bool    Contains (const T& r) const;
                nonvirtual  bool                Contains (const Range<T, TRAITS>& containee) const;

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
                template    <typename T2, typename TRAITS2>
                nonvirtual  bool    Equals (const Range<T2, TRAITS2>& rhs) const;

            public:
                /**
                 *  Returns true iff there are any points shared in common between this range and the rhs range.
                 */
                template    <typename T2, typename TRAITS2>
                nonvirtual  bool    Intersects (const Range<T2, TRAITS2>& rhs) const;

            public:
                /**
                 */
                nonvirtual  Range<T, TRAITS>    Intersection (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 * if two regions are disjoint, this can encompass a larger region than the actual union would
                 */
                nonvirtual  DisjointRange<T, Range<T, TRAITS>> Union (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 * if two regions are disjoint, this can encompass a larger region than the actual union would
                 */
                nonvirtual  Range<T, TRAITS>    UnionBounds (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual  constexpr   T    GetLowerBound () const;

            public:
                /**
                 */
                nonvirtual  constexpr   Openness    GetLowerBoundOpenness () const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual  constexpr   T    GetUpperBound () const;

            public:
                /**
                 */
                nonvirtual  constexpr   Openness    GetUpperBoundOpenness () const;

            public:
                static Characters::String a (T x) { return TraitsType::Format (x); }
                /**
                 *  Print a displayable rendition of the given range, using the argument funciton to format
                 *  the basic ElementType.
                 *
                 *  EXAMPLE:
                 *      Assert (Range<int> (3, 4).Format ([] (int n) { return Characters::Format (L"%d", n); }) == L"[3 ... 4)");
                 *
                 *      @todo add default ElementType format function to TRAITS, and then make this std::function default to that helper.
                 */
                nonvirtual  Characters::String  Format (const function<Characters::String(T)>& formatBound = a /*TraitsType::Format*/) const;

            private:
                T           fBegin_;
                T           fEnd_;
                Openness    fBeginOpenness_;
                Openness    fEndOpenness_;
            };


            /**
             *  Union ()
             */
            template    <typename T, typename TRAITS>
            DisjointRange<T, Range<T, TRAITS>>   operator+ (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs);


            /**
             *  Intersection ()
             */
            template    <typename T, typename TRAITS>
            Range<T, TRAITS>   operator^ (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs);


            /**
             *      Syntactic sugar on Equals()
             */
            template    <typename T, typename TRAITS>
            bool    operator== (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs);


            /**
             *      Syntactic sugar on Equals()
             */
            template    <typename T, typename TRAITS>
            bool    operator!= (const Range<T, TRAITS>& lhs, const Range<T, TRAITS>& rhs);


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Range.inl"

#endif  /*_Stroika_Foundation_Traversal_Range_h_ */
