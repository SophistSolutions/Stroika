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
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  NOTES:      INSPIRED BY READING ABOUT RUBY, but in the end, mostly
 *              based on HealthFrame's DateRangeType/DateTimeRangeType code.
 *
 *  TODO:
 *
 *          @todo   we could allow begin > end as a trick to force empty? Not assert failure? else
 *                  closed/closed can ever be empty. So - wehther discrete range can ever be empty?
 *
 *          @todo   Range<T, TRAITS>::Overlaps and Range<T, TRAITS>::Intersection and Range<T, TRAITS>::UnionBounds
 *                  need fixing for new open/closed flag
 *
 *          @todo   Make Range<T> a smartptr interface (like with containers). Have one rep for HalfOpen and
 *                  another for fullyClosed (subtypes). And Discrte is subtype of FullyClosed.
 *                  I THINK that will go a long way towards addressing sterls (2013-07-05) email issues raised.
 *
 *                  RETHINK in light of new open/closed support
 *
 *          @todo   Add operator< support (compare interface)
 *
 *          @todo   When this works with basic numbers, then add specializations to the Time module so that
 *                  this stuff all works with dates, and datetime etc.
 *
 *          @todo   One open question is should we treat all empty intersections as equal? I think yes. So then
 *                  Range(1,1) == Range(3,3)?
 */



#if     !qSupportTemplateParamterOfNumericLimitsMinMax
#include    <climits>
#endif
namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
             *  Common defines for range-types.
             *
             *  \em Design Note
             *      We used base class instead of namespace so 'Openness' name can be injected directly into the used classes.
             *      There maybe a better way...
             */
            class   RangeBase {
            public:
                enum    class   Openness { eOpen, eClosed };
            };


            /**
             */
            template    <typename T, T MIN, T MAX , RangeBase::Openness beginOpen, RangeBase::Openness endOpen, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            struct  ExplicitRangeTraits : public RangeBase {
                typedef T                   ElementType;
                typedef SIGNED_DIFF_TYPE    SignedDifferenceType;
                typedef UNSIGNED_DIFF_TYPE  UnsignedDifferenceType;

                static  constexpr   Openness    kBeginOpenness  =   beginOpen;
                static  constexpr   Openness    kEndOpenness    =   endOpen;

#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                static  constexpr T kMin    =   MIN;
                static  constexpr T kMax    =   MAX;
#else
                static  const T kMin;
                static  const T kMax;
#endif
            };


            /**
             */
#if     qSupportTemplateParamterOfNumericLimitsMinMax
            template    <typename T>
            struct  DefaultRangeTraits  : ExplicitRangeTraits<T, numeric_limits<T>::min (), numeric_limits<T>::max (), RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, int, unsigned int> {
            };
#else
            template    <typename T>
            struct  DefaultRangeTraits  : ExplicitRangeTraits<T, 0, 1, RangeBase::Openness::eClosed, RangeBase::Openness::eOpen, int, unsigned int> {
#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                static  const T kMin    =   numeric_limits<T>::min ();
                static  const T kMax    =   numeric_limits<T>::max ();
#else
                static  const T kMin;
                static  const T kMax;
#endif
            };
#endif


            /**
             *  Somewhat inspired by, and at least influenced by, the definition in
             *      http://ruby-doc.org/core-2.0/Range.html
             *  However - where Ruby has one type "Range" - we have "Range" and DiscreteRange" - and some ruby Range methods/properties
             *  are expressed only in DiscreteRange<>
             *
             *  Note - you can do Range<float>, but cannot do DiscreteRange<float> - but can do DiscreteRange<int>.
             *
             *  KEY NOTE!!!
             *      HALF OPEN INTERVAL!!!! - LHS is INCLUDED. RHS (END) is NOT INCLUDED!!!!
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
                 *  begin/end similar to Ruby range - except that end is always EXCLUDED (like C++ iterators -
                 *  end refers to past the end).
                 *
                 *  Optional values - if omitted - are replaced with the TRAITS::kMin and TRAITS::kMax values.
                 *
                 *  \req begin <= end (after substitution of optional values)
                 */
                explicit Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end);

            public:
                /**
                 */
                static  Range<T, TRAITS> EmptyRange ();

            public:
                /**
                 */
                static  Range<T, TRAITS> FullRange ();

            public:
                /**
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  end-begin, or distance from begin to end of the range. This is zero iff empty.
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
                 */
                nonvirtual  T    begin () const;

            public:
                /**
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
