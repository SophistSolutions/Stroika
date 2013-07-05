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
 *          @todo   Need a way to represent EMPTY RANGE (has no min/max).
 *                  This is critical to properly implementing iNTERSECTION
 *
 *          @todo   When this works with basic numbers, then add specializations to the Time module so that
 *                  this stuff all works with dates, and datetime etc.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            // See if some way todo TYPETRAITS - to see if IS ENUMERATION - and if so - use eSTART, eEND for min/max
            template    <typename T>
            struct  DefaultRangeTraits {
                typedef ssize_t SignedDifferenceType;
                typedef size_t  UnsignedDifferenceType;

                DEFINE_CONSTEXPR_CONSTANT(SignedDifferenceType, kMin, numeric_limits<T>::min ());
                DEFINE_CONSTEXPR_CONSTANT(SignedDifferenceType, kMax, numeric_limits<T>::max ());
            };


            /**
             */
            template    <typename T, typename TRAITS = DefaultRangeTraits<T>>
            class  Range {
            public:
                /**
                 */
                typedef T   ElementType;

            public:
                explicit Range (const Memory::Optional<T>& min, const Memory::Optional<T>& max);


            public:
                /**
                 *  UNCLEAR - THOUGH OUT DEF _ IF WE WANT TO CONTIN EDGES. ANSWER COULD BE DIFFERNT BETWEEN
                 *  DISCRETE versus NON-DISCRETE cases?
                 *
                 *  Numerically - what makes the most sense is to contain the edges - so assume yes.
                 */
                nonvirtual  bool    Contains (T v) const;

            public:
                /**
                 */
                nonvirtual  bool    Overlaps (const Range<T, TRAITS>& v) const;

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
                nonvirtual  Memory::Optional<T>    GetMin () const;

            public:
                /**
                 */
                nonvirtual  Memory::Optional<T>    GetMax () const;

            public:
                /**
                 *  The min-value is optional - and can be omitted. If omitted, treat as the
                 *  TRAITS::kMin
                 */
                nonvirtual  T    GetEffectiveMin () const;

            public:
                /**
                 *  The max-value is optional - and can be omitted. If omitted, treat as the
                 *  TRAITS::kMax
                 */
                nonvirtual  T    GetEffectiveMax () const;

            private:
                Memory::Optional<T>    fMin_;
                T                      fEffectiveMin_;
                Memory::Optional<T>    fMax_;
                T                      fEffectiveMax_;
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
