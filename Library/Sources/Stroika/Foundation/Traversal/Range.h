/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Range_h_
#define _Stroika_Foundation_Traversal_Range_h_  1

#include    "../StroikaPreComp.h"

#include    <limits>

#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"

#include    "Iterator.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
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
 *          @todo   Should openness be in TRAITS or instance? Having in traits casuse some quirks
 *                  on definition of intersection...
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
            enum    class   Openness { eOpen, eClosed };


            /**
             */
            namespace   RangeTraits {


                /**
                 *  ExplicitRangeTraitsWithoutMinMax<> can be used to specify in line line (type) all the details for the range functionality
                 *  for a given type T.
                 */
                template    <typename T, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
                struct  ExplicitRangeTraitsWithoutMinMax {
                    typedef T                   ElementType;
                    typedef SIGNED_DIFF_TYPE    SignedDifferenceType;
                    typedef UNSIGNED_DIFF_TYPE  UnsignedDifferenceType;

                    static  constexpr   Openness    kLowerBoundOpenness     =   LOWER_BOUND_OPEN;
                    static  constexpr   Openness    kUpperBoundOpenness     =   UPPER_BOUND_OPEN;
                };


                /**
                 *  Shorthand helper to generate a traits object with an explicit min/max. Due to limitations
                 *  in c++ (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf -
                 *  14.3.2 Template non-type arguments - about integral types).
                 */
                template    <typename T, T MIN, T MAX, Openness LOWER_BOUND_OPEN, Openness UPPER_BOUND_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
                struct  ExplicitRangeTraits_Integral : public ExplicitRangeTraitsWithoutMinMax<T, LOWER_BOUND_OPEN, UPPER_BOUND_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                    static  constexpr T kLowerBound    =   MIN;
                    static  constexpr T kUpperBound    =   MAX;
#else
                    static  const T kLowerBound;
                    static  const T kUpperBound;
#endif
                };


                namespace Private_ {
                    template    <typename T>
                    struct  UnsignedDifferenceType_ : conditional <
                    ((is_integral<T>::value || is_enum<T>::value)&&  !is_same<T, bool>::value),
                    typename make_unsigned < decltype (T () - T ()) >::type,
                    decltype (T () - T ())
                    >::type {
                    };
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
                ExplicitRangeTraitsWithoutMinMax < T, Openness::eClosed, Openness::eOpen, decltype (T () - T ()), Private_::UnsignedDifferenceType_<T> >
                >::type {
#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                    static  constexpr T kLowerBound    =   numeric_limits<T>::lowest ();
                    static  constexpr T kUpperBound    =   numeric_limits<T>::max ();
#else
                    static  const T kLowerBound;
                    static  const T kUpperBound;
#endif
                };


            }


            /**
             *  A Range<> is analagous to a mathematical range. It's left and and its right side can
             *  be optionally open or closed.
             *
             *  This Range<> template is similar to Ruby range -
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
             */
            template    <typename T, typename TRAITS = RangeTraits::DefaultRangeTraits<T>>
            class   Range {
            public:
                /**
                 */
                typedef T   ElementType;

            public:
                /**
                 */
                typedef TRAITS   TraitsType;

            public:
                /**
                 *  Range () creates an empty range.
                 *
                 *  Optional values - if omitted - are replaced with the TRAITS::kLowerBound and TRAITS::kUpperBound values.
                 *
                 *  \req begin <= end (after substitution of optional values)
                 */
                explicit Range ();
                explicit Range (const T& begin, const T& end);
                explicit Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end);

            public:
                /**
                 */
                static  Range<T, TRAITS> FullRange ();

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
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  GetUpperBound ()-GetLowerBound (), or distance from GetLowerBound () to end of the range.
                 *  If this is empty (), then GetDistanceSpanned () will be zero but the GetDistanceSpanned CAN be zero without the
                 *  range being empty (if both ends are closed).
                 */
                nonvirtual  typename TRAITS::UnsignedDifferenceType    GetDistanceSpanned () const;

            public:
                /**
                 *  UNCLEAR - THOUGH OUT DEF _ IF WE WANT TO CONTIN EDGES. ANSWER COULD BE DIFFERNT BETWEEN
                 *  DISCRETE versus NON-DISCRETE cases?
                 *
                 *  Numerically - what makes the most sense is to contain the edges - so assume yes.
                 */
                nonvirtual  bool    Contains (const T& r) const;

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
                nonvirtual  bool    Equals (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 */
                nonvirtual  Range<T, TRAITS> Intersection (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 * if two regions are disjoint, this can encompass a larger region than the actual union would
                 */
                nonvirtual  Range<T, TRAITS>    UnionBounds (const Range<T, TRAITS>& rhs) const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual  T    GetLowerBound () const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual  T    GetUpperBound () const;

            public:
                /**
                 */
                static  Openness    GetTraitsLowerBoundOpenness ();

            public:
                /**
                 */
                static  Openness    GetTraitsUpperBoundOpenness ();

            public:
                /**
                 */
                static  T    GetTraitsLowerBound ();

            public:
                /**
                 */
                static  T    GetTraitsUpperBound ();

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Range<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator!= (const Range<T, TRAITS>& rhs) const;

            private:
                T           fBegin_;
                T           fEnd_;
            };


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
