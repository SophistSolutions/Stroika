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
 *  STATUS:     VERY PRELIMINARY DRAFT - No WHERE NEAR COMPILING/TESTABLE.
 *
 *  NOTES:      INSPIRED BY READING ABOUT RUBY, but in the end, mostly
 *              based on HealthFrame's DateRangeType/DateTimeRangeType code.
 *
 *  TODO:
 *          @todo   Add operator< support (compare interface)
 *
 *          @todo   When this works with basic numbers, then add specializations to the Time module so that
 *                  this stuff all works with dates, and datetime etc.
 *
 *          @todo   One open question is should we treat all empty intersections as equal? I think yes. So then
 *                  Range(1,1) == Range(3,3)?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
            // See if some way todo TYPETRAITS - to see if IS ENUMERATION - and if so - use eSTART, eEND for min/max
            * AND wrong type - about singed differnce type =- maybe use declyetype
             */
            template    <typename T>
            struct  DefaultRangeTraits {
                //typedef ssize_t SignedDifferenceType;
                typedef size_t  UnsignedDifferenceType;

				//tmphack to compile on windoze - find better when when I decide what I'm looking for...
                //DEFINE_CONSTEXPR_CONSTANT(T, kMin, numeric_limits<T>::min ());
                //DEFINE_CONSTEXPR_CONSTANT(T, kMax, numeric_limits<T>::max ());
                DEFINE_CONSTEXPR_CONSTANT(T, kMin, -1);
                DEFINE_CONSTEXPR_CONSTANT(T, kMax, 1);
            };


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
            class  Range {
            public:
                /**
                 */
                typedef T   ElementType;

            public:
                /**
                 *  begin/end similar to Ruby range - except that end is always EXCLUDED (like C++ iterators - 
				 *	end refers to past the end).
				 *
				 *	Optional values - if omitted - are replaced with the TRAITS::kMin and TRAITS::kMax values.
				 *
				 *	\req begin <= end (after substitution of optional values)
                 */
                Range ();
                explicit Range (const Memory::Optional<T>& begin, const Memory::Optional<T>& end);

            public:
                /**
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
				 *	end-begin, or distance from begin to end of the range. This is zero iff empty.
                 */
                nonvirtual  typename TRAITS::UnsignedDifferenceType    size () const;

            public:
                /**
                 *  UNCLEAR - THOUGH OUT DEF _ IF WE WANT TO CONTIN EDGES. ANSWER COULD BE DIFFERNT BETWEEN
                 *  DISCRETE versus NON-DISCRETE cases?
                 *
                 *  Numerically - what makes the most sense is to contain the edges - so assume yes.
                 */
                nonvirtual  bool    Contains (const T& v) const;

            public:
                /**
                 */
                nonvirtual  bool    Overlaps (const Range<T, TRAITS>& v) const;

            public:
                /**
                 */
                nonvirtual  bool    Equals (const Range<T, TRAITS>& v) const;

            public:
                /**
                 */
                nonvirtual  Range<T, TRAITS> Intersection (const Range<T, TRAITS>& v) const;

            public:
                /**
                 * if two regions are disjoint, this can encompass a larger region than the actual union would
                 */
                nonvirtual  Range<T, TRAITS> ExpandedUnion (const Range<T, TRAITS>& v) const;

            public:
                /**
                 */
                nonvirtual  T    begin () const;

            public:
                /**
                 */
                nonvirtual  T    end () const;

            private:
                T       fBegin_;
                T       fEnd_;
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
