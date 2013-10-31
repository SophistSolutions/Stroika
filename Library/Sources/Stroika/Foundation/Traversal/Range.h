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
 *          @todo   Should openness be in TRIATS or instance? Having in traits casuse some quirks
 *                  on definition of intersection …
 *
 *          @todo   Consider possibly defining some default parameters for ExplicitRangeTraits<> - like
 *                  size/diff types - probably automatically computed from traits info.
 *
 *                  Or - at least - provide alternate template that has those two diff types autocomputed.
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
             *  Common defines for range-types.
             *
             *  \em Design Note
             *      We used base class instead of namespace so 'Openness' name can be injected directly into the used classes.
             *      There maybe a better way...
             */
            class   RangeBase {
            public:
                /**
                 *  Openness is used to define whether an end of a range is open or closed. Open means
                 *  not containing the endpoint, and closed means containing the endpoint.
                 */
                enum    class   Openness { eOpen, eClosed };
            };


            /**
             *  ExplicitRangeTraitsWithoutMinMax<> can be used to specify in line line (type) all the details for the range functionality
             *  for a given type T.
             */
            template    <typename T, RangeBase::Openness BEGIN_OPEN, RangeBase::Openness END_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            struct  ExplicitRangeTraitsWithoutMinMax : public RangeBase {
                typedef T                   ElementType;
                typedef SIGNED_DIFF_TYPE    SignedDifferenceType;
                typedef UNSIGNED_DIFF_TYPE  UnsignedDifferenceType;

                static  constexpr   Openness    kBeginOpenness  =   BEGIN_OPEN;
                static  constexpr   Openness    kEndOpenness    =   END_OPEN;
            };


            /**
             *  Shorthand helper to generate a traits object with an explicit min/max. Due to limitations
             *  in c++ (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf -
             *  14.3.2 Template non-type arguments - about integral types).
             */
            template    <typename T, T MIN, T MAX, RangeBase::Openness BEGIN_OPEN, RangeBase::Openness END_OPEN, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            struct  ExplicitRangeTraits_Integral : public ExplicitRangeTraitsWithoutMinMax<T, BEGIN_OPEN, END_OPEN, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE> {
#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                static  constexpr T kMin    =   MIN;
                static  constexpr T kMax    =   MAX;
#else
                static  const T kMin;
                static  const T kMax;
#endif
            };


            /**
             *  DefaultRangeTraits<> is generally used automatically - when you construct a Range<> object without
             *  specifying traits.
             *
             *  This defaults to a half-open (lhs closed, rhs-open) range.
             */
            template    <typename T>
            struct  DefaultRangeTraits : enable_if <
                    is_arithmetic<T>::value,
                    ExplicitRangeTraitsWithoutMinMax<T, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, int, unsigned int>
                    >::type {
#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                static  constexpr T kMin    =   numeric_limits<T>::lowest ();
                static  constexpr T kMax    =   numeric_limits<T>::max ();
#else
                static  const T kMin;
                static  const T kMax;
#endif
            };


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
            template    <typename T, typename TRAITS = DefaultRangeTraits<T>>
            class   Range : public RangeBase {
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
                 *  Range/0 creates an empty range.
                 *
                 *  Optional values - if omitted - are replaced with the TRAITS::kMin and TRAITS::kMax values.
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
                 *  A range is considered empty if it contains no points. If begin < end, then clearly this is
                 *  non-empty. If created with Range/0() - then the range this is empty.
                 *
                 *  But if begin == end - this is a trickier case. With both ends CLOSED - that means the begin value
                 *  is contained in the range, so that is not empty.
                 *
                 *  if begin == end, and both ends open, then there are no points contained.
                 *
                 *  But if begin == end, and one one side is open, and the other closed, the one closed point endpoint
                 *  is in the range, so the range is non-empty.
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  end-begin, or distance from begin to end of the range. If this is empty (), then size () will be zero
                 *  but the size CAN be zero without the range being empty (if both ends are closed).
                 */
                nonvirtual  typename TRAITS::UnsignedDifferenceType    size () const;

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
                nonvirtual  T    begin () const;

            public:
                /**
                 *  \req not empty ();
                 */
                nonvirtual  T    end () const;

            public:
                /**
                 */
                static  Openness    GetBeginOpenness ();

            public:
                /**
                 */
                static  Openness    GetEndOpenness ();

            public:
                /**
                 */
                static  T    GetBeginMin ();

            public:
                /**
                 */
                static  T    GetEndMax ();

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
